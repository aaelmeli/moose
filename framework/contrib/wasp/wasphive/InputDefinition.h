/*
 * File:   InputDefinition.h
 * Author: orl
 *
 */

#ifndef WASP_INPUTDEFINITION_H
#define	WASP_INPUTDEFINITION_H

#include <iostream>
#include <ostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include "waspsiren/SIRENInterpreter.h"
#include "waspsiren/SIRENResultSet.h"

namespace wasp
{
class InputDefinition;
class IDObject;
class MinOccursRule;
class MaxOccursRule;
class MinValIncRule;
class MinValExcRule;
class MaxValIncRule;
class MaxValExcRule;
class ValTypeRule;
class ValEnumsRule;
class InputChoicesRule;
class ExistsInRule;
class NotExistsInRule;
class SumOverRule;
class SumOverGroupRule;
class IncreaseOverRule;
class DecreaseOverRule;
class ChildAtMostOneRule;
class ChildExactlyOneRule;
class ChildAtLeastOneRule;
class ChildCountEqualRule;
class ChildUniquenessRule;

class IDObject{

    public:

        IDObject();
        ~IDObject();

        void setObjectName(const std::string & name){
            this->objectName = name;
        }
        void setInputName(const std::string & name){
            this->inputName = name;
        }
        void setObjectPath(const std::string & path){
            this->objectPath = path;
        }
        std::string getObjectName(){
            return this->objectName;
        }
        std::string getInputName(){
            if (this->inputName == "") return this->objectName;
            else                       return this->inputName;
        }
        std::string getObjectPath(){
            return this->objectPath;
        }
        void setObjectType(const std::string& type){
            this->objectType = type;
        }
        std::string getObjectType(){
            return this->objectType;
        }
        void setObjectUnits(const std::string& units){
            this->objectUnits = units;
        }
        bool hasObjectUnits(){
            if (this->objectUnits == "") return false;
            else                         return true;
        }
        std::string getObjectUnits(){
            return this->objectUnits;
        }

        void setObjectDescription(const std::string& description){
            this->objectDescription = description;
        }
        bool hasObjectDescription(){
            if (this->objectDescription == "") return false;
            else                               return true;
        }
        std::string getObjectDescription(){
            return this->objectDescription;
        }

        void setInputDefault(const std::string& inputdefault){
            this->inputDefault = inputdefault;
        }
        bool hasInputDefault(){
            if (this->inputDefault == "") return false;
            else                          return true;
        }
        std::string getInputDefault(){
            return this->inputDefault;
        }

        void setInputTmpl(const std::string& tmplname){
            this->inputTmpl = tmplname;
        }
        bool hasInputTmpl(){
            if (this->inputTmpl == "") return false;
            else                       return true;
        }
        std::string getInputTmpl(){
            return this->inputTmpl;
        }

        void addInputVariant(const std::string & variant){
            this->inputVariantsList.push_back(variant);
        }
        size_t getInputVariantCount(){
            return inputVariantsList.size();
        }
        std::string getInputVariantAt(size_t index){
            return inputVariantsList.at(index);
        }

        void addInputAlias(const std::string & alias){
            this->inputAliasesList.push_back(alias);
        }
        size_t getInputAliasCount(){
            return inputAliasesList.size();
        }
        std::string getInputAliasAt(size_t index){
            return inputAliasesList.at(index);
        }

        void addIDChild(IDObject * child){
            this->children.push_back(child);
        }
        std::vector<IDObject *> getIDChildren(){
            return this->children;
        }

        void setIDParent(IDObject * parent){
            this->parent = parent;
        }
        IDObject * getIDParent(){
            return this->parent;
        }

        IDObject * getValueNode(){
            size_t numChildren = children.size();
            for(size_t i = 0; i < numChildren; i++){
                if (children[i]->getObjectName() == "value"){
                    return children[i];
                }
            }
            return nullptr;
        }

        template<class SchemaAdapter>
        bool addRule(SchemaAdapter, InputDefinition *);

        bool hasMinOccursRule(){
            if (this->minOccursRuleInstance       == NULL) return false;
            else return true;
        }
        bool hasMaxOccursRule(){
            if (this->maxOccursRuleInstance       == NULL) return false;
            else return true;
        }
        bool hasMinValIncRule(){
            if (this->minValIncRuleInstance       == NULL) return false;
            else return true;
        }
        bool hasMinValExcRule(){
            if (this->minValExcRuleInstance       == NULL) return false;
            else return true;
        }
        bool hasMaxValIncRule(){
            if (this->maxValIncRuleInstance       == NULL) return false;
            else return true;
        }
        bool hasMaxValExcRule(){
            if (this->maxValExcRuleInstance       == NULL) return false;
            else return true;
        }
        bool hasValTypeRule(){
            if (this->valTypeRuleInstance         == NULL) return false;
            else return true;
        }
        bool hasValEnumsRule(){
            if (this->valEnumsRuleInstance        == NULL) return false;
            else return true;
        }
        bool hasInputChoicesRule(){
            if (this->inputChoicesRuleInstance    == NULL) return false;
            else return true;
        }
        bool hasExistsInRule(){
            if (this->existsInRuleInstance        == NULL) return false;
            else return true;
        }
        bool hasNotExistsInRule(){
            if (this->notExistsInRuleInstance     == NULL) return false;
            else return true;
        }
        bool hasSumOverRule(){
            if (this->sumOverRuleInstance         == NULL) return false;
            else return true;
        }
        bool hasSumOverGroupRule(){
            if (this->sumOverGroupRuleInstance    == NULL) return false;
            else return true;
        }
        bool hasIncreaseOverRule(){
            if (this->increaseOverRuleInstance    == NULL) return false;
            else return true;
        }
        bool hasDecreaseOverRule(){
            if (this->decreaseOverRuleInstance    == NULL) return false;
            else return true;
        }
        size_t getChildAtMostOneCount(){
            return childAtMostOneRuleList.size();
        }
        size_t getChildExactlyOneCount(){
            return childExactlyOneRuleList.size();
        }
        size_t getChildAtLeastOneCount(){
            return childAtLeastOneRuleList.size();
        }
        size_t getChildCountEqualCount(){
            return childCountEqualRuleList.size();
        }
        size_t getChildUniquenessCount(){
            return childUniquenessRuleList.size();
        }

        MinOccursRule * getMinOccurs(){
            return this->minOccursRuleInstance;
        }
        MaxOccursRule * getMaxOccurs(){
            return this->maxOccursRuleInstance;
        }
        MinValIncRule * getMinValInc(){
            return this->minValIncRuleInstance;
        }
        MinValExcRule * getMinValExc(){
            return this->minValExcRuleInstance;
        }
        MaxValIncRule * getMaxValInc(){
            return this->maxValIncRuleInstance;
        }
        MaxValExcRule * getMaxValExc(){
            return this->maxValExcRuleInstance;
        }
        ValTypeRule * getValType(){
            return this->valTypeRuleInstance;
        }
        ValEnumsRule * getValEnums(){
            return this->valEnumsRuleInstance;
        }
        InputChoicesRule * getInputChoices(){
            return this->inputChoicesRuleInstance;
        }
        ExistsInRule * getExistsIn(){
            return this->existsInRuleInstance;
        }
        NotExistsInRule * getNotExistsIn(){
            return this->notExistsInRuleInstance;
        }
        SumOverRule * getSumOver(){
            return this->sumOverRuleInstance;
        }
        SumOverGroupRule * getSumOverGroup(){
            return this->sumOverGroupRuleInstance;
        }
        IncreaseOverRule * getIncreaseOver(){
            return this->increaseOverRuleInstance;
        }
        DecreaseOverRule * getDecreaseOver(){
            return this->decreaseOverRuleInstance;
        }
        ChildAtMostOneRule * getChildAtMostOneAt(size_t index){
            return childAtMostOneRuleList.at(index);
        }
        ChildExactlyOneRule * getChildExactlyOneAt(size_t index){
            return childExactlyOneRuleList.at(index);
        }
        ChildAtLeastOneRule * getChildAtLeastOneAt(size_t index){
            return childAtLeastOneRuleList.at(index);
        }
        ChildCountEqualRule * getChildCountEqualAt(size_t index){
            return childCountEqualRuleList.at(index);
        }
        ChildUniquenessRule * getChildUniquenessAt(size_t index){
            return childUniquenessRuleList.at(index);
        }

    private:

        std::string objectName;
        std::string inputName;
        std::string objectPath;
        std::string objectType;
        std::string objectUnits;
        std::string objectDescription;
        std::string inputDefault;
        std::string inputTmpl;
        std::vector<std::string> inputVariantsList;
        std::vector<std::string> inputAliasesList;
        std::vector<IDObject *> children;
        IDObject * parent;

        MinOccursRule       * minOccursRuleInstance;
        MaxOccursRule       * maxOccursRuleInstance;
        MinValIncRule       * minValIncRuleInstance;
        MinValExcRule       * minValExcRuleInstance;
        MaxValIncRule       * maxValIncRuleInstance;
        MaxValExcRule       * maxValExcRuleInstance;
        ValTypeRule         * valTypeRuleInstance;
        ValEnumsRule        * valEnumsRuleInstance;
        InputChoicesRule    * inputChoicesRuleInstance;
        ExistsInRule        * existsInRuleInstance;
        NotExistsInRule     * notExistsInRuleInstance;
        SumOverRule         * sumOverRuleInstance;
        SumOverGroupRule    * sumOverGroupRuleInstance;
        IncreaseOverRule    * increaseOverRuleInstance;
        DecreaseOverRule    * decreaseOverRuleInstance;
        std::vector<ChildAtMostOneRule  *> childAtMostOneRuleList;
        std::vector<ChildExactlyOneRule *> childExactlyOneRuleList;
        std::vector<ChildAtLeastOneRule *> childAtLeastOneRuleList;
        std::vector<ChildCountEqualRule *> childCountEqualRuleList;
        std::vector<ChildUniquenessRule *> childUniquenessRuleList;

        template<class SchemaAdapter>
        bool addMinOccursRule       (SchemaAdapter, InputDefinition *);
        
        template<class SchemaAdapter>
        bool addMaxOccursRule       (SchemaAdapter, InputDefinition *);
        
        template<class SchemaAdapter>
        bool addMinValIncRule       (SchemaAdapter, InputDefinition *);
        
        template<class SchemaAdapter>
        bool addMinValExcRule       (SchemaAdapter, InputDefinition *);
        
        template<class SchemaAdapter>
        bool addMaxValIncRule       (SchemaAdapter, InputDefinition *);
        
        template<class SchemaAdapter>
        bool addMaxValExcRule       (SchemaAdapter, InputDefinition *);
        
        template<class SchemaAdapter>
        bool addValTypeRule         (SchemaAdapter, InputDefinition *);
        
        template<class SchemaAdapter>
        bool addValEnumsRule        (SchemaAdapter, InputDefinition *);
        
        template<class SchemaAdapter>
        bool addInputChoicesRule    (SchemaAdapter, InputDefinition *);
        
        template<class SchemaAdapter>
        bool addExistsInRule        (SchemaAdapter, InputDefinition *);
        
        template<class SchemaAdapter>
        bool addNotExistsInRule     (SchemaAdapter, InputDefinition *);
        
        template<class SchemaAdapter>
        bool addSumOverRule         (SchemaAdapter, InputDefinition *);
        
        template<class SchemaAdapter>
        bool addSumOverGroupRule    (SchemaAdapter, InputDefinition *);       
        
        template<class SchemaAdapter>
        bool addIncreaseOverRule    (SchemaAdapter, InputDefinition *);
        
        template<class SchemaAdapter>
        bool addDecreaseOverRule    (SchemaAdapter, InputDefinition *);
        
        template<class SchemaAdapter>
        bool addChildAtMostOneRule  (SchemaAdapter, InputDefinition *);
        
        template<class SchemaAdapter>
        bool addChildExactlyOneRule (SchemaAdapter, InputDefinition *);
        
        template<class SchemaAdapter>
        bool addChildAtLeastOneRule (SchemaAdapter, InputDefinition *);
        
        template<class SchemaAdapter>
        bool addChildCountEqualRule (SchemaAdapter, InputDefinition *);
        
        template<class SchemaAdapter>
        bool addChildUniquenessRule (SchemaAdapter, InputDefinition *);
};

class InputDefinition{

    public:
        static const int POSINF;
        static const int NEGINF;

        template<class SchemaAdapter>
        InputDefinition(SchemaAdapter schRoot,
                        std::ostream & outputStream = std::cout,
                        std::ostream & errorStream  = std::cerr);
        ~InputDefinition();

        void setInitialized(bool yes_or_no){
            this->initialized = yes_or_no;
        }
        bool isInitialized(){
            return this->initialized;
        }

        void setRootObject(IDObject * object){
            this->rootObject = object;
        }
        IDObject * getRootObject()const{
            return this->rootObject;
        }

        IDObject * pathLookup(const std::string& path) const
        {
            // current child (root)
            auto id = getRootObject();

            // selecting root, return it
            if(path == "/")
            {
                return id;
            }

            // list of node names, stream used to populate it
            std::stringstream ss(path);

            for(std::string name; std::getline(ss, name, '/');)
            {
                // skip empty name (consecutive separators)
                if(name.empty())
                {
                    continue;
                }

                // matching child
                IDObject *match = nullptr;

                // look for matching child
                for(auto child : id->getIDChildren())
                {
                    // if there is a "*" grab that one in case there is no name match
                    if (child->getObjectName() == "*") match = child;

                    // child's name matches, stop this loop
                    else if(child->getObjectName() == name)
                    {
                        match = child;
                        break;
                    }
                }

                // no match found, stop
                if(match == nullptr)
                {
                    bool is_dec = (DecorativeNames.find(name) != DecorativeNames.end());

                    return is_dec ? id : nullptr;;
                }

                // update id
                id = match;
            }

            return id;
        }

        std::map< std::string, std::vector<std::string> > * getEnumRefs(){
            return &(this->enumRefs);
        }
        void setErrorStream(std::ostream & error_stream){
            this->errorStream = &error_stream;
        }
        std::ostream * getErrorStream(){
            return this->errorStream;
        }
        void setOutputStream(std::ostream & outputStream){
            this->xout = &outputStream;
        }

        template<class SchemaAdapter>
        bool fillEnumReferences(SchemaAdapter);

        template<class SchemaAdapter>
        bool fillInputDefinition(IDObject *, SchemaAdapter);

        template<class SchemaAdapter>
        bool addEnumRef(SchemaAdapter);

        bool isInt(const std::string& value){
            std::istringstream issRV(value);
            int ftest;
            issRV >> std::noskipws >> ftest;
            if (issRV.eof() && !issRV.fail()) return true;
            else return false;
        }
        bool isReal(const std::string& value){
            std::istringstream issRV(value);
            float ftest;
            issRV >> std::noskipws >> ftest;
            if (issRV.eof() && !issRV.fail()) return true;
            else return false;
        }

        template<class SchemaAdapter>
        bool isValidPath(const std::string& path, SchemaAdapter node){

            std::string pathCopy = path;
            auto tmpNode = node;
            int levels = std::count(pathCopy.begin(), pathCopy.end(), '/') + 1;

            for(int i = 0; i < levels; i++){

                std::string subPath = pathCopy.substr(0, pathCopy.find("/"));
                pathCopy = pathCopy.substr(pathCopy.find("/") + 1);

                if (std::count(subPath.begin(), subPath.end(), '[') == 1){
                    if (std::count(subPath.begin(), subPath.end(), ']') != 1) return false;
                    if (subPath.find("[") >= subPath.find("]")) return false;
                    subPath = subPath.substr(0, subPath.find("["));
                }
                if (subPath == ".."){
                    tmpNode = tmpNode.parent();
                    if(tmpNode.is_null()) return false;
                }
                else if (std::count(subPath.begin(), subPath.end(), '.') != 0){
                    return false;
                }
                else{
                    auto tmp = tmpNode.first_non_decorative_child_by_name(subPath);

                    if (tmp.is_null()){
                        tmp = tmpNode.first_non_decorative_child_by_name("*");

                        if (tmp.is_null()){
                            tmp = tmpNode.first_child_by_name(subPath);
                        }
                    }

                    if (tmp.is_null()) return false;
                    tmpNode = tmp;
                }

            }

            return true;
        }

        template<class SchemaAdapter>
        void streamErrorMessage(SchemaAdapter node, std::string errorMessage){
            (*this->getErrorStream()) << node.name() <<
                                      " :: line: " << node.line() <<
                                      " column: " << node.column() <<
                                      " - " << errorMessage << std::endl;
        }

        const std::vector<std::string> & getValTypeOptions(){
            return this->ValTypeOptions;
        }
        const std::vector<std::string> & getExistsInOptions(){
            return this->ExistsInOptions;
        }
        const std::vector<std::string> & getUniqueOptions(){
            return this->UniqueOptions;
        }
        const std::vector<std::string> & getIncDecOptions(){
            return this->IncDecOptions;
        }
        const std::vector<std::string> & getCCEOptions(){
            return this->CCEOptions;
        }

        bool getAbsolutePath(IDObject * node, std::string& relativePath,
                                              std::string& absolutePath)const;
        void printMarkdownAll();
        void printMarkdownTableOfContents(IDObject *, int level);
        void printMarkdownDocumentation(IDObject *);
        void printMarkdownReferencedLists();
        void addReferenceUsage(std::string refName){
            this->referencesUsed.insert(refName);
        }

        std::string createBreadcrumbString(IDObject * node);

        std::string howManyString(IDObject * IDNode, int minOccurs);

    private:

        bool initialized;
        //std::string filename;
        IDObject * rootObject;
        std::map< std::string, std::vector<std::string> > enumRefs;
        std::set<std::string> referencesUsed;
        std::ostream * errorStream;
        std::ostream * xout;
        std::vector<std::string> ValTypeOptions;
        std::vector<std::string> ExistsInOptions;
        std::vector<std::string> UniqueOptions;
        std::vector<std::string> IncDecOptions;
        std::vector<std::string> CCEOptions;
        std::set   <std::string> DecorativeNames;
};

class MinOccursRule{

    public:
        MinOccursRule(InputDefinition * thisID){
            this->setInputDefinition(thisID);
            this->setConstantValueExists(false);
        }
        ~MinOccursRule(){}

        void setInputDefinition(InputDefinition * thisID){
            this->thisID = thisID;
        }
        InputDefinition * getInputDefinition(){
            return this->thisID;
        }

        template<class SchemaAdapter>
        void streamErrorMessage(SchemaAdapter node, std::string errorMessage){
            this->getInputDefinition()->streamErrorMessage(node, errorMessage);
        }

        void setConstantValueExists(bool yes_or_no){
            this->constantValueExists = yes_or_no;
        }
        void setConstantValue(int value){
            this->constantValue = value;
        }
        bool hasConstantValue(){
            return this->constantValueExists;
        }
        int getConstantValue(){
            return this->constantValue;
        }

        void addLookupPath(const std::string & lookuppath){
            this->lookupPathsList.push_back(lookuppath);
        }
        size_t getLookupPathCount(){
            return lookupPathsList.size();
        }
        std::string getLookupPathAt(size_t index){
            return lookupPathsList.at(index);
        }

    private:
        InputDefinition * thisID;
        bool constantValueExists;
        int constantValue;
        std::vector<std::string> lookupPathsList;

};

class MaxOccursRule{

    public:
        MaxOccursRule(InputDefinition * thisID){
            this->setInputDefinition(thisID);
            this->setConstantValueExists(false);
        }
        ~MaxOccursRule(){}

        void setInputDefinition(InputDefinition * thisID){
            this->thisID = thisID;
        }
        InputDefinition * getInputDefinition(){
            return this->thisID;
        }

        template<class SchemaAdapter>
        void streamErrorMessage(SchemaAdapter node, std::string errorMessage){
            this->getInputDefinition()->streamErrorMessage(node, errorMessage);
        }

        void setConstantValueExists(bool yes_or_no){
            this->constantValueExists = yes_or_no;
        }
        void setConstantValue(int value){
            this->constantValue = value;
        }
        bool hasConstantValue(){
            return this->constantValueExists;
        }
        int getConstantValue(){
            return this->constantValue;
        }

        void addLookupPath(const std::string & lookuppath){
            this->lookupPathsList.push_back(lookuppath);
        }
        size_t getLookupPathCount(){
            return lookupPathsList.size();
        }
        std::string getLookupPathAt(size_t index){
            return lookupPathsList.at(index);
        }

    private:
        InputDefinition * thisID;
        bool constantValueExists;
        int constantValue;
        std::vector<std::string> lookupPathsList;

};

class MinValIncRule{

    public:
        MinValIncRule(InputDefinition * thisID){
            this->setInputDefinition(thisID);
            this->setConstantValueExists(false);
        }
        ~MinValIncRule(){}

        void setInputDefinition(InputDefinition * thisID){
            this->thisID = thisID;
        }
        InputDefinition * getInputDefinition(){
            return this->thisID;
        }

        template<class SchemaAdapter>
        void streamErrorMessage(SchemaAdapter node, std::string errorMessage){
            this->getInputDefinition()->streamErrorMessage(node, errorMessage);
        }

        void setConstantValueExists(bool yes_or_no){
            this->constantValueExists = yes_or_no;
        }
        void setConstantValue(double value){
            this->constantValue = value;
        }
        bool hasConstantValue(){
            return this->constantValueExists;
        }
        double getConstantValue(){
            return this->constantValue;
        }

        void addLookupPath(const std::string & lookuppath){
            this->lookupPathsList.push_back(lookuppath);
        }
        size_t getLookupPathCount(){
            return lookupPathsList.size();
        }
        std::string getLookupPathAt(size_t index){
            return lookupPathsList.at(index);
        }

    private:
        InputDefinition * thisID;
        bool constantValueExists;
        double constantValue;
        std::vector<std::string> lookupPathsList;

};

class MinValExcRule{

    public:
        MinValExcRule(InputDefinition * thisID){
            this->setInputDefinition(thisID);
            this->setConstantValueExists(false);
        }
        ~MinValExcRule(){}

        void setInputDefinition(InputDefinition * thisID){
            this->thisID = thisID;
        }
        InputDefinition * getInputDefinition(){
            return this->thisID;
        }

        template<class SchemaAdapter>
        void streamErrorMessage(SchemaAdapter node, std::string errorMessage){
            this->getInputDefinition()->streamErrorMessage(node, errorMessage);
        }

        void setConstantValueExists(bool yes_or_no){
            this->constantValueExists = yes_or_no;
        }
        void setConstantValue(double value){
            this->constantValue = value;
        }
        bool hasConstantValue(){
            return this->constantValueExists;
        }
        double getConstantValue(){
            return this->constantValue;
        }

        void addLookupPath(const std::string & lookuppath){
            this->lookupPathsList.push_back(lookuppath);
        }
        size_t getLookupPathCount(){
            return lookupPathsList.size();
        }
        std::string getLookupPathAt(size_t index){
            return lookupPathsList.at(index);
        }

    private:
        InputDefinition * thisID;
        bool constantValueExists;
        double constantValue;
        std::vector<std::string> lookupPathsList;

};

class MaxValIncRule{

    public:
        MaxValIncRule(InputDefinition * thisID){
            this->setInputDefinition(thisID);
            this->setConstantValueExists(false);
        }
        ~MaxValIncRule(){}

        void setInputDefinition(InputDefinition * thisID){
            this->thisID = thisID;
        }
        InputDefinition * getInputDefinition(){
            return this->thisID;
        }

        template<class SchemaAdapter>
        void streamErrorMessage(SchemaAdapter node, std::string errorMessage){
            this->getInputDefinition()->streamErrorMessage(node, errorMessage);
        }

        void setConstantValueExists(bool yes_or_no){
            this->constantValueExists = yes_or_no;
        }
        void setConstantValue(double value){
            this->constantValue = value;
        }
        bool hasConstantValue(){
            return this->constantValueExists;
        }
        double getConstantValue(){
            return this->constantValue;
        }

        void addLookupPath(const std::string & lookuppath){
            this->lookupPathsList.push_back(lookuppath);
        }
        size_t getLookupPathCount(){
            return lookupPathsList.size();
        }
        std::string getLookupPathAt(size_t index){
            return lookupPathsList.at(index);
        }

    private:
        InputDefinition * thisID;
        bool constantValueExists;
        double constantValue;
        std::vector<std::string> lookupPathsList;

};

class MaxValExcRule{

    public:
        MaxValExcRule(InputDefinition * thisID){
            this->setInputDefinition(thisID);
            this->setConstantValueExists(false);
        }
        ~MaxValExcRule(){}

        void setInputDefinition(InputDefinition * thisID){
            this->thisID = thisID;
        }
        InputDefinition * getInputDefinition(){
            return this->thisID;
        }

        template<class SchemaAdapter>
        void streamErrorMessage(SchemaAdapter node, std::string errorMessage){
            this->getInputDefinition()->streamErrorMessage(node, errorMessage);
        }

        void setConstantValueExists(bool yes_or_no){
            this->constantValueExists = yes_or_no;
        }
        void setConstantValue(double value){
            this->constantValue = value;
        }
        bool hasConstantValue(){
            return this->constantValueExists;
        }
        double getConstantValue(){
            return this->constantValue;
        }

        void addLookupPath(const std::string & lookuppath){
            this->lookupPathsList.push_back(lookuppath);
        }
        size_t getLookupPathCount(){
            return lookupPathsList.size();
        }
        std::string getLookupPathAt(size_t index){
            return lookupPathsList.at(index);
        }

    private:
        InputDefinition * thisID;
        bool constantValueExists;
        double constantValue;
        std::vector<std::string> lookupPathsList;

};

class ValTypeRule{

    public:
        ValTypeRule(InputDefinition * thisID){
            this->setInputDefinition(thisID);
        }
        ~ValTypeRule(){}

        void setInputDefinition(InputDefinition * thisID){
            this->thisID = thisID;
        }
        InputDefinition * getInputDefinition(){
            return this->thisID;
        }

        template<class SchemaAdapter>
        void streamErrorMessage(SchemaAdapter node, std::string errorMessage){
            this->getInputDefinition()->streamErrorMessage(node, errorMessage);
        }

        void setType(const std::string & type){
            this->type = type;
        }
        std::string getType(){
            return this->type;
        }

        const std::vector<std::string> & getValidOptions(){
            return this->getInputDefinition()->getValTypeOptions();
        }
        bool isValidOption(const std::string& lookupString){
            const std::vector<std::string> & validOptions = this->getValidOptions();
            if (std::find(validOptions.begin(), validOptions.end(), lookupString) ==
                          validOptions.end()){
                return false;
            }
            else return true;
        }

    private:
        InputDefinition * thisID;
        std::string type;

};

class ValEnumsRule{

    public:
        ValEnumsRule(InputDefinition * thisID){
            this->setInputDefinition(thisID);
        }
        ~ValEnumsRule(){}

        void setInputDefinition(InputDefinition * thisID){
            this->thisID = thisID;
        }
        InputDefinition * getInputDefinition(){
            return this->thisID;
        }

        template<class SchemaAdapter>
        void streamErrorMessage(SchemaAdapter node, std::string errorMessage){
            this->getInputDefinition()->streamErrorMessage(node, errorMessage);
        }

        bool isEnumRefNameDefined(const std::string & enumrefname){
            std::map< std::string, std::vector<std::string> > * enumRefPtr =
                                    this->getInputDefinition()->getEnumRefs();
            if (enumRefPtr->find(enumrefname) == enumRefPtr->end()) return false;
            else return true;
        }
        void addEnumRefName(const std::string & enumrefname){
            this->enumRefNames.push_back(enumrefname);
        }
        void addEnumValue(const std::string & enumvalue){
            this->enumList.push_back(enumvalue);
        }
        size_t getEnumsCount(){
            std::map< std::string, std::vector<std::string> > * enumRefPtr =
                                    this->getInputDefinition()->getEnumRefs();
            size_t size = enumList.size();
            for(size_t i = 0; i < enumRefNames.size(); i++){
                size += enumRefPtr->find(enumRefNames[i])->second.size();
            }
            return size;
        }
        std::string getEnumsAt(size_t index){
            if (index < enumList.size()){
                return enumList[index];
            }
            size_t tallySize = enumList.size();
            std::map< std::string, std::vector<std::string> > * enumRefPtr =
                                    this->getInputDefinition()->getEnumRefs();
            for(size_t i = 0; i < enumRefNames.size(); i++){
                if (index < tallySize +
                    enumRefPtr->find(enumRefNames[i])->second.size()){
                    return enumRefPtr->find(enumRefNames[i])->second[index-tallySize];
                }
                else{
                    tallySize +=
                        enumRefPtr->find(enumRefNames[i])->second.size();
                }
            }
            return "";
        }
        
        size_t getEnumRefNameCount(){
            return enumRefNames.size();
        }
        std::string getEnumRefNameAt(size_t index){
            return enumRefNames.at(index);
        }
        size_t getRawEnumsCount(){
            return enumList.size();
        }
        std::string getRawEnumsAt(size_t index){
            return enumList.at(index);
        }        

    private:
        InputDefinition * thisID;
        std::vector<std::string> enumList;
        std::vector<std::string> enumRefNames;

};

class InputChoicesRule{

    public:
        InputChoicesRule(InputDefinition * thisID){
            this->setInputDefinition(thisID);
        }
        ~InputChoicesRule(){}

        void setInputDefinition(InputDefinition * thisID){
            this->thisID = thisID;
        }
        InputDefinition * getInputDefinition(){
            return this->thisID;
        }

        template<class SchemaAdapter>
        void streamErrorMessage(SchemaAdapter node, std::string errorMessage){
            this->getInputDefinition()->streamErrorMessage(node, errorMessage);
        }

        bool isEnumRefNameDefined(const std::string & enumrefname){
            std::map< std::string, std::vector<std::string> > * enumRefPtr =
                                    this->getInputDefinition()->getEnumRefs();
            if (enumRefPtr->find(enumrefname) == enumRefPtr->end()) return false;
            else return true;
        }
        void addEnumRefName(const std::string & enumrefname){
            this->enumRefNames.push_back(enumrefname);
        }
        void addEnumValue(const std::string & enumvalue){
            this->enumList.push_back(enumvalue);
        }
        size_t getEnumsCount(){
            std::map< std::string, std::vector<std::string> > * enumRefPtr =
                                    this->getInputDefinition()->getEnumRefs();
            size_t size = enumList.size();
            for(size_t i = 0; i < enumRefNames.size(); i++){
                size += enumRefPtr->find(enumRefNames[i])->second.size();
            }
            return size;
        }
        std::string getEnumsAt(size_t index){
            if (index < enumList.size()){
                return enumList[index];
            }
            size_t tallySize = enumList.size();
            std::map< std::string, std::vector<std::string> > * enumRefPtr =
                                    this->getInputDefinition()->getEnumRefs();
            for(size_t i = 0; i < enumRefNames.size(); i++){
                if (index < tallySize +
                    enumRefPtr->find(enumRefNames[i])->second.size()){
                    return enumRefPtr->find(enumRefNames[i])->second[index-tallySize];
                }
                else{
                    tallySize +=
                        enumRefPtr->find(enumRefNames[i])->second.size();
                }
            }
            return "";
        }

    private:
        InputDefinition * thisID;
        std::vector<std::string> enumList;
        std::vector<std::string> enumRefNames;

};

class ExistsInRule{

    public:
        ExistsInRule(InputDefinition * thisID){
            this->setInputDefinition(thisID);
        }
        ~ExistsInRule(){}

        void setInputDefinition(InputDefinition * thisID){
            this->thisID = thisID;
        }
        InputDefinition * getInputDefinition(){
            return this->thisID;
        }

        template<class SchemaAdapter>
        void streamErrorMessage(SchemaAdapter node, std::string errorMessage){
            this->getInputDefinition()->streamErrorMessage(node, errorMessage);
        }

        const std::vector<std::string> & getValidOptions(){
            return this->getInputDefinition()->getExistsInOptions();
        }
        bool isValidOption(const std::string& lookupString){
            const std::vector<std::string> & validOptions = this->getValidOptions();
            if (std::find(validOptions.begin(), validOptions.end(), lookupString) ==
                          validOptions.end()){
                return false;
            }
            else return true;
        }
        void setModifierFlag(const std::string& modifierflag){
            this->modifierFlag = modifierflag;
        }
        bool hasModifierFlag(){
            if (this->modifierFlag == "") return false;
            else return true;
        }
        std::string getModifierFlag(){
            return this->modifierFlag;
        }

        void addLookupPath(const std::string & lookuppath){
            this->lookupPathsList.push_back(lookuppath);
        }
        size_t getLookupPathCount(){
            return lookupPathsList.size();
        }
        std::string getLookupPathAt(size_t index){
            return lookupPathsList.at(index);
        }

        bool isEnumRefNameDefined(const std::string & enumrefname){
            std::map< std::string, std::vector<std::string> > * enumRefPtr =
                                    this->getInputDefinition()->getEnumRefs();
            if (enumRefPtr->find(enumrefname) == enumRefPtr->end()) return false;
            else return true;
        }
        void addEnumRefName(const std::string & enumrefname){
            this->enumRefNames.push_back(enumrefname);
        }
        void addConstant(const std::string & constant){
            this->constantsList.push_back(constant);
        }
        size_t getConstantsCount(){
            std::map< std::string, std::vector<std::string> > * enumRefPtr =
                                    this->getInputDefinition()->getEnumRefs();
            size_t size = constantsList.size();
            for(size_t i = 0; i < enumRefNames.size(); i++){
                size += enumRefPtr->find(enumRefNames[i])->second.size();
            }
            return size;
        }
        bool hasConstants(){
            if (this->getConstantsCount() == 0) return false;
            else return true;
        }
        std::string getConstantsAt(size_t index){

            if (index < constantsList.size()){
                return constantsList[index];
            }
            size_t tallySize = constantsList.size();
            std::map< std::string, std::vector<std::string> > * enumRefPtr =
                                    this->getInputDefinition()->getEnumRefs();
            for(size_t i = 0; i < enumRefNames.size(); i++){
                if (index < tallySize +
                    enumRefPtr->find(enumRefNames[i])->second.size()){
                    return enumRefPtr->find(enumRefNames[i])->second[index-tallySize];
                }
                else{
                    tallySize +=
                        enumRefPtr->find(enumRefNames[i])->second.size();
                }
            }
            return "";
        }
        
        size_t getEnumRefNameCount(){
            return enumRefNames.size();
        }
        std::string getEnumRefNameAt(size_t index){
            return enumRefNames.at(index);
        }
        size_t getRawConstantsCount(){
            return constantsList.size();
        }
        std::string getRawConstantsAt(size_t index){
            return constantsList.at(index);  
        }

        template<class INPUTNV>
        size_t lookupNodesCollectByValue(INPUTNV               & given_node ,
                                         std::set<std::string> & found_keys ) const
        {
            size_t original_size = found_keys.size();

            for (auto path : this->lookupPathsList)
            {
                SIRENInterpreter<> selector;

                if( !selector.parseString( path ) ) break;

                SIRENResultSet<INPUTNV> results;

                auto selected = selector.evaluate( given_node , results );

                for(size_t i = 0; i < selected; i++)
                {
                    INPUTNV node = results.adapted(i);

                    found_keys.insert( node.last_as_string() );
                }
            }

            return found_keys.size() - original_size;
        }

        template<class INPUTNV, typename NODESET>
        size_t lookupNodesByValue(INPUTNV & given_node, NODESET & found_nodes) const
        {
            size_t original_size = found_nodes.size();

            std::string givenModString = given_node.last_as_string();

            if ( this->modifierFlag == "Abs" &&
               ( givenModString.at(0) == '-' ||
                 givenModString.at(0) == '+' ))
            {
                givenModString.erase(givenModString.begin());
            }

            size_t zeroIndex = givenModString.find_first_not_of('0');
            if ( zeroIndex != 0 && zeroIndex != std::string::npos )
            {
                try
                {
                    std::stoi(givenModString);
                    givenModString.erase(0, zeroIndex);
                }
                catch(...){}
            }

            for (auto path : this->lookupPathsList)
            {
                SIRENInterpreter<> selector;

                if( !selector.parseString( path ) ) break;

                SIRENResultSet<INPUTNV> results;

                auto selected = selector.evaluate( given_node , results );

                for(size_t i = 0; i < selected; i++)
                {
                    INPUTNV node = results.adapted(i);

                    std::string selectModString = node.last_as_string();

                    if ( this->modifierFlag == "Abs"  &&
                       ( selectModString.at(0) == '-' ||
                         selectModString.at(0) == '+' ))
                    {
                        selectModString.erase(selectModString.begin());
                    }

                    size_t zeroIndex = selectModString.find_first_not_of('0');
                    if ( zeroIndex != 0 && zeroIndex != std::string::npos )
                    {
                        try
                        {
                            std::stoi(selectModString);
                            selectModString.erase(0, zeroIndex);
                        }
                        catch(...){}
                    }

                    if (selectModString == givenModString)
                    {
                        found_nodes.insert( node );
                    }
                }
            }

            return found_nodes.size() - original_size;
        }

    private:
        InputDefinition * thisID;
        std::string modifierFlag;
        std::vector<std::string> lookupPathsList;
        std::vector<std::string> constantsList;
        std::vector<std::string> enumRefNames;


};

class NotExistsInRule{

    public:
        NotExistsInRule(InputDefinition * thisID){
            this->setInputDefinition(thisID);
        }
        ~NotExistsInRule(){}

        void setInputDefinition(InputDefinition * thisID){
            this->thisID = thisID;
        }
        InputDefinition * getInputDefinition(){
            return this->thisID;
        }

        template<class SchemaAdapter>
        void streamErrorMessage(SchemaAdapter node, std::string errorMessage){
            this->getInputDefinition()->streamErrorMessage(node, errorMessage);
        }

        const std::vector<std::string> & getValidOptions(){
            return this->getInputDefinition()->getUniqueOptions();
        }
        bool isValidOption(const std::string& lookupString){
            const std::vector<std::string> & validOptions = this->getValidOptions();
            if (std::find(validOptions.begin(), validOptions.end(), lookupString) ==
                          validOptions.end()){
                return false;
            }
            else return true;
        }
        void setModifierFlag(const std::string& modifierflag){
            this->modifierFlag = modifierflag;
        }
        bool hasModifierFlag(){
            if (this->modifierFlag == "") return false;
            else return true;
        }
        std::string getModifierFlag(){
            return this->modifierFlag;
        }

        void addLookupPath(const std::string & lookuppath){
            this->lookupPathsList.push_back(lookuppath);
        }
        size_t getLookupPathCount(){
            return lookupPathsList.size();
        }
        std::string getLookupPathAt(size_t index){
            return lookupPathsList.at(index);
        }



    private:
        InputDefinition * thisID;
        std::string modifierFlag;
        std::vector<std::string> lookupPathsList;
        std::vector<std::string> constantsList;
        std::vector<std::string> enumRefNames;

};

class SumOverRule{

    public:
        SumOverRule(InputDefinition * thisID){
            this->setInputDefinition(thisID);
        }
        ~SumOverRule(){}

        void setInputDefinition(InputDefinition * thisID){
            this->thisID = thisID;
        }
        InputDefinition * getInputDefinition(){
            return this->thisID;
        }

        template<class SchemaAdapter>
        void streamErrorMessage(SchemaAdapter node, std::string errorMessage){
            this->getInputDefinition()->streamErrorMessage(node, errorMessage);
        }

        void setContext(const std::string& context){
            this->context = context;
        }
        void setSumOverValue(double sumovervalue){
            this->sumOverValue = sumovervalue;
        }
        std::string getContext(){
            return this->context;
        }
        double getSumOverValue(){
            return this->sumOverValue;
        }

    private:
        InputDefinition * thisID;
        std::string context;
        double sumOverValue;

};

class SumOverGroupRule{

    public:
        SumOverGroupRule(InputDefinition * thisID){
            this->setInputDefinition(thisID);
        }
        ~SumOverGroupRule(){}

        void setInputDefinition(InputDefinition * thisID){
            this->thisID = thisID;
        }
        InputDefinition * getInputDefinition(){
            return this->thisID;
        }

        template<class SchemaAdapter>
        void streamErrorMessage(SchemaAdapter node, std::string errorMessage){
            this->getInputDefinition()->streamErrorMessage(node, errorMessage);
        }

        void setContext(const std::string& context){
            this->context = context;
        }
        void setComparePathValue(const std::string& comparepathvalue){
            this->comparePathValue = comparepathvalue;
        }
        void setGroupDivideValue(double groupdividevalue){
            this->groupDivideValue = groupdividevalue;
        }
        void setGroupSumValue(double groupsumvalue){
            this->groupSumValue = groupsumvalue;
        }

        std::string getContext(){
            return this->context;
        }
        std::string getComparePathValue(){
            return this->comparePathValue;
        }
        double getGroupDivideValue(){
            return this->groupDivideValue;
        }
        double getGroupSumValue(){
            return this->groupSumValue;
        }

    private:
        InputDefinition * thisID;
        std::vector<std::string> lookupPathsList;
        std::string context;
        std::string comparePathValue;
        double groupDivideValue;
        double groupSumValue;

};

class IncreaseOverRule{

    public:
        IncreaseOverRule(InputDefinition * thisID){
            this->setInputDefinition(thisID);
        }
        ~IncreaseOverRule(){}

        void setInputDefinition(InputDefinition * thisID){
            this->thisID = thisID;
        }
        InputDefinition * getInputDefinition(){
            return this->thisID;
        }

        template<class SchemaAdapter>
        void streamErrorMessage(SchemaAdapter node, std::string errorMessage){
            this->getInputDefinition()->streamErrorMessage(node, errorMessage);
        }

        const std::vector<std::string> & getValidOptions(){
            return this->getInputDefinition()->getIncDecOptions();
        }
        bool isValidOption(const std::string& lookupString){
            const std::vector<std::string> & validOptions = this->getValidOptions();
            if (std::find(validOptions.begin(), validOptions.end(), lookupString) ==
                          validOptions.end()){
                return false;
            }
            else return true;
        }
        void setContext(const std::string& context){
            this->context = context;
        }
        void setType(const std::string & type){
            this->type = type;
        }

        std::string getContext(){
            return this->context;
        }
        std::string getType(){
            return this->type;
        }

    private:
        InputDefinition * thisID;
        std::string context;
        std::string type;

};

class DecreaseOverRule{

    public:
        DecreaseOverRule(InputDefinition * thisID){
            this->setInputDefinition(thisID);
        }
        ~DecreaseOverRule(){}

        void setInputDefinition(InputDefinition * thisID){
            this->thisID = thisID;
        }
        InputDefinition * getInputDefinition(){
            return this->thisID;
        }

        template<class SchemaAdapter>
        void streamErrorMessage(SchemaAdapter node, std::string errorMessage){
            this->getInputDefinition()->streamErrorMessage(node, errorMessage);
        }

        const std::vector<std::string> & getValidOptions(){
            return this->getInputDefinition()->getIncDecOptions();
        }
        bool isValidOption(const std::string& lookupString){
            const std::vector<std::string> & validOptions = this->getValidOptions();
            if (std::find(validOptions.begin(), validOptions.end(), lookupString) ==
                          validOptions.end()){
                return false;
            }
            else return true;
        }
        void setContext(const std::string& context){
            this->context = context;
        }
        void setType(const std::string & type){
            this->type = type;
        }

        std::string getContext(){
            return this->context;
        }
        std::string getType(){
            return this->type;
        }

    private:
        InputDefinition * thisID;
        std::string context;
        std::string type;

};

class ChildAtMostOneRule{

    public:
        ChildAtMostOneRule(InputDefinition * thisID){
            this->setInputDefinition(thisID);
        }
        ~ChildAtMostOneRule(){}

        void setInputDefinition(InputDefinition * thisID){
            this->thisID = thisID;
        }
        InputDefinition * getInputDefinition(){
            return this->thisID;
        }

        template<class SchemaAdapter>
        void streamErrorMessage(SchemaAdapter node, std::string errorMessage){
            this->getInputDefinition()->streamErrorMessage(node, errorMessage);
        }

        void addPathValuePair(std::pair <std::string,std::string> pathValuePair){
            this->pathValuePairList.push_back(pathValuePair);
        }
        size_t getPathValuePairCount(){
            return pathValuePairList.size();
        }
        std::pair <std::string,std::string> getPathValuePairAt(size_t index){
            return pathValuePairList.at(index);
        }

        template<class INPUTNV>
        bool isChildAllowed(const std::string & name, INPUTNV * node)
        {
            // if name is not in the list then allowed

            bool name_found = false;

            for( size_t i = 0 ; i < pathValuePairList.size() ; i++ )
            {
                if ( name == pathValuePairList[i].first ){
                    name_found = true;
                    break;
                }
            }

            if ( !name_found ) return true;

            for(const auto & path_value_pair : pathValuePairList)
            {
                const std::string & pair_path  = path_value_pair.first;
                const std::string & pair_value = path_value_pair.second;

                // ignore the path for the given name

                if( pair_path == name )
                {
                    continue;
                }

                // lookup other paths in the input

                SIRENInterpreter<> selector;

                if( !selector.parseString( pair_path ) ) continue;

                SIRENResultSet<INPUTNV> results;

                auto selected = selector.evaluate( *node , results );

                if( selected > 0 )
                {
                    // if found and no value provided for path - not allowed

                    if( pair_value == "" )
                    {
                        return false;
                    }

                    // if found and path value matches input value - not allowed

                    for( size_t i = 0 ; i < selected ; i++ )
                    {
                        INPUTNV result = results.adapted(i);

                        if ( result.last_as_string() == pair_value )
                        {
                            return false;
                        }
                    }
                }
            }

            // if none of the above was met - allowed

            return true;
        }

    private:
        InputDefinition * thisID;
        std::vector< std::pair <std::string,std::string> > pathValuePairList;

};

class ChildExactlyOneRule{

    public:
        ChildExactlyOneRule(InputDefinition * thisID){
            this->setInputDefinition(thisID);
        }
        ~ChildExactlyOneRule(){}

        void setInputDefinition(InputDefinition * thisID){
            this->thisID = thisID;
        }
        InputDefinition * getInputDefinition(){
            return this->thisID;
        }

        template<class SchemaAdapter>
        void streamErrorMessage(SchemaAdapter node, std::string errorMessage){
            this->getInputDefinition()->streamErrorMessage(node, errorMessage);
        }

        void addPathValuePair(std::pair <std::string,std::string> pathValuePair){
            this->pathValuePairList.push_back(pathValuePair);
        }
        size_t getPathValuePairCount(){
            return pathValuePairList.size();
        }
        std::pair <std::string,std::string> getPathValuePairAt(size_t index){
            return pathValuePairList.at(index);
        }

        template<class INPUTNV>
        bool isChildAllowed(const std::string & name, INPUTNV * node)
        {
            // if name is not in the list then allowed

            bool name_found = false;

            for( size_t i = 0 ; i < pathValuePairList.size() ; i++ )
            {
                if ( name == pathValuePairList[i].first ){
                    name_found = true;
                    break;
                }
            }

            if ( !name_found ) return true;

            for(const auto & path_value_pair : pathValuePairList)
            {
                const std::string & pair_path  = path_value_pair.first;
                const std::string & pair_value = path_value_pair.second;

                // ignore the path for the given name

                if( pair_path == name )
                {
                    continue;
                }

                // lookup other paths in the input

                SIRENInterpreter<> selector;

                if( !selector.parseString( pair_path ) ) continue;

                SIRENResultSet<INPUTNV> results;

                auto selected = selector.evaluate( *node , results );

                if( selected > 0 )
                {
                    // if found and no value provided for path - not allowed

                    if( pair_value == "" )
                    {
                        return false;
                    }

                    // if found and path value matches input value - not allowed

                    for( size_t i = 0 ; i < selected ; i++ )
                    {
                        INPUTNV result = results.adapted(i);

                        if ( result.last_as_string() == pair_value )
                        {
                            return false;
                        }
                    }
                }
            }

            // if none of the above was met - allowed

            return true;
        }

    private:
        InputDefinition * thisID;
        std::vector< std::pair <std::string,std::string> > pathValuePairList;

};

class ChildAtLeastOneRule{

    public:
        ChildAtLeastOneRule(InputDefinition * thisID){
            this->setInputDefinition(thisID);
        }
        ~ChildAtLeastOneRule(){}

        void setInputDefinition(InputDefinition * thisID){
            this->thisID = thisID;
        }
        InputDefinition * getInputDefinition(){
            return this->thisID;
        }

        template<class SchemaAdapter>
        void streamErrorMessage(SchemaAdapter node, std::string errorMessage){
            this->getInputDefinition()->streamErrorMessage(node, errorMessage);
        }

        void addPathValuePair(std::pair <std::string,std::string> pathValuePair){
            this->pathValuePairList.push_back(pathValuePair);
        }
        size_t getPathValuePairCount(){
            return pathValuePairList.size();
        }
        std::pair <std::string,std::string> getPathValuePairAt(size_t index){
            return pathValuePairList.at(index);
        }

    private:
        InputDefinition * thisID;
        std::vector< std::pair <std::string,std::string> > pathValuePairList;

};

class ChildCountEqualRule{

    public:
        ChildCountEqualRule(InputDefinition * thisID){
            this->setInputDefinition(thisID);
        }
        ~ChildCountEqualRule(){}

        void setInputDefinition(InputDefinition * thisID){
            this->thisID = thisID;
        }
        InputDefinition * getInputDefinition(){
            return this->thisID;
        }

        template<class SchemaAdapter>
        void streamErrorMessage(SchemaAdapter node, std::string errorMessage){
            this->getInputDefinition()->streamErrorMessage(node, errorMessage);
        }

        const std::vector<std::string> & getValidOptions(){
            return this->getInputDefinition()->getCCEOptions();
        }
        bool isValidOption(const std::string& lookupString){
            const std::vector<std::string> & validOptions = this->getValidOptions();
            if (std::find(validOptions.begin(), validOptions.end(), lookupString) ==
                          validOptions.end()){
                return false;
            }
            else return true;
        }
        void setModifierFlag(const std::string& modifierflag){
            this->modifierFlag = modifierflag;
        }
        bool hasModifierFlag(){
            if (this->modifierFlag == "") return false;
            else return true;
        }
        std::string getModifierFlag(){
            return this->modifierFlag;
        }

        void addLookupPath(const std::string & lookuppath){
            this->lookupPathsList.push_back(lookuppath);
        }
        size_t getLookupPathCount(){
            return lookupPathsList.size();
        }
        std::string getLookupPathAt(size_t index){
            return lookupPathsList.at(index);
        }

    private:
        InputDefinition * thisID;
        std::string modifierFlag;
        std::vector<std::string> lookupPathsList;

};

class ChildUniquenessRule{

    public:
        ChildUniquenessRule(InputDefinition * thisID){
            this->setInputDefinition(thisID);
        }
        ~ChildUniquenessRule(){}

        void setInputDefinition(InputDefinition * thisID){
            this->thisID = thisID;
        }
        InputDefinition * getInputDefinition(){
            return this->thisID;
        }

        template<class SchemaAdapter>
        void streamErrorMessage(SchemaAdapter node, std::string errorMessage){
            this->getInputDefinition()->streamErrorMessage(node, errorMessage);
        }

        const std::vector<std::string> & getValidOptions(){
            return this->getInputDefinition()->getUniqueOptions();
        }
        bool isValidOption(const std::string& lookupString){
            const std::vector<std::string> & validOptions = this->getValidOptions();
            if (std::find(validOptions.begin(), validOptions.end(), lookupString) ==
                          validOptions.end()){
                return false;
            }
            else return true;
        }
        void setModifierFlag(const std::string& modifierflag){
            this->modifierFlag = modifierflag;
        }
        bool hasModifierFlag(){
            if (this->modifierFlag == "") return false;
            else return true;
        }
        std::string getModifierFlag(){
            return this->modifierFlag;
        }

        void addLookupPath(const std::string & lookuppath){
            this->lookupPathsList.push_back(lookuppath);
        }
        size_t getLookupPathCount(){
            return lookupPathsList.size();
        }
        std::string getLookupPathAt(size_t index){
            return lookupPathsList.at(index);
        }

    private:
        InputDefinition * thisID;
        std::string modifierFlag;
        std::vector<std::string> lookupPathsList;

};
#include "wasphive/InputDefinition.i.h"
} // namespace wasp

#endif	/* WASP_INPUTDEFINITION_H */
