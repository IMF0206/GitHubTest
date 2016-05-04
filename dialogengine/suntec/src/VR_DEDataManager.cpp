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



#include <fstream>
#include <sstream>
#include "VR_DEDataManager.h"
#include <boost/assign/list_of.hpp>


VR_DEDataManager::VR_DEDataManager(const std::string& path)
{
    VR_LOGD_FUNC();
    m_language = "en";
    m_asrPath = path;
}

VR_DEDataManager::~VR_DEDataManager()
{
    VR_LOGD_FUNC();
}

VoiceMap<std::string, std::string>::type VR_DEDataManager::m_mapLangToEng = boost::assign::map_list_of
        ("ms", "en-gb")
        ("id", "en-gb")
        ("vi", "en-gb")
        ("ar-ml", "ar")
        ("ar-fm", "ar");


bool VR_DEDataManager::initData(const std::string &language)
{
    VR_LOGD_FUNC();
    m_language = language;
    return loadData();
}

bool VR_DEDataManager::releaseData()
{
    VR_LOGD_FUNC();
    return true;
}

bool VR_DEDataManager::loadData()
{
    VR_LOGD_FUNC();
    
    // Convert to normal language
    VoiceMap<std::string, std::string>::iterator it = m_mapLangToEng.find(m_language);
    if (it != m_mapLangToEng.end()) {
        m_language = it->second; 
    } 
    VR_LOGD("loadData language = %s", m_language.c_str());

    // Load meter strings
    std::string state2id = m_asrPath + m_language + "/meter/state_2_id.json";
    loadJsonData(state2id, m_state2id);

    std::string id2text = m_asrPath + m_language + "/meter/id_2_text.json";
    loadJsonData(id2text, m_id2text);

    // It's most important
    std::string promptPath = m_asrPath + m_language + "/prompts/prompts.json";
    bool ok = loadJsonData(promptPath, m_prompts);
    return ok;
}

bool VR_DEDataManager::loadJsonData(const std::string& filePath, rapidjson::Document& doc)
{
    VR_LOGD_FUNC();
    VR_LOG("start loadJsonData from %s", filePath.c_str());
    const size_t BOM_SIZE = 3;

    std::ifstream ifs(filePath.c_str());
    std::stringstream buffer;
    buffer << ifs.rdbuf();
    std::string jsonStr = buffer.str();
    
    // Remove bom header if exist
    if (jsonStr.size() > BOM_SIZE) {
        char bom[BOM_SIZE] = { 239, 187, 191 };
        int result = memcmp(bom, jsonStr.c_str(), BOM_SIZE);
        if (0 == result) {
            jsonStr = jsonStr.substr(BOM_SIZE);
        }
    }

    doc.Parse<0>(jsonStr.c_str());
    if (doc.HasParseError()) {
        VR_ERROR("load json error code = %s", doc.GetParseError());
        return false;
    }

    int num = doc.Size();
    VR_LOGD("json num = %d", num);
    return true;
}

std::string VR_DEDataManager::getScxmlPath(const std::string& agentName)
{
    VR_LOGD_FUNC();
    std::string scxmlPath = m_asrPath + "engine/" + agentName + ".zip";
    std::ifstream fin(scxmlPath);
    if (!fin) {
        VR_ERROR("can't get scxml path %s", scxmlPath.c_str());
        return "";
    }
    return scxmlPath;
}

std::string VR_DEDataManager::getPrompts(const std::string& sKey)
{
    return getPrompts(sKey, sKey);
}

std::string VR_DEDataManager::getPrompts(const std::string& sKey, const std::string &defaultValue)
{
    VR_LOGD_FUNC();
    if (!m_prompts.IsObject()) {
        VR_ERROR("Prompts not be loaded");
        return defaultValue;
    }
    if (!m_prompts.HasMember(sKey.c_str())) {
        VR_ERROR("Prompts not found matched key:%s", sKey.c_str());
        return defaultValue;
    }
    std::string prompt = m_prompts[sKey.c_str()].GetString();
    if (prompt.empty()) {
        VR_ERROR("can't find key = [%s] in prompts", sKey.c_str());
        return defaultValue;
    }
    return prompt;
}

std::string VR_DEDataManager::getState2Text(const std::string& id)
{
    VR_LOGD_FUNC();
    if (!m_state2id.IsObject()) {
        VR_ERROR("m_state2id is not object");
        return id;
    }
    if (!m_state2id.HasMember(id.c_str())) {
        return id;
    }
    std::string state2Id = m_state2id[id.c_str()].GetString();
    if (state2Id.empty()) {
        VR_ERROR("can't find key = [%s] in state2id", id.c_str());
        return id;
    }
    //
    if (!m_id2text.IsObject()) {
        VR_ERROR("m_id2text is not object");
        return state2Id;
    }
    if (!m_id2text.HasMember(state2Id.c_str())) {
        return state2Id;
    }
    std::string text = m_id2text[state2Id.c_str()].GetString();
    if (text.empty()) {
        VR_ERROR("can't find key = [%s] in id2text", state2Id.c_str());
        return state2Id;
    }
    return text;
}

/* EOF */
