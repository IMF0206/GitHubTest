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
 * @file VR_AudioStream.h
 * @brief interface for VR_AudioStream.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_AUDIOSTREAM_H
#define VR_AUDIOSTREAM_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

/* Boost Header */
#include <boost/shared_ptr.hpp>

/* Suntec Header */
#ifndef VR_AUDIOSTREAMIF_H
#    include "VR_AudioStreamIF.h"
#endif

#ifndef __Vr_Asr_Engine_h__
#    include "Vr_Asr_Engine.h"
#endif

#ifndef VR_AUDIOBUFFERMANAGER_H
#    include "VR_AudioBufferManager.h"
#endif

/* Forward Declaration */
class VR_VoiceBoxAudioIn;
class VR_VoiceBoxAudioInListener;
class VR_AudioStreamListener;
class VR_VoconEngineListener;
class VR_VaalStreamWrap;
class VR_VaalStream;

/**
 * @brief The VR_AudioStream class
 *
 * class declaration
 */
class VR_AudioStream : public VR_AudioStreamIF
{
public:
    /* Constructor */
    VR_AudioStream();

    /* Destructor */
    ~VR_AudioStream();

    virtual bool Initialize(VR_AudioStreamListener* listener);
    virtual void UnInitialize();
    virtual bool ShareFrontEnd(void *handleAsr, void *handleAcMod);
    virtual bool ReleaseFrontEnd();
    virtual bool StartAudioInWithBargeIn();
    virtual bool StartAudioInWithCache();
    virtual bool StartAudioIn();
    virtual bool StopAudioIn();
    virtual unsigned int GetAecAudioTypeDataSize();
    virtual bool GetAecAudioTypeData(unsigned int& size, void* data);

    // Get a vaal stream instance, and transfer it to VBT.
    virtual VR_VaalStream* GetVaalStream();
    virtual void SetVoiceTag(bool bOn);
    virtual std::string GetTempPcmFile();

    // BOS detected engine's callback
    void onAsrPhase(N_Vr::N_Asr::C_Event_Phase const& phase);
    void onAsrNotify(N_Vr::N_Asr::C_Event_Notify const& notify);
    void onAsrResult(N_Vr::N_Asr::C_Event_Result const& result);

private:
    VR_AudioBufferManager       m_audioBufferManager;
    VR_VoiceBoxAudioIn*         m_voiceBoxAudioIn;
    VR_VoiceBoxAudioInListener* m_audioInListener;
    VR_VaalStreamWrap*          m_vaalStream;
    VR_AudioStreamListener*     m_audioStreamListener;
    N_Vr::N_Asr::C_Engine*      m_voconEngine;
    boost::shared_ptr<VR_VoconEngineListener>  m_voconEngineListener;
    bool                        m_bBargeInDetecting;
    bool                        m_bFirstAsrFactorySucceed;
};

#endif // VR_AUDIOSTREAM_H
/* EOF */
