#include "waspson/SONInterpreter.h"
#include "wasphive/HIVE.h"
#include "waspcore/TreeNodePool.h"
#include "waspcore/utils.h"
#include "waspson/SONNodeView.h"
#include "gtest/gtest.h"
#include <string>
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>

#include "wasphive/test/Paths.h"

std::string test_dir = TEST_DIR_ROOT;

using namespace wasp;

TEST(HIVE, definition_strided)
{
    std::string schema_path = test_dir + "/schemas/Definition.sch";

    auto schema        = std::make_shared<std::ifstream>(schema_path);
    bool file_bad      = schema->bad() || schema->fail();
    
    EXPECT_FALSE(file_bad);

    auto schema_interpreter = std::make_shared<DefaultSONInterpreter>();
    bool schema_good = schema_interpreter->parseStream(*schema.get(), schema_path);

    std::cout << " -Loaded schema :: " << schema_path << std::endl;
    
    EXPECT_TRUE(schema_good);
    SONNodeView schema_root = schema_interpreter->root();
    std::stringstream definition_errors;
    Definition::SP    definition = std::make_shared<Definition>();
    bool definition_created = HIVE::create_definition(definition.get(), 
                                                        schema_root,
                                                        definition_errors, true);
    std::cout << definition_errors.str();
    ASSERT_TRUE(definition_created);
    ASSERT_TRUE(definition->has("a"));
    auto a = definition->get("a");
    std::vector<std::pair<int, std::string>> aliased_parts = {
                                                            {0, "x"},
                                                            {1, "y"}, 
                                                            {2, "z"},
                                                            {3, "x"},
                                                            {4, "y"},
                                                            {5, "z"},
                                                            {6, "i"},
                                                            {7, "j"}, 
                                                            {8, "k"},
                                                            {9, "i"},
                                                            {10, "j"},
                                                            {11, "k"}
                                                            };

    for (auto part : aliased_parts)
    {
        SCOPED_TRACE(part.first);
        auto part_def = a->get(part.first);
        ASSERT_TRUE(part_def != nullptr);
        EXPECT_EQ(part.second, part_def->name());
    }
}
