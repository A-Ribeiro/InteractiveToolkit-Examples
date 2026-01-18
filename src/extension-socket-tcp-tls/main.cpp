#include <InteractiveToolkit/InteractiveToolkit.h>
#include <InteractiveToolkit/Platform/Platform.h>
#include <InteractiveToolkit/ITKCommon/Date.h>

#include <iostream>
#include <stdlib.h>

#include <InteractiveToolkit-Extension/network/HTTP.h>
#include <InteractiveToolkit-Extension/network/SocketTCP_SSL.h>

using namespace ITKCommon;

const int SERVER_ACCEPT_QUEUE_SIZE = SOMAXCONN;
const int SERVER_MAX_TASKS_QUEUE_SIZE = 200;

void connect(const std::string &url_or_addr_ipv4, bool use_full_url_as_input, const std::string &client_cert_file_ = "")
{
    using namespace ITKExtension::Network;

    std::string client_cert_file = client_cert_file_;

    Platform::SocketTools::URL url;
    std::string addr_ipv4;

    if (use_full_url_as_input)
    {
        url = Platform::SocketTools::URL::parse(url_or_addr_ipv4);
        if (!url.valid)
        {
            printf("Invalid URL format: %s\n", url_or_addr_ipv4.c_str());
            return;
        }

        addr_ipv4 = Platform::SocketTools::resolveHostname(url.hostname);
        if (addr_ipv4.empty())
        {
            printf("Failed to resolve hostname: %s\n", url.hostname.c_str());
            return;
        }

        printf("Parsed URL:\n");
        printf("  Protocol: %s\n", url.protocol.c_str());
        printf("  Hostname: %s\n", url.hostname.c_str());
        printf("  Resolved IPv4: %s\n", addr_ipv4.c_str());
        printf("  Port: %u\n", url.port);
        printf("  Path: %s\n", url.path.c_str());
    }
    else
    {
        addr_ipv4 = Platform::SocketTools::resolveHostname(url_or_addr_ipv4);
        url = Platform::SocketTools::URL("https", url_or_addr_ipv4, "/", 8444);
        url.ssl = false;
    }

    std::shared_ptr<Platform::SocketTCP> clientSocket;

    std::shared_ptr<ITKExtension::Network::SocketTCP_SSL> sslSocket;
    std::shared_ptr<TLS::CertificateChain> certificate_chain;

    if (url.ssl)
    {
        certificate_chain = TLS::CertificateChain::CreateShared();
        if (!client_cert_file.empty())
        {
            if (!certificate_chain->addCertificateFromFile(client_cert_file.c_str()))
            {
                printf("Failed to load client certificate file: %s\n", client_cert_file.c_str());
                return;
            }
        }
        else
            certificate_chain->addSystemCertificates();
        clientSocket = sslSocket = ITKExtension::Network::SocketTCP_SSL::CreateShared();
    }
    else
        clientSocket = std::make_shared<Platform::SocketTCP>();

    if (!clientSocket->connect(addr_ipv4, url.port))
    {
        printf("Connect failed!!!... interrupting current thread\n");
        clientSocket->close();
        return;
    }

    clientSocket->setNoDelay(true);
    // clientSocket->setWriteTimeout(500); // 500 ms write timeout
    // clientSocket->setReadTimeout(1500); // 1500 ms read timeout

    if (sslSocket != nullptr)
    {
        bool try_again = true;
        bool cert_added = false;
        while (try_again)
        {

            bool config_ok;
            if (!client_cert_file.empty()) // use Common Name (CN) from certificate
                config_ok = sslSocket->configureAsClient(certificate_chain, certificate_chain->getCertificate(0)->getSubjectCommonName().c_str(), true);
            else
                config_ok = sslSocket->configureAsClient(certificate_chain, url.hostname.c_str(), true);
            if (!config_ok)
            {
                printf("SSL Socket configuration failed!!!... interrupting current thread\n");
                clientSocket->close();
                return;
            }

            try_again = false;
            auto res = sslSocket->doHandshake([&](const std::string &error, std::shared_ptr<TLS::Certificate> certificate)
                                              {
                if (cert_added)
                    return; // already added cert, avoid loop
                auto der_encoded = certificate->getDEREncoded();
                
                // the client custom certificate needs to be the first, and add system certs after

                certificate_chain = TLS::CertificateChain::CreateShared();
                certificate_chain->addCertificate(der_encoded.data(), der_encoded.size());
                certificate_chain->addSystemCertificates();

                // certificate_chain->specialAddCertificateAsFirst(der_encoded.data(), der_encoded.size());

                client_cert_file = "force_load_first_certificate_from_server";
                printf(" Invalid certificate from: %s\n", certificate->getSubjectCommonName().c_str());
                cert_added = true; 
                try_again = true; });

            if (res == Platform::SOCKET_RESULT_WOULD_BLOCK || res == Platform::SOCKET_RESULT_TIMEOUT)
                Platform::Sleep::millis(1); // avoid busy wait and retry
            else if (res != Platform::SOCKET_RESULT_OK || Platform::Thread::isCurrentThreadInterrupted())
            {
                if (try_again)
                {
                    // recreate sslSocket
                    clientSocket = sslSocket = ITKExtension::Network::SocketTCP_SSL::CreateShared();
                    if (!clientSocket->connect(addr_ipv4, url.port))
                    {
                        printf("Connect failed!!!... interrupting current thread\n");
                        clientSocket->close();
                        return;
                    }
                    clientSocket->setNoDelay(true);

                    continue;
                }
                printf("SSL Handshake failed!!!... interrupting current thread\n");
                clientSocket->close();
                return;
            }
            else
                break; // handshake successful
        }
    }

    auto httpConnection = std::make_shared<ITKExtension::Network::HTTPConnection>(
        clientSocket, HTTPConnectionMode::Client);

    {
        auto req = HTTPRequestAsync::CreateShared();
        req->setDefault(url.hostname, "GET", url.path);
        if (!httpConnection->clientBeginWriteRequest(req))
        {
            printf("Failed to begin writing HTTP request...\n");
            httpConnection->close();
            return;
        }
        while (httpConnection->getState() != HTTPConnectionState::Client_WritingRequestComplete)
        {
            auto result = httpConnection->process();
            if (result == HTTPProcessingResult::Error)
            {
                printf("HTTP Connection processing error during request write...\n");
                httpConnection->close();
                return;
            }
            else if (result == HTTPProcessingResult::InProgress)
            {
                printf("Timeout on client writing socket... keep trying.\n");
                Platform::Sleep::millis(1);
            }
        }
    }

    {
        if (!httpConnection->clientBeginReadResponseHeaders())
        {
            printf("Failed to begin reading HTTP response...\n");
            httpConnection->close();
            return;
        }
        while (httpConnection->getState() != HTTPConnectionState::Client_ReadingResponseBodyComplete)
        {
            auto result = httpConnection->process();
            if (result == HTTPProcessingResult::Error)
            {
                printf("HTTP Connection processing error during request write...\n");
                httpConnection->close();
                return;
            }
            else if (result == HTTPProcessingResult::InProgress)
            {
                if (httpConnection->getState() == HTTPConnectionState::Client_ReadingResponseHeadersComplete)
                    httpConnection->clientContinueBodyReading();
                else
                    printf("Timeout on client reading socket... keep trying.\n");
                Platform::Sleep::millis(1);
            }
        }

        auto res = httpConnection->getResponse();
        printf("Received HTTP Response: %d %s\n", res->status_code, res->reason_phrase.c_str());
        printf("Headers:\n");
        for (const auto &header_pair : res->listHeaders())
            printf("  %s: %s\n", header_pair.first.c_str(), header_pair.second.c_str());

        if (ITKCommon::StringUtil::contains(res->getHeader("Content-Type"), "text/plain") ||
            ITKCommon::StringUtil::contains(res->getHeader("Content-Type"), "text/html") ||
            ITKCommon::StringUtil::contains(res->getHeader("Content-Type"), "application/json"))
            printf("Body: %s\n", res->bodyAsString().c_str());
    }

    printf("Closing connection...\n");
    httpConnection->close();
    // clientSocket->close();
}

void start_server(int port = 8444, const std::string &cert_file = "", const std::string &private_key_file = "")
{
    using namespace ITKExtension::Network;

    // std::shared_ptr<ITKExtension::Network::SocketTCP_SSL> sslSocket;
    std::shared_ptr<TLS::CertificateChain> certificate_chain;
    std::shared_ptr<TLS::PrivateKey> private_key;

    if (!cert_file.empty() && !private_key_file.empty())
    {
        certificate_chain = TLS::CertificateChain::CreateShared();
        private_key = TLS::PrivateKey::CreateShared();

        if (!certificate_chain->addCertificateFromFile(cert_file.c_str()))
        {
            printf("Failed to load server certificate file: %s\n", cert_file.c_str());
            return;
        }
        if (!private_key->setKeyNotEncryptedFromFile(private_key_file.c_str()))
        {
            printf("Failed to load server private key file: %s\n", private_key_file.c_str());
            return;
        }
        printf("Server certificate and private key loaded successfully.\n");
    }

    // blocking: true, reuseAddress: true, noDelay: true
    Platform::SocketTCPAccept serverSocket(true, true, true);

    if (!serverSocket.bindAndListen(
            "INADDR_ANY",              // interface address
            port,                      // port
            SERVER_ACCEPT_QUEUE_SIZE)) // input queue
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

    Platform::ThreadPool threadPool((std::max)(Platform::Thread::QueryNumberOfSystemThreads(), 4));

    Platform::Time time;
    std::shared_ptr<Platform::SocketTCP> clientSocket;

    if (certificate_chain != nullptr && private_key != nullptr)
        clientSocket = std::make_shared<ITKExtension::Network::SocketTCP_SSL>();
    else
        clientSocket = std::make_shared<Platform::SocketTCP>();

    while (!Platform::Thread::isCurrentThreadInterrupted())
    {
        bool connection_accepted = true;
        // blocking mode
        if (serverSocket.accept(clientSocket.get()) != Platform::SOCKET_RESULT_OK)
            break;

        time.update();
        if (time.deltaTime >= 1.0f)
            printf("[%s] task queue size: %u / %i\n", ITKCommon::Date::NowUTC().toISOString().c_str(), threadPool.taskInQueue(), SERVER_MAX_TASKS_QUEUE_SIZE);
        else
            time.rollback_and_set_zero();

        // if there are 200 or more tasks in the queue, refuse new connections
        if (threadPool.taskInQueue() >= SERVER_MAX_TASKS_QUEUE_SIZE)
        {
            clientSocket->close();
            continue;
        }

        auto certificate_chain_ptr = certificate_chain.get();
        auto private_key_ptr = private_key.get();

        threadPool.postTask([clientSocket, certificate_chain_ptr, private_key_ptr]()
                            {
            // thread pool finish is called
            // the remaining tasks in the queue are executed
            if (Platform::Thread::isCurrentThreadInterrupted()) 
            {
                printf("Thread pool finished. Closing remaining connections.\n");
                clientSocket->close();
                return;
            }
                
            // client thread (server side)
            Platform::SocketTCP *socket = clientSocket.get();
            socket->setNoDelay(true);
            socket->setReadTimeout(500);// 500 ms read timeout

            // Platform::Sleep::millis(100); // wait a bit for client to send data

            char client_str[32];
            snprintf(client_str, 32, "%s:%u", inet_ntoa(socket->getAddrOut().sin_addr), ntohs(socket->getAddrOut().sin_port));

            if (certificate_chain_ptr != nullptr && private_key_ptr != nullptr)
            {
                auto ssl_socket = (ITKExtension::Network::SocketTCP_SSL*)socket;
                if (!ssl_socket->configureAsServer(
                        std::shared_ptr<TLS::CertificateChain>(certificate_chain_ptr, [](TLS::CertificateChain *) {}),
                        std::shared_ptr<TLS::PrivateKey>(private_key_ptr, [](TLS::PrivateKey *) {}),
                        false // verify peer
                    ))
                {
                    printf("SSL configuration failed on %s\n", client_str);
                    ssl_socket->close();
                    return;
                }
                while (true)
                {
                    auto res = ssl_socket->doHandshake();
                    if (res == Platform::SOCKET_RESULT_WOULD_BLOCK || res == Platform::SOCKET_RESULT_TIMEOUT)
                        Platform::Sleep::millis(1); // avoid busy wait and retry
                    else
                    if (res != Platform::SOCKET_RESULT_OK || Platform::Thread::isCurrentThreadInterrupted())
                    {
                        printf("SSL Handshake failed on %s\n", client_str);
                        ssl_socket->close();
                        return;
                    }
                    else 
                        break; // handshake successful
                }
            }

            auto httpConnection = std::make_shared<ITKExtension::Network::HTTPConnection>(
                clientSocket, HTTPConnectionMode::Server);

            while (httpConnection->getState() != HTTPConnectionState::Server_ReadingRequestBodyComplete)
            {
                auto result = httpConnection->process();
                if (result == HTTPProcessingResult::Error)
                {
                    printf("HTTP Connection processing error during request write...\n");
                    httpConnection->close();
                    return;
                }
                else if (result == HTTPProcessingResult::InProgress) 
                {
                    if (httpConnection->getState() == HTTPConnectionState::Server_ReadingRequestHeadersComplete)
                        httpConnection->serverContinueBodyReading();
                    else
                        printf("Timeout on server reading socket from %s... keep trying.\n", client_str);
                    Platform::Sleep::millis(1);
                }
            }


            auto req = httpConnection->getRequest();

            auto res = HTTPResponseAsync::CreateShared();

            if (req->method == "GET" && req->path == "/")
                res->setDefault(200)
                    .setBody(
                    "Hello! This is a response from the C++ TCP server.\nYour IP is: " + std::string(client_str),
                    "text/plain");
            else 
                res->setDefault(404)
                    .setBody(res->reason_phrase,"text/plain");


            if (!httpConnection->serverBeginWriteResponse(res))
            {
                printf("Failed to begin reading HTTP response...\n");
                httpConnection->close();
                return;
            }
            while (httpConnection->getState() != HTTPConnectionState::Server_WritingResponseComplete)
            {
                auto result = httpConnection->process();
                if (result == HTTPProcessingResult::Error)
                {
                    printf("HTTP Connection processing error during request write...\n");
                    httpConnection->close();
                    return;
                }
                else if (result == HTTPProcessingResult::InProgress) 
                {
                    printf("Timeout on server writing socket from %s... keep trying.\n", client_str);
                    Platform::Sleep::millis(1);
                }
            }

            httpConnection->close(); });

        if (certificate_chain != nullptr && private_key != nullptr)
            clientSocket = std::make_shared<ITKExtension::Network::SocketTCP_SSL>();
        else
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
    TLS::GlobalSharedState::staticInitialization();

    Platform::Signal::Set([](int sngl)
                          {
        //ITK_ABORT(true, "forced app finishing.\n");
        Platform::Thread::getMainThread()->interrupt(); });

    if (argc == 3 && (strcmp(argv[1], "connect") == 0))
        connect(argv[2], false);
    else if (argc == 3 && (strcmp(argv[1], "wget") == 0))
        connect(argv[2], true);
    else if (argc == 4 && (strcmp(argv[1], "wget") == 0))
        connect(argv[2], true, argv[3]);
    else if (argc == 5 && (strcmp(argv[1], "server") == 0))
        start_server(atoi(argv[2]), argv[3], argv[4]);
    else if (argc == 2 && (strcmp(argv[1], "server") == 0))
        start_server();
    else
    {
        printf("Examples: \n"
               "\n"
               "# start server (on port: 8444):\n"
               "./extension-socket-tcp-tls server\n"
               "\n"
               "# connect to server (on port: 8444):\n"
               "./extension-socket-tcp-tls connect <IP>\n"
               "\n"
               "# start server:\n"
               "./extension-socket-tcp-tls server <port> <cert_file.crt> <private_key_file.key>\n"
               "\n"
               "# HTTPS wget hostname:\n"
               "./extension-socket-tcp-tls wget <http|https>://<subdomain.domain:port>/<path>\n"
               "\n"
               "# HTTPS wget hostname (with certificate):\n"
               "./extension-socket-tcp-tls wget https://<subdomain.domain:port>/<path> <cert_file.crt>\n"
               "\n");
    }

    return 0;
}
