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


#include <stdio.h>
/* VBT Header */
#ifndef VECIOUTSTR_H
#    include "VECIOutStr.h"
#endif

#ifndef VECICSTR_H
#    include "VECICStr.h"
#endif

#ifndef VBT_EXTERNAL_ERRORS_H
#    include "VBTExternalErrors.h"
#endif

#ifndef VBTERRORTOSTRING_H
#    include "VBTErrorToString.h"
#endif

#ifndef IVBTENGINECLIENT_H
#    include "IVBTEngineClient.h"
#endif

#ifndef VBT_SYSTEM_NOTIFICATION_CODES_H
#    include "VBTSystemNotificationCodes.h"
#endif

#ifndef VBT_SDK_AGENT_H
#    include "VBT_SDK_Agent.h"
#endif

#ifndef VBT_SDK_ACTION_TYPES_H
#    include "VBT_SDK_Action_Types.h"
#endif

/* Suntec Header */
#ifndef VR_VOICEBOXAGENTGLOBAL_H
#    include "VR_VoiceBoxAgentGlobal.h"
#endif

#ifndef VR_DIALOGENGINELISTENER_H
#    include "VR_DialogEngineListener.h"
#endif

#ifndef VR_VOICEBOXXMLBUILDER_H
#    include "VR_VoiceBoxXmlBuilder.h"
#endif

#ifndef VR_AUDIOSTREAMIF_H
#    include "VR_AudioStreamIF.h"
#endif

#ifndef VR_VOICEBOXENGINECALLBACK_H
#    include "VR_VoiceBoxEngineCallback.h"
#endif

#ifndef VR_VOICEBOXCATALOGPHONE_H
#    include "VR_VoiceBoxCatalogPhone.h"
#endif

#ifndef VR_VOICEBOXLOG_H
#    include "VR_VoiceBoxLog.h"
#endif

#ifndef VR_CONFIGUREIF_H
#    include "VR_ConfigureIF.h"
#endif

#include "VR_PerformanceLog.h"

using namespace nutshell;
/**
 * @brief The GlobalActionHandler class
 *
 * class declaration
 */
struct GlobalActionHandler
{
    char*             szAction; // Action type
    GlobalMsgHandler  handler;  // Handler pointer
};

// Constructor
VR_VoiceBoxAgentGlobal::VR_VoiceBoxAgentGlobal(
    IVECIEngineClient& client,
    IVECIEngineCommand& engineCommand,
    VR_VoiceBoxEngineCallback& engineCallback
)
: VR_VoiceBoxEventSink(client, engineCommand, engineCallback)
, m_bTslState(false)
, m_bGrammarErr(false)
, m_lLevelValue(0)
, m_pcUsedTrans(NULL)
, m_iGrammarErrCount(0)
{
}

// Destructor
VR_VoiceBoxAgentGlobal::~VR_VoiceBoxAgentGlobal()
{
}

bool
VR_VoiceBoxAgentGlobal::Initialize()
{
    const GlobalActionHandler actionHandlerMap[] = {
        // Action Type : Speaker Adaptation Agent
        { VBT_ACTION_TYPE_SPEAKER_ADAPTATION_COMMAND_01_COMPLETED, // "Command 01 Completed"
            &VR_VoiceBoxAgentGlobal::SpeakerAdaptationCommand },
        { VBT_ACTION_TYPE_SPEAKER_ADAPTATION_COMMAND_02_COMPLETED, // "Command 02 Completed"
            &VR_VoiceBoxAgentGlobal::SpeakerAdaptationCommand },
        { VBT_ACTION_TYPE_SPEAKER_ADAPTATION_COMMAND_03_COMPLETED, // "Command 03 Completed"
            &VR_VoiceBoxAgentGlobal::SpeakerAdaptationCommand },
        { VBT_ACTION_TYPE_SPEAKER_ADAPTATION_COMMAND_04_COMPLETED, // "Command 04 Completed"
            &VR_VoiceBoxAgentGlobal::SpeakerAdaptationCommand },
        { VBT_ACTION_TYPE_SPEAKER_ADAPTATION_COMMAND_05_COMPLETED, // "Command 05 Completed"
            &VR_VoiceBoxAgentGlobal::SpeakerAdaptationCommand },
        { VBT_ACTION_TYPE_SPEAKER_ADAPTATION_COMMAND_06_COMPLETED, // "Command 06 Completed"
            &VR_VoiceBoxAgentGlobal::SpeakerAdaptationCommand },
        { VBT_ACTION_TYPE_SPEAKER_ADAPTATION_COMMAND_07_COMPLETED, // "Command 07 Completed"
            &VR_VoiceBoxAgentGlobal::SpeakerAdaptationCommand },
        { VBT_ACTION_TYPE_SPEAKER_ADAPTATION_COMMAND_08_COMPLETED, // "Command 08 Completed"
            &VR_VoiceBoxAgentGlobal::SpeakerAdaptationCommand },
        { VBT_ACTION_TYPE_SPEAKER_ADAPTATION_COMMAND_09_COMPLETED, // "Command 09 Completed"
            &VR_VoiceBoxAgentGlobal::SpeakerAdaptationCommand },
        { VBT_ACTION_TYPE_SPEAKER_ADAPTATION_COMMAND_10_COMPLETED, // "Command 10 Completed"
            &VR_VoiceBoxAgentGlobal::SpeakerAdaptationCommand },
        { VBT_ACTION_TYPE_SPEAKER_ADAPTATION_START_SPEAKER_ADAPTATION, // "Start Speaker Adaptation"
            &VR_VoiceBoxAgentGlobal::StartSpeakerAdaptation },

        // Action Type : System Agent
        // VBT_ACTION_TYPE_SPEAKER_ADAPTATION_BACK
        { VBT_ACTION_TYPE_SYSTEM_BACK,                       // "Back"
            &VR_VoiceBoxAgentGlobal::SystemBack },
        { VBT_ACTION_TYPE_SYSTEM_BEGINNING_OF_LIST,          // "Beginning of List"
            &VR_VoiceBoxAgentGlobal::SystemBeginOfList },
        // VBT_ACTION_TYPE_SPEAKER_ADAPTATION_CANCEL
        { VBT_ACTION_TYPE_SYSTEM_CANCEL,                     // "Cancel"
            &VR_VoiceBoxAgentGlobal::SystemCancel },
        { VBT_ACTION_TYPE_SYSTEM_CS_VR_COMPLETE,             // "CS VR Complete"
            &VR_VoiceBoxAgentGlobal::SystemCSVRComplete },
        { VBT_ACTION_TYPE_SYSTEM_END_OF_LIST,                // "End of List"
            &VR_VoiceBoxAgentGlobal::SystemEndOfList },
        { VBT_ACTION_TYPE_SYSTEM_ESCALATING_ERROR,           // "Escalating Error"
            &VR_VoiceBoxAgentGlobal::EscalatingError },
        { VBT_ACTION_TYPE_SYSTEM_ESCALATING_ERROR_LIMIT_REACHED, // "Escalating Error Limit Reached"
            &VR_VoiceBoxAgentGlobal::SystemErrorLimitReached },
        { VBT_ACTION_TYPE_SYSTEM_EXIT,                       // "Exit"
            &VR_VoiceBoxAgentGlobal::NullOperation },
        { VBT_ACTION_TYPE_SYSTEM_FIRST_PAGE,                 // "First Page"
            &VR_VoiceBoxAgentGlobal::SystemFirstPage },
        { VBT_ACTION_TYPE_SYSTEM_GENERIC_CONFIRMATION,       // "Generic Confirmation"
            &VR_VoiceBoxAgentGlobal::SystemGenericConfirm },
        { VBT_ACTION_TYPE_SYSTEM_GENERIC_REJECTION,          // "Generic Rejection"
            &VR_VoiceBoxAgentGlobal::SystemGenericReject },
        { VBT_ACTION_TYPE_SYSTEM_GREETING,                   // "Greeting"
            &VR_VoiceBoxAgentGlobal::SystemGreeting },
        // VBT_ACTION_TYPE_SPEAKER_ADAPTATION_HELP
        { VBT_ACTION_TYPE_SYSTEM_HELP,                       // "Help"
            &VR_VoiceBoxAgentGlobal::SystemHelp },
        { VBT_ACTION_TYPE_SYSTEM_LAST_PAGE,                  // "Last Page"
            &VR_VoiceBoxAgentGlobal::SystemLastPage },
        { VBT_ACTION_TYPE_SYSTEM_MORE_HINTS,                 // "More Hints"
            &VR_VoiceBoxAgentGlobal::SystemMoreHints },
        { VBT_ACTION_TYPE_SYSTEM_NEXT_PAGE,                  // "Next Page"
            &VR_VoiceBoxAgentGlobal::SystemNextPage },
        { VBT_ACTION_TYPE_SYSTEM_NO_RECO,                    // "No Reco"
            &VR_VoiceBoxAgentGlobal::SystemNoReco },
        { VBT_ACTION_TYPE_SYSTEM_OFFBOARD_CONNECTIVITY_ERROR, // "Off-board Connectivity Error"
            &VR_VoiceBoxAgentGlobal::SystemConnectError },
        { VBT_ACTION_TYPE_SYSTEM_PREVIOUS_PAGE,               // "Previous Page"
            &VR_VoiceBoxAgentGlobal::SystemPreviousPage },
        { VBT_ACTION_TYPE_SYSTEM_SETTINGS,                    // "Settings"
            &VR_VoiceBoxAgentGlobal::SystemSettings },
        // VBT_ACTION_TYPE_SPEAKER_ADAPTATION_START_OVER
        { VBT_ACTION_TYPE_SYSTEM_START_OVER,                  // "Start Over"
            &VR_VoiceBoxAgentGlobal::SystemStartOver },
        // VBT_ACTION_TYPE_SPEAKER_ADAPTATION_SYSTEM_PAUSE
        { VBT_ACTION_TYPE_SYSTEM_SYSTEM_PAUSE,                // "System Pause"
            &VR_VoiceBoxAgentGlobal::SystemPause },
        // VBT_ACTION_TYPE_SPEAKER_ADAPTATION_SYSTEM_REPEAT
        { VBT_ACTION_TYPE_SYSTEM_SYSTEM_REPEAT,               // "System Repeat"
            &VR_VoiceBoxAgentGlobal::SystemRepeat },
        { VBT_ACTION_TYPE_SYSTEM_VERSION_INFO,                // "Version Info"
            &VR_VoiceBoxAgentGlobal::SystemVersionInfo },
        { VBT_ACTION_TYPE_SYSTEM_WHAT_CAN_I_SAY,              // "What Can I Say"
            &VR_VoiceBoxAgentGlobal::SystemWhatCanISay },
        { "Back - Already At Root",              // "Back - Already At Root"
            &VR_VoiceBoxAgentGlobal::GlobalBackAlreadyAtRoot },
        // help
        { "Hints Navigation - Go To First Page",   // "Hints Navigation - Go To First Page"
            &VR_VoiceBoxAgentGlobal::HintFirstPage },
        { "Hints Navigation - Go To Last Page",    // "Hints Navigation - Go To Last Page"
            &VR_VoiceBoxAgentGlobal::HintLastPage },
        { "Hints Navigation - Next Page",          // "Hints Navigation - Next Page"
            &VR_VoiceBoxAgentGlobal::HintNextPage },
        { "Hints Navigation - Previous Page",      // "Hints Navigation - Previous Page"
            &VR_VoiceBoxAgentGlobal::HintPrevPage },
        { VBT_ACTION_TYPE_HELP_ADDRESS_ENTRY_HELP,              // "Address Entry Help"
            &VR_VoiceBoxAgentGlobal::AddressEntryHelp },
        { VBT_ACTION_TYPE_HELP_ADDRESS_ENTRY_HELP_NOT_ENABLED,              // "Address Entry Help Not Enabled"
            &VR_VoiceBoxAgentGlobal::AddressEntryHelpNotEnabled },
        { VBT_ACTION_TYPE_HELP_APPS_HELP,              // "Apps Help"
            &VR_VoiceBoxAgentGlobal::AppsHelp },
        { VBT_ACTION_TYPE_HELP_APPS_HELP_NOT_ENABLED,              // "Apps Help Not Enabled"
            &VR_VoiceBoxAgentGlobal::AppsHelpNotEnabled },
        { VBT_ACTION_TYPE_HELP_CONFIRM_TUTORIAL_CONFIRM,              // "Confirm Tutorial - Confirm"
            &VR_VoiceBoxAgentGlobal::ConfirmTutorialConfirm },
        { VBT_ACTION_TYPE_HELP_CONFIRM_TUTORIAL_REJECT,              // "Confirm Tutorial - Reject"
            &VR_VoiceBoxAgentGlobal::ConfirmTutorialReject },
        { VBT_ACTION_TYPE_HELP_DESTINATION_HELP,              // "Destination Help"
            &VR_VoiceBoxAgentGlobal::DestinationHelp },
        { VBT_ACTION_TYPE_HELP_DESTINATION_HELP_NOT_ENABLED,              // "Destination Help Not Enabled"
            &VR_VoiceBoxAgentGlobal::DestinationHelpNotEnabled },
        // { VBT_ACTION_TYPE_SYSTEM_WHAT_CAN_I_SAY,              // "Exit"
        //    &VR_VoiceBoxAgentGlobal::SystemExit },
        { VBT_ACTION_TYPE_HELP_HVAC_HELP,              // "HVAC Help"
            &VR_VoiceBoxAgentGlobal::HVACHelp },
        { VBT_ACTION_TYPE_HELP_INFORMATION_HELP,              // "Information Help"
            &VR_VoiceBoxAgentGlobal::InformationHelp },
        { VBT_ACTION_TYPE_HELP_INFORMATION_HELP_NOT_ENABLED,              // "Information Help Not Enabled"
            &VR_VoiceBoxAgentGlobal::InformationHelpNotEnabled },
        { VBT_ACTION_TYPE_HELP_KEYBOARD_DICTATION,              // "Keyboard Dictation"
            &VR_VoiceBoxAgentGlobal::GenericKeyboardDictation },
        { VBT_ACTION_TYPE_HELP_LEXUS_INSIDER_CATCH,              // "Lexus Insider Catch"
            &VR_VoiceBoxAgentGlobal::LexusInsiderCatch },
        { VBT_ACTION_TYPE_HELP_MESSAGING_HELP,              // "Messaging Help"
            &VR_VoiceBoxAgentGlobal::MessagingHelp },
        // { VBT_ACTION_TYPE_SYSTEM_WHAT_CAN_I_SAY,              // "More Hints"
        //    &VR_VoiceBoxAgentGlobal::SystemMoreHints },
        { VBT_ACTION_TYPE_HELP_MUSIC_HELP,              // "Music Help"
            &VR_VoiceBoxAgentGlobal::MusicHelp },
        { VBT_ACTION_TYPE_HELP_PHONE_HELP,              // "Phone Help"
            &VR_VoiceBoxAgentGlobal::PhoneHelp },
        { VBT_ACTION_TYPE_HELP_POI_HELP,              // "POI Help"
            &VR_VoiceBoxAgentGlobal::POIHelp },
        { VBT_ACTION_TYPE_HELP_POI_HELP_NOT_ENABLED,              // "POI Help Not Enabled"
            &VR_VoiceBoxAgentGlobal::POIHelpNotEnabled },
        { VBT_ACTION_TYPE_HELP_RADIO_HELP,              // "Radio Help"
            &VR_VoiceBoxAgentGlobal::RadioHelp },
        // { VBT_ACTION_TYPE_SYSTEM_WHAT_CAN_I_SAY,              // "Settings"
        //    &VR_VoiceBoxAgentGlobal::SystemSettings },
        { VBT_ACTION_TYPE_HELP_SPEAKER_ADAP_CATCH,              // "Speaker Adap Catch"
            &VR_VoiceBoxAgentGlobal::SpeakerAdaptionCatch },
        { VBT_ACTION_TYPE_HELP_SPEAKER_ADAP_CATCH_WHILE_DRIVING,              // "Speaker Adap Catch while Driving"
            &VR_VoiceBoxAgentGlobal::GlobalTaskComplete },
        { VBT_ACTION_TYPE_HELP_TUTORIAL_CATCH,              // "Tutorial Catch"
            &VR_VoiceBoxAgentGlobal::TutorialCatch },
        { "Getting Started With Voice",              // "Tutorial Catch"
            &VR_VoiceBoxAgentGlobal::GettingStartedWithVoice },
        { VBT_ACTION_TYPE_HELP_TUTORIAL_CATCH_WHILE_DRIVING,              // "Tutorial Catch while Driving"
            &VR_VoiceBoxAgentGlobal::GlobalTaskComplete },
        { "Hints Navigation - First Page Request",    // "Hints Navigation - First Page Request"
            &VR_VoiceBoxAgentGlobal::HintFristPageRequest },
        { "Hints Navigation - Last Page Request",     // "Hints Navigation - Last Page Request"
            &VR_VoiceBoxAgentGlobal::HintLastPageRequest },
        { VBT_DRIVER_REQUEST_ACTION_TYPE_HELP_VEHICLE_IN_MOTION_REQUEST,              // "Vehicle in Motion Request"
            &VR_VoiceBoxAgentGlobal::VehicleInMotionRequest },
        { NULL, NULL }
    };

    int i = 0;
    while (true) {
        if (NULL == actionHandlerMap[i].szAction) {
            break;
        }

        m_globalMsgHandler.insert(std::make_pair(
            actionHandlerMap[i].szAction,
            actionHandlerMap[i].handler
            ));

        ++i;
    }

    // m_globalMsgHandler.insert(std::make_pair("VBT Error", &VR_VoiceBoxAgentGlobal::SystemVBTError));
    // m_globalMsgHandler.insert(std::make_pair("VBT Send Command", &VR_VoiceBoxAgentGlobal::SystemSendCommand));

    // help
    // m_globalMsgHandler.insert(std::make_pair("Exit", &VR_VoiceBoxAgentGlobal::SystemExit));
    // m_globalMsgHandler.insert(std::make_pair(VBT_ACTION_TYPE_SYSTEM_MORE_HINTS, &VR_VoiceBoxAgentGlobal::SystemMoreHints));
    // m_globalMsgHandler.insert(std::make_pair("Settings", &VR_VoiceBoxAgentGlobal::SystemSettings));

    m_globalActionResultHandler.insert(std::make_pair("queryVehicleinMotion", &VR_VoiceBoxAgentGlobal::VehicleInMotionReply));
    m_globalActionResultHandler.insert(std::make_pair("queryHintFirstPage", &VR_VoiceBoxAgentGlobal::FirstPageReply));
    m_globalActionResultHandler.insert(std::make_pair("queryHintLastPage", &VR_VoiceBoxAgentGlobal::LastPageReply));
    return true;
}

void
VR_VoiceBoxAgentGlobal::UnInitialize()
{
    m_globalMsgHandler.clear();
    m_globalActionResultHandler.clear();
}

bool
VR_VoiceBoxAgentGlobal::ReplyQueryInfo(
    VR_VoiceBoxXmlParser& parser
)
{
    std::string strActionKey = parser.getValueByKey("op");
    // Dispatch the messages
    VoiceMap<std::string, GlobalActionResultHandler>::const_iterator iterMap = m_globalActionResultHandler.find(strActionKey);
    if (m_globalActionResultHandler.cend() != iterMap) {
        if (NULL != iterMap->second) {
            (this->*(iterMap->second))(parser);
            return true;
        }
    }

    return true;
}

bool
VR_VoiceBoxAgentGlobal::ProcessMessage(
    const std::string& message
)
{
    // Start Recognition Session
    if (std::string::npos != message.find("VBT Button Down")) {
        m_bTslState = true;
    }
    else if (std::string::npos != message.find("startNativeVR")) {
        m_bTslState = false;
    }
    else if (std::string::npos != message.find("clientRenderDone")) {
        if (m_pcUsedTrans != NULL) {
            m_pcUsedTrans->ClientRenderDone();
            m_pcUsedTrans->Release();
            m_pcUsedTrans = NULL;
        }
    }
    else {
        // do nothing
    }

    return true;
}

// QueryAgentSupport
HRESULT
VR_VoiceBoxAgentGlobal::QueryAgentSupport(
    VBT_CSTR szAgent,
    VBT_BOOL *pbSupported
)
{
    if (NULL == pbSupported) {
        VR_ERROR("QueryAgentSupport: NULL Parameter");
        return E_FAIL;
    }

    if (m_bTslState) {
        *pbSupported = VBT_TRUE;
        return S_OK;
    }

    CVECICStr strAgent(&m_client, szAgent);
    if (strAgent.IsEqual(VBT_AGENT_SYSTEM)
        || strAgent.IsEqual(VBT_AGENT_HELP)
        || strAgent.IsEqual(VBT_AGENT_SPEAKER_ADAPTATION)
        || strAgent.IsEqual(_T("Root"))) {
        VR_LOG("QueryAgentSupport: %s", szAgent);
        *pbSupported = VBT_TRUE;
    }
    else {
        *pbSupported = VBT_FALSE;
    }

    return S_OK;
}

// OnProcessStatus
HRESULT
VR_VoiceBoxAgentGlobal::OnProcessStatus(
    VBT_CSTR szStatus
)
{
    if (NULL == szStatus) {
        return E_FAIL;
    }

    return S_OK;
}

// OnSystemNotification
HRESULT
VR_VoiceBoxAgentGlobal::OnSystemNotification(
    VBT_ULONG ulCode,
    IVECIParameterSet *pParameterSet
)
{
    if (NULL == pParameterSet) {
        return E_FAIL;
    }

    std::string strState;

    CVECIOutStr szMessage;
    (void)pParameterSet->GetParameterInfo("Message", AttribValue, &szMessage);
    std::string strMessage;
    if (NULL != szMessage.Get()) {
        strMessage = szMessage.Get();
        VR_LOG("OnSystemNotification: %s [0x%x]\n", szMessage.Get(), ulCode);
    }
    else {
        VR_LOG("Meaasge in pParameterSet of SystemNotification is NULL !!!");
    }

    switch (ulCode) {
    case VBT_NOTIFICATION_ASR_LISTENING:
        {
            VR_LOGP("DE: ASR listen begin...case: 212-133-99 212-134-99 212-135-99 212-136-99");
            VR_AudioStreamIF* pcAudioStream = VR_AudioStreamIF::Instance();
            if (NULL != pcAudioStream) {
                pcAudioStream->StartAudioIn();
            }

            strState = "listening";
        }
        break;

    case VBT_NOTIFICATION_ASR_NOT_LISTENING:
        {
            VR_LOGP("DE: ASR not listening...");
            VR_AudioStreamIF* pcAudioStream = VR_AudioStreamIF::Instance();
            if (NULL != pcAudioStream) {
                pcAudioStream->StopAudioIn();
            }

            // Notify the XML message to the external service
            m_engineCallback.OnBeep(VR_BeepType_Confirmation);
        }
        break;

    case VBT_NOTIFICATION_RECOGNIZED:
        // Play Confirmation Tone
        {
            strState = "processing";
            VR_LOGP("DE: ASR recognized end....");
        }
        break;
    case VBT_NOTIFICATION_NOT_RECOGNIZED:
        {
            strState = "failed";
        }
        break;
    case VBT_NOTIFICATION_ASR_GRAMMAR_DYNAMIC_ID_ERROR:
        {
            m_bGrammarErr = true;
            m_iGrammarErrCount++;
            const int MAXERRCOUNT = 3;
            if (m_iGrammarErrCount >= MAXERRCOUNT) {
                m_bGrammarErr = false;
                m_iGrammarErrCount = 0;
            }
        }
        break;
    // case VBT_NOTIFICATION_TTS_PAUSED:
    //     // Play Confirmation Tone
    //     {
    //         strState = "paused";
    //     }
    //     break;
    // case VBT_NOTIFICATION_ASR_BEGINNING_OF_UTTERANCE:
    //     // Play Confirmation Tone
    //     {
    //         strState = "speechBegin";
    //     }
    //     break;
    // case VBT_NOTIFICATION_ASR_END_OF_UTTERANCE:
    //     // Play Confirmation Tone
    //     {
    //         strState = "recognizing";
    //     }
    //     break;
    // case VBT_NOTIFICATION_TYPE_TTS_STATUS:
    //     // Play Confirmation Tone
    //     {
    //         strState = "reading";
    //     }
    //     break;

    // case VBT_NOTIFICATION_NOT_RECOGNIZED:
    //     // Play Error Tone
    //     break;

    default:
        break;
    }

    if (m_bTslState) {
        std::string headerFormat = "<action name=\"notifyAppXmlMessage\">"
              "<Message source=\"VBT\" class=\"System\" ref=\"Client\">"
                "<Action type=\"VBT System Notification\">"
                  "<Parameter name=\"Code\" value=\"0x%lx\"/>"
                  "<Parameter name=\"Message\" value=\"";
        std::string footer = "\"/>"
                "</Action>"
              "</Message>"
            "</action>";
        const int nFormatLen = 1024;
        char header[nFormatLen] = { 0 };
        snprintf(header, sizeof(header), headerFormat.c_str(), ulCode);
        std::string notification = std::string(header) + std::string(szMessage.Get()) + footer;
        m_engineCallback.OnRecognized(notification);
        return S_OK;
    }

    if (!strState.empty()) {
        std::string strResult;
        std::string strNBestScreen = "false";
        std::string strPrompt;

        if ("listening" == strState) {
            strPrompt = m_strPrompt;
        }

        if ((strState == "processing")
            && (NULL != szMessage.Get())) {
            strResult = szMessage.Get();

            CVECIOutStr szAsrType;
            (void)pParameterSet->GetParameterInfo("AsrType", AttribValue, &szAsrType);
            if (NULL != szAsrType.Get()) {
                std::string strAsrType = szAsrType.Get();

                // when result is address voiceID
                if (std::string::npos != strAsrType.find("OSD")) {
                    strResult = "";
                    VR_LOG("Now recognized result is address.");
                }
            }
            else {
                VR_LOG("AsrType in pParameterSet of SystemNotification is NULL !!!");
            }

            if (m_engineCallback.GetNBestScreenFlg()) {
                if (std::string::npos != strMessage.rfind("1")
                    || (std::string::npos != strMessage.rfind("2"))
                    || (std::string::npos != strMessage.rfind("3"))
                    || (std::string::npos != strMessage.rfind("4"))
                    || (std::string::npos != strMessage.rfind("5"))) {
                    strNBestScreen = "true";
                }
            }
        }

        m_engineCallback.SendVRState(strState, strPrompt, strNBestScreen, strResult);
    }

    return S_OK;
}

// OnAgentResultMessage
HRESULT
VR_VoiceBoxAgentGlobal::OnAgentResultMessage(
    IVECITransaction* pTrans,
    IVECIParsedMessage* pResultMsg
)
{
    if ((NULL == pTrans) || (NULL == pResultMsg)) {
        return E_FAIL;
    }

    if (m_bTslState) {
        CVECIOutStr xmlMsg;
        HRESULT result = pResultMsg->GetString(&xmlMsg);
        if (FAILED(result) || (NULL == xmlMsg.Get())) {
            return E_FAIL;
        }

        m_pcUsedTrans = pTrans;
        m_pcUsedTrans->AddRef();

        SendActionMessage(pResultMsg);

        std::string header = "<action name=\"notifyAppXmlMessage\">";
        std::string footer = "</action>";
        std::string strResultMsg = header + std::string(xmlMsg.Get()) + footer;
        m_engineCallback.OnRecognized(strResultMsg);
        return S_OK;
    }

    if (!ProcessAgentMessage(pResultMsg)) {
        return E_FAIL;
    }

    return S_OK;
}

// OnAgentDriverMessage
HRESULT
VR_VoiceBoxAgentGlobal::OnAgentDriverMessage(
    IVECIParsedMessage* pDriverMsg
)
{
    if (NULL == pDriverMsg) {
        return E_FAIL;
    }

    if (m_bTslState) {
        CVECIOutStr xmlMsg;
        HRESULT result = pDriverMsg->GetString(&xmlMsg);
        if (FAILED(result) || (NULL == xmlMsg.Get())) {
            return E_FAIL;
        }

        SendActionMessage(pDriverMsg);

        std::string header = "<action name=\"notifyAppXmlMessage\">";
        std::string footer = "</action>";
        std::string strDriverMsg = header + std::string(xmlMsg.Get()) + footer;
        m_engineCallback.OnRecognized(strDriverMsg);
        return S_OK;
    }

    if (!ProcessAgentMessage(pDriverMsg)) {
        return E_FAIL;
    }

    return S_OK;
}

// OnSpeak
HRESULT
VR_VoiceBoxAgentGlobal::OnSpeak(
    IVECITransaction* pTrans,
    IVECIStringSet* pSpeakList
)
{
    if ((NULL == pTrans) || (NULL == pSpeakList)) {
        return E_FAIL;
    }

    // Retrieve the text that need to be played
    VBT_ULONG uSize = 0;
    HRESULT hr = S_OK;
    pSpeakList->GetSize(&uSize);
    std::string strText;
    for (VBT_ULONG i = 0; i < uSize; ++i) {
        VBT_STR strTmp;
        pSpeakList->GetItem(i, &strTmp);
        strText = strText + " " + strTmp;
    }

    std::string strLang;
    VR_ConfigureIF * pcConfig = VR_ConfigureIF::Instance();
    if (NULL == pcConfig) {
        strLang = "en-us";
    }
    else {
        strLang = pcConfig->getVRLanguage();
    }

    // Build the play guidance request XML message
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("prompt", "playTts");
    xmlBulder.buildGivenElement(node, "language", strLang, "", "");
    xmlBulder.buildGivenElement(node, "text", strText, "", "");
    xmlBulder.buildGivenElement(node, "phoneme", "", "", "");
    std::string strPlayTts = xmlBulder.getXmlString();

    // Notify the XML message to the external service
    m_engineCallback.OnSpeak(pTrans, strPlayTts);
    return S_OK;
}

// OnCommandComplete
HRESULT
VR_VoiceBoxAgentGlobal::OnCommandComplete(
    IVECITransaction* pTrans
)
{
    if (NULL == pTrans) {
        VR_ERROR("OnCommandComplete: NULL Parameter");
        return E_POINTER;
    }

    VBT_ULONG ulErrorCode;
    HRESULT result = pTrans->GetResultErrorCode(&ulErrorCode);
    if (FAILED(result)) {
        return E_FAIL;
    }

    CVECIOutStr strTrans;
    result = pTrans->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return E_FAIL;
    }

    std::string strTanstmp;
    if (NULL != strTrans.Get()) {
        strTanstmp = strTrans.Get();
        VR_LOG("strTanstmp = %s, ulErrorCode = %s", strTanstmp.c_str(), vbtErrorToString(ulErrorCode));
        if (std::string::npos != strTanstmp.find("AddGrammar")
            || std::string::npos != strTanstmp.find("ReloadGrammar")
            || std::string::npos != strTanstmp.find("RemoveGrammar")
            || std::string::npos != strTanstmp.find("RemoveAllGrammars")) {
            std::pair<std::string, std::string> pairAgent2TransId = m_engineCallback.GetCurrentTransaction();
            if (S_PAUSED != ulErrorCode) {
                bool bGrammarErr = false;
                if (m_bGrammarErr) {
                    bool bNeedAgain = m_engineCallback.ResetGrammarUpdate();
                    if (!bNeedAgain) {
                        m_iGrammarErrCount = 0;
                    }
                    m_bGrammarErr = false;
                    bGrammarErr = true;
                }
                else {
                    m_iGrammarErrCount = 0;
                }
                m_engineCallback.SetCurrentTransaction("", "");
                VR_LOG("first = %s, second = %s", pairAgent2TransId.first.c_str(), pairAgent2TransId.second.c_str());
                m_engineCallback.SetUpdateGammarFlg(true);
                if ("music" == pairAgent2TransId.first) {
                    if (S_OK == ulErrorCode && !bGrammarErr) {
                        m_engineCallback.ConfirmMusicUpdateGrammar(strTanstmp, true);
                    }
                    else {
                        m_engineCallback.ConfirmMusicUpdateGrammar(strTanstmp, false);
                    }
                }
                else if ("hfd" == pairAgent2TransId.first) {
                    VoiceMap<std::string, std::string>::type mapTransition = VR_VoiceBoxCatalogPhone::GetTransactionIdMap();
                    VoiceMap<std::string, std::string>::iterator iter = mapTransition.find(strTanstmp);
                    if (mapTransition.end() != iter) {
                        std::string strGrammar = iter->second;
                        if (S_OK == ulErrorCode && !bGrammarErr) {
                            m_engineCallback.SendGrammarResult(strGrammar, "phone", "1", "0");
                        }
                        else {
                            m_engineCallback.SendGrammarResult(strGrammar, "phone", "1", "2");
                        }
                        mapTransition.erase(iter);
                        VR_VoiceBoxCatalogPhone::SetTransactionIdMap(mapTransition);
                    }
                }
                else {

                }
            }
            else {
                VR_LOG("wait for resuming");
                m_engineCallback.SetWaitResumeGrammar(true);
            }
        }
        else if (std::string::npos != strTanstmp.find("ResumeGrammar")) {
            VR_LOG("has been resumed");
            m_engineCallback.SetWaitResumeGrammar(false);
        }
        else if (std::string::npos != strTanstmp.find("SetDataActiveSource")) {
            std::string strOp;
            std::string strActiveSource;
            m_engineCallback.GetOpActiveSouce(strTanstmp, strOp, strActiveSource);

            std::string strErrCode = (S_OK == ulErrorCode) ? "0" : "1";
            if (!strOp.empty() && !strActiveSource.empty()) {
                m_engineCallback.SendGrammarResult(strOp, "media", strActiveSource, strErrCode);
            }
        }
        else {

        }
    }

    VR_LOG("VR_VoiceBoxAgentGlobal OnCommandComplete: strTrans = %s, ulErrorCode = %s", strTrans.Get(), vbtErrorToString(ulErrorCode));

    StructNode eventName;
    eventName.Name = "event";
    eventName.Value = "commandcomplete-internal";

    VoiceVector<StructNode>::type parameterSet;
    parameterSet.push_back(eventName);

    const int ERROR_CODE_LEN = 16;
    char errorCode[ERROR_CODE_LEN] = { 0 };
    snprintf(errorCode, ERROR_CODE_LEN, "0x%lx", ulErrorCode);
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildGivenElement("send", parameterSet);
    xmlBulder.buildGivenElement(node, "errorstatus", errorCode, "", "");
    if (NULL != strTrans.Get()) {
        xmlBulder.buildGivenElement(node, "transactionid", strTrans.Get(), "", "");
    }

    m_engineCallback.OnRequest(xmlBulder.getXmlString());

    if (m_bTslState) {
        std::string headerFormat = "<action name=\"notifyAppXmlMessage\">"
              "<Message source=\"VBT\" class=\"System\">"
                "<Action type=\"VBT Finished\">"
                  "<Parameter name=\"Code\" value=\"0x%08lx\"/>"
                "</Action>"
              "</Message>"
            "</action>";
        const int nFormatLen = 256;
        char header[nFormatLen] = { 0 };
        snprintf(header, sizeof(header), headerFormat.c_str(), ulErrorCode);
        std::string notification = std::string(header);
        m_engineCallback.OnRecognized(notification);
        return S_OK;
    }

    return S_OK;
}

bool
VR_VoiceBoxAgentGlobal::OnSendXMLMessage(
    IVECITransaction* pTrans
)
{
    if (NULL == pTrans) {
        VR_ERROR("OnSendXMLMessage: NULL Parameter");
        return false;
    }

    VARIANT value;
    value.vt = VT_BOOL;
    value.boolVal = VARIANT_TRUE;

    // Set property of PropAudioLevelThread
    HRESULT result = m_client.SetProperty(PropClientManagedRender, &value);
    if (FAILED(result)) {
        // VR_ERROR("SetProperty Failed, result: %lx", result);
        return false;
    }

    value.vt = VT_BOOL;
    value.boolVal = VARIANT_FALSE;
    result = m_client.SetProperty(PropBackChangeContext, &value);
    if (FAILED(result)) {
        // VR_ERROR("SetProperty Failed, result: %lx", result);
        return false;
    }

    CVECIPtr<IVECITransaction> transaction;
    VBT_BOOL bContinuous = VBT_TRUE;
    result = m_engineCommand.StartRecoSession(&transaction, bContinuous);
    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    m_mapMsgResultHandler.insert(
            std::make_pair(
                strTrans.Get(),
                &VR_VoiceBoxAgentGlobal::OnStartRecoSession
                )
            );

    return true;
}

bool
VR_VoiceBoxAgentGlobal::OnStartRecoSession(
    IVECITransaction* pTrans
)
{
    if (NULL == pTrans) {
        VR_ERROR("OnStartRecoSession: NULL Parameter");
        return false;
    }

    VARIANT value;
    value.vt = VT_BOOL;
    value.boolVal = VARIANT_FALSE;

    // Set property of PropAudioLevelThread
    HRESULT result = m_client.SetProperty(PropClientManagedRender, &value);
    if (FAILED(result)) {
        // VR_ERROR("SetProperty Failed, result: %lx", result);
        return false;
    }

    value.vt = VT_BOOL;
    value.boolVal = VARIANT_TRUE;
    result = m_client.SetProperty(PropBackChangeContext, &value);
    if (FAILED(result)) {
        // VR_ERROR("SetProperty Failed, result: %lx", result);
        return false;
    }

    CVECIPtr<IVECIParsedMessage> pcMsg;
    result = pTrans->GetResultMessage(&pcMsg);

    if (FAILED(result)) {
        // VR_ERROR("SetProperty Failed, result: %lx", result);
        return false;
    }

    CVECIOutStr xmlMsg;
    result = pcMsg->GetString(&xmlMsg);

    if (FAILED(result) || (NULL == xmlMsg.Get())) {
        // VR_ERROR("SetProperty Failed, result: %lx", result);
        return false;
    }

    m_engineCallback.OnRecognized(xmlMsg.Get());

    return true;
}

bool
VR_VoiceBoxAgentGlobal::OnCancelSpecific(
    IVECITransaction* pTrans
)
{
    if (NULL == pTrans) {
        VR_ERROR("OnCancelSpecific: NULL Parameter");
        return false;
    }

    CVECIPtr<IVECIParsedMessage> pcMsg;
    HRESULT result = pTrans->GetResultMessage(&pcMsg);

    if (FAILED(result)) {
        // VR_ERROR("SetProperty Failed, result: %lx", result);
        return false;
    }

    CVECIOutStr xmlMsg;
    result = pcMsg->GetString(&xmlMsg);

    if (FAILED(result) || (NULL == xmlMsg.Get())) {
        // VR_ERROR("SetProperty Failed, result: %lx", result);
        return false;
    }

    VR_LOG("cancel msg : %s", xmlMsg.Get());
    m_engineCallback.OnRecognized(xmlMsg.Get());

    return true;
}

// OnBeepSync
HRESULT
VR_VoiceBoxAgentGlobal::OnBeepSync()
{
    // Notify the XML message to the external service
    m_engineCallback.OnBeep(VR_BeepType_Listening);
    return S_OK;
}

// OnAudioLevelUpdate
HRESULT
VR_VoiceBoxAgentGlobal::OnAudioLevelUpdate(VBT_LONG lValue)
{
    if (m_lLevelValue != lValue) {
        VR_VoiceBoxXmlBuilder xmlBulder;
        pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "Volume");

        (void)xmlBulder.buildGivenElement(xmlNode, "type", "asr", "", "");

        std::ostringstream oss;
        oss<<lValue;
        std::string strValue = oss.str();

        (void)xmlBulder.buildGivenElement(xmlNode, "volume", strValue.c_str(), "", "");

        std::string strReply = xmlBulder.getXmlString();

        m_engineCallback.OnRecognized(strReply);
    }

    return S_OK;
}

bool
VR_VoiceBoxAgentGlobal::ProcessAgentMessage(IVECIParsedMessage *pcMsg)
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
    VoiceMap<std::string, GlobalMsgHandler>::const_iterator iterMap = m_globalMsgHandler.find(strAgentResult);
    if (m_globalMsgHandler.cend() != iterMap) {
        if (NULL != iterMap->second) {
            (this->*(iterMap->second))(pcMsg);
            return true;
        }
    }

    return false;
}

bool
VR_VoiceBoxAgentGlobal::SystemBack(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    VR_LOG("recognizing back");
    VR_LOGP("DE: start running operation... back .... case : 212-121-99 212-127-99");

    m_engineCallback.OnRequest("<send event=\"back-internal\" target=\"de\"/>");
    return true;
}

bool
VR_VoiceBoxAgentGlobal::SystemBeginOfList(
    IVECIParsedMessage *pcMsg
)
{
    return false;
}

bool
VR_VoiceBoxAgentGlobal::SystemCancel(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    VR_LOG("canceled");
    VR_LOGP("DE: start running operation... cancel .... case : 212-121-99 212-127-99");

    m_engineCallback.OnRequest("<send event=\"cancel\" target=\"dm\"/>");
    return true;
}

bool
VR_VoiceBoxAgentGlobal::HintFirstPage(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    VR_LOG("first page");
    m_engineCallback.OnRequest("<send event=\"hintPage\" pageType=\"firstPage\" />");
    return true;
}

bool
VR_VoiceBoxAgentGlobal::HintLastPage(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    VR_LOG("last page");
    m_engineCallback.OnRequest("<send event=\"hintPage\" pageType=\"lastPage\" />");
    return true;
}

bool
VR_VoiceBoxAgentGlobal::HintPrevPage(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    VR_LOG("previous page");
    m_engineCallback.OnRequest("<send event=\"hintPage\" pageType=\"prevPage\" />");
    return true;
}

bool
VR_VoiceBoxAgentGlobal::HintNextPage(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    VR_LOG("next page");
    m_engineCallback.OnRequest("<send event=\"hintPage\" pageType=\"nextPage\" />");
    return true;
}

bool
VR_VoiceBoxAgentGlobal::SystemCSVRComplete(
    IVECIParsedMessage *pcMsg
)
{

    return false;
}

bool
VR_VoiceBoxAgentGlobal::SystemEndOfList(
    IVECIParsedMessage *pcMsg
)
{
    return false;
}

bool
VR_VoiceBoxAgentGlobal::SystemErrorLimitReached(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    // m_engineCallback.OnRequest("<send event=\"cancel\" target=\"dm\"/>");
    return false;
}

bool
VR_VoiceBoxAgentGlobal::SystemFirstPage(
    IVECIParsedMessage *pcMsg
)
{
    return false;
}

bool
VR_VoiceBoxAgentGlobal::SystemGenericConfirm(
    IVECIParsedMessage *pcMsg
)
{
    return false;
}

bool
VR_VoiceBoxAgentGlobal::SystemGenericReject(
    IVECIParsedMessage *pcMsg
)
{
    return false;
}

bool
VR_VoiceBoxAgentGlobal::SystemGreeting(
    IVECIParsedMessage *pcMsg
)
{
    return false;
}

bool
VR_VoiceBoxAgentGlobal::SystemHelp(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.OnRequest("<send event=\"help-internal\" target=\"de\"/>");
    return true;
}

bool
VR_VoiceBoxAgentGlobal::SystemLastPage(
    IVECIParsedMessage *pcMsg
)
{
    return false;
}

bool
VR_VoiceBoxAgentGlobal::SystemNextPage(
    IVECIParsedMessage *pcMsg
)
{
    return false;
}

bool
VR_VoiceBoxAgentGlobal::SystemNoReco(
    IVECIParsedMessage *pcMsg
)
{
    return false;
}

bool
VR_VoiceBoxAgentGlobal::SystemConnectError(
    IVECIParsedMessage *pcMsg
)
{

    return false;
}

bool
VR_VoiceBoxAgentGlobal::SystemPreviousPage(
    IVECIParsedMessage *pcMsg
)
{

    return false;
}

bool
VR_VoiceBoxAgentGlobal::SystemStartOver(
    IVECIParsedMessage *pcMsg
)
{
    m_engineCallback.OnRequest("<send event=\"startover-internal\" target=\"de\"/>");
    return false;
}

bool
VR_VoiceBoxAgentGlobal::SystemPause(
    IVECIParsedMessage *pcMsg
)
{
    VR_LOGP("DE: start running operation... pause .... case : 212-121-99 212-127-99");

    m_engineCallback.OnRequest("<send event=\"pause-internal\" target=\"de\"/>");
    return false;
}

bool
VR_VoiceBoxAgentGlobal::SystemRepeat(
    IVECIParsedMessage *pcMsg
)
{
    VR_LOGP("DE: start running operation... repeat .... case : 212-121-99 212-127-99");

    m_engineCallback.OnRequest("<send event=\"repeat-internal\" target=\"de\"/>");
    return false;
}

bool
VR_VoiceBoxAgentGlobal::SystemVBTError(
    IVECIParsedMessage *pcMsg
)
{
    return false;
}

bool
VR_VoiceBoxAgentGlobal::SystemSendCommand(
    IVECIParsedMessage *pcMsg
)
{
    return false;
}

bool
VR_VoiceBoxAgentGlobal::SystemVersionInfo(
    IVECIParsedMessage *pcMsg
)
{
    return false;
}

bool
VR_VoiceBoxAgentGlobal::SystemWhatCanISay(
    IVECIParsedMessage *pcMsg
)
{
    return false;
}

bool
VR_VoiceBoxAgentGlobal::SpeakerAdaptationCommand(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("SpeakerAdaptationCommand: %s", xmlMessage.Get());

    std::string strAgentResult;
    CVECIOutStr strActionType;
    pcMsg->GetActionType(&strActionType);
    if (NULL != strActionType.Get()) {
        strAgentResult = strActionType.Get();
    }

    if (VBT_ACTION_TYPE_SPEAKER_ADAPTATION_COMMAND_01_COMPLETED == strAgentResult) {
        m_step = 2;
    }
    else if (VBT_ACTION_TYPE_SPEAKER_ADAPTATION_COMMAND_02_COMPLETED == strAgentResult) {
        m_step = 3;
    }
    else if (VBT_ACTION_TYPE_SPEAKER_ADAPTATION_COMMAND_03_COMPLETED == strAgentResult) {
        m_step = 4;
    }
    else if (VBT_ACTION_TYPE_SPEAKER_ADAPTATION_COMMAND_04_COMPLETED == strAgentResult) {
        m_step = 5;
    }
    else if (VBT_ACTION_TYPE_SPEAKER_ADAPTATION_COMMAND_05_COMPLETED == strAgentResult) {
        m_step = 6;
    }
    else if (VBT_ACTION_TYPE_SPEAKER_ADAPTATION_COMMAND_06_COMPLETED == strAgentResult) {
        m_step = 7;
    }
    else if (VBT_ACTION_TYPE_SPEAKER_ADAPTATION_COMMAND_07_COMPLETED == strAgentResult) {
        m_step = 8;
    }
    else if (VBT_ACTION_TYPE_SPEAKER_ADAPTATION_COMMAND_08_COMPLETED == strAgentResult) {
        m_step = 9;
    }
    else if (VBT_ACTION_TYPE_SPEAKER_ADAPTATION_COMMAND_09_COMPLETED == strAgentResult) {
        m_step = 10;
    }
    else if (VBT_ACTION_TYPE_SPEAKER_ADAPTATION_COMMAND_10_COMPLETED == strAgentResult) {
        VR_LOG("task complete");
        return true;
    }
    else {
        VR_LOG("error");
        return false;
    }

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(xmlNode, "agent", "adaptation", "", "");
    xmlBulder.buildGivenElement(xmlNode, "content", "na_adaptation", "", "");
    pugi::xml_node detailNode = xmlBulder.buildGivenElement(xmlNode, "detail", "", "", "");

    CVECIOutStr strPhrase;
    pcMsg->GetActionParameterValue("Text", &strPhrase);

    if (NULL != strPhrase.Get()) {
        xmlBulder.buildGivenElement(detailNode, "phrase", strPhrase.Get(), "", "");
    }
    std::stringstream ss;
    std::string strStep;
    ss<<m_step;
    ss>>strStep;
    xmlBulder.buildGivenElement(detailNode, "step", strStep.c_str(), "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.GetPromptByScreenId("VR-ST-01", m_strPrompt);

    m_engineCallback.OnRecognized(strReply);
    VR_LOG("SpeakerAdaptationCommand: %s", strReply.c_str());
    return true;
}

bool
VR_VoiceBoxAgentGlobal::StartSpeakerAdaptation(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(xmlNode, "agent", "adaptation", "", "");
    xmlBulder.buildGivenElement(xmlNode, "content", "na_adaptation", "", "");
    pugi::xml_node detailNode = xmlBulder.buildGivenElement(xmlNode, "detail", "", "", "");

    CVECIOutStr strPhrase;
    pcMsg->GetActionParameterValue("Text", &strPhrase);

    if (NULL != strPhrase.Get()) {
        xmlBulder.buildGivenElement(detailNode, "phrase", strPhrase.Get(), "", "");
    }
    m_step = 1;
    std::stringstream ss;
    std::string strStep;
    ss<<m_step;
    ss>>strStep;
    xmlBulder.buildGivenElement(detailNode, "step", strStep.c_str(), "", "");
    std::string strReply = xmlBulder.getXmlString();
    VR_LOG("StartSpeakerAdaptation: %s", strReply.c_str());
    m_engineCallback.OnRecognized(strReply);

    m_engineCallback.GetPromptByScreenId("VR-ST-01", m_strPrompt);

    return true;
}

bool
VR_VoiceBoxAgentGlobal::AddressEntryHelp(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    VR_LOG("AddressEntryHelp");
    m_engineCallback.OnRequest("<send event=\"agentHelp\" agent=\"navi\"/>");
    return true;
}

bool
VR_VoiceBoxAgentGlobal::AddressEntryHelpNotEnabled(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("AddressEntryHelpNotEnabled-Stay in this screen: %s", xmlMessage.Get());

    return true;
}

bool
VR_VoiceBoxAgentGlobal::AppsHelp(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    VR_LOG("AppsHelp");
    m_engineCallback.OnRequest("<send event=\"agentHelp\" agent=\"apps\"/>");
    return true;
}

bool
VR_VoiceBoxAgentGlobal::AppsHelpNotEnabled(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("AppsHelpNotEnabled-Stay in this screen: %s", xmlMessage.Get());

    return true;
}

bool
VR_VoiceBoxAgentGlobal::ConfirmTutorialConfirm(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("help", "showTutorialsUI");
    std::string strReply = xmlBulder.getXmlString();
    VR_LOG("ConfirmTutorialConfirm: %s", strReply.c_str());
    m_engineCallback.OnRecognized(strReply);
    return true;

}

bool
VR_VoiceBoxAgentGlobal::ConfirmTutorialReject(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    m_engineCallback.OnRequest("<event name=\"saveTutoAgent\"><agent>topmenu</agent></event>");

    return true;
}

bool
VR_VoiceBoxAgentGlobal::DestinationHelp(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    VR_LOG("DestinationHelp");

    m_engineCallback.OnRequest("<send event=\"agentHelp\" agent=\"navi\"/>");
    return true;
}

bool
VR_VoiceBoxAgentGlobal::DestinationHelpNotEnabled(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("DestinationHelpNotEnabled-Stay in this screen: %s", xmlMessage.Get());

    return true;
}

bool
VR_VoiceBoxAgentGlobal::SystemExit(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    std::string strResult = "";
    CVECIOutStr strActionType;
    pcMsg->GetActionType(&strActionType);
    if (NULL != strActionType.Get()) {
        strResult = strActionType.Get();
    }
    VR_LOG("SystemExit: %s", strResult.c_str());

    return true;
}

bool
VR_VoiceBoxAgentGlobal::HVACHelp(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    VR_LOG("HVACHelp");

    m_engineCallback.OnRequest("<send event=\"agentHelp\" agent=\"climate\"/>");
    return true;
}

bool
VR_VoiceBoxAgentGlobal::InformationHelp(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    VR_LOG("InformationHelp");

    m_engineCallback.OnRequest("<send event=\"agentHelp\" agent=\"information\"/>");
    return true;
}

bool
VR_VoiceBoxAgentGlobal::InformationHelpNotEnabled(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("InformationHelpNotEnabled-Stay in this screen: %s", xmlMessage.Get());

    return true;
}

bool
VR_VoiceBoxAgentGlobal::GenericKeyboardDictation(
    IVECIParsedMessage *pcMsg
)
{

    return false;
}

bool
VR_VoiceBoxAgentGlobal::LexusInsiderCatch(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("help", "showLexusInsiderUI");
    std::string strReply = xmlBulder.getXmlString();
    VR_LOG("LexusInsiderCatch: %s", strReply.c_str());
    m_engineCallback.OnRecognized(strReply);
    return true;
}

bool
VR_VoiceBoxAgentGlobal::MessagingHelp(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    VR_LOG("MessagingHelp");

    m_engineCallback.OnRequest("<send event=\"agentHelp\" agent=\"phone\"/>");
    return true;
}

bool
VR_VoiceBoxAgentGlobal::SystemMoreHints(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    VR_LOGP("DE: start morehints command...case : 215-9-00");

    m_engineCallback.OnRequest("<event name=\"morehint-internal\"></event>");

    return true;
}

bool
VR_VoiceBoxAgentGlobal::MusicHelp(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    VR_LOG("MusicHelp");

    m_engineCallback.OnRequest("<send event=\"agentHelp\" agent=\"music\"/>");
    return true;
}

bool
VR_VoiceBoxAgentGlobal::PhoneHelp(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    VR_LOG("PhoneHelp");
    m_engineCallback.OnRequest("<send event=\"agentHelp\" agent=\"phone\"/>");
    return true;
}

bool
VR_VoiceBoxAgentGlobal::POIHelp(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    VR_LOG("POIHelp");

    m_engineCallback.OnRequest("<send event=\"agentHelp\" agent=\"navi\"/>");
    return true;
}

bool
VR_VoiceBoxAgentGlobal::POIHelpNotEnabled(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("POIHelpNotEnabled-Stay in this screen: %s", xmlMessage.Get());

    return true;
}

bool
VR_VoiceBoxAgentGlobal::RadioHelp(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    VR_LOG("RadioHelp");

    m_engineCallback.OnRequest("<send event=\"agentHelp\" agent=\"radio\"/>");
    return true;
}

bool
VR_VoiceBoxAgentGlobal::SystemSettings(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("help", "showVoiceSettingsUI");
    std::string strReply = xmlBulder.getXmlString();
    VR_LOG("SystemSettings: %s", strReply.c_str());
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentGlobal::SpeakerAdaptionCatch(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(xmlNode, "agent", "help", "", "");
    xmlBulder.buildGivenElement(xmlNode, "content", "na_help_train_voice_recognition", "", "");
    std::string strReply = xmlBulder.getXmlString();
    VR_LOG("SpeakerAdaptionCatch: %s", strReply.c_str());
    m_engineCallback.OnRecognized(strReply);

    m_engineCallback.GetPromptByScreenId("VR-MAN-01", m_strPrompt);

    return true;
}

bool
VR_VoiceBoxAgentGlobal::GlobalTaskComplete(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    std::string strResult = "";
    CVECIOutStr strActionType;
    pcMsg->GetActionType(&strActionType);
    if (NULL != strActionType.Get()) {
        strResult = strActionType.Get();
    }
    VR_LOG("GlobalTaskComplete: %s", strResult.c_str());

    return true;
}

bool
VR_VoiceBoxAgentGlobal::TutorialCatch(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(xmlNode, "agent", "help", "", "");
    xmlBulder.buildGivenElement(xmlNode, "content", "na_help_tutorial_confirmation", "", "");

    m_engineCallback.GetPromptByScreenId("VR-HNT-05", m_strPrompt);

    std::string strReply = xmlBulder.getXmlString();
    VR_LOG("TutorialCatch: %s", strReply.c_str());
    m_engineCallback.OnRecognized(strReply);

    return true;

}

bool
VR_VoiceBoxAgentGlobal::GettingStartedWithVoice(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.GetPromptByScreenId("VR-PVR-01", m_strPrompt);

    return true;
}

bool
VR_VoiceBoxAgentGlobal::HintFristPageRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    std::string strAction = "<action agent=\"help\" op=\"queryHintFirstPage\" />";
    m_engineCallback.OnRequest(strAction);
    return true;
}

bool
VR_VoiceBoxAgentGlobal::HintLastPageRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    std::string strAction = "<action agent=\"help\" op=\"queryHintLastPage\" />";
    m_engineCallback.OnRequest(strAction);
    return true;
}

bool
VR_VoiceBoxAgentGlobal::VehicleInMotionRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    VR_VoiceBoxXmlBuilder xmlBulder;

    pugi::xml_node node = xmlBulder.buildStartActionElement("help", "queryVehicleinMotion");
    std::string strReply = xmlBulder.getXmlString();
    VR_LOG("VehicleInMotionRequest: %s", strReply.c_str());
    m_engineCallback.OnInfoQueryMessage(strReply);

    return true;
}

bool
VR_VoiceBoxAgentGlobal::EscalatingError(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.OnRequest("<event name=\"escalating-error\"></event>");
    return true;
}

bool
VR_VoiceBoxAgentGlobal::FirstPageReply(
    VR_VoiceBoxXmlParser& parser
)
{
    std::string strValue = parser.getValueByKey("value");
    if ("false" == strValue) {
        strValue = "False";
    }
    else {
        strValue = "True";
    }

    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    if (NULL == spParams.ptr()) {
        return false;
    }

    spParams->AddParameter("OnFirstPage", strValue.c_str(), "", "");

    HRESULT retCode = m_engineCommand.DriverActionReply(
                      VBT_AGENT_HELP,
                      "Hints Navigation - First Page Reply",
                      spParams,
                      NULL,
                      NULL);
    if (S_OK != retCode) {
        return false;
    }
    return true;
}

bool
VR_VoiceBoxAgentGlobal::LastPageReply(
    VR_VoiceBoxXmlParser& parser
)
{
    std::string strValue = parser.getValueByKey("value");
    if ("false" == strValue) {
        strValue = "False";
    }
    else {
        strValue = "True";
    }

    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    if (NULL == spParams.ptr()) {
        return false;
    }

    spParams->AddParameter("OnLastPage", strValue.c_str(), "", "");

    HRESULT retCode = m_engineCommand.DriverActionReply(
                      VBT_AGENT_HELP,
                      "Hints Navigation - Last Page Reply",
                      spParams,
                      NULL,
                      NULL);
    if (S_OK != retCode) {
        return false;
    }
    return true;
}

bool
VR_VoiceBoxAgentGlobal::VehicleInMotionReply(
    VR_VoiceBoxXmlParser& parser
)
{
    VR_LOG("VehicleInMotionReply ");
    std::string strMotion = parser.getValueByKey("motion");
    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    if (NULL == spParams.ptr()) {
        return false;
    }

    if ("false" == strMotion) {
        spParams->AddParameter("inMotion", "False", "", "");

        HRESULT retCode  = m_engineCommand.DriverActionReply("Help", "Vehicle in Motion Reply", spParams, NULL, NULL);
    }
    else if ("true" == strMotion) {
        spParams->AddParameter("inMotion", "True", "", "");

        HRESULT retCode  = m_engineCommand.DriverActionReply("Help", "Vehicle in Motion Reply", spParams, NULL, NULL);
    }
    else {
    }

    return true;
}

bool
VR_VoiceBoxAgentGlobal::NullOperation(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr strMsg;
    pcMsg->GetString(&strMsg);

    VR_LOG("Null Operation for the following message:");
    VR_LOG("%s", strMsg.Get());
    return false;
}

bool
VR_VoiceBoxAgentGlobal::GlobalBackAlreadyAtRoot(
    IVECIParsedMessage *pcMsg
)
{
    m_engineCallback.OnRequest("<event name=\"cancel\"> </event>");
    return true;
}

/* EOF */
