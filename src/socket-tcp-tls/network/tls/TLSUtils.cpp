#include "TLSUtils.h"

// #include <mbedtls/version.h>

// #if (MBEDTLS_VERSION_MAJOR < 4)
// #include <mbedtls/ctr_drbg.h>
// #include <mbedtls/entropy.h>
// #endif

#include <mbedtls/error.h>
// #include <mbedtls/net_sockets.h>
// #include <mbedtls/ssl.h>


namespace TLS
{
    std::string TLSUtils::errorMessageFromReturnCode(int errnum)
    {
        char buffer[1024];
        mbedtls_strerror(errnum, buffer, sizeof(buffer));
        return buffer;
    }
}
