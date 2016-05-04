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
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <algorithm>
#include "VR_Log.h"
#include "VR_PerformanceLog.h"


/* VBT Header */
#ifndef IVBTENGINECLIENT_H
#    include "IVBTEngineClient.h"
#endif

#ifndef VECIOUTSTR_H
#    include "VECIOutStr.h"
#endif

#ifndef VECICSTR_H
#    include "VECICStr.h"
#endif

#ifndef VBT_SDK_PREFERENCES_H
#    include "VBT_SDK_Preferences.h"
#endif

#ifndef VBT_SDK_RECOGNITIONSTATE_H
#    include "VBT_SDK_RecognitionState.h"
#endif

#ifndef VBTERRORTOSTRING_H
#    include "VBTErrorToString.h"
#endif

#ifndef VBT_SDK_COMMANDS_H
#    include "VBT_SDK_Commands.h"
#endif

#ifndef VBT_SDK_RECOGNITIONSTATE_H
#    include "VBT_SDK_RecognitionState.h"
#endif

#ifndef VBT_SDK_AGENT_H
#    include "VBT_SDK_Agent.h"
#endif

#ifndef VBT_SDK_ACTION_TYPES_H
#    include "VBT_SDK_Action_Types.h"
#endif

/* Suntec Header */
#ifndef VR_VOICEBOXENGINE_H
#   include "VR_VoiceBoxEngine.h"
#endif

#ifndef VR_VOICEBOXEVENTSINK_H
#    include "VR_VoiceBoxEventSink.h"
#endif

#ifndef VR_VOICEBOXXMLPARSER_H
#    include "VR_VoiceBoxXmlParser.h"
#endif

#ifndef VR_DIALOGENGINELISTENER_H
#    include "VR_DialogEngineListener.h"
#endif

#ifndef VR_VOICEBOXCONTROLLER_H
#    include "VR_VoiceBoxController.h"
#endif

#ifndef VR_VOICEBOXAGENTAPPS_H
#    include "VR_VoiceBoxAgentApps.h"
#endif

#ifndef VR_VOICEBOXAGENTAPPS_AU_H
#    include "VR_VoiceBoxAgentApps_AU.h"
#endif

#ifndef VR_VOICEBOXAGENTAUDIO_H
#    include "VR_VoiceBoxAgentAudio.h"
#endif

#ifndef VR_VOICEBOXAGENTAUDIO_AU_H
#    include "VR_VoiceBoxAgentAudio_AU.h"
#endif

#ifndef VR_VOICEBOXAGENTCLIMATE_H
#    include "VR_VoiceBoxAgentClimate.h"
#endif

#ifndef VR_VOICEBOXAGENTGLOBAL_H
#    include "VR_VoiceBoxAgentGlobal.h"
#endif

#ifndef VR_VOICEBOXAGENTINFO_H
#    include "VR_VoiceBoxAgentInfo.h"
#endif

#ifndef VR_VOICEBOXAGENTNAVI_H
#    include "VR_VoiceBoxAgentNavi.h"
#endif

#ifndef VR_VOICEBOXAGENTPHONE_AU_H
#    include "VR_VoiceBoxAgentPhone_AU.h"
#endif

#ifndef VR_VOICEBOXXMLBUILDER_H
#    include "VR_VoiceBoxXmlBuilder.h"
#endif

#ifndef VR_VOICEBOXLOG_H
#    include "VR_VoiceBoxLog.h"
#endif

#ifndef VR_VOICEBOXCATALOGMANAGER_H
#    include "VR_VoiceBoxCatalogManager.h"
#endif

#ifndef VR_VOICEBOXXMLNODESTRING_H
#    include "VR_VoiceBoxXmlNodeString.h"
#endif

#ifndef VR_CONFIGUREIF_H
#    include "VR_ConfigureIF.h"
#endif

#ifndef VR_AUDIOBUFFER_H
#    include "VR_AudioBuffer.h"
#endif

#ifndef VR_VOICETAGIDMANAGER_H
#include "VR_VoiceTagIDManager.h"
#endif

#ifndef VR_VOICEBOXAPPSXML_H
#    include "VR_VoiceBoxAppsXml.h"
#endif

#ifndef VR_AUDIOSTREAMIF_H
#    include "VR_AudioStreamIF.h"
#endif

#ifndef VR_VOICEBOXDATASTORAGE_H
#    include "VR_VoiceBoxDataStorage.h"
#endif

#ifndef VR_VOICEBOXPHONEDATA_H
#    include "VR_VoiceBoxPhoneData.h"
#endif

#ifndef VR_DEF_H
#    include "VR_Def.h"
#endif

#ifndef VR_VOICEBOXVOICETAG_H
#    include "VR_VoiceBoxVoiceTag.h"
#endif

using namespace nutshell;

#define VR_VOICETAG_ID_PREFIX       "VoiceTagID"
#define HIGH_VERBOSITY      "0"
#define SILENT_VERBOSITY    "2"
#define LOW_VERBOSITY       "4"
#define PROMPTLEVEL     "level"
#define VOICETAGFILE    "voiceTag.wav"

#define VR_ACTION_STOPBEEP "<action agent=\"prompt\" op=\"stopBeep\"></action>"
#define VR_ACTION_CLOSESESSION "<action name=\"closeSession\" />"

const int VR_INVALID_ACTION_ID = -1;

VR_VoiceBoxFrontEndShare VR_VoiceBoxEngine::s_frontEndShare;    ///< FrontEndShare

// Create VoiceBox Engine Instance
VR_VoiceBoxEngineIF* VR_VoiceBoxEngineIF::CreateInstance()
{
    return VR_new VR_VoiceBoxEngine;
}

// Constructor
VR_VoiceBoxEngine::VR_VoiceBoxEngine()
    : m_pcExternalCallback(NULL)
    , m_pcMsgController(NULL)
    , m_pcCatalogController(NULL)
    , m_pcCatalogPhone(NULL)
    , m_pcCatalogManager(NULL)
    , m_pcPlayTransation(NULL)
    , m_sessionState(VR_SessionStateNone)
    , m_bTaskCompelete(false)
    , m_bPlayTTS(false)
    , m_iCurReqId(0)
    , m_iPlayVoiceTagId(0)
    , m_iCurTTSReqId(0)
    , m_iCurrActionId(0)
    , m_messageAvailable(false)
    , m_bUpdatingMapData(false)
    , m_isActiveFM(false)
    , m_isActiveAM(false)
    , m_isSatellite(false)
    , m_isActiveFMHD(false)
    , m_isActiveSatellite(false)
    , m_bBosDetected(false)
    , m_iPromptLevel(VR_PROMPTLEVEL_HIGH)
    , m_bDoCanceling(false)
    , m_bDoCancelVoiceTag(false)
    , m_bEngineStarting(false)
    , m_bUsb1(false)
    , m_bUsb2(false)
    , m_bIpod1(false)
    , m_bIpod2(false)
    , m_bBtAudio(false)
    , m_isNameNull(false)
    , m_bTTSNull(false)
    , m_iStartBeepID(VR_INVALID_ACTION_ID)
    , m_iDoneBeepID(VR_INVALID_ACTION_ID)
    , m_bGrammarErrorCode(true)
    , m_bSession(false)
    , m_bEngineReady(false)
    , m_isIncomingMessage(false)
    , m_country(VR_REGION_US)
    , m_strUsbOrIpodConnected("False")
    , m_userId("")
    , m_lastPlayed(NONE)
    , m_stopVoiceTagBeepOrTTs(VR_INVALID_ACTION_ID)
    , m_stopTraining(VR_INVALID_ACTION_ID)
    , m_bSongInfoAvailable(false)
    , m_iInstallVecIndex(0)
    , m_bInstallingAgent(false)
    , m_PVRStateCurrent(VR_PVRState_None)
    , m_bWaitConfirmation(false)
    , m_bSpeakOverBeep(false)
    , m_bPhoneGrammarAvailable(false)
    , m_bTslNetworkAvailable(false)
    , m_bAppRecoState(VR_AppRecoState_Invalid)
    , m_bWaitGreetingEnd(false)
    , m_bQuitWaitForPrompt(false)
    , m_bTslDialog(false)
    , m_bTslAppsAvailable(false)
    , m_bPVRScreen(false)
    , m_bSettingToStartScreen(false)
    , m_bNBestFlg(false)
    , m_bHVACBasicActive(false)
    , m_bHVACFrontScrnActive(false)
    , m_bHVACRearScrnActive(false)
    , m_bHVACSteeringScrnActive(false)
    , m_bHVACConcModeActive(false)
    , m_bHVACSeatActive(false)
    , m_bWaitForDoneBeep(false)
    , m_strCurResourceState("1")
    , m_bDoCancelTslVR(false)
    , m_bBackInterupted(false)
    , m_bStartSessionWithBargeIn(false)
    , m_pcCatalogAudio(NULL)
    , m_strMsgAvailable(false)
    , m_bHavePVR(false)
    , m_bWaitResumeGrammar(false)
    , m_bRouteStatus(false)
    , m_bCanceled(false)
    , m_bDoCancelSession(false)
    , m_bChangeLanguage(false)
{
    int nAgentNum = AgentType_Num;
    for (int i = 0; i < nAgentNum; ++i) {
        m_agents[i] = NULL;
    }

    m_iIndex = 0;
    m_iHintSize = 0;

}

// Destructor
VR_VoiceBoxEngine::~VR_VoiceBoxEngine()
{
    m_pcExternalCallback = NULL;
    m_pcMsgController = NULL;
    m_pcCatalogController = NULL;
    delete m_pcCatalogManager;
    m_pcCatalogManager = NULL;
    m_pcCatalogPhone = NULL;
    m_pcPlayTransation = NULL;
}

// Create the VoiceBox engine related instances and initialize them
bool
VR_VoiceBoxEngine::Initialize(
    VR_DialogEngineListener*     pcExternalCallback,
    VR_VoiceBoxController*  pcMsgController,
    VR_VoiceBoxController*  pcCatalogController
    )
{
    VR_LOGD_FUNC();
    VR_ConfigureIF * pcConfig = VR_ConfigureIF::Instance();
    if (NULL != pcConfig) {
        m_country = pcConfig->getVRContry();
    }

    m_bEngineStarting = false;

    // Used for notifing the voice recognition result and request to the user
    m_pcExternalCallback = pcExternalCallback;
    // Used for processing the voice recognition related XML messages
    m_pcMsgController = pcMsgController;
    m_pcCatalogController = pcCatalogController;

    VR_AudioStreamIF* pcAudioStream = VR_AudioStreamIF::Instance();
    if (NULL != pcAudioStream) {
        pcAudioStream->Initialize(this);
    }

    m_lstTransaction.clear();
    m_lstUCAppQuitVROp.clear();
    m_lstNeedReactiveAction.clear();
    m_lstCancelOption.clear();

    // Create the aciton-function map
    m_mapMsgHandler.insert(std::make_pair("startAgent", &VR_VoiceBoxEngine::SendRecognitionState));
    m_mapMsgHandler.insert(std::make_pair("startCurrentAgent", &VR_VoiceBoxEngine::SendRecognitionState));
    m_mapMsgHandler.insert(std::make_pair("startNextAgent", &VR_VoiceBoxEngine::SaveNextRecognitionState));
    m_mapMsgHandler.insert(std::make_pair("showHintScreen", &VR_VoiceBoxEngine::ShowHintScreen));
    m_mapMsgHandler.insert(std::make_pair("saveTutoAgent", &VR_VoiceBoxEngine::SaveTuToRecognitionState));
    m_mapMsgHandler.insert(std::make_pair("startDictation", &VR_VoiceBoxEngine::StartDictation));
    m_mapMsgHandler.insert(std::make_pair("buttonPressed", &VR_VoiceBoxEngine::ButtonPressed));
    m_mapMsgHandler.insert(std::make_pair("updateState", &VR_VoiceBoxEngine::UpdateState));
    m_mapMsgHandler.insert(std::make_pair("changeSettings", &VR_VoiceBoxEngine::ChangeSettings));
    m_mapMsgHandler.insert(std::make_pair("changeLanguage", &VR_VoiceBoxEngine::ChangeLanguage));
    m_mapMsgHandler.insert(std::make_pair("fullupdateNotify", &VR_VoiceBoxEngine::FullUpdateNotify));
    m_mapMsgHandler.insert(std::make_pair("getResourceState", &VR_VoiceBoxEngine::GetResourceState));
    m_mapMsgHandler.insert(std::make_pair("stop", &VR_VoiceBoxEngine::Cancel));
    m_mapMsgHandler.insert(std::make_pair("cancel", &VR_VoiceBoxEngine::Cancel));
    m_mapMsgHandler.insert(std::make_pair("back", &VR_VoiceBoxEngine::Back));
    m_mapMsgHandler.insert(std::make_pair("repeat-internal", &VR_VoiceBoxEngine::Repeat));
    m_mapMsgHandler.insert(std::make_pair("help", &VR_VoiceBoxEngine::Help));
    m_mapMsgHandler.insert(std::make_pair("playTts", &VR_VoiceBoxEngine::SpeakDone));
    m_mapMsgHandler.insert(std::make_pair("stopTts", &VR_VoiceBoxEngine::StopSpeakDone));
    m_mapMsgHandler.insert(std::make_pair("playBeep", &VR_VoiceBoxEngine::BeepDone));
    m_mapMsgHandler.insert(std::make_pair("start-internal", &VR_VoiceBoxEngine::StartRecoSession));
    m_mapMsgHandler.insert(std::make_pair("start-bargein", &VR_VoiceBoxEngine::StartRecoSessionWithBargeIn));
    m_mapMsgHandler.insert(std::make_pair("startover", &VR_VoiceBoxEngine::StartOver));
    m_mapMsgHandler.insert(std::make_pair("startover-internal", &VR_VoiceBoxEngine::StartOver));
    m_mapMsgHandler.insert(std::make_pair("commandcomplete-internal", &VR_VoiceBoxEngine::OnCommandComplete));
    m_mapMsgHandler.insert(std::make_pair("pause-internal", &VR_VoiceBoxEngine::Pause));
    m_mapMsgHandler.insert(std::make_pair("getHints", &VR_VoiceBoxEngine::OnGetHints));
    m_mapMsgHandler.insert(std::make_pair("waitStart", &VR_VoiceBoxEngine::SetRecoSessionFlag));
    m_mapMsgHandler.insert(std::make_pair("morehint-internal", &VR_VoiceBoxEngine::MoreHints));
    m_mapMsgHandler.insert(std::make_pair("actionMessage", &VR_VoiceBoxEngine::HandleActionMsg));
    m_mapMsgHandler.insert(std::make_pair("sendVRState", &VR_VoiceBoxEngine::HandleVRState));
    m_mapMsgHandler.insert(std::make_pair("PvrTTS", &VR_VoiceBoxEngine::PVRAction));
    m_mapMsgHandler.insert(std::make_pair("agentHelp", &VR_VoiceBoxEngine::AgentHelp));
    m_mapMsgHandler.insert(std::make_pair("startedNotify", &VR_VoiceBoxEngine::OnDMStartedNotify));
    m_mapMsgHandler.insert(std::make_pair("IncomingMessageInfo", &VR_VoiceBoxEngine::IncomingMessageInfo));
    m_mapMsgHandler.insert(std::make_pair("hintPage", &VR_VoiceBoxEngine::HintPage));
    m_mapMsgHandler.insert(std::make_pair("changeCountry", &VR_VoiceBoxEngine::UpdateMapData));
    m_mapMsgHandler.insert(std::make_pair("help-internal", &VR_VoiceBoxEngine::OnHelpRecognized));
    m_mapMsgHandler.insert(std::make_pair("escalating-error", &VR_VoiceBoxEngine::OnEscalatingError));
    m_mapMsgHandler.insert(std::make_pair("initialpersondata", &VR_VoiceBoxEngine::InitialPersonData));
    m_mapMsgHandler.insert(std::make_pair("prepare", &VR_VoiceBoxEngine::Prepare));
    m_mapMsgHandler.insert(std::make_pair("changeLanguage-internal", &VR_VoiceBoxEngine::ChangeLanguageInner));

    m_mapMsgHandler.insert(std::make_pair("back-internal", &VR_VoiceBoxEngine::Back));

    m_mapMsgBtnHandler.insert(std::make_pair("ptt_hard_key_short_press", &VR_VoiceBoxEngine::PttShort));
    m_mapMsgBtnHandler.insert(std::make_pair("ptt_hard_key_long_press", &VR_VoiceBoxEngine::PttLong));
    m_mapMsgBtnHandler.insert(std::make_pair("hard_key_enter_normal_press", &VR_VoiceBoxEngine::EntryNormalPress));
    m_mapMsgBtnHandler.insert(std::make_pair("meter_hard_key_back_normal_press", &VR_VoiceBoxEngine::BackNormalPress));
    m_mapMsgBtnHandler.insert(std::make_pair("select_one", &VR_VoiceBoxEngine::SelectOne));
    m_mapMsgBtnHandler.insert(std::make_pair("select_two", &VR_VoiceBoxEngine::SelectTwo));
    m_mapMsgBtnHandler.insert(std::make_pair("select_three", &VR_VoiceBoxEngine::SelectThree));
    m_mapMsgBtnHandler.insert(std::make_pair("select_four", &VR_VoiceBoxEngine::SelectFour));
    m_mapMsgBtnHandler.insert(std::make_pair("select_five", &VR_VoiceBoxEngine::SelectFive));
    m_mapMsgBtnHandler.insert(std::make_pair("phone", &VR_VoiceBoxEngine::Phone));
    m_mapMsgBtnHandler.insert(std::make_pair("navi", &VR_VoiceBoxEngine::Navigation));
    m_mapMsgBtnHandler.insert(std::make_pair("apps", &VR_VoiceBoxEngine::Apps));
    m_mapMsgBtnHandler.insert(std::make_pair("audio", &VR_VoiceBoxEngine::Audio));
    m_mapMsgBtnHandler.insert(std::make_pair("info", &VR_VoiceBoxEngine::Info));
    m_mapMsgBtnHandler.insert(std::make_pair("climate", &VR_VoiceBoxEngine::Climate));
    m_mapMsgBtnHandler.insert(std::make_pair("start", &VR_VoiceBoxEngine::SpeakAdaptation));
    m_mapMsgBtnHandler.insert(std::make_pair("start_over", &VR_VoiceBoxEngine::StartOver));
    m_mapMsgBtnHandler.insert(std::make_pair("pause", &VR_VoiceBoxEngine::HandlePause));
    m_mapMsgBtnHandler.insert(std::make_pair("resume", &VR_VoiceBoxEngine::Resume));
    m_mapMsgBtnHandler.insert(std::make_pair("next_page", &VR_VoiceBoxEngine::NextPage));
    m_mapMsgBtnHandler.insert(std::make_pair("previous_page", &VR_VoiceBoxEngine::PrevPage));
    m_mapMsgBtnHandler.insert(std::make_pair("last_page", &VR_VoiceBoxEngine::LastPage));
    m_mapMsgBtnHandler.insert(std::make_pair("first_page", &VR_VoiceBoxEngine::FirstPage));
    m_mapMsgBtnHandler.insert(std::make_pair("yes", &VR_VoiceBoxEngine::ConfirmYes));
    m_mapMsgBtnHandler.insert(std::make_pair("no", &VR_VoiceBoxEngine::ConfirmNo));
    m_mapMsgBtnHandler.insert(std::make_pair("go_directly", &VR_VoiceBoxEngine::GoDirectly));
    m_mapMsgBtnHandler.insert(std::make_pair("add_to_route", &VR_VoiceBoxEngine::AddToRoute));
    m_mapMsgBtnHandler.insert(std::make_pair("go_back", &VR_VoiceBoxEngine::Back));
    m_mapMsgBtnHandler.insert(std::make_pair("along_route", &VR_VoiceBoxEngine::AlongRoute));
    m_mapMsgBtnHandler.insert(std::make_pair("near_destination", &VR_VoiceBoxEngine::NearDestination));
    m_mapMsgBtnHandler.insert(std::make_pair("help", &VR_VoiceBoxEngine::Help));
    m_mapMsgBtnHandler.insert(std::make_pair("more_hints", &VR_VoiceBoxEngine::HandleMoreHints));
    m_mapMsgBtnHandler.insert(std::make_pair("call", &VR_VoiceBoxEngine::Call));
    m_mapMsgBtnHandler.insert(std::make_pair("send_message", &VR_VoiceBoxEngine::SendMessage));
    m_mapMsgBtnHandler.insert(std::make_pair("album", &VR_VoiceBoxEngine::BrowseAlbums));
    m_mapMsgBtnHandler.insert(std::make_pair("artist", &VR_VoiceBoxEngine::BrowseArtists));
    m_mapMsgBtnHandler.insert(std::make_pair("song", &VR_VoiceBoxEngine::BrowseSongs));
    m_mapMsgBtnHandler.insert(std::make_pair("composer", &VR_VoiceBoxEngine::BrowseComposers));
    m_mapMsgBtnHandler.insert(std::make_pair("genre", &VR_VoiceBoxEngine::BrowseGenres));
    m_mapMsgBtnHandler.insert(std::make_pair("podcast", &VR_VoiceBoxEngine::BrowsePodcasts));
    m_mapMsgBtnHandler.insert(std::make_pair("playlist", &VR_VoiceBoxEngine::BrowsePlaylists));
    m_mapMsgBtnHandler.insert(std::make_pair("audiobook", &VR_VoiceBoxEngine::BrowseAudiobooks));
    m_mapMsgBtnHandler.insert(std::make_pair("reply", &VR_VoiceBoxEngine::Reply));
    m_mapMsgBtnHandler.insert(std::make_pair("next", &VR_VoiceBoxEngine::ReadNext));
    m_mapMsgBtnHandler.insert(std::make_pair("previous", &VR_VoiceBoxEngine::ReadPrevious));
    m_mapMsgBtnHandler.insert(std::make_pair("tutorials", &VR_VoiceBoxEngine::Tutorials));
    m_mapMsgBtnHandler.insert(std::make_pair("voice_training", &VR_VoiceBoxEngine::VoiceTraining));
    m_mapMsgBtnHandler.insert(std::make_pair("ptt_hard_key_short_press_special", &VR_VoiceBoxEngine::InternalStartVR));
    m_mapMsgBtnHandler.insert(std::make_pair("setting_voice_training_start", &VR_VoiceBoxEngine::SettingToTraning));

    m_mapAgenttoRecoState.insert(std::make_pair("topmenu", VBT_RECO_STATE_GLOBAL));
    m_mapAgenttoRecoState.insert(std::make_pair("apps", VBT_RECO_STATE_APPS_HOME));
    m_mapAgenttoRecoState.insert(std::make_pair("phone", VBT_RECO_STATE_HFD_HOME));
    m_mapAgenttoRecoState.insert(std::make_pair("in_call", VBT_RECO_STATE_IN_CALL));
    m_mapAgenttoRecoState.insert(std::make_pair("in_message", VBT_RECO_STATE_INCOMING_MESSAGE));
    m_mapAgenttoRecoState.insert(std::make_pair("climate", VBT_RECO_STATE_HVAC_HOME));
    m_mapAgenttoRecoState.insert(std::make_pair("information", VBT_RECO_STATE_INFORMATION_HOME));
    if (VR_REGION_US == m_country) {
        m_mapAgenttoRecoState.insert(std::make_pair("music", VBT_RECO_STATE_MUSIC_HOME));
        m_mapAgenttoRecoState.insert(std::make_pair("media", VBT_RECO_STATE_MUSIC_HOME));
        m_mapAgenttoRecoState.insert(std::make_pair("radio", VBT_RECO_STATE_RADIO_HOME));
    }
    else if (VR_REGION_OC == m_country) {
        m_mapAgenttoRecoState.insert(std::make_pair("music", "Audio Home"));
        m_mapAgenttoRecoState.insert(std::make_pair("media", "Audio Home"));
        m_mapAgenttoRecoState.insert(std::make_pair("radio", "Audio Home"));
    }
    else {
    }

    m_mapAgenttoRecoState.insert(std::make_pair("navi", VBT_RECO_STATE_NAVIGATION_HOME));
    m_mapAgenttoRecoState.insert(std::make_pair("poi", VBT_RECO_STATE_POI_HOME));
    m_mapAgenttoRecoState.insert(std::make_pair("cities_key_dict", VBT_RECO_STATE_CITIES_KEYBOARD_DICTATION));
    m_mapAgenttoRecoState.insert(std::make_pair("generic_key_dict", VBT_RECO_STATE_GENERIC_KEYBOARD_DICTATION));
    m_mapAgenttoRecoState.insert(std::make_pair("hfd_key_dict", VBT_RECO_STATE_HFD_KEYBOARD_DICTATION));
    m_mapAgenttoRecoState.insert(std::make_pair("msg_dict", VBT_RECO_STATE_MESSAGE_DICTATION));
    m_mapAgenttoRecoState.insert(std::make_pair("music_key_dict", VBT_RECO_STATE_MUSIC_KEYBOARD_DICTATION));
    m_mapAgenttoRecoState.insert(std::make_pair("navi_key_dict", VBT_RECO_STATE_NAVIGATION_KEYBOARD_DICTATION));
    m_mapAgenttoRecoState.insert(std::make_pair("poi_key_dict", VBT_RECO_STATE_POI_KEYBOARD_DICTATION));
    m_mapAgenttoRecoState.insert(std::make_pair("states_key_dict", VBT_RECO_STATE_STATES_KEYBOARD_DICTATION));
    m_mapAgenttoRecoState.insert(std::make_pair("streets_key_dict", VBT_RECO_STATE_STREETS_KEYBOARD_DICTATION));
    m_mapAgenttoRecoState.insert(std::make_pair("adaptation", VBT_RECO_STATE_SPEAKER_ADAPTATION));
    m_mapAgenttoRecoState.insert(std::make_pair("voice_tag", VBT_RECO_STATE_VOICE_TAG));

    m_mapHintsScreenID.insert(std::make_pair("none", VBT_HINT_SCREEN_ID_NONE));
    m_mapHintsScreenID.insert(std::make_pair("main", VBT_HINT_SCREEN_ID_MAIN));
    m_mapHintsScreenID.insert(std::make_pair("help", VBT_HINT_SCREEN_ID_HELP));
    m_mapHintsScreenID.insert(std::make_pair("phone", VBT_HINT_SCREEN_ID_PHONE));
    m_mapHintsScreenID.insert(std::make_pair("apps", VBT_HINT_SCREEN_ID_APPS));
    m_mapHintsScreenID.insert(std::make_pair("audio", VBT_HINT_SCREEN_ID_AUDIO));
    m_mapHintsScreenID.insert(std::make_pair("info", VBT_HINT_SCREEN_ID_INFO));
    m_mapHintsScreenID.insert(std::make_pair("climate", VBT_HINT_SCREEN_ID_CLIMATE));
    m_mapHintsScreenID.insert(std::make_pair("navigation", VBT_HINT_SCREEN_ID_NAVIGATION));

    m_lstNeedReactiveAction.push_back(VBT_ACTION_TYPE_APPS_GO_TO_APPS);
    m_lstNeedReactiveAction.push_back(VBT_ACTION_TYPE_HFD_HFD_HOME);
    m_lstNeedReactiveAction.push_back(VBT_ACTION_TYPE_HVAC_GO_TO_HVAC);
    m_lstNeedReactiveAction.push_back(VBT_ACTION_TYPE_HVAC_AC_ALREADY_ON);
    m_lstNeedReactiveAction.push_back(VBT_ACTION_TYPE_HVAC_AC_ALREADY_OFF);
    m_lstNeedReactiveAction.push_back(VBT_ACTION_TYPE_HVAC_CHANGE_TEMPERATURE_ALREADY_AT_REQUESTED_TEMPERATURE);
    m_lstNeedReactiveAction.push_back(VBT_ACTION_TYPE_HVAC_ALREADY_AT_MAX_TEMPERATURE);
    m_lstNeedReactiveAction.push_back(VBT_ACTION_TYPE_HVAC_ALREADY_AT_MIN_TEMPERATURE);
    m_lstNeedReactiveAction.push_back("Decrease Temperature - Already At Min Temperature");
    m_lstNeedReactiveAction.push_back(VBT_ACTION_TYPE_MUSIC_MUSIC_MENU);
    m_lstNeedReactiveAction.push_back(VBT_ACTION_TYPE_RADIO_GO_TO_RADIO);
    m_lstNeedReactiveAction.push_back(VBT_ACTION_TYPE_RADIO_TUNE_FREQUENCY_BAND_AM);
    m_lstNeedReactiveAction.push_back(VBT_ACTION_TYPE_RADIO_TUNE_FREQUENCY_BAND_FM);
    m_lstNeedReactiveAction.push_back(VBT_ACTION_TYPE_RADIO_TUNE_FREQUENCY_CATCH);
    m_lstNeedReactiveAction.push_back(VBT_ACTION_TYPE_RADIO_TUNE_RADIO_CATCH);
    m_lstNeedReactiveAction.push_back(VBT_ACTION_TYPE_RADIO_TUNE_FREQUENCY_BAND_SATELLITE);
    m_lstNeedReactiveAction.push_back("Confirm Tutorial - Reject");
    m_lstNeedReactiveAction.push_back("Start Over");
    m_lstNeedReactiveAction.push_back("Apps Home");
    m_lstNeedReactiveAction.push_back("Audio Home");
    m_lstNeedReactiveAction.push_back("Navigation Home");

    m_lstUCAppQuitVROp.push_back("changeSourceByName");
    m_lstUCAppQuitVROp.push_back("dial");
    m_lstUCAppQuitVROp.push_back("sendMessage");
    m_lstUCAppQuitVROp.push_back("showAddVoiceTagUI");
    m_lstUCAppQuitVROp.push_back("launchApp");
    m_lstUCAppQuitVROp.push_back("showTrainingEndMsg");

    // Before start vbt engine, rename the existed vbt log files
    RenameVbtLog();

    // Creates an Engine Client object and initializes it.
    if (!CreateEngineClient()) {
        VR_ERROR("CreateEngineClient Failed");
        return false;
    }

    // Create an engine command dispatcher object.
    if (!CreateEngineCommand()) {
        VR_ERROR("CreateEngineCommand Failed");
        return false;
    }

    // Creates Event Sink object instance and register it.
    if (!CreateEventSink()) {
        VR_ERROR("CreateEventSink Failed");
        return false;
    }

    // Create an catalog manager object
    if (!CreateCatalogManager()) {
        VR_ERROR("CreateCatalogManager Failed");
    }

    InitPromptOfScreen();

    if (NULL != pcConfig) {
        VR_LOG("The target's model is: %d", pcConfig->getVRProduct());
    }
    return true;
}

void
VR_VoiceBoxEngine::InitPromptOfScreen()
{
    std::string strVRStatePath = VR_ConfigureIF::Instance()->getDataPath();
    strVRStatePath.append("config/VRState.dat");

    FILE * fd = fopen(strVRStatePath.c_str(), "r");
    char buf[256] = { 0 };

    if (NULL == fd) {
        VR_LOG("the fd of the file is null");
        return;
    }

    if (VR_REGION_OC == m_country) {
        while (!feof(fd)) {
            memset(buf, 0, sizeof(buf));
            fgets(buf, sizeof(buf), fd);

            std::string strScreenPrompt = std::string(buf);
            size_t iPos = strScreenPrompt.find("|");

            std::string strScreenId;
            std::string strPromptOC;
            if (std::string::npos != iPos) {
                strScreenId = strScreenPrompt.substr(0, iPos);
            }

            size_t iPos1 = strScreenPrompt.find("|", iPos + 1);
            if (std::string::npos != iPos1) {
                strPromptOC = strScreenPrompt.substr(iPos + 1, iPos1 - iPos - 1);
            }

            if (!strScreenId.empty() && !strPromptOC.empty()) {
                m_mapScreenPromptOC.insert(std::make_pair(strScreenId, strPromptOC));
            }
        }
    }
    else if (VR_REGION_US == m_country) {
        while (!feof(fd)) {
            memset(buf, 0, sizeof(buf));
            fgets(buf, sizeof(buf), fd);

            std::string strScreenPrompt = std::string(buf);
            size_t iPos = strScreenPrompt.find("|");

            std::string strScreenId;
            std::string strPromptMX;
            std::string strPromptCA;
            std::string strPromptUS;

            if (std::string::npos != iPos) {
                strScreenId = strScreenPrompt.substr(0, iPos);
            }
            VoiceVector<std::string>::type vecPrompt;
            size_t iPos1 = strScreenPrompt.find("|", iPos + 1);
            if (std::string::npos != iPos1) { // en-MX
                strPromptMX = strScreenPrompt.substr(iPos + 1, iPos1 - iPos - 1);
                vecPrompt.push_back(strPromptMX);
            }

            size_t iPos2 = strScreenPrompt.find("|", iPos1 + 1);
            if (std::string::npos != iPos2) { // fr-CA
                strPromptCA = strScreenPrompt.substr(iPos1 + 1, iPos2 - iPos1 - 1);
                vecPrompt.push_back(strPromptCA);
            }

            size_t iPos3 = strScreenPrompt.find("|", iPos2 + 1);
            if (std::string::npos != iPos3) { // en-US
                strPromptUS = strScreenPrompt.substr(iPos2 + 1, iPos3 - iPos2 - 1);
                vecPrompt.push_back(strPromptUS);
            }

            m_mapScreenPrompt.insert(std::make_pair(strScreenId, vecPrompt));
        }
    }

    fclose(fd);

    return;
}

// Start the VoiceBox engine with the specified culture
bool
VR_VoiceBoxEngine::Start(const std::string &strCultureName)
{
    VR_LOGD_FUNC();

    VR_LOG("current culture : %s", strCultureName.c_str());

    if (NULL == m_engineCommand.ptr()) {
        // VR_ERROR("The engine have not been initialized");
        return false;
    }
    m_strCultureName = strCultureName;
    m_strInstallCulture = strCultureName;
    m_bEngineStarting = false;

    // Start the dialog engine with the specified culture
    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->StartEngine(
                     &transaction,
                     strCultureName.c_str()
                     );
    if (FAILED(result) || (NULL == transaction.ptr())) {
        // VR_ERROR("Start Engine Failed, result: %lx, transaction: %p", result, transaction);
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnStartEngine
            )
        );

    // Make it a sync operation, wait until the engine is started
    VBT_BOOL bTimeout = VBT_FALSE;
    result = transaction->WaitForCompletion(INFINITE, &bTimeout);
    if (FAILED(result) || bTimeout) {
        // VR_ERROR("Waiting Engine Start Failed, result: %lx, timeout: %n", result, bTimeout);
        return false;
    }

    if ("en-AU" == strCultureName) {
        UpdateMapGrammar();
    }

    if ("" == m_strDevice) {
        m_strAdaptationPath = GetAdaptationProfilePath(strCultureName, "default");
    }
    else {
        m_strAdaptationPath = GetAdaptationProfilePath(strCultureName, m_strDevice);
    }

    result = m_engineCommand->LoadSpeakerProfile(
             &transaction,
             m_strAdaptationPath.c_str());
    if (FAILED(result) || (NULL == transaction.ptr())) {
        // VR_ERROR("error :SpeakAdaptation error");
        return false;
    }
    if (NULL == m_pcCatalogPhone) {
        // m_pcCatalogPhone = VR_new VR_VoiceBoxCatalogPhone(*m_client, *this);
        m_pcCatalogPhone = m_pcCatalogManager->GetCatalogPhone();
        VR_LOG("Get m_pcCatalogPhone");
    }

    m_bEngineReady = true;

    m_pcMsgController->PostMessage("<grammar_disactive agent=\"media\" reply=\"false\" grammarid=\"5\"/>");

    return true;
}

// Stop the VoiceBox engine
void
VR_VoiceBoxEngine::Stop()
{
    VR_LOGD_FUNC();
    m_bEngineStarting = false;
    NotifyResourceState();

    if (NULL == m_engineCommand) {
        VR_ERROR("The engine have not been initialized");
        return;
    }

    if (NULL != m_pcCatalogManager && !m_pairTransaction.first.empty() && !m_pairTransaction.second.empty()) {
        m_pcCatalogManager->CancelGrammarUpdate(m_pairTransaction.first);
    }

    CVECIPtr<IVECITransaction> saveTrans;
    HRESULT saveRet = m_engineCommand->SaveSpeakerProfile(
                      &saveTrans,
                      m_strAdaptationPath.c_str());
    if (FAILED(saveRet) || (NULL == saveTrans.ptr())) {
        VR_ERROR("Save speaker profile failed: %lx", saveRet);
    }
    else {
        VBT_BOOL bSaveTimeout = VBT_FALSE;
        saveRet = saveTrans->WaitForCompletion(INFINITE, &bSaveTimeout);
        if (FAILED(saveRet) || bSaveTimeout) {
            VR_ERROR("Save speaker profile waiting failed: %lx", saveRet);
        }
    }

    // Stop the dialog engine immediately.
    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->StopEngine(
                     &transaction,
                     VBT_TRUE         // Stop immediately
                     );
    if (FAILED(result) || (NULL == transaction.ptr())) {
        // VR_ERROR("Start Engine Failed, result: %lx, transaction: %p", result, transaction);
        return;
    }

    // Wait until the engine is stopped
    VBT_BOOL bTimeout = VBT_FALSE;
    result = transaction->WaitForCompletion(INFINITE, &bTimeout);
    if (FAILED(result) || bTimeout) {
        // VR_ERROR("Waiting Engine Start Failed, result: %lx, timeout: %n", result, bTimeout);
        return;
    }
}

// Uninitialize the dialog engine
void
VR_VoiceBoxEngine::UnInitialize()
{
    VR_LOGD_FUNC();

    // Set the external instance pointers to NULL
    m_pcExternalCallback = NULL;
    m_pcMsgController = NULL;
    m_pcPlayTransation = NULL;

    if (NULL == m_client) {
        return;
    }

    int nAgentNum = AgentType_Num;
    for (int i = 0; i < nAgentNum; ++i) {
        if (NULL == m_agents[i]) {
            continue;
        }

        m_agents[i]->UnInitialize();

        // Unregister event sink and shutdown the engine
        HRESULT ret = m_client->UnregisterEventSink(m_agents[i]);
        if (FAILED(ret)) {
            // VR_ERROR("UnregisterEventSink Failed, ret: %lx", ret);
        }

        // This instance will be deleted in UnregisterEventSink(),
        // so we need not delete it.
        m_agents[i] = NULL;
    }

    HRESULT result = m_client->Shutdown();
    if (FAILED(result)) {
        // VR_ERROR("Shutdown Failed, result: %lx", result);
    }

    m_mapMsgHandler.clear();
    m_mapMsgBtnHandler.clear();
    m_mapCmdResultHandler.clear();
    m_mapAgenttoRecoState.clear();
    m_mapHintsScreenID.clear();
    m_mapActiveSouceTrans.clear();
    m_lstUCAppQuitVROp.clear();
    m_lstNeedReactiveAction.clear();

    VR_AudioStreamIF* pcAudioStream = VR_AudioStreamIF::Instance();
    if (NULL != pcAudioStream) {
        pcAudioStream->UnInitialize();
    }
}

// Creates an Engine Client object and initializes it.
bool
VR_VoiceBoxEngine::CreateEngineClient()
{
    VR_LOGD_FUNC();

    // Creates an Engine Client object instance
    HRESULT result = CreateVBTEngineClient(&m_client);
    if (FAILED(result) || (NULL == m_client.ptr())) {
        // VR_ERROR("Create Engine Failed, result: %lx, client: %p", result, m_client);
        return false;
    }

    VARIANT value;
    value.vt = VT_BOOL;
    value.boolVal = VARIANT_TRUE;

    // Set property of PropAudioLevelThread
    result = m_client->SetProperty(PropAudioLevelThread, &value);
    if (FAILED(result)) {
        // VR_ERROR("SetProperty Failed, result: %lx", result);
        return false;
    }

    value.vt = VT_BOOL;
    value.boolVal = VARIANT_TRUE;
    result = m_client->SetProperty(PropClientManagedTTS, &value);
    if (FAILED(result)) {
        // VR_ERROR("SetProperty Failed, result: %lx", result);
        return false;
    }

    CVECIPtr<IVECIInitParams> initParams;
    result = CreateVECIInitParams(&initParams);
    if (FAILED(result) || (NULL == initParams.ptr())) {
        // VR_ERROR("CreateVECIInitParams Failed, result: %lx, initParams: %p", result, initParams);
        return false;
    }

    // Set the client name setting
    result = initParams->SetName(_T(""));
    if (FAILED(result)) {
        // VR_ERROR("Set Client Name Failed, result: %lx", result);
        return false;
    }

    // Set the VBT runtime component dir to empty string
    // for the default dir
    result = initParams->SetRuntimeDir(_T("/data/vr/config"));
    if (FAILED(result)) {
        // VR_ERROR("Set Runtime Dir Failed, result: %lx", result);
        return false;
    }

    // Set the VBT Engine config file
    VBT_CSTR szCfgFileName = _T("./VoiceBox.cfg");
    FILE * fd = fopen("/data/vr/vbtflg/on.flg", "r");
    if (NULL != fd) {
        fclose(fd);

        VR_LOG("Use VoiceBox_On.cfg");
        szCfgFileName = "VoiceBox_On.cfg";
    }

    result = initParams->SetConfigFileName(szCfgFileName);
    if (FAILED(result)) {
        // VR_ERROR("SetConfigFileName Failed, result: %lx", result);
        return false;
    }

    result = initParams->SetFrontEndSharingFactoryParams(
        VBT_ULONG(&CreateVocon4FrontEndShare), 1);
    if (S_OK != result) {
        FAILED(result);
        return false;
    }

    // Initializes the Engine Client object using an IVECIInitParams object.
    result = m_client->InitEx(initParams);
    if (FAILED(result)) {
        // VR_ERROR("Initialize engine Failed, result: %lx", result);
        return false;
    }

    return true;
}

void
VR_VoiceBoxEngine::RenameVbtLog()
{
    char buffer[256] = { 0 };
    time_t now;
    time(&now);
    struct tm* pTime = NULL;
    pTime = localtime(&now);
    if (NULL == pTime) {
        return;
    }
    (void)snprintf(buffer, sizeof(buffer), "%04d%02d%02d%02d%02d%02d",
        1900 + pTime->tm_year, 1 + pTime->tm_mon, pTime->tm_mday,
        pTime->tm_hour, pTime->tm_min, pTime->tm_sec);

    FILE * pVBTLog = fopen("/data/vr/vbtlog/Engine.vbtlog", "r");
    if (NULL != pVBTLog) {
        fclose(pVBTLog);

        char bufnewname[256] = { 0 };
        (void)snprintf(bufnewname, sizeof(bufnewname), "/data/vr/vbtlog/Engine_%s.vbtlog", buffer);
        (void)rename("/data/vr/vbtlog/Engine.vbtlog", bufnewname);
    }

    FILE *pVECILog = fopen("/data/vr/vbtlog/VECILog_1.vbtlog", "r");
    if (NULL != pVECILog) {
        fclose(pVECILog);

        char bufnewname[256] = { 0 };
        (void)snprintf(bufnewname, sizeof(bufnewname), "/data/vr/vbtlog/VECILog_1_%s.vbtlog", buffer);
        (void)rename("/data/vr/vbtlog/VECILog_1.vbtlog", bufnewname);
    }
}

// Creates Event Sink object instance and register it.
bool VR_VoiceBoxEngine::CreateEventSink()
{
    VR_LOGD_FUNC();

    // If the Engine Client object was NOT created,
    // we could not register the Event Sink object.
    if (NULL == m_client) {
        return false;
    }

        // Create Event Sink object instance
    m_agents[AgentType_Global] = VR_new VR_VoiceBoxAgentGlobal(*m_client, *m_engineCommand, *this);
    if (NULL == m_agents[AgentType_Global]) {
        // VR_ERROR("Create VR_VoiceBoxEventSink Failed");
        return false;
    }

        // Create Event Sink object instance
    m_agents[AgentType_Climate] = VR_new VR_VoiceBoxAgentClimate(*m_client, *m_engineCommand, *this);
    if (NULL == m_agents[AgentType_Climate]) {
        // VR_ERROR("Create VR_VoiceBoxEventSink Failed");
        return false;
    }

        // Create Event Sink object instance
    m_agents[AgentType_Info] = VR_new VR_VoiceBoxAgentInfo(*m_client, *m_engineCommand, *this);
    if (NULL == m_agents[AgentType_Info]) {
        // VR_ERROR("Create VR_VoiceBoxEventSink Failed");
        return false;
    }

        // Create Event Sink object instance
    m_agents[AgentType_Navi] = VR_new VR_VoiceBoxAgentNavi(*m_client, *m_engineCommand, *this);
    if (NULL == m_agents[AgentType_Navi]) {
        // VR_ERROR("Create VR_VoiceBoxEventSink Failed");
        return false;
    }

    // Create Event Sink object instance
    if (VR_REGION_US == m_country) {
        VR_LOG("US VR_VoiceBoxAgentAudio&VR_VoiceBoxAgentPhone new");
        // Create Event Sink object instance
        m_agents[AgentType_Audio] = VR_new VR_VoiceBoxAgentAudio(*m_client, *m_engineCommand, *this);
        if (NULL == m_agents[AgentType_Audio]) {
            // VR_ERROR("Create VR_VoiceBoxEventSink Failed");
            return false;
        }

        m_agents[AgentType_Phone] = VR_new VR_VoiceBoxAgentPhone(*m_client, *m_engineCommand, *this);
        if (NULL == m_agents[AgentType_Phone]) {
            // VR_ERROR("Create VR_VoiceBoxEventSink Failed");
            return false;
        }

        // Create Event Sink object instance
        m_agents[AgentType_Apps] = VR_new VR_VoiceBoxAgentApps(*m_client, *m_engineCommand, *this);
        if (NULL == m_agents[AgentType_Apps]) {
            // VR_ERROR("Create VR_VoiceBoxEventSink Failed");
            return false;
        }
    }
    else if (VR_REGION_OC == m_country) {
        VR_LOG("OC VR_VoiceBoxAgentPhone_AU&VR_VoiceBoxAgentAudio_AU new");
        m_agents[AgentType_Phone] = VR_new VR_VoiceBoxAgentPhone_AU(*m_client, *m_engineCommand, *this);
        if (NULL == m_agents[AgentType_Phone]) {
            // VR_ERROR("Create VR_VoiceBoxEventSink Failed");
            return false;
        }

        m_agents[AgentType_Audio] = VR_new VR_VoiceBoxAgentAudio_AU(*m_client, *m_engineCommand, *this);
        if (NULL == m_agents[AgentType_Audio]) {
            // VR_ERROR("Create VR_VoiceBoxEventSink Failed");
            return false;
        }

        // Create Event Sink object instance
        m_agents[AgentType_Apps] = VR_new VR_VoiceBoxAgentApps_AU(*m_client, *m_engineCommand, *this);
        if (NULL == m_agents[AgentType_Apps]) {
            // VR_ERROR("Create VR_VoiceBoxEventSink Failed");
            return false;
        }
    }
    else {
    }

    int nAgentNum = AgentType_Num;
    for (int i = 0; i < nAgentNum; ++i) {
        if (NULL == m_agents[i]) {
            continue;
        }

        if (!m_agents[i]->Initialize()) {
            return false;
        }

        VBT_INT nMask = EventAgentResult;
        if (AgentType_Global == i) {
            nMask = EventAll;
        }
        // Register Event Sink object instance to the Engine Client object
        HRESULT result = m_client->RegisterEventSink(
                         nMask,
                         VBT_FALSE,
                         m_agents[i]
                         );
        if (FAILED(result)) {
            return false;
        }
    }

    return true;
}

// Create an engine command dispatcher object.
bool VR_VoiceBoxEngine::CreateEngineCommand()
{
    // If the Engine Client object was NOT created,
    // we could not create an engine command dispatcher object.
    if (NULL == m_client) {
        return false;
    }

    // Create an engine command dispatcher instance.
    // This instance builds and sends SMURF messages to the VBT Engine
    HRESULT result = m_client->CreateEngineDispatcher(&m_engineCommand);
    if (FAILED(result) || (NULL == m_engineCommand.ptr())) {
        // VR_ERROR("Create Engine Failed, result: %lx, engine command: %p", result, m_engineCommand);
        return false;
    }

    return true;
}

bool VR_VoiceBoxEngine::CreateCatalogManager()
{
    VR_LOGD_FUNC();

    if (NULL == m_client) {
        return false;
    }

    m_pcCatalogManager = VR_new VR_VoiceBoxCatalogManager(*m_client, *m_engineCommand, *this);
    if (NULL == m_pcCatalogManager) {
        return false;
    }

    return m_pcCatalogManager->Initialize();
}

void
VR_VoiceBoxEngine::ProcessGrammarMSg(const std::string& message)
{
    VR_LOG("Post Message to Catalog controller");

    if (std::string::npos != message.find("category")) {
        if ((VR_REGION_OC == m_country) && std::string::npos != message.find("audiosource")) {
            if (std::string::npos != message.find("Disc")) {
                m_pcCatalogController->PostMessage("<grammar_audiosource_oc agent=\"media\" withdisc=\"true\"></grammar_audiosource_oc>");
            }
            else {
                m_pcCatalogController->PostMessage("<grammar_audiosource_oc agent=\"media\" withdisc=\"false\"></grammar_audiosource_oc>");
            }
            return;
        }

        m_pcCatalogController->PostMessage(message);
    }
    else {
        VR_VoiceBoxXmlParser parser(message);
        std::string strXmlKey = parser.getXmlKey();
        std::string strAgent = parser.getValueByKey("agent");
        if ("phone" == strAgent) {
            if (("grammar_init" == strXmlKey) || ("grammar_active" == strXmlKey)) {
                m_strPath = parser.getValueByKey("path");
            }
            if ("grammar_disactive" == strXmlKey) {
                m_bPhoneGrammarAvailable = false;
            }
            if ("grammar_active" == strXmlKey) {
                m_bPhoneGrammarAvailable = true;
            }
        }
        m_pcCatalogController->PostMessage(message);
    }
}

// Process the messages that came from the external service
void
VR_VoiceBoxEngine::ProcessMessage(const std::string& message, int actionSeqId)
{
    VR_LOGD_FUNC();

    m_iCurrActionId = actionSeqId;

    if (HandleQuitVRApp(message)) {
        return;
    }

    if (message.empty()) {
        return;
    }

    VR_LOG("msg : %s ; id : %i", message.c_str(), actionSeqId);

    VoiceList<int>::iterator it = std::find(m_listPlayVoiceTagSeq.begin(), m_listPlayVoiceTagSeq.end(), actionSeqId);
    if (it != m_listPlayVoiceTagSeq.end()) {
        m_listPlayVoiceTagSeq.erase(it);
        VR_LOGD("remove it, seqId = %d, list size = %d", actionSeqId, m_listPlayVoiceTagSeq.size());
    }

    if (m_iPlayVoiceTagId == actionSeqId) {
        m_iPlayVoiceTagId = 0;
        if (VR_INVALID_ACTION_ID != m_stopVoiceTagBeepOrTTs) {
            m_stopVoiceTagBeepOrTTs = VR_INVALID_ACTION_ID;
            VR_LOG("stop beep action not come back");
        }
        OnRecognized(m_iOss.str());
    }
    if (m_iToturialBeepId == actionSeqId) {
        OnRecognized(m_strToturialResult);
    }
    if (m_iVoiceTrainingBeepId == actionSeqId) {
        OnRecognized(m_strVoiceTrainingResult);
    }

    // Check the header contents
    const int HEADER_LEN = 150;
    std::string header = message.substr(0, HEADER_LEN);

    if (std::string::npos != header.find("queryVehicleinMotion")) {
        VR_LOG("queryVehicleinMotion");
        HandleVehicleDriving(message);
        return;
    }
    if (std::string::npos != header.find("getMessageDetail")) {
        if (VBT_RECO_STATE_INCOMING_MESSAGE == m_strCurrentStateName) {
            VR_LOG("do not solve messagebody");
            return;
        }
    }
    if (std::string::npos != header.find("getMsgDetailStatusNoChange")) {
        if (m_isIncomingMessage) {
            HandleIncomingMessage(message);
            return;
        }
    }
    if (std::string::npos != header.find("queryHintFirstPage")) {
        VR_LOG("queryHintFirstPage");
        QueryHintFirstPage();
        return;
    }
    if (std::string::npos != header.find("queryHintLastPage")) {
        VR_LOG("queryHintLastPage");
        QueryHintLastPage();
        return;
    }
    if (std::string::npos != header.find("smartagent")) {
        SmartAgent(message);
        return;
    }

    if (std::string::npos != header.find("stopTts")) {
        if (actionSeqId == m_iCurTTSReqId) {
            VR_LOG("actionSeqId : %d", actionSeqId);
            TransationSpeakDone(actionSeqId);
        }
    }
    if (std::string::npos != header.find("stopBeep")) {
        if ((m_stopVoiceTagBeepOrTTs == m_iCurrActionId)
            && (m_iCurrActionId != VR_INVALID_ACTION_ID)) {

            m_stopVoiceTagBeepOrTTs = VR_INVALID_ACTION_ID;
            m_listPlayVoiceTagSeq.clear();

            if (0 != m_iPlayVoiceTagId) {
                m_iPlayVoiceTagId = 0;
                OnRecognized(m_iOss.str());
            }

            VR_LOG("cancel recog");

            if (VR_SessionStateStoped == m_sessionState
                || VR_SessionStateNone == m_sessionState) {
                if (m_bDoCancelVoiceTag) {
                    OnRequestAction(VR_ACTION_CLOSESESSION);
                    m_bDoCancelVoiceTag = false;
                }
            }
            else if (VR_SessionStateRestartSessionAfterTTS == m_sessionState) {
                if (m_bDoCancelVoiceTag) {
                    OnRequestAction(VR_ACTION_CLOSESESSION);
                    m_sessionState = VR_SessionStateStoped;
                    m_bDoCancelVoiceTag = false;
                }
            } // stop beep
            else {
                if (m_bDoCancelVoiceTag) {
                    OnRequestAction(VR_ACTION_CLOSESESSION);
                    m_sessionState = VR_SessionStateStoped;
                    m_bDoCancelVoiceTag = false;
                }
            }
        }
    }

    if (std::string::npos != header.find("<action ")) {
        HandleAction(message);
        return;
    }

    if (std::string::npos != header.find("queryRouteExist")) {
        SetRoute(message);
        return;
    }

    if (std::string::npos != header.find("RouteStatusNotify")) {
        NotifyRouteStatus(message);
        return;
    }

    if (std::string::npos == header.find("grammar_new") && std::string::npos != header.find("<category")) {
        ProcessGrammarMSg(message);
        return;
    }

    // If the header contains "grammar", it is for catalog
    if (std::string::npos != header.find("grammar")) {
        // if (std::string::npos != header.find("grammar_diff")) {
        //     GrammarDiffDetail(message);
        // }
        if (std::string::npos != header.find("grammar_new")) {
            GrammarNewDetail(message);
        }
        else {
            if (std::string::npos != header.find("<grammar_active agent=\"media\"")) {
                VR_VoiceBoxXmlParser parser(message);
                m_strDMActiveSourceId = parser.getValueByKey("grammarid");
                if (m_strDMActiveSourceId == m_strGrammarInitSourceId) {
                    if (NULL != m_pcCatalogManager) {
                        m_pcCatalogAudio = m_pcCatalogManager->GetCatalogAudio();
                        if (NULL != m_pcCatalogAudio) {
                            m_pcCatalogAudio->SetDataActiveSource(m_strGrammarInitSourceId);
                        }
                    }
                }
            }
            else if (std::string::npos != header.find("<grammar_disactive agent=\"media\"")) {
                m_strDMActiveSourceId = "5";
            }
            else {

            }

            ProcessGrammarMSg(message);
            return;
        }
    }
    if (std::string::npos != header.find("VoiceTag")) {
        HandleVoiceTag(message);
        return;
    }
    if (std::string::npos != header.find("Install Agent Start")) {
        return;
    }
    if (std::string::npos != header.find("Install Agent End")) {
        return;
    }
    if (std::string::npos != header.find("Install Agent")) {
        // Cancel the curent session
        CancelRecoSession();
        InstallDownloadableAgents(message);
        return;
    }
    if (std::string::npos != header.find("TSL_Token")) {
        VR_LOG("Set TSL Token");
        m_agents[AgentType_Apps]->ProcessMessage(message);
        std::string eventResult = "<event-result name=\"SendAppXmlMessage\" errcode=\"0\"/>";
        OnRecognized(eventResult);
        return;
    }
    if (std::string::npos != header.find("TSLINFO")) {
        // Cancel the curent session
        CancelRecoSession();
        m_agents[AgentType_Apps]->ProcessMessage(message);
        return;
    }

    if (std::string::npos != header.find("StartAppRecognition")) {
        switch (m_bAppRecoState) {
        case VR_AppRecoState_Valid:
            {
                StartAppRecognition(message);
                std::string eventResult = "<event-result name=\"StartAppRecognition\" errcode=\"0\"/>";
                OnRecognized(eventResult);
            }
            break;
        case VR_AppRecoState_Sending:
        case VR_AppRecoState_SendCaching:
            {
                VR_MsgInfo msgInfo;
                msgInfo.iMsgId = actionSeqId;
                msgInfo.strMsg = message;
                m_listAppMessages.push_back(msgInfo);
            }
            break;
        case VR_AppRecoState_Invalid:
            {
                std::string eventResult = "<event-result name=\"StartAppRecognition\" errcode=\"1\"/>";
                OnRecognized(eventResult);
            }
            break;
        default:
            break;
        }

        return;
    }

    if (std::string::npos != header.find("CancelAppRecognition")) {
        switch (m_bAppRecoState) {
        case VR_AppRecoState_Valid:
            {
                if (!CancelAppRecognition(message)) {
                    std::string eventResult = "<event-result name=\"CancelAppRecognition\" errcode=\"1\"/>";
                    OnRecognized(eventResult);
                }
            }
            break;
        case VR_AppRecoState_Sending:
        case VR_AppRecoState_SendCaching:
            {
                VR_MsgInfo msgInfo;
                msgInfo.iMsgId = actionSeqId;
                msgInfo.strMsg = message;
                m_listAppMessages.push_back(msgInfo);
            }
            break;
        case VR_AppRecoState_Invalid:
            {
                std::string eventResult = "<event-result name=\"CancelAppRecognition\" errcode=\"1\"/>";
                OnRecognized(eventResult);
            }
            break;
        default:
            break;
        }

        return;
    }

    if (std::string::npos != header.find("SendAppXmlMessage")) {
        SendAppXmlMessage(message);
        std::string eventResult = "<event-result name=\"SendAppXmlMessage\" errcode=\"0\"/>";
        OnRecognized(eventResult);
        return;
    }

    if (std::string::npos != header.find("updateAppList")) {
        SendAppXmlMessage(message);

        // Update Apps Grammar
        std::string reload = "<event name=\"SendAppXmlMessage\">"
            "<CSVR>"
                "<Message source=\"Client\" class=\"System\">"
                    "<Parameter name=\"Agent\" value=\"Apps\"/>"
                    "<Action type=\"VBT Agent Grammar Update\">"
                        "<Parameter name=\"Table Name\" value=\"AppsAgentApps\"/>"
                        "<Parameter name=\"Operation\" value=\"Reload\"/>"
                    "</Action>"
                "</Message>"
            "</CSVR>"
        "</event>";
        SendAppXmlMessage(reload);

        std::string eventResult = "<event-result name=\"updateAppList\" errcode=\"0\"/>";
        OnRecognized(eventResult);
        return;
    }

    if (std::string::npos != header.find("getSupportedLanguage")) {
        std::string strSupportedLanguage;
        if (VR_REGION_US == m_country) {
            strSupportedLanguage = "<event-result name=\"getSupportedLanguage\" errcode=\"0\">"
                                      "<TSLINFO>"
                                        "<Action type=\"Get Supported Language\"/>"
                                        "<LanguageList>"
                                          "<Item>"
                                            "<Parameter name=\"Language\" value=\"en\"/>"
                                          "</Item>"
                                          "<Item>"
                                            "<Parameter name=\"Language\" value=\"fr\"/>"
                                          "</Item>"
                                          "<Item>"
                                            "<Parameter name=\"Language\" value=\"es\"/>"
                                          "</Item>"
                                        "</LanguageList>"
                                      "</TSLINFO>"
                                    "</event-result>";
        }
        else if (VR_REGION_OC == m_country) {
            strSupportedLanguage = "<event-result name=\"getSupportedLanguage\" errcode=\"0\">"
                                      "<TSLINFO>"
                                        "<Action type=\"Get Supported Language\"/>"
                                        "<LanguageList>"
                                          "<Item>"
                                            "<Parameter name=\"Language\" value=\"en\"/>"
                                          "</Item>"
                                        "</LanguageList>"
                                      "</TSLINFO>"
                                    "</event-result>";
        }
        else {
            return;
        }

        OnRecognized(strSupportedLanguage);
        return;
    }

    VR_VoiceBoxXmlParser parser(message);
    std::string strActionKey = parser.getXmlKey();

    VR_LOG("strActionKey : %s", strActionKey.c_str());
    // Dispatch the messages
    VoiceMap<std::string, MessageHandler>::const_iterator iterMap = m_mapMsgHandler.find(strActionKey);
    if (m_mapMsgHandler.cend() != iterMap) {
        if (NULL != iterMap->second) {
            (this->*(iterMap->second))(parser);
        }
    }
    else {
        std::string strAgent = parser.getValueByKey("agent");
        VR_LOG("ProcessMessage: %s", strAgent.c_str());
        if ("phone" == strAgent) {
            std::string op = parser.getValueByKey("op");
            VR_LOG("op: %s", op.c_str());
            m_agents[AgentType_Phone]->ReplyQueryInfo(parser);
        }
        else if ("climate" == strAgent) {
            m_agents[AgentType_Climate]->ReplyQueryInfo(parser);
        }
        else if ("apps" == strAgent) {
            m_agents[AgentType_Apps]->ReplyQueryInfo(parser);
        }
        else if ("media" == strAgent) {
            m_agents[AgentType_Audio]->ReplyQueryInfo(parser);
        }
        else if ("navi" == strAgent) {
            m_agents[AgentType_Navi]->ReplyQueryInfo(parser);
        }
        else if ("help" == strAgent) {
            m_agents[AgentType_Global]->ReplyQueryInfo(parser);
        }
        else {

        }
    }
}

std::string
VR_VoiceBoxEngine::GetPhonBookDBPath()
{
    VR_LOGD_FUNC();

    return m_strPath;
}

bool
VR_VoiceBoxEngine::GetMessageAvailable()
{
    VR_LOGD_FUNC();

    return m_strMsgAvailable;
}

bool VR_VoiceBoxEngine::GetRouteStatus()
{
    VR_LOGD_FUNC();
    return m_bRouteStatus;
}

void
VR_VoiceBoxEngine::HandleVoiceTag(const std::string& message)
{
    VR_LOGD_FUNC();
    if (NULL == m_client) {
        return;
    }
    pugi::xml_document doc;
    doc.load_string(message.c_str());
    pugi::xml_node eventNode = doc.select_node("//event").node();
    std::string eventName;
    if (eventNode) {
        eventName = eventNode.attribute("name").as_string();
    }
    VR_LOG("eventName: %s", eventName.c_str());
    if ("addRecordVoiceTag" == eventName) {
        VR_LOG("addRecordVoiceTag");
        RecordVoiceTagEvent(eventNode, false);
        return;
    }
    else if ("editRecordVoiceTag" == eventName) {
        RecordVoiceTagEvent(eventNode, true);
        return;
    }
    else if ("saveVoiceTag" == eventName) {
        SaveVoiceTagEvent(eventNode);
        return;
    }
    else if ("deleteVoiceTag" == eventName) {
        DeleteVoiceTagEvent(eventNode);
        return;
    }
    else if ("playVoiceTag" == eventName) {
        PlayVoiceTagEvent(eventNode);
        return;
    }
    else if ("stopVoiceTag" == eventName) {
        StopVoiceTagEvent(eventNode);
        return;
    }
    else if ("cancelRecordVoiceTag" == eventName) {
        CancelRecordVoiceTag(eventNode);
    }
    else if ("cancelSaveVoiceTag" == eventName) {
        CancelSaveVoiceTag(eventNode);
    }
    else if (0 == strcmp(eventName.c_str(), "syncVoiceTag")) {
        SyncVoiceTagEvent(eventNode);
    }
    else {
    }
    return;
}

void
VR_VoiceBoxEngine::SmartAgent(const std::string& message)
{
    VR_LOGD_FUNC();

    pugi::xml_document doc;
    doc.load_string(message.c_str());
    pugi::xml_node eventNode = doc.select_node("//event").node();
    if (eventNode) {
        HandleSmartAgent(eventNode);
        return;
    }

}

void
VR_VoiceBoxEngine::HandleSmartAgent(pugi::xml_node& eventNode)
{
    VR_LOGD_FUNC();

    pugi::xml_node paramNode = eventNode.child("param");
    if (!paramNode) {
        VR_LOG("param is null");
        return;
    }
    std::string paramName = paramNode.attribute("name").as_string();
    std::string userId = paramNode.attribute("userid").as_string();
    VR_LOG("paramName: %s", paramName.c_str());
    VR_LOG("userId: %s", userId.c_str());

    if ("adduser" == paramName) {
        m_userId = userId;
    }
    else if ("deleteuser" == paramName) {
        DeleteSmartAgent(userId);
        m_userId = "";
    }
    else if ("changeuser" == paramName) {
        if ("0" == userId) {
            m_userId = "";
            SetSpeakerProfile("default");
        }
        else {
            m_userId = userId;
            HandlePromptLevel(userId);
            SetSpeakerProfile(userId);
        }
    }
    else {
    }

    std::string strEventResult = "<event-result name=\"smartagent\">";
    strEventResult.append("<param name=\"");
    strEventResult.append(paramName);
    strEventResult.append("\" userid=\"");
    strEventResult.append(userId);
    strEventResult.append("\"/>");
    strEventResult.append("</event-result>");
    OnRecognized(strEventResult);
}

void
VR_VoiceBoxEngine::DeleteSmartAgent(const std::string& userId)
{
    VR_LOGD_FUNC();

    VR_VoiceBoxDataStorage storage;
    std::string promptLevelValueStr;

    storage.GetValue(promptLevelValueStr);
    pugi::xml_document promptLevelValueDoc;
    promptLevelValueDoc.load_string(promptLevelValueStr.c_str());

    pugi::xml_node promptLevelalueNode = promptLevelValueDoc.select_node((std::string("//") + PROMPTLEVEL + userId).c_str()).node();

    if (promptLevelalueNode) {
        std::string nodeName = promptLevelalueNode.name();
        VR_LOG("nodeName: %s", nodeName.c_str());
        promptLevelValueDoc.remove_child(nodeName.c_str());
        std::ostringstream oss;
        promptLevelValueDoc.print(oss);
        promptLevelValueStr = oss.str();

        storage.PutValue(promptLevelValueStr);

        promptLevelValueStr.clear();
        storage.GetValue(promptLevelValueStr);
        VR_LOG("prompt: %s", promptLevelValueStr.c_str());
    }
    else {
        VR_LOG("there is no information for this id in db");
    }
}

void
VR_VoiceBoxEngine::DeleteALLSmartAgent()
{
    VR_LOGD_FUNC();

    VR_VoiceBoxDataStorage storage;
    storage.DeleteValue();
}

void
VR_VoiceBoxEngine::HandlePromptLevel(const std::string& userId)
{
    VR_LOGD_FUNC();

    VR_VoiceBoxDataStorage storage;
    std::string promptLevelValueStr;

    storage.GetValue(promptLevelValueStr);
    pugi::xml_document promptLevelValueDoc;
    promptLevelValueDoc.load_string(promptLevelValueStr.c_str());

    pugi::xml_node promptLevelalueNode = promptLevelValueDoc.select_node((std::string("//") + PROMPTLEVEL + userId).c_str()).node();

    std::string nodeName = promptLevelalueNode.name();
    VR_LOG("nodeName: %s", nodeName.c_str());
    std::string level;
    if (promptLevelalueNode) {
        level = promptLevelalueNode.child("level").text().as_string();
        std::string promptLevel;
        VR_LOG("level: %s", level.c_str());
        if ("0" == level) {
            promptLevel = "off";
        }
        else if ("2" == level) {
            promptLevel = "low";
        }
        else if ("3" == level) {
            promptLevel = "high";
        }
        else {
        }
        std::string strActionResult = "<action agent=\"smartagent\" op=\"setPromptLevel\" >"
                                      "<level>";
        strActionResult.append(promptLevel);
        strActionResult.append("</level></action>");
        OnRequestAction(strActionResult);
    }
    else {
        SavePromptLevel(userId, promptLevelValueDoc);
    }

}

void
VR_VoiceBoxEngine::SavePromptLevel(const std::string& strKeyValue)
{
    VR_LOGD_FUNC();

    VR_LOG("strKeyValue: %s", strKeyValue.c_str());
    int level = VR_PROMPTLEVEL_HIGH;
    if ("" != m_userId) {
        if ("OFF" == strKeyValue) {
            level = VR_PROMPTLEVEL_OFF;
        }
        else if ("SIMPLE" == strKeyValue) {
            level = VR_PROMPTLEVEL_LOW;
        }
        else if ("FULL" == strKeyValue) {
            level = VR_PROMPTLEVEL_HIGH;
        }
        std::string promptLevel;
        std::stringstream ss;
        ss << level;
        ss >> promptLevel;

        VR_VoiceBoxDataStorage storage;
        std::string promptLevelValueStr;
        storage.GetValue(promptLevelValueStr);
        pugi::xml_document promptLevelValueDoc;
        promptLevelValueDoc.load_string(promptLevelValueStr.c_str());
        promptLevelValueDoc.remove_child((std::string(PROMPTLEVEL) + m_userId).c_str());

        pugi::xml_document levelValueDoc;
        levelValueDoc.load_string("");
        pugi::xml_node levelValueNode = levelValueDoc.append_child((std::string(PROMPTLEVEL) + m_userId).c_str());
        levelValueNode.append_child("level").text().set(promptLevel.c_str());
        std::ostringstream osss;
        levelValueNode.print(osss);
        VR_LOG("levelValueNode Value: %s", osss.str().c_str());
        promptLevelValueDoc.append_copy(levelValueNode);

        promptLevelValueStr.clear();
        std::ostringstream oss;
        promptLevelValueDoc.print(oss);
        promptLevelValueStr = oss.str();
        VR_LOG("promptLevelValueStr Value: %s", promptLevelValueStr.c_str());

        storage.PutValue(promptLevelValueStr);
    }

}

void
VR_VoiceBoxEngine::SavePromptLevel(const std::string& userId, pugi::xml_document& promptLevelValueDoc)
{
    VR_LOGD_FUNC();

    pugi::xml_document levelValueDoc;
    levelValueDoc.load_string("");
    pugi::xml_node levelValueNode = levelValueDoc.append_child((std::string(PROMPTLEVEL) + userId).c_str());

    std::string promptLevel;
    std::stringstream ss;
    ss << m_iPromptLevel;
    ss >> promptLevel;
    VR_LOG("promptLevel: %s", promptLevel.c_str());
    levelValueNode.append_child("level").text().set(promptLevel.c_str());
    std::ostringstream osss;
    levelValueNode.print(osss);
    VR_LOG("levelValueNode Value: %s", osss.str().c_str());

    VR_VoiceBoxDataStorage storage;
    std::string promptLevelValueStr;

    promptLevelValueDoc.append_copy(levelValueNode);

    // promptLevelValueStr.clear();
    std::ostringstream oss;
    promptLevelValueDoc.print(oss);
    promptLevelValueStr = oss.str();
    VR_LOG("promptLevelValueStr Value: %s", promptLevelValueStr.c_str());

    storage.PutValue(promptLevelValueStr);

}

void
VR_VoiceBoxEngine::QueryHintFirstPage()
{
    VR_LOGD_FUNC();

    std::string strValue = "false";
    if (m_iIndex <= 5) {
        strValue = "true";
    }

    std::string strActionResult = "<action-result agent=\"help\" op=\"queryHintFirstPage\" >"
                                      "<value>";
    strActionResult.append(strValue);
    strActionResult.append("</value></action-result>");
    VR_VoiceBoxXmlParser parserTmp(strActionResult);
    m_agents[AgentType_Global]->ReplyQueryInfo(parserTmp);
}

void
VR_VoiceBoxEngine::QueryHintLastPage()
{
    VR_LOGD_FUNC();

    std::string strValue = "false";
    if (m_iHintSize == m_iIndex) {
        strValue = "true";
    }
    std::string strActionResult = "<action-result agent=\"help\" op=\"queryHintLastPage\" >"
                                      "<value>";
    strActionResult.append(strValue);
    strActionResult.append("</value></action-result>");
    VR_VoiceBoxXmlParser parserTmp(strActionResult);
    m_agents[AgentType_Global]->ReplyQueryInfo(parserTmp);
}

void
VR_VoiceBoxEngine::HandleVehicleDriving(const std::string& strMessage)
{
    VR_LOGD_FUNC();

    VR_VoiceBoxXmlParser parser(strMessage);
    std::string strAgent = parser.getValueByKey("agent");
    std::string strMotion = parser.getValueByKey("motion");
    VR_LOG("HandleVehicleDriving: %s", strAgent.c_str());
    VR_VoiceBoxEventSink* agentTmp = NULL;
    if ("phone" == strAgent) {
        agentTmp = m_agents[AgentType_Phone];
    }
    else if ("climate" == strAgent) {
        agentTmp = m_agents[AgentType_Climate];
    }
    else if ("apps" == strAgent) {
        agentTmp = m_agents[AgentType_Apps];
    }
    else if ("media" == strAgent) {
        agentTmp = m_agents[AgentType_Audio];
    }
    else if ("navi" == strAgent) {
        agentTmp = m_agents[AgentType_Navi];
    }
    else if ("help" == strAgent) {
        agentTmp = m_agents[AgentType_Global];
    }
    else {
        return;
    }

    std::string strVehicleDriving = "<action-result agent=\"";
    strVehicleDriving.append(strAgent);
    strVehicleDriving.append("\" op=\"queryVehicleinMotion\"><motion>");
    strVehicleDriving.append(strMotion);
    strVehicleDriving.append("</motion></action-result>");
    VR_LOG("strVehicleDriving : %s", strVehicleDriving.c_str());
    VR_VoiceBoxXmlParser parserTmp(strVehicleDriving);

    agentTmp->ReplyQueryInfo(parserTmp);
}

void
VR_VoiceBoxEngine::SetRoute(const std::string& strMessage)
{
    VR_LOGD_FUNC();

    VR_VoiceBoxXmlParser parser(strMessage);
    std::string strExist = parser.getValueByKey("exist");
    SetVBTPrefRouteActive(strExist);

}

void
VR_VoiceBoxEngine::NotifyRouteStatus(const std::string& strMessage)
{
    VR_LOGD_FUNC();

    VR_VoiceBoxXmlParser parser(strMessage);
    std::string strStatus = parser.getValueByKey("status");
    SetVBTPrefRouteActive(strStatus);
}

void
VR_VoiceBoxEngine::SetVBTPrefRouteActive(const std::string& strStatus)
{
    std::string strExist;
    if ("true" == strStatus) {
        VR_LOG("set route true");
        strExist = "True";
        m_bRouteStatus = true;
    }
    else {
        VR_LOG("set route false");
        strExist = "False";
        m_bRouteStatus = false;
    }

    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->SetPreference(
                     &transaction,
                     VBT_AGENT_SYSTEM,
                     VBT_USR_PREF_SYSTEM_ROUTEACTIVE,
                     strExist.c_str());
    if (FAILED(result) || (NULL == transaction.ptr())) {
        VR_LOG("set route error");
    }
}

void
VR_VoiceBoxEngine::PlayVoiceTagEvent(pugi::xml_node& eventNode)
{
    VR_LOGD_FUNC();

    VR_LOG("m_stopVoiceTagBeepOrTTs %d", m_stopVoiceTagBeepOrTTs);

    m_stopVoiceTagBeepOrTTs = VR_INVALID_ACTION_ID;

    std::string voiceTagID(eventNode.child("voiceTagId").text().as_string());
    std::string voiceTagPCMPath;
    if (VR_ConfigureIF::Instance()->isFileExist(VOICETAGFILE)) {
        voiceTagPCMPath.assign(VR_ConfigureIF::Instance()->getUsrPath() + VOICETAGFILE);
    }
    else {
        VR_VoiceBoxVoiceTag voiceTag;
        voiceTagPCMPath = voiceTag.GetVoiceTagPCMPath(voiceTagID, m_deviceAddress);
    }

    if (voiceTagPCMPath.empty()) {
        eventNode.append_attribute("errcode").set_value((int)VR_VoiceTagResult::VOICETAG_FAILURE);
        eventNode.set_name("event-result");
        m_iOss.str("");
        eventNode.print(m_iOss);
        OnRecognized(m_iOss.str());

    }
    else {
        OnBeep(voiceTagPCMPath);
        eventNode.append_attribute("errcode").set_value((int)VR_VoiceTagResult::VOICETAG_OK);
    }
    eventNode.set_name("event-result");
    m_iOss.str("");
    eventNode.print(m_iOss);
    VR_LOG("m_iOss: %s", m_iOss.str().c_str());

    VR_LOG("PlayVoiceTagEvent");
}

void
VR_VoiceBoxEngine::StopVoiceTagEvent(pugi::xml_node& eventNode)
{
    VR_LOGD_FUNC();

    eventNode.set_name("event-result");
    eventNode.append_attribute("errcode").set_value((int)VR_VoiceTagResult::VOICETAG_OK);

    m_iOss.str("");
    eventNode.print(m_iOss);
    VR_LOG("m_iOss: %s", m_iOss.str().c_str());
    if (VR_INVALID_ACTION_ID != m_stopVoiceTagBeepOrTTs) {
        m_iPlayVoiceTagId = 0;
        OnRecognized(m_iOss.str());
    }
    else {
        m_stopVoiceTagBeepOrTTs = OnRequestAction(VR_ACTION_STOPBEEP);
    }

}

void
VR_VoiceBoxEngine::CancelRecordVoiceTag(pugi::xml_node& eventNode)
{
    VR_LOGD_FUNC();

    HandleVoiceTagBeepOrTTs();

    bool removeOK = false;
    if (VR_ConfigureIF::Instance()->isFileExist(VOICETAGFILE)) {
        removeOK = VR_ConfigureIF::Instance()->removeFile(VOICETAGFILE);
        VR_LOG("removeOK :%d", removeOK);
    }
    else {
        removeOK = true;
    }
    if (removeOK) {
        eventNode.append_attribute("errcode").set_value((int)VR_VoiceTagResult::VOICETAG_OK);
    }
    else {
        eventNode.append_attribute("errcode").set_value((int)VR_VoiceTagResult::VOICETAG_FAILURE);
    }
    eventNode.set_name("event-result");

    std::stringstream oss;
    eventNode.print(oss);
    VR_LOG("oss: %s", oss.str().c_str());

    OnRecognized(oss.str());

}

void
VR_VoiceBoxEngine::HandleVoiceTagBeepOrTTs()
{
    m_bDoCancelVoiceTag = true;

    VR_LOGD("list size = %d", m_listPlayVoiceTagSeq.size());
    if (VR_INVALID_ACTION_ID != m_stopVoiceTagBeepOrTTs) {
        VR_LOG("there is no need to stop beep or tts again");
        return;
    }

    if (VR_SessionStateStoped == m_sessionState
        || VR_SessionStateNone == m_sessionState) {
        VR_LOG("VR is over");
        if (m_listPlayVoiceTagSeq.empty()) {
            m_bDoCancelVoiceTag = false;
            OnRequestAction(VR_ACTION_CLOSESESSION);
            return;
        }
    }

    if (m_listPlayVoiceTagSeq.empty()) {
        m_stopVoiceTagBeepOrTTs = VR_INVALID_ACTION_ID;
        CancelVoiceTagRecoSession();
    }
    else {
        switch (m_lastPlayed) {
        case TTS :
            {
                std::string strStopTTS = "<action agent=\"prompt\" op=\"stopTts\"><reqId>";
                strStopTTS.append(std::to_string(m_iCurTTSReqId));
                strStopTTS.append("</reqId></action>");
                m_stopVoiceTagBeepOrTTs = OnRequestAction(strStopTTS);
                VR_LOG("stop current speak id, %d", m_iCurTTSReqId);
                m_mapStopTTSTransation.insert(std::make_pair(m_stopVoiceTagBeepOrTTs, m_iCurTTSReqId));
                break;
            }
        case BEEP :
            {
                m_stopVoiceTagBeepOrTTs = OnRequestAction(VR_ACTION_STOPBEEP);
                VR_LOGD("m_stopVoiceTagBeepOrTTs %d", m_stopVoiceTagBeepOrTTs);
                break;
            }
        default:
            break;
        }
    }
}

void
VR_VoiceBoxEngine::CancelSaveVoiceTag(pugi::xml_node& eventNode)
{
    VR_LOGD_FUNC();

    HandleVoiceTagBeepOrTTs();

    bool removeOK = false;
    if (VR_ConfigureIF::Instance()->isFileExist(VOICETAGFILE)) {
        removeOK = VR_ConfigureIF::Instance()->removeFile(VOICETAGFILE);
        VR_LOG("removeOK :%d", removeOK);
    }
    else {
        removeOK = true;
    }
    if (removeOK) {
        eventNode.append_attribute("errcode").set_value((int)VR_VoiceTagResult::VOICETAG_OK);
    }
    else {
        eventNode.append_attribute("errcode").set_value((int)VR_VoiceTagResult::VOICETAG_FAILURE);
    }
    eventNode.set_name("event-result");

    std::stringstream oss;
    eventNode.print(oss);
    VR_LOG("oss: %s", oss.str().c_str());
    OnRecognized(oss.str());
}

void VR_VoiceBoxEngine::SyncVoiceTagEvent(pugi::xml_node& eventNode)
{
    VR_LOGD_FUNC();
    std::string deviceAddress = eventNode.child("deviceAddress").text().as_string();
    std::string btVoiceTagIDListStr = eventNode.child("voiceTagIds").text().as_string();
    VR_LOG("btVoiceTagIDListStr: %s", btVoiceTagIDListStr.c_str());
    VoiceList<std::string>::type btVoiceTagIDList;
    if (!btVoiceTagIDListStr.empty()) {
        size_t startPos = 0;
        size_t pos = btVoiceTagIDListStr.find(",");
        while (std::string::npos != pos) {
            btVoiceTagIDList.push_back(btVoiceTagIDListStr.substr(startPos, pos - startPos));
            startPos = pos + 1;
            pos = btVoiceTagIDListStr.find(",", startPos);
        }
        btVoiceTagIDList.push_back(btVoiceTagIDListStr.substr(startPos));

    }
    std::string strGrammar;
    SyncVoiceTag(strGrammar, deviceAddress, btVoiceTagIDList);
    VR_LOG("strGrammar : %s", strGrammar.c_str());
    if ("" != strGrammar) {
        m_pcMsgController->PostMessage(strGrammar);
    }

    btVoiceTagIDListStr.clear();
    VoiceList<std::string>::iterator it = btVoiceTagIDList.begin();
    while (btVoiceTagIDList.end() != it) {
        btVoiceTagIDListStr += (*it + ",");
        ++it;
    }
    if (!btVoiceTagIDListStr.empty()) {
        btVoiceTagIDListStr.pop_back();
    }
    eventNode.child("voiceTagIds").text().set(btVoiceTagIDListStr.c_str());
    eventNode.set_name("event-result");
    std::stringstream oss;
    std::string strOss;
    eventNode.print(oss);
    oss >> strOss;
    VR_LOG("strOss: %s", strOss.c_str());
    OnRecognized(oss.str());
}

bool VR_VoiceBoxEngine::SyncVoiceTag(std::string &strGrammar, const std::string &deviceAddress, VoiceList<std::string>::type &btVoiceTagIDList)
{
    VR_LOGD_FUNC();

    VR_VoiceBoxVoiceTag voiceTag;
    if (btVoiceTagIDList.empty()) {
        VR_LOG("Delete ALL");
        return voiceTag.DeleteAllVoiceTag(strGrammar, deviceAddress);
    }

    int key = VR_VoiceTagIDManager::getInstance()->getVoiceTagStorageKey(deviceAddress);
    std::string deviceVoiceTagValueStr;
    VR_VoiceBoxDataStorage storage;
    VR_LOG("Delete VoiceTag Begin");
    storage.GetValue(key, deviceVoiceTagValueStr);
    VR_LOG("VoiceTag Value: %s", deviceVoiceTagValueStr.c_str());

    pugi::xml_document deviceVoiceTagValueDoc;
    deviceVoiceTagValueDoc.load_string(deviceVoiceTagValueStr.c_str());
    pugi::xml_node tempNode = deviceVoiceTagValueDoc.first_child();
    VoiceList<std::string>::type voiceTagIDList;
    while (!tempNode.empty()) {
        std::string voiceTagID = tempNode.name();
        VR_LOG("voiceTagID: %s", voiceTagID.c_str());
        if (std::string::npos != voiceTagID.find(VR_VOICETAG_ID_PREFIX)) {
            voiceTagIDList.push_back(std::string(tempNode.name()).substr(std::string(VR_VOICETAG_ID_PREFIX).size()));
        }
        else {
            deviceVoiceTagValueDoc.remove_child(voiceTagID.c_str());
        }
        tempNode = tempNode.next_sibling();
    }
    std::ostringstream oss;
    deviceVoiceTagValueDoc.print(oss);
    deviceVoiceTagValueStr = oss.str();
    VR_LOG("new voicetag: %s", deviceVoiceTagValueStr.c_str());
    storage.PutValue(key, deviceVoiceTagValueStr);

    btVoiceTagIDList.sort();
    voiceTagIDList.sort();

    VoiceList<std::string>::iterator btIt = btVoiceTagIDList.begin();
    VoiceList<std::string>::iterator it = voiceTagIDList.begin();
    for (; btIt != btVoiceTagIDList.end() && it != voiceTagIDList.end();) {
        if (*btIt < *it) {
            ++btIt;
        }
        else if (*it < *btIt) {
            ++it;
        }
        else {
            btIt = btVoiceTagIDList.erase(btIt);
            it = voiceTagIDList.erase(it);
        }
    }

    it = voiceTagIDList.begin();

    while (it != voiceTagIDList.end()) {
        std::string strTempGrammar;
        VoiceList<std::string>::type voiceTagIDListDelete;
        voiceTagIDListDelete.push_back(*it);
        voiceTag.DeleteVoiceTag(strTempGrammar, deviceAddress, voiceTagIDListDelete);
        VR_LOG("strTempGrammar : %s", strTempGrammar.c_str());
        if ("" != strTempGrammar) {
            m_pcMsgController->PostMessage(strTempGrammar);
        }
        ++it;
    }
    return true;
}


void
VR_VoiceBoxEngine::CancelVoiceTagRecoSession()
{
    VR_LOGD_FUNC();

    // Cancel any ASR or TTS session that might be currently running
    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->CancelSession(&transaction);
    if (FAILED(result) || (NULL == transaction.ptr())) {
        VR_ERROR("CancelSession Failed, result: %lx", result);
        return;
    }

    // Wait until the engine is stopped
    VBT_BOOL bTimeout = VBT_FALSE;
    result = transaction->WaitForCompletion(INFINITE, &bTimeout);
    if (FAILED(result) || bTimeout) {
        VR_ERROR("Waiting Session Cancel Failed, result: %lx", result);
        // return false;
    }
}

void
VR_VoiceBoxEngine::DeleteVoiceTagEvent(pugi::xml_node& eventNode)
{
    VR_LOGD_FUNC();

    VR_LOG("DeleteVoiceTagEvent!");
    VoiceList<std::string>::type voiceTagIDList;
    std::string deviceAddress(eventNode.child("deviceAddress").text().as_string());
    VR_LOG("deviceAddress: %s", deviceAddress.c_str());
    pugi::xml_node tempNode = eventNode.first_child();
    while (!tempNode.empty()) {
        VR_LOG("tempNode");
        std::string name = tempNode.name();
        VR_LOG("name: %s", name.c_str());
        if ("voiceTagId" == std::string(tempNode.name())) {
            std::string voiceTagID(tempNode.text().as_string());
            VR_LOG("voiceTagID: %s", voiceTagID.c_str());
            if (!voiceTagID.empty()) {
                voiceTagIDList.push_back(voiceTagID);
            }
        }
        tempNode = tempNode.next_sibling();
    }
    VR_VoiceBoxVoiceTag voiceTag;
    bool result = false;
    std::string strGrammar;
    if (voiceTagIDList.empty()) {
        result = voiceTag.DeleteAllVoiceTag(strGrammar, deviceAddress);
    }
    else {
        result = voiceTag.DeleteVoiceTag(strGrammar, deviceAddress, voiceTagIDList);
    }
    VR_LOG("strGrammar : %s", strGrammar.c_str());
    if ("" != strGrammar) {
        m_pcMsgController->PostMessage(strGrammar);
    }

    eventNode.remove_child("deviceAddress");
    eventNode.set_name("event-result");
    if (result) {
        eventNode.append_attribute("errcode").set_value((int)VR_VoiceTagResult::VOICETAG_OK);
    }
    else {
        eventNode.append_attribute("errcode").set_value((int)VR_VoiceTagResult::VOICETAG_FAILURE);
    }

    std::stringstream oss;
    std::string strOss;
    eventNode.print(oss);
    VR_LOG("oss: %s", oss.str().c_str());
    oss >> strOss;
    VR_LOG("strOss: %s", strOss.c_str());
    OnRecognized(oss.str());

}

void
VR_VoiceBoxEngine::SaveVoiceTagEvent(pugi::xml_node& eventNode)
{
    VR_LOGD_FUNC();

    HandleVoiceTagBeepOrTTs();

    std::string voiceTagID(eventNode.child("voiceTagId").text().as_string());
    VR_VoiceBoxVoiceTag voiceTag;
    VoiceTagInfo voiceTagPara;
    voiceTagPara.voiceTagID = voiceTagID;
    voiceTagPara.pcmPath = m_tempPcmFile;
    voiceTagPara.phoneme = m_tempPronunc;
    voiceTagPara.contactMsg = m_voiceTagContactMsg;
    std::string strGrammar;
    bool result = voiceTag.SaveVoiceTag(strGrammar, m_deviceAddress, voiceTagPara, "false", m_isVoiceTagUpdate);
    VR_LOG("strGrammar : %s", strGrammar.c_str());
    if ("" != strGrammar) {
        m_pcMsgController->PostMessage(strGrammar);
    }

    eventNode.set_name("event-result");
    if (result) {
        eventNode.append_attribute("errcode").set_value((int)VR_VoiceTagResult::VOICETAG_OK);
    }
    else {
        eventNode.append_attribute("errcode").set_value((int)VR_VoiceTagResult::VOICETAG_FAILURE);
    }

    std::stringstream oss;
    std::string strOss;
    eventNode.print(oss);
    VR_LOG("oss: %s", oss.str().c_str());
    oss >> strOss;
    VR_LOG("strOss: %s", strOss.c_str());
    OnRecognized(oss.str());
}

void
VR_VoiceBoxEngine::RecordVoiceTagEvent(pugi::xml_node& eventNode, bool isUpdate)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand) {
        return;
    }

    if (!m_bPhoneGrammarAvailable) {
        std::string strVoiceTagId(eventNode.child("voiceTagId").text().as_string());
        std::string strActionID(eventNode.child("actionId").text().as_string());
        pugi::xml_document eventDoc;
        eventDoc.load_string("");
        pugi::xml_node newEventNode = eventDoc.append_child("event-result");
        if (!m_isVoiceTagUpdate) {
            newEventNode.append_attribute("name").set_value("addRecordVoiceTag");
        }
        else {
            newEventNode.append_attribute("name").set_value("editRecordVoiceTag");
        }
        newEventNode.append_attribute("errcode").set_value((int)VR_VoiceTagResult::VOICETAG_FAILURE);
        newEventNode.append_child("actionId").text().set(strActionID.c_str());
        newEventNode.append_child("voiceTagId").text().set(strVoiceTagId.c_str());
        std::ostringstream oss;
        newEventNode.print(oss);
        VR_LOG("oss: %s", oss.str().c_str());
        OnRecognized(oss.str());
        return;
    }
    VR_AudioStreamIF* pcAudioStream = VR_AudioStreamIF::Instance();
    if (NULL != pcAudioStream) {
        VR_LOG("SetVoiceTag");
        pcAudioStream->SetVoiceTag(true);
        VR_LOG("SetVoiceTag");
    }
    if (VR_ConfigureIF::Instance()->isFileExist(VOICETAGFILE)) {
        bool removeOK = VR_ConfigureIF::Instance()->removeFile(VOICETAGFILE);
        VR_LOG("removeOK : %d", removeOK);
    }
    std::string deviceAddress(eventNode.child("deviceAddress").text().as_string());
    std::string strActionID(eventNode.child("actionId").text().as_string());
    m_strActionID = strActionID;
    m_deviceAddress = deviceAddress;
    m_tempVoiceTagID = eventNode.child("voiceTagId").text().as_string();
    pugi::xml_node contactNode = eventNode.child("contact");
    std::stringstream oss;
    contactNode.print(oss);
    VR_LOG("contactNode : [%s]", oss.str().c_str());
    if (!isUpdate) {
        m_tempVoiceTagID = "-1";
    }
    VR_LOG("Record voiceTagID: [%s]", m_tempVoiceTagID.c_str());
    m_voiceTagContactMsg = oss.str();
    m_isVoiceTagUpdate = isUpdate;
    // Cancel the curent session
    VR_LOG("m_tempVoiceTagID: %s", m_tempVoiceTagID.c_str());
    if (!m_isVoiceTagUpdate) {
        VR_LOG("create voiceTagId");
        m_tempVoiceTagID = VR_VoiceTagIDManager::getInstance()->getVoiceTagID(m_tempVoiceTagID);
    }

    VR_LOG("m_tempVoiceTagID: %s", m_tempVoiceTagID.c_str());
    std::string strStartAgent = "<event name=\"startCurrentAgent\">";
    strStartAgent.append("<agent>voice_tag</agent>");
    strStartAgent.append("</event>");

    OnRequest(strStartAgent);
    VR_LOG("RecordVoiceTagEvent - voice tag: %s", strStartAgent.c_str());
}

void
VR_VoiceBoxEngine::RecordVoiceTagResult(const std::string& tempPronunc)
{
    m_tempPronunc = tempPronunc;

    VR_LOG("m_tempPronunc: %s", m_tempPronunc.c_str());

    VR_AudioStreamIF* pcAudioStream = VR_AudioStreamIF::Instance();
    if (NULL != pcAudioStream) {
        m_tempPcmFile = pcAudioStream->GetTempPcmFile();
        VR_LOG("m_tempPcmFile: %s", m_tempPcmFile.c_str());
    }
    // m_tempPcmFile = VR_ConfigureIF::Instance()->getUsrPath() + VOICETAGFILE;
    // VR_LOG("m_tempPcmFile: %s", m_tempPcmFile.c_str());
    VR_LOG("m_deviceAddress: %s", m_deviceAddress.c_str());
    VR_LOG("m_voiceTagContactMsg: %s", m_voiceTagContactMsg.c_str());
    VR_LOG("m_isVoiceTagUpdate: %d", m_isVoiceTagUpdate);

    pugi::xml_document eventDoc;
    eventDoc.load_string("");
    pugi::xml_node eventNode = eventDoc.append_child("event-result");
    if (!m_isVoiceTagUpdate) {
        eventNode.append_attribute("name").set_value("addRecordVoiceTag");
    }
    else {
        eventNode.append_attribute("name").set_value("editRecordVoiceTag");
    }
    bool recordResult = true;
    if ("" == m_tempPronunc
        || "voice tag too similar" == m_tempPronunc
        || "no speech detected" == m_tempPronunc
        || "voice tag too short" == m_tempPronunc) {
        eventNode.append_attribute("errcode").set_value((int)VR_VoiceTagResult::VOICETAG_FAILURE);
        recordResult = false;
    }
    else {
        VoiceTagInfo voiceTagPara;
        voiceTagPara.voiceTagID = "-2";
        voiceTagPara.pcmPath = m_tempPcmFile;
        voiceTagPara.phoneme = m_tempPronunc;
        voiceTagPara.contactMsg = m_voiceTagContactMsg;
        VR_VoiceBoxVoiceTag voiceTag;
        std::string strGrammar;
        bool result = voiceTag.SaveVoiceTag(strGrammar, m_deviceAddress, voiceTagPara, "true", m_isVoiceTagUpdate);
        if (result) {
            eventNode.append_attribute("errcode").set_value((int)VR_VoiceTagResult::VOICETAG_OK);
        }
        else {
            eventNode.append_attribute("errcode").set_value((int)VR_VoiceTagResult::VOICETAG_FAILURE);
            recordResult = false;
        }
    }
    if (!recordResult) {
        if (VR_ConfigureIF::Instance()->isFileExist(VOICETAGFILE)) {
            bool removeOK = VR_ConfigureIF::Instance()->removeFile(VOICETAGFILE);
            VR_LOG("removeOK : %d", removeOK);
        }
    }
    eventNode.append_child("actionId").text().set(m_strActionID.c_str());
    eventNode.append_child("voiceTagId").text().set(m_tempVoiceTagID.c_str());
        // std::ostringstream oss;
    m_iOss.str("");
    eventNode.print(m_iOss);
        // m_oss = oss;
    VR_LOG("m_iOss: %s", m_iOss.str().c_str());
    OnRecognized(m_iOss.str());

}

void
VR_VoiceBoxEngine::HandleIncomingMessage(const std::string& message)
{
    std::string strMessage = "<action-result agent=\"phone\" op=\"setIncomingMessageInfo\"><instanceId>";
    strMessage.append(m_messageinfo.instanceId);
    strMessage.append("</instanceId><messageId>");
    strMessage.append(m_messageinfo.messageId);
    strMessage.append("</messageId><messageType>");
    strMessage.append(m_messageinfo.messageType);
    strMessage.append("</messageType><subject>");
    strMessage.append(m_messageinfo.subject);
    strMessage.append("</subject><phoneNumber>");
    strMessage.append(m_messageinfo.phoneNumber);
    strMessage.append("</phoneNumber><phoneType>");
    strMessage.append(m_messageinfo.phoneType);
    strMessage.append("</phoneType><contactID>");
    strMessage.append(m_messageinfo.contactID);
    strMessage.append("</contactID></action-result>");
    VR_VoiceBoxXmlParser parserTemp(strMessage);
    m_agents[AgentType_Phone]->ReplyQueryInfo(parserTemp);
    VR_VoiceBoxXmlParser parserBody(message);
    std::string strBody = parserBody.getValueByKey("messagebody");
    m_client->CreateParameterSet(&m_pVariables);
    if (NULL != m_pVariables.ptr()) {
        m_pVariables->AddParameter(_T("body"), strBody.c_str(), NULL, NULL);
        m_pVariables->AddParameter(_T("clienttts"), _T("False"), NULL, NULL);

        if ("" != m_messageinfo.sender) {
            m_pVariables->AddParameter(_T("entry"), m_messageinfo.sender.c_str(), NULL, NULL);
        }
        else if ("" != m_messageinfo.phoneNumber) {
            m_pVariables->AddParameter(_T("entry"), m_messageinfo.phoneNumber.c_str(), NULL, NULL);
        }
        else {
            m_pVariables->AddParameter(_T("entry"), "unknown", NULL, NULL);
        }
        if ("1" == m_messageinfo.messageType) {
            m_pVariables->AddParameter(_T("number"), "", NULL, NULL);
            m_pVariables->AddParameter(_T("sender"), m_messageinfo.phoneNumber.c_str(), NULL, NULL);
        }
        else {
            m_pVariables->AddParameter(_T("number"), m_messageinfo.phoneNumber.c_str(), NULL, NULL);
            m_pVariables->AddParameter(_T("sender"), m_messageinfo.sender.c_str(), NULL, NULL);
        }
    }

    std::string strStartAgent = "<event name=\"startCurrentAgent\">";
    strStartAgent.append("<agent>in_message</agent>");
    strStartAgent.append("</event>");

    OnRequest(strStartAgent);
    VR_LOG("HandleIncomingMessage: %s", strStartAgent.c_str());
}

std::string
VR_VoiceBoxEngine::GetVoiceTagId(pugi::xml_node& contactNode)
{
    VR_LOG("GetVoiceTagId");

    StructNode structNode;
    VoiceVector<StructNode>::type tempVector;
    std::string FullName = contactNode.attribute("name").as_string();
    VR_LOG("FullName: %s", FullName.c_str());
    pugi::xml_node phoneItemNode = contactNode.first_child();

    while (!phoneItemNode.empty()) {
        structNode.Name = FullName;
        std::string number = phoneItemNode.attribute("number").as_string();
        VR_LOG("number: %s", number.c_str());
        structNode.Value = number;
        tempVector.push_back(structNode);
        phoneItemNode = phoneItemNode.next_sibling();
    }
    std::string strVoiceTagId;
    // VoiceVector<PersonInfo>::type m_vecContact = m_pcCatalogPhone->GetContactInfo();
    /* if (0 == m_vecPersonInfo.size()) {
        VR_LOG("m_vecPersonInfo is null");
    } */
    for (size_t i = 0; i < m_vecPersonInfo.size(); ++i) {
        CVECIPtr<IVECIParameterSet> spContact;
        CVECICStr cFullName(&(*m_client));
        PersonInfo perInfo = m_vecPersonInfo[i];

        // cFullName.Format(_T("%s %s"), perInfo.firstName.c_str(), perInfo.lastName.c_str());
        std::string contactName = perInfo.firstName + " " + perInfo.lastName;
        VR_LOG("contactName :%s", contactName.c_str());
        // std::string strFullName = cFullName.Get();
        // VR_LOG("strFullName :%s", strFullName.c_str());
        for (size_t index = 0; index < perInfo.phoneItemVector.size(); ++index) {
            if (!perInfo.phoneItemVector[index].Value.empty()) {
                VR_LOG("perInfo.phoneItemVector[index].Value :%s", perInfo.phoneItemVector[index].Value.c_str());
                for (size_t item = 0; item < tempVector.size(); ++item) {
                    StructNode tempStuct = tempVector[item];
                    VR_LOG("contactName :%s", contactName.c_str());
                    VR_LOG("Name: %s", tempStuct.Name.c_str());
                    VR_LOG("Value: %s", tempStuct.Value.c_str());
                    if (tempStuct.Value == perInfo.phoneItemVector[index].Value) {
                        VR_LOG("number is IsEqual");
                    }
                    VR_LOG("contactName size: %d", contactName.size());
                    VR_LOG("tempStuct.Name size: %d", tempStuct.Name.size());

                    if (contactName == tempStuct.Name) {
                        VR_LOG("name is IsEqual");
                    }
                    if ((contactName == tempStuct.Name) && (tempStuct.Value == perInfo.phoneItemVector[index].Value)) {
                        VR_LOG("finded");
                        VR_LOG("perInfo.strId: %s", perInfo.strId.c_str());
                        strVoiceTagId = perInfo.strId;
                        VR_LOG("strVoiceTagId : %s", strVoiceTagId.c_str());
                        break;
                    }
                }
            }
        }
    }

    return strVoiceTagId;
}

VR_VoiceBoxCatalogManager*
VR_VoiceBoxEngine::GetCatalogManager()
{
    return m_pcCatalogManager;
}

bool
VR_VoiceBoxEngine::OnRecordingVoiceTag(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    VR_LOG("voice traning completed");

    std::string strStartAgent = "<event name=\"startCurrentAgent\">";
    strStartAgent.append("<agent>voice_tag</agent>");
    strStartAgent.append("</event>");

    OnRequest(strStartAgent);
    VR_LOG("voice tag: %s", strStartAgent.c_str());

    return true;
}

void
VR_VoiceBoxEngine::OnRecognized(const std::string& message)
{
    VR_LOGD_FUNC();

    if (NULL == m_pcExternalCallback) {
        // VR_ERROR("The external callback is NULL");
        return;
    }

    VR_LOG("->DM msg : %s", message.c_str());

    if (std::string::npos != message.find("notifyAppXmlMessage")) {
        OnRequestAction(message);
        return;
    }

    if (std::string::npos != message.find("actionId")) {
        OnRequestAction(message);
        return;
    }
    else if (std::string::npos != message.find("action")) {
        VR_LOG("send the action to queue");
        OnRequest(message);
        return;
    }

    if (std::string::npos != message.find("ShowPopupMessage")) {
        pugi::xml_document doc;
        m_strMsgPrompt = "";
        if (doc.load(message.c_str())) {
            m_strMsgPrompt = doc.select_node("//prompt").node().child_value();
            VR_LOG("prompt:%s", m_strMsgPrompt.c_str());
        }
        else {
            VR_LOG("ShowPopUpMessage error");
        }
        OnRequestAction(message);
        return;
    }

    if (std::string::npos != message.find("ScreenDisplay")) {
        pugi::xml_document doc;
        std::string strContent;
        if (doc.load(message.c_str())) {
            m_strScreenState.clear();
            m_strStep = "";
            m_strScreenState = doc.select_node("//content").node().child_value();
            if (InScreen("phone_one_call_contact_message_selected")) {
                std::string name = doc.select_node("//screenType").node().child_value();
                VR_LOG("name: %s", name.c_str());
                if ("Selected Recent Call No Contact Info" == name) {
                    m_isNameNull = true;
                }
                else if ("One Contact Selected" == name) {
                    m_isNameNull = false;
                }
                else {
                }
            }
            else if (InScreen("adaptation")) {
                m_strStep = doc.select_node("//step").node().child_value();
            }
            else if ("na_topmenu_idle" == m_strScreenState) {
                std::string strMessage = "<action-result agent=\"phone\" op=\"QuitVRInit\"/>";
                VR_VoiceBoxXmlParser parserTemp(strMessage);
                m_agents[AgentType_Phone]->ReplyQueryInfo(parserTemp);
            }
            else {
            }
        }

        if (m_bMoreHint && (m_strScreenState != m_strMoreHints)) {
            VR_LOG("this is not more hints state, m_strMoreHints(%s), strContent(%s)", m_strMoreHints.c_str(), strContent.c_str());
            m_bMoreHint = false;
        }

        if ("" != m_strMsgPrompt) {
            m_strDisplayScreen = message;
            return;
        }
        m_strDisplayScreen = "";
    }

    if (std::string::npos != message.find("processing")) {
        if (VR_SessionStatePaused == m_sessionState
            || VR_SessionStateTemporarilyCanceled == m_sessionState
            || VR_SessionStateRestartSessionAfterTTS == m_sessionState
            || VR_SessionStateCanceled == m_sessionState
            || VR_SessionStatePttLong == m_sessionState) {
            return;
        }
        if (VBT_RECO_STATE_VOICE_TAG == m_strCurrentStateName) {
            pugi::xml_document doc;
            if (doc.load(message.c_str())) {
                std::string tempPronunc;
                tempPronunc = doc.select_node("//result").node().child_value();
                VR_LOG("OnRecognized-tempPronunc : %s", tempPronunc.c_str());
                RecordVoiceTagResult(tempPronunc);

                std::string strMsgVoiceTag = "<display agent=\"Common\" content=\"VRState\">\
                <engineType>local</engineType>\
                <state>processing</state>\
                <tag>Voice Control</tag>\
                <prompt />\
                <nbest>false</nbest>\
                <result />\
                <meter>on</meter>\
                </display>";

                OnRequestAction(strMsgVoiceTag);

                return;
            }
        }
    }

    if (InScreen("help_train_voice_recognition")) {
        m_sessionState = VR_SessionStateSpeakAdaptaion;
    }

    // Notify the recognized result to the external service
    // FIXME:
    OnRequestAction(message);
}

// On Info Query Message
void
VR_VoiceBoxEngine::OnInfoQueryMessage(const std::string& message)
{
    VR_LOGD_FUNC();

    if (NULL == m_pcExternalCallback) {
        // VR_ERROR("The external callback is NULL");
        return;
    }

    // Query some information to the external service
    OnRequestAction(message);
}

// Play beep request
void
VR_VoiceBoxEngine::OnBeep(const std::string& message)
{
    VR_LOGD_FUNC();

    if (NULL == m_pcExternalCallback) {
        // VR_ERROR("The external callback is NULL");
        return;
    }
    VR_LOG("message : %s", message.c_str());
    VR_VoiceBoxXmlBuilder  xmlBuilder;
    pugi::xml_node node = xmlBuilder.buildStartActionElement("prompt", "playBeep");
    xmlBuilder.buildGivenElement(node, "beepFile", message, "", "");
    std::string strBeepXml = xmlBuilder.getXmlString();

    VR_LOG("beepFile : %s", strBeepXml.c_str());
    // Play beep request
    m_iPlayVoiceTagId = OnRequestAction(strBeepXml);

    VR_LOG("m_iPlayVoiceTagId: %d", m_iPlayVoiceTagId);
    if (VBT_RECO_STATE_VOICE_TAG == m_strCurrentStateName) {
        m_listPlayVoiceTagSeq.push_back(m_iPlayVoiceTagId);
    }
    m_lastPlayed = BEEP;
}

void
VR_VoiceBoxEngine::OnBeep(const VR_BeepType& beepType)
{
    VR_LOGD_FUNC();

    if (NULL == m_pcExternalCallback) {
        // VR_ERROR("The external callback is NULL");
        return;
    }

    if ((VR_BeepType_Done != beepType)
        && (m_bDoCanceling || m_bDoCancelVoiceTag || m_bDoCancelSession)) {
        VR_LOG("Don't play return beep, when canceled");
        return;
    }

    if ((VR_BeepType_TSL_Done != beepType) && m_bDoCancelTslVR) {
        VR_LOG("Don't play return beep, when TSL VR canceled");
        return;
    }

    std::string strBeepFileName;
    switch (beepType) {
    case VR_BeepType_Listening:
    {
        m_bWaitConfirmation = true;
        if (VR_SessionStateStartWithiBargeIn == m_sessionState) {
            VR_LOG("This is barge in. Don't play start beep.");
            m_sessionState = VR_SessionStateStarting;
            m_iStartBeepID = VR_INVALID_ACTION_ID;
            if (NULL == m_engineCommand.ptr()) {
                VR_LOG("Point is NULL.");
                return;
            }

            HRESULT result = m_engineCommand->BeepDone();
            if (FAILED(result)) {
                VR_LOG("Result is failed.");
                return;
            }
            return;
        }
        strBeepFileName = "startVR.wav";
        m_sessionState = VR_SessionStateStarting;

        if (m_bSpeakOverBeep) {
            VR_AudioStreamIF* pcAudioStream = VR_AudioStreamIF::Instance();
            if (NULL != pcAudioStream) {
                pcAudioStream->StartAudioInWithCache();
            }
        }

        break;
    }

    case VR_BeepType_Confirmation:
    {
        if ((VR_SessionStateStartWithiBargeIn != m_sessionState)
            && (!m_bWaitConfirmation)) {
            VR_LOG("In this case, this beep don't play");
            return;
        }
        VR_LOGP("DE: VBT beep done after recognition... case : 215-3-00 215-4-00");

        m_bWaitConfirmation = false;
        strBeepFileName = "returnVR.wav";
        break;
    }

    case VR_BeepType_Done:
    case VR_BeepType_TSL_Done:
    {
        strBeepFileName = "endVR.wav";
        break;
    }

    default:
    {
        VR_ERROR("No beep type");
        return;
    }
    }

    VR_VoiceBoxXmlBuilder  xmlBuilder;
    pugi::xml_node node = xmlBuilder.buildStartActionElement("prompt", "playBeep");
    std::string strBeepPath = VR_ConfigureIF::Instance()->getDataPath() + "beep/" + strBeepFileName;
    xmlBuilder.buildGivenElement(node, "beepFile", strBeepPath, "", "");
    std::string strBeepXml = xmlBuilder.getXmlString();

    VR_LOG("beepFile : %s", strBeepXml.c_str());

    // Play beep request
    VR_LOGP("request beep : %s", strBeepFileName.c_str());
    int iBeepID = OnRequestAction(strBeepXml);

    if (VR_BeepType_Listening == beepType) {
        m_iStartBeepID = iBeepID;
        VR_LOG("start beep : %d", m_iStartBeepID);
    }
    else if (VR_BeepType_Done == beepType) {
        m_iDoneBeepID = iBeepID;
        VR_LOG("done beep : %d", m_iDoneBeepID);
    }
    else {
    }
}

// Play guidance request
void
VR_VoiceBoxEngine::OnSpeak(IVECITransaction* pcTrans, const std::string& message)
{
    VR_LOGD_FUNC();

    if (NULL == m_pcExternalCallback) {
        // VR_ERROR("The external callback is NULL");
        return;
    }

    if (VR_REGION_US == m_country) {
        VR_LOG("PVR State %d", m_PVRStateCurrent);
        if (VR_PVRState_WaitPlayTTS == m_PVRStateCurrent) {
            VR_LOG("PVR -> play tts");
            m_PVRStateCurrent = VR_PVRState_PlayingTTS;
        }
    }

    // During back canceling, we should not play the prompt
    if (m_bBackInterupted) {
        if (NULL != pcTrans) {
            pcTrans->SpeakDone();
        }
        return;
    }

    std::string strTTSText;
    pugi::xml_document doc;
    if (doc.load(message.c_str())) {
        strTTSText = doc.select_node("//text").node().child_value();
    }

    if ("" == strTTSText) {
        m_bTTSNull = true;
    }
    else {
        m_bTTSNull = false;
    }

    // setting VRState
    OnRequest("<send event=\"sendVRState\"/>");

    if (m_bTTSNull) {
        if (NULL != pcTrans) {
            pcTrans->SpeakDone();
        }
        return;
    }

    VR_LOG("waiting play tts");

    VR_LOG("speak size, %d", m_mapPlayTTSTransation.size());
    // Play guidance request
    m_pcPlayTransation = pcTrans;
    m_pcPlayTransation->AddRef();

    m_iCurTTSReqId = OnRequestAction(message);
    VR_LOG("current speak id, %d", m_iCurTTSReqId);

    if (VBT_RECO_STATE_VOICE_TAG == m_strCurrentStateName) {
        m_listPlayVoiceTagSeq.push_back(m_iCurTTSReqId);
    }
    m_mapPlayTTSTransation.insert(std::make_pair(m_iCurTTSReqId, m_pcPlayTransation));
    VR_LOG("speak size, %d", m_mapPlayTTSTransation.size());

    m_bPlayTTS = true;
    m_lastPlayed = TTS;
}

void
VR_VoiceBoxEngine::OnRequest(const std::string& message)
{
    if (NULL == m_pcMsgController) {
        VR_LOG("VR_VoiceBoxEngine m_pcMsgController is null");
        return;
    }

    m_pcMsgController->PostMessage(message);
}

void
VR_VoiceBoxEngine::OnPlayTone(const std::string& message)
{
    VR_LOGD_FUNC();

    if (std::string::npos != message.find("Done")) {
        OnBeep(VR_BeepType_TSL_Done);
    }
}

// Cancel the current voice recognition session
bool
VR_VoiceBoxEngine::CancelRecoSession()
{
    VR_LOGD_FUNC();

    VR_LOG("current session : %d", m_sessionState);
    if (VR_SessionStateStoped != m_sessionState
        && VR_SessionStateCanceled != m_sessionState
        && VR_SessionStatePaused != m_sessionState
        && VR_SessionStateResume != m_sessionState
        && VR_SessionStateAutoSendRecogState != m_sessionState
        && VR_SessionStateAutoTutoSendRecogState != m_sessionState
        && VR_SessionStateRestartSessionAfterTTS != m_sessionState
        && VR_SessionStateSendRecogAfterTTS != m_sessionState
        && VR_SessionStateStartOver != m_sessionState) {
        m_sessionState = VR_SessionStateTemporarilyCanceled;
    }

    if (m_bStartSessionWithBargeIn) {
        m_bStartSessionWithBargeIn = false;
        m_bBosDetected = false;
        VR_AudioStreamIF* pcAudioStream = VR_AudioStreamIF::Instance();
        if (NULL != pcAudioStream) {
            pcAudioStream->StopAudioIn();
        }

        VR_LOG("Barge-In session is interrupted");
    }

    m_bDoCancelSession = true;

    // Cancel any ASR or TTS session that might be currently running
    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->CancelSession(&transaction);
    if (FAILED(result) || (NULL == transaction.ptr())) {
        VR_ERROR("CancelSession Failed, result: %lx", result);
        return false;
    }

    // Wait until the engine is stopped
    VBT_BOOL bTimeout = VBT_FALSE;
    result = transaction->WaitForCompletion(INFINITE, &bTimeout);
    if (FAILED(result) || bTimeout) {
        VR_ERROR("Waiting Session Cancel Failed, result: %lx", result);
        // return false;
    }

    m_bDoCancelSession = false;

    StopAllTTS();

    return true;
}

void
VR_VoiceBoxEngine::SendVRState(
    const std::string& strState, const std::string& strPrompt,
    const std::string& strNBestScreen, const std::string& strResult)
{
    VR_LOGD_FUNC();
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node root_node = xmlBulder.buildDisplayElement("Common", "VRState");
    xmlBulder.buildGivenElement(root_node, "engineType", "local", "", "");
    xmlBulder.buildGivenElement(root_node, "state", strState, "", "");
    xmlBulder.buildGivenElement(root_node, "tag", "Voice Control", "", "");
    xmlBulder.buildGivenElement(root_node, "prompt", strPrompt, "", "");
    xmlBulder.buildGivenElement(root_node, "nbest", strNBestScreen, "", "");
    xmlBulder.buildGivenElement(root_node, "result", strResult, "", "");
    xmlBulder.buildGivenElement(root_node, "meter", "on", "", "");

    std::string strReply = xmlBulder.getXmlString();

    OnRecognized(strReply);
}

// Puts the VoiceBox Engine into a specific recognition state.
bool
VR_VoiceBoxEngine::SendRecognitionState(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }
    m_bCanceled = false;
    m_bTaskCompelete = false;
    std::string strActionKey = parser.getXmlKey();
    // Get the statename from the XML message
    std::string strStateName = parser.getValueByKey("agent");
    if ("startAgent" == strActionKey) {
        // m_bsession is set true
        m_bSession = true;
        m_bPVRScreen = false;
        SetUpdateGammarFlg(false);

        if (NULL != m_pcCatalogController && !m_bHavePVR) {
            VR_LOG("need to pause %s grammar: %s", m_pairTransaction.first.c_str(), m_pairTransaction.second.c_str());
            m_pcCatalogController->ProcessOpMessage("<op name=\"needpause\"><value>true</value></op>");
        }

        m_bHavePVR = false;

        if ("adaptation" != strStateName) {
            std::string strPrompt;
            GetPromptByScreenId("VR-SB-01", strPrompt);

            SendVRState("idle", strPrompt, "false", "");
        }
    }

    m_listPlayVoiceTagSeq.clear();
    m_bTslDialog = false;
    m_bWaitGreetingEnd = false;
    m_bDoCancelTslVR = false;

    std::string startNativeVR = "<send event=\"startNativeVR\" target=\"de\"/>";
    m_agents[AgentType_Global]->ProcessMessage(startNativeVR);

    VR_LOG("m_isIncomingMessage :%d", m_isIncomingMessage);
    if ("in_message" == strStateName) {
        if (!m_isIncomingMessage) {
            VR_VoiceBoxXmlBuilder xmlBulder;
            pugi::xml_node node = xmlBulder.buildStartActionElement("phone", "getMsgDetailStatusNoChange");
            xmlBulder.buildGivenElement(node, "instanceId", m_messageinfo.instanceId, "", "");
            xmlBulder.buildGivenElement(node, "messageId", m_messageinfo.messageId, "", "");
            std::string strReply = xmlBulder.getXmlString();
            OnRequestAction(strReply);
            m_isIncomingMessage = true;
            return true;
        }
        else {
            m_isIncomingMessage = false;
            std::string strResult = "<event-result name=\"startAgent\" errcode=\"\"><agent>";
            strResult.append(strStateName);
            strResult.append("</agent> </event-result>");
            OnRecognized(strResult);
        }
    }

    VoiceMap<std::string, std::string>::iterator iter = m_mapAgenttoRecoState.find(strStateName);
    if (m_mapAgenttoRecoState.end() != iter) {
        m_strCurrentStateName = iter->second;
    }

    VR_LOG("m_strCurrentStateName :%s", m_strCurrentStateName.c_str());

    if ("startAgent" == strActionKey) {
        if (VR_REGION_US == m_country) {
            VR_LOG("PVR State %d", m_PVRStateCurrent);
            VR_LOG("PVR -> VR");
            m_PVRStateCurrent = VR_PVRState_None;
        }

        m_mapAction.clear();

        VR_LOGP("receive startagent event");
        std::string strResult = "<event-result name=\"startAgent\" errcode=\"\"><agent>";
        strResult.append(strStateName);
        strResult.append("</agent> </event-result>");
        OnRecognized(strResult);

        if (VR_REGION_OC == m_country) {
            if ("in_call" == strStateName) {
                VR_LOG("au-incall");
                VR_VoiceBoxXmlBuilder xmlBulder;
                pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
                xmlBulder.buildGivenElement(xmlNode, "agent", "phone", "", "");
                xmlBulder.buildGivenElement(xmlNode, "content", "sendtone_speak_tones", "", "");
                std::string strReply = xmlBulder.getXmlString();
                VR_LOG("strReply: %s", strReply.c_str());
                OnRecognized(strReply);
            }
        }
    }

    if ("in_call" != strStateName && "voice_tag" != strStateName && "in_message" != strStateName) {
        VR_LOG("get hints");
        // Prepare the hints
        if (!GetHints()) {
            return false;
        }
    }

    return SendRecogState();
}

void
VR_VoiceBoxEngine::AddRecogItem(
    CVECIPtr<IVECIListItems>& optionalContextList,
    const std::string& strAgent,
    const std::string& strContext)
{
    if (NULL == optionalContextList.ptr()) {
        return;
    }

    CVECIPtr<IVECIParameterSet> spOptional;
    m_client->CreateParameterSet(&spOptional);
    if (NULL == spOptional.ptr()) {
        return;
    }

    spOptional->AddParameter(_T("Agent"), strAgent.c_str(), NULL, NULL);
    spOptional->AddParameter(_T("Context"), strContext.c_str(), NULL, NULL);
    optionalContextList->AddItem(spOptional);
}

bool
VR_VoiceBoxEngine::SendRecogState()
{
    CVECIPtr<IVECITransaction> transaction;

    HRESULT result = S_OK;

    if (VBT_RECO_STATE_VOICE_TAG != m_strCurrentStateName) {
        m_bSpeakOverBeep = true;
    }
    else {
        m_bSpeakOverBeep = false;
    }

    if ("Incoming Message" == m_strCurrentStateName) {
        VR_LOG("strStateName: %s", m_strCurrentStateName.c_str());
        result = m_engineCommand->SendRecognitionState(
            &transaction,
            m_strCurrentStateName.c_str(),
            BoolTrue,
            VBT_FALSE,
            m_pVariables,
            NULL);
    }
    else {
        if (NULL == m_client.ptr()) {
            return false;
        }

        CVECIPtr<IVECIListItems> optionalContextList;
        m_client->CreateListItems(&optionalContextList);
        if (NULL == optionalContextList.ptr()) {
            return false;
        }

        SetOptionalContext(optionalContextList, m_strCurrentStateName, true);

        result = m_engineCommand->SendRecognitionState(
            &transaction,
            m_strCurrentStateName.c_str(),
            BoolTrue,
            VBT_FALSE,
            NULL,
            optionalContextList);
    }

    if (FAILED(result) || (NULL == transaction.ptr())) {
        VR_ERROR("SendRecognitionState Failed, result: %lx", result);
        return false;
    }
    VR_LOG("result: %lx", result);

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    if (NULL != strTrans.Get()) {
        m_strCurrentTransactionID = strTrans.Get();
    }

    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnSendRecognitionState
            )
        );
    return true;
}

void
VR_VoiceBoxEngine::SetOptionalContext(
    CVECIPtr<IVECIListItems>& optionalContextList,
    const std::string& strStateName,
    bool bForSendReco)
{
    if (VR_REGION_US == m_country) {
        if (m_bSongInfoAvailable) {
            AddRecogItem(optionalContextList, "Music", "Song Information Available");
        }

        if (m_messageAvailable) {
            AddRecogItem(optionalContextList, "HFD", "Message Global");
        }

        if (("Global" == strStateName) || (std::string::npos != strStateName.find("Home"))) {
            if (m_isActiveFM) {
                AddRecogItem(optionalContextList, "Radio", "Radio Global Active FM");
            }

            if (m_isActiveAM) {
                AddRecogItem(optionalContextList, "Radio", "Radio Global Active AM");
            }

            if (m_isSatellite) {
                AddRecogItem(optionalContextList, "Radio", "Radio Global Satellite");
            }

            if (m_isActiveFMHD) {
                AddRecogItem(optionalContextList, "Radio", "Radio Global Active FM HD");
            }

            if (m_isActiveSatellite) {
                AddRecogItem(optionalContextList, "Radio", "Radio Global Active Satellite");
            }

            if (m_bHVACBasicActive) {
                AddRecogItem(optionalContextList, "HVAC", "HVAC Global Basic Active");
                AddRecogItem(optionalContextList, "HVAC", "HVAC Global Fan Advanced Active");
                AddRecogItem(optionalContextList, "HVAC", "HVAC Global Temp Advanced Active");
                AddRecogItem(optionalContextList, "HVAC", "HVAC Global Temp Basic Active");
            }

            if (m_bHVACFrontScrnActive) {
                AddRecogItem(optionalContextList, "HVAC", "HVAC Global Front Screen Active");
            }
            if (m_bHVACRearScrnActive) {
                AddRecogItem(optionalContextList, "HVAC", "HVAC Global Rear Screen Active");
            }
            if (m_bHVACSteeringScrnActive) {
                AddRecogItem(optionalContextList, "HVAC", "HVAC Global Steering Screen Active");
            }
            if (m_bHVACConcModeActive) {
                AddRecogItem(optionalContextList, "HVAC", "HVAC Global Concierge Mode Active");
            }
            if (m_bHVACSeatActive) {
                AddRecogItem(optionalContextList, "HVAC", "HVAC Global Seat Active");
            }

            if ("Radio Home" == strStateName) {
                if (m_isActiveFM) {
                    AddRecogItem(optionalContextList, "Radio", "Radio Home Active FM");
                }

                if (m_isActiveAM) {
                    AddRecogItem(optionalContextList, "Radio", "Radio Home Active AM");
                }

                if (m_isSatellite) {
                    AddRecogItem(optionalContextList, "Radio", "Radio Home Satellite");
                }

                if (m_isActiveFMHD) {
                    AddRecogItem(optionalContextList, "Radio", "Radio Home Active FM HD");
                }

                if (m_isActiveSatellite) {
                    AddRecogItem(optionalContextList, "Radio", "Radio Home Active Satellite");
                }
            }

            if ("HVAC Home" == strStateName) {
                if (m_bHVACBasicActive) {
                    AddRecogItem(optionalContextList, "HVAC", "HVAC Home Fan Advanced Active");
                    AddRecogItem(optionalContextList, "HVAC", "HVAC Home Fan Basic Active");
                    AddRecogItem(optionalContextList, "HVAC", "HVAC Home Temp Advanced Active");
                    AddRecogItem(optionalContextList, "HVAC", "HVAC Home Temp Basic Active");
                }
            }

            if ((m_bTslNetworkAvailable && m_bTslAppsAvailable) || bForSendReco) {
                if (("Global" == strStateName) || (std::string::npos != strStateName.find("Home"))) {
                    AddRecogItem(optionalContextList, "Apps", "Apps Global");
                }
            }
        }

    }
    else if (VR_REGION_OC == m_country) {
        VR_ConfigureIF* pcConfig = VR_ConfigureIF::Instance();
        if (NULL != pcConfig) {
            if ((VR_PRODUCT_TYPE_L1 != pcConfig->getVRProduct())
                && (!m_bUpdatingMapData)) {
                AddRecogItem(optionalContextList, "Address Entry", "Address Entry Global");
                AddRecogItem(optionalContextList, "Destination", "In Route");
                AddRecogItem(optionalContextList, "Destination", "Destination Global");
                AddRecogItem(optionalContextList, "Destination", "Destination Menu");
                AddRecogItem(optionalContextList, "POI", "POI Global");

            }
        }
        AddRecogItem(optionalContextList, "Music", "Music Global Named Play Playlist");
        AddRecogItem(optionalContextList, "Music", "Music Global Named Play Song");
        AddRecogItem(optionalContextList, "Music", "Music Global Named");
    }
    else {

    }

}

std::string
VR_VoiceBoxEngine::GetAdaptationProfilePath(const std::string& strLanguage, const std::string& strProfileName)
{
    VR_LOGD_FUNC();

    std::string strAdaptationPath = VR_ConfigureIF::Instance()->getUsrPath();
    strAdaptationPath.append(strLanguage);

    VR_LOG("adaptation path : %s", strAdaptationPath.c_str());
    int iMakeDir = mkdir(strAdaptationPath.c_str(), S_IRWXU);
    VR_LOG("make dir %i", iMakeDir);
    // if (VR_ConfigureIF::Instance()->makeDir(strAdaptationPath)) {
    //     VR_LOG("make adaptation path OK");
    // }
    // else {
    //     VR_LOG("make adaptation path ERROR");
    // }
    strAdaptationPath.append("/");
    strAdaptationPath.append(strProfileName);
    strAdaptationPath.append(".spk");
    return strAdaptationPath;
}

bool
VR_VoiceBoxEngine::SaveNextRecognitionState(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    std::string strStateName = parser.getValueByKey("agent");
    VoiceMap<std::string, std::string>::iterator iter = m_mapAgenttoRecoState.find(strStateName);
    if (m_mapAgenttoRecoState.end() != iter) {
        m_strCurrentStateName = iter->second;
    }

    VR_LOG("get hints");
    // Prepare the hints
    if (!GetHints()) {
        return false;
    }

    m_sessionState = VR_SessionStateAutoSendRecogState;
    return true;
}

bool
VR_VoiceBoxEngine::SaveTuToRecognitionState(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    std::string strStateName = parser.getValueByKey("agent");
    VoiceMap<std::string, std::string>::iterator iter = m_mapAgenttoRecoState.find(strStateName);
    if (m_mapAgenttoRecoState.end() != iter) {
        m_strCurrentStateName = iter->second;
    }

    m_sessionState = VR_SessionStateAutoTutoSendRecogState;
    return true;
}

bool
VR_VoiceBoxEngine::StartDictation(VR_VoiceBoxXmlParser& parser)
{
    return false;
}

bool
VR_VoiceBoxEngine::ButtonPressed(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    m_bWaitConfirmation = false;
    std::string strKeyValue = parser.getValueByKey("value");

    VR_LOG("the button event : %s", strKeyValue.c_str());

    std::string strEventResult = "<event-result name=\"buttonPressed\">";
    strEventResult.append(" <keycode value=\"");
    strEventResult.append(strKeyValue);
    strEventResult.append("\"/>");
    strEventResult.append("</event-result>");

    if (VR_SessionStateStoped == m_sessionState && "tutorials" != strKeyValue && "voice_training" != strKeyValue && "setting_voice_training_start" != strKeyValue) {
        VR_LOG("session is stopped!");
        m_strBtnEventName = "";
        OnRecognized(strEventResult);
        return true;
    }

    if (VR_SessionStateBackRestart == m_sessionState) {
        m_bBackInterupted = true;
    }

    if (m_bPlayTTS) {
        if (NULL == m_pcExternalCallback) {
            OnRecognized(strEventResult);
            return false;
        }
        VR_LOG("playing tts. stop playing");
        m_bPlayTTS = false;
        std::string strStopTTS = "<action agent=\"prompt\" op=\"stopTts\"><reqId>";
        strStopTTS.append(std::to_string(m_iCurTTSReqId));
        strStopTTS.append("</reqId></action>");

        int iStopID = OnRequestAction(strStopTTS);
        VR_LOG("stop current speak id, %d", m_iCurTTSReqId);
        m_mapStopTTSTransation.insert(std::make_pair(iStopID, m_iCurTTSReqId));

        m_strBtnEventName = strKeyValue;
    }
    else {
        VR_LOG("no tts , handle");
        // Dispatch the messages
        VoiceMap<std::string, MessageHandler>::const_iterator iterMap = m_mapMsgBtnHandler.find(strKeyValue);
        if (m_mapMsgBtnHandler.cend() != iterMap) {
            if (NULL != iterMap->second) {
                VR_LOG("btn pressed need to cancel session");
                if (("help" != strKeyValue) && ("setting_voice_training_start" != strKeyValue)) {
                    CancelRecoSession();
                }
                VR_LOG("handle the event");
                m_bTTSNull = true;
                (this->*(iterMap->second))(parser);
            }
        }
        m_strBtnEventName = "";
    }

    OnRecognized(strEventResult);
    return true;
}

bool
VR_VoiceBoxEngine::UpdateState(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand) {
        return false;
    }

    if (m_bInstallingAgent) {
        m_listUpdateState.push_back(parser.getXmlString());
        return true;
    }

    VoiceVector<StructNode>::type updateVector = parser.getVectorInfo();
    std::string strKeyValue;

    bool bUsbOrIpodInfo = false;

    for (size_t i = 1; i < updateVector.size(); i = i + 2) {
        bUsbOrIpodInfo = false;
        VR_LOG("updateVector.Value: %s", updateVector[i].Value.c_str());
        VR_LOG("updateVector.Value: %s", updateVector[i + 1].Value.c_str());
        if ("PHONE_STATE_CONNECTED" == updateVector[i].Value) {
            VR_LOG("updateVector[i].Value   come on");
            VR_LOG("updateVector[i].Value: %s", updateVector[i].Value.c_str());
            strKeyValue = updateVector[i + 1].Value;
            VR_LOG("strKeyValue: %s", strKeyValue.c_str());
            if ("disconnected" == strKeyValue) {
                VR_LOG("strKeyValue: %s", strKeyValue.c_str());
                if (NULL != m_pcCatalogController) {
                    m_pcCatalogController->ProcessOpMessage("<op name=\"deletephonebuf\"></op>");
                }
                if (NULL != m_pcMsgController) {
                    m_pcMsgController->PostMessage("<grammar_disactive agent=\"phone\" sender=\"DE\"></grammar_disactive>");
                }
                CVECIPtr<IVECITransaction> transaction;
                HRESULT result = m_engineCommand->SetPreference(
                                 &transaction,
                                 _T("System"),
                                 VBT_USR_PREF_SYSTEM_PHONECONNECTED,
                                 "False");
                if (FAILED(result) || (NULL == transaction.ptr())) {
                    return false;
                }
                m_messageAvailable = false;
                m_strMsgAvailable = false;
                result = m_engineCommand->SetPreference(
                         &transaction,
                         _T("HFD"),
                         VBT_USR_PREF_HFD_SMSAVAILABLE,
                         "False");
                if (FAILED(result) || (NULL == transaction.ptr())) {
                    return false;
                }
                VR_LOG("m_messageAvailable is  false");

                result = m_engineCommand->SaveSpeakerProfile(
                         &transaction,
                         m_strAdaptationPath.c_str());
                if (FAILED(result) || (NULL == transaction.ptr())) {
                    return false;
                }
                result = m_engineCommand->ResetSpeakerProfile(
                         &transaction);
                if (FAILED(result) || (NULL == transaction.ptr())) {
                    return false;
                }
                break;
            }
            else if ("connected" == strKeyValue) {
                VR_LOG("strKeyValue: %s", strKeyValue.c_str());
                CVECIPtr<IVECITransaction> transaction;
                HRESULT result = m_engineCommand->SetPreference(
                                 &transaction,
                                 _T("System"),
                                 VBT_USR_PREF_SYSTEM_PHONECONNECTED,
                                 "True");
                if (FAILED(result) || (NULL == transaction.ptr())) {
                    return false;
                }
            }
            else {
                if (NULL != m_pcMsgController) {
                    m_pcMsgController->PostMessage("<grammar_disactive agent=\"phone\" sender=\"DE\"></grammar_disactive>");
                }
            }

        }
        if ("PHONE_DEVICE_ID" == updateVector[i].Value) {
            strKeyValue = updateVector[i + 1].Value;
            VR_LOG("strKeyValue: %s", strKeyValue.c_str());
            long int id = std::strtol(strKeyValue.c_str(), NULL, 10);
            std::ostringstream oss;
            oss << id;
            strKeyValue = oss.str();

            VR_LOG("strKeyValue: %s", strKeyValue.c_str());
            if ("0" != strKeyValue) {
                VR_VoiceBoxVoiceTag voiceTag;
                voiceTag.CheckTempVoiceTag(strKeyValue);
                if (NULL != m_pcCatalogPhone) {
                    m_pcCatalogPhone->SetDeviceAddress(strKeyValue);
                }
            }
        }
        if ("PHONE_STATE_REGISTERED" == updateVector[i].Value) {
            if (VR_REGION_OC == m_country) {
                strKeyValue = updateVector[i + 1].Value;
                VR_LOG("register: %s", strKeyValue.c_str());
                CVECIPtr<IVECITransaction> transaction;
                if ("0" == strKeyValue) {
                    HRESULT result = m_engineCommand->SetPreference(
                                     &transaction,
                                     _T("System"),
                                     _T("PhoneRegistered"),
                                     "False");
                    if (FAILED(result) || (NULL == transaction.ptr())) {
                        return false;
                    }
                }
                else {
                    HRESULT result = m_engineCommand->SetPreference(
                                     &transaction,
                                     _T("System"),
                                     _T("PhoneRegistered"),
                                     "True");
                    if (FAILED(result) || (NULL == transaction.ptr())) {
                        return false;
                    }
                }
            }
        }
        if ("PHONE_STATE_MESSAGE_AVAILABLE" == updateVector[i].Value) {
            strKeyValue = updateVector[i + 1].Value;
            VR_LOG("strKeyValue: %s", strKeyValue.c_str());
            if ("false" == updateVector[i+1].Value) {
                m_strMsgAvailable = false;
                m_messageAvailable = true;
                CVECIPtr<IVECITransaction> transaction;
                HRESULT result = m_engineCommand->SetPreference(
                                 &transaction,
                                 _T("HFD"),
                                 VBT_USR_PREF_HFD_SMSAVAILABLE,
                                 "False");
                if (FAILED(result) || (NULL == transaction.ptr())) {
                    return false;
                }
                VR_LOG("m_messageAvailable is  false");
            }
            else if ("true" == updateVector[i+1].Value) {
                m_strMsgAvailable = true;
                m_messageAvailable = true;
                CVECIPtr<IVECITransaction> transaction;
                HRESULT result = m_engineCommand->SetPreference(
                                 &transaction,
                                 _T("HFD"),
                                 VBT_USR_PREF_HFD_SMSAVAILABLE,
                                 "True");
                if (FAILED(result) || (NULL == transaction.ptr())) {
                    return false;
                }
                VR_LOG("m_messageAvailable is  true");
            }
        }
        if ("PHONE_STATE_INCALL" == updateVector[i].Value) {
            strKeyValue = updateVector[i + 1].Value;
            if ("true" == strKeyValue) {
                // VR_LOG("UpdateState: <event name=\"startNextAgent\"><agent>in_call</agent></event>");
                // OnRequest("<event name=\"startNextAgent\"><agent>in_call</agent></event>");
            }
        }
        if ("STEERING_PHONEKEY_TYPE" == updateVector[i].Value) {
            if (VR_REGION_OC == m_country) {
                strKeyValue = updateVector[i + 1].Value;
                VR_LOG("strKeyValue : %s", strKeyValue.c_str());
                if ("TEL" == strKeyValue) {
                    CVECIPtr<IVECITransaction> transaction;
                    HRESULT result = m_engineCommand->SetPreference(
                                     &transaction,
                                     _T("HFD"),
                                     _T("PhoneOffHook"),
                                     "False");
                    if (FAILED(result) || (NULL == transaction.ptr())) {
                        VR_LOG("set phone hook error");
                    }
                }
                 else if ("OFFHOOK" == strKeyValue) {
                    CVECIPtr<IVECITransaction> transaction;
                    HRESULT result = m_engineCommand->SetPreference(
                                     &transaction,
                                     _T("HFD"),
                                     _T("PhoneOffHook"),
                                     "True");
                    if (FAILED(result) || (NULL == transaction.ptr())) {
                        VR_LOG("set phone hook error");
                    }
                }
                else {
                    VR_LOG("invaid parameter");
                }
            }
        }
        if ("OPERESTRICT" == updateVector[i].Value) {
            if (VR_REGION_OC != m_country) {
                if ("on" == updateVector[i+1].Value) {
                    if (VBT_RECO_STATE_SPEAKER_ADAPTATION == m_strCurrentStateName) {
                        StopTraining();
                    }
                }
            }
        }
        if ("RADIO_STATE_FM_ACTIVE" == updateVector[i].Value) {
            m_isActiveFM = ("true" == updateVector[i+1].Value) ? true : false;
        }
        if ("RADIO_STATE_AM_ACTIVE" == updateVector[i].Value) {
            m_isActiveAM = ("true" == updateVector[i+1].Value) ? true : false;
        }
        if ("RADIO_XM_CONNECTED" == updateVector[i].Value) {
            m_isSatellite = ("true" == updateVector[i+1].Value) ? true : false;
        }
        if ("RADIO_STATE_FM_HD_ACTIVE" == updateVector[i].Value) {
            m_isActiveFMHD = ("true" == updateVector[i+1].Value) ? true : false;
        }
        if ("RADIO_STATE_XM_ACTIVE" == updateVector[i].Value) {
            m_isActiveSatellite = ("true" == updateVector[i+1].Value) ? true : false;
        }
        if ("USB_1_CONNECTED" == updateVector[i].Value) {
            m_bUsb1 = ("true" == updateVector[i+1].Value) ? true : false;
            bUsbOrIpodInfo = true;
        }
        if ("USB_2_CONNECTED" == updateVector[i].Value) {
            m_bUsb2 = ("true" == updateVector[i+1].Value) ? true : false;
            bUsbOrIpodInfo = true;
        }
        if ("IPOD_1_CONNECTED" == updateVector[i].Value) {
            m_bIpod1 = ("true" == updateVector[i+1].Value) ? true : false;
            bUsbOrIpodInfo = true;
        }
        if ("IPOD_2_CONNECTED" == updateVector[i].Value) {
            m_bIpod2 = ("true" == updateVector[i+1].Value) ? true : false;
            bUsbOrIpodInfo = true;
        }
        if ("BTAUDIO_CONNECTED" == updateVector[i].Value) {
            m_bBtAudio = ("true" == updateVector[i+1].Value) ? true : false;
        }
        if ("CLIMATE_FANSPEED_MAX" == updateVector[i].Value) {
            CVECIPtr<IVECITransaction> transaction;
            HRESULT result = m_engineCommand->SetPreference(
                &transaction, _T("HVAC"),
                VBT_USR_PREF_HVAC_MAXFANSPEED,
                updateVector[i+1].Value.c_str());
            if (FAILED(result) || (NULL == transaction.ptr())) {
                return false;
            }
        }
        if ("MUSIC_SONG_INFORMATION_AVAILABLE" == updateVector[i].Value) {
            m_bSongInfoAvailable = ("true" == updateVector[i+1].Value) ? true : false;
        }
        if ("MEDIAINFOKEY_MAGICSTRING" == updateVector[i].Value) {
            VR_LOG("GraceNoteMagicNumber = %s", updateVector[i+1].Value.c_str());
            CVECIPtr<IVECITransaction> transaction;
            HRESULT result = m_engineCommand->SetPreference(
                &transaction, _T("System"),
                VBT_SYS_PREF_SPEECHANDSOUND_GRACENOTEMAGICNUMBER,
                updateVector[i+1].Value.c_str());
            if (FAILED(result) || (NULL == transaction.ptr())) {
                return false;
            }
        }
        if ("TSL_STATE_AVAILABLE" == updateVector[i].Value) {
            VR_LOG("TSL Network Available = %s", updateVector[i+1].Value.c_str());
            if ("true" == updateVector[i+1].Value) {
                m_bTslNetworkAvailable = true;
                if (m_bTslAppsAvailable) {
                    CVECIPtr<IVECITransaction> transaction;
                    HRESULT result = m_engineCommand->SetPreference(
                                     &transaction,
                                     VBT_AGENT_APPS,
                                     VBT_USR_PREF_APPS_APPSAVAILABLE,
                                     "True");
                    if (FAILED(result) || (NULL == transaction.ptr())) {
                        return false;
                    }
                }
            }
            else {
                m_bTslNetworkAvailable = false;
                CVECIPtr<IVECITransaction> transaction;
                HRESULT result = m_engineCommand->SetPreference(
                                 &transaction,
                                 VBT_AGENT_APPS,
                                 VBT_USR_PREF_APPS_APPSAVAILABLE,
                                 "False");
                if (FAILED(result) || (NULL == transaction.ptr())) {
                    return false;
                }
            }
        }
        if ("TSL_APPS_AVAILABLE" == updateVector[i].Value) {
            VR_LOG("TSL Apps Available = %s", updateVector[i+1].Value.c_str());
            if ("true" == updateVector[i+1].Value) {
                m_bTslAppsAvailable = true;
                if (m_bTslNetworkAvailable) {
                    CVECIPtr<IVECITransaction> transaction;
                    HRESULT result = m_engineCommand->SetPreference(
                                     &transaction,
                                     VBT_AGENT_APPS,
                                     VBT_USR_PREF_APPS_APPSAVAILABLE,
                                     "True");
                    if (FAILED(result) || (NULL == transaction.ptr())) {
                        return false;
                    }
                }
            }
            else {
                m_bTslAppsAvailable = false;
                CVECIPtr<IVECITransaction> transaction;
                HRESULT result = m_engineCommand->SetPreference(
                                 &transaction,
                                 VBT_AGENT_APPS,
                                 VBT_USR_PREF_APPS_APPSAVAILABLE,
                                 "False");
                if (FAILED(result) || (NULL == transaction.ptr())) {
                    return false;
                }
            }
        }
        if ("CLIMATE_BASIC_ACTIVE" == updateVector[i].Value) {
            m_bHVACBasicActive = ("true" == updateVector[i+1].Value) ? true : false;
        }
        if ("CLIMATE_FRONT_SCREEN_ACTIVE" == updateVector[i].Value) {
            m_bHVACFrontScrnActive = ("true" == updateVector[i+1].Value) ? true : false;
        }
        if ("CLIMATE_REAR_SCREEN_ACTIVE" == updateVector[i].Value) {
            m_bHVACRearScrnActive = ("true" == updateVector[i+1].Value) ? true : false;
        }
        if ("CLIMATE_STEERING_SCREEN_ACTIVE" == updateVector[i].Value) {
            m_bHVACSteeringScrnActive = ("true" == updateVector[i+1].Value) ? true : false;
        }
        if ("CLIMATE_LEXUS_CONCIERGE_SCREEN_ACTIVE" == updateVector[i].Value) {
            m_bHVACConcModeActive = ("true" == updateVector[i+1].Value) ? true : false;
        }
        if ("CLIMATE_CONCIERGE_ACTIVE" == updateVector[i].Value) {
            m_bHVACConcModeActive = ("true" == updateVector[i+1].Value) ? true : false;
        }
        if ("CLIMATE_FRONT_SEAT_SCREEN_ACTIVE" == updateVector[i].Value) {
            m_bHVACSeatActive = ("true" == updateVector[i+1].Value) ? true : false;
        }
        if ("CLIMATE_REAR_SEAT_SCREEN_ACTIVE" == updateVector[i].Value) {
            m_bHVACSeatActive = ("true" == updateVector[i+1].Value) ? true : false;
        }
        if ("CLIMATE_SEAT_OPERATION_SCREEN_ACTIVE" == updateVector[i].Value) {
            m_bHVACSeatActive = ("true" == updateVector[i+1].Value) ? true : false;
        }
    }

    if (bUsbOrIpodInfo) {
        std::string strUsbOrIpodConnected;
        if (m_bUsb1 || m_bUsb2 || m_bIpod1 || m_bIpod2) {
            VR_LOG("has usb or ipod connected");
            strUsbOrIpodConnected = "True";
        }
        else {
            VR_LOG("has not usb or ipod connected");
            strUsbOrIpodConnected = "False";
        }

        if (strUsbOrIpodConnected != m_strUsbOrIpodConnected) {
            if ("False" == strUsbOrIpodConnected) {
                CVECIPtr<IVECITransaction> transaction1;
                HRESULT result = m_engineCommand->SetPreference(&transaction1, _T("Music"),
                    VBT_USR_PREF_MUSIC_AUDIOSOURCECONNECTED, strUsbOrIpodConnected.c_str());
                if (FAILED(result) || (NULL == transaction1.ptr())) {
                    return false;
                }
            }

            m_strUsbOrIpodConnected = strUsbOrIpodConnected;
        }
    }

    std::string strEventResult = "<event-result name=\"updateState\">";
    for (size_t i = 1; i < updateVector.size(); i = i + 2) {
        strEventResult.append(" <item key=\"");
        strEventResult.append(updateVector[i].Value);
        strEventResult.append("\" value= \"");
        strEventResult.append(updateVector[i+1].Value);
        strEventResult.append("\"/>");
    }
    strEventResult.append("</event-result>");
    VR_LOG("strEventResult: %s", strEventResult.c_str());
    OnRecognized(strEventResult);
    VR_LOG("updateEnd");
    return true;
}

void
VR_VoiceBoxEngine::SetSpeakerProfile(const std::string& strKeyValue)
{
    VR_LOGD_FUNC();

    m_strDevice.clear();
    m_strAdaptationPath.clear();
    m_strDevice = strKeyValue;
    m_strAdaptationPath = GetAdaptationProfilePath(m_strCultureName, m_strDevice);
    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->LoadSpeakerProfile(
                     &transaction,
     m_strAdaptationPath.c_str());
    if (FAILED(result) || (NULL == transaction.ptr())) {
        result = m_engineCommand->ResetSpeakerProfile(
                 &transaction);
        if (FAILED(result) || (NULL == transaction.ptr())) {
            return;
        }
    }

}

bool
VR_VoiceBoxEngine::ChangeSettings(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();
    VoiceVector<StructNode>::type settingVector = parser.getVectorInfo();
    std::string strKeyValue;
    for (size_t i = 1; i < settingVector.size(); i = i + 2) {
        VR_LOG("updateVector.Value: %s", settingVector[i].Value.c_str());
        VR_LOG("updateVector.Value: %s", settingVector[i + 1].Value.c_str());
        if ("promptLevel" == settingVector[i].Value) {
            VR_LOG("updateVector[i].Value   come on");
            VR_LOG("updateVector[i].Value: %s", settingVector[i].Value.c_str());
            strKeyValue = settingVector[i + 1].Value;
            SavePromptLevel(strKeyValue);
            VR_LOG("strKeyValue: %s", strKeyValue.c_str());
            if ("OFF" == strKeyValue) {
                VR_LOG("strKeyValue: %s", strKeyValue.c_str());
                setPromptLevel(VR_PROMPTLEVEL_OFF);
            }
            else if ("SIMPLE" == strKeyValue) {
                VR_LOG("strKeyValue: %s", strKeyValue.c_str());
                setPromptLevel(VR_PROMPTLEVEL_LOW);
            }
            else if ("FULL" == strKeyValue) {
                VR_LOG("strKeyValue: %s", strKeyValue.c_str());
                setPromptLevel(VR_PROMPTLEVEL_HIGH);
            }
        }
    }

    std::string strEventResult = "<event-result name=\"changeSettings\">";
    for (size_t i = 1; i < settingVector.size(); i = i + 2) {
        strEventResult.append(" <param name=\"");
        strEventResult.append(settingVector[i].Value);
        strEventResult.append("\" value= \"");
        strEventResult.append(settingVector[i+1].Value);
        strEventResult.append("\"/>");
    }
    strEventResult.append("</event-result>");
    VR_LOG("strEventResult: %s", strEventResult.c_str());
    OnRecognized(strEventResult);
    VR_LOG("changeSettings");
    return false;
}

bool
VR_VoiceBoxEngine::ChangeLanguage(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();
    if (NULL == m_engineCommand) {
        return false;
    }

    std::string strLanguage = parser.getValueByKey("language");
    if ("en-us" == strLanguage || "fr-ca" == strLanguage || "es-mx" == strLanguage) {
        m_lstLanguage.push_back(strLanguage);
    }
    else {
        SendChangeLanguageResult(strLanguage);
        return false;
    }

    if (!m_bEngineStarting) {
        VR_LOG("engine isn't started");
        return true;
    }

    if (NULL != m_pcCatalogManager && !m_pairTransaction.first.empty() && !m_pairTransaction.second.empty()) {
        m_pcCatalogManager->CancelGrammarUpdate(m_pairTransaction.first);
    }

    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->SaveSpeakerProfile(
                     &transaction,
                     m_strAdaptationPath.c_str());
    if (FAILED(result) || (NULL == transaction.ptr())) {
        VR_LOG("SaveSpeakerProfile failed");
    }

    CancelRecoSession();

    m_strLanguage = strLanguage;
    m_bChangeLanguage = true;

    if ("en-us" == strLanguage) {
        Restart("en-US");
    }
    else if ("fr-ca" == strLanguage) {
        Restart("fr-CA");
    }
    else if ("es-mx" == strLanguage) {
        Restart("es-MX");
    }
    else {

    }

    std::string notifyChangeLanguage = "<action-result agent=\"phone\" op=\"changeLanguage\"><language>";
    notifyChangeLanguage.append(strLanguage);
    notifyChangeLanguage.append("</language></action-result>");
    VR_VoiceBoxXmlParser tmpParser(notifyChangeLanguage);
    m_agents[AgentType_Phone]->ReplyQueryInfo(tmpParser);

    return false;
}

bool
VR_VoiceBoxEngine::ChangeLanguageInner(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand) {
        return false;
    }

    std::string strLanguage = parser.getValueByKey("language");
    m_strLanguage = strLanguage;
    m_bChangeLanguage = true;

    if ("en-us" == strLanguage) {
        Restart("en-US");
    }
    else if ("fr-ca" == strLanguage) {
        Restart("fr-CA");
    }
    else if ("es-mx" == strLanguage) {
        Restart("es-MX");
    }
    else {

    }

    std::string notifyChangeLanguage = "<action-result agent=\"phone\" op=\"changeLanguage\"><language>";
    notifyChangeLanguage.append(strLanguage);
    notifyChangeLanguage.append("</language></action-result>");
    VR_VoiceBoxXmlParser tmpParser(notifyChangeLanguage);
    m_agents[AgentType_Phone]->ReplyQueryInfo(tmpParser);
    return true;
}

bool
VR_VoiceBoxEngine::FullUpdateNotify(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    std::string strStatus = parser.getValueByKey("status");
    VR_LOG("status : %s", strStatus.c_str());

    std::string strEventResult = "<event-result name=\"fullupdateNotify\">";
    strEventResult.append("<status>");
    strEventResult.append(strStatus);
    strEventResult.append("</status>");
    strEventResult.append("</event-result>");
    OnRecognized(strEventResult);

    if (VR_REGION_OC != m_country) {
        VR_LOG("country is error");
        return false;
    }

    if ("off" == strStatus) {
        m_bUpdatingMapData = false;
    }
    else if ("navifulldata" == strStatus) {
        m_bUpdatingMapData = true;

        if ((VR_SessionStateStoped != m_sessionState)
            && (VR_SessionStateNone != m_sessionState)
            && (VR_INVALID_ACTION_ID == m_iDoneBeepID)) {
            StopAllTTS();
            OnBeep(VR_BeepType_Done);

            CancelRecoSession();
        }

        OnRequest("<grammar_delete agent=\"poi\" />");
    }
    else if ("finished" == strStatus) {
        m_bUpdatingMapData = false;

        if ((VR_SessionStateStoped != m_sessionState)
            && (VR_SessionStateNone != m_sessionState)
            && (VR_INVALID_ACTION_ID == m_iDoneBeepID)) {
            StopAllTTS();
            OnBeep(VR_BeepType_Done);

            CancelRecoSession();
        }

        VR_ConfigureIF* pcConfig = VR_ConfigureIF::Instance();
        std::string strLanguage = pcConfig->getVRLanguage();

        if (VR_LANGUAGE_EN_AU == strLanguage) {
            Restart("en-AU");
        }
    }
    else {
        VR_LOG("status is out of our choose");
        return false;
    }

    return true;
}

bool
VR_VoiceBoxEngine::GetResourceState(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    std::string strState = "0";

    if (m_bDoCanceling) {
        VR_LOG("Canceling");
        strState = "2";
    }
    // else if (bAddingGrammar && !m_bEngineStarting) {
    //     // VR_LOG("adding grammar and engine is stoped");
    //     VR_LOG("engine is stoped");
    //     strState = "3";
    // }
    else if (!m_bEngineStarting) {
        VR_LOG("engine is stoped");
        strState = "2";
    }
    else if (m_bInstallingAgent) {
        VR_LOG("engine is installing agents");
        strState = "2";
    }
    else {
        VR_LOG("state is OK");
    }

    std::string strEventResult = "<event-result name=\"getResourceState\">";
    strEventResult.append("<state>");
    strEventResult.append(strState);
    strEventResult.append("</state>");
    strEventResult.append("</event-result>");

    OnRecognized(strEventResult);
    return true;
}

bool
VR_VoiceBoxEngine::PttShort(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    VR_LOG("current session : %d", m_sessionState);
    VoiceList<std::string>::iterator iter = std::find(m_lstNeedReactiveAction.begin(), m_lstNeedReactiveAction.end(), m_strActionType);
    if (VR_SessionStatePaused == m_sessionState) {
        Resume(parser);
    }
    else if (VR_SessionStateResume == m_sessionState) {
        StartSession();
    }
    else if (VR_SessionStateRestartSessionAfterTTS == m_sessionState) {
        StartSession();
    }
    else if (VR_SessionStateAutoSendRecogState == m_sessionState) {
        VR_LOG("menu clicked, stop tts. when session over, send recog state");
    }
    else if (VR_SessionStateAutoTutoSendRecogState == m_sessionState) {
        if (!GetHints()) {
            return false;
        }

        SendRecogState();

        m_sessionState = VR_SessionStateTemporarilyCanceled;
    }
    else if (VR_SessionStateBackRestart == m_sessionState) {
        VR_LOG("back wait for startrecosession");
    }
    else if (m_bTaskCompelete && iter == m_lstNeedReactiveAction.end()) {
        VR_LOG("need to quit VR");
    }
    else if (VR_SessionStateSendRecogAfterTTS == m_sessionState || VR_SessionStateStartOver == m_sessionState) {
        VR_LOG("need to display hint");
    }
    else {
        m_sessionState = VR_SessionStateAutoRestartSession;
    }

    return true;
}

bool
VR_VoiceBoxEngine::PttLong(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    SendVRState("failed", "", "false", "");

    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->SendCommand(
                     &transaction,
                     _T("System"),
                     VBT_COMMAND_SYSTEM_CANCEL,
                     VBT_FALSE,
                     NULL,
                     NULL);
    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    m_mapCmdResultHandler.insert(
            std::make_pair(
                strTrans.Get(),
                &VR_VoiceBoxEngine::OnPttLong
                )
            );
    m_sessionState = VR_SessionStatePttLong;

    return true;
}

bool
VR_VoiceBoxEngine::EntryNormalPress(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->SendCommand(
                     &transaction,
                     _T("System"),
                     VBT_COMMAND_SYSTEM_START_OVER,
                     VBT_FALSE,
                     NULL,
                     NULL);
    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    m_mapCmdResultHandler.insert(
            std::make_pair(
                strTrans.Get(),
                &VR_VoiceBoxEngine::OnStartOver
                )
            );

    return true;
}

bool
VR_VoiceBoxEngine::BackNormalPress(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    SendVRState("failed", "", "false", "");

    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->SendCommand(
                     &transaction,
                     _T("System"),
                     VBT_COMMAND_SYSTEM_CANCEL,
                     VBT_FALSE,
                     NULL,
                     NULL);
    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    m_mapCmdResultHandler.insert(
            std::make_pair(
                strTrans.Get(),
                &VR_VoiceBoxEngine::OnPttLong
                )
            );
    m_sessionState = VR_SessionStatePttLong;

    return true;
}

bool
VR_VoiceBoxEngine::SelectOne(VR_VoiceBoxXmlParser& parser)
{
    return Select(0);
}


bool
VR_VoiceBoxEngine::SelectTwo(VR_VoiceBoxXmlParser& parser)
{
    return Select(1);
}


bool
VR_VoiceBoxEngine::SelectThree(VR_VoiceBoxXmlParser& parser)
{
    return Select(2);
}


bool
VR_VoiceBoxEngine::SelectFour(VR_VoiceBoxXmlParser& parser)
{
    return Select(3);
}


bool
VR_VoiceBoxEngine::SelectFive(VR_VoiceBoxXmlParser& parser)
{
    return Select(4);
}

bool
VR_VoiceBoxEngine::Select(int iIndex)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->SelectListItem(
                     &transaction,
                     (VBT_ULONG)iIndex);
    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    VR_LOG("current string : %s", strTrans.Get());
    m_mapCmdResultHandler.insert(
            std::make_pair(
                strTrans.Get(),
                &VR_VoiceBoxEngine::OnSelect
                )
            );
    return true;
}


bool
VR_VoiceBoxEngine::Phone(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    std::string strStartAgent = "<event name=\"startCurrentAgent\">";
    strStartAgent.append("<agent>phone</agent>");
    strStartAgent.append("</event>");

    OnRequest(strStartAgent);
    return true;
}

bool
VR_VoiceBoxEngine::Navigation(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    std::string strStartAgent = "<event name=\"startCurrentAgent\">";
    strStartAgent.append("<agent>navi</agent>");
    strStartAgent.append("</event>");

    OnRequest(strStartAgent);
    return true;
}

bool
VR_VoiceBoxEngine::Apps(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    std::string strStartAgent = "<event name=\"startCurrentAgent\">";
    strStartAgent.append("<agent>apps</agent>");
    strStartAgent.append("</event>");

    OnRequest(strStartAgent);
    return true;
}

bool
VR_VoiceBoxEngine::Audio(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();
    VR_LOGP("DE: start go to audio button command...case : 215-6");


    std::string strStartAgent = "<event name=\"startCurrentAgent\">";
    strStartAgent.append("<agent>music</agent>");
    strStartAgent.append("</event>");

    OnRequest(strStartAgent);
    return true;
}

bool
VR_VoiceBoxEngine::Info(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    std::string strStartAgent = "<event name=\"startCurrentAgent\">";
    strStartAgent.append("<agent>information</agent>");
    strStartAgent.append("</event>");

    OnRequest(strStartAgent);
    return true;
}

bool
VR_VoiceBoxEngine::Climate(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    OnRequest("<event name=\"startCurrentAgent\"> <agent>climate</agent> </event>");

    return true;
}

bool
VR_VoiceBoxEngine::SpeakAdaptation(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    std::string strStartAgent = "<event name=\"startCurrentAgent\">";
    strStartAgent.append("<agent>adaptation</agent>");
    strStartAgent.append("</event>");

    OnRequest(strStartAgent);
    VR_LOG("speak adaptation: %s", strStartAgent.c_str());

    return true;
}

bool
VR_VoiceBoxEngine::HandlePause(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->SendCommand(
                     &transaction,
                     _T("System"),
                     VBT_COMMAND_SYSTEM_PAUSE,
                     VBT_FALSE,
                     NULL,
                     NULL);
    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }
    m_sessionState = VR_SessionStatePaused;

    SendVRState("paused", "", "false", "");

    if (NULL != strTrans.Get()) {
        m_strCurrentTransactionID = strTrans.Get();
    }
    return true;
}

bool
VR_VoiceBoxEngine::Pause(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }
    m_sessionState = VR_SessionStatePaused;

    return true;
}

bool
VR_VoiceBoxEngine::Resume(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->Resume(
                     &transaction,
                     VBT_TRUE);
    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    if (NULL != strTrans.Get()) {
        m_strCurrentTransactionID = strTrans.Get();
        m_lstTransaction.push_front(m_strCurrentTransactionID);
    }

    m_sessionState = VR_SessionStateResume;
    VR_LOG("resume strTrans: %s", strTrans.Get());
    m_mapCmdResultHandler.insert(
            std::make_pair(
                strTrans.Get(),
                &VR_VoiceBoxEngine::OnStartRecoSession
                )
            );
    return true;
}

bool
VR_VoiceBoxEngine::NextPage(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    std::string strAgent;
    std::string strCommand;
    if (m_bMoreHint) {
        strAgent = VBT_AGENT_HELP;
        strCommand = "Hints Navigation - Next Page";
    }
    else {
        strAgent = VBT_AGENT_SYSTEM;
        strCommand = VBT_COMMAND_SYSTEM_GLOBAL_NEXT_PAGE;
    }

    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->SendCommand(
                     &transaction,
                     strAgent.c_str(),
                     strCommand.c_str(),
                     VBT_FALSE,
                     NULL,
                     NULL);
    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    if (NULL != strTrans.Get()) {
        m_strCurrentTransactionID = strTrans.Get();
    }

    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnNextPage
            )
        );

    return true;
}

bool
VR_VoiceBoxEngine::PrevPage(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    std::string strAgent;
    std::string strCommand;
    if (m_bMoreHint) {
        strAgent = VBT_AGENT_HELP;
        strCommand = "Hints Navigation - Previous Page";
    }
    else {
        strAgent = VBT_AGENT_SYSTEM;
        strCommand = VBT_COMMAND_SYSTEM_GLOBAL_PREVIOUS_PAGE;
    }

    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->SendCommand(
                     &transaction,
                     strAgent.c_str(),
                     strCommand.c_str(),
                     VBT_FALSE,
                     NULL,
                     NULL);
    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    if (NULL != strTrans.Get()) {
        m_strCurrentTransactionID = strTrans.Get();
    }

    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnPrevPage
            )
        );

    return true;
}

bool
VR_VoiceBoxEngine::LastPage(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->SendCommand(
                     &transaction,
                     _T("System"),
                     VBT_COMMAND_SYSTEM_GLOBAL_LAST_PAGE,
                     VBT_FALSE,
                     NULL,
                     NULL);
    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    if (NULL != strTrans.Get()) {
        m_strCurrentTransactionID = strTrans.Get();
    }
    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnLastPage
            )
        );

    return true;
}

bool
VR_VoiceBoxEngine::FirstPage(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->SendCommand(
                     &transaction,
                     _T("System"),
                     VBT_COMMAND_SYSTEM_GLOBAL_FIRST_PAGE,
                     VBT_FALSE,
                     NULL,
                     NULL);
    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    if (NULL != strTrans.Get()) {
        m_strCurrentTransactionID = strTrans.Get();
    }
    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnFristPage
            )
        );

    return true;
}

bool
VR_VoiceBoxEngine::ConfirmYes(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = S_OK;
    if (InScreen("help_tutorial_confirmation")) {
        result = m_engineCommand->SendCommand(
                 &transaction,
                 _T("Help"),
                 VBT_COMMAND_CONFIRM_TUTORIAL_CONFIRM,
                 VBT_FALSE,
                 NULL,
                 NULL);
    }
    else {
        result = m_engineCommand->SendCommand(
                 &transaction,
                 _T("System"),
                 VBT_COMMAND_SYSTEM_GLOBAL_YES,
                 VBT_FALSE,
                 NULL,
                 NULL);
    }
    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    if (NULL != strTrans.Get()) {
        m_strCurrentTransactionID = strTrans.Get();
    }
    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnConfirmYes
            )
        );

    return true;
}

bool
VR_VoiceBoxEngine::GoDirectly(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    std::string strCommand;

    if ("Active Route Confirmation" == m_strActionType) {
        strCommand = "Select User Intent - Replace Current Route";
    }
    else if ("Select Preset Assignment" == m_strActionType) {
        strCommand = "Select Preset Assignment - Destination";
    }
    else if ("Select Previous Destination Assignment" == m_strActionType) {
        strCommand = "Select Previous Destination Assignment - Destination";
    }
    else if ("Get Assignment" == m_strActionType) {
        strCommand = "Get Assignment - Replace Route";
    }
    else {
        return false;
    }

    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->SendCommand(
                     &transaction,
                     m_strAgentName.c_str(),
                     strCommand.c_str(),
                     VBT_FALSE,
                     NULL,
                     NULL);
    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    if (NULL != strTrans.Get()) {
        m_strCurrentTransactionID = strTrans.Get();
    }

    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnGoDirectly
            )
        );

    return true;
}

bool
VR_VoiceBoxEngine::AddToRoute(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    std::string strCommand;
    if ("Active Route Confirmation" == m_strActionType) {
        strCommand = "Select User Intent - Add to Route";
    }
    else if ("Select Preset Assignment" == m_strActionType) {
        strCommand = "Select Preset Assignment - Waypoint";
    }
    else if ("Select Previous Destination Assignment" == m_strActionType) {
        strCommand = "Select Previous Destination Assignment - Waypoint";
    }
    else if ("Get Assignment" == m_strActionType) {
        strCommand = "Get Assignment - Add to Route";
    }
    else {
        return false;
    }

    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->SendCommand(
                     &transaction,
                     m_strAgentName.c_str(),
                     strCommand.c_str(),
                     VBT_FALSE,
                     NULL,
                     NULL);
    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    if (NULL != strTrans.Get()) {
        m_strCurrentTransactionID = strTrans.Get();
    }

    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnAddToRoute
            )
        );
    return true;
}

bool
VR_VoiceBoxEngine::Call(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }
    VR_LOG("VR_VoiceBoxEngine::Call");
    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = S_OK;
    VR_LOG("m_strScreenState: %s", m_strScreenState.c_str());
    if (InScreen("phone_speak_digits")) {
        result = m_engineCommand->SendCommand(
                 &transaction,
                 _T("HFD"),
                 VBT_COMMAND_DIGITS_FOLLOW_UP_CALL,
                 VBT_FALSE,
                 NULL,
                 NULL);
    }
    else if (InScreen("phone_one_call_contact_message_selected")) {
        VR_LOG("m_isNameNull: %d", m_isNameNull);
        if (m_isNameNull) {
            result = m_engineCommand->SendCommand(
                     &transaction,
                     _T("HFD"),
                     VBT_COMMAND_ONE_CALL_SELECTED_CALL,
                     VBT_FALSE,
                     NULL,
                     NULL);
        }
        else {
            result = m_engineCommand->SendCommand(
                     &transaction,
                     _T("HFD"),
                     VBT_COMMAND_ONE_CONTACT_SELECTED_CALL,
                     VBT_FALSE,
                     NULL,
                     NULL);
        }
    }
    else if (InScreen("phone_one_message_selected")) {
        result = m_engineCommand->SendCommand(
                 &transaction,
                 _T("HFD"),
                 VBT_COMMAND_ONE_MESSAGE_SELECTED_CALL,
                 VBT_FALSE,
                 NULL,
                 NULL);
    }
    else {
    }

    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    if (NULL != strTrans.Get()) {
        m_strCurrentTransactionID = strTrans.Get();
        VR_LOG("VR_VoiceBoxEngine::m_strCurrentTransactionID:%s", m_strCurrentTransactionID.c_str());
    }
    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnCall
            )
        );

    return true;
}

bool
VR_VoiceBoxEngine::SendMessage(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }
    VR_LOG("VR_VoiceBoxEngine::SendMessage new");
    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = S_OK;
    if (m_isNameNull) {
        result = m_engineCommand->SendCommand(
                 &transaction,
                 _T("HFD"),
                 VBT_COMMAND_ONE_CALL_SELECTED_SMS,
                 VBT_FALSE,
                 NULL,
                 NULL);
    }
    else {
        result = m_engineCommand->SendCommand(
                 &transaction,
                 _T("HFD"),
                 VBT_COMMAND_ONE_CONTACT_SELECTED_SMS_WITH_PHONETYPE_WITH_QUICK_REPLY,
                 VBT_FALSE,
                 NULL,
                 NULL);
    }

    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    if (NULL != strTrans.Get()) {
        m_strCurrentTransactionID = strTrans.Get();
        VR_LOG("VR_VoiceBoxEngine::m_strCurrentTransactionID:%s", m_strCurrentTransactionID.c_str());
    }
    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnSendMessage
            )
        );

    return true;
}

bool
VR_VoiceBoxEngine::Reply(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }
    VR_LOG("VR_VoiceBoxEngine::Reply");
    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->SendCommand(
                     &transaction,
                     _T("HFD"),
                     VBT_COMMAND_ONE_MESSAGE_SELECTED_REPLY,
                     VBT_FALSE,
                     NULL,
                     NULL);
    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    if (NULL != strTrans.Get()) {
        m_strCurrentTransactionID = strTrans.Get();
        VR_LOG("VR_VoiceBoxEngine::m_strCurrentTransactionID:%s", m_strCurrentTransactionID.c_str());
    }
    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnReply
            )
        );

    return true;
}

bool
VR_VoiceBoxEngine::ReadNext(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }
    VR_LOG("VR_VoiceBoxEngine::ReadNext");
    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->SendCommand(
                     &transaction,
                     _T("HFD"),
                     VBT_COMMAND_ONE_MESSAGE_SELECTED_READ_NEXT,
                     VBT_FALSE,
                     NULL,
                     NULL);
    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    if (NULL != strTrans.Get()) {
        m_strCurrentTransactionID = strTrans.Get();
        VR_LOG("VR_VoiceBoxEngine::m_strCurrentTransactionID:%s", m_strCurrentTransactionID.c_str());
    }
    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnReadNext
            )
        );

    return true;
}

bool
VR_VoiceBoxEngine::ReadPrevious(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }
    VR_LOG("VR_VoiceBoxEngine::ReadPrevious");
    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->SendCommand(
                     &transaction,
                     _T("HFD"),
                     VBT_COMMAND_ONE_MESSAGE_SELECTED_READ_PREVIOUS,
                     VBT_FALSE,
                     NULL,
                     NULL);
    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    if (NULL != strTrans.Get()) {
        m_strCurrentTransactionID = strTrans.Get();
        VR_LOG("VR_VoiceBoxEngine::m_strCurrentTransactionID:%s", m_strCurrentTransactionID.c_str());
    }
    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnReadPrevious
            )
        );

    return true;
}

bool
VR_VoiceBoxEngine::Tutorials(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    std::string strText;
    std::string strLang;
    VR_ConfigureIF * pcConfig = VR_ConfigureIF::Instance();
    if (NULL == pcConfig) {
        strLang = "en-us";
        strText = "Use the manual controls to select the tutorial you want to view.";
    }
    else {
        strLang = pcConfig->getVRLanguage();
        if ("en-us" == strLang) {
            strText = "Use the manual controls to select the tutorial you want to view.";
        }
        else if ("fr-ca" == strLang) {
            strText = "Utilisez les contrôles manuels pour sélectionner le tutoriel que vous voulez afficher.";
        }
        else if ("es-mx" == strLang) {
            strText = "Use los controles manuales para seleccionar el tutorial que desea ver.";
        }
        else {
        }
    }

    // Build the play guidance request XML message
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("prompt", "playTts");
    xmlBulder.buildGivenElement(node, "language", strLang, "", "");
    xmlBulder.buildGivenElement(node, "text", strText, "", "");
    xmlBulder.buildGivenElement(node, "phoneme", "", "", "");
    std::string strPlayTts = xmlBulder.getXmlString();
    VR_LOG("strPlayTts: %s", strPlayTts.c_str());
    m_iToturialBeepId = OnRequestAction(strPlayTts);
    VR_LOG("m_iToturialBeepId, %d", m_iToturialBeepId);
    VR_VoiceBoxXmlBuilder xmlBulderAction;
    pugi::xml_node nodeAciton = xmlBulderAction.buildStartActionElement("help", "showTutorialsUI");
    m_strToturialResult = xmlBulderAction.getXmlString();
    VR_LOG("m_strToturialResult: %s", m_strToturialResult.c_str());
}

bool
VR_VoiceBoxEngine::VoiceTraining(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    std::string strText;
    std::string strLang;
    VR_ConfigureIF * pcConfig = VR_ConfigureIF::Instance();
    if (NULL == pcConfig) {
        strLang = "en-us";
        strText = "Going to voice training mode.";
    }
    else {
        strLang = pcConfig->getVRLanguage();
        if ("en-us" == strLang) {
            strText = "Going to voice training mode.";
        }
        else if ("fr-ca" == strLang) {
            strText = "Je vais à l'entraînement de la reconnaissance vocale.";
        }
        else if ("es-mx" == strLang) {
            strText = "Entrando al modo de reconocimiento de voz.";
        }
        else {
        }
    }

    // Build the play guidance request XML message
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("prompt", "playTts");
    xmlBulder.buildGivenElement(node, "language", strLang, "", "");
    xmlBulder.buildGivenElement(node, "text", strText, "", "");
    xmlBulder.buildGivenElement(node, "phoneme", "", "", "");
    std::string strPlayTts = xmlBulder.getXmlString();
    VR_LOG("strPlayTts: %s", strPlayTts.c_str());
    m_iVoiceTrainingBeepId = OnRequestAction(strPlayTts);
    VR_LOG("m_iVoiceTrainingBeepId, %d", m_iVoiceTrainingBeepId);

    VR_VoiceBoxXmlBuilder xmlBulderDisplay;
    pugi::xml_node xmlNode = xmlBulderDisplay.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulderDisplay.buildGivenElement(xmlNode, "agent", "help", "", "");
    xmlBulderDisplay.buildGivenElement(xmlNode, "content", "na_help_train_voice_recognition", "", "");
    m_strVoiceTrainingResult = xmlBulderDisplay.getXmlString();
    VR_LOG("m_strVoiceTrainingResult: %s", m_strVoiceTrainingResult.c_str());
}

bool
VR_VoiceBoxEngine::SettingToTraning(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    m_bSettingToStartScreen = true;
    return true;
}

bool
VR_VoiceBoxEngine::InternalStartVR(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    m_pcMsgController->PostMessage("<event name=\"startAgent\"><agent>topmenu</agent></event>");
}

bool
VR_VoiceBoxEngine::ConfirmNo(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = S_OK;
    if (InScreen("help_tutorial_confirmation")) {
        result = m_engineCommand->SendCommand(
                 &transaction,
                 _T("Help"),
                 VBT_COMMAND_CONFIRM_TUTORIAL_REJECT,
                 VBT_FALSE,
                 NULL,
                 NULL);
    }
    else {
        result = m_engineCommand->SendCommand(
                 &transaction,
                 _T("System"),
                 VBT_COMMAND_SYSTEM_GLOBAL_NO,
                 VBT_FALSE,
                 NULL,
                 NULL);
    }
    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    if (NULL != strTrans.Get()) {
        m_strCurrentTransactionID = strTrans.Get();
    }
    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnConfirmNo
            )
        );

    return true;
}

bool
VR_VoiceBoxEngine::AlongRoute(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->SendCommand(
                     &transaction,
                     "POI",
                     "POI List - Along Route",
                     VBT_FALSE,
                     NULL,
                     NULL);
    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    if (NULL != strTrans.Get()) {
        m_strCurrentTransactionID = strTrans.Get();
    }

    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnAlongRoute
            )
        );
    return true;
}

bool
VR_VoiceBoxEngine::NearDestination(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->SendCommand(
                     &transaction,
                     "POI",
                     "POI List - Near Destination",
                     VBT_FALSE,
                     NULL,
                     NULL);
    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    if (NULL != strTrans.Get()) {
        m_strCurrentTransactionID = strTrans.Get();
    }

    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnNearDestination
            )
        );
    return true;
}

bool
VR_VoiceBoxEngine::StartSession()
{
    VR_LOGD_FUNC();

    if (m_bCanceled) {
        VR_LOG("m_bCanceled : %d", m_bCanceled);
        m_bCanceled = false;
        return false;
    }
    CVECIPtr<IVECITransaction> transaction;
    // For VoiceTag, the continue flag should be VBT_FALSE
    VBT_BOOL bContinuous = VBT_TRUE;
    HRESULT result = m_engineCommand->StartRecoSession(&transaction, bContinuous);
    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    if (NULL != strTrans.Get()) {
        m_strCurrentTransactionID = strTrans.Get();
        m_lstTransaction.push_front(m_strCurrentTransactionID);
    }
    m_sessionState = VR_SessionStateStarting;

    VR_LOG("***start session : %s", strTrans.Get());
    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnStartRecoSession
            )
        );

    return true;
}

// Start a voice recognition session
bool
VR_VoiceBoxEngine::StartRecoSession(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if ((NULL == m_engineCommand.ptr()) || (m_bCanceled)) {
        VR_LOG("m_bCanceled : %d", m_bCanceled);
        m_bCanceled = false;
        return false;
    }

    if ("" == m_strCurrentStateName) {
        m_strCurrentStateName = m_strStartOverStateName;
    }

    m_strPreSessionTransactionID = m_strCurrentTransactionID;

    VARIANT var;
    var.vt = VT_BOOL;
    var.boolVal = VARIANT_FALSE;
    m_client->SetProperty(PropClientManagedRender, &var);
    var.boolVal = VARIANT_FALSE;
    m_client->SetProperty(PropBackChangeContext, &var);

    CVECIPtr<IVECITransaction> transaction;
    // For VoiceTag, the continue flag should be VBT_FALSE
    VBT_BOOL bContinuous;
    if (VBT_RECO_STATE_VOICE_TAG == m_strCurrentStateName) {
        VR_LOG("m_strCurrentStateName : %s", m_strCurrentStateName.c_str());
        bContinuous = VBT_FALSE;
    }
    else {
        bContinuous = VBT_TRUE;
    }
    HRESULT result = m_engineCommand->StartRecoSession(&transaction, bContinuous);
    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    m_sessionState = VR_SessionStateStarting;

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    if (NULL != strTrans.Get()) {
        m_strCurrentTransactionID = strTrans.Get();
        m_lstTransaction.push_front(m_strCurrentTransactionID);
    }

    VR_LOG("***start session : %s", strTrans.Get());
    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnStartRecoSession
            )
        );

    return true;
}

void
VR_VoiceBoxEngine::SendCancelResult()
{
    if (m_bPVRScreen) {
        OnRecognized("<display xmlns=\"\" agent=\"Common\" content=\"QuitVRApp\"> </display>");
        m_bHavePVR = false;
        m_bPVRScreen = false;
        if (NULL != m_pcCatalogController) {
            m_pcCatalogController->ProcessOpMessage("<op name=\"needpause\"><value>false</value></op>");
            if (m_bWaitResumeGrammar) {
                VR_LOG("quit of PVRScreen, then resume grammar update");
                m_pcCatalogController->ProcessOpMessage("<op name=\"needresume\"><value>true</value></op>");
            }
            else {
                SetUpdateGammarFlg(true);
            }
        }
    }

    std::string cancelResult;
    while (!m_lstCancelOption.empty()) {
        std::string option = m_lstCancelOption.front();
        VR_LOG("sendCancelMsg : get option = %s", option.c_str());
        if (option.empty()) {
            cancelResult = "<event-result name=\"cancel\"/>";
        }
        else {
            cancelResult = "<event-result name=\"cancel\" option=\"" + option +"\"/>";
        }

        OnRecognized(cancelResult);
        VR_LOG("sendCancelMsg : msg = %s", cancelResult.c_str());
        m_lstCancelOption.pop_front();
    }
}

// Cancel the current voice recognition session
bool
VR_VoiceBoxEngine::Cancel(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    m_bCanceled = true;
    VR_LOG("m_sessionState = %d", m_sessionState);

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    if (m_bTslDialog) {
        std::string option = parser.getValueByKey("option");
        m_lstCancelOption.push_back(option);
        SendCancelResult();
        return true;
    }

    std::string strTarget = parser.getValueByKey("target");
    if ("" == strTarget) {
        VR_LOG("DM cancel");
        std::string option = parser.getValueByKey("option");
        if (!m_lstCancelOption.empty()) {
            VR_LOG("it is doing canceled event");
            m_lstCancelOption.push_back(option);
            return true;
        }
        else {
            m_lstCancelOption.push_back(option);
        }
        m_strBtnEventName = "";
        m_bDoCanceling = true;
        NotifyResourceState();
    }
    else {
        m_bDoCanceling = false;
        NotifyResourceState();
    }

    if (VR_REGION_US == m_country) {
        VR_LOG("PVR State %d", m_PVRStateCurrent);
        if (VR_PVRState_PlayingTTS == m_PVRStateCurrent) {
            OnRequest("<event name=\"PvrTTS\"><StatusName>PVRExit</StatusName></event>");
            m_bPVRScreen = true;
            VR_LOG("PVR Screen, wait TTS played");
            return true;
        }
        else if (VR_PVRState_WaitPlayTTS == m_PVRStateCurrent || VR_PVRState_PlayedTTS == m_PVRStateCurrent) {
            OnRequest("<event name=\"PvrTTS\"><StatusName>PVRExit</StatusName></event>");
            m_bPVRScreen = true;
            m_PVRStateCurrent = VR_PVRState_None;
        }
        else {
            m_PVRStateCurrent = VR_PVRState_None;
        }
    }

    if (VR_SessionStateStoped == m_sessionState
        || VR_SessionStateNone == m_sessionState) {
        if (m_bDoCanceling) {
            m_bDoCanceling = false;
            SendCancelResult();
            NotifyResourceState();
            if (m_bSettingToStartScreen) {
                m_bSettingToStartScreen = false;
                VR_LOG("VR is not start");
                std::string strQuitVRApp = "<display xmlns=\"\" agent=\"Common\" content=\"QuitVRApp\">"
                                "</display>";
                OnRecognized(strQuitVRApp);
            }
        }
        return true;
    }
    if (m_bSettingToStartScreen) {
        VR_LOG("reset the mark");
        m_bSettingToStartScreen = false;
    }

    m_sessionState = VR_SessionStateCanceled;
    if ((m_iDoneBeepID == VR_INVALID_ACTION_ID) && m_bDoCanceling) {
        StopAllTTS();
        CancelRecoSession();
        OnBeep(VR_BeepType_Done);
    }

    return true;
}

// Repeat the last voice recognition session
bool
VR_VoiceBoxEngine::Repeat(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    // Cancel the curent session
    if (!CancelRecoSession()) {
        return false;
    }

    CVECIPtr<IVECITransaction> spCurrentTrans;
    CVECIPtr<IVECITransaction> spLastTrans;
    HRESULT result = m_engineCommand->Repeat(&spCurrentTrans, VBT_TRUE, &spLastTrans);
    if (FAILED(result) || (NULL == spCurrentTrans.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = spCurrentTrans->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    if (NULL != strTrans.Get()) {
        m_strCurrentTransactionID = strTrans.Get();
    }
    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnRepeat
            )
        );

    if (NULL != spLastTrans.ptr()) {
        result = spCurrentTrans->GetTransactionId(&strTrans);
        if (FAILED(result)) {
            return false;
        }

        m_strLastTransactionID = strTrans.Get();
    }

    return true;
}

// Back the voice recognition session to the previous context.
bool
VR_VoiceBoxEngine::Back(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    VR_LOG("Handle back command");
    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    m_bBackInterupted = false;

    VR_LOG("cancel session");
    // Cancel the curent session
    CancelRecoSession();

    VR_LOG("check more hints state");
    if (m_bMoreHint) {
        VR_LOG("this is hints state");

        // Prepare the hints
        if (!GetHints()) {
            return false;
        }

        if (VR_SessionStateRestartSessionAfterTTS == m_sessionState
            || VR_SessionStatePaused == m_sessionState) {
            SendRecogState();
        }
        else {
            m_sessionState = VR_SessionStateAutoSendRecogState;
        }
        return true;
    }

    CVECIPtr<IVECITransaction> spCurrentTrans;
    CVECIPtr<IVECITransaction> spPrevTrans;
    VR_LOG("do back command");
    HRESULT result = m_engineCommand->Back(&spCurrentTrans, VBT_TRUE, &spPrevTrans);
    if (FAILED(result) || (NULL == spCurrentTrans.ptr())) {
        return false;
    }

    CVECIOutStr strCurTrans;
    result = spCurrentTrans->GetTransactionId(&strCurTrans);
    if (FAILED(result)) {
        return false;
    }

    if (NULL != strCurTrans.Get()) {
        m_strCurrentTransactionID = strCurTrans.Get();
        VR_LOG("current tran : %s", strCurTrans.Get());
    }

    CVECIOutStr strTrans;

    if (NULL != spPrevTrans.ptr()) {
        VR_LOG("this is not the first session");
        result = spPrevTrans->GetTransactionId(&strTrans);
        if (FAILED(result)) {
            return false;
        }
        CVECIPtr<IVECIParsedMessage> spMsg;
        spPrevTrans->GetResultMessage(&spMsg);

        m_mapCmdResultHandler.insert(
            std::make_pair(
            strCurTrans.Get(),
            &VR_VoiceBoxEngine::OnBack));

        VR_LOG("prev tran : %s", strTrans.Get());
        if (m_strPreSessionTransactionID == strTrans.Get()) {
            if (VBT_RECO_STATE_IN_CALL == m_strCurrentStateName) {
                std::string strScreen = "<display agent=\"Common\" content=\"ScreenDisplay\">"
                                            "<agent>phone</agent>"
                                            "<content>";
                std::string strContent = "phone_in_call";
                BuildScreenContent(strContent);
                if (VR_REGION_OC == m_country) {
                    strContent = "sendtone_speak_tones";
                }
                strScreen.append(strContent);
                strScreen.append("</content></display>");

                OnRecognized(strScreen);
            }
            else if (VBT_RECO_STATE_INCOMING_MESSAGE == m_strCurrentStateName) {
                std::string strScreen = "<display agent=\"Common\" content=\"ScreenDisplay\">"
                                            "<agent>phone</agent>"
                                            "<content>";

                std::string strContent = "phone_in_message";
                BuildScreenContent(strContent);

                strScreen.append(strContent);
                strScreen.append("</content></display>");

                OnRecognized(strScreen);
            }
            else if (VBT_RECO_STATE_SPEAKER_ADAPTATION == m_strCurrentStateName) {
                if ("1" == m_strStep) {
                    VR_LOG("ADAPTATION");
                    m_sessionState = VR_SessionStateBackQuit;
                    OnBeep(VR_BeepType_Done);
                    return true;
                }
            }
            else {
                VR_LOG("get hints");
                GetHints();
            }
        }
        else {
            VR_LOG("Process Agent Message");

            if (NULL != spMsg.ptr()) {
                CVECIOutStr strAgentName;
                (void)spMsg->GetAgentName(&strAgentName);

                if (strAgentName.IsEqual("HFD")) {
                    m_agents[AgentType_Phone]->ProcessAgentMessage(spMsg);
                }
                else if (strAgentName.IsEqual("Music") || strAgentName.IsEqual("Radio")) {
                    m_agents[AgentType_Audio]->ProcessAgentMessage(spMsg);
                }
                else if (strAgentName.IsEqual("Apps")) {
                    m_agents[AgentType_Apps]->ProcessAgentMessage(spMsg);
                }
                else if (strAgentName.IsEqual("HVAC")) {
                    m_agents[AgentType_Climate]->ProcessAgentMessage(spMsg);
                }
                else if (strAgentName.IsEqual("Root")
                    || strAgentName.IsEqual(VBT_AGENT_SYSTEM)
                    || strAgentName.IsEqual(VBT_AGENT_HELP)
                    || strAgentName.IsEqual(VBT_AGENT_SPEAKER_ADAPTATION)) {
                    m_agents[AgentType_Global]->ProcessAgentMessage(spMsg);
                }
                else if (strAgentName.IsEqual("Information")) {
                    m_agents[AgentType_Info]->ProcessAgentMessage(spMsg);
                }
                else if (strAgentName.IsEqual("Navigation")
                    || strAgentName.IsEqual(VBT_AGENT_ADDRESS_ENTRY)
                    || strAgentName.IsEqual(VBT_AGENT_DESTINATION)
                    || strAgentName.IsEqual(VBT_AGENT_POI)) {
                    VR_LOG("VBT will call the function!");
                }
                else {

                }
            }
        }
        m_sessionState = VR_SessionStateBackRestart;
    }
    else {
        VR_LOG("this is the first session");
        // VR quit back to haptic
        m_sessionState = VR_SessionStateBackQuit;
        OnBeep(VR_BeepType_Done);
    }

    return true;
}

// Restart the voice recognition session
bool
VR_VoiceBoxEngine::StartOver(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    VR_LOG("current session %d", m_sessionState);
    if (VBT_RECO_STATE_IN_CALL == m_strCurrentStateName) {
        std::string strScreen = "<display agent=\"Common\" content=\"ScreenDisplay\">"
        "<agent>phone</agent>"
        "<content>";
        std::string strContent = "phone_in_call";
        BuildScreenContent(strContent);
        if (VR_REGION_OC == m_country) {
            strContent = "sendtone_speak_tones";
        }
        strScreen.append(strContent);
        strScreen.append("</content></display>");

        OnRecognized(strScreen);
    }
    else if (VBT_RECO_STATE_INCOMING_MESSAGE == m_strCurrentStateName) {
        std::string strScreen = "<display agent=\"Common\" content=\"ScreenDisplay\">"
        "<agent>phone</agent>"
        "<content>";

        std::string strContent = "phone_in_message";
        BuildScreenContent(strContent);

        strScreen.append(strContent);
        strScreen.append("</content></display>");

        OnRecognized(strScreen);
    }
    else {
    }
    m_strStartOverStateName = m_strCurrentStateName;
    if (VR_SessionStatePaused == m_sessionState
        || VR_SessionStateRestartSessionAfterTTS == m_sessionState) {
        VR_LOG("get hints");
        // Prepare the hints
        if (!GetHints()) {
            return false;
        }

        SendRecogState();
        m_sessionState = VR_SessionStateTemporarilyCanceled;
    }
    else {
        VR_LOG("after the session completed to do start over");
        m_sessionState = VR_SessionStateStartOver;
    }

    return true;
}

// Update the hints and play the related help guidance
bool
VR_VoiceBoxEngine::Help(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    // Cancel the curent session
    if (!CancelRecoSession()) {
        return false;
    }

    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->SendCommand(
                     &transaction,
                     _T("System"),
                     _T("System Help"),
                     VBT_FALSE,
                     NULL,
                     NULL);
    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    if (NULL != strTrans.Get()) {
        m_strCurrentTransactionID = strTrans.Get();
    }
    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnHelp
            )
        );
    m_sessionState = VR_SessionStateRestartSessionAfterTTS;

    if (VR_REGION_US == m_country) {
        ShowHelpMoreHintsScreen();
    }

    return true;
}

bool
VR_VoiceBoxEngine::OnGetHints(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();
    VR_LOGP("receive getHints request");
    if (NULL == m_client.ptr()) {
        return false;
    }

    m_strAction = "";
    m_lstTransaction.clear();

    std::string strStateName = parser.getValueByKey("agentName");
    // std::string strPageSize = parser.getValueByKey("pageSize");

    std::string strResult = "<event-result name=\"getHints\" errcode=\"\" ><agentName>";
    strResult.append(strStateName);
    strResult.append("</agentName> <pageSize>5</pageSize> </event-result>");

    HRESULT retCode = m_client->CreateStringSet(&m_spHints);
    if (S_OK != retCode) {
        return false;
    }
    if (NULL == m_spHints.ptr()) {
        return false;
    }

    m_bMoreHint = false;

    VBT_INT nScreenId = m_mapHintsScreenID["none"];
    std::string strVBTStateName;

    GetVBTInfoByUIStateName(strStateName, nScreenId, strVBTStateName);

    m_strUIStateName = strStateName;

    CVECIPtr<IVECIListItems> optionalContextList;
    m_client->CreateListItems(&optionalContextList);
    if (NULL == optionalContextList.ptr()) {
        return false;
    }

    SetOptionalContext(optionalContextList, strVBTStateName, false);

    retCode = m_engineCommand->GetRecognitionStateHints("Global", nScreenId, optionalContextList, &m_spHints);
    if (S_OK != retCode) {
        VR_LOG("GetRecognitionStateHints: %x", retCode);
        return false;
    }

    m_iIndex = 0;
    m_iHintSize = 0;

    m_spHints->GetSize(&m_iHintSize);
    (void)DisplayHints("", true);

    VR_LOGP("reply hints result");
    OnRecognized(strResult);

    return true;
}

bool
VR_VoiceBoxEngine::HandleMoreHints(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    m_bMoreHint = true;

    VR_LOG("------------ VR_VoiceBoxEngine MoreHints");

    // Cancel the curent session
    if (!CancelRecoSession()) {
        return false;
    }

    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->SendCommand(
                     &transaction,
                     VBT_AGENT_HELP,
                     VBT_COMMAND_SYSTEM_MORE_HINTS,
                     VBT_FALSE,
                     NULL,
                     NULL);

    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnMoreHints
            )
        );

    m_iIndex = 0;

    (void)DisplayHints("VR-HNT-04");

    return true;
}

bool
VR_VoiceBoxEngine::MoreHints(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();
    VR_LOGP("DE: start morehints button command...case : 215-8");

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    m_bMoreHint = true;
    m_iIndex = 0;

    DisplayHints("VR-HNT-04");
    return true;
}

bool
VR_VoiceBoxEngine::HandleActionMsg(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    m_strActionClass = parser.getValueByKey("actionClass");

    if (VR_REGION_OC == m_country) {
        std::string strActionType = parser.getValueByKey("actionType");
        if (("Escalating Error" != strActionType) && ("No Reco" != strActionType)) {
            // m_strActionType and m_strAgentName are only used for navigation
            m_strActionType = strActionType;
            m_strAgentName = parser.getValueByKey("agentName");
        }
    }
    else if (VR_REGION_US == m_country) {
        m_strActionType = parser.getValueByKey("actionType");
        m_strAgentName = parser.getValueByKey("agentName");
    }
    else {
        VR_LOG("ERROR! Invalid Country !!");
    }

    VR_LOG("m_strActionType = [%s], m_strActionClass = [%s]", m_strActionType.c_str(), m_strActionClass.c_str());

    if ("Task Complete" == m_strActionClass) {
        m_bTaskCompelete = true;
    }
    else {
        m_bTaskCompelete = false;
    }

    return true;
}

bool
VR_VoiceBoxEngine::HandleVRState(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (m_bTTSNull) {
        if ("" != m_strAction) {
            VR_LOG("run the action");
            OnRequestAction(m_strAction);
        }
        return true;
    }

    bool bBargeInPrompt = false;

    if (NULL != VR_ConfigureIF::Instance()) {
        if (VR_ConfigureIF::Instance()->getVROverPrompt()) {
            m_bBosDetected = false;
            VR_AudioStreamIF* pcAudioStream = VR_AudioStreamIF::Instance();
            if ((VBT_RECO_STATE_SPEAKER_ADAPTATION != m_strCurrentStateName)
                && (VBT_RECO_STATE_VOICE_TAG != m_strCurrentStateName)
                && (VR_PVRState_PlayingTTS != m_PVRStateCurrent)) {

                if ((VR_SessionStatePaused != m_sessionState)
                    && (VR_SessionStatePttLong != m_sessionState)
                    && (!m_bTaskCompelete)
                    && (NULL != pcAudioStream)) {
                    bBargeInPrompt = true;
                    pcAudioStream->StartAudioInWithBargeIn();
                }
            }
        }
    }

    VR_LOG("current session : %d", m_sessionState);
    std::string strState;
    std::string strPrompt;
    if (VR_SessionStatePaused == m_sessionState) {
        GetPromptByScreenId("VR-SB-05", strPrompt);
        strState = "paused";
    }
    else {
        if (VR_SessionStateAutoSendRecogState != m_sessionState
            && m_bTaskCompelete) {
            VR_LOG("m_bTaskCompelete true");
            strState = "reading";
        }
        else {
            VR_LOG("m_bTaskCompelete false");

            strPrompt = VR_VoiceBoxEventSink::m_strPrompt;
            strState = bBargeInPrompt ? "promptWithBargein" : "promptPlaying";
        }
    }

    SendVRState(strState, strPrompt, "false", "");

    return true;
}

bool
VR_VoiceBoxEngine::SetRecoSessionFlag(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    m_sessionState = VR_SessionStateAutoRestartSession;
    return true;
}

bool
VR_VoiceBoxEngine::DisplayHints(const std::string& strHintScreenId, bool bGetMoreHint)
{
    VR_LOGD_FUNC();

    if (NULL == m_spHints) {
        return false;
    }

    std::string strDisplayContent;

    if (bGetMoreHint) {
        strDisplayContent = "HintsDisplay";
    }
    else {
        strDisplayContent = "ScreenDisplay";
    }

    const int DEFAULTPAGESIZE = 4;
    const int  MOREHINTPAGESIZE = 5;
    int iHintPageSize = MOREHINTPAGESIZE;
    if (VR_REGION_US == m_country) {
        iHintPageSize = ("VR-HNT-04" == strHintScreenId) ? MOREHINTPAGESIZE : DEFAULTPAGESIZE;
    }

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildDisplayElement("Common", strDisplayContent);
    xmlBulder.buildGivenElement(node, "agent", m_strUIStateName.c_str(), "", "");
    std::string strContent = (m_bMoreHint) ? m_strMoreHints : m_strContent;
    xmlBulder.buildGivenElement(node, "content", strContent.c_str(), "", ""); // content
    pugi::xml_node nodeHints = xmlBulder.buildGivenElement(node, "hints", "", "", "");
    pugi::xml_node nodeList = xmlBulder.buildGivenElement(nodeHints, "list", "", "", "");
    pugi::xml_node nodeHeader = xmlBulder.buildGivenElement(nodeList, "header", "", "", "");
    xmlBulder.buildGivenElement(nodeHeader, "startIndex", std::to_string(m_iIndex), "", "");
    xmlBulder.buildGivenElement(nodeHeader, "pageSize", std::to_string(iHintPageSize), "", "");
    xmlBulder.buildGivenElement(nodeHeader, "count", std::to_string(m_iHintSize), "", "");
    pugi::xml_node nodeItems = xmlBulder.buildGivenElement(nodeList, "items", "", "", "");

    VoiceVector<std::string>::type vecItem;
    std::string strHintStart;
    std::string strHintEnd;

    if ("fr-CA" == m_strCultureName) {
        strHintStart = "« ";
        strHintEnd = " »";
    }
    else {
        strHintStart = "“";
        strHintEnd = "”";
    }

    // Retrieve Each Hints
    for (VBT_ULONG i = 0; i < iHintPageSize; ++i) {
        CVECIOutStr strHint;
        (void)m_spHints->GetItem(m_iIndex, &strHint);
        if (m_iIndex < m_iHintSize) {
            ++m_iIndex;
        }
        else {
            break;
        }

        VoiceVector<StructNode>::type attributeVector;
        StructNode strNode;

        strNode.Name = "hint";
        if (NULL != strHint.Get()) {
            std::string strHintTmp = strHint.Get();
            strNode.Value = strHintStart;
            strNode.Value.append(strHintTmp);
            strNode.Value.append(strHintEnd);
        }
        attributeVector.push_back(strNode);

        xmlBulder.buildListItemChildElement(nodeItems, "", hint, attributeVector);
    }

    std::string strReply = xmlBulder.getXmlString();

    OnRecognized(strReply);

    if (!strHintScreenId.empty()) {
        VRSateForHint(strHintScreenId);
    }

    return true;
}

void
VR_VoiceBoxEngine::GetUIInfoByVBTStateName(int& nScreenId, const std::string strStateName)
{
    VR_LOGD_FUNC();

    if (strStateName.empty()) {
        return;
    }

    nScreenId = m_mapHintsScreenID["none"];
    m_strContent.clear();
    m_strUIStateName.clear();
    m_strMoreHints.clear();

    if ("Global" == strStateName) {
        nScreenId = m_mapHintsScreenID["main"];
        m_strContent = "topmenu_idle";
        m_strUIStateName = "topmenu";
        m_strMoreHints = "topmenu_more_hints";

    }
    else if ("HFD Home" == strStateName) {
        nScreenId = m_mapHintsScreenID["phone"];
        m_strContent = "phone_idle";
        m_strUIStateName = "phone";
        m_strMoreHints = "phone_more_hints";
    }
    else if ("Navigation Home" == strStateName || "POI Home" == strStateName) {
        nScreenId = m_mapHintsScreenID["navigation"];
        m_strContent = "navi_idle";
        m_strUIStateName = "navi";
        m_strMoreHints = "navi_more_hints";
    }
    else if ("HVAC Home" == strStateName) {
        nScreenId = m_mapHintsScreenID["climate"];
        m_strContent = "climate_idle";
        m_strUIStateName = "climate";
    }
    else if ("Information Home" == strStateName) {
        nScreenId = m_mapHintsScreenID["info"];
        m_strContent = "info_idle";
        m_strUIStateName = "info";
    }
    else if ("Music Home" == strStateName || "Radio Home" == strStateName
        || "Audio Home" == strStateName) {
        nScreenId = m_mapHintsScreenID["audio"];
        m_strContent = "media_idle";
        m_strUIStateName = "media";
        m_strMoreHints = "media_more_hints";
    }
    else if ("Apps Home" == strStateName) {
        nScreenId = m_mapHintsScreenID["apps"];
        m_strContent = "apps_idle";
        m_strUIStateName = "apps";
        m_strMoreHints = "apps_more_hints";
    }
    else if ("Help" == strStateName) {
        nScreenId = m_mapHintsScreenID["help"];
        m_strContent = "topmenu_idle";
        m_strUIStateName = "topmenu";
        m_strMoreHints = "topmenu_more_hints";
    }
    else {
        nScreenId = m_mapHintsScreenID["help"];
        m_strContent = "help_idle";
        m_strUIStateName = "help";
    }

    BuildScreenContent(m_strContent);
    BuildScreenContent(m_strMoreHints);
}

void
VR_VoiceBoxEngine::GetVBTInfoByUIStateName(const std::string& strUISateName,
    int& nScreenId, std::string& strVBTStateName)
{
    VR_LOGD_FUNC();

    if (strUISateName.empty()) {
        return;
    }

    nScreenId = m_mapHintsScreenID["none"];
    m_strContent.clear();
    strVBTStateName.clear();

    if ("apps" == strUISateName) {
        nScreenId = m_mapHintsScreenID["apps"];
        m_strContent = "apps_idle";
        strVBTStateName = "Apps Home";
    }
    else if ("topmenu" == strUISateName) {
        nScreenId = m_mapHintsScreenID["main"];
        m_strContent = "topmenu_idle";
        strVBTStateName = "Global";
    }
    else if ("phone" == strUISateName) {
        nScreenId = m_mapHintsScreenID["phone"];
        m_strContent = "phone_idle";
        strVBTStateName = "HFD Home";
    }
    else if ("climate" == strUISateName) {
        nScreenId = m_mapHintsScreenID["climate"];
        m_strContent = "climate_idle";
        strVBTStateName = "HVAC Home";
    }
    else if ("information" == strUISateName) {
        nScreenId = m_mapHintsScreenID["info"];
        m_strContent = "info_idle";
        strVBTStateName = "Information Home";
    }
    else if ("music" == strUISateName) {
        nScreenId = m_mapHintsScreenID["audio"];
        m_strContent = "media_idle";
        if (VR_REGION_US == m_country) {
            strVBTStateName = "Music Home";
        }
        else if (VR_REGION_OC == m_country) {
            strVBTStateName = "Audio Home";
        }
        else {
        }
    }
    else if ("navi" == strUISateName) {
        nScreenId = m_mapHintsScreenID["navigation"];
        m_strContent = "navi_idle";
        strVBTStateName = "Navigation Home";
    }
    else if ("poi" == strUISateName) {
        nScreenId = m_mapHintsScreenID["navigation"];
        m_strContent = "navi_idle";
        strVBTStateName = "POI Home";
    }
    else if ("radio" == strUISateName) {
        nScreenId = m_mapHintsScreenID["audio"];
        m_strContent = "media_idle";
        strVBTStateName = "Radio Home";
    }

    BuildScreenContent(m_strContent);
}

// Get hints content from VBT
bool
VR_VoiceBoxEngine::PrepareHintsContent(const std::string& strStateName)
{
    if ((NULL ==  m_client.ptr()) || (NULL == m_engineCommand.ptr())) {
        return false;
    }

    HRESULT retCode = m_client->CreateStringSet(&m_spHints);
    if (S_OK != retCode) {
        return false;
    }
    if (NULL == m_spHints.ptr()) {
        return false;
    }

    VBT_INT nScreenId = m_mapHintsScreenID["none"];

    GetUIInfoByVBTStateName(nScreenId, strStateName);

    CVECIPtr<IVECIListItems> optionalContextList;
    m_client->CreateListItems(&optionalContextList);
    if (NULL == optionalContextList.ptr()) {
        return false;
    }

    SetOptionalContext(optionalContextList, strStateName, false);

    retCode = m_engineCommand->GetRecognitionStateHints("Global", nScreenId, optionalContextList, &m_spHints);
    if (S_OK != retCode) {
        VR_LOG("GetRecognitionStateHints: %x", retCode);
        return false;
    }

    m_iIndex = 0;
    m_spHints->GetSize(&m_iHintSize);

    return true;
}

// Get the hints for the specified state name.
bool
VR_VoiceBoxEngine::GetHints()
{
    VR_LOGD_FUNC();

    VR_LOG("this is not more hints state");
    m_bMoreHint = false;

    if (!PrepareHintsContent(m_strCurrentStateName)) {
        return false;
    }

    (void)DisplayHints("VR-HNT-01");

    return true;
}

// When the guidance play is completed, this function will be called.
bool
VR_VoiceBoxEngine::SpeakDone(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if ("" != m_strMsgPrompt) {
        VR_LOG("close popup message");
        ClosePopupMsg();

        if ("" != m_strDisplayScreen) {
            VR_LOG("jump screen");
            OnRequestAction(m_strDisplayScreen);
        }
    }

    if (NULL != VR_ConfigureIF::Instance()) {
        if (VR_ConfigureIF::Instance()->getVROverPrompt()) {
            m_bBosDetected = false;
            VR_AudioStreamIF* pcAudioStream = VR_AudioStreamIF::Instance();
            if (NULL != pcAudioStream) {
                pcAudioStream->StopAudioIn();
            }
        }
    }

    if (VR_REGION_US == m_country) {
        VR_LOG("PVR State %d", m_PVRStateCurrent);
        if (VR_PVRState_PlayingTTS == m_PVRStateCurrent) {
            if (m_bDoCanceling) {
                VR_LOG("PVR -> cancel event to DM");
                m_bDoCanceling = false;
                SendCancelResult();
                m_PVRStateCurrent = VR_PVRState_None;
                NotifyResourceState();
                return true;
            }
            else {
                VR_LOG("PVR -> TTS over");
                m_PVRStateCurrent = VR_PVRState_PlayedTTS;
            }
        }
    }

    if (m_mapPlayTTSTransation.empty()) {
        VR_LOG("play TTS is finished");
        return true;
    }

    TransationSpeakDone(m_iCurrActionId);

    if (VR_SessionStatePttLong == m_sessionState) {
        VR_LOG("stop session");
        OnBeep(VR_BeepType_Done);
        return true;
    }

    VR_LOG("play tts over");
    if (m_bPlayTTS) {
        VR_LOG("it is by play tts!");
    }
    else { // this is not need  when the function of stop tts is achieved
        VR_LOG("it is by stop tts!");
        VoiceMap<std::string, MessageHandler>::const_iterator iterMap = m_mapMsgBtnHandler.find(m_strBtnEventName);
        if (m_mapMsgBtnHandler.cend() != iterMap) {
            if (NULL != iterMap->second) {
                VR_LOG("handle the event");
                if ("help" != m_strBtnEventName) {
                    CancelRecoSession();
                }
                m_bTTSNull = true;
                (this->*(iterMap->second))(parser);
            }
        }
        m_strBtnEventName = "";
    }
    m_bPlayTTS = false;

    if (m_bTaskCompelete && ("" != m_strAction)) {
        OnRequestAction(m_strAction);
    }

    if (VR_SessionStateRestartSessionAfterTTS == m_sessionState) {
        StartSession();
    }
    else if (VR_SessionStateQuitAfterTTS == m_sessionState) {
        VR_LOG("stop session");

        // Notify the XML message to the external service
        OnBeep(VR_BeepType_Done);
    }
    else if (VR_SessionStateSendRecogAfterTTS == m_sessionState) {
        VR_LOG("get hints");
        GetHints();

        SendRecogState();
    }
    else {

    }

    if (m_bWaitGreetingEnd && (m_mapPlayTTSTransation.empty())) {
        m_bWaitGreetingEnd = false;
        StartAppRecoSession();
    }


    if (m_bQuitWaitForPrompt && (m_mapPlayTTSTransation.empty())) {
        m_bQuitWaitForPrompt = false;

        m_bWaitForDoneBeep = true;
        OnBeep(VR_BeepType_TSL_Done);
    }

    return true;
}

bool
VR_VoiceBoxEngine::StopSpeakDone(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if ("" != m_strMsgPrompt) {
        VR_LOG("close popup message");
        ClosePopupMsg();

        if ("" != m_strDisplayScreen) {
            VR_LOG("jump screen");
            OnRequestAction(m_strDisplayScreen);
        }
    }

    if (NULL != VR_ConfigureIF::Instance()) {
        if (VR_ConfigureIF::Instance()->getVROverPrompt()) {
            VR_AudioStreamIF* pcAudioStream = VR_AudioStreamIF::Instance();
            if ((!m_bBosDetected) && (NULL != pcAudioStream)) {
                pcAudioStream->StopAudioIn();
            }

            m_bBosDetected = false;
        }
    }

    if (VR_REGION_US == m_country) {
        VR_LOG("PVR State %d", m_PVRStateCurrent);
        if (VR_PVRState_PlayingTTS == m_PVRStateCurrent) {
            if (m_bDoCanceling) {
                VR_LOG("PVR -> cancel event to DM");
                m_bDoCanceling = false;
                SendCancelResult();
                m_PVRStateCurrent = VR_PVRState_None;
                NotifyResourceState();
                return true;
            }
            else {
                VR_LOG("PVR -> TTS over");
                m_PVRStateCurrent = VR_PVRState_PlayedTTS;
            }
        }
    }

    if (m_bQuitWaitForPrompt) {
        m_bQuitWaitForPrompt = false;

        m_bWaitForDoneBeep = true;
        OnBeep(VR_BeepType_TSL_Done);
    }

    if (m_mapStopTTSTransation.empty()) {
        VR_LOG("stopTTS is finished");
        if (!m_strAction.empty()) {
            OnRequestAction(m_strAction);
        }
        return true;
    }

    if (!TransationStopSpeakDone(m_iCurrActionId)) {
        VR_LOG("speak done id is not same");
        return false;
    }

    if ((m_stopTraining == m_iCurrActionId)
        && (m_iCurrActionId != VR_INVALID_ACTION_ID)) {
        m_stopTraining = VR_INVALID_ACTION_ID;
        CancelRecoSession();
        VR_VoiceBoxXmlBuilder xmlBulder;
        pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
        xmlBulder.buildGivenElement(xmlNode, "agent", "help", "", "");
        xmlBulder.buildGivenElement(xmlNode, "content", "na_help_train_voice_recognition", "", "");
        std::string strReply = xmlBulder.getXmlString();
        VR_LOG("SpeakerAdaptionCatch: %s", strReply.c_str());
        OnRecognized(strReply);
    }

    if ((m_stopVoiceTagBeepOrTTs == m_iCurrActionId)
        && (m_iCurrActionId != VR_INVALID_ACTION_ID)) {

        m_stopVoiceTagBeepOrTTs = VR_INVALID_ACTION_ID;
        m_listPlayVoiceTagSeq.clear();

        if (0 != m_iPlayVoiceTagId) {
            m_iPlayVoiceTagId = 0;
            OnRecognized(m_iOss.str());
        }

        VR_LOG("cancel recog");
        if (VR_SessionStateStoped == m_sessionState
            || VR_SessionStateNone == m_sessionState) {
            if (m_bDoCancelVoiceTag) {
                OnRequestAction(VR_ACTION_CLOSESESSION);
                m_bDoCancelVoiceTag = false;
            }
        }
        else if (VR_SessionStateRestartSessionAfterTTS == m_sessionState) {
            if (m_bDoCancelVoiceTag) {
                OnRequestAction(VR_ACTION_CLOSESESSION);
                m_sessionState = VR_SessionStateStoped;
                m_bDoCancelVoiceTag = false;
            }
        }
        else {
            if (m_bDoCancelVoiceTag) {
                CancelVoiceTagRecoSession();
            }
        }
    }

    if ("" == m_strBtnEventName) {
        VR_LOG("The button event name is NULL. It is not a button event.");
        return true;
    }

    // Dispatch the messages
    VoiceMap<std::string, MessageHandler>::const_iterator iterMap = m_mapMsgBtnHandler.find(m_strBtnEventName);
    if (m_mapMsgBtnHandler.cend() != iterMap) {
        if (NULL != iterMap->second) {
            if ("help" != m_strBtnEventName) {
                VR_LOG("stop tts to cancel session");
                VoiceList<std::string>::iterator iter = std::find(m_lstNeedReactiveAction.begin(), m_lstNeedReactiveAction.end(), m_strActionType);
                if (m_bTaskCompelete && iter == m_lstNeedReactiveAction.end()) {
                    VR_LOG("not need to CancelSession");
                    StopAllTTS();
                }
                else {
                    CancelRecoSession();
                }
            }

            VR_LOG("handle the event");
            m_bTTSNull = true;
            (this->*(iterMap->second))(parser);
        }
    }

    if (m_bTaskCompelete && ("" != m_strAction)) {
        OnRequestAction(m_strAction);
    }

    if (VR_SessionStateQuitAfterTTS == m_sessionState) {
        VR_LOG("stop session");
        OnBeep(VR_BeepType_Done);
    }
    else if (VR_SessionStateSendRecogAfterTTS == m_sessionState) {
        VR_LOG("get hints");
        GetHints();

        SendRecogState();
    }
    else {

    }

    m_strBtnEventName = "";
    return true;
}

// When the beep play is completed, this function will be called.
bool
VR_VoiceBoxEngine::BeepDone(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    if ((m_iStartBeepID == m_iCurrActionId) && (m_iCurrActionId != VR_INVALID_ACTION_ID)) {
        VR_LOG("start beep");
        m_iStartBeepID = VR_INVALID_ACTION_ID;
        HRESULT result = m_engineCommand->BeepDone();
        if (FAILED(result)) {
            return false;
        }
    }

    if ((m_iDoneBeepID == m_iCurrActionId) && (m_iCurrActionId != VR_INVALID_ACTION_ID)) {
        m_strMsgPrompt = "";
        m_bPlayTTS = false;
        if (m_bDoCanceling) {
            VR_LOG("This is the cancel event by DM");
            m_bDoCanceling = false;
            SendCancelResult();
            NotifyResourceState();
        }
        HandleQuitVR();
    }

    if (m_bWaitForDoneBeep) {
        m_bWaitForDoneBeep = false;
        if (VR_AppRecoState_SendCaching != m_bAppRecoState) {
            std::string eventResult = "<event-result name=\"CancelAppRecognition\" errcode=\"0\"/>";
            OnRecognized(eventResult);
        }
        else {
            m_pcMsgController->PostMessage(m_strTslAppRecoStateMsg);
            m_strTslAppRecoStateMsg = "";
        }
    }

    return true;
}

// When the command is completed, call the following functions
bool
VR_VoiceBoxEngine::OnCommandComplete(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    std::string transactionID = parser.getValueByKey("transactionid");
    std::string errorCode = parser.getValueByKey("errorstatus");
    VR_LOG("OnCommandComplete: %s [%s]\n", transactionID.c_str(), errorCode.c_str());

    VoiceMap<std::string, MessageHandler>::const_iterator iterMap =
        m_mapCmdResultHandler.find(transactionID);
    if (m_mapCmdResultHandler.cend() != iterMap) {
        if (NULL != iterMap->second) {
            (this->*(iterMap->second))(parser);
            VR_LOG("delete finish command info");
            m_mapCmdResultHandler.erase(iterMap);
        }
    }

    if (std::string::npos != transactionID.find("SetDataActiveSource")) {
        if (NULL != m_pcCatalogController) {
            std::string strCurrentMsg = m_pcCatalogController->GetCurrentMessage();
            VR_LOG("strCurrentMsg = %s", strCurrentMsg.c_str());
            if (std::string::npos != strCurrentMsg.find("<grammar_active agent=\"media\"")
                || std::string::npos != strCurrentMsg.find("<grammar_disactive agent=\"media\"")) {
                SetUpdateGammarFlg(true);
            }
        }
    }

    return true;
}

// When SendRecognitionState is completed, this function will be called.
bool
VR_VoiceBoxEngine::OnSendRecognitionState(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if ("" == m_strCurrentStateName) {
        return true;
    }

    // if ((S_OK != ulCode) && (S_FALSE != ulCode)) {
    //     return false;
    // }
    VR_LOG("current vbt name : %s", m_strCurrentStateName.c_str());

    if (VBT_RECO_STATE_VOICE_TAG == m_strCurrentStateName) {
        if (m_bPlayTTS) {
            VR_LOG("playing tts, wait");
            m_sessionState = VR_SessionStateRestartSessionAfterTTS;
        }
        else {
            VR_LOG("no tts");
            m_pcMsgController->PostMessage("<send event=\"start-internal\" target=\"de\"/>");
        }
    }
    if (VBT_RECO_STATE_IN_CALL == m_strCurrentStateName) {
        if (m_bPlayTTS) {
            VR_LOG("in call playing tts, wait");
            m_sessionState = VR_SessionStateRestartSessionAfterTTS;
            m_strPreSessionTransactionID = m_strCurrentTransactionID;
        }
        else {
            VR_LOG("in call no tts");
            m_pcMsgController->PostMessage("<send event=\"start-internal\" target=\"de\"/>");
        }
    }
    else if (VBT_RECO_STATE_SPEAKER_ADAPTATION != m_strCurrentStateName) {
        if (VR_REGION_OC == m_country) {
            if (m_bPlayTTS) {
                VR_LOG("playing tts, wait");
                m_sessionState = VR_SessionStateRestartSessionAfterTTS;
            }
            else {
                VR_LOG("no tts");
                m_pcMsgController->PostMessage("<send event=\"start-internal\" target=\"de\"/>");
            }
            return true;
        }
        m_pcMsgController->PostMessage("<send event=\"start-internal\" target=\"de\"/>");
    }
    else {
        if (NULL == m_engineCommand.ptr()) {
            return false;
        }

        m_strPreSessionTransactionID = m_strCurrentTransactionID;

        CVECIPtr<IVECITransaction> transaction;
        HRESULT result = m_engineCommand->SendCommand(
                         &transaction,
                         VBT_RECO_STATE_SPEAKER_ADAPTATION,
                         VBT_COMMAND_NOOP_START_SPEAKER_ADAPTATION,
                         VBT_FALSE,
                         NULL,
                         NULL);
        if (FAILED(result)) {
            return false;
        }

        if (FAILED(result)) {
            return false;
        }

        if (FAILED(result)) {
            return false;
        }

        CVECIOutStr strTrans;
        result = transaction->GetTransactionId(&strTrans);
        if (FAILED(result)) {
            return false;
        }

        VR_LOG("speak adaptation strTrans: %s", strTrans.Get());
        if (NULL != strTrans.Get()) {
            m_strCurrentTransactionID = strTrans.Get();
        }

        m_mapCmdResultHandler.insert(
                std::make_pair(
                    strTrans.Get(),
                    &VR_VoiceBoxEngine::OnSpeakAdaptation
                    )
                );
    }
    return true;
}

// When StartRecoSession is completed, this function will be called.
bool
VR_VoiceBoxEngine::OnStartRecoSession(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    VR_LOG("current vbt name : %s", m_strCurrentStateName.c_str());

    if (NULL == m_pcMsgController) {
        return false;
    }

    if (m_lstTransaction.empty()) {
        VR_LOG("start sessin Transaction is empty");
        return false;
    }
    else {
        VoiceList<std::string>::iterator iterF = m_lstTransaction.begin();
        std::string strTransaction = parser.getValueByKey("transactionid");

        if (strTransaction != *iterF) {
            VoiceList<std::string>::iterator iterFind;
            iterFind = std::find(m_lstTransaction.begin(), m_lstTransaction.end(), strTransaction);
            if (iterFind != m_lstTransaction.end()) {
                m_lstTransaction.erase(iterFind);
            }
            VR_LOG("start sessin Transaction is not the new");
            return false;
        }
        else {
            m_lstTransaction.erase(iterF);
            VR_LOG("start sessin Transaction is the new");
        }
    }

    if (VBT_RECO_STATE_SPEAKER_ADAPTATION == m_strCurrentStateName) {
        if (VR_SessionStateStarting == m_sessionState) {
            std::string strAction = "<action agent=\"help\" op=\"showTrainingEndMsg\" />";
            std::string strActionName = "showTrainingEndMsg";
            VR_LOG("action op %s", strActionName.c_str());
            m_mapAction.insert(std::make_pair(
                    strActionName,
                    strAction));
            OnRequestAction(strAction);
            m_strAction = strAction;

            CVECIPtr<IVECITransaction> transaction;
            VR_LOG("SaveSpeakerProfile: %s", m_strAdaptationPath.c_str());
            HRESULT result = m_engineCommand->SaveSpeakerProfile(
                             &transaction,
                             m_strAdaptationPath.c_str());
            OnBeep(VR_BeepType_Done);
            if (FAILED(result) || (NULL == transaction.ptr())) {
                VR_ERROR("SaveSpeakerProfile error");
                return false;
            }

            return true;
        }
    }

    if (VBT_RECO_STATE_VOICE_TAG == m_strCurrentStateName) {
        VR_LOG("This is voice tag.After TTS, quit.");
        if (m_bDoCancelVoiceTag) {
            OnRequestAction(VR_ACTION_CLOSESESSION);
            m_bDoCancelVoiceTag = false;
        }
        m_sessionState = VR_SessionStateStoped;
        m_strCurrentStateName = "";
        return true;
    }

    VR_LOG("current session: %d", m_sessionState);
    switch (m_sessionState) {
    case VR_SessionStateNone:
    {
        VR_ERROR("session is not started");
        break;
    }
    case VR_SessionStateAutoSendRecogState:
    {
        VR_LOG("send recog state");
        SendRecogState();
        break;
    }
    case VR_SessionStateStartOver:
    {
        VR_LOG("start over");
        // Prepare the hints
        if (!GetHints()) {
            return false;
        }

        m_strCurrentStateName = m_strStartOverStateName;
        SendRecogState();
        break;
    }
    case VR_SessionStateAutoTutoSendRecogState:
    {
        VR_LOG("tutorials to topmenu");
        // Prepare the hints
        if (!GetHints()) {
            return false;
        }

        SendRecogState();
        break;
    }
    case VR_SessionStateAutoRestartSession:
    {
        VR_LOG("start session");
        StartSession();
        break;
    }
    case VR_SessionStatePaused:
    {
        SendVRState("paused", "", "false", "");
        break;
    }
    case VR_SessionStateStarting:
    case VR_SessionStateStartWithiBargeIn:
    {
        VR_LOG("stop session");

        // Notify the XML message to the external service
        OnBeep(VR_BeepType_Done);
        break;
    }
    case VR_SessionStateCanceled:
    {
        if (!m_bDoCanceling) {
            OnBeep(VR_BeepType_Done);
        }
        break;
    }
    case VR_SessionStateStoped:
    case VR_SessionStateBackQuit:
    {
        VR_LOG("session is stopped");
        break;
    }
    default:
    {
        VR_LOG("session waiting start");
        break;
    }
    }

    return true;
}

// When Repeat is completed, this function will be called.
bool
VR_VoiceBoxEngine::OnRepeat(VR_VoiceBoxXmlParser& parser)
{
    // VBT_ULONG ulCode = S_OK;
    // if ((S_OK != ulCode) && (S_FALSE != ulCode)) {
    //     return false;
    // }

    // if (strTrans.Get() != m_strCurrentTransactionID) {
    //     return false;
    // }

    // if ("" == m_strLastTransactionID) {
    //     return false;
    // }

    // m_strLastTransactionID = "";

    OnRequest("<send event=\"start-internal\" target=\"de\"/>");
    return true;
}

// When Back is completed, this function will be called.
bool
VR_VoiceBoxEngine::OnBack(VR_VoiceBoxXmlParser& parser)
{
    VR_LOG("m_sessionState = %d", m_sessionState);
    // VBT_ULONG ulCode = S_OK;
    // if ((S_OK != ulCode) && (S_FALSE != ulCode)) {
    //     return false;
    // }

    // if (strTrans.Get() != m_strCurrentTransactionID) {
    //     return false;
    // }

    // if ("" == m_strLastTransactionID) {
    //     return false;
    // }

    // m_strLastTransactionID = "";

    m_bBackInterupted = false;

    if ((VR_SessionStateBackRestart == m_sessionState)
        || (VR_SessionStateAutoRestartSession == m_sessionState)) {
        StartSession();
    }

    return true;
}

// When Help is completed, this function will be called.
bool
VR_VoiceBoxEngine::OnHelp(VR_VoiceBoxXmlParser& parser)
{
    // VBT_ULONG ulCode = S_OK;
    // if ((S_OK != ulCode) && (S_FALSE != ulCode)) {
    //     return false;
    // }

    // if (strTrans.Get() != m_strCurrentTransactionID) {
    //     return false;
    // }

    // OnRequest("<send event=\"start-internal\" target=\"de\"/>");

    if (m_bTTSNull) {
        StartSession();
    }
    else {
        m_sessionState = VR_SessionStateRestartSessionAfterTTS;
    }
    return true;
}

bool
VR_VoiceBoxEngine::OnHelpRecognized(VR_VoiceBoxXmlParser& parser)
{
    if (VR_REGION_US == m_country) {
        ShowHelpMoreHintsScreen();
    }

    return true;
}

bool
VR_VoiceBoxEngine::OnSelect(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (!m_bTaskCompelete) {
        if (m_bTTSNull) {
            StartSession();
        }
        else {
            m_sessionState = VR_SessionStateRestartSessionAfterTTS;
        }
    }
    else {
        if (m_bTTSNull) {
            VR_LOG("stop session");

            // Notify the XML message to the external service
            OnBeep(VR_BeepType_Done);
        }
        else {
            m_sessionState = VR_SessionStateQuitAfterTTS;
        }
    }
    return true;
}


bool
VR_VoiceBoxEngine::OnSpeakAdaptation(VR_VoiceBoxXmlParser& parser)
{
    VR_LOG("voice traning completed");
    if (!m_bTTSNull) {
        m_sessionState = VR_SessionStateRestartSessionAfterTTS;
    }
    else {
        StartSession();
    }

    return true;
}

bool
VR_VoiceBoxEngine::OnMoreHints(VR_VoiceBoxXmlParser& parser)
{
    if (!m_bTTSNull) {
        m_sessionState = VR_SessionStateRestartSessionAfterTTS;
    }
    else {
        StartSession();
    }
    return true;
}

bool
VR_VoiceBoxEngine::OnNextPage(VR_VoiceBoxXmlParser& parser)
{
    if (!m_bTTSNull) {
        m_sessionState = VR_SessionStateRestartSessionAfterTTS;
    }
    else {
        StartSession();
    }
    return true;
}

bool
VR_VoiceBoxEngine::OnPrevPage(VR_VoiceBoxXmlParser& parser)
{
    if (!m_bTTSNull) {
        m_sessionState = VR_SessionStateRestartSessionAfterTTS;
    }
    else {
        StartSession();
    }
    return true;
}

bool
VR_VoiceBoxEngine::OnLastPage(VR_VoiceBoxXmlParser& parser)
{
    if (!m_bTTSNull) {
        m_sessionState = VR_SessionStateRestartSessionAfterTTS;
    }
    else {
        StartSession();
    }
    return true;
}

bool
VR_VoiceBoxEngine::OnFristPage(VR_VoiceBoxXmlParser& parser)
{
    if (!m_bTTSNull) {
        m_sessionState = VR_SessionStateRestartSessionAfterTTS;
    }
    else {
        StartSession();
    }
    return true;
}

bool
VR_VoiceBoxEngine::OnAlongRoute(VR_VoiceBoxXmlParser& parser)
{
    if (!m_bTaskCompelete) {
        if (m_bTTSNull) {
            StartSession();
        }
        else {
            m_sessionState = VR_SessionStateRestartSessionAfterTTS;
        }
    }
    else {
        if (m_bTTSNull) {
            VR_LOG("stop session");

            // Notify the XML message to the external service
            OnBeep(VR_BeepType_Done);
        }
        else {
            m_sessionState = VR_SessionStateQuitAfterTTS;
        }
    }
    return true;
}

bool
VR_VoiceBoxEngine::OnNearDestination(VR_VoiceBoxXmlParser& parser)
{
    if (!m_bTaskCompelete) {
        if (m_bTTSNull) {
            StartSession();
        }
        else {
            m_sessionState = VR_SessionStateRestartSessionAfterTTS;
        }
    }
    else {
        if (m_bTTSNull) {
            VR_LOG("stop session");

            // Notify the XML message to the external service
            OnBeep(VR_BeepType_Done);
        }
        else {
            m_sessionState = VR_SessionStateQuitAfterTTS;
        }
    }
    return true;
}

bool
VR_VoiceBoxEngine::OnConfirmYes(VR_VoiceBoxXmlParser& parser)
{
    if (!m_bTaskCompelete) {
        if (m_bTTSNull) {
            StartSession();
        }
        else {
            m_sessionState = VR_SessionStateRestartSessionAfterTTS;
        }
    }
    else {
        if (m_bTTSNull) {
            VR_LOG("stop session");

            // Notify the XML message to the external service
            OnBeep(VR_BeepType_Done);
        }
        else {
            m_sessionState = VR_SessionStateQuitAfterTTS;
        }
    }
    return true;
}

bool
VR_VoiceBoxEngine::OnGoDirectly(VR_VoiceBoxXmlParser& parser)
{
    if (!m_bTaskCompelete) {
        if (m_bTTSNull) {
            StartSession();
        }
        else {
            m_sessionState = VR_SessionStateRestartSessionAfterTTS;
        }
    }
    else {
        if (m_bTTSNull) {
            VR_LOG("stop session");

            // Notify the XML message to the external service
            OnBeep(VR_BeepType_Done);
        }
        else {
            m_sessionState = VR_SessionStateQuitAfterTTS;
        }
    }
    return true;
}

bool
VR_VoiceBoxEngine::OnAddToRoute(VR_VoiceBoxXmlParser& parser)
{
    if (!m_bTaskCompelete) {
        if (m_bTTSNull) {
            StartSession();
        }
        else {
            m_sessionState = VR_SessionStateRestartSessionAfterTTS;
        }
    }
    else {
        if (m_bTTSNull) {
            VR_LOG("stop session");

            // Notify the XML message to the external service
            OnBeep(VR_BeepType_Done);
        }
        else {
            m_sessionState = VR_SessionStateQuitAfterTTS;
        }
    }
    return true;
}

bool
VR_VoiceBoxEngine::OnCall(VR_VoiceBoxXmlParser& parser)
{
    VR_LOG("VR_VoiceBoxEngine::OnCall:");
    if (!m_bTTSNull) {
        m_sessionState = VR_SessionStateQuitAfterTTS;
    }
    else {
        VR_LOG("stop session");

        // Notify the XML message to the external service
        OnBeep(VR_BeepType_Done);
    }
    return true;
}

bool
VR_VoiceBoxEngine::OnSendMessage(VR_VoiceBoxXmlParser& parser)
{
    VR_LOG("VR_VoiceBoxEngine::OnSendMessage:");
    if (!m_bTTSNull) {
        m_sessionState = VR_SessionStateRestartSessionAfterTTS;
    }
    else {
        StartSession();
    }

    return true;
}

bool
VR_VoiceBoxEngine::OnReply(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();
    if (!m_bTTSNull) {
        m_sessionState = VR_SessionStateRestartSessionAfterTTS;
    }
    else {
        StartSession();
    }

    return true;
}

bool
VR_VoiceBoxEngine::OnReadNext(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (!m_bTTSNull) {
        m_sessionState = VR_SessionStateRestartSessionAfterTTS;
    }
    else {
        StartSession();
    }
    return true;
}

bool
VR_VoiceBoxEngine::OnReadPrevious(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (!m_bTTSNull) {
        m_sessionState = VR_SessionStateRestartSessionAfterTTS;
    }
    else {
        StartSession();
    }
    return true;
}

bool
VR_VoiceBoxEngine::OnConfirmNo(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (VR_SessionStateAutoTutoSendRecogState == m_sessionState) {
        if (m_bTTSNull) {
            VR_LOG("get hints");
            GetHints();

            SendRecogState();
        }
        else {
            VR_LOG("after TTS, send recog and get hints");
            m_sessionState = VR_SessionStateSendRecogAfterTTS;
        }
    }
    else {
        if (m_bTTSNull) {
            VR_LOG("start session");
            StartSession();
        }
        else {
            VR_LOG("after TTS, start session");
            m_sessionState = VR_SessionStateRestartSessionAfterTTS;
        }
    }
    return true;
}

bool
VR_VoiceBoxEngine::OnInstallAgent(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    // restart engine
    std::string strRestartEngine = "<send event=\"restartEngine\"/>";
    OnRecognized(strRestartEngine);


    // test
    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    // Start the dialog engine with the specified culture
    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->RestartEngine(
                     &transaction,
                     m_strCultureName.c_str()
                     );
    if (FAILED(result) || (NULL == transaction.ptr())) {
        // VR_ERROR("Start Engine Failed, result: %lx, transaction: %p", result, transaction);
        return false;
    }

    // Make it a sync operation, wait until the engine is started
    VBT_BOOL bTimeout = VBT_FALSE;
    result = transaction->WaitForCompletion(INFINITE, &bTimeout);
    if (FAILED(result) || bTimeout) {
        // VR_ERROR("Waiting Engine Start Failed, result: %lx, timeout: %n", result, bTimeout);
        return false;
    }

    return true;
}

bool
VR_VoiceBoxEngine::OnStartEngine(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();
    std::string errorCode = parser.getValueByKey("errorstatus");
    if (m_bChangeLanguage) {
        m_bChangeLanguage = false;
        if ("0x0" == errorCode) {
            ChangeLanguageResult(true);
        }
        else {
            ChangeLanguageResult(false);
            m_bEngineStarting = false;
        }
    }
    else {
        m_bEngineStarting = ("0x0" == errorCode) ? true : false;
    }

    NotifyResourceState();

    return true;
}

void
VR_VoiceBoxEngine::ChangeLanguageResult(const bool bResult)
{

    std::string strChangeLanguage = "<event name=\"changeLanguage-internal\">";
    strChangeLanguage.append("<language>");
    strChangeLanguage.append(m_lstLanguage.back());
    strChangeLanguage.append("</language>");
    strChangeLanguage.append("</event>");
    bool bFinish = false;
    if (bResult) {
        if (m_strLanguage == m_lstLanguage.back()) {
            m_bEngineStarting = true;
            bFinish = true;
        }
        else {
            m_bEngineStarting = false;
            OnRequest(strChangeLanguage);
            VR_LOG("start engine not OK");
        }
    }
    else {
        if (m_strLanguage != m_lstLanguage.back()) {
            OnRequest(strChangeLanguage);
        }
        else {
            bFinish = true;
        }
    }
    if (bFinish) {
        VoiceList<std::string>::iterator iter = m_lstLanguage.begin();
        for (; m_lstLanguage.end() != iter; ++iter) {
            SendChangeLanguageResult(*iter);
        }

        m_lstLanguage.clear();
    }
}

bool
VR_VoiceBoxEngine::OnPttLong(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (m_bTTSNull) {
        // Notify the XML message to the external service
        OnBeep(VR_BeepType_Done);
    }
    else {
        m_sessionState = VR_SessionStateQuitAfterTTS;
    }

    return true;
}

// Start the VoiceBox engine with the specified culture
bool
VR_VoiceBoxEngine::Restart(const std::string &strCultureName)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        // VR_ERROR("The engine have not been initialized");
        return false;
    }

    m_mapActiveSouceTrans.clear();
    m_mapGrammarSourceId.clear();
    m_bEngineStarting = false;
    NotifyResourceState();

    // Start the dialog engine with the specified culture
    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->RestartEngine(
                     &transaction,
                     strCultureName.c_str()
                     );
    if (FAILED(result) || (NULL == transaction.ptr())) {
        VR_ERROR("Retart Engine Failed, result: %lx, transaction: %p", result, transaction.ptr());
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnStartEngine
            )
        );

    // Make it a sync operation, wait until the engine is started
    VBT_BOOL bTimeout = VBT_FALSE;
    result = transaction->WaitForCompletion(INFINITE, &bTimeout);
    if (FAILED(result) || bTimeout) {
        VR_ERROR("Waiting Engine ReStart Failed, result: %lx, timeout: %n", result, bTimeout);
        return false;
    }

    m_strCultureName = strCultureName;
    m_strInstallCulture = strCultureName;

    // Disable VoiceBox's stream audio out function
    // According to VBT's suggestion
    std::string strMuteResponses = "true";
    if ("en-AU" == strCultureName) {
        strMuteResponses = "false";
    }
    result = m_engineCommand->SetSystemParameter(
             NULL,
             _T("SystemParam"),
             VBT_SYS_PREF_SYSTEMPARAM_MUTERESPONSES,
             strMuteResponses.c_str()
             );

    if (FAILED(result)) {
        VR_ERROR("SetSystemParameter Failed, result: %lx", result);
    }

    if ("en-AU" == strCultureName) {
        UpdateMapGrammar();
    }

    if ("" == m_strDevice) {
        m_strAdaptationPath = GetAdaptationProfilePath(strCultureName, "default");
    }
    else {
        m_strAdaptationPath = GetAdaptationProfilePath(strCultureName, m_strDevice);
    }
    result = m_engineCommand->LoadSpeakerProfile(
             &transaction,
             m_strAdaptationPath.c_str());
    if (FAILED(result) || (NULL == transaction.ptr())) {
        VR_ERROR("error :SpeakAdaptation error");
        return false;
    }

    m_pcMsgController->PostMessage("<grammar_disactive agent=\"media\" reply=\"false\" grammarid=\"5\"/>");

    return true;
}

bool
VR_VoiceBoxEngine::StartAppRecognition(const std::string& message)
{
    VR_LOGD_FUNC();

    if (message.empty()) {
        return false;
    }

    m_bWaitGreetingEnd = false;
    m_bDoCancelTslVR = false;
    VoiceVector<std::string>::type messages;

    VR_VoiceBoxAppsXml appsXml;
    if (!appsXml.GetVBTXml(message, messages)) {
        return false;
    }

    if (1 == messages.size()) {
        if ("VBT Button Down" != appsXml.GetVBTActionType(messages[0])) {
            return false;
        }

        if (!m_mapPlayTTSTransation.empty()) {
            VR_LOG("VBT Button Down - Stop TTS");
            StopAllTTS();
        }

        CVECIPtr<IVECITransaction> transaction;
        HRESULT result = m_engineCommand->CancelSession(&transaction);
        if (FAILED(result) || (NULL == transaction.ptr())) {
            VR_LOG("VBT Button Down - Cancel Session Failed");
        }

        return StartAppRecoSession();
    }
    else if (messages.size() > 1) {
        if ("VBT Send Command" != appsXml.GetVBTActionType(messages[0])) {
            return false;
        }
        if ("VBT Button Down" != appsXml.GetVBTActionType(messages[1])) {
            return false;
        }

        VR_LOG("Send App Command");
        CVECIPtr<IVECITransaction> transaction;
        HRESULT result = m_engineCommand->SendXmlMessage(&transaction, messages[0].c_str());
        if (FAILED(result) || (NULL == transaction.ptr())) {
            return false;
        }

        CVECIOutStr strTrans;
        result = transaction->GetTransactionId(&strTrans);
        if (FAILED(result)) {
            return false;
        }

        m_bTslDialog = true;
        m_agents[AgentType_Global]->ProcessMessage(messages[1]);
        m_mapCmdResultHandler.insert(
            std::make_pair(
                strTrans.Get(),
                &VR_VoiceBoxEngine::OnSendAppCommand
                )
            );

        return true;
    }
    else {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxEngine::CancelAppRecognition(const std::string& message)
{
    VR_LOGD_FUNC();

    if (message.empty()) {
        return false;
    }

    m_bDoCancelTslVR = true;
    m_bWaitGreetingEnd = false;
    m_sessionState = VR_SessionStateStoped;
    VoiceVector<std::string>::type messages;

    VR_VoiceBoxAppsXml appsXml;
    appsXml.GetVBTXml(message, messages);

    if (messages.size() > 0) {
        if ("VBT Cancel" != appsXml.GetVBTActionType(messages[0])) {
            return false;
        }
    }

    {
        if ((!m_bTaskCompelete) && (!m_mapPlayTTSTransation.empty())) {
            VR_LOG("VBT Cancel - Stop TTS");
            StopAllTTS();
        }

        m_bTslDialog = false;

        VR_LOG("Cancel Recognition Session");
        CVECIPtr<IVECITransaction> transaction;
        HRESULT result = m_engineCommand->CancelSession(&transaction);
        if (FAILED(result) || (NULL == transaction.ptr())) {
            return false;
        }
    }

    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->GetVersion(&transaction);
    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    m_bQuitWaitForPrompt = false;
    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnGetVersion
            )
        );

    return true;
}

bool
VR_VoiceBoxEngine::SendAppXmlMessage(const std::string& message)
{
    VR_LOGD_FUNC();

    if (message.empty()) {
        return false;
    }

    VoiceVector<std::string>::type messages;

    VR_VoiceBoxAppsXml appsXml;
    if (!appsXml.GetVBTXml(message, messages)) {
        return false;
    }

    if (messages.size() > 0) {
        std::string msgClass = appsXml.GetMessageClass(messages[0]);
        if ("Driver" == msgClass) {
            VR_LOG("Send XML : Driver Reply");
            HRESULT result = m_engineCommand->DriverXmlReply(messages[0].c_str());
            if (FAILED(result)) {
                return false;
            }

            return true;
        }

        std::string actionType = appsXml.GetVBTActionType(messages[0]);
        if ("VBT Cancel" == actionType) {
            VR_LOG("Send XML : VBT Cancel");

            if (!m_mapPlayTTSTransation.empty()) {
                VR_LOG("Send XML : VBT Cancel - Stop TTS");
                m_bWaitGreetingEnd = false;
                StopAllTTS();
            }

            m_bWaitConfirmation = false;
            CVECIPtr<IVECITransaction> transaction;
            HRESULT result = m_engineCommand->CancelSession(&transaction);
            if (FAILED(result) || (NULL == transaction.ptr())) {
                return false;
            }
        }
        else if ("Rendering Done" == actionType) {
            VR_LOG("Send XML : Rendering Done");
            std::string clientRenderDone = "<send event=\"clientRenderDone\" target=\"de\"/>";
            m_agents[AgentType_Global]->ProcessMessage(clientRenderDone);
        }
        else if ("VBT System Resume" == actionType) {
            VR_LOG("Send XML : Global System Resume");
            m_bWaitConfirmation = false;
            CVECIPtr<IVECITransaction> transaction;
            HRESULT result = m_engineCommand->CancelSession(&transaction);
            if (FAILED(result) || (NULL == transaction.ptr())) {
                return false;
            }

            CVECIPtr<IVECITransaction> resumeTransaction;
            result = m_engineCommand->Resume(&resumeTransaction, VBT_TRUE);
            if (FAILED(result) || (NULL == transaction.ptr())) {
                return false;
            }
        }
        else if ("Play Tone" == actionType) {
            std::string toneName = appsXml.GetVBTActionParamValue(messages[0], "Tone Name");
            VR_LOG("Send XML : Play Tone [%s]", toneName.c_str());
            if ("Done" == toneName) {

            }
            else if ("Transition" == toneName) {

            }
            else if ("Off-board processing" == toneName) {

            }
            else {
                // do nothing
            }
        }
        else if ("VBT Send Command"  == actionType) {
            VR_LOG("Send XML : VBT Send Command");

            if (!m_mapPlayTTSTransation.empty()) {
                VR_LOG("Send XML : Command - Stop TTS");
                m_bWaitGreetingEnd = false;
                StopAllTTS();
            }

            m_bWaitConfirmation = false;
            CVECIPtr<IVECITransaction> transaction;
            HRESULT result = m_engineCommand->CancelSession(&transaction);
            if (FAILED(result) || (NULL == transaction.ptr())) {
                return false;
            }

            std::string command = appsXml.GetVBTCommand(messages[0]);
            if ("System Global Back" == command) {
                VR_LOG("Send XML : Global Back");
                CVECIPtr<IVECITransaction> backTransaction;
                CVECIPtr<IVECITransaction> preTransaction;
                result = m_engineCommand->Back(&backTransaction, VBT_TRUE, &preTransaction);
                if (FAILED(result) || (NULL == backTransaction.ptr())) {
                    return false;
                }

                if (NULL == preTransaction.ptr()) {
                    VR_LOG("Send XML : Global Back - Exit");
                    return true;
                }

                CVECIOutStr strTrans;
                result = backTransaction->GetTransactionId(&strTrans);
                if (FAILED(result)) {
                    return false;
                }

                m_mapCmdResultHandler.insert(
                    std::make_pair(
                        strTrans.Get(),
                        &VR_VoiceBoxEngine::OnAppBack
                        )
                    );

                return true;
            }
            else if ("System Repeat" == command) {
                VR_LOG("Send XML : System Repeat");
                CVECIPtr<IVECITransaction> resumeTransaction;
                result = m_engineCommand->Resume(&resumeTransaction, VBT_TRUE);
                if (FAILED(result) || (NULL == resumeTransaction.ptr())) {
                    return false;
                }

                return true;
            }
            else {

            }

            CVECIPtr<IVECITransaction> sendXml;
            result = m_engineCommand->SendXmlMessage(&sendXml, messages[0].c_str());
            if (FAILED(result) || (NULL == sendXml.ptr())) {
                return false;
            }

            CVECIOutStr strTransId;
            result = sendXml->GetTransactionId(&strTransId);
            if (FAILED(result)) {
                return false;
            }

            m_mapCmdResultHandler.insert(
                std::make_pair(
                    strTransId.Get(),
                    &VR_VoiceBoxEngine::OnSendAppXmlMsg
                    )
                );
        }
        else if ("VBT Send Recognition State" == actionType) {
            VR_LOG("Send Recognition State");
            if (!m_mapPlayTTSTransation.empty()
                && (std::string::npos == m_strActionType.find("Start Over"))) {
                StopAllTTS();
                m_strTslAppRecoStateMsg = message;
                m_bAppRecoState = VR_AppRecoState_SendCaching;
                VR_LOG("Send Recognition State : Caching");
                return true;
            }

            m_bDoCancelTslVR = false;
            m_bTaskCompelete = false;

            CVECIPtr<IVECITransaction> transaction;
            HRESULT result = m_engineCommand->SendXmlMessage(&transaction, messages[0].c_str());
            if (FAILED(result) || (NULL == transaction.ptr())) {
                return false;
            }

            CVECIOutStr strTrans;
            result = transaction->GetTransactionId(&strTrans);
            if (FAILED(result)) {
                return false;
            }

            m_bAppRecoState = VR_AppRecoState_Sending;

            m_mapCmdResultHandler.insert(
                std::make_pair(
                    strTrans.Get(),
                    &VR_VoiceBoxEngine::OnSendAppRecognitionState
                    )
                );
        }
        else {
            VR_LOG("Send XML : others");
            CVECIPtr<IVECITransaction> transaction;
            HRESULT result = m_engineCommand->SendXmlMessage(&transaction, messages[0].c_str());
            if (FAILED(result) || (NULL == transaction.ptr())) {
                return false;
            }
        }
    }

    return true;
}

bool
VR_VoiceBoxEngine::OnSendAppRecognitionState(VR_VoiceBoxXmlParser& parser)
{
    std::string transactionID = parser.getValueByKey("transactionid");
    std::string errorCode = parser.getValueByKey("errorstatus");
    VR_LOG("OnSendAppRecognitionState: %s [%s]\n", transactionID.c_str(), errorCode.c_str());

    if ("0x0" != errorCode) {
        m_bAppRecoState = VR_AppRecoState_Invalid;

        while (!m_listAppMessages.empty()) {
            VR_MsgInfo msgInfo = m_listAppMessages.front();
            m_pcMsgController->PostMessage(msgInfo.strMsg, msgInfo.iMsgId);
            m_listAppMessages.pop_front();
        }

        std::string strForceEnd = "<action name=\"notifyAppRecognitionForceEnd\"/>";
        OnRequestAction(strForceEnd);
        return false;
    }
    else {
        m_bAppRecoState = VR_AppRecoState_Valid;

        while (!m_listAppMessages.empty()) {
            VR_MsgInfo msgInfo = m_listAppMessages.front();
            m_pcMsgController->PostMessage(msgInfo.strMsg, msgInfo.iMsgId);
            m_listAppMessages.pop_front();
        }
    }

    return true;
}

bool
VR_VoiceBoxEngine::OnSendAppCommand(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    std::string errorCode = parser.getValueByKey("errorstatus");
    VR_LOG("OnSendAppCommand: [%s]\n", errorCode.c_str());

    if (("0x0" != errorCode) || m_bTaskCompelete) {
        return false;
    }

    if (!m_mapPlayTTSTransation.empty()) {
        m_bWaitGreetingEnd = true;
        return true;
    }

    return StartAppRecoSession();
}

bool
VR_VoiceBoxEngine::OnSendAppXmlMsg(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    std::string errorCode = parser.getValueByKey("errorstatus");
    VR_LOG("OnSendAppXmlMsg: [%s]\n", errorCode.c_str());

    if (("0x0" != errorCode) || m_bTaskCompelete) {
        return false;
    }

    if (!m_mapPlayTTSTransation.empty()) {
        m_bWaitGreetingEnd = true;
        return true;
    }

    return StartAppRecoSession();
}

bool
VR_VoiceBoxEngine::StartAppRecoSession()
{
    VR_LOGD_FUNC();

    VARIANT var;
    var.vt = VT_BOOL;
    var.boolVal = VARIANT_TRUE;
    m_client->SetProperty(PropClientManagedRender, &var);
    var.boolVal = VARIANT_FALSE;
    m_client->SetProperty(PropBackChangeContext, &var);

    CVECIPtr<IVECITransaction> transaction;
    VBT_BOOL bContinuous = VBT_TRUE;
    HRESULT result = m_engineCommand->StartRecoSession(&transaction, bContinuous);
    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    if (NULL != strTrans.Get()) {
        m_strCurrentTransactionID = strTrans.Get();
        m_lstTransaction.push_front(m_strCurrentTransactionID);
    }

    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnStartAppRecoSession
            )
        );

    return true;
}

bool
VR_VoiceBoxEngine::OnStartAppRecoSession(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    std::string transactionID = parser.getValueByKey("transactionid");
    std::string errorCode = parser.getValueByKey("errorstatus");
    VR_LOG("OnStartAppRecoSession: %s [%s]\n", transactionID.c_str(), errorCode.c_str());

    return true;
}

bool
VR_VoiceBoxEngine::OnGetVersion(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (!m_mapPlayTTSTransation.empty()) {
        m_bQuitWaitForPrompt = true;
        return true;
    }

    if (m_bQuitWaitForPrompt || m_bWaitForDoneBeep) {
        VR_LOG("OnGetVersion: Ignore redundant Done beep");
        return true;
    }

    m_bWaitForDoneBeep = true;
    OnBeep(VR_BeepType_TSL_Done);

    return true;
}

bool
VR_VoiceBoxEngine::OnAppBack(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    std::string errorCode = parser.getValueByKey("errorstatus");
    VR_LOG("OnAppBack: [%s]\n", errorCode.c_str());

    if (("0x0" != errorCode) || m_bTaskCompelete) {
        return false;
    }

    if (!m_mapPlayTTSTransation.empty()) {
        m_bWaitGreetingEnd = true;
        return true;
    }

    return StartAppRecoSession();
}

HRESULT
VR_VoiceBoxEngine::CreateVocon4FrontEndShare(
    void* userData,
    IVBTVocon4FrontEndShare** ppShare
)
{
    VR_LOGD_FUNC();

    *ppShare = &s_frontEndShare;
    return S_OK;
}

void
VR_VoiceBoxEngine::OnBosDetected()
{
    VR_LOGD_FUNC();

    // If there is no playing, do nothing
    if (!m_bPlayTTS) {
        return;
    }

    m_strBtnEventName = "";
    m_bBosDetected = true;

    std::string strStopTTS = "<action agent=\"prompt\" op=\"stopTts\"><reqId>";
    strStopTTS.append(std::to_string(m_iCurTTSReqId));
    strStopTTS.append("</reqId></action>");

    int iStopID = OnRequestAction(strStopTTS);
    VR_LOG("stop current speak id, %d", m_iCurTTSReqId);
    m_mapStopTTSTransation.insert(std::make_pair(iStopID, m_iCurTTSReqId));

    CancelRecoSession();

    m_bStartSessionWithBargeIn = true;
    m_pcMsgController->PostMessage("<send event=\"start-bargein\" target=\"de\"/>");
}

void
VR_VoiceBoxEngine::setPromptLevel(int promptLevel)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        // VR_ERROR("The engine have not been initialized");
        return;
    }
    m_iPromptLevel = promptLevel;
    std::string level;
    if (VR_PROMPTLEVEL_OFF == promptLevel) {
        level = SILENT_VERBOSITY;
        VR_LOG("level : %s", level.c_str());
    }
    else if (VR_PROMPTLEVEL_LOW == promptLevel) {
        level = LOW_VERBOSITY;
        VR_LOG("level : %s", level.c_str());
    }
    else if (VR_PROMPTLEVEL_HIGH == promptLevel) {
        level = HIGH_VERBOSITY;
        VR_LOG("level : %s", level.c_str());
    }
    else {
        VR_LOG("return");
        return;
    }
    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->SetSystemParameter(
                     &transaction,
                     _T("SystemParam"),
                     VBT_SYS_PREF_SYSTEMPARAM_VERBOSITY,
                     level.c_str());
    if (FAILED(result) || (NULL == transaction.ptr())) {
        // VR_ERROR("Start Engine Failed, result: %lx, transaction: %p", result, transaction);
        return;
    }
}

// Start a voice recognition session
bool
VR_VoiceBoxEngine::StartRecoSessionWithBargeIn(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    if (!m_bStartSessionWithBargeIn) {
        return false;
    }
    m_bStartSessionWithBargeIn = false;

    char* pcAecData = NULL;
    unsigned int nAecDataSize = 0;
    VR_AudioStreamIF* pcAudioStream = VR_AudioStreamIF::Instance();
    if (NULL != pcAudioStream) {
        unsigned int size = pcAudioStream->GetAecAudioTypeDataSize();
        if (0 == size) {
            return false;
        }

        pcAecData = VR_new char[size];
        if (NULL == pcAecData) {
            return false;
        }

        if (!pcAudioStream->GetAecAudioTypeData(nAecDataSize, pcAecData)) {
            delete[] pcAecData;
            pcAecData = NULL;
            return false;
        }
    }

    VR_LOG("Get Aec data successfully!");

    VARIANT var;
    var.vt = VT_BOOL;
    var.boolVal = VARIANT_FALSE;
    m_client->SetProperty(PropClientManagedRender, &var);
    var.boolVal = VARIANT_FALSE;
    m_client->SetProperty(PropBackChangeContext, &var);

    CVECIPtr<IVECITransaction> transaction;
    // For VoiceTag, the continue flag should be VBT_FALSE
    VBT_BOOL bContinuous = VBT_TRUE;
    HRESULT result = m_engineCommand->StartRecoSessionWithBargeIn(
        &transaction,
        bContinuous,
        VBT_ULONG(pcAecData),
        nAecDataSize
    );

    delete[] pcAecData;
    pcAecData = NULL;

    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    VR_LOG("Start Reco Session with barge in successfully!");
    m_sessionState = VR_SessionStateStartWithiBargeIn;

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    if (NULL != strTrans.Get()) {
        m_strCurrentTransactionID = strTrans.Get();
        m_lstTransaction.push_front(m_strCurrentTransactionID);
    }

    VR_LOG("Start session : %s", strTrans.Get());
    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnStartRecoSession
            )
        );

    return true;
}

bool
VR_VoiceBoxEngine::HandleAction(const std::string& strAction)
{
    VR_LOGD_FUNC();

    VR_VoiceBoxXmlParser parser(strAction);
    std::string strActionName = parser.getXmlKey();
    VR_LOG("action op %s", strActionName.c_str());
    m_mapAction.insert(std::make_pair(
            strActionName,
            strAction));

    if (VR_SessionStateAutoSendRecogState != m_sessionState
        && VR_SessionStateAutoTutoSendRecogState != m_sessionState
        && m_bTaskCompelete) {
        m_strAction = strAction;
    }
    else {
        OnRequestAction(strAction);
    }
    return true;
}

void
VR_VoiceBoxEngine::HandleQuitVR()
{
    VR_LOGD_FUNC();

    m_strMsgPrompt = "";
    m_bPlayTTS = false;
    m_iStartBeepID = VR_INVALID_ACTION_ID;
    m_iDoneBeepID = VR_INVALID_ACTION_ID;
    m_strCurrentStateName = "";
    m_bDoCanceling = false;
    m_bBackInterupted = false;
    m_strScreenState.clear();
    NotifyResourceState();

    m_sessionState = VR_SessionStateStoped;

    if (VR_REGION_US == m_country) {
        m_PVRStateCurrent = VR_PVRState_None;
    }

    SendVRState("quit", "", "false", "");

    if ("" == m_strAction) {
        std::string strQuitVRApp = "<display xmlns=\"\" agent=\"Common\" content=\"QuitVRApp\">"
                                "</display>";

        OnRecognized(strQuitVRApp);
    }
    else {
        m_strAction = "";
    }

    if (NULL != VR_ConfigureIF::Instance()) {
        if (VR_ConfigureIF::Instance()->getVROverPrompt()) {
            VR_AudioStreamIF* pcAudioStream = VR_AudioStreamIF::Instance();
            if (NULL != pcAudioStream) {
                pcAudioStream->StopAudioIn();
            }
        }
    }
    StopAllTTS();

    m_bSession = false;

    if (NULL != m_pcCatalogController) {
        VR_LOG("do not need to pause grammar");
        m_pcCatalogController->ProcessOpMessage("<op name=\"needpause\"><value>false</value></op>");

        if (m_bWaitResumeGrammar) {
            VR_LOG("quit of VR, then resume grammar update");
            m_pcCatalogController->ProcessOpMessage("<op name=\"needresume\"><value>true</value></op>");
        }
        else {
            SetUpdateGammarFlg(true);
        }
    }
}

bool
VR_VoiceBoxEngine::HandleQuitVRApp(const std::string& strMessage)
{
    VR_LOGD_FUNC();

    if (m_mapAction.empty()) {
        VR_LOG("there is not action");
        return false;
    }

    VR_VoiceBoxXmlParser parser(strMessage);
    std::string strActionKey = parser.getXmlKey();

    VR_LOG("action op %s", strActionKey.c_str());

    VoiceMap<std::string, std::string>::const_iterator cit = m_mapAction.find(strActionKey);

    if (cit != m_mapAction.cend()) {
        std::string strErrCode = parser.getValueByKey("errcode");
        std::string strAgent = parser.getValueByKey("agent");
        VR_LOG("action errcode %s", strErrCode.c_str());
        bool bNeedVRQuit = false;
        if ("false" == strErrCode || "climate" == strAgent) {
            bNeedVRQuit = true;
        }
        else if ("true" == strErrCode) {
            if (VR_REGION_OC == m_country) {
                bNeedVRQuit = (("media" == strAgent) || "phone" == strAgent) ? true :false;
            }
            else {
                if (m_lstUCAppQuitVROp.end() != std::find(m_lstUCAppQuitVROp.begin(), m_lstUCAppQuitVROp.end(), strActionKey)) {
                    bNeedVRQuit = false;
                }
                else {
                    bNeedVRQuit = true;
                }
            }
        }
        else {

        }

        if (bNeedVRQuit) {
            std::string strQuitVRApp = "<display xmlns=\"\" agent=\"Common\" content=\"QuitVRApp\">"
                                       "</display>";

            OnRecognized(strQuitVRApp);

            m_strAction = "";
        }
        m_mapAction.erase(cit);
    }
    else {
        VR_LOG("it is not action");
        return false;
    }

    return true;
}

bool
VR_VoiceBoxEngine::BrowseAlbums(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    BrowseMusicByType("Browse Albums");

    return true;
}

bool
VR_VoiceBoxEngine::BrowseArtists(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    BrowseMusicByType("Browse Artists");

    return true;
}

bool
VR_VoiceBoxEngine::BrowseSongs(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    BrowseMusicByType("Browse Songs");

    return true;
}

bool
VR_VoiceBoxEngine::BrowseComposers(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    BrowseMusicByType("Browse Composers");

    return true;
}

bool
VR_VoiceBoxEngine::BrowseGenres(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    BrowseMusicByType("Browse Genres");

    return true;
}

bool
VR_VoiceBoxEngine::BrowsePodcasts(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    BrowseMusicByType("Browse Podcasts");

    return true;
}

bool
VR_VoiceBoxEngine::BrowsePlaylists(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    BrowseMusicByType("Browse Playlists");

    return true;
}

bool
VR_VoiceBoxEngine::BrowseAudiobooks(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    BrowseMusicByType("Browse Audiobooks");

    return true;
}

bool
VR_VoiceBoxEngine::BrowseMusicByType(const std::string& strMusicType)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->SendCommand(
                     &transaction,
                     _T("Music"),
                     strMusicType.c_str(),
                     VBT_FALSE,
                     NULL,
                     NULL);
    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    m_mapCmdResultHandler.insert(
            std::make_pair(
                strTrans.Get(),
                &VR_VoiceBoxEngine::OnMusicType));

    return true;
}

bool
VR_VoiceBoxEngine::OnMusicType(VR_VoiceBoxXmlParser& parser)
{
    if (!m_bTaskCompelete) {
        if (m_bTTSNull) {
            StartSession();
        }
        else {
            m_sessionState = VR_SessionStateRestartSessionAfterTTS;
        }
    }
    else {
        if (m_bTTSNull) {
            VR_LOG("stop session");

            // Notify the XML message to the external service
            OnBeep(VR_BeepType_Done);
        }
        else {
            m_sessionState = VR_SessionStateQuitAfterTTS;
        }
    }

    return true;
}

int
VR_VoiceBoxEngine::OnRequestAction(const std::string& strMsg)
{
    VR_LOGD_FUNC();

    m_ActionIDSync.SynchronizeStart();
    int iID = m_iCurReqId;
    ++m_iCurReqId;

    const int VR_VBT_MAX_REQUEST_ID = 100000;
    if (m_iCurReqId >= VR_VBT_MAX_REQUEST_ID) {
        m_iCurReqId = 1;
    }
    m_ActionIDSync.SynchronizeEnd();

    if (NULL != m_pcExternalCallback) {
        m_pcExternalCallback->OnRequestAction(strMsg, iID);
    }

    return iID;
}

void
VR_VoiceBoxEngine::InsertMusicUpdateGrammar(
    const std::string& strTransId,
    const std::string& strSourceId)
{
    VR_LOG("InsertMusicUpdateGrammar: strTransId = %s, strSourceId = %s", strTransId.c_str(), strSourceId.c_str());

    if (!m_mapGrammarSourceId.empty()) {
        VoiceMap<std::string, VoiceList<std::string>::type >::iterator iter = m_mapGrammarSourceId.find(strSourceId);
        if (m_mapGrammarSourceId.end() != iter) {
            iter->second.push_back(strTransId);
        }
        else {
            VoiceList<std::string>::type lstTmp;
            lstTmp.push_back(strTransId);

            m_mapGrammarSourceId.insert(std::make_pair(strSourceId, lstTmp));
        }
    }
    else {
        VoiceList<std::string>::type lstTmp;
        lstTmp.push_back(strTransId);

        m_mapGrammarSourceId.insert(std::make_pair(strSourceId, lstTmp));
    }
}

void
VR_VoiceBoxEngine::ConfirmMusicUpdateGrammar(
    const std::string& strTransId, const bool bGrammarErrorCode)
{
    VR_LOG("m_mapGrammarSourceId size = %d", m_mapGrammarSourceId.size());
    VoiceMap<std::string, VoiceList<std::string>::type >::iterator iter = m_mapGrammarSourceId.begin();
    for (; iter != m_mapGrammarSourceId.end();) {
        VoiceList<std::string>::iterator iterLst = std::find(iter->second.begin(), iter->second.end(), strTransId);
        if (iterLst != iter->second.end()) {
            VR_LOG("ConfirmMusicUpdateGrammar: strTransId = %s", strTransId.c_str());
            m_bGrammarErrorCode = m_bGrammarErrorCode && bGrammarErrorCode;
            iter->second.erase(iterLst);
            if (iter->second.empty()) {
                if ("5" != iter->first) {
                    VR_LOGP("DE: finish update of music grammar... case : 212-12-99 212-13-99 212-14-99 212-15-99 212-137-99 212-139-99");

                    std::string strErrCode = m_bGrammarErrorCode ? "0" : "1";

                    SendGrammarResult("grammar", "media", iter->first, strErrCode);
                    SetGrammarInitSourceId("5");

                    SetUpdateGammarFlg(true);
                }

                m_bGrammarErrorCode = true;

                m_mapGrammarSourceId.erase(iter++);
            }
            else {
                ++iter;
            }
        }
        else {
            ++iter;
        }
    }

    return;
}

void
VR_VoiceBoxEngine::SetUpdateGammarFlg(bool bFinishUpdate)
{
    if (NULL != m_pcCatalogController) {
        if (bFinishUpdate) {
            m_pcCatalogController->ProcessOpMessage("<op name=\"finishupdate\"><value>true</value></op>");
        }
        else {
            m_pcCatalogController->ProcessOpMessage("<op name=\"finishupdate\"><value>false</value></op>");
        }

    }
}

// strAudioNameSrc :in/out parameter
void
VR_VoiceBoxEngine::ChangeAudioSourceName(std::string& strAudioName)
{
    if ("USB" == strAudioName && m_bIpod1 && m_bUsb2) {
        strAudioName = "USB2";
    }
    else if ("iPod" == strAudioName && m_bUsb1 && m_bIpod2) {
        strAudioName = "iPod2";
    }
    else {

    }

    return;
}

std::string
VR_VoiceBoxEngine::GetAudioConnected(const std::string& strAudioName)
{
    std::string strAudioConnect;
    if ("USB" == strAudioName) {
        if (m_bUsb1) {
            strAudioConnect = "True";
        }
        else {
            if (m_bIpod1 && m_bUsb2) {
                strAudioConnect = "True";
            }
            else {
                strAudioConnect = "False";
            }
        }
    }
    else if ("USB2" == strAudioName) {
        if (m_bIpod1 && m_bUsb2) {
            strAudioConnect = "False";
        }
        else {
            strAudioConnect = (m_bUsb2) ? "True" : "False";
        }
    }
    else if ("iPod" == strAudioName) {
        if (m_bIpod1) {
            strAudioConnect = "True";
        }
        else {
            if (m_bUsb1 && m_bIpod2) {
                strAudioConnect = "True";
            }
            else {
                strAudioConnect = "False";
            }
        }
    }
    else if ("iPod2" == strAudioName) {
        if (m_bUsb1 && m_bIpod2) {
            strAudioConnect = "False";
        }
        else {
            strAudioConnect = (m_bIpod2) ? "True" : "False";
        }
    }
    else if ("Bluetooth Audio" == strAudioName) {
        strAudioConnect = (m_bBtAudio) ? "True" : "False";
    }
    else {
        strAudioConnect = "False";
    }

    return strAudioConnect;
}

std::string
VR_VoiceBoxEngine::GetAudioConnected()
{
    return m_strUsbOrIpodConnected;
}

bool
VR_VoiceBoxEngine::InScreen(const std::string& strContent)
{
    std::string strScreenContent = strContent;
    if (VR_REGION_US == m_country) {
        strScreenContent = "na_" + strScreenContent;
    }

    if (strScreenContent != m_strScreenState) {
        return false;
    }

    return true;
}

void
VR_VoiceBoxEngine::BuildScreenContent(std::string& strContent)
{
    if ("" == strContent) {
        VR_LOG("content is empty");
        return;
    }

    std::string strScreenContent = strContent;

    if (VR_REGION_US == m_country) {
        strScreenContent = "na_" + strScreenContent;
    }

    strContent = strScreenContent;
}

bool
VR_VoiceBoxEngine::PVRAction(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();
    std::string strStatusName = parser.getValueByKey("StatusName");

    m_bTslDialog = false;
    m_bWaitGreetingEnd = false;
    m_bDoCancelTslVR = false;

    if ("PVREnter" == strStatusName) {
        m_bPVRScreen = true;
        m_bHavePVR = true;
        m_bTaskCompelete = false;
        SetUpdateGammarFlg(false);

        if (NULL != m_pcCatalogController) {
            VR_LOG("need to pause %s grammar: %s", m_pairTransaction.first.c_str(), m_pairTransaction.second.c_str());
            m_pcCatalogController->ProcessOpMessage("<op name=\"needpause\"><value>true</value></op>");
        }

        m_PVRStateCurrent = VR_PVRState_WaitPlayTTS;
        if (NULL == m_engineCommand.ptr()) {
            return false;
        }
        CVECIPtr<IVECITransaction> transaction;
        HRESULT result = m_engineCommand->SendCommand(
                         &transaction,
                         VBT_AGENT_HELP,
                         "No-op Getting Started With Voice",
                         VBT_FALSE,
                         NULL,
                         NULL);
        if (FAILED(result) || (NULL == transaction.ptr())) {
            return false;
        }
    }
    else if ("PVRExit" == strStatusName) {
        CancelRecoSession();
    }
    else {
    }

    std::string strEventResult = "<event-result name=\"PvrTTS\">"
                                     "<StatusName>";
    strEventResult.append(strStatusName);
    strEventResult.append("</StatusName>");
    strEventResult.append("</event-result>");

    OnRequestAction(strEventResult);

    return true;
}

void
VR_VoiceBoxEngine::GetPromptByScreenId(const std::string& strScreenId, std::string& strPrompt)
{
    if (strScreenId.empty()) {
        return;
    }

    strPrompt.clear();
    m_bNBestFlg = false;

    if (VR_REGION_OC == m_country) {
        VoiceMap<std::string, std::string>::iterator iterOC = m_mapScreenPromptOC.find(strScreenId);
        if (iterOC != m_mapScreenPromptOC.end()) {
            strPrompt = iterOC->second;
        }
    }
    else if (VR_REGION_US == m_country) {
        int index = 0;
        if ("en-US" == m_strCultureName) {
            index = 2;
        }
        else if ("fr-CA" == m_strCultureName) {
            index = 1;
        }
        else if ("en-MX" == m_strCultureName) {
            index = 0;
        }
        else {

        }

        VoiceMap<std::string, VoiceVector<std::string>::type >::type::iterator iter = m_mapScreenPrompt.find(strScreenId);
        if (iter != m_mapScreenPrompt.end()) {
            strPrompt = iter->second[index];
        }
    }

    m_bNBestFlg = ("VR-SYS-01" == strScreenId) ? true : false;

    VR_LOG("GetPromptByScreenId: strScreenId = %s, strPrompt = %s", strScreenId.c_str(), strPrompt.c_str());

    return;
}

void
VR_VoiceBoxEngine::VRSateForHint(const std::string& strScreenId)
{
    std::string strState;
    VR_ConfigureIF * pcConfig = VR_ConfigureIF::Instance();
    if (NULL != pcConfig && pcConfig->getVROverPrompt()) {
        strState = "promptWithBargein";
    }
    else {
        strState = "promptPlaying";
    }

    std::string strPrompt;
    GetPromptByScreenId(strScreenId, strPrompt);
    VR_VoiceBoxEventSink::m_strPrompt = strPrompt;

    SendVRState(strState, strPrompt, "false", "");

    return;
}

bool
VR_VoiceBoxEngine::TransationSpeakDone()
{
    VR_LOGD_FUNC();
    VoiceMap<int, IVECITransaction*>::const_iterator iterMap =
        m_mapPlayTTSTransation.begin();

    while (iterMap != m_mapPlayTTSTransation.cend()) {
        if (NULL != iterMap->second) {
            m_bPlayTTS = false;
            iterMap->second->SpeakDone();
            iterMap->second->Release();
        }
        m_mapPlayTTSTransation.erase(iterMap++);
    }

    m_mapStopTTSTransation.clear();
    return true;
}

bool
VR_VoiceBoxEngine::TransationSpeakDone(const int& iActionID)
{
    VR_LOGD_FUNC();

    VR_LOG("speak size : %d", m_mapPlayTTSTransation.size());
    IVECITransaction* pTran = NULL;
    VoiceMap<int, IVECITransaction*>::const_iterator iterMap =
        m_mapPlayTTSTransation.find(iActionID);
    if (m_mapPlayTTSTransation.cend() != iterMap) {
        VR_LOG("find IVECITransaction");
        pTran = iterMap->second;
        m_mapPlayTTSTransation.erase(iterMap);
    }
    else {
        return false;
    }

    if (NULL != pTran) {
        HRESULT result = pTran->SpeakDone();
        m_bPlayTTS = false;
        pTran->Release();
        if (FAILED(result)) {
            return false;
        }
    }
    else {
        return false;
    }
    VR_LOG("speak size : %d", m_mapPlayTTSTransation.size());

    return true;
}

bool
VR_VoiceBoxEngine::TransationStopSpeakDone(const int& iActionID)
{
    VR_LOGD_FUNC();

    VR_LOG("speak size : %d", m_mapStopTTSTransation.size());
    m_bPlayTTS = false;
    int iPlayTTSID = 0;
    VoiceMap<int, int>::const_iterator iterMap =
        m_mapStopTTSTransation.find(iActionID);
    if (m_mapStopTTSTransation.cend() != iterMap) {
        VR_LOG("find IVECITransaction");
        iPlayTTSID = iterMap->second;
        m_mapStopTTSTransation.erase(iterMap);
    }
    else {
        return false;
    }

    VR_LOG("speak size : %d", m_mapStopTTSTransation.size());
    return TransationSpeakDone(iPlayTTSID);
}

void
VR_VoiceBoxEngine::StopAllTTS()
{
    VR_LOGD_FUNC();

    VoiceMap<int, IVECITransaction*>::const_iterator iterMap =
        m_mapPlayTTSTransation.cbegin();
    if (m_mapPlayTTSTransation.cend() != iterMap) {
        std::string strStopTTS = "<action agent=\"prompt\" op=\"stopTts\"><reqId>";
        strStopTTS.append(std::to_string(iterMap->first));
        strStopTTS.append("</reqId></action>");
        OnRequestAction(strStopTTS);

        if (NULL != iterMap->second) {
            iterMap->second->SpeakDone();
            iterMap->second->Release();
        }

        ++iterMap;
    }
    m_bPlayTTS = false;

    m_mapPlayTTSTransation.clear();
    m_mapStopTTSTransation.clear();
}

bool
VR_VoiceBoxEngine::AgentHelp(VR_VoiceBoxXmlParser& parser)
{
    std::string strAgent = parser.getValueByKey("agent");

    VoiceMap<std::string, std::string>::iterator iter = m_mapAgenttoRecoState.find(strAgent);
    if (m_mapAgenttoRecoState.end() != iter) {
        m_strCurrentStateName = iter->second;
    }

    VR_LOG("get hints");
    // Prepare the hints
    if (!GetHints()) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxEngine::HintPage(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    std::string strPageType = parser.getValueByKey("pageType");
    const int  MOREHINTPAGESIZE = 5;
    if ("firstPage" == strPageType) {
        VR_LOG("first hints page");
        m_iIndex = 0;
    }
    else if ("lastPage" == strPageType) {
        VR_LOG("last hints page");
        int iMod = m_iHintSize % MOREHINTPAGESIZE;
        if (0 == iMod) {
            m_iIndex = m_iHintSize - MOREHINTPAGESIZE;
        }
        else {
            m_iIndex = MOREHINTPAGESIZE * (m_iHintSize / MOREHINTPAGESIZE);
        }
    }
    else if ("nextPage" == strPageType) {
        VR_LOG("next hints page");
    }
    else if ("prevPage" == strPageType) {
        VR_LOG("previous hints page");
        int iMod = m_iIndex % MOREHINTPAGESIZE;
        iMod = (iMod == 0) ? MOREHINTPAGESIZE : iMod;
        m_iIndex = m_iIndex - iMod - MOREHINTPAGESIZE;
    }
    else {
        return false;
    }

    DisplayHints("VR-HNT-04");

    return true;
}

bool
VR_VoiceBoxEngine::UpdateMapData(VR_VoiceBoxXmlParser& parser)
{
    std::string strCountryID = parser.getValueByKey("countryID");
    VR_LOG("current %s, next %s", m_strCountryID.c_str(), strCountryID.c_str());
    if (strCountryID != m_strCountryID) {
        VR_ConfigureIF* pcConfig = VR_ConfigureIF::Instance();
        std::string strLanguage = pcConfig->getVRLanguage();
        if (VR_LANGUAGE_EN_AU == strLanguage) {
            Restart("en-AU");
        }
    }

    return true;
}

bool
VR_VoiceBoxEngine::InitialPersonData(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (!m_bEngineStarting) {
        VR_LOG("engine isn't started");
        OnRecognized("<event-result name=\"initialpersondata\" />");
        return true;
    }

    if (NULL != m_pcCatalogManager && !m_pairTransaction.first.empty() && !m_pairTransaction.second.empty()) {
        m_pcCatalogManager->CancelGrammarUpdate(m_pairTransaction.first);
    }

    DeleteALLSmartAgent();
    if (NULL != m_pcMsgController) {
        ResetTslAgents();

        if (NULL != m_pcCatalogManager) {
            m_pcCatalogManager->InitialPersonData();
        }

        Restart(m_strCultureName);
    }

    OnRecognized("<event-result name=\"initialpersondata\" />");

    return true;
}

bool
VR_VoiceBoxEngine::Prepare(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    OnRecognized("<event-result name=\"prepare\" errcode=\"0\"></event-result>");

    return true;
}

bool
VR_VoiceBoxEngine::IncomingMessageInfo(VR_VoiceBoxXmlParser& parser)
{
    m_messageinfo.instanceId = parser.getValueByKey("instanceId");
    m_messageinfo.messageId = parser.getValueByKey("messageId");
    m_messageinfo.phoneNumber = parser.getValueByKey("phoneNumber");
    m_messageinfo.messageType = parser.getValueByKey("messageType");
    m_messageinfo.sender = parser.getValueByKey("sender");
    m_messageinfo.phoneType = parser.getValueByKey("phoneType");
    m_messageinfo.subject = parser.getValueByKey("subject");
    m_messageinfo.contactID = parser.getValueByKey("contactId");
}

bool
VR_VoiceBoxEngine::GetUpdateMapState() const
{
    return m_bUpdatingMapData;
}

bool
VR_VoiceBoxEngine::OnDMStartedNotify(VR_VoiceBoxXmlParser& parser)
{
    if (!m_bEngineReady) {
        return false;
    }

    if (m_bEngineStarting) {
        std::string strStartFinish = "<action agent=\"destatus\" op=\"notifyStartFinish\"></action>";
        OnRequestAction(strStartFinish);
        SendDEStatus("init", "0");
        SendDEStatus("engine", "0");
    }

    if (VR_REGION_US == m_country) {
        std::string strSupportedLanguage = "<action name=\"notifyTSLVRInfo\">"
                                              "<TSLINFO>"
                                                "<Action type=\"Notify Supported Language\"/>"
                                                "<LanguageList>"
                                                  "<Item>"
                                                    "<Parameter name=\"Language\" value=\"en\"/>"
                                                  "</Item>"
                                                  "<Item>"
                                                    "<Parameter name=\"Language\" value=\"fr\"/>"
                                                  "</Item>"
                                                  "<Item>"
                                                    "<Parameter name=\"Language\" value=\"es\"/>"
                                                  "</Item>"
                                                "</LanguageList>"
                                              "</TSLINFO>"
                                            "</action>";
        OnRequestAction(strSupportedLanguage);
    }
    else if (VR_REGION_OC == m_country) {
        std::string strSupportedLanguage = "<action name=\"notifyTSLVRInfo\">"
                                              "<TSLINFO>"
                                                "<Action type=\"Notify Supported Language\"/>"
                                                "<LanguageList>"
                                                  "<Item>"
                                                    "<Parameter name=\"Language\" value=\"en\"/>"
                                                  "</Item>"
                                                "</LanguageList>"
                                              "</TSLINFO>"
                                            "</action>";
        OnRequestAction(strSupportedLanguage);

        // query route exist state when first start
        OnInfoQueryMessage("<action agent=\"navi\" op=\"queryRouteExist\" />");
    }
    else {
        // do nothing
    }

    if ((NULL != m_engineCommand.ptr())) {
        CVECIPtr<IVECITransaction> transaction;
        HRESULT result = m_engineCommand->SetPreference(
                         &transaction,
                         _T("System"),
                         VBT_USR_PREF_SYSTEM_PHONECONNECTED,
                         "False");
        if (FAILED(result) || (NULL == transaction.ptr())) {
            VR_LOG("set phoneconnect error");
        }
        result = m_engineCommand->SetPreference(
                 &transaction,
                 _T("HFD"),
                 VBT_USR_PREF_HFD_SMSAVAILABLE,
                 "False");
        if (FAILED(result) || (NULL == transaction.ptr())) {
            VR_LOG("set phone smsavailable error");
        }

        result = m_engineCommand->SetPreference(&transaction, _T("Music"), VBT_USR_PREF_MUSIC_AUDIOSOURCECONNECTED, "False");
        if (FAILED(result) || (NULL == transaction.ptr())) {
            VR_LOG("set Music error");
        }
    }

    return true;
}

bool
VR_VoiceBoxEngine::OnStartOver(VR_VoiceBoxXmlParser& parser)
{
    if (m_bTTSNull) {
        if (VR_SessionStateStartOver == m_sessionState) {
            VR_LOG("get hints");
            // Prepare the hints
            if (!GetHints()) {
                return false;
            }

            SendRecogState();
        }
    }
    else {
        m_sessionState = VR_SessionStateSendRecogAfterTTS;
    }

    return true;
}

void
VR_VoiceBoxEngine::NotifyInstallAgentResult(bool bInstallResult)
{
    // If install agent failed, we need to delete all the installed agents.
    if (!bInstallResult) {
        ResetTslAgents();
    }

    if (VR_REGION_US == m_country) {
        // After install, reset the engine to current language
        RestartForInstallTslAgent(m_strCultureName);
        m_strInstallCulture = m_strCultureName;
    }

    // Notify the install process for the last one,
    // so that restart engine operation is included in TSL install process.
    if (bInstallResult && !m_vecTSLInstallInfo.empty()) {
        int lastIndex = m_vecTSLInstallInfo.size() - 1;
        NotifyInstallProgress(m_vecTSLInstallInfo[lastIndex].path, lastIndex);
    }

    std::string strInstallFormat = "<event-result name=\"install\" errcode=\"0\">"
                                     "<TSLINFO>"
                                       "<Action type=\"Installed Result\">"
                                         "<Parameter name=\"Result\" value=\"%s\"/>"
                                       "</Action>"
                                     "</TSLINFO>"
                                   "</event-result>";
    const int nInstallFormatLen = 256;
    char result[nInstallFormatLen] = { 0 };
    snprintf(result, sizeof(result), strInstallFormat.c_str(), (bInstallResult? "True" : "False"));
    OnRecognized(result);

    m_vecTSLInstallInfo.clear();
    m_iInstallVecIndex = 0;
    m_bInstallingAgent = false;

    // Process the caching update state messages
    while (!m_listUpdateState.empty()) {
        std::string strUpdateState = m_listUpdateState.front();
        m_listUpdateState.pop_front();
        VR_VoiceBoxXmlParser parser(strUpdateState);
        UpdateState(parser);
    }

    SetUpdateGammarFlg(true);
}

bool
VR_VoiceBoxEngine::InstallDownloadableAgents(const std::string& message)
{
    // Install TSL agent
    VR_LOG("Install TSL agents");

    m_iInstallVecIndex = 0;
    m_vecTSLInstallInfo.clear();

    VR_VoiceBoxXmlParser parser(message);
    std::string strActionType = parser.getValueByKey("type");
    if ("Install Agent" != strActionType) {
        return false;
    }

    m_bInstallingAgent = true;

    // Before install, cancel the grammar related operation
    if (NULL != m_pcCatalogManager && !m_pairTransaction.first.empty() && !m_pairTransaction.second.empty()) {
        m_pcCatalogManager->CancelGrammarUpdate(m_pairTransaction.first);
        SetUpdateGammarFlg(false);
    }

    bool bUninstallAll = false;
    VoiceVector<Parameter>::type parameterVector = parser.getMsgParameterValue();
    if ("Uninstall Previous Agent" == parameterVector[0].nameValue) {
        if ("True" == parameterVector[0].value) {
            bUninstallAll = true;
        }

        VR_LOG("Install - Uninstall Previous agents : %d", bUninstallAll);
    }

    for (int i = 1; i + 1 < parameterVector.size(); i += 2) {
        TSLInstallAgentInfo info;
        if ("Path" == parameterVector[i].nameValue) {
            info.path = parameterVector[i].value;
            VR_LOG("Path: %s", info.path.c_str());
        }

        if ("Language" == parameterVector[i + 1].nameValue) {
            info.culture = GetCulture(parameterVector[i + 1].value);
            VR_LOG("Language: %s", info.culture.c_str());
        }

        m_vecTSLInstallInfo.push_back(info);
    }

    NotifyInstallProgress("", -1);

    if (bUninstallAll) {
        if (UninstallAllAgents()) {
            if (m_vecTSLInstallInfo.empty()) {
                NotifyInstallAgentResult(true);
            }
        }
        else {
            NotifyInstallAgentResult(false);
            return false;
        }
    }
    else {
        if (m_vecTSLInstallInfo.empty()) {
            NotifyInstallAgentResult(true);
        }
        else {
            if (!DoInstallAgent(m_vecTSLInstallInfo[m_iInstallVecIndex])) {
                NotifyInstallAgentResult(false);
                return false;
            }
        }
    }

    return true;
}

bool
VR_VoiceBoxEngine::UninstallAllAgents()
{
    VR_LOGD_FUNC();

    std::string culture;
    if (VR_REGION_US == m_country) {
        if (!UninstallAllAgentsInSync("es-MX")) {
            return false;
        }

        if (!UninstallAllAgentsInSync("fr-CA")) {
            return false;
        }

        culture = "en-US";
    }
    else if (VR_REGION_OC == m_country) {
        culture = "en-AU";
    }
    else {
        return false;
    }

    // Uninstall all TSL agent
    VR_LOG("Uninstall all TSL agents");
    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->UninstallAllDownloadableAgent(
        &transaction,
        NULL,
        culture.c_str()
    );
    if (FAILED(result) || (NULL == transaction.ptr())) {
        VR_LOG("Uninstall all Failed");
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnAllAgentsUninstalled
            )
        );

    return true;
}

bool
VR_VoiceBoxEngine::UninstallAllAgentsInSync(std::string culture)
{
    VR_LOGD_FUNC();

    // Uninstall all the agents
    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->UninstallAllDownloadableAgent(
        &transaction,
        NULL,
        culture.c_str()
    );

    if (FAILED(result) || (NULL == transaction.ptr())) {
        VR_LOG("Uninstall all %s agents failed: %xl", culture.c_str(), result);
        return false;
    }

    VBT_BOOL bTimeout = VBT_FALSE;
    result = transaction->WaitForCompletion(INFINITE, &bTimeout);
    if (FAILED(result) || bTimeout) {
        VR_LOG("Wait uninstall %s agents failed: %xl", culture.c_str(), result);
        return false;
    }

    return true;
}

bool
VR_VoiceBoxEngine::DoInstallAgent(TSLInstallAgentInfo& installInfo)
{
    VR_LOGD_FUNC();

    if (m_strInstallCulture != installInfo.culture) {
        if (!RestartForInstallTslAgent(installInfo.culture)) {
            return false;
        }

        m_strInstallCulture = installInfo.culture;
    }

    // Prepare the agent file path
    CVECIPtr<IVECIStringSet> spDAPFile;
    m_client->CreateStringSet(&spDAPFile);
    spDAPFile->AddItem(installInfo.path.c_str());

    // Install the agent
    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->InstallDownloadableAgent(
        &transaction,
        NULL,
        _T("/tmp/"),
        installInfo.culture.c_str(),
        spDAPFile
    );
    if (FAILED(result) || (NULL == transaction.ptr())) {
        VR_LOG("Install Failed");
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    m_mapCmdResultHandler.insert(
        std::make_pair(
            strTrans.Get(),
            &VR_VoiceBoxEngine::OnAgentInstalled
            )
        );

    return true;
}

bool
VR_VoiceBoxEngine::OnAllAgentsUninstalled(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    std::string errorCode = parser.getValueByKey("errorstatus");
    unsigned long error = strtoul(errorCode.c_str(), NULL, 0);

    if (S_OK != error) {
        VR_LOG("Install Downloadable Agent Failed: %d", error);
        NotifyInstallAgentResult(false);
        return false;
    }

    if (!m_vecTSLInstallInfo.empty()) {
        m_iInstallVecIndex = 0;
        if (!DoInstallAgent(m_vecTSLInstallInfo[m_iInstallVecIndex])) {
            NotifyInstallAgentResult(false);
            return false;
        }
    }

    return true;
}

bool
VR_VoiceBoxEngine::OnAgentInstalled(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    std::string errorCode = parser.getValueByKey("errorstatus");
    unsigned long error = strtoul(errorCode.c_str(), NULL, 0);

    if (S_OK != error) {
        VR_LOG("Install Downloadable Agent Failed: %d", error);
        NotifyInstallAgentResult(false);
        return false;
    }

    // Notify TSL service install process, except the last one
    if (m_vecTSLInstallInfo.size() > (m_iInstallVecIndex + 1)) {
        NotifyInstallProgress(m_vecTSLInstallInfo[m_iInstallVecIndex].path, m_iInstallVecIndex);
    }

    ++m_iInstallVecIndex;
    if (m_vecTSLInstallInfo.size() > m_iInstallVecIndex) {
        if (!DoInstallAgent(m_vecTSLInstallInfo[m_iInstallVecIndex])) {
            NotifyInstallAgentResult(false);
            return false;
        }
    }
    else {
        NotifyInstallAgentResult(true);
    }

    return true;
}

std::string
VR_VoiceBoxEngine::GetCulture(std::string& language)
{
    std::string culture;
    if (VR_REGION_US == m_country) {
        if ("en" == language) {
            culture = "en-US";
        }
        else if ("fr" == language) {
            culture = "fr-CA";
        }
        else if ("es" == language) {
            culture = "es-MX";
        }
        else {
            // do nothing
        }
    }
    else if (VR_REGION_OC == m_country) {
        if ("en" == language) {
            culture = "en-AU";
        }
    }
    else {
    }

    return culture;
}

void
VR_VoiceBoxEngine::ClosePopupMsg()
{
    VR_LOGD_FUNC();

    std::string strDismissPopupMessage = "<display agent=\"Common\" content=\"DismissPopupMessage\">";
    strDismissPopupMessage.append("<prompt>");
    strDismissPopupMessage.append(m_strMsgPrompt);
    strDismissPopupMessage.append("</prompt>");
    strDismissPopupMessage.append("<type>normal</type>");
    strDismissPopupMessage.append("</display>");

    OnRequestAction(strDismissPopupMessage);
    m_strMsgPrompt = "";
}

void
VR_VoiceBoxEngine::UpdateMapGrammar()
{
    VR_ConfigureIF* pcConfig = VR_ConfigureIF::Instance();
    std::string strMessage = "<action-result agent=\"navi\" op=\"requestDefaultInfo\" ><countryId>";
    if (NULL != pcConfig) {
        if (VR_PRODUCT_TYPE_L1 != pcConfig->getVRProduct()) {
            std::string strMapdata = pcConfig->getMapDataPath();
            if (1 == pcConfig->getCountryIDForVBT()) {
                VR_LOG("AU");
                m_strCountryID = "1";
                strMapdata = strMapdata + "/ENA_AU.osd";
                strMessage.append("1");
            }
            else {
                VR_LOG("NZ");
                m_strCountryID = "2";
                strMapdata = strMapdata + "/ENA_NZ.osd";
                strMessage.append("2");
            }

            std::fstream fileOSD;
            fileOSD.open(strMapdata, std::ios::in);
            if (fileOSD) {
                fileOSD.close();

                VR_LOG("set CountryGrammarArchive");
                m_engineCommand->SetPreference(
                     NULL,
                     VBT_AGENT_SYSTEM,
                     VBT_USR_PREF_SYSTEM_COUNTRYGRAMMARARCHIVE,
                     strMapdata.c_str()
                     );
            }
            else {
                VR_LOG("file is not exist!");
                m_engineCommand->SetPreference(
                     NULL,
                     VBT_AGENT_SYSTEM,
                     VBT_USR_PREF_SYSTEM_COUNTRYGRAMMARARCHIVE,
                     ""
                     );
            }

            VR_LOG("set POI Grammar");
            OnRequest("<grammar_init agent=\"poi\" />");
        }
    }
    else {
        strMessage.append("1");
    }
    strMessage.append("</countryId></action-result>");
    VR_VoiceBoxXmlParser parser(strMessage);
    m_agents[AgentType_Navi]->ReplyQueryInfo(parser);
}

void
VR_VoiceBoxEngine::SendGrammarResult(
    const std::string& strOp, const std::string& strAgent,
    const std::string& strGrammarId, const std::string& strErrCode)
{
    std::string strGrammarResult = "<grammar_result op=\"";
    strGrammarResult.append(strOp);
    strGrammarResult.append("\" agent=\"");
    strGrammarResult.append(strAgent);
    strGrammarResult.append("\" grammarid=\"");
    strGrammarResult.append(strGrammarId);
    strGrammarResult.append("\" errcode=\"");
    strGrammarResult.append(strErrCode);
    strGrammarResult.append("\" />");
    VR_LOG("strGrammarResult = %s", strGrammarResult.c_str());
    OnRecognized(strGrammarResult);
}

bool
VR_VoiceBoxEngine::OnEscalatingError(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (VR_REGION_US == m_country) {
        ShowHelpMoreHintsScreen();
    }

    return true;
}

void
VR_VoiceBoxEngine::ShowHelpMoreHintsScreen()
{
    VR_LOGD_FUNC();

    if (("na_topmenu_idle" == m_strScreenState)
        || ("na_media_idle" == m_strScreenState)
        || ("na_phone_idle" == m_strScreenState)
        || ("na_apps_idle" == m_strScreenState)
        || ("na_climate_idle" == m_strScreenState)) {
        // Prepare the help related hints
        PrepareHintsContent("Help");

        // show more hints screen on top screen
        m_bMoreHint = true;
        m_iIndex = 0;

        DisplayHints("VR-HNT-04");
    }
}

void
VR_VoiceBoxEngine::SetActiveSouceTrans(
    const std::string& strTransId, const std::string& strOp, const std::string& strSouceId)
{
    VR_LOG("strTransId = %s, strOp = %s, strSouceId = %s", strTransId.c_str(), strOp.c_str(), strSouceId.c_str());
    m_mapActiveSouceTrans.insert(std::make_pair(strTransId, std::make_pair(strOp, strSouceId)));
}

void
VR_VoiceBoxEngine::GetOpActiveSouce(const std::string& strTransId, std::string& strOp, std::string& strActiveSource)
{
    strOp.clear();
    strActiveSource.clear();

    VR_LOG("strTransId = %s", strTransId.c_str());

    VoiceMap<std::string, std::pair<std::string, std::string>>::iterator iter = m_mapActiveSouceTrans.find(strTransId);
    if (m_mapActiveSouceTrans.end() != iter) {
        strOp = iter->second.first;
        strActiveSource = iter->second.second;
    }
}

std::string
VR_VoiceBoxEngine::getHints(const std::string& hintsParams)
{
    VR_LOGD_FUNC();

    if (hintsParams.empty() || NULL == m_client.ptr()) {
        return "";
    }

    VR_VoiceBoxXmlParser parser(hintsParams);

    m_strAction = "";
    m_lstTransaction.clear();

    std::string strStateName = parser.getValueByKey("agentName");
    // std::string strPageSize = parser.getValueByKey("pageSize");

    HRESULT retCode = m_client->CreateStringSet(&m_spHints);
    if (S_OK != retCode) {
        return false;
    }
    if (NULL == m_spHints.ptr()) {
        return false;
    }

    m_bMoreHint = false;

    VBT_INT nScreenId = m_mapHintsScreenID["none"];
    std::string strVBTStateName;

    GetVBTInfoByUIStateName(strStateName, nScreenId, strVBTStateName);

    m_strUIStateName = strStateName;

    CVECIPtr<IVECIListItems> optionalContextList;
    m_client->CreateListItems(&optionalContextList);
    if (NULL == optionalContextList.ptr()) {
        return false;
    }

    SetOptionalContext(optionalContextList, strVBTStateName, false);

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    retCode = m_engineCommand->GetRecognitionStateHints("Global", nScreenId, optionalContextList, &m_spHints);
    if (S_OK != retCode) {
        VR_LOG("GetRecognitionStateHints: %x", retCode);
        return false;
    }

    m_iIndex = 0;
    m_iHintSize = 0;

    m_spHints->GetSize(&m_iHintSize);
    std::string strHints = SynchronizeHints();

    return strHints;
}

std::string
VR_VoiceBoxEngine::SynchronizeHints()
{
    VR_LOGD_FUNC();

    if (NULL == m_spHints) {
        return false;
    }

    const int  DEFAULTPAGESIZEUS = 4;
    const int  DEFAULTPAGESIZEOC = 5;
    int iHintPageSize = (VR_REGION_OC == m_country) ? DEFAULTPAGESIZEOC : DEFAULTPAGESIZEUS;

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildDisplayElement("Common", "HintsDisplay");
    xmlBulder.buildGivenElement(node, "agent", m_strUIStateName.c_str(), "", "");
    std::string strContent = (m_bMoreHint) ? m_strMoreHints : m_strContent;
    xmlBulder.buildGivenElement(node, "content", strContent.c_str(), "", ""); // content
    pugi::xml_node nodeHints = xmlBulder.buildGivenElement(node, "hints", "", "", "");
    pugi::xml_node nodeList = xmlBulder.buildGivenElement(nodeHints, "list", "", "", "");
    pugi::xml_node nodeHeader = xmlBulder.buildGivenElement(nodeList, "header", "", "", "");
    xmlBulder.buildGivenElement(nodeHeader, "startIndex", std::to_string(m_iIndex), "", "");
    xmlBulder.buildGivenElement(nodeHeader, "pageSize", std::to_string(iHintPageSize), "", "");
    xmlBulder.buildGivenElement(nodeHeader, "count", std::to_string(m_iHintSize), "", "");
    pugi::xml_node nodeItems = xmlBulder.buildGivenElement(nodeList, "items", "", "", "");

    VoiceVector<std::string>::type vecItem;
    std::string strHintStart;
    std::string strHintEnd;
    if ("fr-CA" == m_strCultureName) {
        strHintStart = "« ";
        strHintEnd = " »";
    }
    else {
        strHintStart = "“";
        strHintEnd = "”";
    }

    // Retrieve Each Hints
    for (VBT_ULONG i = 0; i < iHintPageSize; ++i) {
        CVECIOutStr strHint;
        (void)m_spHints->GetItem(m_iIndex, &strHint);
        if (m_iIndex < m_iHintSize) {
            ++m_iIndex;
        }
        else {
            break;
        }

        VoiceVector<StructNode>::type attributeVector;
        StructNode strNode;

        strNode.Name = "hint";
        if (NULL != strHint.Get()) {
            std::string strHintTmp = strHint.Get();
            strNode.Value = strHintStart;
            strNode.Value.append(strHintTmp);
            strNode.Value.append(strHintEnd);
        }
        attributeVector.push_back(strNode);

        xmlBulder.buildListItemChildElement(nodeItems, "", hint, attributeVector);
    }

    return xmlBulder.getXmlString();
}

void
VR_VoiceBoxEngine::SetNBestScreenFlg(const bool& bNBestFlg)
{
    m_bNBestFlg = bNBestFlg;
}

bool
VR_VoiceBoxEngine::GetNBestScreenFlg()
{
    return m_bNBestFlg;
}

bool
VR_VoiceBoxEngine::ShowHintScreen(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    VR_LOG("get hints");
    // Prepare the hints
    if (!GetHints()) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxEngine::IsAppsInstalled()
{
    return m_bTslAppsAvailable;
}

bool
VR_VoiceBoxEngine::IsAppsEnabled()
{
    return m_bTslNetworkAvailable;
}

void VR_VoiceBoxEngine::NotifyResourceState()
{
    VR_LOGD_FUNC();

    std::string strState = "0";
    if (m_bDoCanceling || !m_bEngineStarting) {
        strState = "1";
    }

    if (strState == m_strCurResourceState) {
        VR_LOG("m_strCurResourceState [%s] is not changed", strState.c_str());
        return;
    }
    else {
        m_strCurResourceState = strState;
        VR_LOG("m_strCurResourceState is changed to [%s]", m_strCurResourceState.c_str());
        SendDEStatus("engine", m_strCurResourceState);
    }
}

void VR_VoiceBoxEngine::SendDEStatus(const std::string& strType, const std::string& strStatus)
{
    std::string strEventResult = "<action agent=\"destatus\" op=\"notifyResourceState\">";
    strEventResult.append("<resourceStateType>");
    strEventResult.append(strType);
    strEventResult.append("</resourceStateType>");
    strEventResult.append("<resourceState>");
    strEventResult.append(strStatus);
    strEventResult.append("</resourceState>");
    strEventResult.append("</action>");

    OnRequestAction(strEventResult);
}

void
VR_VoiceBoxEngine::GrammarDiffDetail(const std::string& strMessage)
{
    if (strMessage.empty()) {
        return;
    }

   std::string strGrammarDiff = strMessage;
    size_t iPos = strGrammarDiff.find("<category name");
    std::string strGrammarDiffFront = strGrammarDiff.substr(0, iPos - 1);
    strGrammarDiffFront.append("</grammar_diff>");
    std::vector<std::string> vecCategory;
    vecCategory.push_back(strGrammarDiffFront);
    size_t iPosStart = strGrammarDiff.find("<category name=\"");
    size_t iPosEnd = strGrammarDiff.find("</category>", iPosStart);
    while (!strGrammarDiff.empty() && std::string::npos != iPosStart && std::string::npos != iPosEnd) {
        std::string strCategory = strGrammarDiff.substr(iPosStart, iPosEnd - iPosStart + 11);
        std::string strFrontCategory;
        std::string strAdd;
        std::string strDelete;
        size_t iDeleteStart = strCategory.find("<delete>");
        size_t iDeleteEnd = strCategory.find("</delete>");
        size_t iAddStart = strCategory.find("<add>");
        size_t iAddEnd = strCategory.find("</add>");
        if (std::string::npos != iAddStart && std::string::npos != iAddEnd && std::string::npos != iDeleteStart && std::string::npos != iDeleteEnd) {
            if (iDeleteStart < iDeleteEnd && iDeleteEnd < iAddStart && iAddStart < iAddEnd) {
                strFrontCategory = strCategory.substr(0, iDeleteStart - 1);
            }
            else if (iAddStart < iAddEnd && iAddEnd < iDeleteStart && iDeleteStart < iDeleteEnd) {
                strFrontCategory = strCategory.substr(0, iAddStart - 1);
            }
            else {

            }

            strAdd = strCategory.substr(iAddStart, iAddEnd - iAddStart + 6);
            strDelete = strCategory.substr(iDeleteStart, iDeleteEnd - iDeleteStart + 9);

            strAdd = strFrontCategory + strAdd + "</category>";
            strDelete = strFrontCategory + strDelete + "</category>";

            vecCategory.push_back(strAdd);
            vecCategory.push_back(strDelete);
        }
        else {
            vecCategory.push_back(strCategory);
        }
        strGrammarDiff = strGrammarDiff.substr(iPosEnd + 11);
        iPosStart = strGrammarDiff.find("<category name=\"");
        iPosEnd = strGrammarDiff.find("</category>", iPosStart);
    }

    for (int index = 0; index <  vecCategory.size(); ++index) {
        std::string strTmp;
        if (index < vecCategory.size() - 1) {
            strTmp = vecCategory[index];
        }
        else {
            size_t iPos = vecCategory[index].find_first_of(">");
            strTmp = vecCategory[index].insert(iPos, " isLast=\"true\"");
        }

        ProcessGrammarMSg(strTmp);
    }

    return;
}

void
VR_VoiceBoxEngine::GrammarNewDetail(const std::string& strMessage)
{
    std::string strSub = strMessage;
    size_t iPosStart = strSub.find("<category name=\"");
    size_t iPosEnd = strSub.find("</category>", iPosStart);
    VoiceVector<std::string>::type vecCategory;
    while (!strSub.empty() && std::string::npos != iPosStart && std::string::npos != iPosEnd) {
        std::string strCategory = strSub.substr(iPosStart, iPosEnd - iPosStart + 11);
        strSub = strSub.substr(iPosEnd + 11);
        iPosStart = strSub.find("<category name=\"");
        iPosEnd = strSub.find("</category>", iPosStart);
        if (std::string::npos != strCategory.find("quickreplymessage")) {
            vecCategory.push_back(strCategory);
        }
        else if (std::string::npos != strCategory.find("phonetype")) {
            vecCategory.push_back(strCategory);
        }
        else if (std::string::npos != strCategory.find("messagetype")) {
            vecCategory.push_back(strCategory);
        }
        else if (std::string::npos != strCategory.find("audiosource")) {
            vecCategory.push_back(strCategory);
        }
        else if (std::string::npos != strCategory.find("fmgenre")) {
            vecCategory.push_back(strCategory);
        }
        else if (std::string::npos != strCategory.find("satchannelname")) {
            vecCategory.push_back(strCategory);
        }
        else if (std::string::npos != strCategory.find("satchannelnumber")) {
            vecCategory.push_back(strCategory);
        }
        else if (std::string::npos != strCategory.find("satgenre")) {
            vecCategory.push_back(strCategory);
        }
        else if (std::string::npos != strCategory.find("hdsubchannel")) {
            vecCategory.push_back(strCategory);
        }
        else {

        }
    }

    for (int index = 0; index <  vecCategory.size(); ++index) {
        std::string strTmp;
        if (index < vecCategory.size() - 1) {
            strTmp = vecCategory[index];
        }
        else {
            size_t iPos = vecCategory[index].find_first_of(">");
            strTmp = vecCategory[index].insert(iPos, " isLast=\"true\"");
        }

        ProcessGrammarMSg(strTmp);
    }
}

void
VR_VoiceBoxEngine::StopTraining()
{
    if (m_bPlayTTS) {
        VR_LOG("playing tts. stop playing");
        m_bPlayTTS = false;
        std::string strStopTTS = "<action agent=\"prompt\" op=\"stopTts\"><reqId>";
        strStopTTS.append(std::to_string(m_iCurTTSReqId));
        strStopTTS.append("</reqId></action>");

        m_stopTraining = OnRequestAction(strStopTTS);
        VR_LOG("m_stopTraining, %d", m_stopTraining);
        m_mapStopTTSTransation.insert(std::make_pair(m_stopTraining, m_iCurTTSReqId));
    }
    else {
        VR_LOG("no tts , handle");
        CancelRecoSession();
        VR_VoiceBoxXmlBuilder xmlBulder;
        pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
        xmlBulder.buildGivenElement(xmlNode, "agent", "help", "", "");
        xmlBulder.buildGivenElement(xmlNode, "content", "na_help_train_voice_recognition", "", "");
        std::string strReply = xmlBulder.getXmlString();
        VR_LOG("SpeakerAdaptionCatch: %s", strReply.c_str());
        OnRecognized(strReply);
    }
}

void
VR_VoiceBoxEngine::ResetTslAgents()
{
    VR_LOGD_FUNC();

    if (VR_REGION_US == m_country) {
        UninstallAllAgentsInSync("es-MX");
        UninstallAllAgentsInSync("fr-CA");
        UninstallAllAgentsInSync("en-US");
    }
    else if (VR_REGION_OC == m_country) {
        UninstallAllAgentsInSync("en-AU");
    }
    else {
        // do nothing
    }
}

std::string
VR_VoiceBoxEngine::GetMDActiveSourceId()
{
    return m_strDMActiveSourceId;
}

void
VR_VoiceBoxEngine::SetGrammarInitSourceId(const std::string& strGrammarInitSourceId)
{
    m_strGrammarInitSourceId = strGrammarInitSourceId;
}

bool
VR_VoiceBoxEngine::ResetGrammarUpdate()
{
    if (NULL == m_pcCatalogController) {
        return false;
    }
    std::string strCurrentCatalogMsg = m_pcCatalogController->GetCurrentMessage();
    if ((std::string::npos != strCurrentCatalogMsg.find("<grammar_init"))
        || (std::string::npos != strCurrentCatalogMsg.find("<grammar_active agent=\"phone\""))
        || (std::string::npos != strCurrentCatalogMsg.find("<grammar_diff"))
        || (std::string::npos != strCurrentCatalogMsg.find("<add>"))
        || (std::string::npos != strCurrentCatalogMsg.find("<delete>"))
        || (std::string::npos != strCurrentCatalogMsg.find("<grammar_disactive agent=\"phone\">"))) {
        return false;
    }
    else {
        m_pcCatalogController->ProcessOpMessage("<op name=\"updateagain\"></op>");
        return true;
    }
}

bool
VR_VoiceBoxEngine::GetEngineStatus()
{
    // During install agents, the VR engine should not be used
    if (m_bInstallingAgent) {
        return false;
    }

    return m_bEngineStarting;
}

void
VR_VoiceBoxEngine::SetWaitResumeGrammar(bool bWaitResumeGrammar)
{
    VR_LOGD_FUNC();
    m_bWaitResumeGrammar = bWaitResumeGrammar;
}

void
VR_VoiceBoxEngine::SetCurrentTransaction(const std::string& strAgent, const std::string& strTransId)
{
    m_pairTransaction = std::make_pair(strAgent, strTransId);
}

std::pair<std::string, std::string>
VR_VoiceBoxEngine::GetCurrentTransaction()
{
    return m_pairTransaction;
}

bool
VR_VoiceBoxEngine::RestartForInstallTslAgent(const std::string &strCultureName)
{
    VR_LOGD_FUNC();

    if (NULL == m_engineCommand.ptr()) {
        return false;
    }

    // Start the dialog engine with the specified culture
    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand->RestartEngine(
                     &transaction,
                     strCultureName.c_str()
                     );
    if (FAILED(result) || (NULL == transaction.ptr())) {
        return false;
    }

    CVECIOutStr strTrans;
    result = transaction->GetTransactionId(&strTrans);
    if (FAILED(result)) {
        return false;
    }

    // Make it a sync operation, wait until the engine is started
    VBT_BOOL bTimeout = VBT_FALSE;
    result = transaction->WaitForCompletion(INFINITE, &bTimeout);
    if (FAILED(result) || bTimeout) {
        return false;
    }

    return true;
}

void
VR_VoiceBoxEngine::NotifyInstallProgress(const std::string& strPath, int index)
{
    const int nProgressFormatLen = 1024;
    std::string strProgressFormat = "<action name=\"notifyInstallProgress\">"
                                      "<TSLINFO>"
                                        "<Action type=\"Install Progress\">"
                                          "<Parameter name=\"Path\" value=\"%s\"/>"
                                          "<Parameter name=\"Index\" value=\"%d\"/>"
                                        "</Action>"
                                      "</TSLINFO>"
                                    "</action>";

    char progress[nProgressFormatLen] = { 0 };
    snprintf(progress, sizeof(progress), strProgressFormat.c_str(), strPath.c_str(), index);
    OnRequestAction(progress);
}

std::string
VR_VoiceBoxEngine::GetCultureName()
{
    return m_strCultureName;
}

void
VR_VoiceBoxEngine::SendChangeLanguageResult(const std::string& strLanguage)
{
    std::string strEventResult = "<event-result name=\"changeLanguage\">";
    strEventResult.append("<language>");
    strEventResult.append(strLanguage);
    strEventResult.append("</language>");
    strEventResult.append("</event-result>");

    OnRecognized(strEventResult);

    return;
}

/* EOF */