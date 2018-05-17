#include <iostream>
#include <mutex>
#include <atomic>

namespace zmq_client 
{

    class zmqLogger {
        static std::string m_prefix;
        static int m_logLevel;
        static bool m_logEnabled;
        
        static std::mutex& getMutex();

        private:
            zmqLogger() { Setup(); }
            static std::atomic<zmqLogger*> m_instance;
            static std::mutex m_mutex;

        public:

            static zmqLogger* Instance();

            void Setup();

            void SetPrefix(const std::string& a_prefix);

            void EnableLog();
            void DisableLog();
            void SetLogEnabled(const bool& a_enabled);
        
            void Log(const std::string& a_msg, const int a_logLevel = 0);

        public:

            // Log Levels. Order matters.
            enum LOG_LEVELS : int {
                DEFAULT         = 0,
                CACHE_QUERIES   = 1,
                CACHE_FILE_IO   = 2,
                ZMQ_INTERNAL    = 3,
                ZMQ_QUERIES     = 4,
                ZMQ_ERROR       = 5,
            };
    };
};