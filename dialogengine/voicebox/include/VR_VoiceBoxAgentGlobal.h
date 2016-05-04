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
 * @file VR_VoiceBoxAgentGlobal.h
 * @brief interface for EngineWrapper callback.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VOICEBOXAGENTGLOBAL_H
#define VR_VOICEBOXAGENTGLOBAL_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif


#    include <sstream>

/* Suntec Header */
#ifndef VR_VOICEBOXEVENTSINK_H
#    include "VR_VoiceBoxEventSink.h"
#endif

#ifndef MEM_MAP_H
#    include "MEM_map.h"
#endif

/* Forward Declaration */
class VR_VoiceBoxAgentGlobal;

// Define the agent message handler pointer
typedef bool (VR_VoiceBoxAgentGlobal::*GlobalMsgHandler)(IVECIParsedMessage*);
typedef bool (VR_VoiceBoxAgentGlobal::*GlobalActionResultHandler)(VR_VoiceBoxXmlParser&);

// Define the action-function map
typedef bool (VR_VoiceBoxAgentGlobal::*GlobalMsgResultHandler)(IVECITransaction*);

/**
 * @brief The VR_VoiceBoxAgentGlobal class
 *
 * class declaration
 */
class VR_VoiceBoxAgentGlobal : public VR_VoiceBoxEventSink
{
public:
    /**
     * Constructor
     *
     */
    VR_VoiceBoxAgentGlobal(
        IVECIEngineClient& client,
        IVECIEngineCommand& engineCommand,
        VR_VoiceBoxEngineCallback& engineCallback
        );

    /**
     * Destructor
     *
     */
    virtual ~VR_VoiceBoxAgentGlobal();

    /**
     * Initialize
     *
     * Initialize
     *
     * @param none
     * @return bool
     * @attention none
     * @see none
     */
    virtual bool Initialize();

    /**
     * UnInitialize
     *
     * UnInitialize
     *
     * @param none
     * @return none
     * @attention none
     * @see none
     */
    virtual void UnInitialize();

    /**
     * ReplyQueryInfo
     *
     * ReplyQueryInfo
     *
     * @param none
     * @return bool
     * @attention none
     * @see none
     */
    virtual bool ReplyQueryInfo(
        VR_VoiceBoxXmlParser& parser
        );

    virtual bool ProcessMessage(
        const std::string& message
        );

    // IVECIEventSink's pure virtual functions
    virtual HRESULT QueryAgentSupport(VBT_CSTR szAgent, VBT_BOOL *pbSupported);
    virtual HRESULT OnAgentResultMessage(IVECITransaction* pTrans, IVECIParsedMessage* pResultMsg);
    virtual HRESULT OnAgentDriverMessage(IVECIParsedMessage* pDriverMsg);
    virtual HRESULT OnCommandComplete(IVECITransaction* pTrans);
    virtual HRESULT OnAudioLevelUpdate(VBT_LONG lValue);
    virtual HRESULT OnProcessStatus(VBT_CSTR szState);
    virtual HRESULT OnSystemNotification(unsigned long ulCode, IVECIParameterSet *pParameterSet);
    virtual HRESULT OnSpeak(IVECITransaction *pTrans, IVECIStringSet* pSpeakList);
    virtual HRESULT OnBeepSync();

    virtual bool ProcessAgentMessage(IVECIParsedMessage *pcMsg);

private:
    VoiceMap<std::string, GlobalMsgHandler>::type m_globalMsgHandler;
    VoiceMap<std::string, GlobalActionResultHandler>::type m_globalActionResultHandler;
    VoiceMap<std::string, GlobalMsgResultHandler>::type m_mapMsgResultHandler;

    bool m_bTslState;
    int m_step;
    VBT_LONG m_lLevelValue;
    IVECITransaction* m_pcUsedTrans;
    bool m_bGrammarErr;
    int m_iGrammarErrCount;

    /**
     * Process Agent Message
     *
     * Process message that was sent by VoiceBox
     *
     * @param pcMsg [IN] : Agent result message or info driver message
     * @return bool
     * @attention none
     * @see none
     */

    /* Agent: System */
    bool SystemBack(IVECIParsedMessage *pcMsg);
    bool SystemBeginOfList(IVECIParsedMessage *pcMsg);
    bool SystemCancel(IVECIParsedMessage *pcMsg);
    bool SystemCSVRComplete(IVECIParsedMessage *pcMsg);
    bool SystemEndOfList(IVECIParsedMessage *pcMsg);
    bool SystemErrorLimitReached(IVECIParsedMessage *pcMsg);
    bool SystemFirstPage(IVECIParsedMessage *pcMsg);
    bool SystemGenericConfirm(IVECIParsedMessage *pcMsg);
    bool SystemGenericReject(IVECIParsedMessage *pcMsg);
    bool SystemGreeting(IVECIParsedMessage *pcMsg);
    bool SystemHelp(IVECIParsedMessage *pcMsg);
    bool SystemLastPage(IVECIParsedMessage *pcMsg);
    bool SystemNextPage(IVECIParsedMessage *pcMsg);
    bool SystemNoReco(IVECIParsedMessage *pcMsg);
    bool SystemConnectError(IVECIParsedMessage *pcMsg);
    bool SystemPreviousPage(IVECIParsedMessage *pcMsg);
    bool SystemStartOver(IVECIParsedMessage *pcMsg);
    bool SystemPause(IVECIParsedMessage *pcMsg);
    bool SystemRepeat(IVECIParsedMessage *pcMsg);
    bool SystemVBTError(IVECIParsedMessage *pcMsg);
    bool SystemSendCommand(IVECIParsedMessage *pcMsg);
    bool SystemVersionInfo(IVECIParsedMessage *pcMsg);
    bool SystemWhatCanISay(IVECIParsedMessage *pcMsg);

    /* Agent: Speaker Adaptation */
    bool SpeakerAdaptationCommand(IVECIParsedMessage *pcMsg);
    bool StartSpeakerAdaptation(IVECIParsedMessage *pcMsg);

    bool NullOperation(IVECIParsedMessage *pcMsg);
    // Help
    bool AddressEntryHelp(IVECIParsedMessage *pcMsg);
    bool AddressEntryHelpNotEnabled(IVECIParsedMessage *pcMsg);
    bool AppsHelp(IVECIParsedMessage *pcMsg);
    bool AppsHelpNotEnabled(IVECIParsedMessage *pcMsg);
    bool ConfirmTutorialConfirm(IVECIParsedMessage *pcMsg);
    bool ConfirmTutorialReject(IVECIParsedMessage *pcMsg);
    bool DestinationHelp(IVECIParsedMessage *pcMsg);
    bool DestinationHelpNotEnabled(IVECIParsedMessage *pcMsg);
    bool SystemExit(IVECIParsedMessage *pcMsg);
    bool HVACHelp(IVECIParsedMessage *pcMsg);
    bool InformationHelp(IVECIParsedMessage *pcMsg);
    bool InformationHelpNotEnabled(IVECIParsedMessage *pcMsg);
    bool GenericKeyboardDictation(IVECIParsedMessage *pcMsg);
    bool LexusInsiderCatch(IVECIParsedMessage *pcMsg);
    bool MessagingHelp(IVECIParsedMessage *pcMsg);
    bool SystemMoreHints(IVECIParsedMessage *pcMsg);
    bool MusicHelp(IVECIParsedMessage *pcMsg);
    bool PhoneHelp(IVECIParsedMessage *pcMsg);
    bool POIHelp(IVECIParsedMessage *pcMsg);
    bool POIHelpNotEnabled(IVECIParsedMessage *pcMsg);
    bool RadioHelp(IVECIParsedMessage *pcMsg);
    bool SystemSettings(IVECIParsedMessage *pcMsg);
    bool SpeakerAdaptionCatch(IVECIParsedMessage *pcMsg);
    bool GlobalTaskComplete(IVECIParsedMessage *pcMsg);
    bool TutorialCatch(IVECIParsedMessage *pcMsg);
    bool GettingStartedWithVoice(IVECIParsedMessage *pcMsg);
    bool HintFristPageRequest(IVECIParsedMessage *pcMsg);
    bool HintLastPageRequest(IVECIParsedMessage *pcMsg);
    bool VehicleInMotionRequest(IVECIParsedMessage *pcMsg);
    bool HintFirstPage(IVECIParsedMessage *pcMsg);
    bool HintLastPage(IVECIParsedMessage *pcMsg);
    bool HintPrevPage(IVECIParsedMessage *pcMsg);
    bool HintNextPage(IVECIParsedMessage *pcMsg);
    bool EscalatingError(IVECIParsedMessage *pcMsg);
    bool getHint(const std::string strStateName, const std::string strVRState);
    // Help reply
    bool FirstPageReply(VR_VoiceBoxXmlParser& parser);
    bool LastPageReply(VR_VoiceBoxXmlParser& parser);
    bool VehicleInMotionReply(VR_VoiceBoxXmlParser& parser);

    bool OnSendXMLMessage(IVECITransaction* pTrans);
    bool OnStartRecoSession(IVECITransaction* pTrans);
    bool OnCancelSpecific(IVECITransaction* pTrans);
    bool GlobalBackAlreadyAtRoot(IVECIParsedMessage *pcMsg);

    /**
     * Copy Constructor
     *
     */
    VR_VoiceBoxAgentGlobal(const VR_VoiceBoxAgentGlobal&);

    /**
     * Copy Assignment Operator
     *
     */
    VR_VoiceBoxAgentGlobal &operator=(const VR_VoiceBoxAgentGlobal&);
};


#endif // VR_VOICEBOXAGENTGLOBAL_H
/* EOF */
