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

/**
 * @file VR_NaviAddressCommon.h
 * @brief Declaration file of VR_NaviAddressCommon.
 *
 * This file includes the declaration of VR_NaviAddressCommon.
 *
 * @attention used for C++ only.
 */

#ifndef VR_NAVI_ADDRESS_COMMON_H
#define VR_NAVI_ADDRESS_COMMON_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include "MEM_list.h"
#include "MEM_map.h"

#include <pugixml.hpp>

extern const char* VR_MSG_RESPONSE_NODENAME;
extern const char* VR_MSG_NAVI_ZONE_ID;
extern const char* VR_MSG_NAVI_CITY_ID;
extern const char* VR_MSG_NAVI_STREET_ID;
extern const char* VR_MSG_NAVI_ZONE_NAME_INDEX;
extern const char* VR_MSG_NAVI_CITY_NAME_INDEX;
extern const char* VR_MSG_NAVI_STREET_NAME_INDEX;
extern const char* VR_MSG_NAVI_HOUSE_NUMBER;
extern const char* VR_MSG_NAVI_TYPE_STATE;
extern const char* VR_MSG_NAVI_TYPE_CITY;
extern const char* VR_MSG_NAVI_TYPE_STREET;
extern const char* VR_OPERATION_ADDRESS_MAPPING;
extern const char* VR_OPERATION_BUILD_ADDRESS;
extern const char* VR_OPERATION_SEND_VOICE_ID;
extern const char* VR_OPERATION_BUILD_ADDRESS_NAME;
extern const char* VR_OPERATION_GET_POI_PROMPT;

class sqlite3;
class VR_ConfigureIF;

/**
 * @brief The VR_NaviAddressCommon class
 *
 * provide common address search function for navi
 */

class VR_NaviAddressCommon
{
public:
    VR_NaviAddressCommon(VR_ConfigureIF* config);
    virtual ~VR_NaviAddressCommon();

    bool addressMapping(const std::string &reqMsg, std::string &response);
    bool buildAddress(const std::string &reqMsg, std::string &response);
    bool buildAddressName(const std::string &reqMsg, std::string &response);
    bool getPOIPrompt(const std::string &reqMsg, std::string &response);

    void setCountryID(const std::string &countryID);

protected:
    // get global VoiceID of city and state
    bool getGlobalVoiceID(const std::string &searchID, std::string &globalVoiceID);
    // get searchIDs according to global voiceID
    // FIXME: if different streetGlobalVoiceID can match same streetSearchID, there is problem
    void getSearchIDList(const std::string &globalVoiceID, VoiceList<std::string>::type &searchIDList);
    // if old Data return searchID, and new Data(PP data) return searchID_typeID
    void processReceivedSearchID(std::string &searchID, const std::string &nameIndex);

private:
    sqlite3 * m_dbHandler;
    VR_ConfigureIF* m_configure;
    int m_dbResult;
    std::string m_countryID;
    std::string m_dataVersion;
    std::string m_zoneNameIndex;
    static VoiceMap<std::string, std::string>::type m_searchIDVoiceIDCacheMap;

    bool (VR_NaviAddressCommon::*m_getSearchIDListFunc)(const std::string &, const VoiceList<std::string>::type &, VoiceList<std::string>::type &, bool);

    // query searchID for address mapping
    bool querySearchID(const VoiceList<std::string>::type &zoneGlobalVoiceIDList,
        const VoiceList<std::string>::type &cityGlobalVoiceIDList,
        const VoiceList<std::string>::type &streetGlobalVoiceIDList);

    // query searchID for build address
    bool queryZoneSearchID(pugi::xpath_node_set &zoneNodes, VoiceList<std::string>::type &zoneSearchIDList);
    bool queryCitySearchID(pugi::xpath_node_set &cityNodes,
        const std::string &zoneGlobalVoiceID,
        VoiceList<std::string>::type &citySearchIDList);
    bool queryStreetSearchID(pugi::xpath_node_set &streetNodes,
        const std::string &cityGlobalVoiceID,
        VoiceList<std::string>::type &streetSearchIDList);

    // get result items node xml according the searchIDs
    int getResultItem(pugi::xml_node &itemsNode,
        const VoiceList<std::string>::type &zoneSearchIDList,
        const VoiceList<std::string>::type &citySearchIDList,
        const VoiceList<std::string>::type &streetSearchIDList,
        const std::string &houseNumber);
    // initialize the response node xml
    void initResponseDoc(pugi::xml_document &responseDoc);
    // create the template of item node xml
    void initItemNodeTemplate(pugi::xml_document &templateNodeDoc);

    // sqlite3 callback
    static int getSingleListCallback(void *stringList, int columnNum, char **columnValue, char **columnName);
    static int getSearchIDCallback(void *stringList, int columnNum, char **columnValue, char **columnName);

    std::string getTableName(bool isPhoneme = false);
    bool getOldDataSearchIDList(const std::string &tableName, const VoiceList<std::string>::type &globalVoiceIDList, VoiceList<std::string>::type &searchIDList, bool isClearMap = false);
    bool getNewDataSearchIDList(const std::string &tableName, const VoiceList<std::string>::type &globalVoiceIDList, VoiceList<std::string>::type &searchIDList, bool isClearMap = false);

    void processSearchID(std::string &searchID, std::string &nameIndex, VoiceList<std::string>::const_iterator &it);
    static void handleMultiSearchID(const std::string &voiceID, const std::string &multiSearchID, VoiceList<std::string>::type &searchIDList);
};

#endif /* VR_NAVI_ADDRESS_COMMON_H */
/* EOF */