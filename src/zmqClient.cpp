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
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/filesystem.hpp>

namespace zmq_client
{
    // -- Public

    // Constructors and Destructors

    zmqClient::zmqClient() : 
        m_useCache(zmq_client::ZMQ_CACHE_QUERIES), 
        m_cacheToDisk(zmq_client::ZMQ_CACHE_EXTERNAL),
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
        zmqLogger::Instance()->Log("Destroyed " + m_clientID + " client.", zmqLogger::LOG_LEVELS::ZMQ_INTERNAL);
    }

    // -- Protected 

    // Setup and destroy wrappers

    void zmqClient::setup()
    {
        // Cache query setup
        if(const char* env_p = std::getenv("ZMQ_CACHE_QUERIES")) {
            m_useCache = (env_p[0] == '1');
        }
        if(const char* env_p = std::getenv("ZMQ_CACHE_EXTERNAL")) {
            m_cacheToDisk = (env_p[0] == '1');
        }

        // the session_id is set by the DCC app on scene load/new scene
        if (const char* sessionID = std::getenv("TURRET_SESSION_ID")) {
            m_sessionID = sessionID;
        }

        zmqLogger::Instance()->Log("Created ZMQ Client. Caching Queries (Internal: "
                                   + std::string((m_useCache ? "True" : "False")) + ", External - "
                                   + std::string((m_cacheToDisk ? "True" : "False")) + ")",
                                   zmqLogger::LOG_LEVELS::ZMQ_INTERNAL);

        // Get cache path from env var if exists
        std::string clientIDUppercase = m_clientID;
        std::transform(clientIDUppercase.begin(), clientIDUppercase.end(),clientIDUppercase.begin(), ::toupper);

        // a cache filepath override was specified by env var:
        if(const char* env_p = std::getenv((clientIDUppercase + "_CACHE_LOCATION").c_str())) {
            m_cacheFilePath = env_p;
        }
        // determine cache filepath by variables:
        else {
            m_cacheFilePath = ZMQ_CACHE_LOCATION + m_clientID + "_" + m_sessionID + ZMQ_CACHE_FILETYPE;
        }

        // Load cache from file
        if(m_cacheToDisk) {
            zmqLogger::Instance()->Log(m_clientID + " resolver loading cache: "
                                       + std::to_string(loadCache()), zmqLogger::LOG_LEVELS::CACHE_FILE_IO);
        }
    }

    void zmqClient::destroy()
    {
        if(m_cacheToDisk) {
            saveCache();
        }
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
        if(!(boost::filesystem::exists(ZMQ_CACHE_LOCATION))) {
            if (boost::filesystem::create_directory(ZMQ_CACHE_LOCATION)) {
                zmqLogger::Instance()->Log(m_clientID + " resolver created zmq cache directory: " + ZMQ_CACHE_LOCATION);
            }
        }

        std::fstream fs(m_cacheFilePath.c_str(), std::fstream::out | std::ios::binary);
        boost::archive::text_oarchive oarch(fs);
//        boost::archive::binary_oarchive oarch(fs);
        oarch << m_cachedQueries;
        fs.close();
        zmqLogger::Instance()->Log(m_clientID + " resolver saved cache to " + m_cacheFilePath, zmqLogger::LOG_LEVELS::CACHE_FILE_IO);
    }

    bool zmqClient::loadCache()
    {
        std::fstream fs(m_cacheFilePath.c_str(), std::fstream::in | std::ios::binary);

        if(!fs.is_open()) {
            // Error opening file
            zmqLogger::Instance()->Log(m_clientID + " resolver no cache file present on disk.", zmqLogger::LOG_LEVELS::CACHE_FILE_IO);
            return false;
        }

        boost::archive::text_iarchive iarch(fs);
//        boost::archive::binary_iarchive iarch(fs);
        iarch >> m_cachedQueries;

        fs.close();

        zmqLogger::Instance()->Log(m_clientID + " resolver loaded cache from " + m_cacheFilePath, zmqLogger::LOG_LEVELS::CACHE_FILE_IO);
        zmqLogger::Instance()->Log(m_clientID + " resolver cache holds " + std::to_string(m_cachedQueries.size()) + " queries.", zmqLogger::LOG_LEVELS::CACHE_FILE_IO);


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
            if(i > 1) {
                zmqLogger::Instance()->Log(m_clientID + " resolver parser had to retry: " + std::to_string(i),
                                           zmqLogger::LOG_LEVELS::DEFAULT);
            }

            if(m_useCache) {
                zmqLogger::Instance()->Log(m_clientID + " resolver searching cache for query: " + a_query, zmqLogger::LOG_LEVELS::CACHE_QUERIES);

                // Search for cached result
                const std::map<std::string, zmq_client::zmqQueryCache>::iterator cached_result = m_cachedQueries.find(a_query);

                if (cached_result != m_cachedQueries.end()) {
                    //std::cout << "Current time: " << std::time(0) << "\n";
                    //std::cout << "Cache time: " << cached_result->second.timestamp << "\n";
                    // If cache is still fresh
                    //if(std::time(0) - cached_result->second.timestamp <= zmq_client::ZMQ_CACHE_TIMEOUT)
                    
                    zmqLogger::Instance()->Log(m_clientID + " resolver received Cached response: " + cached_result->second.resolved_path, zmqLogger::LOG_LEVELS::CACHE_QUERIES);
                    return cached_result->second.resolved_path;
                }
            }

            // Check socket status
            zmq::context_t m_context(1);
            zmq::socket_t m_socket(m_context, ZMQ_REQ);
            m_socket.connect("tcp://" + zmq_client::ZMQ_SERVER + ":" + zmq_client::ZMQ_PORT);
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

                zmqLogger::Instance()->Log(m_clientID + " resolver ZMQ ERROR: " + std::to_string(errnum) + " : " + std::string(errmsg), zmqLogger::LOG_LEVELS::ZMQ_ERROR);
                continue;
            }
        
            // Store the reply
            std::string realPath = std::string((char *)reply.data());

            if(realPath != "NOT_FOUND") {
                if (realPath[0] != '/') {
                    continue;
                }
            }

            zmqLogger::Instance()->Log(m_clientID + " resolver received query response: " + a_query + " | " + realPath, zmqLogger::LOG_LEVELS::ZMQ_QUERIES);

            if(m_useCache) {
                // Cache reply
                zmqQueryCache cache = {realPath, std::time(0)};
                m_cachedQueries.insert(std::make_pair(a_query, cache));
            }

            m_socket.close();
            m_context.close();
            return realPath;
        }

        zmqLogger::Instance()->Log(m_clientID + " resolver unable to query after "
                                   + std::to_string(zmq_client::ZMQ_RETRIES)
                                   + " retries.", zmqLogger::LOG_LEVELS::ZMQ_ERROR);

        return "Unable to parse query";
    }
}