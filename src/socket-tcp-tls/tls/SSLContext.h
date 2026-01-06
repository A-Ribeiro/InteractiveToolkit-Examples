#pragma once

#include "PrivateKey.h"
#include "CertificateChain.h"

// #include <InteractiveToolkit/Platform/SocketTCP.h>
// #include <InteractiveToolkit/EventCore/Callback.h>

// #include <mbedtls/error.h>
// #include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>

// #include <InteractiveToolkit/EventCore/Callback.h>
// #include <InteractiveToolkit/Platform/Core/ObjectBuffer.h>


namespace TLS
{
    class SSLContext
    {
        bool initialized;
    public:
        bool handshake_done;
        mbedtls_ssl_context ssl_context;
        mbedtls_ssl_config ssl_config;

        // for server
        PrivateKey private_key;

        // for server and client
        CertificateChain certificate_chain;
    };
}
