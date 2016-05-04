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
 * @file VR_DataAccessorManager.h
 * @brief Declaration file of VR_DataAccessorManager.
 *
 * This file includes the declaration of VR_DataAccessorManager.
 *
 * @attention used for C++ only.
 */

#ifndef VR_DATA_ACCESSOR_MANAGER_H
#define VR_DATA_ACCESSOR_MANAGER_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include "VR_VoiceTagManager.h"
#include "VR_DataSynchronizer.h"
#include "VR_DECommonIF.h"
#include "VR_ConfigureIF.h"

#ifndef VR_MACRO_H
#   include "VR_Macro.h"
#endif

#include "MEM_list.h"
#include "MEM_map.h"
#include "MEM_queue.h"

#include <pcrecpp.h>
#include <boost/function.hpp>
#include <pugixml.hpp>
#include <string>


class sqlite3;
class VR_AsrRequestor;
class VR_DataAccessor;

typedef boost::shared_ptr<VR_DataSynchronizer> spVR_DataSynchronizer;
VR_DECLARE_CLASS_WITH_SMART_PTR(VR_DataAccessorContact);
VR_DECLARE_CLASS_WITH_SMART_PTR(VR_DataAccessorNaviIF);
VR_DECLARE_CLASS_WITH_SMART_PTR(VR_DataAccessorVoiceTag);
VR_DECLARE_CLASS_WITH_SMART_PTR(VR_DataAccessorMedia);

extern const int VR_MAX_MUSIC_GRAMMAR_COUNT;

namespace pugi
{
    typedef boost::shared_ptr<xml_document> sp_xml_document;
}

/**
 * @brief The VR_DataAccessorManager class
 *
 * cache data for DialogEngine and send data to ASR for update grammar
 *
 * attention:
 * grammar_enable phone msg should receive before grammar_new contact msg
 */

class VR_DataAccessorManager : public VR_VoiceTagManager
{
public:
    VR_DataAccessorManager(VR_AsrRequestor* asrRequestor, VR_DECommonIF* common, VR_ConfigureIF* config);
    virtual ~VR_DataAccessorManager();

    // state
    virtual void updateState(const std::string &stateMsg);
    virtual void getUpdateState(std::string& stateMsg);
    virtual void setUpdateStateCallback(boost::function<void(const std::string &)> &callback);
    virtual void setNotifyCallback(boost::function<void(const std::string &)> &callback);

    // grammar
    virtual void updateGrammar(const std::string &grammarMsg);
    virtual void clearAllGrammar();

    virtual bool saveVoiceTagGrammar(const std::string &voiceTagID, const std::string &name, const std::string &phoneme, const std::string &deviceAddress);
    virtual bool deleteVoiceTagGrammar(const VoiceList<std::pair<std::string, std::string>>::type &deleteList, const std::string &deviceAddress);
    virtual bool deleteAllVoiceTagGrammar(const std::string &deviceAddress);
    virtual bool clearVoiceTagGrammar();

    // update voice tag grammar callback
    void voiceTagUpdateGrammarCallback(const N_Vr::N_Asr::C_Event_Phase &phaseEvent, const std::string &deviceAddress);
    void voiceTagDeleteGrammarCallback(const N_Vr::N_Asr::C_Event_Phase &phaseEvent, const std::string &deviceAddress);

    // update grammar callback
    void updateGrammarCallback(const N_Vr::N_Asr::C_Event_Phase &phaseEvent, const std::string &category);
    void clearGrammarCallback(const N_Vr::N_Asr::C_Event_Phase &phaseEvent);

    // info
    virtual void getInfo(const std::string &operation, const std::string &reqMsg, std::string &response);

    virtual void setCountryID(const std::string &countryID);

    // receive recognize status as grammar manager
    void onRecognizeBegin();
    void onRecognizeEnd();

    // handle update grammar finish
    void onUpdateGrammarCategoryFinish(const std::string &category);
    void onUpdateGrammarFinish();

protected:
    spVR_DataSynchronizer m_pDataSynchronizer;
    VoiceList<std::string>::type m_musicCategoryList;
    VoiceMap<std::string, std::string>::type m_categoryContextIDMap;

    // grammar usability
    virtual void setMusicGrammarActive(const std::string &grammarID, bool enable, int songCount, int otherCount);
    virtual void setPhoneContactGrammarActive(bool enable);
    void grammarActive(const std::string &contextID, const VoiceList<std::string>::type &ruleIDList);
    void grammarActive(const std::string &contextID, const std::string &ruleID);
    void grammarDisactive(const std::string &contextID, const VoiceList<std::string>::type &ruleIDList);
    void grammarDisactive(const std::string &contextID, const std::string &ruleID);
    bool isCategoryAvailable(const std::string &category);
    void getAvailableCategory(VoiceList<std::string>::type &categoryList, int songCount, int otherCount);

private:
    boost::function<void(const std::string &)> m_updateStateCallback;
    boost::function<void(const std::string &)> m_notifyCallback;

    sqlite3 * m_dbHandler;
    int m_dbResult;
    std::string m_activedGrammarID;

    spVR_DataAccessorContact m_accessorContact;
    spVR_DataAccessorNaviIF m_accessorNavi;
    spVR_DataAccessorVoiceTag m_accessorVoiceTag;
    spVR_DataAccessorMedia m_accessorMedia;

    VR_ConfigureIF* m_configure;
    VR_DECommonIF* m_deCommonIF;

    bool m_isMusicGrammarDroped;
    bool m_isPhoneGrammarActive;

    bool m_requestUpdateMediaGrammarFinish;

    std::string m_voiceTagContextID;

    VoiceMap<std::string, std::string>::type m_stateMap;

    VoiceQueue<pugi::sp_xml_document>::type m_grammarMsgQuque;
    VoiceList<pugi::sp_xml_document>::type m_generateGrammarList;
    bool m_bIsRecognizing;

    // regex expression for music alias
    static pcrecpp::RE m_bracketsRule;
    static pcrecpp::RE m_featuringRule;
    static pcrecpp::RE m_consecutiveSpaceRule;

    // handle grammar msg
    void handleGrammarMsg(pugi::sp_xml_document &spGrammarMsgDoc);
    void handleGrammarActiveMsg(pugi::sp_xml_document &spGrammarMsgDoc);
    void handleGrammarGenerateMsg(pugi::sp_xml_document &spGrammarMsgDoc);
    void generateGrammar(pugi::sp_xml_document &spGrammarMsgDoc);

    // handle grammar update
    void handleMediaGrammarInit(const std::string &path, const std::string &grammarID, int songCount, int otherCount);
    void handlePhoneGrammarInit(const std::string &path);
    void handleMediaGrammarDiff(pugi::xml_node &category, const std::string &grammarID, int songCount, int otherCount);
    void handleGrammarNew(pugi::xml_node &category);

    // handle grammar active
    void handleMusicActive(bool isActive, const std::string &grammarID, int songCount, int otherCount, const std::string &path, bool needResponse = true);
    void handlePhoneActive(bool isActive, const std::string &path, bool needResponse = true);

    // check deckless mode
    void checkDecklessMode(pugi::xml_node &formalNode);

    // process grammar new message
    void processContactGrammarNew(VoiceList<spC_Term>::type &addList, const std::string &path);
    // void processQuickReplyMsgGrammarNew(pugi::xml_node &messageNode, VoiceList<spC_Term>::type &addList);
    // void processAudioSourceGrammarNew(pugi::xml_node &idNode, VoiceList<spC_Term>::type &addList);
    // void processNoGrammarAndSaveToDB(const std::string &tableName, pugi::xml_node &idNode);
    // void processOtherGrammarNew(pugi::xml_node &idNode, std::string &categoryName, VoiceList<spC_Term>::type &addList);

    // info
    VR_DataAccessor * getAccessor(const std::string &operation);

    // init
    void createTable();

    // sqlite3 callback
    static int getSingleColumnList(void* stringList, int columnNum, char **columnValue, char **columnName);
    static int getSingleColumnValue(void* stringValue, int columnNum, char **columnValue, char **columnName);
    static int genCTermList(void* addList, int columnNum, char **columnValue, char **columnName);
    static int genMusicCTermListWithAlias(void* addList, int columnNum, char **columnValue, char **columnName);
    static int getRecordCountCallback(void *number, int columnNum, char **columnValue, char **columnName);

    static bool getMusicItemAlias(const std::string &songName, VoiceList<std::string>::type &aliasList);

    // DataAccessor DB
    int getRecordCount(sqlite3 *dbHandler, const std::string &tableName);
    std::string getMusicDBTableName(const std::string &categoryName);
    void insertRecord(const std::string &tableName, const std::string &value1, const std::string &value2, const std::string &extendValue = std::string());

    // state
    void notifyStateUpdated();
    void initializeState();
    void updateMusicGrammarState(const std::string &categoryName, const std::string &grammarID);
    void updateOtherGrammarState(const std::string &categoryName);
    void checkMusicGrammarState();

    void getFullName(std::string &fullName, const std::string firstName, const std::string lastName, bool isNormal = true);
    static std::string trim(std::string str);
    const std::string & getState(const std::string &stateKey);
    void setState(const std::string &stateKey, const std::string &stateValue);

    // check grammar need update
    void checkGrammarNeedUpdate();
};



#endif /* VR_DATA_ACCESSOR_MANAGER_H */
/* EOF */
