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

/* Standard Header */

/* VBT Header */
// #ifndef VECIOUTSTR_H
// #    include "VECIOutStr.h"
// #endif
#include "VR_Log.h"

#ifndef VECICSTR_H
#    include "VECICStr.h"
#endif

// #ifndef VBT_EXTERNAL_ERRORS_H
// #    include "VBTExternalErrors.h"
// #endif

#ifndef IVBTENGINECLIENT_H
#    include "IVBTEngineClient.h"
#endif

#ifndef VBT_SDK_AGENT_H
#    include "VBT_SDK_Agent.h"
#endif

/* Suntec Header */
#ifndef VR_VOICEBOXAGENTINFO_H
#    include "VR_VoiceBoxAgentInfo.h"
#endif

#ifndef VR_VOICEBOXLOG_H
#    include "VR_VoiceBoxLog.h"
#endif

// #ifndef VR_VOICEBOXXMLBUILDER_H
// #    include "VR_VoiceBoxXmlBuilder.h"
// #endif

#ifndef VR_VOICEBOXENGINECALLBACK_H
#    include "VR_VoiceBoxEngineCallback.h"
#endif

// Constructor
VR_VoiceBoxAgentInfo::VR_VoiceBoxAgentInfo(
    IVECIEngineClient& client,
    IVECIEngineCommand& engineCommand,
    VR_VoiceBoxEngineCallback& engineCallback
)
: VR_VoiceBoxEventSink(client, engineCommand, engineCallback)
{
}

// Destructor
VR_VoiceBoxAgentInfo::~VR_VoiceBoxAgentInfo()
{
}

bool
VR_VoiceBoxAgentInfo::Initialize()
{
    return true;
}

void
VR_VoiceBoxAgentInfo::UnInitialize()
{
}

bool
VR_VoiceBoxAgentInfo::ReplyQueryInfo(
    VR_VoiceBoxXmlParser& parser
)
{
    return true;
}

// QueryAgentSupport
HRESULT
VR_VoiceBoxAgentInfo::QueryAgentSupport(
    VBT_CSTR szAgent,
    VBT_BOOL *pbSupported
)
{
    if (NULL == pbSupported) {
        VR_ERROR("QueryAgentSupport: NULL Parameter");
        return E_FAIL;
    }

    CVECICStr strAgent(&m_client, szAgent);
    // Indicate we want to receive events for Information agent messages
    if (strAgent.IsEqual(VBT_AGENT_INFORMATION)) {
        VR_LOG("QueryAgentSupport: %s", szAgent);
        *pbSupported = VBT_TRUE;
    }
    else {
        *pbSupported = VBT_FALSE;
    }

    return S_OK;
}

// OnAgentResultMessage
HRESULT
VR_VoiceBoxAgentInfo::OnAgentResultMessage(
    IVECITransaction* pTrans,
    IVECIParsedMessage* pResultMsg
)
{
    if ((NULL == pTrans) || (NULL == pResultMsg)) {
        VR_ERROR("OnAgentResultMessage: NULL Parameter");
        return E_FAIL;
    }

    if (!ProcessAgentMessage(pResultMsg)) {
        return E_FAIL;
    }

    return S_OK;
}

// OnAgentDriverMessage
HRESULT
VR_VoiceBoxAgentInfo::OnAgentDriverMessage(
    IVECIParsedMessage* pDriverMsg
)
{
    if (NULL == pDriverMsg) {
        return E_FAIL;
    }

    if (!ProcessAgentMessage(pDriverMsg)) {
        return E_FAIL;
    }

    return S_OK;
}

bool
VR_VoiceBoxAgentInfo::ProcessAgentMessage(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    if (!SendActionMessage(pcMsg)) {
        return false;
    }

    std::string strAgentResult = "";
    do {
        CVECIOutStr strActionType;
        pcMsg->GetActionType(&strActionType);
        if (NULL != strActionType.Get()) {
            strAgentResult = strActionType.Get();
            break;
        }

        CVECIOutStr strListType;
        pcMsg->GetListType(&strListType);
        if (NULL != strListType.Get()) {
            strAgentResult = strListType.Get();
            break;
        }

        return false;

    } while (false);

    VR_LOG("ProcessAgentMessage: %s", strAgentResult.c_str());

    // Dispatch the messages
    VoiceMap<std::string, InfoMsgHandler>::const_iterator iterMap = m_infoMsgHandler.find(strAgentResult);
    if (m_infoMsgHandler.cend() != iterMap) {
        if (NULL != iterMap->second) {
            (this->*(iterMap->second))(pcMsg);
            return true;
        }
    }

    return false;
}

/* EOF */