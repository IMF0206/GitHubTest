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
 * @file VR_VoiceBoxAgentClimate.h
 * @brief interface for EngineWrapper callback.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VOICEBOXAGENTCLIMATE_H
#define VR_VOICEBOXAGENTCLIMATE_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

/* Suntec Header */
#ifndef VR_VOICEBOXEVENTSINK_H
#    include "VR_VoiceBoxEventSink.h"
#endif

#ifndef MEM_MAP_H
#    include "MEM_map.h"
#endif


/* Forward Declaration */
class VR_VoiceBoxAgentClimate;

// Define the agent message handler pointer
typedef bool (VR_VoiceBoxAgentClimate::*ClimateMsgHandler)(IVECIParsedMessage*);
typedef bool (VR_VoiceBoxAgentClimate::*ClimateResultResult)(VR_VoiceBoxXmlParser&);

/**
 * @brief The VR_VoiceBoxAgentClimate class
 *
 * class declaration
 */
class VR_VoiceBoxAgentClimate : public VR_VoiceBoxEventSink
{
public:
    /**
     * Constructor
     *
     */
    VR_VoiceBoxAgentClimate(
        IVECIEngineClient& client,
        IVECIEngineCommand& engineCommand,
        VR_VoiceBoxEngineCallback& engineCallback
        );

    /**
     * Destructor
     *
     */
    virtual ~VR_VoiceBoxAgentClimate();

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

    // IVECIEventSink's pure virtual functions
    virtual HRESULT QueryAgentSupport(VBT_CSTR szAgent, VBT_BOOL *pbSupported);
    virtual HRESULT OnAgentResultMessage(IVECITransaction* pTrans, IVECIParsedMessage* pResultMsg);
    virtual HRESULT OnAgentDriverMessage(IVECIParsedMessage* pDriverMsg);

    virtual bool ProcessAgentMessage(IVECIParsedMessage *pcMsg);

private:
    VoiceMap<std::string, ClimateMsgHandler>::type m_climateMsgHandler;
    VoiceMap<std::string, ClimateResultResult>::type m_climateMsgResult;


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

    /**
     * Copy Constructor
     *
     */
    VR_VoiceBoxAgentClimate(const VR_VoiceBoxAgentClimate&);

    /**
     * Copy Assignment Operator
     *
     */
    VR_VoiceBoxAgentClimate &operator=(const VR_VoiceBoxAgentClimate&);

    bool TaskComplete(IVECIParsedMessage *pcMsg);

    bool ChangeFanSpeed(IVECIParsedMessage *pcMsg);
    bool ConciergeModeOn(IVECIParsedMessage *pcMsg);
    // bool ConciergeModeOff(IVECIParsedMessage *pcMsg);
    bool HVACConciergeScreen(IVECIParsedMessage *pcMsg);
    bool HVACFrontScreen(IVECIParsedMessage *pcMsg);
    bool HVACFrontSeatScreen(IVECIParsedMessage *pcMsg);
    bool HVACRearScreen(IVECIParsedMessage *pcMsg);
    bool HVACRearSeatScreen(IVECIParsedMessage *pcMsg);
    bool HVACSeatOperationScreen(IVECIParsedMessage *pcMsg);
    bool HVACSteeringScreen(IVECIParsedMessage *pcMsg);
    bool ChangeTemperature(IVECIParsedMessage *pcMsg);
    bool DecreaseTemperature(IVECIParsedMessage *pcMsg);
    bool IncreaseTemperature(IVECIParsedMessage *pcMsg);
    
    bool ACOn(IVECIParsedMessage *pcMsg);
    bool ACOff(IVECIParsedMessage *pcMsg);
    bool ACAlreadyOff(IVECIParsedMessage *pcMsg);
    bool ACAlreadyOn(IVECIParsedMessage *pcMsg);
    bool AlreadyAtMaxTemperature(IVECIParsedMessage *pcMsg);
    bool AlreadyAtMinTemperature(IVECIParsedMessage *pcMsg);
    bool ChangeTemperatureAlreadyAtRequestedTemperature(IVECIParsedMessage *pcMsg);
    bool GoToHVAC(IVECIParsedMessage *pcMsg);
    bool GetTemperature(IVECIParsedMessage *pcMsg);
    bool GetFanSpeed(IVECIParsedMessage *pcMsg);
    bool ClimateBack(IVECIParsedMessage *pcMsg);
    bool ClimateCancel(IVECIParsedMessage *pcMsg);
    bool ClimateHelp(IVECIParsedMessage *pcMsg);
    bool ClimateStartOver(IVECIParsedMessage *pcMsg);
    bool ClimateStartPause(IVECIParsedMessage *pcMsg);
    bool ClimateStartRepeat(IVECIParsedMessage *pcMsg);
    bool EscalatingError(IVECIParsedMessage *pcMsg);
    
    bool ACOnRequest(IVECIParsedMessage *pcMsg);
    bool ConciergeModeOnRequest(IVECIParsedMessage *pcMsg);
    bool DecreaseFanSpeedRequest(IVECIParsedMessage *pcMsg);
    bool DecreaseTemperatureRequest(IVECIParsedMessage *pcMsg);
    bool FanSpeedValidRequest(IVECIParsedMessage *pcMsg);
    bool IncreaseFanSpeedRequest(IVECIParsedMessage *pcMsg);
    bool IncreaseTemperatureRequest(IVECIParsedMessage *pcMsg);
    bool TemperatureMaxRequest(IVECIParsedMessage *pcMsg);
    bool TemperatureMinRequest(IVECIParsedMessage *pcMsg);
    bool TemperatureValidRequest(IVECIParsedMessage *pcMsg);
    bool ChangeMaxTemperature(IVECIParsedMessage *pcMsg);
    bool ChangeMinTemperature(IVECIParsedMessage *pcMsg);
    bool DecTempAlreadyMinTemp(IVECIParsedMessage *pcMsg);
    bool GetTempAlreadyMinTemp(IVECIParsedMessage *pcMsg);

    bool QueryStatus(VR_VoiceBoxXmlParser& parser);
    bool CheckFanSpeed(VR_VoiceBoxXmlParser& parser);
    bool CheckTemperature(VR_VoiceBoxXmlParser& parser);
    bool GetValidTemperatureReply(VR_VoiceBoxXmlParser& parser);
    bool GetValidFanSpeedReply(VR_VoiceBoxXmlParser& parser);

    void ShowScreen(const std::string& strType);
    void SendChangeTemperature(const std::string& strTemperature);
    void TurnClimate(const std::string& strType, const std::string& strStatus);
    void GetValidTemperature(const std::string& strType);
    void GetValidFanSpeed(const std::string& strType);
    void ChangeTemperature(const std::string& strTemperature);
};


#endif // VR_VOICEBOXAGENTCLIMATE_H
/* EOF */