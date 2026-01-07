#pragma once

// #include "SocketTCP_SSL.h"
#include <InteractiveToolkit/common.h>

namespace Platform
{
    class SocketTCP;
}

namespace ITKExtension
{
    namespace Network
    {

        const int HTTP_MAX_HEADER_COUNT = 100;
        const int HTTP_MAX_HEADER_RAW_SIZE = 1024;
        const int HTTP_READ_BUFFER_CHUNK_SIZE = 4 * 1024; // 4 KB
        const int HTTP_MAX_BODY_SIZE = 100 * 1024 * 1024; // 100 MB

        class HTTP
        {
        protected:
            virtual bool read_first_line(const std::string &firstLine) = 0;
            virtual std::string mount_first_line() = 0;

            static bool is_valid_header_character(char _chr);

        public:
            std::unordered_map<std::string, std::string> headers;
            std::vector<uint8_t> body;

            HTTP() = default;
            virtual ~HTTP() = default;

            virtual void clear() = 0;

            bool readFromStream(Platform::SocketTCP *socket);

            bool writeToStream(Platform::SocketTCP *socket);

            HTTP &setHeader(const std::string &key,
                            const std::string &value);

            HTTP &setBody(const std::string &body = "",
                          const std::string &content_type = "text/plain");

            HTTP &setBody(const uint8_t *body, uint32_t body_size,
                          const std::string &content_type = "application/octet-stream");

            std::string bodyAsString() const;
        };

    }
}