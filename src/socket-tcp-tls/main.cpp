#include <InteractiveToolkit/InteractiveToolkit.h>
#include <InteractiveToolkit/Platform/Platform.h>

#include <iostream>
#include <stdlib.h>

#include "network/HTTPRequest.h"
#include "network/HTTPResponse.h"

#include "network/tls/GlobalSharedState.h"

using namespace ITKCommon;

void connect(const std::string addr_ipv4)
{
    Platform::SocketTCP clientSocket;

    if (!clientSocket.connect(addr_ipv4, 8444))
    {
        printf("Connect failed!!!... interrupting current thread\n");
        clientSocket.close();
        return;
    }

    clientSocket.setNoDelay(true);
    // clientSocket.setWriteTimeout(500); // 500 ms write timeout
    // clientSocket.setReadTimeout(1500); // 1500 ms read timeout

    {
        ITKExtension::Network::HTTPRequest req;
        req.setDefault(addr_ipv4, "GET", "/");
        if (!req.writeToStream(&clientSocket))
        {
            printf("Failed to send HTTP Request...\n");
            clientSocket.close();
            return;
        }
    }

    {
        ITKExtension::Network::HTTPResponse res;
        if (!res.readFromStream(&clientSocket))
        {
            printf("Failed to read HTTP Response...\n");
            clientSocket.close();
            return;
        }

        printf("Received HTTP Response: %d %s\n", res.status_code, res.reason_phrase.c_str());
        if (res.getHeader("Content-Type") == "text/plain")
            printf("Body (%u bytes): %s\n", (uint32_t)res.body.size(), res.bodyAsString().c_str());
    }

    printf("Closing connection...\n");

    clientSocket.close();
}

void start_server()
{
    // blocking: true, reuseAddress: true, noDelay: true
    Platform::SocketTCPAccept serverSocket(true, true, true);

    if (!serverSocket.bindAndListen(
            "INADDR_ANY", // interface address
            8444,         // port
            10))          // input queue
    {
        printf("Error to bind address\n");
        serverSocket.close();
        return;
    }

    {
        char server_str[32];
        snprintf(server_str, 32, "%s:%u", inet_ntoa(serverSocket.getAddr().sin_addr), ntohs(serverSocket.getAddr().sin_port));
        printf("server started at %s\n", server_str);
    }

    Platform::ThreadPool threadPool(std::max(Platform::Thread::QueryNumberOfSystemThreads(), 4));

    Platform::Time time;
    Platform::SocketTCP *clientSocket = new Platform::SocketTCP();
    while (!Platform::Thread::isCurrentThreadInterrupted())
    {
        bool connection_accepted = true;
        // blocking mode
        if (!serverSocket.accept(clientSocket))
        {
            if (serverSocket.isSignaled() || serverSocket.isClosed() || !serverSocket.isListening())
                break;
            connection_accepted = false;
            continue; // probably a thread interrupt was triggered... force while to check
        }

        time.update();
        if (time.deltaTime >= 1.0f)
            printf("thread pool queue size: %u / 200\n", threadPool.taskInQueue());
        else
            time.rollback_and_set_zero();

        // if there are 200 or more tasks in the queue, refuse new connections
        if (threadPool.taskInQueue() >= 200)
        {
            clientSocket->close();
            continue;
        }

        threadPool.postTask([clientSocket]()
                            {
            // client thread (server side)
            Platform::SocketTCP *socket = clientSocket;
            socket->setNoDelay(true);
            socket->setReadTimeout(500);// 500 ms read timeout

            // Platform::Sleep::millis(100); // wait a bit for client to send data

            char client_str[32];
            snprintf(client_str, 32, "%s:%u", inet_ntoa(socket->getAddrOut().sin_addr), ntohs(socket->getAddrOut().sin_port));

            ITKExtension::Network::HTTPRequest req;
            if (!req.readFromStream(socket))
            {
                printf("Failed to read HTTP Request on %s\n", client_str);
                socket->close();
                delete socket;
                return;
            }

            ITKExtension::Network::HTTPResponse res;

            if (req.method == "GET" && req.path == "/")
                res.setDefault(200)
                    .setBody(
                    "Hello! This is a response from the C++ TCP server.\nYour IP is: " + std::string(client_str),
                    "text/plain");
            else 
                res.setDefault(404)
                    .setBody(res.reason_phrase,"text/plain");

            res.writeToStream(socket);

            socket->close();
            delete socket; });

        clientSocket = new Platform::SocketTCP();
    }

    serverSocket.close();
    delete clientSocket;

    threadPool.finish();
}

int main(int argc, char *argv[])
{
    Path::setWorkingPath(Path::getExecutablePath(argv[0]));
    Platform::ThreadDataSet::Instance()->setGlobalThreadPriority(Platform::ThreadPriority::Normal);
    Platform::Thread::staticInitialization();
    TLS::GlobalSharedState::staticInitialization();

    Platform::Signal::Set([](int sngl)
                          {
        //ITK_ABORT(true, "forced app finishing.\n");
        Platform::Thread::getMainThread()->interrupt(); });

    if (argc == 3 && (strcmp(argv[1], "connect") == 0))
        connect(argv[2]);
    else if (argc == 2 && (strcmp(argv[1], "server") == 0))
        start_server();
    else
    {
        printf("Examples: \n"
               "\n"
               "# start server:\n"
               "./socket-tcp server\n"
               "\n"
               "# connect to server:\n"
               "./socket-tcp connect <IP>\n"
               "\n");
    }

    return 0;
}
