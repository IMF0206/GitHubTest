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

#include "VR_DataAccessorVoiceTag.h"
#include "VR_VoiceTagManager.h"
#include "VR_Log.h"

#include <sstream>


#define VR_MSG_RESPONSE_NODENAME                    "data"
#define VR_MSG_RESPONSE_NUMBER                      "count"
#define VR_MSG_RESPONSE_LIST                        "list"
#define VR_MSG_RESPONSE_LIST_ID                     "id"
#define VR_MSG_RESPONSE_LIST_ID_PREFIX              "list_"
#define VR_MSG_RESPONSE_LIST_HEADER                 "header"
#define VR_MSG_RESPONSE_LIST_HEADER_PAGESIZE        "pageSize"
#define VR_MSG_RESPONSE_LIST_HEADER_STARTINDEX      "startIndex"
#define VR_MSG_RESPONSE_LIST_HEADER_COUNT           "count"
#define VR_MSG_RESPONSE_LIST_ITEMS                  "items"
#define VR_MSG_RESPONSE_LIST_ITEMS_ITEM             "item"

#define VR_MSG_VOICETAG_INFO                            "voicetagInfo"
#define VR_MSG_VOICETAG_INFO_MULTI_TYPE                 "multiType"
#define VR_MSG_VOICETAG_INFO_MULTI_TYPE_TRUE            "true"
#define VR_MSG_VOICETAG_INFO_MULTI_TYPE_FALSE           "false"

#define VR_MSG_VOICETAG_INFO_CONTACT_PREFIX     "contact"
#define VR_MSG_VOICETAG_INFO_NAME_SUFFIX        "Name"
#define VR_MSG_VOICETAG_INFO_CANDIDATA_PREFIX   "candidata"
#define VR_MSG_VOICETAG_INFO_ID_SUFFIX          "Id"
#define VR_MSG_VOICETAG_INFO_TYPE_SUFFIX        "Type"
#define VR_MSG_VOICETAG_INFO_TYPE_NAME_SUFFIX   "TypeName"
#define VR_MSG_VOICETAG_INFO_NUMBER_SUFFIX      "Number"

#define VR_MSG_VOICETAG_EMPTY_DEFAULT   ""

#define VR_VOICETAG_CONTACT_NODE                "contact"
#define VR_VOICETAG_CONTACT_NODE_NAME           "name"
#define VR_VOICETAG_CONTACT_PHONE_ITEM          "phone_item"
#define VR_VOICETAG_CONTACT_PHONE_ITEM_TYPE     "type"
#define VR_VOICETAG_CONTACT_PHONE_ITEM_NUMBER   "number"

#define VR_VOICETAG_MAX_NAME_RESULT     3
#define VR_VOICETAG_MAX_DETAIL_RESULT   4

VR_DataAccessorVoiceTag::VR_DataAccessorVoiceTag(VR_VoiceTagQuerier *querier)
    : m_querier(querier)
{
    m_operationMap[VR_OPERATION_REQ_VOICETAG] = VoiceTagOperation::ReqVoiceTag;
    m_operationMap[VR_OPERATION_REQ_VOICETAG_DETAIL] = VoiceTagOperation::ReqVoiceTagDetail;
    m_operationMap[VR_OPERATION_GET_VOICETAG_NUMBER] = VoiceTagOperation::GetVoiceTagNumber;

    m_voiceTagInfoSampleDoc.load_string("<voicetagInfo>"
                                        "<multiType>false</multiType>"
                                        "<candidata1Type>0</candidata1Type>"
                                        "<candidata1TypeName>''</candidata1TypeName>"
                                        "<candidata1Number>''</candidata1Number>"
                                        "<candidata2Type>0</candidata2Type>"
                                        "<candidata2TypeName>''</candidata2TypeName>"
                                        "<candidata2Number>''</candidata2Number>"
                                        "<candidata3Type>0</candidata3Type>"
                                        "<candidata3TypeName>''</candidata3TypeName>"
                                        "<candidata3Number>''</candidata3Number>"
                                        "<candidata4Type>0</candidata4Type>"
                                        "<candidata4TypeName>''</candidata4TypeName>"
                                        "<candidata4Number>''</candidata4Number>"
                                      "</voicetagInfo>");
}

bool VR_DataAccessorVoiceTag::getInfo(const std::string &operation, const std::string &reqMsg, std::string &response)
{
    VR_LOGD_FUNC();
    VR_LOGD("Operation [%s]\nreqMsg [%s]", operation.c_str(), reqMsg.c_str());
    VoiceTagOperation op = VoiceTagOperation::None;
    if (m_operationMap.end() != m_operationMap.find(operation)) {
        op = m_operationMap[operation];
    }
    switch (op) {
    case VoiceTagOperation::ReqVoiceTag:
        {
            return reqVoiceTag(reqMsg, response);
        }
    case VoiceTagOperation::ReqVoiceTagDetail:
        {
            return reqVoiceTagDetail(reqMsg, response);
        }
    case VoiceTagOperation::GetVoiceTagNumber:
        {
            return getVoiceTagNumber(response);
        }
    default:
        {
            return false; // can not find the operation
        }
    }
}

bool VR_DataAccessorVoiceTag::isOperationHandled(const std::string &operation)
{
    return (m_operationMap.end() != m_operationMap.find(operation));
}

// void VR_DataAccessorVoiceTag::requestService(const std::string &agent, const std::string &reqName)
// {

// }

bool VR_DataAccessorVoiceTag::reqVoiceTag(const std::string &reqMsg, std::string &response)
{
    VR_LOGD_FUNC();

    // create response message
    pugi::xml_document responseDoc;
    responseDoc.load_string("");
    pugi::xml_node responseNode = responseDoc.append_child(VR_MSG_RESPONSE_NODENAME);
    responseNode.append_child(VR_MSG_RESPONSE_LIST).append_attribute(VR_MSG_RESPONSE_LIST_ID).set_value((std::string(VR_MSG_RESPONSE_LIST_ID_PREFIX) + VR_OPERATION_REQ_VOICETAG).c_str());
    responseNode.child(VR_MSG_RESPONSE_LIST).append_child(VR_MSG_RESPONSE_LIST_HEADER);
    responseNode.child(VR_MSG_RESPONSE_LIST).append_child(VR_MSG_RESPONSE_LIST_ITEMS);

    pugi::xml_node headerNode = responseNode.child(VR_MSG_RESPONSE_LIST).child(VR_MSG_RESPONSE_LIST_HEADER);
    headerNode.append_child(VR_MSG_RESPONSE_LIST_HEADER_PAGESIZE).text().set("3");
    headerNode.append_child(VR_MSG_RESPONSE_LIST_HEADER_STARTINDEX).text().set("0");
    headerNode.append_child(VR_MSG_RESPONSE_LIST_HEADER_COUNT).text().set(VR_MSG_VOICETAG_EMPTY_DEFAULT);

    pugi::xml_node itemsNode = responseNode.child(VR_MSG_RESPONSE_LIST).child(VR_MSG_RESPONSE_LIST_ITEMS);

    std::ostringstream oss;
    responseNode.print(oss);
    response = oss.str();
    oss.str("");

    // process request message
    VR_LOGD("reqMsg [%s]", reqMsg.c_str());
    pugi::xml_document msgDoc;
    msgDoc.load_string(reqMsg.c_str());
    pugi::xml_node reqItemsNode = msgDoc.select_node((std::string("//") + VR_MSG_RESPONSE_LIST_ITEMS).c_str()).node();
    pugi::xml_node reqItemNode = reqItemsNode.first_child();

    // process data
    int count = 0;

    while (!reqItemNode.empty() && (count < VR_VOICETAG_MAX_NAME_RESULT)) {
        pugi::xml_node currentItemNode = reqItemNode;
        reqItemNode = reqItemNode.next_sibling();

        std::string voiceTagID = currentItemNode.child("voice_tag_contact_id").text().as_string();
        std::string contactID = m_querier->getContactIDByVoiceTagID(voiceTagID);

        pugi::xml_document contactDoc;
        contactDoc.load_string(m_querier->getContactInfo(voiceTagID).c_str());

        if (contactDoc.first_child().empty()) {
            continue;
        }

        pugi::xml_node contactNode = contactDoc.select_node((std::string("//") + VR_VOICETAG_CONTACT_NODE).c_str()).node();
        std::string voiceTagName = contactNode.attribute(VR_VOICETAG_CONTACT_NODE_NAME).value();

        pugi::xml_node itemNode = itemsNode.append_child(VR_MSG_RESPONSE_LIST_ITEMS_ITEM);
        itemNode.append_copy(m_voiceTagInfoSampleDoc.select_node((std::string("//") + VR_MSG_VOICETAG_INFO).c_str()).node());
        pugi::xml_node voiceTagInfoNode = itemNode.child(VR_MSG_VOICETAG_INFO);
        pugi::xml_node multiTypeNode = voiceTagInfoNode.child(VR_MSG_VOICETAG_INFO_MULTI_TYPE);
        std::string countStr;
        oss << (count + 1);
        countStr = oss.str();
        oss.str("");
        std::string contactNameStr(std::string(VR_MSG_VOICETAG_INFO_CONTACT_PREFIX) + VR_MSG_VOICETAG_INFO_NAME_SUFFIX);
        voiceTagInfoNode.insert_child_before(contactNameStr.c_str(), multiTypeNode).text().set((voiceTagName.empty() ? VR_MSG_VOICETAG_EMPTY_DEFAULT : voiceTagName.c_str()));
        std::string contactIDStr(std::string(VR_MSG_VOICETAG_INFO_CONTACT_PREFIX) + countStr + VR_MSG_VOICETAG_INFO_ID_SUFFIX);
        voiceTagInfoNode.insert_child_before(contactIDStr.c_str(), multiTypeNode).text().set((contactID.empty() ? VR_MSG_VOICETAG_EMPTY_DEFAULT : contactID.c_str()));

        pugi::xml_node phoneItem = contactNode.first_child();
        int phoneItemIndex = 0;
        while (!phoneItem.empty() && (phoneItemIndex < VR_VOICETAG_MAX_DETAIL_RESULT)) {
            std::string typeID = phoneItem.attribute(VR_VOICETAG_CONTACT_PHONE_ITEM_TYPE).value();
            std::string number = phoneItem.attribute(VR_VOICETAG_CONTACT_PHONE_ITEM_NUMBER).value();

            std::string phoneItemIndexStr;
            oss << (phoneItemIndex + 1);
            phoneItemIndexStr = oss.str();
            oss.str("");

            if (!typeID.empty()) {
                std::string candidataTypeStr(std::string(VR_MSG_VOICETAG_INFO_CANDIDATA_PREFIX) + phoneItemIndexStr + VR_MSG_VOICETAG_INFO_TYPE_SUFFIX);
                voiceTagInfoNode.child(candidataTypeStr.c_str()).text().set(typeID.c_str());
                std::string candidataTypeNameStr(std::string(VR_MSG_VOICETAG_INFO_CANDIDATA_PREFIX) + phoneItemIndexStr + VR_MSG_VOICETAG_INFO_TYPE_NAME_SUFFIX);
                voiceTagInfoNode.child(candidataTypeNameStr.c_str()).text().set(getPhoneTypeName(typeID).c_str());
            }

            if (!number.empty()) {
                std::string candidataNumberStr(std::string(VR_MSG_VOICETAG_INFO_CANDIDATA_PREFIX) + phoneItemIndexStr + VR_MSG_VOICETAG_INFO_NUMBER_SUFFIX);
                voiceTagInfoNode.child(candidataNumberStr.c_str()).text().set(number.c_str());
            }

            phoneItem = phoneItem.next_sibling();
            ++phoneItemIndex;
        }

        if (1 < phoneItemIndex) {
            voiceTagInfoNode.child(VR_MSG_VOICETAG_INFO_MULTI_TYPE).text().set(VR_MSG_VOICETAG_INFO_MULTI_TYPE_TRUE);
        }

        ++count;
    }

    if (VR_VOICETAG_MAX_NAME_RESULT >= count) {
        oss << count;
        responseNode.select_node((std::string("//") + VR_MSG_RESPONSE_LIST_HEADER_COUNT).c_str()).node().text().set(oss.str().c_str());
        oss.str("");
    }

    responseNode.print(oss);
    response = oss.str();
    VR_LOGD("response msg is [%s]", response.c_str());

    return true;
}

bool VR_DataAccessorVoiceTag::reqVoiceTagDetail(const std::string &reqMsg, std::string &response)
{
    VR_LOGD_FUNC();

    // create response message
    pugi::xml_document responseDoc;
    responseDoc.load_string("");
    pugi::xml_node responseNode = responseDoc.append_child(VR_MSG_RESPONSE_NODENAME);
    responseNode.append_copy(m_voiceTagInfoSampleDoc.select_node((std::string("//") + VR_MSG_VOICETAG_INFO).c_str()).node());

    std::ostringstream oss;
    responseNode.print(oss);
    response = oss.str();
    oss.str("");

    pugi::xml_node voiceTagInfoNode = responseNode.child(VR_MSG_VOICETAG_INFO);
    pugi::xml_node multiTypeNode = voiceTagInfoNode.child(VR_MSG_VOICETAG_INFO_MULTI_TYPE);

    // process request message
    VR_LOGD("reqMsg [%s]", reqMsg.c_str());
    pugi::xml_document msgDoc;
    msgDoc.load_string(reqMsg.c_str());
    std::string contactNameStr(std::string(VR_MSG_VOICETAG_INFO_CONTACT_PREFIX) + VR_MSG_VOICETAG_INFO_NAME_SUFFIX);
    std::string contactIDStr(std::string(VR_MSG_VOICETAG_INFO_CONTACT_PREFIX) + VR_MSG_VOICETAG_INFO_ID_SUFFIX);
    std::string contactID = msgDoc.select_node((std::string("//") + contactIDStr).c_str()).node().text().as_string();
    std::string voiceTagID = m_querier->getVoiceTagIDByContactID(contactID);
    if (voiceTagID.empty()) {
        VR_ERROR("can not find voiceTagID by contactID[%s]", contactID.c_str());
        return false;
    }

    pugi::xml_document contactDoc;
    contactDoc.load_string(m_querier->getContactInfo(voiceTagID).c_str());

    if (contactDoc.first_child().empty()) {
        voiceTagInfoNode.insert_child_before(contactNameStr.c_str(), multiTypeNode).text().set(VR_MSG_VOICETAG_EMPTY_DEFAULT);
        return false;
    }

    pugi::xml_node contactNode = contactDoc.select_node((std::string("//") + VR_VOICETAG_CONTACT_NODE).c_str()).node();
    std::string contactName = contactNode.attribute(VR_VOICETAG_CONTACT_NODE_NAME).value();
    voiceTagInfoNode.insert_child_before(contactNameStr.c_str(), multiTypeNode).text().set(contactName.empty() ? VR_MSG_VOICETAG_EMPTY_DEFAULT : contactName.c_str());
    pugi::xml_node phoneItem = contactNode.first_child();
    int phoneItemIndex = 0;
    while (!phoneItem.empty() && (phoneItemIndex < VR_VOICETAG_MAX_DETAIL_RESULT)) {
        std::string typeID = phoneItem.attribute(VR_VOICETAG_CONTACT_PHONE_ITEM_TYPE).value();
        std::string number = phoneItem.attribute(VR_VOICETAG_CONTACT_PHONE_ITEM_NUMBER).value();

        std::string phoneItemIndexStr;
        oss << (phoneItemIndex + 1);
        phoneItemIndexStr = oss.str();
        oss.str("");

        if (!typeID.empty()) {
            std::string candidataTypeStr(std::string(VR_MSG_VOICETAG_INFO_CANDIDATA_PREFIX) + phoneItemIndexStr + VR_MSG_VOICETAG_INFO_TYPE_SUFFIX);
            voiceTagInfoNode.child(candidataTypeStr.c_str()).text().set(typeID.c_str());
            std::string candidataTypeNameStr(std::string(VR_MSG_VOICETAG_INFO_CANDIDATA_PREFIX) + phoneItemIndexStr + VR_MSG_VOICETAG_INFO_TYPE_NAME_SUFFIX);
            voiceTagInfoNode.child(candidataTypeNameStr.c_str()).text().set(getPhoneTypeName(typeID).c_str());
        }

        if (!number.empty()) {
            std::string candidataNumberStr(std::string(VR_MSG_VOICETAG_INFO_CANDIDATA_PREFIX) + phoneItemIndexStr + VR_MSG_VOICETAG_INFO_NUMBER_SUFFIX);
            voiceTagInfoNode.child(candidataNumberStr.c_str()).text().set(number.c_str());
        }

        phoneItem = phoneItem.next_sibling();
        ++phoneItemIndex;
    }

    if (1 < phoneItemIndex) {
        voiceTagInfoNode.child(VR_MSG_VOICETAG_INFO_MULTI_TYPE).text().set(VR_MSG_VOICETAG_INFO_MULTI_TYPE_TRUE);
    }

    responseNode.print(oss);
    response = oss.str();
    VR_LOGD("response msg is [%s]", response.c_str());

    return true;
}

bool VR_DataAccessorVoiceTag::getVoiceTagNumber(std::string &response)
{
    VR_LOGD_FUNC();
    // create response message
    pugi::xml_document responseDoc;
    responseDoc.load_string("");
    pugi::xml_node responseNode = responseDoc.append_child(VR_MSG_RESPONSE_NODENAME);
    pugi::xml_node numberNode = responseNode.append_child(VR_MSG_RESPONSE_NUMBER);

    std::ostringstream oss;
    int count = m_querier->getVoiceTagCount();
    oss << count;
    numberNode.text().set(oss.str().c_str());
    oss.str("");

    responseNode.print(oss);
    response = oss.str();
    VR_LOGD("response msg is [%s]", response.c_str());

    return true;
}

/* EOF */
