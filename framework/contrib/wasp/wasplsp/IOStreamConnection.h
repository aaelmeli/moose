#ifndef WASPLSP_IOSTREAM_CONNECTION_H
#define WASPLSP_IOSTREAM_CONNECTION_H

#include <string>
#include <iostream>
#include <sstream>
#include <memory>
#include "wasplsp/LSP.h"
#include "wasplsp/Connection.h"
#include "wasplsp/ServerImpl.h"
#include "waspcore/decl.h"

namespace wasp {

class DataObject;

namespace lsp  {

class WASP_PUBLIC IOStreamConnection : public Connection
{
  public:

    IOStreamConnection(ServerImpl * server){ this->server = server; }

    ~IOStreamConnection(){}

    typedef std::shared_ptr<Connection> SP;

    /** read packet string from standard input and store in provided object
     * @param object - reference to object to fill with data from read packet
     * @param errors - reference to error stream to pass to RPCStringToObject
     * @return - true if the read and conversion to object were successful
     */
    bool read( DataObject & object , std::stringstream & errors )
    {
        bool pass = true;

        // read and check "Content-Length:" word from standard input

        std::string content_length_key;

        std::cin >> content_length_key;

        wasp_check( content_length_key == m_rpc_content_len_key );

        // read and check content length integer value from standard input

        int content_length_val;

        std::cin >> content_length_val;

        wasp_check( content_length_val > 0 );

        // gobble up all space until the next non-whitespace from standard input

        std::cin >> std::ws;

        // create a content string of size content_length_val

        std::string content( content_length_val , ' ' );

        // read exactly content_length_val bytes from standard input - blocking

        std::cin.read( &content[0] , content_length_val );

        // compose rpc_stream from from Content-Length: Length + \r\n + content

        std::stringstream rpc_stream;

        rpc_stream << content_length_key
                   << " "
                   << content_length_val
                   << m_rpc_separator
                   << content;

        // convert the composed full rpc_stream to the provided object reference

        pass &= RPCStringToObject( rpc_stream.str() , object , errors );

        return true;
    }

    /** write data from provided object to standard output as packet string
     * @param object - object holding data to write to standard output
     * @param errors - reference to error stream to pass to objectToRPCString
     * @return - true if the conversion to the packet and write were successful
     */
    bool write( DataObject & object , std::stringstream & errors )
    {
        bool pass = true;

        // convert the provided object into the local rpc string

        std::string rpc_string;

        pass &= objectToRPCString( object , rpc_string , errors );

        // write the local rpc string to standard output

        std::cout << rpc_string;

        // flush the standard output buffer

        std::cout.flush();

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
};

} // namespace lsp
} // namespace wasp

#endif // WASPLSP_IOSTREAM_CONNECTION_H
