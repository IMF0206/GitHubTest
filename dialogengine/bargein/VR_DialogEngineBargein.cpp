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
#include "VR_DialogEngineBargein.h"
#include <sstream>
#include <fstream>
#include <string>
#include <functional>
#include <iostream>
#include <algorithm>
#include <dirent.h>
#include <stdio.h>

#include "VR_Log.h"
#include "pugixml.hpp"
#include "VR_Def.h"
#include "config.h"
#include "VC_CommDef.h"
#include "VR_ParamConfig.h"

using namespace N_Vr;
using namespace N_Asr;

static const char* const kParamSettingFile = "/data/vr_bargein_param_setting.conf";

#define VR_BARGEIN_PROMPT_PATH      "/data/vr_bargein_prompts/"
#define VR_BARGEIN_DEBUG_TTS_FILE   "/data/vr_bargein_debug.txt"
#define VR_BARGEIN_DEBUG_WAV_FILE   "/data/vr_bargein_debug.wav"
#define VR_BARGEIN_PRESSURE_TEST_FILE   "/data/vr_bargein_pressure_test"
#define VR_IO_WAV_FILE   "/data/vr_data/sequences.txt"

#define TTS_DELAY_INTERVAL (400*1000)       //  400ms
#define ASR_DELAY_INTERVAL (100*1000)       //  100ms

extern "C" VR_API VR_DialogEngineIF* VR_CreateDialogEngine()
{
    VR_LOGD_FUNC();

    VR_DialogEngineIF* engine = NULL;

    engine = new(MEM_Voice) VR_DialogEngineBargein();
    RETURN(engine);
}

VR_DialogEngineBargein::VR_DialogEngineBargein()
    : m_pEngineListener(nullptr)
    , m_AsrIsRunning(false)
    , m_debugMode(BARGEIN_DEBUG_NONE)
    , m_ttsPlayType(PLAY_TTS_TYPE)
    , m_pWriter(VR_new VC_WavFileWriter())
{
}

VR_DialogEngineBargein::~VR_DialogEngineBargein()
{
    delete m_pWriter;
}

bool VR_DialogEngineBargein::Initialize(VR_DialogEngineListener *listener, const VR_Settings &settings)
{
    VR_LOGD_FUNC();
    m_pEngineListener = listener;

    m_debugMode = BARGEIN_DEBUG_NONE;
    std::ifstream debugBargeinWav(VR_BARGEIN_DEBUG_WAV_FILE);
    std::ifstream debugBargeinTTS(VR_BARGEIN_DEBUG_TTS_FILE);
    if (debugBargeinWav) {  // Wav file has more priority than TTS file
        m_debugMode = BARGEIN_DEBUG_WAV;
    }
    else if (debugBargeinTTS) {
        m_debugMode = BARGEIN_DEBUG_TTS;
        pushTxt2List(VR_BARGEIN_DEBUG_TTS_FILE);
    }

    VR_LOG("init debug mode:%d", m_debugMode);
    if (BARGEIN_DEBUG_NONE == m_debugMode) {
        initTTSText();
    }
    m_ttsTextsIt = m_ttsTexts.begin();

    m_spAsrEngine.reset(C_Engine::Factory("eng"));
    if (!m_spAsrEngine) {
        VR_ERROR("init asr failed");
        // return false;
    }

    m_spAudioInSource.reset(VR_new VR_AudioInSource());
    if (!m_spAudioInSource) {
        VR_ERROR("init audioInSource failed");
        return false;
    }
    return true;
}

// async
bool VR_DialogEngineBargein::Start()
{
    return true;
}

// async
void VR_DialogEngineBargein::Stop()
{
    return;
}

// async
bool VR_DialogEngineBargein::SendMessage(const std::string& message, int actionSeqId)
{
    VR_LOG("receive msg=[%s]", message.c_str());
    pugi::xml_document doc;
    doc.load_string(message.c_str());
    pugi::xml_node eventNode = doc.select_node("//event").node();
    std::string eventName = eventNode.attribute("name").as_string();
    if ("startAgent" == eventName) {
        handlePtt();
        requestAction("<event-result name=\"startAgent\" errcode=\"\">"
                      "<agent>topmenu</agent>"
                      "</event-result>");
        return true;
    }
    else if ("buttonPressed" == eventName) {
        // <keycode value="pttShort"/>
        pugi::xml_node keyNode = doc.select_node("//keycode").node();
        std::string valueName = keyNode.attribute("value").as_string();
        if (valueName == "pttShort") {
            handlePtt();
        }
        return true;
    }

    else if ("cancel" == eventName) {
        closeSession();
        return true;
    }
    else if ("getResourceState" == eventName) {
        std::string msg = "<event-result name=\"getResourceState\">"
                          "<state>0</state>"
                          "</event-result>";
        requestAction(msg);
    }


    pugi::xml_node actionResultNode = doc.select_node("//action-result").node();
    std::string op = actionResultNode.attribute("op").as_string();

    if (op == "playTts" || op == "stopTts" || op == "playBeep" || op == "stopBeep") {
        std::ifstream debugBargeinPresTest(VR_BARGEIN_PRESSURE_TEST_FILE);
        if (debugBargeinPresTest) {
            VR_LOGD("Bargein Pressure test, handlePtt again, sleep 2s.");
            usleep(2000 * 1000);
            handlePtt();
        }
        else {
            // this->cancelAsr();
            // requestAction("<display agent='Common' content='VRState'><state>quit</state></display>");
            requestAction("<display xmlns=\"\" agent=\"Common\" content=\"QuitVRApp\"></display>");
        }
    }
    return true;
}


void VR_DialogEngineBargein::handlePtt()
{
    VR_LOGD_FUNC();

    VR_LOGD("receive a PTT event");
    requestAsr();
    // usleep(TTS_DELAY_INTERVAL);
}

void VR_DialogEngineBargein::closeSession()
{
    VR_LOGD_FUNC();

    if (BARGEIN_DEBUG_WAV == m_debugMode) {
        stopWav();
    }
    else if (BARGEIN_DEBUG_TTS == m_debugMode) {
        stopTTs();
    }
    else {
        if (PLAY_WAV_TYPE == m_ttsPlayType) {
            stopWav();
        }
        else {
            stopTTs();
        }
    }
    usleep(ASR_DELAY_INTERVAL);
    cancelAsr();
}

void VR_DialogEngineBargein::createDocList(VoiceVector<std::string>::type &doc_list, std::string dicPath)
{
    struct dirent entry;
    struct dirent *res = NULL;
    std::string real_dir = dicPath;
    DIR *dir = opendir(real_dir.c_str());
    if (dir != NULL) {
        for (int return_code = readdir_r(dir, &entry, &res); res != NULL && return_code == 0; return_code = readdir_r(dir, &entry, &res)) {
            if (entry.d_type != DT_DIR) {
                VR_LOG("%s", entry.d_name);
                std::string fileName(entry.d_name);
                if ((std::string::npos != fileName.find(".txt"))
                    || (std::string::npos != fileName.find(".wav"))) {
                    doc_list.push_back(std::string(VR_BARGEIN_PROMPT_PATH) + fileName);
                    VR_LOG("%s", entry.d_name);
                }
            }
        }
        std::sort(doc_list.begin(), doc_list.end());
        closedir(dir); // close dir
    }
    else {
        VR_ERROR("open dic [%s] failed", dicPath.c_str());
    }
}

void VR_DialogEngineBargein::pushTxt2List(const std::string& fileName)
{
    VR_LOGD_FUNC();
    std::ifstream bargeinFile;
    bargeinFile.open(fileName.c_str());
    if (!bargeinFile.is_open()) {
        VR_ERROR("Failed to open tts text file: %s\n", fileName.c_str());
        return;
    }

    std::string res;

    // check if is wav
    std::size_t pos = fileName.find(".wav");
    if (pos == std::string::npos) {
        // txt
        std::string textLine;
        while (true) {
            std::getline(bargeinFile, textLine);
            if (!bargeinFile) {
                break;
            }
            res.append(textLine);
        }
    }
    else {
        // wav
        res = fileName;
    }

    VR_LOG("push sentence into list:[%s]", res.c_str());
    m_ttsTexts.push_back(res);
}

void VR_DialogEngineBargein::initTTSText()
{
    VR_LOGD_FUNC();
    std::string string_Path_File_Info = VR_BARGEIN_PROMPT_PATH;
    createDocList(m_vectorFiles, string_Path_File_Info);

    std::for_each(m_vectorFiles.begin(), m_vectorFiles.end(), std::bind(&VR_DialogEngineBargein::pushTxt2List, this, std::placeholders::_1));

    if (m_ttsTexts.empty()) {
        m_ttsTexts.push_back("There is no custom TTS sentences.");
        VR_ERROR("no bargein TTS or wave files");
    }
}

// sync
void VR_DialogEngineBargein::UnInitialize()
{
    m_ttsTexts.clear();
}


void VR_DialogEngineBargein::stopTTs()
{
    VR_LOGD_FUNC();
    std::string msg("<action agent=\"prompt\" op=\"stopTts\"><reqId>123</reqId></action>");
    requestAction(msg);
}

void VR_DialogEngineBargein::playTTs()
{
    VR_LOGD_FUNC();
    std::string txt;
    if (m_ttsTextsIt == m_ttsTexts.end()) {
        m_ttsTextsIt = m_ttsTexts.begin();
    }
    txt = *m_ttsTextsIt;


    std::size_t pos = txt.find(".wav");
    if (pos == std::string::npos) {
        // txt
        m_ttsPlayType = PLAY_TTS_TYPE;
        requestAction(VR_PLAYTTS_BAEGEIN(txt));
    }
    else {
        // wav
        m_ttsPlayType = PLAY_WAV_TYPE;
        requestAction(VR_PLAY_BEEP_BARGEIN(txt));
    }

    ++m_ttsTextsIt;
    VR_LOG("bargein playTTs=[%s]", txt.c_str());
}

void VR_DialogEngineBargein::playWav()
{
    VR_LOGD_FUNC();
    std::string wav(VR_BARGEIN_DEBUG_WAV_FILE);
    requestAction(VR_PLAY_BEEP_BARGEIN(wav));
}

void VR_DialogEngineBargein::stopWav()
{
    VR_LOGD_FUNC();
    std::string msg("<action agent=\"prompt\" op=\"stopBeep\"><reqId>123</reqId></action>");
    requestAction(msg);
}

void VR_DialogEngineBargein::playBeep(BeepType type)
{
    VR_LOGD_FUNC();
    std::string beepStr;
    switch (type) {
    case BeepType::start:
    {
        beepStr = START_BEEPPATH;
        break;
    }
    case BeepType::end:
    {
        beepStr = END_BEEPPATH;
        break;
    }
    case BeepType::processed:
    {
        beepStr = RETURN_BEEPPATH;
        break;
    }
    default:
        break;
    }
    requestAction(VR_PLAY_BEEP(beepStr));
}

bool VR_DialogEngineBargein::isAsrNotRunning()
{
    return !m_AsrIsRunning;
}

void VR_DialogEngineBargein::requestAsr()
{
    VR_LOGD_FUNC();
    if (!m_spAsrEngine) {
        VR_ERROR("Asr engine is null");
        return;
    }
    std::unique_lock<std::mutex> lk(m_AsrMutex);
    m_AsrCoVar.wait(lk, std::bind(&VR_DialogEngineBargein::isAsrNotRunning, this));

    m_AsrIsRunning = true;

    // Set barge-in or debug mode
    m_spAudioInSource->setAudioMode(m_debugMode ? VC_AUDIO_MODE_BARGEIN_WITH_DEBUG : VC_AUDIO_MODE_BARGEIN, this);
    VR_LOG("requestVR m_debugMode = %d", m_debugMode);

    // Audioin prepare
    m_spAudioInSource->Prepare();
    VR_LOG("requestVR prepare");

    // Set asr paramter
    paramSetting();
    VR_LOG("requestVR paramSetting");

    C_Request_Recognize req;
    req.m_b_Barge_In = 1;
    req.m_string_Id_Dialog = "topmenu_idle";
    req.m_string_Id_Party = "Origin";
    req.m_spo_Audio_In = boost::shared_ptr<VR_AudioInSource>(m_spAudioInSource.get(), ([](...)mutable{}));
    req.m_function_On_Event_Phase = boost::bind(&VR_DialogEngineBargein::onAsrPhase, this, _1);
    req.m_function_On_Event_Notify = boost::bind(&VR_DialogEngineBargein::onAsrNotify, this, _1);
    req.m_function_On_Event_Result = boost::bind(&VR_DialogEngineBargein::onAsrResult, this, _1);
    req.m_b_Measure_Time = false;

    VR_LOG("ASR recognize start");
    E_Result ret = m_spAsrEngine->Recognize_Start(req);
    if (ret == E_Result::E_Result_Succeeded) {
        m_AsrIsRunning = true;
        VR_LOG("Recognize_Start success");
    }
    else {
        m_spAudioInSource->UnPrepare();
        VR_ERROR("Recognize_Start failed");
    }
}

void VR_DialogEngineBargein::paramSetting()
{
    VR_LOGD_FUNC();

    C_Request_Param_Set param;

    // Asr paramter for VAD
    int paramSensitivity = 10;
    int paramAbsoluteThreshold = -3000;
    int paramMinspeech = 144; // ms

    // Asr paramter for time out
    int paramTimeoutAudio = 0;  // ms
    int paramTimeoutSilenceLeading = 0;  // ms
    int paramTimeoutSpeech = 0;  // ms
    int paramTimeoutSilenceTrailing = 0;  // ms

    // read paramter from file "/data/vr_bargein_param_setting.conf"
    ParamConfig config;
    if (config.Load(kParamSettingFile)) {
        config.Get("paramSensitivity", paramSensitivity);
        config.Get("paramAbsoluteThreshold", paramAbsoluteThreshold);
        config.Get("paramMinspeech", paramMinspeech);

        // Asr paramter for time out
        config.Get("paramTimeoutAudio", paramTimeoutAudio);
        config.Get("paramTimeoutSilenceLeading", paramTimeoutSilenceLeading);
        config.Get("paramTimeoutSpeech", paramTimeoutSpeech);
        config.Get("paramTimeoutSilenceTrailing", paramTimeoutSilenceTrailing);
    }

    // Asr paramter for VAD
    param.m_e_Param_Type = E_Param_Type_Fx_Sensitivity;
    param.m_i_Value = paramSensitivity;
    m_spAsrEngine->Param_Set(param);

    param.m_e_Param_Type = E_Param_Type_Fx_Absolute_Threshold;
    param.m_i_Value = paramAbsoluteThreshold;
    m_spAsrEngine->Param_Set(param);

    param.m_e_Param_Type = E_Param_Type_Fx_Minspeech;
    param.m_i_Value = paramMinspeech;
    m_spAsrEngine->Param_Set(param);

    // Asr paramter for time out
    param.m_e_Param_Type = E_Param_Type_Audio_Timeout;
    param.m_i_Value = paramTimeoutAudio;
    m_spAsrEngine->Param_Set(param);

    param.m_e_Param_Type = E_Param_Type_Fx_Timeout_Silence_Leading;
    param.m_i_Value = paramTimeoutSilenceLeading;
    m_spAsrEngine->Param_Set(param);

    param.m_e_Param_Type = E_Param_Type_Fx_Timeout_Speech;
    param.m_i_Value = paramTimeoutSpeech;
    m_spAsrEngine->Param_Set(param);

    param.m_e_Param_Type = E_Param_Type_Fx_Timeout_Silence_Trailing;
    param.m_i_Value = paramTimeoutSilenceTrailing;
    m_spAsrEngine->Param_Set(param);
}

void VR_DialogEngineBargein::cancelAsr()
{
    VR_LOGD_FUNC();
    std::unique_lock<std::mutex> lk(m_AsrMutex);
    if (m_AsrIsRunning) {
        m_spAsrEngine->Recognize_Cancel();
    }
}

void VR_DialogEngineBargein::onAsrPhase(C_Event_Phase const& phase)
{
    C_Event_Phase::E_Event_Phase_Type msgType = phase.Get_Event_Phase_Type();
    switch (msgType) {
    case C_Event_Phase::E_Event_Phase_Type::E_Event_Phase_Type_Proc_End:
    {
        //    playBeep(BeepType::processed);
        std::unique_lock<std::mutex> lk(m_AsrMutex);
        m_AsrIsRunning = false;
        m_AsrMutex.unlock();
        VR_LOG("asr processed over");
        m_AsrCoVar.notify_one();
        break;
    }
    case C_Event_Phase::E_Event_Phase_Type::E_Event_Phase_Type_Speech_Begin_Fx:
    {
        VR_LOG("E_Event_Phase_Type_Speech_Begin_Fx, bargein debug mode=%d", m_debugMode);
        if (BARGEIN_DEBUG_NONE == m_debugMode) {
            closeSession();
        }
        break;
    }
    case C_Event_Phase::E_Event_Phase_Type::E_Event_Phase_Type_Speech_End_Fx:
    {
        break;
    }
    case C_Event_Phase::E_Event_Phase_Type::E_Event_Phase_Type_Speech_End_Rec:
    {
        break;
    }

    case C_Event_Phase::E_Event_Phase_Type::E_Event_Phase_Type_Listen_Begin:
    {
        break;
    }

    case C_Event_Phase::E_Event_Phase_Type::E_Event_Phase_Type_Listen_End:
    {
        // playBeep(BeepType::end);
        break;
    }
    default:
        break;
    }
}

void VR_DialogEngineBargein::onAsrNotify(C_Event_Notify const& notify)
{
}

void VR_DialogEngineBargein::onAsrResult(C_Event_Result const& result)
{
    VR_LOGD_FUNC();
}

void VR_DialogEngineBargein::OnAudioInStartedAction()
{
    VR_LOGD_FUNC();

    if (BARGEIN_DEBUG_WAV == m_debugMode) {
        playWav();
    }
    else if (BARGEIN_DEBUG_TTS == m_debugMode) {
        playTTs();
    }
    else {
        playTTs();
    }
}

void VR_DialogEngineBargein::requestAction(const std::string& msg)
{
    VR_LOGD_FUNC();
    assert(!m_pEngineListener);
    m_pEngineListener->OnRequestAction(msg, 0);
}

/* EOF */
