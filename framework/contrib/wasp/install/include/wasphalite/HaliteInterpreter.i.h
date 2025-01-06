#ifndef WASP_HALITEINTERPRETER_I_H
#define WASP_HALITEINTERPRETER_I_H

#include "wasphalite/HaliteInterpreter.h"

namespace wasp
{
template<class S>
HaliteInterpreter<S>::HaliteInterpreter()
    : Interpreter<S>()
    , m_attribute_start_delim("<")
    , m_attribute_end_delim(">")
    , m_attribute_start_name("_S_")
    , m_attribute_end_name("_E_")
    , m_attribute_options_delim(":")
    , m_file_offset(0)
    , m_has_file(false)
{
}
template<class S>
HaliteInterpreter<S>::HaliteInterpreter(std::ostream& err)
    : Interpreter<S>(err)
    , m_attribute_start_delim("<")
    , m_attribute_end_delim(">")
    , m_attribute_start_name("_S_")
    , m_attribute_end_name("_E_")
    , m_attribute_options_delim(":")
    , m_file_offset(0)
    , m_has_file(false)
{
}
template<class S>
HaliteInterpreter<S>::~HaliteInterpreter()
{
}
template<class S>
bool HaliteInterpreter<S>::parse(std::istream& in,
                                 size_t        startLine,
                                 size_t        startColumn)
{
    return parseStream(
        in, hasFile() ? Interpreter<S>::stream_name() : "stream input",
        startLine, startColumn);
}
template<class S>
bool HaliteInterpreter<S>::parseStream(std::istream&      in,
                                       const std::string& sname,
                                       size_t             start_line,
                                       size_t             start_column)
{
    Interpreter<S>::stream_name()  = sname;
    Interpreter<S>::start_line()   = start_line;
    Interpreter<S>::start_column() = start_column;

    bool parsed = parse_content(in);

    Interpreter<S>::commit_stages();

    return parsed;
}
template<class S>
bool HaliteInterpreter<S>::parseFile(const std::string& filename, size_t line)
{
    std::ifstream in(filename.c_str());
    if (!in.good())
    {
        Interpreter<S>::error_stream()
            << "file '" << filename
            << "' is either inaccessible or doesn't exist!"
               " Unable to read."
            << std::endl;
        return false;
    }
    m_has_file = true;
    return parseStream(in, filename, line);
}
template<class S>
bool HaliteInterpreter<S>::parseString(const std::string& input,
                                       const std::string& sname,
                                       size_t             startLine,
                                       size_t             startColumn)
{
    std::istringstream iss(input);
    return parseStream(iss, sname, startLine, startColumn);
}
template<class S>
bool HaliteInterpreter<S>::parse_content(std::istream& in)
{
    // process all lines of the file
    std::string line;
    while (!in.eof() && in.good())
    {
        std::getline(in, line);
        bool line_processed = parse_line(line);

        if (line_processed == false)
            return false;

        ++m_file_offset;  // increment past newline
    }
    if (!in.eof() && in.fail())
    {
        Interpreter<S>::error_stream() << Interpreter<S>::stream_name()
                                       << " - error while reading" << std::endl;
        return false;
    }

    //
    if (Interpreter<S>::line_count() > 0 &&
        line.empty())  // pop last empty line
    {
        //        Interpreter<S>::pop_line();
        wasp_tagged_line("Parsed file Line count "
                         << Interpreter<S>::line_count());
    }

    return true;
}
template<class S>
void HaliteInterpreter<S>::capture_leaf(const std::string& node_name,
                                        size_t             node_type,
                                        const std::string& data,
                                        size_t             token_type,
                                        size_t             file_offset)
{
    // acquire the soon-to-be-fullfilled token index
    size_t token_i = Interpreter<S>::token_count();

    // push the token text
    Interpreter<S>::push_token(data.c_str(), token_type, file_offset);
    // push the leaf node representing the token
    size_t node_i =
        Interpreter<S>::push_leaf(node_type, node_name.c_str(), token_i);

    // stage the leaf for committal as a child of the document
    // templates are very flat; nearly all nodes are a child
    // of the document root
    Interpreter<S>::push_staged_child(node_i);
}

template<class S>
bool HaliteInterpreter<S>::parse_line(const std::string& line)
{
    //
    // A line could be the following
    // 1) a plain line of text
    // 2) a line of text with attributes
    // 3) an import or repeat statement
    // 4) the start of an actioned conditional block (if,ifdef,ifndef)
    // 5) the continuation of an actioned conditional block (elseif, else)
    // 6) the termination of an actioned conditional block (endif)
    //

    //
    // identify all attribute locations
    //
    SubStringIndexer                  attribute_declarators;
    SubStringIndexer::IndexPairs_type attribute_indices;
    if (attribute_declarators.index(line, m_attribute_start_delim))
    {
        SubStringIndexer attribute_terminators(line, m_attribute_end_delim);
        attribute_indices = attribute_declarators.merge(attribute_terminators);
    }
    size_t current_column_index = 0;
    // check for file import statement
    bool               is_directive = line.compare(0, 1, "#") == 0;
    static std::string import_stmt  = "#import";
    bool               is_import    = false;
    static std::string repeat_stmt  = "#repeat";
    bool               is_repeat    = false;
    static std::string ifdef_stmt   = "#ifdef";
    bool               is_ifdef     = false;  // assume false
    static std::string ifndef_stmt  = "#ifndef";
    bool               is_ifndef    = false;  // assume false
    static std::string if_stmt      = "#if";
    bool               is_if        = false;  // assume false
    static std::string elseif_stmt  = "#elseif";
    bool               is_elseif    = false;  // assume false
    static std::string else_stmt    = "#else";
    bool               is_else      = false;  // assume false
    static std::string endif_stmt   = "#endif";
    bool               is_endif     = false;  // assume false

    if (is_directive &&
        (is_import = line.compare(0, import_stmt.size(), import_stmt) == 0))
    {  // capture import declarator
        Interpreter<S>::push_staged(wasp::FILE, "import", {});
        size_t offset = m_file_offset + current_column_index;
        capture_leaf("decl", wasp::DECL,
                     line.substr(current_column_index, import_stmt.size()),
                     wasp::STRING, offset);
        current_column_index += import_stmt.size();
    }
    else if (is_directive &&
             (is_ifdef = line.compare(0, ifdef_stmt.size(), ifdef_stmt) == 0))
    {
        Interpreter<S>::push_staged(wasp::PREDICATED_CHILD, "A", {});
        Interpreter<S>::push_staged(wasp::CONDITIONAL, "ifdef", {});
        size_t offset = m_file_offset + current_column_index;
        capture_leaf("decl", wasp::DECL,
                     line.substr(current_column_index, ifdef_stmt.size()),
                     wasp::STRING, offset);
        current_column_index += ifdef_stmt.size();
    }
    else if (is_directive && (is_ifndef = line.compare(0, ifndef_stmt.size(),
                                                       ifndef_stmt) == 0))
    {
        Interpreter<S>::push_staged(wasp::PREDICATED_CHILD, "A", {});
        Interpreter<S>::push_staged(wasp::CONDITIONAL, "ifndef", {});
        size_t offset = m_file_offset + current_column_index;
        capture_leaf("decl", wasp::DECL,
                     line.substr(current_column_index, ifndef_stmt.size()),
                     wasp::STRING, offset);
        current_column_index += ifndef_stmt.size();
    }
    else if (is_directive &&
             (is_if = line.compare(0, if_stmt.size(), if_stmt) == 0))
    {
        Interpreter<S>::push_staged(wasp::PREDICATED_CHILD, "A", {});
        Interpreter<S>::push_staged(wasp::CONDITIONAL, "if", {});
        size_t offset = m_file_offset + current_column_index;
        capture_leaf("decl", wasp::DECL,
                     line.substr(current_column_index, if_stmt.size()),
                     wasp::STRING, offset);
        current_column_index += if_stmt.size();
    }
    // TODO clean up!
    else if (is_directive && (is_elseif = line.compare(0, elseif_stmt.size(),
                                                       elseif_stmt) == 0))
    {
        // check for required condition to be open
        wasp_check(Interpreter<S>::staged_count() > 0);
        size_t staged_type =
            Interpreter<S>::staged_type(Interpreter<S>::staged_count() - 1);
        bool is_true = staged_type == wasp::WASP_TRUE;
        if (!is_true && staged_type != wasp::PREDICATED_CHILD)
        {
            Interpreter<S>::error_stream()
                << "***Error : line " << Interpreter<S>::line_count() + 1
                << " is an unmatched conditional elseif."
                << " The matching #if, #ifdef, #ifndef, or #elseif"
                << " is missing." << std::endl;
            return false;
        }
        // commit/close the current staged conditional to the parse tree
        if (is_true)
        {
            Interpreter<S>::commit_staged(Interpreter<S>::staged_count() - 1);
            wasp_check(wasp::CONDITIONAL ==
                       Interpreter<S>::staged_type(
                           Interpreter<S>::staged_count() - 1));
            Interpreter<S>::commit_staged(Interpreter<S>::staged_count() - 1);
        }
        // push new elseif staged conditional
        Interpreter<S>::push_staged(wasp::CONDITIONAL, "elseif", {});
        size_t offset = m_file_offset + current_column_index;
        capture_leaf("decl", wasp::DECL,
                     line.substr(current_column_index, elseif_stmt.size()),
                     wasp::STRING, offset);
        current_column_index += elseif_stmt.size();
    }
    // TODO clean up!
    else if (is_directive &&
             (is_else = line.compare(0, else_stmt.size(), else_stmt) == 0))
    {
        // check for required condition to be open
        wasp_check(Interpreter<S>::staged_count() > 0);
        size_t staged_type =
            Interpreter<S>::staged_type(Interpreter<S>::staged_count() - 1);

        bool is_true = staged_type == wasp::WASP_TRUE;
        if (!is_true && staged_type != wasp::PREDICATED_CHILD)
        {
            Interpreter<S>::error_stream()
                << "***Error : line " << Interpreter<S>::line_count() + 1
                << " is an unmatched conditional else."
                << " The matching #if, #ifdef, #ifndef, or #elseif"
                << " is missing." << std::endl;
            return false;
        }
        // commit/close the current staged conditional to the parse tree
        if (is_true)
        {
            Interpreter<S>::commit_staged(Interpreter<S>::staged_count() - 1);
            wasp_check(wasp::CONDITIONAL ==
                       Interpreter<S>::staged_type(
                           Interpreter<S>::staged_count() - 1));
            Interpreter<S>::commit_staged(Interpreter<S>::staged_count() - 1);
        }
        // push new else staged conditional
        Interpreter<S>::push_staged(wasp::CONDITIONAL, "else", {});
        size_t offset = m_file_offset + current_column_index;
        capture_leaf("decl", wasp::DECL,
                     line.substr(current_column_index, else_stmt.size()),
                     wasp::STRING, offset);
        current_column_index += else_stmt.size();
    }
    // TODO clean up!
    else if (is_directive &&
             (is_endif = line.compare(0, endif_stmt.size(), endif_stmt) == 0))
    {
        wasp_check(Interpreter<S>::staged_count() > 0);
        size_t staged_type =
            Interpreter<S>::staged_type(Interpreter<S>::staged_count() - 1);
        bool is_true   = staged_type == wasp::WASP_TRUE;
        bool is_action = staged_type == wasp::PREDICATED_CHILD;
        if (!is_true  // any if construct will have TRUE
            && staged_type != wasp::CONDITIONAL &&
            !is_action)  // else has no TRUE,
        {
            Interpreter<S>::error_stream()
                << "***Error : line " << Interpreter<S>::line_count() + 1
                << " is an unmatched conditional terminator - '" << line << "'."
                << " The matching #if, #ifdef, #ifndef, #elseif,"
                << " or #else is missing." << std::endl;
            return false;
        }
        if (!is_action)
        {
            // commit/close the current staged conditional TRUE content to the
            // parse tree
            Interpreter<S>::commit_staged(Interpreter<S>::staged_count() - 1);
        }

        // commit/close the current staged conditional to the parse tree
        if (is_true)
            Interpreter<S>::commit_staged(Interpreter<S>::staged_count() - 1);
        // capture the terminator as a leaf of the current staged action
        size_t offset = m_file_offset + current_column_index;
        capture_leaf("endif", wasp::TERM,
                     line.substr(current_column_index, endif_stmt.size()),
                     wasp::STRING, offset);
        // commit/close the current staged Action content to the parse tree
        Interpreter<S>::commit_staged(Interpreter<S>::staged_count() - 1);
        // update the current column
        current_column_index += endif_stmt.size();
    }
    else if (is_directive && (is_repeat = line.compare(0, repeat_stmt.size(),
                                                       repeat_stmt) == 0))
    {  // capture import declarator
        Interpreter<S>::push_staged(wasp::REPEAT, "repeat", {});
        size_t offset = m_file_offset + current_column_index;
        capture_leaf("decl", wasp::DECL,
                     line.substr(current_column_index, repeat_stmt.size()),
                     wasp::STRING, offset);
        current_column_index += repeat_stmt.size();
    }
    bool condition_captured = false;
    if (attribute_indices.empty() == false)
    {  // in addition to the attributes, capture the components before, between,
        // and after
        // capture the condition
        bool is_condition  = is_ifdef || is_ifndef || is_if || is_elseif;
        condition_captured = is_condition;
        if (is_condition)
        {
            // push condition
            Interpreter<S>::push_staged(wasp::EXPRESSION, "C", {});
        }
        size_t current_attribute_index = 0;
        size_t limit                   = attribute_indices.size();
        // capture up to the conclusion of the attributes
        capture(line, current_column_index, current_attribute_index,
                attribute_indices, limit);
    }
    // if line is plain text, capture
    if (is_import == false && is_repeat == false && is_ifdef == false &&
        is_ifndef == false && is_if == false && is_elseif == false &&
        is_else == false && is_endif == false && attribute_indices.empty())
    {
        // only capture lines with content
        if (line.empty() == false)
        {
            capture_leaf("txt", wasp::STRING, line, wasp::STRING,
                         m_file_offset);
        }
    }
    // capture potential trailing text
    else
    {
        // capture the condition
        bool is_condition = (is_ifdef || is_ifndef || is_if || is_elseif);
        if (is_condition && !condition_captured)
        {
            // push condition
            Interpreter<S>::push_staged(wasp::EXPRESSION, "C", {});
        }
        // current_column index has been updated by capture(), etc.
        size_t offset           = m_file_offset + current_column_index;
        size_t remaining_length = line.size() - current_column_index;
        wasp_check(current_column_index + remaining_length <= line.size());

        if (remaining_length > 0)
        {
            capture_leaf("txt", wasp::STRING,
                         line.substr(current_column_index, remaining_length),
                         wasp::STRING, offset);
        }

        // when closing import/repeat statement or condition expression, commit
        // the tree
        if (is_import || is_repeat || is_condition)
        {
            Interpreter<S>::commit_staged(Interpreter<S>::staged_count() - 1);
        }

        // when conditional is in play, we need to stage all
        // template components that are to be emitted when
        // the conditional evaluates as TRUE
        if (is_condition)
        {
            // push condition
            Interpreter<S>::push_staged(wasp::WASP_TRUE, "T", {});
        }
    }
    // compute the new offset
    m_file_offset += line.size();
    Interpreter<S>::push_line_offset(m_file_offset);
    wasp_tagged_line("Pushed line " << Interpreter<S>::line_count()
                                    << " offset " << m_file_offset
                                    << " for line '" << line << "'");
    return true;
}
template<class S>
void HaliteInterpreter<S>::capture(
    const std::string&                       data,
    size_t&                                  current_column_index,
    size_t&                                  current_attribute_index,
    const SubStringIndexer::IndexPairs_type& attribute_indices,
    size_t                                   limit)
{
    wasp_require(limit <= attribute_indices.size());

    std::vector<size_t> depths = SubStringIndexer::depths(attribute_indices);
    wasp_check(depths.size() == attribute_indices.size());

    if (limit == 0)
        limit = attribute_indices.size();

    std::vector<SubStringIndexer::IndexPair_type> open_tree;

    for (size_t i = current_attribute_index; i < limit; ++i)
    {
        const SubStringIndexer::IndexPair_type& attribute_index =
            attribute_indices[i];
        // check for preceding text/name that needs to be put into tree
        if (attribute_index.first > current_column_index)
        {
            size_t length      = attribute_index.first - current_column_index;
            size_t file_offset = m_file_offset + current_column_index;
            const std::string& prefixed =
                data.substr(current_column_index, length);
            capture_attribute_text(prefixed, file_offset, !open_tree.empty());
        }
        Interpreter<S>::push_staged(wasp::IDENTIFIER, "attr", {});
        {  // capture declarative delimiter
            size_t file_offset = m_file_offset + attribute_index.first;
            capture_leaf(m_attribute_start_delim, wasp::DECL,
                         m_attribute_start_delim, wasp::STRING, file_offset);
        }

        // increment current column to next potential block of text
        current_column_index =
            attribute_index.first + m_attribute_start_delim.size();
        // initial assumed depth to worst scenario, no more attributes
        // in this situation, all open trees must be closed/committed
        int depth_delta = open_tree.size() * -1;
        // acquire the
        if (i + 1 < attribute_indices.size())
        {
            depth_delta = depths[i + 1] - depths[i];
        }

        // The next attribute should be a sibling or uncle in the tree.
        // Capture the current attribute's text, terminator,
        // commit open sub tree and update current column index
        // to next potential block of text.
        if (depth_delta <= 0)
        {
            capture_attribute_delim(data, current_column_index,
                                    attribute_index.second);

        }  // end of depth == 0
        // The next attribute is an uncle/great/great uncle etc.
        // Capture the appropriate open ancestral subtrees
        if (depth_delta < 0)
        {
            while (depth_delta != 0)
            {
                const auto& prev = open_tree.back();
                capture_attribute_delim(data, current_column_index,
                                        prev.second);
                ++depth_delta;
                open_tree.pop_back();
            }
        }
        // The next attribute is a child.
        // Push onto the open tree stack for future closure.
        else if (depth_delta > 0)
        {
            open_tree.push_back(attribute_index);
        }
    }  // end of attribute loop
}

template<class S>
bool HaliteInterpreter<S>::evaluate(std::ostream& out,
                                    DataAccessor& data,
                                    std::ostream* activity_log)
{
    auto   tree_view = Interpreter<S>::root();
    size_t line = 1, column = 1;
    data.store(attr_start_name(), attr_start_delim());
    data.store(attr_end_name(), attr_end_delim());

    bool result = evaluate(data, tree_view, out, line, column);

    int remaining_lines = Interpreter<S>::line_count() - line;
    wasp_tagged_line("Remaining lines = " << remaining_lines);
    if (remaining_lines > 0)
    {
        wasp_tagged_line("Emitting " << Interpreter<S>::line_count() << "-"
                                     << line << "=" << remaining_lines
                                     << " remaining lines...");
        out << std::string(remaining_lines, '\n');
    }
    return result;
}

template<class S>
bool HaliteInterpreter<S>::evaluate(DataAccessor&   data,
                                    const NodeView& tree_view,
                                    std::ostream&   out,
                                    size_t&         current_line,
                                    size_t&         current_column)
{
    for (size_t i = 0; i < tree_view.child_count(); ++i)
    {
        const auto& child_view = tree_view.child_at(i);
        if (!evaluate_component(data, child_view, out, current_line,
                                current_column))
        {
            return false;
        }
    }

    return true;
}
template<class S>
bool HaliteInterpreter<S>::evaluate_component(DataAccessor&   data,
                                              const NodeView& tree_view,
                                              std::ostream&   out,
                                              size_t&         current_line,
                                              size_t&         current_column)
{
    auto child_type = tree_view.type();
    switch (child_type)
    {
        case wasp::STRING:
            // print the text and update the current line and column
            wasp::print_from(out, tree_view, current_line, current_column);
            break;
        case wasp::IDENTIFIER:
        {
            std::stringstream substitution;
            if (!print_attribute(data, tree_view, substitution, current_line,
                                 current_column))
            {
                return false;
            }
            out << substitution.str();
        }
        break;
        case wasp::FILE:
            if (!import_file(data, tree_view, out, current_line,
                             current_column))
                return false;
            break;
        case wasp::REPEAT:
            if (!repeat_file(data, tree_view, out, current_line,
                             current_column))
                return false;
            break;
        // actioned conditional blocks ifdef,etc.
        case wasp::PREDICATED_CHILD:
            if (!conditional(data, tree_view, out, current_line,
                             current_column))
                return false;
            break;
        default:
            wasp_tagged_line("unknown construct " << tree_view.data());
            wasp_not_implemented("template construct at line " +
                                 std::to_string(current_line));
            break;
    }  // end of switch

    return true;
}
template<class S>
bool HaliteInterpreter<S>::print_attribute(DataAccessor&   data,
                                           const NodeView& attr_view,
                                           std::ostream&   out,
                                           size_t&         line,
                                           size_t&         column)
{
    // attributes must have '<' txt? '>'
    // e.g., < txt> or <>
    wasp_require(attr_view.child_count() > 1);
    std::stringstream attr_str;
    size_t            start_column =
        column;  // todo ensure column is propogated appropriately
    size_t new_line = attr_view.line();
    int    delta    = new_line - line;

    wasp_tagged_line(info(attr_view) << " line delta " << delta);
    if (delta > 0)
    {
        wasp_tagged_line("inserting " << delta << " newline(s).");
        out << std::string(delta, '\n');
    }
    std::stringstream options_str;
    bool              has_options = false;
    // accumulate an attribute string
    for (size_t i = 1, count = attr_view.child_count() - 1; i < count; ++i)
    {
        auto child_view = attr_view.child_at(i);
        auto type       = child_view.type();
        switch (type)
        {
            // plain text to be printed for variable substitution
            case wasp::STRING:
            {
                if (!has_options)
                {
                    wasp_tagged_line("capturing static attribute text of '"
                                     << child_view.data() << "'");
                    line = new_line;
                    attr_str << child_view.data();
                }
                else
                    options_str << child_view.data();
                break;
            }
            // nested attribute, recurse
            case wasp::IDENTIFIER:
                if (!has_options &&
                    !print_attribute(data, child_view, attr_str, line, column))
                {
                    return false;
                }
                else if (has_options &&
                         !print_attribute(data, child_view, options_str, line,
                                          column))
                {
                    return false;
                }

                break;
            // indicates the attribute has options for substitution
            case wasp::FUNCTION:
                has_options = true;
                options_str << child_view.data();

                break;
            default:
                wasp_not_implemented("nested attribute printing");
        }
    }

    SubstitutionOptions options;
    if (has_options)
    {
        wasp_tagged_line("Options string is '" << options_str.str() << "'");
        if (!attribute_options(options, options_str.str(), new_line))
        {
            return false;
        }
    }
    // TODO - add optimization to check if above loop only encounters STRING
    // and optionally FUNCTION in which indicate a variable can be directly
    // substituted
    // and formatted and no expression evaluation is needed

    // attribute string contains the full attribute name
    ExprInterpreter<S> expr(Interpreter<S>::error_stream());
    wasp_tagged_line("expression '"
                     << attr_str.str() << "' starting on line " << line
                     << " and column "
                     << start_column + m_attribute_start_delim.size());

    // if attribute is empty - then error and return false
    if (attr_str.str().find_first_not_of(" \t") == std::string::npos)
    {
        Interpreter<S>::error_stream()
            << "***Error : empty attribute substitution on line " << line
            << " and column " << column << "." << std::endl;
        return false;
    }

    if (false ==
        expr.parse(attr_str, line,
                   start_column + m_attribute_start_delim.size()))
    {
        wasp_tagged_line("Failed parsing expression evaluation...");
        return false;
    }
    if (options.ranges().empty())
    {
        Result result;
        if (options.has_use())
        {
            const std::string& scope_name = options.use();
            wasp_tagged_line("Using options with name '" << scope_name << "'");
            DataArray*  use_array = data.array(scope_name);
            DataObject* use_obj   = data.object(scope_name);
            if (use_array != nullptr)
            {
                for (size_t e = 0, array_size = use_array->size();
                     e < array_size; ++e)
                {
                    const Value& element = use_array->at(e);
                    if (!element.is_object())
                        continue;  // todo
                    DataObject* use_object = element.to_object();
                    wasp_check(use_object);
                    DataAccessor use(use_object, &data);
                    result = expr.evaluate(use);
                    if (result.is_error())
                    {
                        return false;
                    }
                    if (!process_result(result, options, line, out))
                        return false;
                    if (e + 1 != array_size)
                        out << options.separator();
                }
            }
            else if (use_obj != nullptr)
            {
                // capture new scope with appropriate parent
                DataAccessor use(use_obj, &data);
                result = expr.evaluate(use);
                if (!process_result(result, options, line, out))
                    return false;
            }
            else if (!options.optional())
            {
                Interpreter<S>::error_stream()
                    << "***Error : unable to substitute expression on line "
                    << line << " using '" << options.use()
                    << "' - the data object or array could not be found."
                    << std::endl;
                return false;
            }
        }
        else
        {
            result = expr.evaluate(data);
            if (!process_result(result, options, line, out))
                return false;
        }
    }
    else
    {
        DataAccessor use(data);
        if (options.has_use())
        {
            const std::string& obj_name = options.use();
            DataObject*        use_obj  = data.object(obj_name);
            if (use_obj == nullptr && !options.optional())
            {
                Interpreter<S>::error_stream()
                    << "***Error : unable to substitute expression on line "
                    << line << " using '" << options.use()
                    << "' - the data object could not be found." << std::endl;
                return false;
            }
            // Missing use object is not an error as user
            // indicated it was optional
            else if (use_obj == nullptr && options.optional())
            {
                return true;
            }
            else{
                // capture new scope with appropriate parent
                use = DataAccessor(use_obj, &data);
            }
        }
        DataObject   o;
        DataAccessor layer(&o, &use);
        options.initialize(layer);
        for (;;)
        {
            auto result = expr.evaluate(layer);
            if (!process_result(result, options, line, out))
                return false;
            if (!options.next(layer))
                break;
            // if delimiter is not to be emitted (the common case)
            // emit the separator
            if (!options.emit_now())
            {
                out << options.separator();
            }
            else
            {  // else emit the delimiter
                wasp_tagged_line("Emitting stride delimiter...");
                out << options.emit_delim();
            }
        }
    }
    auto last_attr_component = attr_view.child_at(attr_view.child_count() - 1);
    column = last_attr_component.column() + m_attribute_end_delim.size();
    line   = new_line;
    return true;
}
template<class S>
bool HaliteInterpreter<S>::process_result(const Result&              result,
                                          const SubstitutionOptions& options,
                                          size_t                     line,
                                          std::ostream&              out)
{
    if (result.is_error() && !options.optional())
    {
        wasp_tagged_line(result.string());
        Interpreter<S>::error_stream() << result.string();
        return false;
    }
    if (result.is_error() == false)
    {
        wasp_tagged_line("expression result is " << result.as_string());
        if (options.has_format() && !options.silent())
        {
            if (!result.format(out, options.format(),
                               Interpreter<S>::error_stream()))
            {
                Interpreter<S>::error_stream()
                    << std::endl
                    << "***Error : on line " << line
                    << " failed to format result - " << result.as_string()
                    << " - as '" << options.format() << "'." << std::endl;
                return false;
            }
        }
        else if (!options.silent())
        {
            if (!result.format(out))
            {
                Interpreter<S>::error_stream()
                    << std::endl
                    << "***Error : on line " << line
                    << " failed to format result - " << result.as_string()
                    << "." << std::endl;
                return false;
            }
        }
    }
    return true;
}

template<class S>
bool HaliteInterpreter<S>::conditional(DataAccessor&   data,
                                       const NodeView& action_view,
                                       std::ostream&   out,
                                       size_t&         line,
                                       size_t&         column)
{
    // action fields start with if,ifdef,ifndef
    size_t child_count = action_view.child_count();
    wasp_require(child_count > 0);

    const auto& term_view   = action_view.child_at(child_count - 1);
    size_t      action_line = action_view.line();
    int         delta       = action_line - line;
    if (delta > 0)
    {
        wasp_tagged_line("inserting " << delta << " newline(s).");
        out << std::string(delta, '\n');
        line += delta;
        wasp_tagged_line("conditional block has " << delta
                                                  << " lines to emit for "
                                                  << info(action_view));
    }
    line = action_line;
    wasp_require(term_view.type() == wasp::TERM);
    size_t i = 0;
    for (; i < child_count; ++i)
    {
        const auto& child_view = action_view.child_at(i);
        if (child_view.type() == wasp::TERM)
            break;  // 'endif'
        std::string action_name = child_view.name();

        wasp_check(action_name.size() >= 2);
        // check for 'if','ifdef','ifndef'
        if (action_name.compare(0, 2, "if") == 0 ||
            (action_name.size() > 3 &&
             action_name.compare(action_name.size() - 3, 3, "eif") ==
                 0))  // 'elseif'
        {
            // child at 0 is decl, 1 is the condition,
            const auto cond_view = child_view.child_at(1);
            // simple name lookup to determine existance
            if (cond_view.child_count() == 1)
            {
                std::string var_name = trim(cond_view.data(), " ");
                bool        exists   = data.exists(var_name);
                wasp_tagged_line(action_name << " exists? " << std::boolalpha
                                             << exists);
                // if'n'def
                if (action_name.size() > 3 && action_name.at(2) == 'n')
                {
                    wasp_tagged_line("conducting ifndef...");
                    // ifndef and variable exists
                    // this action is not to be performed
                    if (exists)
                    {
                        continue;  // try next block in action list
                                   // (#else,#elseif)
                    }
                }
                // if, ifdef and variable doesn't exist, not performing action
                else if (!exists)
                {
                    continue;  // try next block in action list
                }
                // made the gauntlet - evaluate

                ++line;  // account for #if, etc.
                column                       = 1;
                const auto& action_true_view = child_view.child_at(2);
                int         delta            = action_true_view.line() - line;
                if (delta > 0)
                {
                    wasp_tagged_line("inserting " << delta << " newline(s).");
                    out << std::string(delta, '\n');
                    line += delta;
                    wasp_tagged_line("conditional block has "
                                     << delta << " lines to emit for "
                                     << info(child_view));
                }
                if (!evaluate(data, action_true_view, out, line, column))
                {
                    return false;
                }
                break;
            }
            // more than 1 component indicates attributes need evaluation
            else if (cond_view.child_count() > 1)  // ' ' attribute...
            {
                SubstitutionOptions options;
                std::stringstream   attr_str;
                size_t              cline = cond_view.line();
                size_t              ccol  = column;
                for (size_t ci = 0; ci < cond_view.child_count(); ++ci)
                {
                    const auto& if_cond_view = cond_view.child_at(ci);

                    auto type = if_cond_view.type();
                    switch (type)
                    {
                        // plain text to be printed for variable substitution
                        case wasp::STRING:

                            wasp::print_from(attr_str, if_cond_view, cline,
                                             ccol);
                            break;
                        // nested attribute, recurse
                        case wasp::IDENTIFIER:
                            if (!print_attribute(data, if_cond_view, attr_str,
                                                 cline, ccol))
                            {
                                return false;
                            }
                            break;
                        // indicates the attribute has options for substitution
                        case wasp::FUNCTION:
                            // legal, but not used
                            attribute_options(options, if_cond_view.data(),
                                              cline);
                            break;
                        default:
                            wasp_not_implemented(if_cond_view.data() +
                                                 " is not supported.");
                    }
                }
                ExprInterpreter<> expr(Interpreter<S>::error_stream());
                if (false ==
                    expr.parse(attr_str, line,
                               column + m_attribute_start_delim.size()))
                {
                    return false;
                }
                auto result = expr.evaluate(data);
                if (result.is_error())
                {
                    wasp_tagged_line(result.string());
                    return false;
                }
                // if result is true, evaluate the Condition node
                if ((result.is_bool() && result.boolean()) ||
                    (result.is_number() && result.number() != 0.0) ||
                    (result.is_string() && data.exists(result.string())))
                {
                    const auto& action_true_view = child_view.child_at(2);
                    line   = cline + 1;  // account for #if, etc.
                    column = 1;
                    if (!evaluate(data, action_true_view, out, line, column))
                    {
                        return false;
                    }
                    break;  // leave #if,elseif,else chain
                }
                continue;  // this if construct failed
            }
            else
            {
                Interpreter<S>::error_stream()
                    << "Error : conditional action (#if,#elseif, etc.) "
                       "must have a single argument. E.g., 'name' or "
                    << m_attribute_start_delim << "name"
                    << m_attribute_end_delim << "." << std::endl;
                return false;
            }
            wasp_not_implemented("parameterized conditional");
        }
        else
        {  // #else

            wasp_check(child_view.name() == std::string("else"));
            line   = child_view.line() + 1;
            column = 1;
            // #else followed by template lines... eval the lines
            for (size_t j = 1, count = child_view.child_count(); j < count; ++j)
            {
                const auto& else_child_view = child_view.child_at(j);
                if (!evaluate_component(data, else_child_view, out, line,
                                        column))
                {
                    return false;
                }
            }
            break;
        }
    }
    // capture any trailing newlines
    // between the last text
    // and the next action
    if (i + 1 < child_count)
    {
        wasp_check(i + 1 <= action_view.child_count());
        const auto& end_of_action = action_view.child_at(i + 1);
        int         delta         = end_of_action.line() - line;
        wasp_check(delta >= 0);
        wasp_tagged_line("conditional line delta " << delta << " between line "
                                                   << line << " and end action "
                                                   << info(end_of_action));
        if (delta > 0)
        {
            wasp_tagged_line("inserting " << delta << " newline(s).");
            out << std::string(delta, '\n');
        }
    }
    line = term_view.line() + 1;
    wasp_tagged_line("concluding condition starting on line " << line);
    column = 1;
    return true;
}

template<class S>
bool HaliteInterpreter<S>::import_file(DataAccessor&   data,
                                       const NodeView& import_view,
                                       std::ostream&   out,
                                       size_t&         line,
                                       size_t&         column)
{
    // attributes must have '#import txt'
    // e.g., #import txt or #import txt<a1>txt<a2>...
    wasp_require(import_view.child_count() > 1);
    size_t import_line = import_view.line();
    int    delta       = import_line - line;
    wasp_check(delta >= 0);
    if (delta > 0)
    {
        wasp_tagged_line("inserting " << delta << " newline(s).");
        out << std::string(delta, '\n');
    }

    std::stringstream import_str;
    // accumulate an attribute string
    for (size_t i = 1, count = import_view.child_count(); i < count; ++i)
    {
        const auto& child_view = import_view.child_at(i);
        auto        type       = child_view.type();
        switch (type)
        {
            case wasp::STRING:
                wasp::print(import_str, child_view);
                break;
            case wasp::IDENTIFIER:
                if (!print_attribute(data, child_view, import_str, import_line,
                                     column))
                {
                    return false;
                }
                break;
            default:
                wasp_not_implemented("parameterized file import '" +
                                     child_view.data() + "'");
        }
    }
    std::string        path            = import_str.str();
    static std::string using_str       = " using ";
    size_t             using_i         = path.find(using_str);
    bool               has_using       = using_i != std::string::npos;
    bool               using_reference = false;
    std::string        using_what;

    if (has_using)
    {
        using_what = path.substr(using_i + using_str.size());
        wasp_tagged_line("path is '" << path << "'");
        wasp_tagged_line("Found using stmt, import now '"
                         << path.substr(0, using_i) << "' using '" << using_what
                         << "'");
        path       = path.substr(0, using_i);
        using_what = wasp::strip_quotes(wasp::trim(using_what, " "));
        // the text after ' using ' is a variable name
        using_reference = data.exists(using_what);
        wasp_tagged_line("Using reference? " << std::boolalpha
                                             << using_reference);
    }
    path = wasp::trim(path, " \t");
    wasp_tagged_line("importing '"
                     << path << "' relative to '"
                     << wasp::dir_name(Interpreter<S>::stream_name()) << "'");

    std::ifstream relative_to_working_dir(path.c_str());
    std::string   relative_to_current_path =
        wasp::dir_name(Interpreter<S>::stream_name()) + "/" + path;
    std::ifstream        relative_to_current(relative_to_current_path.c_str());
    HaliteInterpreter<S> nested_interp(Interpreter<S>::error_stream());
    nested_interp.attr_start_delim() = this->attr_start_delim();
    nested_interp.attr_end_delim()   = this->attr_end_delim();

    bool                 import = false;
    bool                 parsed = false;
    if (!relative_to_working_dir.good())
    {
        if (!relative_to_current.good())
        {
            Interpreter<S>::error_stream()
                << "***Error : in '" << Interpreter<S>::stream_name()
                << "' on line " << import_line << " - unable to open '" << path
                << "' for import." << std::endl;
            return false;
        }
        nested_interp.setStreamName(relative_to_current_path, true);
        parsed = nested_interp.parse(relative_to_current);
    }
    else
    {
        nested_interp.setStreamName(path, true);
        parsed = nested_interp.parse(relative_to_working_dir);
    }
    if (parsed == false)
    {
        Interpreter<S>::error_stream()
            << "***Error : on line " << import_line << " of '"
            << Interpreter<S>::stream_name() << "' - unable to process '"
            << nested_interp.stream_name() << "'." << std::endl;
        return false;
    }
    wasp_tagged_line("importing");
    if (using_reference)
    {
        DataObject* obj   = nullptr;
        DataArray*  array = nullptr;
        wasp_tagged_line("Importing using_reference(" << using_what << ")");
        if ((obj = data.object(using_what)) != nullptr)
        {
            DataAccessor ref(obj, &data);
            import = nested_interp.evaluate(out, ref);
        }
        else if ((array = data.array(using_what)) != nullptr)
        {
            import = true;  // assume true ... catches empty array scenario
            wasp_tagged_line("importing " << array->size() << " times...");
            for (size_t array_i = 0; array_i < array->size(); ++array_i)
            {
                const auto& variable_at_i = array->at(array_i);
                if (variable_at_i.is_object())
                {
                    DataAccessor ref(variable_at_i.to_object(), &data);
                    import = nested_interp.evaluate(out, ref);
                    if (!import)
                    {
                        Interpreter<S>::error_stream()
                            << "***Error : on line " << import_line << " of '"
                            << Interpreter<S>::stream_name()
                            << "' - unable to import '"
                            << nested_interp.stream_name() << "'." << std::endl;
                        return false;
                    }
                }
                else
                {
                    wasp_not_implemented(
                        "iterative import using scalar elements");
                    // TODO - encapsulate element in object as a generically
                    // named child, 'value', etc.
                }
            }
        }
        else
        {
            wasp_not_implemented("import using scalar component");
        }
    }
    else if (has_using)
    {  // check that the component being used is a json payload
        if (using_what.size() > 0 && using_what.front() != '{' &&
            using_what.front() != '[')
        {
            // doesn't look like a json payload... wasn't a known parameter...
            // error
            Interpreter<S>::error_stream()
                << "***Error : unable to import '" << path << "' on line "
                << import_view.line() << ", the component being used '"
                << using_what << "' is not a known variable." << std::endl;
            return false;
        }
        std::stringstream data_by_copy_str;
        data_by_copy_str << using_what;
        DataObject::SP data_by_copy;
        if (!wasp::generate_object<JSONObjectParser>(
                data_by_copy, data_by_copy_str, Interpreter<S>::error_stream()))
        {
            return false;
        }
        DataAccessor data_by_copy_accessor(data_by_copy.get(), &data);
        import = nested_interp.evaluate(out, data_by_copy_accessor);
    }
    else
    {
        import = nested_interp.evaluate(out, data);
        if (!import)
        {
            Interpreter<S>::error_stream()
                << "***Error : on line " << import_line << " of '"
                << Interpreter<S>::stream_name() << "' - unable to process '"
                << nested_interp.stream_name() << "'." << std::endl;
        }
    }

    column = 1;
    line += delta + 2;
    wasp_tagged_line("import successful? " << std::boolalpha << import);
    return import;
}
template<class S>
bool HaliteInterpreter<S>::repeat_file(DataAccessor&   data,
                                       const NodeView& repeat_view,
                                       std::ostream&   out,
                                       size_t&         line,
                                       size_t&         column)
{
    // attributes must have '#repear txt'
    // e.g., #repeat txt or #repeat txt<a1>txt<a2>...
    wasp_require(repeat_view.child_count() > 1);
    size_t repeat_line = repeat_view.line();
    int    delta       = repeat_line - line;
    wasp_check(delta >= 0);
    if (delta > 0)
    {
        wasp_tagged_line("inserting " << delta << " newline(s) before repeat.");
        out << std::string(delta, '\n');
    }

    std::stringstream repeat_str;
    // accumulate an attribute string
    for (size_t i = 1, count = repeat_view.child_count(); i < count; ++i)
    {
        const auto& child_view = repeat_view.child_at(i);
        auto        type       = child_view.type();
        switch (type)
        {
            case wasp::STRING:
                wasp::print(repeat_str, child_view);
                break;
            case wasp::IDENTIFIER:
                print_attribute(data, child_view, repeat_str, repeat_line,
                                column);
                break;
            default:
                wasp_not_implemented("parameterized file repeat '" +
                                     child_view.data() + "'");
        }
    }
    std::string        path      = repeat_str.str();
    static std::string using_str = " using ";
    size_t             using_i   = path.find(using_str);
    bool               has_using = using_i != std::string::npos;

    std::string        using_what;
    std::vector<Range> imports;
    if (has_using)
    {
        using_what = path.substr(using_i + using_str.size());
        wasp_tagged_line("path is '" << path << "'");
        wasp_tagged_line("Found using stmt, import now '"
                         << path.substr(0, using_i) << "' using '" << using_what
                         << "'");
        path       = path.substr(0, using_i);
        using_what = wasp::trim(using_what, " ");

        std::string error_msg;
        if (!extract_ranges(using_what, imports, error_msg))
        {
            Interpreter<S>::error_stream()
                << "***Error : on line " << repeat_line
                << " - unable to extract file repeat range info; " << error_msg
                << "." << std::endl;
            return false;
        }
    }
    path = wasp::trim(path, " \t");
    wasp_tagged_line("repeating '"
                     << path << "' relative to '"
                     << wasp::dir_name(Interpreter<S>::stream_name()) << "'");
    std::ifstream relative_to_working_dir(path.c_str());
    std::string   relative_to_current_path =
        wasp::dir_name(Interpreter<S>::stream_name()) + "/" + path;
    std::ifstream        relative_to_current(relative_to_current_path.c_str());
    HaliteInterpreter<S> nested_interp(Interpreter<S>::error_stream());
    bool                 import = false;
    bool                 parsed = false;
    if (!relative_to_working_dir.good())
    {
        if (!relative_to_current.good())
        {
            Interpreter<S>::error_stream()
                << "***Error : unable to open '" << path << "'." << std::endl;
            return false;
        }
        nested_interp.setStreamName(relative_to_current_path, true);
        parsed = nested_interp.parse(relative_to_current);
    }
    else
    {
        nested_interp.setStreamName(path, true);
        parsed = nested_interp.parse(relative_to_working_dir);
    }
    if (parsed == false)
    {
        return false;
    }
    wasp_tagged_line("importing");
    if (has_using)
    {
        // This logic is proof of concept and will not function with multiple
        // variables
        wasp_check(imports.empty() == false);
        DataObject   o;
        DataAccessor import_data(&o, &data);
        import = import_range(import_data, nested_interp, imports, 0, out);
    }
    else
    {
        import = nested_interp.evaluate(out, data);
    }
    // update contet information:
    // Repeats require a line, and always produce
    // 1) a new line when content is present.
    // 2) a newline to ensure any new content does not emit an extraneous
    // newline
    line += delta + 1;
    // Because repeat requires its own line, column will be reset back to 1.
    column = 1;
    return import;
}

template<class S>
bool HaliteInterpreter<S>::extract_ranges(std::string         range_data,
                                          std::vector<Range>& ranges,
                                          std::string&        error)
{
    size_t assign_i = range_data.find("=");
    if (assign_i == std::string::npos)
    {
        error =
            "unable to determine range variable - missing assign '=' character";
        return false;
    }
    std::string variable = trim(range_data.substr(0, assign_i), " ");
    wasp_tagged_line("Variable = " << variable);

    size_t delim_i = range_data.find_first_of(",;");
    int    start = 0, end = 0, stride = 1;

    // extract start
    if (delim_i == std::string::npos)
    {  // no terminator delim exists 'var = x'

        if (assign_i + 1 >= range_data.size())
        {
            error = "no range start was specified for '" + variable + "'";
            return false;
        }
        bool ok = false;
        to_type(start, range_data.substr(assign_i + 1), &ok);
        if (!ok)
        {
            error = "unable to extract range start for '" + variable + "'";
            return false;
        }
        end = start;  // default
        ranges.push_back(Range(variable, start, end, stride));
        return true;
    }
    else
    {
        bool               ok     = false;
        size_t             length = delim_i - 1 - assign_i;
        const std::string& start_str =
            trim(range_data.substr(assign_i + 1, length), " ");
        to_type(start, start_str, &ok);
        wasp_tagged_line("start '" << start_str << "' - " << std::boolalpha
                                   << ok);
        if (!ok)
        {
            error = "unable to extract delimited range start for '" + variable +
                    "'";
            return false;
        }
        end = start;
    }
    size_t start_delim_i = delim_i;
    wasp_check(delim_i < range_data.size());
    bool has_more = range_data[start_delim_i] == ',';

    if (has_more)
    {
        // extract end
        delim_i = range_data.find_first_of(",;", start_delim_i + 1);
        if (delim_i == std::string::npos)
        {  // no terminator delim exists ', end'

            if (start_delim_i + 1 >= range_data.size())
            {
                error = "no range end was specified for '" + variable + "'";
                return false;
            }
            bool               ok        = false;
            const std::string& range_end = range_data.substr(start_delim_i + 1);
            wasp_tagged_line("range end is '" << range_end << "'");
            to_type(end, range_end, &ok);
            if (!ok)
            {
                error = "unable to extract range end for '" + variable + "'";
                return false;
            }
            ranges.push_back(Range(variable, start, end, stride));
            return true;
        }
        else
        {
            bool               ok     = false;
            size_t             length = delim_i - 1 - start_delim_i;
            const std::string& range_end =
                range_data.substr(start_delim_i + 1, length);
            wasp_tagged_line("range end '" << range_end << "'");
            to_type(end, range_end, &ok);
            if (!ok)
            {
                error = "unable to extract delimited range end for '" +
                        variable + "'";
                return false;
            }
        }

        // extract stride
        size_t end_delim_i = delim_i;
        has_more           = range_data[end_delim_i] == ',';
        if (has_more)  // stride available
        {
            wasp_check(delim_i < range_data.size());
            // extract stride
            delim_i = range_data.find_first_of(";", end_delim_i + 1);
            if (delim_i == std::string::npos)
            {  // no terminator delim exists ', end'

                if (end_delim_i + 1 >= range_data.size())
                {
                    error =
                        "no range stride was specified for '" + variable + "'";
                    return false;
                }
                bool               ok = false;
                const std::string& range_stride =
                    range_data.substr(end_delim_i + 1);
                wasp_tagged_line("range stride is '" << range_stride << "'");
                to_type(stride, range_stride, &ok);
                if (!ok)
                {
                    error =
                        "unable to extract range stride for '" + variable + "'";
                    return false;
                }
                ranges.push_back(Range(variable, start, end, stride));
                return true;
            }
            else
            {
                bool   ok     = false;
                size_t length = delim_i - end_delim_i;
                to_type(stride, range_data.substr(end_delim_i + 1, length),
                        &ok);
                if (!ok)
                {
                    error = "unable to extract delimited range stride for '" +
                            variable + "'";
                    return false;
                }
            }
        }  // end of stride
    }      // end of 'end' and possible 'stride' acquisition
    wasp_tagged_line("pushing " << variable << "," << start << "," << end << ","
                                << stride);
    ranges.push_back(Range(variable, start, end, stride));
    if (delim_i != std::string::npos && delim_i + 1 < range_data.size())
    {
        // check for additional ranges after a semi-colon
        wasp_tagged_line("delim_i = " << delim_i << " vs" << range_data.size()
                                      << " has_more " << std::boolalpha
                                      << has_more);

        range_data = trim(range_data.substr(delim_i + 1), " ");
        if (range_data.empty())
            return true;
        wasp_tagged_line("recursing with remaining range '" << range_data
                                                            << "'");
        return extract_ranges(range_data, ranges, error);
    }

    return true;
}

template<class S>
bool HaliteInterpreter<S>::import_range(DataAccessor&         data,
                                        HaliteInterpreter<S>& file_interpreter,
                                        const std::vector<Range>& imports,
                                        size_t                    i,
                                        std::ostream&             out)
{
    wasp_require(i < imports.size());
    wasp_tagged_line("looping " << imports[i].name);
    for (int r = imports[i].start; r <= imports[i].end; r += imports[i].stride)
    {
        wasp_tagged_line("looping r=" << r << " from " << imports[i].start
                                      << " to " << imports[i].end);
        data.store(imports[i].name, r);
        // bottom of recursion - evaluate
        if (i + 1 == imports.size())
        {
            if (!file_interpreter.evaluate(out, data))
            {
                wasp_tagged_line("failing iterative file import");
                return false;
            }
        }
        else if (!import_range(data, file_interpreter, imports, i + 1, out))
        {
            return false;
        }
    }
    return true;
}

template<class S>
void HaliteInterpreter<S>::capture_attribute_text(const std::string& text,
                                                  size_t             offset,
                                                  bool extract_options)
{
    size_t options_index =
        extract_options ? text.find(m_attribute_options_delim) : text.size();
    bool contains_options =
        extract_options && options_index != std::string::npos;
    if (!contains_options)
        options_index = text.size();

    // capture text
    capture_leaf("txt", wasp::STRING, text.substr(0, options_index),
                 wasp::STRING, offset);
    size_t options_text_size = text.size() - options_index;
    // capture attribute options
    if (options_text_size > 0)
    {
        capture_leaf("opt", wasp::FUNCTION,
                     text.substr(options_index, options_text_size),
                     wasp::STRING, offset + options_index);
    }
}

template<class S>
void HaliteInterpreter<S>::capture_attribute_delim(const std::string& data,
                                                   size_t& current_column_index,
                                                   size_t  attribute_end_index)
{
    wasp_require(attribute_end_index >= current_column_index);
    size_t remaining_length = attribute_end_index - current_column_index;
    // capture text, if any
    if (remaining_length > 0)
    {
        size_t file_offset = m_file_offset + current_column_index;
        capture_attribute_text(
            data.substr(current_column_index, remaining_length), file_offset,
            true);
    }
    // capture attribute terminator
    size_t file_offset = m_file_offset + attribute_end_index;
    capture_leaf(m_attribute_end_delim, wasp::TERM, m_attribute_end_delim,
                 wasp::STRING, file_offset);
    // close/commit the subtree
    wasp_check(Interpreter<S>::staged_count() > 1);
    Interpreter<S>::commit_staged(Interpreter<S>::staged_count() - 1);
    // update current column to end of delimiter
    current_column_index = attribute_end_index + m_attribute_end_delim.size();
}
template<class S>
bool HaliteInterpreter<S>::attribute_options(SubstitutionOptions& options,
                                             const std::string&   data,
                                             size_t               line)
{
    wasp_tagged_line("getting options for '" << data << "'");
    static std::string fmt = "fmt=", sep = "sep=", use = "use=",
                       emits      = "emit=";
    size_t            format_i    = data.find(fmt);
    size_t            separator_i = data.find(sep);
    size_t            use_i       = data.find(use);
    size_t            emit_i      = data.find(emits);
    std::stringstream iterative_options_str;
    size_t            start_i = 1;
    if (data.size() > 1)
    {
        switch (data[1])
        {
            case '?':
                options.optional() = true;
                start_i            = 2;
                break;
            case '|':
                options.silent() = true;
                start_i          = 2;
                break;
        }
    }
    std::set<std::pair<size_t, size_t>> intervals;
    if (format_i != std::string::npos)
    {
        size_t term_i  = data.find(';', format_i);
        size_t length  = data.size() - (format_i + fmt.size());
        size_t delim_s = 0;
        if (term_i != std::string::npos)
        {
            length  = term_i - (format_i + fmt.size());
            delim_s = 1;
        }
        options.format() = data.substr(format_i + fmt.size(), length);
        wasp_tagged_line("Format of '" << options.format() << "' captured");
        // capture the format text interval
        intervals.insert(
            std::make_pair(format_i, format_i + fmt.size() + length + delim_s));
    }
    if (separator_i != std::string::npos)
    {
        size_t term_i  = data.find(';', separator_i);
        size_t length  = data.size() - (separator_i + sep.size());
        size_t delim_s = 0;
        if (term_i != std::string::npos)
        {
            length  = term_i - (separator_i + sep.size());
            delim_s = 1;
        }
        options.separator() = data.substr(separator_i + sep.size(), length);
        wasp_tagged_line("Separator of '" << options.separator()
                                          << "' captured");
        // capture the separator text interval
        intervals.insert(std::make_pair(
            separator_i, separator_i + sep.size() + length + delim_s));
    }
    if (use_i != std::string::npos)
    {
        size_t term_i  = data.find(';', use_i);
        size_t length  = data.size() - (use_i + use.size());
        size_t delim_s = 0;
        if (term_i != std::string::npos)
        {
            length  = term_i - (use_i + use.size());
            delim_s = 1;
        }
        options.use() = trim(data.substr(use_i + use.size(), length), " ");
        wasp_tagged_line("use of '" << options.use() << "' captured");
        // capture the use text
        intervals.insert(
            std::make_pair(use_i, use_i + use.size() + length + delim_s));
    }
    if (emit_i != std::string::npos)
    {
        size_t term_i  = data.find(';', emit_i);
        size_t length  = data.size() - (emit_i + emits.size());
        size_t delim_s = 0;
        if (term_i != std::string::npos)
        {
            length  = term_i - (emit_i + emits.size());
            delim_s = 1;
        }
        // emit is of the form emit=delim , stride
        // delim is captured as a string
        // stride must be a non-negative integer

        options.emit_delim() =
            trim(data.substr(emit_i + emits.size(), length), " ");
        wasp_tagged_line("emit of '" << options.emit_delim() << "' captured");
        // TODO - what if emit_delim is a comma itself?
        size_t comma_i = options.emit_delim().find(',');
        if (comma_i != std::string::npos)
        {
            std::stringstream s_stride;
            s_stride << options.emit_delim().substr(comma_i + 1);
            wasp_tagged_line("Emit stride identified as '" << s_stride.str()
                                                           << "'");
            options.emit_delim().erase(comma_i);
            s_stride >> options.emit_stride();
            if (s_stride.fail())
            {
                Interpreter<S>::error_stream()
                    << "***Error : the emit stride failed to be processed on "
                       "line "
                    << line << ". Looking for a non-negative integer, found '"
                    << s_stride.str() << "'." << std::endl;
                return false;
            }
            else if (options.emit_stride() < 0)
            {
                Interpreter<S>::error_stream()
                    << "***Error : the emit stride on line " << line
                    << " must be a non-negative integer."
                       " Found '"
                    << s_stride.str() << "' processed as '"
                    << options.emit_stride() << "'." << std::endl;
                return false;
            }
            wasp_tagged_line("Processed emit with delim='"
                             << options.emit_delim() << "'"
                             << " and stride='" << options.emit_stride()
                             << "'");
        }
        // capture the emit text
        intervals.insert(
            std::make_pair(emit_i, emit_i + emits.size() + length + delim_s));
    }
    size_t last_i = start_i;
    for (auto itr = intervals.begin(); itr != intervals.end(); itr++)
    {
        if (last_i > itr->first && last_i <= itr->second)
        {
            Interpreter<S>::error_stream()
                << "***Error : a delimiter appears to be missing on line "
                << line << "." << std::endl;
            return false;
        }
        if (last_i < itr->first)
        {
            const std::string& extra = data.substr(last_i, itr->first - last_i);
            wasp_tagged_line("found extra '" << extra << "'");
            iterative_options_str << extra;
        }
        last_i = itr->second;
    }
    // capture trailing
    if (last_i < data.size())
    {
        const std::string& extra = data.substr(last_i, data.size() - last_i);
        wasp_tagged_line("found extra '" << extra << "'");
        iterative_options_str << extra;
    }
    std::string iterative_options =
        wasp::trim(iterative_options_str.str(), " ");
    wasp_tagged_line("iterative options '" << iterative_options << "'");
    if (!iterative_options.empty())
    {
        std::string error;
        bool        extracted =
            extract_ranges(iterative_options, options.ranges(), error);
        if (!extracted)
        {
            Interpreter<S>::error_stream()
                << "***Error : unable to acquire attribute options on line "
                << line << "; " << error << "." << std::endl;
            return false;
        }
    }
    return true;
}
}  // namespace wasp
#endif
