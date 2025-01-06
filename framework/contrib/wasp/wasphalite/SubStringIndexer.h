#ifndef HALITE_SUBSTRING_INDEXER_H
#define HALITE_SUBSTRING_INDEXER_H

#include <vector>
#include <string>
#include <utility>

#include "waspcore/decl.h"

namespace wasp
{
class WASP_PUBLIC SubStringIndexer
{
  public:
    typedef std::vector<size_t> Index_type;
    typedef std::pair<size_t, size_t> IndexPair_type;
    typedef std::vector<IndexPair_type> IndexPairs_type;
    SubStringIndexer();
    SubStringIndexer(const SubStringIndexer& orig);
    ~SubStringIndexer();

    SubStringIndexer(const std::string& str, const std::string& substr);
    bool index(const std::string& str, const std::string& substr);

    /**
     * @brief merge produce a merged set of index pairs
     * @param other the indices to merge with
     * @return a vector of paired indices order from left to right
     * This uses a simple opening/closing matching algorithm
     * where the indices contained in other are deemed the closing indices
     * Note : Not all indices will be in the resulting merged set, only those
     * that are deemed matching
     */
    IndexPairs_type merge(const SubStringIndexer& other) const;
    /**
     * @brief depths calculates the depth of each indexed string
     * @param indices the pairs of indices, sorted, to determine depth
     * @return vector of depths. These depths are >= 1 and useful in identifying
     * hierarchy of substrs
     * If Depth1 less than Depth2, Depth2 is a child of Depth1, etc.
     * If Depth2 is equal to Depth3, they are siblings.
     * If Depth4 is greater than Depth3, it is an uncle, and sibling to Depth1
     * In this way we can determine <Depth1 <Depth2><Depth3>><Depth4>
     */
    static std::vector<size_t> depths(const IndexPairs_type& indices);

    const Index_type& data() const { return m_indices; }

  private:
    Index_type m_indices;
};

}  // wasp
#endif
