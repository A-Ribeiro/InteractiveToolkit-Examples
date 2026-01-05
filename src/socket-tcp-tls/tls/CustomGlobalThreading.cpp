#include "CustomGlobalThreading.h"

#include <InteractiveToolkit/common.h>

namespace TLS
{

#if defined(MBEDTLS_THREADING_ALT)

    int CustomGlobalThreading::mutex_init(mbedtls_threading_mutex_t *ptr)
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
    void CustomGlobalThreading::mutex_destroy(mbedtls_threading_mutex_t *ptr)
    {
        delete static_cast<std::mutex *>(*ptr);
        *ptr = nullptr;
    }
    int CustomGlobalThreading::mutex_lock(mbedtls_threading_mutex_t *ptr)
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
    int CustomGlobalThreading::mutex_unlock(mbedtls_threading_mutex_t *ptr)
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

    int CustomGlobalThreading::cond_init(mbedtls_platform_condition_variable_t *ptr)
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
    void CustomGlobalThreading::cond_destroy(mbedtls_platform_condition_variable_t *ptr)
    {
        delete static_cast<std::condition_variable *>(*ptr);
        *ptr = nullptr;
    }
    int CustomGlobalThreading::cond_signal(mbedtls_platform_condition_variable_t *ptr)
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
    int CustomGlobalThreading::cond_broadcast(mbedtls_platform_condition_variable_t *ptr)
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
    int CustomGlobalThreading::cond_wait(mbedtls_platform_condition_variable_t *ptr,
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

    CustomGlobalThreading::CustomGlobalThreading()
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
            &CustomGlobalThreading::mutex_init,
            &CustomGlobalThreading::mutex_destroy,
            &CustomGlobalThreading::mutex_lock,
            &CustomGlobalThreading::mutex_unlock,

            &CustomGlobalThreading::cond_init,
            &CustomGlobalThreading::cond_destroy,
            &CustomGlobalThreading::cond_signal,
            &CustomGlobalThreading::cond_broadcast,
            &CustomGlobalThreading::cond_wait);
#endif
#endif
    }

    CustomGlobalThreading::~CustomGlobalThreading()
    {
#if defined(MBEDTLS_THREADING_ALT)
        mbedtls_threading_free_alt();
#endif
    }

    void CustomGlobalThreading::InitializeBeforeAnyTLSCommand()
    {
#if defined(MBEDTLS_THREADING_ALT)
        static CustomGlobalThreading instance;
#endif
    }
}
