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
 * @file VR_VoiceBoxAudioIn.h
 * @brief interface for VR_VoiceBoxAudioIn.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VOICEBOXAUDIOIN_H
#define VR_VOICEBOXAUDIOIN_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

/* Forward Declaration */
class VC_AudioIn;
class VR_VoiceBoxAudioInListener;
class VC_WavFileWriter;

/**
 * @brief The VR_VoiceBoxAudioIn class
 *
 * class declaration
 */
class VR_VoiceBoxAudioIn
{
public:
    VR_VoiceBoxAudioIn(
        VR_VoiceBoxAudioInListener& listener
    );

    ~VR_VoiceBoxAudioIn();

    bool StartAudioIn();

    bool StopAudioIn();

    /** 
     * Open audio recorder device
     * @param sampleRate audio in device sample rate,such as 8000, 16000 HZ
     */
    bool Open();
    
    /** 
     * Start audio recorder recording
     */
    bool Start();

    /** 
     * Stop audio recorder recording
     */
    bool Stop();
    
    /** 
     * Close audio recorder 
     */
    bool Close();

    void SetBargeIn(bool bOn);

    void SetVoiceTag(bool bOn);

    std::string GetTempPcmFile();

    /**
     * Get AEC audio type data
     */
    unsigned int GetAecAudioTypeDataSize();
    bool GetAecAudioTypeData(unsigned int& size, void* data);

private:
    enum
    {
        BUFFER_COUNT = 4,
        BUFFER_SIZE  = 4096,
        SAMPLE_RATE  = 16000,
    };

    VC_AudioIn*                  m_audioIn;
    VR_VoiceBoxAudioInListener&  m_listener;
    bool                         m_bRecording;
    bool                         m_bBargeIn;
    bool                         m_bVoiceTag;
    bool                         m_bInputWavOption;

private:
    int                          initDir();
    std::string                  m_voiceTagFilePath;  
    
};

#endif // VR_VOICEBOXAUDIOIN_H
/* EOF */
