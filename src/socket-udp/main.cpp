#include <InteractiveToolkit/InteractiveToolkit.h>
#include <InteractiveToolkit/Platform/Platform.h>

#include <iostream>
#include <cstdlib>

using namespace ITKCommon;

void client_thread()
{
    printf("CLIENT THREAD\n");

    Platform::SocketUDP socket_write;
    socket_write.createFD(false, true, 1);
    socket_write.bind(); // ephemeral port and address...

    struct sockaddr_in server_addr = Platform::SocketUtils::mountAddress("127.0.0.1", 5002);

    char buffer[] = "UDP Send.";

    int count = 0;

    while (!Platform::Thread::isCurrentThreadInterrupted())
    {
        uint32_t bytes_written;
        if (!socket_write.write_buffer(server_addr,
                                      (uint8_t *)buffer, strlen(buffer) + 1,
                                      &bytes_written))
        {
            if (socket_write.isSignaled())
                break;

            // printf("[client] data not sent... bytes_written: %i, count:%i\n", bytes_written, count++);

            printf(".write error.(%i)\n", count++);
            Platform::Sleep::millis(300);
        }

        {
            char buffer[Platform::NetworkConstants::UDP_DATA_MTU_MINIMUM];
            struct sockaddr_in server_addr_copy = server_addr;
            uint32_t readed_bytes;
            if (socket_write.read_buffer(&server_addr_copy, (uint8_t *)buffer, Platform::NetworkConstants::UDP_DATA_MTU_MINIMUM, &readed_bytes))
            {
                buffer[readed_bytes] = '\0';
                printf("    (%i) Data from Server -> %s\n", count++, buffer);
            }
            else
            {
                printf(".no data from server.(%i)\n", count++);
                Platform::Sleep::millis(300);
            }
        }
    }

    socket_write.close();
}

void server_thread()
{

    printf("SERVER THREAD\n");

    Platform::SocketUDP socket_server;
    socket_server.createFD(true, true, 1);
    socket_server.bind("INADDR_ANY", 5002);

    struct sockaddr_in in_addr = {0};

    char buffer[Platform::NetworkConstants::UDP_DATA_MTU_MINIMUM];

    int count = 0;

    while (!Platform::Thread::isCurrentThreadInterrupted())
    {
        uint32_t bytes_readed;
        if (socket_server.read_buffer(&in_addr,
                                      (uint8_t *)buffer, Platform::NetworkConstants::UDP_DATA_MTU_MINIMUM,
                                      &bytes_readed))
        {

            // printf("[server] data received. bytes_readed: %i, count:%i\n", bytes_readed, count++);

            buffer[bytes_readed] = '\0';
            printf("    Data from Client(%i) -> %s\n", count++, buffer);

            char hello_world[] = "Hello World!!!";
            if (!socket_server.write_buffer(in_addr, (uint8_t *)hello_world, strlen(hello_world) + 1))
            {

                printf("[server] WRITE ERROR!!! waiting... (%i) \n", count++);
                Platform::Sleep::millis(10000);
            }
        }
        else
        {
            if (socket_server.isSignaled())
                break;
            // printf("[server] no data. bytes_readed: %i, count:%i\n", bytes_readed, count++);
            Platform::Sleep::millis(300);
        }
    }

    socket_server.close();
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

    if (argc == 2 && (strcmp(argv[1], "client") == 0))
    {
        client_thread();
    }
    else if (argc == 2 && (strcmp(argv[1], "server") == 0))
    {
        server_thread();
    }
    else
    {
        printf("Examples: \n"
               "\n"
               "# start client UDP socket:\n"
               "./socket-udp client\n"
               "\n"
               "# start server UDP socket:\n"
               "./socket-udp server\n"
               "\n");
    }

    return 0;
}
