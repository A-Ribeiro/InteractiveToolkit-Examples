#include <InteractiveToolkit-Extension/network/HTTPConnection.h>
#include <InteractiveToolkit/Platform/Thread.h>
#include <InteractiveToolkit/Platform/ThreadWithParameters.h>
#include <InteractiveToolkit/Platform/Core/SmartVector.h>
#include <InteractiveToolkit/Platform/Signal.h>

using namespace Platform;
using namespace ITKExtension::Network;

// Server configuration
const uint16_t SERVER_PORT = 8080;
int THREAD_POOL_SIZE = Platform::Thread::QueryNumberOfSystemThreads();
const int MAX_CONNECTIONS_PER_THREAD = 100;

// Connection manager for each thread
class ConnectionManager
{
private:
    Platform::SmartVector<std::shared_ptr<HTTPConnection>> connection_list;
    Platform::SmartVector<std::shared_ptr<HTTPConnection>> connections_copy;
    Platform::Mutex mutex;

public:
    void addConnection(std::shared_ptr<SocketTCP> socket)
    {
        Platform::AutoLock auto_lock(&mutex);
        socket->setBlocking(false);
        connection_list.push_back(std::make_shared<HTTPConnection>(socket,
                                                                   HTTPConnectionMode::Server));
        printf("[ConnectionManager] Active connections: %zu\n", connection_list.size());

    }

    void processConnections()
    {
        // make a copy to iterate without locking
        {
            Platform::AutoLock auto_lock(&mutex);
            connections_copy = connection_list;
        }
        std::vector<size_t> to_remove;

        for (size_t i = 0; i < connections_copy.size(); i++)
        {
            auto &connection = connections_copy[i];

            HTTPProcessingResult result = connection->process();

            HTTPConnectionState curr_conn_state = connection->getState();
            if (curr_conn_state == HTTPConnectionState::Server_ReadingRequestComplete)
                handleRequest(connection);
            else if (curr_conn_state == HTTPConnectionState::Server_WritingResponseComplete)
                to_remove.push_back(i);
            else if (curr_conn_state == HTTPConnectionState::Closed)
                to_remove.push_back(i);
            else if (curr_conn_state == HTTPConnectionState::Error)
            {
                printf("[ConnectionManager] Connection error detected\n");
                to_remove.push_back(i);
            }
        }

        // remove closed connections
        {
            Platform::AutoLock auto_lock(&mutex);
            for (ptrdiff_t i = (ptrdiff_t)to_remove.size() - 1; i >= 0; i--)
            {
                auto connection_it = connection_list.begin() + to_remove[i];
                (*connection_it)->close();
                connection_list.erase(connection_it);
            }
            if (to_remove.size() > 0)
                printf("[ConnectionManager] Removed %zu closed connections, %zu remaining\n",
                       to_remove.size(), connection_list.size());
        }

        // free all sockets in the copy
        connections_copy.clear();
    }

    void handleRequest(std::shared_ptr<HTTPConnection> connection)
    {
        auto request = connection->getRequest();
        auto response = HTTPResponseAsync::CreateShared();

        printf("[HTTP] %s %s %s\n",
               request->method.c_str(),
               request->path.c_str(),
               request->http_version.c_str());

        // Route handling
        if (request->path == "/" || request->path == "/index.html")
        {
            response->setDefault(200);
            response->setBody(
                "<html><body>"
                "<h1>Welcome to Async HTTP Server</h1>"
                "<p>This server uses non-blocking sockets and thread pool.</p>"
                "<p>Try these endpoints:</p>"
                "<ul>"
                "<li><a href=\"/hello\">/hello</a> - Simple greeting</li>"
                "<li>/echo - POST data will be echoed back</li>"
                "<li><a href=\"/large\">/large</a> - Large response (chunked transfer)</li>"
                "</ul>"
                "</body></html>",
                "text/html");
        }
        else if (request->path == "/hello")
        {
            response->setDefault(200);
            response->setBody(
                "{\"message\": \"Hello from Async HTTP Server!\", \"method\": \"" + request->method + "\"}",
                "application/json");
        }
        else if (request->path == "/echo")
        {
            response->setDefault(200);
            if (request->bodyAsVector().size() > 0)
            {
                response->setBody(
                    request->bodyAsVector().data(),
                    (uint32_t)request->bodyAsVector().size(),
                    request->getHeader("Content-Type").empty() ? "application/octet-stream" : request->getHeader("Content-Type"));
            }
            else
            {
                response->setBody("{\"message\": \"No body to echo\"}", "application/json");
            }
        }
        else if (request->path == "/large")
        {
            // Generate a large response to test chunked transfer encoding
            std::string large_body;
            for (int i = 0; i < 10000; i++)
            {
                large_body += "Line " + std::to_string(i) + ": This is a test of chunked transfer encoding. ";
                large_body += "The HTTPConnection class will automatically use chunked encoding for large bodies.\n";
            }
            response->setDefault(200);
            response->setBody(large_body, "text/plain");
            printf("[HTTP] Sending large response: %zu bytes (will use chunked encoding)\n", large_body.size());
        }
        else if (request->path == "/headers")
        {
            std::string body = "Request Headers:\n\n";
            for (const auto &header : request->listHeaders())
            {
                body += header.first + ": " + header.second + "\n";
            }
            response->setDefault(200);
            response->setBody(body, "text/plain");
        }
        else
        {
            response->setDefault(404);
            response->setBody(
                "<html><body><h1>404 Not Found</h1><p>The requested path was not found.</p></body></html>",
                "text/html");
        }

        // Set server header
        response->setHeader("Server", "ITK-AsyncHTTP/1.0");
        response->setHeader("Connection", "keep-alive");

        // Begin writing response
        connection->serverBeginWriteResponse(response);
    }

    size_t getConnectionCount() const
    {
        return connection_list.size();
    }
};

// Worker thread function
void workerThread(int thread_id, std::shared_ptr<ConnectionManager> manager)
{
    printf("[Worker %d] Started\n", thread_id);
    while (!Platform::Thread::isCurrentThreadInterrupted())
    {
        manager->processConnections();
        Platform::Sleep::millis(1);
        // printf(" %d ", thread_id);fflush(stdout);

    }
    printf("[Worker %d] Stopped\n", thread_id);
}

int main(int argc, char *argv[])
{
    Platform::Signal::Set([](int sngl)
                          { Platform::Thread::getMainThread()->interrupt(); });

    printf("=================================\n");
    printf("Async HTTP Server Example\n");
    printf("=================================\n\n");

    // Create server socket
    SocketTCPAccept server(
        true, // non-blocking
        true, // reuseAddress
        true  // noDelay
    );

    if (!server.bindAndListen("INADDR_ANY", SERVER_PORT, 50))
    {
        printf("[ERROR] Failed to listen on port %u\n", SERVER_PORT);
        return 1;
    }

    printf("[Server] Listening on port %u\n", SERVER_PORT);
    printf("[Server] Thread pool size: %d\n", THREAD_POOL_SIZE);
    printf("[Server] Press Ctrl+C to stop\n\n");

    // Create connection managers
    std::vector<std::shared_ptr<ConnectionManager>> managers;
    for (int i = 0; i < THREAD_POOL_SIZE; i++)
    {
        managers.push_back(std::make_shared<ConnectionManager>());
    }

    // Wait for interrupt (in a real application, you'd want proper signal handling)
    printf("Server running... (This example runs indefinitely. Use Ctrl+C to stop)\n");

    // Create worker threads
    using ThreadType = Platform::ThreadWithParameters<void(int thread_id, std::shared_ptr<ConnectionManager> manager)>;
    std::vector<std::shared_ptr<ThreadType>> workers;
    for (int i = 0; i < THREAD_POOL_SIZE; i++)
        workers.push_back(std::make_shared<ThreadType>(&workerThread, i, managers[i]));
    for (int i = 0; i < THREAD_POOL_SIZE; i++)
        workers[i]->start();

    printf("[Acceptor] Started\n");
    int next_manager = 0;
    while (!Platform::Thread::isCurrentThreadInterrupted())
    {
        auto client = std::make_shared<SocketTCP>();
        SocketResult result = server.accept(client.get());
        if (result != SOCKET_RESULT_OK)
        {
            printf("[Acceptor] error on accept new connections\n");
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
        worker->interrupt();
    for (auto &worker : workers)
        worker->wait();
    workers.clear();

    printf("[Server] Shutdown complete\n");

    return 0;
}
