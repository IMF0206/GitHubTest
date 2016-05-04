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

#include "VR_DataAccessorManager.h"

#include "VR_DataAccessorContact.h"
#include "VR_DataAccessorNavi.h"
#include "VR_DataAccessorNaviCN.h"
#include "VR_DataAccessorVoiceTag.h"
#include "VR_DataAccessorMedia.h"

#include "Vr_Asr_Engine.h"
#include "VR_PerformanceLog.h"

#include "VR_ConfigureIF.h"
#include "VR_DECommon.h"
#include "VR_Log.h"
#include "VR_Def.h"

#include "MEM_list.h"
#include "MEM_map.h"

#include <algorithm>
#include <boost/shared_array.hpp>
#include <boost/bind.hpp>
#include <cstdio>
#include <sqlite3.h>
#include <sstream>
#include <sys/types.h>

const int VR_MAX_MUSIC_GRAMMAR_COUNT = 15000;

#define MUSIC_AGENT_PREFIX          "MusicAgent"

#define VR_ITEM_ID                  "id"
#define VR_ITEM_NAME                "name"
#define VR_ITEM_SHORTCUT            "shortcut"
#define VR_ITEM_PRON                "pron"
#define VR_PHONE_TYPE               "type"
#define VR_PHONE_NUMBER             "number"
#define VR_GRAMMAR_ID               "grammarid"
#define VR_GRAMMAR_AGENT            "agent"
#define VR_GRAMMAR_PATH             "path"
#define VR_GRAMMAR_SONG_COUNT       "songitemcount"
#define VR_GRAMMAR_GENERAL_COUNT    "generalitemcount"
#define VR_GRAMMAR_GENRE_COUNT      "genreitemcount"
#define VR_DELETE_NODE_NAME         "delete"
#define VR_ADD_NODE_NAME            "add"
#define VR_CATEGORY_NAME            "name"

#define VR_DEVICE_ADDRESS           "deviceaddress"

#define VR_AGENT_MEDIA  "media"
#define VR_AGENT_PHONE  "phone"

#define VR_GRAMMAR_MSG_INIT         "grammar_init"
#define VR_GRAMMAR_MSG_DIFF         "grammar_diff"
#define VR_GRAMMAR_MSG_NEW          "grammar_new"
#define VR_GRAMMAR_MSG_PREUPDATE    "grammar_preupdate"
#define VR_GRAMMAR_MSG_ACTIVE       "grammar_active"
#define VR_GRAMMAR_MSG_DISACTIVE    "grammar_disactive"

#define VR_GRAMMAR_CATEGORY_CONTACT             "contact"
#define VR_GRAMMAR_CATEGORY_PHONETYPE           "phonetype"
#define VR_GRAMMAR_CATEGORY_QUICKREPLYMESSAGE   "quickreplymessage"
#define VR_GRAMMAR_CATEGORY_MESSAGETYPE         "messagetype"
#define VR_GRAMMAR_CATEGORY_DATA                "data"
#define VR_GRAMMAR_CATEGORY_SEARCHAPP           "searchapp"
#define VR_GRAMMAR_CATEGORY_AUDIOSOURCE         "audiosource"
#define VR_GRAMMAR_CATEGORY_FMGENRE             "fmgenre"
#define VR_GRAMMAR_CATEGORY_SATCHANNELNAME      "satchannelname"
#define VR_GRAMMAR_CATEGORY_SATCHANNELNUMBER    "satchannelnumber"
#define VR_GRAMMAR_CATEGORY_SATGENRE            "satgenre"
#define VR_GRAMMAR_CATEGORY_HDSUBCHANNEL        "hdsubchannel"
#define VR_GRAMMAR_CATEGORY_APPS                "apps"
#define VR_GRAMMAR_CATEGORY_APPSOFFBOARD        "appsoffboard"
#define VR_GRAMMAR_CATEGORY_VOICETAG            "voicetag"

#define VR_DB_TABLENAME_PHONERECORDS            "phonerecords"
#define VR_DB_TABLENAME_CONTACT                 VR_GRAMMAR_CATEGORY_CONTACT
#define VR_DB_TABLENAME_PHONETYPE               VR_GRAMMAR_CATEGORY_PHONETYPE
#define VR_DB_TABLENAME_QUICKREPLYMESSAGE       VR_GRAMMAR_CATEGORY_QUICKREPLYMESSAGE
#define VR_DB_TABLENAME_MESSAGETYPE             VR_GRAMMAR_CATEGORY_MESSAGETYPE
#define VR_DB_TABLENAME_DATA                    VR_GRAMMAR_CATEGORY_DATA
#define VR_DB_TABLENAME_SEARCHAPP               VR_GRAMMAR_CATEGORY_SEARCHAPP
#define VR_DB_TABLENAME_AUDIOSOURCE             VR_GRAMMAR_CATEGORY_AUDIOSOURCE
#define VR_DB_TABLENAME_FMGENRE                 VR_GRAMMAR_CATEGORY_FMGENRE
#define VR_DB_TABLENAME_SATCHANNELNAME          VR_GRAMMAR_CATEGORY_SATCHANNELNAME
#define VR_DB_TABLENAME_SATCHANNELNUMBER        VR_GRAMMAR_CATEGORY_SATCHANNELNUMBER
#define VR_DB_TABLENAME_SATGENRE                VR_GRAMMAR_CATEGORY_SATGENRE
#define VR_DB_TABLENAME_HDSUBCHANNEL            VR_GRAMMAR_CATEGORY_HDSUBCHANNEL
#define VR_DB_TABLENAME_APPS                    VR_GRAMMAR_CATEGORY_APPS
#define VR_DB_TABLENAME_APPSOFFBOARD            VR_GRAMMAR_CATEGORY_APPSOFFBOARD

#define VR_DICTIONARY_STATE_SYNCING     "DICTIONARY_SYNCING"
#define VR_DICTIONARY_STATE_OK          "DICTIONARY_OK"
#define VR_DICTIONARY_STATE_NONE        "DICTIONARY_NONE"

#define VR_GLOBALSTATE_NUMBER_OF_TOTAL              "NUMBER_OF_TOTAL"
#define VR_GLOBALSTATE_NUMBER_OF_MUSIC              "NUMBER_OF_MUSIC"
#define VR_GLOBALSTATE_NUMBER_OF_ARTIST             "NUMBER_OF_ARTIST"
#define VR_GLOBALSTATE_NUMBER_OF_ALBUM              "NUMBER_OF_ALBUM"
#define VR_GLOBALSTATE_NUMBER_OF_PLAYLIST           "NUMBER_OF_PLAYLIST"
#define VR_GLOBALSTATE_ACTIVED_SOURCE_ID            "ACTIVED_SOURCE_ID"
#define VR_GLOBALSTATE_MEDIA_MUSIC_SOURCE_SUPPORT   "MEDIA_MUSIC_SOURCE_SUPPORT"
#define VR_GLOBALSTATE_UPDATING_SOURCE_ID           "UPDATING_SOURCE_ID"
#define VR_GLOBALSTATE_MUSIC_DEVICE_CONNECTED       "MUSIC_DEVICE_CONNECTED"

#define VR_GLOBALSTATE_GRAMMAR_UPDATE_STATUS                "GRAMMAR_UPDATE_STATUS"
#define VR_GLOBALSTATE_GRAMMAR_UPDATE_STATUS_NEED_UPDATE    "needUpdate"
#define VR_GLOBALSTATE_GRAMMAR_UPDATE_STATUS_NO_CHANGE      "noChange"

#define VR_GLOBALSTATE_MUSIC_DICTIONARY_STATE       "MUSIC_DICTIONARY_STATE"
#define VR_GLOBALSTATE_ARTIST_DICTIONARY_STATE      "ARTIST_DICTIONARY_STATE"
#define VR_GLOBALSTATE_ALBUM_DICTIONARY_STATE       "ALBUM_DICTIONARY_STATE"
#define VR_GLOBALSTATE_PLAYLIST_DICTIONARY_STATE    "PLAYLIST_DICTIONARY_STATE"
#define VR_GLOBALSTATE_SONG_DICTIONARY_STATE        "SONG_DICTIONARY_STATE"
#define VR_GLOBALSTATE_GENRE_DICTIONARY_STATE       "GENRE_DICTIONARY_STATE"
#define VR_GLOBALSTATE_PHONEBOOK_DICTIONARY_STATE   "PHONEBOOK_DICTIONARY_STATE"
#define VR_GLOBALSTATE_DICTIONARY_STATE_SUFFIX      "_DICTIONARY_STATE"

#define VR_GLOBALSTATE_PHONE_DEVICE_ID              "PHONE_DEVICE_ID"
#define VR_GLOBALSTATE_PHONE_STATE_CONNECTED        "PHONE_STATE_CONNECTED"
#define VR_GLOBALSTATE_PHONE_STATE_REGISTERED       "PHONE_STATE_REGISTERED"
#define VR_GLOBALSTATE_PHONEBOOK_LIST_EXIST         "PHONEBOOK_LIST_EXIST"

#define VR_GLOBALSTATE_DECKLESS_MODE                "MEDIA_DECKLESS_MODEL"
#define VR_GLOBALSTATE_DECKLESS_MODE_DECKLESS       "DECKLESS"
#define VR_GLOBALSTATE_DECKLESS_MODE_NOT_DECKLESS   "NOT_DECKLESS"

#define VR_GLOBALSTATE_VALUE_TRUE   "true"
#define VR_GLOBALSTATE_VALUE_FALSE  "false"
#define VR_GLOBALSTATE_VALUE_ZERO   "0"
const std::string VR_GLOBALSTATE_VALUE_EMPTY = "";

// remove paired parentheses and square brackets
pcrecpp::RE VR_DataAccessorManager::m_bracketsRule("(?:\\((?:(?>[^\\[\\]\\(\\)]+)|(?0))*\\)|\\[(?:(?>[^\\[\\]\\(\\)]+)|(?0))*\\])");
// get alias before "ft keyword" and after it
pcrecpp::RE VR_DataAccessorManager::m_featuringRule("(.*?)(Ft\\.|Feat\\.|Featuring|ft\\.|feat\\.|featuring)");
// replace consecutive space with one space in media item alias
pcrecpp::RE VR_DataAccessorManager::m_consecutiveSpaceRule("  +");

VR_DataAccessorManager::VR_DataAccessorManager(VR_AsrRequestor* asrRequestor, VR_DECommonIF* common, VR_ConfigureIF* config)
    : VR_VoiceTagManager(config)
    , m_pDataSynchronizer(VR_new VR_DataSynchronizer(asrRequestor))
    , m_accessorContact(nullptr)
    , m_accessorNavi(nullptr)
    , m_accessorVoiceTag(nullptr)
    , m_accessorMedia(nullptr)
    , m_configure(config)
    , m_deCommonIF(common)
    , m_isMusicGrammarDroped(false)
    , m_isPhoneGrammarActive(false)
    , m_requestUpdateMediaGrammarFinish(false)
    , m_bIsRecognizing(false)
{
    VR_DECommonIF::DE_Country region = m_deCommonIF->getCountryType();

    // Init Other Accessors
    if (VR_DECommonIF::DE_Country::cn == region) {
        m_accessorNavi.reset(VR_new VR_DataAccessorNaviCN);
    }
    else {
        m_accessorNavi.reset(VR_new VR_DataAccessorNavi(m_configure));
    }
    m_accessorVoiceTag.reset(VR_new VR_DataAccessorVoiceTag(this));
    m_accessorMedia.reset(VR_new VR_DataAccessorMedia);
    m_accessorMedia->setGrammarUpdateStatus(VR_GLOBALSTATE_GRAMMAR_UPDATE_STATUS_NO_CHANGE);
    VR_LOGD("Grammar Update Status: noChange");
    m_accessorContact.reset(VR_new VR_DataAccessorContact);

    // load contextID according region
    switch (region) {
    case VR_DECommonIF::DE_Country::vt:
    {
        m_categoryContextIDMap["voicetag"] = "ctx_voice_tag_contact_name_list_";
        m_voiceTagContextID = m_categoryContextIDMap["voicetag"];
        break;
    }
    case VR_DECommonIF::DE_Country::eu:
    {
        m_categoryContextIDMap["genre"] = "ctx_media_play_genre_list_";
        m_musicCategoryList.push_back("genre");
    }
    // EU region has more category than default
    default:
    {
        m_categoryContextIDMap["contact"] = "ctx_phone_contact_name_list_1";
        m_categoryContextIDMap["album"] = "ctx_media_play_album_list_";
        m_categoryContextIDMap["artist"] = "ctx_media_play_artist_list_";
        m_categoryContextIDMap["playlist"] = "ctx_media_play_playlist_list_";
        m_categoryContextIDMap["song"] = "ctx_media_play_music_list_";

        m_musicCategoryList.push_back("album");
        m_musicCategoryList.push_back("artist");
        m_musicCategoryList.push_back("playlist");
        m_musicCategoryList.push_back("song");
        break;
    }
    }

    initializeState();
}

VR_DataAccessorManager::~VR_DataAccessorManager()
{
    m_accessorMedia->closeDB();
    m_accessorContact->closeDB();
}

void VR_DataAccessorManager::updateState(const std::string &stateMsg)
{
    pugi::xml_document msgDoc;
    msgDoc.load_string(stateMsg.c_str());
    pugi::xml_node eventNode = msgDoc.select_node("//event").node();

    bool isStateChanged = false;
    std::string prePhoneDeviceID = getState(VR_GLOBALSTATE_PHONE_DEVICE_ID);

    for (pugi::xml_node_iterator it = eventNode.begin(); it != eventNode.end(); ++it) {
        std::string key(it->attribute("key").as_string());
        std::string value(it->attribute("value").as_string());
        if (VR_GLOBALSTATE_PHONE_STATE_CONNECTED == key) {
            if ("connected" == value) {
                value = VR_GLOBALSTATE_VALUE_TRUE;
            }
            else {
                value = VR_GLOBALSTATE_VALUE_FALSE;
                handlePhoneActive(false, "", false);
            }
        }
        if (VR_GLOBALSTATE_PHONE_DEVICE_ID == key) {
            int id = std::strtol(value.c_str(), NULL, 10);
            std::ostringstream oss;
            oss << id;
            value = oss.str();
        }
        if (VR_GLOBALSTATE_PHONE_STATE_REGISTERED == key) {
            int id = std::strtol(value.c_str(), NULL, 10);
            if (id > 0) {
                value = VR_GLOBALSTATE_VALUE_TRUE;
            }
            else {
                value = VR_GLOBALSTATE_VALUE_FALSE;
            }
        }
        std::string keyState(getState(key));
        if (value == keyState) {
            continue;
        }
        else {
            setState(key, value);
            isStateChanged = true;
        }
        VR_LOG("GlobalState [%s] updated [%s]", key.c_str(), value.c_str());
    }

    if (!isStateChanged) {
        VR_LOG("state not changed");
        return;
    }

    std::string USB_1_State(getState("USB_1_CONNECTED"));
    std::string USB_2_State(getState("USB_2_CONNECTED"));
    std::string IPOD_1_State(getState("IPOD_1_CONNECTED"));
    std::string IPOD_2_State(getState("IPOD_2_CONNECTED"));

    if (VR_GLOBALSTATE_VALUE_TRUE == USB_1_State
        || VR_GLOBALSTATE_VALUE_TRUE == USB_2_State
        || VR_GLOBALSTATE_VALUE_TRUE == IPOD_1_State
        || VR_GLOBALSTATE_VALUE_TRUE == IPOD_2_State) {
        std::string globalMusicConnected(getState(VR_GLOBALSTATE_MUSIC_DEVICE_CONNECTED));
        if (VR_GLOBALSTATE_VALUE_TRUE != globalMusicConnected) {
            setState(VR_GLOBALSTATE_MUSIC_DEVICE_CONNECTED, VR_GLOBALSTATE_VALUE_TRUE);
            VR_LOG("GlobalState [" VR_GLOBALSTATE_MUSIC_DEVICE_CONNECTED "] updated [" VR_GLOBALSTATE_VALUE_TRUE "]");
        }
    }
    else {
        std::string globalMusicConnected(getState(VR_GLOBALSTATE_MUSIC_DEVICE_CONNECTED));
        if (VR_GLOBALSTATE_VALUE_FALSE != globalMusicConnected) {
            setState(VR_GLOBALSTATE_MUSIC_DEVICE_CONNECTED, VR_GLOBALSTATE_VALUE_FALSE);
            VR_LOG("GlobalState [" VR_GLOBALSTATE_MUSIC_DEVICE_CONNECTED "] updated [" VR_GLOBALSTATE_VALUE_FALSE "]");
        }
    }

    // process voicetag grammar active/inactive
    if (!m_voiceTagContextID.empty()) {
        pugi::xml_node phoneConnectNode = eventNode.find_child_by_attribute(
            "key",
            VR_GLOBALSTATE_PHONE_STATE_CONNECTED);
        pugi::xml_node phoneDeviceNode = eventNode.find_child_by_attribute(
            "key",
            VR_GLOBALSTATE_PHONE_DEVICE_ID);
        std::string deviceAddress = getState(VR_GLOBALSTATE_PHONE_DEVICE_ID);
        if (!phoneConnectNode.empty() || !phoneDeviceNode.empty()) {
            std::string globalPhoneConnected(getState(VR_GLOBALSTATE_PHONE_STATE_CONNECTED));
            if ((VR_GLOBALSTATE_VALUE_TRUE == globalPhoneConnected) && (VR_GLOBALSTATE_VALUE_ZERO != deviceAddress)) {
                setCurrentDevice(deviceAddress);
                grammarActive("ctx_voice_tag_recognize",
                    std::string("grm_cmd_voice_tag_recognize#rul_slt_voice_tag_contact_name_list_") + deviceAddress);
            }
        }
        std::string globalPhoneID(getState(VR_GLOBALSTATE_PHONE_DEVICE_ID));
        if (!phoneDeviceNode.empty()
            && (prePhoneDeviceID != globalPhoneID)
            && (VR_GLOBALSTATE_VALUE_ZERO != prePhoneDeviceID)) {
            grammarDisactive("ctx_voice_tag_recognize",
                std::string("grm_cmd_voice_tag_recognize#rul_slt_voice_tag_contact_name_list_") + prePhoneDeviceID);
            clearCurrentDevice();
        }
    }

    notifyStateUpdated();
}

void VR_DataAccessorManager::getUpdateState(std::string& stateMsg)
{
    pugi::xml_document msgDoc;
    msgDoc.load_string("");
    pugi::xml_node dataNode = msgDoc.append_child("data");
    dataNode.append_attribute("id").set_value("globalState");
    pugi::xml_node gNode = dataNode.append_child("g");
    gNode.append_attribute("xmlns").set_value("");

    for (VoiceMap<std::string, std::string>::iterator it = m_stateMap.begin();
        it != m_stateMap.end();
        ++it) {
        pugi::xml_node itemNode = gNode.append_child("item");
        itemNode.append_attribute("key").set_value(it->first.c_str());
        itemNode.append_attribute("value").set_value(it->second.c_str());
    }

    std::ostringstream oss;
    dataNode.print(oss);
    stateMsg = oss.str();
}

void VR_DataAccessorManager::setUpdateStateCallback(
    boost::function<void(const std::string &)> &callback)
{
    m_updateStateCallback = callback;
}

void VR_DataAccessorManager::setNotifyCallback(
    boost::function<void(const std::string &)> &callback)
{
    m_notifyCallback = callback;
}

void VR_DataAccessorManager::updateGrammar(const std::string &grammarMsg)
{
    VR_LOGD_FUNC();
    VR_LOG("updateGrammar: received GrammarMsg [%s] ", grammarMsg.c_str());
    pugi::sp_xml_document spGrammarMsgDoc(VR_new pugi::xml_document());
    if (!spGrammarMsgDoc->load_string(grammarMsg.c_str())) {
        VR_ERROR("load xml from grammarMsg failed");
        return;
    }

    if (m_bIsRecognizing) {
        // cache grammar msg
        m_grammarMsgQuque.push(spGrammarMsgDoc);

        // check whether grammar need change
        const std::string & globalActivedSourceID = getState(VR_GLOBALSTATE_ACTIVED_SOURCE_ID);
        VR_LOG("updateGrammar: active source [%s]", globalActivedSourceID.c_str());

        std::string msgName = spGrammarMsgDoc->first_child().name();
        bool grammarNeedchange = false;
        std::string grammarSourceID = spGrammarMsgDoc->first_child().attribute(VR_GRAMMAR_ID).as_string();
        std::string agent = spGrammarMsgDoc->first_child().attribute(VR_GRAMMAR_AGENT).as_string();
        if (globalActivedSourceID == grammarSourceID && VR_AGENT_MEDIA == agent) {
            if (VR_GRAMMAR_MSG_DISACTIVE == msgName) {
                std::string reply = spGrammarMsgDoc->first_child().attribute("reply").as_string();
                VR_LOG("updateGrammar: grammarid [%s] reply[%s]", grammarSourceID.c_str(), reply.c_str());
                if ("true" == reply) {
                    grammarNeedchange = true;
                }
            }
            else if (VR_GRAMMAR_MSG_INIT == msgName || VR_GRAMMAR_MSG_DIFF == msgName) {
                grammarNeedchange = true;
            }
        }

        // set change status
        if (grammarNeedchange) {
            setState(VR_GLOBALSTATE_GRAMMAR_UPDATE_STATUS, VR_GLOBALSTATE_GRAMMAR_UPDATE_STATUS_NEED_UPDATE);
            m_accessorMedia->setGrammarUpdateStatus(VR_GLOBALSTATE_GRAMMAR_UPDATE_STATUS_NEED_UPDATE);
            VR_LOGD("Grammar Update Status: needUpdate");
        }
    }
    else {
        handleGrammarMsg(spGrammarMsgDoc);
    }
}

void VR_DataAccessorManager::clearAllGrammar()
{
    m_pDataSynchronizer->stopGrammar();
    VR_DECommonIF::DE_Country region = m_deCommonIF->getCountryType();
    if (VR_DECommonIF::DE_Country::vt == region) {
        // clear voiceTag grammar
        clearVoiceTagGrammar();
    }
    else {
        // clear music grammar
        for (int i = 1; i < 5; ++i) {
            std::ostringstream oss;
            oss << i;
            std::string index = oss.str();
            for (VoiceList<std::string>::iterator it = m_musicCategoryList.begin();
                it != m_musicCategoryList.end();
                ++it) {
                std::string contextID = m_categoryContextIDMap[*it] + index;
                m_pDataSynchronizer->deleteGrammar(contextID);
            }
        }
    }

    // clear phone grammar
    m_pDataSynchronizer->deleteGrammar(
        "ctx_phone_contact_name_list_1",
        boost::bind(&VR_DataAccessorManager::clearGrammarCallback,
            this,
            _1));
}

bool VR_DataAccessorManager::saveVoiceTagGrammar(
    const std::string &voiceTagID,
    const std::string &name,
    const std::string &phoneme,
    const std::string &deviceAddress)
{
    if (m_voiceTagContextID.empty()) {
        return false;
    }
    VoiceList<spC_Term>::type addList;
    addList.push_back(VR_DataSynchronizer::getCTerm(atoi(voiceTagID.c_str()), name, phoneme));
    return m_pDataSynchronizer->updateGrammar(
        m_voiceTagContextID + deviceAddress,
        addList,
        boost::bind(&VR_DataAccessorManager::voiceTagUpdateGrammarCallback,
            this,
            _1,
            deviceAddress));
}

bool VR_DataAccessorManager::deleteVoiceTagGrammar(
    const VoiceList<std::pair<std::string, std::string>>::type &deleteList,
    const std::string &deviceAddress)
{
    if (m_voiceTagContextID.empty()) {
        return false;
    }
    VoiceList<spC_Term>::type delList;
    for (VoiceList<std::pair<std::string, std::string>>::const_iterator it = deleteList.cbegin();
        it != deleteList.cend();
        ++it) {
        delList.push_back(
            VR_DataSynchronizer::getCTerm(
                atoi((it->first).c_str()),
                it->second));
    }
    return m_pDataSynchronizer->deleteGrammar(
        m_voiceTagContextID + deviceAddress,
        delList,
        boost::bind(&VR_DataAccessorManager::voiceTagDeleteGrammarCallback,
            this,
            _1,
            deviceAddress));
}

bool VR_DataAccessorManager::deleteAllVoiceTagGrammar(const std::string &deviceAddress)
{
    if (m_voiceTagContextID.empty()) {
        return false;
    }
    return m_pDataSynchronizer->deleteGrammar(
        m_voiceTagContextID + deviceAddress,
        boost::bind(&VR_DataAccessorManager::voiceTagDeleteGrammarCallback,
            this,
            _1,
            deviceAddress));
}

bool VR_DataAccessorManager::clearVoiceTagGrammar()
{
    if (m_voiceTagContextID.empty()) {
        return false;
    }
    bool result = m_pDataSynchronizer->deleteGrammar(m_voiceTagContextID + "1");
    result = result && m_pDataSynchronizer->deleteGrammar(m_voiceTagContextID + "2");
    result = result && m_pDataSynchronizer->deleteGrammar(m_voiceTagContextID + "3");
    result = result && m_pDataSynchronizer->deleteGrammar(m_voiceTagContextID + "4");
    result = result && m_pDataSynchronizer->deleteGrammar(m_voiceTagContextID + "5");
    return result;
}

void VR_DataAccessorManager::voiceTagUpdateGrammarCallback(
    const N_Vr::N_Asr::C_Event_Phase &phaseEvent,
    const std::string &deviceAddress)
{
}

void VR_DataAccessorManager::voiceTagDeleteGrammarCallback(
    const N_Vr::N_Asr::C_Event_Phase &phaseEvent,
    const std::string &deviceAddress)
{
}

void VR_DataAccessorManager::updateGrammarCallback(
    const N_Vr::N_Asr::C_Event_Phase &phaseEvent,
    const std::string &category)
{
    N_Vr::N_Asr::C_Event_Phase::E_Event_Phase_Type msgType = phaseEvent.Get_Event_Phase_Type();
    switch (msgType) {
    case N_Vr::N_Asr::C_Event_Phase::E_Event_Phase_Type_Proc_Begin:
        {
            break;
        }
    case N_Vr::N_Asr::C_Event_Phase::E_Event_Phase_Type_Proc_End:
        {
            m_pDataSynchronizer->notifyUpdateGrammarCategoryFinish(category);
            break;
        }
    default:
        break;
    }
}

void VR_DataAccessorManager::clearGrammarCallback(const N_Vr::N_Asr::C_Event_Phase &phaseEvent)
{
    N_Vr::N_Asr::C_Event_Phase::E_Event_Phase_Type msgType = phaseEvent.Get_Event_Phase_Type();
    switch (msgType) {
    case N_Vr::N_Asr::C_Event_Phase::E_Event_Phase_Type_Proc_Begin:
        {
            break;
        }
    case N_Vr::N_Asr::C_Event_Phase::E_Event_Phase_Type_Proc_End:
        {
            if (m_notifyCallback) {
                m_notifyCallback("<event-result name=\"initialpersondata\"></event>");
            }
            break;
        }
    default:
        break;
    }
}

void VR_DataAccessorManager::getInfo(
    const std::string &operation,
    const std::string &reqMsg,
    std::string &response)
{
    VR_LOGD_FUNC();
    VR_DataAccessor * accessor = getAccessor(operation);
    if (accessor) {
        accessor->getInfo(operation, reqMsg, response);
    }
}

void VR_DataAccessorManager::setCountryID(const std::string &countryID)
{
    m_accessorNavi->setCountryID(countryID);
}

void VR_DataAccessorManager::onRecognizeBegin()
{
    VR_LOGD_FUNC();
    m_bIsRecognizing = true;

    // check grammar need update
    checkGrammarNeedUpdate();
}

void VR_DataAccessorManager::onRecognizeEnd()
{
    VR_LOGD_FUNC();
    m_bIsRecognizing = false;
    while (!m_grammarMsgQuque.empty()) {
        handleGrammarMsg(m_grammarMsgQuque.front());
        m_grammarMsgQuque.pop();
    }
}

void VR_DataAccessorManager::onUpdateGrammarCategoryFinish(const std::string &category)
{
    std::string key = category;
    if (VR_GRAMMAR_CATEGORY_CONTACT == key) {
        key = "PHONEBOOK";
        VR_PERF_LOG("case:212-16-799 212-17-799 212-18-799 DataAccessor generate phonebook grammar end");
        if (m_notifyCallback) {
            m_notifyCallback("<grammar_result op=\"grammar\" agent=\"" VR_AGENT_PHONE "\" errcode=\"0\" />");
        }
    }
    std::transform(key.begin(), key.end(), key.begin(), toupper);
    key += VR_GLOBALSTATE_DICTIONARY_STATE_SUFFIX;
    setState(key, VR_DICTIONARY_STATE_OK);
    VR_LOG("GlobalState [%s] updated [" VR_DICTIONARY_STATE_OK "]", key.c_str());
    if (category != VR_GRAMMAR_CATEGORY_CONTACT) {
        checkMusicGrammarState();
    }
    notifyStateUpdated();

    m_pDataSynchronizer->updateGrammarCategoryFinish(category);
    if (m_pDataSynchronizer->isAsrIdle()) {
        onUpdateGrammarFinish();
    }
}

void VR_DataAccessorManager::onUpdateGrammarFinish()
{
    // check grammar state need update
    checkGrammarNeedUpdate();

    // process next grammar msg
    if (m_bIsRecognizing) {
        return;
    }

    if (m_generateGrammarList.empty()) {
        return;
    }

    generateGrammar(m_generateGrammarList.front());
    m_generateGrammarList.pop_front();
}

void VR_DataAccessorManager::handleGrammarMsg(pugi::sp_xml_document &spGrammarMsgDoc)
{
    // execute actvie msg and push grammar msg to grammarstack
    VR_LOGD_FUNC();
    if (!m_voiceTagContextID.empty()) {
        VR_LOG("VoiceTag Region not process normal grammar message");
        return;
    }
    std::string nodeName(spGrammarMsgDoc->first_child().name());

    if (VR_GRAMMAR_MSG_ACTIVE == nodeName || VR_GRAMMAR_MSG_DISACTIVE == nodeName) {
        handleGrammarActiveMsg(spGrammarMsgDoc);
    }
    else if (VR_GRAMMAR_MSG_INIT == nodeName || VR_GRAMMAR_MSG_DIFF == nodeName || VR_GRAMMAR_MSG_NEW == nodeName) {
        handleGrammarGenerateMsg(spGrammarMsgDoc);
    }
}

void VR_DataAccessorManager::handleGrammarActiveMsg(pugi::sp_xml_document &spGrammarMsgDoc)
{
    VR_LOGD_FUNC();
    std::string nodeName(spGrammarMsgDoc->first_child().name());
    std::string grammarID(spGrammarMsgDoc->first_child().attribute(VR_GRAMMAR_ID).as_string());
    std::string agent(spGrammarMsgDoc->first_child().attribute(VR_GRAMMAR_AGENT).as_string());
    std::string path(spGrammarMsgDoc->first_child().attribute(VR_GRAMMAR_PATH).as_string());
    int songCount(atoi(spGrammarMsgDoc->first_child().attribute(VR_GRAMMAR_SONG_COUNT).as_string()));
    int otherCount(atoi(spGrammarMsgDoc->first_child().attribute(VR_GRAMMAR_GENERAL_COUNT).as_string()));

    if (VR_GRAMMAR_MSG_ACTIVE == nodeName) {
        if (VR_AGENT_MEDIA == agent) {
            handleMusicActive(true, grammarID, songCount, otherCount, path);
        }
        else if (VR_AGENT_PHONE == agent) {
            handlePhoneActive(true, m_configure->getUsrPath() + path);
        }
    }
    else if (VR_GRAMMAR_MSG_DISACTIVE == nodeName) {
        bool needReply = true;
        if (std::string("false") == spGrammarMsgDoc->first_child().attribute("reply").as_string()) {
            needReply = false;
        }
        if (VR_AGENT_MEDIA == agent) {
            handleMusicActive(false, grammarID, 0, 0, "", needReply);
        }
        else if (VR_AGENT_PHONE == agent) {
            handlePhoneActive(false, "");
        }
    }
}

void VR_DataAccessorManager::handleGrammarGenerateMsg(pugi::sp_xml_document &spGrammarMsgDoc)
{
    VR_LOGD_FUNC();
    // remove old conflict grammar msg
    pugi::xml_node msgNode = spGrammarMsgDoc->first_child();
    if (std::string(VR_GRAMMAR_MSG_INIT) == msgNode.name()) {
        std::string currentAgent = msgNode.attribute(VR_GRAMMAR_AGENT).as_string();
        std::string currentGrammarID = msgNode.attribute(VR_GRAMMAR_ID).as_string();

        VoiceList<pugi::sp_xml_document>::iterator it = m_generateGrammarList.begin();
        while (m_generateGrammarList.end() != it) {
            std::string agent = (*it)->first_child().attribute(VR_GRAMMAR_AGENT).as_string();
            std::string grammarID = (*it)->first_child().attribute(VR_GRAMMAR_ID).as_string();
            if (currentAgent == agent && currentGrammarID == grammarID) {
                it = m_generateGrammarList.erase(it);
                continue;
            }
            ++it;
        }
    }

    m_generateGrammarList.push_back(spGrammarMsgDoc);

    // process next grammar msg
    if (m_pDataSynchronizer->isAsrIdle()) {
        generateGrammar(m_generateGrammarList.front());
        m_generateGrammarList.pop_front();
    }
}

void VR_DataAccessorManager::generateGrammar(pugi::sp_xml_document &spGrammarMsgDoc)
{
    VR_LOGD_FUNC();
    std::string nodeName(spGrammarMsgDoc->first_child().name());
    std::string grammarID(spGrammarMsgDoc->first_child().attribute(VR_GRAMMAR_ID).as_string());
    std::string agent(spGrammarMsgDoc->first_child().attribute(VR_GRAMMAR_AGENT).as_string());
    std::string path(spGrammarMsgDoc->first_child().attribute(VR_GRAMMAR_PATH).as_string());
    int songCount(atoi(spGrammarMsgDoc->first_child().attribute(VR_GRAMMAR_SONG_COUNT).as_string()));
    int otherCount(atoi(spGrammarMsgDoc->first_child().attribute(VR_GRAMMAR_GENERAL_COUNT).as_string()));

    VR_DECommonIF::DE_Country eRegion = m_deCommonIF->getCountryType();
    if (VR_DECommonIF::DE_Country::eu == eRegion) {
        otherCount += atoi(spGrammarMsgDoc->first_child().attribute(VR_GRAMMAR_GENRE_COUNT).as_string());
    }

    if (VR_GRAMMAR_MSG_INIT == nodeName) { // notify ASR update Grammar by read DB
        if (VR_AGENT_MEDIA == agent) {
            handleMediaGrammarInit(path, grammarID, songCount, otherCount);
        }
        else if (VR_AGENT_PHONE == agent) {
            handlePhoneGrammarInit(m_configure->getUsrPath() + path);
        }
    }
    else if (VR_GRAMMAR_MSG_DIFF == nodeName) { // notify ASR update Grammar by diff msg
        if (VR_DECommonIF::DE_Country::cn == eRegion) {
            handleMediaGrammarInit(path, grammarID, songCount, otherCount);
        }
        else {
            pugi::xml_node category;
            category = spGrammarMsgDoc->first_child().first_child();
            handleMediaGrammarDiff(category, grammarID, songCount, otherCount);
        }
    }
    else if (VR_GRAMMAR_MSG_NEW == nodeName) { // notify ASR update Dynamic Criteria
        pugi::xml_node category;
        category = spGrammarMsgDoc->first_child().first_child();
        handleGrammarNew(category);
    }
}

void VR_DataAccessorManager::handleMediaGrammarInit(
    const std::string &path,
    const std::string &grammarID,
    int songCount,
    int otherCount)
{
    VR_LOGD_FUNC();
    VR_LOGP("case:212-9-760 212-10-760 212-11-760 DataAccessor get music full grammar message");
    sqlite3 * dbHandler;
    int result = sqlite3_open_v2(path.c_str(), &dbHandler, SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX, NULL);
    if (SQLITE_OK != result) {
        VR_ERROR("Open DB file %s failed, resultID: [%d]", path.c_str(), result);
        sqlite3_close(dbHandler);
        return;
    }

    VoiceList<std::string>::type grammarList;
    getAvailableCategory(grammarList, songCount, otherCount);

    if (grammarList.empty()) {
        std::string msgToDM(std::string("<grammar_result op=\"grammar\" agent=\"" VR_AGENT_MEDIA "\" grammarid=\"")
        + grammarID
        + "\" errcode=\"1\" />");
        if (m_notifyCallback) {
            m_notifyCallback(msgToDM);
        }
    }
    else {
        if (grammarList.size() < m_musicCategoryList.size()) {
            m_isMusicGrammarDroped = true;
        }
    }

    m_requestUpdateMediaGrammarFinish = false;
    char * errmsg = nullptr;
    std::string sqlRequest;
    std::string sqlCommand("SELECT nExternalId,cName,cPronunciation FROM ");
    std::string categoryName;
    std::string contextID;
    int (*callback)(void* addList, int columnNum, char **columnValue, char **columnName);
    VoiceList<spC_Term>::type addList;
    for (VoiceList<std::string>::iterator it = m_musicCategoryList.begin();
        it != m_musicCategoryList.end();
        ++it) {
        addList.clear();
        categoryName.assign(*it);

        if (!isCategoryAvailable(categoryName)) {
            continue;
        }

        callback = genMusicCTermListWithAlias;

        contextID = m_categoryContextIDMap[categoryName] + grammarID;

        VR_LOGD("ContextID: [%s]", contextID.c_str());

        size_t size = grammarList.size();
        grammarList.remove(categoryName);
        if (grammarList.size() != size) {
            sqlRequest.assign(sqlCommand + getMusicDBTableName(categoryName) + ";");

            result = sqlite3_exec(dbHandler,
                sqlRequest.c_str(),
                callback, &addList,
                &errmsg);
            if (SQLITE_OK != result) {
                VR_ERROR("Run SQL request [%s] failed, error code: [%d], error msg: [%s]",
                    sqlRequest.c_str(),
                    result,
                    errmsg);
                continue;
            }
        }

        VR_LOGD("Records Number: [%d]", addList.size());

        VR_LOGD("Start updateGrammar");
        VR_LOGP("case:212-9-770 212-10-770 212-11-770 DataAccessor generate music full grammar begin");
        updateMusicGrammarState(categoryName, grammarID);
        m_pDataSynchronizer->updateGrammar(
            contextID,
            addList,
            boost::bind(&VR_DataAccessorManager::updateGrammarCallback,
                this,
                _1,
                categoryName),
            true);
    }
    sqlite3_close(dbHandler);
    m_requestUpdateMediaGrammarFinish = true;
}

void VR_DataAccessorManager::handleMediaGrammarDiff(
    pugi::xml_node &category,
    const std::string &grammarID,
    int songCount,
    int otherCount)
{
    VR_LOGD_FUNC();

    VoiceList<std::string>::type grammarList;
    getAvailableCategory(grammarList, songCount, otherCount);

    if (grammarList.empty()) {
        std::string msgToDM(std::string("<grammar_result op=\"grammar\" agent=\"" VR_AGENT_MEDIA "\" grammarid=\"")
        + grammarID
        + "\" errcode=\"1\" />");
        if (m_notifyCallback) {
            m_notifyCallback(msgToDM);
        }
    }
    else {
        if (grammarList.size() < m_musicCategoryList.size()) {
            m_isMusicGrammarDroped = true;
        }
    }

    m_requestUpdateMediaGrammarFinish = false;
    VR_LOGP("case:212-12-760 212-13-760 212-14-760 212-15-760 DataAccessor get music diff grammar message");
    VoiceList<spC_Term>::type addList;
    VoiceList<spC_Term>::type delList;
    std::string categoryName;
    std::string contextID;
    pugi::xml_node tempNode;
    while (!category.empty()) {
        addList.clear();
        delList.clear();
        categoryName.assign(category.attribute(VR_CATEGORY_NAME).as_string());
        size_t size = grammarList.size();
        grammarList.remove(categoryName);

        if (grammarList.size() == size || !isCategoryAvailable(categoryName)) {
            category = category.next_sibling();
            continue;
        }

        contextID = m_categoryContextIDMap[categoryName] + grammarID;

        VR_LOGD("ContextID: [%s]", contextID.c_str());

        // add "delete item"
        tempNode = category.child(VR_DELETE_NODE_NAME);
        if (!tempNode.empty()) {
            tempNode = tempNode.first_child();
            while (!tempNode.empty()) {
                delList.push_back(
                    VR_DataSynchronizer::getCTerm(
                        tempNode.attribute(VR_ITEM_ID).as_int(),
                        std::string(tempNode.attribute(VR_ITEM_NAME).as_string())));
                tempNode = tempNode.next_sibling();
            }
        }

        // add "add item"
        tempNode = category.child(VR_ADD_NODE_NAME);
        if (!tempNode.empty()) {
            tempNode = tempNode.first_child();
            while (!tempNode.empty()) {
                int id(tempNode.attribute(VR_ITEM_ID).as_int());
                std::string name(tempNode.attribute(VR_ITEM_NAME).as_string());
                std::string pron(tempNode.attribute(VR_ITEM_PRON).as_string());
                addList.push_back(
                    VR_DataSynchronizer::getCTerm(
                        id,
                        name,
                        pron));
                // add alias
                VoiceList<std::string>::type aliasList;
                if (getMusicItemAlias(name, aliasList)) {
                    VoiceList<std::string>::iterator it = aliasList.begin();
                    while (it != aliasList.end()) {
                        addList.push_back(
                            VR_DataSynchronizer::getCTerm(
                                id,
                                *it,
                                pron));
                        ++it;
                    }
                }
                tempNode = tempNode.next_sibling();
            }
        }

        if (!addList.empty() || !delList.empty()) {
            VR_LOGD("Start updateGrammar");
            VR_LOGP("case:212-12-770 212-13-770 212-14-770 212-15-770 DataAccessor generate music diff grammar begin");
            updateMusicGrammarState(categoryName, grammarID);
            m_pDataSynchronizer->updateGrammar(
                contextID,
                addList,
                delList,
                boost::bind(&VR_DataAccessorManager::updateGrammarCallback,
                    this,
                    _1,
                    categoryName));
        }
        category = category.next_sibling();
    }
    m_requestUpdateMediaGrammarFinish = true;
}

void VR_DataAccessorManager::handlePhoneGrammarInit(const std::string &path)
{
    VR_PERF_LOG("case:212-9-760 212-10-760 212-11-760 DataAccessor get phone init grammar message");
    VoiceList<spC_Term>::type addList;
    processContactGrammarNew(addList, path);
    updateOtherGrammarState(VR_GRAMMAR_CATEGORY_CONTACT);
    std::string contextID = m_categoryContextIDMap[VR_GRAMMAR_CATEGORY_CONTACT];
    VR_LOGD("ContextID: [%s]", contextID.c_str());
    VR_PERF_LOG("case:212-16-770 212-17-770 212-18-770 DataAccessor generate phonebook grammar begin");
    m_pDataSynchronizer->updateGrammar(
        contextID,
        addList,
        boost::bind(&VR_DataAccessorManager::updateGrammarCallback,
            this,
            _1,
            VR_GRAMMAR_CATEGORY_CONTACT),
        true);
}

void VR_DataAccessorManager::handleGrammarNew(pugi::xml_node &category)
{
    VR_LOGD_FUNC();
    VR_LOGP("case:212-16-760 212-17-760 212-18-760 DataAccessor get grammar_new message");
    VoiceList<spC_Term>::type addList;
    std::string categoryName;
    std::string contextID;

    while (!category.empty()) {
        addList.clear();
        categoryName.assign(category.attribute(VR_CATEGORY_NAME).as_string());

        if (VR_GRAMMAR_CATEGORY_AUDIOSOURCE == categoryName) {
            pugi::xml_node formalNode = category.first_child();
            checkDecklessMode(formalNode);
        }

        if (!isCategoryAvailable(categoryName)) {
            category = category.next_sibling();
            continue;
        }
        contextID = m_categoryContextIDMap[categoryName];
        VR_LOGD("ContextID: [%s]", contextID.c_str());

        // if (VR_GRAMMAR_CATEGORY_AUDIOSOURCE == categoryName) {
        //     pugi::xml_node idNode = category.first_child();
        //     processAudioSourceGrammarNew(idNode, addList);
        // }
        // else if (VR_GRAMMAR_CATEGORY_QUICKREPLYMESSAGE == categoryName) {
        //     pugi::xml_node messageNode = category.first_child();
        //     processQuickReplyMsgGrammarNew(messageNode, addList);
        // }
        // else if (VR_GRAMMAR_CATEGORY_SATCHANNELNUMBER == categoryName) { // is number, not create grammar
        //     pugi::xml_node idNode = category.first_child();
        //     processNoGrammarAndSaveToDB(categoryName, idNode);
        // }
        // else if (VR_GRAMMAR_CATEGORY_PHONETYPE == categoryName) { // phonetype not create grammar
        //     pugi::xml_node idNode = category.first_child();
        //     processNoGrammarAndSaveToDB(categoryName, idNode);
        // }
        // else {
        //     pugi::xml_node idNode = category.first_child();
        //     processOtherGrammarNew(idNode, categoryName, addList);
        // }
        // else {
            VR_LOGD("unhandle category, skip");
            category = category.next_sibling();
            continue;
        // }

        updateOtherGrammarState(categoryName);
        m_pDataSynchronizer->updateGrammar(
            contextID,
            addList,
            boost::bind(&VR_DataAccessorManager::updateGrammarCallback,
                this,
                _1,
                categoryName),
            true);
        category = category.next_sibling();
    }
}

void VR_DataAccessorManager::handleMusicActive(
    bool isActive,
    const std::string &grammarID,
    int songCount,
    int otherCount,
    const std::string &path,
    bool needResponse)
{
    VR_LOGD_FUNC();
    std::string grammarState;
    std::string operation;
    std::string globalActivedSourceID(getState(VR_GLOBALSTATE_ACTIVED_SOURCE_ID));
    if (isActive) {
        if (VR_GLOBALSTATE_VALUE_ZERO != globalActivedSourceID) {
            setMusicGrammarActive(globalActivedSourceID, false, 0, 0);
        }
        m_accessorMedia->openDB(path);

        // update state
        setState(VR_GLOBALSTATE_NUMBER_OF_MUSIC, m_accessorMedia->getSongNumber());
        setState(VR_GLOBALSTATE_NUMBER_OF_ARTIST, m_accessorMedia->getArtistNumber());
        setState(VR_GLOBALSTATE_NUMBER_OF_ALBUM, m_accessorMedia->getAlbumNumber());
        setState(VR_GLOBALSTATE_NUMBER_OF_PLAYLIST, m_accessorMedia->getPlaylistNumber());

        setMusicGrammarActive(grammarID, true, songCount, otherCount);

        int total = songCount + otherCount;
        total = total > VR_MAX_MUSIC_GRAMMAR_COUNT ? otherCount : total;
        std::ostringstream oss;
        oss << total;
        setState(VR_GLOBALSTATE_NUMBER_OF_TOTAL, oss.str());
        setState(VR_GLOBALSTATE_ACTIVED_SOURCE_ID, grammarID);
        setState(VR_GLOBALSTATE_MEDIA_MUSIC_SOURCE_SUPPORT, VR_GLOBALSTATE_VALUE_TRUE);

        grammarState = VR_DICTIONARY_STATE_OK;
        operation = "active";
    }
    else {
        setMusicGrammarActive(grammarID, false, 0, 0);
        if (grammarID != globalActivedSourceID) {
            return;
        }

        m_accessorMedia->closeDB();
        setState(VR_GLOBALSTATE_NUMBER_OF_MUSIC, VR_GLOBALSTATE_VALUE_ZERO);
        setState(VR_GLOBALSTATE_NUMBER_OF_ARTIST, VR_GLOBALSTATE_VALUE_ZERO);
        setState(VR_GLOBALSTATE_NUMBER_OF_ALBUM, VR_GLOBALSTATE_VALUE_ZERO);
        setState(VR_GLOBALSTATE_NUMBER_OF_PLAYLIST, VR_GLOBALSTATE_VALUE_ZERO);
        setState(VR_GLOBALSTATE_NUMBER_OF_TOTAL, VR_GLOBALSTATE_VALUE_ZERO);
        setState(VR_GLOBALSTATE_ACTIVED_SOURCE_ID, VR_GLOBALSTATE_VALUE_ZERO);
        setState(VR_GLOBALSTATE_MEDIA_MUSIC_SOURCE_SUPPORT, VR_GLOBALSTATE_VALUE_FALSE);

        VR_LOG("GlobalState update:\n"
            "[" VR_GLOBALSTATE_NUMBER_OF_TOTAL "]:[%s]\n"
            "[" VR_GLOBALSTATE_NUMBER_OF_MUSIC "]:[%s]\n"
            "[" VR_GLOBALSTATE_NUMBER_OF_ARTIST "]:[%s]\n"
            "[" VR_GLOBALSTATE_NUMBER_OF_ALBUM "]:[%s]\n"
            "[" VR_GLOBALSTATE_NUMBER_OF_PLAYLIST "]:[%s]\n"
            "[" VR_GLOBALSTATE_ACTIVED_SOURCE_ID "]:[%s]\n"
            "[" VR_GLOBALSTATE_MEDIA_MUSIC_SOURCE_SUPPORT "]:[%s]\n"
            , getState(VR_GLOBALSTATE_NUMBER_OF_TOTAL).c_str()
            , getState(VR_GLOBALSTATE_NUMBER_OF_MUSIC).c_str()
            , getState(VR_GLOBALSTATE_NUMBER_OF_ARTIST).c_str()
            , getState(VR_GLOBALSTATE_NUMBER_OF_ALBUM).c_str()
            , getState(VR_GLOBALSTATE_NUMBER_OF_PLAYLIST).c_str()
            , getState(VR_GLOBALSTATE_ACTIVED_SOURCE_ID).c_str()
            , getState(VR_GLOBALSTATE_MEDIA_MUSIC_SOURCE_SUPPORT).c_str());

        grammarState = VR_DICTIONARY_STATE_NONE;
        operation = "disactive";
    }

    VR_LOG("GlobalState update:\n"
        "[" VR_GLOBALSTATE_NUMBER_OF_TOTAL "]:[%s]\n"
        "[" VR_GLOBALSTATE_NUMBER_OF_MUSIC "]:[%s]\n"
        "[" VR_GLOBALSTATE_NUMBER_OF_ARTIST "]:[%s]\n"
        "[" VR_GLOBALSTATE_NUMBER_OF_ALBUM "]:[%s]\n"
        "[" VR_GLOBALSTATE_NUMBER_OF_PLAYLIST "]:[%s]\n"
        "[" VR_GLOBALSTATE_ACTIVED_SOURCE_ID "]:[%s]\n"
        "[" VR_GLOBALSTATE_MEDIA_MUSIC_SOURCE_SUPPORT "]:[%s]\n"
        , getState(VR_GLOBALSTATE_NUMBER_OF_MUSIC).c_str()
        , getState(VR_GLOBALSTATE_NUMBER_OF_TOTAL).c_str()
        , getState(VR_GLOBALSTATE_NUMBER_OF_ARTIST).c_str()
        , getState(VR_GLOBALSTATE_NUMBER_OF_ALBUM).c_str()
        , getState(VR_GLOBALSTATE_NUMBER_OF_PLAYLIST).c_str()
        , getState(VR_GLOBALSTATE_ACTIVED_SOURCE_ID).c_str()
        , getState(VR_GLOBALSTATE_MEDIA_MUSIC_SOURCE_SUPPORT).c_str());
    std::string globalUpdatingSourceID(getState(VR_GLOBALSTATE_UPDATING_SOURCE_ID));
    if (VR_GLOBALSTATE_VALUE_ZERO == globalUpdatingSourceID) {
        setState(VR_GLOBALSTATE_MUSIC_DICTIONARY_STATE, grammarState);
        setState(VR_GLOBALSTATE_ARTIST_DICTIONARY_STATE, grammarState);
        setState(VR_GLOBALSTATE_ALBUM_DICTIONARY_STATE, grammarState);
        setState(VR_GLOBALSTATE_PLAYLIST_DICTIONARY_STATE, grammarState);
        setState(VR_GLOBALSTATE_SONG_DICTIONARY_STATE, grammarState);
        VR_LOG("GlobalState update:\n"
            "[" VR_GLOBALSTATE_MUSIC_DICTIONARY_STATE "]:[%s]\n"
            "[" VR_GLOBALSTATE_ARTIST_DICTIONARY_STATE "]:[%s]\n"
            "[" VR_GLOBALSTATE_ALBUM_DICTIONARY_STATE "]:[%s]\n"
            "[" VR_GLOBALSTATE_PLAYLIST_DICTIONARY_STATE "]:[%s]\n"
            "[" VR_GLOBALSTATE_SONG_DICTIONARY_STATE "]:[%s]\n"
            , getState(VR_GLOBALSTATE_MUSIC_DICTIONARY_STATE).c_str()
            , getState(VR_GLOBALSTATE_ARTIST_DICTIONARY_STATE).c_str()
            , getState(VR_GLOBALSTATE_ALBUM_DICTIONARY_STATE).c_str()
            , getState(VR_GLOBALSTATE_PLAYLIST_DICTIONARY_STATE).c_str()
            , getState(VR_GLOBALSTATE_SONG_DICTIONARY_STATE).c_str());
    }
    if (needResponse && m_notifyCallback) {
        m_notifyCallback(std::string("<grammar_result op=\"")
            + operation
            + "\" agent=\"" VR_AGENT_MEDIA "\" grammarid=\""
            + grammarID
            + "\" errcode=\"0\" />");
    }
    notifyStateUpdated();
}

void VR_DataAccessorManager::handlePhoneActive(bool isActive, const std::string &path, bool needResponse)
{
    VR_LOGD_FUNC();
    std::string operation;
    bool isSwitched = false;
    if (isActive) {
        operation = "active";
        if (!m_isPhoneGrammarActive) {
            m_isPhoneGrammarActive = true;
            isSwitched = true;

            m_accessorContact->openDB(path);
            setPhoneContactGrammarActive(true);
            int contactCount = m_accessorContact->getContactCount();
            if (0 != contactCount) {
                setState(VR_GLOBALSTATE_PHONEBOOK_LIST_EXIST, VR_GLOBALSTATE_VALUE_TRUE);
            }
            else {
                setState(VR_GLOBALSTATE_PHONEBOOK_LIST_EXIST, VR_GLOBALSTATE_VALUE_FALSE);
            }
            setState(VR_GLOBALSTATE_PHONEBOOK_DICTIONARY_STATE, VR_DICTIONARY_STATE_OK);
            VR_LOG("GlobalState [" VR_GLOBALSTATE_PHONEBOOK_LIST_EXIST "] updated [%s]\n"
                "GlobalState [" VR_GLOBALSTATE_PHONEBOOK_DICTIONARY_STATE "] updated [" VR_DICTIONARY_STATE_OK "]"
                , getState(VR_GLOBALSTATE_PHONEBOOK_LIST_EXIST).c_str());
        }
    }
    else {
        operation = "disactive";
        if (m_isPhoneGrammarActive) {
            m_isPhoneGrammarActive = false;
            isSwitched = true;

            setPhoneContactGrammarActive(false);
            m_accessorContact->closeDB();
            setState(VR_GLOBALSTATE_PHONEBOOK_LIST_EXIST, VR_GLOBALSTATE_VALUE_FALSE);
            setState(VR_GLOBALSTATE_PHONEBOOK_DICTIONARY_STATE, VR_DICTIONARY_STATE_NONE);
            VR_LOG("GlobalState [" VR_GLOBALSTATE_PHONEBOOK_LIST_EXIST "] updated [%s]", getState(VR_GLOBALSTATE_PHONEBOOK_LIST_EXIST).c_str());
            VR_LOG("GlobalState [" VR_GLOBALSTATE_PHONEBOOK_DICTIONARY_STATE "] updated [" VR_DICTIONARY_STATE_NONE "]");
        }
    }
    if (needResponse && m_notifyCallback) {
        m_notifyCallback(std::string("<grammar_result op=\"") + operation + "\" agent=\"" VR_AGENT_PHONE "\" errcode=\"0\" />");
    }
    if (isSwitched) {
        notifyStateUpdated();
    }
}

void VR_DataAccessorManager::checkDecklessMode(pugi::xml_node &formalNode)
{
    std::string decklessMode = getState(VR_GLOBALSTATE_DECKLESS_MODE);
    setState(VR_GLOBALSTATE_DECKLESS_MODE, VR_GLOBALSTATE_DECKLESS_MODE_DECKLESS);
    while (!formalNode.empty()) {
        std::string audiosourceName = formalNode.attribute(VR_ITEM_NAME).as_string();
        if ("CD" == audiosourceName) {
            setState(VR_GLOBALSTATE_DECKLESS_MODE, VR_GLOBALSTATE_DECKLESS_MODE_NOT_DECKLESS);
            break;
        }
        formalNode = formalNode.next_sibling();
    }
    std::string globalDecklessMode(getState(VR_GLOBALSTATE_DECKLESS_MODE));
    if (decklessMode != globalDecklessMode) {
        notifyStateUpdated();
    }
}

void VR_DataAccessorManager::processContactGrammarNew(
    VoiceList<spC_Term>::type &addList,
    const std::string &path)
{
    sqlite3 * dbHandler;
    int result = sqlite3_open_v2(path.c_str(), &dbHandler, SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX, NULL);
    if (SQLITE_OK != result) {
        VR_ERROR("Open DB file %s failed, resultID: [%d]", path.c_str(), result);
        sqlite3_close(dbHandler);
        return;
    }

    std::string sqlCommand("SELECT id,full FROM contact");
    char * errmsg = NULL;
    result = sqlite3_exec(dbHandler, sqlCommand.c_str(), genCTermList, &addList, &errmsg);
    if (SQLITE_OK != result) {
        VR_ERROR("Run SQL request [%s] failed, error code: [%d], error msg: [%s]", sqlCommand.c_str(), result, errmsg);
        sqlite3_close(dbHandler);
        return;
    }
    sqlite3_close(dbHandler);
}

// void VR_DataAccessorManager::processQuickReplyMsgGrammarNew(
//     pugi::xml_node &messageNode,
//     VoiceList<spC_Term>::type &addList)
// {
//     while (!messageNode.empty()) {
//         unsigned int id = messageNode.attribute(VR_ITEM_ID).as_int();
//         std::string idStr(messageNode.attribute(VR_ITEM_ID).as_string());
//         std::string name(messageNode.attribute(VR_ITEM_NAME).as_string());
//         addList.push_back(VR_DataSynchronizer::getCTerm(id, name));
//         insertRecord(VR_GRAMMAR_CATEGORY_QUICKREPLYMESSAGE, idStr, name);
//         messageNode = messageNode.next_sibling();
//     }
// }

// void VR_DataAccessorManager::processAudioSourceGrammarNew(
//     pugi::xml_node &idNode,
//     VoiceList<spC_Term>::type &addList)
// {
//     pugi::xml_node nameNode;
//     while (!idNode.empty()) {
//         unsigned int id = idNode.attribute(VR_ITEM_ID).as_int();
//         std::string idStr = idNode.attribute(VR_ITEM_ID).as_string();
//         std::string nameStr = idNode.attribute(VR_ITEM_NAME).as_string();
//         std::string grammarId = idNode.attribute(VR_GRAMMAR_ID).as_string();
//         insertRecord(VR_GRAMMAR_CATEGORY_AUDIOSOURCE, idStr, nameStr, grammarId);

//         nameNode = idNode.first_child();
//         while (!nameNode.empty()) {
//             addList.push_back(
//                 VR_DataSynchronizer::getCTerm(
//                     id,
//                     std::string(nameNode.attribute(VR_ITEM_NAME).as_string()),
//                     std::string(nameNode.attribute(VR_ITEM_PRON).as_string())));
//             nameNode = nameNode.next_sibling();
//         }
//         idNode = idNode.next_sibling();
//     }
// }

// void VR_DataAccessorManager::processNoGrammarAndSaveToDB(
//     const std::string &tableName,
//     pugi::xml_node &idNode)
// {
//     VR_LOGD_FUNC();
//     while (!idNode.empty()) {
//         std::string idStr(idNode.attribute(VR_ITEM_ID).as_string());
//         std::string name(idNode.attribute(VR_ITEM_NAME).as_string());
//         insertRecord(tableName, idStr, name);
//         idNode = idNode.next_sibling();
//     }
// }

// void VR_DataAccessorManager::processOtherGrammarNew(
//     pugi::xml_node &idNode,
//     std::string &categoryName,
//     VoiceList<spC_Term>::type &addList)
// {
//     pugi::xml_node nameNode;
//     while (!idNode.empty()) {
//         unsigned int id = idNode.attribute(VR_ITEM_ID).as_int();
//         std::string idStr = idNode.attribute(VR_ITEM_ID).as_string();
//         std::string nameStr = idNode.attribute(VR_ITEM_NAME).as_string();
//         std::string shortcut = idNode.attribute(VR_ITEM_SHORTCUT).as_string();
//         insertRecord(categoryName, idStr, nameStr, shortcut);

//         if (!shortcut.empty()) {
//             addList.push_back(VR_DataSynchronizer::getCTerm(id, shortcut.c_str()));
//         }
//         nameNode = idNode.first_child();
//         while (!nameNode.empty()) {
//             addList.push_back(
//                 VR_DataSynchronizer::getCTerm(
//                     id,
//                     std::string(nameNode.attribute(VR_ITEM_NAME).as_string()),
//                     std::string(nameNode.attribute(VR_ITEM_PRON).as_string())));
//             nameNode = nameNode.next_sibling();
//         }
//         idNode = idNode.next_sibling();
//     }
// }

void VR_DataAccessorManager::setMusicGrammarActive(
    const std::string &grammarID,
    bool enable,
    int songCount,
    int otherCount)
{
    VoiceList<std::string>::type mediaMainList;
    VoiceList<std::string>::type categoryList;
    if (enable) {
        getAvailableCategory(categoryList, songCount, otherCount);
    }
    else {
        categoryList = m_musicCategoryList;
    }
    std::string categoryName;

    for (VoiceList<std::string>::iterator it = categoryList.begin();
        it != categoryList.end();
        ++it) {
        if ("song" == *it) {
            categoryName = "music";
        }
        else {
            categoryName = *it;
        }

        mediaMainList.push_back(std::string("grm_cmd_media_main#rul_slt_media_play_") + categoryName + "_list_" + grammarID);
        std::string ruleID = std::string("grm_cmd_media_play_")
            + categoryName + "#rul_slt_media_play_"
            + categoryName + "_list_" + grammarID;
        if (enable) {
            grammarActive(std::string("ctx_media_play_") + categoryName, ruleID);
        }
        else {
            grammarDisactive(std::string("ctx_media_play_") + categoryName, ruleID);
        }
    }
    if (enable) {
        grammarActive("ctx_media_main", mediaMainList);
        if ((songCount + otherCount) > VR_MAX_MUSIC_GRAMMAR_COUNT
            && songCount <= VR_MAX_MUSIC_GRAMMAR_COUNT) {
            mediaMainList.remove(std::string("grm_cmd_media_main#rul_slt_media_play_music_list_") + grammarID);
        }
        grammarActive("ctx_topmenu_main", mediaMainList);
    }
    else {
        grammarDisactive("ctx_media_main", mediaMainList);
        grammarDisactive("ctx_topmenu_main", mediaMainList);
    }
}

void VR_DataAccessorManager::setPhoneContactGrammarActive(bool enable)
{
    if (enable) {
        grammarActive("ctx_phone_contact", "grm_cmd_phone_contact#rul_slt_phone_contact_name_list_1");
        grammarActive("ctx_phone_main", "grm_cmd_phone_main#rul_slt_phone_contact_name_list_1");
        grammarActive("ctx_topmenu_main", "grm_cmd_phone_main#rul_slt_phone_contact_name_list_1");
    }
    else {
        grammarDisactive("ctx_phone_contact", "grm_cmd_phone_contact#rul_slt_phone_contact_name_list_1");
        grammarDisactive("ctx_phone_main", "grm_cmd_phone_main#rul_slt_phone_contact_name_list_1");
        grammarDisactive("ctx_topmenu_main", "grm_cmd_phone_main#rul_slt_phone_contact_name_list_1");
    }
}

void VR_DataAccessorManager::grammarActive(const std::string &contextID, const VoiceList<std::string>::type &ruleIDList)
{
    m_pDataSynchronizer->setGrammarActive(contextID, true, ruleIDList);
}

void VR_DataAccessorManager::grammarActive(const std::string &contextID, const std::string &ruleID)
{
    VoiceList<std::string>::type ruleIDList(1, ruleID);
    m_pDataSynchronizer->setGrammarActive(contextID, true, ruleIDList);
}

void VR_DataAccessorManager::grammarDisactive(const std::string &contextID, const VoiceList<std::string>::type &ruleIDList)
{
    m_pDataSynchronizer->setGrammarActive(contextID, false, ruleIDList);
}

void VR_DataAccessorManager::grammarDisactive(const std::string &contextID, const std::string &ruleID)
{
    VoiceList<std::string>::type ruleIDList(1, ruleID);
    m_pDataSynchronizer->setGrammarActive(contextID, false, ruleIDList);
}

bool VR_DataAccessorManager::isCategoryAvailable(const std::string &category)
{
    return (m_categoryContextIDMap.end() != m_categoryContextIDMap.find(category));
}

void VR_DataAccessorManager::getAvailableCategory(VoiceList<std::string>::type &categoryList, int songCount, int otherCount)
{
    int totalCount = otherCount + songCount;
    categoryList = m_musicCategoryList;

    if (totalCount > VR_MAX_MUSIC_GRAMMAR_COUNT) {
        if (otherCount > VR_MAX_MUSIC_GRAMMAR_COUNT) {
            categoryList.clear();
            categoryList.push_back("song");
        }
        if (songCount > VR_MAX_MUSIC_GRAMMAR_COUNT) {
            categoryList.remove("song");
        }
    }
}

VR_DataAccessor * VR_DataAccessorManager::getAccessor(const std::string &operation)
{
    if (m_accessorContact->isOperationHandled(operation)) {
        return m_accessorContact.get();
    }
    else if (m_accessorNavi->isOperationHandled(operation)) {
        return m_accessorNavi.get();
    }
    else if (m_accessorVoiceTag->isOperationHandled(operation)) {
        return m_accessorVoiceTag.get();
    }
    else if (m_accessorMedia->isOperationHandled(operation)) {
        return m_accessorMedia.get();
    }
    else {
        return nullptr;
    }
}

// void VR_DataAccessorManager::createTable()
// {
//     char * errmsg = nullptr;
//     int result;
//     VoiceList<std::string>::type tableNameList;
//     std::string sqlRequest("SELECT name FROM sqlite_master WHERE type='table';");
//     result = sqlite3_exec(m_dbHandler,
//         sqlRequest.c_str(),
//         getSingleColumnList,
//         &tableNameList,
//         &errmsg);
//     if (SQLITE_OK != result) {
//         VR_ERROR("Run SQL request [%s] failed, error code: [%d], error msg: [%s]",
//             sqlRequest.c_str(),
//             result,
//             errmsg);
//         return;
//     }

//     VoiceMap<std::string, std::string>::type tableFormatMap;
//     tableFormatMap.insert(
//         std::pair<std::string, std::string>(
//             VR_DB_TABLENAME_CONTACT,
//             "id integer,first varchar(255),last varchar(255),full varchar(255)"));
//     tableFormatMap.insert(
//         std::pair<std::string, std::string>(
//             VR_DB_TABLENAME_PHONERECORDS,
//             "id integer,typeid integer,number varchar(255)"));
//     tableFormatMap.insert(
//         std::pair<std::string, std::string>(
//             VR_DB_TABLENAME_AUDIOSOURCE,
//             "id integer,name varchar(255),grammarid varchar(255)"));
//     // tableFormatMap.insert(std::pair<std::string, std::string>(VR_DB_TABLENAME_PHONETYPE, "typeid integer,typename varchar(255)"));
//     // tableFormatMap.insert(std::pair<std::string, std::string>(VR_DB_TABLENAME_QUICKREPLYMESSAGE, "id integer,name varchar(255)"));
//     // tableFormatMap.insert(std::pair<std::string, std::string>(VR_DB_TABLENAME_MESSAGETYPE, "typeid integer,typename varchar(255)"));
//     // tableFormatMap.insert(std::pair<std::string, std::string>(VR_DB_TABLENAME_DATA, "id integer,name varchar(255),shortcut varchar(255)")); // navi agent
//     // tableFormatMap.insert(std::pair<std::string, std::string>(VR_DB_TABLENAME_SEARCHAPP, "id integer,name varchar(255)"));
//     // tableFormatMap.insert(std::pair<std::string, std::string>(VR_DB_TABLENAME_FMGENRE, "id integer,name varchar(255)"));
//     // tableFormatMap.insert(std::pair<std::string, std::string>(VR_DB_TABLENAME_SATCHANNELNAME, "id integer,name varchar(255)"));
//     // tableFormatMap.insert(std::pair<std::string, std::string>(VR_DB_TABLENAME_SATCHANNELNUMBER, "id integer,name integer"));
//     // tableFormatMap.insert(std::pair<std::string, std::string>(VR_DB_TABLENAME_SATGENRE, "id integer,name varchar(255)"));
//     // tableFormatMap.insert(std::pair<std::string, std::string>(VR_DB_TABLENAME_HDSUBCHANNEL, "id integer,name varchar(255),intval integer"));
//     // tableFormatMap.insert(std::pair<std::string, std::string>(VR_DB_TABLENAME_APPS, "id integer,name varchar(255),shortcut varchar(255)"));
//     // tableFormatMap.insert(std::pair<std::string, std::string>(VR_DB_TABLENAME_APPSOFFBOARD, "id integer,name varchar(255),shortcut varchar(255)"));

//     std::size_t size = tableNameList.size();
//     for (VoiceMap<std::string, std::string>::iterator it = tableFormatMap.begin();
//         it != tableFormatMap.end();
//         ++it) {
//         tableNameList.remove(it->first);
//         if (size != tableNameList.size()) {
//             size = tableNameList.size();
//         }
//         else {
//             sqlRequest.assign("CREATE TABLE " + it->first + "(" + it->second + ")");
//             result = sqlite3_exec(m_dbHandler, sqlRequest.c_str(), NULL, NULL, &errmsg);
//             if (SQLITE_OK != result) {
//                 VR_ERROR("Run SQL request [%s] failed, error code: [%d], error msg: [%s]",
//                     sqlRequest.c_str(), result, errmsg);
//                 return;
//             }
//         }
//     }
// }

int VR_DataAccessorManager::getSingleColumnList(
    void *stringList,
    int columnNum,
    char **columnValue,
    char **columnName)
{
    if (columnValue[0]) {
        ((VoiceList<std::string>::type*)stringList)->push_back(columnValue[0]);
    }
    return 0;
}

int VR_DataAccessorManager::getSingleColumnValue(
    void *stringValue,
    int columnNum,
    char **columnValue,
    char **columnName)
{
    if (columnValue[0]) {
        *((std::string *)stringValue) = columnValue[0];
    }
    return 0;
}

int VR_DataAccessorManager::genCTermList(
    void *addList,
    int columnNum,
    char **columnValue,
    char **columnName)
{
    if (nullptr == columnValue[0] || nullptr == columnValue[1]) {
        return 0;
    }

    std::string pron;
    if (3 == columnNum && columnValue[2]) {
        pron.assign(columnValue[2]);
    }
    ((VoiceList<spC_Term>::type*)addList)->push_back(
        VR_DataSynchronizer::getCTerm(
            atoi(columnValue[0]),
            std::string(columnValue[1]),
            pron));
    return 0;
}

int VR_DataAccessorManager::genMusicCTermListWithAlias(
    void *addList,
    int columnNum,
    char **columnValue,
    char **columnName)
{
    if (nullptr == columnValue[0] || nullptr == columnValue[1]) {
        return 0;
    }

    int id = atoi(columnValue[0]);
    std::string songName(columnValue[1]);
    std::string pron;
    if (columnValue[2]) {
        pron.assign(columnValue[2]);
    }

    ((VoiceList<spC_Term>::type*)addList)->push_back(
        VR_DataSynchronizer::getCTerm(
            id,
            songName,
            pron));
    VoiceList<std::string>::type aliasList;
    if (getMusicItemAlias(songName, aliasList)) {
        VoiceList<std::string>::iterator it = aliasList.begin();
        while (it != aliasList.end()) {
            ((VoiceList<spC_Term>::type*)addList)->push_back(
                VR_DataSynchronizer::getCTerm(
                    id,
                    *it,
                    pron));
            ++it;
        }
    }
    return 0;
}

bool VR_DataAccessorManager::getMusicItemAlias(const std::string &songName, VoiceList<std::string>::type &aliasList)
{
    if (songName.empty()) {
        return false;
    }

    // remove paired parentheses and square brackets
    string result = songName;
    int number = m_bracketsRule.GlobalReplace("", &result);
    m_consecutiveSpaceRule.GlobalReplace(" ", &result);
    result = trim(result);
    if (result.empty()) {
        return false;
    }

    if (0 != number) {
        aliasList.push_back(result);
        VR_LOGD("Media Alias: %s", result.c_str());
    }

    // process Ft. ft. Feat. feat. and Featuring featuring
    pcrecpp::StringPiece input(result);
    std::string alias;
    while (m_featuringRule.Consume(&input, &alias)) {
        aliasList.push_back(trim(alias));
        VR_LOGD("Media Alias: %s", alias.c_str());
    }
    if (input.size() > 0 && (unsigned)input.size() != result.size()) {
        aliasList.push_back(trim(std::string(input.data())));
        VR_LOGD("Media Alias: %s", input.data());
    }

    if (aliasList.empty()) {
        return false;
    }

    return true;
}

std::string VR_DataAccessorManager::getMusicDBTableName(const std::string &categoryName)
{
    std::string tableName;
    tableName = categoryName;
    tableName[0] = toupper(tableName[0]);
    tableName.assign(std::string(MUSIC_AGENT_PREFIX) + tableName + "s");
    return tableName;
}

// void VR_DataAccessorManager::insertRecord(
//     const std::string &tableName,
//     const std::string &value1,
//     const std::string &value2,
//     const std::string &extendValue)
// {
//     std::string sqlRequest(std::string("INSERT INTO ") + tableName
//         + " VALUES (\"" + value1 + "\", \"" + value2 + "\""
//         + (extendValue.empty() ? "" : (", \"" + extendValue + "\""))
//         + ")");
//     sqlite3_exec(m_dbHandler, sqlRequest.c_str(), NULL, NULL, NULL);
// }

void VR_DataAccessorManager::notifyStateUpdated()
{
    if (m_updateStateCallback) {
        m_updateStateCallback("");
    }
}

void VR_DataAccessorManager::checkMusicGrammarState()
{
    if (!m_requestUpdateMediaGrammarFinish) {
        return;
    }
    std::string globalMusicDicState(getState(VR_GLOBALSTATE_MUSIC_DICTIONARY_STATE));
    std::string globalGenreDicState(getState(VR_GLOBALSTATE_GENRE_DICTIONARY_STATE));
    std::string globalArtistDicState(getState(VR_GLOBALSTATE_ARTIST_DICTIONARY_STATE));
    std::string globalAlbumDicState(getState(VR_GLOBALSTATE_ALBUM_DICTIONARY_STATE));
    std::string globalPlaylistDicState(getState(VR_GLOBALSTATE_PLAYLIST_DICTIONARY_STATE));
    std::string globalSongDicState(getState(VR_GLOBALSTATE_SONG_DICTIONARY_STATE));
    if (VR_DICTIONARY_STATE_SYNCING == globalMusicDicState
        && VR_DICTIONARY_STATE_SYNCING != globalGenreDicState
        && VR_DICTIONARY_STATE_SYNCING != globalArtistDicState
        && VR_DICTIONARY_STATE_SYNCING != globalAlbumDicState
        && VR_DICTIONARY_STATE_SYNCING != globalPlaylistDicState
        && VR_DICTIONARY_STATE_SYNCING != globalSongDicState) {
        setState(VR_GLOBALSTATE_MUSIC_DICTIONARY_STATE, VR_DICTIONARY_STATE_OK);
        VR_LOG("GlobalState [" VR_GLOBALSTATE_MUSIC_DICTIONARY_STATE "] updated [" VR_DICTIONARY_STATE_OK "]");
        std::string errcode;
        if (m_isMusicGrammarDroped) {
            errcode = "1";
            m_isMusicGrammarDroped = false;
        }
        else {
            errcode = "0";
        }
        std::string msgToDM(std::string("<grammar_result op=\"grammar\" agent=\"" VR_AGENT_MEDIA "\" grammarid=\"")
            + getState(VR_GLOBALSTATE_UPDATING_SOURCE_ID)
            + "\" errcode=\""
            + errcode
            + "\" />");
        if (m_notifyCallback) {
            m_notifyCallback(msgToDM);
        }
        setState(VR_GLOBALSTATE_UPDATING_SOURCE_ID, VR_GLOBALSTATE_VALUE_ZERO);
        VR_LOG("GlobalState [" VR_GLOBALSTATE_UPDATING_SOURCE_ID "] updated [%s]", getState(VR_GLOBALSTATE_UPDATING_SOURCE_ID).c_str());
        VR_PERF_LOG("case:212-9-780 212-10-780 212-11-780 212-12-780 212-13-780 212-14-780 212-15-780 DataAccessor generate music grammar end");
    }
}

void VR_DataAccessorManager::initializeState()
{
    m_stateMap.clear();
    pugi::xml_document msgDoc;
    std::string globalStatePath;
    bool isGetData = m_deCommonIF->isGetFromData();
    if (isGetData) {
        globalStatePath = m_configure->getDataPath() + "settings/globalState.xml";
    }
    else {
        globalStatePath = "/system/etc/voicerecog/settings/globalState.xml";
    }
    msgDoc.load_file(globalStatePath.c_str());
    pugi::xpath_node_set nodes = msgDoc.select_nodes("//item");

    for (pugi::xpath_node_set::iterator it = nodes.begin(); it != nodes.end(); ++it) {
        setState(it->node().attribute("key").as_string(), it->node().attribute("value").as_string());
    }

    setState(VR_GLOBALSTATE_UPDATING_SOURCE_ID, VR_GLOBALSTATE_VALUE_ZERO);
    setState(VR_GLOBALSTATE_ACTIVED_SOURCE_ID, VR_GLOBALSTATE_VALUE_ZERO);
    // should add in glabalState.xml
    setState(VR_GLOBALSTATE_PHONE_DEVICE_ID, VR_GLOBALSTATE_VALUE_ZERO);
}

void VR_DataAccessorManager::updateMusicGrammarState(const std::string &categoryName, const std::string &grammarID)
{
    std::string globalUpdatingSourceID(getState(VR_GLOBALSTATE_UPDATING_SOURCE_ID));
    if (grammarID != globalUpdatingSourceID) {
        setState(VR_GLOBALSTATE_UPDATING_SOURCE_ID, grammarID);
        VR_LOG("GlobalState [" VR_GLOBALSTATE_UPDATING_SOURCE_ID "] updated [%s]", grammarID.c_str());
    }
    std::string key = categoryName;
    std::string globalMuiscDicState(getState(VR_GLOBALSTATE_MUSIC_DICTIONARY_STATE));
    if (VR_DICTIONARY_STATE_SYNCING != globalMuiscDicState) {
        setState(VR_GLOBALSTATE_MUSIC_DICTIONARY_STATE, VR_DICTIONARY_STATE_SYNCING);
        VR_LOG("GlobalState [" VR_GLOBALSTATE_MUSIC_DICTIONARY_STATE "] updated [" VR_DICTIONARY_STATE_SYNCING "]");
    }
    std::transform(key.begin(), key.end(), key.begin(), toupper);
    key += VR_GLOBALSTATE_DICTIONARY_STATE_SUFFIX;
    setState(key, VR_DICTIONARY_STATE_SYNCING);
    VR_LOG("GlobalState [%s] updated [" VR_DICTIONARY_STATE_SYNCING "]", key.c_str());

    notifyStateUpdated();
}

void VR_DataAccessorManager::updateOtherGrammarState(const std::string &categoryName)
{
    std::string key = categoryName;
    if (VR_GRAMMAR_CATEGORY_CONTACT == key) {
        key = "PHONEBOOK";
    }
    key += VR_GLOBALSTATE_DICTIONARY_STATE_SUFFIX;
    setState(key, VR_DICTIONARY_STATE_SYNCING);
    VR_LOG("GlobalState [%s] updated [" VR_DICTIONARY_STATE_SYNCING "]", key.c_str());

    notifyStateUpdated();
}

void VR_DataAccessorManager::getFullName(std::string &fullName, const std::string firstName, const std::string lastName, bool isNormal)
{
    if (isNormal) {
        fullName.assign(firstName
            + ((firstName.empty() || lastName.empty()) ? "" : " ")
            + lastName);
    }
    else {
        fullName.assign(lastName + firstName);
    }
}

std::string VR_DataAccessorManager::trim(std::string str)
{
    if (str.empty()) {
        return str;
    }
    str.erase(0, str.find_first_not_of(" "));
    size_t foundPos = str.find_last_not_of(" ");
    if (foundPos != std::string::npos) {
        str.erase(str.find_last_not_of(" ") + 1);
    }
    else {
        str.clear();
    }

    return str;
}

const std::string & VR_DataAccessorManager::getState(const std::string &stateKey)
{
    if (m_stateMap.end() != m_stateMap.find(stateKey)) {
        return m_stateMap.at(stateKey);
    }
    else {
        return VR_GLOBALSTATE_VALUE_EMPTY;
    }
}

void VR_DataAccessorManager::setState(const std::string &stateKey, const std::string &stateValue)
{
    m_stateMap[stateKey] = stateValue;
}

void VR_DataAccessorManager::checkGrammarNeedUpdate()
{
    VR_LOGD_FUNC();
    std::string globalActivedSourceID(getState(VR_GLOBALSTATE_ACTIVED_SOURCE_ID));

    bool grammarNeedUpdate = false;
    VoiceList<pugi::sp_xml_document>::iterator it = m_generateGrammarList.begin();
    while (m_generateGrammarList.end() != it) {
        if (globalActivedSourceID == (*it)->first_child().attribute(VR_GRAMMAR_ID).as_string()
            && (std::string(VR_AGENT_MEDIA) == (*it)->first_child().attribute(VR_GRAMMAR_AGENT).as_string())) {
            grammarNeedUpdate = true;
            break;
        }
        ++it;
    }

    if (grammarNeedUpdate) {
        setState(VR_GLOBALSTATE_GRAMMAR_UPDATE_STATUS, VR_GLOBALSTATE_GRAMMAR_UPDATE_STATUS_NEED_UPDATE);
        m_accessorMedia->setGrammarUpdateStatus(VR_GLOBALSTATE_GRAMMAR_UPDATE_STATUS_NEED_UPDATE);
        VR_LOGD("Grammar Update Status: needUpdate");
    }
    else {
        setState(VR_GLOBALSTATE_GRAMMAR_UPDATE_STATUS, VR_GLOBALSTATE_GRAMMAR_UPDATE_STATUS_NO_CHANGE);
        m_accessorMedia->setGrammarUpdateStatus(VR_GLOBALSTATE_GRAMMAR_UPDATE_STATUS_NO_CHANGE);
        VR_LOGD("Grammar Update Status: noChange");
    }
}

/* EOF */
