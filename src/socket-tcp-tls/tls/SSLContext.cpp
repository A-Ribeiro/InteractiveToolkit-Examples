#include "SSLContext.h"
#include "TLSUtils.h"

#include <InteractiveToolkit/Platform/SocketTCP.h>

namespace TLS
{

    bool SSLContext::isInitialized() const
    {
        return initialized;
    }

    void SSLContext::initialize_structures()
    {
        if (!initialized)
        {
            mbedtls_ssl_init(&ssl_context);
            mbedtls_ssl_config_init(&ssl_config);
            initialized = true;
        }
    }
    void SSLContext::release_structures()
    {
        if (initialized)
        {
            mbedtls_ssl_config_free(&ssl_config);
            mbedtls_ssl_free(&ssl_context);
            initialized = false;
            is_client = false;
            is_server = false;
            handshake_done = false;
            private_key = nullptr;
            certificate_chain = nullptr;
        }
    }

    SSLContext::SSLContext() : initialized(false), is_client(false), is_server(false), handshake_done(false),
                               private_key(nullptr), certificate_chain(nullptr), ssl_context{}, ssl_config{}
    {
    }

    SSLContext::~SSLContext()
    {
        release_structures();
    }

    bool SSLContext::setupAsClient(std::shared_ptr<CertificateChain> &certificate_chain, const char *hostname, bool verify_peer)
    {
        if (this->certificate_chain != nullptr)
            return false;
        initialize_structures();

        is_client = true;
        this->certificate_chain = certificate_chain;

        int result = mbedtls_ssl_config_defaults(&ssl_config,
                                                 MBEDTLS_SSL_IS_CLIENT,
                                                 MBEDTLS_SSL_TRANSPORT_STREAM,
                                                 MBEDTLS_SSL_PRESET_DEFAULT);
        if (result != 0)
        {
            printf("Error setting up TLS: %s\n", TLSUtils::errorMessageFromReturnCode(result).c_str());
            release_structures();
            return false;
        }

        // random number generator explicitly is required before Mbed TLS 4.x.x
#if (MBEDTLS_VERSION_MAJOR < 4)
        mbedtls_ssl_conf_rng(&ssl_config, mbedtls_ctr_drbg_random, &GlobalSharedState::Instance()->ctr_drbg_context);
#endif

        // peer verification mode
        mbedtls_ssl_conf_authmode(&ssl_config, verify_peer ? MBEDTLS_SSL_VERIFY_REQUIRED : MBEDTLS_SSL_VERIFY_NONE);

        mbedtls_ssl_conf_ca_chain(&ssl_config, &certificate_chain->x509_crt, &certificate_chain->x509_crl);

        result = mbedtls_ssl_setup(&ssl_context, &ssl_config);
        if (result != 0)
        {
            printf("Error setting up TLS: %s\n", TLSUtils::errorMessageFromReturnCode(result).c_str());
            release_structures();
            return false;
        }

        // Set the hostname that is used for peer verification and sent via SNI if it is supported
        // Only for clients
        result = mbedtls_ssl_set_hostname(&ssl_context, hostname);
        if (result != 0)
        {
            printf("Error setting up TLS: %s\n", TLSUtils::errorMessageFromReturnCode(result).c_str());
            release_structures();
            return false;
        }

        return true;
    }

    bool SSLContext::setupAsServer(std::shared_ptr<CertificateChain> &certificate_chain, std::shared_ptr<PrivateKey> &private_key, bool verify_peer)
    {
        if (this->certificate_chain != nullptr)
            return false;
        initialize_structures();

        is_server = true;
        this->certificate_chain = certificate_chain;
        this->private_key = private_key;

        int result = mbedtls_ssl_config_defaults(&ssl_config,
                                                 MBEDTLS_SSL_IS_SERVER,
                                                 MBEDTLS_SSL_TRANSPORT_STREAM,
                                                 MBEDTLS_SSL_PRESET_DEFAULT);
        if (result != 0)
        {
            printf("Error setting up TLS: %s\n", TLSUtils::errorMessageFromReturnCode(result).c_str());
            release_structures();
            return false;
        }

        // random number generator explicitly is required before Mbed TLS 4.x.x
#if (MBEDTLS_VERSION_MAJOR < 4)
        mbedtls_ssl_conf_rng(&ssl_config, mbedtls_ctr_drbg_random, &GlobalSharedState::Instance()->ctr_drbg_context);
#endif

        // peer verification mode
        mbedtls_ssl_conf_authmode(&ssl_config, verify_peer ? MBEDTLS_SSL_VERIFY_REQUIRED : MBEDTLS_SSL_VERIFY_NONE);

        // certificate chain and private key
        //   -> load CA chain, except the first
        //   -> set the 1st CA certificate as own certificate (server certificate)
        if (certificate_chain->x509_crt.next != nullptr)
            mbedtls_ssl_conf_ca_chain(&ssl_config, certificate_chain->x509_crt.next, nullptr);
        result = mbedtls_ssl_conf_own_cert(&ssl_config, &certificate_chain->x509_crt, &private_key->private_key_context);
        if (result != 0)
        {
            printf("Error loading server certificate: %s\n", TLSUtils::errorMessageFromReturnCode(result).c_str());
            release_structures();
            return false;
        }

        result = mbedtls_ssl_setup(&ssl_context, &ssl_config);
        if (result != 0)
        {
            printf("Error setting up TLS: %s\n", TLSUtils::errorMessageFromReturnCode(result).c_str());
            release_structures();
            return false;
        }

        return true;
    }

    void SSLContext::communicateWithThisSocket(Platform::SocketTCP *socket)
    {
        if (this->certificate_chain == nullptr)
            return;

        mbedtls_ssl_set_bio(
            &ssl_context,
            &socket,
            [](void *context, const unsigned char *data, std::size_t size) -> int
            {
                Platform::SocketTCP *tcp_socket = static_cast<Platform::SocketTCP *>(context);
                uint32_t write_feedback;
                if (!tcp_socket->write_buffer(data, size, &write_feedback))
                    return MBEDTLS_ERR_SSL_WANT_WRITE;
                return (int)write_feedback;
            },
            [](void *context, unsigned char *data, std::size_t size) -> int
            {
                Platform::SocketTCP *tcp_socket = static_cast<Platform::SocketTCP *>(context);
                uint32_t read_feedback;
                tcp_socket->setReadTimeout(0);
                if (!tcp_socket->read_buffer(data, size, &read_feedback))
                    return MBEDTLS_ERR_SSL_WANT_READ;
                return (int)read_feedback;
            },
            [](void *context, unsigned char *data, std::size_t size, uint32_t timeout_ms) -> int
            {
                Platform::SocketTCP *tcp_socket = static_cast<Platform::SocketTCP *>(context);
                uint32_t read_feedback;
                tcp_socket->setReadTimeout(timeout_ms);
                if (!tcp_socket->read_buffer(data, size, &read_feedback))
                {
                    if (tcp_socket->isReadTimedout())
                        return MBEDTLS_ERR_SSL_TIMEOUT;
                    return MBEDTLS_ERR_SSL_WANT_READ;
                }
                return (int)read_feedback;
            });
    }

}
