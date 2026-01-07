#pragma once

#include <InteractiveToolkit/Platform/SocketTCP.h>
#include "tls/SSLContext.h"

namespace Platform
{
    class SocketTCP_SSL : public SocketTCP
    {
    public:
        TLS::SSLContext sslContext;

        bool write_buffer(const uint8_t *data, uint32_t size, uint32_t *write_feedback)
        {
            return sslContext.write_buffer(data, size, write_feedback);
        }
        bool read_buffer(uint8_t *data, uint32_t size, uint32_t *read_feedback, bool only_returns_if_match_exact_size = false)
        {
            if (only_returns_if_match_exact_size)
            {
                *read_feedback = 0;
                while (*read_feedback < size)
                {
                    if (!sslContext.read_buffer(data + *read_feedback, size - *read_feedback, read_feedback))
                        return *read_feedback == size;
                }
                return true;
            }
            return sslContext.read_buffer(data, size, read_feedback);
        }
        void close()
        {
            sslContext.close();
            SocketTCP::close();
        }
    };
}
