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

/* Standard Header */
#include <cstring>

/* Suntec Header */
#ifndef VR_VOICEBOXXMLBUILDER_H
#    include "VR_VoiceBoxXmlBuilder.h"
#endif

// Constructor
VR_VoiceBoxXmlBuilder::VR_VoiceBoxXmlBuilder()
{
}

// Destructor
VR_VoiceBoxXmlBuilder::~VR_VoiceBoxXmlBuilder()
{
}

/**
* build the start tag of a Message element
*/

pugi::xml_node VR_VoiceBoxXmlBuilder::buildStartMessageElement(const std::string &strSource, const std::string &strClass,
    const std::string &strRef)
{
    pugi::xml_node root_node = m_docTargetXML.append_child("Message");
    root_node.append_attribute("source").set_value(strSource.c_str());
    root_node.append_attribute("class").set_value(strClass.c_str());
    if (0 != strcmp("", strRef.c_str())) {
        root_node.append_attribute("ref").set_value(strRef.c_str());
    }
    return root_node;
}

/**
* build the start tag of a action result  element
*/

pugi::xml_node VR_VoiceBoxXmlBuilder::buildStartActionResultElement(const std::string &strAgent, const std::string &strOp,
    const std::string &strErrCode)
{
    pugi::xml_node root_node = m_docTargetXML.append_child("action-result");
    root_node.append_attribute("agent").set_value(strAgent.c_str());
    root_node.append_attribute("op").set_value(strOp.c_str());
    if (0 != strcmp("", strErrCode.c_str())) {
        root_node.append_attribute("errcode").set_value(strErrCode.c_str());
    }
    return root_node;
}


/**
* build the start tag of a action  element
*/

pugi::xml_node VR_VoiceBoxXmlBuilder::buildStartActionElement(const std::string &strAgent, const std::string &strOp)
{
    pugi::xml_node root_node = m_docTargetXML.append_child("action");
    root_node.append_attribute("agent").set_value(strAgent.c_str());
    root_node.append_attribute("op").set_value(strOp.c_str());
    return root_node;
}

/**
*build the start tag of a display element
*/

pugi::xml_node VR_VoiceBoxXmlBuilder::buildDisplayElement(const std::string &strAgent, const std::string &strContent)
{
    pugi::xml_node root_node = m_docTargetXML.append_child("display");
    root_node.append_attribute("agent").set_value(strAgent.c_str());
    root_node.append_attribute("content").set_value(strContent.c_str());
    return root_node;
}


/**
* build the start tag of a system element
*/

pugi::xml_node VR_VoiceBoxXmlBuilder::buildSystemResultElement(const std::string &element, const std::string &strErrCode)
{
    pugi::xml_node root_node = m_docTargetXML.append_child(element.c_str());
    root_node.append_attribute("errcode").set_value(strErrCode.c_str());
    return root_node;
}

/**
* build the start tag of a display element
*/

pugi::xml_node VR_VoiceBoxXmlBuilder::buildStartDisplayElement(const std::string &strAgent, const std::string &strContent)
{
    pugi::xml_node root_node = m_docTargetXML.append_child("display");
    root_node.append_attribute("agent").set_value(strAgent.c_str());
    root_node.append_attribute("content").set_value(strContent.c_str());
    return root_node;
}

/**
* build the start tag of a given element
*/

pugi::xml_node VR_VoiceBoxXmlBuilder::buildGivenElement(pugi::xml_node node, const std::string &strElement,
    const std::string &strValue, const std::string &strAttribute, const std::string &strAttributeValue)
{
    pugi::xml_node givenNode = node.append_child(strElement.c_str());
    if (!strValue.empty()) {
        givenNode.append_child(pugi::node_pcdata).set_value(strValue.c_str());
    }
    if (!strAttribute.empty()) {
        givenNode.append_attribute(strAttribute.c_str()).set_value(strAttributeValue.c_str());
    }
    return givenNode;
}

/**
* build the start tag of a given element
*/

pugi::xml_node VR_VoiceBoxXmlBuilder::buildGivenElement(pugi::xml_node node, const std::string &strElement,
    const std::string &strValue, VoiceVector<StructNode>::type attributeVector)
{
    pugi::xml_node givenNode = node.append_child(strElement.c_str());
    if (0 != strcmp("", strValue.c_str())) {
        givenNode.append_child(pugi::node_pcdata).set_value(strValue.c_str());
    }
    VoiceVector<StructNode>::iterator iteratorValue;
    for (iteratorValue = attributeVector.begin(); iteratorValue != attributeVector.end(); ++iteratorValue) {
        if (0 != strcmp("", (*iteratorValue).Name.c_str())) {
            givenNode.append_attribute((*iteratorValue).Name.c_str()).set_value((*iteratorValue).Value.c_str());
        }
    }
    return givenNode;
}

/**
* build the start tag of a given element
*/

pugi::xml_node VR_VoiceBoxXmlBuilder::buildGivenElement(const std::string &strElement, VoiceVector<StructNode>::type attributeVector)
{
    pugi::xml_node root_node = m_docTargetXML.append_child(strElement.c_str());
    if (attributeVector.empty()) {
        return root_node;
    }

    VoiceVector<StructNode>::iterator iteratorValue;
    for (iteratorValue = attributeVector.begin(); iteratorValue != attributeVector.end(); ++iteratorValue) {
        if (0 != strcmp("", (*iteratorValue).Name.c_str())) {
            root_node.append_attribute((*iteratorValue).Name.c_str()).set_value((*iteratorValue).Value.c_str());
        }
    }

    return root_node;
}

/**
 * build the node of item list
 */

pugi::xml_node VR_VoiceBoxXmlBuilder::buildListItemElement(pugi::xml_node node, const std::string &strListItem,
    VoiceVector<std::string>::type strItem)
{
    if (strItem.empty()) {
        return node;
    }
    pugi::xml_node itemNode;
    VoiceVector<std::string>::iterator itemIterator;
    std::string itemValue = "";
    for (itemIterator = strItem.begin(); itemIterator != strItem.end(); ++itemIterator) {
        itemValue = *itemIterator;
        itemNode = node.append_child(strListItem.c_str());
        itemNode.append_child(pugi::node_pcdata).set_value(itemValue.c_str());
    }
    return itemNode;
}

/**
 * build the node of item child element
 */

void VR_VoiceBoxXmlBuilder::buildListItemChildElement(pugi::xml_node node, const std::string &strElementType, const std::string* nodeString, VoiceVector<StructNode>::type m_vector)
{
    pugi::xml_node itemNode;
    pugi::xml_node itemChildNode;
    pugi::xml_node itemchildValueNode;
    itemNode = node.append_child("item");
    if (!strElementType.empty()) {
        itemChildNode = itemNode.append_child(strElementType.c_str());
    }
    
    std::string name = "";
    std::string value = "";
    size_t count = 0;
    VoiceVector<StructNode>::iterator iteratorValue;
    for (iteratorValue = m_vector.begin(); iteratorValue != m_vector.end(); ++iteratorValue, ++count) {
        name = (*iteratorValue).Name;
        value = (*iteratorValue).Value;
        while ("" == nodeString[count]) {
            count = 0;
            itemNode = node.append_child("item");
            if (!strElementType.empty()) {
                itemChildNode = itemNode.append_child(strElementType.c_str());
            }
        }
        if (itemChildNode) {
            itemchildValueNode = itemChildNode.append_child(name.c_str());
        }
        else {
            itemchildValueNode = itemNode.append_child(name.c_str());
        }
        itemchildValueNode.append_child(pugi::node_pcdata).set_value(value.c_str());
    }
}

/**
* build the start tag of a Message Parameter element
*/

pugi::xml_node VR_VoiceBoxXmlBuilder::buildMsgParameterElement(pugi::xml_node node, const std::string &name,
        const std::string &value)
{
    pugi::xml_node ParameterNode = node.append_child("Parameter");
    ParameterNode.append_attribute("name").set_value(name.c_str());
    ParameterNode.append_attribute("value").set_value(value.c_str());
    return ParameterNode;
}

/**
* build the start tag of a Message action element
*/

pugi::xml_node VR_VoiceBoxXmlBuilder::buildMessageActionElement(pugi::xml_node node, const std::string &strType)
{
    pugi::xml_node actionNode = node.append_child("Action");
    actionNode.append_attribute("type").set_value(strType.c_str());
    return actionNode;
}

/**
 * build the node of Parameter list
 */

void VR_VoiceBoxXmlBuilder::buildMsgParameterList(pugi::xml_node node, VoiceVector<Parameter>::type parameter)
{
    if (parameter.empty()) {
        return;
    }
    pugi::xml_node parameterNode;
    VoiceVector<Parameter>::iterator parameterIterator;
    std::string nameValue = "";
    std::string value = "";
    for (parameterIterator = parameter.begin(); parameterIterator != parameter.end(); ++parameterIterator) {
        nameValue = (*parameterIterator).nameValue;
        value = (*parameterIterator).value;
        parameterNode = node.append_child("Parameter");
        parameterNode.append_attribute("name").set_value(nameValue.c_str());
        parameterNode.append_attribute("value").set_value(value.c_str());    
    }
}

/**
 * get the string xml
 */

std::string VR_VoiceBoxXmlBuilder::getXmlString()
{
    pugi::xml_node rootNode = m_docTargetXML.first_child();
    xml_string_writer writer;
    if (rootNode) {
        m_docTargetXML.save(writer);
    }
    return writer.result;
}
/* EOF */
