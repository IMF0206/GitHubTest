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
#include <iostream>
#include <sstream>

/* Suntec Header */
#ifndef VR_VOICEBOXAPPSXML_H
#    include "VR_VoiceBoxAppsXml.h"
#endif

#ifndef HEADER_PUGIXML_HPP
#    include "pugixml.hpp"
#endif

// Constructor
VR_VoiceBoxAppsXml::VR_VoiceBoxAppsXml()
{
}

// Destructor
VR_VoiceBoxAppsXml::~VR_VoiceBoxAppsXml()
{
}

bool
VR_VoiceBoxAppsXml::GetVBTXml(const std::string& xml,
    VoiceVector<std::string>::type& messages
)
{
    if (xml.empty()) {
        return false;
    }

    pugi::xml_document doc;
    if (!doc.load(xml.c_str())) {
        return false;
    }

    // Extract the tag:"Mesage" nodes
    bool bFound = false;
    pugi::xml_node csvrNodes = doc.child("event").child("CSVR");
    for (pugi::xml_node_iterator it = csvrNodes.begin(); it != csvrNodes.end(); ++it) {
        std::string strTag = it->name();
        if ("Message" == strTag) {
            std::ostringstream oss;
            it->print(oss);
            messages.push_back(oss.str());
            bFound = true;   
        }
    }
 
    return bFound;
}

std::string
VR_VoiceBoxAppsXml::GetVBTActionType(const std::string& message)
{
    std::string actionType;
    if (message.empty()) {
        return actionType;
    }

    pugi::xml_document doc;
    if (!doc.load(message.c_str())) {
        return actionType;
    }

    pugi::xml_node msgNodes = doc.child("Message");
    for (pugi::xml_node_iterator it = msgNodes.begin(); it != msgNodes.end(); ++it) {
        std::string strTag = it->name();
        if ("Action" == strTag) {
            actionType = it->attribute("type").value();
            return actionType;
        }
    }

    return actionType;
}

std::string
VR_VoiceBoxAppsXml::GetVBTCommand(const std::string& message)
{
    std::string command;
    if (message.empty()) {
        return command;
    }

    pugi::xml_document doc;
    if (!doc.load(message.c_str())) {
        return command;
    }

    pugi::xml_node paramNodes = doc.child("Message").child("Action").child("Execution");
    for (pugi::xml_node_iterator it = paramNodes.begin(); it != paramNodes.end(); ++it) {
        std::string strTag = it->attribute("name").value();
        if ("Command" == strTag) {
            command = it->attribute("value").value();
            return command;
        }
    }

    return command;
}

std::string
VR_VoiceBoxAppsXml::GetMessageClass(const std::string& message)
{
    std::string msgClass;
    if (message.empty()) {
        return msgClass;
    }

    pugi::xml_document doc;
    if (!doc.load(message.c_str())) {
        return msgClass;
    }

    pugi::xml_node msgNode = doc.child("Message");
    msgClass = msgNode.attribute("class").value();

    return msgClass;
}

std::string
VR_VoiceBoxAppsXml::GetVBTActionParamValue(const std::string& message, const std::string& paramName)
{
    std::string paramValue;
    if (message.empty()) {
        return paramValue;
    }

    pugi::xml_document doc;
    if (!doc.load(message.c_str())) {
        return paramValue;
    }

    pugi::xml_node msgNodes = doc.child("Message").child("Action");
    for (pugi::xml_node_iterator it = msgNodes.begin(); it != msgNodes.end(); ++it) {
        std::string strTag = it->attribute("name").value();
        if (paramName == strTag) {
            paramValue = it->attribute("value").value();
            return paramValue;
        }
    }

    return paramValue;
}

/* EOF */
