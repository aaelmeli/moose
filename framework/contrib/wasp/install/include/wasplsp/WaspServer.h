#ifndef WASPLSP_WASP_SERVER_H
#define WASPLSP_WASP_SERVER_H

#include <string>
#include <memory>
#include <sstream>
#include <fstream>
#include <cstdio>
#include "wasplsp/LSP.h"
#include "wasplsp/ServerImpl.h"
#include "wasplsp/Connection.h"
#include "wasplsp/ThreadConnection.h"
#include "wasphive/InputDefinition.h"
#include "wasphalite/HaliteInterpreter.h"
#include "waspcore/Object.h"
#include "waspcore/Interpreter.h"
#include "waspcore/decl.h"

namespace wasp {
namespace lsp  {

template < class INPUT                         ,
           class INPUTNV                       ,
           class SCHEMA                        ,
           class SCHEMANV                      ,
           class VALIDATOR                     ,
           class CONNECTION = ThreadConnection >

class WASP_PUBLIC WaspServer : public ServerImpl
{
  public:

    WaspServer() : is_setup(false)
    {
        // set server connection to a shared pointer to new templated connection

        connection = std::make_shared<CONNECTION>(this);

        // set server capabilities to expect full text document when changed

        this->server_capabilities[m_text_doc_sync] = DataObject();

        this->server_capabilities[m_text_doc_sync][m_open_close] = true;

        this->server_capabilities[m_text_doc_sync][m_change] = m_change_full;
    }

    /** get read / write connection - specific to this server implemention
     * @return - shared pointer to the server's read / write connection
     */
    std::shared_ptr<Connection> getConnection()
    {
        return connection;
    }

    /** setup server for input validation and template based autocompletion
     * @param validator - validator to be used in parseDocumentForDiagnostics
     * @param schema - schema parse tree to be used by the provided validator
     * @param template_dir - template directory to be used in autocompletion
     * @return - true if server setup was successful and everything was set
     */
    bool setup( std::shared_ptr<VALIDATOR> & validator    ,
                std::shared_ptr<SCHEMA>    & schema       ,
                const std::string          & template_dir );

    /**
     * @brief struct of autocompletion fields used when building completion list
     */
    struct AutoComplete
    {
        std::string label;
        std::string new_text;
        std::string description;
        std::string complete_type;
        AutoComplete(){}
    };

  private:

    /** parse document for diagnostics - specific to this server implemention
     * @param diagnosticsList - data array of diagnostics data objects to fill
     * @return - true if completed successfully - does not indicate parse fail
     */
    bool parseDocumentForDiagnostics(
                          DataArray  & diagnosticsList );

    /** update document text changes - specific to this server implemention
     * @param replacement_text - text to be replaced over the provided range
     * @param start_line - starting replace line number ( zero-based )
     * @param start_character - starting replace column number ( zero-based )
     * @param end_line - ending replace line number ( zero-based )
     * @param end_character - ending replace column number ( zero-based )
     * @param range_length - length of replace range - server specific
     * @return - true if the document text was updated successfully
     */
    bool updateDocumentTextChanges(
                    const std::string & replacement_text ,
                          int           start_line       ,
                          int           start_character  ,
                          int           end_line         ,
                          int           end_character    ,
                          int           range_length     );

    /** gather document completion items - specific to this server implemention
     * @param completionItems - data array of completion item objects to fill
     * @param is_incomplete - flag indicating if the completions are complete
     * @param line - line to be used for completions gathering logic
     * @param character - column to be used for completions gathering logic
     * @return - true if the gathering of items completed successfully
     */
    bool gatherDocumentCompletionItems(
                          DataArray & completionItems  ,
                          bool      & is_incomplete    ,
                          int         line             ,
                          int         character        );

    /** gather definition locations - specific to this server implemention
     * @param definitionLocations - data array of locations objects to fill
     * @param line - line to be used for locations gathering logic
     * @param character - column to be used for locations gathering logic
     * @return - true if the gathering of locations completed successfully
     */
    bool gatherDocumentDefinitionLocations(
                          DataArray & definitionLocations ,
                          int         line                ,
                          int         character           );

    /** gather references locations - specific to this server implemention
     * @param referencesLocations - data array of locations objects to fill
     * @param line - line to be used for locations gathering logic
     * @param character - column to be used for locations gathering logic
     * @param include_declaration - flag indicating declaration inclusion
     * @return - true if the gathering of locations completed successfully
     */
    bool gatherDocumentReferencesLocations(
                          DataArray & referencesLocations ,
                          int         line                ,
                          int         character           ,
                          bool        include_declaration );

    /** gather formatting text edits - specific to this server implemention
     * @param formattingTextEdits - data array of text edit objects to fill
     * @param start_line - starting line to be used for formatting logic
     * @param start_character - starting column to be used for formatting logic
     * @param end_line - ending line to be used for formatting logic
     * @param end_character - ending column to be used for formatting logic
     * @param tab_size - value of the size of a tab in spaces for formatting
     * @param insert_spaces - flag indicating whether to use spaces for tabs
     * @return - true if the gathering of text edits completed successfully
     */
    bool gatherDocumentFormattingTextEdits(
                          DataArray & formattingTextEdits ,
                          int         start_line          ,
                          int         start_character     ,
                          int         end_line            ,
                          int         end_character       ,
                          int         tab_size            ,
                          bool        insert_spaces       );

    /** gather document symbols - specific to this server implemention
     * @param documentSymbols - data array of symbols data objects to fill
     * @return - true if the gathering of symbols completed successfully
     */
    bool gatherDocumentSymbols(
                          DataArray & documentSymbols );

    /** recursively fill document symbols from the given node
     * @param node - root node for further tree traversal recursion
     * @param parent - data object with data array of symbol children objects
     * @return - true if no problems with this level of the resursion
     */
    bool traverseParseTreeAndFillSymbols(
                          INPUTNV      node   ,
                          DataObject & parent );

    /** is given node allowed to be added according to the input definition
     * @param node - the given node in question of being allowed to be added
     * @param def - the input definition that decides if the node is allowed
     * @return - true if the node may be added without being invalid
     */
    bool isAllowed( INPUTNV * node , IDObject * def );

    /** use the given input definition to expand any templates for completion
     * @param def - input definition used to find any templates to expand
     * @param options - list of completion options to be filled
     * @return - true if no problems adding any possible template expansion
     */
    bool addTemplate( IDObject  * def                     ,
                      std::vector<AutoComplete> & options );

    /** read from connection into object - specific to this server's connection
     * @param object - reference to object to be read into
     * @return - true if the read from the connection completed successfully
     */
    bool connectionRead( DataObject & object )
    {
       return this->connection->read( object , this->errors );
    }

    /** write object json to connection - specific to this server's connection
     * @param object - reference to object with contents to write to connection
     * @return - true if the write to the connection completed successfully
     */
    bool connectionWrite( DataObject & object )
    {
       return this->connection->write( object , this->errors );
    }

    /**
     * @brief is_setup - has the required server specific setup taken place
     */
    bool is_setup;

    /**
     * @brief parser - parser used to parse input and hold root for other use
     */
    std::shared_ptr<INPUT> parser;

    /**
     * @brief validator - validator to be used in parseDocumentForDiagnostics
     */
    std::shared_ptr<VALIDATOR> validator;

    /**
     * @param schema - schema parse tree to be used by the provided validator
     */
    std::shared_ptr<SCHEMA> schema;

    /**
     * @param template_dir - template directory to be used in autocompletion
     */
    std::string template_dir;

    /**
     * @param inputdefinition - input definition rules used for autocompletion
     */
    std::shared_ptr<InputDefinition> inputdefinition;

    /**
     * @brief shared pointer to this server's templated read / write connection
     */
    std::shared_ptr<CONNECTION> connection;
};

} // namespace lsp
} // namespace wasp

#include "wasplsp/WaspServer.i.h"

#endif // WASPLSP_WASP_SERVER_H
