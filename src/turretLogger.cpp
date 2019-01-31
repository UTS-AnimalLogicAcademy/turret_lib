//
// Copyright 2019 University of Technology, Sydney
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
// documentation files (the "Software"), to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
// to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
//   * The above copyright notice and this permission notice shall be included in all copies or substantial portions of
//     the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

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
        if(&m_instance == nullptr) {
            std::lock_guard<std::mutex> lock(m_mutex);
            if(&m_instance == nullptr) {
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
                std::cout << "[" << m_prefix.c_str() << "] " << a_msg.c_str() << std::endl;
            }
        }
    }

};
