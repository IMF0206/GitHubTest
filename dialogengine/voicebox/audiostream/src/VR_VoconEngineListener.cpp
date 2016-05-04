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
#ifndef VR_VOCONENGINELISTENER_H
#    include "VR_VoconEngineListener.h"
#endif

#ifndef VR_VOICEBOXAUDIOIN_H
#    include "VR_VoiceBoxAudioIn.h"
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

// Constructor
VR_VoconEngineListener::VR_VoconEngineListener(
    VR_VoiceBoxAudioIn& audioIn,
    VR_AudioBufferManager& bufferManager
)
: m_audioIn(audioIn)
, m_bufferManager(bufferManager)
, m_pcAecData(NULL)
{
}

// Destructor
VR_VoconEngineListener::~VR_VoconEngineListener()
{
    delete[] m_pcAecData;
}

int
VR_VoconEngineListener::M_Fn_Start()
{
    VR_LOGD_FUNC();

    return 0;
}

void
VR_VoconEngineListener::M_Fn_Get_Data_Type_Audio_SSE(
    void ** ppvoid_Data_Type_Audio_SSE,
    size_t * pi_Size_Data_Type_Audio_SSE
)
{
    VR_LOGD_FUNC();

    *pi_Size_Data_Type_Audio_SSE = 0;

    unsigned int size = m_audioIn.GetAecAudioTypeDataSize();
    if (0 == size) {
        return;
    }

    char* pcAecData = VR_new char[size];
    if (NULL == pcAecData) {
        return;
    }

    unsigned int nAecDataSize = 0;
    if (!m_audioIn.GetAecAudioTypeData(nAecDataSize, pcAecData)) {
        delete[] pcAecData;
        pcAecData = NULL;
        return;
    }

    delete[] m_pcAecData;
    m_pcAecData = pcAecData;

    *ppvoid_Data_Type_Audio_SSE = m_pcAecData;
    *pi_Size_Data_Type_Audio_SSE = nAecDataSize;
}

int
VR_VoconEngineListener::M_Fn_Stop()
{
    VR_LOGD_FUNC();

    return 0;
}

int
VR_VoconEngineListener::M_Fn_Data_Available_Check(bool* pb_Data_Available)
{
    VR_LOGD_FUNC();

    *pb_Data_Available = m_bufferManager.IsAvailable();
    return 0;
}

int
VR_VoconEngineListener::M_Fn_Data_Fetch(void** ppvoid_Buffer, size_t* pi_Size)
{
    VR_LOGD_FUNC();

    return m_bufferManager.FetchData(ppvoid_Buffer, pi_Size);
}

int
VR_VoconEngineListener::M_Fn_Data_Return(void* const& rpvoid_Buffer)
{
    VR_LOGD_FUNC();

    return m_bufferManager.ReturnData(rpvoid_Buffer);
}

/* EOF */