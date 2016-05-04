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
 * @file VR_EUDialogEngine.h
 * @brief dialog engine class define
 *
 *
 * @attention used for C++ only.
 */

#ifndef VR_EU_DIALOGENGINE_H
#define VR_EU_DIALOGENGINE_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include "config.h"
#include "VR_DataProcessor.h"
#include "VR_DialogEngineListener.h"
#include "VR_DialogEngineIF.h"
#include "VR_MonitorForBack.h"
#include "uscxml/concurrency/BlockingQueue.h"
#include "uscxml/plugins/invoker/vr/VRServiceRequestor.h"
#include "pugixml.hpp"

#include "MEM_stack.h"
#include "MEM_map.h"
#include "uscxml/Message.h"
#include "VR_InterpreterManager.h"

#include "BL_Base.h"
#include <boost/scoped_array.hpp>
#include <boost/shared_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <chrono>
#include "Vr_Asr_Engine.h"
#include "Vr_Asr_Event_Notify.h"
#include "VR_DataAccessorManager.h"
#include "VR_AsrRequestor.h"
#include "VR_AudioInSource.h"
#include "VR_DECommonIF.h"
#include "VR_ConfigureIF.h"
#include "VR_DEMessageBuilder.h"
#include "VR_IntentionParser.h"

VR_DECLARE_CLASS_WITH_SMART_PTR(VR_DEDataManager); // contain language and country
typedef boost::shared_ptr<N_Vr::N_Asr::C_Request_Activate> spC_Request_Activate;

namespace uscxml
{
class Event;
}

/**
 * @brief The VR_DialogController class
 *
 * dialog controller class
 */
class VR_API VR_EUDialogEngine : public VR_DialogEngineIF, public VRServiceRequestor, public VR_MonitorForBack, public VR_AsrRequestor
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
        READY = 0,
        GRAMMAR_NOT_READY = 1,
        ASR_LANGUAGE_NOT_RESOURCE = 2
     };

    enum PlayedType
    {
        NONE = 0,
        TTS = 1,
        BEEP = 2
    };

public:
    VR_EUDialogEngine(VR_ConfigureIF* configureInstance);
    virtual ~VR_EUDialogEngine();

    virtual bool Initialize(VR_DialogEngineListener* listerner, const VR_Settings &settings);
    virtual std::string getHints(const std::string& hintsParams) override;
    virtual bool Start();
    virtual void Stop();
    virtual bool SendMessage(const std::string& event, int actionSeqId = -1);
    virtual void UnInitialize();


    // virtual VR_ServiceRequestor
    virtual void requestService(const uscxml::Event &interalEvent);

    // ASR callbacks with Boost bind
    virtual bool updateGrammar(N_Vr::N_Asr::C_Request_Context_List_Update& updateMsg);
    virtual bool genVoiceTagPhoneme()
    {
        return true;
    }

    virtual void setGrammarActive(const std::string &contextID, bool isActive, const VoiceList<std::string>::type &ruleIDList);
    virtual void updateGrammarCategoryFinish(const std::string &category);
    virtual void updateGrammarFinish();

    void onAsrPhase(N_Vr::N_Asr::C_Event_Phase const& phase);
    void onAsrNotify(N_Vr::N_Asr::C_Event_Notify const& notify);
    void onAsrResult(N_Vr::N_Asr::C_Event_Result const& result);

    // for monitor inherit to VR_DialogEngineIFM
    void pushTmpToStack();
    void saveTmpState(const std::string& stateName, const std::string& stateAttr);
    std::string getTmpAttr();
    void clearSameAgentState();
    void clearTmpState();
    void saveCurrentState(const std::string& stateName, const std::string& stateAttr);


protected:
    VR_DISALLOW_EVIL_CONSTRUCTORS(VR_EUDialogEngine);

    typedef void(VR_EUDialogEngine::*ptr_handleFunc)(uscxml::Event&);
    VoiceMap<std::string, ptr_handleFunc>::type _eventHandles;
    VoiceMap<std::string, ptr_handleFunc>::type _asyncEventHandles;
    VoiceMap<std::string, spC_Request_Activate>::type _grammarDeactiveMap;
    VR_DataAccessorManager* _dataAccessorManager;
    PlayedType m_lastPlayed;

    // init in construct
    VR_DialogEngineListener* _actionListener;
    VR_InterpreterManager* _interManager;
    boost::shared_ptr<VR_AudioInSource>  _audioInSource;
    tthread::thread* _thread;
    tthread::recursive_mutex _mutex;
    tthread::condition_variable _condVar;

    // data init in Initilize fun
    uscxml::concurrency::BlockingQueue<uscxml::Event*> _internalServiceQueue;
    VoiceStack<BackStateInfo>::type _stateSatck; // back
    std::string _currentAgent;
    std::string _lastAgent;
    std::pair<std::string, std::string> _currentIntention;
    spVR_DEDataManager _deDataManager;

    std::string _backStateName; // save the state name in backing, it is also the mark for transition when changeAgent
    bool _resendEvent;
    std::string _forward_back;
    BackStateInfo _tmpStateBack;
    BackStateInfo _currentStateBackChangeAgent;
    bool _isRunning;
    int _seqId;
    VR_DataProcessor _dataProcessor;
    VoiceMap<std::string, std::string>::type _setting;
    uscxml::Event _currentEvent;
    bool _continueAsr;
    N_Vr::N_Asr::C_Engine* _engine;
    std::string _grammarStr;
    int _asrResultId;

    enum AddressType
    {
        address_state = 0,
        address_city = 1,
        address_street = 2
    };

    struct  AddressForAsr
    {
        bool s_bAddress;
        AddressType s_enAddressType;
        unsigned int s_id;
    };

    AddressForAsr _addressForAsr;

    tthread::recursive_mutex _mutexAsr;
    tthread::condition_variable _condVarAsr;
    bool _asrIsRunning;
    bool _changeLanguage;
    std::string _languageId;
    bool _needBargein;
    bool _interrputRecv;
    ResourceState _resourceState;
    std::string _countrySearchId;

    tthread::recursive_mutex _mutexTTS;
    tthread::recursive_mutex _mutexBEEP;
    tthread::condition_variable _condVarTTS;
    tthread::condition_variable _condVarBEEP;
    VoiceList<int>::type _listPlayTTsSeq;
    VoiceList<int>::type _listPlayBeepSeq;
    // the seqIds of play TTS that need to stop
    VoiceList<int>::type _listStopTTsSeq;
    // the seqIds of play BEEP that need to stop
    VoiceList<int>::type _listStopBeepSeq;

    tthread::recursive_mutex _mutexStartBeep;
    tthread::condition_variable _condVarStartBeep;
    int m_startBeepSeqId;
    int m_startBeepLatency;

    bool _bHasQuitVRApp;
    bool _bHasCloseSession;
    bool _bNaviFullData;
    bool _bMusicFilter;

    tthread::recursive_mutex _mutexRequest;
    VR_ConfigureIF* m_pConfigureIF;
    VR_DECommonIF* m_pDECommonIF;
    VR_DEMessageBuilder* m_pDEMessageBuilder;
    VR_IntentionParser* m_pIntentionParser;
    bool m_isAsrReturnPlayBeep;

    std::chrono::system_clock::time_point m_audioInPrepareTime;
    bool m_sessionState;
    // std::string m_option;
    VoiceList<std::string>::type optionList;
    tthread::recursive_mutex _mutexOption;

    enum CancelType
    {
        CANCEL_NONE = 0,
        CANCEL_PENDING = 1
    };

    CancelType _canceltype;

    bool m_isAsrReadyFirstTime;
    bool m_isAgentReadyFirstTime;
   
private:
    void doStop();
    void loadHandleEventFuncMap();
    void initGrammarDeactiveMap();
    std::string namelistToStr(uscxml::Event& reqCopy, const std::string& dataName);
    std::string namelistToStr(uscxml::Event& reqCopy);

    static void run(void* instance); // for thread
    void doRun();
    void step(uscxml::Event* pEvent);
    void quitVRApp();
    void responseAsrError();
    // void initDic();
    // void initVRState();

    std::string parseAsrToIntention(pugi::xml_node doc);
    // void loadJsonData(const std::string& filePath, uscxml::Data& outData);
    VoiceList<std::string>::type parserPrompt(pugi::xml_node& doc);
    void parserPromptReplaceKeyWithValue(std::string& promptStr, VoiceMap<std::string, std::string>::type& dicMap);
    void parserVRState(pugi::xml_node& doc);
    void removeSpaceInAsrResult(pugi::xml_node asrNode);
    void resetAsr(bool mock);
    void clearTempData();
    void setItemValue(pugi::xml_node doc, const std::string skey, const std::string svalue);
    static void closeDMSession(void*);

    // external event
    void processActionResultFromDM(const std::string& eventStr, int seqId);
    void processGrammarFromDM(const std::string& eventStr);
    void processEventFromDM(const std::string& eventStr);
    void processStartOverEvent();
    void processCancelEvent(std::string& option);
    bool processStartAgentEvent(const pugi::xml_node& eventNode);
    bool processPrepareAgentEvent(const pugi::xml_node& eventNode);
    void processGetHintsEvent(const pugi::xml_node& eventNode);
    void processStartDictationEvent();
    void processUpdateStateEvent(const std::string& eventStr);
    void processSettingEvent(const pugi::xml_node& eventNode);
    void processActionResult(const std::string& eventName, int seqId);
    void processInitializePersonData();
    void processChangeLanguage(const pugi::xml_node& language);
    void waitCloseSession(std::string& option);

    void processStartBeepEnd(int seqId);

    void postEvent(uscxml::Event& reqCopy);

    void doHandleAsr(uscxml::Event& reqCopy);
    // internal event
    void requestVR(const std::string& grammer);
    int requestAction(const std::string& action);
    void handlePreInitInterpreter(uscxml::Event& reqCopy);
    void handlePreUpdateData(uscxml::Event& reqCopy);
    void handleDoBack(uscxml::Event& reqCopy);
    void handleAsr(uscxml::Event& reqCopy);
    void handleTTS(uscxml::Event& reqCopy);
    void handleTTSOnly(uscxml::Event& reqCopy);
    void handleDisplay(uscxml::Event& reqCopy);
    void handleFetchItem(uscxml::Event& reqCopy);
    void handleCloseSession(uscxml::Event& reqCopy);
    void handleFetchData(uscxml::Event& reqCopy);
    void handleResendAsrEvent(uscxml::Event& reqCopy);
    void handleAction(uscxml::Event& reqCopy);
    void handleInterrupt(uscxml::Event& reqCopy);
    void interruptActionResult();
    void handleReturnKeyOrButton(uscxml::Event&);
    void handleTTSStop(uscxml::Event& reqCopy);
    void handleTTSWithAsr(uscxml::Event& reqCopy);

    void handleAsrStartover(uscxml::Event& reqCopy);
    void handleAsrImmediately(uscxml::Event& reqCopy);
    void handlePopCurrentAgentStacks(uscxml::Event& reqCopy);
    void handleIsAddrSupported(uscxml::Event& reqCopy);
    void handleBack(uscxml::Event& evt);
    void handleChangeAgent(uscxml::Event& evt);
    void handleDoChangeAgent(uscxml::Event& evt);
    void handleBackAgent(uscxml::Event& evt);
    void handleInitAsr(uscxml::Event& evt);
    void handleInitInterpreter(uscxml::Event& evt);
    void handleStartOverEvent(uscxml::Event& evt);
    void handleSendMessage(uscxml::Event& evt);
    void handleQuitVRApp(uscxml::Event& evt);
    void handleUpdateGrammarCategoryFinish(uscxml::Event& evt);
    void handleUpdateGrammarFinish(uscxml::Event& evt);
    // invoke in uscxml thread
    void handleDoUpdateDataModel(uscxml::Event& reqCopy);
    void handleReceiveOpResult(uscxml::Event& reqCopy);
    void handleAgentReadyFirstTime(uscxml::Event& evt);

    int reqDmPlayBeep(const std::string& beepPath);
    int reqDmPlayTTS(const std::string& ttsTxt);
    int reqDmPlayTTS(const VoiceList<std::string>::type& txtList);
    int reqDmStopBeep();
    int reqDmStopTTS();

    std::string parseAsrGrammar(uscxml::Event& reqCopy);
    std::string changeGrammarIDForAsr(const std::string &grammar);

    void onStateUpdate(const std::string &msgToDM);

    void initPhoneTypeName();

    // for sessionStateChanged
    void sessionStateSet(bool val);
    bool sessionStateGet();

    // for resourceState Msg Format Change
    void resourceStateChange(std::string type, ResourceState value);

    // for cancel option Msg
    void sendCancelMsg();

    // for preInit updatestate
    void doStateUpdate(const std::string &msgToDM, bool notifly);

    // remove same contact name, get full contact id by search DB and set the max confidence phonetypeid
    void preprocessPhoneTypeResult(pugi::xml_node &result);

    void notifyStartFinishAfterCheck();

    // for prepare message done.preinit
    std::string buildInitParm();
    void processContentBeforeDisplay(pugi::xml_node &displayNode);

    // for "goto RESPONSE_EVENT_RESULT"
    void responseEventResult(pugi::xml_node &eventNode);
};

#endif
/* EOF */
