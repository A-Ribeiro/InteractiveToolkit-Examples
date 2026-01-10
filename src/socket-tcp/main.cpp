#include <InteractiveToolkit/InteractiveToolkit.h>
#include <InteractiveToolkit/Platform/Platform.h>

#include <iostream>
#include <stdlib.h>

using namespace ITKCommon;

const int SERVER_ACCEPT_QUEUE_SIZE = SOMAXCONN;
const int SERVER_MAX_TASKS_QUEUE_SIZE = 200;

void connect(const std::string addr_ipv4)
{

    Platform::SocketTCP clientSocket;

    printf("connecting...\n");
    if (!clientSocket.connect(Platform::SocketTools::resolveHostname(addr_ipv4), Platform::NetworkConstants::PUBLIC_PORT_START))
    {
        printf("Connect failed!!!... interrupting current thread\n");
        clientSocket.close();
        return;
    }

    printf("done.\n");

    clientSocket.setNoDelay(true);

    char init[16] = "init";
    printf("sending: \"init\" size: %u bytes\n", (uint32_t)sizeof(init));
    if (clientSocket.write_buffer(
            (uint8_t *)&init,
            sizeof(init)) != Platform::SOCKET_RESULT_OK)
    {
        printf("Failed to send handshake...\n");
        clientSocket.close();
        return;
    }

    printf("receiving...\n");
    char response[64];
    uint32_t read_feedback;
    if (clientSocket.read_buffer(
            (uint8_t *)&response,
            sizeof(response),
            &read_feedback) != Platform::SOCKET_RESULT_OK)
    {
        printf("Failed to receive handshake...read feedback: %u\n", read_feedback);
        clientSocket.close();
        return;
    }
    else
        printf("receiving: \"%s\" size: %u bytes\n", response, read_feedback);

    printf("closing connection\n");
    clientSocket.close();
}

void start_server(bool blocking = true)
{
    printf("start server\n");

    printf("use_blocking_sockets:%i\n", blocking);

    Platform::SocketTCPAccept serverSocket(
        blocking, // blocking
        true,     // reuseAddress
        true      // noDelay
    );

    if (!serverSocket.bindAndListen(
            "INADDR_ANY",                                  // interface address
            Platform::NetworkConstants::PUBLIC_PORT_START, // port
            10                                             // input queue
            ))
    {
        printf("Error to bind address\n");
        serverSocket.close();
        return;
    }

    Platform::ThreadPool threadPool((std::max)(Platform::Thread::QueryNumberOfSystemThreads(), 4));
    std::shared_ptr<Platform::SocketTCP> clientSocket = std::make_shared<Platform::SocketTCP>();
    while (!Platform::Thread::isCurrentThreadInterrupted())
    {

        bool connection_accepted = true;
        if (blocking)
        {
            // blocking mode
            if (serverSocket.accept(clientSocket.get()) != Platform::SOCKET_RESULT_OK)
                break;
        }
        else
        {
            // non-blocking mode
            auto result = serverSocket.accept(clientSocket.get());
            if (result == Platform::SOCKET_RESULT_WOULD_BLOCK)
            {
                printf("*");
                fflush(stdout);
                Platform::Sleep::millis(500);
                continue;
            }
            else if (result != Platform::SOCKET_RESULT_OK)
                break;
        }

        threadPool.postTask([clientSocket]()
                            {
            if (Platform::Thread::isCurrentThreadInterrupted()) 
            {
                printf("Thread pool finished. Closing remaining connections.\n");
                clientSocket->close();
                return;
            }
            // client thread (server side)
            Platform::SocketTCP *socket = clientSocket.get();
            socket->setNoDelay(true);

            char client_str[32];
            snprintf(client_str, 32, "%s:%u", inet_ntoa(socket->getAddrOut().sin_addr), ntohs(socket->getAddrOut().sin_port));

            char initial_string[16] = {0};
            uint32_t read_feedback;
            if (socket->read_buffer((uint8_t*)initial_string, sizeof(initial_string), &read_feedback) != Platform::SOCKET_RESULT_OK) {
                printf("Connection or thread interrupted with the read feedback: %u\n", read_feedback);
                socket->close();
                return;
            }

            printf("[%s] receiving: \"%s\" size: %u bytes\n", client_str, initial_string, read_feedback);

            Platform::Sleep::millis(1000);

            char response[64];
            uint32_t write_feedback;
            snprintf(response, 64, "Your IP is: %s", client_str);

            socket->write_buffer(
                (uint8_t*)response,
                sizeof(response),
                &write_feedback
            );

            printf("[%s] closing socket\n", client_str);
            socket->close(); });

        clientSocket = std::make_shared<Platform::SocketTCP>();
    }

    serverSocket.close();

    threadPool.finish();
}

int main(int argc, char *argv[])
{
    Path::setWorkingPath(Path::getExecutablePath(argv[0]));
    Platform::ThreadDataSet::Instance()->setGlobalThreadPriority(Platform::ThreadPriority::Normal);
    Platform::Thread::staticInitialization();

    Platform::Signal::Set([](int sngl)
                          {
        //ITK_ABORT(true, "forced app finishing.\n");
        Platform::Thread::getMainThread()->interrupt(); });

    if (argc == 3 && (strcmp(argv[1], "connect") == 0))
        connect(argv[2]);
    else if (argc == 2 && (strcmp(argv[1], "server") == 0))
        start_server(true);
    else if (argc == 3 && (strcmp(argv[1], "server") == 0))
        start_server(strcmp(argv[2], "non-block") != 0);
    else
    {
        printf("Examples: \n"
               "\n"
               "# start server:\n"
               "./socket-tcp server [non-block]\n"
               "\n"
               "# connect to server:\n"
               "./socket-tcp connect <IP>\n"
               "\n");
    }

    return 0;
}
