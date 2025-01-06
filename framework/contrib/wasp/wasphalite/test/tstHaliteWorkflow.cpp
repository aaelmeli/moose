/*
 * File:   tstHalite.cpp
 * Author: Robert A. Lefebvre
 */

#include <cstdlib>
#include <cstdio>  // std::remove(file_name)
#include <sstream>
#include <string>
#include <stdexcept>
#include <functional>
#include "waspcore/Object.h"
#include "wasphalite/HaliteWorkflow.h"

#include "wasphalite/test/Paths.h"

#include "gtest/gtest.h"

using namespace std;
using namespace wasp;

TEST(Halite, Workflow)
{
    {
        std::stringstream template_stream;
        template_stream << R"INPUT(<attribute>)INPUT";
        std::stringstream output_stream;
        DataObject::SP    parameters = std::make_shared<DataObject>();
        DataObject&       ref        = *(parameters.get());
        ref["attribute"]             = "value";

        HaliteWorkflow workflow;
        bool           template_result =
            workflow.parseTemplate(template_stream, std::cerr);
        ASSERT_TRUE(template_result);
        bool render_result =
            workflow.renderTemplate(parameters, output_stream, std::cerr);
        ASSERT_TRUE(render_result);
        ASSERT_EQ(output_stream.str().compare("value"), 0);
    }
    {  // test using default variables PI
        std::stringstream template_stream;
        template_stream << R"INPUT(<'attribute'*pi>)INPUT";
        std::stringstream output_stream;
        DataObject::SP    parameters = std::make_shared<DataObject>();
        DataObject&       ref        = *(parameters.get());
        ref["attribute"]             = 1.;

        HaliteWorkflow workflow;
        bool           template_result =
            workflow.parseTemplate(template_stream, std::cerr);
        ASSERT_TRUE(template_result);
        workflow.setUseDefaultVariables(true);
        bool render_result =
            workflow.renderTemplate(parameters, output_stream, std::cerr);
        ASSERT_TRUE(render_result);
        wasp_line("Result '" << output_stream.str() << "'");
        ASSERT_EQ(output_stream.str().compare("3.14159"), 0);
    }
    {  // test using default variables E
        std::stringstream template_stream;
        template_stream << R"INPUT(<'attribute'*e>)INPUT";
        std::stringstream output_stream;
        DataObject::SP    parameters = std::make_shared<DataObject>();
        DataObject&       ref        = *(parameters.get());
        ref["attribute"]             = 1.;

        HaliteWorkflow workflow;
        bool           template_result =
            workflow.parseTemplate(template_stream, std::cerr);
        ASSERT_TRUE(template_result);
        workflow.setUseDefaultVariables(true);
        bool render_result =
            workflow.renderTemplate(parameters, output_stream, std::cerr);
        ASSERT_TRUE(render_result);
        wasp_line("Result '" << output_stream.str() << "'");
        ASSERT_EQ(output_stream.str().compare("2.71828"), 0);
    }
    {  // test using default function cos(0) is 1
        std::stringstream template_stream;
        template_stream << R"INPUT(<cos('attribute')>)INPUT";
        std::stringstream output_stream;
        DataObject::SP    parameters = std::make_shared<DataObject>();
        DataObject&       ref        = *(parameters.get());
        ref["attribute"]             = 0.;

        HaliteWorkflow workflow;
        bool           template_result =
            workflow.parseTemplate(template_stream, std::cerr);
        ASSERT_TRUE(template_result);
        workflow.setUseDefaultFunctions(true);
        bool render_result =
            workflow.renderTemplate(parameters, output_stream, std::cerr);
        ASSERT_TRUE(render_result);
        wasp_line("Result '" << output_stream.str() << "'");
        ASSERT_EQ(output_stream.str().compare("1"), 0);
    }
}

TEST(Halite, ParseFile)
{
    {  // test using the #if with emissions
        std::string template_file = wasp::dir_name(SOURCE_DIR + "/") +
                                    "/data/111-if-newline-emission.tmpl";

        std::stringstream output_stream;
        DataObject::SP    parameters = std::make_shared<DataObject>();
        DataObject&       ref        = *(parameters.get());

        std::vector<double> array = {1., 2.};
        DataObject          child;
        DataArray           array_obj;
        array_obj.resize(array.size());
        for (size_t i = 0; i < array.size(); ++i)
        {
            array_obj[i] = array[i];
        }
        child["me"] = array_obj;
        DataArray parent_array_obj;
        parent_array_obj.push_back(child);
        parent_array_obj.push_back(child);
        ref["foo"]          = 1;
        ref["parent array"] = parent_array_obj;

        HaliteWorkflow workflow;
        bool           template_result =
            workflow.parseTemplateFile(template_file, std::cerr);
        ASSERT_TRUE(template_result);
        bool render_result =
            workflow.renderTemplate(parameters, output_stream, std::cerr);
        ASSERT_TRUE(render_result);
        std::stringstream blessed_ss;
        blessed_ss << "Line 1" << std::endl
                   << "1.000     2.000     " << std::endl
                   << "1.000     2.000     " << std::endl
                   << "Line 2" << std::endl;
        EXPECT_EQ(blessed_ss.str(), output_stream.str());
        std::cout << output_stream.str() << std::endl;
    }
    {  // test using the #if w/o emissions
        std::string template_file = wasp::dir_name(SOURCE_DIR + "/") +
                                    "/data/111-if-newline-emission.tmpl";

        std::stringstream output_stream;
        DataObject::SP    parameters = std::make_shared<DataObject>();
        DataObject&       ref        = *(parameters.get());

        std::vector<double> array = {1., 2.};
        DataObject          child;
        DataArray           array_obj;
        array_obj.resize(array.size());
        for (size_t i = 0; i < array.size(); ++i)
        {
            array_obj[i] = array[i];
        }
        child["me"] = array_obj;
        DataArray parent_array_obj;
        parent_array_obj.push_back(child);
        parent_array_obj.push_back(child);
        ref["foo"]          = 0;
        ref["parent array"] = parent_array_obj;

        HaliteWorkflow workflow;
        bool           template_result =
            workflow.parseTemplateFile(template_file, std::cerr);
        ASSERT_TRUE(template_result);
        bool render_result =
            workflow.renderTemplate(parameters, output_stream, std::cerr);
        ASSERT_TRUE(render_result);
        std::stringstream blessed_ss;
        blessed_ss << "Line 1" << std::endl << "Line 2" << std::endl;
        EXPECT_EQ(blessed_ss.str(), output_stream.str());
        std::cout << output_stream.str() << std::endl;
    }
}
