#include "gtest/gtest.h"
#include <string>
#include <iostream>
#include <sstream>
#include <memory>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <fstream>
#include "waspson/SONInterpreter.h"
#include "waspson/SONNodeView.h"
#include "waspcore/utils.h"
#include "waspcore/version.h"
#include "wasphive/InputDefinition.h"
#include "wasphive/test/Paths.h"

// This macro prints better compare text when a test fails than using EXPECT_EQ
#define EXPECT_OUTPUT(EXPECTED,ACTUAL) EXPECT_TRUE(EXPECTED==ACTUAL) << \
      "\nExpected Output:\n" << "*********************" << EXPECTED << "*********************\n" \
      "\nActual Output:\n" << "*********************" << ACTUAL << "*********************\n"

std::string test_dir = TEST_AUTODOC_ROOT;
using namespace wasp;

struct AutoDocTest
{
    std::string                            schema_path;
    std::shared_ptr<std::ifstream>         schema;
    std::shared_ptr<DefaultSONInterpreter> schema_interpreter;
    std::shared_ptr<std::stringstream>     output_data;
};

bool load_file_as_string(const std::string& file_path,
                         std::shared_ptr<std::stringstream>& s)
{
    return load_file(file_path, *s.get());
}

bool load_streams(AutoDocTest&          t,
                  const std::string& oname,
                  const std::string& sname)
{
    {
        std::string output_path = test_dir + "/outputs/" + oname;
        SCOPED_TRACE(output_path);
        t.output_data = std::make_shared<std::stringstream>();
        EXPECT_TRUE(load_file_as_string(output_path, t.output_data));
        bool file_bad = t.output_data->bad() || t.output_data->fail();
        std::cout << " -Loaded output (gold) :: " << output_path << std::endl;
        EXPECT_FALSE(file_bad);
    }
    t.schema_path = test_dir + "/schemas/" + sname;
    t.schema      = std::make_shared<std::ifstream>(t.schema_path);
    bool file_bad      = t.schema->bad() || t.schema->fail();
    std::cout << " -Loaded schema :: " << t.schema_path << std::endl;
    EXPECT_FALSE(file_bad);
    return !file_bad;
}
bool load_ast(AutoDocTest& t)
{
    t.schema_interpreter = std::make_shared<DefaultSONInterpreter>();
    bool schema = t.schema_interpreter->parseStream(*t.schema, t.schema_path);
    EXPECT_TRUE(schema);
    return schema;
}

void do_test(const std::string& name)
{
    SCOPED_TRACE(name);
    AutoDocTest t;
    ASSERT_TRUE(load_streams(t, name + ".gld", name + ".sch"));
    ASSERT_TRUE(load_ast(t));

    std::stringstream output;
    std::stringstream errors;
    SONNodeView schema_adapter = t.schema_interpreter->root();
    InputDefinition inpdef(schema_adapter, output, errors);

    inpdef.printMarkdownAll();

    EXPECT_TRUE(errors.str().size() == 0);
    EXPECT_OUTPUT(t.output_data->str(), output.str());
}

TEST(HIVE, test_001)
{
    do_test("test_001");
}
