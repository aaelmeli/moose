#ifndef WASPLSP_THREAD_CONNECTION_H
#define WASPLSP_THREAD_CONNECTION_H

#include <string>
#include <sstream>
#include <mutex>
#include <condition_variable>
#include <memory>
#include "wasplsp/LSP.h"
#include "wasplsp/Connection.h"
#include "wasplsp/ServerImpl.h"
#include "waspcore/decl.h"

namespace wasp {

class DataObject;

namespace lsp  {

class WASP_PUBLIC ThreadConnection : public Connection
{
  public:

    ThreadConnection(ServerImpl * server){ this->server = server; }

    ~ThreadConnection(){}

    typedef std::shared_ptr<Connection> SP;

    /** read connection packet string and store in provided object
     * @param object - reference to object to fill with data from packet string
     * @param errors - reference to error stream to pass to RPCStringToObject
     * @return - true if conversion from packet string to object was successful
     */
    bool read( DataObject & object , std::stringstream & errors )
    {
        bool pass = true;

        // unique lock using connection's mutex_lock for ready_to_read

        std::unique_lock<std::mutex> unique_lock( mutex_lock );

        // if packet empty - wait for ready_to_read to be triggered by write()

        if( this->packet.empty() )
        {
            ready_to_read.wait( unique_lock );
        }

        // copy the connection's packet string to local rpc_string

        std::string rpc_string = this->packet;

        // empty out the connection's packet string

        this->packet.clear();

        // trigger ready_to_write to let write() know to move on

        ready_to_write.notify_all();

        // convert the local rpc string to the provided object reference

        pass &= RPCStringToObject( rpc_string , object , errors );

        return true;
    }

    /** write data from provided object to connection packet string
     * @param object - object holding data to write to connection packet string
     * @param errors - reference to error stream to pass to objectToRPCString
     * @return - true if conversion from object to packet string was successful
     */
    bool write( DataObject & object , std::stringstream & errors )
    {
        bool pass = true;

        // unique lock using connection's mutex_lock for ready_to_write

        std::unique_lock<std::mutex> unique_lock( mutex_lock );

        // convert the provided object into the local rpc string

        std::string rpc_string;

        pass &= objectToRPCString( object , rpc_string , errors );

        // copy the local rpc string to the connection's packet string

        this->packet = rpc_string;

        // trigger ready_to_read to let read() know that packet may be read

        ready_to_read.notify_all();

        // wait for ready_to_write to be triggered by read() to move on

        ready_to_write.wait( unique_lock );

        return true;
    }

    /** check if the server is initialized and reading from the connection
     * @return - true if server is initialized and reading from the connection
     */
    bool isServerRunning() const
    {
        return ( server != nullptr && server->isRunning() );
    }

    /** get all errors that have been stored on the server for any reason
     * @return - error string of all errors that have been stored on the server
     */
    std::string getServerErrors() const
    {
        return server->getErrors();
    }

  private:

    /**
     * @brief server - server used for this connection's reading and writing
     */
    ServerImpl * server;

    /**
     * @brief mutex_lock - mutex used for unique locks for conditional waits
     */
    std::mutex mutex_lock;

    /**
     * @brief ready_to_read - used to know when it is okay to read from packet
     */
    std::condition_variable ready_to_read;

    /**
     * @brief ready_to_read - used to know when it is okay to write to packet
     */
    std::condition_variable ready_to_write;

    /**
     * @brief packet - packet string used for all locked reading and writing
     */
    std::string  packet;
};

} // namespace lsp
} // namespace wasp

#endif // WASPLSP_THREAD_CONNECTION_H
