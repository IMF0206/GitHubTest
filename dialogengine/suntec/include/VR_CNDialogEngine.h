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
 * @file VR_CNDialogEngine.h
 * @brief dialog engine cn class define
 *
 *
 * @attention used for C++ only.
 */

#ifndef VR_CNDIALOGENGINE_H
#define VR_CNDIALOGENGINE_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include "config.h"
#include "VR_DataProcessor.h"
#include "uscxml/plugins/invoker/vr/VRServiceRequestor.h"
#include "VR_DialogEngineListener.h"
#include "VR_DialogEngineIF.h"
#include "VR_MonitorForBack.h"
#include "uscxml/concurrency/BlockingQueue.h"
#include "pugixml.hpp"
#include "MEM_stack.h"
#include "MEM_map.h"
#include "uscxml/Message.h"
#include "VR_InterpreterManager.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "BL_Base.h"
#include <atomic>
#include <boost/scoped_array.hpp>
#include <boost/shared_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "Vr_Asr_Engine.h"
#include "Vr_Asr_Event_Notify.h"
#include "VR_DataAccessorManagerCN.h"
#include "VR_AsrRequestor.h"
#include "VR_AudioInSource.h"
#include "VR_DECommonIF.h"
#include "VR_ConfigureIF.h"
#include "VR_DEMessageBuilder.h"
#include "VR_IntentionParser.h"
#include "VR_CommuMediation.h"

VR_DECLARE_CLASS_WITH_SMART_PTR(VR_DEDataManager); // contain language and country

namespace uscxml
{
class Event;
}

/**
 * @brief The VR_DialogController class
 *
 * dialog controller class
 */
class VR_API VR_CNDialogEngine : public VR_DialogEngineIF, public VRServiceRequestor, public VR_MonitorForBack, public VR_AsrRequestor
{
public:
    struct BackStateInfo
    {
        BackStateInfo()
        {

        }

        std::string agentName;
        std::string stateName;
        std::string stateAttr;
    };

    enum ResourceState
    {
        READY = 0, // REVIEW: default
        GRAMMAR_NOT_READY = 1,
        ASR_LANGUAGE_NOT_RESOURCE = 2
        // REVIEW: add busy, running
     };

public:
    VR_CNDialogEngine(VR_ConfigureIF* configureInstance);
    virtual ~VR_CNDialogEngine();

    virtual bool Initialize(VR_DialogEngineListener* listerner, const VR_Settings &settings);
    virtual bool Start();
    virtual std::string getHints(const std::string& hintsParams) override;
    virtual void Stop();
    virtual bool SendMessage(const std::string& event, int actionSeqId = -1);
    virtual void UnInitialize();


    // virtual VR_ServiceRequestor
    virtual void requestService(const uscxml::Event &interalEvent);

    // ASR callbacks with Boost bind
    virtual bool updateGrammar(N_Vr::N_Asr::C_Request_Context_List_Update& updateMsg);
    virtual bool genVoiceTagPhoneme(); // REVIEW: remove
    virtual void setGrammarActive(const std::string &contextID, bool isActive, const VoiceList<std::string>::type &ruleIDList);
    virtual void updateGrammarCategoryFinish(const std::string &category);
    virtual void updateGrammarFinish();

    void onAsrPhase(N_Vr::N_Asr::C_Event_Phase const& phase);
    void onAsrNotify(N_Vr::N_Asr::C_Event_Notify const& notify);
    void onAsrResult(N_Vr::N_Asr::C_Event_Result const& result);

    // REVIEW: remove, only for voicetag login
    void onVoiceTagPhase(N_Vr::N_Asr::C_Event_Phase const& phase);
    void onVoiceTagNotify(N_Vr::N_Asr::C_Event_Notify const& notify);
    void onVoiceTagResult(N_Vr::N_Asr::C_Event_Result const& result);

    // for monitor inherit to VR_DialogEngineIFM
    void pushTmpToStack();
    void saveTmpState(const std::string& stateName, const std::string& stateAttr);
    std::string getTmpAttr();
    void clearSameAgentState();
    void clearTmpState();
    void saveCurrentState(const std::string& stateName, const std::string& stateAttr);

protected:
    VR_DISALLOW_EVIL_CONSTRUCTORS(VR_CNDialogEngine);

    typedef void(VR_CNDialogEngine::*ptr_handleFunc)(uscxml::Event&);
    VoiceMap<std::string, ptr_handleFunc>::type _eventHandles;
    VoiceMap<std::string, ptr_handleFunc>::type _asyncEventHandles;
    VR_DataAccessorManagerCN* _dataAccessorManager;
    std::string m_tempPronunc; // REVIEW: remove, china no voicetag
    N_Vr::N_Asr::C_Event_Notify::E_Event_Notify_Type m_voiceTagNotifyType; // REVIEW: remove

    // init in construct
    VR_DialogEngineListener* _actionListener;
    VR_InterpreterManager* _interManager;
    boost::shared_ptr<VR_AudioInSource>  _audioInSource;
    tthread::thread* _thread; // REVIEW: modify to BL_THREAD
    tthread::recursive_mutex _mutex; // REVIEW:
    tthread::condition_variable _condVar; // REVIEW:

    // data init in Initilize fun
    uscxml::concurrency::BlockingQueue<uscxml::Event*> _internalServiceQueue;
    VoiceStack<BackStateInfo>::type _stateSatck; // back
    std::string _currentAgent;
    std::string _lastAgent; // RVIEW: remove
    std::pair<std::string, std::string> _currentIntention;
    spVR_DEDataManager _deDataManager;

    std::string _backStateName; // save the state name in backing, it is also the mark for transition when changeAgent
    bool _resendEvent; // REVIEW: only for back to topmenu is false
    std::string _forward_back;
    BackStateInfo _tmpStateBack;
    BackStateInfo _currentStateBackChangeAgent;
    std::atomic<bool> _isRunning;
    int _seqId;
    VR_DataProcessor _dataProcessor;
    VoiceMap<std::string, std::string>::type _setting; // REVIEW:remove
    uscxml::Event _currentEvent; // REVIEW: remove
    bool _continueAsr; // REVIEW:
    bool _interrputRecv;
    N_Vr::N_Asr::C_Engine* _asrEngine;
    std::string _grammarStr; // REVIEW: for startOver
    int _asrResultId; // REVIEW: put into dataProcessor

    // REVIEW: for asr address recog
    enum AddressType
    {
        address_state = 0,
        address_city = 1,
        address_district = 2
    };

    struct  AddressForAsr
    {
        bool s_bAddress;
        AddressType s_enAddressType;
        std::string s_strJsonProv;
        std::string s_strJsonProvCity;
    };

    AddressForAsr _addressForAsr; // REVIEW: remove and put into scxml dataModel
    //

//    tthread::recursive_mutex _mutexAsr;
    tthread::condition_variable _condVarAsr;
    bool _asrIsRunning;
    bool _changeLanguage;
    std::string _languageId;
    bool _needBargein; // REVIEW: remove
    ResourceState _resourceState; // REVIEW:

    enum SessionState
    {
        SESSION_IDLE,
        SESSION_PREPARE,
        SESSION_RUNNING,
        SESSION_QUITING,
        SESSION_QUITING_WAITING
    };

    SessionState _sessionState;

    int _lastEndBeepSeqId;
    int _lastStartBeepSeqId;
    int _lastTtsSeqId;
    int _lastStopBeepSeqId;
    int _lastStopTtsSeqId;

    // -------------------------------
    bool _bMapDataPreparing;
    bool _songDisableSceneFlag;
    bool _bDisableTopmenuSong;
    // cmnid to tagnames map
    VoiceMap<std::string, VoiceVector<std::string>::type>::type m_cmnIdTagnamesMap;
    tthread::recursive_mutex _mutexRequest;


    VR_ConfigureIF* m_pConfigureIF;
    VR_DECommonIF* m_pDECommonIF;
    VR_DEMessageBuilder* m_pDEMessageBuilder;

    int m_iVehiclePosLong;
    int m_iVehiclePosLat;
    std::string m_strAsrMode;
    VR_IntentionParser* m_pIntentionParser;
    std::string m_strCommunicationStatus;
    bool m_bCommunicationStatusFlag;
    tthread::recursive_mutex m_mutexCommunicationStatus;
    tthread::condition_variable m_condVarCommunicationStatus;

    bool _asrReturnBeepPlayed;
    bool _audioSessionState;
    VoiceList<std::string>::type optionList;
    tthread::recursive_mutex _mutexOption;

    boost::shared_ptr<VR_CommuMediation> m_spCommuMediation;

    bool m_isAsrReadyFirstTime;
    bool m_isAgentReadyFirstTime;

private:
    void doStop();
    void loadHandleEventFuncMap();
    std::string namelistToStr(uscxml::Event& reqCopy, const std::string& dataName);
    std::string namelistToStr(uscxml::Event& reqCopy);

    static void run(void* instance); // for thread
    void doRun();
    void step(uscxml::Event* pEvent);
    void responseAsrError();
    // void initDic();
    // void initVRState();

    int  transformPhoneTypeID(int iflytekTypeId);
    std::string parseAsrToIntention(const std::string& xml);
    std::string parseAsrInfoStr(const std::string& xml);
    // void loadJsonData(const std::string& filePath, uscxml::Data& outData);
    VoiceList<std::string>::type parserPrompt(pugi::xml_node& doc);
    void parserPromptReplaceKeyWithValue(std::string& promptStr, VoiceMap<std::string, std::string>::type& dicMap);
    void parserVRState(pugi::xml_node& doc);
    void resetAsr(bool enableNavi);
    void clearTempData();
    void setItemValue(pugi::xml_node doc, const std::string skey, const std::string svalue);
  //  static void closeDMSession(void* instance);
  //  void waitCloseSession();
    void closeSession(uscxml::Event& reqCopy);
    // external event
    void processActionResultFromDM(const std::string& eventStr, int seqId);
    void processGrammarFromDM(const std::string& eventStr);
    void processEventFromDM(const std::string& eventStr);
    void processStartOverEvent();
    void processCancelEvent(const std::string& option);
    bool processStartAgentEvent(const pugi::xml_node& eventNode);
    bool processPrepareAgentEvent(const pugi::xml_node& eventNode);
    void processGetHintsEvent(const pugi::xml_node& eventNode);
    void processStartDictationEvent();
    void processUpdateStateEvent(const std::string& eventStr);
    void processSettingEvent(const pugi::xml_node& eventNode);
    void processDoneEvent(const std::string& eventName, int seqId);
    void processChangeLanguage(const pugi::xml_node& language);
    void languageParamSet();
    void processInitializePersonData();
    void processUpdateCommStatusEvent(const pugi::xml_node& eventNode);

    void doHandleAsr(uscxml::Event& reqCopy);
    // internal event
    void requestVR(const std::string& grammer);
    int requestAction(const std::string& action);
    void handlePreInitInterpreter(uscxml::Event& evt);
    void handlePreUpdateData(uscxml::Event& reqCopy);
    void handleDoBack(uscxml::Event& evt);
    void handleAsr(uscxml::Event& evt);
    void handleTTS(uscxml::Event& evt);
    void handleDisplay(uscxml::Event& evt);
    void handleFetchItem(uscxml::Event& evt);
    void handleCloseSession(uscxml::Event& evt);
    void handleFetchData(uscxml::Event& evt);
    void handleResendAsrEvent(uscxml::Event& evt);
    void handleAction(uscxml::Event& evt);
    void handleInterrupt(uscxml::Event& evt);
    void interruptActionResult();
    void handleReturnKeyOrButton(uscxml::Event& evt);
    bool stopTTS(uscxml::Event& evt);
    void handleTTSWithAsr(uscxml::Event& evt);
    void postEvent(uscxml::Event& evt);
    void handleAsrStartover(uscxml::Event& evt);
    void handleAsrImmediately(uscxml::Event& evt);
    void handlePopCurrentAgentStacks(uscxml::Event& evt);
    void handleBack(uscxml::Event& evt);
    void handleChangeAgent(uscxml::Event& evt);
    void handleDoChangeAgent(uscxml::Event& evt);
    void handleBackAgent(uscxml::Event& evt);
    void handleInitAsr(uscxml::Event& evt);
    void handleInitInterpreter(uscxml::Event& evt);
    void handleStartOverEvent(uscxml::Event& evt);
    void handleSendMessage(uscxml::Event& evt);
    void handleGetWebSearchResult(uscxml::Event& evt);
    void handleGetCommunicationStatus(uscxml::Event& evt);
    void handleAsrEvent(uscxml::Event& evt);

    void handleQuitVRApp(uscxml::Event& evt);
    void handleUpdateGrammarCategoryFinish(uscxml::Event& evt);
    void handleUpdateGrammarFinish(uscxml::Event& evt);
    void handleAgentReadyFirstTime(uscxml::Event& evt);
    void quitByError(uscxml::Event& evt);

    // invoke in uscxml thread
    void handleDoUpdateDataModel(uscxml::Event& reqCopy);
    void handleReceiveOpResult(uscxml::Event& reqCopy);

    int reqPlayBeep(const std::string& beepPath);
    int reqPlayTTS(const std::string& ttsTxt);
    int reqPlayTTS(const VoiceList<std::string>::type& txtList);
    void playTTS(uscxml::Event& evt, bool continueAsr);

    void parseAsrGrammar(uscxml::Event& reqCopy);
    std::string changeGrammarIDForAsr(const std::string &grammar);

    void onStateUpdate(const std::string &stateMsg);

    void addGrammarData();

    bool GetDataTagName(VoiceMap<std::string, std::string>::type & mapSlotValue, std::string strCMD, int iID, std::string strName);

    // tagnames for asrjson subObjs map
    void readTagDataConf(const std::string strConfFilePath);
    // add intentAsr parse for chinaVR
    std::string parseAsrAddressStrNBst(const std::string& xml);
    std::string parseAsrPOIStrNBst(const std::string& xml);

    bool getPOIFromWebData(const std::string& objData, std::string& xmlStr);
    bool getInformationFromWebData(const std::string& objData, std::string& xmlStr);

    void initPhoneTypeName();

    // add for CN Climate state
    void updateStateClimateMap(const std::string &stateMsg);
    bool checkClimateIntentIsValid(const std::string strCmnId);

    int  networkStatusChanged(int status);
    // for sessionStateChanged
    void sessionStateSet(bool val);
    bool sessionStateGet();

    // for resourceState Msg Format Change
    void resourceStateChange(std::string type, ResourceState value);

    // for cancel option Msg
    void replyCancelAndReady();
    void replyCancel();

    // for preInit updatestate
    void doStateUpdate(const std::string &msgToDM, bool notifly);

    // remove same contact name, get full contact id by search DB and set the max confidence phonetypeid
    void preprocessPhoneTypeResult(pugi::xml_node &result);

    void notifyStartFinishAfterCheck();

    // for prepare message done.preinit
    std::string buildInitParm();

    // for "goto returnStep"
    void responseEventResult(pugi::xml_node &eventNode);
};

#endif
/* EOF */
