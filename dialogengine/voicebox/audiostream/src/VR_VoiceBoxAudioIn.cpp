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

/* Suntec Header */

#ifndef VC_COMMONIF_H
#    include "VC_CommonIF.h"
#endif

#ifndef VR_VOICEBOXAUDIOIN_H
#    include "VR_VoiceBoxAudioIn.h"
#endif

#ifndef VR_VOICEBOXAUDIOINLISTENER_H
#    include "VR_VoiceBoxAudioInListener.h"
#endif

#ifndef VR_LOG_H
#    include "VR_Log.h"
#endif

#include "VR_ConfigureIF.h"

#include "VR_PerformanceLog.h"
// Constructor
VR_VoiceBoxAudioIn::VR_VoiceBoxAudioIn(
    VR_VoiceBoxAudioInListener& listener
)
: m_audioIn(NULL)
, m_listener(listener)
, m_bRecording(false)
, m_bBargeIn(false)
, m_bVoiceTag(false)
, m_bInputWavOption(false)
{
    // m_audioIn = VC_CommonIF::Instance()->CreateAudioIn();
}

// Destructor
VR_VoiceBoxAudioIn::~VR_VoiceBoxAudioIn()
{
    m_audioIn = NULL;
}



bool
VR_VoiceBoxAudioIn::StartAudioIn()
{
    if (m_bRecording) {
        return true;
    }

    if (m_bVoiceTag) {
        m_voiceTagFilePath = VR_ConfigureIF::Instance()->getUsrPath() + "voiceTag.wav";
        m_bVoiceTag = false;
    }
    else {
        m_voiceTagFilePath = "";
    }
    VR_LOG("m_voiceTagFilePath: %s", m_voiceTagFilePath.c_str());
    m_listener.SetVoiceTagPath(m_voiceTagFilePath);

    VR_LOGP("AudioIn open");

    if (!Open()) {
        return false;
    }

    VR_LOGP("AudioIn start");
    if (!Start()) {
        return false;
    }

    m_bRecording = true;
    return true;
}

bool
VR_VoiceBoxAudioIn::StopAudioIn()
{
    if (!m_bRecording) {
        return true;
    }

    m_bRecording = false;

    VR_LOGP("AudioIn stop");
    Stop();
    VR_LOGP("AudioIn close");
    Close();

    return true;
}

bool
VR_VoiceBoxAudioIn::Open()
{
    VR_LOGD_FUNC();

    bool inputWavOption = VR_ConfigureIF::Instance()->getInputWavOption();
    if (NULL != m_audioIn && m_bInputWavOption != inputWavOption) {
        delete m_audioIn;
        m_audioIn = NULL;
    }

    if (NULL == m_audioIn) {
        m_audioIn = VC_CommonIF::Instance()->CreateAudioIn(inputWavOption);
        m_bInputWavOption = inputWavOption;
    }

    if (NULL == m_audioIn) {
        return false;
    }

    INT sampleRate = SAMPLE_RATE;
    INT fragSize   = BUFFER_SIZE;
    INT fragCount  = BUFFER_COUNT;
    if (!m_audioIn->Open(&sampleRate, &fragSize, &fragCount, m_bBargeIn ? VC_AUDIO_MODE_BARGEIN : VC_AUDIO_MODE_NORMAL)) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAudioIn::Start()
{
    VR_LOGD_FUNC();

    if (NULL == m_audioIn) {
        return false;
    }

    if (!m_audioIn->Start(&m_listener)) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAudioIn::Stop()
{
    VR_LOGD_FUNC();

    if (NULL == m_audioIn) {
        return false;
    }

    if (!m_audioIn->Stop(true)) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAudioIn::Close()
{
    VR_LOGD_FUNC();

    if (NULL == m_audioIn) {
        return false;
    }

    if (!m_audioIn->Close()) {
        return false;
    }

    return true;
}

void
VR_VoiceBoxAudioIn::SetBargeIn(bool bOn)
{
    VR_LOGD_FUNC();

    m_bBargeIn = bOn;
}

std::string
VR_VoiceBoxAudioIn::GetTempPcmFile()
{
    VR_LOG("VR_VoiceBoxAudioIn-GetTempPcmFile-m_voiceTagFilePath: %s", m_voiceTagFilePath.c_str());
    return m_voiceTagFilePath;
}

void
VR_VoiceBoxAudioIn::SetVoiceTag(bool bOn)
{
    VR_LOGD_FUNC();
    VR_LOG("VR_VoiceBoxAudioIn-SetVoiceTag");
    m_bVoiceTag = bOn;
    VR_LOG("VR_VoiceBoxAudioIn-m_bvoicetag: %d", bOn);
}

unsigned int
VR_VoiceBoxAudioIn::GetAecAudioTypeDataSize()
{
    return m_listener.GetAecAudioTypeDataSize();
}

bool
VR_VoiceBoxAudioIn::GetAecAudioTypeData(unsigned int& size, void* data)
{
    return m_listener.GetAecAudioTypeData(size, data);
}

/* EOF */
