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
 * @file VR_DialogEngineBargein.h
 * @brief inteface for VR_DialogEngineBargein or asr engine to perfer command
 *
 *
 * @attention used for C++ only.
 */

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#ifndef _VR_DIALOGENGINE_BARGEIN_H_
#define _VR_DIALOGENGINE_BARGEIN_H_

#include "VR_Macro.h"
#include "config.h"
#include "stdafx.h"
#include <memory>
#include <mutex>
#include <condition_variable>
#include "VR_DialogEngineIF.h"
#include "VR_AudioInSource.h"
#include <boost/scoped_array.hpp>
#include <boost/shared_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "Vr_Asr_Engine.h"

/**
 * @brief The VR_DialogEngineBargein class
 *
 * dialogEngine for bargin model
 */
class VR_API VR_DialogEngineBargein : public VR_DialogEngineIF, public VR_AudioInSource::Listener
{
public:
    enum BeepType
    {
        start = 1,
        end = 2,
        processed = 3
    };

public:
    VR_DialogEngineBargein();

    virtual ~VR_DialogEngineBargein();

    // sync
    virtual bool Initialize(VR_DialogEngineListener *listener, const VR_Settings &settings);
    
    virtual std::string getHints(const std::string& hintsParams)
    {
        return "";
    }

    // async
    virtual bool Start();

    // async
    virtual void Stop();

    // async
    virtual bool SendMessage(const std::string& message, int actionSeqId = -1);

    // sync
    virtual void UnInitialize();

    bool isAsrNotRunning();

protected:
    enum BargeinDebugMode
    {
        BARGEIN_DEBUG_NONE,
        BARGEIN_DEBUG_WAV,
        BARGEIN_DEBUG_TTS
    };

    enum TTsPlayType
    {
        PLAY_WAV_TYPE,
        PLAY_TTS_TYPE
    };



    // Not ownned
    VR_DialogEngineListener* m_pEngineListener;

    std::shared_ptr<N_Vr::N_Asr::C_Engine> m_spAsrEngine;
    std::shared_ptr<VR_AudioInSource> m_spAudioInSource;
    std::mutex m_AsrMutex;
    std::condition_variable m_AsrCoVar;
    bool m_AsrIsRunning;

    VoiceList<std::string>::type m_ttsTexts;
    VoiceVector<std::string>::type m_vectorFiles;
    VoiceList<std::string>::iterator m_ttsTextsIt;

    TTsPlayType m_ttsPlayType;
    BargeinDebugMode  m_debugMode;

    // Inherrited from VR_AudioInSource::Listener
    virtual void OnAudioInStartedAction();

private:
    void stopTTs();
    void playTTs();
    void playBeep(BeepType type);
    void stopWav();
    void playWav();
    void requestAsr();
    void cancelAsr();
    void requestAction(const std::string& msg);
    void initTTSText();
    void closeSession();
    void handlePtt();
    void onAsrPhase(N_Vr::N_Asr::C_Event_Phase const& phase);
    void onAsrNotify(N_Vr::N_Asr::C_Event_Notify const& notify);
    void onAsrResult(N_Vr::N_Asr::C_Event_Result const& result);
    void createDocList(VoiceVector<std::string>::type &doc_list, std::string dicPath);
    void pushTxt2List(const std::string& fileName);
    void paramSetting();

    VR_DialogEngineBargein(const VR_DialogEngineBargein &copy);
    VR_DialogEngineBargein & operator=(const VR_DialogEngineBargein &copy);

    VC_WavFileWriter*      m_pWriter;
};

#endif // _VR_DIALOGENGINE_BARGEIN_H_
/* EOF */
