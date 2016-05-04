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
#include "VR_Log.h"

/* VBT Header */

#ifndef VECICSTR_H
#    include "VECICStr.h"
#endif

#ifndef IVBTENGINECLIENT_H
#    include "IVBTEngineClient.h"
#endif

#ifndef VBT_SDK_AGENT_H
#    include "VBT_SDK_Agent.h"
#endif

#ifndef VBT_SDK_ACTION_TYPES_H
#    include "VBT_SDK_Action_Types.h"
#endif

/* Suntec Header */
#ifndef VR_VOICEBOXAGENTCLIMATE_H
#    include "VR_VoiceBoxAgentClimate.h"
#endif

#ifndef VR_VOICEBOXLOG_H
#    include "VR_VoiceBoxLog.h"
#endif

#ifndef VR_VOICEBOXXMLBUILDER_H
#    include "VR_VoiceBoxXmlBuilder.h"
#endif

#ifndef VR_VOICEBOXENGINECALLBACK_H
#    include "VR_VoiceBoxEngineCallback.h"
#endif

// ClimateActionHandler
struct ClimateActionHandler
{
    char*             szAction; // Action type
    ClimateMsgHandler  handler;  // Handler pointer
};

// Constructor
VR_VoiceBoxAgentClimate::VR_VoiceBoxAgentClimate(
    IVECIEngineClient& client,
    IVECIEngineCommand& engineCommand,
    VR_VoiceBoxEngineCallback& engineCallback
)
: VR_VoiceBoxEventSink(client, engineCommand, engineCallback)
{
}

// Destructor
VR_VoiceBoxAgentClimate::~VR_VoiceBoxAgentClimate()
{
}

bool
VR_VoiceBoxAgentClimate::Initialize()
{
    const ClimateActionHandler actionHandlerMap[] = {
        // Action Type
        { VBT_ACTION_TYPE_HVAC_AC_OFF,
        &VR_VoiceBoxAgentClimate::ACOff }, // "AC Off" Task Complete
        { VBT_ACTION_TYPE_HVAC_AC_ON,
        &VR_VoiceBoxAgentClimate::ACOn }, // "AC On" Task Complete
        { VBT_ACTION_TYPE_HVAC_ALREADY_AT_MAX_FAN_SPEED,
        &VR_VoiceBoxAgentClimate::TaskComplete }, // "Already At Max Fan Speed" Task Complete
        { VBT_ACTION_TYPE_HVAC_ALREADY_AT_MIN_FAN_SPEED,
        &VR_VoiceBoxAgentClimate::TaskComplete }, // "Already At Min Fan Speed" Task Complete
        { VBT_ACTION_TYPE_HVAC_CHANGE_FAN_SPEED,
        &VR_VoiceBoxAgentClimate::ChangeFanSpeed }, // "Change Fan Speed" Task Complete
        { VBT_ACTION_TYPE_HVAC_CHANGE_FAN_SPEED_ALREADY_AT_REQUESTED_FAN_SPEED,
        &VR_VoiceBoxAgentClimate::TaskComplete }, // "Change Fan Speed - Already At Requested Fan Speed" Task Complete
        { VBT_ACTION_TYPE_HVAC_CHANGE_TEMPERATURE,
        &VR_VoiceBoxAgentClimate::ChangeTemperature }, // "Change Temperature" Task Complete
        { VBT_ACTION_TYPE_HVAC_CHANGE_TEMPERATURE_OVER_MAX_VALUE,
        &VR_VoiceBoxAgentClimate::ChangeTemperature }, // "Change Temperature - Over Max Value" Task Complete
        { VBT_ACTION_TYPE_HVAC_CHANGE_TEMPERATURE_UNDER_MIN_VALUE,
        &VR_VoiceBoxAgentClimate::ChangeTemperature }, // "Change Temperature - Under Min Value" Task Complete
        { VBT_ACTION_TYPE_HVAC_CHANGE_TEMPERATURE_MAX,
        &VR_VoiceBoxAgentClimate::ChangeMaxTemperature }, // "Change Temperature Max" Task Complete
        { VBT_ACTION_TYPE_HVAC_CHANGE_TEMPERATURE_MIN,
        &VR_VoiceBoxAgentClimate::ChangeMinTemperature }, // "Change Temperature Min" Task Complete
        { VBT_ACTION_TYPE_HVAC_CONCIERGE_MODE_ALREADY_ON,
        &VR_VoiceBoxAgentClimate::TaskComplete }, // "Concierge Mode Already On" Task Complete
        { VBT_ACTION_TYPE_HVAC_CONCIERGE_MODE_ON,
        &VR_VoiceBoxAgentClimate::ConciergeModeOn }, // "Concierge Mode On" Task Complete
        { VBT_ACTION_TYPE_HVAC_DECREASE_FAN_SPEED,
        &VR_VoiceBoxAgentClimate::ChangeFanSpeed }, // "Decrease Fan Speed" Task Complete
        { VBT_ACTION_TYPE_HVAC_DECREASE_TEMPERATURE,
        &VR_VoiceBoxAgentClimate::DecreaseTemperature }, // "Decrease Temperature" Task Complete
        { VBT_ACTION_TYPE_HVAC_HVAC_CONCIERGE_SCREEN,
        &VR_VoiceBoxAgentClimate::HVACConciergeScreen }, // "HVAC Concierge Screen" Task Complete
        { VBT_ACTION_TYPE_HVAC_HVAC_FRONT_SCREEN,
        &VR_VoiceBoxAgentClimate::HVACFrontScreen }, // "HVAC Front Screen" Task Complete
        { VBT_ACTION_TYPE_HVAC_HVAC_FRONT_SEAT_SCREEN,
        &VR_VoiceBoxAgentClimate::HVACFrontSeatScreen }, // "HVAC Front Seat Screen" Task Complete
        { VBT_ACTION_TYPE_HVAC_HVAC_REAR_SCREEN,
        &VR_VoiceBoxAgentClimate::HVACRearScreen }, // "HVAC Rear Screen" Task Complete
        { VBT_ACTION_TYPE_HVAC_HVAC_REAR_SEAT_SCREEN,
        &VR_VoiceBoxAgentClimate::HVACRearSeatScreen }, // "HVAC Rear Seat Screen" Task Complete
        { VBT_ACTION_TYPE_HVAC_HVAC_SEAT_OPERATION_SCREEN,
        &VR_VoiceBoxAgentClimate::HVACSeatOperationScreen }, // "HVAC Seat Operation Screen" Task Complete
        { VBT_ACTION_TYPE_HVAC_HVAC_STEERING_SCREEN,
        &VR_VoiceBoxAgentClimate::HVACSteeringScreen }, // "HVAC Steering Screen" Task Complete
        { VBT_ACTION_TYPE_HVAC_INCREASE_FAN_SPEED,
        &VR_VoiceBoxAgentClimate::ChangeFanSpeed }, // "Increase Fan Speed" Task Complete
        { VBT_ACTION_TYPE_HVAC_INCREASE_TEMPERATURE,
        &VR_VoiceBoxAgentClimate::IncreaseTemperature }, // "Increase Temperature" Task Complete
        { VBT_ACTION_TYPE_HVAC_SID_ERROR,
        &VR_VoiceBoxAgentClimate::TaskComplete }, // "SID Error" Task Complete
        { VBT_ACTION_TYPE_HVAC_AC_ALREADY_OFF,
        &VR_VoiceBoxAgentClimate::ACAlreadyOff }, // "AC Already Off" Task Complete (re-activate VR)
        { VBT_ACTION_TYPE_HVAC_AC_ALREADY_ON,
        &VR_VoiceBoxAgentClimate::ACAlreadyOn }, // "AC Already On" Task Complete (re-activate VR)
        { VBT_ACTION_TYPE_HVAC_ALREADY_AT_MAX_TEMPERATURE,
        &VR_VoiceBoxAgentClimate::AlreadyAtMaxTemperature }, // "Already At Max Temperature" Task Complete (re-activate VR)
        { VBT_ACTION_TYPE_HVAC_ALREADY_AT_MIN_TEMPERATURE,
        &VR_VoiceBoxAgentClimate::AlreadyAtMinTemperature }, // "Already At Min Temperature" Task Complete (re-activate VR)
        { VBT_ACTION_TYPE_HVAC_CHANGE_TEMPERATURE_ALREADY_AT_REQUESTED_TEMPERATURE,
        &VR_VoiceBoxAgentClimate::ChangeTemperatureAlreadyAtRequestedTemperature }, // "Change Temperature - Already At Requested Temperature" Follow-Up
        { VBT_ACTION_TYPE_HVAC_GO_TO_HVAC,
        &VR_VoiceBoxAgentClimate::GoToHVAC }, // "Go To HVAC" Task Complete (re-activate VR)
        { VBT_ACTION_TYPE_HVAC_GET_TEMPERATURE,
        &VR_VoiceBoxAgentClimate::GetTemperature }, // "Get Temperature" Follow-Up
        { VBT_ACTION_TYPE_HVAC_GET_FAN_SPEED,
        &VR_VoiceBoxAgentClimate::GetFanSpeed }, // "Get Fan Speed" Follow-Up
        // { VBT_ACTION_TYPE_HVAC_GET_TEMPERATURE_ALREADY_AT_MAX_TEMPERATURE, // "Get Temperature - Already At Max Temperature" Task Complete
        { "Get Temperature - Already At Max Temperature",
        &VR_VoiceBoxAgentClimate::TaskComplete },
        { "Decrease Temperature - Already At Min Temperature",
        &VR_VoiceBoxAgentClimate::DecTempAlreadyMinTemp },   // Task Complete (re-activate VR)
        { "Get Fan Speed - AC Already Off",
        &VR_VoiceBoxAgentClimate::TaskComplete },         // "Get Fan Speed - AC Already Off" Task Complete
        { "Get Temperature - Already At Min Temperature",
        &VR_VoiceBoxAgentClimate::GetTempAlreadyMinTemp },  // Task Complete (re-activate VR)
        { VBT_ACTION_TYPE_HVAC_BACK,                                              // "Back"
        &VR_VoiceBoxAgentClimate::ClimateBack },
        { VBT_ACTION_TYPE_HVAC_CANCEL, // "Cancel"
        &VR_VoiceBoxAgentClimate::ClimateCancel },
        { VBT_ACTION_TYPE_HVAC_HELP, // "Help"
        &VR_VoiceBoxAgentClimate::ClimateHelp },
        { VBT_ACTION_TYPE_HVAC_START_OVER, // "Start Over"
        &VR_VoiceBoxAgentClimate::ClimateStartOver },
        { VBT_ACTION_TYPE_HVAC_SYSTEM_PAUSE, // "Start Pause"
        &VR_VoiceBoxAgentClimate::ClimateStartPause },
        { VBT_ACTION_TYPE_HVAC_SYSTEM_REPEAT, // "Start Repeat"
        &VR_VoiceBoxAgentClimate::ClimateStartRepeat },
        { VBT_ACTION_TYPE_HVAC_ESCALATING_ERROR,  // "Escalating Error"
        &VR_VoiceBoxAgentClimate::EscalatingError },

        // Driver Info
        { VBT_DRIVER_REQUEST_ACTION_TYPE_HVAC_AC_ON_REQUEST,
        &VR_VoiceBoxAgentClimate::ACOnRequest }, // "AC On Request"
        { VBT_DRIVER_REQUEST_ACTION_TYPE_HVAC_CONCIERGE_MODE_ON_REQUEST,
        &VR_VoiceBoxAgentClimate::ConciergeModeOnRequest }, // "Concierge Mode On Request"
        { VBT_DRIVER_REQUEST_ACTION_TYPE_HVAC_DECREASE_FAN_SPEED_REQUEST,
        &VR_VoiceBoxAgentClimate::DecreaseFanSpeedRequest }, // "Decrease Fan Speed Request"
        { VBT_DRIVER_REQUEST_ACTION_TYPE_HVAC_DECREASE_TEMPERATURE_REQUEST,
        &VR_VoiceBoxAgentClimate::DecreaseTemperatureRequest }, // "Decrease Temperature Request"
        { VBT_DRIVER_REQUEST_ACTION_TYPE_HVAC_FAN_SPEED_VALID_REQUEST,
        &VR_VoiceBoxAgentClimate::FanSpeedValidRequest }, // "Fan Speed Valid Request"
        { VBT_DRIVER_REQUEST_ACTION_TYPE_HVAC_INCREASE_FAN_SPEED_REQUEST,
        &VR_VoiceBoxAgentClimate::IncreaseFanSpeedRequest }, // "Increase Fan Speed Request"
        { VBT_DRIVER_REQUEST_ACTION_TYPE_HVAC_INCREASE_TEMPERATURE_REQUEST,
        &VR_VoiceBoxAgentClimate::IncreaseTemperatureRequest }, // "Increase Temperature Request"
        { VBT_DRIVER_REQUEST_ACTION_TYPE_HVAC_TEMPERATURE_MAX_REQUEST,
        &VR_VoiceBoxAgentClimate::TemperatureMaxRequest }, // "Temperature Max Request"
        { VBT_DRIVER_REQUEST_ACTION_TYPE_HVAC_TEMPERATURE_MIN_REQUEST,
        &VR_VoiceBoxAgentClimate::TemperatureMinRequest }, // "Temperature Min Request"
        { VBT_DRIVER_REQUEST_ACTION_TYPE_HVAC_TEMPERATURE_VALID_REQUEST,
        &VR_VoiceBoxAgentClimate::TemperatureValidRequest }, // "Temperature Valid Request"
        { NULL, NULL }
    };

    int i = 0;
    while (true) {
        if (NULL == actionHandlerMap[i].szAction) {
            break;
        }

        m_climateMsgHandler.insert(std::make_pair(
            actionHandlerMap[i].szAction,
            actionHandlerMap[i].handler
            ));

        ++i;
    }

    m_climateMsgResult.insert(std::make_pair("queryStatus", &VR_VoiceBoxAgentClimate::QueryStatus));
    m_climateMsgResult.insert(std::make_pair("getValidTemperature", &VR_VoiceBoxAgentClimate::GetValidTemperatureReply));
    m_climateMsgResult.insert(std::make_pair("getValidFanSpeed", &VR_VoiceBoxAgentClimate::GetValidFanSpeedReply));
    m_climateMsgResult.insert(std::make_pair("checkFanSpeed", &VR_VoiceBoxAgentClimate::CheckFanSpeed));
    m_climateMsgResult.insert(std::make_pair("checkTemperature", &VR_VoiceBoxAgentClimate::CheckTemperature));

    return true;
}

void
VR_VoiceBoxAgentClimate::UnInitialize()
{
    m_climateMsgHandler.clear();
    m_climateMsgResult.clear();
}

bool
VR_VoiceBoxAgentClimate::ReplyQueryInfo(
    VR_VoiceBoxXmlParser& parser
)
{
    std::string strActionKey = parser.getXmlKey();

    VoiceMap<std::string, ClimateResultResult>::const_iterator iterMap = m_climateMsgResult.find(strActionKey);
    if (m_climateMsgResult.cend() != iterMap) {
        if (NULL != iterMap->second) {
            (this->*(iterMap->second))(parser);
            return true;
        }
    }

    return true;
}

// QueryAgentSupport
HRESULT
VR_VoiceBoxAgentClimate::QueryAgentSupport(
    VBT_CSTR szAgent,
    VBT_BOOL *pbSupported
)
{
    if (NULL == pbSupported) {
        VR_ERROR("QueryAgentSupport: NULL Parameter");
        return E_FAIL;
    }

    CVECICStr strAgent(&m_client, szAgent);
    if (strAgent.IsEqual(VBT_AGENT_HVAC)) {
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
VR_VoiceBoxAgentClimate::OnAgentResultMessage(
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
VR_VoiceBoxAgentClimate::OnAgentDriverMessage(
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
VR_VoiceBoxAgentClimate::ProcessAgentMessage(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    if (!SendActionMessage(pcMsg)) {
        return false;
    }

    std::string strAgentResult;
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
    VoiceMap<std::string, ClimateMsgHandler>::const_iterator iterMap = m_climateMsgHandler.find(strAgentResult);
    if (m_climateMsgHandler.cend() != iterMap) {
        if (NULL != iterMap->second) {
            (this->*(iterMap->second))(pcMsg);
            return true;
        }
    }

    return false;
}

void
VR_VoiceBoxAgentClimate::TurnClimate(const std::string& strType, const std::string& strStatus)
{
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("climate", "turn");
    (void)xmlBulder.buildGivenElement(node, "type", strType, "", "");
    (void)xmlBulder.buildGivenElement(node, "status", strStatus, "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

}

bool
VR_VoiceBoxAgentClimate::ACOn(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("ACOn: %s\n", xmlMsg.Get());

    TurnClimate("AC", "ON");

    return true;
}

bool
VR_VoiceBoxAgentClimate::ACOff(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("ACOff: %s\n", xmlMsg.Get());

    TurnClimate("AC", "OFF");

    return true;
}

void
VR_VoiceBoxAgentClimate::ShowScreen(
    const std::string& strType
)
{
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("climate", "showScreen");
    (void)xmlBulder.buildGivenElement(node, "type", strType, "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);
}

bool
VR_VoiceBoxAgentClimate::HVACConciergeScreen(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("HVACConciergeScreen: %s\n", xmlMsg.Get());

    ShowScreen("CONCIERGE");

    return true;
}

bool
VR_VoiceBoxAgentClimate::HVACFrontScreen(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("HVACFrontScreen: %s\n", xmlMsg.Get());

    ShowScreen("FRONT");

    return true;
}

bool
VR_VoiceBoxAgentClimate::HVACFrontSeatScreen(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("HVACFrontSeatScreen: %s\n", xmlMsg.Get());

    ShowScreen("FRONT_SEAT");

    return true;
}

bool
VR_VoiceBoxAgentClimate::HVACRearScreen(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("HVACRearScreen: %s\n", xmlMsg.Get());

    ShowScreen("REAR");

    return true;
}

bool
VR_VoiceBoxAgentClimate::HVACRearSeatScreen(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("HVACRearSeatScreen: %s\n", xmlMsg.Get());

    ShowScreen("REAR_SEAT");

    return true;
}

bool
VR_VoiceBoxAgentClimate::HVACSeatOperationScreen(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("HVACSeatOperationScreen: %s\n", xmlMsg.Get());

    ShowScreen("SEAT_OPERATION");

    return true;
}

bool
VR_VoiceBoxAgentClimate::HVACSteeringScreen(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("HVACSteeringScreen: %s\n", xmlMsg.Get());

    ShowScreen("STEERING");

    return true;
}

bool
VR_VoiceBoxAgentClimate::ChangeFanSpeed(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("ChangeFanSpeed: %s\n", xmlMsg.Get());

    std::string strFanSpeed = GetActionParaByName(pcMsg, "Fan Speed");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("climate", "changeFanSpeed");
    (void)xmlBulder.buildGivenElement(node, "speed", strFanSpeed, "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentClimate::DecreaseTemperature(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("DecreaseTemperature: %s\n", xmlMsg.Get());

    std::string strTemperature = GetActionParaByName(pcMsg, "Temperature");

    SendChangeTemperature(strTemperature);

    return true;
}

bool
VR_VoiceBoxAgentClimate::IncreaseTemperature(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("IncreaseTemperature: %s\n", xmlMsg.Get());

    std::string strTemperature = GetActionParaByName(pcMsg, "Temperature");

    SendChangeTemperature(strTemperature);

    return true;
}

void
VR_VoiceBoxAgentClimate::SendChangeTemperature(const std::string& strTemperature)
{
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("climate", "changeTemperature");
    (void)xmlBulder.buildGivenElement(node, "degree", strTemperature, "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);
}

bool
VR_VoiceBoxAgentClimate::ChangeMinTemperature(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("ChangeMinTemperature: %s\n", xmlMsg.Get());

    SendChangeTemperature("min");

    return true;
}

bool
VR_VoiceBoxAgentClimate::ChangeMaxTemperature(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("ChangeMaxTemperature: %s\n", xmlMsg.Get());

    SendChangeTemperature("max");

    return true;
}

bool
VR_VoiceBoxAgentClimate::ChangeTemperature(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("ChangeTemperature: %s\n", xmlMsg.Get());

    std::string strTemperature = GetActionParaByName(pcMsg, "Temperature");

    SendChangeTemperature(strTemperature);

    return true;
}

bool
VR_VoiceBoxAgentClimate::ConciergeModeOn(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("ConciergeModeOn: %s\n", xmlMsg.Get());

    TurnClimate("CONCIERGE_MODE", "ON");

    return true;
}

bool
VR_VoiceBoxAgentClimate::TaskComplete(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("TaskComplete: %s\n", xmlMsg.Get());

    return true;
}

bool
VR_VoiceBoxAgentClimate::ACAlreadyOff(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("ACAlreadyOff: %s\n", xmlMsg.Get());

    m_engineCallback.OnRequest("<event name=\"startNextAgent\"><agent>climate</agent></event>");

    return true;
}

bool
VR_VoiceBoxAgentClimate::ACAlreadyOn(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("ACAlreadyOn: %s\n", xmlMsg.Get());

    m_engineCallback.OnRequest("<event name=\"startNextAgent\"><agent>climate</agent></event>");

    return true;
}

bool
VR_VoiceBoxAgentClimate::AlreadyAtMaxTemperature(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("AlreadyAtMaxTemperature: %s\n", xmlMsg.Get());

    m_engineCallback.OnRequest("<event name=\"startNextAgent\"><agent>climate</agent></event>");

    return true;
}

bool
VR_VoiceBoxAgentClimate::AlreadyAtMinTemperature(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("AlreadyAtMinTemperature: %s\n", xmlMsg.Get());

    m_engineCallback.OnRequest("<event name=\"startNextAgent\"><agent>climate</agent></event>");

    return true;
}

bool
VR_VoiceBoxAgentClimate::ChangeTemperatureAlreadyAtRequestedTemperature(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("ChangeTemperatureAlreadyAtRequestedTemperature: %s\n", xmlMsg.Get());

    // 2 \ 3 3 no change
    m_engineCallback.OnRequest("<event name=\"startNextAgent\"><agent>climate</agent></event>");

    return true;
}

bool
VR_VoiceBoxAgentClimate::GoToHVAC(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("GoToHVAC: %s\n", xmlMsg.Get());

    m_engineCallback.GetPromptByScreenId("VR-HNT-01", m_strPrompt);

    m_engineCallback.OnRequest("<event name=\"startNextAgent\"><agent>climate</agent></event>");

    return true;
}

bool
VR_VoiceBoxAgentClimate::DecTempAlreadyMinTemp(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("GoToHVAC: %s\n", xmlMsg.Get());

    m_engineCallback.OnRequest("<event name=\"startNextAgent\"><agent>climate</agent></event>");

    return true;
}

bool
VR_VoiceBoxAgentClimate::GetTempAlreadyMinTemp(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("GoToHVAC: %s\n", xmlMsg.Get());

    m_engineCallback.OnRequest("<event name=\"startNextAgent\"><agent>climate</agent></event>");

    return true;
}

bool
VR_VoiceBoxAgentClimate::GetTemperature(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("GetTemperature: %s\n", xmlMsg.Get());

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(xmlNode, "agent", "climate", "", "");
    xmlBulder.buildGivenElement(xmlNode, "content", "na_climate_set_the_temperature", "", "");
    m_engineCallback.GetPromptByScreenId("VR-HVAC-01", m_strPrompt);

    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnRecognized(strReply);

    return false;
}

bool
VR_VoiceBoxAgentClimate::GetFanSpeed(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("GetFanSpeed: %s\n", xmlMsg.Get());

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(xmlNode, "agent", "climate", "", "");
    xmlBulder.buildGivenElement(xmlNode, "content", "na_climate_set_the_fan_speed", "", "");
    m_engineCallback.GetPromptByScreenId("VR-HVAC-02", m_strPrompt);

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentClimate::ACOnRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("ACOnRequest: %s\n", xmlMsg.Get());

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("climate", "queryStatus");
    (void)xmlBulder.buildGivenElement(node, "type", "AC", "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnInfoQueryMessage(strReply);

    return true;
}

bool
VR_VoiceBoxAgentClimate::ConciergeModeOnRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("ConciergeModeOnRequest: %s\n", xmlMsg.Get());

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("climate", "queryStatus");
    (void)xmlBulder.buildGivenElement(node, "type", "CONCIERGE_MODE", "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnInfoQueryMessage(strReply);

    return true;
}

bool
VR_VoiceBoxAgentClimate::QueryStatus(
    VR_VoiceBoxXmlParser& parser
)
{
    std::string strType = parser.getValueByKey("type");
    std::string strStatus = parser.getValueByKey("status");
    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    if (NULL == spParams.ptr()) {
        return false;
    }

    std::string strReply;
    std::string strParamName;

    if ("AC" == strType) {
        strReply = "AC On Reply";
        strParamName = "AC On";
    }

    if ("CONCIERGE_MODE" == strType) {
        strReply = "Concierge Mode On Reply";
        strParamName = "Concierge Mode On";
    }

    if ("ON" == strStatus) {
        (void)spParams->AddParameter(strParamName.c_str(), "True", "", "");
    }
    else if ("OFF" == strStatus) {
        (void)spParams->AddParameter(strParamName.c_str(), "False", "", "");
    }
    else {

    }

    HRESULT retCode  = m_engineCommand.DriverActionReply("HVAC", strReply.c_str(), spParams, NULL, NULL);
    if (S_OK != retCode) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentClimate::DecreaseFanSpeedRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("DecreaseFanSpeedRequest: %s\n", xmlMsg.Get());

    GetValidFanSpeed("decrease");

    return true;
}

void
VR_VoiceBoxAgentClimate::GetValidFanSpeed(const std::string& strType)
{
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("climate", "getValidFanSpeed");
    (void)xmlBulder.buildGivenElement(node, "type", strType, "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnInfoQueryMessage(strReply);

}

bool
VR_VoiceBoxAgentClimate::DecreaseTemperatureRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("DecreaseTemperatureRequest: %s\n", xmlMsg.Get());

    GetValidTemperature("decrease");

    return true;
}

bool
VR_VoiceBoxAgentClimate::FanSpeedValidRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("FanSpeedValidRequest: %s\n", xmlMsg.Get());

    std::string strFanSpeed = GetActionParaByName(pcMsg, "Fan Speed");

    VR_VoiceBoxXmlBuilder xmlBulder;

    pugi::xml_node node = xmlBulder.buildStartActionElement("climate", "checkFanSpeed");
    (void)xmlBulder.buildGivenElement(node, "speed", strFanSpeed, "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnInfoQueryMessage(strReply);

    return true;
}

bool
VR_VoiceBoxAgentClimate::IncreaseFanSpeedRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("IncreaseFanSpeedRequest: %s\n", xmlMsg.Get());

    GetValidFanSpeed("increase");

    return true;
}

void
VR_VoiceBoxAgentClimate::GetValidTemperature(const std::string& strType)
{
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("climate", "getValidTemperature");
    (void)xmlBulder.buildGivenElement(node, "type", strType, "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnInfoQueryMessage(strReply);
}

bool
VR_VoiceBoxAgentClimate::IncreaseTemperatureRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("IncreaseTemperatureRequest: %s\n", xmlMsg.Get());

    GetValidTemperature("increase");

    return true;
}

bool
VR_VoiceBoxAgentClimate::TemperatureMaxRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("TemperatureMaxRequest: %s\n", xmlMsg.Get());

    GetValidTemperature("max");

    return true;
}

bool
VR_VoiceBoxAgentClimate::TemperatureMinRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("TemperatureMinRequest: %s\n", xmlMsg.Get());

    GetValidTemperature("min");

    return true;
}

bool
VR_VoiceBoxAgentClimate::TemperatureValidRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("TemperatureValidRequest: %s\n", xmlMsg.Get());

    std::string strTemperature = GetActionParaByName(pcMsg, "Temperature");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("climate", "checkTemperature");
    (void)xmlBulder.buildGivenElement(node, "degree", strTemperature, "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnInfoQueryMessage(strReply);

    return true;
}

bool
VR_VoiceBoxAgentClimate::CheckFanSpeed(
    VR_VoiceBoxXmlParser& parser
)
{
    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    if (NULL == spParams.ptr()) {
        return false;
    }

    // fan speed min\max
    std::string strStatus = parser.getValueByKey("status");
    if ("0" == strStatus) {
        strStatus = "Success";
    }
    else if ("1" == strStatus) {
        strStatus = "Already At Requested Fan Speed";
    }
    else if ("2" == strStatus) {
        strStatus = "Invalid Fan Speed";
    }
    else {

    }

    (void)spParams->AddParameter("Status", strStatus.c_str(), "", "");

    HRESULT retCode  = m_engineCommand.DriverActionReply("HVAC", "Fan Speed Valid Reply", spParams, NULL, NULL);
    if (S_OK != retCode) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentClimate::CheckTemperature(
    VR_VoiceBoxXmlParser& parser
)
{
    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    if (NULL == spParams.ptr()) {
        return false;
    }

    std::string strStatus = parser.getValueByKey("status");

    if ("0" == strStatus) {
        strStatus = "Success";
    }
    else if ("1" == strStatus) {
        strStatus = "Already At Requested Temperature";
    }
    else if ("2" == strStatus) {
        strStatus = "Over Max Value";
    }
    else if ("3" == strStatus) {
        strStatus = "Under Min Value";
    }
    else {

    }

    (void)spParams->AddParameter("Status", strStatus.c_str(), "", "");

    HRESULT retCode  = m_engineCommand.DriverActionReply("HVAC", "Temperature Valid Reply", spParams, NULL, NULL);
    if (S_OK != retCode) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentClimate::GetValidTemperatureReply(
    VR_VoiceBoxXmlParser& parser
)
{
    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    if (NULL == spParams.ptr()) {
        return false;
    }

    std::string strType = parser.getValueByKey("type");
    std::string strStatus = parser.getValueByKey("status");
    std::string strReply;
    std::string strDegree;
    if ("decrease" == strType) {
        strReply = "Decrease Temperature Reply";
        strDegree = parser.getValueByKey("degree");
        (void)spParams->AddParameter("Temperature", strDegree.c_str(), "", "");
    }
    else if ("increase" == strType) {
        strReply = "Increase Temperature Reply";
        strDegree = parser.getValueByKey("degree");
        (void)spParams->AddParameter("Temperature", strDegree.c_str(), "", "");
    }
    else if ("max" == strType) {
        strReply = "Temperature Max Reply";
    }
    else if ("min" == strType) {
        strReply = "Temperature Min Reply";
    }
    else {

    }

    if ("0" == strStatus) {
        strStatus = "Success";
    }
    else if ("1" == strStatus) {
        strStatus = "Already At Minimum";
    }
    else if ("2" == strStatus) {
        strStatus = "Already At Maximum";
    }
    else {

    }

    (void)spParams->AddParameter("Status", strStatus.c_str(), "", "");

    HRESULT retCode  = m_engineCommand.DriverActionReply("HVAC", strReply.c_str(), spParams, NULL, NULL);
    if (S_OK != retCode) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentClimate::GetValidFanSpeedReply(
    VR_VoiceBoxXmlParser& parser
)
{
    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    if (NULL == spParams.ptr()) {
        return false;
    }

    // std::string strType = parser.getValueByKey("type");
    std::string strStatus = parser.getValueByKey("status");
    std::string strSpeed = parser.getValueByKey("speed");

    (void)spParams->AddParameter("Fan Speed", strSpeed.c_str(), "", "");

    if ("0" == strStatus) {
        strStatus = "Success";
    }
    else if ("1" == strStatus) {
        strStatus = "Already At Minimum";
    }
    else if ("2" == strStatus) {
        strStatus = "Already At Maximum";
    }
    else {

    }

    (void)spParams->AddParameter("Status", strStatus.c_str(), "", "");

    HRESULT retCode  = m_engineCommand.DriverActionReply("HVAC", "Fan Speed Reply", spParams, NULL, NULL);
    if (S_OK != retCode) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentClimate::ClimateBack(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("ClimateBack: %s\n", xmlMsg.Get());
    m_engineCallback.OnRequest("<send event=\"back-internal\" target=\"de\"/>");

    return true;
}

bool
VR_VoiceBoxAgentClimate::ClimateCancel(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("ClimateCancel: %s\n", xmlMsg.Get());

    m_engineCallback.OnRequest("<send event=\"cancel\" target=\"dm\"/>");

    return true;
}

bool
VR_VoiceBoxAgentClimate::ClimateHelp(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("ClimateHelp: %s\n", xmlMsg.Get());

    return true;
}

bool
VR_VoiceBoxAgentClimate::ClimateStartOver(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("ClimateStartOver: %s\n", xmlMsg.Get());

    m_engineCallback.OnRequest("<send event=\"startover-internal\" target=\"de\"/>");

    return true;
}

bool
VR_VoiceBoxAgentClimate::ClimateStartPause(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("ClimateStartPause: %s\n", xmlMsg.Get());

    m_engineCallback.OnRequest("<send event=\"pause-internal\" target=\"de\"/>");

    return true;
}

bool
VR_VoiceBoxAgentClimate::ClimateStartRepeat(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("ClimateStartRepeat: %s\n", xmlMsg.Get());

    m_engineCallback.OnRequest("<send event=\"repeat-internal\" target=\"de\"/>");

    return true;
}

bool
VR_VoiceBoxAgentClimate::EscalatingError(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.OnRequest("<event name=\"escalating-error\"></event>");
    return true;
}

/* EOF */
