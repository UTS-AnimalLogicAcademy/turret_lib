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

#include "turretClient.h"

#include <cstdlib>
#include <ctime>
#include <zmq.hpp>

#ifndef TURRETLOGGER_H_
#define TURRETLOGGER_H_
#include "turretLogger.h"
#endif

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>

#include <sys/stat.h>

#include <boost/serialization/map.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/filesystem.hpp>

/* Environment Variables:
 *
 * TURRET_SESSION_ID -
 * Can be set by DCC app on scene load/new scene, to uniquify logs per session.
 * Multiple turret clients for a DCC session will share the same session id.
 *
 * TURRET_${CLIENTID}_CACHE_LOCATION -
 * eg: TURRET_USD_CACHE_LOATION, TURRET_KLF_CACHE_LOCATION
 * Can be used to specify a previously created cache file on disk.  If a valid cache
 * file is specified, no live resolves will be performed - any queried keys must be in
 * the cache.
 * This can be set per turret client (eg: usd, klf).
 *
 * TURRET_${CLIENTID}_CACHE_TO_DISK -
 * If set to 1, the client will write out the resolved asset cache to disk when the
 * destructor is called.
 * This can be set per turret client (eg: usd, klf).
 *
 */

namespace turret_client
{
    // -- Public

    turretClient::turretClient() :
        m_cacheToDisk(false),
        m_clientID("default"),
        m_serverIP(turret_client::DEFAULT_ZMQ_SERVER),
        m_serverPort(turret_client::DEFAULT_ZMQ_PORT),
        m_timeout(turret_client::DEFAULT_ZMQ_TIMEOUT),
        m_retries(turret_client::DEFAULT_ZMQ_RETRIES),
        m_resolveFromFileCache(false),
        m_allowLiveResolves(true),
        m_cacheFilePath("") {
        setup();
    }

    turretClient::turretClient(const char* a_clientID) : 
        m_cacheToDisk(false),
        m_clientID(a_clientID),
        m_serverIP(turret_client::DEFAULT_ZMQ_SERVER),
        m_serverPort(turret_client::DEFAULT_ZMQ_PORT),
        m_timeout(turret_client::DEFAULT_ZMQ_TIMEOUT),
        m_retries(turret_client::DEFAULT_ZMQ_RETRIES),
        m_resolveFromFileCache(false),
        m_cacheFilePath("") {
        setup();
    }

    turretClient::~turretClient() {
        destroy();
        turretLogger::Instance()->Log("Destroyed " + m_clientID + " client.", turretLogger::LOG_LEVELS::ZMQ_QUERIES);
    }

    std::string turretClient::resolve_name(const std::string& a_path) {
        const std::string parsed_path = this->parse_query(a_path);
        return parsed_path;
    }

    bool turretClient::resolve_exists(const std::string& a_path) {
        const std::string parsed_path = turretClient::resolve_name(a_path);
        if(parsed_path == "NOT_FOUND")
            return false;
        else
            return true;
    }

    bool turretClient::matches_schema(const std::string& a_path) {
        return a_path.find(TANK_PREFIX_SHORT) == 0;
    }
    // -- End Public

    // -- Protected
    void turretClient::setup() {
        std::string clientIDUppercase = m_clientID;
        std::transform(clientIDUppercase.begin(), clientIDUppercase.end(),clientIDUppercase.begin(), ::toupper);

        // The session_id is set by the DCC app on scene load/new scene
        if (const char* sessionID = std::getenv("TURRET_SESSION_ID")) {
            m_sessionID = sessionID;
        }

	    // Initialize server settings
	
        if (const char* serverIP = std::getenv("TURRET_SERVER_IP")) { 
            m_serverIP = serverIP; 
            turretLogger::Instance()->Log("Turret " + m_clientID + "will use value from 'TURRET_SERVER_IP' environment variable for server IP: " + std::string(m_serverIP), turretLogger::LOG_LEVELS::DEFAULT);
        }

        if (const char* serverPort = std::getenv("TURRET_SERVER_PORT")) { 
            m_serverPort = serverPort; 
            turretLogger::Instance()->Log("Turret " + m_clientID + " will use value from 'TURRET_SERVER_PORT' environment variable for server Port: " + std::string(m_serverPort), turretLogger::LOG_LEVELS::DEFAULT);
        }

        if (const char* timeout = std::getenv("TURRET_TIMEOUT")) { 
            m_timeout = std::stoi(timeout); 
            turretLogger::Instance()->Log("Turret " + m_clientID + " will use value from 'TURRET_TIMEOUT' environment variable for timeout: " + std::to_string(m_timeout), turretLogger::LOG_LEVELS::DEFAULT);
        }

        if (const char* retries = std::getenv("TURRET_RETRIES")) { 
            m_retries = std::stoi(retries);
            turretLogger::Instance()->Log("Turret " + m_clientID + " will use value from 'TURRET_RETRIES' environment variable for retries: " + std::to_string(m_retries), turretLogger::LOG_LEVELS::DEFAULT);
        }

        // use env var value to determine whether live resolves are allowed
        if (const char* allowLiveResolves = std::getenv(("TURRET_" + clientIDUppercase + "_ALLOW_LIVE_RESOLVES").c_str())) {
            m_allowLiveResolves = std::stoi(allowLiveResolves);

            if (m_allowLiveResolves){
                turretLogger::Instance()->Log("Turret " + m_clientID + " will allow live resolves");
            }
            else{
                turretLogger::Instance()->Log("Turret " + m_clientID + " will disable live resolves");
            }

        }
        // default - live resolves are allowed
        else{
            turretLogger::Instance()->Log("$TURRET_" + clientIDUppercase + "_ALLOW_LIVE_RESOLVES not set, turret " + m_clientID + " will default to allowing live resolves");
            m_allowLiveResolves = true;
        }

        // Check if a disk cache location is provided by env var.  If it is, the client
        // will load previously resolved values from it:
        if(const char* cache_location = std::getenv(("TURRET_" + clientIDUppercase + "_CACHE_LOCATION").c_str())) {

            turretLogger::Instance()->Log("turret " + m_clientID + " was given a cache location");

            // check if file exists
            if (boost::filesystem::exists(cache_location)){

                turretLogger::Instance()->Log("Turret " + m_clientID + " will load resolved assets from cache file: "
                                              + std::string(cache_location),
                                              turretLogger::LOG_LEVELS::ZMQ_QUERIES);

                m_resolveFromFileCache = true;
                m_cacheFilePath = cache_location;
                loadCache();
            }

            else{
                turretLogger::Instance()->Log("turret " + m_clientID + " was given a cache location, but the file does not exist" + std::string(cache_location));
            }


        }

        // Cache live resolves to disk - controlled by environment variable so DCC apps can opt in or out
        if(const char* write_disk_cache = std::getenv(("TURRET_" + clientIDUppercase + "_CACHE_TO_DISK").c_str()))
        {

            turretLogger::Instance()->Log("Turret " + m_clientID + " session id: " + m_sessionID, turretLogger::LOG_LEVELS::ZMQ_INTERNAL);
            
            m_cacheToDisk = (write_disk_cache[0] == '1');

            if (m_cacheToDisk && m_sessionID.empty()){
                turretLogger::Instance()->Log("Turret " + m_clientID +
                                                      " m_cacheToDisk is True, but m_sessionID is not set, throwing exception in constructor.",
                                              turretLogger::LOG_LEVELS::ZMQ_INTERNAL);
                throw;
            }

            m_cacheFilePath = TURRET_CACHE_DIR + m_clientID + "_" + m_sessionID + TURRET_CACHE_EXT;

            turretLogger::Instance()->Log("Turret " + m_clientID + " will cache resolves to disk",
                                          turretLogger::LOG_LEVELS::ZMQ_INTERNAL);

            // Check that the location on disk exists. Create if it doesn't
            if(!(boost::filesystem::exists(TURRET_CACHE_DIR))) {
                if (boost::filesystem::create_directory(TURRET_CACHE_DIR)) {
                    boost::filesystem::permissions(TURRET_CACHE_DIR, boost::filesystem::perms::all_all);
                    turretLogger::Instance()->Log(m_clientID + " resolver created" + m_clientID + "cache directory: "
                                                  + TURRET_CACHE_DIR);
                }
            }

        }

        else{
            turretLogger::Instance()->Log("Turret " + m_clientID + " will not cache resolves to disk",
                                          turretLogger::LOG_LEVELS::ZMQ_QUERIES);
        }

    }

    void turretClient::destroy()
    {
        if((m_cacheToDisk) && (!m_cachedQueries.empty())) {
            saveCache();
        }
    }

    void turretClient::saveCache() {

        try {
            std::fstream fs(m_cacheFilePath.c_str(), std::fstream::out | std::ios::binary);
            boost::archive::text_oarchive oarch(fs);
            oarch << m_cachedQueries;
            fs.close();
            turretLogger::Instance()->Log(m_clientID + " resolver saved cache to " + m_cacheFilePath,
                                          turretLogger::LOG_LEVELS::CACHE_FILE_IO);
        }
        catch(boost::archive::archive_exception) {
            turretLogger::Instance()->Log(m_clientID + " resolver could not save cache to " + m_cacheFilePath,
                                          turretLogger::LOG_LEVELS::CACHE_FILE_IO);
        }


    }

    bool turretClient::loadCache() {
        std::fstream fs(m_cacheFilePath.c_str(), std::fstream::in | std::ios::binary);

        if(!fs.is_open()) {
            turretLogger::Instance()->Log(m_clientID + " resolver no cache file present on disk.",
                                          turretLogger::LOG_LEVELS::CACHE_FILE_IO);
            return false;
        }

        boost::archive::text_iarchive iarch(fs);
        iarch >> m_cachedQueries;

        fs.close();

        return true;
    }

    std::string turretClient::parse_query(const std::string& a_query) {

        std::string query = a_query;

        const char *env = std::getenv("TURRET_PLATFORM_ID");
        if (env) {
            std::string platform = std::string(env);
            if(!platform.empty()) {
                query += "&platform=" + platform;
            }
        }

        for(int i = 0; i < m_retries; i++) {
            if(i > 1) {
                turretLogger::Instance()->Log(m_clientID + " resolver parser had to retry: " + std::to_string(i),
                                           turretLogger::LOG_LEVELS::DEFAULT);
            }

            // Search for cached result
            const std::map<std::string, turret_client::turretQueryCache>::iterator cached_result = m_cachedQueries.find(query);

            // result was found in cache
            if (cached_result != m_cachedQueries.end()) {
                turretLogger::Instance()->Log(m_clientID + " resolver received cached response: "
                                              + cached_result->second.resolved_path + " for query: " + query
                                              + "\n", turretLogger::LOG_LEVELS::ZMQ_QUERIES);

                return cached_result->second.resolved_path;
            }

            // Halt if live resolves are disabled
            if (m_allowLiveResolves == false){
                return "uncached_query";
            }


            // Perform live resolve

            // Check socket status
            zmq::context_t m_context(1);
            zmq::socket_t m_socket(m_context, ZMQ_REQ);
            m_socket.connect("tcp://" + m_serverIP + ":" + m_serverPort);
            m_socket.setsockopt(ZMQ_LINGER, m_timeout);
            m_socket.setsockopt(ZMQ_RCVTIMEO, m_timeout);

            // Create zmq request
            zmq::message_t request(query.c_str(), query.length());
            
            // Send zmq request
            m_socket.send(request/* , ZMQ_NOBLOCK */);

            // Wait for the reply
            zmq::message_t reply;
            int result = m_socket.recv(&reply);

            if(result < 1) {
                int errnum = zmq_errno();
                //There has been an error
                const char* errmsg = zmq_strerror(errnum);

                turretLogger::Instance()->Log(m_clientID + " resolver ZMQ ERROR: " + std::to_string(errnum)
                                              + " : " + std::string(errmsg), turretLogger::LOG_LEVELS::ZMQ_ERROR);
                continue;
            }
        
            // Store the reply
            std::string realPath = std::string((char *)reply.data());

            if(realPath == "NOT_FOUND") continue;

            // Cache the reply
            turretQueryCache cache = {realPath, std::time(0)};
            // insert will not add duplicate keys
            m_cachedQueries.insert(std::make_pair(query, cache));

            turretLogger::Instance()->Log(m_clientID + " resolver received live response: "
                                          + realPath + " for query: " + query + "\n",
                                          turretLogger::LOG_LEVELS::ZMQ_QUERIES);

            m_socket.close();
            m_context.close();
            return realPath;
        }

        turretLogger::Instance()->Log(m_clientID + " resolver unable to query after "
                                   + std::to_string(m_retries)
                                   + " retries.", turretLogger::LOG_LEVELS::ZMQ_ERROR);

        return "Unable to parse query";
    }
}
