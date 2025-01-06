/*
 * File:   tstHalite.cpp
 * Author: Robert A. Lefebvre
 */

#include <cstdlib>
#include <sstream>
#include <string>
#include <stdexcept>

#include <utility>
#include "wasphalite/SubStringIndexer.h"

#include "gtest/gtest.h"
using namespace std;
using namespace wasp;

TEST(Halite, sub_string_index)
{
    std::string      str    = " sssss";
    std::string      substr = "s";
    SubStringIndexer indicies;
    ASSERT_TRUE(indicies.index(str, substr));
    const SubStringIndexer::Index_type& index = indicies.data();
    ASSERT_TRUE(index.empty() == false);
    SubStringIndexer::Index_type expected = {1, 2, 3, 4, 5};
    ASSERT_EQ(expected, index);
}

TEST(Halite, multichar_sub_string_index)
{
    std::string      str    = " sssss";
    std::string      substr = "ss";
    SubStringIndexer indicies;
    ASSERT_TRUE(indicies.index(str, substr));
    const SubStringIndexer::Index_type& index = indicies.data();
    ASSERT_TRUE(index.empty() == false);
    SubStringIndexer::Index_type expected = {1, 3};
    ASSERT_EQ(expected, index);
}
/**
 * @brief TEST test matching indices
 */
TEST(Halite, sub_string_merge_simple)
{
    std::string      str = " foo <ted> bar";
    SubStringIndexer start_indices;
    {
        std::string startstr = "<";
        ASSERT_TRUE(start_indices.index(str, startstr));
        const SubStringIndexer::Index_type& index = start_indices.data();
        ASSERT_TRUE(index.empty() == false);
        SubStringIndexer::Index_type expected = {5};
        ASSERT_EQ(expected, index);
    }
    SubStringIndexer end_indices;
    {
        std::string endstr = ">";
        ASSERT_TRUE(end_indices.index(str, endstr));
        const SubStringIndexer::Index_type& index = end_indices.data();
        ASSERT_TRUE(index.empty() == false);
        SubStringIndexer::Index_type expected = {9};
        ASSERT_EQ(expected, index);
    }

    SubStringIndexer::IndexPairs_type results =
        start_indices.merge(end_indices);

    {
        ASSERT_EQ(1, results.size());
        SubStringIndexer::IndexPairs_type expected = {{5, 9}};
        ASSERT_EQ(expected, results);
    }
}

/**
 * @brief TEST test matching indices with nested matches
 */
TEST(Halite, sub_string_merge_nested)
{
    std::string      str = " foo <<ted> bar>";
    SubStringIndexer start_indices;
    {
        std::string startstr = "<";
        ASSERT_TRUE(start_indices.index(str, startstr));
        const SubStringIndexer::Index_type& index = start_indices.data();
        ASSERT_TRUE(index.empty() == false);
        SubStringIndexer::Index_type expected = {5, 6};
        ASSERT_EQ(expected, index);
    }
    SubStringIndexer end_indices;
    {
        std::string endstr = ">";
        ASSERT_TRUE(end_indices.index(str, endstr));
        const SubStringIndexer::Index_type& index = end_indices.data();
        ASSERT_TRUE(index.empty() == false);
        SubStringIndexer::Index_type expected = {10, 15};
        ASSERT_EQ(expected, index);
    }

    SubStringIndexer::IndexPairs_type results =
        start_indices.merge(end_indices);

    {
        ASSERT_EQ(2, results.size());
        SubStringIndexer::IndexPairs_type expected = {{5, 15}, {6, 10}};
        ASSERT_EQ(expected, results);
    }
}

/**
 * @brief TEST test matching indices with nested matches
 */
TEST(Halite, sub_string_merge_nested_unmatched)
{
    std::string      str = "< foo <<ted> bar>";
    SubStringIndexer start_indices;
    {
        std::string startstr = "<";
        ASSERT_TRUE(start_indices.index(str, startstr));
        const SubStringIndexer::Index_type& index = start_indices.data();
        ASSERT_TRUE(index.empty() == false);
        SubStringIndexer::Index_type expected = {0, 6, 7};
        ASSERT_EQ(expected, index);
    }
    SubStringIndexer end_indices;
    {
        std::string endstr = ">";
        ASSERT_TRUE(end_indices.index(str, endstr));
        const SubStringIndexer::Index_type& index = end_indices.data();
        ASSERT_TRUE(index.empty() == false);
        SubStringIndexer::Index_type expected = {11, 16};
        ASSERT_EQ(expected, index);
    }

    SubStringIndexer::IndexPairs_type results =
        start_indices.merge(end_indices);

    {
        ASSERT_EQ(2, results.size());
        SubStringIndexer::IndexPairs_type expected = {{6, 16}, {7, 11}};
        ASSERT_EQ(expected, results);
    }
}

/**
 * @brief TEST test indices involving unmatched/hanging indices
 */
TEST(Halite, sub_string_merge_hanging)
{
    std::string      str = " foo ><ted>< bar";
    SubStringIndexer start_indices;
    {
        std::string startstr = "<";
        ASSERT_TRUE(start_indices.index(str, startstr));
        const SubStringIndexer::Index_type& index = start_indices.data();
        ASSERT_TRUE(index.empty() == false);
        SubStringIndexer::Index_type expected = {6, 11};
        ASSERT_EQ(expected, index);
    }
    SubStringIndexer end_indices;
    {
        std::string endstr = ">";
        ASSERT_TRUE(end_indices.index(str, endstr));
        const SubStringIndexer::Index_type& index = end_indices.data();
        ASSERT_TRUE(index.empty() == false);
        SubStringIndexer::Index_type expected = {5, 10};
        ASSERT_EQ(expected, index);
    }

    SubStringIndexer::IndexPairs_type results =
        start_indices.merge(end_indices);

    {
        ASSERT_EQ(1, results.size());
        SubStringIndexer::IndexPairs_type expected = {{6, 10}};
        ASSERT_EQ(expected, results);
    }
}

/**
 * @brief TEST test multiple indices involving unmatched/hanging indices
 */
TEST(Halite, sub_string_merge_multiple)
{
    std::string      str = "> foo ><ted>< <bar>";
    SubStringIndexer start_indices;
    {
        std::string startstr = "<";
        ASSERT_TRUE(start_indices.index(str, startstr));
        const SubStringIndexer::Index_type& index = start_indices.data();
        ASSERT_TRUE(index.empty() == false);
        SubStringIndexer::Index_type expected = {7, 12, 14};
        ASSERT_EQ(expected, index);
    }
    SubStringIndexer end_indices;
    {
        std::string endstr = ">";
        ASSERT_TRUE(end_indices.index(str, endstr));
        const SubStringIndexer::Index_type& index = end_indices.data();
        ASSERT_TRUE(index.empty() == false);
        SubStringIndexer::Index_type expected = {0, 6, 11, 18};
        ASSERT_EQ(expected, index);
    }

    SubStringIndexer::IndexPairs_type results =
        start_indices.merge(end_indices);

    {
        ASSERT_EQ(2, results.size());
        SubStringIndexer::IndexPairs_type expected = {{7, 11}, {14, 18}};
        ASSERT_EQ(expected, results);
    }
}

/**
 * @brief TEST test multiple indices involving unmatched/hanging indices
 */
TEST(Halite, multichar_sub_string_merge_multiple)
{
    std::string      str = "}} foo }}{{ted}}{{ {{bar}}";
    SubStringIndexer start_indices;
    {
        std::string startstr = "{{";
        ASSERT_TRUE(start_indices.index(str, startstr));
        const SubStringIndexer::Index_type& index = start_indices.data();
        ASSERT_TRUE(index.empty() == false);
        SubStringIndexer::Index_type expected = {9, 16, 19};
        ASSERT_EQ(expected, index);
    }
    SubStringIndexer end_indices;
    {
        std::string endstr = "}}";
        ASSERT_TRUE(end_indices.index(str, endstr));
        const SubStringIndexer::Index_type& index = end_indices.data();
        ASSERT_TRUE(index.empty() == false);
        SubStringIndexer::Index_type expected = {0, 7, 14, 24};
        ASSERT_EQ(expected, index);
    }

    SubStringIndexer::IndexPairs_type results =
        start_indices.merge(end_indices);

    {
        ASSERT_EQ(2, results.size());
        SubStringIndexer::IndexPairs_type expected = {{9, 14}, {19, 24}};
        ASSERT_EQ(expected, results);
    }
}
/**
 * @brief TEST test matching indices with nested matches
 */
TEST(Halite, mulitchar_sub_string_merge_nested_unmatched)
{
    std::string      str = "{{ foo {{{{ted}} bar}}";
    SubStringIndexer start_indices;
    {
        std::string startstr = "{{";
        ASSERT_TRUE(start_indices.index(str, startstr));
        const SubStringIndexer::Index_type& index = start_indices.data();
        ASSERT_TRUE(index.empty() == false);
        SubStringIndexer::Index_type expected = {0, 7, 9};
        ASSERT_EQ(expected, index);
    }
    SubStringIndexer end_indices;
    {
        std::string endstr = "}}";
        ASSERT_TRUE(end_indices.index(str, endstr));
        const SubStringIndexer::Index_type& index = end_indices.data();
        ASSERT_TRUE(index.empty() == false);
        SubStringIndexer::Index_type expected = {14, 20};
        ASSERT_EQ(expected, index);
    }

    SubStringIndexer::IndexPairs_type results =
        start_indices.merge(end_indices);

    {
        ASSERT_EQ(2, results.size());
        SubStringIndexer::IndexPairs_type expected = {{7, 20}, {9, 14}};
        ASSERT_EQ(expected, results);
    }
}
TEST(Halite, sub_string_merge_nested2)
{
    std::string      str = "<<a>b><<<a>b>c>";
    SubStringIndexer start_indices;
    {
        std::string startstr = "<";
        ASSERT_TRUE(start_indices.index(str, startstr));
        const SubStringIndexer::Index_type& index = start_indices.data();
        ASSERT_TRUE(index.empty() == false);
        SubStringIndexer::Index_type expected = {0, 1, 6, 7, 8};
        ASSERT_EQ(expected, index);
    }
    SubStringIndexer end_indices;
    {
        std::string endstr = ">";
        ASSERT_TRUE(end_indices.index(str, endstr));
        const SubStringIndexer::Index_type& index = end_indices.data();
        ASSERT_TRUE(index.empty() == false);
        SubStringIndexer::Index_type expected = {3, 5, 10, 12, 14};
        ASSERT_EQ(expected, index);
    }

    SubStringIndexer::IndexPairs_type results =
        start_indices.merge(end_indices);

    {
        ASSERT_EQ(5, results.size());
        SubStringIndexer::IndexPairs_type expected = {
            {0, 5}, {1, 3}, {6, 14}, {7, 12}, {8, 10}};
        ASSERT_EQ(expected, results);
    }

    std::vector<size_t> depths = SubStringIndexer::depths(results);
    {
        ASSERT_EQ(5, depths.size());
        std::vector<size_t> expected = {1, 2, 1, 2, 3};
        ASSERT_EQ(expected, depths);
    }
}

TEST(Halite, depths)
{
    // test nested <   >
    SubStringIndexer::IndexPairs_type data   = {{0, 4}};
    std::vector<size_t>               depths = SubStringIndexer::depths(data);
    {
        std::vector<size_t> expected = {1};
        ASSERT_EQ(expected, depths);
    }
}
TEST(Halite, depths_nested_siblings)
{
    // test nested <<><><>>
    SubStringIndexer::IndexPairs_type data   = {{0, 7}, {1, 2}, {3, 4}, {5, 6}};
    std::vector<size_t>               depths = SubStringIndexer::depths(data);
    {
        std::vector<size_t> expected = {1, 2, 2, 2};
        ASSERT_EQ(expected, depths);
    }
}

TEST(Halite, depths_nested_cousins)
{
    // test nested <<><>><<<>>>>
    SubStringIndexer::IndexPairs_type data = {{0, 5},  {1, 2},  {3, 4},
                                              {6, 11}, {7, 10}, {8, 9}};
    std::vector<size_t> depths = SubStringIndexer::depths(data);
    {
        std::vector<size_t> expected = {1, 2, 2, 1, 2, 3};
        ASSERT_EQ(expected, depths);
    }
}
