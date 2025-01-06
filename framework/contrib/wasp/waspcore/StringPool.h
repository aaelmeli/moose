#ifndef WASP_STRINGPOOL_H
#define WASP_STRINGPOOL_H
#include <cstdint>
#include <vector>
#include <iostream>
#include "waspcore/decl.h"

namespace wasp
{
/**
 * @brief default_token_index_type_size
 */
typedef std::uint32_t default_token_index_type_size;
/**
 * @class StringPool Memory pool and data accessor for string data
 * This data consists of indexed null terminated character array data.
 */
template<typename token_index_type_size = default_token_index_type_size>
class WASP_PUBLIC StringPool
{
  public:
    typedef token_index_type_size index_type_size;
    StringPool();
    StringPool(const StringPool<index_type_size>& orig);
    ~StringPool();
    // acquire the string data for the token at the given index
    const char* data(index_type_size index) const;
    /**
     * @brief push push a new string into the pool
     * @param str the new string to place into the pool
     */
    void push(const char* str);
    /**
     * @brief pop remove the last data inserted
     */
    void pop();  // pop last string

    /**
     * @brief size acquire the number characters in the pool
     * @return std::size_t the number of chars
     */
    std::size_t size() const { return m_data.size(); }
    /**
     * @brief string_count acquire the number of strings in the pool
     * @return std::size_t the number of strings in the pool
     */
    std::size_t string_count() const { return m_token_data_indices.size(); }

    /**
     * @brief set the data at the given location
     * @param data_index the index of the location to change
     * @param str the new string
     * @return true, iff the set was successful.
     * NOTE : current implementation only allows for setting the str
     * for the last inserted string.
     */
    bool set(index_type_size data_index, const char* str);

  private:
    /**
     * @brief m_data null terminated character array
     */
    std::vector<char> m_data;
    /**
     * @brief m_token_data_indices the indices of each token
     */
    std::vector<index_type_size>
        m_token_data_indices;  // token starting indices into m_data
};
#include "waspcore/StringPool.i.h"
}  // end of namespace
#endif
