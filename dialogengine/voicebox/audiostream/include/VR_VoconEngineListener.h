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
 * @file VR_VoconEngineListener.h
 * @brief interface for VR_VoconEngineListener.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VOCONENGINELISTENER_H
#define VR_VOCONENGINELISTENER_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

/* Suntec Header */
#ifndef __Vr_Asr_Audio_In_h__
#    include "Vr_Asr_Audio_In.h"
#endif

/* Forward Declaration */
class VR_VoiceBoxAudioIn;
class VR_AudioBufferManager;

/**
 * @brief The ~VR_VoconEngineListener(); class
 *
 * class declaration
 */
class VR_VoconEngineListener : public N_Vr::N_Asr::C_Audio_In
{
public:
    VR_VoconEngineListener(
        VR_VoiceBoxAudioIn& audioIn,
        VR_AudioBufferManager& bufferManager
    );

    ~VR_VoconEngineListener();

    virtual void
    M_Fn_Get_Data_Type_Audio_SSE(
        void ** ppvoid_Data_Type_Audio_SSE,
        size_t * pi_Size_Data_Type_Audio_SSE
    );
    virtual int M_Fn_Start();
    virtual int M_Fn_Stop();
    virtual int M_Fn_Data_Available_Check(bool* pb_Data_Available);
    virtual int M_Fn_Data_Fetch(void** ppvoid_Buffer, size_t* pi_Size);
    virtual int M_Fn_Data_Return(void* const& rpvoid_Buffer);

private:
    VR_VoiceBoxAudioIn&     m_audioIn;
    VR_AudioBufferManager&  m_bufferManager;
    char*                   m_pcAecData;
};

#endif // VR_VOCONENGINELISTENER_H
/* EOF */
