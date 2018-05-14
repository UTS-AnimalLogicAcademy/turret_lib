#pragma once

#include <string>
#include <map>
#include <ctime>

#include <boost/serialization/serialization.hpp>

namespace zmq_client
{

    const std::string TANK_PREFIX = "tank://";
    const std::string TANK_PREFIX_SHORT = "tank:";

    const std::string ZMQ_SERVER = "localhost";
    const std::string ZMQ_PORT = "5555";


    const int ZMQ_TIMEOUT = 60000;
    const int ZMQ_RETRIES = 50;

    const bool ZMQ_CACHE_QUERIES = false;
    const double ZMQ_CACHE_TIMEOUT = 100.0;
    const std::string ZMQ_CACHE_LOCATION = "/usr/tmp/zmq/";
    const std::string ZMQ_CACHE_FILETYPE = ".zmqcache";

    struct zmqQueryCache {
        std::string resolved_path;
        std::time_t timestamp;

        private:
            friend class boost::serialization::access;
            template<class Archive>
            void serialize(Archive &ar, const unsigned int version)
            {
                ar & resolved_path;
                ar & timestamp;
            }
    };

    class zmqClient {
        public:
            zmqClient();
            zmqClient(const char* a_clientID);
            ~zmqClient();

            std::string resolve_name(const std::string& a_path);
            bool resolve_exists(const std::string& a_path);
            bool matches_schema(const std::string& a_path);
        
            void SetUseCache(const bool& a_useCache) { m_useCache = a_useCache; }
            bool GetUseCache() { return m_useCache; }

            void SetClientID(const char* a_clientID) { m_clientID = std::string(a_clientID); }
            const char* GetClientID() { return m_clientID.c_str(); }

            void ClearCache();

        protected:
            void setup();
            void destroy();

        protected:
            std::string parse_query(const std::string& a_query);

            void saveCache();
            bool loadCache();
            void appendCache();

        protected:
            bool m_useCache;

            std::string m_clientID;
            std::string m_cacheFilePath;

            //<Tank_Query, Cache_Result>
            std::map<std::string, zmqQueryCache> m_cachedQueries;
    };
}