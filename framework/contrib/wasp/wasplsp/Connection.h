#ifndef WASPLSP_CONNECTION_H
#define WASPLSP_CONNECTION_H

#include <string>
#include <sstream>
#include <memory>
#include "waspcore/decl.h"

namespace wasp {

class DataObject;

namespace lsp  {

class WASP_PUBLIC Connection
{
  public:

    Connection(){}

    virtual ~Connection() = default;

    typedef std::shared_ptr<Connection> SP;

    /** read packet string from specific connection and store in provided object
     * @param object - reference to object to fill with data from read packet
     * @param errors - reference to error stream to pass to RPCStringToObject
     * @return - true if the read and conversion to object were successful
     */
    virtual bool read ( DataObject & object , std::stringstream & errors ) = 0;

    /** write data from provided object to specific connection as packet string
     * @param object - object holding data to write to specific connection
     * @param errors - reference to error stream to pass to objectToRPCString
     * @return - true if the conversion to the packet and write were successful
     */
    virtual bool write( DataObject & object , std::stringstream & errors ) = 0;

    /** check if the server is initialized and reading from the connection
     * @return - true if server is initialized and reading from the connection
     */
    virtual bool isServerRunning() const = 0;

    /** get all errors that have been stored on the server for any reason
     * @return - error string of all errors that have been stored on the server
     */
    virtual std::string getServerErrors() const = 0;
};

} // namespace lsp
} // namespace wasp

#endif // WASPLSP_CONNECTION_H
