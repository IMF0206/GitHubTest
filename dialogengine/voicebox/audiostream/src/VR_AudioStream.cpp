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

#include <boost/bind.hpp>

#ifndef VR_AUDIOSTREAM_H
#    include "VR_AudioStream.h"
#endif

#ifndef VR_VOCONENGINELISTENER_H
#    include "VR_VoconEngineListener.h"
#endif

#ifndef VR_AUDIOBUFFER_H
#    include "VR_AudioBuffer.h"
#endif

#ifndef VR_VOICEBOXAUDIOINLISTENER_H
#    include "VR_VoiceBoxAudioInListener.h"
#endif

#ifndef VR_AUDIOSTREAMLISTENER_H
#    include "VR_AudioStreamListener.h"
#endif

#ifndef VR_VOICEBOXAUDIOIN_H
#    include "VR_VoiceBoxAudioIn.h"
#endif

#ifndef VR_VAALSTREAMWRAP_H
#    include "VR_VaalStreamWrap.h"
#endif

#ifndef VR_CONFIGUREIF_H
#    include "VR_ConfigureIF.h"
#endif

#ifndef VR_LOG_H
#    include "VR_Log.h"
#endif

#ifndef VR_DEF_H
#    include "VR_Def.h"
#endif

VR_AudioStream g_audioStream;

VR_AudioStreamIF* VR_AudioStreamIF::Instance()
{
    return &g_audioStream;
}

VR_AudioStream::VR_AudioStream()
: m_voiceBoxAudioIn(NULL)
, m_audioInListener(NULL)
, m_vaalStream(NULL)
, m_audioStreamListener(NULL)
, m_voconEngine(NULL)
, m_bBargeInDetecting(false)
, m_bFirstAsrFactorySucceed(false)
{
}

VR_AudioStream::~VR_AudioStream()
{
}

bool
VR_AudioStream::Initialize(VR_AudioStreamListener* listener)
{
    m_audioStreamListener = listener;

    m_audioInListener = VR_new VR_VoiceBoxAudioInListener(m_audioBufferManager);
    if (NULL == m_audioInListener) {
        return false;
    }

    m_voiceBoxAudioIn = VR_new VR_VoiceBoxAudioIn(*m_audioInListener);
    if (NULL == m_voiceBoxAudioIn) {
        return false;
    }

    m_vaalStream = VR_new VR_VaalStreamWrap(*m_voiceBoxAudioIn, m_audioBufferManager);
    if (NULL == m_vaalStream) {
        return false;
    }

    m_voconEngineListener = boost::shared_ptr<VR_VoconEngineListener>(
        VR_new VR_VoconEngineListener(*m_voiceBoxAudioIn, m_audioBufferManager)
    );

    return true;
}

void
VR_AudioStream::UnInitialize()
{
    m_audioStreamListener = NULL;

    delete m_voiceBoxAudioIn;
    m_voiceBoxAudioIn = NULL;

    delete m_audioInListener;
    m_audioInListener = NULL;

    delete m_vaalStream;
    m_vaalStream = NULL;

    delete m_voconEngine;
    m_voconEngine = NULL;
}

bool
VR_AudioStream::ShareFrontEnd(void *handleAsr, void *handleAcMod)
{
    VR_LOGD_FUNC();

    // Null Checking
    if ((NULL == handleAsr) || (NULL == handleAcMod)) {
        VR_ERROR("ASR handler is NULL");
        return false;
    }

    // If ASR handler changed, we need to delete the previous engine.
    if (NULL != m_voconEngine) {
        delete m_voconEngine;
        m_voconEngine = NULL;
    }

    // Create BOS detecting engine
    N_Vr::N_Asr::C_Request_Factory requestFactory;
    requestFactory.m_e_Type_Engine = N_Vr::N_Asr::E_Type_Engine_Vocon_Via_Shared;
    requestFactory.m_string_Id_Language = "bos";
    requestFactory.m_po_Vocon_LH_COMPONENT_Asr = handleAsr;
    requestFactory.m_po_Vocon_LH_OBJECT_Mod_Ac = handleAcMod;
    m_voconEngine = N_Vr::N_Asr::C_Engine::Factory(requestFactory);
    // if (NULL == m_voconEngine) {
    //     VR_ERROR("Create BOS detecting engine failed");
    //     return false;
    // }

   if (NULL == m_voconEngine) {
        VR_LOG("Create BOS failed with \"bos\" language id, try to factory with \"ena\"");
        m_bFirstAsrFactorySucceed = false;

        // Fixme: factory again with "ena" language
        requestFactory.m_string_Id_Language = "ena";
        m_voconEngine = N_Vr::N_Asr::C_Engine::Factory(requestFactory);
        if (NULL == m_voconEngine) {
            VR_ERROR("Create BOS detecting engine failed");
            return false;
        }
    }
    else {
        VR_LOG("Create BOS succeed with \"bos\" language id");
        m_bFirstAsrFactorySucceed = true;
    }

    return true;
}

bool
VR_AudioStream::ReleaseFrontEnd()
{
    VR_LOGD_FUNC();

    if (NULL != m_voconEngine) {
        delete m_voconEngine;
        m_voconEngine = NULL;
    }

    return true;
}

bool
VR_AudioStream::StartAudioInWithBargeIn()
{
    VR_LOGD_FUNC();

    if (NULL == m_voconEngine) {
        VR_ERROR("The BOS detecting engine is NULL");
        return false;
    }

    if (NULL == m_voiceBoxAudioIn) {
        VR_ERROR("The audio in instance is NULL");
        return false;
    }

    m_voiceBoxAudioIn->SetBargeIn(true);

    m_audioBufferManager.SetBosDetecting(true);
    m_audioBufferManager.SetBosDetected(false);
    m_audioBufferManager.SetAudioCachingFlag(false);
    m_audioBufferManager.Clear();

    if (!m_voiceBoxAudioIn->StartAudioIn()) {
        VR_LOG("VR_AudioStream: Start audio in failed");
        return false;
    }

    VR_ConfigureIF * pcConfig = VR_ConfigureIF::Instance();
    VR_PRODUCT_TYPE product = VR_PRODUCT_TYPE_UNKNOWN;
    if (NULL != pcConfig) {
        product = pcConfig->getVRProduct();
    }
    VR_LOG("The current product type is [%d]", product);

    // Interupt level
    N_Vr::N_Asr::C_Request_Param_Set param;
    switch (product) {
    case VR_PRODUCT_TYPE_T0:
    case VR_PRODUCT_TYPE_T1:
        {
            param.m_e_Param_Type = N_Vr::N_Asr::E_Param_Type_Fx_Sensitivity;
            param.m_i_Value = 25;
            m_voconEngine->Param_Set(param);

            param.m_e_Param_Type = N_Vr::N_Asr::E_Param_Type_Fx_Absolute_Threshold;
            param.m_i_Value = -3800;
            m_voconEngine->Param_Set(param);
            break;
        }
    default :
        {
            VR_LOG("will set default value for ASR !!!");
            param.m_e_Param_Type = N_Vr::N_Asr::E_Param_Type_Fx_Sensitivity;
            param.m_i_Value = 10;
            m_voconEngine->Param_Set(param);

            param.m_e_Param_Type = N_Vr::N_Asr::E_Param_Type_Fx_Absolute_Threshold;
            param.m_i_Value = -3000;
            m_voconEngine->Param_Set(param);
            break;
        }
    }

    param.m_e_Param_Type = N_Vr::N_Asr::E_Param_Type_Fx_Minspeech;
    param.m_i_Value = 144;
    m_voconEngine->Param_Set(param);

    N_Vr::N_Asr::C_Request_Recognize request;
    request.m_b_Barge_In = 1;
    request.m_string_Id_Party = "Origin";
    if (m_bFirstAsrFactorySucceed) {
        request.m_string_Id_Dialog = "bos_detection";
    }
    else {
        request.m_string_Id_Dialog = "test";
    }
    VR_LOG("Request BOS with dialog id \"%s\"", request.m_string_Id_Dialog.c_str());
    // request.m_string_Id_Dialog = "test";
    request.m_spo_Audio_In = m_voconEngineListener;
    request.m_function_On_Event_Phase = boost::bind(&VR_AudioStream::onAsrPhase, this, _1);
    request.m_function_On_Event_Notify = boost::bind(&VR_AudioStream::onAsrNotify, this, _1);
    request.m_function_On_Event_Result = boost::bind(&VR_AudioStream::onAsrResult, this, _1);
    request.m_b_Measure_Time = false;
    if (N_Vr::N_Asr::E_Result_Succeeded != m_voconEngine->Recognize_Start(request)) {
        VR_ERROR("Start BOS detecting failed");
        return false;
    }

    m_bBargeInDetecting = true;
    return true;
}

bool
VR_AudioStream::StartAudioInWithCache()
{
    VR_LOGD_FUNC();

    if (NULL == m_voiceBoxAudioIn) {
        return false;
    }
    m_voiceBoxAudioIn->SetBargeIn(true);
    m_audioBufferManager.SetAudioCachingFlag(true);
    m_audioBufferManager.Clear();

    return m_voiceBoxAudioIn->StartAudioIn();
}

bool
VR_AudioStream::StartAudioIn()
{
    VR_LOGD_FUNC();

    if (NULL == m_voiceBoxAudioIn) {
        return false;
    }

    if (m_bBargeInDetecting && (NULL != m_voconEngine)) {
        m_voconEngine->Recognize_Cancel();
        m_bBargeInDetecting = false;
        m_audioBufferManager.SetBosDetected(false);
    }

    m_audioBufferManager.SetBosDetecting(false);
    m_audioBufferManager.SetAudioCachingFlag(false);

    return m_voiceBoxAudioIn->StartAudioIn();
}

bool
VR_AudioStream::StopAudioIn()
{
    VR_LOGD_FUNC();
    if (NULL == m_voiceBoxAudioIn) {
        return false;
    }

    if (m_bBargeInDetecting && (NULL != m_voconEngine)) {
        m_voconEngine->Recognize_Cancel();
        m_bBargeInDetecting = false;
    }

    m_voiceBoxAudioIn->StopAudioIn();
    m_voiceBoxAudioIn->SetBargeIn(false);

    m_audioBufferManager.SetBosDetected(false);
    m_audioBufferManager.SetBosDetecting(false);

    return true;
}

unsigned int
VR_AudioStream::GetAecAudioTypeDataSize()
{
    VR_LOGD_FUNC();
    if (NULL == m_audioInListener) {
        VR_ERROR("BOS detecting listener is NULL");
        return 0;
    }

    return m_audioInListener->GetAecAudioTypeDataSize();
}

bool
VR_AudioStream::GetAecAudioTypeData(unsigned int& size, void* data)
{
    VR_LOGD_FUNC();

    if (NULL == m_audioInListener) {
        VR_ERROR("BOS detecting listener is NULL");
        return false;
    }

    return m_audioInListener->GetAecAudioTypeData(size, data);
}


VR_VaalStream*
VR_AudioStream::GetVaalStream()
{
    if (NULL == m_vaalStream) {
        VR_ERROR("VAAL stream instance is NULL");
        return NULL;
    }

    return m_vaalStream->GetVaalStream();
}

void
VR_AudioStream::onAsrPhase(N_Vr::N_Asr::C_Event_Phase const& phase)
{
    VR_LOGD_FUNC();

    VR_LOG("On ASR Phase: %d", phase.Get_Event_Phase_Type());
    switch (phase.Get_Event_Phase_Type()) {
    case N_Vr::N_Asr::C_Event_Phase::E_Event_Phase_Type_Speech_Begin_Fx:
        {
            m_audioBufferManager.SetBosDetected(true);

            if (NULL != m_audioStreamListener) {
                m_audioStreamListener->OnBosDetected();
            }

            if (NULL != m_voconEngine) {
                m_voconEngine->Recognize_Cancel();
            }
            m_bBargeInDetecting = false;
        }
        break;
    default:
        break;
    }
}

void
VR_AudioStream::onAsrNotify(N_Vr::N_Asr::C_Event_Notify const& notify)
{
    // do nothing
}

void
VR_AudioStream::onAsrResult(N_Vr::N_Asr::C_Event_Result const& result)
{
    // do nothing
}

std::string
VR_AudioStream::GetTempPcmFile()
{
    VR_LOG("VR_AudioStream-GetTempPcmFile");
    // m_voiceBoxAudioIn->SetVoiceTag(true);
    std::string voiceTagPath = m_voiceBoxAudioIn->GetTempPcmFile();

    return voiceTagPath;
}

void
VR_AudioStream::SetVoiceTag(bool bOn)
{
    if (NULL == m_voiceBoxAudioIn) {
        VR_LOG("VR_AudioStream-SetVoiceTag null");
        return;
    }
    VR_LOG("VR_AudioStream-SetVoiceTag");
    m_voiceBoxAudioIn->SetVoiceTag(bOn);
}
/* EOF */
