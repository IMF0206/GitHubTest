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
 * @file VR_DeData.h
 * @brief process data in DE
 *
 *
 * @attention used for C++ only.
 */
#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#ifndef _VR_DEDATAMAGAGER_H_
#define _VR_DEDATAMAGAGER_H_


#include "config.h"
#include "MEM_map.h"
#include <string>
#include "rapidjson/document.h"

/**
 * @brief The VR_DeData class
 *
 * for process list data in DE
 */
class VR_DEDataManager
{
public:
    VR_DEDataManager(const std::string& path);
    ~VR_DEDataManager();

    bool initData(const std::string &language);
    bool releaseData();
    bool changeLanguage(const std::string& language);
    std::string getScxmlPath(const std::string& agentName);
    std::string getPrompts(const std::string& sKey);
    std::string getPrompts(const std::string& sKey, const std::string &defaultValue);
    std::string getState2Text(const std::string& id); // REVIEW: for meter

protected:
    VR_DISALLOW_EVIL_CONSTRUCTORS(VR_DEDataManager);
    std::string m_language;
    rapidjson::Document m_prompts;
    rapidjson::Document m_state2id;
    rapidjson::Document m_id2text;

    static VoiceMap<std::string, std::string>::type m_mapLangToEng;
    std::string m_asrPath;

private:
    bool loadData();
    bool loadJsonData(const std::string& filePath, rapidjson::Document& doc);
};

#endif // _VR_DEDATAMAGAGER_H_
/* EOF */
