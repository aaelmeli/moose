#ifndef WASP_EDDINTERPRETER_I_H
#define WASP_EDDINTERPRETER_I_H

template<class S>
EDDInterpreter<S>::EDDInterpreter()
    : Interpreter<S>()
    , traceLexing(false)
    , traceParsing(false)
    , singleParse(false)
    , m_parent(nullptr)
    , m_current(nullptr)
    , mHasFile(false)
{
    definition();  // create empty definition needed by interpreter
    this->m_decorative_node_types = {wasp::COMMENT, 
                               wasp::WASP_COMMA,
                               wasp::DIVIDE, 
                               wasp::TERM};
}
template<class S>
EDDInterpreter<S>::EDDInterpreter(std::ostream& err)
    : Interpreter<S>(err)
    , traceLexing(false)
    , traceParsing(false)
    , singleParse(false)
    , m_parent(nullptr)
    , m_current(nullptr)
    , mHasFile(false)
{
    definition();  // create empty definition needed by interpreter
    this->m_decorative_node_types = {wasp::COMMENT, 
                               wasp::WASP_COMMA,
                               wasp::DIVIDE, 
                               wasp::TERM};
}
template<class S>
EDDInterpreter<S>::~EDDInterpreter()
{
    m_current = nullptr;
}

template<class S>
EDDInterpreter<S>* EDDInterpreter<S>::create_nested_interpreter(Super* parent)
{
    wasp_require(parent);
    auto* interp = new EDDInterpreter<S>(parent->error_stream());
    wasp_insist(dynamic_cast<EDDInterpreter<S>*>(parent) != nullptr, 
        "parent interpreter must be the same type");
    interp->m_parent = dynamic_cast<EDDInterpreter<S>*>(parent);
    // Propogate the document definition
    interp->set_definition_store(this->definition_store());
    // Ensure the current definition scope is incorporated into nested interpreter
    interp->m_current = m_current;
    wasp_check (interp->m_current);
    return interp;
}
template<class S>
bool EDDInterpreter<S>::parse(std::istream& in,
                             size_t        startLine,
                             size_t        startColumn)
{
    return parseStream(
        in, hasFile() ? Interpreter<S>::stream_name() : "stream input",
        startLine, startColumn);
}
template<class S>
bool EDDInterpreter<S>::parseStream(std::istream&      in,
                                   const std::string& sname,
                                   size_t             start_line,
                                   size_t             start_column)
{
    return Interpreter<S>::template parse_impl<EDDIParser>(in, sname, start_line,
                                                          start_column);
}
template<class S>
bool EDDInterpreter<S>::parseFile(const std::string& filename, size_t line)
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
bool EDDInterpreter<S>::parseString(const std::string& input,
                                   const std::string& sname,
                                   size_t             startLine,
                                   size_t             startColumn)
{
    std::istringstream iss(input);
    return parseStream(iss, sname, startLine, startColumn);
}

template<class S>
const AbstractDefinition* EDDInterpreter<S>::definition() const
{
    wasp_require(m_current != nullptr);
    return m_current;
}
template<class S>
AbstractDefinition* EDDInterpreter<S>::definition()
{
    if (m_current == nullptr)
    {
        m_definition = std::make_shared<Definition>();
        m_current    = &(*m_definition);
    }

    return m_current;
}
template<class S>
size_t EDDInterpreter<S>::push_staged(size_t                     node_type,
                                     const std::string&         node_name,
                                     const std::vector<size_t>& child_indices)
{
    auto stage_count =
        Interpreter<S>::push_staged(node_type, node_name, child_indices);
    wasp_check(m_current->has(node_name));
    m_current = m_current->get(node_name);  // push new definition

    // Increment non-decorative child count
    // for purpose of index'd aliasing
    size_t staged_child_count = 0;
    for (const auto&  c_index : child_indices)
    {
        auto child_node_type = this->type(c_index);
        
        if (!this->is_decorative(child_node_type))
        {
            ++staged_child_count;
        }
        // Sections are delimited via '/'
        if (child_node_type == DIVIDE)
        {
            this->m_staged.back().m_section_count++;
        }
    }
    this->m_staged.back().m_non_decorative_child_count = staged_child_count;
    return stage_count;
}

template<class S>
size_t EDDInterpreter<S>::push_staged_child(size_t child_index)
{
    auto stage_count =
        Interpreter<S>::push_staged_child(child_index);

    // Update the non-decorative child count
    // for purposes of index'd aliasing
    auto child_node_type = this->type(child_index);
    if (!this->is_decorative(child_node_type))
    {
        this->m_staged.back().m_non_decorative_child_count++;
    }
    // Sections are delimited via '/'
    if (child_node_type == DIVIDE)
    {
        this->m_staged.back().m_section_count++;
    }
    return stage_count;
}
template<class S>
size_t EDDInterpreter<S>::commit_staged(size_t stage_index)
{
    auto node_index = Interpreter<S>::commit_staged(stage_index);
    wasp_check(m_current);
    if ( m_current->parent() ) m_current = m_current->parent();  // pops current definition
    return node_index;
}

//---------------------------------------------------------------------------//
template<class S>
bool EDDInterpreter<S>::process_staged_node(size_t& new_staged_index,
                                    const std::string& stage_name,
                                    size_t node_index,
                                    const location& loc,
                                    std::ostream& err)
{

    if (stage_name == "command_part")
    {
        return process_document_command(new_staged_index,
                                        node_index,
                                        loc,
                                        err);
    }
    else
    {
        wasp_not_implemented(stage_name);
    }
    return true;
}                                    

//---------------------------------------------------------------------------//
template<class S>
bool EDDInterpreter<S>::process_document_command(size_t& new_staged_index, 
                                size_t node_index, 
                                const location& loc,
                                std::ostream& err)
{
    bool failed_processing = false;
    auto token_type = this->node_token_type(node_index);
    auto node_type = this->type(node_index);
    auto staged_type = this->staged_type(this->staged_count()-1);

    bool is_key_value = node_type == wasp::KEYED_VALUE;
    // If this is a potential start of a new command        
    wasp_check(this->definition());
    auto staged_index = this->staged_count()-1;
    const auto& child_indices = this->staged_child_indices(staged_index);

    // Obtain count of children that are not decorative
    size_t staged_child_count = this->staged_non_decorative_child_count(staged_index);
    size_t staged_section_count = this->staged_section_count(staged_index);

    auto prev_part_line = loc.end.line;  // initialize to current line
    if (!child_indices.empty())
    {
        prev_part_line = this->node_token_line(child_indices.back());
    }
    bool is_named = this->definition()->has("_name");

    // Account for the part's name (if it is named) in the staged child count
    // I.e., the name and subsequent part will have been staged, so if named
    // the staged index should go from 2->0, etc.
    int child_index = staged_child_count - (is_named ? 2 : 1);
    std::string index_name = "_"+std::to_string(child_index);
    std::string section_name = "s_" + std::to_string(staged_section_count);                                        
    std::string even_odd_name = (is_named?staged_child_count:staged_child_count-1)%2 == 0
                                ? "_even" : "_odd";
    AbstractDefinition* strided_definition = this->definition()->get(child_index);

    // Lambdas for logic deduplication
    auto is_index = [&]()
    {
        return this->staged_count() > 1
            && staged_child_count >= 1
            && this->definition()->has(even_odd_name);
    };
    auto do_index = [&]()
    {
        std::string actual_name;
        this->definition()->delta(even_odd_name, actual_name);
        this->set_type(node_index, wasp::VALUE);
        bool name_set_success = this->set_name(node_index, actual_name.c_str());
        wasp_insist(name_set_success, "Failed to assign name for node at " << loc);
        return this->push_staged_child(node_index);        
    };
    auto is_section = [&]()
    {
        return this->staged_count() > 1
            && staged_child_count >= 1
            && this->definition()->has(section_name);
    };
    auto do_section = [&]()
    {
        // If staged child index is aliased to a named component
        // we need to capture it appropriately
        std::string actual_name;
        this->definition()->delta(section_name, actual_name);
        this->set_type(node_index, wasp::VALUE);
        bool name_set_success = this->set_name(node_index, actual_name.c_str());
        wasp_insist(name_set_success, "Failed to assign name for node at " << loc);
        return this->push_staged_child(node_index);
    };
    auto do_stride = [&]()
    {
        // If staged child index is aliased to a strided component
        // we need to capture it appropriately        
        this->set_type(node_index, wasp::VALUE);
        bool name_set_success = this->set_name(node_index, strided_definition->actual_name().c_str());
        wasp_insist(name_set_success, "Failed to assign name for node at " << loc);
        return this->push_staged_child(node_index);
    };
    if ( wasp::COMMENT == token_type
            && loc.end.line !=  prev_part_line
            && staged_type  != wasp::OBJECT
            && this->staged_count() > 1)
    {
        // Check for scenario where comment is trailing on a different line
        // this comment belongs to parent scope
        // terminate the current staged data
        this->commit_staged(staged_index);
        // Stage
        new_staged_index = this->push_staged_child(node_index);
    }
    else if ( wasp::COMMENT == token_type
            || wasp::WASP_COMMA == token_type
            || wasp::DIVIDE == token_type
            || wasp::TERM == token_type)
    {
        new_staged_index = this->push_staged_child(node_index);
        // terminator ';' commits the current stage
        if ( wasp::TERM == token_type && staged_child_count > 0
                && this->staged_count() > 1)
        {
            this->commit_staged(staged_index);
        }
    }    
    else if ( this->staged_count() > 1
            && staged_child_count == 1
            && is_named )
    {
        // if there are stages, and the existing stage only contains
        // the declarator (child_count==1), and the block/command is named
        // we need to consume/recast the first child as the '_name' node
        this->set_type(node_index, wasp::IDENTIFIER);
        bool name_set_success = this->set_name(node_index, "_name");
        wasp_insist(name_set_success, "Failed to assign name for node at " << loc);
        new_staged_index = this->push_staged_child(node_index);
    }
    else if (this->staged_count() > 1
            && staged_child_count >= 1
            && this->definition()->has(index_name) )
    {
        // If staged child index is aliased to a named component
        // we need to capture it appropriately
        this->definition()->delta(index_name, index_name);
        this->set_type(node_index, wasp::VALUE);
        bool name_set_success = this->set_name(node_index, index_name.c_str());
        wasp_insist(name_set_success, "Failed to assign name for node at " << loc);
        new_staged_index = this->push_staged_child(node_index);
    }
    else if (is_key_value ||
                token_type == wasp::STRING ||
                token_type == wasp::QUOTED_STRING)
    {
        std::string data = is_key_value ? this->name(node_index)
                                        : this->data(node_index);
        int delta = this->definition()->delta(data, data);
        if( -1 == delta ) // no adjustment, not a command
        {
            if (is_index())
            {
                new_staged_index = do_index();
            }
            else if (is_section())
            {   
                new_staged_index = do_section();
            }
            else if (strided_definition)
            {
                new_staged_index = do_stride();
            } 
            else{
                // the string is not a new command, capture as a value
                // correcting part name and type to be decl
                // must occur prior to prior stage commital
                this->set_type(node_index, wasp::VALUE);
                if (std::strcmp(this->name(node_index), "decl") == 0)
                {
                    bool name_set_success = this->set_name(node_index, "value");
                    wasp_insist(name_set_success, "Failed to assign name for node at " << loc);
                }
                new_staged_index = this->push_staged_child(node_index);
            }
        }
        else{
            // if nothing has been staged and we are a nested document
            // we need to update definition
            if( staged_child_count == 0 && this->staged_count() == 1
                    && this->document_parent() != nullptr)
            {
                auto* parent_doc = this->document_parent();
                while (delta > 0)
                {
                    auto* parent_definition = this->definition()->parent();
                    this->set_current_definition(parent_definition);
                    wasp_check(static_cast<int>(parent_doc->staged_count()) > delta);
                    parent_doc->commit_staged(parent_doc->staged_count()-1);
                    --delta;
                }
            }
            else
            {
                wasp_ensure( delta < static_cast<int>(this->staged_count()) );
                // commit prior stages
                while( delta > 0 ){
                    if ( this->staged_count() == 0 ) // user error
                    {
                        err << "'" << data << "' has been identified, but belongs to a different scope." << std::endl;
                        failed_processing = true;
                        this->set_failed(failed_processing);
                    }
                    else
                    {
                        this->commit_staged(this->staged_count()-1);
                    }
                    --delta;
                }
            }

            if ( is_key_value )
            {
                // Ensure Aliased' key-value are named by their definition
                this->set_name(node_index, data.c_str());
                new_staged_index = this->push_staged_child(node_index);
            }
            else
            {
                std::vector<size_t> child_indices = {node_index};
                new_staged_index = this->push_staged(wasp::ARRAY // commands are
                                        ,data.c_str()
                                        ,child_indices);                                      
            }
        }        
    }
    else if (this->staged_count() > 1
            && staged_child_count >= 1
            && strided_definition )
    {
        new_staged_index = do_stride();
    }
    else if (is_index())
    {
        // if staged index
        new_staged_index = do_index();
    }
    else if (is_section())
    {
        new_staged_index = do_section();
    }    
    else
    {
        // This is a part of a command, stage in existing stage
        new_staged_index = this->push_staged_child(node_index);
    }
    return !failed_processing;
}

//---------------------------------------------------------------------------//                                
#endif
