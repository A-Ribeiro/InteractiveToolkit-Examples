#include "GlobalSharedState.h"

#include <InteractiveToolkit/common.h>
#include <InteractiveToolkit/ITKCommon/ITKAbort.h>

#include "TLSUtils.h"

#if (MBEDTLS_VERSION_MAJOR >= 4)
#include <psa/crypto.h>
#endif

namespace TLS
{

#if defined(MBEDTLS_THREADING_ALT)

    int GlobalSharedState::mutex_init(mbedtls_threading_mutex_t *ptr)
    {
        try
        {
            *ptr = static_cast<void *>(new std::mutex());
        }
        catch (const std::exception &)
        {
            return MBEDTLS_ERR_THREADING_USAGE_ERROR;
        }
        return 0;
    }
    void GlobalSharedState::mutex_destroy(mbedtls_threading_mutex_t *ptr)
    {
        delete static_cast<std::mutex *>(*ptr);
        *ptr = nullptr;
    }
    int GlobalSharedState::mutex_lock(mbedtls_threading_mutex_t *ptr)
    {
        try
        {
            static_cast<std::mutex *>(*ptr)->lock();
        }
        catch (const std::exception &)
        {
            return MBEDTLS_ERR_THREADING_USAGE_ERROR;
        }
        return 0;
    }
    int GlobalSharedState::mutex_unlock(mbedtls_threading_mutex_t *ptr)
    {
        try
        {
            static_cast<std::mutex *>(*ptr)->unlock();
        }
        catch (const std::exception &)
        {
            return MBEDTLS_ERR_THREADING_USAGE_ERROR;
        }
        return 0;
    }

#if (MBEDTLS_VERSION_MAJOR >= 4)

    int GlobalSharedState::cond_init(mbedtls_platform_condition_variable_t *ptr)
    {
        try
        {
            *ptr = static_cast<void *>(new std::condition_variable());
        }
        catch (const std::exception &)
        {
            return MBEDTLS_ERR_THREADING_USAGE_ERROR;
        }
        return 0;
    }
    void GlobalSharedState::cond_destroy(mbedtls_platform_condition_variable_t *ptr)
    {
        delete static_cast<std::condition_variable *>(*ptr);
        *ptr = nullptr;
    }
    int GlobalSharedState::cond_signal(mbedtls_platform_condition_variable_t *ptr)
    {
        try
        {
            static_cast<std::condition_variable *>(*ptr)->notify_one();
        }
        catch (const std::exception &)
        {
            return MBEDTLS_ERR_THREADING_USAGE_ERROR;
        }
        return 0;
    }
    int GlobalSharedState::cond_broadcast(mbedtls_platform_condition_variable_t *ptr)
    {
        try
        {
            static_cast<std::condition_variable *>(*ptr)->notify_all();
        }
        catch (const std::exception &)
        {
            return MBEDTLS_ERR_THREADING_USAGE_ERROR;
        }
        return 0;
    }
    int GlobalSharedState::cond_wait(mbedtls_platform_condition_variable_t *ptr,
                                     mbedtls_platform_mutex_t *mutex)
    {
        try
        {
            std::unique_lock<std::mutex> lock(*static_cast<std::mutex *>(*mutex), std::adopt_lock);
            static_cast<std::condition_variable *>(*ptr)->wait(lock);
            lock.release();
        }
        catch (const std::exception &)
        {
            return MBEDTLS_ERR_THREADING_USAGE_ERROR;
        }
        return 0;
    }
#endif

#endif

    GlobalSharedState::GlobalSharedState()
    {
#if defined(MBEDTLS_THREADING_ALT)
#if (MBEDTLS_VERSION_MAJOR < 4)
        mbedtls_threading_set_alt(
            &CustomGlobalThreading::mutex_init,
            &CustomGlobalThreading::mutex_destroy,
            &CustomGlobalThreading::mutex_lock,
            &CustomGlobalThreading::mutex_unlock);
#elif (MBEDTLS_VERSION_MAJOR >= 4)
        mbedtls_threading_set_alt(
            &GlobalSharedState::mutex_init,
            &GlobalSharedState::mutex_destroy,
            &GlobalSharedState::mutex_lock,
            &GlobalSharedState::mutex_unlock,

            &GlobalSharedState::cond_init,
            &GlobalSharedState::cond_destroy,
            &GlobalSharedState::cond_signal,
            &GlobalSharedState::cond_broadcast,
            &GlobalSharedState::cond_wait);
#endif
#endif

#if (MBEDTLS_VERSION_MAJOR < 4)
        // crypto library init before or equal 3.x.x
        mbedtls_entropy_init(&entropyContext);
        mbedtls_ctr_drbg_init(&ctrDrbgContext);

        int result = mbedtls_ctr_drbg_seed(&ctrDrbgContext,
                                           mbedtls_entropy_func,
                                           &entropyContext,
                                           "-mbedtls-" MBEDTLS_VERSION_STRING_FULL,
                                           strlen("-mbedtls-" MBEDTLS_VERSION_STRING_FULL));

        ITK_ABORT(result != 0, "Failed to seed DRBG: %s\n", errorMessageFromReturnCode(result).c_str());
#else
        // crypto library init after or equal 4.x.x
        psa_status_t result = psa_crypto_init();
        ITK_ABORT(result != PSA_SUCCESS, "Failed to initialize crypto library\n");
#endif
    }

    GlobalSharedState::~GlobalSharedState()
    {
#if (MBEDTLS_VERSION_MAJOR < 4)
        mbedtls_ctr_drbg_free(&ctrDrbgContext);
        mbedtls_entropy_free(&entropyContext);
#endif

#if defined(MBEDTLS_THREADING_ALT)
        mbedtls_threading_free_alt();
#endif
    }

    GlobalSharedState *GlobalSharedState::Instance()
    {
        static GlobalSharedState instance;
        return &instance;
    }
}
