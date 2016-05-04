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
 * @file VR_VoiceBoxCatalogAudio.h
 * @brief interface for EngineWrapper callback.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VOICEBOXCATALOGAUDIO_H
#define VR_VOICEBOXCATALOGAUDIO_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <string>

/* VBT Header */
#ifndef VECIPTR_H
#    include "VECIPtr.h"
#endif

/* Suntec Header */
#ifndef VR_VOICEBOXCATALOG_H
#    include "VR_VoiceBoxCatalog.h"
#endif

#ifndef MEM_VECTOR_H
#    include "MEM_vector.h"
#endif

#ifndef VR_VOICEBOXENGINECALLBACK_H
#    include "VR_VoiceBoxEngineCallback.h"
#endif

#include "VR_VoiceBoxXmlParser.h"

#ifndef MEM_MAP_H
#    include "MEM_map.h"
#endif

#ifndef MEM_LIST_H
#    include "MEM_list.h"
#endif

/* Forward Declaration */

/**
 * @brief The VR_VoiceBoxCatalogAudio class
 *
 * class declaration
 */
class VR_VoiceBoxCatalogAudio : public VR_VoiceBoxCatalog
{
public:
    /**
     * Constructor
     *
     */
    VR_VoiceBoxCatalogAudio(
        IVECIEngineClient& client,
        IVECIEngineCommand& engineCommand,
        VR_VoiceBoxEngineCallback& engineCallback
        );

    /**
     * Destructor
     *
     */
    virtual ~VR_VoiceBoxCatalogAudio();

    /**
     * Initialize
     *
     * Initialize
     *
     * @param none
     * @return bool
     * @attention none
     * @see none
     */
    virtual bool Initialize();

    bool UpdateMusicAudioSources(VR_VoiceBoxXmlParser& parser);
    bool UpdateRadioFMGenres(VR_VoiceBoxXmlParser& parser);
    bool UpdateRadioSatelliteChannelNames(VR_VoiceBoxXmlParser& parser);
    bool UpdateRadioSatelliteChannelNumbers(VR_VoiceBoxXmlParser& parser);
    bool UpdateRadioSatelliteGenres(VR_VoiceBoxXmlParser& parser);
    bool UpdateRadioHDSubChannels(VR_VoiceBoxXmlParser& parser);

    bool MusicGrammarDiff(VR_VoiceBoxXmlParser& parser);

    bool MusicDeleteUpdate(VR_VoiceBoxXmlParser& parser);

    bool MusicAddUpdate(VR_VoiceBoxXmlParser& parser);

    bool SetupMusicData(VR_VoiceBoxXmlParser& parser);

    bool MusicGrammarActive(VR_VoiceBoxXmlParser& parser);

    bool MusicGrammarDisActive(VR_VoiceBoxXmlParser& parser);

    void PauseGrammarMusic();

    void PauseGrammarRadio();

    void ResumeGrammarMusic();

    void ResumeGrammarRadio();

    bool MusicGrammarAudioSourceOC(VR_VoiceBoxXmlParser& parser);

    void InitialMusicPersonData();

    void SetDataActiveSource(const std::string& strActiveSourceId);

    void CancelMusicGrammarUpdate();

    void CancelRadioGrammarUpdate();

    static std::string m_strSourceId;
    static std::string m_strDBPath;
    static bool m_bIPodUSB2;
    static bool m_bUSBIPod2;

private:
    IVECIEngineClient&                  m_client;           ///< EngineClient
    IVECIEngineCommand&                 m_engineCommand;
    VR_VoiceBoxEngineCallback&          m_engineCallback; ///< Callback for the dialog engine
    CVECIPtr<IVECIAgentDataCommand>     m_agentDataCommandMusic; ///< Process AgentData related request
    CVECIPtr<IVECIAgentDataCommand>     m_agentDataCommandRadio; ///< Process AgentData related request
    std::string                        m_strSourceIdDiff;

    VoiceMap<std::string, VoiceList<std::string >::type >::type m_mapUSENAudioSource;
    VoiceMap<std::string, VoiceList<std::string >::type >::type m_mapUSESAudioSource;
    VoiceMap<std::string, VoiceList<std::string >::type >::type m_mapUSFRAudioSource;

    /**
     * Copy Constructor
     *
     */
    VR_VoiceBoxCatalogAudio(const VR_VoiceBoxCatalogAudio&);

    bool LoadAudioSource(
    const VoiceVector<AudioSourceInfo>::type& vecAudioSource,
    CVECIPtr<IVECIListItems>& audioSourceList);

    bool LoadFormalPronInfo(
    const VoiceVector<FormalPronInfo>::type& vecFormalPronInfo,
    CVECIPtr<IVECIListItems>& listItems);

    bool LoadSatChannelNumber(
    const VoiceVector<FormalTwoItem>::type& vecSatChannelNumber,
    CVECIPtr<IVECIListItems>& satChannelNumberList);

    bool LoadHdSubChannel(
    const VoiceVector<FormalOtherInfo>::type& vecHdSubChannelInfo,
    CVECIPtr<IVECIListItems>& listHdSubChannel);

    void GetTableHandlerName(const std::string& strGrammarName,
    std::string& strTableName, std::string& strHandlerName);

    void AddAudioSourceItem(const std::string& stdId,
    const std::string& strFormal, const std::string& strAlias,
    const std::string& strGrammarId, const std::string& strIspermanent,
    CVECIPtr<IVECIListItems>& audioSourceList);

    void AddAudioSourceItem(
    const std::string& strId, const std::string& strFormal, const std::string& strGrammarId,
    const VoiceMap<std::string, VoiceList<std::string >::type >::type& mapUSAudioSource,
    CVECIPtr<IVECIListItems>& audioSourceList);

    void SetMusicDBPathByPath(std::string& strPath);

    void SetMusicEnableByCount(const int iSongItem, const int iOtherItem);

    void MusicRemoveAllGrammars(const std::string& strSourceId);
    bool RadioAgentFMGenres(const VoiceVector<FormalPronInfo>::type& vecFmGenre, const std::string& strLastFlg);
    bool RadioAgentSatelliteChannelNames(const VoiceVector<FormalPronInfo>::type& vecSatChannelName, const std::string& strLastFlg);
    bool RadioAgentSatelliteChannelNumbers(const VoiceVector<FormalTwoItem>::type& vecSatChannelNumber, const std::string& strLastFlg);
    bool RadioAgentSatelliteGenres(const VoiceVector<FormalPronInfo>::type& vecSatGenre, const std::string& strLastFlg);
    bool RadioAgentHDSubChannels(const VoiceVector<FormalOtherInfo>::type& vecHdSubChannel, const std::string& strLastFlg);
    bool RadioRemoveGrammer(const std::string& strTableName, const std::string& strHandlerName, const std::string& strLastFlg);

    void SaveMusicSourceId(const std::string& strSourceId);

    void InsertAudioSourceBuf(
    const std::string& strFormal, const std::string& strAlias,
    VoiceMap<std::string, VoiceList<std::string >::type >::type& mapUSAudioSource);

    /**
     * Copy Assignment Operator
     *
     */
    VR_VoiceBoxCatalogAudio &operator=(const VR_VoiceBoxCatalogAudio&);

};


#endif // VR_VOICEBOXCATALOGAUDIO_H
/* EOF */