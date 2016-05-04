/**
 * Copyright @ 2014 - 2017 Suntec Software(Shanghai) Co., Ltd.
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

#include "VR_DataAccessorNavi.h"
#include "VR_VoiceIDConverter.h"
#include "VR_Log.h"
#include "VR_ConfigureIF.h"

#include <pugixml.hpp>
#include <sstream>
#include <fstream>
#include <string>


#define VR_MSG_SEND_VOICE_ID_TYPE    "type"
#define VR_MSG_SEND_VOICE_ID_VOICEID "voiceId"

VR_DataAccessorNavi::VR_DataAccessorNavi(VR_ConfigureIF* config)
    : VR_NaviAddressCommon(config)
{
    m_operationMap[VR_OPERATION_ADDRESS_MAPPING] = NaviOperation::AddressMapping;
    m_operationMap[VR_OPERATION_BUILD_ADDRESS] = NaviOperation::BuildAddress;
    m_operationMap[VR_OPERATION_SEND_VOICE_ID] = NaviOperation::SendVoiceID;
    m_operationMap[VR_OPERATION_BUILD_ADDRESS_NAME] = NaviOperation::BuildAddressName;
    m_operationMap[VR_OPERATION_GET_POI_PROMPT] = NaviOperation::GetPOIPrompt;
    m_operationMap[VR_OPERATION_BUILD_POINT_TYPE] = NaviOperation::BuildPointType;
}

VR_DataAccessorNavi::~VR_DataAccessorNavi()
{
}

bool VR_DataAccessorNavi::getInfo(const std::string &operation, const std::string &reqMsg, std::string &response)
{
    VR_LOGD_FUNC();
    VR_LOGD("Operation [%s]\nreqMsg [%s]", operation.c_str(), reqMsg.c_str());

    std::string processedReqMsg;
    processRequestMsg(reqMsg, processedReqMsg);
    VR_LOGD("processed reqMsg [%s]", processedReqMsg.c_str());

    NaviOperation op = NaviOperation::None;
    if (m_operationMap.end() != m_operationMap.find(operation)) {
        op = m_operationMap[operation];
    }
    switch (op) {
    case NaviOperation::AddressMapping:
        {
            return addressMapping(processedReqMsg, response);
        }
    case NaviOperation::BuildAddress:
        {
            return buildAddress(processedReqMsg, response);
        }
    case NaviOperation::SendVoiceID:
        {
            return sendVoiceID(processedReqMsg, response);
        }
    case NaviOperation::BuildAddressName:
        {
            return buildAddressName(reqMsg, response);
        }
    case NaviOperation::GetPOIPrompt:
        {
            return getPOIPrompt(reqMsg, response);
        }
    case NaviOperation::BuildPointType:
        {
            return buildPointType(reqMsg, response);
        }
    default:
        {
            return false; // can not find the operation
        }
    }
}

bool VR_DataAccessorNavi::isOperationHandled(const std::string &operation)
{
    return (m_operationMap.end() != m_operationMap.find(operation));
}

void VR_DataAccessorNavi::setCountryID(const std::string &countryID)
{
    VR_NaviAddressCommon::setCountryID(countryID);
}

// void VR_DataAccessorNavi::requestService(const std::string &agent, const std::string &reqName)
// {

// }

bool VR_DataAccessorNavi::sendVoiceID(const std::string &reqMsg, std::string &response)
{
    VR_LOGD_FUNC();

    // create response message
    pugi::xml_document responseDoc;
    responseDoc.load_string("");
    responseDoc.append_child(VR_MSG_RESPONSE_NODENAME);
    pugi::xml_node responseNode = responseDoc.child(VR_MSG_RESPONSE_NODENAME);
    responseNode.append_child(VR_MSG_SEND_VOICE_ID_TYPE);
    responseNode.append_child(VR_MSG_SEND_VOICE_ID_VOICEID);

    std::ostringstream oss;
    responseNode.print(oss);
    response = oss.str();
    oss.str("");

    // process request message
    VR_LOGD("reqMsg [%s]", reqMsg.c_str());
    pugi::xml_document msgDoc;
    msgDoc.load_string(reqMsg.c_str());
    std::string type = msgDoc.select_node((std::string("//") + VR_MSG_SEND_VOICE_ID_TYPE).c_str()).node().text().as_string();
    std::string searchIDNodeName;
    std::string nameIndexNodeName;
    unsigned long long int baseID = 0;
    std::string (VR_VoiceIDConverter::*funcPoint)(const std::string &);
    if (VR_MSG_NAVI_TYPE_CITY == type) {
        searchIDNodeName = VR_MSG_NAVI_ZONE_ID;
        nameIndexNodeName = VR_MSG_NAVI_ZONE_NAME_INDEX;
        baseID = 3000000;
        funcPoint = &VR_VoiceIDConverter::getZoneID;
    }
    else if (VR_MSG_NAVI_TYPE_STREET == type) {
        searchIDNodeName = VR_MSG_NAVI_CITY_ID;
        nameIndexNodeName = VR_MSG_NAVI_CITY_NAME_INDEX;
        baseID = 4000000;
        funcPoint = &VR_VoiceIDConverter::getCityID;
    }
    else {
        VR_ERROR("unhandle type to sendVoiceID");
        return false;
    }
    std::string searchID = msgDoc.select_node((std::string("//") + searchIDNodeName).c_str()).node().text().as_string();
    std::string nameIndex = msgDoc.select_node((std::string("//") + nameIndexNodeName).c_str()).node().text().as_string();
    if ("''" == nameIndex) {
        nameIndex = "0";
    }
    std::string globalVoiceID;
    processReceivedSearchID(searchID, nameIndex);
    if (!getGlobalVoiceID(searchID, globalVoiceID)) {
        return false;
    }
    VR_VoiceIDConverter converter;
    std::string voiceIDStr;
    unsigned long long int voiceID = 0;
    voiceIDStr = (converter.*funcPoint)(globalVoiceID);
    std::stringstream ss;
    VR_LOGD("VoiceID:[%s]", voiceIDStr.c_str());
    ss << voiceIDStr;
    ss >> voiceID;
    ss.clear();
    ss.str("");
    voiceID += baseID;
    ss << voiceID;
    ss >> voiceIDStr;
    VR_LOGD("VoiceID:[%s]", voiceIDStr.c_str());
    responseNode.child(VR_MSG_SEND_VOICE_ID_TYPE).text().set(type.c_str());
    responseNode.child(VR_MSG_SEND_VOICE_ID_VOICEID).text().set(voiceIDStr.c_str());

    responseNode.print(oss);
    response = oss.str();
    return true;
}

void VR_DataAccessorNavi::processRequestMsg(const std::string &inputMsg, std::string &outputMsg)
{
    pugi::xml_document msgDoc;
    msgDoc.load_string(inputMsg.c_str());
    std::stringstream ss;

    pugi::xpath_node_set zoneNodes = msgDoc.select_nodes("//CMN2031_navi_addr_state_x");
    for (pugi::xpath_node_set::iterator it = zoneNodes.begin(); it != zoneNodes.end(); ++it) {
        it->node().set_name(VR_MSG_NAVI_ZONE_ID);
        std::string idStr = it->node().text().as_string();
        if (idStr.empty()) {
            continue;
        }
        long long int id;
        ss << idStr;
        ss >> id;
        id = id - 3000000;
        ss.clear();
        ss.str("");
        ss << id;
        it->node().text().set(ss.str().c_str());
        ss.clear();
        ss.str("");
    }

    pugi::xpath_node_set cityNodes = msgDoc.select_nodes("//CMN2032_navi_addr_city_x");
    for (pugi::xpath_node_set::iterator it = cityNodes.begin(); it != cityNodes.end(); ++it) {
        it->node().set_name(VR_MSG_NAVI_CITY_ID);
        std::string idStr = it->node().text().as_string();
        if (idStr.empty()) {
            continue;
        }
        long long int id;
        ss << idStr;
        ss >> id;
        id = id - 4000000;
        ss.clear();
        ss.str("");
        ss << id;
        it->node().text().set(ss.str().c_str());
        ss.clear();
        ss.str("");
    }

    pugi::xpath_node_set streetNodes = msgDoc.select_nodes("//CMN2033_navi_addr_street_x");
    for (pugi::xpath_node_set::iterator it = streetNodes.begin(); it != streetNodes.end(); ++it) {
        it->node().set_name(VR_MSG_NAVI_STREET_ID);
        std::string idStr = it->node().text().as_string();
        if (idStr.empty()) {
            continue;
        }
        long long int id;
        ss << idStr;
        ss >> id;
        id = id - 8000000;
        ss.clear();
        ss.str("");
        ss << id;
        it->node().text().set(ss.str().c_str());
        ss.clear();
        ss.str("");
    }

    // treat both number and VR_MSG_NAVI_HOUSE_NUMBER(houseNumber) as house number node for ASR Data
    pugi::xpath_node_set houseNumberNodes = msgDoc.select_nodes("//number");
    for (pugi::xpath_node_set::iterator it = houseNumberNodes.begin(); it != houseNumberNodes.end(); ++it) {
        it->node().set_name(VR_MSG_NAVI_HOUSE_NUMBER);
    }

    msgDoc.print(ss);
    outputMsg = ss.str();
}

/* EOF */
