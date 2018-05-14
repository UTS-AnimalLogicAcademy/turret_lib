#include <iostream>

namespace zmq_client 
{

    class zmqLogger {


        public:
            const static char* LOG_PREFIX;
            static void Log(const char* a_message);
    };
}; 