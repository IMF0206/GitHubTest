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
 * @file VR_VoiceBoxEngineCallback.h
 * @brief interface for VR_VoiceBoxEngineCallback.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VOICEBOXENGINECALLBACK_H
#define VR_VOICEBOXENGINECALLBACK_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

/* Standard Header */
#include <string>

/* Forward Declaration */
class IVECITransaction;

enum VR_BeepType
{
    VR_BeepType_Listening = 0,
    VR_BeepType_Confirmation,
    VR_BeepType_Done,
    VR_BeepType_TSL_Done
};

/**
 * @brief The VR_VoiceBoxEngineCallback class
 *
 * class declaration
 */
class VR_VoiceBoxEngineCallback
{
public:
    /**
     * Destructor
     *
     */
    virtual ~VR_VoiceBoxEngineCallback() {}

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
    virtual void OnRecognized(const std::string& message) = 0;

    virtual void SetUpdateGammarFlg(bool bFinishUpdate) = 0;

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
    virtual void OnInfoQueryMessage(const std::string& message) = 0;

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
    virtual void OnBeep(const VR_BeepType& beepType) = 0;

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
    virtual void OnBeep(const std::string& message) = 0;

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
    virtual void OnSpeak(IVECITransaction* pcTrans, const std::string& message) = 0;

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
    virtual void OnRequest(const std::string& message) = 0;

    virtual void OnPlayTone(const std::string& message) = 0;

    virtual void setActionResultValue(const std::string& message) = 0;

    virtual void InsertMusicUpdateGrammar(const std::string& strTransId,
        const std::string& strSourceId) = 0;

    virtual void ConfirmMusicUpdateGrammar(
        const std::string& strTransId, const bool bGrammarErrorCode) = 0;

    virtual std::string GetAudioConnected(const std::string& strAudioName) = 0;

    virtual std::string GetAudioConnected() = 0;

    virtual void ChangeAudioSourceName(std::string& strAudioName) = 0;

    virtual void GetPromptByScreenId(const std::string& strScreenId, std::string& strPrompt) = 0;

    virtual bool GetUpdateMapState() const = 0;

    virtual void SendGrammarResult(
    const std::string& strOp, const std::string& strAgent,
    const std::string& strGrammarId, const std::string& strErrCode) = 0;

    virtual void SetActiveSouceTrans(const std::string& strTransId, const std::string& strOp, const std::string& strSouceId) = 0;

    virtual void GetOpActiveSouce(const std::string& strTransId, std::string& strOp, std::string& strActiveSource) = 0;

    virtual void SetNBestScreenFlg(const bool& bNBestFlg) = 0;

    virtual bool GetNBestScreenFlg() = 0;

    virtual bool IsAppsInstalled() = 0;

    virtual bool IsAppsEnabled() = 0;

    virtual std::string GetPhonBookDBPath() = 0;

    virtual std::string GetMDActiveSourceId() = 0;

    virtual void SetGrammarInitSourceId(const std::string& strGrammarInitSourceId) = 0;

    virtual bool ResetGrammarUpdate() = 0;

    virtual bool GetEngineStatus() = 0;

    virtual bool GetMessageAvailable() = 0;

    virtual void SetCurrentTransaction(const std::string& strAgent, const std::string& strTransId) = 0;

    virtual std::pair<std::string, std::string> GetCurrentTransaction() = 0;

    virtual void SetWaitResumeGrammar(bool bWaitResumeGrammar) = 0;

    virtual void SendVRState(const std::string& strState, const std::string& strPrompt,
        const std::string& strNBestScreen, const std::string& strResult) = 0;

    virtual bool GetRouteStatus() = 0;

    virtual std::string GetCultureName() = 0;

};

#endif // VR_VOICEBOXENGINECALLBACK_H
/* EOF */
