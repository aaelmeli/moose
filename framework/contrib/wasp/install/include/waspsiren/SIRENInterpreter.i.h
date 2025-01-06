#ifndef SIREN_SIRENINTERPRETER_I_H
#define SIREN_SIRENINTERPRETER_I_H
template<class S>
SIRENInterpreter<S>::SIRENInterpreter()
    : Interpreter<S>(), traceLexing(false), traceParsing(false)
{
}
template<class S>
SIRENInterpreter<S>::SIRENInterpreter(std::ostream& err)
    : Interpreter<S>(err), traceLexing(false), traceParsing(false)
{
}
template<class S>
SIRENInterpreter<S>::~SIRENInterpreter()
{
}
template<class S>
bool SIRENInterpreter<S>::parse(std::istream& in,
                                std::size_t   startLine,
                                std::size_t   startColumn)
{
    return parseStream(in, "selection statement", startLine, startColumn);
}
template<class S>
bool SIRENInterpreter<S>::parseStream(std::istream&      in,
                                      const std::string& sname,
                                      std::size_t        start_line,
                                      std::size_t        start_column)
{
    return Interpreter<S>::template parse_impl<SIRENParser>(
        in, sname, start_line, start_column);
}
template<class S>
bool SIRENInterpreter<S>::parseString(const std::string& input,
                                      const std::string& sname,
                                      std::size_t        startLine,
                                      std::size_t        startColumn)
{
    std::istringstream iss(input);
    return parseStream(iss, sname, startLine, startColumn);
}

template<class S>
template<typename TAdapter>
std::size_t
SIRENInterpreter<S>::evaluate(TAdapter&                 node,
                              SIRENResultSet<TAdapter>& result) const
{
    // the first selection
    // is either a document root-based selection
    // or a node-relative selection
    NodeView selection_root = Interpreter<S>::root();

    if (selection_root.child_count() == 0)
        return 0;

    NodeView first_selection = selection_root.child_at(0);

    // if root-based, need to select the root of the document
    // relative to the node given
    // When first selection is '/', the user wants to only select the root
    bool is_root_oriented =
        first_selection.type() == wasp::DOCUMENT_ROOT
        // first selection of 'any' is a root oriented selection
        || first_selection.type() == wasp::ANY;
    bool is_only_root_oriented = is_root_oriented &&
                                 first_selection.child_count() <=
                                     1;  // is it only '/', not '/' and 'child'

    std::vector<TAdapter> stage;
    if (is_root_oriented)
    {
        TAdapter anode(node);
        while (anode.has_parent())
        {
            anode = anode.parent();
        }
        // the root of the document
        if (is_only_root_oriented)
        {
            result.push(anode);
            return result.result_count();
        }
        stage.push_back(anode);
    }
    else
    {
        stage.push_back(node);
    }

    evaluate(first_selection, result, stage);
    for (std::size_t i = 0; i < stage.size(); ++i)
    {
        result.push(stage[i]);
    }
    return result.result_count();
}
template<class S>
template<typename TAdapter>
std::size_t SIRENInterpreter<S>::evaluate(const NodeView&           context,
                                          SIRENResultSet<TAdapter>& result,
                                          std::vector<TAdapter>&    stage) const
{
    switch (context.type())
    {
        default:
            return -1;  // ERROR - unsupported type (underflow)
            break;
        case DOCUMENT_ROOT:
            if (context.child_count() > 0)
            {  // '/' relative_selection
                NodeView child_context = context.child_at(1);
                evaluate(child_context, result, stage);
            }
            break;
        case SEPARATOR:
            break;
        case DECL:  // named child search
            search_child_name(context, stage);
            break;
        case PARENT:  // select parent of current nodes '..'
        {
            std::size_t count = stage.size();
            for (std::size_t i = 0; i < count; ++i)
            {
                const TAdapter& node = stage[i];
                if (node.has_parent())
                    stage.push_back(node.parent());
            }
            stage.erase(stage.begin(), stage.begin() + count);
        }
        break;
        case OBJECT:  // selection / selection
        {
            NodeView left_selection = context.child_at(0);
            // TODO - ensure selections are legit
            if (evaluate(left_selection, result, stage) > 0)
            {
                NodeView right_selection = context.child_at(2);
                evaluate(right_selection, result, stage);
            }
        }
        break;
        case PREDICATED_CHILD:  // obj[id=value] or obj[1:3:2]
        {
            // 'obj' = [0]
            // '['   = [1]
            // 'id=value' | '1:3:2' = [2]
            NodeView predicate_node = context.child_at(2);
            if (predicate_node.type() == KEYED_VALUE)
            {
                search_conditional_predicated_child(context, stage);
            }
            else if (predicate_node.type() == INDEX)
            {
                search_index_predicated_child(context, stage);
            }
        }
        break;
    }  // end of switch on context type

    return stage.size();
}
template<class S>
template<typename TAdapter>
void SIRENInterpreter<S>::search_child_name(const NodeView&        context,
                                            std::vector<TAdapter>& stage) const
{
    if (stage.empty())
        return;
    // the name for which to search
    const char* name       = context.name();
    std::size_t stage_size = stage.size();
    for (std::size_t index = 0; index < stage_size; ++index)
    {
        TAdapter node = stage[index];
        auto itr = node.begin();
        while(itr)
        {
            const TAdapter& child_node = itr.get();
            // if child is a match, push back onto stage
            if (wildcard_string_match(name, child_node.name()))
            {
                stage.push_back(child_node);
            }
            itr.next();
        }
    }
    stage.erase(stage.begin(), stage.begin() + stage_size);
}
template<class S>
template<typename TAdapter>
void SIRENInterpreter<S>::search_conditional_predicated_child(
    const NodeView& context, std::vector<TAdapter>& stage) const
{
    // context should be something like
    // obj [ id=value ]
    // obj = [0]
    // '[' = [1]
    // 'id=value' = [2]
    // ']' = [3]
    // TODO - ensure context fulfills expectations
    NodeView child_name_context = context.child_at(0);
    // predicate context should be something like
    // id = value
    // id = [0]
    // '=' = [1]
    // value = [2]
    NodeView predicate_context = context.child_at(2);
    // TODO - ensure predicate context fulfills expectations
    NodeView predicate_name_context  = predicate_context.child_at(0);
    NodeView predicate_value_context = predicate_context.child_at(2);

    // the names for which to search
    const char*        name            = child_name_context.name();
    const char*        predicate_name  = predicate_name_context.name();
    const std::string& predicate_value = predicate_value_context.data();
    std::size_t        stage_size      = stage.size();
    for (std::size_t index = 0; index < stage_size; ++index)
    {
        TAdapter node = stage[index];
        for (auto citr = node.begin(); citr != node.end(); citr.next())
        {
            const TAdapter& child_node = citr.get();
            // if child is a match, push back onto stage
            if (wildcard_string_match(name, child_node.name()))
            {
                // prior to pushing, must determine if the
                // predicate passes
                // TODO - added expression evaluator
                // for proper robustness.
                // string compare fails quickly '1' == '1.0' fails, but should
                // not considering user is expecting it is a numeric comparison.
                bool predicate_accepted = false;  // assume predicate fails
                for (auto gitr = child_node.begin(); gitr != child_node.end();
                     gitr.next())
                {
                    const TAdapter& g_child_node = gitr.get();
                    // if grand child name is a match, need to determine
                    // if value matches
                    if (wildcard_string_match(predicate_name,
                                              g_child_node.name()))
                    {
                        const std::string& g_child_node_value =
                            g_child_node.data();
                        predicate_accepted =
                            predicate_value == g_child_node_value;
                        if (predicate_accepted)
                        {
                            break;  // break from grandchild loop
                        }
                    }
                }
                if (predicate_accepted)
                {
                    stage.push_back(child_node);
                }
            }
        }
    }
    stage.erase(stage.begin(), stage.begin() + stage_size);
}
template<class S>
template<typename TAdapter>
void SIRENInterpreter<S>::search_index_predicated_child(
    const NodeView& context, std::vector<TAdapter>& stage) const
{
    // context should be something like
    // obj [ 1 ] | obj [ 1:10 ] | obj [ 1:10:2 ]
    // obj = [0]
    // '[' = [1]
    // index '1' | '1:10' | '1:10:2' = [2]
    // ']' = [3]
    // TODO - ensure context fulfills expectations
    NodeView child_name_context = context.child_at(0);
    // predicate context should be something like
    // index  - 1 child
    // start : end - 3 children
    // start : end : stride - 5 children
    NodeView predicate_context = context.child_at(2);
    // TODO - ensure predicate context fulfills expectations
    std::size_t start_i = 0, end_i = 0, stride = 1;
    std::size_t incident_count = 0;

    // the names for which to search
    const char* name = child_name_context.name();

    // single index selection - start = end, stride =1
    if (predicate_context.child_count() == 1)
    {  // TODO check type
        NodeView index = predicate_context.child_at(0);
        end_i = start_i = index.to_int();
    }
    else if (predicate_context.child_count() == 3)
    {  // TODO check type
        NodeView sindex = predicate_context.child_at(0);
        start_i         = sindex.to_int();
        NodeView eindex = predicate_context.child_at(2);
        end_i           = eindex.to_int();
    }
    else if (predicate_context.child_count() == 5)
    {
        // TODO check type
        NodeView sindex      = predicate_context.child_at(0);
        start_i              = sindex.to_int();
        NodeView eindex      = predicate_context.child_at(2);
        end_i                = eindex.to_int();
        NodeView stride_node = predicate_context.child_at(4);
        stride = stride_node.to_int() - 1;  // remove 1 because 1-based
    }
    int stride_remainder   = 1;  // always start at 1 to capture first node
    std::size_t stage_size = stage.size();
    for (std::size_t index = 0; index < stage_size; ++index)
    {
        TAdapter node = stage[index];
        for (auto citr = node.begin(); citr != node.end(); citr.next())
        {
            const TAdapter& child_node = citr.get();
            // if child is a match, push back onto stage
            if (strcmp(name, child_node.name()) == 0)
            {
                ++incident_count;  // increment prior to comparison - 1 based
                                   // indices
                bool within_range =
                    incident_count >= start_i && incident_count <= end_i;
                if (within_range)
                    --stride_remainder;
                if (within_range && stride_remainder == 0)
                {
                    stage.push_back(child_node);
                    stride_remainder = stride;  // reset stride
                }
            }
            // early terminate when our range has been exhausted
            else if (incident_count > end_i)
            {
                break;
            }
        }
    }
    stage.erase(stage.begin(), stage.begin() + stage_size);
}
#endif
