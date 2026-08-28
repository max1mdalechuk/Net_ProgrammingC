#ifndef WRAPPERS_H
#define WRAPPERS_H

#ifdef _WIN32
    #include <winsock2.h>
    #include <windows.h>
#else
    #include <pthread.h>
    #include <unistd.h>
    typedef void* LPVOID;
#endif

#include <stdio.h>


class Mutex {
private:
#ifdef _WIN32
    CRITICAL_SECTION m_cs;
#else
    pthread_mutex_t m_mutex;
#endif

public:
    Mutex() {
#ifdef _WIN32
        InitializeCriticalSection(&m_cs);
#else
        pthread_mutex_init(&m_mutex, NULL);
#endif
    }

    ~Mutex() {
#ifdef _WIN32
        DeleteCriticalSection(&m_cs);
#else
        pthread_mutex_destroy(&m_mutex);
#endif
    }

    void Lock() {
#ifdef _WIN32
        EnterCriticalSection(&m_cs);
#else
        pthread_mutex_lock(&m_mutex);
#endif
    }

    void Unlock() {
#ifdef _WIN32
        LeaveCriticalSection(&m_cs);
#else
        pthread_mutex_unlock(&m_mutex);
#endif
    }
};


class Thread {
private:
#ifdef _WIN32
    HANDLE m_hThread;
    DWORD m_id;
    
    static DWORD WINAPI InternalRun(LPVOID param) {
        Thread* pThis = (Thread*)param;
        pThis->Run();
        return 0;
    }
#else
    pthread_t m_thread;

    static void* InternalRun(void* param) {
        Thread* pThis = (Thread*)param;
        pThis->Run();
        return NULL;
    }
#endif

public:
    Thread() {
#ifdef _WIN32
        m_hThread = NULL;
#endif
    }

    virtual ~Thread() {
        if (IsRunning()) {
#ifdef _WIN32
            CloseHandle(m_hThread);
#endif
        }
    }

    virtual void Run() = 0;

    bool Start() {
#ifdef _WIN32
        m_hThread = CreateThread(NULL, 0, InternalRun, this, 0, &m_id);
        return m_hThread != NULL;
#else
        return pthread_create(&m_thread, NULL, InternalRun, this) == 0;
#endif
    }

    void Join() {
#ifdef _WIN32
        WaitForSingleObject(m_hThread, INFINITE);
#else
        pthread_join(m_thread, NULL);
#endif
    }

    bool IsRunning() const {
#ifdef _WIN32
        return m_hThread != NULL;
#else
        return m_thread != 0;
#endif
    }
    
    void Detach() {
#ifdef _WIN32
        if (m_hThread) {
            CloseHandle(m_hThread);
            m_hThread = NULL;
        }
#else
        pthread_detach(m_thread);
#endif
    }
};

#endif 