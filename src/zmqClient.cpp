#include "zmqClient.h"

#include <cstdlib>
#include <ctime>

#include <zmq.hpp>

#include "zmqLogger.h"

#include <sstream>
#include <fstream>

#include <sys/stat.h>

#include <boost/serialization/map.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/filesystem.hpp>

namespace zmq_client
{
    // -- Public

    // Constructors and Destructors

    zmqClient::zmqClient() : 
        m_useCache(zmq_client::ZMQ_CACHE_QUERIES), 
        m_cacheToDisk(zmq_client::ZMQ_CACHE_EXTERNAL) 
        m_clientID("default"), 
        m_cacheFilePath("")
    { 
        setup();
    }

    zmqClient::zmqClient(const char* a_clientID) : 
        m_useCache(zmq_client::ZMQ_CACHE_QUERIES), 
        m_cacheToDisk(zmq_client::ZMQ_CACHE_EXTERNAL), 
        m_clientID(a_clientID), 
        m_cacheFilePath("")
    {
        setup();
    }

    zmqClient::~zmqClient() 
    { 
        destroy();
        std::cout << zmqLogger::LOG_PREFIX << "Destroyed ZMQ Client." << std::endl;
    }

    // -- Protected 

    // Setup and destroy wrappers

    void zmqClient::setup()
    {
        // Cache query setup
        if(const char* env_p = std::getenv("ZMQ_CACHE_QUERIES"))
            m_useCache = (env_p[0] == '1');
        if(const char* env_p = std::getenv("ZMQ_CACHE_EXTERNAL"))
            m_cacheToDisk = (env_p[0] == '1');
            
        
        std::cout << zmqLogger::LOG_PREFIX << "Created ZMQ Client! Caching Queries: (Internal - " << (m_useCache ? "True" : "False") << ", External - " << (m_cacheToDisk ? "True" : "False") << std::endl;

        // Disk location for zmq cache file
        m_cacheFilePath = ZMQ_CACHE_LOCATION + m_clientID + ZMQ_CACHE_FILETYPE;

        // Get cache path from env var if exists
        std::string clientIDUppercase = m_clientID;
        std::transform(clientIDUppercase.begin(), clientIDUppercase.end(),clientIDUppercase.begin(), ::toupper);

        if(const char* env_p = std::getenv((clientIDUppercase + "_CACHE_LOCATION").c_str()))
            m_cacheFilePath = env_p;

        // Load cache from file
        if(m_cacheToDisk)
            std::cout << "Loading cache: " << loadCache();
    }

    void zmqClient::destroy()
    {
        if(m_cacheToDisk)
            saveCache();
    }

    // -- Protected

    // Cache functions

    void zmqClient::ClearCache()
    {
        m_cachedQueries.clear();

        // TODO: Should it also save over the cache on disk?
    }

    // -- Protected

    // Cache related functions
    
    void zmqClient::saveCache()
    {
        // Check that the location on disk exists. Create if it doesn't
        if(!(boost::filesystem::exists(ZMQ_CACHE_LOCATION))){
            if (boost::filesystem::create_directory(ZMQ_CACHE_LOCATION))
                std::cout << "Created zmq cache directory: " << ZMQ_CACHE_LOCATION << std::endl;
        }

        std::fstream fs(m_cacheFilePath.c_str(), std::fstream::out | std::ios::binary);

        boost::archive::text_oarchive oarch(fs);
        oarch << m_cachedQueries;

        fs.close();

        std::cout << "Saved cache to " << m_cacheFilePath << std::endl;
    }

    bool zmqClient::loadCache()
    {
        std::fstream fs(m_cacheFilePath.c_str(), std::fstream::in | std::ios::binary);

        if(!fs.is_open()) {
            // Error opening file
            std::cout << "No cache file present on disk.\n";
            return false;
        }

        boost::archive::text_iarchive iarch(fs);
        iarch >> m_cachedQueries;

        fs.close();

        std::cout << "Loaded cache from " << m_cacheFilePath << std::endl;
        std::cout << "Cache holds " << m_cachedQueries.size() << " queries." << std::endl;


        return true;
    }

    void zmqClient::appendCache()
    {
        loadCache(); //This may introduce cache collisions, test how std::map handles
        saveCache();
    }

    // -- Protected

    // Resolving functions
    
    std::string zmqClient::resolve_name(const std::string& a_path)
    {
        //std::cout << "ALA USD Resolver - resolving name: " << a_path << "\n\n";
        const std::string parsed_path = this->parse_query(a_path);
        return parsed_path;
    }

    bool zmqClient::resolve_exists(const std::string& a_path)
    {
        const std::string parsed_path = zmqClient::resolve_name(a_path);
        if(parsed_path == "NOT_FOUND")
            return false;
        else 
            return true;
    }

    // Returns if path matches tank query schema
    bool zmqClient::matches_schema(const std::string& a_path)
    {
        return a_path.find(TANK_PREFIX_SHORT) == 0;
    }

    // -- Protected

    // Parseing functions

    std::string zmqClient::parse_query(const std::string& a_query)
    {
        for(int i = 0; i < zmq_client::ZMQ_RETRIES; i++)
        {
            if(i > 1)
                std::cout << zmqLogger::LOG_PREFIX << "Parser had to retry: " << i <<  " | " << a_query << "\n\n\n";

            if(m_useCache) {
                std::cout << "Searching cache for query: " << a_query << std::endl;

                // Search for cached result
                const std::map<std::string, zmq_client::zmqQueryCache>::iterator cached_result = m_cachedQueries.find(a_query);

                if (cached_result != m_cachedQueries.end()) {
                    //std::cout << "Current time: " << std::time(0) << "\n";
                    //std::cout << "Cache time: " << cached_result->second.timestamp << "\n";
                    // If cache is still fresh
                    //if(std::time(0) - cached_result->second.timestamp <= zmq_client::ZMQ_CACHE_TIMEOUT)
                    std::cout << zmqLogger::LOG_PREFIX << "Received Cached response: " << cached_result->second.resolved_path << "\n\n";
                    return cached_result->second.resolved_path;
                }
            }

            // Check socket status

            zmq::context_t m_context(1);
            zmq::socket_t m_socket(m_context, ZMQ_REQ);
        
            //std::cout << "ALA USD Resolver - Connecting to local zmq server..." << "\n";
            m_socket.connect("tcp://" + std::string(zmq_client::ZMQ_SERVER) + ":" + std::string(zmq_client::ZMQ_PORT));

            m_socket.setsockopt(ZMQ_LINGER, zmq_client::ZMQ_TIMEOUT);
            m_socket.setsockopt(ZMQ_RCVTIMEO, zmq_client::ZMQ_TIMEOUT);

            // Create zmq request
            zmq::message_t request(a_query.c_str(), a_query.length());
            
            // Send zmq request
            m_socket.send(request/* , ZMQ_NOBLOCK */);

            // Wait for the reply
            zmq::message_t reply;
            int result = m_socket.recv(&reply);

            if(result < 1) {
                int errnum = zmq_errno();
                //There has been an error
                const char* errmsg = zmq_strerror(errnum);

                std::cout << zmqLogger::LOG_PREFIX << "ZMQ ERROR: " << errnum << " : " << errmsg << "\n\n\n";
                
                continue;
            }
        
            // Store the reply
            std::string realPath = std::string((char *)reply.data());

            if(realPath != "NOT_FOUND")
                if(realPath[0] != '/')
                    continue;

            std::cout << zmqLogger::LOG_PREFIX << "Received Query Response: " << a_query << "  |  " << realPath << "\n\n";

            if(m_useCache) {
                // Cache reply
                zmqQueryCache cache = {realPath, std::time(0)};
                
                m_cachedQueries.insert(std::make_pair(a_query, cache));
            }

            m_socket.close();
            m_context.close();

            return realPath;
        }
        return "Unable to parse query";
    }
}