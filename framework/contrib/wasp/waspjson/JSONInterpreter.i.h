#ifndef WASP_JSONINTERPRETER_I_H
#define WASP_JSONINTERPRETER_I_H

template<class S>
JSONInterpreter<S>::JSONInterpreter()
    : Interpreter<S>()
    , traceLexing(false)
    , traceParsing(false)
    , singleParse(false)
    , mHasFile(false)
{
}
template<class S>
JSONInterpreter<S>::JSONInterpreter(std::ostream& err)
    : Interpreter<S>(err)
    , traceLexing(false)
    , traceParsing(false)
    , singleParse(false)
    , mHasFile(false)
{
}
template<class S>
JSONInterpreter<S>::~JSONInterpreter()
{
}
template<class S>
bool JSONInterpreter<S>::parse(std::istream& in,
                               size_t        startLine,
                               size_t        startColumn)
{
    return parseStream(
        in, hasFile() ? Interpreter<S>::stream_name() : "stream input",
        startLine, startColumn);
}
template<class S>
bool JSONInterpreter<S>::parseStream(std::istream&      in,
                                     const std::string& sname,
                                     size_t             start_line,
                                     size_t             start_column)
{
    return Interpreter<S>::template parse_impl<JSONParser>(
        in, sname, start_line, start_column);
}
template<class S>
bool JSONInterpreter<S>::parseFile(const std::string& filename, size_t line)
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
bool JSONInterpreter<S>::parseString(const std::string& input,
                                     const std::string& sname,
                                     size_t             startLine,
                                     size_t             startColumn)
{
    std::istringstream iss(input);
    return parseStream(iss, sname, startLine, startColumn);
}

template<class S>
bool JSONInterpreter<S>::generate_object(DataObject&   obj,
                                         std::ostream& err) const
{
    auto root_view = Interpreter<S>::root();
    if (root_view.is_null())
        return true;  // empty file OK
    return generate_object_internal(root_view, obj, err);
}

template<class S>
bool JSONInterpreter<S>::generate_object_internal(const NodeView& view,
                                                  DataObject&     obj,
                                                  std::ostream&   err) const
{
    wasp_require(view.is_null() == false);
    for (size_t i = 0, count = view.child_count(); i < count; ++i)
    {
        auto        child_view = view.child_at(i);
        auto        child_type = child_view.type();
        std::string child_name = child_view.name();
        switch (child_type)
        {
            case wasp::DECL:
            case wasp::ASSIGN:  // :
            case wasp::LBRACE:
            case wasp::RBRACE:
            case wasp::WASP_COMMA:
                continue;  // skip decorative
            case wasp::OBJECT:
            {
                obj[child_name] = DataObject();
                wasp_check(Value::TYPE_OBJECT == obj[child_name].type());
                if (false == generate_object_internal(
                                 child_view, *obj[child_name].to_object(), err))
                {
                    return false;
                }
                break;
            }
            case wasp::ARRAY:
            {
                obj[child_name] = DataArray();
                wasp_check(Value::TYPE_ARRAY == obj[child_name].type());
                if (false == generate_array_internal(
                                 child_view, *obj[child_name].to_array(), err))
                {
                    return false;
                }
                break;
            }
            case wasp::KEYED_VALUE:
            {
                // 3 children required - key : value
                wasp_check(child_view.child_count() == 3);
                auto value_view = child_view.child_at(2);
                auto result_pair =
                    obj.insert(std::make_pair(child_name, Value()));
                if (result_pair.second == false)
                {
                    err << "***Error : value component '" << child_name
                        << "' at line " << child_view.line() << " and column "
                        << child_view.column() << " is duplicate. "
                        << std::endl;
                    return false;
                }
                if (false == generate_value_internal(
                                 value_view, result_pair.first->second, err))
                {
                    return false;
                }

                break;
            }
            default:
                err << "***Error : object component '" << child_name
                    << "' at line " << child_view.line() << " and column "
                    << child_view.column() << " is of an unknown type ("
                    << child_type << ")" << std::endl;
                return false;
        }
    }
    return true;
}

template<class S>
bool JSONInterpreter<S>::generate_array_internal(const NodeView& view,
                                                 DataArray&      array,
                                                 std::ostream&   err) const
{
    wasp_require(view.is_null() == false);
    size_t child_index = 0;
    for (size_t i = 0, count = view.child_count(); i < count; ++i)
    {
        auto child_view = view.child_at(i);
        auto child_type = child_view.type();

        if (child_type == wasp::DECL || child_type == wasp::ASSIGN ||
            child_type == wasp::LBRACKET || child_type == wasp::RBRACKET ||
            child_type == wasp::WASP_COMMA)
        {
            continue;  // skip decorative
        }
        std::string child_name = child_view.name();
        switch (child_type)
        {
            case wasp::OBJECT:
            {
                array[child_index] = DataObject();
                wasp_check(Value::TYPE_OBJECT == array[child_index].type());
                if (false ==
                    generate_object_internal(
                        child_view, *array[child_index].to_object(), err))
                {
                    return false;
                }
                break;
            }
            case wasp::ARRAY:
            {
                array[child_index] = DataArray();
                wasp_check(Value::TYPE_ARRAY == array[child_index].type());
                if (false ==
                    generate_array_internal(
                        child_view, *array[child_index].to_array(), err))
                {
                    return false;
                }
                break;
            }
            case wasp::VALUE:

                if (false == generate_value_internal(child_view,
                                                     array[child_index], err))
                {
                    return false;
                }
                break;
            default:
                err << "***Error : array element '" << child_name
                    << "' at line " << child_view.line() << " and column "
                    << child_view.column() << " is of an unknown type ("
                    << child_type << ")" << std::endl;
                return false;
        }
        ++child_index;
    }
    return true;
}
template<class S>
bool JSONInterpreter<S>::generate_value_internal(const NodeView& value_view,
                                                 wasp::Value&    value,
                                                 std::ostream&   err) const
{
    wasp_check(value_view.is_null() == false);
    auto value_type = value_view.token_type();
    switch (value_type)
    {
        case wasp::WASP_NULL:
            value = Value();  // null value
            break;
        case wasp::INTEGER:
            value = value_view.to_int();
            break;
        case wasp::REAL:
            value = value_view.to_double();
            break;
        case wasp::WASP_TRUE:
        case wasp::WASP_FALSE:
            value = value_view.to_bool();
            break;
        case wasp::QUOTED_STRING:
            value = value_view.to_string();
            break;
        default:
            err << "***Error : JSON Value at line " << value_view.line()
                << " and column " << value_view.column()
                << " is an unsupported type. Only true, false, null,"
                << " number, and quoted string are supported." << std::endl;
            return false;
    }
    return true;
}

#endif
