#include "waspcore/wasp_bug.h"
#include "wasphalite/HaliteWorkflow.h"

#ifdef WASP_DEBUG_OUTPUT
#include <iomanip>
#endif

namespace wasp
{
bool HaliteWorkflow::useDefaultVariables() const
{
    return m_use_default_variables;
}

void HaliteWorkflow::setUseDefaultVariables(bool use_default_variables)
{
    m_use_default_variables = use_default_variables;
}
bool HaliteWorkflow::useDefaultFunctions() const
{
    return m_use_default_functions;
}

void HaliteWorkflow::setUseDefaultFunctions(bool use_default_functions)
{
    m_use_default_functions = use_default_functions;
}

bool HaliteWorkflow::parseTemplate(std::istream& template_stream,
                                   std::ostream& error_stream)
{
    wasp_tagged_line("parsing template stream...");
    m_halite_interpreter = std::make_shared<halite_interp_type>(error_stream);

    m_template_parse_result =
        m_halite_interpreter->parseStream(template_stream);
    wasp_tagged_line("\ttemplate parse result " << std::boolalpha
                                                << m_template_parse_result);
    return m_template_parse_result;
}
bool HaliteWorkflow::parseTemplateFile(const std::string& template_file,
                                       std::ostream&      error_stream)
{
    wasp_tagged_line("parsing template stream...");
    m_halite_interpreter = std::make_shared<halite_interp_type>(error_stream);

    m_template_parse_result = m_halite_interpreter->parseFile(template_file);
    wasp_tagged_line("\ttemplate parse result " << std::boolalpha
                                                << m_template_parse_result);
    return m_template_parse_result;
}

bool HaliteWorkflow::renderTemplate(const DataObject::SP& parameters,
                                    std::ostream&         output_stream,
                                    std::ostream&         message_stream) const
{
    wasp_tagged_line("rendering template...");
    wasp_tagged_line("\ttemplate parse result " << std::boolalpha
                                                << m_template_parse_result);
    wasp_check(m_template_parse_result);
    wasp_check(m_halite_interpreter);

    bool         result = false;
    DataAccessor accessor(parameters.get());
    // Provide for the use of cos, sin, tan... functions in this template
    if (m_use_default_functions)
        accessor.add_default_functions();
    // Provide for the use of reference constants like PI in this template
    if (m_use_default_variables)
        accessor.add_default_variables();

    result = m_halite_interpreter->evaluate(output_stream, accessor,
                                            &message_stream);
    wasp_tagged_line("\ttemplate render result " << std::boolalpha << result);
    return result;
}

}  // namespace
