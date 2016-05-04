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

/* Standard Header */
#include <sstream>
/* VBT Header */
// #ifndef VECIOUTSTR_H
// #    include "VECIOutStr.h"
// #endif
#include "VR_Log.h"


#ifndef VECICSTR_H
#    include "VECICStr.h"
#endif

#ifndef IVBTENGINECLIENT_H
#    include "IVBTEngineClient.h"
#endif

#ifndef VBT_SDK_AGENT_H
#    include "VBT_SDK_Agent.h"
#endif

#ifndef VBT_SDK_ACTION_TYPES_H
#    include "VBT_SDK_Action_Types.h"
#endif

#ifndef VBT_SDK_LIST_TYPES_H
#    include "VBT_SDK_List_Types.h"
#endif

/* Suntec Header */
#ifndef VR_VOICEBOXAGENTAUDIO_AU_H
#    include "VR_VoiceBoxAgentAudio_AU.h"
#endif

#ifndef VR_VOICEBOXXMLNODESTRING_H
#    include "VR_VoiceBoxXmlNodeString.h"
#endif

#ifndef VR_VOICEBOXLOG_H
#    include "VR_VoiceBoxLog.h"
#endif

#ifndef VR_VOICEBOXENGINECALLBACK_H
#    include "VR_VoiceBoxEngineCallback.h"
#endif

#ifndef VR_VOICEBOXCATALOGAUDIO_H
#    include "VR_VoiceBoxCatalogAudio.h"
#endif

using namespace nutshell;

// audioActionHandler
struct AudioActionHandler_AU
{
    char*             szAction; // Action type
    AudioMsgHandler_AU  handler;  // Handler pointer
};

const std::string cstVO31001 = "Problem with audio device connection. Check your iPod or USB device.";
const std::string cstVO31002 = "Your music library is being prepared for use by voice recognition... Try again later.";
const std::string cstVO31003 = "Because there are no artists detected on your device, this command is not available.";
const std::string cstVO31004 = "Because of the number of artists detected on your device, this command is not available.";
const std::string cstVO31005 = "Because your music library could not be prepared for voice recognition, this command is not available.";
const std::string cstVO31006 = "Problem with audio device connection. Check your iPod or USB device.";
const std::string cstVO31007 = "Your music library is being prepared for use by voice recognition... Try again later.";
const std::string cstVO31008 = "Because there are no albums detected on your device, this command is not available.";
const std::string cstVO31009 = "Because of the number of albums detected on your device, this command is not available.";
const std::string cstVO31010 = "Because your music library could not be prepared for voice recognition, this command is not available.";
const std::string cstVO31011 = "Problem with audio device connection. Check your iPod or USB device.";
const std::string cstVO31012 = "Your music library is being prepared for use by voice recognition... Try again later.";
const std::string cstVO31013 = "Because there are no playlists detected on your device, this command is not available.";
const std::string cstVO31014 = "Because of the number of playlists detected on your device, this command is not available.";
const std::string cstVO31015 = "Because of the amount of music detected on your device, this command is not available.";
const std::string cstVO31016 = "Problem with audio device connection. Check your iPod or USB device.";
const std::string cstVO31017 = "Your music library is being prepared for use by voice recognition... Try again later.";
const std::string cstVO31018 = "Because there are no songs detected on your device, this command is not available.";
const std::string cstVO31019 = "Because of the number of songs detected on your device, this command is not available.";
const std::string cstVO31020 = "Because of the amount of music detected on your device, this command is not available.";
const std::string cstVO31021 = "Problem with audio device connection. Check your iPod or USB device.";
const std::string cstVO31022 = "Your music library is being prepared for use by voice recognition... Try again later.";
const std::string cstVO31023 = "Your music library is being prepared for use by voice recognition... Try again later.";
const std::string cstVO31024 = "Problem with audio device connection. Check your iPod or USB device.";
const std::string cstVO31025 = "Problem with audio device connection. Check your iPod or USB device.";
const std::string cstVO31026 = "Your music library is being prepared for use by voice recognition... Try again later.";
const std::string cstVO31027 = "No music files found.";
const std::string cstVO31028 = "Problem with audio device connection. Check your iPod or USB device.";
const std::string cstVO31029 = "Your music library is being prepared for use by voice recognition... Try again later.";
const std::string cstVO31030 = "Preset [%s] is not available.";
const std::string cstVO31031 = "There are no USB devices connected.";
const std::string cstVO31032 = "There is no connected iPod.";
const std::string cstVO31033 = "There are no Bluetooth audio devices connected.";

// Constructor
VR_VoiceBoxAgentAudio_AU::VR_VoiceBoxAgentAudio_AU(
    IVECIEngineClient& client,
    IVECIEngineCommand& engineCommand,
    VR_VoiceBoxEngineCallback& engineCallback
)
: VR_VoiceBoxEventSink(client, engineCommand, engineCallback)
{
}

// Destructor
VR_VoiceBoxAgentAudio_AU::~VR_VoiceBoxAgentAudio_AU()
{
}

bool
VR_VoiceBoxAgentAudio_AU::Initialize()
{
    const AudioActionHandler_AU actionHandlerMap[] = {
        { "Audio Home",
        &VR_VoiceBoxAgentAudio_AU::AudioHome },  // "Audio Home" Task Complete (re-activate VR)
        { "Audio Menu",
        &VR_VoiceBoxAgentAudio_AU::AudioMenu },  // "Audio Menu" Follow Up
        { "Busy Updating Grammar",
        &VR_VoiceBoxAgentAudio_AU::BusyUpdatingGrammar },  // "Busy Updating Grammar" Task Complete
        { "Change Source",
        &VR_VoiceBoxAgentAudio_AU::ChangeSource },  // "Change Source" Task Complete Go to top screen for selected source
        { "Change Source - Not Connected",
        &VR_VoiceBoxAgentAudio_AU::ChangeSourceNotConnected },   // "Change Source - Not Connected" Task Complete
        { "No Albums",
        &VR_VoiceBoxAgentAudio_AU::NoAlbums },  // "No Albums" Task Complete
        { "No Artists",
        &VR_VoiceBoxAgentAudio_AU::NoArtists },  // "No Artists" Task Complete
        { "No Audio Source Connected",
        &VR_VoiceBoxAgentAudio_AU::NoAudioSourceConnected },  // "No Audio Source Connected" Task Complete
        { "No Playlists",
        &VR_VoiceBoxAgentAudio_AU::NoPlaylists },  // "No Playlists" Task Complete
        { "No Songs",
        &VR_VoiceBoxAgentAudio_AU::NoSongs },  // "No Songs" Task Complete
        { "Play Album",
        &VR_VoiceBoxAgentAudio_AU::PlayAlbum },  // "Play Album" Task Complete
        { "Play Albums",
        &VR_VoiceBoxAgentAudio_AU::PlayAlbums },  // "Play Albums" Follow-Up  5
        { "Play Artist",
        &VR_VoiceBoxAgentAudio_AU::PlayArtist },  // "Play Artist" Task Complete
        { "Play Artists",
        &VR_VoiceBoxAgentAudio_AU::PlayArtists },  // "Play Artists" Follow-Up  7
        { "Play Disambiguate Album Artist",
        &VR_VoiceBoxAgentAudio_AU::PlayDisambiguateAlbumArtist },  // "Play Disambiguate Album Artist" Follow-Up  VR-SYS-01
        { "Play Disambiguate Song Album",
        &VR_VoiceBoxAgentAudio_AU::PlayDisambiguateSongAlbum },  // "Play Disambiguate Song Album" Follow-Up 4
        { "Play Disambiguate Song Artist",
        &VR_VoiceBoxAgentAudio_AU::PlayDisambiguateSongArtist },  // "Play Disambiguate Song Artist" Follow-Up 2
        { "Play Playlist",
        &VR_VoiceBoxAgentAudio_AU::PlayPlaylist },  // "Play Playlist" Task Complete
        { "Play Playlist - No Songs",
        &VR_VoiceBoxAgentAudio_AU::PlayPlaylistNoSongs },  // "Play Playlist - No Songs" Task Complete
        { "Play Playlists",
        &VR_VoiceBoxAgentAudio_AU::PlayPlaylists },  // "Play Playlists" Follow-Up  16
        { "Play Song",
        &VR_VoiceBoxAgentAudio_AU::PlaySong },  // "Play Song" Task Complete
        { "Play Songs",
        &VR_VoiceBoxAgentAudio_AU::PlaySongs },  // "Play Songs" Follow-Up  20
        { "n-Best",
        &VR_VoiceBoxAgentAudio_AU::AudioNBest },
        { "Too Many Albums",
        &VR_VoiceBoxAgentAudio_AU::TooManyAlbums },   // "Too Many Albums" Follow-Up  3
        { "Too Many Artists",
        &VR_VoiceBoxAgentAudio_AU::TooManyArtists },   // "Too Many Artists" Follow-Up  3
        { "Too Many Playlists",
        &VR_VoiceBoxAgentAudio_AU::TooManyPlaylists },   // "Too Many Playlists" Follow-Up  3
        { "Too Many Songs",
        &VR_VoiceBoxAgentAudio_AU::TooManySongs },   // "Too Many Songs" Follow-Up  3
        // Info Driver
        { "Source Connected Request",
        &VR_VoiceBoxAgentAudio_AU::SourceConnectedRequest }, // "Get Current Album Request"

        { VBT_ACTION_TYPE_MUSIC_BACK, // "Back"
        &VR_VoiceBoxAgentAudio_AU::AudioBack },
        { VBT_ACTION_TYPE_MUSIC_CANCEL, // "Cancel"
        &VR_VoiceBoxAgentAudio_AU::AudioCancel },
        { VBT_ACTION_TYPE_MUSIC_HELP, // "Help"
        &VR_VoiceBoxAgentAudio_AU::AudioHelp },
        { VBT_ACTION_TYPE_MUSIC_START_OVER, // "Start Over"
        &VR_VoiceBoxAgentAudio_AU::AudioStartOver },
        { VBT_ACTION_TYPE_MUSIC_SYSTEM_PAUSE, // "Start Pause"
        &VR_VoiceBoxAgentAudio_AU::AudioStartPause },
        { VBT_ACTION_TYPE_MUSIC_SYSTEM_REPEAT, // "Start Repeat"
        &VR_VoiceBoxAgentAudio_AU::AudioStartRepeat },

        // Radio
        { "Preset Not Defined",
        &VR_VoiceBoxAgentAudio_AU::PresetNotDefined },  // "Preset Not Defined" Task Complete
        { "Presets Not Available",
        &VR_VoiceBoxAgentAudio_AU::PresetsNotAvailable },  // "Presets Not Available" Task Complete
        { "Tune AM Frequency",
        &VR_VoiceBoxAgentAudio_AU::TuneAMFrequency },  // "Tune AM Frequency" Task Complete
        { "Tune FM Frequency",
        &VR_VoiceBoxAgentAudio_AU::TuneFMFrequency },  // "Tune FM Frequency" Task Complete
        { "Tune Frequency Catch",
        &VR_VoiceBoxAgentAudio_AU::TuneFrequencyCatch },  // "Tune Frequency Catch" Task Complete (re-activate VR) \  Follow-Up
        { "Tune Preset AM",
        &VR_VoiceBoxAgentAudio_AU::TunePreset },  // "Tune Preset AM" Task Complete
        { "Tune Preset Catch",
        &VR_VoiceBoxAgentAudio_AU::TunePresetCatch },  // "Tune Preset Catch" Task Complete
        { "Tune Preset DAB",
        &VR_VoiceBoxAgentAudio_AU::TunePreset },  // "Tune Preset DAB" Task Complete
        { "Tune Preset FM",
        &VR_VoiceBoxAgentAudio_AU::TunePreset },  // "Tune Preset FM" Task Complete

        // Info Driver
        { "Preset Defined Request",
        &VR_VoiceBoxAgentAudio_AU::PresetDefinedRequest }, // "Preset Defined Request"
        { "Presets Available Request",
        &VR_VoiceBoxAgentAudio_AU::PresetsAvailableRequest }, // "Presets Available Request"

        { VBT_ACTION_TYPE_RADIO_BACK, // "Back"
        &VR_VoiceBoxAgentAudio_AU::AudioBack },
        { VBT_ACTION_TYPE_RADIO_CANCEL, // "Cancel"
        &VR_VoiceBoxAgentAudio_AU::AudioCancel },
        { VBT_ACTION_TYPE_RADIO_HELP, // "Help"l
        &VR_VoiceBoxAgentAudio_AU::AudioHelp },
        { VBT_ACTION_TYPE_RADIO_START_OVER, // "Start Over"
        &VR_VoiceBoxAgentAudio_AU::AudioStartOver },
        { VBT_ACTION_TYPE_RADIO_SYSTEM_PAUSE, // "Start Pause"
        &VR_VoiceBoxAgentAudio_AU::AudioStartPause },
        { VBT_ACTION_TYPE_RADIO_SYSTEM_REPEAT, // "Start Repeat"
        &VR_VoiceBoxAgentAudio_AU::AudioStartRepeat },
        { NULL, NULL }
    };

    int i = 0;
    while (true) {
        if (NULL == actionHandlerMap[i].szAction) {
            break;
        }

        m_audioMsgHandler.insert(std::make_pair(
            actionHandlerMap[i].szAction,
            actionHandlerMap[i].handler
            ));

        ++i;
    }

    m_audioMsgResult.insert(std::make_pair("queryPresetDefined", &VR_VoiceBoxAgentAudio_AU::PresetDefinedReply));
    m_audioMsgResult.insert(std::make_pair("queryPresetsAvailable", &VR_VoiceBoxAgentAudio_AU::PresetsAvailableReply));

    m_musicCommandtoContent.insert(std::make_pair("Play Album", "media_music_album_select"));
    m_musicCommandtoContent.insert(std::make_pair("Play Artist", "media_music_artist_select"));
    m_musicCommandtoContent.insert(std::make_pair("Play Playlist", "media_music_select_song_from_playlist"));
    m_musicCommandtoContent.insert(std::make_pair("Play Song", "media_music_song_select"));
    m_musicCommandtoContent.insert(std::make_pair("Play Select Album Play", "media_music_album_select"));
    m_musicCommandtoContent.insert(std::make_pair("Play Select Artist Play", "media_music_artist_select"));
    m_musicCommandtoContent.insert(std::make_pair("Play Select Playlist Play", "media_music_select_song_from_playlist"));
    m_musicCommandtoContent.insert(std::make_pair("Play Select Song Play", "media_music_song_select"));

    return true;
}

void
VR_VoiceBoxAgentAudio_AU::UnInitialize()
{
    m_audioMsgHandler.clear();
    m_audioMsgResult.clear();
    m_musicCommandtoContent.clear();
}

bool
VR_VoiceBoxAgentAudio_AU::ReplyQueryInfo(
    VR_VoiceBoxXmlParser& parser
)
{
    std::string strActionKey = parser.getXmlKey();

    VR_LOG("VR_VoiceBoxAgentAudio_AU ReplyQueryInfo strActionKey = %s \n", strActionKey.c_str());

    VoiceMap<std::string, AudioResultResult_AU>::const_iterator iterMap = m_audioMsgResult.find(strActionKey);
    if (m_audioMsgResult.cend() != iterMap) {
        if (NULL != iterMap->second) {
            (this->*(iterMap->second))(parser);
            return true;
        }
    }
    return true;
}

// QueryAgentSupport
HRESULT
VR_VoiceBoxAgentAudio_AU::QueryAgentSupport(
    VBT_CSTR szAgent,
    VBT_BOOL *pbSupported
)
{
    if (NULL == pbSupported) {
        VR_ERROR("QueryAgentSupport: NULL Parameter");
        return E_FAIL;
    }

    CVECICStr strAgent(&m_client, szAgent);
    if (strAgent.IsEqual(VBT_AGENT_MUSIC)
        || strAgent.IsEqual(VBT_AGENT_RADIO)) {
        VR_LOG("QueryAgentSupport: %s", szAgent);
        *pbSupported = VBT_TRUE;
    }
    else {
        *pbSupported = VBT_FALSE;
    }

    return S_OK;
}

// OnAgentResultMessage
HRESULT
VR_VoiceBoxAgentAudio_AU::OnAgentResultMessage(
    IVECITransaction* pTrans,
    IVECIParsedMessage* pResultMsg
)
{
    if ((NULL == pTrans) || (NULL == pResultMsg)) {
        VR_ERROR("OnAgentResultMessage: NULL Parameter");
        return E_FAIL;
    }

    if (!ProcessAgentMessage(pResultMsg)) {
        return E_FAIL;
    }

    return S_OK;
}

// OnAgentDriverMessage
HRESULT
VR_VoiceBoxAgentAudio_AU::OnAgentDriverMessage(
    IVECIParsedMessage* pDriverMsg
)
{
    if (NULL == pDriverMsg) {
        return E_FAIL;
    }

    if (!ProcessAgentMessage(pDriverMsg)) {
        return E_FAIL;
    }

    return S_OK;
}

bool
VR_VoiceBoxAgentAudio_AU::ProcessAgentMessage(IVECIParsedMessage *pcMsg)
{
    VR_LOGP("DE: receive VBT processing data, will run operation about audio...case 212-122 212-128");

    if (NULL == pcMsg) {
        return false;
    }

    if (!SendActionMessage(pcMsg)) {
        return false;
    }

    std::string strAgentResult;
    do {
        CVECIOutStr strActionType;
        (void)pcMsg->GetActionType(&strActionType);
        if (NULL != strActionType.Get()) {
            strAgentResult = strActionType.Get();
            break;
        }

        CVECIOutStr strListType;
        (void)pcMsg->GetListType(&strListType);
        if (NULL != strListType.Get()) {
            strAgentResult = strListType.Get();
            break;
        }

        return false;

    } while (false);

    VR_LOG("ProcessAgentMessage: %s", strAgentResult.c_str());

    // Dispatch the messages
    VoiceMap<std::string, AudioMsgHandler_AU>::const_iterator iterMap = m_audioMsgHandler.find(strAgentResult);
    if (m_audioMsgHandler.cend() != iterMap) {
        if (NULL != iterMap->second) {
            (this->*(iterMap->second))(pcMsg);
            return true;
        }
    }

    return true;
}

void
VR_VoiceBoxAgentAudio_AU::SendMusicPlay(const std::string& strSourceID,
    const std::string& strPlayType,
    const std::string& strID,
    const std::string& strShuffle,
    const std::string& strSubId)
{
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("media", "play");
    if (!strSourceID.empty()) {
        (void)xmlBulder.buildGivenElement(node, "sourceid", strSourceID, "", "");
    }
    if (!strPlayType.empty()) {
        (void)xmlBulder.buildGivenElement(node, "playType", strPlayType, "", "");
    }
    if (!strID.empty()) {
        (void)xmlBulder.buildGivenElement(node, "id", strID, "", "");
    }
    if (!strShuffle.empty()) {
        (void)xmlBulder.buildGivenElement(node, "shuffle", strShuffle, "", "");
    }
    if (!strSubId.empty()) {
        (void)xmlBulder.buildGivenElement(node, "subid", strSubId, "", "");
    }

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return;
}

// Play Album
bool
VR_VoiceBoxAgentAudio_AU::PlayAlbum(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    std::string strAlbumID = GetActionParaByName(pcMsg, "Album ID");

    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    SendMusicPlay(strSourceID, "PLAY_ALBUM", strAlbumID, "false", "");

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::PlayArtist(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    std::string strArtistID = GetActionParaByName(pcMsg, "Artist ID");

    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    SendMusicPlay(strSourceID, "PLAY_ARTIST", strArtistID, "false", "");

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::PlayPlaylist(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    std::string strPlaylistID = GetActionParaByName(pcMsg, "Playlist ID");

    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    SendMusicPlay(strSourceID, "PLAY_PLAYLIST", strPlaylistID, "false", "");

    return true;
}


bool
VR_VoiceBoxAgentAudio_AU::PlaySong(IVECIParsedMessage *pcMsg)
{
    VR_LOGP("DE: receive VBT processing data, send action playsong to DM...case 212-122-99 212-128-99");


    if (NULL == pcMsg) {
        return false;
    }

    std::string strSongID = GetActionParaByName(pcMsg, "Song ID");

    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    SendMusicPlay(strSourceID, "PLAY_SONG", strSongID, "false", "");

    return true;
}

pugi::xml_node
VR_VoiceBoxAgentAudio_AU::BuildScreenDisplayFront(VR_VoiceBoxXmlBuilder& xmlBulder,
    const std::string& strContent, const std::string& strPromtValue)
{
    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");

    (void)xmlBulder.buildGivenElement(xmlNode, "agent", "media", "", "");

    (void)xmlBulder.buildGivenElement(xmlNode, "content", strContent, "", "");

    if (!strPromtValue.empty()) {
        pugi::xml_node nodeDetail = xmlBulder.buildGivenElement(xmlNode, "detail", "", "", "");
        (void)xmlBulder.buildGivenElement(nodeDetail, "promptvalue", strPromtValue, "", "");
    }

    return xmlNode;
}

void
VR_VoiceBoxAgentAudio_AU::BuildScreenDisplayAll(
    const std::string& strContent,
    const std::string& strScreenId)
{
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");

    (void)xmlBulder.buildGivenElement(xmlNode, "agent", "media", "", "");

    (void)xmlBulder.buildGivenElement(xmlNode, "content", strContent, "", "");

    m_engineCallback.GetPromptByScreenId(strScreenId, m_strPrompt);

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return;
}

void
VR_VoiceBoxAgentAudio_AU::BuildListHeader(IVECIParsedMessage *pcMsg,
    VR_VoiceBoxXmlBuilder& xmlBulder, pugi::xml_node& nodeList)
{
    if (NULL == pcMsg) {
        return;
    }

    std::string strStartIndex = GetStartIndex(pcMsg);
    std::string strListCount = GetActionParaByName(pcMsg, "List Count");

    pugi::xml_node nodeHeader = xmlBulder.buildGivenElement(nodeList, "header", "", "", "");

    (void)xmlBulder.buildGivenElement(nodeHeader, "pageSize", "5", "", "");
    (void)xmlBulder.buildGivenElement(nodeHeader, "startIndex", strStartIndex, "", "");
    (void)xmlBulder.buildGivenElement(nodeHeader, "count", strListCount, "", "");

    return;
}

void
VR_VoiceBoxAgentAudio_AU::BuildMusicInfo(VR_VoiceBoxXmlBuilder& xmlBulder, pugi::xml_node& xmlNode,
    const std::string& strName, const std::string& strSourceId, const std::string& strId)
{
    VoiceVector<StructNode>::type attributeVector;
    StructNode node;

    if (!strName.empty()) {
        node.Name = "name";
        node.Value = strName;
        attributeVector.push_back(node);
    }

    if (!strSourceId.empty()) {
        node.Name = "sourceId";
        node.Value = strSourceId;
        attributeVector.push_back(node);
    }

    if (!strId.empty()) {
        node.Name = "id";
        node.Value = strId;
        attributeVector.push_back(node);
    }

    xmlBulder.buildListItemChildElement(xmlNode, "musicInfo", musicInfo, attributeVector);
}

void
VR_VoiceBoxAgentAudio_AU::BuildListInfo(IVECIParsedMessage *pcMsg,
    VR_VoiceBoxXmlBuilder& xmlBulder,
    pugi::xml_node nodeSelectList,
    const std::string& strNamePara,
    const std::string& strIDPara,
    const std::string& strSourceID)
{
    pugi::xml_node nodeList = xmlBulder.buildGivenElement(nodeSelectList, "list", "", "", "");

    BuildListHeader(pcMsg, xmlBulder, nodeList);

    pugi::xml_node nodeItems = xmlBulder.buildGivenElement(nodeList, "items", "", "", "");

    VBT_ULONG uSize = 0;
    (void)pcMsg->GetListSize(&uSize);
    for (VBT_ULONG i = 0; i < uSize; ++i) {
        std::string strName = GetListParaByName(pcMsg, i, strNamePara);
        std::string strID = GetListParaByName(pcMsg, i, strIDPara);

        BuildMusicInfo(xmlBulder, nodeItems, strName, strSourceID, strID);
    }

}

bool
VR_VoiceBoxAgentAudio_AU::PlayAlbums(IVECIParsedMessage *pcMsg)  // 5
{
    if (NULL == pcMsg) {
        return false;
    }

    BuildScreenDisplayAll("media_music_speak_album_name", "VR-MUS-05");

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::PlayDisambiguateAlbumArtist(IVECIParsedMessage *pcMsg) // 1
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("PlayDisambiguateAlbumArtist: %s\n", xmlMsg.Get());

    std::string strAlbumName = GetActionParaByName(pcMsg, "Album");
    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode;

    xmlNode = BuildScreenDisplayFront(xmlBulder, "media_music_artist_select", strAlbumName);

    pugi::xml_node nodeSelectList = xmlBulder.buildGivenElement(xmlNode, "selectList", "", "", "");

    CVECIOutStr strListType;
    (void)pcMsg->GetListType(&strListType);

   if (NULL != strListType.Get()) {
        BuildListInfo(pcMsg, xmlBulder, nodeSelectList, "Artist", "Artist ID", strSourceID);
    }

    m_engineCallback.GetPromptByScreenId("VR-SYS-01", m_strPrompt);
    m_engineCallback.SetNBestScreenFlg(false);

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::PlayDisambiguateSongAlbum(IVECIParsedMessage *pcMsg) // 1
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("PlayDisambiguateSongAlbum: %s\n", xmlMsg.Get());

    std::string strSongName = GetActionParaByName(pcMsg, "Song");
    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode;

    xmlNode = BuildScreenDisplayFront(xmlBulder, "media_music_album_select", strSongName);

    pugi::xml_node nodeSelectList = xmlBulder.buildGivenElement(xmlNode, "selectList", "", "", "");

    CVECIOutStr strListType;
    (void)pcMsg->GetListType(&strListType);

    if (NULL != strListType.Get()) {
        BuildListInfo(pcMsg, xmlBulder, nodeSelectList, "Album", "Album ID", strSourceID);
    }

    m_engineCallback.GetPromptByScreenId("VR-SYS-01", m_strPrompt);
    m_engineCallback.SetNBestScreenFlg(false);

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::PlayDisambiguateSongArtist(IVECIParsedMessage *pcMsg) // 1
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("PlayDisambiguateSongArtist: %s", xmlMsg.Get());

    std::string strSongName = GetActionParaByName(pcMsg, "Song");
    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode;

    xmlNode = BuildScreenDisplayFront(xmlBulder, "media_music_artist_select", strSongName);

    pugi::xml_node nodeSelectList = xmlBulder.buildGivenElement(xmlNode, "selectList", "", "", "");

    CVECIOutStr strListType;
    (void)pcMsg->GetListType(&strListType);

    if (NULL != strListType.Get()) {
        BuildListInfo(pcMsg, xmlBulder, nodeSelectList, "Artist", "Artist ID", strSourceID);
    }

    m_engineCallback.GetPromptByScreenId("VR-SYS-01", m_strPrompt);
    m_engineCallback.SetNBestScreenFlg(false);

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::PlayPlaylists(IVECIParsedMessage *pcMsg) // 16
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("OnPlaylists: %s", xmlMsg.Get());

    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode = BuildScreenDisplayFront(xmlBulder, "media_music_play_list_select", "");

    pugi::xml_node nodeSelectList = xmlBulder.buildGivenElement(xmlNode, "selectList", "", "", "");

    CVECIOutStr strListType;
    (void)pcMsg->GetListType(&strListType);

    if (NULL != strListType.Get()) {
        BuildListInfo(pcMsg, xmlBulder, nodeSelectList, "Playlist", "Playlist ID", strSourceID);
    }

    m_engineCallback.GetPromptByScreenId("VR-MUS-16", m_strPrompt);

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::PlayArtists(IVECIParsedMessage *pcMsg) // 7
{
    if (NULL == pcMsg) {
        return false;
    }

    BuildScreenDisplayAll("media_music_speak_artist_name", "VR-MUS-07");

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::PlaySongs(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    BuildScreenDisplayAll("media_music_speak_song_name", "VR-MUS-20");


    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::AudioHome(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }
    VR_LOGP("DE: start go to audio command...case : 215-7-00");

    m_engineCallback.GetPromptByScreenId("VR-HNT-01", m_strPrompt);

    m_engineCallback.OnRequest("<event name=\"startNextAgent\"><agent>music</agent></event>");

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::AudioMenu(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.OnRequest("<event name=\"showHintScreen\"><agent>music</agent></event>");

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::BusyUpdatingGrammar(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    TaskComplete(pcMsg);
    ShowPopupMsg(cstVO31002);

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::ChangeSource(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    std::string strSourceName = GetActionParaByName(pcMsg, "Source");
    if ("USB" == strSourceName || "iPod" == strSourceName) {
        m_engineCallback.ChangeAudioSourceName(strSourceName);
    }

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("media", "changeSourceByName");
    (void)xmlBulder.buildGivenElement(node, "sourceName", strSourceName, "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::ChangeSourceNotConnected(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    TaskComplete(pcMsg);
    std::string strSourceName = GetActionParaByName(pcMsg, "Source");

    VR_LOG("source %s", strSourceName.c_str());
    std::string strPrompt;
    if ("USB" == strSourceName || "USB2" == strSourceName) {
        strPrompt = cstVO31031;
    }
    else if ("iPod" == strSourceName || "iPod2" == strSourceName) {
        strPrompt = cstVO31032;
    }
    else if ("Bluetooth Audio" == strSourceName) {
        strPrompt = cstVO31033;
    }
    else {
        VR_LOG("the source is out of my choose");
        return false;
    }
    ShowPopupMsg(strPrompt);

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::NoAlbums(
    IVECIParsedMessage *pcMsg
)
{
    TaskComplete(pcMsg);
    ShowPopupMsg(cstVO31008);

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::NoArtists(
    IVECIParsedMessage *pcMsg
)
{
    TaskComplete(pcMsg);
    ShowPopupMsg(cstVO31003);

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::NoAudioSourceConnected(
    IVECIParsedMessage *pcMsg
)
{
    TaskComplete(pcMsg);
    ShowPopupMsg(cstVO31001);
    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::NoPlaylists(
    IVECIParsedMessage *pcMsg
)
{
    TaskComplete(pcMsg);
    ShowPopupMsg(cstVO31013);
    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::NoSongs(
    IVECIParsedMessage *pcMsg
)
{
    TaskComplete(pcMsg);
    ShowPopupMsg(cstVO31018);
    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::PlayPlaylistNoSongs(
    IVECIParsedMessage *pcMsg
)
{
    TaskComplete(pcMsg);
    ShowPopupMsg(cstVO31027);
    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::TooManyAlbums(
    IVECIParsedMessage *pcMsg
)
{
    TaskComplete(pcMsg);
    ShowPopupMsg(cstVO31009);
    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::TooManyArtists(
    IVECIParsedMessage *pcMsg
)
{
    TaskComplete(pcMsg);
    ShowPopupMsg(cstVO31004);
    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::TooManyPlaylists(
    IVECIParsedMessage *pcMsg
)
{
    TaskComplete(pcMsg);
    ShowPopupMsg(cstVO31014);
    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::TooManySongs(
    IVECIParsedMessage *pcMsg
)
{
    TaskComplete(pcMsg);
    ShowPopupMsg(cstVO31019);
    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::SourceConnectedRequest(
    IVECIParsedMessage *pcMsg
)
{
    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    if (NULL == spParams.ptr()) {
        return false;
    }

    std::string strAudioConnected;

    std::string strSourceName = GetActionParaByName(pcMsg, "Source");
    VR_LOG("strSourceName = %s", strSourceName.c_str());
    if ("AM Radio" == strSourceName || "FM Radio" == strSourceName
        || "DAB" == strSourceName || "Disc" == strSourceName
        || "Auxiliary" == strSourceName) {
        strAudioConnected = "True";
    }
    else {
        strAudioConnected = m_engineCallback.GetAudioConnected(strSourceName);
    }

    (void)spParams->AddParameter("Connected", strAudioConnected.c_str(), "", "");

    HRESULT retCode = m_engineCommand.DriverActionReply("Music", "Source Connected Reply", spParams, NULL, NULL);
    if (S_OK != retCode) {
        return false;
    }

    return true;
}

void
VR_VoiceBoxAgentAudio_AU::OnInfoRequest(const std::string& strOp)
{
    VR_VoiceBoxXmlBuilder xmlBulder;

    pugi::xml_node node = xmlBulder.buildStartActionElement("media", strOp);
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnInfoQueryMessage(strReply);
}

int
VR_VoiceBoxAgentAudio_AU::GetNameByID(void* strName, int nCount, char** pValue, char** pName)
{
    std::string * strTmp = (std::string *)strName;
    *strTmp = pValue[0];

    return 0;
}

bool
VR_VoiceBoxAgentAudio_AU::AudioNBest(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("MusicNBest: %s", xmlMessage.Get());

    VR_VoiceBoxXmlBuilder xmlBulder;

    CVECIOutStr strCommand;
    (void)pcMsg->GetValueByKey("Message.List.Execution.Parameter.Command.value", &strCommand);
    std::string strContent;
    if (NULL != strCommand.Get()) {
        VoiceMap<std::string, std::string>::iterator iter = m_musicCommandtoContent.find(strCommand.Get());
        if (m_musicCommandtoContent.end() != iter) {
            strContent = iter->second;
        }
    }

    pugi::xml_node xmlNode = BuildScreenDisplayFront(xmlBulder, strContent.c_str(), "");
    VBT_ULONG uSize = 0;
    (void)pcMsg->GetListSize(&uSize);

    pugi::xml_node nodeSelectList = xmlBulder.buildGivenElement(xmlNode, "selectList", "", "", "");

    pugi::xml_node nodeList = xmlBulder.buildGivenElement(nodeSelectList, "list", "", "", "");

    pugi::xml_node nodeHeader = xmlBulder.buildGivenElement(nodeList, "header", "", "", "");

    uSize = (uSize > 5) ? 5 : uSize;
    std::ostringstream oss;
    oss<<uSize;
    std::string strCount = oss.str();

    (void)xmlBulder.buildGivenElement(nodeHeader, "pageSize", "5", "", "");
    (void)xmlBulder.buildGivenElement(nodeHeader, "startIndex", "0", "", "");
    (void)xmlBulder.buildGivenElement(nodeHeader, "count", strCount, "", "");

    pugi::xml_node nodeItems = xmlBulder.buildGivenElement(nodeList, "items", "", "", "");

    for (VBT_ULONG i = 0; i < uSize; ++i) {
        std::string strName = GetListParaByName(pcMsg, i, "Name");
        std::string strId = GetListParaByName(pcMsg, i, "ID");

        BuildMusicInfo(xmlBulder, nodeItems, strName, "", strId);
    }

    m_engineCallback.GetPromptByScreenId("VR-SYS-01", m_strPrompt);

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

// ------------------------------------------------------------Radio
bool
VR_VoiceBoxAgentAudio_AU::PresetDefinedRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    std::string strPreset = GetActionParaByName(pcMsg, "Preset");
    std::string strBand = GetActionParaByName(pcMsg, "Band");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("media", "queryPresetDefined");
    (void)xmlBulder.buildGivenElement(node, "band", strBand, "", "");
    (void)xmlBulder.buildGivenElement(node, "number", strPreset, "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnInfoQueryMessage(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::PresetDefinedReply(
    VR_VoiceBoxXmlParser& parser
)
{
    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    if (NULL == spParams.ptr()) {
        return false;
    }

    std::string strDefined = parser.getValueByKey("defined");

    (void)spParams->AddParameter("Defined", strDefined.c_str(), "", "");

    HRESULT retCode = m_engineCommand.DriverActionReply("Radio", "Preset Defined Reply", spParams, NULL, NULL);
    if (S_OK != retCode) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::PresetsAvailableRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    // OnInfoRequest("queryPresetsAvailable");

    // For AU model: Presets always available
    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    if (NULL == spParams.ptr()) {
        return false;
    }

    (void)spParams->AddParameter("Available", "True", "", "");

    HRESULT retCode = m_engineCommand.DriverActionReply("Radio", "Presets Available Reply", spParams, NULL, NULL);
    if (S_OK != retCode) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::PresetsAvailableReply(
    VR_VoiceBoxXmlParser& parser
)
{
    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    if (NULL == spParams.ptr()) {
        return false;
    }

    std::string strAvailable = parser.getValueByKey("available");

    (void)spParams->AddParameter("Available", strAvailable.c_str(), "", "");

    HRESULT retCode = m_engineCommand.DriverActionReply("Radio", "Presets Available Reply", spParams, NULL, NULL);
    if (S_OK != retCode) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::TunePresetCatch(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    BuildScreenDisplayAll("media_radio_speak_preset_number", "VR-RAD-04");

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::TuneFrequencyCatch(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    BuildScreenDisplayAll("media_radio_speak_radio_frequency", "VR-RAD-03");

    return true;
}

void
VR_VoiceBoxAgentAudio_AU::TuneFrequency(
    const std::string& strBand,
    const std::string& strFrequency
)
{
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("media", "tuneFrequency");

    if (!strBand.empty()) {
        (void)xmlBulder.buildGivenElement(node, "band", strBand, "", "");
    }

    if (!strFrequency.empty()) {
        (void)xmlBulder.buildGivenElement(node, "frequency", strFrequency, "", "");
    }

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return;
}

bool
VR_VoiceBoxAgentAudio_AU::TuneAMFrequency(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("TuneAMFrequency: %s", xmlMsg.Get());

    std::string strStationFrequency = GetActionParaByName(pcMsg, "Station Frequency");

    TuneFrequency("AM", strStationFrequency);

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::TuneFMFrequency(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("TuneFMFrequency: %s", xmlMsg.Get());

    std::string strStationFrequency = GetActionParaByName(pcMsg, "Station Frequency");

    TuneFrequency("FM", strStationFrequency);

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::TunePreset(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("TunePreset: %s", xmlMsg.Get());

    std::string strPreset = GetActionParaByName(pcMsg, "Preset");
    std::string strFrequencyBand = GetActionParaByName(pcMsg, "Frequency Band");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("media", "playByPreset");
    (void)xmlBulder.buildGivenElement(node, "band", strFrequencyBand, "", "");
    (void)xmlBulder.buildGivenElement(node, "number", strPreset, "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::TaskComplete(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr strActionType;

    (void)pcMsg->GetActionType(&strActionType);
    if (NULL != strActionType.Get()) {
        VR_LOG("ActionType = %s", strActionType.Get());
    }

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::AudioBack(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    m_engineCallback.OnRequest("<send event=\"back-internal\" target=\"de\"/>");
    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::AudioCancel(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.OnRequest("<send event=\"cancel\" target=\"dm\"/>");

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::AudioHelp(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::AudioStartOver(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.OnRequest("<send event=\"startover-internal\" target=\"de\"/>");

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::AudioStartPause(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.OnRequest("<send event=\"pause-internal\" target=\"de\"/>");

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::AudioStartRepeat(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.OnRequest("<send event=\"repeat-internal\" target=\"de\"/>");

    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::PresetNotDefined(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    TaskComplete(pcMsg);
    std::string strPrompt = cstVO31030;
    std::string strPresetNumber = GetActionParaByName(pcMsg, "Preset");

    const int nUninstallAllFormatLen = 256;
    char result[nUninstallAllFormatLen] = { 0 };
    snprintf(result, sizeof(result), strPrompt.c_str(), strPresetNumber.c_str());

    ShowPopupMsg(result);
    return true;
}

bool
VR_VoiceBoxAgentAudio_AU::PresetsNotAvailable(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    TaskComplete(pcMsg);

    return true;
}
/* EOF */
