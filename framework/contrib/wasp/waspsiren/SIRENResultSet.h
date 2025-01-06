#ifndef SIREN_SIRENRESULTSET_H
#define SIREN_SIRENRESULTSET_H
#include <string>
#include <vector>
#include "waspcore/decl.h"
namespace wasp
{
/**
 * @brief class SIRENResultSet contains the evaluated results from a selection
 *
 */
template<typename TAdapter>
class WASP_PUBLIC SIRENResultSet
{
  public:
    typedef std::pair<std::string, std::string> ScalarResult;
    SIRENResultSet();
    SIRENResultSet(const SIRENResultSet& orig);
    ~SIRENResultSet();

    /**
     * @brief result_count indicates the number of results contain in the set
     * @return set count
     */
    std::size_t result_count() const { return results.size(); }
    std::size_t size() const { return results.size(); }
    /**
     * @brief is_adapted determines if the result at the given index is of type
     * \TAdapter
     * @param index
     * @return true, iff the type at the given index is \TAdapter
     */
    bool is_adapted(std::size_t index) const
    {
        return results[index].first == ADAPTED;
    }
    /**
     * @brief is_scalar determine if the result at the given index is a named
     * result (count,"3")
     * @param index
     * @return true, iff the type at the given index is a computed result
     */
    bool is_scalar(std::size_t index) const
    {
        return results[index].first == SCALAR;
    }

    /**
     * @brief adapted acquires the adapted result
     * @param index the index of the adapted result
     * @return TAdapter the adapter to the node type
     */
    const TAdapter& adapted(std::size_t index) const
    {
        return adapted_results[results[index].second];
    }
    /**
     * @brief scalar acquires the ScalarResult consisting of a name and value.
     * @param index the index of the scalar result.
     * @return ScalarResult is the named
     */
    const ScalarResult& scalar(std::size_t index) const
    {
        return scalar_results[results[index].second];
    }

    void push(const TAdapter& node);
    void push(const std::string& name, const std::string& data);

  private:
    // a result could be an adapted result from a parse tree (TreeNodeView)
    // or a calculated, named result (count,10)
    std::vector<TAdapter>     adapted_results;
    std::vector<ScalarResult> scalar_results;
    enum type
    {
        ADAPTED,
        SCALAR
    };
    std::vector<std::pair<type, unsigned int>> results;
};
#include "waspsiren/SIRENResultSet.i.h"
}  // end of namespace
#endif
