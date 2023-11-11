#include <InteractiveToolkit/InteractiveToolkit.h>
#include <InteractiveToolkit/Platform/Platform.h>

#include <iostream>
#include <stdlib.h>

using namespace ITKCommon;

void connect(const std::string addr_ipv4)
{

    Platform::SocketTCP clientSocket;

    printf("connecting...\n");
    if (!clientSocket.connect(addr_ipv4, Platform::NetworkConstants::PUBLIC_PORT_START))
    {
        printf("Connect failed!!!... interrupting current thread\n");
        clientSocket.close();
        return;
    }

    printf("done.\n");

    clientSocket.setNoDelay(true);

    char init[16] = "init";
    printf("sending: \"init\" size: %u bytes\n", (uint32_t)sizeof(init));
    if (!clientSocket.write_buffer(
        (uint8_t *)&init, 
        sizeof(init)))
    {
        printf("Failed to send handshake...\n");
        clientSocket.close();
        return;
    }

    printf("receiving...\n");
    char response[32];
    uint32_t read_feedback;
    if (!clientSocket.read_buffer(
            (uint8_t *)&response,
            sizeof(response),
            &read_feedback))
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

bool use_blocking_sockets = true;

void start_server(bool blocking = true)
{

    use_blocking_sockets = blocking;

    printf("start server\n");

    printf("use_blocking_sockets:%i\n", use_blocking_sockets);

    Platform::SocketTCPAccept serverSocket(
        use_blocking_sockets, // blocking
        true,                 // reuseAddress
        true                  // noDelay
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

    Platform::SocketTCP *clientSocket = new Platform::SocketTCP();
    std::vector<Platform::Thread *> threads;

    while (!Platform::Thread::isCurrentThreadInterrupted())
    {

        bool connection_accepted = true;
        if (use_blocking_sockets)
        {
            // blocking mode
            if (!serverSocket.accept(clientSocket))
            {
                if (serverSocket.isSignaled() || serverSocket.isClosed() || !serverSocket.isListening())
                    break;
                connection_accepted = false;
            }
        }
        else
        {
            // non-blocking mode
            if (!serverSocket.accept(clientSocket))
            {
                printf("*");
                fflush(stdout);
                Platform::Sleep::millis(500);
                connection_accepted = false;
            }
        }

        // clear deleted threads
        for (int i = (int)threads.size() - 1; i >= 0; i--)
        {
            if (!threads[i]->isAlive())
            {
                printf("removing closed thread...\n");
                delete threads[i];
                threads.erase(threads.begin() + i);
            }
        }

        if (!connection_accepted)
            continue;

        Platform::Thread *aux;
        threads.push_back(aux = new Platform::Thread([clientSocket]()
                                                     {
            // client thread (server side)
            Platform::SocketTCP *socket = clientSocket;
            socket->setNoDelay(true);

            char client_str[16];
            sprintf(client_str, "%s:%u", inet_ntoa(socket->getAddr().sin_addr), ntohs(socket->getAddr().sin_port));

            char initial_string[16] = {0};
            uint32_t read_feedback;
            if (!socket->read_buffer((uint8_t*)initial_string, sizeof(initial_string), &read_feedback)) {
                printf("Connection or thread interrupted with the read feedback: %u\n", read_feedback);
                socket->close();
                delete socket;
                return;
            }

            printf("[%s] receiving: \"%s\" size: %u bytes\n", client_str, initial_string, read_feedback);

            Platform::Sleep::millis(1000);

            char response[32];
            uint32_t write_feedback;
            sprintf(response, "Your IP is: %s", client_str);

            socket->write_buffer(
                (uint8_t*)response,
                sizeof(response),
                &write_feedback
            );

            printf("[%s] closing socket\n", client_str);
            socket->close();
            delete socket;
            }));

        aux->start();

        clientSocket = new Platform::SocketTCP();
    }

    serverSocket.close();

    delete clientSocket;

    for (size_t i = 0; i < threads.size(); i++)
    {
        threads[i]->interrupt();
        threads[i]->wait();
        delete threads[i];
    }
    threads.clear();
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
        start_server( true );
    else if (argc == 3 && (strcmp(argv[1], "server") == 0))
        start_server( strcmp(argv[2], "non-block") != 0 );
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
