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
#ifndef VR_VOICEBOXAGENTAUDIO_AU_H
#define VR_VOICEBOXAGENTAUDIO_AU_H

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
class VR_VoiceBoxAgentAudio_AU;

// Define the agent message handler pointer
typedef bool (VR_VoiceBoxAgentAudio_AU::*AudioMsgHandler_AU)(IVECIParsedMessage*);
typedef bool (VR_VoiceBoxAgentAudio_AU::*AudioResultResult_AU)(VR_VoiceBoxXmlParser&);

/**
 * @brief The VR_VoiceBoxAgentAudio class
 *
 * class declaration
 */
class VR_VoiceBoxAgentAudio_AU : public VR_VoiceBoxEventSink
{
public:
    /**
     * Constructor
     *
     */
    VR_VoiceBoxAgentAudio_AU(
        IVECIEngineClient& client,
        IVECIEngineCommand& engineCommand,
        VR_VoiceBoxEngineCallback& engineCallback
        );

    /**
     * Destructor
     *
     */
    virtual ~VR_VoiceBoxAgentAudio_AU();

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
    VoiceMap<std::string, AudioMsgHandler_AU>::type m_audioMsgHandler;
    VoiceMap<std::string, AudioResultResult_AU>::type m_audioMsgResult;

    VoiceMap<std::string, std::string>::type m_musicCommandtoContent;

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
    VR_VoiceBoxAgentAudio_AU(const VR_VoiceBoxAgentAudio_AU&);

    /**
     * Copy Assignment Operator
     *
     */
    VR_VoiceBoxAgentAudio_AU &operator=(const VR_VoiceBoxAgentAudio_AU&);
    bool AudioHome(IVECIParsedMessage *pcMsg);
    bool AudioMenu(IVECIParsedMessage *pcMsg);
    bool BusyUpdatingGrammar(IVECIParsedMessage *pcMsg);
    bool ChangeSource(IVECIParsedMessage *pcMsg);
    bool ChangeSourceNotConnected(IVECIParsedMessage *pcMsg);
    bool NoAlbums(IVECIParsedMessage *pcMsg);
    bool NoArtists(IVECIParsedMessage *pcMsg);
    bool NoAudioSourceConnected(IVECIParsedMessage *pcMsg);
    bool NoPlaylists(IVECIParsedMessage *pcMsg);
    bool NoSongs(IVECIParsedMessage *pcMsg);
    bool PlayAlbum(IVECIParsedMessage *pcMsg);
    bool PlayAlbums(IVECIParsedMessage *pcMsg);
    bool PlayArtist(IVECIParsedMessage *pcMsg);
    bool PlayArtists(IVECIParsedMessage *pcMsg);
    bool PlayDisambiguateAlbumArtist(IVECIParsedMessage *pcMsg);
    bool PlayDisambiguateSongAlbum(IVECIParsedMessage *pcMsg);
    bool PlayPlaylist(IVECIParsedMessage *pcMsg);
    bool PlayDisambiguateSongArtist(IVECIParsedMessage *pcMsg);
    bool PlayPlaylistNoSongs(IVECIParsedMessage *pcMsg);
    bool PlayPlaylists(IVECIParsedMessage *pcMsg);
    bool PlaySong(IVECIParsedMessage *pcMsg);
    bool PlaySongs(IVECIParsedMessage *pcMsg);
    bool TooManyAlbums(IVECIParsedMessage *pcMsg);
    bool TooManyArtists(IVECIParsedMessage *pcMsg);
    bool TooManyPlaylists(IVECIParsedMessage *pcMsg);
    bool TooManySongs(IVECIParsedMessage *pcMsg);
    bool SourceConnectedRequest(IVECIParsedMessage* parser);
    bool TaskComplete(IVECIParsedMessage *pcMsg);
    bool AudioNBest(IVECIParsedMessage *pcMsg);

    // -------------------------------------- Radio
    bool PresetNotDefined(IVECIParsedMessage *pcMsg);
    bool PresetsNotAvailable(IVECIParsedMessage *pcMsg);
    bool TuneAMFrequency(IVECIParsedMessage *pcMsg);
    bool TuneFMFrequency(IVECIParsedMessage *pcMsg);
    bool TuneFrequencyCatch(IVECIParsedMessage *pcMsg);
    bool TunePresetCatch(IVECIParsedMessage *pcMsg);

    bool TunePreset(IVECIParsedMessage *pcMsg);

    bool PresetDefinedRequest(IVECIParsedMessage *pcMsg);
    bool PresetsAvailableRequest(IVECIParsedMessage *pcMsg);

    bool AudioBack(IVECIParsedMessage *pcMsg);
    bool AudioCancel(IVECIParsedMessage *pcMsg);
    bool AudioHelp(IVECIParsedMessage *pcMsg);
    bool AudioStartOver(IVECIParsedMessage *pcMsg);
    bool AudioStartPause(IVECIParsedMessage *pcMsg);
    bool AudioStartRepeat(IVECIParsedMessage *pcMsg);

    bool PresetDefinedReply(VR_VoiceBoxXmlParser& parser);
    bool PresetsAvailableReply(VR_VoiceBoxXmlParser& parser);
   
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

    static int GetNameByID(void*, int nCount, char** pValue, char** pName);

    void BuildListInfo(IVECIParsedMessage *pcMsg, 
    VR_VoiceBoxXmlBuilder& xmlBulder,
    pugi::xml_node nodeSelectList, 
    const std::string& strNamePara, 
    const std::string& strIDPara, 
    const std::string& strSourceID);

};

#endif // VR_VOICEBOXAGENTAUDIO_H
/* EOF */
