#include <InteractiveToolkit/InteractiveToolkit.h>
#include <InteractiveToolkit/Platform/Platform.h>
#include <InteractiveToolkit/Platform/Thread.h>
#include <InteractiveToolkit/Platform/ThreadWithParameters.h>
#include <InteractiveToolkit/Platform/Core/SmartVector.h>
#include <InteractiveToolkit/Platform/Signal.h>

#include <InteractiveToolkit-Extension/network/HTTP.h>

#include <iostream>
#include <memory>

using namespace ITKExtension::Network;
using namespace Platform;

// Server configuration
const uint16_t SERVER_PORT = 9001;
int THREAD_POOL_SIZE = Platform::Thread::QueryNumberOfSystemThreads();
const int MAX_CONNECTIONS_PER_THREAD = 100;

//
// WebSocket Server Connection Manager
//
class WebSocketConnectionManager
{
private:
    Platform::SmartVector<std::shared_ptr<WebSocketConnection>> connection_list;
    Platform::SmartVector<std::shared_ptr<WebSocketConnection>> connections_copy;
    Platform::Mutex mutex;

public:
    void addConnection(std::shared_ptr<SocketTCP> socket)
    {
        Platform::AutoLock auto_lock(&mutex);
        socket->setBlocking(false);
        auto ws_conn = std::make_shared<WebSocketConnection>(socket, WebSocketConnectionMode::Server);
        connection_list.push_back(ws_conn);
        printf("[ConnectionManager] Active connections: %zu\n", connection_list.size());
    }

    void processConnections()
    {
        // Make a copy to iterate without locking
        {
            Platform::AutoLock auto_lock(&mutex);
            connections_copy = connection_list;
        }

        std::vector<size_t> to_remove;

        for (size_t i = 0; i < connections_copy.size(); i++)
        {
            auto &connection = connections_copy[i];

            connection->process();

            WebSocketConnectionState curr_state = connection->getState();

            // Handle connected state - process incoming frames
            if (curr_state == WebSocketConnectionState::Connected ||
                curr_state == WebSocketConnectionState::ProcessingConnection)
            {
                while (connection->hasReadingFrame())
                {
                    if (auto frame = connection->getReadingFrame())
                        handleFrame(connection, frame);
                }
            }
            else if (curr_state == WebSocketConnectionState::Closed)
            {
                printf("[ConnectionManager] Connection closed\n");
                to_remove.push_back(i);
            }
            else if (curr_state == WebSocketConnectionState::Error)
            {
                printf("[ConnectionManager] Connection error detected\n");
                to_remove.push_back(i);
            }
        }

        // Remove closed connections
        {
            Platform::AutoLock auto_lock(&mutex);
            for (ptrdiff_t i = (ptrdiff_t)to_remove.size() - 1; i >= 0; i--)
            {
                auto connection_it = connection_list.begin() + to_remove[i];
                (*connection_it)->close();
                connection_list.erase(connection_it);
            }
            if (to_remove.size() > 0)
            {
                printf("[ConnectionManager] Removed %zu closed connections, %zu remaining\n",
                       to_remove.size(), connection_list.size());
            }
        }

        // Free all sockets in the copy
        connections_copy.clear();
    }

    void handleFrame(std::shared_ptr<WebSocketConnection> connection, std::shared_ptr<WebSocketFrame> frame)
    {
        if (frame->isText())
        {
            std::string text;
            frame->getText(&text);
            printf("[WebSocket] Received text: %s\n", text.c_str());

            // Echo back with a prefix
            std::string response = "Echo: " + text;
            connection->sendText(response);
        }
        else if (frame->isBinary())
        {
            std::vector<uint8_t> data;
            frame->getBinary(&data);
            printf("[WebSocket] Received binary data: %zu bytes\n", data.size());

            // Echo back binary data
            connection->sendBinary(data);
        }
        else if (frame->isClose())
        {
            WebSocketCloseCode code;
            std::string reason;
            frame->getCloseInfo(&code, &reason);
            printf("[WebSocket] Received close frame: code=%d, reason=%s\n",
                   static_cast<int>(code), reason.c_str());
            // Close response is sent automatically by WebSocketConnection
        }
        else if (frame->isPing())
        {
            printf("[WebSocket] Received ping\n");
            // Pong response is sent automatically by WebSocketConnection
        }
        else if (frame->isPong())
        {
            printf("[WebSocket] Received pong\n");
        }
    }

    size_t getConnectionCount() const
    {
        return connection_list.size();
    }

    void broadcastText(const std::string &message)
    {
        Platform::AutoLock auto_lock(&mutex);
        for (auto &conn : connection_list)
        {
            if (conn->getState() == WebSocketConnectionState::Connected ||
                conn->getState() == WebSocketConnectionState::ProcessingConnection)
            {
                conn->sendText(message);
            }
        }
    }
};

// Worker thread function for server
void serverWorkerThread(int thread_id, std::shared_ptr<WebSocketConnectionManager> manager)
{
    printf("[Worker %d] Started\n", thread_id);
    while (!Platform::Thread::isCurrentThreadInterrupted())
    {
        manager->processConnections();
        Platform::Sleep::millis(1);
    }
    printf("[Worker %d] Stopped\n", thread_id);
}

//
// WebSocket Server
//
void runServer()
{
    printf("=================================\n");
    printf("WebSocket Server Example\n");
    printf("=================================\n\n");

    Platform::Signal::Set([](int sngl)
                          { Platform::Thread::getMainThread()->interrupt(); });

    // Create server socket
    SocketTCPAccept server(
        true, // non-blocking
        true, // reuseAddress
        true  // noDelay
    );

    if (!server.bindAndListen("INADDR_ANY", SERVER_PORT, 50))
    {
        printf("[ERROR] Failed to listen on port %u\n", SERVER_PORT);
        return;
    }

    printf("[Server] Listening on port %u\n", SERVER_PORT);
    printf("[Server] Thread pool size: %d\n", THREAD_POOL_SIZE);
    printf("[Server] Press Ctrl+C to stop\n\n");
    printf("[Server] Test with: websocat ws://localhost:%u\n\n", SERVER_PORT);

    // Create connection managers
    std::vector<std::shared_ptr<WebSocketConnectionManager>> managers;
    for (int i = 0; i < THREAD_POOL_SIZE; i++)
    {
        managers.push_back(std::make_shared<WebSocketConnectionManager>());
    }

    // Create worker threads
    using ThreadType = Platform::ThreadWithParameters<void(int thread_id, std::shared_ptr<WebSocketConnectionManager> manager)>;
    std::vector<std::shared_ptr<ThreadType>> workers;
    for (int i = 0; i < THREAD_POOL_SIZE; i++)
    {
        workers.push_back(std::make_shared<ThreadType>(&serverWorkerThread, i, managers[i]));
    }
    for (int i = 0; i < THREAD_POOL_SIZE; i++)
    {
        workers[i]->start();
    }

    printf("[Acceptor] Started\n");
    int next_manager = 0;
    while (!Platform::Thread::isCurrentThreadInterrupted())
    {
        auto client = std::make_shared<SocketTCP>();
        SocketResult result = server.accept(client.get());
        if (result != SOCKET_RESULT_OK)
        {
            if (result == SOCKET_RESULT_WOULD_BLOCK || result == SOCKET_RESULT_TIMEOUT)
            {
                Platform::Sleep::millis(1);
                continue;
            }
            printf("[Acceptor] Error on accept new connections\n");
            break;
        }

        char client_str[32];
        snprintf(client_str, 32, "%s:%u",
                 inet_ntoa(client->getAddrOut().sin_addr),
                 ntohs(client->getAddrOut().sin_port));
        printf("[Acceptor] New connection accepted from %s\n", client_str);

        // Round-robin distribution to managers
        managers[next_manager]->addConnection(client);
        next_manager = (next_manager + 1) % managers.size();
    }

    // Cleanup
    printf("\n[Server] Shutting down...\n");
    server.close();
    for (auto &worker : workers)
    {
        worker->interrupt();
    }
    for (auto &worker : workers)
    {
        worker->wait();
    }
    workers.clear();

    printf("[Server] Shutdown complete\n");
}

//
// WebSocket Client
//
void runClient(const std::string &host, uint16_t port, const std::string &path)
{
    printf("=================================\n");
    printf("WebSocket Client Example\n");
    printf("=================================\n\n");

    Platform::Signal::Set([](int sngl)
                          { Platform::Thread::getMainThread()->interrupt(); });

    printf("[Client] Connecting to %s:%u%s\n", host.c_str(), port, path.c_str());

    // Create socket
    auto socket = std::make_shared<SocketTCP>();

    // Connect to server
    if (!socket->connect(
        Platform::SocketTools::resolveHostname(
        host.c_str()
        )
        , port))
    {
        printf("[Client] Failed to connect to server\n");
        return;
    }

    printf("[Client] Connected to server\n");

    // Set non-blocking mode
    socket->setBlocking(false);

    // Create WebSocket connection
    auto ws_conn = std::make_shared<WebSocketConnection>(socket, WebSocketConnectionMode::Client);

    // Begin handshake
    if (!ws_conn->clientBeginHandshake(host, path))
    {
        printf("[Client] Failed to begin handshake\n");
        return;
    }

    printf("[Client] WebSocket handshake initiated\n");

    // Process until connected
    while (!Platform::Thread::isCurrentThreadInterrupted())
    {
        ws_conn->process();

        WebSocketConnectionState state = ws_conn->getState();
        if (state == WebSocketConnectionState::Connected)
        {
            printf("[Client] WebSocket connected!\n");
            break;
        }
        else if (state == WebSocketConnectionState::Error)
        {
            printf("[Client] WebSocket handshake failed\n");
            return;
        }
        else if (state == WebSocketConnectionState::Closed)
        {
            printf("[Client] Connection closed during handshake\n");
            return;
        }

        Platform::Sleep::millis(1);
    }

    // Send some test messages
    printf("[Client] Sending test messages...\n");

    ws_conn->sendText("Hello, WebSocket Server!");
    ws_conn->sendText("This is a test message.");
    ws_conn->sendPing("ping-test");

    // Binary data test
    std::vector<uint8_t> binary_data = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
    ws_conn->sendBinary(binary_data);

    // Main loop - process messages
    printf("[Client] Processing messages. Press Ctrl+C to stop.\n\n");

    int message_count = 0;
    while (!Platform::Thread::isCurrentThreadInterrupted())
    {
        ws_conn->process();

        // Check state
        WebSocketConnectionState state = ws_conn->getState();
        if (state == WebSocketConnectionState::Error)
        {
            printf("[Client] Connection error\n");
            break;
        }
        else if (state == WebSocketConnectionState::Closed)
        {
            printf("[Client] Connection closed\n");
            break;
        }

        // Process incoming frames
        while (ws_conn->hasReadingFrame())
        {
            auto frame = ws_conn->getReadingFrame();
            if (frame)
            {
                if (frame->isText())
                {
                    std::string text;
                    frame->getText(&text);
                    printf("[Client] Received: %s\n", text.c_str());
                }
                else if (frame->isBinary())
                {
                    std::vector<uint8_t> data;
                    frame->getBinary(&data);
                    printf("[Client] Received binary: %zu bytes\n", data.size());
                }
                else if (frame->isPong())
                {
                    printf("[Client] Received pong\n");
                }
                else if (frame->isClose())
                {
                    WebSocketCloseCode code;
                    std::string reason;
                    frame->getCloseInfo(&code, &reason);
                    printf("[Client] Server closed connection: %d - %s\n",
                           static_cast<int>(code), reason.c_str());
                }

                message_count++;
            }
        }

        // Send periodic messages
        static int counter = 0;
        counter++;
        if (counter % 3000 == 0) // Every 3 seconds at 1ms sleep
        {
            std::string msg = "Client message #" + std::to_string(counter / 3000);
            ws_conn->sendText(msg);
            printf("[Client] Sent: %s\n", msg.c_str());
        }

        Platform::Sleep::millis(1);
    }

    // Send close frame
    printf("[Client] Sending close frame...\n");
    ws_conn->sendClose(WebSocketCloseCode::Normal, "Client shutting down");

    // Give some time to send close frame
    for (int i = 0; i < 100 && ws_conn->hasOutgoingFrames(); i++)
    {
        ws_conn->process();
        Platform::Sleep::millis(10);
    }

    ws_conn->close();
    printf("[Client] Disconnected\n");
}

void printUsage(const char *programName)
{
    printf("WebSocket Example\n");
    printf("\n");
    printf("Usage:\n");
    printf("  %s server                          - Start WebSocket server on port %u\n", programName, SERVER_PORT);
    printf("  %s client [host] [port] [path]     - Connect to WebSocket server\n", programName);
    printf("\n");
    printf("Examples:\n");
    printf("  %s server\n", programName);
    printf("  %s client localhost %u /\n", programName, SERVER_PORT);
    // printf("  %s client echo.websocket.org 80 /\n", programName);
    printf("\n");
    printf("Testing with external tools:\n");
    printf("  websocat ws://localhost:%u\n", SERVER_PORT);
    printf("  wscat -c ws://localhost:%u\n", SERVER_PORT);
    printf("\n");
}

int main(int argc, char *argv[])
{
    Platform::Thread::getMainThread();

    if (argc < 2)
    {
        printUsage(argv[0]);
        return 0;
    }

    std::string mode = argv[1];

    if (mode == "server")
    {
        runServer();
    }
    else if (mode == "client")
    {
        std::string host = "localhost";
        uint16_t port = SERVER_PORT;
        std::string path = "/";

        if (argc >= 3)
            host = argv[2];
        if (argc >= 4)
            port = (uint16_t)atoi(argv[3]);
        if (argc >= 5)
            path = argv[4];

        runClient(host, port, path);
    }
    else
    {
        printf("Unknown mode: %s\n\n", mode.c_str());
        printUsage(argv[0]);
        return 1;
    }

    return 0;
}
