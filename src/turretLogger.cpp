#include "turretLogger.h"
#include <stdlib.h>

namespace turret_client {
    // -- Private
    std::string turretLogger::m_prefix = "Turret";
    int turretLogger::m_logLevel = turretLogger::ZMQ_INTERNAL;
    bool turretLogger::m_logEnabled = true;

    std::mutex& turretLogger::getMutex() { 
        static std::mutex m;
        return m;
    }

    std::atomic<turretLogger*> turretLogger::m_instance { nullptr };
    std::mutex turretLogger::m_mutex;

    // -- Public
    turretLogger* turretLogger::Instance() {
        if(m_instance == nullptr) {
            std::lock_guard<std::mutex> lock(m_mutex);
            if(m_instance == nullptr) {
                m_instance = new turretLogger();
            }
        }
        return m_instance;
    }

    void turretLogger::Setup() {
        if(const char* env_p = std::getenv("DEBUG_LOG_LEVEL"))
            m_logLevel = atoi(env_p);
        if(const char* env_p = std::getenv("DEBUG_ENABLED"))
            m_logEnabled = (atoi(env_p) == 1) ? true : false;
    }

    void turretLogger::EnableLog() {
        m_logEnabled = true;
    }

    void turretLogger::DisableLog() {
        m_logEnabled = false;
    }

    void turretLogger::SetLogEnabled(const bool& a_enabled) {
        m_logEnabled = a_enabled;
    }

    void turretLogger::Log(const std::string& a_msg, const int a_logLevel) {
        if(m_logEnabled) {
            std::lock_guard<std::mutex> _(getMutex());
            if(a_logLevel >= m_logLevel) {
                std::cout << "[" << m_prefix << "] " << a_msg << std::endl;
            }
        }
    }

};
