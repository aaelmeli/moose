#include "waspdakota/ParametersFile.h"

#include "waspcore/wasp_bug.h"

using namespace wasp;

ParametersFile::ParametersFile(){}
ParametersFile::ParametersFile(const ParametersFile &orig)
{

}


bool ParametersFile::load(std::istream &input, std::ostream &errors)
{
    int variable_count = 0;
    int line = 1;
    input >> variable_count;

    if (input.fail())
    {
        errors << "***Error: line "<<line
               <<", unable to retrieve the number of variables!"<<std::endl;
        return false;
    }

    std::string variables_label;
    input >> variables_label;

    if (input.fail())
    {
        errors << "***Error: line "<<line
               <<", unable to retrieve the variables label!"<<std::endl;
        return false;
    }
    if (variables_label != "variables")
    {
        errors << "***Error: line "<<line
               <<", unable to retrieve the variables label, found '"
              <<variables_label<<"' instead"<<std::endl;
        return false;
    }
    if (variable_count < 0 )
    {
        errors <<"***Error: line "<<line
              <<", variable count must be non-negative, but was provided as "<<variable_count
             <<std::endl;
        return false;
    }
    m_variable.count= variable_count;
    m_variable.name = variables_label;
    ++line;
    if ( !load_block(m_variable, input, errors, line) )
    {
        errors << "***Error: line "<<line
               <<", unable to process "
              <<m_variable.name<<" parameters due to an unknown issue."
             <<std::endl;
    }

    while (input.good())
    {
        std::string value, label;

        // This is either another block (functions) or the eval id
        input >> value >> label;

        if (input.fail())
        {
            errors << "***Error: line "<<line
                   <<", unable to retrieve the label!"<<std::endl;
            return false;
        }

        // the evaluation id is the concluding file component
        if (label == "eval_id")
        {
            m_eval_id = value;
            break;
        }

        int block_line = line;

        Block m_unknown;
        m_unknown.count = std::stoi(value);
        m_unknown.name = label;

        if (!load_block(m_unknown,input, errors, line))
        {
            errors << "***Error: line "<<line
                   <<", unable to process "
                  <<m_unknown.name<<" parameters due to an unknown issue."
                 <<std::endl;
        }

        if ("functions" == m_unknown.name)
        {
            m_function = m_unknown;
            // Remove 'ASV_<I>:'
            remove_prefix(m_function);
        }
        else if ("derivative_variables" == m_unknown.name)
        {
            m_derivative_variable = m_unknown;
            // Remove 'DVV_<I>:'
            remove_prefix(m_derivative_variable);
        }
        else if ("analysis_components" == m_unknown.name)
        {
            m_analysis_component = m_unknown;
            // Remove 'ASV_<I>:'
            remove_prefix(m_analysis_component);
        }
        else
        {
            errors << "***Error: line "<<block_line
                   <<", "<<m_unknown.name<<" is an unknown parameter block!"
                  <<std::endl<<"\t Expecting "
                 <<"functions, derivative_variables, or analysis_components."
                 <<std::endl;
            return false;
        }
    } // end of while(input.good())

    wasp_ensure(m_variable.count == static_cast<int>(m_variable.label_values.size()));
    wasp_ensure(m_function.count == static_cast<int>(m_function.label_values.size()));
    wasp_ensure(m_derivative_variable.count == static_cast<int>(m_derivative_variable.label_values.size()));
    wasp_ensure(m_analysis_component.count == static_cast<int>(m_analysis_component.label_values.size()));
    return true;
}

bool ParametersFile::load_block(Block& b, std::istream& input,
                                std::ostream& error, int& line)
{
    for (int i_variable = 0; i_variable < b.count; ++i_variable)
    {
        ++line;
        std::string value;
        std::string label;

        input >> value >> label;
        add(b, label, value);
    }
    return true;
}
void ParametersFile::add(Block& b, const std::string &label,
                                  const std::string &value)
{
    b.label_values.push_back({label,value});
}

void ParametersFile::remove_prefix(Block &b)
{
    for( auto& p : b.label_values)
    {
        // Label is first element
        size_t index = p.first.find(":");
        if (index != std::string::npos)
        {
            p.first.erase(0,index+1);
        }
    }
}

std::string ParametersFile::variable_label(size_t i) const
{
    wasp_require(i < m_variable.label_values.size());
    return m_variable.label_values[i].first;
}
std::string ParametersFile::variable_value(size_t i) const
{
    wasp_require(i < m_variable.label_values.size());
    return m_variable.label_values[i].second;
}

std::string ParametersFile::function_label(size_t i) const
{
    wasp_require(i < m_function.label_values.size());
    return m_function.label_values[i].first;
}
std::string ParametersFile::function_value(size_t i) const
{
    wasp_require(i < m_function.label_values.size());
    return m_function.label_values[i].second;
}

std::string ParametersFile::derivative_variable_label(size_t i) const
{
    wasp_require(i < m_derivative_variable.label_values.size());
    return m_derivative_variable.label_values[i].first;
}
std::string ParametersFile::derivative_variable_value(size_t i) const
{
    wasp_require(i < m_derivative_variable.label_values.size());
    return m_derivative_variable.label_values[i].second;
}

std::string ParametersFile::analysis_component_label(size_t i) const
{
    wasp_require(i < m_analysis_component.label_values.size());
    return m_analysis_component.label_values[i].first;
}
std::string ParametersFile::analysis_component_value(size_t i) const
{
    wasp_require(i < m_analysis_component.label_values.size());
    return m_analysis_component.label_values[i].second;
}
ParametersFile::Block::Block() : count(0){}
ParametersFile::Block::Block(const Block &orig) : count(orig.count),
    name(orig.name), label_values(orig.label_values)
{}



