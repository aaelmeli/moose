#include <functional>
#include "waspcore/utils.h"

namespace wasp {
namespace lsp  {

template < class INPUT      ,
           class INPUTNV    ,
           class SCHEMA     ,
           class SCHEMANV   ,
           class VALIDATOR  ,
           class CONNECTION >
bool WaspServer<INPUT,INPUTNV,SCHEMA,SCHEMANV,VALIDATOR,CONNECTION>::
   setup( std::shared_ptr<VALIDATOR> & validator    ,
          std::shared_ptr<SCHEMA>    & schema       ,
          const std::string          & template_dir )
{
    bool pass = true;

    // set the server's validator shared_ptr to the provided validator

    this->validator = validator;

    // set the server's schema shared_ptr to the provided schema

    this->schema = schema;

    // get the root of the schema parse tree to make the input definition

    SCHEMANV schema_root = this->schema->root();

    // make the input definition using the schema and store on the server

    this->inputdefinition = std::make_shared<InputDefinition>( schema_root  ,
                                                               this->errors ,
                                                               this->errors );

    // set the server's template_dir string to the provided template directory

    this->template_dir = template_dir;

    // the required server setup is now finished

    this->is_setup = true;

    return pass;
}

template < class INPUT      ,
           class INPUTNV    ,
           class SCHEMA     ,
           class SCHEMANV   ,
           class VALIDATOR  ,
           class CONNECTION >
bool WaspServer<INPUT,INPUTNV,SCHEMA,SCHEMANV,VALIDATOR,CONNECTION>::
    parseDocumentForDiagnostics(
                      DataArray & diagnosticsList )
{
    if (!this->is_setup)
    {
        this->errors << m_error_prefix << "Server needs to be setup" << std::endl;
        return false;
    }

    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    if (!this->is_document_open)
    {
        this->errors << m_error_prefix << "Server has no open document" << std::endl;
        return false;
    }

    bool pass = true;

    // make a new parser capturing errors to a stream and set the server parser

    std::stringstream parse_errors;

    this->parser = std::make_shared<INPUT>(parse_errors);

    // use the parser to parse the entire currently set document text

    if ( !this->parser->parseString( this->document_text , "" ) )
    {
        // walk over parse_errors stream if there are any parse errors

        while( parse_errors.good() )
        {
            std::string error;

            // get each parse error line

            std::getline( parse_errors, error );

            if ( error.empty() ) continue;

            size_t dot   = error.find('.' , 1);

            size_t colon = error.find(':' , 1);

            // parse line number out of the parse error and convert to integer

            std::string line_str = error.substr( 1, dot - 1 );

            int line;

            std::stringstream( line_str ) >> line;

            // parse column number out of the parse error and convert to integer

            std::string column_str = error.substr( dot + 1, colon - dot - 1 );

            int column;

            std::stringstream( column_str ) >> column;

            // parse the actual message content out of the parse error

            std::string message  = error.substr( colon + 1 );

            message = trim( message, " " );

            wasp_check( line > 0 );

            wasp_check( column > 0 );

            wasp_check( !message.empty() );

            // according to the protocol - line and column number are zero based

            line--;
            column--;

            // build a diagnostics object out of the line, column, and message

            DataObject diagnostic;

            pass &= buildDiagnosticObject( diagnostic   ,
                                           this->errors ,
                                           line         ,
                                           column       ,
                                           line         ,
                                           column       ,
                                           1            ,
                                           "parse"      ,
                                           "parser"     ,
                                           message      );

            // push the created diagnostic object onto the diagnostics list

            diagnosticsList.push_back(diagnostic);
        }
    }

    SCHEMANV schema_root = this->schema->root();

    INPUTNV  input_root  = this->parser->root();

    std::vector<std::string> validation_errors;

    // use the validator and schema to validate the currently set document text

    if ( !this->validator->validate(schema_root, input_root, validation_errors) )
    {
        // walk over the validation errors vector if the document is not valid

        for (auto i : validation_errors)
        {
            std::size_t foundline   = i.find( "line:" );

            std::size_t foundcolumn = i.find( " column:" );

            std::size_t founddash   = i.find( " - " );

            // parse line number out of the message and convert to integer

            std::string line_str = i.substr( foundline + 5, foundcolumn - (foundline + 5) );

            int line;

            std::stringstream( line_str ) >> line;

            // parse column number out of the message and convert to integer

            std::string column_str = i.substr( foundcolumn + 8, founddash - (foundcolumn + 8) );

            int column;

            std::stringstream( column_str ) >> column;

            // parse the actual message content out of the validation error

            std::string message = i.substr( founddash + 3 );

            wasp_check( line > 0 );

            wasp_check( column > 0 );

            wasp_check( !message.empty() );

            // according to the protocol - line and column number are zero based

            line--;
            column--;

            // build a diagnostics object out of the line, column, and message

            DataObject diagnostic;

            pass &= buildDiagnosticObject( diagnostic   ,
                                           this->errors ,
                                           line         ,
                                           column       ,
                                           line         ,
                                           column       ,
                                           1            ,
                                           "validate"   ,
                                           "validator"  ,
                                           message      );

            // push the created diagnostic object onto the diagnostics list

            diagnosticsList.push_back(diagnostic);
        }

    }

    return pass;
}

template < class INPUT      ,
           class INPUTNV    ,
           class SCHEMA     ,
           class SCHEMANV   ,
           class VALIDATOR  ,
           class CONNECTION >
bool WaspServer<INPUT,INPUTNV,SCHEMA,SCHEMANV,VALIDATOR,CONNECTION>::
    updateDocumentTextChanges(
                const std::string & replacement_text ,
                      int           start_line       ,
                      int           start_character  ,
                      int           end_line         ,
                      int           end_character    ,
                      int           range_length     )
{
    if (!this->is_setup)
    {
        this->errors << m_error_prefix << "Server needs to be setup" << std::endl;
        return false;
    }

    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    if (!this->is_document_open)
    {
        this->errors << m_error_prefix << "Server has no open document" << std::endl;
        return false;
    }

    bool pass = true;

    // TODO - in the future add logic to replace text using line and column
    // but for now expect all parameters to be -1 and replace entire document

    if ( ! (start_line      == -1 &&
            start_character == -1 &&
            end_line        == -1 &&
            end_character   == -1 &&
            range_length    == -1 ) )
    {
        this->errors << m_error_prefix << "WASPServer only currently supports full document changes" << std::endl;
        return false;
    }

    this->document_text = replacement_text;

    return pass;
}

template < class INPUT      ,
           class INPUTNV    ,
           class SCHEMA     ,
           class SCHEMANV   ,
           class VALIDATOR  ,
           class CONNECTION >
bool WaspServer<INPUT,INPUTNV,SCHEMA,SCHEMANV,VALIDATOR,CONNECTION>::
    gatherDocumentCompletionItems(
                      DataArray & completionItems  ,
                      bool      & is_incomplete    ,
                      int         line             ,
                      int         character        )
{
    if (!this->is_setup)
    {
        this->errors << m_error_prefix << "Server needs to be setup" << std::endl;
        return false;
    }

    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    if (!this->is_document_open)
    {
        this->errors << m_error_prefix << "Server has no open document" << std::endl;
        return false;
    }

    bool pass = true;

    wasp_check( this->inputdefinition );

    INPUTNV node = wasp::findNodeUnderLineColumn( this->parser->root() ,
                                                  line                 ,
                                                  character            );

    // list of context nodes to investigate for autocomplete options

    std::vector<INPUTNV> contexts;
    {
        // ascend hierarchy, gathering context nodes, only push non-decorative contexts

        for ( auto tmp = node                                             ;
              tmp.path().find( node.path() ) == 0 && !tmp.is_terminator() ;
              tmp = tmp.parent()                                          )
        {
            if ( !tmp.is_decorative() && tmp.type() != wasp::VALUE )
            {
                contexts.push_back( tmp );
            }

            if ( !tmp.has_parent() ) break;
        }

        // no context nodes, just use the node under the given line and column

        if ( contexts.empty() )
        {
            contexts.push_back( node );
        }
    }

    // list of options prioritized by indentation

    using ItemPair = std::pair<int, std::vector<AutoComplete>>;

    std::vector<ItemPair> prioritized_options;

    // process each context node

    for ( auto & context : contexts )
    {
        // input definition for the given node

        IDObject * def = this->inputdefinition->pathLookup( context.path() );

        // if context is valid but its definition is not, then continue to next

        if ( !def )
        {
            continue;
        }

        // list of items to populate

        std::vector<AutoComplete> options;

        // exists-in from input definition
        if ( auto ei = def->getExistsIn() )
        {
            // make lookup copy and move to parent if node is "value" and not in schema

            INPUTNV lookup_node = context;

            if ( lookup_node.type() == wasp::VALUE && def->getObjectName() != "value" )
            {
                if ( lookup_node.has_parent() )
                {
                    lookup_node = lookup_node.parent();
                }
            }

            // retrieve allowable values

            std::set<std::string> values;

            ei->lookupNodesCollectByValue( lookup_node , values );

            // populate string list

            for ( const std::string & iterate_value : values )
            {
                const std::string & text_value = iterate_value;

                options.push_back( AutoComplete() );

                AutoComplete & ac = options.back();

                ac.label         = text_value;
                ac.new_text      = text_value;
                ac.description   = text_value;
                ac.complete_type = "existsin";
            }

            // add constants

            for ( size_t i = 0 , ie = ei->getConstantsCount() ; i < ie ; i++ )
            {
                const std::string & text_value = ei->getConstantsAt(i);

                options.push_back( AutoComplete() );

                AutoComplete & ac = options.back();

                ac.label         = text_value;
                ac.new_text      = text_value;
                ac.description   = text_value;
                ac.complete_type = "existsin";
            }
        }

        // enum listing

        else if( auto ve = def->getValEnums() )
        {
            // populate string list

            for ( size_t i = 0 , ie = ve->getEnumsCount() ; i < ie ; i++ )
            {
                const std::string & text_value = ve->getEnumsAt(i);

                options.push_back( AutoComplete() );

                AutoComplete & ac = options.back();

                ac.label         = text_value;
                ac.new_text      = text_value;
                ac.description   = text_value;
                ac.complete_type = "valenums";
            }
        }

        // if no enums and if this is a "value" node, then try the parent for enums

        else if ( def->getIDParent() != nullptr && def->getIDParent()->getValEnums() )
        {
            if ( def->getObjectName() == "value" )
            {
                auto ve = def->getIDParent()->getValEnums();

                // populate string list

                for ( size_t i = 0, ie = ve->getEnumsCount(); i < ie; i++ )
                {
                    const std::string & text_value = ve->getEnumsAt(i);

                    options.push_back( AutoComplete() );

                    AutoComplete & ac = options.back();

                    ac.label         = text_value;
                    ac.new_text      = text_value;
                    ac.description   = text_value;
                    ac.complete_type = "valenums";
                }
            }
        }

        // input choice listing / used for autocomplete but not validation

        else if ( auto ic = def->getInputChoices() )
        {
            // populate string list

            for ( size_t i = 0 , ie = ic->getEnumsCount() ; i < ie ; i++ )
            {
                const std::string & text_value = ic->getEnumsAt(i);

                options.push_back( AutoComplete() );

                AutoComplete & ac = options.back();

                ac.label         = text_value;
                ac.new_text      = text_value;
                ac.description   = text_value;
                ac.complete_type = "inputchoices";
            }
        }

        // autocomplete templates for the current context

        else
        {
            const auto & children = def->getIDChildren();

            // look for templates supported by the current context

            for( IDObject * child : children )
            {
                // is this child allowed in the input context

                if( this->isAllowed( &context, child ) )
                {
                    this->addTemplate( child , options );
                }
            }
        }

        // only add if we found autocomplete options

        if ( !options.empty() )
        {
            // int pnc = context ? context->column() : 1;
            // int cc  = cursor.positionInBlock() + 1;
            // prioritized_options.push_back( { pnc - cc , options } );

            prioritized_options.push_back( { context.column() , options } );
        }
    }

    // prioritize by negative values in descending order
    // then non-negative values in ascending order

    auto negative = std::stable_partition( prioritized_options.begin() ,
                                           prioritized_options.end()   ,
                                           []( const ItemPair & ip     )
                                             { return ip.first < 0;  } );

    std::stable_sort( prioritized_options.begin()                 ,
                      negative                                    ,
                      []( const ItemPair & l , const ItemPair & r )
                        { return l.first > r.first;             } );

    std::stable_sort( negative                                    ,
                      prioritized_options.end()                   ,
                      []( const ItemPair & l , const ItemPair & r )
                        { return l.first <= r.first;            } );

    // add options sorted by priority to ordered items list

    for ( const auto & options_vec : prioritized_options )
    {
        for ( auto options : options_vec.second )
        {
            completionItems.push_back( DataObject() );

            DataObject * item = completionItems.back().to_object();

            pass &= buildCompletionObject( *item                  ,
                                            this->errors          ,
                                            options.label         ,
                                            line                  ,
                                            character             ,
                                            line                  ,
                                            character             ,
                                            options.new_text      ,
                                            1                     ,
                                            options.complete_type ,
                                            options.description   ,
                                            false                 ,
                                            false                 );
        }
    }

    // set is_incomplete flag to same as pass / fail and return

    is_incomplete = pass;

    return pass;
}

template < class INPUT      ,
           class INPUTNV    ,
           class SCHEMA     ,
           class SCHEMANV   ,
           class VALIDATOR  ,
           class CONNECTION >
bool WaspServer<INPUT,INPUTNV,SCHEMA,SCHEMANV,VALIDATOR,CONNECTION>::
    gatherDocumentDefinitionLocations(
                      DataArray & definitionLocations ,
                      int         line                ,
                      int         character           )
{
    if (!this->is_setup)
    {
        this->errors << m_error_prefix << "Server needs to be setup" << std::endl;
        return false;
    }

    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    if (!this->is_document_open)
    {
        this->errors << m_error_prefix << "Server has no open document" << std::endl;
        return false;
    }

    bool pass = true;

    INPUTNV selectedNode = wasp::findNodeUnderLineColumn( this->parser->root() ,
                                                          line                 ,
                                                          character            );

    // get the input definition node for this input node's path

    wasp_check( this->inputdefinition );

    auto def = this->inputdefinition->pathLookup( selectedNode.path() );

    // make lookup copy and move to parent if node is "value" and not in schema

    INPUTNV lookup_node = selectedNode;

    if ( lookup_node.type() == wasp::VALUE && def->getObjectName() != "value" )
    {
        if( lookup_node.has_parent() )
        {
            lookup_node = lookup_node.parent();
        }
    }

    // create set of found lookup nodes using a custom lambda comparator

    std::set<INPUTNV, std::function<bool(const INPUTNV & , const INPUTNV &)>>
                found_nodes( [] ( const INPUTNV & l , const INPUTNV & r )
                                { return ( l.line()   <  r.line() ||
                                         ( l.line()   == r.line() &&
                                           l.column() <  r.column() ) ); } );

    // if no definition, existsin, or lookups - just use info for selected node
    // otherwise iterate through all found nodes building definition locations

    if( def == nullptr  || def->getExistsIn() == nullptr  ||
        def->getExistsIn()->lookupNodesByValue(lookup_node, found_nodes) == 0 )
    {
        int start_line      = selectedNode.line();
        int start_character = selectedNode.column();
        int end_line        = selectedNode.last_line();
        int end_character   = selectedNode.last_column();

        definitionLocations.push_back( DataObject() );

        DataObject * location = definitionLocations.back().to_object();

        pass &= buildLocationObject( *location            ,
                                      this->errors        ,
                                      this->document_path ,
                                      start_line          ,
                                      start_character     ,
                                      end_line            ,
                                      end_character       );
    }
    else
    {
        for( auto iterate_node : found_nodes )
        {
            int start_line      = iterate_node.line();
            int start_character = iterate_node.column();
            int end_line        = iterate_node.last_line();
            int end_character   = iterate_node.last_column();

            definitionLocations.push_back( DataObject() );

            DataObject * location = definitionLocations.back().to_object();

            pass &= buildLocationObject( *location            ,
                                          this->errors        ,
                                          this->document_path ,
                                          start_line          ,
                                          start_character     ,
                                          end_line            ,
                                          end_character       );
        }
    }

    return pass;
}

template < class INPUT      ,
           class INPUTNV    ,
           class SCHEMA     ,
           class SCHEMANV   ,
           class VALIDATOR  ,
           class CONNECTION >
bool WaspServer<INPUT,INPUTNV,SCHEMA,SCHEMANV,VALIDATOR,CONNECTION>::
    gatherDocumentReferencesLocations(
                      DataArray & referencesLocations ,
                      int         line                ,
                      int         character           ,
                      bool        include_declaration )
{
    if (!this->is_setup)
    {
        this->errors << m_error_prefix << "Server needs to be setup" << std::endl;
        return false;
    }

    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    if (!this->is_document_open)
    {
        this->errors << m_error_prefix << "Server has no open document" << std::endl;
        return false;
    }

    bool pass = true;

    // TODO

    return pass;
}

template < class INPUT      ,
           class INPUTNV    ,
           class SCHEMA     ,
           class SCHEMANV   ,
           class VALIDATOR  ,
           class CONNECTION >
bool WaspServer<INPUT,INPUTNV,SCHEMA,SCHEMANV,VALIDATOR,CONNECTION>::
    gatherDocumentFormattingTextEdits(
                      DataArray & formattingTextEdits ,
                      int         start_line          ,
                      int         start_character     ,
                      int         end_line            ,
                      int         end_character       ,
                      int         tab_size            ,
                      bool        insert_spaces       )
{
    if (!this->is_setup)
    {
        this->errors << m_error_prefix << "Server needs to be setup" << std::endl;
        return false;
    }

    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    if (!this->is_document_open)
    {
        this->errors << m_error_prefix << "Server has no open document" << std::endl;
        return false;
    }

    bool pass = true;

    // TODO

    return pass;
}

template < class INPUT      ,
           class INPUTNV    ,
           class SCHEMA     ,
           class SCHEMANV   ,
           class VALIDATOR  ,
           class CONNECTION >
bool WaspServer<INPUT,INPUTNV,SCHEMA,SCHEMANV,VALIDATOR,CONNECTION>::
    gatherDocumentSymbols(
                      DataArray & documentSymbols )
{
    if (!this->is_setup)
    {
        this->errors << m_error_prefix << "Server needs to be setup" << std::endl;
        return false;
    }

    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    if (!this->is_document_open)
    {
        this->errors << m_error_prefix << "Server has no open document" << std::endl;
        return false;
    }

    bool pass = true;

    // store the non decorative children of the root and cache the size

    const auto & children = this->parser->root().non_decorative_children();

    size_t children_size = children.size();

    // walk over non decorative children of the root

    for( size_t i = 0; i < children_size; i++ )
    {
        // store the current child node

        auto node = children[i];

        // get the node name / detail / line / column / last_line / last_column

        std::string name        = node.name();
        std::string detail      = node.is_leaf() ? node.data() : name;
        int         line        = node.line();
        int         column      = node.column();
        int         last_line   = node.last_line();
        int         last_column = node.last_column();

        // according to the protocol - line and column number are zero based

        line--;
        column--;
        last_line--;
        last_column--;

        // according to the protocol - last_column is right AFTER last character

        last_column++;

        // build a document symbol object from the node info and push onto array

        documentSymbols.push_back( DataObject() );

        DataObject * child = documentSymbols.back().to_object();

        pass &= buildDocumentSymbolObject( *child        ,
                                            this->errors ,
                                            name         ,
                                            detail       ,
                                            1            ,
                                            false        ,
                                            line         ,
                                            column       ,
                                            last_line    ,
                                            last_column  ,
                                            line         ,
                                            column       ,
                                            last_line    ,
                                            last_column  );

        // call method to recursively fill document symbols for each child node

        pass &= this->traverseParseTreeAndFillSymbols( node , *child );
    }

    return pass;
}

template < class INPUT      ,
           class INPUTNV    ,
           class SCHEMA     ,
           class SCHEMANV   ,
           class VALIDATOR  ,
           class CONNECTION >
bool WaspServer<INPUT,INPUTNV,SCHEMA,SCHEMANV,VALIDATOR,CONNECTION>::
    traverseParseTreeAndFillSymbols(
                      INPUTNV      node   ,
                      DataObject & parent )
{
    bool pass = true;

    // store the non decorative children of the given node and cache the size

    const auto & children = node.non_decorative_children();

    size_t children_size = children.size();

    // walk over non decorative children of the given node

    for( size_t i = 0; i < children_size; i++ )
    {
        // store the current child node

        auto node = children[i];

        // get the node name / detail / line / column / last_line / last_column

        std::string name        = node.name();
        std::string detail      = node.is_leaf() ? node.data() : name;
        int         line        = node.line();
        int         column      = node.column();
        int         last_line   = node.last_line();
        int         last_column = node.last_column();

        // according to the protocol - line and column number are zero based

        line--;
        column--;
        last_line--;
        last_column--;

        // according to the protocol - last_column is right AFTER last character

        last_column++;

        // build a document symbol object from the node info and push onto array

        DataObject & child = addDocumentSymbolChild( parent );

        pass &= buildDocumentSymbolObject( child        ,
                                           this->errors ,
                                           name         ,
                                           detail       ,
                                           1            ,
                                           false        ,
                                           line         ,
                                           column       ,
                                           last_line    ,
                                           last_column  ,
                                           line         ,
                                           column       ,
                                           last_line    ,
                                           last_column  );

        // call method to recursively fill document symbols for each child node

        pass &= this->traverseParseTreeAndFillSymbols( node , child );
    }

    return pass;
}

template < class INPUT      ,
           class INPUTNV    ,
           class SCHEMA     ,
           class SCHEMANV   ,
           class VALIDATOR  ,
           class CONNECTION >
bool WaspServer<INPUT,INPUTNV,SCHEMA,SCHEMANV,VALIDATOR,CONNECTION>::
    isAllowed( INPUTNV * node , IDObject * def )
{
    wasp_require( node != nullptr );

    wasp_require( def != nullptr );

    // number of definition instances

    auto count = node->child_by_name( def->getObjectName() ).size();

    // max-occurrences definition

    auto maxOccurs = def->getMaxOccurs();

    // failed max-occurrences check

    if( maxOccurs != nullptr && static_cast<int>( count ) >= maxOccurs->getConstantValue() )
    {
        // if the above failed, then we should only return false for unallowed autocomplete
        // if there actually is a constant value or there are no input lookup paths associated

        if ( maxOccurs->hasConstantValue() || maxOccurs->getLookupPathCount() == 0 )
        {
            return false;
        }
    }

    //get the definition node's parent

    auto defParent = def->getIDParent();

    if( defParent != nullptr )
    {
        // child-exactly-one check

        for( size_t i = 0, ie = defParent->getChildExactlyOneCount(); i < ie; i++ )
        {
            if( !defParent->getChildExactlyOneAt( i )->isChildAllowed( def->getObjectName(), node ) )
            {
                return false;
            }
        }

        // child-at-most-one check

        for( size_t i = 0, ie = defParent->getChildAtMostOneCount(); i < ie; i++ )
        {
            if( !defParent->getChildAtMostOneAt( i )->isChildAllowed( def->getObjectName(), node ) )
            {
                return false;
            }
        }
    }

    // if none of the above checks indicated not-allowed - then allowed

    return true;
}

template < class INPUT      ,
           class INPUTNV    ,
           class SCHEMA     ,
           class SCHEMANV   ,
           class VALIDATOR  ,
           class CONNECTION >
bool WaspServer<INPUT,INPUTNV,SCHEMA,SCHEMANV,VALIDATOR,CONNECTION>::
    addTemplate( IDObject  * def                     ,
                 std::vector<AutoComplete> & options )
{
    wasp_require( def != nullptr );

    bool pass = true;

    // child's metadata

    std::string name        = def->getObjectName();
    std::string inputtype   = def->getObjectType();
    std::string inputname   = def->getInputName();
    std::string description = def->getObjectDescription();

    // definition's value node, defaults to definition node

    IDObject * valdef = def;

    // definition's value node isn't null and has the needed input value info

    if( IDObject * vn = def->getValueNode() )
    {
        if ( ( vn->hasInputDefault()                                                       ) ||
             ( vn->hasValEnumsRule() && !def->hasInputDefault()                            ) ||
             ( vn->hasValTypeRule()  && !def->hasInputDefault() && !def->hasValEnumsRule() ) )
        {
            valdef = vn;
        }
    }

    // set inputvalue for template parameter payload based on schema

    std::string inputvalue = "0";

    if ( valdef->hasInputDefault() )
    {
        inputvalue = valdef->getInputDefault();
    }
    else if( valdef->hasValEnumsRule() &&
             valdef->getValEnums()->getEnumsCount() > 0 )
    {
        inputvalue = valdef->getValEnums()->getEnumsAt( 0 );
    }
    else if( valdef->hasValTypeRule() )
    {
        const std::string & valtype = valdef->getValType()->getType();
        if      ( valtype == "String" ) inputvalue = "insert_string_here";
        else if ( valtype == "Int"    ) inputvalue = "1";
        else if ( valtype == "Real"   ) inputvalue = "0.0";
    }

    // template paramters

    std::string json_parameters = "{  \"Name\":       \"" + name       + "\","
                                  "   \"InputType\":  \"" + inputtype  + "\","
                                  "   \"InputName\":  \"" + inputname  + "\","
                                  "   \"InputValue\": \"" + inputvalue + "\"}";

    // account for possible input variants

    if( def->getInputVariantCount() > 0 )
    {
        // add item for each variant

        for( size_t i = 0, ie = def->getInputVariantCount(); i < ie; i++ )
        {
            // variant name

            const std::string & variant = def->getInputVariantAt( i );

            // variant file path

            std::string variant_path = this->template_dir + "/" +
                                       name + "." + variant + ".tmpl";

            // replace ' ' and + with underscores to map to its file name

            std::replace( variant_path.begin(), variant_path.end(), ' ', '_');
            std::replace( variant_path.begin(), variant_path.end(), '+', '_');

            // if variant file exists then expand template using json payload

            if ( wasp::file_exists( variant_path ) )
            {

                // if template expansion passes then add display and result

                std::stringstream expanded_result;

                if ( wasp::expand_template( expanded_result ,
                                            this->errors    ,
                                            this->errors    ,
                                            variant_path    ,
                                            json_parameters ) )
                {
                    options.push_back( AutoComplete() );

                    AutoComplete & ac = options.back();

                    ac.label         = inputname + " - " + variant;
                    ac.new_text      = expanded_result.str() ;
                    ac.description   = description;
                    ac.complete_type = "inputvariants";
                }
            }
        }
    }

    // account for possible input template

    if( def->hasInputTmpl() )
    {
        const std::string & template_path = this->template_dir + "/" +
                                            def->getInputTmpl() + ".tmpl";

        // if template file exists then expand template using  json payload

        if ( wasp::file_exists( template_path ) )
        {
            // if template expansion passes then add display and result

            std::stringstream expanded_result;

            if ( wasp::expand_template( expanded_result ,
                                        this->errors    ,
                                        this->errors    ,
                                        template_path   ,
                                        json_parameters ) )
            {
                options.push_back( AutoComplete() );

                AutoComplete & ac = options.back();

                ac.label         = inputname;
                ac.new_text      = expanded_result.str() ;
                ac.description   = description;
                ac.complete_type = "inputtmpl";
            }
        }
    }

    return pass;
}

} // namespace lsp
} // namespace wasp
