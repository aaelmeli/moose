/*
 * File:   InputDefinition.cpp
 * Author: orl
 *
 */
#include "InputDefinition.h"
#include "waspcore/wasp_node.h"
#include "waspcore/wasp_bug.h"
#include <cstring>
#include <iomanip>

#define TOCTAB   4
#define MAXDEPTH 3
const int InputDefinition::POSINF = std::numeric_limits<int>::max();
const int InputDefinition::NEGINF = std::numeric_limits<int>::min();

template<class SchemaAdapter>
InputDefinition::InputDefinition(SchemaAdapter schRoot,
                                 std::ostream & outputStream,
                                 std::ostream & errorStream)
{
    ValTypeOptions  = { "Int", "Real", "String", "RealOrQuestion",
                        "IntOrYesOrNo", "IntOrAsterisk", "Fido"  };
    ExistsInOptions = { "Abs", "BeforePeriod"                    };
    UniqueOptions   = { "Abs", "Zero", "AbsZero"                 };
    IncDecOptions   = { "Strict", "Mono"                         };
    CCEOptions      = { "IfExists", "EvenNone"                   };
    DecorativeNames.insert( "decl"           );
    DecorativeNames.insert( "term"           );
    DecorativeNames.insert( "assign"         );
    DecorativeNames.insert( "comma"          );
    DecorativeNames.insert( "minus"          );
    DecorativeNames.insert( "lparen"         );
    DecorativeNames.insert( "rparen"         );
    DecorativeNames.insert( "lbrack"         );
    DecorativeNames.insert( "rbrack"         );
    DecorativeNames.insert( "scale_comment"  );
    DecorativeNames.insert( "file_injection" );
    DecorativeNames.insert( "element_count"  );
    DecorativeNames.insert( "value"          );

    setOutputStream(outputStream);
    setErrorStream(errorStream);

    IDObject * IDRoot = new IDObject();
    IDRoot->setObjectPath("/");
    IDRoot->setObjectName("root");
    IDRoot->setObjectType("root");
    IDRoot->setIDParent(nullptr);

    bool pass = true;
    pass &= fillEnumReferences(schRoot);
    pass &= fillInputDefinition(IDRoot, schRoot);

    if(pass == false)
    {
        setRootObject(IDRoot);
        setInitialized(false);
    }
    else
    {
        setRootObject(IDRoot);
        setInitialized(true);
    }
}

InputDefinition::~InputDefinition()
{
    if (getRootObject() != nullptr)
    {
        delete getRootObject();
    }
}

template<class SchemaAdapter>
bool InputDefinition::fillEnumReferences(SchemaAdapter schRoot)
{
    bool pass = true;
    for(const SchemaAdapter &child : schRoot.non_decorative_children())
    {
        // THIS IS AN ENUM
        if(child.type() != wasp::OBJECT)
        {
            pass &= addEnumRef(child);
        }
    }
    return pass;
}

template<class SchemaAdapter>
bool InputDefinition::addEnumRef(SchemaAdapter enumArray)
{
    bool pass = true;
    std::vector<std::string> enumList;
    for(const SchemaAdapter &child : enumArray.non_decorative_children())
    {
        std::string lowerString = child.to_string();
        transform(lowerString.begin(), lowerString.end(),
                  lowerString.begin(), ::tolower);
        enumList.push_back(lowerString);
    }
    enumRefs.insert(make_pair(enumArray.name(), enumList));
    return pass;
}

template<class SchemaAdapter>
bool InputDefinition::fillInputDefinition(IDObject * IDNode,
                                          SchemaAdapter schNode)
{
    if( schNode.is_null() ) return false;
    bool pass = true;

    for(SchemaAdapter child : schNode.non_decorative_children())
    {
        // THIS IS AN OBJECT (NOT A RULE / NOT AN ENUM)
        if (child.type() == wasp::OBJECT)
        {
            IDObject * tmpIDObject = new IDObject();
            tmpIDObject->setObjectName(child.name());
            tmpIDObject->setObjectPath(child.path());

            SchemaAdapter inputNameNode = child.first_non_decorative_child_by_name("InputName");
            if (!inputNameNode.is_null())
            {
                tmpIDObject->setInputName(inputNameNode.last_as_string());
            }

            SchemaAdapter inputVariantsNode = child.first_non_decorative_child_by_name("InputVariants");
            if (!inputVariantsNode.is_null())
            {
                const std::vector<SchemaAdapter>& children = inputVariantsNode.non_decorative_children();
                for(size_t i = 0; i < children.size(); i++)
                {
                    tmpIDObject->addInputVariant(children[i].last_as_string());
                }
            }

            SchemaAdapter inputAliasesNode = child.first_non_decorative_child_by_name("InputAliases");
            if (!inputAliasesNode.is_null())
            {
                const std::vector<SchemaAdapter>& children = inputAliasesNode.non_decorative_children();
                for(size_t i = 0; i < children.size(); i++)
                {
                    const auto& child = children[i];
                    if (std::strcmp(child.name(),"STRIDE") != 0)
                    {
                        tmpIDObject->addInputAlias(child.last_as_string());
                    }
                }
            }

            if (child.child_by_type(wasp::OBJECT).size() == 1 &&
                !child.first_non_decorative_child_by_name("value").is_null())
            {
                SchemaAdapter valueNode = child.first_non_decorative_child_by_name("value");
                SchemaAdapter maxOccursNode = valueNode.first_non_decorative_child_by_name("MaxOccurs");
                if (maxOccursNode.is_null())
                {
                    tmpIDObject->setObjectType("flagarray");
                }
                else
                {
                    std::string maxOccursValue = maxOccursNode.last_as_string();
                    if      (maxOccursValue == "0") tmpIDObject->setObjectType("flag");
                    else if (maxOccursValue == "1") tmpIDObject->setObjectType("flagvalue");
                    else                            tmpIDObject->setObjectType("flagarray");
                }
            }

            else if (child.child_by_type(wasp::OBJECT).size() != 0)
            {
                tmpIDObject->setObjectType("container");
            }

            else if (child.name() == std::string("value") && !child.parent().is_null() &&
                     child.parent().child_by_type(wasp::OBJECT).size() == 1)
            {
                tmpIDObject->setObjectType("value");
            }

            else
            {
                tmpIDObject->setObjectType("flagvalue");
            }

            SchemaAdapter inputTypeNode = child.first_non_decorative_child_by_name("InputType");
            if (!inputTypeNode.is_null())
            {
                tmpIDObject->setObjectType(inputTypeNode.last_as_string());
            }

            SchemaAdapter unitsNode = child.first_non_decorative_child_by_name("Units");
            if (!unitsNode.is_null())
            {
                tmpIDObject->setObjectUnits(unitsNode.last_as_string());
            }

            SchemaAdapter descriptionNode = child.first_non_decorative_child_by_name("Description");
            if (!descriptionNode.is_null())
            {
                tmpIDObject->setObjectDescription(descriptionNode.last_as_string());
            }

            SchemaAdapter inputDefaultNode = child.first_non_decorative_child_by_name("InputDefault");
            if (!inputDefaultNode.is_null())
            {
                tmpIDObject->setInputDefault(inputDefaultNode.last_as_string());
            }

            SchemaAdapter inputTmplNode = child.first_non_decorative_child_by_name("InputTmpl");
            if (!inputTmplNode.is_null())
            {
                tmpIDObject->setInputTmpl(inputTmplNode.last_as_string());
            }

            IDNode->addIDChild(tmpIDObject);
            tmpIDObject->setIDParent(IDNode);
            pass &= fillInputDefinition(tmpIDObject, child);
        }

        // THIS IS A RULE (NOT AN ENUM)
        else if (!child.parent().parent().is_null())
        {
            pass &= IDNode->addRule(child, this);
        }
    }

    return pass;
}

std::string md_escape(const std::string& src)
{
    if (src == "/") return "root_anchor";
    std::stringstream dst;
    for (char ch : src)
    {
        switch (ch)
        {
            case '/':  dst << "";  break;
            default:   dst << ch;  break;
        }
    }
    std::string return_string = dst.str();
    transform(return_string.begin(), return_string.end(),
              return_string.begin(), ::tolower);
    return return_string;
}

bool InputDefinition::getAbsolutePath(IDObject * node, std::string& relPath,
                                                 std::string& absPath) const
{

    std::string pathCopy = relPath;
    IDObject * tmpNode = node;
    int levels = std::count(pathCopy.begin(), pathCopy.end(), '/') + 1;

    for(int i = 0; i < levels; i++)
    {
        std::string subPath = pathCopy.substr(0, pathCopy.find("/"));
        pathCopy = pathCopy.substr(pathCopy.find("/") + 1);

        if (std::count(subPath.begin(), subPath.end(), '[') == 1)
        {
            if ( (std::count(subPath.begin(), subPath.end(), ']') != 1) ||
                 (subPath.find("[") >= subPath.find("]")) )
            {
                return false;
            }
            subPath = subPath.substr(0, subPath.find("["));
        }
        if (subPath == "..")
        {
            tmpNode = tmpNode->getIDParent();
            if(tmpNode == nullptr) return false;
        }
        else if (std::count(subPath.begin(), subPath.end(), '.') != 0)
        {
            return false;
        }
        else{
            IDObject * tmp = nullptr;
            std::vector<IDObject *> children = tmpNode->getIDChildren();
            size_t numChildren = children.size();
            for(size_t i = 0; i < numChildren; i++)
            {
                if (children[i]->getObjectName() == subPath)
                {
                    tmp = children[i];
                    break;
                }
            }
            if (tmp == nullptr) return false;
            tmpNode = tmp;
        }
    }

    if (tmpNode->getObjectName() == "value")
    {
        if (tmpNode->getIDParent() != nullptr &&
            tmpNode->getIDParent()->getObjectType() != "root")
        {
            IDObject * parent = tmpNode->getIDParent();
            if (!(parent->getIDChildren().size() == 2 &&
                  parent->getIDChildren()[0]->getObjectName() == "id" &&
                  parent->getIDChildren()[1]->getObjectName() == "value"))
            {
                tmpNode = parent;
                if (relPath.substr(relPath.find_last_of("/")+1) == "value")
                {
                    relPath.erase(relPath.begin() + relPath.find_last_of("/"),
                                                    relPath.end());
                }
            }
        }
    }

    absPath = tmpNode->getObjectPath();
    return true;
}

std::string InputDefinition::createBreadcrumbString(IDObject * node)
{
    if (node == nullptr) return "";
    IDObject * current = node;

    std::string breadcrumb;
    if (current->getObjectType() == "root")
    {
        breadcrumb = "[/](#root_anchor)";
    }
    else
    {
        breadcrumb = std::string("[") +
                     std::string(current->getInputName()) +
                     std::string("](#") +
                     std::string(md_escape(current->getObjectPath())) +
                     std::string(")");
    }

    while(current->getIDParent() != nullptr)
    {
        current = current->getIDParent();
        std::string piece;
        
        if (current->getObjectType() == "root")
        {
            piece = std::string("[/](#root_anchor)");
            breadcrumb = piece + breadcrumb;
        }
        else
        {
            piece = std::string("[") +
                    std::string(current->getInputName()) +
                    std::string("](#") +
                    std::string(md_escape(current->getObjectPath())) +
                    std::string(")");
            breadcrumb = piece + "/" + breadcrumb;
        }  
    }
    return breadcrumb;
}

std::string InputDefinition::howManyString(IDObject * IDNode, int minOccurs)
{
    int minOccursInt = minOccurs;
    int maxOccursInt = POSINF;
    std::string minOccursString = std::to_string(minOccurs);
    std::string maxOccursString = std::string("NoLimit");
    if (IDNode->hasMinOccursRule())
    {
        MinOccursRule * rule = IDNode->getMinOccurs();
        size_t lookupPathsCount = rule->getLookupPathCount();
        if (lookupPathsCount != 0)
        {
            std::string relPath = rule->getLookupPathAt(0);
            std::string absPath;
            if (rule->getInputDefinition()->getAbsolutePath(IDNode, relPath, absPath))
            {
                relPath.erase(relPath.begin(), relPath.begin()+relPath.find_last_of("/")+1);
                minOccursString = std::string("[") + std::string(relPath) + std::string("](#") + std::string(md_escape(absPath)) + std::string(")");
            }
            else minOccursString = std::string(relPath);
        }
        else if (rule->hasConstantValue())
        {
            minOccursInt = rule->getConstantValue();
            minOccursString = std::to_string(minOccursInt);
        }
    }
    if (IDNode->hasMaxOccursRule())
    {
        MaxOccursRule * rule = IDNode->getMaxOccurs();
        size_t lookupPathsCount = rule->getLookupPathCount();
        if (lookupPathsCount != 0)
        {
            std::string relPath = rule->getLookupPathAt(0);
            std::string absPath;
            if (rule->getInputDefinition()->getAbsolutePath(IDNode, relPath, absPath))
            {
                relPath.erase(relPath.begin(), relPath.begin()+relPath.find_last_of("/")+1);
                maxOccursString = std::string("[") + std::string(relPath) + std::string("](#") + std::string(md_escape(absPath)) + std::string(")");
            }
            else
            {
                maxOccursString = std::string(relPath);
            }
        }
        else if (rule->hasConstantValue())
        {
            maxOccursInt = rule->getConstantValue();
            if (maxOccursInt == POSINF)
            {
                maxOccursString = std::string("NoLimit");
            }
            else
            {
                maxOccursString = std::to_string(maxOccursInt);
            }
        }
    }
    std::string returnString;
    if (minOccursString == maxOccursString)  returnString = minOccursString;
    else if (minOccursInt+1 == maxOccursInt) returnString = minOccursString + std::string(" or ") + maxOccursString;
    else if (maxOccursString == "NoLimit")   returnString = minOccursString + std::string(" or more");
    else                                     returnString = minOccursString + std::string(" to ") + maxOccursString;
    return returnString;
}

void InputDefinition::printMarkdownAll()
{
    printMarkdownTableOfContents( getRootObject(), 0 );
    printMarkdownDocumentation  ( getRootObject() );
    printMarkdownReferencedLists( );
}

void InputDefinition::printMarkdownTableOfContents(IDObject * IDNode, int level)
{

    if (IDNode->getObjectType() == "root")
    {
        *xout << "<a name=\"startofinput\"></a>" << std::endl << std::endl;
        *xout << "### " << "Table Of Contents" << std::endl;
        *xout << "- [root](#root_anchor)" << std::endl;
    }
    else if ((IDNode->getObjectType() == "container" || level == 1) &&
             (level <= MAXDEPTH && IDNode->getObjectName() != "EndOfSchema"))
    {
        for(int space=0; space < TOCTAB*(level); space++) *xout << " ";
        *xout << "- [" <<
                 IDNode->getObjectName() <<
                 "](#" <<
                 md_escape(IDNode->getObjectPath()) <<
                 ")" <<
                 std::endl;
    }

    size_t numChildren = IDNode->getIDChildren().size();
    for(size_t i = 0; i < numChildren; i++)
    {
        printMarkdownTableOfContents(IDNode->getIDChildren()[i], level+1);
    }

    if (IDNode->getObjectType() == "root"){
        *xout << "- [Referenced Choice Lists](#refchoicelists)" << std::endl <<
                 std::endl << "---" << std::endl << std::endl;
    }
}

void InputDefinition::printMarkdownDocumentation(IDObject * IDNode)
{
    if (IDNode->getObjectType() == "root" || IDNode->getObjectType() == "container")
    {
        *xout << "### <a name=\"" << md_escape(IDNode->getObjectPath()) << "\"></a>" << createBreadcrumbString(IDNode) << std::endl;
        if (IDNode->hasObjectDescription())
        {
            *xout << "#### " << IDNode->getObjectDescription() << std::endl << std::endl;
        }
        if (IDNode->getObjectType() == "container")
        {
        *xout << "##### How Many: " << howManyString(IDNode,0) << std::endl << std::endl;
        }
    }
    else return;

    *xout << "Name|Type|HowMany|ValueType|Restrictions|Description" << std::endl;
    *xout << ":---:|:---:|:---:|:---:|:---:|:---:|" << std::endl;

    size_t numChildren = IDNode->getIDChildren().size();
    for(size_t i = 0; i < numChildren; i++)
    {
        IDObject * child = IDNode->getIDChildren()[i];

        if (child->getObjectName() == "EndOfSchema") continue;

        if (child->getObjectType() == "container")
        {
            *xout << "[" << child->getInputName() << "](#" << md_escape(child->getObjectPath()) << ")|";
            if (child->getIDChildren().size() == 2 &&
                child->getIDChildren()[0]->getObjectName() == "id" &&
                child->getIDChildren()[1]->getObjectName() == "value")
            {
                *xout << "TaggedValue";
            }
            else
            {
                *xout << "SubObject";
            }
            *xout << "|" << howManyString(child,0) << "|||" << child->getObjectDescription() << "|" << std::endl;
            continue;
        }
            
        *xout << child->getInputName() << "<a name=\"" << md_escape(child->getObjectPath()) << "\"></a>" << "|";

        if (child->getObjectType() == "flagarray")
        {
            IDObject * childValueNode = child->getValueNode();
            wasp_insist(childValueNode, "flagarray has null value node");
            
            *xout << "Array of Size<br>" << howManyString(childValueNode,1);

            if (childValueNode->hasIncreaseOverRule() &&
                childValueNode->getIncreaseOver()->getContext() == "..")
            {
                *xout << "<br>Increasing Values";
            }
            else if ( childValueNode->hasDecreaseOverRule() &&
                      childValueNode->getDecreaseOver()->getContext() == ".." )
            {
                *xout << "<br>Decreasing Values";
            }
        }
        else if (child->getObjectName() == "id")        *xout << "Tag";
        else if (child->getObjectName() == "value")     *xout << "Value";
        else if (child->getObjectType() == "flagvalue") *xout << "KeyedValue";
        
        *xout << "|" << howManyString(child,0) << "|";

        IDObject * childValueNode;
        if (child->getObjectType() == "flagarray") childValueNode = child->getValueNode();
        else                                       childValueNode = child;
        wasp_insist(childValueNode, "flagarray has null value node");

        std::string valueType = "String";
        std::string valueChoices;
        std::string valueRange;
        std::string valueKeys;
        std::string valueRestrictions;

        // valueType :: ValType
        if (childValueNode->hasValTypeRule())
        {
            valueType = childValueNode->getValType()->getType();
            if (valueType == "Int") valueType = "Integer";
        }
        *xout << valueType << "|";

        // valueChoices :: CHOICES :: raw text      :: ValEnums Constants
        // valueChoices :: CHOICES :: clickable REF :: ValEnums Refs
        if (childValueNode->hasValEnumsRule())
        {
            ValEnumsRule * rule = childValueNode->getValEnums();
            size_t rawEnumsCount = rule->getRawEnumsCount();
            size_t enumsRefNameCount = rule->getEnumRefNameCount();
            for(size_t i = 0; i < rawEnumsCount; i++)
            {
                valueChoices += rule->getRawEnumsAt(i) + std::string("<br>");
            }
            for(size_t i = 0; i < enumsRefNameCount; i++)
            {
                valueChoices += std::string("REF:[") + rule->getEnumRefNameAt(i) + std::string("](#ref-") + md_escape(rule->getEnumRefNameAt(i)) + std::string(")") + std::string("<br>");
                rule->getInputDefinition()->addReferenceUsage(rule->getEnumRefNameAt(i));
            }
        }

        // valueRange :: RANGE :: (/[MIN,MAX]/) :: (Min/Max)Val(Exc/Inc)
        if (valueType == "Integer" || valueType == "Real")
        {
            std::string minDelim = "\\(";
            std::string minVal = "-INF";
            std::string maxVal = "+INF";
            std::string maxDelim = "\\)";

            if (childValueNode->hasMinValIncRule())
            {
                MinValIncRule * rule = childValueNode->getMinValInc();
                minDelim = std::string("\\[");
                size_t lookupPathsCount = rule->getLookupPathCount();
                if (lookupPathsCount != 0)
                {
                    std::string relPath = rule->getLookupPathAt(0);
                    std::string absPath;
                    if (rule->getInputDefinition()->getAbsolutePath(childValueNode, relPath, absPath))
                    {
                        relPath.erase(relPath.begin(), relPath.begin()+relPath.find_last_of("/")+1);
                        minVal = std::string("[") + relPath + std::string("](#") + md_escape(absPath) + std::string(")");
                    }
                    else
                    {
                        minVal =  relPath;
                    }
                }
                else if (rule->hasConstantValue())
                {
                    if (rule->getConstantValue() == NEGINF)
                    {
                        minVal = std::string("-INF");
                        minDelim = std::string("\\(");
                    }
                    else{
                        if (valueType == "Real")
                        {
                            std::stringstream stream;
                            stream << std::fixed << std::setprecision(1) << rule->getConstantValue();
                            minVal = stream.str();
                        }
                        else
                        {
                            minVal = std::to_string((int)(rule->getConstantValue()));
                        }
                    }
                }
            }
            else if (childValueNode->hasMinValExcRule())
            {
                MinValExcRule * rule = childValueNode->getMinValExc();
                minDelim = std::string("\\(");
                size_t lookupPathsCount = rule->getLookupPathCount();
                if (lookupPathsCount != 0)
                {
                    std::string relPath = rule->getLookupPathAt(0);
                    std::string absPath;
                    if (rule->getInputDefinition()->getAbsolutePath(childValueNode, relPath, absPath))
                    {
                        relPath.erase(relPath.begin(), relPath.begin()+relPath.find_last_of("/")+1);
                        minVal = std::string("[") + relPath + std::string("](#") + md_escape(absPath) + std::string(")");
                    }
                    else
                    {
                        minVal = relPath;
                    }
                }
                else if (rule->hasConstantValue()){
                    if (rule->getConstantValue() == NEGINF)
                    {
                        minVal = std::string("-INF");
                    }
                    else{
                        if (valueType == "Real")
                        {
                            std::stringstream stream;
                            stream << std::fixed << std::setprecision(1) << rule->getConstantValue();
                            minVal = stream.str();
                        }
                        else
                        {
                            minVal = std::to_string((int)(rule->getConstantValue()));
                        }
                    }
                }
            }
            if (childValueNode->hasMaxValIncRule())
            {
                MaxValIncRule * rule = childValueNode->getMaxValInc();
                maxDelim = std::string("\\]");
                size_t lookupPathsCount = rule->getLookupPathCount();
                if (lookupPathsCount != 0)
                {
                    std::string relPath = rule->getLookupPathAt(0);
                    std::string absPath;
                    if (rule->getInputDefinition()->getAbsolutePath(childValueNode, relPath, absPath))
                    {
                        relPath.erase(relPath.begin(), relPath.begin()+relPath.find_last_of("/")+1);
                        maxVal = std::string("[") + relPath + std::string("](#") + md_escape(absPath) + std::string(")");
                    }
                    else
                    {
                        maxVal = relPath;
                    }
                }
                else if (rule->hasConstantValue())
                {
                    if (rule->getConstantValue() == POSINF)
                    {
                        maxVal = std::string("+INF");
                        maxDelim = std::string("\\)");
                    }
                    else
                    {
                        if (valueType == "Real")
                        {
                            std::stringstream stream;
                            stream << std::fixed << std::setprecision(1) << rule->getConstantValue();
                            maxVal = stream.str();
                        }
                        else
                        {
                            maxVal = std::to_string((int)(rule->getConstantValue()));
                        }
                    }
                }
            }
            else if (childValueNode->hasMaxValExcRule())
            {
                MaxValExcRule * rule = childValueNode->getMaxValExc();
                maxDelim = std::string("\\)");
                size_t lookupPathsCount = rule->getLookupPathCount();
                if (lookupPathsCount != 0)
                {
                    std::string relPath = rule->getLookupPathAt(0);
                    std::string absPath;
                    if (rule->getInputDefinition()->getAbsolutePath(childValueNode, relPath, absPath))
                    {
                        relPath.erase(relPath.begin(), relPath.begin()+relPath.find_last_of("/")+1);
                        maxVal = std::string("[") + relPath + std::string("](#") + md_escape(absPath) + std::string(")");
                    }
                    else
                    {
                        maxVal = relPath;
                    }
                }
                else if (rule->hasConstantValue())
                {
                    if (rule->getConstantValue() == POSINF)
                    {
                        maxVal = std::string("+INF");
                    }
                    else
                    {
                        if (valueType == "Real")
                        {
                            std::stringstream stream;
                            stream << std::fixed << std::setprecision(1) << rule->getConstantValue();
                            maxVal = stream.str();
                        }
                        else
                        {
                            maxVal = std::to_string((int)(rule->getConstantValue()));
                        }
                    }
                }
            }
            valueRange = minDelim + minVal + std::string(",") + maxVal + maxDelim;
        }

        // valueKeys    :: LOOKUPKEYS :: clickable context name  :: ExistsIn Paths
        // valueChoices :: CHOICES    :: raw text                :: ExistsIn Extras
        // valueChoices :: CHOICES    :: clickable REF           :: ExistsIn ExtraRefs
        if (childValueNode->hasExistsInRule())
        {
            ExistsInRule * rule = childValueNode->getExistsIn();
            size_t lookupPathsCount = rule->getLookupPathCount();
            if (lookupPathsCount != 0)
            {
                for(size_t i = 0; i < lookupPathsCount; i++)
                {
                    std::string relPath = rule->getLookupPathAt(i);
                    std::string absPath;
                    if (rule->getInputDefinition()->getAbsolutePath(childValueNode, relPath, absPath))
                    {
                        std::string holdingName = absPath;
                        if (std::count(holdingName.begin(), holdingName.end(), '/') > 1)
                        {
                            std::reverse(holdingName.begin(), holdingName.end());
                            holdingName.erase(holdingName.begin(), holdingName.begin()+holdingName.find_first_of("/")+1);
                            holdingName.erase(holdingName.begin()+holdingName.find_first_of("/"), holdingName.end());
                            std::reverse(holdingName.begin(), holdingName.end());
                        }
                        else holdingName = "root";
                        relPath.erase(relPath.begin(), relPath.begin()+relPath.find_last_of("/")+1);
                        valueKeys += std::string("[") + holdingName + " " + relPath + std::string("](#") + md_escape(absPath) + std::string(")") + std::string("<br>");
                    }
                    else
                    {
                        valueKeys += relPath + std::string("<br>");
                    }
                }
            }
            size_t constantsCount = rule->getRawConstantsCount();
            for(size_t i = 0; i < constantsCount; i++)
            {
                valueChoices += rule->getRawConstantsAt(i) + std::string("<br>");
            }
            size_t enumsRefNameCount = rule->getEnumRefNameCount();
            for(size_t i = 0; i < enumsRefNameCount; i++)
            {
                valueChoices += std::string("REF:[") + rule->getEnumRefNameAt(i) + std::string("](#ref-") + md_escape(rule->getEnumRefNameAt(i)) + std::string(")") + std::string("<br>");
                rule->getInputDefinition()->addReferenceUsage(rule->getEnumRefNameAt(i));
            }
        }

        if (!valueRange.empty())
        {
            valueRestrictions += std::string("__Range__<br>") + valueRange;
            if (!valueChoices.empty() || !valueKeys.empty())
            {
                valueRestrictions += std::string("<br><br>");
            }
        }
        if (!valueChoices.empty())
        {
            valueRestrictions += std::string("__Choices__<br>") + valueChoices;
            if (!valueKeys.empty())
            {
                valueRestrictions += std::string("<br>");
            }
        }
        if (!valueKeys.empty())
        {
            valueRestrictions += std::string("__InputKeys__<br>") + valueKeys;
        }
        *xout << valueRestrictions << "|" << child->getObjectDescription() << "|" << std::endl;
    }

    *xout << std::endl << "---" << std::endl << std::endl;

    for(size_t i = 0; i < numChildren; i++)
    {
        printMarkdownDocumentation(IDNode->getIDChildren()[i]);
    }
}

void InputDefinition::printMarkdownReferencedLists()
{
    std::map<std::string, std::vector<std::string>>* enumRefPtr = getEnumRefs();
    bool headerPrinted = false;
    for (const auto & map_it : *enumRefPtr )
    {
        if (referencesUsed.find(map_it.first) != referencesUsed.end())
        {
            if (!headerPrinted)
            {
                *xout << "[Start of Input](#startofinput)" << std::endl << std::endl;
                *xout << "## Referenced Choice Lists";
                *xout << "<a name=\"refchoicelists\"></a>" << std::endl;
                headerPrinted = true;
            }

            *xout << std::endl << "### <a name=\"ref-" << md_escape(map_it.first) <<
                                "\"></a>" << map_it.first << std::endl;

            for (const auto & vec_it : map_it.second) *xout << vec_it << " ";

            *xout << std::endl << std::endl << "[Start of Input](#startofinput)";
            *xout << std::endl << std::endl << "---" << std::endl << std::endl;
        }
    }
}

/********** IDObject **********/

IDObject::IDObject(){
    this->minOccursRuleInstance       = NULL;
    this->maxOccursRuleInstance       = NULL;
    this->minValIncRuleInstance       = NULL;
    this->minValExcRuleInstance       = NULL;
    this->maxValIncRuleInstance       = NULL;
    this->maxValExcRuleInstance       = NULL;
    this->valTypeRuleInstance         = NULL;
    this->valEnumsRuleInstance        = NULL;
    this->inputChoicesRuleInstance    = NULL;
    this->existsInRuleInstance        = NULL;
    this->notExistsInRuleInstance     = NULL;
    this->sumOverRuleInstance         = NULL;
    this->sumOverGroupRuleInstance    = NULL;
    this->increaseOverRuleInstance    = NULL;
    this->decreaseOverRuleInstance    = NULL;
}

IDObject::~IDObject(){

    if (this->hasMinOccursRule())       delete this->getMinOccurs();
    if (this->hasMaxOccursRule())       delete this->getMaxOccurs();
    if (this->hasMinValIncRule())       delete this->getMinValInc();
    if (this->hasMinValExcRule())       delete this->getMinValExc();
    if (this->hasMaxValIncRule())       delete this->getMaxValInc();
    if (this->hasMaxValExcRule())       delete this->getMaxValExc();
    if (this->hasValTypeRule())         delete this->getValType();
    if (this->hasValEnumsRule())        delete this->getValEnums();
    if (this->hasInputChoicesRule())    delete this->getInputChoices();
    if (this->hasExistsInRule())        delete this->getExistsIn();
    if (this->hasNotExistsInRule())     delete this->getNotExistsIn();
    if (this->hasSumOverRule())         delete this->getSumOver();
    if (this->hasSumOverGroupRule())    delete this->getSumOverGroup();    
    if (this->hasIncreaseOverRule())    delete this->getIncreaseOver();
    if (this->hasDecreaseOverRule())    delete this->getDecreaseOver();

    if (this->getChildAtMostOneCount() > 0){
        size_t count = this->getChildAtMostOneCount();
        for(size_t i = 0; i < count; i++){
            delete this->getChildAtMostOneAt(i);
        }
    }
    if (this->getChildExactlyOneCount() > 0){
        size_t count = this->getChildExactlyOneCount();
        for(size_t i = 0; i < count; i++){
            delete this->getChildExactlyOneAt(i);
        }
    }
    if (this->getChildAtLeastOneCount() > 0){
        size_t count = this->getChildAtLeastOneCount();
        for(size_t i = 0; i < count; i++){
            delete this->getChildAtLeastOneAt(i);
        }
    }
    if (this->getChildCountEqualCount() > 0){
        size_t count = this->getChildCountEqualCount();
        for(size_t i = 0; i < count; i++){
            delete this->getChildCountEqualAt(i);
        }
    }
    if (this->getChildUniquenessCount() > 0){
        size_t count = this->getChildUniquenessCount();
        for(size_t i = 0; i < count; i++){
            delete this->getChildUniquenessAt(i);
        }
    }

    std::vector<IDObject *> children = this->getIDChildren();
    for(size_t i = 0; i < children.size(); i++){
        delete children[i];
    }

}

template<class SchemaAdapter>
bool IDObject::addRule(SchemaAdapter node, InputDefinition * thisID){

    bool pass = true;

    if      (node.name() == std::string("MinOccurs"))       pass &= this->addMinOccursRule       (node, thisID);
    else if (node.name() == std::string("MaxOccurs"))       pass &= this->addMaxOccursRule       (node, thisID);
    else if (node.name() == std::string("MinValInc"))       pass &= this->addMinValIncRule       (node, thisID);
    else if (node.name() == std::string("MinValExc"))       pass &= this->addMinValExcRule       (node, thisID);
    else if (node.name() == std::string("MaxValInc"))       pass &= this->addMaxValIncRule       (node, thisID);
    else if (node.name() == std::string("MaxValExc"))       pass &= this->addMaxValExcRule       (node, thisID);
    else if (node.name() == std::string("ValType"))         pass &= this->addValTypeRule         (node, thisID);
    else if (node.name() == std::string("ValEnums"))        pass &= this->addValEnumsRule        (node, thisID);
    else if (node.name() == std::string("InputChoices"))    pass &= this->addInputChoicesRule    (node, thisID);
    else if (node.name() == std::string("ExistsIn"))        pass &= this->addExistsInRule        (node, thisID);
    else if (node.name() == std::string("NotExistsIn"))     pass &= this->addNotExistsInRule     (node, thisID);
    else if (node.name() == std::string("SumOver"))         pass &= this->addSumOverRule         (node, thisID);
    else if (node.name() == std::string("SumOverGroup"))    pass &= this->addSumOverGroupRule    (node, thisID);    
    else if (node.name() == std::string("IncreaseOver"))    pass &= this->addIncreaseOverRule    (node, thisID);
    else if (node.name() == std::string("DecreaseOver"))    pass &= this->addDecreaseOverRule    (node, thisID);
    else if (node.name() == std::string("ChildAtMostOne"))  pass &= this->addChildAtMostOneRule  (node, thisID);
    else if (node.name() == std::string("ChildExactlyOne")) pass &= this->addChildExactlyOneRule (node, thisID);
    else if (node.name() == std::string("ChildAtLeastOne")) pass &= this->addChildAtLeastOneRule (node, thisID);
    else if (node.name() == std::string("ChildCountEqual")) pass &= this->addChildCountEqualRule (node, thisID);
    else if (node.name() == std::string("ChildUniqueness")) pass &= this->addChildUniquenessRule (node, thisID);

    else if (node.name() == std::string("ToDo")              ||
             node.name() == std::string("Units")             ||
             node.name() == std::string("Description")       ||
             node.name() == std::string("InputName")         ||
             node.name() == std::string("InputTerm")         ||
             node.name() == std::string("InputVariants")     ||
             node.name() == std::string("InputAliases")      ||
             node.name() == std::string("InputDefault")      ||
             node.name() == std::string("InputTmpl")         ||
             node.name() == std::string("InputType")) pass &= true;

    else{
        std::string errorMessage = " *** unsupported rule: " + std::string(node.name());
        thisID->streamErrorMessage(node.parent(), errorMessage);
        pass &= false;
    }

    return pass;

}

template<class SchemaAdapter>
bool IDObject::addMinOccursRule(SchemaAdapter node, InputDefinition * thisID){

    bool pass = true;
    bool ruleAlreadyInTree = false;
    MinOccursRule * rule;
    if (this->hasMinOccursRule()){
        rule = this->getMinOccurs();
        ruleAlreadyInTree = true;
    }
    else rule = new MinOccursRule(thisID);
    std::string ruleValue = node.to_string();

    if (ruleValue == "NoLimit"){
        if (rule->hasConstantValue()){
            std::string errorMessage = "multiple constant values defined";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }
        else{
            rule->setConstantValueExists(true);
            rule->setConstantValue(InputDefinition::NEGINF);
        }
    }
    else if (rule->getInputDefinition()->isInt(ruleValue)){
        if (rule->hasConstantValue()){
            std::string errorMessage = "multiple constant values defined";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }
        else if(stoi(ruleValue) < 0){
            std::string errorMessage = "negative value " + ruleValue + " not allowed";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }
        else{
            rule->setConstantValueExists(true);
            rule->setConstantValue(stoi(ruleValue));
        }
    }
    else if (rule->getInputDefinition()->isValidPath(ruleValue, node.parent())){
        rule->addLookupPath(ruleValue);
    }
    else{
        std::string errorMessage = "path \"" + ruleValue +
                                   "\" checked against \"" +
                                   node.parent().path() +
                                   "\" is not valid";
        rule->streamErrorMessage(node, errorMessage);
        pass &= false;
    }

    if (pass == true  && !ruleAlreadyInTree) this->minOccursRuleInstance = rule;
    if (pass == false && !ruleAlreadyInTree) delete rule;
    return pass;

}

template<class SchemaAdapter>
bool IDObject::addMaxOccursRule(SchemaAdapter node, InputDefinition * thisID){

    bool pass = true;
    bool ruleAlreadyInTree = false;
    MaxOccursRule * rule;
    if (this->hasMaxOccursRule()){
        rule = this->getMaxOccurs();
        ruleAlreadyInTree = true;
    }
    else rule = new MaxOccursRule(thisID);
    std::string ruleValue = node.to_string();

    if (ruleValue == "NoLimit"){
        if (rule->hasConstantValue()){
            std::string errorMessage = "multiple constant values defined";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }
        else{
            rule->setConstantValueExists(true);
            rule->setConstantValue(InputDefinition::POSINF);
        }
    }
    else if (rule->getInputDefinition()->isInt(ruleValue)){
        if (rule->hasConstantValue()){
            std::string errorMessage = "multiple constant values defined";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }
        else if(stoi(ruleValue) < 0){
            std::string errorMessage = "negative value " + ruleValue + " not allowed";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }
        else{
            rule->setConstantValueExists(true);
            rule->setConstantValue(stoi(ruleValue));
        }
    }
    else if (rule->getInputDefinition()->isValidPath(ruleValue, node.parent())){
        rule->addLookupPath(ruleValue);
    }
    else{
        std::string errorMessage = "path \"" + ruleValue +
                                   "\" checked against \"" +
                                   node.parent().path() +
                                   "\" is not valid";
        rule->streamErrorMessage(node, errorMessage);
        pass &= false;
    }

    if (pass == true  && !ruleAlreadyInTree) this->maxOccursRuleInstance = rule;
    if (pass == false && !ruleAlreadyInTree) delete rule;
    return pass;

}

template<class SchemaAdapter>
bool IDObject::addMinValIncRule(SchemaAdapter node, InputDefinition * thisID){

    bool pass = true;
    bool ruleAlreadyInTree = false;
    MinValIncRule * rule;
    if (this->hasMinValIncRule()){
        rule = this->getMinValInc();
        ruleAlreadyInTree = true;
    }
    else rule = new MinValIncRule(thisID);
    std::string ruleValue = node.to_string();

    if (ruleValue == "NoLimit"){
        if (rule->hasConstantValue()){
            std::string errorMessage = "multiple constant values defined";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }
        else{
            rule->setConstantValueExists(true);
            rule->setConstantValue(InputDefinition::NEGINF);
        }
    }
    else if (rule->getInputDefinition()->isReal(ruleValue)){
        if (rule->hasConstantValue()){
            std::string errorMessage = "multiple constant values defined";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }
        else{
            rule->setConstantValueExists(true);
            rule->setConstantValue(stod(ruleValue));
        }
    }
    else if (rule->getInputDefinition()->isValidPath(ruleValue, node.parent())){
        rule->addLookupPath(ruleValue);
    }
    else{
        std::string errorMessage = "path \"" + ruleValue +
                                   "\" checked against \"" +
                                   node.parent().path() +
                                   "\" is not valid";
        rule->streamErrorMessage(node, errorMessage);
        pass &= false;
    }

    if (pass == true  && !ruleAlreadyInTree) this->minValIncRuleInstance = rule;
    if (pass == false && !ruleAlreadyInTree) delete rule;
    return pass;

}

template<class SchemaAdapter>
bool IDObject::addMinValExcRule(SchemaAdapter node, InputDefinition * thisID){

    bool pass = true;
    bool ruleAlreadyInTree = false;
    MinValExcRule * rule;
    if (this->hasMinValExcRule()){
        rule = this->getMinValExc();
        ruleAlreadyInTree = true;
    }
    else rule = new MinValExcRule(thisID);
    std::string ruleValue = node.to_string();

    if (ruleValue == "NoLimit"){
        if (rule->hasConstantValue()){
            std::string errorMessage = "multiple constant values defined";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }
        else{
            rule->setConstantValueExists(true);
            rule->setConstantValue(InputDefinition::NEGINF);
        }
    }
    else if (rule->getInputDefinition()->isReal(ruleValue)){
        if (rule->hasConstantValue()){
            std::string errorMessage = "multiple constant values defined";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }
        else{
            rule->setConstantValueExists(true);
            rule->setConstantValue(stod(ruleValue));
        }
    }
    else if (rule->getInputDefinition()->isValidPath(ruleValue, node.parent())){
        rule->addLookupPath(ruleValue);
    }
    else{
        std::string errorMessage = "path \"" + ruleValue +
                                   "\" checked against \"" +
                                   node.parent().path() +
                                   "\" is not valid";
        rule->streamErrorMessage(node, errorMessage);
        pass &= false;
    }

    if (pass == true  && !ruleAlreadyInTree) this->minValExcRuleInstance = rule;
    if (pass == false && !ruleAlreadyInTree) delete rule;
    return pass;

}

template<class SchemaAdapter>
bool IDObject::addMaxValIncRule(SchemaAdapter node, InputDefinition * thisID){

    bool pass = true;
    bool ruleAlreadyInTree = false;
    MaxValIncRule * rule;
    if (this->hasMaxValIncRule()){
        rule = this->getMaxValInc();
        ruleAlreadyInTree = true;
    }
    else rule = new MaxValIncRule(thisID);
    std::string ruleValue = node.to_string();

    if (ruleValue == "NoLimit"){
        if (rule->hasConstantValue()){
            std::string errorMessage = "multiple constant values defined";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }
        else{
            rule->setConstantValueExists(true);
            rule->setConstantValue(InputDefinition::POSINF);
        }
    }
    else if (rule->getInputDefinition()->isReal(ruleValue)){
        if (rule->hasConstantValue()){
            std::string errorMessage = "multiple constant values defined";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }
        else{
            rule->setConstantValueExists(true);
            rule->setConstantValue(stod(ruleValue));
        }
    }
    else if (rule->getInputDefinition()->isValidPath(ruleValue, node.parent())){
        rule->addLookupPath(ruleValue);
    }
    else{
        std::string errorMessage = "path \"" + ruleValue +
                                   "\" checked against \"" +
                                   node.parent().path() +
                                   "\" is not valid";
        rule->streamErrorMessage(node, errorMessage);
        pass &= false;
    }

    if (pass == true  && !ruleAlreadyInTree) this->maxValIncRuleInstance = rule;
    if (pass == false && !ruleAlreadyInTree) delete rule;
    return pass;

}

template<class SchemaAdapter>
bool IDObject::addMaxValExcRule(SchemaAdapter node, InputDefinition * thisID){

    bool pass = true;
    bool ruleAlreadyInTree = false;
    MaxValExcRule * rule;
    if (this->hasMaxValExcRule()){
        rule = this->getMaxValExc();
        ruleAlreadyInTree = true;
    }
    else rule = new MaxValExcRule(thisID);
    std::string ruleValue = node.to_string();

    if (ruleValue == "NoLimit"){
        if (rule->hasConstantValue()){
            std::string errorMessage = "multiple constant values defined";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }
        else{
            rule->setConstantValueExists(true);
            rule->setConstantValue(InputDefinition::POSINF);
        }
    }
    else if (rule->getInputDefinition()->isReal(ruleValue)){
        if (rule->hasConstantValue()){
            std::string errorMessage = "multiple constant values defined";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }
        else{
            rule->setConstantValueExists(true);
            rule->setConstantValue(stod(ruleValue));
        }
    }
    else if (rule->getInputDefinition()->isValidPath(ruleValue, node.parent())){
        rule->addLookupPath(ruleValue);
    }
    else{
        std::string errorMessage = "path \"" + ruleValue +
                                   "\" checked against \"" +
                                   node.parent().path() +
                                   "\" is not valid";
        rule->streamErrorMessage(node, errorMessage);
        pass &= false;
    }

    if (pass == true  && !ruleAlreadyInTree) this->maxValExcRuleInstance = rule;
    if (pass == false && !ruleAlreadyInTree) delete rule;
    return pass;

}

template<class SchemaAdapter>
bool IDObject::addValTypeRule(SchemaAdapter node, InputDefinition * thisID){

    bool pass = true;
    ValTypeRule * rule;
    if (this->hasValTypeRule()){
        std::string errorMessage = "multiple " + std::string(node.name()) + " rules defined";
        thisID->streamErrorMessage(node.parent(), errorMessage);
        pass &= false;
    }
    rule = new ValTypeRule(thisID);

    std::string ruleValue = node.to_string();
    if (rule->isValidOption(ruleValue)){
        rule->setType(ruleValue);
    }
    else{
        const std::vector<std::string> & validOptions = rule->getValidOptions();
        std::string errorMessage = "\"" + ruleValue + "\" is not a valid option " +
                                   " - valid choices are [ ";

        for (auto it = validOptions.begin(); it != validOptions.end(); it++){
            errorMessage += "\"" + (*it) + "\" ";
        }
        errorMessage += "]";
        rule->streamErrorMessage(node, errorMessage);
        pass &= false;
    }

    if (pass == true) this->valTypeRuleInstance = rule;
    else delete rule;
    return pass;

}

template<class SchemaAdapter>
bool IDObject::addValEnumsRule(SchemaAdapter node, InputDefinition * thisID){

    bool pass = true;
    ValEnumsRule * rule;
    if (this->hasValEnumsRule()){
        std::string errorMessage = "multiple " + std::string(node.name()) + " rules defined";
        thisID->streamErrorMessage(node.parent(), errorMessage);
        pass &= false;
    }
    rule = new ValEnumsRule(thisID);

    const auto & children = node.non_decorative_children();
    for(size_t i = 0; i < children.size(); i++){

        if (children[i].name() == std::string("REF")){
            std::string enumRefName = children[i].to_string();
            if (rule->isEnumRefNameDefined(enumRefName)){
                rule->addEnumRefName(enumRefName);
            }
            else{
                std::string errorMessage = enumRefName +
                                           " is not defined in enum references";
                rule->streamErrorMessage(node, errorMessage);
                pass &= false;
            }
        }
        else{
            std::string lowerString = children[i].to_string();
            transform(lowerString.begin(), lowerString.end(),
                                                lowerString.begin(), ::tolower);
            rule->addEnumValue(lowerString);
        }

    }

    if (pass == true) this->valEnumsRuleInstance = rule;
    else delete rule;
    return pass;

}

template<class SchemaAdapter>
bool IDObject::addInputChoicesRule(SchemaAdapter node, InputDefinition * thisID){

    bool pass = true;
    InputChoicesRule * rule;
    if (this->hasInputChoicesRule()){
        std::string errorMessage = "multiple " + std::string(node.name()) + " rules defined";
        thisID->streamErrorMessage(node.parent(), errorMessage);
        pass &= false;
    }
    rule = new InputChoicesRule(thisID);

    const auto & children = node.non_decorative_children();
    for(size_t i = 0; i < children.size(); i++){

        if (children[i].name() == std::string("REF")){
            std::string enumRefName = children[i].to_string();
            if (rule->isEnumRefNameDefined(enumRefName)){
                rule->addEnumRefName(enumRefName);
            }
            else{
                std::string errorMessage = enumRefName +
                                           " is not defined in enum references";
                rule->streamErrorMessage(node, errorMessage);
                pass &= false;
            }
        }
        else{
            std::string lowerString = children[i].to_string();
            transform(lowerString.begin(), lowerString.end(),
                                                lowerString.begin(), ::tolower);
            rule->addEnumValue(lowerString);
        }

    }

    if (pass == true) this->inputChoicesRuleInstance = rule;
    else delete rule;
    return pass;

}

template<class SchemaAdapter>
bool IDObject::addExistsInRule(SchemaAdapter node, InputDefinition * thisID){

    bool pass = true;
    size_t validPathsCount = 0;
    ExistsInRule * rule;
    if (this->hasExistsInRule()){
        std::string errorMessage = "multiple " + std::string(node.name()) + " rules defined";
        thisID->streamErrorMessage(node.parent(), errorMessage);
        pass &= false;
    }
    rule = new ExistsInRule(thisID);

    std::string modifierFlag = node.id();
    if (modifierFlag != ""){
        if (rule->isValidOption(modifierFlag)){
            rule->setModifierFlag(modifierFlag);
        }
        else{
            const std::vector<std::string> & validOptions = rule->getValidOptions();
            std::string errorMessage = "\"" + modifierFlag + "\" is not a valid option " +
                                       " - valid choices are [ ";

            for (auto it = validOptions.begin(); it != validOptions.end(); it++){
                errorMessage += "\"" + (*it) + "\" ";
            }
            errorMessage += "]";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }
    }

    const auto & children = node.non_decorative_children();
    for(size_t i = 0; i < children.size(); i++){

        if (children[i].name() == std::string("EXTRAREF")){
            std::string enumRefName = children[i].to_string();
            if (rule->isEnumRefNameDefined(enumRefName)){
                rule->addEnumRefName(enumRefName);
            }
            else{
                std::string errorMessage = enumRefName +
                                           " is not defined in enum references";
                rule->streamErrorMessage(node, errorMessage);
                pass &= false;
            }
        }

        else if (children[i].name() == std::string("EXTRA")){
            std::string lowerString = children[i].to_string();
            std::transform(lowerString.begin(), lowerString.end(), lowerString.begin(), ::tolower);
            rule->addConstant(lowerString);
        }

        else if (children[i].name() == std::string("RANGE")){
            const auto & rangeChildren = children[i].non_decorative_children();
            if (rangeChildren.size() != 2){
                std::string errorMessage = "range does not have exactly two values";
                rule->streamErrorMessage(node, errorMessage);
                pass &= false;
            }
            else{
                int itestStart;
                int itestEnd;
                std::istringstream rangeStreamStart(rangeChildren[0].to_string());
                std::istringstream rangeStreamEnd  (rangeChildren[1].to_string());
                rangeStreamStart >> std::noskipws >> itestStart;
                rangeStreamEnd   >> std::noskipws >> itestEnd;
                if    (!rangeStreamStart.eof() || rangeStreamStart.fail()){
                    std::string errorMessage = "\"" + rangeChildren[0].to_string() +
                                               "\" start of range not a valid number";
                    rule->streamErrorMessage(node, errorMessage);
                    pass &= false;
                }
                else if (!rangeStreamEnd.eof() || rangeStreamEnd.fail()){
                    std::string errorMessage = "\"" + rangeChildren[1].to_string() +
                                               "\" end of range not a valid number";
                    rule->streamErrorMessage(node, errorMessage);
                    pass &= false;
                }
                else{
                    int rangeStart = stoi(rangeChildren[0].to_string());
                    int rangeEnd = stoi(rangeChildren[1].to_string());
                    if (rangeStart >= rangeEnd){
                        std::string errorMessage = "\"" + rangeChildren[0].to_string() +
                                                   "\" start of range is greater than or equal to \"" +
                                                   rangeChildren[1].to_string() +
                                                   "\" end of range";
                        rule->streamErrorMessage(node, errorMessage);
                        pass &= false;
                    }
                    else{
                        for(int rangeLoop = rangeStart; rangeLoop <= rangeEnd; rangeLoop++){
                            rule->addConstant(std::to_string(rangeLoop));
                        }
                    }
                }
            }
            continue;
        }

        else{
            std::string path = children[i].to_string();
            if (rule->getInputDefinition()->isValidPath(path, node.parent())){
                rule->addLookupPath(path);
                validPathsCount++;
            }
        }

    }

    if (pass == true) this->existsInRuleInstance = rule;
    else delete rule;
    return pass;

}

template<class SchemaAdapter>
bool IDObject::addNotExistsInRule(SchemaAdapter node, InputDefinition * thisID){

    bool pass = true;
    size_t validPathsCount = 0;
    NotExistsInRule * rule;
    if (this->hasNotExistsInRule()){
        std::string errorMessage = "multiple " + std::string(node.name()) + " rules defined";
        thisID->streamErrorMessage(node.parent(), errorMessage);
        pass &= false;
    }
    rule = new NotExistsInRule(thisID);

    std::string modifierFlag = node.id();
    if (modifierFlag != ""){
        if (rule->isValidOption(modifierFlag)){
            rule->setModifierFlag(modifierFlag);
        }
        else{
            const std::vector<std::string> & validOptions = rule->getValidOptions();
            std::string errorMessage = "\"" + modifierFlag + "\" is not a valid option " +
                                       " - valid choices are [ ";

            for (auto it = validOptions.begin(); it != validOptions.end(); it++){
                errorMessage += "\"" + (*it) + "\" ";
            }
            errorMessage += "]";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }
    }

    const auto & children = node.non_decorative_children();
    for(size_t i = 0; i < children.size(); i++){

            std::string path = children[i].to_string();
            if (rule->getInputDefinition()->isValidPath(path, node.parent())){
                rule->addLookupPath(path);
                validPathsCount++;
            }

    }

    if (validPathsCount == 0){
        std::string errorMessage = "no paths valid when checked against " +
                                   node.parent().path();
        rule->streamErrorMessage(node, errorMessage);
        pass &= false;
    }

    if (pass == true) this->notExistsInRuleInstance = rule;
    else delete rule;
    return pass;

}

template<class SchemaAdapter>
bool IDObject::addSumOverRule(SchemaAdapter node, InputDefinition * thisID){

    bool pass = true;
    SumOverRule * rule;
    if (this->hasSumOverRule()){
        std::string errorMessage = "multiple " + std::string(node.name()) + " rules defined";
        thisID->streamErrorMessage(node.parent(), errorMessage);
        pass &= false;
    }
    rule = new SumOverRule(thisID);

    std::string path = node.id();
    if (path == ""){
        std::string errorMessage = "context path missing";
        rule->streamErrorMessage(node, errorMessage);
        pass &= false;
    }
    else{
        if (rule->getInputDefinition()->isValidPath(path, node.parent())){
            rule->setContext(path);
        }
        else{
            std::string errorMessage = "path \"" + path +
                                       "\" checked against \"" +
                                       node.parent().path() +
                                       "\" is not valid";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }
    }

    std::string ruleValue = node.to_string();
    if (rule->getInputDefinition()->isReal(ruleValue)){
        rule->setSumOverValue(stod(ruleValue));
    }
    else{
        std::string errorMessage = "value \"" + ruleValue +
                                   "\" is not a number";
        rule->streamErrorMessage(node, errorMessage);
        pass &= false;
    }

    if (pass == true) this->sumOverRuleInstance = rule;
    else delete rule;
    return pass;

}

template<class SchemaAdapter>
bool IDObject::addSumOverGroupRule(SchemaAdapter node, InputDefinition * thisID){

    bool pass = true;
    SumOverGroupRule * rule;
    if (this->hasSumOverGroupRule()){
        std::string errorMessage = "multiple " + std::string(node.name()) + " rules defined";
        thisID->streamErrorMessage(node.parent(), errorMessage);
        pass &= false;
    }
    rule = new SumOverGroupRule(thisID);

    std::string path = node.id();
    if (path == ""){
        std::string errorMessage = "context path missing";
        rule->streamErrorMessage(node, errorMessage);
        pass &= false;
    }
    else{
        if (rule->getInputDefinition()->isValidPath(path, node.parent())){
            rule->setContext(path);
        }
        else{
            std::string errorMessage = "path \"" + path +
                                       "\" checked against \"" +
                                       node.parent().path() +
                                       "\" is not valid";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }
    }

    auto comparePathNode = node.first_non_decorative_child_by_name("ComparePath");
    if (comparePathNode.is_null()){
        std::string errorMessage = "missing ComparePath";
        rule->streamErrorMessage(node, errorMessage);
        pass &= false;
    }
    else{
        std::string comparePathValue = comparePathNode.to_string();

        if (!rule->getInputDefinition()->isValidPath(comparePathValue, node.parent())){
            std::string errorMessage = "ComparePath \"" + comparePathValue +
                                       "\" checked against \"" +
                                       node.parent().path() +
                                       "\" is not valid";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;

        }
        else{
            rule->setComparePathValue(comparePathValue);
        }
    }

    auto groupDivideNode = node.first_non_decorative_child_by_name("GroupDivide");
    if (groupDivideNode.is_null()){
        std::string errorMessage = "missing GroupDivide";
        rule->streamErrorMessage(node, errorMessage);
        pass &= false;
    }
    else{
        std::string groupDivideValue = groupDivideNode.to_string();
        if (!rule->getInputDefinition()->isReal(groupDivideValue)){
            std::string errorMessage = "GroupDivide value \"" + groupDivideValue +
                                       "\" is not a number";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }
        else{
            rule->setGroupDivideValue(stod(groupDivideValue));
        }
    }

    auto groupSumNode = node.first_non_decorative_child_by_name("GroupSum");
    if (groupSumNode.is_null()){
        std::string errorMessage = "missing GroupSum";
        rule->streamErrorMessage(node, errorMessage);
        pass &= false;
    }
    else{
        std::string groupSumValue = groupSumNode.to_string();
        if (!rule->getInputDefinition()->isReal(groupSumValue)){
            std::string errorMessage = "GroupSum value \"" + groupSumValue +
                                       "\" is not a number";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }
        else{
            rule->setGroupSumValue(stod(groupSumValue));
        }
    }

    if (pass == true) this->sumOverGroupRuleInstance = rule;
    else delete rule;
    return pass;

}

template<class SchemaAdapter>
bool IDObject::addIncreaseOverRule(SchemaAdapter node, InputDefinition * thisID){

    bool pass = true;
    IncreaseOverRule * rule;
    if (this->hasIncreaseOverRule()){
        std::string errorMessage = "multiple " + std::string(node.name()) + " rules defined";
        thisID->streamErrorMessage(node.parent(), errorMessage);
        pass &= false;
    }
    rule = new IncreaseOverRule(thisID);

    std::string path = node.id();
    if (path == ""){
        std::string errorMessage = "context path missing";
        rule->streamErrorMessage(node, errorMessage);
        pass &= false;
    }
    else{
        if (rule->getInputDefinition()->isValidPath(path, node.parent())){
            rule->setContext(path);
        }
        else{
            std::string errorMessage = "path \"" + path +
                                       "\" checked against \"" +
                                       node.parent().path() +
                                       "\" is not valid";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }
    }

    std::string ruleValue = node.to_string();
    if (rule->isValidOption(ruleValue)){
        rule->setType(ruleValue);
    }
    else{
        const std::vector<std::string> & validOptions = rule->getValidOptions();
        std::string errorMessage = "\"" + ruleValue + "\" is not a valid option " +
                                   " - valid choices are [ ";

        for (auto it = validOptions.begin(); it != validOptions.end(); it++){
            errorMessage += "\"" + (*it) + "\" ";
        }
        errorMessage += "]";
        rule->streamErrorMessage(node, errorMessage);
        pass &= false;
    }

    if (pass == true) this->increaseOverRuleInstance = rule;
    else delete rule;
    return pass;

}

template<class SchemaAdapter>
bool IDObject::addDecreaseOverRule(SchemaAdapter node, InputDefinition * thisID){

    bool pass = true;
    DecreaseOverRule * rule;
    if (this->hasDecreaseOverRule()){
        std::string errorMessage = "multiple " + std::string(node.name()) + " rules defined";
        thisID->streamErrorMessage(node.parent(), errorMessage);
        pass &= false;
    }
    rule = new DecreaseOverRule(thisID);

    std::string path = node.id();
    if (path == ""){
        std::string errorMessage = "context path missing";
        rule->streamErrorMessage(node, errorMessage);
        pass &= false;
    }
    else{
        if (rule->getInputDefinition()->isValidPath(path, node.parent())){
            rule->setContext(path);
        }
        else{
            std::string errorMessage = "path \"" + path +
                                       "\" checked against \"" +
                                       node.parent().path() +
                                       "\" is not valid";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }
    }

    std::string ruleValue = node.to_string();
    if (rule->isValidOption(ruleValue)){
        rule->setType(ruleValue);
    }
    else{
        const std::vector<std::string> & validOptions = rule->getValidOptions();
        std::string errorMessage = "\"" + ruleValue + "\" is not a valid option " +
                                   " - valid choices are [ ";

        for (auto it = validOptions.begin(); it != validOptions.end(); it++){
            errorMessage += "\"" + (*it) + "\" ";
        }
        errorMessage += "]";
        rule->streamErrorMessage(node, errorMessage);
        pass &= false;
    }

    if (pass == true) this->decreaseOverRuleInstance = rule;
    else delete rule;
    return pass;

}

template<class SchemaAdapter>
bool IDObject::addChildAtMostOneRule(SchemaAdapter node, InputDefinition * thisID){

    bool pass = true;
    size_t validPathsCount = 0;
    ChildAtMostOneRule * rule = new ChildAtMostOneRule(thisID);

    const auto & children = node.non_decorative_children();
    for(size_t i = 0; i < children.size(); i++){

        std::string path;
        std::string value;
        if (children[i].child_count() == 0){
            path = children[i].to_string();
            value = "";
        }
        else{
            path = children[i].name();
            value = children[i].to_string();
        }

        if (rule->getInputDefinition()->isValidPath(path, node.parent())){
            std::pair<std::string,std::string> pathValuePair = std::make_pair(path,value);
            rule->addPathValuePair(pathValuePair);
            validPathsCount++;
        }
        else{
            std::string errorMessage = "path \"" + path +
                                       "\" checked against \"" +
                                       node.parent().path() +
                                       "\" is not valid";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }

    }

    if (pass == true) this->childAtMostOneRuleList.push_back(rule);
    else delete rule;
    return pass;

}

template<class SchemaAdapter>
bool IDObject::addChildExactlyOneRule(SchemaAdapter node, InputDefinition * thisID){

    bool pass = true;
    size_t validPathsCount = 0;
    ChildExactlyOneRule * rule = new ChildExactlyOneRule(thisID);

    const auto & children = node.non_decorative_children();
    for(size_t i = 0; i < children.size(); i++){

        std::string path;
        std::string value;
        if (children[i].child_count() == 0){
            path = children[i].to_string();
            value = "";
        }
        else{
            path = children[i].name();
            value = children[i].to_string();
        }

        if (rule->getInputDefinition()->isValidPath(path, node.parent())){
            std::pair<std::string,std::string> pathValuePair = std::make_pair(path,value);
            rule->addPathValuePair(pathValuePair);
            validPathsCount++;
        }
        else{
            std::string errorMessage = "path \"" + path +
                                       "\" checked against \"" +
                                       node.parent().path() +
                                       "\" is not valid";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }

    }

    if (pass == true) this->childExactlyOneRuleList.push_back(rule);
    else delete rule;
    return pass;

}

template<class SchemaAdapter>
bool IDObject::addChildAtLeastOneRule(SchemaAdapter node, InputDefinition * thisID){

    bool pass = true;
    size_t validPathsCount = 0;
    ChildAtLeastOneRule * rule = new ChildAtLeastOneRule(thisID);

    const auto & children = node.non_decorative_children();
    for(size_t i = 0; i < children.size(); i++){

        std::string path;
        std::string value;
        if (children[i].child_count() == 0){
            path = children[i].to_string();
            value = "";
        }
        else{
            path = children[i].name();
            value = children[i].to_string();
        }

        if (rule->getInputDefinition()->isValidPath(path, node.parent())){
            std::pair<std::string,std::string> pathValuePair = std::make_pair(path,value);
            rule->addPathValuePair(pathValuePair);
            validPathsCount++;
        }
        else{
            std::string errorMessage = "path \"" + path +
                                       "\" checked against \"" +
                                       node.parent().path() +
                                       "\" is not valid";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }

    }

    if (pass == true) this->childAtLeastOneRuleList.push_back(rule);
    else delete rule;
    return pass;

}

template<class SchemaAdapter>
bool IDObject::addChildCountEqualRule(SchemaAdapter node, InputDefinition * thisID){

    bool pass = true;
    size_t validPathsCount = 0;
    ChildCountEqualRule * rule = new ChildCountEqualRule(thisID);

    std::string modifierFlag = node.id();
    if (modifierFlag == ""){
        const std::vector<std::string> & validOptions = rule->getValidOptions();
        std::string errorMessage = "option type not provided - valid choices are [ ";
        for (auto it = validOptions.begin(); it != validOptions.end(); it++){
            errorMessage += "\"" + (*it) + "\" ";
        }
        errorMessage += "]";
        rule->streamErrorMessage(node, errorMessage);
        pass &= false;
    }
    else{
        if (rule->isValidOption(modifierFlag)){
            rule->setModifierFlag(modifierFlag);
        }
        else{
            const std::vector<std::string> & validOptions = rule->getValidOptions();
            std::string errorMessage = "\"" + modifierFlag + "\" is not a valid option " +
                                       " - valid choices are [ ";

            for (auto it = validOptions.begin(); it != validOptions.end(); it++){
                errorMessage += "\"" + (*it) + "\" ";
            }
            errorMessage += "]";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }
    }

    const auto & children = node.non_decorative_children();
    for(size_t i = 0; i < children.size(); i++){

        std::string path = children[i].to_string();
        if (rule->getInputDefinition()->isValidPath(path, node.parent())){
            rule->addLookupPath(path);
            validPathsCount++;
        }
        else{
            std::string errorMessage = "path \"" + path +
                                       "\" checked against \"" +
                                       node.parent().path() +
                                       "\" is not valid";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }

    }


    if (pass == true) this->childCountEqualRuleList.push_back(rule);
    else delete rule;
    return pass;

}

template<class SchemaAdapter>
bool IDObject::addChildUniquenessRule(SchemaAdapter node, InputDefinition * thisID){

    bool pass = true;
    size_t validPathsCount = 0;
    ChildUniquenessRule * rule = new ChildUniquenessRule(thisID);

    std::string modifierFlag = node.id();
    if (modifierFlag != ""){
        if (rule->isValidOption(modifierFlag)){
            rule->setModifierFlag(modifierFlag);
        }
        else{
            const std::vector<std::string> & validOptions = rule->getValidOptions();
            std::string errorMessage = "\"" + modifierFlag + "\" is not a valid option " +
                                       " - valid choices are [ ";

            for (auto it = validOptions.begin(); it != validOptions.end(); it++){
                errorMessage += "\"" + (*it) + "\" ";
            }
            errorMessage += "]";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }
    }

    const auto & children = node.non_decorative_children();
    for(size_t i = 0; i < children.size(); i++){

        std::string path = children[i].to_string();
        if (rule->getInputDefinition()->isValidPath(path, node.parent())){
            rule->addLookupPath(path);
            validPathsCount++;
        }
        else{
            std::string errorMessage = "path \"" + path +
                                       "\" checked against \"" +
                                       node.parent().path() +
                                       "\" is not valid";
            rule->streamErrorMessage(node, errorMessage);
            pass &= false;
        }

    }

    if (pass == true) this->childUniquenessRuleList.push_back(rule);
    else delete rule;
    return pass;

}
