#ifndef WASP_DAKOTA_PARAMETERSFILE_H
#define WASP_DAKOTA_PARAMETERSFILE_H

#include "waspcore/decl.h"
#include "waspcore/Object.h"
#include "wasphalite/HaliteInterpreter.h"

#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace wasp{

/**
 * @brief The ParametersFile class
 * This is a class that encapsulates the standard Dakota parameter file format
 *
 * This format is composed of 'blocks' of data
 * indicating variables, functions, derived variables, and analysis compnent
 * labels and values, lastly the dakota evaluation identifier
 *
 *  <int> 'variables'
 *  <double> <label_cdv_i> (i=1 to n_cdv)
 *  <int>    <label_ddiv_i> (i=1 to n_ddiv)
 *  <string> <label_ddsv_i> (i=1 to n_ddsv)
 *  <double> <label_ddrv_i> (i=1 to n_ddrv)
 *  <double> <label_cauv_i> (i=1 to n_cauv)
 *  <int>    <label_dauiv_i> (i=1 to n_dauiv)
 *  <string> <label_dausv_i> (i=1 to n_dausv)
 *  <double> <label_daurv_i> (i=1 to n_daurv)
 *  <double> <label_ceuv_i> (i=1 to n_ceuv)
 *  <int>    <label_deuiv_i> (i=1 to n_deuiv)
 *  <string> <label_deusv_i> (i=1 to n_deusv)
 *  <double> <label_deurv_i> (i=1 to n_deurv)
 *  <double> <label_csv_i> (i=1 to n_csv)
 *  <int>    <label_dsiv_i> (i=1 to n_dsiv)
 *  <string> <label_dssv_i> (i=1 to n_dssv)
 *  <double> <label_dsrv_i> (i=1 to n_dsrv)
 *  <int> 'functions'
 *  <int>  'ASV_'<i>':'<label_response_i> (i=1 to m)
 *  <int> 'derived_variables'
 *  <int> 'DVV_'<i>':'<label_cdv_i> (i=1 to p)
 *  <int> 'analysis_components'
 *  <string> 'AC_'<i>':'<analysis_driver_name_i> (i=1 to q)
 *  <string> eval_id
 *
 */
class WASP_PUBLIC ParametersFile
{
public:
    ParametersFile();
    ParametersFile(const ParametersFile& orig);

    /**
     * @brief load the parameter file with the given stream
     * @param input the stream from which to parse
     * @param errors the stream on which errors will be reported
     * @return
     */
    bool load(std::istream& input, std::ostream& errors);

private:

    struct Block{
        Block();
        Block(const Block& orig);
        Block& operator= ( const Block & o)
        {
            count = o.count;
            name = o.name;
            label_values = o.label_values;
            return *this;
        }
        int count;
        std::string name;
        std::vector<std::pair<std::string, std::string>> label_values;
    };

    static bool load_block(Block& b, std::istream& input,
                              std::ostream& error, int& line);

    /**
     * @brief add adds the given label and value to block
     * @param b the block
     * @param label the label
     * @param value the value (in string form)
     */
    static void add(Block& b, const std::string& label,
                    const std::string& value);
    /**
     * @brief remove_prefix removes the lead text including the first ':'
     * @param b the block on which the labels will have their prefix removed
     */
    static void remove_prefix(Block& b);

    Block m_variable;
    Block m_function;
    Block m_derivative_variable;
    Block m_analysis_component;

    std::string m_eval_id;

public:
    size_t variable_count() const { return m_variable.label_values.size();}
    std::string variable_label(size_t i) const;
    std::string variable_value(size_t i) const;

    size_t function_count() const { return m_function.label_values.size();}
    std::string function_label(size_t i) const;
    std::string function_value(size_t i) const;

    size_t derivative_variable_count() const { return m_derivative_variable.label_values.size();}
    std::string derivative_variable_label(size_t i) const;
    std::string derivative_variable_value(size_t i) const;

    size_t analysis_component_count() const { return m_analysis_component.label_values.size();}
    std::string analysis_component_label(size_t i) const;
    std::string analysis_component_value(size_t i) const;

    std::string eval_id() const {return m_eval_id;}


}; // end of ParametersFile

inline void insert_typed(DataObject::SP& obj_ptr, const std::string& label,
                    const std::string& value)
{
    bool ok = false;

    double d;
    wasp::to_type<double>(d,value, &ok);
    if (ok)
    {
        obj_ptr->insert(std::make_pair(label,d));
        return;
    }
    int i;
    wasp::to_type<int>(i,value, &ok);
    if (ok)
    {
        obj_ptr->insert(std::make_pair(label,i));
        return;
    }

    obj_ptr->insert(std::make_pair(label, value));
}

/**
 * @brief substitute_template convenience function for template substition
 * @param result the substituted template
 * @param elog the error log
 * @param alog the activity log
 * @param template_file the template path
 * @param parameter_file the parameter path
 * @param defaultVars use default variables (e, pi, etc.)
 * @param defaultFuncs use default functions (cos, sin, etc.)
 * @param ldelim the opening/left attribute delimiter
 * @param rdelim the closing/right attribute delimiter
 * @param hop the hierarchical operator ('.', etc.)
 * @return true if the template had no issues in being substituted
 */
inline WASP_PUBLIC bool substitute_template(std::ostream&      result,
                                        std::ostream&      elog,
                                        std::ostream&      alog,
                                        const std::string& template_file,
                                        const std::string& parameter_file,
                                        bool               defaultVars,
                                        bool               defaultFuncs,
                                        const std::string& ldelim,
                                        const std::string& rdelim,
                                        const std::string& hop)
{
    HaliteInterpreter<
        TreeNodePool<unsigned int, unsigned int,
                     TokenPool<unsigned int, unsigned int, unsigned int>>>
        halite(elog);
    halite.attr_start_delim() = ldelim;
    halite.attr_end_delim() = rdelim;

    bool tmpl_failed = !halite.parseFile(template_file);
    if (tmpl_failed)
    {
        elog << "***Error : Parsing of template " << template_file << " failed!"
             << std::endl;
        return false;
    }
    // No parameter file, expand template
    if (parameter_file.empty())
    {
        DataObject   o;
        DataAccessor data(&o, nullptr, hop);
        if (defaultVars)
            data.add_default_variables();
        if (defaultFuncs)
            data.add_default_functions();
        bool emitted = halite.evaluate(result, data, &alog);

        if (!emitted)
        {
            return false;
        }
        return true;
    }

    // process param file
    std::ifstream param_file_stream(parameter_file);
    if (!param_file_stream)
    {
        elog << "***Error : Parameter file could not be opened! "
             <<parameter_file<< std::endl;
        return false;
    }
    DataObject::SP obj_ptr = std::make_shared<DataObject>();
    ParametersFile parameters;
    bool           param_failed = !parameters.load(param_file_stream,elog);
    if (param_failed)
    {
        elog << "***Error : failed to load " << parameter_file
             <<"!"<<std::endl;
        return false;
    }

    // add variables to DataObject
    for( size_t i = 0; i < parameters.variable_count(); ++i)
    {
        const auto& label = parameters.variable_label(i);
        const auto& value = parameters.variable_value(i);
        insert_typed(obj_ptr,label, value);
    }
    // add functions to DataObject
    for( size_t i = 0; i < parameters.function_count(); ++i)
    {
        const auto& label = parameters.function_label(i);
        const auto& value = parameters.function_value(i);
        insert_typed(obj_ptr,label, value);
    }
    // add derivative variables
    for( size_t i = 0; i < parameters.derivative_variable_count(); ++i)
    {
        const auto& label = parameters.derivative_variable_label(i);
        const auto& value = parameters.derivative_variable_value(i);
        insert_typed(obj_ptr,label, value);
    }
    // add analysis components
    for( size_t i = 0; i < parameters.analysis_component_count(); ++i)
    {
        const auto& label = parameters.analysis_component_label(i);
        const auto& value = parameters.analysis_component_value(i);
        insert_typed(obj_ptr,label, value);
    }

    DataAccessor data(obj_ptr.get(), nullptr, hop);
    if (defaultVars)
        data.add_default_variables();
    if (defaultFuncs)
        data.add_default_functions();

    bool emitted = halite.evaluate(result, data, &alog);

    if (!emitted)
    {
        return false;
    }
    return true;
} // end of substitute_template
} // end namespace
#endif
