#include <InteractiveToolkit/InteractiveToolkit.h>
#include <InteractiveToolkit/Platform/Platform.h>

#include <iostream>
#include <stdlib.h>

using namespace ITKCommon;

void connect(const std::string addr_ipv4)
{
    Platform::SocketTCP clientSocket;

    printf("connecting...\n");
    if (!clientSocket.connect(addr_ipv4, 8443))
    {
        printf("Connect failed!!!... interrupting current thread\n");
        clientSocket.close();
        return;
    }

    printf("done.\n");

    clientSocket.setNoDelay(true);
    clientSocket.setWriteTimeout(500); // 500 ms write timeout
    clientSocket.setReadTimeout(1500); // 1500 ms read timeout

    const char *HTTP_GET_REQUEST =
        "GET / HTTP/1.1\r\n"
        "Connection: close\r\n"
        "\r\n";

    printf("sending: \"HTTP_GET_REQUEST\" size: %u bytes\n", (uint32_t)strlen(HTTP_GET_REQUEST));
    if (!clientSocket.write_buffer(
            (uint8_t *)HTTP_GET_REQUEST,
            (uint32_t)strlen(HTTP_GET_REQUEST)))
    {
        if (clientSocket.isWriteTimedout())
            printf("write timed out...\n");

        printf("Failed to send handshake...\n");
        clientSocket.close();
        return;
    }

    printf("receiving...\n");
    char response[1024];
    uint32_t read_feedback;
    while (true)
    {
        if (!clientSocket.read_buffer(
            (uint8_t *)&response,
            sizeof(response),
            &read_feedback))
        {
            if (clientSocket.isReadTimedout())
                printf("read timed out...\n");

            printf("read end...read feedback: %u\n", read_feedback);
            clientSocket.close();
            return;
        }
        
        printf("receiving: \"%s\" size: %u bytes\n", std::string(response, read_feedback).c_str(), read_feedback);
    }

    printf("closing connection\n");
    clientSocket.close();
}

void start_server()
{
    printf("start server on port 8443\n");

    Platform::SocketTCPAccept serverSocket(
        true, // blocking
        true, // reuseAddress
        true  // noDelay
    );

    if (!serverSocket.bindAndListen(
            "INADDR_ANY", // interface address
            8443,         // port
            10            // input queue
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
        // blocking mode
        if (!serverSocket.accept(clientSocket))
        {
            if (serverSocket.isSignaled() || serverSocket.isClosed() || !serverSocket.isListening())
                break;
            connection_accepted = false;
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
            socket->setReadTimeout(500);// 500 ms read timeout

            char client_str[32];
            snprintf(client_str, 32, "%s:%u", inet_ntoa(socket->getAddrOut().sin_addr), ntohs(socket->getAddrOut().sin_port));

            std::vector<char> line;
            line.reserve(1024);
            int max_http_header_lines = 100;

            char input_buffer[1024] = {0};
            uint32_t read_feedback;
            uint32_t curr_reading;
            bool found_crlf = false;

            std::string requested_path;

            while (!found_crlf) {

                if (!socket->read_buffer((uint8_t*)input_buffer, sizeof(input_buffer), &read_feedback)) {
                    if (socket->isReadTimedout()){

                        printf("[%s] Read NO Headers timed out\n", client_str);
                        socket->close();
                        delete socket;

                        return;
                    } else {
                        printf("[%s] Connection or thread interrupted with the read feedback: %u\n", client_str, read_feedback);
                        socket->close();
                        delete socket;
                        return;
                    }
                }
                // printf("[%s] receiving: \"%s\" size: %u bytes\n", client_str, input_buffer, read_feedback);


                curr_reading = 0;
                do {
                    for(uint32_t i=curr_reading;i<read_feedback;i++) {
                        line.push_back(input_buffer[i]);
                        // printf("line: \"%s\"\n", std::string(line.begin(), line.end()).c_str());
                        if (line.size() >= 1024 || input_buffer[i] == '\0'){
                            printf("[%s] HTTP line too long: %u\n", client_str, (uint32_t)line.size());
                            socket->close();
                            delete socket;
                            return;
                        }
                        if (line.size() >= 2 && line[line.size()-1] == '\n' && line[line.size()-2] == '\r')
                            break;
                    }

                    found_crlf = line.size() == 2 && line[0] == '\r' && line[1] == '\n';

                    curr_reading += (uint32_t)line.size();

                    if (line.size() >= 2 && line[line.size()-2] == '\r' && line[line.size()-1] == '\n') {
                        line.pop_back();
                        line.pop_back();
                    }

                    std::string header = std::string(line.begin(), line.end());

                    printf("[%s] header: \"%s\"\n", client_str, header.c_str());

                    if (ITKCommon::StringUtil::startsWith(header, "GET")){
                        auto parts = ITKCommon::StringUtil::tokenizer(header, " ");
                        if (parts.size() == 3 && ITKCommon::StringUtil::startsWith(parts[2], "HTTP/"))
                            requested_path = parts[1];
                    }

                    max_http_header_lines--;
                    if (max_http_header_lines <= 0) {
                        printf("[%s] Too many HTTP header lines readed...\n", client_str);
                        socket->close();
                        delete socket;
                        return;
                    }

                    line.clear();
                } while(!found_crlf && curr_reading < read_feedback);
            }


            //Platform::Sleep::millis(1000);

            printf("[%s] writting response...\n", client_str);

            uint32_t write_feedback;

            if (requested_path.compare("/") != 0){
                // not the / path... return not found
                const char* not_found =
                    "HTTP/1.1 404 Not Found\r\n"
                    "Content-Type: text/plain\r\n"
                    "Content-Length: 13\r\n"
                    "\r\n"
                    "404 Not Found";
                socket->write_buffer(
                    (uint8_t*)not_found,
                    strlen(not_found),
                    &write_feedback
                );
            } else {

                char HTTP_Headers[256];
                char HTTP_Body[1024];

                snprintf(HTTP_Body, 1024,
                    "Your IP is: %s", client_str);

                snprintf(HTTP_Headers, 256,
                    "HTTP/1.1 200 OK\r\n"
                    "Connection: close\r\n"
                    "Content-Type: text/plain\r\n"
                    "Content-Length: %i\r\n"
                    "\r\n", (int)strlen(HTTP_Body));


                socket->write_buffer(
                    (uint8_t*)HTTP_Headers,
                    strlen(HTTP_Headers),
                    &write_feedback
                );
                socket->write_buffer(
                    (uint8_t*)HTTP_Body,
                    strlen(HTTP_Body),
                    &write_feedback
                );
            }

            socket->close();
            delete socket; }));

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
