#include "waspcore/TokenPool.h"
#include "gtest/gtest.h"
#include <iostream>
#include <string>

using namespace wasp;

// fake type enumerations
char word    = 0;
char integer = 1;
char real    = 2;
char assign  = 3;
TEST(TokenPool, push_test)
{
    //'ted 234\n'
    TokenPool<char> tp;
    {
        std::string                   data        = "ted";
        default_token_index_type_size token_index = 0;
        tp.push(data.c_str(), word, 0);  // line 1 column 1
        ASSERT_EQ(1, tp.size());
        auto str = tp.str(token_index);
        ASSERT_EQ(data, str);
        ASSERT_EQ(1, tp.line(token_index));
        ASSERT_EQ(1, tp.column(token_index));
        ASSERT_EQ(0, tp.offset(token_index));
        ASSERT_EQ(word, tp.type(token_index));
    }
    {
        std::string                   data        = "234";
        default_token_index_type_size token_index = 1;
        tp.push(data.c_str(), integer, 4);  // line 1 column 5
        ASSERT_EQ(2, tp.size());
        std::string str = tp.str(token_index);
        ASSERT_EQ(data, str);
        ASSERT_EQ(1, tp.line(token_index));
        ASSERT_EQ(5, tp.column(token_index));
        ASSERT_EQ(4, tp.offset(token_index));
        ASSERT_EQ(integer, tp.type(token_index));
    }
    {
        tp.push_line(7);  // line 1
        ASSERT_EQ(1, tp.line_count());
        ASSERT_EQ(7, tp.line_offset(0));
        ASSERT_EQ(2, tp.size());  // still only 2 tokens
    }
    //'ted 234\n'    <-- 0- 7 bytes
    //'234.234 word' <-- 8-19 bytes
    {  // token after a new line
        std::string                   data        = "234.234";
        default_token_index_type_size token_index = 2;
        tp.push(data.c_str(), real, 8);  // line 2 column 1
        ASSERT_EQ(3, tp.size());
        std::string str = tp.str(token_index);
        ASSERT_EQ(data, str);
        ASSERT_EQ(2, tp.line(token_index));
        ASSERT_EQ(1, tp.column(token_index));
        ASSERT_EQ(8, tp.offset(token_index));
        ASSERT_EQ(real, tp.type(token_index));
    }
    {
        std::string                   data        = "word";
        default_token_index_type_size token_index = 3;
        tp.push(data.c_str(), word, 16);  // line 2 column 9
        ASSERT_EQ(4, tp.size());
        std::string str = tp.str(token_index);
        ASSERT_EQ(data, str);
        ASSERT_EQ(2, tp.line(token_index));
        ASSERT_EQ(9, tp.column(token_index));
        ASSERT_EQ(16, tp.offset(token_index));
        ASSERT_EQ(word, tp.type(token_index));
    }
    ASSERT_EQ(4, tp.size());
    ASSERT_EQ(1, tp.line_count());
    ASSERT_EQ(7, tp.line_offset(0));
}

TEST(TokenPool, pop_test)
{
    //'ted 234\n'
    TokenPool<char> tp;
    {
        std::string                   data        = "ted";
        default_token_index_type_size token_index = 0;
        tp.push(data.c_str(), word, 0);  // line 1 column 1
        ASSERT_EQ(1, tp.size());
        std::string str = tp.str(token_index);
        ASSERT_EQ(data, str);
        ASSERT_EQ(1, tp.line(token_index));
        ASSERT_EQ(1, tp.column(token_index));
        ASSERT_EQ(0, tp.offset(token_index));
        ASSERT_EQ(word, tp.type(token_index));
    }
    {
        std::string                   data        = "234";
        default_token_index_type_size token_index = 1;
        tp.push(data.c_str(), integer, 4);  // line 1 column 5
        ASSERT_EQ(2, tp.size());
        std::string str = tp.str(token_index);
        ASSERT_EQ(data, str);
        ASSERT_EQ(1, tp.line(token_index));
        ASSERT_EQ(5, tp.column(token_index));
        ASSERT_EQ(4, tp.offset(token_index));
        ASSERT_EQ(integer, tp.type(token_index));
    }
    {
        tp.push_line(7);  // line 1
        ASSERT_EQ(1, tp.line_count());
        ASSERT_EQ(7, tp.line_offset(0));
        ASSERT_EQ(2, tp.size());  // still only 2 tokens
    }
    {
        tp.push_line(8);  // line 2
        ASSERT_EQ(2, tp.line_count());
        ASSERT_EQ(8, tp.line_offset(1));
        ASSERT_EQ(2, tp.size());  // still only 2 tokens
    }
    //'ted 234\n\n'    <-- 0- 8 bytes
    //'234.234 word' <-- 9-19 bytes
    {  // token after a new line
        std::string                   data        = "234.234";
        default_token_index_type_size token_index = 2;
        tp.push(data.c_str(), real, 9);  // line 3 column 1
        ASSERT_EQ(3, tp.size());
        std::string str = tp.str(token_index);
        ASSERT_EQ(data, str);
        ASSERT_EQ(3, tp.line(token_index));
        ASSERT_EQ(1, tp.column(token_index));
        ASSERT_EQ(9, tp.offset(token_index));
        ASSERT_EQ(real, tp.type(token_index));
    }
    {
        std::string                   data        = "word";
        default_token_index_type_size token_index = 3;
        tp.push(data.c_str(), word, 17);  // line 3 column 9
        ASSERT_EQ(4, tp.size());
        std::string str = tp.str(token_index);
        ASSERT_EQ(data, str);
        ASSERT_EQ(3, tp.line(token_index));
        ASSERT_EQ(9, tp.column(token_index));
        ASSERT_EQ(17, tp.offset(token_index));
        ASSERT_EQ(word, tp.type(token_index));
    }
    ASSERT_EQ(2, tp.line_count());
    ASSERT_EQ(4, tp.size());
    {  // pop line
        tp.pop_line();
        ASSERT_EQ(1, tp.line_count());
    }
    {  // pop token
        tp.pop_token();
        ASSERT_EQ(3, tp.size());
    }
    ASSERT_EQ(1, tp.line_count());
    ASSERT_EQ(7, tp.line_offset(0));
}

TEST(TokenPool, copy_test)
{
    TokenPool<char>          tp;
    std::vector<std::string> data       = {"ted", "234", "234.234", "word"};
    std::vector<char>        token_type = {word, integer, real, word};
    std::vector<default_file_offset_type_size> token_offsets = {0, 4, 9, 17};
    std::vector<std::size_t>                   token_line    = {1, 1, 3, 3};
    std::vector<std::size_t>                   token_column  = {1, 5, 1, 9};
    ASSERT_EQ(data.size(), token_type.size());
    ASSERT_EQ(data.size(), token_line.size());
    ASSERT_EQ(data.size(), token_column.size());
    ASSERT_EQ(data.size(), token_offsets.size());
    for (default_file_offset_type_size i = 0; i < data.size(); ++i)
    {
        tp.push(data[i].c_str(), token_type[i], token_offsets[i]);
    }
    std::vector<default_file_offset_type_size> line_offset = {7, 8};
    for (default_token_index_type_size i = 0; i < line_offset.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            tp.push_line(line_offset[i]);
            ASSERT_EQ(i + 1, tp.line_count());
            ASSERT_EQ(line_offset[i], tp.line_offset(i));
            ASSERT_EQ(4, tp.size());  // still only 4 tokens
        }
    }

    for (default_token_index_type_size token_index = 0;
         token_index < data.size(); ++token_index)
    {
        {
            SCOPED_TRACE(token_index);
            ASSERT_EQ(4, tp.size());
            std::string str = tp.str(token_index);
            ASSERT_EQ(data[token_index], str);
            ASSERT_EQ(token_line[token_index], tp.line(token_index));
            ASSERT_EQ(token_column[token_index], tp.column(token_index));
            ASSERT_EQ(token_offsets[token_index], tp.offset(token_index));
            ASSERT_EQ(token_type[token_index], tp.type(token_index));
        }
    }
    ASSERT_EQ(2, tp.line_count());
    ASSERT_EQ(4, tp.size());

    TokenPool<char> copy(tp);
    for (default_token_index_type_size i = 0; i < line_offset.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(2, copy.line_count());
            ASSERT_EQ(line_offset[i], copy.line_offset(i));
            ASSERT_EQ(4, copy.size());  // still only 4 tokens
        }
    }

    for (default_token_index_type_size token_index = 0;
         token_index < data.size(); ++token_index)
    {
        {
            SCOPED_TRACE(token_index);
            ASSERT_EQ(4, copy.size());
            std::string str = copy.str(token_index);
            ASSERT_EQ(data[token_index], str);
            ASSERT_EQ(token_line[token_index], copy.line(token_index));
            ASSERT_EQ(token_column[token_index], copy.column(token_index));
            ASSERT_EQ(token_offsets[token_index], copy.offset(token_index));
            ASSERT_EQ(token_type[token_index], copy.type(token_index));
        }
    }
    {  // pop line
        tp.pop_line();
        ASSERT_EQ(1, tp.line_count());
    }
    {  // pop token
        tp.pop_token();
        ASSERT_EQ(3, tp.size());
    }
    ASSERT_EQ(1, tp.line_count());
    ASSERT_EQ(7, tp.line_offset(0));
}

TEST(TokenPool, single_line_column)
{
    //'key =  3.142'
    std::vector<std::string> data   = {"key", "=", "3.142"};
    std::vector<std::size_t> column = {1, 5, 8};
    std::vector<std::size_t> offset = {0, 4, 7};
    std::vector<char>        type   = {word, assign, real};
    ASSERT_EQ(data.size(), column.size());
    ASSERT_EQ(data.size(), offset.size());
    ASSERT_EQ(data.size(), type.size());

    TokenPool<char> tp;
    for (std::size_t i = 0; i < data.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            tp.push(data[i].c_str(), type[i], offset[i]);  // line 1 column 1
            ASSERT_EQ(i + 1, tp.size());
            auto str = tp.str(i);
            ASSERT_EQ(data[i], str);
            ASSERT_EQ(1, tp.line(i));
            ASSERT_EQ(offset[i], tp.offset(i));
            ASSERT_EQ(column[i], tp.column(i));
            ASSERT_EQ(type[i], tp.type(i));

            // test the first token attributes to ensure additional
            // token inserts do not change the compute logic for line/column
            auto key = tp.str(0);
            ASSERT_EQ(data[0], key);
            ASSERT_EQ(1, tp.line(0));
            ASSERT_EQ(offset[0], tp.offset(0));
            ASSERT_EQ(column[0], tp.column(0));
            ASSERT_EQ(type[0], tp.type(0));
        }
    }
}

TEST(TokenPool, multiple_line_column)
{
    // token metadata
    struct Info
    {
        std::string data;
        char        type;
        std::size_t offset;
        std::size_t line;
        std::size_t column;
        std::size_t last_line;
        std::size_t last_column;

        Info(const std::string& d,
             char               t,
             size_t             o,
             size_t             l,
             size_t             c,
             size_t             ll,
             size_t             lc)
            : data(d)
            , type(t)
            , offset(o)
            , line(l)
            , column(c)
            , last_line(ll)
            , last_column(lc)
        {
        }
    };

    // key1 = "multi-line\n    string"
    // key2 = "another\nmulti-line\nstring"
    std::vector<std::vector<Info>> info = {
        {{"key1", word, 0, 1, 1, 1, 4},
         {"=", assign, 5, 1, 6, 1, 6},
         {"multi-line\n    string", word, 7, 1, 8, 2, 10}},
        {{"key2", word, 29, 3, 1, 3, 4},
         {"=", assign, 34, 3, 6, 3, 6},
         {"another\nmulti-line\nstring", word, 36, 3, 8, 5, 6}}};

    // token pool for testing
    TokenPool<char> tp;

    // token index
    std::size_t index = 0;

    for (std::size_t i = 0, ie = info.size(); i < ie; i++)
    {
        SCOPED_TRACE(i);
        const auto& line = info[i];

        for (std::size_t j = 0, je = line.size(); j < je; j++, index++)
        {
            SCOPED_TRACE(j);

            // current info
            const auto& ci = line[j];
            tp.push(ci.data.c_str(), ci.type, ci.offset);

            ASSERT_EQ(index + 1, tp.size());

            // verify token data are correct
            auto str = tp.str(index);
            ASSERT_EQ(ci.data, str);
            ASSERT_EQ(ci.type, tp.type(index));
            ASSERT_EQ(ci.offset, tp.offset(index));
            ASSERT_EQ(ci.line, tp.line(index));
            ASSERT_EQ(ci.column, tp.column(index));
            ASSERT_EQ(ci.last_line, tp.last_line(index));
            ASSERT_EQ(ci.last_column, tp.last_column(index));
        }

        tp.push_line(line.back().offset + line.back().data.size());
    }
}
