#include "CertificateChain.h"
#include "TLSUtils.h"
#include "SystemCertificates.h"
#include <InteractiveToolkit/common.h>
#include <InteractiveToolkit/ITKCommon/FileSystem/File.h>

namespace TLS
{

    bool CertificateChain::isInitialized() const
    {
        return initialized;
    }

    void CertificateChain::initialize_structures()
    {
        if (!initialized)
        {
            mbedtls_x509_crt_init(&x509_crt);
            mbedtls_x509_crl_init(&x509_crl);
            initialized = true;
        }
    }
    void CertificateChain::release_structures()
    {
        if (initialized)
        {
            mbedtls_x509_crl_free(&x509_crl);
            mbedtls_x509_crt_free(&x509_crt);
            initialized = false;
        }
    }

    CertificateChain::CertificateChain() : initialized(false), x509_crt{}, x509_crl{}
    {
    }

    CertificateChain::~CertificateChain()
    {
        release_structures();
    }

    bool CertificateChain::addCertificate(const uint8_t *data, size_t length, bool add_all_certificates_is_required)
    {
        initialize_structures();
        std::string dataStr((const char *)data, length);
        int result = mbedtls_x509_crt_parse(&x509_crt, (const unsigned char *)dataStr.c_str(), strlen(dataStr.c_str()) + 1);
        if (result < 0)
            printf("Failed to add certificate: %s\n", TLS::TLSUtils::errorMessageFromReturnCode(result).c_str());
        if (add_all_certificates_is_required)
        {
            if (result > 0)
                printf("Error: %d certificates were added, some were ignored.\n", result);
            return result == 0;
        }
        // may ignore some certificates...
        return result >= 0;
    }
    bool CertificateChain::addCertificateRevokationList(const uint8_t *data, size_t length, bool add_all_crl_is_required)
    {
        initialize_structures();
        std::string dataStr((const char *)data, length);
        int result = mbedtls_x509_crl_parse(&x509_crl, (const unsigned char *)dataStr.c_str(), strlen(dataStr.c_str()) + 1);
        if (result < 0)
            printf("Failed to add certificate: %s\n", TLS::TLSUtils::errorMessageFromReturnCode(result).c_str());
        if (add_all_crl_is_required)
        {
            if (result > 0)
                printf("Error: %d certificates were added, some were ignored.\n", result);
            return result == 0;
        }
        // may ignore some certificates...
        return result >= 0;
    }

    bool CertificateChain::addCertificateFromFile(const char *path, bool add_all_certificates_is_required)
    {
        auto file = ITKCommon::FileSystem::File::FromPath(path);
        if (!file.isFile)
            return false;
        Platform::ObjectBuffer data;
        if (!file.readContentToObjectBuffer(&data))
            return false;
        return addCertificate(data.data, data.size, add_all_certificates_is_required);
    }

    bool CertificateChain::addCertificateRevokationListFromFile(const char *path, bool add_all_crl_is_required)
    {
        auto file = ITKCommon::FileSystem::File::FromPath(path);
        if (!file.isFile)
            return false;
        Platform::ObjectBuffer data;
        if (!file.readContentToObjectBuffer(&data))
            return false;
        return addCertificateRevokationList(data.data, data.size, add_all_crl_is_required);
    }

    bool CertificateChain::addSystemCertificates(bool add_all_certificates_is_required, bool add_all_crl_is_required)
    {
        SystemCertificates sys_certs;
        return sys_certs.iterate_over_x509_certificates( //
            [this, add_all_certificates_is_required](const uint8_t *data, size_t size)
            { addCertificate(data, size, add_all_certificates_is_required); },
            [this, add_all_crl_is_required](const uint8_t *data, size_t size)
            { addCertificateRevokationList(data, size, add_all_crl_is_required); });
    }

}
