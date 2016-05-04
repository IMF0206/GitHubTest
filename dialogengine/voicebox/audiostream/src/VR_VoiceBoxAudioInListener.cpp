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
#ifndef VR_VOICEBOXAUDIOINLISTENER_H
#    include "VR_VoiceBoxAudioInListener.h"
#endif

#ifndef VR_AUDIOBUFFERMANAGER_H
#    include "VR_AudioBufferManager.h"
#endif

#ifndef VR_LOG_H
#    include "VR_Log.h"
#endif

#ifndef VR_DEF_H
#    include "VR_Def.h"
#endif

#include <sstream>
#include "VR_ConfigureIF.h"
#include "BL_Dir.h"
#include "VC_WavFileWriter.h"
#include "BL_SystemTime.h"

// Constructor
VR_VoiceBoxAudioInListener::VR_VoiceBoxAudioInListener(
    VR_AudioBufferManager& bufferManager
)
: m_bufferManager(bufferManager)
, m_pAecData(NULL)
, m_nAecDataSize(0)
, m_nFileNum(0)
, m_bOutFolderCreated(false)
{
}

// Destructor
VR_VoiceBoxAudioInListener::~VR_VoiceBoxAudioInListener()
{
    delete m_pAecData;
    m_pAecData = NULL;
}

int VR_VoiceBoxAudioInListener::CreateOutFolder()
{
    char buffer[100] = { 0 };

    BL_SystemTime cTime;
    BL_TimeSystem currentTime = { 0 };
    if (BL_NOERROR == cTime.GetSystemTime(&currentTime)) {
        snprintf(buffer, sizeof(buffer), "%02d%02d%02d%02d%02d",
            currentTime.Time.wMonth, currentTime.Time.wDay,
            currentTime.Time.wHour, currentTime.Time.wMinute,
            currentTime.Time.wSecond);
    }
    else {
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        snprintf(buffer, sizeof(buffer), "%02d%02d%09d",
            (int)(ts.tv_sec/60), (int)(ts.tv_sec%60), (int)(ts.tv_nsec));
    }

    std::string s_time(buffer);
    VR_LOG("System time is : %s", s_time.c_str());

    BL_Dir recordDir(BL_FILE_PREFIX_RW);
    recordDir.MakeDir((std::string("VR/VBT/") + std::string("Records/")).c_str());
    recordDir.MakeDir((std::string("VR/VBT/Records/") + s_time).c_str());
    m_recordFilePath = VR_ConfigureIF::Instance()->getUsrPath() + "Records/" + s_time + "/";
    return 0;
}

VOID VR_VoiceBoxAudioInListener::OnAudioInData(VOID* data, INT len)
{
    VR_LOGD_FUNC();
    if (m_spWavWriter) {
        m_spWavWriter->Write(data, len);
    }
    m_bufferManager.OnAudioInData(data, len);
}

VOID VR_VoiceBoxAudioInListener::OnAudioInCustom(int type, VOID* data, INT len)
{
    VR_LOGD_FUNC();

    if ((len <= 0) || (NULL == data)) {
        m_nAecDataSize = 0;
        return;
    }

    char* pData = VR_new char[len];
    if (NULL == pData) {
        m_nAecDataSize = 0;
        return;
    }

    if (NULL != m_pAecData) {
        delete m_pAecData;
        m_pAecData = NULL;
    }

    m_pAecData = pData;
    memcpy(m_pAecData, (char*)data, len);
    m_nAecDataSize = len;
}

VOID VR_VoiceBoxAudioInListener::OnAudioInStarted()
{
    VR_LOGD_FUNC();

    if (IsVoiceTag()) {
        m_spWavWriter.reset(VR_new VC_WavFileWriter());
        m_spWavWriter->Open(m_voiceTagPath);
    }
    else {
        bool outputWav = VR_ConfigureIF::Instance()->getOutputWavOption();
        if (outputWav) {
            if (!m_bOutFolderCreated) {
                CreateOutFolder();
                m_bOutFolderCreated = true;
            }

            std::stringstream ss;
            ss << m_recordFilePath << ++m_nFileNum << ".wav";

            VR_LOGD("wav file path is :%s", ss.str().c_str());
            m_spWavWriter.reset(VR_new VC_WavFileWriter());
            m_spWavWriter->Open(ss.str());
        }
    }
}

VOID VR_VoiceBoxAudioInListener::OnAudioInStopped()
{
    VR_LOGD_FUNC();

    if (m_spWavWriter) {
        m_spWavWriter->Close(); // here maybe lost some data if audio stop no sync.
        m_spWavWriter.reset();
    }
}

unsigned int
VR_VoiceBoxAudioInListener::GetAecAudioTypeDataSize()
{
    return m_nAecDataSize;
}

bool
VR_VoiceBoxAudioInListener::GetAecAudioTypeData(unsigned int& size, void* data)
{
    VR_LOGD_FUNC();

    if ((m_nAecDataSize <= 0) || (NULL == m_pAecData)) {
        return false;
    }

    size = m_nAecDataSize;
    if (NULL == data) {
        return true;
    }

    memcpy((char*) data, m_pAecData, size);
    return true;
}

void
VR_VoiceBoxAudioInListener::SetVoiceTagPath(const std::string& strFile)
{
    m_voiceTagPath = strFile;
}

/* EOF */
