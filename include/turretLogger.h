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

#include <iostream>
#include <mutex>
#include <atomic>

namespace turret_client
{

    class turretLogger {
        static std::string m_prefix;
        static int m_logLevel;
        static bool m_logEnabled;
        static std::mutex& getMutex();

        private:
            turretLogger() { Setup(); }
            static std::atomic<turretLogger*> m_instance;
            static std::mutex m_mutex;

        public:

            static turretLogger* Instance();

            void Setup();

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