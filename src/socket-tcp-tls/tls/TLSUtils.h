#pragma once

// #include <InteractiveToolkit/Platform/SocketTCP.h>
#include <InteractiveToolkit/common.h>

namespace TLS
{
    class TLSUtils
    {
        public:
        static std::string errorMessageFromReturnCode(int errnum);
    };
}
