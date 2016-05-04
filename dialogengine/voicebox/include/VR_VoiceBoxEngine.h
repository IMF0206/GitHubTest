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
 * @file VR_VoiceBoxEngine.h
 * @brief interface for EngineWrapper callback.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VOICEBOXENGINE_H
#define VR_VOICEBOXENGINE_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <sstream>

/* VBT Header */
#ifndef VECIPTR_H
#    include "VECIPtr.h"
#endif

#ifndef VBT_SDK_HINTS_H
#    include "VBT_SDK_Hints.h"
#endif

/* Suntec Header */
#ifndef VR_VOICEBOXENGINEIF_H
#    include "VR_VoiceBoxEngineIF.h"
#endif

#ifndef VR_VOICEBOXENGINECALLBACK_H
#    include "VR_VoiceBoxEngineCallback.h"
#endif

#ifndef VR_VOICEBOXXMLPARSER_H
#    include "VR_VoiceBoxXmlParser.h"
#endif

#ifndef VR_VOICEBOXAGENTPHONE_H
#    include "VR_VoiceBoxAgentPhone.h"
#endif

#ifndef VR_VOICEBOXCATALOGPHONE_H
#    include "VR_VoiceBoxCatalogPhone.h"
#endif

#ifndef VR_VOICEBOXCATALOGAUDIO_H
#    include "VR_VoiceBoxCatalogAudio.h"
#endif

#ifndef MEM_MAP_H
#    include "MEM_map.h"
#endif

#ifndef VR_VOICEBOXFRONTENDSHARE_H
#    include "VR_VoiceBoxFrontEndShare.h"
#endif

#ifndef VR_AUDIOSTREAMLISTENER_H
#    include "VR_AudioStreamListener.h"
#endif

#ifndef VR_VOICEBOXSYNCOBJECT_H
#    include "VR_VoiceBoxSyncObject.h"
#endif

#ifndef VR_CONFIGUREIF_H
#    include "VR_ConfigureIF.h"
#endif

/* Forward Declaration */
class VR_VoiceBoxEngine;
class VR_VoiceBoxEventSink;
class VR_DialogEngineListener;
class VR_VoiceBoxAgentManager;

// TSLInstallAgentInfo
struct TSLInstallAgentInfo
{
    std::string path;
    std::string culture;
};

// Define the action-function map
typedef bool (VR_VoiceBoxEngine::* MessageHandler)(VR_VoiceBoxXmlParser&);

/**
 * @brief The VR_VoiceBoxEngine class
 *
 * class declaration
 */
class VR_VoiceBoxEngine : public VR_VoiceBoxEngineIF
                        , public VR_VoiceBoxEngineCallback
                        , public VR_AudioStreamListener
{
public:
    /**
     * Constructor
     *
     */
    VR_VoiceBoxEngine();

    /**
     * Destructor
     *
     */
    virtual ~VR_VoiceBoxEngine();

    /**
     * Initialize
     *
     * Initialize VoiceBox Engine
     *
     * @param pcExternalCallback  [IN] : External service's callback
     * @param pcMsgController     [IN] : XML message controller
     * @param pcCatalogController [IN] : XML message controller
     * @return bool
     * @attention none
     * @see none
     */
    virtual bool Initialize(
        VR_DialogEngineListener*     pcExternalCallback,
        VR_VoiceBoxController*  pcMsgController,
        VR_VoiceBoxController*  pcCatalogController
        );

    /**
     * Start
     *
     * Start VoiceBox Engine
     *
     * @param strCultureName [IN] : Culture name
     * @return bool
     * @attention none
     * @see none
     */
    virtual bool Start(const std::string &strCultureName);

    /**
     * Stop
     *
     * Stop VoiceBox Engine
     *
     * @param none
     * @return none
     * @attention none
     * @see none
     */
    virtual void Stop();

    /**
     * UnInitialize
     *
     * Shutdown VoiceBox Engine
     *
     * @param none
     * @return none
     * @attention none
     * @see none
     */
    virtual void UnInitialize();

    /**
     * ProcessMessage
     *
     * Process Voice Recognition related messages
     *
     * @param none
     * @return none
     * @attention none
     * @see none
     */
    virtual void ProcessMessage(const std::string& message, int actionSeqId = -1);

    /**
     * GetCatalogManager
     *
     * Get the pointer for catalog manager
     *
     * @param none
     * @return VR_VoiceBoxCatalogManager*
     * @attention none
     * @see none
     */
    virtual VR_VoiceBoxCatalogManager* GetCatalogManager();

    /**
     * On Recognized
     *
     * The voice command was recognized
     *
     * @param message [IN] : Agent result message
     * @return bool
     * @attention none
     * @see none
     */
    virtual void OnRecognized(const std::string& message);

    virtual void SetUpdateGammarFlg(bool bFinishUpdate);

    /**
     * On Info Query Message
     *
     * On Info Query Message
     *
     * @param message [IN] : Info query message
     * @return bool
     * @attention none
     * @see none
     */
    virtual void OnInfoQueryMessage(const std::string& message);

    /**
     * OnBeep
     *
     * Play beep request
     *
     * @param message [IN] : play beep XML message
     * @return bool
     * @attention none
     * @see none
     */
    virtual void OnBeep(const std::string& message);

    /**
     * OnBeep
     *
     * Play beep request
     *
     * @param beepType [IN] : play beep type
     * @return bool
     * @attention none
     * @see none
     */
    virtual void OnBeep(const VR_BeepType& beepType);

    /**
     * OnSpeak
     *
     * Play guidance request
     *
     * @param pcTrans [IN] : transaction for play guidance
     * @param message [IN] : play guidance XML message
     * @return bool
     * @attention none
     * @see none
     */
    virtual void OnSpeak(IVECITransaction* pcTrans, const std::string& message);

    /**
     * On Request
     *
     * Send an request message
     *
     * @param message [IN] : Request message
     * @return bool
     * @attention none
     * @see none
     */
    virtual void OnRequest(const std::string& message);

    virtual void OnPlayTone(const std::string& message);

    static HRESULT CreateVocon4FrontEndShare(
        void* userData,
        IVBTVocon4FrontEndShare** ppShare
        );

    virtual void OnBosDetected();

    virtual void InsertMusicUpdateGrammar(
        const std::string& strTransId,
        const std::string& strSourceId);

    virtual void ConfirmMusicUpdateGrammar(
        const std::string& strTransId, const bool bGrammarErrorCode);

    virtual std::string GetAudioConnected(const std::string& strAudioName);

    virtual std::string GetAudioConnected();

    virtual void ChangeAudioSourceName(std::string& strAudioName);

    virtual void GetPromptByScreenId(const std::string& strScreenId, std::string& strPrompt);

    virtual bool GetUpdateMapState() const;

    virtual void SendGrammarResult(
    const std::string& strOp, const std::string& strAgent,
    const std::string& strGrammarId, const std::string& strErrCode);

    virtual void SetActiveSouceTrans(const std::string& strTransId, const std::string& strOp, const std::string& strSouceId);

    virtual void GetOpActiveSouce(const std::string& strTransId, std::string& strOp, std::string& strActiveSource);

    virtual std::string getHints(const std::string& hintsParams);

    virtual void SetNBestScreenFlg(const bool& bNBestFlg);

    virtual bool GetNBestScreenFlg();

    virtual std::string GetPhonBookDBPath();

    virtual std::string GetMDActiveSourceId();

    virtual void SetGrammarInitSourceId(const std::string& strGrammarInitSourceId);

    virtual bool GetEngineStatus();

    virtual bool GetMessageAvailable();

    virtual bool ResetGrammarUpdate();

    virtual void SetWaitResumeGrammar(bool bWaitResumeGrammar);

    virtual void SetCurrentTransaction(const std::string& strAgent, const std::string& strTransId);

    virtual std::pair<std::string, std::string> GetCurrentTransaction();

    virtual bool GetRouteStatus();

    virtual std::string GetCultureName();

private:
    /**
     * CreateEngineClient
     *
     * Create the VoiceBox engine related instances and initialize them.
     *
     * @param none
     * @return bool
     * @attention none
     * @see none
     */
    bool CreateEngineClient();

    /**
     * CreateEventSink
     *
     * Creates Event Sink object instance and register it.
     *
     * @param none
     * @return bool
     * @attention none
     * @see none
     */
    bool CreateEventSink();

    /**
     * CreateEngineCommand
     *
     * Create an engine command dispatcher object.
     *
     * @param none
     * @return bool
     * @attention none
     * @see none
     */
    bool CreateEngineCommand();

    /**
     * CreateCatalogManager
     *
     * Create catalog manager.
     *
     * @param none
     * @return bool
     * @attention none
     * @see none
     */
    bool CreateCatalogManager();

    /**
     * GetHints
     *
     * Get Hints according to the specified state name.
     *
     * @param strStateName [IN] : State name
     * @return bool
     * @attention none
     * @see none
     */
    bool GetHints();

    /**
     * CancelRecoSession
     *
     * Cancel the current voice recognition session.
     *
     * @param none
     * @return bool
     * @attention none
     * @see none
     */
    bool CancelRecoSession();

    /**
     * SendRecognitionState
     *
     * Puts the VoiceBox Engine into a specific recognition state.
     *
     * @param parser [IN] : XML parser instance
     * @return bool
     * @attention none
     * @see none
     */
    bool SendRecognitionState(VR_VoiceBoxXmlParser& parser);

    /**
     * SaveNextRecognitionState
     *
     * Puts the VoiceBox Engine into a specific recognition state.
     *
     * @param parser [IN] : XML parser instance
     * @return bool
     * @attention none
     * @see none
     */
    bool SaveNextRecognitionState(VR_VoiceBoxXmlParser& parser);

    bool SaveTuToRecognitionState(VR_VoiceBoxXmlParser& parser);

    /**
     * StartRecoSession
     *
     * Sends start listening request to VoiceBox Engine.
     *
     * @param parser [IN] : XML parser instance
     * @return bool
     * @attention none
     * @see none
     */
    bool StartRecoSession(VR_VoiceBoxXmlParser& parser);

    /**
     * CancelRecoSession
     *
     * Cancel the current voice recognition session.
     *
     * @param parser [IN] : XML parser instance
     * @return bool
     * @attention none
     * @see none
     */
    bool Cancel(VR_VoiceBoxXmlParser& parser);

    /**
     * Repeat
     *
     * Returns the transaction object generated after the last command,
     * and optionally plays last TTS.
     *
     * @param parser [IN] : XML parser instance
     * @return bool
     * @attention none
     * @see none
     */
    bool Repeat(VR_VoiceBoxXmlParser& parser);

    /**
     * Back
     *
     * Puts the VBT Engine into the previous state before the last command,
     * and optionally plays previous TTS.
     *
     * @param parser [IN] : XML parser instance
     * @return bool
     * @attention none
     * @see none
     */
    bool Back(VR_VoiceBoxXmlParser& parser);

    /**
     * Start Over
     *
     * Restart the voice recognition
     *
     * @param parser [IN] : XML parser instance
     * @return bool
     * @attention none
     * @see none
     */
    bool StartOver(VR_VoiceBoxXmlParser& parser);

    /**
     * Help
     *
     * Update the hints and play a help guidance
     *
     * @param parser [IN] : XML parser instance
     * @return bool
     * @attention none
     * @see none
     */
    bool Help(VR_VoiceBoxXmlParser& parser);

    /**
     * SpeakDone
     *
     * After guidance play is completed, notify VoiceBox engine.
     *
     * @param parser [IN] : XML parser instance
     * @return bool
     * @attention none
     * @see none
     */
    bool SpeakDone(VR_VoiceBoxXmlParser& parser);

    /**
     * BeepDone
     *
     * After beep play is completed, notify VoiceBox engine.
     *
     * @param parser [IN] : XML parser instance
     * @return bool
     * @attention none
     * @see none
     */
    bool BeepDone(VR_VoiceBoxXmlParser& parser);

    bool StopSpeakDone(VR_VoiceBoxXmlParser& parser);

    bool StartDictation(VR_VoiceBoxXmlParser& parser);
    bool ButtonPressed(VR_VoiceBoxXmlParser& parser);
    bool UpdateState(VR_VoiceBoxXmlParser& parser);
    bool ChangeSettings(VR_VoiceBoxXmlParser& parser);
    bool ChangeLanguage(VR_VoiceBoxXmlParser& parser);
    bool ChangeLanguageInner(VR_VoiceBoxXmlParser& parser);
    bool FullUpdateNotify(VR_VoiceBoxXmlParser& parser);
    bool GetResourceState(VR_VoiceBoxXmlParser& parser);

    bool SelectOne(VR_VoiceBoxXmlParser& parser);
    bool SelectTwo(VR_VoiceBoxXmlParser& parser);
    bool SelectThree(VR_VoiceBoxXmlParser& parser);
    bool SelectFour(VR_VoiceBoxXmlParser& parser);
    bool SelectFive(VR_VoiceBoxXmlParser& parser);
    bool PttShort(VR_VoiceBoxXmlParser& parser);
    bool PttLong(VR_VoiceBoxXmlParser& parser);
    bool EntryNormalPress(VR_VoiceBoxXmlParser& parser);
    bool BackNormalPress(VR_VoiceBoxXmlParser& parser);
    bool Phone(VR_VoiceBoxXmlParser& parser);
    bool Navigation(VR_VoiceBoxXmlParser& parser);
    bool Apps(VR_VoiceBoxXmlParser& parser);
    bool Audio(VR_VoiceBoxXmlParser& parser);
    bool Info(VR_VoiceBoxXmlParser& parser);
    bool Climate(VR_VoiceBoxXmlParser& parser);
    bool SpeakAdaptation(VR_VoiceBoxXmlParser& parser);
    bool Pause(VR_VoiceBoxXmlParser& parser);
    bool HandlePause(VR_VoiceBoxXmlParser& parser);
    bool Resume(VR_VoiceBoxXmlParser& parser);
    bool NextPage(VR_VoiceBoxXmlParser& parser);
    bool PrevPage(VR_VoiceBoxXmlParser& parser);
    bool LastPage(VR_VoiceBoxXmlParser& parser);
    bool FirstPage(VR_VoiceBoxXmlParser& parser);
    bool ConfirmYes(VR_VoiceBoxXmlParser& parser);
    bool ConfirmNo(VR_VoiceBoxXmlParser& parser);
    bool GoDirectly(VR_VoiceBoxXmlParser& parser);
    bool AddToRoute(VR_VoiceBoxXmlParser& parser);
    bool Call(VR_VoiceBoxXmlParser& parser);
    bool SendMessage(VR_VoiceBoxXmlParser& parser);
    bool Reply(VR_VoiceBoxXmlParser& parser);
    bool ReadNext(VR_VoiceBoxXmlParser& parser);
    bool ReadPrevious(VR_VoiceBoxXmlParser& parser);
    bool Tutorials(VR_VoiceBoxXmlParser& parser);
    bool VoiceTraining(VR_VoiceBoxXmlParser& parser);
    bool InternalStartVR(VR_VoiceBoxXmlParser& parser);
    bool SettingToTraning(VR_VoiceBoxXmlParser& parser);
    bool AlongRoute(VR_VoiceBoxXmlParser& parser);
    bool NearDestination(VR_VoiceBoxXmlParser& parser);
    bool MoreHints(VR_VoiceBoxXmlParser& parser);
    bool HandleMoreHints(VR_VoiceBoxXmlParser& parser);
    bool HandleActionMsg(VR_VoiceBoxXmlParser& parser);
    bool HandleVRState(VR_VoiceBoxXmlParser& parser);
    bool BrowseAlbums(VR_VoiceBoxXmlParser& parser);
    bool BrowseArtists(VR_VoiceBoxXmlParser& parser);
    bool BrowseSongs(VR_VoiceBoxXmlParser& parser);
    bool BrowseComposers(VR_VoiceBoxXmlParser& parser);
    bool BrowseGenres(VR_VoiceBoxXmlParser& parser);
    bool BrowsePodcasts(VR_VoiceBoxXmlParser& parser);
    bool BrowsePlaylists(VR_VoiceBoxXmlParser& parser);
    bool BrowseAudiobooks(VR_VoiceBoxXmlParser& parser);
    bool BrowseMusicByType(const std::string& strMusicType);
    bool OnMusicType(VR_VoiceBoxXmlParser& parser);
    bool ShowHintScreen(VR_VoiceBoxXmlParser& parser);

    bool PVRAction(VR_VoiceBoxXmlParser& parser);
    bool AgentHelp(VR_VoiceBoxXmlParser& parser);
    bool IncomingMessageInfo(VR_VoiceBoxXmlParser& parser);
    bool HintPage(VR_VoiceBoxXmlParser& parser);
    bool UpdateMapData(VR_VoiceBoxXmlParser& parser);
    bool InitialPersonData(VR_VoiceBoxXmlParser& parser);
    bool Prepare(VR_VoiceBoxXmlParser& parser);

    bool StartAppRecognition(const std::string& message);
    bool CancelAppRecognition(const std::string& message);
    bool SendAppXmlMessage(const std::string& message);

    bool OnSendAppRecognitionState(VR_VoiceBoxXmlParser& parser);
    bool OnSendAppCommand(VR_VoiceBoxXmlParser& parser);
    bool StartAppRecoSession();
    bool OnStartAppRecoSession(VR_VoiceBoxXmlParser& parser);
    bool OnGetVersion(VR_VoiceBoxXmlParser& parser);
    bool OnAppBack(VR_VoiceBoxXmlParser& parser);
    bool OnSendAppXmlMsg(VR_VoiceBoxXmlParser& parser);

    /**
     * OnCommandComplete
     *
     * Process the command complete event
     *
     * @param parser [IN] : XML parser instance
     * @return bool
     * @attention none
     * @see none
     */
    bool OnCommandComplete(VR_VoiceBoxXmlParser& parser);

    /**
     * OnSendRecognitionState
     *
     * Process when SendRecognitionState is completed.
     *
     * @param parser [IN] : XML parser instance
     * @return bool
     * @attention none
     * @see none
     */
    bool OnSendRecognitionState(VR_VoiceBoxXmlParser& parser);

    /**
     * OnStartRecoSession
     *
     * Process when StartRecoSession is completed.
     *
     * @param parser [IN] : XML parser instance
     * @return bool
     * @attention none
     * @see none
     */
    bool OnStartRecoSession(VR_VoiceBoxXmlParser& parser);

    /**
     * OnRepeat
     *
     * Process when Repeat is completed.
     *
     * @param parser [IN] : XML parser instance
     * @return bool
     * @attention none
     * @see none
     */
    bool OnRepeat(VR_VoiceBoxXmlParser& parser);

    /**
     * OnBack
     *
     * Process when Back is completed.
     *
     * @param parser [IN] : XML parser instance
     * @return bool
     * @attention none
     * @see none
     */
    bool OnBack(VR_VoiceBoxXmlParser& parser);

    /**
     * OnHelp
     *
     * Process when Help is completed.
     *
     * @param parser [IN] : XML parser instance
     * @return bool
     * @attention none
     * @see none
     */
    bool OnHelp(VR_VoiceBoxXmlParser& parser);
    bool OnHelpRecognized(VR_VoiceBoxXmlParser& parser);
    bool OnCall(VR_VoiceBoxXmlParser& parser);
    bool OnSendMessage(VR_VoiceBoxXmlParser& parser);
    bool OnReply(VR_VoiceBoxXmlParser& parser);
    bool OnReadNext(VR_VoiceBoxXmlParser& parser);
    bool OnReadPrevious(VR_VoiceBoxXmlParser& parser);
    bool OnSpeakAdaptation(VR_VoiceBoxXmlParser& parser);
    bool OnSelect(VR_VoiceBoxXmlParser& parser);
    bool OnEscalatingError(VR_VoiceBoxXmlParser& parser);

    void DeleteVoiceTagEvent(pugi::xml_node& eventNode);

    void SaveVoiceTagEvent(pugi::xml_node& eventNode);

    void PlayVoiceTagEvent(pugi::xml_node& eventNode);

    void StopVoiceTagEvent(pugi::xml_node& eventNode);

    void CancelRecordVoiceTag(pugi::xml_node& eventNode);

    void HandleVoiceTagBeepOrTTs();

    void CancelSaveVoiceTag(pugi::xml_node& eventNode);

    void SyncVoiceTagEvent(pugi::xml_node& eventNode);

    bool SyncVoiceTag(std::string &strGrammar, const std::string &deviceAddress, VoiceList<std::string>::type &btVoiceTagIDList);

    void CancelVoiceTagRecoSession();

    void RecordVoiceTagEvent(pugi::xml_node& eventNode, bool isUpdate);

    void RecordVoiceTagResult(const std::string& tempPronunc);

    void HandleIncomingMessage(const std::string& message);

    bool OnRecordingVoiceTag(VR_VoiceBoxXmlParser& parser);

    bool OnMoreHints(VR_VoiceBoxXmlParser& parser);
    bool OnNextPage(VR_VoiceBoxXmlParser& parser);
    bool OnPrevPage(VR_VoiceBoxXmlParser& parser);
    bool OnLastPage(VR_VoiceBoxXmlParser& parser);
    bool OnFristPage(VR_VoiceBoxXmlParser& parser);
    bool OnAlongRoute(VR_VoiceBoxXmlParser& parser);
    bool OnNearDestination(VR_VoiceBoxXmlParser& parser);
    bool OnConfirmYes(VR_VoiceBoxXmlParser& parser);
    bool OnConfirmNo(VR_VoiceBoxXmlParser& parser);
    bool OnGoDirectly(VR_VoiceBoxXmlParser& parser);
    bool OnAddToRoute(VR_VoiceBoxXmlParser& parser);
    bool OnInstallAgent(VR_VoiceBoxXmlParser& parser);

    bool OnGetHints(VR_VoiceBoxXmlParser& parser);

    bool OnStartEngine(VR_VoiceBoxXmlParser& parser);

    bool OnPttLong(VR_VoiceBoxXmlParser& parser);

    bool SetRecoSessionFlag(VR_VoiceBoxXmlParser& parser);

    bool OnDMStartedNotify(VR_VoiceBoxXmlParser& parser);

    bool OnStartOver(VR_VoiceBoxXmlParser& parser);

    bool DisplayHints(const std::string& strHintScreenId, bool bGetMoreHint = false);

    /**
     * Restart
     *
     * Restart VoiceBox Engine
     *
     * @param strCultureName [IN] : Culture name
     * @return bool
     * @attention none
     * @see none
     */
    bool Restart(const std::string &strCultureName);

    void HandleVoiceTag(const std::string& message);
    void SmartAgent(const std::string& message);
    void HandleSmartAgent(pugi::xml_node& eventNode);
    void HandlePromptLevel(const std::string& userId);
    void SavePromptLevel(const std::string& userId, pugi::xml_document& promptLevelValueDoc);
    void SavePromptLevel(const std::string& strKeyValue);
    void SetSpeakerProfile(const std::string& strKeyValue);
    void DeleteSmartAgent(const std::string& userId);
    void DeleteALLSmartAgent();

    bool Select(int iIndex);

    bool SendRecogState();

    void setPromptLevel(int promptLevel);

    void setActionResultValue(const std::string& strActionResult)
    {
        m_strActionResult = strActionResult;
    }

    /**
     * StartRecoSessionWithBargeIn
     *
     * Sends start listening request to VoiceBox Engine.
     *
     * @param parser [IN] : XML parser instance
     * @return bool
     * @attention none
     * @see none
     */
    bool StartRecoSessionWithBargeIn(VR_VoiceBoxXmlParser& parser);

    bool HandleAction(const std::string& strAction);

    void QueryHintFirstPage();
    void QueryHintLastPage();
    void HandleVehicleDriving(const std::string& strMessage);

    void SetRoute(const std::string& strMessage);
    void NotifyRouteStatus(const std::string& strMessage);
    void SetVBTPrefRouteActive(const std::string& strStatus);

    void HandleQuitVR();

    std::string GetVoiceTagId(pugi::xml_node& contactNode);

    int OnRequestAction(const std::string& strMsg);

    void InitPromptOfScreen();

    void VRSateForHint(const std::string& strScreenId);

    void ProcessGrammarMSg(const std::string& message);

    bool HandleQuitVRApp(const std::string& strMessage);

    void AddRecogItem(CVECIPtr<IVECIListItems>& optionalContextList,
    const std::string& strAgent, const std::string& strContext);

    void SetOptionalContext(CVECIPtr<IVECIListItems>& optionalContextList,
                            const std::string& strStateName,
                            bool bForSendReco);

    bool InstallDownloadableAgents(const std::string& message);

    bool UninstallAllAgents();
    bool UninstallAllAgentsInSync(std::string culture);

    bool OnAllAgentsUninstalled(VR_VoiceBoxXmlParser& parser);

    bool OnAgentInstalled(VR_VoiceBoxXmlParser& parser);

    void NotifyInstallAgentResult(bool bInstallResult);

    bool DoInstallAgent(TSLInstallAgentInfo& installInfo);

    std::string GetCulture(std::string& language);

    void ClosePopupMsg();

    void SendCancelResult();

    void RenameVbtLog();

    std::string SynchronizeHints();

    bool IsAppsInstalled();

    bool IsAppsEnabled();

    void GrammarDiffDetail(const std::string& strMessage);

    void GrammarNewDetail(const std::string& strMessage);

    bool RestartForInstallTslAgent(const std::string &strCultureName);

    void ChangeLanguageResult(const bool bResult);

    void SendChangeLanguageResult(const std::string& strLanguage);

private:
    enum AgentType
    {
        AgentType_Apps,
        AgentType_Audio,
        AgentType_Climate,
        AgentType_Global,
        AgentType_Info,
        AgentType_Navi,
        AgentType_Phone,
        AgentType_Num
    };

    enum VR_VoiceTagResult
    {
        VOICETAG_OK = 0,
        VOICETAG_NOSPEECH = 1,
        VOICETAG_TOOSHORT = 2,
        VOICETAG_SIMILAR = 3,
        VOICETAG_FAILURE = 4
    };

    enum VR_SessionState
    {
        VR_SessionStateNone = -1,
        VR_SessionStateStarting = 0, // start session
        VR_SessionStateTemporarilyCanceled, // cancel session
        VR_SessionStateCanceled, // handle the cancel command
        VR_SessionStateStartOver,
        VR_SessionStateAutoSendRecogState, // start new session with getting hints after session over
        VR_SessionStateAutoTutoSendRecogState,
        VR_SessionStateAutoRestartSession, // start new session after the current session over
        VR_SessionStateBackQuit, // handle the back command, the last transtion is NULL
        VR_SessionStateBackRestart, // handle the back command, the last transtion is not NULL
        VR_SessionStateStoped, // the session is over
        VR_SessionStateRestartSessionAfterTTS,
        VR_SessionStateSendRecogAfterTTS,
        VR_SessionStateQuitAfterTTS,
        VR_SessionStateSpeakAdaptaion,
        VR_SessionStateResume,
        VR_SessionStatePaused, // the session is paused
        VR_SessionStateStartWithiBargeIn, // start new session with barge in
        VR_SessionStatePttLong
    };

    enum VR_PVRState
    {
        VR_PVRState_None = 0,
        VR_PVRState_WaitPlayTTS = 1,
        VR_PVRState_PlayingTTS = 2,
        VR_PVRState_PlayedTTS = 3,
    };

    enum PlayedType
    {
        NONE = 0,
        TTS = 1,
        BEEP = 2
    };

    enum VR_AppRecoState
    {
        VR_AppRecoState_Invalid = 0,
        VR_AppRecoState_Sending = 1,
        VR_AppRecoState_Valid = 2,
        VR_AppRecoState_SendCaching = 3,
    };

    struct VR_MsgInfo
    {
        int iMsgId;
        std::string strMsg;
    };

    CVECIPtr<IVECIEngineClient> 		m_client;			///< EngineClient
    CVECIPtr<IVECIEngineCommand> 		m_engineCommand;	///< Process Engine ralted request
    CVECIPtr<IVECIParameterSet>         m_pVariables;
    VoiceMap<std::string, MessageHandler>::type m_mapMsgHandler;
    VoiceMap<std::string, MessageHandler>::type m_mapMsgBtnHandler;
    VoiceMap<std::string, MessageHandler>::type m_mapCmdResultHandler;
    VoiceMap<int, IVECITransaction*>::type m_mapPlayTTSTransation;
    VoiceMap<int, int>::type m_mapStopTTSTransation;
    VoiceMap<std::string, VoiceList<std::string>::type >::type m_mapGrammarSourceId;
    VoiceMap<std::string, std::string>::type m_mapAgenttoRecoState;
    VoiceMap<std::string, std::string>::type m_mapAction;
    VoiceList<std::string>::type        m_lstUCAppQuitVROp;
    VoiceList<std::string>::type        m_lstNeedReactiveAction;
    VR_VoiceBoxEventSink*        		m_agents[AgentType_Num]; ///< Receive VBT's callback
    VR_DialogEngineListener*            m_pcExternalCallback; ///< External service's callback
    VR_VoiceBoxController*              m_pcMsgController;    ///< Message Controller
    VR_VoiceBoxController*              m_pcCatalogController; ///< Catalog Controller
    VR_VoiceBoxCatalogPhone*            m_pcCatalogPhone;   ///< Catalog phone
    VR_VoiceBoxCatalogManager*          m_pcCatalogManager;   ///< Catalog manager
    VR_VoiceBoxCatalogAudio*            m_pcCatalogAudio;
    IVECITransaction*                   m_pcPlayTransation;   ///< Transaction for OnSpeak
    std::string                         m_strCurrentStateName; ///< Current state name
    std::string                         m_strStartOverStateName; ///< Start over state name
    std::string                         m_strCurrentTransactionID;  ///< Current transaction ID
    std::string                         m_strPreSessionTransactionID;  ///< Current transaction ID
    std::string                         m_strLastTransactionID;  ///< Last transaction ID
    std::string                         m_strCultureName;
    std::string                         m_tempPcmFile;
    std::string                         m_tempPronunc;
    CVECIPtr<IVECIStringSet>            m_spHints;
    std::string                         m_strUIStateName;
    std::string                         m_strContent;
    VBT_ULONG                           m_iIndex;
    VBT_ULONG                           m_iHintSize;
    std::string                         m_strMoreHints;
    std::string                         m_strAdaptationPath;
    std::string                         m_strDevice;
    bool                                m_bMoreHint;
    std::string                         m_tempVoiceTagID;
    std::string                         m_voiceTagContactMsg;
    bool                                m_isVoiceTagUpdate;
    std::string                         m_deviceAddress;
    VR_SessionState                     m_sessionState;
    std::string                         m_strActionResult;
    std::string                         m_strDMActiveSourceId;
    std::string                         m_strGrammarInitSourceId;
    bool                                m_bTaskCompelete;
    bool                                m_bPlayTTS;
    std::string                         m_strBtnEventName;
    int                                 m_iCurReqId;
    int                                 m_iPlayVoiceTagId;
    std::stringstream                   m_iOss;
    int                                 m_iCurTTSReqId;
    int                                 m_iCurrActionId;
    bool                                m_messageAvailable;
    bool                                m_bUpdatingMapData;
    bool                                m_isActiveFM;
    bool                                m_isActiveAM;
    bool                                m_isSatellite;
    bool                                m_isActiveFMHD;
    bool                                m_isActiveSatellite;
    static VR_VoiceBoxFrontEndShare     s_frontEndShare;    ///< FrontEndShare
    bool                                m_bBosDetected;
    VoiceMap<std::string, int>::type    m_mapHintsScreenID;
    std::string                         m_strAction;
    std::string                         m_strActionID;
    int                                 m_iPromptLevel;
    bool                                m_bDoCanceling;
    bool                                m_bDoCancelVoiceTag;
    bool                                m_bEngineStarting;
    bool                                m_bUsb1;
    bool                                m_bUsb2;
    bool                                m_bIpod1;
    bool                                m_bIpod2;
    bool                                m_bBtAudio;
    bool                                m_bHVACBasicActive;
    bool                                m_bHVACFrontScrnActive;
    bool                                m_bHVACRearScrnActive;
    bool                                m_bHVACSteeringScrnActive;
    bool                                m_bHVACConcModeActive;
    bool                                m_bHVACSeatActive;
    VoiceVector<PersonInfo>::type       m_vecPersonInfo;
    std::string                         m_strScreenState;
    bool                                m_isNameNull;
    bool                                m_bTTSNull;
    int                                 m_iStartBeepID;
    int                                 m_iDoneBeepID;
    bool                                m_bGrammarErrorCode;
    bool                                m_bSession;
    bool                                m_bEngineReady;
    bool                                m_isIncomingMessage;
    VR_VoiceBoxSyncObject               m_ActionIDSync;
    VR_REGION_TYPE                      m_country;
    std::string                         m_strUsbOrIpodConnected;
    std::string                         m_userId;
    PlayedType                          m_lastPlayed;
    int                                 m_stopVoiceTagBeepOrTTs;
    int                                 m_stopTraining;
    bool                                m_bSongInfoAvailable;
    int                                 m_iToturialBeepId;
    int                                 m_iVoiceTrainingBeepId;
    std::string                         m_strToturialResult;
    std::string                         m_strVoiceTrainingResult;
    std::string                         m_strAgentName;
    std::string                         m_strActionType;
    std::string                         m_strActionClass;
    std::string                         m_strMsgPrompt;
    std::string                         m_strDisplayScreen;
    std::string                         m_strCountryID;
    std::string                         m_strStep;
    MessageInfo                         m_messageinfo;
    std::string                         m_strPath;
    VoiceMap<std::string, VoiceVector<std::string>::type >::type m_mapScreenPrompt;
    VoiceMap<std::string, std::string>::type m_mapScreenPromptOC;
    VoiceList<std::string>::type m_lstTransaction;
    VoiceMap<std::string, std::pair<std::string, std::string>>::type m_mapActiveSouceTrans;
    VoiceVector<TSLInstallAgentInfo>::type m_vecTSLInstallInfo;
    VoiceList<int>::type m_listPlayVoiceTagSeq;
    int                                 m_iInstallVecIndex;
    bool                                m_bInstallingAgent;
    VR_PVRState                         m_PVRStateCurrent;
    bool                                m_bWaitConfirmation;
    bool                                m_bSpeakOverBeep;
    bool                                m_bPhoneGrammarAvailable;
    bool                                m_bTslNetworkAvailable;
    VR_AppRecoState                     m_bAppRecoState;
    VoiceList<VR_MsgInfo>::type         m_listAppMessages;
    bool                                m_bWaitGreetingEnd;
    bool                                m_bQuitWaitForPrompt;
    bool                                m_bTslDialog;
    bool                                m_bTslAppsAvailable;
    bool                                m_bPVRScreen;
    bool                                m_bSettingToStartScreen;
    bool                                m_bNBestFlg;
    bool                                m_bWaitForDoneBeep;
    std::string                         m_strCurResourceState;
    bool                                m_bDoCancelTslVR;
    std::string                         m_strTslAppRecoStateMsg;
    bool                                m_bBackInterupted;
    bool                                m_bStartSessionWithBargeIn;
    bool                                m_strMsgAvailable;
    bool                                m_bHavePVR;
    bool                                m_bWaitResumeGrammar;
    bool                                m_bCanceled;
    std::pair<std::string, std::string>   m_pairTransaction;
    std::string                         m_strInstallCulture;
    VoiceList<std::string>::type        m_listUpdateState;
    bool                                m_bRouteStatus;
    bool                                m_bDoCancelSession;
    bool                                m_bChangeLanguage;
    std::string                         m_strLanguage;
    VoiceList<std::string>::type        m_lstLanguage;
    VoiceList<std::string>::type        m_lstCancelOption;

    void GetUIInfoByVBTStateName(int& nScreenId, const std::string strStateName);

    void GetVBTInfoByUIStateName(const std::string& strUISateName,
        int& nScreenId, std::string& strVBTStateName);

    bool StartSession();

    bool InScreen(const std::string& strContent);
    void BuildScreenContent(std::string& strContent);
    bool TransationSpeakDone(const int& iActionID);
    bool TransationStopSpeakDone(const int& iActionID);
    bool TransationSpeakDone();
    void StopAllTTS();
    void UpdateMapGrammar();
    void ShowHelpMoreHintsScreen();
    bool PrepareHintsContent(const std::string& strStateName);
    void NotifyResourceState();
    void SendDEStatus(const std::string& strType, const std::string& strStatus);
    void StopTraining();
    void ResetTslAgents();
    void SendVRState(const std::string& strState, const std::string& strPrompt,
        const std::string& strNBestScreen, const std::string& strResult);
    void NotifyInstallProgress(const std::string& strPath, int index);

    std::string GetAdaptationProfilePath(const std::string& strLanguage, const std::string& strProfileName);

    /**
     * Copy Constructor
     *
     */
    VR_VoiceBoxEngine(const VR_VoiceBoxEngine&);

    /**
     * Copy Assignment Operator
     *
     */
    VR_VoiceBoxEngine &operator=(const VR_VoiceBoxEngine&);
};

#endif // VR_VOICEBOXENGINE_H
/* EOF */
