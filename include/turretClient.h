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

#pragma once

#include <string>
#include <map>
#include <ctime>

#include <boost/serialization/serialization.hpp>

#include "tbb/concurrent_hash_map.h"

namespace turret_client
{
    const std::string TANK_PREFIX = "tank://";
    const std::string TANK_PREFIX_SHORT = "tank:";

    const std::string DEFAULT_ZMQ_SERVER = "localhost";
    const std::string DEFAULT_ZMQ_PORT = "5555";

    const int DEFAULT_ZMQ_TIMEOUT = 60000;
    const int DEFAULT_ZMQ_RETRIES = 50;

    const std::string TURRET_CACHE_DIR = "/usr/tmp/turret/";
    const std::string TURRET_CACHE_EXT = ".turretcache";

    struct turretQueryCache {
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

    class turretClient {
        public:
            turretClient();
            turretClient(const char* a_clientID);
            ~turretClient();
            std::string resolve_name(const std::string& a_path);
            bool resolve_exists(const std::string& a_path);
            bool matches_schema(const std::string& a_path);
            void SetClientID(const char* a_clientID) { m_clientID = std::string(a_clientID); }
            const char* GetClientID() { return m_clientID.c_str(); }

        protected:
            void setup();
            void destroy();
            std::string parse_query(const std::string& a_query);
            void saveCache();
            bool loadCache();
            void appendCache();
            std::string m_clientID; // set by constructor
            std::string m_serverIP;
            std::string m_serverPort;
            int m_timeout;
            int m_retries;
            bool m_doLog;
            bool m_cacheToDisk; // set by env var $TURRET_CLIENTID_CACHE_TO_DISK=1, default is false
            bool m_resolveFromFileCache; // set by env var $TURRET_CLIENTID_CACHE_LOCATION=/path/to/cache
            bool m_allowLiveResolves; // set by env var $TURRET_CLIENTID_ALLOW_LIVE_RESOLVES
            std::string m_sessionID; // set by $TURRET_SESSION_ID=uuid
            std::string m_cacheFilePath;
            std::string m_cacheDir;
            tbb::concurrent_hash_map<std::string, turretQueryCache> m_cachedQueries;
    };
}
