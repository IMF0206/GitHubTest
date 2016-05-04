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

#include "VR_ContactAccessor.h"
#include "VR_ContactProcessor.h"
#include "VR_Log.h"

#include "MEM_list.h"
#include "MEM_set.h"

#include <sqlite3.h>
#include <pugixml.hpp>
#include <sstream>

#define VR_MSG_PHONE_INFO                           "phoneInfo"
#define VR_MSG_PHONE_INFO_QUERYTYPE                 "queryType"
#define VR_MSG_PHONE_INFO_QUERYTYPE_NAMEANDTYPE     "NAME_AND_TYPE"

//#define VR_MSG_BUILD_INFO_PHONE_LIST                "phoneList"
//#define VR_MSG_BUILD_INFO_SCREEN_LIST               "screenList"


#define VR_MSG_IS_IN_PHONEBOOK                      "isInPhonebook"

#define VR_MSG_RESPONSE_PHONE_INFO_LIST                     "list"
#define VR_MSG_RESPONSE_PHONE_INFO_LIST_ID                  "id"
#define VR_MSG_RESPONSE_PHONE_INFO_LIST_HEADER              "header"
#define VR_MSG_RESPONSE_PHONE_INFO_LIST_ITEMS               "items"

#define VR_MSG_RESPONSE_PHONE_INFO_LIST_ITEMS_TEMPNODE_NAME "fullname"
#define VR_MSG_RESPONSE_PHONE_INFO_LIST_ITEMS_TEMPNODE_ID   "contactId"

VR_ContactAccessor::VR_ContactAccessor()
    : m_dbHandler(nullptr)
    , m_isContactInPhonebook(false)
    {}

bool VR_ContactAccessor::queryPhonebookDatabase(const std::string &contactID,const std::string &phonetypeID,const std::string &number)
{
    // query database
    std::string sqlRequest;
    if (!contactID.empty()) {
        sqlRequest = sqlRequest + "SELECT contact.id,contact.full,phonerecords.typeid,phonerecords.indexid,phonerecords.number FROM phonerecords,contact WHERE contact.id=phonerecords.id AND contact.id=\"" + contactID + "\" ";
    }
    if (!phonetypeID.empty()) {
        int totalTypeID = atoi(phonetypeID.c_str());
        int typeID = totalTypeID % 4;
        int indexID = totalTypeID / 4;
        std::ostringstream oss;
        oss << typeID;
        std::string typeIDStr = oss.str();
        oss.str("");
        oss << indexID;
        std::string indexIDStr = oss.str();
        if (0 == indexID || 1 == indexID) {
            indexIDStr = "0,1";
        }
        sqlRequest.append("INTERSECT SELECT contact.id,contact.full,phonerecords.typeid,phonerecords.indexid,phonerecords.number FROM phonerecords,contact WHERE contact.id=phonerecords.id AND phonerecords.typeid=\"").append(typeIDStr).append("\" AND phonerecords.indexid IN (").append(indexIDStr).append(") ");
    }
    if (!number.empty()) {
        sqlRequest = sqlRequest + "INTERSECT SELECT contact.id,contact.full,phonerecords.typeid,phonerecords.indexid,phonerecords.number FROM phonerecords,contact WHERE contact.id=phonerecords.id AND number=\"" + number + "\"";
    }
    if (sqlRequest.empty()) {
        VR_ERROR("phoneinfo is empty, can not query.");
        return false;
    }
    if (std::size_t(0) == sqlRequest.find("INTERSECT")) {
        sqlRequest.erase(0, 10);
    }
    char * errorMsg = NULL;
    int result = sqlite3_exec(m_dbHandler, sqlRequest.c_str(), checkContactInPhonebookCallback, &resultNode, &errorMsg);
    if (SQLITE_OK != result) {
        VR_ERROR("Run SQL request [%s] failed, error code: [%d], error msg: [%s]", sqlRequest.c_str(), result, errorMsg);
        return false;
    }
    return true;
}

bool VR_ContactAccessor::queryDatabase_PhoneInfo(bool isTypeFound,pugi::xml_node itemsNode,std::ostringstream oss,const std::string &fullname,const std::string &contactId,const std::string queryType)
{
    // query database
    std::string sqlRequest(std::string("SELECT typeid,indexid,number FROM phonerecords WHERE id =\"") + contactId + "\"");
    char * errorMsg = NULL;
    int result;
    isTypeFound = true;
    itemsNode.append_attribute(VR_MSG_RESPONSE_PHONE_INFO_LIST_ITEMS_TEMPNODE_NAME).set_value(fullname.c_str());
    itemsNode.append_attribute(VR_MSG_RESPONSE_PHONE_INFO_LIST_ITEMS_TEMPNODE_ID).set_value(contactId.c_str());

    if (VR_MSG_PHONE_INFO_QUERYTYPE_NAMEANDTYPE == queryType && !phonetypeID.empty()) {
        int totalTypeID = atoi(phonetypeID.c_str());
        int typeID = totalTypeID % 4;
        int indexID = totalTypeID / 4;
        oss << typeID;
        std::string typeIDStr = oss.str();
        oss.str("");
        oss << indexID;
        std::string indexIDStr = oss.str();
        oss.str("");
        if (0 == indexID || 1 == indexID) {
            indexIDStr = "0,1";
        }
        result = sqlite3_exec(m_dbHandler, (sqlRequest + " AND typeid=\"" + typeIDStr +"\" AND indexid IN (" + indexIDStr + ")").c_str(), getPhoneInfoCallback, &itemsNode, &errorMsg);
        if (SQLITE_OK != result) {
            VR_ERROR("Run SQL request [%s] failed, error code: [%d], error msg: [%s]", sqlRequest.c_str(), result, errorMsg);
            return false;
        }
        if (itemsNode.first_child().empty()) {
            isTypeFound = false;
        }
    }

    if (itemsNode.first_child().empty()) {
        result = sqlite3_exec(m_dbHandler, sqlRequest.c_str(), getPhoneInfoCallback, &itemsNode, &errorMsg);
        if (SQLITE_OK != result) {
            VR_ERROR("Run SQL request [%s] failed, error code: [%d], error msg: [%s]", sqlRequest.c_str(), result, errorMsg);
            return false;
        }
    }

    itemsNode.remove_attribute(VR_MSG_RESPONSE_PHONE_INFO_LIST_ITEMS_TEMPNODE_NAME);
    itemsNode.remove_attribute(VR_MSG_RESPONSE_PHONE_INFO_LIST_ITEMS_TEMPNODE_ID);

}

int VR_ContactAccessor::getContactCount()
{
    VR_LOGD_FUNC();
    if (SQLITE_OK != m_dbResult) {
        return 0;
    }
    int number = 0;
    std::string sqlRequest("SELECT count(*) FROM contact");
    char * errmsg = NULL;
    int result = sqlite3_exec(m_dbHandler, sqlRequest.c_str(), getRecordCountCallback, &number, &errmsg);
    if (SQLITE_OK != result) {
        VR_ERROR("Run SQL request [%s] failed, error code: [%d], error msg: [%s]", sqlRequest.c_str(), result, errmsg);
        return 0;
    }
    return number;
}

// get phone type string used for script
std::string VR_ContactAccessor::getPhoneTypeString(int id)
{
    switch (id) {
    case 0:
        return "MOBILE";
    case 1:
        return "HOME";
    case 2:
        return "WORK";
    case 3:
        return "OTHER";
    default:
        return "OTHER";
    }
}



/* EOF */
