#ifndef WASP_DDINTERPRETER_I_H
#define WASP_DDINTERPRETER_I_H

template<class S>
DDInterpreter<S>::DDInterpreter()
    : Interpreter<S>()
    , traceLexing(false)
    , traceParsing(false)
    , singleParse(false)
    , m_current(nullptr)
    , mHasFile(false)
{
    definition();  // create empty definition needed by interpreter
}
template<class S>
DDInterpreter<S>::DDInterpreter(std::ostream& err)
    : Interpreter<S>(err)
    , traceLexing(false)
    , traceParsing(false)
    , singleParse(false)
    , m_current(nullptr)
    , mHasFile(false)
{
    definition();  // create empty definition needed by interpreter
}
template<class S>
DDInterpreter<S>::~DDInterpreter()
{
    m_current = nullptr;
}
template<class S>
bool DDInterpreter<S>::parse(std::istream& in,
                             size_t        startLine,
                             size_t        startColumn)
{
    return parseStream(
        in, hasFile() ? Interpreter<S>::stream_name() : "stream input",
        startLine, startColumn);
}
template<class S>
bool DDInterpreter<S>::parseStream(std::istream&      in,
                                   const std::string& sname,
                                   size_t             start_line,
                                   size_t             start_column)
{
    return Interpreter<S>::template parse_impl<DDIParser>(in, sname, start_line,
                                                          start_column);
}
template<class S>
bool DDInterpreter<S>::parseFile(const std::string& filename, size_t line)
{
    std::ifstream in(filename.c_str());
    if (!in.good())
    {
        Interpreter<S>::error_stream()
            << "file '" << filename
            << "' is either inaccessible or doesn't exist! Unable to read."
            << std::endl;
        return false;
    }
    mHasFile = true;
    return parseStream(in, filename, line);
}
template<class S>
bool DDInterpreter<S>::parseString(const std::string& input,
                                   const std::string& sname,
                                   size_t             startLine,
                                   size_t             startColumn)
{
    std::istringstream iss(input);
    return parseStream(iss, sname, startLine, startColumn);
}

template<class S>
const AbstractDefinition* DDInterpreter<S>::definition() const
{
    wasp_require(m_current != nullptr);
    return m_current;
}
template<class S>
AbstractDefinition* DDInterpreter<S>::definition()
{
    if (m_current == nullptr)
    {
        m_definition = std::make_shared<Definition>();
        m_current    = &(*m_definition);
    }
    return m_current;
}
template<class S>
size_t DDInterpreter<S>::push_staged(size_t                     node_type,
                                     const std::string&         node_name,
                                     const std::vector<size_t>& child_indices)
{
    auto stage_count =
        Interpreter<S>::push_staged(node_type, node_name, child_indices);
    wasp_check(m_current->has(node_name));
    m_current = m_current->get(node_name);  // push new definition
    return stage_count;
}
template<class S>
size_t DDInterpreter<S>::commit_staged(size_t stage_index)
{
    auto node_index = Interpreter<S>::commit_staged(stage_index);
    wasp_check(m_current);
    m_current = m_current->parent();  // pops current definition
    return node_index;
}
#endif
