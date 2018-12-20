#include "turretClient.h"

#include <cstdlib>
#include <ctime>

#include <zmq.hpp>

#include "turretLogger.h"

#include <sstream>
#include <fstream>

#include <sys/stat.h>

#include <boost/serialization/map.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/filesystem.hpp>

namespace turret_client
{
    // -- Public

    // Constructors and Destructors

    turretClient::turretClient() : 
        m_useCache(turret_client::ZMQ_CACHE_QUERIES), 
        m_cacheToDisk(turret_client::ZMQ_CACHE_EXTERNAL),
        m_clientID("default"), 
        m_cacheFilePath("")
    { 
        setup();
    }

    turretClient::turretClient(const char* a_clientID) : 
        m_useCache(turret_client::ZMQ_CACHE_QUERIES), 
        m_cacheToDisk(turret_client::ZMQ_CACHE_EXTERNAL), 
        m_clientID(a_clientID), 
        m_cacheFilePath("")
    {
        setup();
    }

    turretClient::~turretClient() 
    { 
        destroy();
        turretLogger::Instance()->Log("Destroyed " + m_clientID + " client.", turretLogger::LOG_LEVELS::ZMQ_INTERNAL);
    }

    // -- Protected 

    // Setup and destroy wrappers

    void turretClient::setup()
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

        turretLogger::Instance()->Log("Created ZMQ Client. Caching Queries (Internal: "
                                   + std::string((m_useCache ? "True" : "False")) + ", External - "
                                   + std::string((m_cacheToDisk ? "True" : "False")) + ")",
                                   turretLogger::LOG_LEVELS::ZMQ_INTERNAL);

        // Get cache path from env var if exists
        std::string clientIDUppercase = m_clientID;
        std::transform(clientIDUppercase.begin(), clientIDUppercase.end(),clientIDUppercase.begin(), ::toupper);

        // a cache filepath override was specified by env var:
        if(const char* env_p = std::getenv((clientIDUppercase + "_CACHE_LOCATION").c_str())) {
            m_cacheFilePath = env_p;
            m_cacheToDisk = false;
        }

        // determine cache filepath by variables:
        else {
            m_cacheFilePath = ZMQ_CACHE_LOCATION + m_clientID + "_" + m_sessionID + ZMQ_CACHE_FILETYPE;
        }

        // Load cache from file
        if(m_cacheToDisk) {
            turretLogger::Instance()->Log(m_clientID + " resolver loading cache: "
                                       + std::to_string(loadCache()), turretLogger::LOG_LEVELS::CACHE_FILE_IO);
        }
    }

    void turretClient::destroy()
    {
        if(m_cacheToDisk) {
            saveCache();
        }
    }

    // -- Protected

    // Cache functions

    void turretClient::ClearCache()
    {
        m_cachedQueries.clear();
        // TODO: Should it also save over the cache on disk?
    }

    // -- Protected

    // Cache related functions
    
    void turretClient::saveCache()
    {
        // Check that the location on disk exists. Create if it doesn't
        if(!(boost::filesystem::exists(ZMQ_CACHE_LOCATION))) {
            if (boost::filesystem::create_directory(ZMQ_CACHE_LOCATION)) {
                turretLogger::Instance()->Log(m_clientID + " resolver created zmq cache directory: " + ZMQ_CACHE_LOCATION);
            }
        }

        std::fstream fs(m_cacheFilePath.c_str(), std::fstream::out | std::ios::binary);
        boost::archive::text_oarchive oarch(fs);
//        boost::archive::binary_oarchive oarch(fs);
        oarch << m_cachedQueries;
        fs.close();
        turretLogger::Instance()->Log(m_clientID + " resolver saved cache to " + m_cacheFilePath, turretLogger::LOG_LEVELS::CACHE_FILE_IO);
    }

    bool turretClient::loadCache()
    {
        std::fstream fs(m_cacheFilePath.c_str(), std::fstream::in | std::ios::binary);

        if(!fs.is_open()) {
            // Error opening file
            turretLogger::Instance()->Log(m_clientID + " resolver no cache file present on disk.", turretLogger::LOG_LEVELS::CACHE_FILE_IO);
            return false;
        }

        boost::archive::text_iarchive iarch(fs);
//        boost::archive::binary_iarchive iarch(fs);
        iarch >> m_cachedQueries;

        fs.close();

        turretLogger::Instance()->Log(m_clientID + " resolver loaded cache from " + m_cacheFilePath, turretLogger::LOG_LEVELS::CACHE_FILE_IO);
        turretLogger::Instance()->Log(m_clientID + " resolver cache holds " + std::to_string(m_cachedQueries.size()) + " queries.", turretLogger::LOG_LEVELS::CACHE_FILE_IO);

        return true;
    }

    void turretClient::appendCache()
    {
        loadCache(); //This may introduce cache collisions, test how std::map handles
        saveCache();
    }

    // -- Protected

    // Resolving functions
    
    std::string turretClient::resolve_name(const std::string& a_path)
    {
        const std::string parsed_path = this->parse_query(a_path);
        return parsed_path;
    }

    bool turretClient::resolve_exists(const std::string& a_path)
    {
        const std::string parsed_path = turretClient::resolve_name(a_path);
        if(parsed_path == "NOT_FOUND")
            return false;
        else 
            return true;
    }

    // Returns if path matches tank query schema
    bool turretClient::matches_schema(const std::string& a_path)
    {
        return a_path.find(TANK_PREFIX_SHORT) == 0;
    }

    // -- Protected

    // Parseing functions

    std::string turretClient::parse_query(const std::string& a_query)
    {
        for(int i = 0; i < turret_client::ZMQ_RETRIES; i++)
        {
            if(i > 1) {
                turretLogger::Instance()->Log(m_clientID + " resolver parser had to retry: " + std::to_string(i),
                                           turretLogger::LOG_LEVELS::DEFAULT);
            }

            if(m_useCache) {
                turretLogger::Instance()->Log(m_clientID + " resolver searching cache for query: " + a_query, turretLogger::LOG_LEVELS::CACHE_QUERIES);

                // Search for cached result
                const std::map<std::string, turret_client::turretQueryCache>::iterator cached_result = m_cachedQueries.find(a_query);

                if (cached_result != m_cachedQueries.end()) {
                    //std::cout << "Current time: " << std::time(0) << "\n";
                    //std::cout << "Cache time: " << cached_result->second.timestamp << "\n";
                    // If cache is still fresh
                    //if(std::time(0) - cached_result->second.timestamp <= turret_client::ZMQ_CACHE_TIMEOUT)
                    
                    turretLogger::Instance()->Log(m_clientID + " resolver received Cached response: " + cached_result->second.resolved_path, turretLogger::LOG_LEVELS::CACHE_QUERIES);
                    return cached_result->second.resolved_path;
                }
            }

            // Check socket status
            zmq::context_t m_context(1);
            zmq::socket_t m_socket(m_context, ZMQ_REQ);
            m_socket.connect("tcp://" + turret_client::ZMQ_SERVER + ":" + turret_client::ZMQ_PORT);
            m_socket.setsockopt(ZMQ_LINGER, turret_client::ZMQ_TIMEOUT);
            m_socket.setsockopt(ZMQ_RCVTIMEO, turret_client::ZMQ_TIMEOUT);

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

                turretLogger::Instance()->Log(m_clientID + " resolver ZMQ ERROR: " + std::to_string(errnum) + " : " + std::string(errmsg), turretLogger::LOG_LEVELS::ZMQ_ERROR);
                continue;
            }
        
            // Store the reply
            std::string realPath = std::string((char *)reply.data());

            if(realPath != "NOT_FOUND") {
                if (realPath[0] != '/') {
                    continue;
                }
            }

            turretLogger::Instance()->Log(m_clientID + " resolver received query response: " + a_query + " | " + realPath, turretLogger::LOG_LEVELS::ZMQ_QUERIES);

            if(m_useCache) {
                // Cache reply
                turretQueryCache cache = {realPath, std::time(0)};
                m_cachedQueries.insert(std::make_pair(a_query, cache));
            }

            m_socket.close();
            m_context.close();
            return realPath;
        }

        turretLogger::Instance()->Log(m_clientID + " resolver unable to query after "
                                   + std::to_string(turret_client::ZMQ_RETRIES)
                                   + " retries.", turretLogger::LOG_LEVELS::ZMQ_ERROR);

        return "Unable to parse query";
    }
}
