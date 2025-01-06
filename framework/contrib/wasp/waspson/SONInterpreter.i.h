#ifndef WASP_SONINTERPRETER_I_H
#define WASP_SONINTERPRETER_I_H

template<class S>
SONInterpreter<S>::SONInterpreter()
    : Interpreter<S>()
    , traceLexing(false)
    , traceParsing(false)
    , singleParse(false)
    , mHasFile(false)
    , m_parent(nullptr)
{
}
template<class S>
SONInterpreter<S>::SONInterpreter(std::ostream& err)
    : Interpreter<S>(err)
    , traceLexing(false)
    , traceParsing(false)
    , singleParse(false)
    , mHasFile(false)
    , m_parent(nullptr)
{
}
template<class S>
SONInterpreter<S>::~SONInterpreter()
{
}
template<class S>
bool SONInterpreter<S>::parse(std::istream& in,
                              size_t        startLine,
                              size_t        startColumn)
{
    return parseStream(
        in, hasFile() ? Interpreter<S>::stream_name() : "stream input",
        startLine, startColumn);
}
template<class S>
bool SONInterpreter<S>::parseStream(std::istream&      in,
                                    const std::string& sname,
                                    size_t             start_line,
                                    size_t             start_column)
{
    return Interpreter<S>::template parse_impl<SONParser>(in, sname, start_line,
                                                          start_column);
}
template<class S>
bool SONInterpreter<S>::parseFile(const std::string& filename, size_t line)
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
bool SONInterpreter<S>::parseString(const std::string& input,
                                    const std::string& sname,
                                    size_t             startLine,
                                    size_t             startColumn)
{
    std::istringstream iss(input);
    return parseStream(iss, sname, startLine, startColumn);
}

template<class S>
SONInterpreter<S>* SONInterpreter<S>::create_nested_interpreter(Interpreter<S>* parent)
{
    wasp_require(parent);
    auto* interp = new SONInterpreter<S>(parent->error_stream());
    wasp_insist(dynamic_cast<SONInterpreter<S>*>(parent) != nullptr, 
        "parent interpreter must be the same type");
    interp->m_parent = dynamic_cast<SONInterpreter<S>*>(parent);

    // Make sure the nested interpreter knows to search relative from
    // the directory from which it is being included
    interp->search_paths().push_back(wasp::dir_name(parent->stream_name()));
    return interp;
}
#endif
