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
 * @file VR_VoiceBoxAgentAudio.h
 * @brief interface for EngineWrapper callback.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VOICEBOXAGENTAUDIO_H
#define VR_VOICEBOXAGENTAUDIO_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

/* Suntec Header */
#ifndef VR_VOICEBOXEVENTSINK_H
#    include "VR_VoiceBoxEventSink.h"
#endif

#ifndef MEM_MAP_H
#    include "MEM_map.h"
#endif

#ifndef VR_VOICEBOXXMLBUILDER_H
#    include "VR_VoiceBoxXmlBuilder.h"
#endif

/* Forward Declaration */
class VR_VoiceBoxAgentAudio;

// Define the agent message handler pointer
typedef bool (VR_VoiceBoxAgentAudio::*AudioMsgHandler)(IVECIParsedMessage*);
typedef bool (VR_VoiceBoxAgentAudio::*AudioResultResult)(VR_VoiceBoxXmlParser&);

/**
 * @brief The VR_VoiceBoxAgentAudio class
 *
 * class declaration
 */
class VR_VoiceBoxAgentAudio : public VR_VoiceBoxEventSink
{
public:
    /**
     * Constructor
     *
     */
    VR_VoiceBoxAgentAudio(
        IVECIEngineClient& client,
        IVECIEngineCommand& engineCommand,
        VR_VoiceBoxEngineCallback& engineCallback
        );

    /**
     * Destructor
     *
     */
    virtual ~VR_VoiceBoxAgentAudio();

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

    /**
     * UnInitialize
     *
     * UnInitialize
     *
     * @param none
     * @return none
     * @attention none
     * @see none
     */
    virtual void UnInitialize();

    /**
     * ReplyQueryInfo
     *
     * ReplyQueryInfo
     *
     * @param none
     * @return bool
     * @attention none
     * @see none
     */
    virtual bool ReplyQueryInfo(
        VR_VoiceBoxXmlParser& parser
        );

    // IVECIEventSink's pure virtual functions
    virtual HRESULT QueryAgentSupport(VBT_CSTR szAgent, VBT_BOOL *pbSupported);
    virtual HRESULT OnAgentResultMessage(IVECITransaction* pTrans, IVECIParsedMessage* pResultMsg);
    virtual HRESULT OnAgentDriverMessage(IVECIParsedMessage* pDriverMsg);

    virtual bool ProcessAgentMessage(IVECIParsedMessage *pcMsg);

private:
    VoiceMap<std::string, AudioMsgHandler>::type m_audioMsgHandler;
    VoiceMap<std::string, AudioResultResult>::type m_audioMsgResult;

    VoiceMap<std::string, std::string>::type m_radioCommandtoContent;

    /**
     * Process Agent Message
     *
     * Process message that was sent by VoiceBox
     *
     * @param pcMsg [IN] : Agent result message or info driver message
     * @return bool
     * @attention none
     * @see none
     */

    /**
     * Copy Constructor
     *
     */
    VR_VoiceBoxAgentAudio(const VR_VoiceBoxAgentAudio&);

    /**
     * Copy Assignment Operator
     *
     */
    VR_VoiceBoxAgentAudio &operator=(const VR_VoiceBoxAgentAudio&);
    bool AudioOn(IVECIParsedMessage *pcMsg);
    bool AudioOff(IVECIParsedMessage *pcMsg);
    bool BrowseAlbum(IVECIParsedMessage *pcMsg);
    bool BrowseArtist(IVECIParsedMessage *pcMsg);
    bool BrowseArtistSongs(IVECIParsedMessage *pcMsg);
    bool BrowseComposer(IVECIParsedMessage *pcMsg);
    bool BrowseGenre(IVECIParsedMessage *pcMsg);
    bool BrowsePlaylist(IVECIParsedMessage *pcMsg);
    bool BrowsePodcast(IVECIParsedMessage *pcMsg);
    bool ChangeSourceCatch(IVECIParsedMessage *pcMsg);
    bool PlayDisambiguateSongAlbum(IVECIParsedMessage *pcMsg);
    bool PlayDisambiguateSongArtist(IVECIParsedMessage *pcMsg);
    bool OnPodcasts(IVECIParsedMessage *pcMsg);
    bool OnAlbums(IVECIParsedMessage *pcMsg);
    bool OnArtists(IVECIParsedMessage *pcMsg);
    bool OnDisambiguateAlbumArtist(IVECIParsedMessage *pcMsg);
    bool OnAudiobooks(IVECIParsedMessage *pcMsg);
    bool OnCatch(IVECIParsedMessage *pcMsg);
    bool OnComposers(IVECIParsedMessage *pcMsg);
    bool OnGenres(IVECIParsedMessage *pcMsg);
    bool OnPlaylists(IVECIParsedMessage *pcMsg);
    bool OnSongs(IVECIParsedMessage *pcMsg);
    bool MusicMenu(IVECIParsedMessage *pcMsg);
    bool ChangeSource(IVECIParsedMessage *pcMsg);
    bool MusicKeyboardDictation(IVECIParsedMessage *pcMsg);
    bool MusicNBest(IVECIParsedMessage *pcMsg);
    bool GetCurrentAlbumRequest(IVECIParsedMessage *pcMsg);
    bool GetCurrentArtistRequest(IVECIParsedMessage *pcMsg);
    bool GetCurrentComposerRequest(IVECIParsedMessage *pcMsg);
    bool GetCurrentGenreRequest(IVECIParsedMessage *pcMsg);
    bool GetPodcastEpisodesRequest(IVECIParsedMessage *pcMsg);
    bool TaskComplete(IVECIParsedMessage *pcMsg);
    bool GetCurrentAlbum(VR_VoiceBoxXmlParser& parser);
    bool GetCurrentArtist(VR_VoiceBoxXmlParser& parser);
    bool GetCurrentComposer(VR_VoiceBoxXmlParser& parser);
    bool GetCurrentGenre(VR_VoiceBoxXmlParser& parser);
    bool AudioNBest(IVECIParsedMessage *pcMsg);

    bool PlayAlbum(IVECIParsedMessage *pcMsg);
    bool PlayAll(IVECIParsedMessage *pcMsg);
    bool PlayArtist(IVECIParsedMessage *pcMsg);
    bool PlayAudioBook(IVECIParsedMessage *pcMsg);
    bool PlayComposer(IVECIParsedMessage *pcMsg);
    bool PlayGenre(IVECIParsedMessage *pcMsg);
    bool PlayPlaylist(IVECIParsedMessage *pcMsg);
    bool PlayPodcast(IVECIParsedMessage *pcMsg);
    bool PlayPodcastItem(IVECIParsedMessage *pcMsg);
    bool PlaySong(IVECIParsedMessage *pcMsg);
    bool ShufflePlayAlbum(IVECIParsedMessage *pcMsg);
    bool ShufflePlayArtist(IVECIParsedMessage *pcMsg);
    bool ShufflePlayComposer(IVECIParsedMessage *pcMsg);
    bool ShufflePlayPlaylist(IVECIParsedMessage *pcMsg);

    // -------------------------------------- Radio
    bool GoToRadio(IVECIParsedMessage *pcMsg);
    bool TuneRadioCatch(IVECIParsedMessage *pcMsg);
    bool TuneFrequencyBandAM(IVECIParsedMessage *pcMsg);
    bool TuneFrequencyBandFM(IVECIParsedMessage *pcMsg);
    bool TunePresetCatch(IVECIParsedMessage *pcMsg);
    bool TuneFMGenreCatch(IVECIParsedMessage *pcMsg);
    bool TuneSatelliteGenreCatch(IVECIParsedMessage *pcMsg);
    bool TuneFrequencyBandSatellite(IVECIParsedMessage *pcMsg);
    bool TuneFrequencyCatch(IVECIParsedMessage *pcMsg);

    bool ActivateRadio(IVECIParsedMessage *pcMsg);
    bool ChangeHDSubChannel(IVECIParsedMessage *pcMsg);
    bool TuneAMFrequency(IVECIParsedMessage *pcMsg);
    bool TuneFMFrequency(IVECIParsedMessage *pcMsg);
    bool TuneFMGenre(IVECIParsedMessage *pcMsg);
    bool TuneSatelliteChannelName(IVECIParsedMessage *pcMsg);
    bool TuneSatelliteChannelNumber(IVECIParsedMessage *pcMsg);
    bool TuneSatelliteGenre(IVECIParsedMessage *pcMsg);
    bool VBTSendCommand(IVECIParsedMessage *pcMsg);
    bool TuneFMHDFrequency(IVECIParsedMessage *pcMsg);
    bool TunePreset(IVECIParsedMessage *pcMsg);
    bool TuneFrequencyBandSatelliteStation(IVECIParsedMessage *pcMsg);
    bool TuneFrequencyBandAMFrequency(IVECIParsedMessage *pcMsg);
    bool TuneFrequencyBandFMFrequency(IVECIParsedMessage *pcMsg);
    bool RadioNBest(IVECIParsedMessage *pcMsg);
    bool AudioBack(IVECIParsedMessage *pcMsg);
    bool AudioCancel(IVECIParsedMessage *pcMsg);
    bool AudioHelp(IVECIParsedMessage *pcMsg);
    bool AudioStartOver(IVECIParsedMessage *pcMsg);
    bool AudioStartPause(IVECIParsedMessage *pcMsg);
    bool AudioStartRepeat(IVECIParsedMessage *pcMsg);
    bool EscalatingError(IVECIParsedMessage *pcMsg);

    bool ActiveFrequencyBandRequest(IVECIParsedMessage *pcMsg);
    bool FrequencyBandStatusRequest(IVECIParsedMessage *pcMsg);
    bool HDSubChannelAvailforCurFreRequest(IVECIParsedMessage *pcMsg);
    bool HDSubchannelAvailableforFreRequest(IVECIParsedMessage *pcMsg);
    bool PresetDefinedRequest(IVECIParsedMessage *pcMsg);
    bool PresetsAvailableRequest(IVECIParsedMessage *pcMsg);
    bool RadioStatusRequest(IVECIParsedMessage *pcMsg);
    bool StationsAvailableinFMGenreRequest(IVECIParsedMessage *pcMsg);

    bool ActiveFrequencyBandReply(VR_VoiceBoxXmlParser& parser);
    bool FrequencyBandStatusReply(VR_VoiceBoxXmlParser& parser);
    bool HDSubChannelAvailforCurFreReply(VR_VoiceBoxXmlParser& parser);
    bool HDSubchannelAvailableforFreReply(VR_VoiceBoxXmlParser& parser);
    bool PresetDefinedReply(VR_VoiceBoxXmlParser& parser);
    bool PresetsAvailableReply(VR_VoiceBoxXmlParser& parser);
    bool StationsAvailableinFMGenreReply(VR_VoiceBoxXmlParser& parser);
    bool RadioStatusReply(VR_VoiceBoxXmlParser& parser);

    void SendMusicPlay(const std::string& strSourceID,
    const std::string& strPlayType, const std::string& strID,
    const std::string& strShuffle, const std::string& strSubId);

    pugi::xml_node BuildScreenDisplayFront(VR_VoiceBoxXmlBuilder& xmlBulder,
    const std::string& strContent, const std::string& strPromtValue);

    void BuildScreenDisplayAll(const std::string& strContent, const std::string& strPrompt);

    void BuildListHeader(IVECIParsedMessage *pcMsg,
    VR_VoiceBoxXmlBuilder& xmlBulder, pugi::xml_node& nodeList);

    void BuildMusicInfo(VR_VoiceBoxXmlBuilder& xmlBulder, pugi::xml_node& xmlNode,
    const std::string& strName, const std::string& strSourceId, const std::string& strId);

    void OnInfoRequest(const std::string& strOp);

    void TuneFrequency(const std::string& strBand, const std::string& strFrequency);

    void PlayByGenre(const std::string& strBand, const std::string& strGenre,
    const std::string& strID);

    static int GetRelateInfoBySql(void* strName, int nCount, char** pValue, char** pName);

    void GetRelateInfoByBaseID(const std::string& strBaseTableName,
    const std::string& strRelateTableName, const std::string& strRelateIndexName,
    const std::string& strIndexName, const std::string& strId,
    VoiceMap<std::string, std::string>::type& mapExternalId2Name);

    void BuildListInfo(IVECIParsedMessage *pcMsg,
    VR_VoiceBoxXmlBuilder& xmlBulder,
    pugi::xml_node nodeSelectList,
    const std::string& strNamePara,
    const std::string& strIDPara,
    const std::string& strSourceID);

    std::string GetStatusByErrCode(const std::string& strErrCode);

};

#endif // VR_VOICEBOXAGENTAUDIO_H
/* EOF */