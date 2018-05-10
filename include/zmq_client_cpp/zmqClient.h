#pragma once

#include <string>
#include <map>

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

    struct zmqQueryCache {
        std::string resolved_path;
        std::time_t timestamp;
    };

    class zmqClient {
        public:
            zmqClient();
            ~zmqClient();

            std::string resolve_name(const std::string& a_path);
            bool resolve_exists(const std::string& a_path);
            bool matches_schema(const std::string& a_path);
        
            void SetUseCache(const bool& a_useCache) { m_useCache = a_useCache; }

        protected:
            std::string parse_query(const std::string& a_query);

        protected:
            bool m_useCache;

            //<Tank_Query, Cache_Result>
            std::map<std::string, zmqQueryCache> m_cachedQueries;
    };
}