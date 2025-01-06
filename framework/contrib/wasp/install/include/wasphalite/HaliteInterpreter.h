
#ifndef WASP_HALITEINTERPRETER_H
#define WASP_HALITEINTERPRETER_H

#include <cstdint>
#include <set>
#include <string>
#include <fstream>
#include <istream>
#include <ostream>
#include <iostream>

#include "waspcore/Interpreter.h"
#include "wasphalite/SubStringIndexer.h"
#include "waspexpr/ExprInterpreter.h"
#include "waspjson/JSONObjectParser.hpp"
#include "wasphalite/DataAccessor.h"
#include "waspcore/wasp_bug.h"
#include "waspcore/decl.h"

namespace wasp
{
/**
 * @brief expand_template convenience function for expanding a template with an
 * optional data set
 * @param result the output stream to capture the resulting expansion
 * @param elog the stream to report error messages on
 * @param alog the stream to report expansion activity on
 * @param template_file the template file to expand
 * @param json_parameter_file the optional data to to drive expansion
 * @param defaultVars include default variables (e, pi, nl,...)
 * @param defaultFuncs include default functions (sin, cos, tang,...)
 * @return true, iff the template expansion functions
 */
WASP_PUBLIC bool expand_template(std::ostream&      result,
                                 std::ostream&      elog,
                                 std::ostream&      alog,
                                 const std::string& template_file,
                                 const std::string& json_parameter_file = "",
                                 bool               defaultVars         = false,
                                 bool               defaultFuncs = false,
                                 const std::string& ldelim="<",
                                 const std::string& rdelim=">",
                                 const std::string& hop=".");

// How many input node type's (section, value, etc.) in a Halite file
typedef std::uint8_t HaliteNodeType_t;

// How many input token type's (word, int, real, comma, etc.) in a Halite file
typedef std::uint8_t HaliteTokenType_t;

// How many bytes in a file
typedef std::uint8_t  HaliteTinyFileSize_t;
typedef std::uint16_t HaliteMediumFileSize_t;
typedef std::uint32_t HaliteFileSize_t;
typedef std::uint64_t HaliteLargeFileSize_t;

// How many tokens in a file (5 reals, 100 ints, 50 words, etc.)
typedef std::uint8_t  HaliteTinyTokenSize_t;
typedef std::uint16_t HaliteMediumTokenSize_t;
typedef std::uint32_t HaliteTokenSize_t;
typedef std::uint64_t HaliteLargeTokenSize_t;

// Tiny file TokenPool (less than 256 bytes)
typedef TokenPool<
    // Token type
    HaliteTokenType_t,
    // Max number of token
    HaliteTinyTokenSize_t,
    // Max number of bytes in the file
    HaliteTinyFileSize_t>
    HaliteTinyTokenPool;

// Medium file TokenPool (less than 65k bytes)
typedef TokenPool<
    // Token type
    HaliteTokenType_t,
    // Max number of token
    HaliteMediumTokenSize_t,
    // Max number of bytes in the file
    HaliteMediumFileSize_t>
    HaliteMediumTokenPool;

// Regular file TokenPool (less than 4b bytes)
typedef TokenPool<
    // Token type
    HaliteTokenType_t,
    // Max number of token
    HaliteTokenSize_t,
    // Max number of bytes in the file
    HaliteFileSize_t>
    HaliteTokenPool;

// Large file TokenPool (greater than 4b bytes)
typedef TokenPool<
    // Token type
    HaliteTokenType_t,
    // Max number of token
    HaliteLargeTokenSize_t,
    // Max number of bytes in the file
    HaliteLargeFileSize_t>
    HaliteLargeTokenPool;

// Regular NodePool storage
typedef TreeNodePool<
    // Node type
    HaliteNodeType_t,
    // Max number of nodes in the file
    std::uint32_t,
    // Regular TokenPool
    HaliteTokenPool>
    HaliteNodePool;

template<class S = HaliteNodePool>
class WASP_PUBLIC HaliteInterpreter : public Interpreter<S>
{
  public:
    typedef S                                  Storage_type;
    typedef std::shared_ptr<HaliteInterpreter> SharedPtr;
    HaliteInterpreter();
    HaliteInterpreter(std::ostream& err);
    virtual ~HaliteInterpreter();

    /** Invoke the lexer and parser for a stream.
     * @param in        input stream
     * @param sname     stream name for error messages
     * @return          true if successfully parsed
     */
    bool parseStream(std::istream&      in,
                     const std::string& sname       = "stream input",
                     size_t             startLine   = 1u,
                     size_t             startColumn = 1u);
    bool
    parse(std::istream& input, size_t startLine = 1u, size_t startColumn = 1u);
    /** Invoke the lexer and parser on an input string.
     * @param input     input string
     * @param sname     stream name for error messages
     * @return          true if successfully parsed
     */
    bool parseString(const std::string& input,
                     const std::string& sname       = "string stream",
                     size_t             startLine   = 1u,
                     size_t             startColumn = 1u);
    /** Invoke the lexer and parser on a file. Use parse_stream with a
     * std::ifstream if detection of file reading errors is required.
     * @param filename  input file name
     * @return          true if successfully parsed
     */
    bool parseFile(const std::string& filename, size_t startLine = 1u);

    /**
     * @brief evaluate evaluates the template emitting the expansion into out
     * stream
     * @param out the stream on which to emit the expanded template
     * @param data the accessor to the data for template expansion
     * @param activity_log an optional activity log to emit template activity on
     * @return true, iff the template expanded with no errors.
     */
    bool evaluate(std::ostream& out,
                  DataAccessor& data,
                  std::ostream* activity_log = nullptr);

  public:
    struct Range
    {
        std::string name;
        int         start;
        int         end;
        int         stride;
        Range(const std::string& name, int start, int end, int stride)
            : name(name), start(start), end(end), stride(stride)
        {
        }
        Range(const Range& o)
            : name(o.name), start(o.start), end(o.end), stride(o.stride)
        {
        }
    };

    bool import_range(DataAccessor&             data,
                      HaliteInterpreter<S>&     file_interpreter,
                      const std::vector<Range>& imports,
                      size_t                    index,
                      std::ostream&             out);
    static bool extract_ranges(std::string         range_data,
                               std::vector<Range>& ranges,
                               std::string&        error);

  private:
    bool parse_content(std::istream& in);

    bool parse_line(const std::string& line);

    /**
     * @brief capture_attribute_text processes attribute text and constructs
     * appropriate leaf nodes.
     * @param txt the text of the attribute - contains attribute name and
     * potentially attribute options (format, etc)
     * @param offset the offset into the file of this attribute text.
     * @param extract_options indicates whether to look for attribute options in
     * the given text
     * This will call capture_leaf as needed.
     */
    void capture_attribute_text(const std::string& txt,
                                size_t             offset,
                                bool               extract_options);
    /**
     * @brief capture_attribute_delim convenience method for capturing
     * terminator and potential trailing text
     * @param data the data string being examined for traililng text and
     * terminator capture
     * @param current_column_index the current column being examined - will be
     * updated upon capture
     * @param attribute_end_index the terminating index of the attribute
     */
    void capture_attribute_delim(const std::string& data,
                                 size_t&            current_column_index,
                                 size_t             attribute_end_index);
    void capture_leaf(const std::string& node_name,
                      size_t             node_type,
                      const std::string& data,
                      size_t             token_type,
                      size_t             file_offset);

    /**
     * @brief capture captures the given attributes into a tree structure
     * @param current_column_index the line's column to begin capture
     * @param current_attribute_index
     * @param attribute_indices
     * @param limit default of 0, meaning no limit.
     */
    void capture(const std::string& data,
                 size_t&            current_column_index,
                 size_t&            current_attribute_index,
                 const SubStringIndexer::IndexPairs_type& attribute_indices,
                 size_t                                   limit = 0);

    /**
     * @brief print_attribute prints attribute to output stream
     * @param attr_view tree view containing attribute structure
     * @param out the stream to emit the attribute substitution result
     * @param line the line on which the substitution is taking place
     * @param column the column at which the substitution is starting, will be
     * updated
     * @return true, iff no errors occurred during the substitution/printing of
     * the attribute
     */
    bool print_attribute(DataAccessor&   data,
                         const NodeView& attr_view,
                         std::ostream&   out,
                         size_t&         line,
                         size_t&         column);

    class SubstitutionOptions
    {
      public:
        SubstitutionOptions()
            : m_emit({"", 0})
            , m_separator(" ")
            , m_optional(false)
            , m_silent(false)
        {
        }
        SubstitutionOptions(const SubstitutionOptions& orig)
            : m_emit(orig.m_emit)
            , m_format(orig.m_format)
            , m_use(orig.m_use)
            , m_separator(orig.m_separator)
            , m_optional(orig.m_optional)
            , m_silent(orig.m_silent)
            , m_ranges(orig.m_ranges)
            , m_index(orig.m_index)
        {
        }
        bool has_format() const { return m_format.empty() == false; }
        const std::string& format() const { return m_format; }
        std::string&       format() { return m_format; }

        bool               has_emit() const { return m_emit.second != 0; }
        int&               emit_stride() { return m_emit.second; }
        int                emit_stride() const { return m_emit.second; }
        std::string&       emit_delim() { return m_emit.first; }
        const std::string& emit_delim() const { return m_emit.first; }

        /**
         * @brief has_use determine whether the substitution has an object scope
         * to use
         * @return true, iff the user has specified 'use=object_name'
         */
        bool               has_use() const { return m_use.empty() == false; }
        const std::string& use() const { return m_use; }
        std::string&       use() { return m_use; }

        bool  optional() const { return m_optional; }
        bool& optional() { return m_optional; }

        const std::string& separator() const { return m_separator; }
        std::string&       separator() { return m_separator; }

        bool  silent() const { return m_silent; }
        bool& silent() { return m_silent; }

        const std::vector<Range>& ranges() const { return m_ranges; }
        std::vector<Range>&       ranges() { return m_ranges; }

        void reset_emit_count_down() { m_emit_count_down = emit_stride(); }
        /**
         * @brief emit determine if the emit delimiter should be emitted
         * @return true, iff emit is present and has iterated sufficiently
         */
        bool emit_now() const { return (has_emit() && m_emit_count_down == 0); }
        void initialize(DataAccessor& d)
        {
            // if delimiter emission is present
            // initialize the iterator
            if (has_emit())
            {
                reset_emit_count_down();
            }
            m_index.resize(m_ranges.size());
            for (size_t i = 0; i < m_ranges.size(); ++i)
            {
                m_index[i] = m_ranges[i].start;
                d.store(m_ranges[i].name, m_index[i]);
                wasp_tagged_line("init " << m_ranges[i].name << "="
                                         << m_index[i]);
            }
        }
        bool has_next() const
        {
            wasp_check(m_index.size() == m_ranges.size());
            for (int i = m_index.size() - 1; i >= 0; --i)
            {
                if (has_next(i))
                    return true;
            }
            wasp_tagged_line("has next false");
            return false;
        }
        /**
         * @brief has_next determine if the ith loop has another iteration
         * @param i the loop index
         * @return true, iff the ith loop has another iteration available
         */
        bool has_next(int i) const
        {
            wasp_require(i >= 0 && i < static_cast<int>(m_ranges.size()));
            wasp_require(i >= 0 && i < static_cast<int>(m_index.size()));
            wasp_tagged_line("checking range of "
                             << m_ranges[i].name << " "
                             << m_index[i] + m_ranges[i].stride << " past "
                             << m_ranges[i].end);
            if (m_ranges[i].stride > 0)
            {
                if (m_index[i] + m_ranges[i].stride <= m_ranges[i].end)
                {
                    wasp_tagged_line("has next true with positive stride");
                    return true;
                }
            }
            else
            {
                if (m_index[i] + m_ranges[i].stride >= m_ranges[i].end)
                {
                    wasp_tagged_line("has next true with negative stride");
                    return true;
                }
            }
            return false;
        }
        bool next(DataAccessor& d)
        {
            // Iterate over loops inner-most to outer
            for (int i = m_index.size() - 1; i >= 0; --i)
            {
                if (has_next(i))
                {
                    m_index[i] += m_ranges[i].stride;
                    d.store(m_ranges[i].name, m_index[i]);
                    if (has_emit())
                    {
                        if (m_emit_count_down == 0)
                        {
                            reset_emit_count_down();
                        }
                        --m_emit_count_down;
                        wasp_tagged_line(
                            "next  delimiter emission count down of "
                            << m_emit_count_down);
                    }
                    wasp_tagged_line("next " << m_ranges[i].name << "="
                                             << m_index[i]);
                    return true;
                }
                else if (i == 0)  // i = 0, and done with loop
                {
                    wasp_tagged_line("next = false");
                    return false;
                }
                else
                {  // reset index and iterate at parent index
                    m_index[i] = m_ranges[i].start;
                    d.store(m_ranges[i].name, m_index[i]);
                    wasp_tagged_line("resetting " << m_ranges[i].name << "="
                                                  << m_index[i]);
                }
            }
            wasp_tagged_line("next = false");
            return false;
        }

      private:
        // The delimiter to emit and the stride at which to emit
        // delim='\n' and stride=5 will emit '\n' every 5th iteration
        std::pair<std::string, int> m_emit;
        // The current emit iterator, counting down to zero
        int m_emit_count_down;
        // format of the substitution
        std::string        m_format;
        std::string        m_use;
        std::string        m_separator;
        bool               m_optional;
        bool               m_silent;
        std::vector<Range> m_ranges;
        std::vector<int>   m_index;
        //...
    };
    /**
     * @brief attribute_options processes the options listed in data
     * @param options the options to populate
     * @param data the data containing the text representation of options
     * @return true, iff no issues arose in option extract
     */
    bool attribute_options(SubstitutionOptions& options,
                           const std::string&   data,
                           size_t               line);

    bool accumulate_attribute(DataAccessor&   data,
                              const NodeView& attr_view,
                              std::ostream&   out,
                              size_t&         line,
                              size_t&         column) const;
    /**
     * @brief conditional handles actioned conditional blocks
     * @param data the data accessor
     * @param action_view view of the action field (if,elseif,else...)
     * @param out the stream to emit the conditioned field
     * @param line the line from which the field begins
     * @param column the column from which the field begins
     * @return
     */
    bool conditional(DataAccessor&   data,
                     const NodeView& action_view,
                     std::ostream&   out,
                     size_t&         line,
                     size_t&         column);
    bool evaluate(DataAccessor&   data,
                  const NodeView& action_view,
                  std::ostream&   out,
                  size_t&         line,
                  size_t&         column);
    bool evaluate_component(DataAccessor&   data,
                            const NodeView& action_view,
                            std::ostream&   out,
                            size_t&         line,
                            size_t&         column);

    /**
     * @brief import_file imports the file represented by the given tree view
     * @param import_view the tree view containing the directive and potentially
     * parameterized text
     * @param out the output stream on which to emit the file contents
     * @param line the line on which the import is taking place
     * @param column the column at which the import starts
     * @return true, iff the file and its contents were successfully imported
     * and emitted
     */
    bool import_file(DataAccessor&   data,
                     const NodeView& import_view,
                     std::ostream&   out,
                     size_t&         line,
                     size_t&         column);
    bool repeat_file(DataAccessor&   data,
                     const NodeView& import_view,
                     std::ostream&   out,
                     size_t&         line,
                     size_t&         column);
    /**
     * @brief process_result conduct formatting, error checking of the result
     * @param result the result to be emitted to the output stream
     * @param options the options dictating format, etc.
     * @param line the line on which this is occurring
     * @param out the output stream to write the result
     * @return true, iff no error occurred.
     */
    bool process_result(const Result&              result,
                        const SubstitutionOptions& options,
                        size_t                     line,
                        std::ostream&              out);

  public:  // public variables
    /**
     * @brief setStreamName sets the name of this stream and indicates whether
     * the stream/name is a file[path]
     * @param name the name of the stream or file[path]
     * @param isFile [default=false] indicates whether the name is a file path
     */
    void setStreamName(const std::string& name, bool isFile = false)
    {
        Interpreter<S>::stream_name() = name;
        m_has_file                    = isFile;
    }

    /**
     * @brief hasFile indicates whether this parser's stream name is a file path
     * @return
     */
    bool hasFile() const { return m_has_file; }

    const std::string& attr_start_delim() const
    {
        return m_attribute_start_delim;
    }
    std::string& attr_start_delim() { return m_attribute_start_delim; }

    const std::string& attr_end_delim() const { return m_attribute_end_delim; }
    std::string&       attr_end_delim() { return m_attribute_end_delim; }

    const std::string& attr_start_name() const
    {
        return m_attribute_start_name;
    }
    std::string& attr_start_name() { return m_attribute_start_name; }

    const std::string& attr_end_name() const { return m_attribute_end_name; }
    std::string&       attr_end_name() { return m_attribute_end_name; }

  private:  // private variables
    //
    // Attribute are delimited by 3 components.
    // 1) declarator or starting delimitor
    // 2) terminator or end delimitor
    // 3) an attribute substitution options indicator
    // E.g., < V : O > where start='<', end ='>', and option = ':'
    // V and O indicate the variable name and options respectively
    //

    /**
     * @brief attribute_start_delim attribute substitution declarator
     */
    std::string m_attribute_start_delim;
    /**
     * @brief attribute_end_delim attribute substitution terminator
     */
    std::string m_attribute_end_delim;

    /**
     * @brief m_attribute_start_name the referenceable variable name for start
     * delim
     */
    std::string m_attribute_start_name;
    /**
     * @brief m_attribute_end_name the referenceable variable name for end delim
     */
    std::string m_attribute_end_name;

    /**
     * @brief m_attribute_options_delim attribute substitution options indicator
     */
    std::string m_attribute_options_delim;

    size_t m_file_offset;

  private:  // private methods
    /**
     * @brief mHasFile indicates whether this parser was instantiated via a file
     */
    bool m_has_file;
};  // end of HaliteInterpreter class

inline WASP_PUBLIC bool expand_template(std::ostream&      result,
                                        std::ostream&      elog,
                                        std::ostream&      alog,
                                        const std::string& template_file,
                                        const std::string& json_parameter_file,
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
    if (json_parameter_file.empty())
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

    // if json_parameter_file starts with '{' and ends with '}' then treat it as
    // a string holding the json parameter file contents to parse - otherwise
    // treat it as a filepath to the json parametr file

    DataObject::SP obj_ptr;
    bool json_failed = true;

    if ( json_parameter_file.size()  >   2  &&
         json_parameter_file.front() == '{' &&
         json_parameter_file.back()  == '}' )
    {
        std::istringstream json_input_stream(json_parameter_file);
        JSONObjectParser generator(obj_ptr, json_input_stream, elog, nullptr);
        json_failed = generator.parse() != 0;
    }
    else
    {
        std::ifstream json_file_stream(json_parameter_file);
        JSONObjectParser generator(obj_ptr, json_file_stream, elog, nullptr);
        json_failed = generator.parse() != 0;
    }
    if (json_failed)
    {
        elog << "***Error : Parsing of data " << json_parameter_file
             << " failed!" << std::endl;
        return false;
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
}
typedef HaliteInterpreter<> DefaultHaliteInterpreter;
}  // namespace wasp

#include "wasphalite/HaliteInterpreter.i.h"
#endif  // WASPHaliteIntERPRETER_H
