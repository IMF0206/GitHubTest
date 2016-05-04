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

#include "VR_NaviAddressCommon.h"
#include "VR_VoiceIDConverter.h"
#include "VR_ConfigureIF.h"
#include "VR_Log.h"

#include <sqlite3.h>
#include <sstream>
#include <fstream>
#include <string>
#include <boost/assign/list_of.hpp>


const char* VR_MSG_RESPONSE_NODENAME        = "data";
const char* VR_MSG_NAVI_ZONE_ID             = "zoneId";
const char* VR_MSG_NAVI_CITY_ID             = "cityId";
const char* VR_MSG_NAVI_STREET_ID           = "streetId";
const char* VR_MSG_NAVI_ZONE_NAME_INDEX     = "zoneNameIndex";
const char* VR_MSG_NAVI_CITY_NAME_INDEX     = "cityNameIndex";
const char* VR_MSG_NAVI_STREET_NAME_INDEX   = "streetNameIndex";
const char* VR_MSG_NAVI_HOUSE_NUMBER        = "houseNumber";
const char* VR_MSG_NAVI_TYPE_STATE          = "STATE";
const char* VR_MSG_NAVI_TYPE_CITY           = "CITY";
const char* VR_MSG_NAVI_TYPE_STREET         = "STREET";
const char* VR_OPERATION_ADDRESS_MAPPING    = "_addressMapping";
const char* VR_OPERATION_BUILD_ADDRESS      = "_buildAddress";
const char* VR_OPERATION_SEND_VOICE_ID      = "_sendVoiceID";
const char* VR_OPERATION_BUILD_ADDRESS_NAME = "_buildAddressName";
const char* VR_OPERATION_GET_POI_PROMPT     = "_getPOIPrompt";


#define VR_MSG_NAVI_ZONE_NAME           "zone"
#define VR_MSG_NAVI_CITY_NAME           "city"
#define VR_MSG_NAVI_STREET_NAME         "street"
#define VR_MSG_NAVI_HOUSE_NUMBER_NAME   "houseNumber"
#define VR_MSG_NAVI_ADDRESS_NAME        "name"

#define VR_MSG_NAVI_LIST             "list"
#define VR_MSG_NAVI_ITEMS            "items"
#define VR_MSG_NAVI_ITEM             "item"
#define VR_MSG_NAVI_ADDRESS_ID       "addressId"
#define VR_MSG_NAVI_TYPE_NUMBER      "NUMBER"

#define VR_MSG_BUILD_ADDRESS_BUILD_TYPE                 "buildType"
#define VR_MSG_BUILD_ADDRESS_ADDRESS_INFO               "addressInfo"
#define VR_MSG_BUILD_ADDRESS_ASR_RESULT                 "asrResult"
#define VR_MSG_BUILD_ADDRESS_BUILD_ADDRESS_NAME_NODE    "address"

#define VR_MSG_RESPONSE_ADDRESS_MAPPING_TYPE                "mappingType"
#define VR_MSG_RESPONSE_ADDRESS_MAPPING_TYPE_FULL           "FULL"
#define VR_MSG_RESPONSE_ADDRESS_MAPPING_TYPE_TILL_STREET    "TILL_STREET"
#define VR_MSG_RESPONSE_ADDRESS_MAPPING_TYPE_TILL_CITY      "TILL_CITY"
#define VR_MSG_RESPONSE_ADDRESS_MAPPING_TYPE_TILL_STATE     "TILL_STATE"
#define VR_MSG_RESPONSE_ADDRESS_LIST                        "list"
#define VR_MSG_RESPONSE_ADDRESS_LIST_ID                     "id"
#define VR_MSG_RESPONSE_ADDRESS_LIST_ID_PREFIX              "list_"
#define VR_MSG_RESPONSE_ADDRESS_LIST_HEADER                 "header"
#define VR_MSG_RESPONSE_ADDRESS_LIST_HEADER_PAGESIZE        "pageSize"
#define VR_MSG_RESPONSE_ADDRESS_LIST_HEADER_STARTINDEX      "startIndex"
#define VR_MSG_RESPONSE_ADDRESS_LIST_HEADER_COUNT           "count"
#define VR_MSG_RESPONSE_ADDRESS_LIST_ITEMS                  "items"
#define VR_MSG_RESPONSE_ADDRESS_LIST_ITEMS_ITEM             "item"

#define VOICE_ID_SEARCH_ID_DB               "voice_search_id.db"
#define VOICE_ID_SEARCH_ID_DB_TABLE_PREFIX  "voice_search_id_"
#define POI_PROMPT_TABLE_PREFIX             "pcp_tbl_"

VoiceMap<std::string, std::string>::type VR_NaviAddressCommon::m_searchIDVoiceIDCacheMap;

VR_NaviAddressCommon::VR_NaviAddressCommon(VR_ConfigureIF* config)
    : m_dbHandler(nullptr)
    , m_configure(config)
    , m_countryID()
    , m_dataVersion()
    , m_getSearchIDListFunc(&VR_NaviAddressCommon::getOldDataSearchIDList)
{
    std::string dbPath = m_configure->getMapDataPath() + "/" + VOICE_ID_SEARCH_ID_DB;

    m_dbResult = sqlite3_open_v2(dbPath.c_str(), &m_dbHandler, SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX, NULL);
    if (SQLITE_OK != m_dbResult) {
        VR_ERROR("Open DB file %s failed, resultID: [%d]", std::string(VOICE_ID_SEARCH_ID_DB).c_str(), m_dbResult);
    }
    if (SQLITE_OK == m_dbResult) {
        VoiceList<std::string>::type resultList;
        std::string sqlRequest(std::string("SELECT db_version FROM version_tbl"));
        char * errorMsg = NULL;
        int result = sqlite3_exec(m_dbHandler, sqlRequest.c_str(), getSingleListCallback, &resultList, &errorMsg);
        if (SQLITE_OK != result) {
            VR_ERROR("Run SQL request [%s] failed, error code: [%d], error msg: [%s]", sqlRequest.c_str(), result, errorMsg);
        }
        else {
            if (!resultList.empty()) {
                m_dataVersion.assign(resultList.front());
                m_getSearchIDListFunc = &VR_NaviAddressCommon::getNewDataSearchIDList;
            }
        }
    }
    else {
        VR_ERROR("VoiceIDSearchID DB file not exist, path: [%s]", dbPath.c_str());
    }
}

VR_NaviAddressCommon::~VR_NaviAddressCommon()
{
    sqlite3_close(m_dbHandler);
    m_dbHandler = nullptr;
}

void VR_NaviAddressCommon::setCountryID(const std::string &countryID)
{
    m_countryID.assign(countryID);
}

bool VR_NaviAddressCommon::addressMapping(const std::string &reqMsg, std::string &response)
{
    VR_LOGD_FUNC();

    // create response message
    pugi::xml_document responseDoc;
    initResponseDoc(responseDoc);
    pugi::xml_node responseNode = responseDoc.child(VR_MSG_RESPONSE_NODENAME);
    pugi::xml_node headerNode = responseNode.child(VR_MSG_RESPONSE_ADDRESS_LIST).child(VR_MSG_RESPONSE_ADDRESS_LIST_HEADER);
    pugi::xml_node itemsNode = responseNode.child(VR_MSG_RESPONSE_ADDRESS_LIST).child(VR_MSG_RESPONSE_ADDRESS_LIST_ITEMS);

    // set response data
    pugi::xml_document templateNodeDoc;
    initItemNodeTemplate(templateNodeDoc);
    pugi::xml_node itemNodeTemplate = templateNodeDoc.child(VR_MSG_NAVI_ITEM);
    pugi::xml_node tempAddresssIDItemNode = itemsNode.append_copy(itemNodeTemplate);

    std::ostringstream oss;
    responseNode.print(oss);
    response = oss.str();
    oss.str("");

    // process request message
    VR_LOGD("reqMsg [%s]", reqMsg.c_str());
    pugi::xml_document msgDoc;
    msgDoc.load_string(reqMsg.c_str());
    pugi::xml_node listNode = msgDoc.select_node((std::string("//") + VR_MSG_NAVI_ITEMS).c_str()).node();
    pugi::xml_node firstItem;
    if (!listNode.empty()) {
        firstItem = listNode.first_child();
    }

    if (firstItem.empty()) {
        VR_ERROR("asrResult list is empty, no voiceid input");
        return false;
    }

    std::string mappingType;
    VR_VoiceIDConverter converter;
    VR_GlobalVoiceIDResult firstResult;
    {
        std::string zoneid = firstItem.child(VR_MSG_NAVI_ZONE_ID).text().as_string();
        std::string cityid = firstItem.child(VR_MSG_NAVI_CITY_ID).text().as_string();
        std::string streetid = firstItem.child(VR_MSG_NAVI_STREET_ID).text().as_string();
        std::string houseNumber = firstItem.child(VR_MSG_NAVI_HOUSE_NUMBER).text().as_string();
        firstResult = converter.getGlobalVoiceID(zoneid, cityid, streetid, houseNumber);
    }

    VoiceList<VR_GlobalVoiceIDResult>::type resultList;
    VoiceList<std::string>::type zoneGlobalVoiceIDList;
    VoiceList<std::string>::type cityGlobalVoiceIDList;
    VoiceList<std::string>::type streetGlobalVoiceIDList;
    switch (firstResult.type) {
    case VR_MappingType::EMPTY:
        {
            VR_ERROR("zoneid is empty");
            return false;
        }
        break;
    case VR_MappingType::TILL_STATE:
        {
            mappingType = VR_MSG_RESPONSE_ADDRESS_MAPPING_TYPE_TILL_STATE;
            zoneGlobalVoiceIDList.push_back(firstResult.zoneid);
        }
        break;
    case VR_MappingType::TILL_CITY:
        {
            mappingType = VR_MSG_RESPONSE_ADDRESS_MAPPING_TYPE_TILL_CITY;
            zoneGlobalVoiceIDList.push_back(firstResult.zoneid);
            cityGlobalVoiceIDList.push_back(firstResult.cityid);
        }
        break;
    case VR_MappingType::TILL_STREET:
        {
            mappingType = VR_MSG_RESPONSE_ADDRESS_MAPPING_TYPE_TILL_STREET;
            zoneGlobalVoiceIDList.push_back(firstResult.zoneid);
            cityGlobalVoiceIDList.push_back(firstResult.cityid);
            streetGlobalVoiceIDList.push_back(firstResult.streetid);
        }
        break;
    case VR_MappingType::FULL:
        {
            mappingType = VR_MSG_RESPONSE_ADDRESS_MAPPING_TYPE_FULL;
            zoneGlobalVoiceIDList.push_back(firstResult.zoneid);
            cityGlobalVoiceIDList.push_back(firstResult.cityid);
            streetGlobalVoiceIDList.push_back(firstResult.streetid);
        }
        break;
    default:
        break;
    }
    resultList.push_back(firstResult);

    for (pugi::xml_node itemNode = firstItem.next_sibling();
        !itemNode.empty();
        itemNode = itemNode.next_sibling()) {
        std::string zoneid = itemNode.child(VR_MSG_NAVI_ZONE_ID).text().as_string();
        std::string cityid = itemNode.child(VR_MSG_NAVI_CITY_ID).text().as_string();
        std::string streetid = itemNode.child(VR_MSG_NAVI_STREET_ID).text().as_string();
        std::string houseNumber = itemNode.child(VR_MSG_NAVI_HOUSE_NUMBER).text().as_string();

        VR_GlobalVoiceIDResult result = converter.getGlobalVoiceID(zoneid, cityid, streetid, houseNumber);

        if ((VR_ConfigureIF::VR_EngineType::VBT != m_configure->getEngineType()) && (firstResult.type != result.type)) {
            continue;
        }
        switch (firstResult.type) {
        case VR_MappingType::TILL_STATE:
            {
                zoneGlobalVoiceIDList.push_back(result.zoneid);
            }
            break;
        case VR_MappingType::TILL_CITY:
            {
                zoneGlobalVoiceIDList.push_back(result.zoneid);
                cityGlobalVoiceIDList.push_back(result.cityid);
            }
            break;
        case VR_MappingType::TILL_STREET:
        case VR_MappingType::FULL:
            {
                zoneGlobalVoiceIDList.push_back(result.zoneid);
                cityGlobalVoiceIDList.push_back(result.cityid);
                streetGlobalVoiceIDList.push_back(result.streetid);
            }
            break;
        default:
            break;
        }
        resultList.push_back(result);
    }

    if (!querySearchID(zoneGlobalVoiceIDList, cityGlobalVoiceIDList, streetGlobalVoiceIDList)) {
        return false;
    }

    itemsNode.remove_child(tempAddresssIDItemNode);
    int itemCount = 0;

    for (VoiceList<VR_GlobalVoiceIDResult>::iterator it = resultList.begin();
        it != resultList.end();
        ++it) {
        VoiceList<std::string>::type zoneSearchIDList;
        VoiceList<std::string>::type citySearchIDList;
        VoiceList<std::string>::type streetSearchIDList;
        getSearchIDList(it->zoneid, zoneSearchIDList);
        getSearchIDList(it->cityid, citySearchIDList);
        getSearchIDList(it->streetid, streetSearchIDList);
        int count = getResultItem(itemsNode, zoneSearchIDList, citySearchIDList, streetSearchIDList, it->houseNumber);
        itemCount += count;
    }

    char buffer[5] = {};
    snprintf(buffer, 5, "%d", itemCount);
    headerNode.child(VR_MSG_RESPONSE_ADDRESS_LIST_HEADER_COUNT).text().set(buffer);

    responseNode.child(VR_MSG_RESPONSE_ADDRESS_MAPPING_TYPE).text().set(mappingType.c_str());
    responseNode.print(oss);
    response = oss.str();
    VR_LOGD("response msg is [%s]", response.c_str());

    return true;
}

bool VR_NaviAddressCommon::buildAddress(const std::string &reqMsg, std::string &response)
{
    VR_LOGD_FUNC();
    // create response message
    pugi::xml_document responseDoc;
    initResponseDoc(responseDoc);
    pugi::xml_node responseNode = responseDoc.child(VR_MSG_RESPONSE_NODENAME);
    pugi::xml_node headerNode = responseNode.child(VR_MSG_RESPONSE_ADDRESS_LIST).child(VR_MSG_RESPONSE_ADDRESS_LIST_HEADER);
    pugi::xml_node itemsNode = responseNode.child(VR_MSG_RESPONSE_ADDRESS_LIST).child(VR_MSG_RESPONSE_ADDRESS_LIST_ITEMS);

    // process request message
    VR_LOGD("reqMsg [%s]", reqMsg.c_str());
    pugi::xml_document msgDoc;
    msgDoc.load_string(reqMsg.c_str());
    std::string buildType = msgDoc.select_node((std::string("//") + VR_MSG_BUILD_ADDRESS_BUILD_TYPE).c_str()).node().text().as_string();
    pugi::xml_node addressInfoNode = msgDoc.select_node((std::string("//") + VR_MSG_BUILD_ADDRESS_ADDRESS_INFO).c_str()).node();
    pugi::xml_node asrResuldNode = msgDoc.select_node((std::string("//") + VR_MSG_BUILD_ADDRESS_ASR_RESULT).c_str()).node();

    // set response data
    pugi::xml_document templateNodeDoc;
    initItemNodeTemplate(templateNodeDoc);
    pugi::xml_node itemNodeTemplate = templateNodeDoc.child(VR_MSG_NAVI_ITEM);
    pugi::xml_node tempAddresssIDItemNode = itemsNode.append_copy(itemNodeTemplate);
    pugi::xml_node tempAddresssIDNode = tempAddresssIDItemNode.first_child();
    tempAddresssIDNode.child(VR_MSG_NAVI_ZONE_ID).text().set(addressInfoNode.child(VR_MSG_NAVI_ZONE_ID).text().as_string());
    tempAddresssIDNode.child(VR_MSG_NAVI_ZONE_NAME_INDEX).text().set(addressInfoNode.child(VR_MSG_NAVI_ZONE_NAME_INDEX).text().as_string());
    tempAddresssIDNode.child(VR_MSG_NAVI_CITY_ID).text().set(addressInfoNode.child(VR_MSG_NAVI_CITY_ID).text().as_string());
    tempAddresssIDNode.child(VR_MSG_NAVI_CITY_NAME_INDEX).text().set(addressInfoNode.child(VR_MSG_NAVI_CITY_NAME_INDEX).text().as_string());
    tempAddresssIDNode.child(VR_MSG_NAVI_STREET_ID).text().set(addressInfoNode.child(VR_MSG_NAVI_STREET_ID).text().as_string());
    tempAddresssIDNode.child(VR_MSG_NAVI_STREET_NAME_INDEX).text().set(addressInfoNode.child(VR_MSG_NAVI_STREET_NAME_INDEX).text().as_string());
    tempAddresssIDNode.child(VR_MSG_NAVI_HOUSE_NUMBER).text().set(addressInfoNode.child(VR_MSG_NAVI_HOUSE_NUMBER).text().as_string());

    std::ostringstream oss;
    responseNode.print(oss);
    response = oss.str();
    oss.str("");

    if (asrResuldNode.first_child().empty()) {
        VR_ERROR("asrResult list is empty, no voiceid input");
        return false;
    }

    asrResuldNode.print(oss);
    pugi::xml_document asrResuldNodeDoc;
    asrResuldNodeDoc.load_string(oss.str().c_str());
    oss.str("");

    if (VR_MSG_NAVI_TYPE_NUMBER == buildType) {
        VoiceList<std::string>::type numberList;
        pugi::xpath_node_set numberNodes = asrResuldNodeDoc.select_nodes((std::string("//") + VR_MSG_NAVI_HOUSE_NUMBER).c_str());
        for (pugi::xpath_node_set::iterator it = numberNodes.begin(); it != numberNodes.end(); ++it) {
            std::string number = it->node().text().as_string();
            if (!number.empty()) {
                numberList.push_back(number);
            }
        }
        itemsNode.remove_child(tempAddresssIDItemNode);
        pugi::xml_document itemNodeInfoDoc;
        itemNodeInfoDoc.load_string("");
        pugi::xml_node itemNodeInfoTemplate = itemNodeInfoDoc.append_child(VR_MSG_NAVI_ITEM);
        itemNodeInfoTemplate.append_copy(addressInfoNode);
        for (VoiceList<std::string>::iterator it = numberList.begin(); it != numberList.end(); ++it) {
            pugi::xml_node infoNode = itemsNode.append_copy(itemNodeInfoTemplate).child(VR_MSG_BUILD_ADDRESS_ADDRESS_INFO);
            infoNode.child(VR_MSG_NAVI_HOUSE_NUMBER).text().set(it->c_str());
        }

        char buffer[5] = {};
        snprintf(buffer, 5, "%d", numberList.size());
        headerNode.child(VR_MSG_RESPONSE_ADDRESS_LIST_HEADER_COUNT).text().set(buffer);

        responseNode.print(oss);
        response = oss.str();
        VR_LOGD("response msg is [%s]", response.c_str());

        return true;
    }

    // query data
    VoiceList<std::string>::type zoneSearchIDList;
    VoiceList<std::string>::type citySearchIDList;
    VoiceList<std::string>::type streetSearchIDList;
    if (VR_MSG_NAVI_TYPE_STATE == buildType) {
        pugi::xpath_node_set zoneNodes = asrResuldNodeDoc.select_nodes((std::string("//") + VR_MSG_NAVI_ZONE_ID).c_str());
        if (!queryZoneSearchID(zoneNodes, zoneSearchIDList)) {
            return false;
        }
    }
    else if (VR_MSG_NAVI_TYPE_CITY == buildType) {
        // get zoneSearchIDList
        std::string zoneSearchID(addressInfoNode.child(VR_MSG_NAVI_ZONE_ID).text().as_string());
        std::string zoneNameIndex(addressInfoNode.child(VR_MSG_NAVI_ZONE_NAME_INDEX).text().as_string());
        if ("''" == zoneNameIndex) {
            zoneNameIndex = "0";
        }
        m_zoneNameIndex = zoneNameIndex;
        processReceivedSearchID(zoneSearchID, zoneNameIndex);
        zoneSearchIDList.push_back(zoneSearchID);
        std::string zoneGlobalVoiceID;
        if (!getGlobalVoiceID(zoneSearchID, zoneGlobalVoiceID)) {
            return false;
        }
        pugi::xpath_node_set cityNodes = asrResuldNodeDoc.select_nodes((std::string("//") + VR_MSG_NAVI_CITY_ID).c_str());
        if (!queryCitySearchID(cityNodes, zoneGlobalVoiceID, citySearchIDList)) {
            return false;
        }
    }
    else if (VR_MSG_NAVI_TYPE_STREET == buildType) {
        // get zoneSearchIDList
        std::string zoneSearchID(addressInfoNode.child(VR_MSG_NAVI_ZONE_ID).text().as_string());
        std::string zoneNameIndex(addressInfoNode.child(VR_MSG_NAVI_ZONE_NAME_INDEX).text().as_string());
        if ("''" == zoneNameIndex) {
            zoneNameIndex = m_zoneNameIndex;
        }
        processReceivedSearchID(zoneSearchID, zoneNameIndex);
        zoneSearchIDList.push_back(zoneSearchID);
        // get citySearchIDList
        std::string citySearchID(addressInfoNode.child(VR_MSG_NAVI_CITY_ID).text().as_string());
        std::string cityNameIndex(addressInfoNode.child(VR_MSG_NAVI_CITY_NAME_INDEX).text().as_string());
        if ("''" == cityNameIndex) {
            cityNameIndex = "0";
        }
        processReceivedSearchID(citySearchID, cityNameIndex);
        citySearchIDList.push_back(citySearchID);
        std::string cityGlobalVoiceID;
        if (!getGlobalVoiceID(citySearchID, cityGlobalVoiceID)) {
            return false;
        }
        pugi::xpath_node_set streetNodes = asrResuldNodeDoc.select_nodes((std::string("//") + VR_MSG_NAVI_STREET_ID).c_str());
        if (!queryStreetSearchID(streetNodes, cityGlobalVoiceID, streetSearchIDList)) {
            return false;
        }
    }

    itemsNode.remove_child(tempAddresssIDItemNode);
    int itemCount = getResultItem(itemsNode, zoneSearchIDList, citySearchIDList, streetSearchIDList, "");

    char buffer[5] = {};
    snprintf(buffer, 5, "%d", itemCount);
    headerNode.child(VR_MSG_RESPONSE_ADDRESS_LIST_HEADER_COUNT).text().set(buffer);

    responseNode.print(oss);
    response = oss.str();
    VR_LOGD("response msg is [%s]", response.c_str());

    return true;
}

bool VR_NaviAddressCommon::buildAddressName(const std::string &reqMsg, std::string &response)
{
    VR_LOGD_FUNC();
    response = reqMsg;
    return true;
}

bool VR_NaviAddressCommon::getPOIPrompt(const std::string &reqMsg, std::string &response)
{
    if (SQLITE_OK == m_dbResult) {
        std::string language(m_configure->getTTSLanguage(m_configure->getVRLanguage()));
        std::string searchID(reqMsg);
        VoiceList<std::string>::type promptList;
        std::string sqlRequest(std::string("SELECT prompt_name FROM " POI_PROMPT_TABLE_PREFIX) + language + " WHERE search_id=" + searchID);
        char * errorMsg = NULL;
        int result = sqlite3_exec(m_dbHandler, sqlRequest.c_str(), getSingleListCallback, &promptList, &errorMsg);
        if (SQLITE_OK != result) {
            VR_ERROR("Run SQL request [%s] failed, error code: [%d], error msg: [%s]", sqlRequest.c_str(), result, errorMsg);
            return false;
        }

        if (!promptList.empty() && !promptList.front().empty()) {
            response = promptList.front();
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }
    return true;
}

bool VR_NaviAddressCommon::querySearchID(
    const VoiceList<std::string>::type &zoneGlobalVoiceIDList,
    const VoiceList<std::string>::type &cityGlobalVoiceIDList,
    const VoiceList<std::string>::type &streetGlobalVoiceIDList)
{
    VoiceList<std::string>::type zoneSearchIDList;
    VoiceList<std::string>::type citySearchIDList;
    VoiceList<std::string>::type streetSearchIDList;
    if (SQLITE_OK == m_dbResult) {
        sqlite3_exec(m_dbHandler, "BEGIN", NULL, NULL, NULL);
        std::string tableName;
        tableName.assign(getTableName());
        if (tableName.empty()) {
            VR_ERROR("Can not get VoiceIDSearchID tableName");
            return false;
        }
        if (!zoneGlobalVoiceIDList.empty()) {
            if (!(this->*m_getSearchIDListFunc)(tableName, zoneGlobalVoiceIDList, zoneSearchIDList, true)) {
                return false;
            }
            if (!cityGlobalVoiceIDList.empty()) {
                if (!(this->*m_getSearchIDListFunc)(tableName, cityGlobalVoiceIDList, citySearchIDList, false)) {
                    return false;
                }
                if (!streetGlobalVoiceIDList.empty()) {
                    if (!(this->*m_getSearchIDListFunc)(tableName, streetGlobalVoiceIDList, streetSearchIDList, false)) {
                        return false;
                    }
                }
            }
        }
        sqlite3_exec(m_dbHandler, "COMMIT", NULL, NULL, NULL);
    }
    else {
        return false;
    }
    return true;
}

bool VR_NaviAddressCommon::queryZoneSearchID(pugi::xpath_node_set &zoneNodes, VoiceList<std::string>::type &zoneSearchIDList)
{
    if (SQLITE_OK == m_dbResult) {
        std::string tableName;
        tableName.assign(getTableName());
        if (tableName.empty()) {
            VR_ERROR("Can not get VoiceIDSearchID tableName");
            return false;
        }
        VR_VoiceIDConverter converter;
        VoiceList<std::string>::type zoneGlobalVoiceIDList;
        for (pugi::xpath_node_set::iterator it = zoneNodes.begin(); it != zoneNodes.end(); ++it) {
            std::string zoneVoiceID = it->node().text().as_string();
            if (!zoneVoiceID.empty()) {
                zoneGlobalVoiceIDList.push_back(converter.getGlobalZoneID(zoneVoiceID));
            }
        }
        if (zoneGlobalVoiceIDList.empty()) {
            VR_ERROR("no available zone voiceid from asr result");
            return false;
        }
        if (!(this->*m_getSearchIDListFunc)(tableName, zoneGlobalVoiceIDList, zoneSearchIDList, true)) {
            return false;
        }
    }
    else {
        return false;
    }
    return true;
}

bool VR_NaviAddressCommon::queryCitySearchID(
    pugi::xpath_node_set &cityNodes,
    const std::string &zoneGlobalVoiceID,
    VoiceList<std::string>::type &citySearchIDList)
{
    if (SQLITE_OK == m_dbResult) {
        std::string tableName;
        tableName.assign(getTableName());
        if (tableName.empty()) {
            VR_ERROR("Can not get VoiceIDSearchID tableName");
            return false;
        }
        VR_VoiceIDConverter converter;
        VoiceList<std::string>::type cityGlobalVoiceIDList;
        for (pugi::xpath_node_set::iterator it = cityNodes.begin(); it != cityNodes.end(); ++it) {
            std::string cityVoiceID = it->node().text().as_string();
            if (!cityVoiceID.empty()) {
                cityGlobalVoiceIDList.push_back(converter.getGlobalCityID(zoneGlobalVoiceID, cityVoiceID));
            }
        }
        if (cityGlobalVoiceIDList.empty()) {
            VR_ERROR("no available city voiceid from asr result");
            return false;
        }
        if (!(this->*m_getSearchIDListFunc)(tableName, cityGlobalVoiceIDList, citySearchIDList, false)) {
            return false;
        }
    }
    else {
        return false;
    }
    return true;
}

bool VR_NaviAddressCommon::queryStreetSearchID(
    pugi::xpath_node_set &streetNodes,
    const std::string &cityGlobalVoiceID,
    VoiceList<std::string>::type &streetSearchIDList)
{
    if (SQLITE_OK == m_dbResult) {
        std::string tableName;
        tableName.assign(getTableName());
        if (tableName.empty()) {
            VR_ERROR("Can not get VoiceIDSearchID tableName");
            return false;
        }
        VR_VoiceIDConverter converter;
        VoiceList<std::string>::type streetGlobalVoiceIDList;
        for (pugi::xpath_node_set::iterator it = streetNodes.begin(); it != streetNodes.end(); ++it) {
            std::string streetVoiceID = it->node().text().as_string();
            if (!streetVoiceID.empty()) {
                streetGlobalVoiceIDList.push_back(converter.getGlobalStreetID(cityGlobalVoiceID, streetVoiceID));
            }
        }
        if (streetGlobalVoiceIDList.empty()) {
            VR_ERROR("no available street voiceid from asr result");
            return false;
        }
        if (!(this->*m_getSearchIDListFunc)(tableName, streetGlobalVoiceIDList, streetSearchIDList, false)) {
            return false;
        }
    }
    else {
        return false;
    }
    return true;
}

int VR_NaviAddressCommon::getResultItem(
    pugi::xml_node &itemsNode,
    const VoiceList<std::string>::type &zoneSearchIDList,
    const VoiceList<std::string>::type &citySearchIDList,
    const VoiceList<std::string>::type &streetSearchIDList,
    const std::string &houseNumber)
{
    int count = 0;
    pugi::xml_document templateNodeDoc;
    initItemNodeTemplate(templateNodeDoc);
    pugi::xml_node itemNodeTemplate = templateNodeDoc.child(VR_MSG_NAVI_ITEM);
    for (VoiceList<std::string>::const_iterator zoneIterator = zoneSearchIDList.cbegin(); zoneIterator != zoneSearchIDList.cend(); ++zoneIterator) {

        std::string zoneSearchID;
        std::string zoneNameIndex;
        processSearchID(zoneSearchID, zoneNameIndex, zoneIterator);

        if (citySearchIDList.empty()) {
            pugi::xml_node addressNode = itemsNode.append_copy(itemNodeTemplate).child(VR_MSG_NAVI_ADDRESS_ID);
            addressNode.child(VR_MSG_NAVI_ZONE_ID).text().set(zoneSearchID.c_str());
            addressNode.child(VR_MSG_NAVI_ZONE_NAME_INDEX).text().set(zoneNameIndex.c_str());
            ++count;
        }
        else {
            for (VoiceList<std::string>::const_iterator cityIterator = citySearchIDList.cbegin(); cityIterator != citySearchIDList.cend(); ++cityIterator) {

                std::string citySearchID;
                std::string cityNameIndex;
                processSearchID(citySearchID, cityNameIndex, cityIterator);

                if (streetSearchIDList.empty()) {
                    pugi::xml_node addressNode = itemsNode.append_copy(itemNodeTemplate).child(VR_MSG_NAVI_ADDRESS_ID);
                    addressNode.child(VR_MSG_NAVI_ZONE_ID).text().set(zoneSearchID.c_str());
                    addressNode.child(VR_MSG_NAVI_ZONE_NAME_INDEX).text().set(zoneNameIndex.c_str());
                    addressNode.child(VR_MSG_NAVI_CITY_ID).text().set(citySearchID.c_str());
                    addressNode.child(VR_MSG_NAVI_CITY_NAME_INDEX).text().set(cityNameIndex.c_str());
                    ++count;
                }
                else {
                    for (VoiceList<std::string>::const_iterator streetIterator = streetSearchIDList.cbegin(); streetIterator != streetSearchIDList.cend(); ++streetIterator) {
                        std::string streetSearchID;
                        std::string streetNameIndex;
                        processSearchID(streetSearchID, streetNameIndex, streetIterator);

                        pugi::xml_node addressNode = itemsNode.append_copy(itemNodeTemplate).child(VR_MSG_NAVI_ADDRESS_ID);
                        addressNode.child(VR_MSG_NAVI_ZONE_ID).text().set(zoneSearchID.c_str());
                        addressNode.child(VR_MSG_NAVI_ZONE_NAME_INDEX).text().set(zoneNameIndex.c_str());
                        addressNode.child(VR_MSG_NAVI_CITY_ID).text().set(citySearchID.c_str());
                        addressNode.child(VR_MSG_NAVI_CITY_NAME_INDEX).text().set(cityNameIndex.c_str());
                        addressNode.child(VR_MSG_NAVI_STREET_ID).text().set(streetSearchID.c_str());
                        addressNode.child(VR_MSG_NAVI_STREET_NAME_INDEX).text().set(streetNameIndex.c_str());
                        if (!houseNumber.empty()) {
                            addressNode.child(VR_MSG_NAVI_HOUSE_NUMBER).text().set(houseNumber.c_str());
                        }
                        ++count;
                    }
                }
            }
        }
    }
    return count;
}

void VR_NaviAddressCommon::initResponseDoc(pugi::xml_document &responseDoc)
{
    responseDoc.load_string("");
    responseDoc.append_child(VR_MSG_RESPONSE_NODENAME);
    pugi::xml_node responseNode = responseDoc.child(VR_MSG_RESPONSE_NODENAME);
    responseNode.append_child(VR_MSG_RESPONSE_ADDRESS_MAPPING_TYPE);
    responseNode.append_child(VR_MSG_RESPONSE_ADDRESS_LIST).append_attribute(VR_MSG_RESPONSE_ADDRESS_LIST_ID).set_value((std::string(VR_MSG_RESPONSE_ADDRESS_LIST_ID_PREFIX) + VR_OPERATION_ADDRESS_MAPPING).c_str());
    responseNode.child(VR_MSG_RESPONSE_ADDRESS_LIST).append_child(VR_MSG_RESPONSE_ADDRESS_LIST_HEADER);
    responseNode.child(VR_MSG_RESPONSE_ADDRESS_LIST).append_child(VR_MSG_RESPONSE_ADDRESS_LIST_ITEMS);

    pugi::xml_node headerNode = responseNode.child(VR_MSG_RESPONSE_ADDRESS_LIST).child(VR_MSG_RESPONSE_ADDRESS_LIST_HEADER);
    headerNode.append_child(VR_MSG_RESPONSE_ADDRESS_LIST_HEADER_PAGESIZE).text().set("5");
    headerNode.append_child(VR_MSG_RESPONSE_ADDRESS_LIST_HEADER_STARTINDEX).text().set("0");
    headerNode.append_child(VR_MSG_RESPONSE_ADDRESS_LIST_HEADER_COUNT).text().set("0");
}

void VR_NaviAddressCommon::initItemNodeTemplate(pugi::xml_document &templateNodeDoc)
{
    templateNodeDoc.load_string("");
    pugi::xml_node itemNodeTemplate = templateNodeDoc.append_child(VR_MSG_NAVI_ITEM);
    pugi::xml_node addressIDNode = itemNodeTemplate.append_child(VR_MSG_NAVI_ADDRESS_ID);
    addressIDNode.append_child(VR_MSG_NAVI_ZONE_ID);
    addressIDNode.append_child(VR_MSG_NAVI_ZONE_NAME_INDEX);
    addressIDNode.append_child(VR_MSG_NAVI_CITY_ID);
    addressIDNode.append_child(VR_MSG_NAVI_CITY_NAME_INDEX);
    addressIDNode.append_child(VR_MSG_NAVI_STREET_ID);
    addressIDNode.append_child(VR_MSG_NAVI_STREET_NAME_INDEX);
    addressIDNode.append_child(VR_MSG_NAVI_HOUSE_NUMBER);
}

int VR_NaviAddressCommon::getSingleListCallback(void *stringList, int columnNum, char **columnValue, char **columnName)
{
    VR_LOGD_FUNC();
    if (columnValue[0]) {
        std::string value(columnValue[0]);
        if (!value.empty()) {
            ((VoiceList<std::string>::type*)stringList)->push_back(columnValue[0]);
        }
    }
    return 0;
}

int VR_NaviAddressCommon::getSearchIDCallback(void *stringList, int columnNum, char **columnValue, char **columnName)
{
    VR_LOGD_FUNC();
    if (columnValue[0] && columnValue[1]) {
        std::string result(columnValue[0]);
        std::string voiceID(columnValue[1]);
        handleMultiSearchID(voiceID, result, *((VoiceList<std::string>::type*)stringList));
    }
    return 0;
}

std::string VR_NaviAddressCommon::getTableName(bool isPhoneme)
{
    if (m_countryID.empty()) {
        return std::string();
    }
    std::string tableName(VOICE_ID_SEARCH_ID_DB_TABLE_PREFIX);
    std::string phonemeLanguage;
    phonemeLanguage.assign(m_configure->getTTSLanguage(m_configure->getVRLanguage()));
    if (phonemeLanguage.empty()) {
        return std::string();
    }
    tableName.assign(tableName + m_countryID + "_" + phonemeLanguage);
    if (isPhoneme) {
        tableName += "_phoneme";
    }
    return tableName;
}

bool VR_NaviAddressCommon::getOldDataSearchIDList(const std::string &tableName, const VoiceList<std::string>::type &globalVoiceIDList, VoiceList<std::string>::type &searchIDList, bool isClearMap)
{
    VR_LOGD_FUNC();

    if (isClearMap) {
        m_searchIDVoiceIDCacheMap.clear();
    }

    std::string globalVoiceIDListStr("(");
    for (VoiceList<std::string>::const_iterator it = globalVoiceIDList.cbegin(); it != globalVoiceIDList.cend(); ++it) {
        globalVoiceIDListStr += (*it + ",");
    }
    globalVoiceIDListStr.replace(globalVoiceIDListStr.size() - 1, 1, ")");
    std::string sqlRequest(std::string("SELECT search_id,voice_id FROM ") + tableName + " WHERE voice_id IN " + globalVoiceIDListStr);
    char * errorMsg = NULL;
    int result = sqlite3_exec(m_dbHandler, sqlRequest.c_str(), getSearchIDCallback, &searchIDList, &errorMsg);
    if (SQLITE_OK != result) {
        VR_ERROR("Run SQL request [%s] failed, error code: [%d], error msg: [%s]", sqlRequest.c_str(), result, errorMsg);
        return false;
    }
    if (VR_ConfigureIF::VR_EngineType::VBT != m_configure->getEngineType()) {
        searchIDList.unique();
    }
    return true;
}

bool VR_NaviAddressCommon::getNewDataSearchIDList(const std::string &tableName, const VoiceList<std::string>::type &globalVoiceIDList, VoiceList<std::string>::type &searchIDList, bool isClearMap)
{
    VR_LOGD_FUNC();

    if (isClearMap) {
        m_searchIDVoiceIDCacheMap.clear();
    }

    VoiceList<std::string>::const_iterator it = globalVoiceIDList.cbegin();
    while (searchIDList.size() < 5 && it != globalVoiceIDList.cend()) {
        std::string sqlRequest(std::string("SELECT search_id,voice_id FROM ") + tableName + " WHERE voice_id=" + *it);
        char * errorMsg = NULL;
        int result = sqlite3_exec(m_dbHandler, sqlRequest.c_str(), getSearchIDCallback, &searchIDList, &errorMsg);
        if (SQLITE_OK != result) {
            VR_ERROR("Run SQL request [%s] failed, error code: [%d], error msg: [%s]", sqlRequest.c_str(), result, errorMsg);
            return false;
        }
        ++it;
    }

    if (searchIDList.size() < 5) {
        it = globalVoiceIDList.cbegin();
        while (searchIDList.size() < 5 && it != globalVoiceIDList.cend()) {
            VoiceList<std::string>::type baseNameIDList;
            std::string sqlRequest(std::string("SELECT base_name_id FROM ") + tableName + " WHERE voice_id=" + *it);
            char * errorMsg = NULL;
            int result = sqlite3_exec(m_dbHandler, sqlRequest.c_str(), getSingleListCallback, &baseNameIDList, &errorMsg);
            if (SQLITE_OK != result) {
                VR_ERROR("Run SQL request [%s] failed, error code: [%d], error msg: [%s]", sqlRequest.c_str(), result, errorMsg);
                return false;
            }

            if (baseNameIDList.empty()) {
                ++it;
                continue;
            }
            // e.g. baseNameID = "baseName1,baseName2"
            std::string baseNameID(baseNameIDList.front());

            sqlRequest.assign(std::string("SELECT base_name_vl FROM ")
                + tableName
                + "_basename WHERE base_name_id in ("
                + baseNameID
                + ")");
            errorMsg = NULL;
            VoiceList<std::string>::type baseNameValueList;
            result = sqlite3_exec(m_dbHandler, sqlRequest.c_str(), getSingleListCallback, &baseNameValueList, &errorMsg);
            if (SQLITE_OK != result) {
                VR_ERROR("Run SQL request [%s] failed, error code: [%d], error msg: [%s]", sqlRequest.c_str(), result, errorMsg);
                return false;
            }
            for (VoiceList<std::string>::iterator baseNameIt = baseNameValueList.begin();
                baseNameIt != baseNameValueList.end();
                ++baseNameIt) {
                handleMultiSearchID(*it, *baseNameIt, searchIDList);
            }
            if (VR_ConfigureIF::VR_EngineType::VBT != m_configure->getEngineType()) {
                searchIDList.unique();
            }
            ++it;
        }
    }
    return true;
}

bool VR_NaviAddressCommon::getGlobalVoiceID(const std::string &searchID, std::string &globalVoiceID)
{
    VoiceMap<std::string, std::string>::iterator it;
    it = m_searchIDVoiceIDCacheMap.find(searchID);
    if (m_searchIDVoiceIDCacheMap.end() != it) {
        globalVoiceID.assign(it->second);
        return true;
    }
    else {
        VR_ERROR("can not find voiceid of searchid: [%s]", searchID.c_str());
        return false;
    }
}

void VR_NaviAddressCommon::getSearchIDList(const std::string &globalVoiceID, VoiceList<std::string>::type &searchIDList)
{
    VoiceMap<std::string, std::string>::iterator it = m_searchIDVoiceIDCacheMap.begin();
    while (m_searchIDVoiceIDCacheMap.end() != it) {
        if (it->second == globalVoiceID) {
            searchIDList.push_back(it->first);
        }
        ++it;
    }
}

void VR_NaviAddressCommon::processReceivedSearchID(std::string &searchID, const std::string &nameIndex)
{
    if (!m_dataVersion.empty()) {
        searchID = searchID + "_" + nameIndex;
    }
}

void VR_NaviAddressCommon::processSearchID(std::string &searchID, std::string &nameIndex, VoiceList<std::string>::const_iterator &it)
{
    nameIndex = "0";
    if (m_dataVersion.empty()) {
        searchID.assign(*it);
    }
    else {
        size_t underlinePos = (*it).find('_');
        if (std::string::npos != underlinePos) {
            searchID.assign(it->substr(0, underlinePos));
            nameIndex.assign(it->substr(underlinePos + 1));
        }
        else {
            searchID.assign(*it);
        }
    }
}

void VR_NaviAddressCommon::handleMultiSearchID(const std::string &voiceID, const std::string &multiSearchID, VoiceList<std::string>::type &searchIDList)
{
    if (!multiSearchID.empty()) {
        size_t pos = multiSearchID.find(",");
        size_t startPos = 0;
        std::string searchID;
        while (std::string::npos != pos) {
            searchID.assign(multiSearchID.substr(startPos, pos - startPos));
            if (!searchID.empty()) {
                searchIDList.push_back(searchID);
                m_searchIDVoiceIDCacheMap[searchID] = voiceID;
            }
            startPos = pos + 1;
            pos = multiSearchID.find(",", startPos);
        }
        searchID.assign(multiSearchID.substr(startPos));
        if (!searchID.empty()) {
            searchIDList.push_back(searchID);
            m_searchIDVoiceIDCacheMap[searchID] = voiceID;
        }
    }
}

/* EOF */
