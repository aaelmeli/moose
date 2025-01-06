#ifndef WASPLSP_SYMBOL_ITERATOR_H
#define WASPLSP_SYMBOL_ITERATOR_H

#include <vector>
#include <string>
#include <sstream>
#include <memory>
#include "waspcore/Object.h"
#include "waspcore/decl.h"
#include "wasplsp/LSP.h"

namespace wasp {
namespace lsp  {

class WASP_PUBLIC SymbolIterator
{
  public:

    SymbolIterator( DataObject::SP response )
    {
        // add reponse as the root of the symbol lineage

        this->symbols_lineage.push_back( &*response );

        // store shared_ptr to response root to prevent dangling pointer

        m_response = response;

        // recusively sort each level of symbol tree

        this->recursiveSortByPosition();
    }

    ~SymbolIterator(){}

    typedef std::shared_ptr<SymbolIterator> SP;

    /** get the size of children symbol array at the currently set lineage level
     * @return - size of children at current level - zero if children array null
     */
    size_t getChildSize() const;

    /** get the path from root to leaf ( plus line / col ) of at currentl level
     * @return - string representing path with '/' delimiter ( plus line / col )
     */
    std::string getPath() const;

    /** move the symbol to be the child at the given index of the current level
     * @param index - index of the child symbol that should become currently set
     * @return - true if index not out of bounds and the move was successful
     */
    bool moveToChildAt( size_t index );

    /** move the currently setsymbol to be the parent of the current symbol level
     * @return - true if not already at root and the move up was successful
     */
    bool moveToParent();

    /** dissect current document symbol into the provided parameter references
     * @param name - name of this document symbol
     * @param detail - detail string for this document symbol
     * @param kind - kind value for this document symbol
     * @param deprecated - flag indicating if this document symbol is deprecated
     * @param start_line - starting line number for symbol ( zero-based )
     * @param start_character - starting column number for symbol ( zero-based )
     * @param end_line - ending line number for symbol ( zero-based )
     * @param end_character - ending column number for symbol ( zero-based )
     * @param selection_start_line - starting selection line ( zero-based )
     * @param selection_start_character - starting selection column ( zero-based )
     * @param selection_end_line - ending selection line ( zero-based )
     * @param selection_end_character - ending selection column ( zero-based )
     * @return - true if the object was successfully dissected without error
     */
    bool dissectCurrentSymbol( std::string  & name                      ,
                               std::string  & detail                    ,
                               int          & kind                      ,
                               bool         & deprecated                ,
                               int          & start_line                ,
                               int          & start_character           ,
                               int          & end_line                  ,
                               int          & end_character             ,
                               int          & selection_start_line      ,
                               int          & selection_start_character ,
                               int          & selection_end_line        ,
                               int          & selection_end_character   );

    /** get all errors that have been stored by symbol iterator for any reason
     * @return - error string of all errors stored by symbol iterator
     */
    std::string getErrors() const { return this->errors.str(); }

  private:

    /**
     * @brief m_response - response root shared_ptr to prevent dangling pointer
     */
    DataObject::SP m_response;

    /**
     * @brief symbols_lineage - vector from root symbol to current set symbol
     */
    std::vector<DataObject *> symbols_lineage;

    /**
     * @brief errors - all errors stored by symbol iterator for any reason
     */
    std::stringstream errors;

    /** recusively sort each child level of symbol tree by line / col position
     * @return - true if the symbols are hierarchical and sorting had no issues
     */
    bool recursiveSortByPosition();
};

} // namespace lsp
} // namespace wasp

#endif // WASPLSP_SYMBOL_ITERATOR_H
