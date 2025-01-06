/*
 * File:   HIVE.i.h
 * Author: Brandon Langley, Robert Lefebvre (migrated to WASP)
 *
 * Created on September 17, 2014, 8:19 PM
 */

template<class SchemaAdapter, class InputAdapter>
bool HIVE::select_nodes(SIRENResultSet<InputAdapter>& results,
                        SchemaAdapter&                schema_node,
                        InputAdapter&                 input_node,
                        const std::string&            selection_path,
                        std::vector<std::string>&     errors)
{
    std::stringstream look_up_error;

    DefaultSIRENInterpreter inputSelector(look_up_error);

    if (!inputSelector.parseString(selection_path))
    {
        errors.push_back(FileScope(schema_node) + Error::SirenParseError(schema_node,
                                                look_up_error.str()));
        return false;
    }
    inputSelector.evaluate(input_node, results);
    return true;
}

template<class SchemaAdapter, class InputAdapter>
bool HIVE::validate(SchemaAdapter&            schema_node,
                    InputAdapter&             input_node,
                    std::vector<std::string>& errors)
{
    if (schema_node.is_null() || input_node.is_null())
    {
        return false;
    }
    bool pass = true;

    pass = traverse_schema(schema_node, input_node, errors);

    sort_errors(errors);
    return pass;
}

template<class SchemaAdapter, class InputAdapter>
bool HIVE::traverse_schema(SchemaAdapter&            schema_node,
                           InputAdapter&             input_node,
                           std::vector<std::string>& errors)
{
    if (stop)
        return true;

    bool pass = true;

    if (schema_node.is_null())
        return false;
    SIRENResultSet<InputAdapter> selection;
    if (!select_nodes(selection, schema_node, input_node,
                      schema_node.path(), errors))
    {
        return false;
    }

    /*
     * Determine if this schema input node is marked as UNKNOWN
     */
    SchemaAdapter schema_node_id = schema_node.id_child();
    if (selection.size() != 0 && !schema_node_id.is_null()
            && schema_node_id.data() == "UNKNOWN")
    {
        for( size_t i_input = 0; i_input < selection.size(); ++i_input)
        {
            const auto& input_node_instance = selection.adapted(i_input);
            errors.push_back(FileScope(input_node_instance) + Error::UnknownInputNode(input_node_instance));
        }
        pass &= false;
    }

    const typename SchemaAdapter::Collection& children =
        schema_node.non_decorative_children();
    bool                  hasToDo = false;
    bool                  isAny   = false;
    std::set<std::string> definitionChildren;

    for (size_t i = 0, count = children.size(); i < count; i++)
    {
        SchemaAdapter      tmpNode     = children[i];
        const std::string& tmpNodeName = tmpNode.name();
        bool               isToDo      = tmpNodeName == "ToDo";

        // This shortcut to stop processing schema allows one to use
        // enumerations flexibly in the schema without the variables
        // being treated like part of the schema and causing an error to
        // be thrown.
        if (tmpNodeName == "EndOfSchema")
            break;

        isAny |= tmpNodeName == "*";

        if (isToDo || (tmpNodeName == "Units") ||
            (tmpNodeName == "Description") || (tmpNodeName == "InputName") ||
            (tmpNodeName == "InputTerm") || (tmpNodeName == "InputType") ||
            (tmpNodeName == "InputVariants") ||
            (tmpNodeName == "InputAliases") ||
            (tmpNodeName == "InputChoices") ||
            (tmpNodeName == "InputDefault") || (tmpNodeName == "InputTmpl"))
        {
            hasToDo |= isToDo;
            continue;
        }

        else if (tmpNodeName == "MinOccurs")
        {
            pass &= validateMinOccurs(tmpNode, input_node, errors);
        }

        else if (tmpNodeName == "MaxOccurs")
        {
            pass &= validateMaxOccurs(tmpNode, input_node, errors);
        }

        else if (tmpNodeName == "ValType")
        {
            pass &= validateValType(tmpNode, input_node, errors);
        }

        else if (tmpNodeName == "ValEnums")
        {
            pass &= validateValEnums(tmpNode, input_node, errors);
        }

        else if (tmpNodeName == "MinValInc")
        {
            pass &= validateMinValInc(tmpNode, input_node, errors);
        }

        else if (tmpNodeName == "MaxValInc")
        {
            pass &= validateMaxValInc(tmpNode, input_node, errors);
        }

        else if (tmpNodeName == "MinValExc")
        {
            pass &= validateMinValExc(tmpNode, input_node, errors);
        }

        else if (tmpNodeName == "MaxValExc")
        {
            pass &= validateMaxValExc(tmpNode, input_node, errors);
        }

        else if (tmpNodeName == "ExistsIn")
        {
            pass &= validateExistsIn(tmpNode, input_node, errors);
        }

        else if (tmpNodeName == "NotExistsIn")
        {
            pass &= validateNotExistsIn(tmpNode, input_node, errors);
        }

        else if (tmpNodeName == "SumOver")
        {
            pass &= validateSumOver(tmpNode, input_node, errors);
        }

        else if (tmpNodeName == "SumOverGroup")
        {
            pass &= validateSumOverGroup(tmpNode, input_node, errors);
        }

        else if (tmpNodeName == "IncreaseOver")
        {
            pass &= validateIncreaseOver(tmpNode, input_node, errors);
        }

        else if (tmpNodeName == "DecreaseOver")
        {
            pass &= validateDecreaseOver(tmpNode, input_node, errors);
        }

        else if (tmpNodeName == "ChildAtMostOne")
        {
            pass &= validateChildAtMostOne(tmpNode, input_node, errors);
        }

        else if (tmpNodeName == "ChildExactlyOne")
        {
            pass &= validateChildExactlyOne(tmpNode, input_node, errors);
        }

        else if (tmpNodeName == "ChildAtLeastOne")
        {
            pass &= validateChildAtLeastOne(tmpNode, input_node, errors);
        }

        else if (tmpNodeName == "ChildCountEqual")
        {
            pass &= validateChildCountEqual(tmpNode, input_node, errors);
        }

        else if (tmpNodeName == "ChildUniqueness")
        {
            pass &= validateChildUniqueness(tmpNode, input_node, errors);
        }

        else
        {
            /* Error if there is a non-object schema rule that we do not
               recognize */
            if (tmpNode.type() != wasp::OBJECT)
            {
                errors.push_back(FileScope(tmpNode) + Error::BadSchemaRule(
                    tmpNodeName, tmpNode.line(), tmpNode.column()));
                return false;
            }

            /* Only continue child schema traversal if this node exists in input
             */
            if (selection.size() != 0)
            {                
                pass &= traverse_schema(tmpNode, input_node, errors);
            }
            definitionChildren.insert(tmpNodeName);
        }
    }
    bool          is_wild_card = std::strcmp(schema_node.name(), "*") == 0;
    bool          schema_node_has_parent = schema_node.has_parent();
    SchemaAdapter parent_schema_node;
    if (schema_node_has_parent)
        parent_schema_node = schema_node.parent();
    if (!isAny && !hasToDo)
    {
        /* Error if there is a non-decorative input child with no schema rule */
        for (size_t i = 0; i < selection.size(); i++)
        {
            if (is_wild_card && schema_node_has_parent &&
                parent_schema_node
                        .first_child_by_name(selection.adapted(i).name())
                        .is_null() == false)
            {
                continue;
            }
            const typename InputAdapter::Collection& children =
                selection.adapted(i).non_decorative_children();

            for (size_t j = 0; j < children.size(); j++)
            {
                InputAdapter inputChild = children[j];

                if (std::strcmp(inputChild.name(), "value") != 0)
                {
                    if (definitionChildren.find(inputChild.name()) ==
                        definitionChildren.end())
                    {
                        errors.push_back(FileScope(inputChild) + Error::NotExistInSchema(
                            inputChild.line(), inputChild.column(),
                            inputChild.path()));
                        pass = false;
                    }
                }
            }
        }
    }

    return pass;
}

template<class SchemaAdapter, class InputAdapter>
bool HIVE::validateMinOccurs(SchemaAdapter&            schema_node,
                             InputAdapter&             input_node,
                             std::vector<std::string>& errors)
{
    if (schema_node.to_string() == "0")
        return true;

    std::string nodeName       = schema_node.parent().name();
    std::string nodeParentPath = schema_node.parent().parent().path();
    std::string ruleName       = getFullRuleName(schema_node.name());
    std::string ruleValue      = schema_node.to_string();
    bool        pass           = true;

    SIRENResultSet<InputAdapter> selection;

    if (!select_nodes(selection, schema_node, input_node,
                      nodeParentPath, errors))
    {
        return false;
    }
    std::istringstream issRV(ruleValue);
    int                itestRV;
    issRV >> std::noskipws >> itestRV;
    for (size_t i = 0; i < selection.size(); i++)
    {
        if (selection.adapted(i).is_decorative() ||
            selection.adapted(i).type() == wasp::VALUE)
            continue;

        size_t childNodeCount = 0;

        if (nodeName == "*")
            childNodeCount =
                selection.adapted(i).non_decorative_children_count();
        else
            childNodeCount = selection.adapted(i).child_count_by_name(nodeName);
        if (!issRV.eof() || issRV.fail())
        {
            std::stringstream       look_up_error;
            DefaultSIRENInterpreter inputSelectorlookup(look_up_error);

            if (!inputSelectorlookup.parseString(ruleValue.substr(3)))
            {
                errors.push_back(FileScope(schema_node) + Error::SirenParseError(schema_node, look_up_error.str()));
                return false;
            }
            SIRENResultSet<InputAdapter> selectionLookup;
            InputAdapter                 inode = selection.adapted(i);
            inputSelectorlookup.evaluate(inode, selectionLookup);

            if (selectionLookup.size() > 1)
            {
                errors.push_back(FileScope(inode) + 
                    Error::MoreThanOneValue(inode.line(), inode.column(),
                                            nodeName, ruleName, ruleValue));
                pass = false;
            }
            else if (selectionLookup.size() == 1)
            {
                std::istringstream issRV2(
                    selectionLookup.adapted(0).to_string());
                issRV2 >> std::noskipws >> itestRV;
                auto aNode = selection.adapted(i);
                if (!issRV2.eof() || issRV2.fail())
                {
                    errors.push_back(FileScope(aNode) + 
                        Error::NotAValidNumber(aNode.line(),
                                               aNode.column(),
                                               nodeName, ruleName, ruleValue));
                    pass = false;
                }
                else if (childNodeCount <
                         stoi(selectionLookup.adapted(0).to_string()))
                {

                    errors.push_back(FileScope(aNode) + Error::Occurrence(
                        aNode.line(),
                        aNode.column(),
                        aNode.name(), childNodeCount, nodeName,
                        ruleName, selectionLookup.adapted(0).to_string(),
                        ruleValue));
                    pass = false;
                }
            }
        }

        else if (childNodeCount < stoi(ruleValue))
        {
            auto aNode = selection.adapted(i);
            errors.push_back(FileScope(aNode) + Error::Occurrence(
                aNode.line(), aNode.column(),
                aNode.name(), childNodeCount, nodeName, ruleName,
                ruleValue));
            pass = false;
        }
    }

    return pass;
}

template<class SchemaAdapter, class InputAdapter>
bool HIVE::validateMaxOccurs(SchemaAdapter&            schema_node,
                             InputAdapter&             input_node,
                             std::vector<std::string>& errors)
{
    if (schema_node.to_string() == "NoLimit")
        return true;
    auto schema_node_parent = schema_node.parent();
    wasp_check(schema_node_parent.is_null() == false);
    auto schema_node_grandparent = schema_node_parent.parent();
    wasp_check(schema_node_grandparent.is_null() == false);
    std::string       nodeName       = schema_node_parent.name();
    std::string       nodeParentPath = schema_node_grandparent.path();
    std::string       ruleName       = getFullRuleName(schema_node.name());
    std::string       ruleValue      = schema_node.to_string();
    bool              pass           = true;
    std::stringstream look_up_error;
    DefaultSIRENInterpreter inputSelector(look_up_error);

    if (!inputSelector.parseString(nodeParentPath))
    {
        errors.push_back(FileScope(schema_node_grandparent) + 
                        Error::SirenParseError(schema_node_grandparent,look_up_error.str()));
        return false;
    }
    SIRENResultSet<InputAdapter> selection;
    inputSelector.evaluate(input_node, selection);

    std::istringstream issRV(ruleValue);
    int                itestRV;
    issRV >> std::noskipws >> itestRV;

    for (size_t i = 0; i < selection.size(); i++)
    {
        if (selection.adapted(i).is_decorative() ||
            selection.adapted(i).type() == wasp::VALUE)
            continue;

        size_t childNodeCount;

        if (nodeName == "*")
            childNodeCount =
                selection.adapted(i).non_decorative_children_count();
        else
            childNodeCount = selection.adapted(i).child_count_by_name(nodeName);

        if (!issRV.eof() || issRV.fail())
        {
            std::stringstream       look_up_error;
            DefaultSIRENInterpreter inputSelectorlookup(look_up_error);

            if (!inputSelectorlookup.parseString(ruleValue.substr(3)))
            {
                errors.push_back(FileScope(schema_node) + Error::SirenParseError(schema_node,look_up_error.str()));
                return false;
            }
            SIRENResultSet<InputAdapter> selectionLookup;
            InputAdapter                 inode = selection.adapted(i);
            inputSelectorlookup.evaluate(inode, selectionLookup);

            if (selectionLookup.size() > 1)
            {
                errors.push_back(FileScope(selection.adapted(i)) + Error::MoreThanOneValue(
                    selection.adapted(i).line(), selection.adapted(i).column(),
                    nodeName, ruleName, ruleValue));
                pass = false;
            }
            else if (selectionLookup.size() == 1)
            {
                std::istringstream issRV2(
                    selectionLookup.adapted(0).to_string());
                issRV2 >> std::noskipws >> itestRV;

                if (!issRV2.eof() || issRV2.fail())
                {
                    errors.push_back(FileScope(selection.adapted(i)) + 
                        Error::NotAValidNumber(selection.adapted(i).line(),
                                               selection.adapted(i).column(),
                                               nodeName, ruleName, ruleValue));
                    pass = false;
                }
                else if (childNodeCount >
                         stoi(selectionLookup.adapted(0).to_string()))
                {
                    errors.push_back(FileScope(selection.adapted(i)) + Error::Occurrence(
                        selection.adapted(i).line(),
                        selection.adapted(i).column(),
                        selection.adapted(i).name(), childNodeCount, nodeName,
                        ruleName, selectionLookup.adapted(0).to_string(),
                        ruleValue));
                    pass = false;
                }
            }
        }

        else if (childNodeCount > stoi(ruleValue))
        {
            errors.push_back(FileScope(selection.adapted(i)) + Error::Occurrence(
                selection.adapted(i).line(), selection.adapted(i).column(),
                selection.adapted(i).name(), childNodeCount, nodeName, ruleName,
                ruleValue));
            pass = false;
        }
    }

    return pass;
}

template<class SchemaAdapter, class InputAdapter>
bool HIVE::validateValType(SchemaAdapter&            schema_node,
                           InputAdapter&             input_node,
                           std::vector<std::string>& errors)
{
    std::string nodePath  = schema_node.parent().path();
    std::string ruleName  = getFullRuleName(schema_node.name());
    std::string ruleValue = schema_node.to_string();
    bool        pass      = true;

    if ((ruleValue != "Int") && (ruleValue != "Real") &&
        (ruleValue != "String") && (ruleValue != "RealOrQuestion") &&
        (ruleValue != "IntOrYesOrNo") && (ruleValue != "IntOrAsterisk") &&
        (ruleValue != "Fido"))
    {
        errors.push_back(FileScope(schema_node) + Error::BadOption(
            schema_node.name(), ruleValue,
            schema_node.non_decorative_children()[0].line(),
            schema_node.non_decorative_children()[0].column(),
            "Int Real String RealOrQuestion IntOrYesOrNo IntOrAsterisk Fido"));
        return false;
    }
    std::stringstream       look_up_error;
    DefaultSIRENInterpreter inputSelector(look_up_error);

    if (!inputSelector.parseString(nodePath))
    {
        errors.push_back(FileScope(schema_node) + Error::SirenParseError(schema_node,
                                                look_up_error.str()));
        return false;
    }
    SIRENResultSet<InputAdapter> input_selection;
    inputSelector.evaluate(input_node, input_selection);

    for (size_t i = 0; i < input_selection.size(); i++)
    {
        std::istringstream iss(input_selection.adapted(i).to_string());

        if ((ruleValue == "Int") || (ruleValue == "Real") ||
            (ruleValue == "String"))
        {
            if (ruleValue == "Int")
            {
                int itest;
                iss >> std::noskipws >> itest;
            }

            if (ruleValue == "Real")
            {
                double ftest;
                iss >> std::noskipws >> ftest;
            }

            if (ruleValue == "String")
            {
                // std::string stest;
                // iss >> std::noskipws >> stest;
                iss.setstate(std::ios::eofbit);
            }

            if (!iss.eof() || iss.fail())
            {
                std::string valueNodeName;

                if ((std::strcmp(input_selection.adapted(i).name(), "value") ==
                     0) &&
                    input_selection.adapted(i).has_parent())
                {
                    valueNodeName = input_selection.adapted(i).parent().name();
                }
                else
                {
                    valueNodeName = input_selection.adapted(i).name();
                }

                errors.push_back(FileScope(input_selection.adapted(i)) + Error::BadValType(
                    input_selection.adapted(i).line(),
                    input_selection.adapted(i).column(), valueNodeName,
                    input_selection.adapted(i).to_string(), ruleValue));
                pass = false;
            }
        }
        else if (ruleValue == "RealOrQuestion")
        {
            if (input_selection.adapted(i).to_string() != "?")
            {
                float ftest;
                iss >> std::noskipws >> ftest;

                if (!iss.eof() || iss.fail())
                {
                    std::string valueNodeName;

                    if ((std::strcmp(input_selection.adapted(i).name(),
                                     "value") == 0) &&
                        input_selection.adapted(i).has_parent())
                    {
                        valueNodeName =
                            input_selection.adapted(i).parent().name();
                    }
                    else
                    {
                        valueNodeName = input_selection.adapted(i).name();
                    }

                    errors.push_back(FileScope(input_selection.adapted(i)) + Error::BadValType(
                        input_selection.adapted(i).line(),
                        input_selection.adapted(i).column(), valueNodeName,
                        input_selection.adapted(i).to_string(), ruleValue));
                    pass = false;
                }
            }
        }
        else if (ruleValue == "IntOrYesOrNo")
        {
            std::string lowerString = input_selection.adapted(i).to_string();
            transform(lowerString.begin(), lowerString.end(),
                      lowerString.begin(), ::tolower);

            if ((lowerString != "yes") && (lowerString != "no"))
            {
                int itest;
                iss >> std::noskipws >> itest;

                if (!iss.eof() || iss.fail())
                {
                    std::string valueNodeName;

                    if ((std::strcmp(input_selection.adapted(i).name(),
                                     "value") == 0) &&
                        input_selection.adapted(i).has_parent())
                    {
                        valueNodeName =
                            input_selection.adapted(i).parent().name();
                    }
                    else
                    {
                        valueNodeName = input_selection.adapted(i).name();
                    }

                    errors.push_back(FileScope(input_selection.adapted(i)) + Error::BadValType(
                        input_selection.adapted(i).line(),
                        input_selection.adapted(i).column(), valueNodeName,
                        input_selection.adapted(i).to_string(), ruleValue));
                    pass = false;
                }
            }
        }
        else if (ruleValue == "IntOrAsterisk")
        {
            if (input_selection.adapted(i).to_string() != "*")
            {
                int itest;
                iss >> std::noskipws >> itest;

                if (!iss.eof() || iss.fail())
                {
                    std::string valueNodeName;

                    if ((std::strcmp(input_selection.adapted(i).name(),
                                     "value") == 0) &&
                        input_selection.adapted(i).has_parent())
                    {
                        valueNodeName =
                            input_selection.adapted(i).parent().name();
                    }
                    else
                    {
                        valueNodeName = input_selection.adapted(i).name();
                    }

                    errors.push_back(FileScope(input_selection.adapted(i)) + Error::BadValType(
                        input_selection.adapted(i).line(),
                        input_selection.adapted(i).column(), valueNodeName,
                        input_selection.adapted(i).to_string(), ruleValue));
                    pass = false;
                }
            }
        }
        else if (ruleValue == "Fido")
        {
            /* fido check */
        }
    }

    return pass;
}

template<class SchemaAdapter, class InputAdapter>
bool HIVE::validateValEnums(SchemaAdapter&            schema_node,
                            InputAdapter&             input_node,
                            std::vector<std::string>& errors)
{
    auto schema_node_parent = schema_node.parent();
    wasp_check(schema_node_parent.is_null() == false);
    std::string nodePath = schema_node_parent.path();
    bool        pass     = true;

    // CREATE THE DEQUE OF THE VALUES FOR THIS NODE
    std::stringstream look_up_error;

    DefaultSIRENInterpreter inputSelector(look_up_error);

    if (!inputSelector.parseString(nodePath))
    {
        errors.push_back(FileScope(schema_node_parent) + Error::SirenParseError(schema_node_parent,
                                                look_up_error.str()));
        return false;
    }
    SIRENResultSet<InputAdapter> selection;
    inputSelector.evaluate(input_node, selection);

    // CREATE ENUM UNORDERED SET
    std::set<std::string>  enumSet;
    std::set<std::string>* enumSetPtr = NULL;

    if (selection.size() != 0)
    {
        const typename SchemaAdapter::Collection& refNodes =
            schema_node.child_by_name("REF");
        std::vector<std::string> refNames;

        for (int i = 0; i < refNodes.size(); i++)
        {
            refNames.push_back(refNodes[i].to_string());
        }
        sort(refNames.begin(), refNames.end());
        std::string refTag =
            std::accumulate(refNames.begin(), refNames.end(), std::string(""));

        if (refNames.size() != 0)
        {
            std::map<std::string, std::set<std::string>>::iterator enumRefIter;
            enumRefIter = enumRef.find(refTag);

            if (enumRefIter == enumRef.end())
            {
                enumRef.insert(make_pair(refTag, std::set<std::string>()));
                enumRefIter = enumRef.find(refTag);

                for (int i = 0; i < refNames.size(); i++)
                {
                    std::string refName = refNames[i];

                    SchemaAdapter tmpschema_node = schema_node;

                    while (tmpschema_node.has_parent())
                    {
                        tmpschema_node = tmpschema_node.parent();
                    }
                    tmpschema_node =
                        tmpschema_node.first_non_decorative_child_by_name(
                            refName);

                    if (tmpschema_node.is_null())
                    {
                        auto aNode = schema_node.non_decorative_children()[0];
                        errors.push_back(FileScope(aNode) + Error::BadEnumReference(
                            refName,
                            aNode.line(),
                            aNode.column()));
                        return false;
                    }

                    const typename SchemaAdapter::Collection& children =
                        tmpschema_node.non_decorative_children();

                    for (int ic = children.size() - 1; ic >= 0; ic--)
                    {
                        std::string lowerString = children[ic].to_string();
                        transform(lowerString.begin(), lowerString.end(),
                                  lowerString.begin(), ::tolower);
                        enumRefIter->second.insert(lowerString);
                    }
                }
            }

            enumSetPtr = &enumRefIter->second;
        }
        else
        {
            const typename SchemaAdapter::Collection& children =
                schema_node.non_decorative_children();

            for (int i = children.size() - 1; i >= 0; i--)
            {
                std::string lowerString = children[i].to_string();
                transform(lowerString.begin(), lowerString.end(),
                          lowerString.begin(), ::tolower);
                enumSet.insert(lowerString);
            }
            enumSetPtr = &enumSet;
        }
    }

    // LOOP OVER THIS DEQUE CHECKING EACH VALUES EXISTANCE IN THE ENUM UNORDERED
    // SET
    for (size_t i = 0; i < selection.size(); i++)
    {
        std::string tempString = selection.adapted(i).to_string();

        // if tempString is quoted (single or double), remove quotes before
        // checking
        if ((tempString.front() == '\'') && (tempString.back() == '\'') &&
            (tempString.size() > 1))
        {
            tempString.erase(0, 1);
            tempString.erase(tempString.size() - 1);
        }
        else if ((tempString.front() == '"') && (tempString.back() == '"') &&
                 (tempString.size() > 1))
        {
            tempString.erase(0, 1);
            tempString.erase(tempString.size() - 1);
        }

        // if tempString is an integer, remove leading zeros before checking
        size_t zeroIndex = tempString.find_first_not_of('0');

        if ((zeroIndex != 0) && (zeroIndex != std::string::npos))
        {
            int                itest;
            std::istringstream iss(tempString);
            iss >> std::noskipws >> itest;

            if (iss.eof() && !iss.fail())
            {
                tempString.erase(0, zeroIndex);
            }
        }

        transform(tempString.begin(), tempString.end(), tempString.begin(),
                  ::tolower);

        if (enumSetPtr->find(tempString) == enumSetPtr->end())
        {
            // CLOSEST MATCH ENUM LIST REPORTING

            std::string valueNodeName;

            if ((std::strcmp(selection.adapted(i).name(), "value") == 0) &&
                selection.adapted(i).has_parent())
            {
                valueNodeName = selection.adapted(i).parent().name();
            }
            else
            {
                valueNodeName = selection.adapted(i).name();
            }

            std::string closestEnums;

            if (enumSetPtr->size() <= MAXENUMERRORCOUNT)
            {
                for (auto setIter = enumSetPtr->begin();
                     setIter != enumSetPtr->end();)
                {
                    closestEnums += "\"" + (*setIter) + "\"";

                    if (++setIter != enumSetPtr->end())
                        closestEnums += " ";
                }
            }
            else
            {
                auto setIter      = enumSetPtr->upper_bound(tempString);
                int  countFromEnd = -1;
                auto tmpSetIter   = setIter;

                for (int j = 0; j < MAXENUMERRORCOUNT / 2; j++)
                {
                    if (tmpSetIter == enumSetPtr->end())
                    {
                        countFromEnd = j;
                        break;
                    }
                    tmpSetIter++;
                }

                if (countFromEnd != -1)
                {
                    for (int j = 0; j < MAXENUMERRORCOUNT - countFromEnd; j++)
                    {
                        setIter--;
                    }
                }
                else
                {
                    for (int j = 0; j < MAXENUMERRORCOUNT / 2; j++)
                    {
                        if (setIter == enumSetPtr->begin())
                            break;
                        setIter--;
                    }
                }

                if (setIter != enumSetPtr->begin())
                {
                    closestEnums += "... ";
                }

                for (int j = 0; j < MAXENUMERRORCOUNT; j++)
                {
                    closestEnums += "\"" + (*setIter) + "\"";
                    setIter++;

                    if (setIter == enumSetPtr->end())
                        break;

                    if (j + 1 != MAXENUMERRORCOUNT)
                        closestEnums += " ";
                }

                if (setIter != enumSetPtr->end())
                {
                    closestEnums += " ...";
                }
            }

            errors.push_back(FileScope(selection.adapted(i)) + Error::BadEnum(
                selection.adapted(i).line(), selection.adapted(i).column(),
                valueNodeName, tempString, closestEnums));
            pass = false;
        }
    }

    return pass;
}

template<class SchemaAdapter, class InputAdapter>
bool HIVE::validateMinValInc(SchemaAdapter&            schema_node,
                             InputAdapter&             input_node,
                             std::vector<std::string>& errors)
{
    if (schema_node.to_string() == "NoLimit")
        return true;

    std::string nodePath  = schema_node.parent().path();
    std::string ruleName  = getFullRuleName(schema_node.name());
    std::string ruleValue = schema_node.to_string();
    bool        pass      = true;

    std::stringstream       look_up_error;
    DefaultSIRENInterpreter inputSelector(look_up_error);

    if (!inputSelector.parseString(nodePath))
    {
        errors.push_back(FileScope(schema_node) + Error::SirenParseError(schema_node,
                                                look_up_error.str()));
        return false;
    }
    SIRENResultSet<InputAdapter> selection;
    inputSelector.evaluate(input_node, selection);

    for (size_t i = 0; i < selection.size(); i++)
    {
        std::istringstream iss(selection.adapted(i).to_string());
        float              ftest;
        iss >> std::noskipws >> ftest;

        if (!iss.eof() || iss.fail())
        {
            std::string valueNodeName;

            if ((std::strcmp(selection.adapted(i).name(), "value") == 0) &&
                selection.adapted(i).has_parent())
            {
                valueNodeName = selection.adapted(i).parent().name();
            }
            else
            {
                valueNodeName = selection.adapted(i).name();
            }

            errors.push_back(FileScope(selection.adapted(i)) + Error::WrongTypeForRule(
                selection.adapted(i).line(), selection.adapted(i).column(),
                valueNodeName, selection.adapted(i).to_string(), ruleName));
            pass = false;
        }
        else
        {
            std::istringstream issRV(ruleValue);
            float              ftestRV;
            issRV >> std::noskipws >> ftestRV;

            if (!issRV.eof() || issRV.fail())
            {
                std::stringstream       look_up_error;
                DefaultSIRENInterpreter inputSelectorlookup(look_up_error);

                if (!inputSelectorlookup.parseString(ruleValue))
                {
                    errors.push_back(FileScope(schema_node) + 
                        Error::SirenParseError(schema_node,
                                               look_up_error.str()));
                    return false;
                }
                SIRENResultSet<InputAdapter> selectionLookup;
                InputAdapter                 inode = selection.adapted(i);
                inputSelectorlookup.evaluate(inode, selectionLookup);

                if (selectionLookup.size() > 1)
                {
                    errors.push_back(FileScope(inode) + Error::MoreThanOneValue(
                        inode.line(), inode.column(), inode.name(), ruleName,
                        ruleValue));
                    pass = false;
                }
                else if (selectionLookup.size() == 1)
                {
                    std::istringstream issRV2(
                        selectionLookup.adapted(0).to_string());
                    issRV2 >> std::noskipws >> ftestRV;

                    if (!issRV2.eof() || issRV2.fail())
                    {
                        errors.push_back(FileScope(selection.adapted(i)) + Error::NotAValidNumber(
                            selection.adapted(i).line(),
                            selection.adapted(i).column(),
                            selection.adapted(i).name(), ruleName, ruleValue));
                        pass = false;
                    }
                    else if (stod(selection.adapted(i).to_string()) <
                             stod(issRV2.str()))
                    {
                        std::string valueNodeName;

                        if ((std::strcmp(selection.adapted(i).name(),
                                         "value") == 0) &&
                            selection.adapted(i).has_parent())
                        {
                            valueNodeName =
                                selection.adapted(i).parent().name();
                        }
                        else
                        {
                            valueNodeName = selection.adapted(i).name();
                        }

                        errors.push_back(FileScope(selection.adapted(i)) + Error::MinMax(
                            selection.adapted(i).line(),
                            selection.adapted(i).column(), valueNodeName,
                            selection.adapted(i).to_string(), ruleName,
                            issRV2.str(), ruleValue));
                        pass = false;
                    }
                }
            }
            else if (stod(selection.adapted(i).to_string()) < stod(ruleValue))
            {
                std::string valueNodeName;

                if ((std::strcmp(selection.adapted(i).name(), "value") == 0) &&
                    selection.adapted(i).has_parent())
                {
                    valueNodeName = selection.adapted(i).parent().name();
                }
                else
                {
                    valueNodeName = selection.adapted(i).name();
                }

                errors.push_back(FileScope(selection.adapted(i)) + Error::MinMax(
                    selection.adapted(i).line(), selection.adapted(i).column(),
                    valueNodeName, selection.adapted(i).to_string(), ruleName,
                    ruleValue));
                pass = false;
            }
        }
    }

    return pass;
}

template<class SchemaAdapter, class InputAdapter>
bool HIVE::validateMaxValInc(SchemaAdapter&            schema_node,
                             InputAdapter&             input_node,
                             std::vector<std::string>& errors)
{
    if (schema_node.to_string() == "NoLimit")
        return true;
    auto schema_node_parent = schema_node.parent();
    wasp_check(schema_node_parent.is_null() == false);
    std::string nodePath  = schema_node_parent.path();
    std::string ruleName  = getFullRuleName(schema_node.name());
    std::string ruleValue = schema_node.to_string();
    bool        pass      = true;

    std::stringstream       look_up_error;
    DefaultSIRENInterpreter inputSelector(look_up_error);

    if (!inputSelector.parseString(nodePath))
    {
        errors.push_back(FileScope(schema_node_parent) + Error::SirenParseError(schema_node_parent,
                                                look_up_error.str()));
        return false;
    }
    SIRENResultSet<InputAdapter> selection;
    inputSelector.evaluate(input_node, selection);

    for (size_t i = 0; i < selection.size(); i++)
    {
        std::istringstream iss(selection.adapted(i).to_string());
        float              ftest;
        iss >> std::noskipws >> ftest;

        if (!iss.eof() || iss.fail())
        {
            std::string valueNodeName;

            if ((std::strcmp(selection.adapted(i).name(), "value") == 0) &&
                selection.adapted(i).has_parent())
            {
                valueNodeName = selection.adapted(i).parent().name();
            }
            else
            {
                valueNodeName = selection.adapted(i).name();
            }

            errors.push_back(FileScope(selection.adapted(i)) +Error::WrongTypeForRule(
                selection.adapted(i).line(), selection.adapted(i).column(),
                valueNodeName, selection.adapted(i).to_string(), ruleName));
            pass = false;
        }
        else
        {
            std::istringstream issRV(ruleValue);
            float              ftestRV;
            issRV >> std::noskipws >> ftestRV;

            if (!issRV.eof() || issRV.fail())
            {
                std::stringstream       look_up_error;
                DefaultSIRENInterpreter inputSelectorlookup(look_up_error);

                if (!inputSelectorlookup.parseString(ruleValue))
                {
                    errors.push_back(FileScope(schema_node) +
                        Error::SirenParseError(schema_node,
                                               look_up_error.str()));
                    return false;
                }
                SIRENResultSet<InputAdapter> selectionLookup;
                InputAdapter                 inode = selection.adapted(i);
                inputSelectorlookup.evaluate(inode, selectionLookup);

                if (selectionLookup.size() > 1)
                {
                    errors.push_back(FileScope(selection.adapted(i)) + Error::MoreThanOneValue(
                        selection.adapted(i).line(),
                        selection.adapted(i).column(),
                        selection.adapted(i).name(), ruleName, ruleValue));
                    pass = false;
                }
                else if (selectionLookup.size() == 1)
                {
                    std::istringstream issRV2(
                        selectionLookup.adapted(0).to_string());
                    issRV2 >> std::noskipws >> ftestRV;

                    if (!issRV2.eof() || issRV2.fail())
                    {
                        errors.push_back(FileScope(selection.adapted(i)) + Error::NotAValidNumber(
                            selection.adapted(i).line(),
                            selection.adapted(i).column(),
                            selection.adapted(i).name(), ruleName, ruleValue));
                        pass = false;
                    }
                    else if (stod(selection.adapted(i).to_string()) >
                             stod(issRV2.str()))
                    {
                        std::string valueNodeName;

                        if ((std::strcmp(selection.adapted(i).name(),
                                         "value") == 0) &&
                            selection.adapted(i).has_parent())
                        {
                            valueNodeName =
                                selection.adapted(i).parent().name();
                        }
                        else
                        {
                            valueNodeName = selection.adapted(i).name();
                        }

                        errors.push_back(FileScope(selection.adapted(i)) + Error::MinMax(
                            selection.adapted(i).line(),
                            selection.adapted(i).column(), valueNodeName,
                            selection.adapted(i).to_string(), ruleName,
                            issRV2.str(), ruleValue));
                        pass = false;
                    }
                }
            }
            else if (stod(selection.adapted(i).to_string()) > stod(ruleValue))
            {
                std::string valueNodeName;

                if ((std::strcmp(selection.adapted(i).name(), "value") == 0) &&
                    selection.adapted(i).has_parent())
                {
                    valueNodeName = selection.adapted(i).parent().name();
                }
                else
                {
                    valueNodeName = selection.adapted(i).name();
                }

                errors.push_back(FileScope(selection.adapted(i)) + Error::MinMax(
                    selection.adapted(i).line(), selection.adapted(i).column(),
                    valueNodeName, selection.adapted(i).to_string(), ruleName,
                    ruleValue));
                pass = false;
            }
        }
    }

    return pass;
}

template<class SchemaAdapter, class InputAdapter>
bool HIVE::validateMinValExc(SchemaAdapter&            schema_node,
                             InputAdapter&             input_node,
                             std::vector<std::string>& errors)
{
    if (schema_node.to_string() == "NoLimit")
        return true;
    auto schema_node_parent = schema_node.parent();
    wasp_check(schema_node_parent.is_null() == false);
    std::string nodePath  = schema_node_parent.path();
    std::string ruleName  = getFullRuleName(schema_node.name());
    std::string ruleValue = schema_node.to_string();
    bool        pass      = true;

    std::stringstream       look_up_error;
    DefaultSIRENInterpreter inputSelector(look_up_error);

    if (!inputSelector.parseString(nodePath))
    {
        errors.push_back(FileScope(schema_node_parent) + Error::SirenParseError(schema_node_parent,
                                                look_up_error.str()));
        return false;
    }
    SIRENResultSet<InputAdapter> selection;
    inputSelector.evaluate(input_node, selection);

    for (size_t i = 0; i < selection.size(); i++)
    {
        std::istringstream iss(selection.adapted(i).to_string());
        float              ftest;
        iss >> std::noskipws >> ftest;

        if (!iss.eof() || iss.fail())
        {
            std::string valueNodeName;

            if ((std::strcmp(selection.adapted(i).name(), "value") == 0) &&
                selection.adapted(i).has_parent())
            {
                valueNodeName = selection.adapted(i).parent().name();
            }
            else
            {
                valueNodeName = selection.adapted(i).name();
            }

            errors.push_back(FileScope(selection.adapted(i)) + Error::WrongTypeForRule(
                selection.adapted(i).line(), selection.adapted(i).column(),
                valueNodeName, selection.adapted(i).to_string(), ruleName));
            pass = false;
        }
        else
        {
            std::istringstream issRV(ruleValue);
            float              ftestRV;
            issRV >> std::noskipws >> ftestRV;

            if (!issRV.eof() || issRV.fail())
            {
                std::stringstream       look_up_error;
                DefaultSIRENInterpreter inputSelectorlookup(look_up_error);

                if (!inputSelectorlookup.parseString(ruleValue))
                {
                    errors.push_back(FileScope(schema_node) + 
                        Error::SirenParseError(schema_node,
                                               look_up_error.str()));
                    return false;
                }
                SIRENResultSet<InputAdapter> selectionLookup;
                InputAdapter                 inode = selection.adapted(i);
                inputSelectorlookup.evaluate(inode, selectionLookup);

                if (selectionLookup.size() > 1)
                {
                    errors.push_back(FileScope(selection.adapted(i)) + Error::MoreThanOneValue(
                        selection.adapted(i).line(),
                        selection.adapted(i).column(),
                        selection.adapted(i).name(), ruleName, ruleValue));
                    pass = false;
                }
                else if (selectionLookup.size() == 1)
                {
                    std::istringstream issRV2(
                        selectionLookup.adapted(0).to_string());
                    issRV2 >> std::noskipws >> ftestRV;

                    if (!issRV2.eof() || issRV2.fail())
                    {
                        errors.push_back(FileScope(selection.adapted(i)) + Error::NotAValidNumber(
                            selection.adapted(i).line(),
                            selection.adapted(i).column(),
                            selection.adapted(i).name(), ruleName, ruleValue));
                        pass = false;
                    }
                    else if (stod(selection.adapted(i).to_string()) <=
                             stod(issRV2.str()))
                    {
                        std::string valueNodeName;

                        if ((std::strcmp(selection.adapted(i).name(),
                                         "value") == 0) &&
                            selection.adapted(i).has_parent())
                        {
                            valueNodeName =
                                selection.adapted(i).parent().name();
                        }
                        else
                        {
                            valueNodeName = selection.adapted(i).name();
                        }

                        errors.push_back(FileScope(selection.adapted(i)) + Error::MinMax(
                            selection.adapted(i).line(),
                            selection.adapted(i).column(), valueNodeName,
                            selection.adapted(i).to_string(), ruleName,
                            issRV2.str(), ruleValue));
                        pass = false;
                    }
                }
            }
            else if (stod(selection.adapted(i).to_string()) <= stod(ruleValue))
            {
                std::string valueNodeName;

                if ((std::strcmp(selection.adapted(i).name(), "value") == 0) &&
                    selection.adapted(i).has_parent())
                {
                    valueNodeName = selection.adapted(i).parent().name();
                }
                else
                {
                    valueNodeName = selection.adapted(i).name();
                }

                errors.push_back(FileScope(selection.adapted(i)) + Error::MinMax(
                    selection.adapted(i).line(), selection.adapted(i).column(),
                    valueNodeName, selection.adapted(i).to_string(), ruleName,
                    ruleValue));
                pass = false;
            }
        }
    }

    return pass;
}

template<class SchemaAdapter, class InputAdapter>
bool HIVE::validateMaxValExc(SchemaAdapter&            schema_node,
                             InputAdapter&             input_node,
                             std::vector<std::string>& errors)
{
    if (schema_node.to_string() == "NoLimit")
        return true;
    auto schema_node_parent = schema_node.parent();
    wasp_check(schema_node_parent.is_null() == false);
    std::string nodePath  = schema_node_parent.path();
    std::string ruleName  = getFullRuleName(schema_node.name());
    std::string ruleValue = schema_node.to_string();
    bool        pass      = true;

    std::stringstream       look_up_error;
    DefaultSIRENInterpreter inputSelector(look_up_error);

    if (!inputSelector.parseString(nodePath))
    {
        errors.push_back(FileScope(schema_node_parent) + Error::SirenParseError(schema_node_parent,
                                                look_up_error.str()));
        return false;
    }
    SIRENResultSet<InputAdapter> selection;
    inputSelector.evaluate(input_node, selection);

    for (size_t i = 0; i < selection.size(); i++)
    {
        std::istringstream iss(selection.adapted(i).to_string());
        float              ftest;
        iss >> std::noskipws >> ftest;

        if (!iss.eof() || iss.fail())
        {
            std::string valueNodeName;

            if ((std::strcmp(selection.adapted(i).name(), "value") == 0) &&
                selection.adapted(i).has_parent())
            {
                valueNodeName = selection.adapted(i).parent().name();
            }
            else
            {
                valueNodeName = selection.adapted(i).name();
            }

            errors.push_back(FileScope(selection.adapted(i)) + Error::WrongTypeForRule(
                selection.adapted(i).line(), selection.adapted(i).column(),
                valueNodeName, selection.adapted(i).to_string(), ruleName));
            pass = false;
        }
        else
        {
            std::istringstream issRV(ruleValue);
            float              ftestRV;
            issRV >> std::noskipws >> ftestRV;

            if (!issRV.eof() || issRV.fail())
            {
                std::stringstream       look_up_error;
                DefaultSIRENInterpreter inputSelectorLookup(look_up_error);

                if (!inputSelectorLookup.parseString(ruleValue))
                {
                    errors.push_back(FileScope(schema_node) + 
                        Error::SirenParseError(schema_node,
                                               look_up_error.str()));
                    return false;
                }
                SIRENResultSet<InputAdapter> selectionLookup;
                InputAdapter                 inode = selection.adapted(i);
                inputSelectorLookup.evaluate(inode, selectionLookup);

                if (selectionLookup.size() > 1)
                {
                    errors.push_back(FileScope(selection.adapted(i)) + Error::MoreThanOneValue(
                        selection.adapted(i).line(),
                        selection.adapted(i).column(),
                        selection.adapted(i).name(), ruleName, ruleValue));
                    pass = false;
                }
                else if (selectionLookup.size() == 1)
                {
                    std::istringstream issRV2(
                        selectionLookup.adapted(0).to_string());
                    issRV2 >> std::noskipws >> ftestRV;

                    if (!issRV2.eof() || issRV2.fail())
                    {
                        errors.push_back(FileScope(selection.adapted(i)) + Error::NotAValidNumber(
                            selection.adapted(i).line(),
                            selection.adapted(i).column(),
                            selection.adapted(i).name(), ruleName, ruleValue));
                        pass = false;
                    }
                    else if (stod(selection.adapted(i).to_string()) >=
                             stod(issRV2.str()))
                    {
                        std::string valueNodeName;

                        if ((std::strcmp(selection.adapted(i).name(),
                                         "value") == 0) &&
                            selection.adapted(i).has_parent())
                        {
                            valueNodeName =
                                selection.adapted(i).parent().name();
                        }
                        else
                        {
                            valueNodeName = selection.adapted(i).name();
                        }

                        errors.push_back(FileScope(selection.adapted(i)) + Error::MinMax(
                            selection.adapted(i).line(),
                            selection.adapted(i).column(), valueNodeName,
                            selection.adapted(i).to_string(), ruleName,
                            issRV2.str(), ruleValue));
                        pass = false;
                    }
                }
            }
            else if (stod(selection.adapted(i).to_string()) >= stod(ruleValue))
            {
                std::string valueNodeName;

                if ((std::strcmp(selection.adapted(i).name(), "value") == 0) &&
                    selection.adapted(i).has_parent())
                {
                    valueNodeName = selection.adapted(i).parent().name();
                }
                else
                {
                    valueNodeName = selection.adapted(i).name();
                }

                errors.push_back(FileScope(selection.adapted(i)) + Error::MinMax(
                    selection.adapted(i).line(), selection.adapted(i).column(),
                    valueNodeName, selection.adapted(i).to_string(), ruleName,
                    ruleValue));
                pass = false;
            }
        }
    }

    return pass;
}

template<class SchemaAdapter, class InputAdapter>
bool HIVE::validateExistsIn(SchemaAdapter&            schema_node,
                            InputAdapter&             input_node,
                            std::vector<std::string>& errors)
{
    auto schema_node_parent = schema_node.parent();
    wasp_check(schema_node_parent.is_null() == false);
    std::string nodePath         = schema_node_parent.path();
    std::string ruleName         = getFullRuleName(schema_node.name());
    std::string ruleId           = schema_node.id();
    bool        pass             = true;
    int         errorCount       = 0;
    bool        absRule          = false;
    bool        beforePeriodRule = false;

    if (ruleId != "" && ruleId != "Abs" && ruleId != "BeforePeriod")
    {
        errors.push_back(FileScope(schema_node) + Error::BadOption(
            schema_node.name(), ruleId, schema_node.id_child().line(),
            schema_node.id_child().column(), "Abs BeforePeriod"));
        return false;
    }
    if (ruleId == "Abs")
    {
        absRule = true;
    }
    if (ruleId == "BeforePeriod")
    {
        beforePeriodRule = true;
    }

    std::stringstream       look_up_error;
    DefaultSIRENInterpreter inputSelector(look_up_error);
    if (!inputSelector.parseString(nodePath))
    {
        errors.push_back(FileScope(schema_node_parent) + Error::SirenParseError(schema_node_parent,
                                                look_up_error.str()));
        return false;
    }
    SIRENResultSet<InputAdapter> selection;
    inputSelector.evaluate(input_node, selection);

    // CREATE LOOKUP UNORDERED SET
    std::unordered_set<std::string> lookupSet;
    std::set<std::string>*          refSetPtr = NULL;
    std::set<std::string>           aliasNamesFound;
    InputAdapter                    savedParentInputNode;
    InputAdapter                    tmpParentInputNode;
    bool                            setContextNow = false;
    bool                            clearSetsNow  = false;

    bool expressionsAreOkay = false;

    const typename SchemaAdapter::Collection& refNodes =
        schema_node.child_by_name("EXTRAREF");
    std::vector<std::string> refNames;
    for (int i = 0; i < refNodes.size(); i++)
    {
        refNames.push_back(refNodes[i].to_string());
    }
    sort(refNames.begin(), refNames.end());
    std::string refTag =
        accumulate(refNames.begin(), refNames.end(), std::string(""));
    if (refNames.size() != 0)
    {
        std::map<std::string, std::set<std::string>>::iterator enumRefIter;
        enumRefIter = enumRef.find(refTag);

        if (enumRefIter == enumRef.end())
        {
            enumRef.insert(make_pair(refTag, std::set<std::string>()));
            enumRefIter = enumRef.find(refTag);

            for (int i = 0; i < refNames.size(); i++)
            {
                std::string refName = refNames[i];

                SchemaAdapter tmpschema_node = schema_node;
                while (tmpschema_node.has_parent())
                {
                    tmpschema_node = tmpschema_node.parent();
                }
                tmpschema_node =
                    tmpschema_node.first_non_decorative_child_by_name(refName);

                if (tmpschema_node.is_null())
                {
                    auto aNode = schema_node.non_decorative_children()[0];
                    errors.push_back(FileScope(aNode) + Error::BadEnumReference(
                        refName,
                        aNode.line(),
                        aNode.column()));
                    return false;
                }

                const typename SchemaAdapter::Collection& children =
                    tmpschema_node.non_decorative_children();
                for (int ic = children.size() - 1; ic >= 0; ic--)
                {
                    std::string lowerString = children[ic].to_string();
                    transform(lowerString.begin(), lowerString.end(),
                              lowerString.begin(), ::tolower);
                    enumRefIter->second.insert(lowerString);
                }
            }
        }
        refSetPtr = &enumRefIter->second;
    }
    const typename SchemaAdapter::Collection& children =
        schema_node.non_decorative_children();
    std::map<int, typename DefaultSIRENInterpreter::SharedPtr>
        childrenSelectors;
    for (size_t i = 0; i < selection.size(); i++)
    {
        for (int loop = 0, count = children.size(); loop < count; loop++)
        {
            if (loop == 0)
                setContextNow = true;
            if (std::strcmp(children[loop].name(), "EXTRA") == 0)
            {
                std::string lowerString = children[loop].to_string();
                if (lowerString == "ExpressionsAreOkay")
                    expressionsAreOkay = true;
                else
                {
                    transform(lowerString.begin(), lowerString.end(),
                              lowerString.begin(), ::tolower);
                    lookupSet.insert(lowerString);
                }
                continue;
            }

            if (std::strcmp(children[loop].name(), "RANGE") == 0)
            {
                const typename SchemaAdapter::Collection& rangeChildren =
                    children[loop].non_decorative_children();
                if (rangeChildren.size() != 2)
                {
                    errors.push_back(FileScope(children[loop]) + Error::RangeNotTwoVals(
                        children[loop].line(), children[loop].column()));
                    pass = false;
                }
                else
                {
                    int                itestStart;
                    int                itestEnd;
                    std::istringstream rangeStreamStart(
                        rangeChildren[0].to_string());
                    std::istringstream rangeStreamEnd(
                        rangeChildren[1].to_string());
                    rangeStreamStart >> std::noskipws >> itestStart;
                    rangeStreamEnd >> std::noskipws >> itestEnd;
                    if (!rangeStreamStart.eof() || rangeStreamStart.fail())
                    {
                        errors.push_back(FileScope(rangeChildren[0]) + Error::RangeNotValidNum(
                            rangeChildren[0].to_string(),
                            rangeChildren[0].line(),
                            rangeChildren[0].column()));
                        pass = false;
                    }
                    else if (!rangeStreamEnd.eof() || rangeStreamEnd.fail())
                    {
                        errors.push_back(FileScope(rangeChildren[1]) + Error::RangeNotValidNum(
                            rangeChildren[1].to_string(),
                            rangeChildren[1].line(),
                            rangeChildren[1].column()));
                        pass = false;
                    }
                    else
                    {
                        int rangeStart = stoi(rangeChildren[0].to_string());
                        int rangeEnd   = stoi(rangeChildren[1].to_string());
                        if (rangeStart >= rangeEnd)
                        {
                            errors.push_back(FileScope(rangeChildren[0]) + Error::RangeInvalid(
                                rangeChildren[0].to_string(),
                                rangeChildren[1].to_string(),
                                rangeChildren[0].line(),
                                rangeChildren[0].column()));
                            pass = false;
                        }
                        else
                        {
                            for (int rangeLoop = rangeStart;
                                 rangeLoop <= rangeEnd; rangeLoop++)
                            {
                                lookupSet.insert(std::to_string(rangeLoop));
                            }
                        }
                    }
                }
                continue;
            }

            if (std::strcmp(children[loop].name(), "EXTRAREF") == 0)
            {
                continue;
            }

            std::string ruleValue   = children[loop].to_string();
            int         parentCount = ruleValue.find_first_not_of("./") / 3;
            if (setContextNow)
            {
                tmpParentInputNode = selection.adapted(i);
                for (int parentLoop = 0; parentLoop < parentCount; parentLoop++)
                {
                    if (!tmpParentInputNode.has_parent())
                    {
                        errors.push_back(FileScope(schema_node) + Error::BadSchemaPath(
                            schema_node.name(), ruleValue,
                            children[loop].line(), children[loop].column()));
                        return false;
                    }
                    tmpParentInputNode = tmpParentInputNode.parent();
                    setContextNow      = false;
                    clearSetsNow       = true;
                }
            }

            if (!(tmpParentInputNode == savedParentInputNode))
            {
                if (!savedParentInputNode.is_null() && clearSetsNow)
                {
                    lookupSet.clear();
                    aliasNamesFound.clear();
                    clearSetsNow = false;
                }
                auto sitr = childrenSelectors.find(loop);
                // if no children selector has been created, do so now
                if (sitr == childrenSelectors.end())
                {
                    // this avoids runtime cost of reparsing the same
                    // siren expression many times
                    typename DefaultSIRENInterpreter::SharedPtr
                        inputSelectorLookup =
                            std::make_shared<DefaultSIRENInterpreter>(
                                look_up_error);
                    if (!inputSelectorLookup->parseString(ruleValue))
                    {
                        errors.push_back(FileScope(schema_node) + 
                            Error::SirenParseError(schema_node,
                                                   look_up_error.str()));
                        return false;
                    }

                    childrenSelectors.insert(
                        std::make_pair(loop, inputSelectorLookup));
                }

                SIRENResultSet<InputAdapter> selectionLookup;
                InputAdapter                 inode = selection.adapted(i);
                childrenSelectors[loop]->evaluate(inode, selectionLookup);

                for (size_t j = 0; j < selectionLookup.size(); j++)
                {
                    std::string tempString =
                        selectionLookup.adapted(j).to_string();
                    if (absRule &&
                        (tempString.at(0) == '-' || tempString.at(0) == '+'))
                    {
                        tempString.erase(tempString.begin());
                    }
                    size_t periodIndex = tempString.find('.');
                    if (beforePeriodRule && periodIndex != std::string::npos)
                    {
                        tempString = tempString.substr(0, periodIndex);
                    }
                    size_t zeroIndex = tempString.find_first_not_of('0');
                    if (zeroIndex != 0 && zeroIndex != std::string::npos)
                    {
                        int                itest;
                        std::istringstream iss(tempString);
                        iss >> std::noskipws >> itest;
                        if (iss.eof() && !iss.fail())
                        {
                            tempString.erase(0, zeroIndex);
                        }
                    }
                    if (selectionLookup.adapted(j).has_parent() &&
                        std::strcmp(selectionLookup.adapted(j).parent().name(),
                                    "alias") != 0)
                    {
                        transform(tempString.begin(), tempString.end(),
                                  tempString.begin(), ::tolower);
                    }
                    lookupSet.insert(tempString);
                }
            }
        }
        savedParentInputNode     = tmpParentInputNode;
        std::string lookupString = selection.adapted(i).to_string();
        if (absRule && (lookupString.at(0) == '-' || lookupString.at(0) == '+'))
        {
            lookupString.erase(lookupString.begin());
        }
        size_t periodIndex = lookupString.find('.');
        if (beforePeriodRule && periodIndex != std::string::npos)
        {
            lookupString = lookupString.substr(0, periodIndex);
        }
        size_t zeroIndex = lookupString.find_first_not_of('0');
        if (zeroIndex != 0 && zeroIndex != std::string::npos)
        {
            int                itest;
            std::istringstream iss(lookupString);
            iss >> std::noskipws >> itest;
            if (iss.eof() && !iss.fail())
            {
                lookupString.erase(0, zeroIndex);
            }
        }

        std::string lowerLookupString = lookupString;
        transform(lowerLookupString.begin(), lowerLookupString.end(),
                  lowerLookupString.begin(), ::tolower);

        if ((std::strcmp(selection.adapted(i).parent().name(), "alias") == 0 ||
             lookupSet.find(lowerLookupString) == lookupSet.end()) &&
            (std::strcmp(selection.adapted(i).parent().name(), "alias") != 0 ||
             lookupSet.find(lookupString) == lookupSet.end()) &&
            (refSetPtr == NULL ||
             refSetPtr->find(lowerLookupString) == refSetPtr->end()))
        {
            if (expressionsAreOkay && is_valid_expression(lookupString))
                continue;
            std::string valueNodeName;
            if (std::strcmp(selection.adapted(i).name(), "value") == 0 &&
                selection.adapted(i).has_parent())
            {
                valueNodeName = selection.adapted(i).parent().name();
            }
            else
            {
                valueNodeName = selection.adapted(i).name();
            }

            const typename SchemaAdapter::Collection& childrenErr =
                schema_node.non_decorative_children();
            std::string lookupPaths;
            for (int loop = 0, count = childrenErr.size(); loop < count; loop++)
            {
                if (std::strcmp(childrenErr[loop].name(), "EXTRA") != 0 &&
                    std::strcmp(childrenErr[loop].name(), "EXTRAREF") != 0 &&
                    std::strcmp(childrenErr[loop].name(), "RANGE") != 0)
                {
                    lookupPaths += childrenErr[loop].to_string();
                    if (loop + 1 != count)
                    {
                        lookupPaths += "\n        ";
                    }
                }
            }

            errors.push_back(FileScope(selection.adapted(i)) + Error::NotExistsIn(
                selection.adapted(i).line(), selection.adapted(i).column(),
                valueNodeName, lookupString, lookupPaths));
            errorCount++;
            if (errorCount >= MAXERRORS)
            {
                errors.push_back(FileScope(selection.adapted(i)) + Error::ErrorLimit(
                    selection.adapted(i).line(), selection.adapted(i).column(),
                    nodePath, ruleName, MAXERRORS));
                return false;
            }
            pass = false;
        }
    }
    return pass;
}
template<class SchemaAdapter, class InputAdapter>
bool HIVE::validateNotExistsIn(SchemaAdapter&            schema_node,
                               InputAdapter&             input_node,
                               std::vector<std::string>& errors)
{
    auto schema_node_parent = schema_node.parent();
    wasp_check(schema_node_parent.is_null() == false);
    std::string nodePath   = schema_node_parent.path();
    std::string ruleName   = getFullRuleName(schema_node.name());
    std::string ruleId     = schema_node.id();
    bool        pass       = true;
    int         errorCount = 0;
    bool        absRule    = false;
    bool        zeroRule   = false;

    // check the optional option supplied to this rule
    if ((ruleId != "") && (ruleId != "Abs") && (ruleId != "Zero") &&
        (ruleId != "AbsZero"))
    {
        errors.push_back(FileScope(schema_node) + Error::BadOption(
            schema_node.name(), ruleId, schema_node.id_child().line(),
            schema_node.id_child().column(), "Abs Zero AbsZero"));
        return false;
    }

    if ((ruleId == "Abs") || (ruleId == "AbsZero"))
    {
        absRule = true;
    }

    if ((ruleId == "Zero") || (ruleId == "AbsZero"))
    {
        zeroRule = true;
    }

    // gather all of the nodes for which this rule applies
    std::stringstream       look_up_error;
    DefaultSIRENInterpreter inputSelector(look_up_error);

    if (!inputSelector.parseString(nodePath))
    {
        errors.push_back(FileScope(schema_node_parent) + Error::SirenParseError(schema_node_parent,
                                                look_up_error.str()));
        return false;
    }
    SIRENResultSet<InputAdapter> selection;
    inputSelector.evaluate(input_node, selection);

    // gather all of the lookup paths for this rule
    const typename SchemaAdapter::Collection& lookupPaths =
        schema_node.non_decorative_children();

    // loop through all of the lookup paths for this rule
    for (int j = 0; j < lookupPaths.size(); j++)
    {
        std::string lookupPath = lookupPaths[j].to_string();

        // std::set up the siren for this specific lookup path
        std::stringstream       look_up_error;
        DefaultSIRENInterpreter childSelector(look_up_error);

        if (!childSelector.parseString(lookupPath))
        {
            errors.push_back(FileScope(lookupPaths[j]) + Error::SirenParseError(lookupPaths[j],
                                                    look_up_error.str()));
            return false;
        }

        // loop over all of the nodes for which this rule applies
        for (size_t i = 0; i < selection.size(); i++)
        {
            std::unordered_map<std::string, InputAdapter> lookupMap;

            // gather all of the nodes that are adapted this lookup path
            // relative to this node
            SIRENResultSet<InputAdapter> childSelection;
            InputAdapter                 inode = selection.adapted(i);
            childSelector.evaluate(inode, childSelection);

            // loop over all of these nodes and add their values to
            // a lookup map or a std::set of ranges if an alias
            for (size_t loop = 0; loop < childSelection.size(); loop++)
            {
                // get the value
                std::string insertString =
                    childSelection.adapted(loop).to_string();
                transform(insertString.begin(), insertString.end(),
                          insertString.begin(), ::tolower);

                // if this is not an alias, then modify the std::string based on
                // the optional
                // flag supplied to this rule, and add it to the lookup
                {
                    if (absRule && ((insertString.at(0) == '-') ||
                                    (insertString.at(0) == '+')))
                    {
                        insertString.erase(insertString.begin());
                    }
                    size_t zeroIndex = insertString.find_first_not_of('0');

                    if ((zeroIndex != 0) && (zeroIndex != std::string::npos))
                    {
                        int                itest;
                        std::istringstream iss(insertString);
                        iss >> std::noskipws >> itest;

                        if (iss.eof() && !iss.fail())
                        {
                            insertString.erase(0, zeroIndex);
                        }
                    }
                    lookupMap.insert(
                        make_pair(insertString, childSelection.adapted(loop)));
                }
            }

            // now we will check all values adapted this node against the lookup
            // map
            // created above and list std::set of alias ranges created above

            // get the value
            std::string initialString = selection.adapted(i).to_string();
            transform(initialString.begin(), initialString.end(),
                      initialString.begin(), ::tolower);

            // determine if it is an alias or not
            // if it is an alias, then std::set the outer loop to the number
            // of ranges for this alias
            // otherwise, std::set the outer loop to just 1,
            // so we only do the logic once
            //           bool isAlias = false;
            int outterloop = 1;

            // if an alias, then loop over all of the ranges
            // otherwise, only run through this logic once
            for (int j = 0; j < outterloop; j++)
            {
                // if this is an alias, then std::set the inner loop to be
                // from the start of this range to the end of this range
                // otherwise, only run through this logic once
                int innerloopstart = 1;
                int innerloopend   = 1;

                // if an alias, then loop over all values in this range
                // otherwise, just run through the logic once
                for (int l = innerloopstart; l <= innerloopend; l++)
                {
                    // if an alias, then std::set the lookupstring to be this
                    // specific integer within this range of this alias
                    // otherwise, modify the std::string based on the optional
                    // flag supplied to the rule
                    std::string lookupString;

                    //                   if (isAlias){
                    //                       lookupString = to_string(l);
                    //                   }
                    //                   else
                    {
                        lookupString = initialString;

                        if (zeroRule && (lookupString == "0"))
                        {
                            continue;
                        }

                        if (absRule && ((lookupString.at(0) == '-') ||
                                        (lookupString.at(0) == '+')))
                        {
                            lookupString.erase(lookupString.begin());
                        }
                        size_t zeroIndex = lookupString.find_first_not_of('0');

                        if ((zeroIndex != 0) &&
                            (zeroIndex != std::string::npos))
                        {
                            int                itest;
                            std::istringstream iss(lookupString);
                            iss >> std::noskipws >> itest;

                            if (iss.eof() && !iss.fail())
                            {
                                lookupString.erase(0, zeroIndex);
                            }
                        }
                    }

                    // if this value is in the std::set of non aliases we have
                    // already seen
                    // or in the std::set of aliases we have already seen, then
                    // we have a match
                    // and an input validation error
                    InputAdapter foundNode;
                    auto         lookupMapIter = lookupMap.find(lookupString);

                    if (lookupMapIter != lookupMap.end())
                    {
                        if (foundNode.is_null())
                            foundNode = lookupMapIter->second;

                        // if the name ofthis node is just the word value
                        // then std::set the name that will be reported to be
                        // its parent's name
                        std::string valueNodeName;

                        if ((std::strcmp(selection.adapted(i).name(),
                                         "value") == 0) &&
                            selection.adapted(i).has_parent())
                        {
                            valueNodeName =
                                selection.adapted(i).parent().name();
                        }
                        else
                        {
                            valueNodeName = selection.adapted(i).name();
                        }
                        errors.push_back(FileScope(selection.adapted(i)) + Error::AlsoExistsAt(
                            selection.adapted(i).line(),
                            selection.adapted(i).column(), valueNodeName,
                            lookupString, lookupPath, foundNode.line(),
                            foundNode.column(), ruleName));

                        // if we max out our errors, then report that and bail
                        // out
                        errorCount++;

                        if (errorCount >= MAXERRORS)
                        {
                            errors.push_back(FileScope(selection.adapted(i)) + Error::ErrorLimit(
                                selection.adapted(i).line(),
                                selection.adapted(i).column(), nodePath,
                                ruleName, MAXERRORS));
                            return false;
                        }

                        pass = false;
                    }
                }
            }
        }
    }

    // return whether this rule passed everything
    // or had adapted least a single validation failure
    return pass;
}

template<class SchemaAdapter, class InputAdapter>
bool HIVE::validateSumOver(SchemaAdapter&            schema_node,
                           InputAdapter&             input_node,
                           std::vector<std::string>& errors)
{
    auto schema_node_parent = schema_node.parent();
    wasp_check(schema_node_parent.is_null() == false);
    std::string nodeName  = schema_node_parent.name();
    std::string nodePath  = schema_node_parent.path();
    std::string ruleName  = getFullRuleName(schema_node.name());
    std::string ruleValue = schema_node.to_string();
    std::string ruleId    = schema_node.id();
    bool        pass      = true;

    std::stringstream look_up_error;

    DefaultSIRENInterpreter inputSelector(look_up_error);

    if (!inputSelector.parseString(nodePath + "/" + ruleId))
    {
        errors.push_back(FileScope(schema_node) + Error::SirenParseError(schema_node,
                                                look_up_error.str()));
        return false;
    }
    SIRENResultSet<InputAdapter> selection;
    inputSelector.evaluate(input_node, selection);

    if (selection.size() != 0)
    {
        std::stringstream       look_up_error;
        DefaultSIRENInterpreter inputSelectorlookup(look_up_error);

        if (!inputSelectorlookup.parseString(selection.adapted(0).path()))
        {
            errors.push_back(FileScope(schema_node) + Error::SirenParseError(schema_node,
                                                    look_up_error.str()));
            return false;
        }
        SIRENResultSet<InputAdapter> selectionLookup;
        inputSelectorlookup.evaluate(input_node, selectionLookup);

        for (size_t i = 0; i < selectionLookup.size(); i++)
        {
            std::stringstream       look_up_error;
            DefaultSIRENInterpreter sumSelector(look_up_error);

            if (!sumSelector.parseString(
                    nodePath.substr(selection.adapted(0).path().length() + 1)))
            {
                errors.push_back(FileScope(schema_node) + Error::SirenParseError(schema_node,
                                                        look_up_error.str()));
                return false;
            }

            SIRENResultSet<InputAdapter> sumSelection;
            InputAdapter                 inode = selectionLookup.adapted(i);
            sumSelector.evaluate(inode, sumSelection);

            if (sumSelection.size() != 0)
            {
                double sum          = 0;
                bool   numberslegal = true;

                for (size_t j = 0; j < sumSelection.size(); j++)
                {
                    std::istringstream iss(sumSelection.adapted(j).to_string());
                    float              ftest;
                    iss >> std::noskipws >> ftest;

                    if (!iss.eof() || iss.fail())
                    {
                        std::string valueNodeName;

                        if ((std::strcmp(sumSelection.adapted(j).name(),
                                         "value") == 0) &&
                            sumSelection.adapted(j).has_parent())
                        {
                            valueNodeName =
                                sumSelection.adapted(j).parent().name();
                        }
                        else
                        {
                            valueNodeName = sumSelection.adapted(j).name();
                        }

                        errors.push_back(FileScope(sumSelection.adapted(i)) + Error::WrongTypeForRule(
                            sumSelection.adapted(j).line(),
                            sumSelection.adapted(j).column(), valueNodeName,
                            sumSelection.adapted(j).to_string(), ruleName));
                        pass         = false;
                        numberslegal = false;
                    }
                    else
                    {
                        sum += stod(sumSelection.adapted(j).to_string());
                    }
                }

                if ((numberslegal == true) &&
                    ((sum > (stod(ruleValue) + SUMERROR)) ||
                     (sum < (stod(ruleValue) - SUMERROR))))
                {
                    std::stringstream tmpsumstream;
                    tmpsumstream << sum;
                    errors.push_back(FileScope(selectionLookup.adapted(i)) + Error::SumProd(
                        selectionLookup.adapted(i).line(),
                        selectionLookup.adapted(i).column(),
                        selectionLookup.adapted(i).name(), nodeName,
                        tmpsumstream.str(), ruleName, ruleValue, ruleId));
                    pass = false;
                }
            }
        }
    }

    return pass;
}

template<class SchemaAdapter, class InputAdapter>
bool HIVE::validateSumOverGroup(SchemaAdapter&            schema_node,
                                InputAdapter&             input_node,
                                std::vector<std::string>& errors)
{
    bool          pass     = true;
    std::string   nodeName = schema_node.parent().name();
    std::string   nodePath = schema_node.parent().path();
    std::string   ruleName = getFullRuleName(schema_node.name());
    std::string   ruleId   = schema_node.id();
    SchemaAdapter compare_path_schema_node =
        schema_node.first_child_by_name("ComparePath");

    if (compare_path_schema_node.is_null())
    {
        errors.push_back(FileScope(schema_node) + 
            Error::MissingArgument(schema_node.name(), "ComparePath",
                                   schema_node.line(), schema_node.column()));
        return false;
    }
    SchemaAdapter group_divide_schema_node =
        schema_node.first_child_by_name("GroupDivide");

    if (group_divide_schema_node.is_null())
    {
        errors.push_back(FileScope(schema_node) + 
            Error::MissingArgument(schema_node.name(), "GroupDivide",
                                   schema_node.line(), schema_node.column()));
        return false;
    }
    SchemaAdapter group_sum_schema_node =
        schema_node.first_child_by_name("GroupSum");

    if (group_sum_schema_node.is_null())
    {
        errors.push_back(FileScope(schema_node) + Error::MissingArgument(schema_node.name(), "GroupSum",
                                                schema_node.line(),
                                                schema_node.column()));
        return false;
    }

    std::string comparePath = compare_path_schema_node.to_string();
    int         groupDivide = group_divide_schema_node.to_int();
    // TODO check zero as divisor
    double                  groupSum = group_sum_schema_node.to_double();
    std::stringstream       look_up_error;
    DefaultSIRENInterpreter inputSelector(look_up_error);

    if (!inputSelector.parseString(nodePath + "/" + ruleId))
    {
        errors.push_back(FileScope(schema_node) + Error::SirenParseError(schema_node.parent(),
                                                look_up_error.str()));
        return false;
    }
    SIRENResultSet<InputAdapter> selection;
    inputSelector.evaluate(input_node, selection);

    if (selection.size() != 0)
    {
        std::stringstream       look_up_error;
        DefaultSIRENInterpreter inputSelectorlookup(look_up_error);

        if (!inputSelectorlookup.parseString(selection.adapted(0).path()))
        {
            errors.push_back(FileScope(schema_node) + Error::SirenParseError(schema_node,
                                                    look_up_error.str()));
            return false;
        }
        SIRENResultSet<InputAdapter> selectionLookup;
        inputSelectorlookup.evaluate(input_node, selectionLookup);

        DefaultSIRENInterpreter sumSelector(look_up_error);

        if (!sumSelector.parseString(
                nodePath.substr(selection.adapted(0).path().length() + 1)))
        {
            errors.push_back(FileScope(schema_node) + Error::SirenParseError(schema_node,
                                                    look_up_error.str()));
            return false;
        }
        DefaultSIRENInterpreter comparePathSelector(look_up_error);

        if (!comparePathSelector.parseString(comparePath))
        {
            errors.push_back(FileScope(compare_path_schema_node) + Error::SirenParseError(compare_path_schema_node,
                                                    look_up_error.str()));
            return false;
        }

        for (size_t i = 0; i < selectionLookup.size(); i++)
        {
            SIRENResultSet<InputAdapter> sumSelection;
            InputAdapter                 inode = selectionLookup.adapted(i);
            sumSelector.evaluate(inode, sumSelection);

            typename std::map<int, std::vector<InputAdapter>> groupAddends;
            typename std::map<int, typename std::vector<InputAdapter>>::iterator
                groupAddendsIter;

            for (size_t j = 0; j < sumSelection.size(); j++)
            {
                SIRENResultSet<InputAdapter> comparePathSelection;
                InputAdapter                 jnode = sumSelection.adapted(j);
                comparePathSelector.evaluate(jnode, comparePathSelection);

                int tempCompareQuotient;

                if (comparePathSelection.size() != 0)
                {
                    tempCompareQuotient =
                        comparePathSelection.adapted(0).to_int() / groupDivide;

                    groupAddendsIter = groupAddends.find(tempCompareQuotient);

                    if (groupAddendsIter != groupAddends.end())
                    {
                        groupAddendsIter->second.push_back(
                            sumSelection.adapted(j));
                    }
                    else
                    {
                        groupAddends[tempCompareQuotient] = {
                            sumSelection.adapted(j)};
                    }
                }
            }

            if (groupAddends.size() != 0)
            {
                for (groupAddendsIter = groupAddends.begin();
                     groupAddendsIter != groupAddends.end(); groupAddendsIter++)
                {
                    double tempSum      = 0;
                    bool   numberslegal = true;

                    for (size_t k = 0; k < groupAddendsIter->second.size(); k++)
                    {
                        std::istringstream iss(
                            groupAddendsIter->second[k].to_string());
                        float ftest;
                        iss >> std::noskipws >> ftest;

                        if (!iss.eof() || iss.fail())
                        {
                            std::string valueNodeName;

                            if ((std::strcmp(groupAddendsIter->second[k].name(),
                                             "value") == 0) &&
                                groupAddendsIter->second[k].has_parent())
                            {
                                valueNodeName =
                                    groupAddendsIter->second[k].parent().name();
                            }
                            else
                            {
                                valueNodeName =
                                    groupAddendsIter->second[k].name();
                            }

                            errors.push_back(FileScope(groupAddendsIter->second[k]) + Error::WrongTypeForRule(
                                groupAddendsIter->second[k].line(),
                                groupAddendsIter->second[k].column(),
                                valueNodeName,
                                groupAddendsIter->second[k].to_string(),
                                ruleName));
                            pass         = false;
                            numberslegal = false;
                        }
                        else
                        {
                            tempSum +=
                                stod(groupAddendsIter->second[k].to_string());
                        }
                    }

                    if ((numberslegal == true) &&
                        ((tempSum > (groupSum + SUMERROR)) ||
                         (tempSum < (groupSum - SUMERROR))))
                    {
                        std::stringstream tmptsstream;
                        tmptsstream << tempSum;
                        std::stringstream tmpgsstream;
                        tmpgsstream << groupSum;
                        errors.push_back(FileScope(selectionLookup.adapted(i)) + Error::SumProdGroup(
                            selectionLookup.adapted(i).line(),
                            selectionLookup.adapted(i).column(),
                            selectionLookup.adapted(i).name(), nodeName,
                            tmptsstream.str(), comparePath, groupDivide,
                            groupAddendsIter->first, ruleName,
                            tmpgsstream.str(), ruleId));
                        pass = false;
                    }
                }
            }
        }
    }

    return pass;
}

template<class SchemaAdapter, class InputAdapter>
bool HIVE::validateIncreaseOver(SchemaAdapter&            schema_node,
                                InputAdapter&             input_node,
                                std::vector<std::string>& errors)
{
    std::string nodeName  = schema_node.parent().name();
    std::string nodePath  = schema_node.parent().path();
    std::string ruleName  = getFullRuleName(schema_node.name());
    std::string ruleValue = schema_node.to_string();
    std::string ruleId    = schema_node.id();
    bool        pass      = true;

    if ((ruleValue != "Mono") && (ruleValue != "Strict"))
    {
        errors.push_back(FileScope(schema_node) + Error::BadOption(
            schema_node.name(), ruleValue,
            schema_node.non_decorative_children()[0].line(),
            schema_node.non_decorative_children()[0].column(), "Mono Strict"));
        return false;
    }
    std::stringstream       look_up_error;
    DefaultSIRENInterpreter inputSelector(look_up_error);

    if (!inputSelector.parseString(nodePath + "/" + ruleId))
    {
        errors.push_back(FileScope(schema_node) + Error::SirenParseError(schema_node.parent(),
                                                look_up_error.str()));
        return false;
    }
    SIRENResultSet<InputAdapter> selection;
    inputSelector.evaluate(input_node, selection);

    if (selection.size() != 0)
    {
        DefaultSIRENInterpreter inputSelectorLookup(look_up_error);

        if (!inputSelectorLookup.parseString(selection.adapted(0).path()))
        {
            errors.push_back(FileScope(schema_node) + Error::SirenParseError(schema_node.parent(),
                                                    look_up_error.str()));
            return false;
        }
        SIRENResultSet<InputAdapter> selectionLookup;
        inputSelectorLookup.evaluate(input_node, selectionLookup);

        for (size_t i = 0; i < selectionLookup.size(); i++)
        {
            DefaultSIRENInterpreter incrSelector(look_up_error);

            if (!incrSelector.parseString(
                    nodePath.substr(selection.adapted(0).path().length() + 1)))
            {
                errors.push_back(FileScope(schema_node) + Error::SirenParseError(schema_node.parent(),
                                                        look_up_error.str()));
                return false;
            }

            SIRENResultSet<InputAdapter> incrSelection;
            InputAdapter                 inode = selectionLookup.adapted(i);
            incrSelector.evaluate(inode, incrSelection);

            bool numberslegal = true;

            for (size_t j = 0;
                 incrSelection.size() != 0 && j < incrSelection.size() - 1; j++)
            {
                if (j == 0)
                {
                    std::istringstream issFirst(
                        incrSelection.adapted(j).to_string());
                    float ftestFirst;
                    issFirst >> std::noskipws >> ftestFirst;

                    if (!issFirst.eof() || issFirst.fail())
                    {
                        std::string valueNodeName;

                        if ((std::strcmp(incrSelection.adapted(j).name(),
                                         "value") == 0) &&
                            incrSelection.adapted(j).has_parent())
                        {
                            valueNodeName =
                                incrSelection.adapted(j).parent().name();
                        }
                        else
                        {
                            valueNodeName = incrSelection.adapted(j).name();
                        }

                        errors.push_back(FileScope(incrSelection.adapted(j)) + Error::WrongTypeForRule(
                            incrSelection.adapted(j).line(),
                            incrSelection.adapted(j).column(), valueNodeName,
                            incrSelection.adapted(j).to_string(), ruleName));
                        pass         = false;
                        numberslegal = false;
                    }
                }

                std::istringstream issSecond(
                    incrSelection.adapted(j + 1).to_string());
                float ftestSecond;
                issSecond >> std::noskipws >> ftestSecond;

                if (!issSecond.eof() || issSecond.fail())
                {
                    std::string valueNodeName;

                    if ((std::strcmp(incrSelection.adapted(j + 1).name(),
                                     "value") == 0) &&
                        incrSelection.adapted(j + 1).has_parent())
                    {
                        valueNodeName =
                            incrSelection.adapted(j + 1).parent().name();
                    }
                    else
                    {
                        valueNodeName = incrSelection.adapted(j + 1).name();
                    }

                    errors.push_back(FileScope(incrSelection.adapted(j+1)) + Error::WrongTypeForRule(
                        incrSelection.adapted(j + 1).line(),
                        incrSelection.adapted(j + 1).column(), valueNodeName,
                        incrSelection.adapted(j + 1).to_string(), ruleName));
                    pass         = false;
                    numberslegal = false;
                }

                if ((numberslegal == true) && (ruleValue == "Mono") &&
                    (stod(incrSelection.adapted(j).to_string()) >
                     stod(incrSelection.adapted(j + 1).to_string())))
                {
                    errors.push_back(FileScope(selectionLookup.adapted(i)) + Error::IncreaseDecrease(
                        selectionLookup.adapted(i).line(),
                        selectionLookup.adapted(i).column(),
                        selectionLookup.adapted(i).name(), nodeName, ruleValue,
                        ruleName, ruleId, incrSelection.adapted(j + 1).line(),
                        incrSelection.adapted(j + 1).column()));
                    pass = false;
                }

                else if ((numberslegal == true) && (ruleValue == "Strict") &&
                         (stod(incrSelection.adapted(j).to_string()) >=
                          stod(incrSelection.adapted(j + 1).to_string())))
                {
                    errors.push_back(FileScope(selectionLookup.adapted(i)) + Error::IncreaseDecrease(
                        selectionLookup.adapted(i).line(),
                        selectionLookup.adapted(i).column(),
                        selectionLookup.adapted(i).name(), nodeName, ruleValue,
                        ruleName, ruleId, incrSelection.adapted(j + 1).line(),
                        incrSelection.adapted(j + 1).column()));
                    pass = false;
                }
            }
        }
    }

    return pass;
}

template<class SchemaAdapter, class InputAdapter>
bool HIVE::validateDecreaseOver(SchemaAdapter&            schema_node,
                                InputAdapter&             input_node,
                                std::vector<std::string>& errors)
{
    std::string nodeName  = schema_node.parent().name();
    std::string nodePath  = schema_node.parent().path();
    std::string ruleName  = getFullRuleName(schema_node.name());
    std::string ruleValue = schema_node.to_string();
    std::string ruleId    = schema_node.id();
    bool        pass      = true;
    if ((ruleValue != "Mono") && (ruleValue != "Strict"))
    {
        errors.push_back(FileScope(schema_node) + Error::BadOption(
            schema_node.name(), ruleValue,
            schema_node.non_decorative_children()[0].line(),
            schema_node.non_decorative_children()[0].column(), "Mono Strict"));
        return false;
    }

    std::stringstream       look_up_error;
    DefaultSIRENInterpreter inputSelector(look_up_error);
    std::string             path = nodePath + "/" + ruleId;
    if (!inputSelector.parseString(path))
    {
        errors.push_back(FileScope(schema_node) + Error::SirenParseError(schema_node,
                                                look_up_error.str()));
        return false;
    }
    SIRENResultSet<InputAdapter> selection;
    inputSelector.evaluate(input_node, selection);
    if (selection.size() != 0)
    {
        DefaultSIRENInterpreter inputSelectorlookup(look_up_error);

        if (!inputSelectorlookup.parseString(selection.adapted(0).path()))
        {
            errors.push_back(FileScope(schema_node) + Error::SirenParseError(schema_node,
                                                    look_up_error.str()));
            return false;
        }
        SIRENResultSet<InputAdapter> selectionLookup;
        inputSelectorlookup.evaluate(input_node, selectionLookup);

        for (size_t i = 0; i < selectionLookup.size(); i++)
        {
            DefaultSIRENInterpreter decrSelector(look_up_error);

            if (!decrSelector.parseString(
                    nodePath.substr(selection.adapted(0).path().length() + 1)))
            {
                errors.push_back(FileScope(schema_node) + Error::SirenParseError(schema_node,
                                                        look_up_error.str()));
                return false;
            }

            SIRENResultSet<InputAdapter> decrSelection;
            InputAdapter                 inode = selectionLookup.adapted(i);
            decrSelector.evaluate(inode, decrSelection);

            bool numberslegal = true;

            for (size_t j = 0;
                 decrSelection.size() != 0 && j < decrSelection.size() - 1; j++)
            {
                if (j == 0)
                {
                    std::istringstream issFirst(
                        decrSelection.adapted(j).to_string());
                    float ftestFirst;
                    issFirst >> std::noskipws >> ftestFirst;

                    if (!issFirst.eof() || issFirst.fail())
                    {
                        std::string valueNodeName;

                        if ((std::strcmp(decrSelection.adapted(j).name(),
                                         "value") == 0) &&
                            decrSelection.adapted(j).has_parent())
                        {
                            valueNodeName =
                                decrSelection.adapted(j).parent().name();
                        }
                        else
                        {
                            valueNodeName = decrSelection.adapted(j).name();
                        }

                        errors.push_back(FileScope(decrSelection.adapted(j)) + Error::WrongTypeForRule(
                            decrSelection.adapted(j).line(),
                            decrSelection.adapted(j).column(), valueNodeName,
                            decrSelection.adapted(j).to_string(), ruleName));
                        pass         = false;
                        numberslegal = false;
                    }
                }

                std::istringstream issSecond(
                    decrSelection.adapted(j + 1).to_string());
                float ftestSecond;
                issSecond >> std::noskipws >> ftestSecond;

                if (!issSecond.eof() || issSecond.fail())
                {
                    std::string valueNodeName;

                    if ((std::strcmp(decrSelection.adapted(j + 1).name(),
                                     "value") == 0) &&
                        decrSelection.adapted(j + 1).has_parent())
                    {
                        valueNodeName =
                            decrSelection.adapted(j + 1).parent().name();
                    }
                    else
                    {
                        valueNodeName = decrSelection.adapted(j + 1).name();
                    }

                    errors.push_back(FileScope(decrSelection.adapted(j+1)) + Error::WrongTypeForRule(
                        decrSelection.adapted(j + 1).line(),
                        decrSelection.adapted(j + 1).column(), valueNodeName,
                        decrSelection.adapted(j + 1).to_string(), ruleName));
                    pass         = false;
                    numberslegal = false;
                }

                if ((numberslegal == true) && (ruleValue == "Mono") &&
                    (stod(decrSelection.adapted(j).to_string()) <
                     stod(decrSelection.adapted(j + 1).to_string())))
                {
                    errors.push_back(FileScope(selectionLookup.adapted(i)) + Error::IncreaseDecrease(
                        selectionLookup.adapted(i).line(),
                        selectionLookup.adapted(i).column(),
                        selectionLookup.adapted(i).name(), nodeName, ruleValue,
                        ruleName, ruleId, decrSelection.adapted(j + 1).line(),
                        decrSelection.adapted(j + 1).column()));
                    pass = false;
                }

                else if ((numberslegal == true) && (ruleValue == "Strict") &&
                         (stod(decrSelection.adapted(j).to_string()) <=
                          stod(decrSelection.adapted(j + 1).to_string())))
                {
                    errors.push_back(FileScope(selectionLookup.adapted(i)) + Error::IncreaseDecrease(
                        selectionLookup.adapted(i).line(),
                        selectionLookup.adapted(i).column(),
                        selectionLookup.adapted(i).name(), nodeName, ruleValue,
                        ruleName, ruleId, decrSelection.adapted(j + 1).line(),
                        decrSelection.adapted(j + 1).column()));
                    pass = false;
                }
            }
        }
    }

    return pass;
}

template<class SchemaAdapter, class InputAdapter>
bool HIVE::validateChildAtMostOne(SchemaAdapter&            schema_node,
                                  InputAdapter&             input_node,
                                  std::vector<std::string>& errors)
{
    std::string nodePath = schema_node.parent().path();
    std::string ruleName = getFullRuleName(schema_node.name());
    bool        pass     = true;

    std::stringstream look_up_error;

    DefaultSIRENInterpreter inputSelector(look_up_error);

    if (!inputSelector.parseString(nodePath))
    {
        errors.push_back(FileScope(schema_node) + Error::SirenParseError(schema_node.parent(),
                                                look_up_error.str()));
        return false;
    }
    SIRENResultSet<InputAdapter> selection;
    inputSelector.evaluate(input_node, selection);

    const typename SchemaAdapter::Collection& children =
        schema_node.non_decorative_children();
    std::vector<int> selectedChildrenCount(selection.size(), 0);

    for (int j = 0; j < children.size(); j++)
    {
        std::string lookupPath;

        if (children[j].child_count() == 0)
            lookupPath = children[j].to_string();
        else
            lookupPath = children[j].name();

        DefaultSIRENInterpreter childSelector(look_up_error);

        if (!childSelector.parseString(lookupPath))
        {
            errors.push_back(FileScope(schema_node) + Error::SirenParseError(schema_node,
                                                    look_up_error.str()));
            return false;
        }

        for (size_t i = 0; i < selection.size(); i++)
        {
            SIRENResultSet<InputAdapter> childSelection;
            InputAdapter                 inode = selection.adapted(i);
            childSelector.evaluate(inode, childSelection);

            if (childSelection.size() != 0)
            {
                if (children[j].child_count() != 0)
                {
                    for (size_t k = 0; k < childSelection.size(); k++)
                    {
                        std::string selectionLower =
                            childSelection.adapted(k).to_string();
                        std::string childrenJLower = children[j].to_string();
                        transform(selectionLower.begin(), selectionLower.end(),
                                  selectionLower.begin(), ::tolower);
                        transform(childrenJLower.begin(), childrenJLower.end(),
                                  childrenJLower.begin(), ::tolower);
                        if (selectionLower == childrenJLower)
                        {
                            selectedChildrenCount[i]++;
                            break;
                        }
                    }
                }
                else
                    selectedChildrenCount[i]++;
            }
        }
    }

    for (size_t i = 0; i < selection.size(); i++)
    {
        if (selectedChildrenCount[i] > 1)
        {
            const typename SchemaAdapter::Collection& choices = children;
            std::string                               childNames;

            for (int js = 0; js < choices.size(); js++)
            {
                childNames += choices[js].data();

                if (js + 1 != choices.size())
                    childNames += " ";
            }
            errors.push_back(FileScope(selection.adapted(i)) + Error::ChildMostExactLeast(
                selection.adapted(i).line(), selection.adapted(i).column(),
                selection.adapted(i).name(), childNames, ruleName));
            pass = false;
        }
    }

    return pass;
}

template<class SchemaAdapter, class InputAdapter>
bool HIVE::validateChildExactlyOne(SchemaAdapter&            schema_node,
                                   InputAdapter&             input_node,
                                   std::vector<std::string>& errors)
{
    std::string nodePath = schema_node.parent().path();
    std::string ruleName = getFullRuleName(schema_node.name());
    bool        pass     = true;

    SIRENResultSet<InputAdapter> selection;

    if (!select_nodes(selection, schema_node, input_node, nodePath, errors))
    {
        return false;
    }

    std::vector<int> selectionChildrenFound(selection.size(), 0);
    const typename SchemaAdapter::Collection& children =
        schema_node.non_decorative_children();

    for (int j = 0; j < children.size(); j++)
    {
        std::string lookupPath;

        if (children[j].child_count() == 0)
            lookupPath = children[j].to_string();
        else
            lookupPath = children[j].name();
        std::stringstream       look_up_error;
        DefaultSIRENInterpreter childSelector(look_up_error);

        if (!childSelector.parseString(lookupPath))
        {
            errors.push_back(FileScope(schema_node) + Error::SirenParseError(schema_node,
                                                    look_up_error.str()));
            return false;
        }

        for (size_t i = 0; i < selection.size(); i++)
        {
            SIRENResultSet<InputAdapter> childSelection;
            InputAdapter                 inode = selection.adapted(i);
            childSelector.evaluate(inode, childSelection);

            if (childSelection.size() != 0)
            {
                if (children[j].child_count() != 0)
                {
                    for (size_t k = 0; k < childSelection.size(); k++)
                    {
                        std::string selectionLower =
                            childSelection.adapted(k).to_string();
                        std::string childrenJLower = children[j].to_string();
                        transform(selectionLower.begin(), selectionLower.end(),
                                  selectionLower.begin(), ::tolower);
                        transform(childrenJLower.begin(), childrenJLower.end(),
                                  childrenJLower.begin(), ::tolower);
                        if (selectionLower == childrenJLower)
                        {
                            selectionChildrenFound[i]++;
                            break;
                        }
                    }
                }
                else
                    selectionChildrenFound[i]++;
            }
        }
    }

    for (size_t i = 0; i < selection.size(); i++)
    {
        if (selectionChildrenFound[i] > 1)
        {
            const typename SchemaAdapter::Collection& choices = children;
            std::string                               childNames;

            for (int js = 0; js < choices.size(); js++)
            {
                childNames += choices[js].data();

                if (js + 1 != choices.size())
                    childNames += " ";
            }

            errors.push_back(FileScope(selection.adapted(i)) + Error::ChildMostExactLeast(
                selection.adapted(i).line(), selection.adapted(i).column(),
                selection.adapted(i).name(), childNames, ruleName,
                "more than one"));
            pass = false;
        }
        else if (selectionChildrenFound[i] < 1)
        {
            const typename SchemaAdapter::Collection& choices = children;
            std::string                               childNames;

            for (int js = 0; js < choices.size(); js++)
            {
                childNames += choices[js].data();

                if (js + 1 != choices.size())
                    childNames += " ";
            }
            errors.push_back(FileScope(selection.adapted(i)) + Error::ChildMostExactLeast(
                selection.adapted(i).line(), selection.adapted(i).column(),
                selection.adapted(i).name(), childNames, ruleName, "zero"));
            pass = false;
        }
    }

    return pass;
}

template<class SchemaAdapter, class InputAdapter>
bool HIVE::validateChildAtLeastOne(SchemaAdapter&            schema_node,
                                   InputAdapter&             input_node,
                                   std::vector<std::string>& errors)
{
    std::string nodePath = schema_node.parent().path();
    std::string ruleName = getFullRuleName(schema_node.name());
    bool        pass     = true;

    SIRENResultSet<InputAdapter> selection;

    if (!select_nodes(selection, schema_node, input_node, nodePath, errors))
    {
        return false;
    }

    std::vector<int> selectedChildCounts(selection.size(), 0);
    const typename SchemaAdapter::Collection& children =
        schema_node.non_decorative_children();

    for (int j = 0; j < children.size(); j++)
    {
        std::string lookupPath;

        if (children[j].child_count() == 0)
            lookupPath = children[j].to_string();
        else
            lookupPath = children[j].name();
        std::stringstream       look_up_error;
        DefaultSIRENInterpreter childSelector(look_up_error);

        if (!childSelector.parseString(lookupPath))
        {
            errors.push_back(FileScope(children[j]) + Error::SirenParseError(children[j],
                                                    look_up_error.str()));
            return false;
        }
        for (size_t i = 0; i < selection.size(); i++)
        {
            SIRENResultSet<InputAdapter> childSelection;
            InputAdapter                 inode = selection.adapted(i);
            childSelector.evaluate(inode, childSelection);

            if (childSelection.size() != 0)
            {
                if (children[j].child_count() != 0)
                {
                    for (size_t k = 0; k < childSelection.size(); k++)
                    {
                        std::string selectionLower =
                            childSelection.adapted(k).to_string();
                        std::string childrenJLower = children[j].to_string();
                        transform(selectionLower.begin(), selectionLower.end(),
                                  selectionLower.begin(), ::tolower);
                        transform(childrenJLower.begin(), childrenJLower.end(),
                                  childrenJLower.begin(), ::tolower);
                        if (selectionLower == childrenJLower)
                        {
                            selectedChildCounts[i]++;
                            break;
                        }
                    }
                }
                else
                    selectedChildCounts[i]++;
            }
        }
    }
    auto input_grandparent = schema_node.parent().parent();
    bool is_wild_card      = nodePath.back() == '*';
    for (size_t i = 0; i < selection.size(); i++)
    {
        if (is_wild_card &&
            (selection.adapted(i).is_decorative() ||
             input_grandparent.first_child_by_name(selection.adapted(i).name())
                     .is_null() == false))
            continue;
        if (selectedChildCounts[i] < 1)
        {
            const typename SchemaAdapter::Collection& choices = children;
            std::string                               childNames;

            for (int js = 0; js < choices.size(); js++)
            {
                childNames += choices[js].data();

                if (js + 1 != choices.size())
                    childNames += " ";
            }
            errors.push_back(FileScope(selection.adapted(i)) + Error::ChildMostExactLeast(
                selection.adapted(i).line(), selection.adapted(i).column(),
                selection.adapted(i).name(), childNames, ruleName));
            pass = false;
        }
    }

    return pass;
}

template<class SchemaAdapter, class InputAdapter>
bool HIVE::validateChildCountEqual(SchemaAdapter&            schema_node,
                                   InputAdapter&             input_node,
                                   std::vector<std::string>& errors)
{
    std::string nodePath = schema_node.parent().path();
    std::string ruleName = getFullRuleName(schema_node.name());
    std::string ruleId   = schema_node.id();
    bool        pass     = true;

    if ((ruleId != "IfExists") && (ruleId != "EvenNone"))
    {
        errors.push_back(FileScope(schema_node) + 
            Error::BadOption(schema_node.name(), ruleId,
                             schema_node.non_decorative_children()[0].line(),
                             schema_node.non_decorative_children()[0].column(),
                             "IfExists EvenNone"));
        return false;
    }

    std::stringstream       look_up_error;
    DefaultSIRENInterpreter inputSelector(look_up_error);

    if (!inputSelector.parseString(nodePath))
    {
        errors.push_back(FileScope(schema_node) + Error::SirenParseError(schema_node,
                                                look_up_error.str()));
        return false;
    }
    SIRENResultSet<InputAdapter> selection;
    inputSelector.evaluate(input_node, selection);

    for (size_t i = 0; i < selection.size(); i++)
    {
        int                                       tallyChildCount = 0;
        const typename SchemaAdapter::Collection& children =
            schema_node.non_decorative_children();
        bool gotTally   = false;
        bool foundError = false;

        for (int j = 0; j < children.size(); j++)
        {
            DefaultSIRENInterpreter childSelector(look_up_error);
            std::string lookupPath = children[j].child_count() == 0 ?
                                     children[j].to_string() :
                                     children[j].name();
            if (!childSelector.parseString(lookupPath))
            {
                errors.push_back(FileScope(children[j]) + Error::SirenParseError(children[j],
                                                        look_up_error.str()));
                return false;
            }
            SIRENResultSet<InputAdapter> childSelection;
            InputAdapter                 inode = selection.adapted(i);
            childSelector.evaluate(inode, childSelection);

            int localSumCount = 0;
            if (children[j].child_count() != 0)
            {
                std::string chj = children[j].to_string();
                transform(chj.begin(), chj.end(), chj.begin(), ::tolower);

                for (size_t k = 0; k < childSelection.size(); k++)
                {
                    std::string sel = childSelection.adapted(k).to_string();
                    transform(sel.begin(), sel.end(), sel.begin(), ::tolower);

                    if (sel == chj)
                    {
                        localSumCount++;
                    }
                }
            }
            else
            {
                localSumCount = childSelection.size();
            }

            if (!foundError && gotTally && (localSumCount != tallyChildCount) &&
                ((ruleId == "EvenNone") ||
                 ((ruleId == "IfExists") && (localSumCount != 0))))
            {
                const typename SchemaAdapter::Collection& choices =
                    schema_node.non_decorative_children();
                std::string childNames;

                for (int js = 0; js < choices.size(); js++)
                {
                    childNames += choices[js].data();

                    if (js + 1 != choices.size())
                        childNames += " ";
                }
                errors.push_back(FileScope(selection.adapted(i)) + Error::ChildCountEqual(
                    selection.adapted(i).line(), selection.adapted(i).column(),
                    selection.adapted(i).name(), ruleName, ruleId, childNames));
                foundError = true;
                pass       = false;
            }
            else if ((ruleId == "EvenNone") ||
                     ((ruleId == "IfExists") && (localSumCount != 0)))
            {
                tallyChildCount = localSumCount;
                gotTally        = true;
            }
        }
    }

    return pass;
}

template<class SchemaAdapter, class InputAdapter>
bool HIVE::validateChildUniqueness(SchemaAdapter&            schema_node,
                                   InputAdapter&             input_node,
                                   std::vector<std::string>& errors)
{
    std::string nodePath   = schema_node.parent().path();
    std::string ruleName   = getFullRuleName(schema_node.name());
    std::string ruleId     = schema_node.id();
    bool        pass       = true;
    int         errorCount = 0;
    bool        absRule    = false;
    bool        zeroRule   = false;

    // check the optional option supplied to this rule
    if ((ruleId != "") && (ruleId != "Abs") && (ruleId != "Zero") &&
        (ruleId != "AbsZero"))
    {
        errors.push_back(FileScope(schema_node) + Error::BadOption(
            schema_node.name(), ruleId, schema_node.id_child().line(),
            schema_node.id_child().column(), "Abs Zero AbsZero"));
        return false;
    }

    if ((ruleId == "Abs") || (ruleId == "AbsZero"))
    {
        absRule = true;
    }

    if ((ruleId == "Zero") || (ruleId == "AbsZero"))
    {
        zeroRule = true;
    }

    // gather all of the nodes for which this rule applies
    std::stringstream       look_up_error;
    DefaultSIRENInterpreter inputSelector(look_up_error);

    if (!inputSelector.parseString(nodePath))
    {
        errors.push_back(FileScope(schema_node) + Error::SirenParseError(schema_node,
                                                look_up_error.str()));
        return false;
    }
    SIRENResultSet<InputAdapter> selection;
    inputSelector.evaluate(input_node, selection);

    // gather all of the lookup paths for this rule
    const typename SchemaAdapter::Collection& lookupPaths =
        schema_node.non_decorative_children();

    // loop over all of the nodes for which this rule applies
    for (size_t i = 0; i < selection.size(); i++)
    {
        // get a fresh std::set of data structures for each node
        std::map<std::string, InputAdapter> lookupMap;
        std::map<InputAdapter, std::string> nodesToPaths;
        std::set<std::string> collided;

        // for this node, loop over all of the lookup paths for this rule
        for (int j = 0; j < lookupPaths.size(); j++)
        {
            std::string lookupPath = lookupPaths[j].to_string();

            // gather all of the nodes that are adapted this lookup path
            // relative to this node
            DefaultSIRENInterpreter childSelector(look_up_error);

            if (!childSelector.parseString(lookupPath))
            {
                errors.push_back(FileScope(lookupPaths[j]) + Error::SirenParseError(lookupPaths[j],
                                                        look_up_error.str()));
                return false;
            }
            SIRENResultSet<InputAdapter> childSelection;
            InputAdapter                 inode = selection.adapted(i);
            childSelector.evaluate(inode, childSelection);

            // for this node, for this lookup path, loop over all of the
            // relative nodes
            for (size_t loop = 0; loop < childSelection.size(); loop++)
            {
                // get the value
                std::string initialString =
                    childSelection.adapted(loop).to_string();
                transform(initialString.begin(), initialString.end(),
                          initialString.begin(), ::tolower);

                int outterloop = 1;

                // if an alias, then loop over all of the ranges
                // otherwise, only run through this logic once
                for (int k = 0; k < outterloop; k++)
                {
                    // if this is an alias, then std::set the inner loop to be
                    // from the start of this range to the end of this range
                    // otherwise, only run through this logic once
                    std::string lookupString;
                    int         innerloopstart = 1;
                    int         innerloopend   = 1;

                    // if an alias, then loop over all values in this range
                    // otherwise, just run through the logic once
                    for (int l = innerloopstart; l <= innerloopend; l++)
                    {
                        // if an alias, then std::set the lookupstring to be
                        // this
                        // specific integer within this range of this alias
                        // otherwise, modify the std::string based on the
                        // optional
                        // flag supplied to the rule
                        //                       if (isAlias){
                        //                           lookupString =
                        // to_string(l);
                        //                       }
                        //                       else
                        {
                            lookupString = initialString;

                            if (zeroRule && (lookupString == "0"))
                            {
                                continue;
                            }

                            if (absRule && ((lookupString.at(0) == '-') ||
                                            (lookupString.at(0) == '+')))
                            {
                                lookupString.erase(lookupString.begin());
                            }
                            size_t zeroIndex =
                                lookupString.find_first_not_of('0');

                            if ((zeroIndex != 0) &&
                                (zeroIndex != std::string::npos))
                            {
                                int                itest;
                                std::istringstream iss(lookupString);
                                iss >> std::noskipws >> itest;

                                if (iss.eof() && !iss.fail())
                                {
                                    lookupString.erase(0, zeroIndex);
                                }
                            }
                        }

                        // if this value is in the std::set of non aliases we
                        // have already seen
                        // or in the std::set of aliases we have already seen,
                        // then we have a match
                        // and an input validation error
                        InputAdapter foundNode;
                        auto lookupMapIter = lookupMap.find(lookupString);

                        if (lookupMapIter != lookupMap.end())
                        {
                            if (foundNode.is_null())
                                foundNode = lookupMapIter->second;

                            // retrieve the found path for the node that was
                            // matched
                            std::string foundPath;

                            if (nodesToPaths.count(foundNode))
                            {
                                foundPath =
                                    nodesToPaths.find(foundNode)->second;
                            }

                            // if this path from this node is just the word
                            // value
                            // then std::set the name/path that will be reported
                            // to be
                            // its parent's name
                            std::string valueNodeName;

                            if ((lookupPath == "value") &&
                                childSelection.adapted(loop).has_parent())
                            {
                                valueNodeName = childSelection.adapted(loop)
                                                    .parent()
                                                    .name();
                            }
                            else
                            {
                                valueNodeName = lookupPath;
                            }

                            // if this path from the matching node is just the
                            // word value,
                            // then std::set the name/path that will be reported
                            // to be
                            // its parent's name
                            std::string valuePathName;

                            if ((foundPath == "value") &&
                                foundNode.has_parent())
                            {
                                valuePathName = foundNode.parent().name();
                            }
                            else
                            {
                                valuePathName = foundPath;
                            }

                            // format the error message and push the std::string
                            // into the std::vector
                            // of all validation error messages
                            errors.push_back(FileScope(childSelection.adapted(loop)) + Error::AlsoExistsAt(
                                childSelection.adapted(loop).line(),
                                childSelection.adapted(loop).column(),
                                valueNodeName, lookupString, valuePathName,
                                foundNode.line(), foundNode.column(),
                                ruleName));

                            // if this is the first time this lookup std::string
                            // has had a collision
                            // then we need to report the node that was matched
                            // as well
                            if (!collided.count(lookupString))
                            {
                                std::string valueNodeName;

                                if ((foundPath == "value") &&
                                    foundNode.has_parent())
                                {
                                    valueNodeName = foundNode.parent().name();
                                }
                                else
                                {
                                    valueNodeName = foundPath;
                                }
                                std::string valuePathName;

                                if ((lookupPath == "value") &&
                                    childSelection.adapted(loop).has_parent())
                                {
                                    valuePathName = childSelection.adapted(loop)
                                                        .parent()
                                                        .name();
                                }
                                else
                                {
                                    valuePathName = lookupPath;
                                }
                                errors.push_back(FileScope(foundNode) + Error::AlsoExistsAt(
                                    foundNode.line(), foundNode.column(),
                                    valueNodeName, lookupString, valuePathName,
                                    childSelection.adapted(loop).line(),
                                    childSelection.adapted(loop).column(),
                                    ruleName));

                                // make note that this std::string has already
                                // had a collision,
                                // so we don't need to do this logic for this
                                // std::string again
                                collided.insert(lookupString);
                            }

                            // if we max out our errors, then report that and
                            // bail out
                            errorCount++;

                            if (errorCount >= MAXERRORS)
                            {
                                errors.push_back(FileScope(childSelection.adapted(loop)) + Error::ErrorLimit(
                                    childSelection.adapted(loop).line(),
                                    childSelection.adapted(loop).column(),
                                    nodePath, ruleName, MAXERRORS));
                                return false;
                            }

                            pass = false;
                        }
                    }

                    // if this node is an alias, then push this range std::set
                    // into the std::set of
                    // range checks to check in the future, and also keep track
                    // of the path
                    // was was used to find this node - in case we have a
                    // collision and need
                    // to report this path
                    //                   if (isAlias){
                    //
                    //
                    //
                    //
                    //
                    //
                    //
                    //
                    // rangeSet.push_back(make_pair(&((aliasMap.find(initialString))->second[k]),
                    //
                    //
                    //
                    //
                    //
                    //
                    //
                    //
                    //
                    //                          childSelection.adapted(loop)));
                    //
                    //
                    //
                    //
                    //
                    //
                    //
                    //
                    // nodesToPaths.insert(make_pair(childSelection.adapted(loop),
                    // lookupPath));
                    //                   }
                    // if this node is not an alias, then put its std::string
                    // value in the lookup
                    // map to check in the future, and also keep track of the
                    // path that was
                    // used to find this node - in case we have a collision and
                    // need to report
                    // this path
                    //                   else
                    {
                        lookupMap.insert(make_pair(
                            lookupString, childSelection.adapted(loop)));
                        nodesToPaths.insert(make_pair(
                            childSelection.adapted(loop), lookupPath));
                    }
                }
            }
        }
    }

    // return whether this rule passed everything
    // or had adapted least a single validation failure
    return pass;
}
