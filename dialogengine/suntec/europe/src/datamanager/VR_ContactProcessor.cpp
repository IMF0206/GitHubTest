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

#include "VR_ContactProcessor.h"
#include "VR_Log.h"

#include "MEM_list.h"
#include "MEM_set.h"

#include <sqlite3.h>
#include <pugixml.hpp>
#include <sstream>

#define VR_MSG_PHONE_INFO                           "phoneInfo"
#define VR_MSG_PHONE_INFO_CONTACT_ID                "contactId"
#define VR_MSG_PHONE_INFO_CONTACT_NAME              "contactName"
#define VR_MSG_PHONE_INFO_PHONETYPE_ID              "phoneType"
#define VR_MSG_PHONE_INFO_PHONETYPE_NAME            "phoneTypeName"
#define VR_MSG_PHONE_INFO_LIST_TYPE                 "listType"
#define VR_MSG_PHONE_INFO_NUMBER                    "number"
#define VR_MSG_PHONE_INFO_QUERYTYPE                 "queryType"
#define VR_MSG_PHONE_INFO_QUERYTYPE_NAMEANDTYPE     "NAME_AND_TYPE"

#define VR_MSG_BUILD_INFO_PHONE_LIST                "phoneList"
#define VR_MSG_BUILD_INFO_SCREEN_LIST               "screenList"


#define VR_MSG_IS_IN_PHONEBOOK                      "isInPhonebook"
#define VR_MSG_IS_IN_PHONEBOOK_TRUE                 "true"
#define VR_MSG_IS_IN_PHONEBOOK_FALSE                "false"

#define VR_MSG_RESPONSE_NODENAME                            "data"
#define VR_MSG_RESPONSE_PHONE_INFO_RESULT                   "result"
#define VR_MSG_RESPONSE_PHONE_INFO_RESULT_FOUND             "TYPE_FOUND"
#define VR_MSG_RESPONSE_PHONE_INFO_RESULT_NOT_FOUND         "NO_TYPE_FOUND"
#define VR_MSG_RESPONSE_PHONE_INFO_LIST                     "list"
#define VR_MSG_RESPONSE_PHONE_INFO_LIST_ID                  "id"
#define VR_MSG_RESPONSE_PHONE_INFO_LIST_ID_PREFIX           "list_"
#define VR_MSG_RESPONSE_PHONE_INFO_LIST_HEADER              "header"
#define VR_MSG_RESPONSE_PHONE_INFO_LIST_HEADER_PAGESIZE     "pageSize"
#define VR_MSG_RESPONSE_PHONE_INFO_LIST_HEADER_STARTINDEX   "startIndex"
#define VR_MSG_RESPONSE_PHONE_INFO_LIST_HEADER_COUNT        "count"
#define VR_MSG_RESPONSE_PHONE_INFO_LIST_ITEMS               "items"
#define VR_MSG_RESPONSE_PHONE_INFO_LIST_ITEMS_ITEM          "item"

#define VR_MSG_RESPONSE_PHONE_INFO_LIST_ITEMS_TEMPNODE_NAME "fullname"
#define VR_MSG_RESPONSE_PHONE_INFO_LIST_ITEMS_TEMPNODE_ID   "contactId"

VR_ContactProcessor::VR_ContactProcessor()
    : m_dbHandler(nullptr)
    , m_isContactInPhonebook(false)
{
    m_operationMap[VR_OPERATION_CHECK_CONTACT_IN_PHONEBOOK] = ContactOperation::CheckContactInPhonebook;
    m_operationMap[VR_OPERATION_GET_PHONE_INFO] = ContactOperation::GetPhoneInfo;
    m_operationMap[VR_OPERATION_BUILD_INFO_LIST] = ContactOperation::BuildInfoList;
    m_operationMap[VR_OPERATION_GET_INFO_BY_TYPE] = ContactOperation::GetInfoByType;
    m_operationMap[VR_OPERATION_GET_CONTACT_IDS] = ContactOperation::GetContactIds;
}

bool VR_ContactProcessor::getInfo(const std::string &operation, const std::string &reqMsg, std::string &response)
{
    VR_LOGD_FUNC();
    VR_LOGD("Operation [%s]\nreqMsg [%s]", operation.c_str(), reqMsg.c_str());
    ContactOperation op = ContactOperation::None;
    if (m_operationMap.end() != m_operationMap.find(operation)) {
        op = m_operationMap[operation];
    }
    switch (op) {
    case ContactOperation::CheckContactInPhonebook:
        {
            return checkContactInPhonebook(reqMsg, response);
        }
    case ContactOperation::GetPhoneInfo:
        {
            return getPhoneInfo(reqMsg, response);
        }
    case ContactOperation::BuildInfoList:
        {
            return buildInfoList(reqMsg, response);
        }
    case ContactOperation::GetInfoByType:
        {
            return getInfoByType(reqMsg, response);
        }
    case ContactOperation::GetContactIds:
        {
            return getContactIds(reqMsg, response);
        }
    default:
        {
            return false;
        }
    }
}

bool VR_ContactProcessor::isOperationHandled(const std::string &operation)
{
    return (m_operationMap.end() != m_operationMap.find(operation));
}

void VR_ContactProcessor::openDB(const std::string &path)
{
    m_dbResult = sqlite3_open_v2(path.c_str(), &m_dbHandler, SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX, NULL);

    if (SQLITE_OK != m_dbResult) {
        closeDB();
        VR_ERROR("Open Musice DB file %s failed, resultID: [%d]", path.c_str(), m_dbResult);
    }
}

void VR_ContactProcessor::closeDB()
{
    sqlite3_close(m_dbHandler);
    m_dbResult = SQLITE_ERROR;
    m_dbHandler = nullptr;
}

// void VR_ContactProcessor::requestService(const std::string &agent, const std::string &reqName)
// {

// }

bool VR_ContactProcessor::checkContactInPhonebook(const std::string &reqMsg, std::string &response)
{
    VR_LOGD_FUNC();

    // create response message
    pugi::xml_document responseDoc;
    responseDoc.load_string("");
    responseDoc.append_child(VR_MSG_RESPONSE_NODENAME);
    pugi::xml_node responseNode = responseDoc.child(VR_MSG_RESPONSE_NODENAME);
    responseNode.append_child(VR_MSG_IS_IN_PHONEBOOK);

    std::ostringstream oss;
    responseNode.print(oss);
    response = oss.str();
    oss.str("");

    if (SQLITE_OK != m_dbResult) {
        return false;
    }

    // process request message
    pugi::xml_document msgDoc;
    msgDoc.load_string(reqMsg.c_str());
    pugi::xml_node phoneinfoNode = msgDoc.select_node((std::string("//") + VR_MSG_PHONE_INFO).c_str()).node();
    if (std::string(VR_MSG_PHONE_INFO) != phoneinfoNode.name()) {
        VR_ERROR("msg node is %s not phoneinfo, please check.", phoneinfoNode.name());
        return false;
    }

    phoneinfoNode = responseNode.append_copy(phoneinfoNode);
    responseNode.print(oss);
    response = oss.str();
    oss.str("");

    std::string contactID = phoneinfoNode.child(VR_MSG_PHONE_INFO_CONTACT_ID).text().as_string();
    std::string phonetypeID = phoneinfoNode.child(VR_MSG_PHONE_INFO_PHONETYPE_ID).text().as_string();
    std::string number = phoneinfoNode.child(VR_MSG_PHONE_INFO_NUMBER).text().as_string();

    pugi::xml_document resultDoc;
    resultDoc.load_string("");
    pugi::xml_node resultNode = resultDoc.append_child("resultNode");
    if ("0" != contactID) {
        // query database
        bool status = queryPhonebookDatabase(contactID,phonetypeID,number);
        if(!status)
        {
            return status;
        }
        // std::string sqlRequest;
        // if (!contactID.empty()) {
        //     sqlRequest = sqlRequest + "SELECT contact.id,contact.full,phonerecords.typeid,phonerecords.indexid,phonerecords.number FROM phonerecords,contact WHERE contact.id=phonerecords.id AND contact.id=\"" + contactID + "\" ";
        // }
        // if (!phonetypeID.empty()) {
        //     int totalTypeID = atoi(phonetypeID.c_str());
        //     int typeID = totalTypeID % 4;
        //     int indexID = totalTypeID / 4;
        //     std::ostringstream oss;
        //     oss << typeID;
        //     std::string typeIDStr = oss.str();
        //     oss.str("");
        //     oss << indexID;
        //     std::string indexIDStr = oss.str();
        //     if (0 == indexID || 1 == indexID) {
        //         indexIDStr = "0,1";
        //     }
        //     sqlRequest.append("INTERSECT SELECT contact.id,contact.full,phonerecords.typeid,phonerecords.indexid,phonerecords.number FROM phonerecords,contact WHERE contact.id=phonerecords.id AND phonerecords.typeid=\"").append(typeIDStr).append("\" AND phonerecords.indexid IN (").append(indexIDStr).append(") ");
        // }
        // if (!number.empty()) {
        //     sqlRequest = sqlRequest + "INTERSECT SELECT contact.id,contact.full,phonerecords.typeid,phonerecords.indexid,phonerecords.number FROM phonerecords,contact WHERE contact.id=phonerecords.id AND number=\"" + number + "\"";
        // }
        // if (sqlRequest.empty()) {
        //     VR_ERROR("phoneinfo is empty, can not query.");
        //     return false;
        // }
        // if (std::size_t(0) == sqlRequest.find("INTERSECT")) {
        //     sqlRequest.erase(0, 10);
        // }
        // char * errorMsg = NULL;
        // int result = sqlite3_exec(m_dbHandler, sqlRequest.c_str(), checkContactInPhonebookCallback, &resultNode, &errorMsg);
        // if (SQLITE_OK != result) {
        //     VR_ERROR("Run SQL request [%s] failed, error code: [%d], error msg: [%s]", sqlRequest.c_str(), result, errorMsg);
        //     return false;
        //}
    }

    // process data
    if (!resultNode.first_child().empty()) {
        responseNode.child(VR_MSG_IS_IN_PHONEBOOK).text().set(VR_MSG_IS_IN_PHONEBOOK_TRUE);
        phoneinfoNode.child(VR_MSG_PHONE_INFO_CONTACT_ID).text().set(resultNode.child(VR_MSG_PHONE_INFO_CONTACT_ID).text().as_string());
        phoneinfoNode.child(VR_MSG_PHONE_INFO_CONTACT_NAME).text().set(resultNode.child(VR_MSG_PHONE_INFO_CONTACT_NAME).text().as_string());
        phoneinfoNode.child(VR_MSG_PHONE_INFO_PHONETYPE_ID).text().set(resultNode.child(VR_MSG_PHONE_INFO_PHONETYPE_ID).text().as_string());
        phoneinfoNode.child(VR_MSG_PHONE_INFO_PHONETYPE_NAME).text().set(getPhoneTypeName(resultNode.child(VR_MSG_PHONE_INFO_PHONETYPE_ID).text().as_string()).c_str());
        phoneinfoNode.child(VR_MSG_PHONE_INFO_NUMBER).text().set(resultNode.child(VR_MSG_PHONE_INFO_NUMBER).text().as_string());
        phoneinfoNode.child(VR_MSG_PHONE_INFO_LIST_TYPE).text().set("0");
    }
    else {
        responseNode.child(VR_MSG_IS_IN_PHONEBOOK).text().set(VR_MSG_IS_IN_PHONEBOOK_FALSE);
    }

    responseNode.print(oss);
    response = oss.str();
    VR_LOGD("response msg is [%s]", response.c_str());

    return true;
}

bool VR_ContactProcessor::getPhoneInfo(const std::string &reqMsg, std::string &response)
{
    VR_LOGD_FUNC();

    // create response message
    pugi::xml_document responseDoc;
    responseDoc.load_string("");
    responseDoc.append_child(VR_MSG_RESPONSE_NODENAME);
    pugi::xml_node responseNode = responseDoc.child(VR_MSG_RESPONSE_NODENAME);
    responseNode.append_child(VR_MSG_RESPONSE_PHONE_INFO_RESULT);
    responseNode.append_child(VR_MSG_RESPONSE_PHONE_INFO_LIST).append_attribute(VR_MSG_RESPONSE_PHONE_INFO_LIST_ID).set_value((std::string(VR_MSG_RESPONSE_PHONE_INFO_LIST_ID_PREFIX) + VR_OPERATION_GET_PHONE_INFO).c_str());
    responseNode.child(VR_MSG_RESPONSE_PHONE_INFO_LIST).append_child(VR_MSG_RESPONSE_PHONE_INFO_LIST_HEADER);
    responseNode.child(VR_MSG_RESPONSE_PHONE_INFO_LIST).append_child(VR_MSG_RESPONSE_PHONE_INFO_LIST_ITEMS);

    pugi::xml_node headerNode = responseNode.child(VR_MSG_RESPONSE_PHONE_INFO_LIST).child(VR_MSG_RESPONSE_PHONE_INFO_LIST_HEADER);
    headerNode.append_child(VR_MSG_RESPONSE_PHONE_INFO_LIST_HEADER_PAGESIZE).text().set("0");
    headerNode.append_child(VR_MSG_RESPONSE_PHONE_INFO_LIST_HEADER_STARTINDEX).text().set("0");
    headerNode.append_child(VR_MSG_RESPONSE_PHONE_INFO_LIST_HEADER_COUNT).text().set("0");

    pugi::xml_node itemsNode = responseNode.child(VR_MSG_RESPONSE_PHONE_INFO_LIST).child(VR_MSG_RESPONSE_PHONE_INFO_LIST_ITEMS);

    std::ostringstream oss;
    responseNode.print(oss);
    response = oss.str();
    oss.str("");

    if (SQLITE_OK != m_dbResult) {
        return false;
    }

    // process request message
    VR_LOGD("reqMsg [%s]", reqMsg.c_str());
    pugi::xml_document msgDoc;
    msgDoc.load_string(reqMsg.c_str());
    pugi::xml_node queryTypeNode = msgDoc.select_node((std::string("//") + VR_MSG_PHONE_INFO_QUERYTYPE).c_str()).node();
    std::string queryType(queryTypeNode.text().as_string());
    if (queryType.empty()) {
        VR_ERROR("queryType is empty in getPhoneInfo msg, please check.");
        return false;
    }
    pugi::xml_node phoneinfoNode = msgDoc.select_node((std::string("//") + VR_MSG_PHONE_INFO).c_str()).node();
    if (phoneinfoNode.empty()) {
        VR_ERROR("phoneinfo is empty in getPhoneInfo msg, please check.");
        return false;
    }
    std::string contactId(phoneinfoNode.child(VR_MSG_PHONE_INFO_CONTACT_ID).text().as_string());
    if (contactId.empty()) {
        VR_ERROR("contact id is empty in getPhoneInfo msg, please check.");
        return false;
    }
    std::string fullname(phoneinfoNode.child(VR_MSG_PHONE_INFO_CONTACT_NAME).text().as_string());
    if (fullname.empty()) {
        VR_ERROR("name is empty in getPhoneInfo msg, please check.");
        return false;
    }
    std::string phonetypeID = phoneinfoNode.child(VR_MSG_PHONE_INFO_PHONETYPE_ID).text().as_string();
    if (VR_MSG_PHONE_INFO_QUERYTYPE_NAMEANDTYPE == queryType && phonetypeID.empty()) {
        VR_ERROR("queryType is nameAndType but phonetypeID is empty, please check.");
        return false;
    }

    // // query database
    bool isTypeFound = true;
    queryDatabase_PhoneInfo(isTypeFound,itemsNode,oss,fullname,contactId,queryType);
    // std::string sqlRequest(std::string("SELECT typeid,indexid,number FROM phonerecords WHERE id =\"") + contactId + "\"");
    // char * errorMsg = NULL;
    // int result;
    // bool isTypeFound = true;
    // itemsNode.append_attribute(VR_MSG_RESPONSE_PHONE_INFO_LIST_ITEMS_TEMPNODE_NAME).set_value(fullname.c_str());
    // itemsNode.append_attribute(VR_MSG_RESPONSE_PHONE_INFO_LIST_ITEMS_TEMPNODE_ID).set_value(contactId.c_str());

    // if (VR_MSG_PHONE_INFO_QUERYTYPE_NAMEANDTYPE == queryType && !phonetypeID.empty()) {
    //     int totalTypeID = atoi(phonetypeID.c_str());
    //     int typeID = totalTypeID % 4;
    //     int indexID = totalTypeID / 4;
    //     oss << typeID;
    //     std::string typeIDStr = oss.str();
    //     oss.str("");
    //     oss << indexID;
    //     std::string indexIDStr = oss.str();
    //     oss.str("");
    //     if (0 == indexID || 1 == indexID) {
    //         indexIDStr = "0,1";
    //     }
    //     result = sqlite3_exec(m_dbHandler, (sqlRequest + " AND typeid=\"" + typeIDStr +"\" AND indexid IN (" + indexIDStr + ")").c_str(), getPhoneInfoCallback, &itemsNode, &errorMsg);
    //     if (SQLITE_OK != result) {
    //         VR_ERROR("Run SQL request [%s] failed, error code: [%d], error msg: [%s]", sqlRequest.c_str(), result, errorMsg);
    //         return false;
    //     }
    //     if (itemsNode.first_child().empty()) {
    //         isTypeFound = false;
    //     }
    // }

    // if (itemsNode.first_child().empty()) {
    //     result = sqlite3_exec(m_dbHandler, sqlRequest.c_str(), getPhoneInfoCallback, &itemsNode, &errorMsg);
    //     if (SQLITE_OK != result) {
    //         VR_ERROR("Run SQL request [%s] failed, error code: [%d], error msg: [%s]", sqlRequest.c_str(), result, errorMsg);
    //         return false;
    //     }
    // }

    // itemsNode.remove_attribute(VR_MSG_RESPONSE_PHONE_INFO_LIST_ITEMS_TEMPNODE_NAME);
    // itemsNode.remove_attribute(VR_MSG_RESPONSE_PHONE_INFO_LIST_ITEMS_TEMPNODE_ID);

    // process data
    int phoneInfoRecordCount = 0;
    pugi::xml_node itemNode = itemsNode.first_child();
    while (!itemNode.empty()) {
        ++phoneInfoRecordCount;
        itemNode = itemNode.next_sibling();
    }

    char buffer[5] = {};
    snprintf(buffer, 5, "%d", phoneInfoRecordCount);
    headerNode.child(VR_MSG_RESPONSE_PHONE_INFO_LIST_HEADER_COUNT).text().set(buffer);

    if (VR_MSG_PHONE_INFO_QUERYTYPE_NAMEANDTYPE == queryType && !isTypeFound) {
        responseNode.child(VR_MSG_RESPONSE_PHONE_INFO_RESULT).text().set(VR_MSG_RESPONSE_PHONE_INFO_RESULT_NOT_FOUND);
    }
    else {
        responseNode.child(VR_MSG_RESPONSE_PHONE_INFO_RESULT).text().set(VR_MSG_RESPONSE_PHONE_INFO_RESULT_FOUND);
    }

    if (0 == phoneInfoRecordCount) {
        itemsNode.append_child(VR_MSG_RESPONSE_PHONE_INFO_LIST_ITEMS_ITEM).append_copy(phoneinfoNode);
    }

    responseNode.print(oss);
    response = oss.str();
    VR_LOGD("response msg is [%s]", response.c_str());

    return true;
}

bool VR_ContactProcessor::buildInfoList(const std::string &reqMsg, std::string &response)
{
    VR_LOGD_FUNC();
    m_phoneListStr.clear();

    // process request message
    VR_LOGD("reqMsg [%s]", reqMsg.c_str());
    pugi::xml_document msgDoc;
    msgDoc.load_string(reqMsg.c_str());
    pugi::xml_node phoneListNode = msgDoc.select_node((std::string("//") + VR_MSG_BUILD_INFO_PHONE_LIST).c_str()).node();
    pugi::xml_node screenListNode = msgDoc.select_node((std::string("//") + VR_MSG_BUILD_INFO_SCREEN_LIST).c_str()).node();

    // create default response message
    std::ostringstream oss;
    screenListNode.print(oss);
    response = oss.str();
    oss.str("");

    if (!phoneListNode) {
        VR_ERROR("reqMsg do not have phoneList node");
        return false;
    }

    if (!screenListNode) {
        VR_ERROR("reqMsg do not have screenList node");
        return false;
    }


    int typeID[] = { 0, 1, 2, 3 };
    VoiceSet<int>::type typeSet(typeID, typeID + 4);
    pugi::xpath_node_set nodes = phoneListNode.select_nodes("//phoneType");
    for (pugi::xpath_node_set::iterator it = nodes.begin(); it != nodes.end(); ++it) {
        pugi::xml_text phoneTypeText = it->node().text();
        if (!phoneTypeText.empty()) {
            typeSet.erase(phoneTypeText.as_int());
        }
    }

    pugi::xml_node screenCountNode = msgDoc.select_node((std::string("//") + VR_MSG_BUILD_INFO_SCREEN_LIST + "//count").c_str()).node();
    if (!screenCountNode) {
        VR_ERROR("screenList node do not have count subnode");
        return false;
    }
    int count = screenCountNode.text().as_int();

    pugi::xml_node screenItemsNode = msgDoc.select_node((std::string("//") + VR_MSG_BUILD_INFO_SCREEN_LIST + "//items").c_str()).node();
    if (!screenItemsNode) {
        VR_ERROR("screenList node do not have items subnode");
        return false;
    }

    for (VoiceSet<int>::iterator it = typeSet.begin(); it != typeSet.end(); ++it) {
        pugi::xml_node itemNode = screenItemsNode.find_child_by_attribute("item", "phoneType", getPhoneTypeString(*it).c_str());
        if (screenItemsNode.remove_child(itemNode)) {
            --count;
        }
    }
    screenCountNode.text().set(count);

    screenListNode.print(oss);
    response = oss.str();
    oss.str("");
    VR_LOGD("response msg is [%s]", response.c_str());

    phoneListNode.print(oss);
    m_phoneListStr = oss.str();

    return true;
}

bool VR_ContactProcessor::getInfoByType(const std::string &reqMsg, std::string &response)
{
    VR_LOGD_FUNC();

    // create default response message
    response = "<phoneInfo>"
                    "<contactId></contactId>"
                    "<contactName>''</contactName>"
                    "<phoneKind>''</phoneKind>"
                    "<phoneType>0</phoneType>"
                    "<phoneTypeName>''</phoneTypeName>"
                    "<number>''</number>"
                    "<listType>0</listType>"
                    "<timestamp></timestamp>"
                "</phoneInfo>";

    // process request message
    VR_LOGD("reqMsg [%s]", reqMsg.c_str());
    pugi::xml_document msgDoc;
    msgDoc.load_string(reqMsg.c_str());
    pugi::xml_node typeNode = msgDoc.select_node("//type").node();
    if (!typeNode) {
        VR_ERROR("can not find type in reqMsg");
        return false;
    }

    if (m_phoneListStr.empty()) {
        VR_ERROR("error in buildInfoList or not invoke buildInfoList before, can not get phoneList");
        return false;
    }

    std::string type = typeNode.text().as_string();

    pugi::xml_document phoneListDoc;
    phoneListDoc.load_string(m_phoneListStr.c_str());

    pugi::xpath_node_set nodes = phoneListDoc.select_nodes("//phoneInfo");
    pugi::xml_node phoneInfoNode;
    for (pugi::xpath_node_set::iterator it = nodes.begin(); it != nodes.end(); ++it) {
        pugi::xml_text phoneTypeText = it->node().child("phoneType").text();
        if (!phoneTypeText.empty()) {
            if (type == getPhoneTypeString(phoneTypeText.as_int())) {
                phoneInfoNode = it->node();
                break;
            }
        }
    }

    std::ostringstream oss;
    phoneInfoNode.print(oss);
    response = oss.str();
    VR_LOGD("response msg is [%s]", response.c_str());

    return true;
}

bool VR_ContactProcessor::getContactIds(const std::string &reqMsg, std::string &response)
{
    VR_LOGD_FUNC();

    if (SQLITE_OK != m_dbResult) {
        return false;
    }

    // process request message
    pugi::xml_document msgDoc;
    msgDoc.load_string(reqMsg.c_str());
    pugi::xml_node nameNode = msgDoc.select_node("//name").node();
    if (nameNode.empty()) {
        return false;
    }
    else {
        std::string name = nameNode.text().as_string();
        std::string sqlRequest("SELECT id FROM contact WHERE full=\"");
        sqlRequest = sqlRequest + name + "\"";
        char * errmsg = NULL;
        VoiceList<std::string>::type idList;
        int result = sqlite3_exec(m_dbHandler, sqlRequest.c_str(), getContactIdsCallback, &idList, &errmsg);
        if (SQLITE_OK != result) {
            VR_ERROR("Run SQL request [%s] failed, error code: [%d], error msg: [%s]", sqlRequest.c_str(), result, errmsg);
            return false;
        }

        // create response message
        pugi::xml_document responseDoc;
        responseDoc.load_string("");
        responseDoc.append_child(VR_MSG_RESPONSE_NODENAME);
        pugi::xml_node responseNode = responseDoc.child(VR_MSG_RESPONSE_NODENAME);
        for (VoiceList<std::string>::iterator it = idList.begin(); it != idList.end(); ++it) {
            responseNode.append_child("id").text().set(it->c_str());
        }
        std::ostringstream oss;
        responseNode.print(oss);
        response = oss.str();
        oss.str("");
        return true;
    }
}

// int VR_ContactProcessor::getContactCount()
// {
//     VR_LOGD_FUNC();
//     if (SQLITE_OK != m_dbResult) {
//         return 0;
//     }
//     int number = 0;
//     std::string sqlRequest("SELECT count(*) FROM contact");
//     char * errmsg = NULL;
//     int result = sqlite3_exec(m_dbHandler, sqlRequest.c_str(), getRecordCountCallback, &number, &errmsg);
//     if (SQLITE_OK != result) {
//         VR_ERROR("Run SQL request [%s] failed, error code: [%d], error msg: [%s]", sqlRequest.c_str(), result, errmsg);
//         return 0;
//     }
//     return number;
// }

int VR_ContactProcessor::checkContactInPhonebookCallback(void *resultNode, int columnNum, char **columnValue, char **columnName)
{
    pugi::xml_node * resultNodePoint = (pugi::xml_node *)resultNode;
    if (resultNodePoint->first_child().empty() && columnValue[0] && columnValue[1] && columnValue[2] && columnValue[3] && columnValue[4]) {
        resultNodePoint->append_child(VR_MSG_PHONE_INFO_CONTACT_ID).text().set(columnValue[0]);
        resultNodePoint->append_child(VR_MSG_PHONE_INFO_CONTACT_NAME).text().set(columnValue[1]);
        int typeID = atoi(columnValue[2]);
        int indexID = atoi(columnValue[3]);
        resultNodePoint->append_child(VR_MSG_PHONE_INFO_PHONETYPE_ID).text().set(typeID + indexID * 4);
        resultNodePoint->append_child(VR_MSG_PHONE_INFO_NUMBER).text().set(columnValue[4]);
    }
    return 0;
}

int VR_ContactProcessor::getPhoneInfoCallback(void *itemsNode, int columnNum, char **columnValue, char **columnName)
{
    if (columnValue[0] && columnValue[1] && columnValue[2]) {
        pugi::xml_node * itemsNodePoint = (pugi::xml_node *)itemsNode;
        pugi::xml_node phoneinfoNode = itemsNodePoint->append_child(VR_MSG_RESPONSE_PHONE_INFO_LIST_ITEMS_ITEM).append_child(VR_MSG_PHONE_INFO);
        phoneinfoNode.append_child(VR_MSG_PHONE_INFO_CONTACT_ID).text().set(itemsNodePoint->attribute(VR_MSG_RESPONSE_PHONE_INFO_LIST_ITEMS_TEMPNODE_ID).value());
        phoneinfoNode.append_child(VR_MSG_PHONE_INFO_CONTACT_NAME).text().set(itemsNodePoint->attribute(VR_MSG_RESPONSE_PHONE_INFO_LIST_ITEMS_TEMPNODE_NAME).value());
        int typeID = atoi(columnValue[0]);
        int indexID = atoi(columnValue[1]);
        int totalTypeID = typeID + indexID * 4;
        std::ostringstream oss;
        oss << totalTypeID;
        std::string totalTypeIDStr = oss.str();
        phoneinfoNode.append_child(VR_MSG_PHONE_INFO_PHONETYPE_ID).text().set(totalTypeID);
        phoneinfoNode.append_child(VR_MSG_PHONE_INFO_PHONETYPE_NAME).text().set(getPhoneTypeName(totalTypeIDStr).c_str());
        phoneinfoNode.append_child(VR_MSG_PHONE_INFO_NUMBER).text().set(columnValue[2]);
        phoneinfoNode.append_child(VR_MSG_PHONE_INFO_LIST_TYPE).text().set("0");
    }
    return 0;
}

int VR_ContactProcessor::getRecordCountCallback(void *number, int columnNum, char **columnValue, char **columnName)
{
    if (columnValue[0]) {
        *(int *)number = atoi(columnValue[0]);
    }
    return 0;
}

int VR_ContactProcessor::getContactIdsCallback(void *idListPoint, int columnNum, char **columnValue, char **columnName)
{
    if (columnValue[0]) {
        ((VoiceList<std::string>::type*)idListPoint)->push_back(columnValue[0]);
    }
    return 0;
}

//// get phone type string used for script
//std::string VR_ContactProcessor::getPhoneTypeString(int id)
//{
//    switch (id) {
//    case 0:
//        return "MOBILE";
//    case 1:
//        return "HOME";
//    case 2:
//        return "WORK";
//    case 3:
//        return "OTHER";
//    default:
//        return "OTHER";
//    }
//}


/* EOF */
