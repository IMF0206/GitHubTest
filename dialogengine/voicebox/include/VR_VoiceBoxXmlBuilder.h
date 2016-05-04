/**
 * Copyright @ 2015 - 2016 Suntec Software(Shanghai) Co., Ltd.
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are NOT permitted except as agreed by
 * Suntec Software(Shanghai) Co., Ltd.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */

/**
 * @file VR_VoiceBoxXmlBuilder.h
 * @brief interface for EngineWrapper callback.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VOICEBOXXMLBUILDER_H
#define VR_VOICEBOXXMLBUILDER_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

/* Standard Header */
#include <string>
#include <iostream>

/* Suntec Header */
#ifndef MEM_VECTOR_H
#    include "MEM_vector.h"
#endif

#ifndef HEADER_PUGIXML_HPP
#    include "pugixml.hpp"
#endif

#ifndef VR_VOICEBOXXMLPARSER_H
#    include "VR_VoiceBoxXmlParser.h"
#endif

struct xml_string_writer : pugi::xml_writer
{
    std::string result;
    virtual void write(const void* data, size_t size)
    {
        result += std::string(static_cast<const char*>(data), size);
    }
};

// Describe the state of information
struct NodeInfo
{
    std::string parameterName;
    std::string parameterValue;
};

/**
 * @brief The VR_VoiceBoxXmlBuilder class
 *
 * class declaration
 */
class VR_VoiceBoxXmlBuilder
{
public:
    VR_VoiceBoxXmlBuilder();
    // message
    virtual ~VR_VoiceBoxXmlBuilder();

    /*
     * build the node of Message
     *
     * we can build the node of message by the value of source, class, ref
     *
     * @param strSource : the value of source
     * @param strClass : the value of class
     * @param strRef : the value of ref
     * @return the child'node of message
     */

    pugi::xml_node buildStartMessageElement(const std::string &strSource, const std::string &strClass,
        const std::string &strRef);

    /*
     * build the node of Action
     *
     * @param node : the node of father
     * @param strType : the type of action
     * @return the child'node of action
     */

    pugi::xml_node buildMessageActionElement(pugi::xml_node node, const std::string &strType);

    /*
     * build the node of parameter
     *
     * @param node : the node of father
     * @param name : the name of parameter
     * @param value : the value of parameter
     * @return the child's node of parameter
     */

    pugi::xml_node buildMsgParameterElement(pugi::xml_node node, const std::string &name,
        const std::string &value);

    /*
     * build the node of list's parameter
     *
     * @param node : the node of father
     * @param parameter : the information of parameter
     */

    void buildMsgParameterList(pugi::xml_node node, VoiceVector<Parameter>::type parameter);
    // action

    /*
     * build the node of action-result
     *
     * @param strAgent : the value of agent
     * @param strOp : the value of op
     * @param strErrCode : the value of strErrCode
     * @return the child's node of action-result
     */

    pugi::xml_node buildStartActionResultElement(const std::string &strAgent, const std::string &strOp,
        const std::string &strErrCode);

    /*
     * build the node of action-result
     *
     * @param strAgent : the value of agent
     * @param strOp : the value of op
     * @return the child's node of action-result
     */

    pugi::xml_node buildStartActionElement(const std::string &strAgent, const std::string &strOp);

    /*
     * build the node of display
     *
     * @param strAgent : the value of agent
     * @param strContent : the value of strContent
     * @return the child's node of display
     */

    pugi::xml_node buildDisplayElement(const std::string &strAgent, const std::string &strContent);

    /*
     * build the node of system-result
     *
     * @param element : the value of element
     * @param strErrCode : the value of strErrCode
     * @return the child's node of system-result
     */

    pugi::xml_node buildSystemResultElement(const std::string &element, const std::string &strErrCode);

     /*
     * build the node of given element
     *
     * @param node : the node of father
     * @param strElement : the name element
     * @param strValue : the name of value
     * @param strAttribute : the name of attribute
     * @param strAttributeValue : the value of attribute
     * @return the child's node of given element
     */

    pugi::xml_node buildGivenElement(pugi::xml_node node, const std::string &strElement, const std::string &strValue,
        const std::string &strAttribute, const std::string &strAttributeValue);

    /*
     * build the node of given element
     *
     * @param node : the node of father
     * @param strElement : the name element
     * @param strValue : the name of value
     * @param attributeVector : the group of attribute
     * @return the child's node of given element
     */

    pugi::xml_node buildGivenElement(pugi::xml_node node, const std::string &strElement, const std::string &strValue,
        VoiceVector<StructNode>::type attributeVector);

    /*
     * build the node of given element
     *
     * @param node : the node of father
     * @param strElement : the name element
     * @param attributeVector : the group of attribute
     * @return the child's node of given element
     */

    pugi::xml_node buildGivenElement(const std::string &strElement, VoiceVector<StructNode>::type attributeVector);


    /*
     * build the node of item
     *
     * @param node : the node of father
     * @param strListItem : the name of item
     * @param strItem : the value of item
     * @return the child's node of item
     */

    pugi::xml_node buildListItemElement(pugi::xml_node node, const std::string &strListItem, VoiceVector<std::string>::type strItem);

    /*
     * build the node of item's child
     *
     * @param node : the node of father
     * @param strElementType : the type of element
     * @param nodeString : the group of string
     * @param m_vector : the group of node
     * return the child's node of item's child
     */

    void buildListItemChildElement(pugi::xml_node node, const std::string &strElementType,
        const std::string* nodeString, VoiceVector<StructNode>::type m_vector);

    /*
     * build the node of display
     *
     * @param strAgent : the value of agent
     * @param strContent : the value of strContent
     * @return the child's node of display
     */

    pugi::xml_node buildStartDisplayElement(const std::string &strAgent, const std::string &strContent);
    // get the string format of building xml
    std::string getXmlString();
    
private:
    // build the xml
    pugi::xml_document m_xml_document;
    pugi::xml_document m_docTargetXML;
};

#endif // VR_VOICEBOXXMLBUILDER_H
/* EOF */
