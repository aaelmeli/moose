#ifndef WASP_HALITEWORKFLOW_H
#define WASP_HALITEWORKFLOW_H

#include <fstream>

#include "waspcore/Object.h"
#include "waspcore/TokenPool.h"
#include "wasphalite/HaliteInterpreter.h"
#include "waspcore/decl.h"

namespace wasp
{
class WASP_PUBLIC HaliteWorkflow
{
  private:
    bool m_use_default_variables = false;
    bool m_use_default_functions = false;
    bool m_template_parse_result = false;

    typedef DefaultHaliteInterpreter halite_interp_type;
    halite_interp_type::SharedPtr    m_halite_interpreter;

  public:
    bool useDefaultVariables() const;
    void setUseDefaultVariables(bool use_default_functions);

    bool useDefaultFunctions() const;
    void setUseDefaultFunctions(bool use_default_functions);

    bool parseTemplate(std::istream& template_stream,
                       std::ostream& error_stream);
    bool parseTemplateFile(const std::string& template_file,
                           std::ostream&      error_stream);

    bool renderTemplate(const DataObject::SP& parameters,
                        std::ostream&         output_stream,
                        std::ostream&         message_stream) const;
};  // class

}  // namespace
#endif /** WASP_HALITEWORKFLOW_H */
