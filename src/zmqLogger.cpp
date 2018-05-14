#include "zmqLogger.h"

#include <iostream>


namespace zmq_client 
{

    const char* zmqLogger::LOG_PREFIX = "[ZMQ Resolver] ";

    void zmqLogger::Log(const char* a_message)
    {
        std::cout << LOG_PREFIX << a_message << std::endl;
    }

}