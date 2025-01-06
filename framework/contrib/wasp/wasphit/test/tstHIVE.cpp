#include "waspson/SONInterpreter.h"
#include "waspson/SONNodeView.h"
#include "wasphive/HIVE.h"
#include "waspcore/TreeNodePool.h"
#include "waspcore/utils.h"
#include "wasphit/HITInterpreter.h"
#include "wasphit/HITNodeView.h"
#include "gtest/gtest.h"
#include <string>
#include <iostream>
#include <sstream>
#include <memory>

#include "wasphit/test/Paths.h"

std::string test_dir = TEST_DIR_ROOT;

using namespace wasp;

struct HITHIVETest
{
    std::string input_fail_path;
    std::shared_ptr<std::ifstream>
                                              input_fail;  // test failing input for schema to exercise
    std::shared_ptr<DefaultHITInterpreter> input_fail_interpreter;

    std::string input_pass_path;
    std::shared_ptr<std::ifstream>
                                              input_pass;  // test passing input for schema to exercise
    std::shared_ptr<DefaultHITInterpreter> input_pass_interpreter;

    std::string                            schema_path;
    std::shared_ptr<std::ifstream>         schema;  // schema to validate input
    std::shared_ptr<DefaultSONInterpreter> schema_interpreter;

    std::shared_ptr<std::stringstream> output_data;  // expected output
};

bool load_file_as_string(const std::string&                  file_path,
                         std::shared_ptr<std::stringstream>& s)
{
    return load_file(file_path, *s.get());
}

bool load_streams(HITHIVETest&    t,
                  const std::string& fname,
                  const std::string& pname,
                  const std::string& oname,
                  const std::string& sname)
{
    t.input_fail_path = test_dir + "/inputs/" + fname;
    t.input_fail      = std::make_shared<std::ifstream>(t.input_fail_path);
    bool file_bad     = t.input_fail->bad() || t.input_fail->fail();
    std::cout << " -Loaded fail input :: " << t.input_fail_path << std::endl;
    EXPECT_FALSE(file_bad);

    t.input_pass_path = test_dir + "/inputs/" + pname;
    t.input_pass      = std::make_shared<std::ifstream>(t.input_pass_path);
    file_bad          = t.input_pass->bad() || t.input_pass->fail();
    std::cout << " -Loaded pass input :: " << t.input_pass_path << std::endl;
    EXPECT_FALSE(file_bad);

    {
        std::string output_path = test_dir + "/outputs/" + oname;
        SCOPED_TRACE(output_path);
        t.output_data = std::make_shared<std::stringstream>();
        EXPECT_TRUE(load_file_as_string(output_path, t.output_data));
        file_bad = t.output_data->bad() || t.output_data->fail();
        std::cout << " -Loaded output (gold) :: " << output_path << std::endl;
        EXPECT_FALSE(file_bad);
    }
    t.schema_path = test_dir + "/schemas/" + sname;
    t.schema      = std::make_shared<std::ifstream>(t.schema_path);
    file_bad      = t.schema->bad() || t.schema->fail();
    std::cout << " -Loaded schema :: " << t.schema_path << std::endl;
    EXPECT_FALSE(file_bad);
    return !file_bad;
}
// load abstract syntax trees (dom)
bool load_ast(HITHIVETest& t)
{
    t.input_fail_interpreter = std::make_shared<DefaultHITInterpreter>();
    bool input_fail_good     = t.input_fail_interpreter->parseStream(*t.input_fail, t.input_fail_path);

    EXPECT_TRUE(input_fail_good);

    t.input_pass_interpreter = std::make_shared<DefaultHITInterpreter>();
    bool input_pass_good     = t.input_pass_interpreter->parseStream(*t.input_pass, t.input_pass_path);
    EXPECT_TRUE(input_pass_good);

    t.schema_interpreter = std::make_shared<DefaultSONInterpreter>();
    bool schema_good =
        t.schema_interpreter->parseStream(*t.schema, t.schema_path);
    EXPECT_TRUE(schema_good);

    return schema_good && input_fail_good && input_pass_good;
}

// Schema is defaulted to test name
void do_test(const std::string& name, std::string sch="")
{
    SCOPED_TRACE(name);
    if (sch.empty()) sch = name;
    HIVE           hive;
    HITHIVETest t;
    ASSERT_TRUE(load_streams(t, name + ".fail.gp", name + ".pass.gp",
                             name + ".fail.gld", sch + ".sch"));
    ASSERT_TRUE(load_ast(t));
    std::vector<std::string> errors;
    SONNodeView              schema_adapter = t.schema_interpreter->root();
    HITNodeView input_fail_adapter       = t.input_fail_interpreter->root();
    HITNodeView input_pass_adapter       = t.input_pass_interpreter->root();
    bool valid = hive.validate(schema_adapter, input_fail_adapter, errors);
    std::string msgs = HIVE::combine(errors);
    EXPECT_FALSE(valid);
    ASSERT_EQ(t.output_data->str(), msgs);
    errors.clear();
    valid = hive.validate(schema_adapter, input_pass_adapter, errors);
    msgs  = HIVE::combine(errors);
    if (!valid)
        std::cout << msgs << std::endl;
    EXPECT_TRUE(valid);
}

TEST(HITHIVE, MinOccurs)
{
    do_test("MinOccurs");
}
TEST(HITHIVE, MaxOccurs)
{
    do_test("MaxOccurs");
}

TEST(HITHIVE, ValEnums)
{
    do_test("ValEnums");
}

TEST(HITHIVE, ValType)
{
    do_test("ValType");
}
TEST(HITHIVE, MinValInc)
{
    do_test("MinValInc");
}
TEST(HITHIVE, MinValExc)
{
    do_test("MinValExc");
}
TEST(HITHIVE, MaxValInc)
{
    do_test("MaxValInc");
}
TEST(HITHIVE, MaxValExc)
{
    do_test("MaxValExc");
}
TEST(HITHIVE, ChildAtLeastOne)
{
    do_test("ChildAtLeastOne");
}
TEST(HITHIVE, ChildAtMostOne)
{
    do_test("ChildAtMostOne");
}
TEST(HITHIVE, ChildCountEqual)
{
    do_test("ChildCountEqual");
}
TEST(HITHIVE, ChildExactlyOne)
{
    do_test("ChildExactlyOne");
}
TEST(HITHIVE, ChildUniqueness)
{
    do_test("ChildUniqueness");
}
TEST(HITHIVE, DecreaseOver)
{
    do_test("DecreaseOver");
}
TEST(HITHIVE, ExistsIn)
{
    do_test("ExistsIn");
}
TEST(HITHIVE, Extras)
{
    do_test("Extras");
}
TEST(HITHIVE, IncreaseOver)
{
    do_test("IncreaseOver");
}
TEST(HITHIVE, NotExistsIn)
{
    do_test("NotExistsIn");
}
TEST(HITHIVE, SumOver)
{
    do_test("SumOver");
}
TEST(HITHIVE, SumOverGroup)
{
    do_test("SumOverGroup");
}
TEST(HITHIVE, validation_with_includes)
{
    SCOPED_TRACE("ChildAtLeastOneInclude");
    do_test("ChildAtLeastOneInclude", "ChildAtLeastOne");
}