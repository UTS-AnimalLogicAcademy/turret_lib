#include "zmqLogger.h"
#include <stdlib.h>

namespace zmq_client 
{
    // -- Private

    std::string zmqLogger::m_prefix = "ZMQ Resolver";
    int zmqLogger::m_logLevel = zmqLogger::ZMQ_INTERNAL;
    bool zmqLogger::m_logEnabled = true;

    std::mutex& zmqLogger::getMutex() { 
        static std::mutex m;
        return m;
    }

    std::atomic<zmqLogger*> zmqLogger::m_instance { nullptr };
    std::mutex zmqLogger::m_mutex;

    // -- Public

    zmqLogger* zmqLogger::Instance()
    {
        if(m_instance == nullptr) {
            std::lock_guard<std::mutex> lock(m_mutex);
            if(m_instance == nullptr) {
                m_instance = new zmqLogger();
            }
        }
        return m_instance;
    }

    void zmqLogger::Setup()
    {
        if(const char* env_p = std::getenv("DEBUG_LOG_LEVEL"))
            m_logLevel = atoi(env_p);
        if(const char* env_p = std::getenv("DEBUG_ENABLED"))
            m_logEnabled = (atoi(env_p) == 1) ? true : false;
    }

    void zmqLogger::SetPrefix(const std::string& a_prefix) { m_prefix = a_prefix; }

    void zmqLogger::EnableLog()
    {
        m_logEnabled = true;
    }

    void zmqLogger::DisableLog() 
    {
        m_logEnabled = false;
    }

    void zmqLogger::SetLogEnabled(const bool& a_enabled) 
    {
        m_logEnabled = a_enabled;
    }

    void zmqLogger::Log(const std::string& a_msg, const int a_logLevel) {
        if(m_logEnabled) {
            std::lock_guard<std::mutex> _(getMutex());
            if(a_logLevel >= m_logLevel) {
                std::cout << "[" << m_prefix << "] " << a_msg << std::endl;
            }
        }
    }

};