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
#include <sqlite3.h>
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
#ifndef VR_VOICEBOXAGENTAUDIO_H
#    include "VR_VoiceBoxAgentAudio.h"
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
struct AudioActionHandler
{
    char*             szAction; // Action type
    AudioMsgHandler  handler;  // Handler pointer
};

// Constructor
VR_VoiceBoxAgentAudio::VR_VoiceBoxAgentAudio(
    IVECIEngineClient& client,
    IVECIEngineCommand& engineCommand,
    VR_VoiceBoxEngineCallback& engineCallback
)
: VR_VoiceBoxEventSink(client, engineCommand, engineCallback)
{
}

// Destructor
VR_VoiceBoxAgentAudio::~VR_VoiceBoxAgentAudio()
{
}

bool
VR_VoiceBoxAgentAudio::Initialize()
{
    const AudioActionHandler actionHandlerMap[] = {
        { VBT_ACTION_TYPE_MUSIC_BROWSE_PODCAST_NO_EPISODES,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "Browse Podcast - No Episodes" Task Complete
        { VBT_ACTION_TYPE_MUSIC_BROWSE_CURRENT_ALBUM_SID_ERROR,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "Browse Current Album - SID Error" Task Complete
        { VBT_ACTION_TYPE_MUSIC_BROWSE_CURRENT_ALBUM_INFORMATION_NOT_AVAILABLE,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "Browse Current Album - Information Not Available" Task Complete
        { VBT_ACTION_TYPE_MUSIC_BROWSE_CURRENT_ALBUM_NO_SONG_PLAYING,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "Browse Current Album - No Song Playing" Task Complete
        { VBT_ACTION_TYPE_MUSIC_BROWSE_CURRENT_ARTIST_INFORMATION_NOT_AVAILABLE,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "Browse Current Artist - Information Not Available" Task Complete
        { VBT_ACTION_TYPE_MUSIC_BROWSE_CURRENT_ARTIST_NO_SONG_PLAYING,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "Browse Current Artist - No Song Playing" Task Complete
        { VBT_ACTION_TYPE_MUSIC_BROWSE_CURRENT_ARTIST_SID_ERROR,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "Browse Current Artist - SID Error" Task Complete
        { VBT_ACTION_TYPE_MUSIC_BROWSE_CURRENT_COMPOSER_INFORMATION_NOT_AVAILABLE,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "Browse Current Composer - Information Not Available" Task Complete
        { VBT_ACTION_TYPE_MUSIC_BROWSE_CURRENT_COMPOSER_NO_SONG_PLAYING,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "Browse Current Composer - No Song Playing" Task Complete
        { VBT_ACTION_TYPE_MUSIC_BROWSE_CURRENT_COMPOSER_SID_ERROR,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "Browse Current Composer - SID Error" Task Complete
        { VBT_ACTION_TYPE_MUSIC_BROWSE_CURRENT_GENRE_INFORMATION_NOT_AVAILABLE,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "Browse Current Genre - Information Not Available" Task Complete
        { VBT_ACTION_TYPE_MUSIC_BROWSE_CURRENT_GENRE_NO_SONG_PLAYING,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "Browse Current Genre - No Song Playing" Task Complete
        { VBT_ACTION_TYPE_MUSIC_BROWSE_CURRENT_GENRE_SID_ERROR,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "Browse Current Genre - SID Error" Task Complete
        { VBT_ACTION_TYPE_MUSIC_NO_ALBUMS,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "No Albums" Task Complete
        { VBT_ACTION_TYPE_MUSIC_NO_ARTISTS,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "No Artists" Task Complete
        { VBT_ACTION_TYPE_MUSIC_NO_AUDIO_SOURCE_CONNECTED,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "No Audio Source Connected" Task Complete
        { VBT_ACTION_TYPE_MUSIC_NO_AUDIOBOOKS,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "No Audiobooks" Task Complete
        { VBT_ACTION_TYPE_MUSIC_NO_COMPOSERS,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "No Composers" Task Complete
        { VBT_ACTION_TYPE_MUSIC_NO_GENRES,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "No Genres" Task Complete
        { VBT_ACTION_TYPE_MUSIC_NO_PLAYLISTS,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "No Playlists" Task Complete
        { VBT_ACTION_TYPE_MUSIC_NO_PODCASTS,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "No Podcasts" Task Complete
        { VBT_ACTION_TYPE_MUSIC_NO_SOURCES_AVAILABLE,
        &VR_VoiceBoxAgentAudio::TaskComplete },   // "No Sources Available" Task Complete
        { VBT_ACTION_TYPE_MUSIC_SELECT_SOURCE_NONE,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "Select Source None" Task Complete
        { VBT_ACTION_TYPE_MUSIC_NO_SONGS,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "No Songs" Task Complete
        { VBT_ACTION_TYPE_MUSIC_BUSY_UPDATING_GRAMMAR,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "Busy Updating Grammar" Task Complete

        { VBT_ACTION_TYPE_MUSIC_AUDIO_OFF,
        &VR_VoiceBoxAgentAudio::AudioOff },  //  "Audio Off" Task Complete
        { VBT_ACTION_TYPE_MUSIC_AUDIO_ON,
        &VR_VoiceBoxAgentAudio::AudioOn },   // "Audio On" Task Complete
        { VBT_ACTION_TYPE_MUSIC_PLAY_ALBUM,
        &VR_VoiceBoxAgentAudio::PlayAlbum },  // "Play Album" Task Complete
        { VBT_ACTION_TYPE_MUSIC_PLAY_ALL,
        &VR_VoiceBoxAgentAudio::PlayAll },  // "Play All" Task Complete
        { VBT_ACTION_TYPE_MUSIC_PLAY_ARTIST,
        &VR_VoiceBoxAgentAudio::PlayArtist },  // "Play Artist" Task Complete
        { VBT_ACTION_TYPE_MUSIC_PLAY_AUDIOBOOK,
        &VR_VoiceBoxAgentAudio::PlayAudioBook },  // "Play Audiobook" Task Complete
        { VBT_ACTION_TYPE_MUSIC_PLAY_COMPOSER,
        &VR_VoiceBoxAgentAudio::PlayComposer },  // "Play Composer" Task Complete
        { VBT_ACTION_TYPE_MUSIC_PLAY_GENRE,
        &VR_VoiceBoxAgentAudio::PlayGenre },  // "Play Genre" Task Complete
        { VBT_ACTION_TYPE_MUSIC_PLAY_PLAYLIST,
        &VR_VoiceBoxAgentAudio::PlayPlaylist },  // "Play Playlist" Task Complete
        { VBT_ACTION_TYPE_MUSIC_PLAY_PODCAST,
        &VR_VoiceBoxAgentAudio::PlayPodcast },  // "Play Podcast" Task Complete
        { VBT_ACTION_TYPE_MUSIC_PLAY_PODCAST_ITEM,
        &VR_VoiceBoxAgentAudio::PlayPodcastItem },  // "Play Podcast Item" Task Complete
        { VBT_ACTION_TYPE_MUSIC_PLAY_SONG,
        &VR_VoiceBoxAgentAudio::PlaySong },  // "Play Song" Task Complete
        { VBT_ACTION_TYPE_MUSIC_SHUFFLE_PLAY_ALBUM,
        &VR_VoiceBoxAgentAudio::ShufflePlayAlbum },  // "Shuffle Play Album" Task Complete
        { VBT_ACTION_TYPE_MUSIC_SHUFFLE_PLAY_ARTIST,
        &VR_VoiceBoxAgentAudio::ShufflePlayArtist },  // "Shuffle Play Artist" Task Complete
        { VBT_ACTION_TYPE_MUSIC_SHUFFLE_PLAY_COMPOSER,
        &VR_VoiceBoxAgentAudio::ShufflePlayComposer },  // "Shuffle Play Composer" Task Complete
        { VBT_ACTION_TYPE_MUSIC_SHUFFLE_PLAY_PLAYLIST,
        &VR_VoiceBoxAgentAudio::ShufflePlayPlaylist },  // "Shuffle Play Playlist" Task Complete

        { VBT_ACTION_TYPE_MUSIC_BROWSE_DISAMBIGUATE_ALBUM_ARTIST,
        &VR_VoiceBoxAgentAudio::OnDisambiguateAlbumArtist },  // "Browse Disambiguate Album Artist" Follow-Up  1
        { VBT_ACTION_TYPE_MUSIC_PLAY_DISAMBIGUATE_ALBUM_ARTIST,
        &VR_VoiceBoxAgentAudio::OnDisambiguateAlbumArtist },  // "Play Disambiguate Album Artist" Follow-Up  1
        { VBT_ACTION_TYPE_MUSIC_PLAY_DISAMBIGUATE_SONG_ARTIST,
        &VR_VoiceBoxAgentAudio::PlayDisambiguateSongArtist },  // "Play Disambiguate Song Artist" Follow-Up 2
        { VBT_ACTION_TYPE_MUSIC_BROWSE_CATCH,
        &VR_VoiceBoxAgentAudio::OnCatch },   // "Browse Catch" Follow-Up  3
        { VBT_ACTION_TYPE_MUSIC_PLAY_CATCH,
        &VR_VoiceBoxAgentAudio::OnCatch },   // "Play Catch" Follow-Up  3
        { VBT_ACTION_TYPE_MUSIC_PLAY_DISAMBIGUATE_SONG_ALBUM,
        &VR_VoiceBoxAgentAudio::PlayDisambiguateSongAlbum },  // "Play Disambiguate Song Album" Follow-Up 4
        { VBT_ACTION_TYPE_MUSIC_BROWSE_ALBUMS,
        &VR_VoiceBoxAgentAudio::OnAlbums },  // "Browse Albums" Follow-Up  5
        { VBT_ACTION_TYPE_MUSIC_PLAY_ALBUMS,
        &VR_VoiceBoxAgentAudio::OnAlbums },  // "Play Albums" Follow-Up  5
        { VBT_ACTION_TYPE_MUSIC_BROWSE_ALBUM,
        &VR_VoiceBoxAgentAudio::BrowseAlbum },  // "Browse Album" Follow-Up  6
        { VBT_ACTION_TYPE_MUSIC_BROWSE_ARTISTS,
        &VR_VoiceBoxAgentAudio::OnArtists },  // "Browse Artists" Follow-Up  7
        { VBT_ACTION_TYPE_MUSIC_PLAY_ARTISTS,
        &VR_VoiceBoxAgentAudio::OnArtists },  // "Play Artists" Follow-Up  7
        { VBT_ACTION_TYPE_MUSIC_BROWSE_ARTIST,
        &VR_VoiceBoxAgentAudio::BrowseArtist },  // "Browse Artist" Follow-Up  8
        { VBT_ACTION_TYPE_MUSIC_BROWSE_ARTIST_SONGS,
        &VR_VoiceBoxAgentAudio::BrowseArtistSongs },  // "Browse Artist Songs" Follow-Up  9
        { VBT_ACTION_TYPE_MUSIC_BROWSE_AUDIOBOOKS,
        &VR_VoiceBoxAgentAudio::OnAudiobooks },  // "Browse Audiobooks" Follow-Up  10
        { VBT_ACTION_TYPE_MUSIC_PLAY_AUDIOBOOKS,
        &VR_VoiceBoxAgentAudio::OnAudiobooks },  // "Play Audiobooks" Follow-Up  10
        { VBT_ACTION_TYPE_MUSIC_BROWSE_COMPOSERS,
        &VR_VoiceBoxAgentAudio::OnComposers },  // "Browse Composers" Follow-Up  11
        { VBT_ACTION_TYPE_MUSIC_PLAY_COMPOSERS,
        &VR_VoiceBoxAgentAudio::OnComposers },  // "Play Composers" Follow-Up  11
        { VBT_ACTION_TYPE_MUSIC_BROWSE_COMPOSER,
        &VR_VoiceBoxAgentAudio::BrowseComposer },  // "Browse Composer" Follow-Up 12
        { VBT_ACTION_TYPE_MUSIC_BROWSE_GENRES,
        &VR_VoiceBoxAgentAudio::OnGenres },  // "Browse Genres" Follow-Up  13
        { VBT_ACTION_TYPE_MUSIC_PLAY_GENRES,
        &VR_VoiceBoxAgentAudio::OnGenres },  // "Play Genres" Follow-Up  13
        { VBT_ACTION_TYPE_MUSIC_BROWSE_GENRE,
        &VR_VoiceBoxAgentAudio::BrowseGenre },  // "Browse Genre" Follow-Up  14
        { VBT_ACTION_TYPE_MUSIC_BROWSE_PLAYLISTS,
        &VR_VoiceBoxAgentAudio::OnPlaylists },  // "Browse Playlists" Follow-Up  16
        { VBT_ACTION_TYPE_MUSIC_PLAY_PLAYLISTS,
        &VR_VoiceBoxAgentAudio::OnPlaylists },  // "Play Playlists" Follow-Up  16
        { VBT_ACTION_TYPE_MUSIC_BROWSE_PLAYLIST,
        &VR_VoiceBoxAgentAudio::BrowsePlaylist },  // "Browse Playlist" Follow-Up  17
        { VBT_ACTION_TYPE_MUSIC_BROWSE_PODCASTS,
        &VR_VoiceBoxAgentAudio::OnPodcasts },  // "Browse Podcasts" Follow-Up  18
        { VBT_ACTION_TYPE_MUSIC_PLAY_PODCASTS,
        &VR_VoiceBoxAgentAudio::OnPodcasts },  // "Play Podcasts" Follow-Up  18
        { VBT_ACTION_TYPE_MUSIC_BROWSE_SONGS,
        &VR_VoiceBoxAgentAudio::OnSongs },  // "Browse Songs" Follow-Up  20
        { VBT_ACTION_TYPE_MUSIC_PLAY_SONGS,
        &VR_VoiceBoxAgentAudio::OnSongs },  // "Play Songs" Follow-Up  20
        { VBT_ACTION_TYPE_MUSIC_CHANGE_SOURCE_CATCH,
        &VR_VoiceBoxAgentAudio::ChangeSourceCatch },  // "Change Source Catch" Follow-Up  21
        { VBT_ACTION_TYPE_MUSIC_BROWSE_PODCAST,
        &VR_VoiceBoxAgentAudio::BrowsePodcast },  // "Browse Podcast" Follow-Up  23

        { VBT_ACTION_TYPE_MUSIC_MUSIC_MENU,
        &VR_VoiceBoxAgentAudio::MusicMenu },  // "Music Menu" Task Complete (re-activate VR)
        { VBT_ACTION_TYPE_MUSIC_CHANGE_SOURCE,
        &VR_VoiceBoxAgentAudio::ChangeSource },  // "Change Source" Task Complete Go to top screen for selected source
        { VBT_ACTION_TYPE_MUSIC_MUSIC_KEYBOARD_DICTATION,
        &VR_VoiceBoxAgentAudio::MusicKeyboardDictation },  // "Music Keyboard Dictation" Task Complete
        { VBT_LIST_TYPE_MUSIC_NBEST,
        &VR_VoiceBoxAgentAudio::AudioNBest }, // Music nBest
        { VBT_ACTION_TYPE_MUSIC_BACK, // "Back"
        &VR_VoiceBoxAgentAudio::AudioBack },
        { VBT_ACTION_TYPE_MUSIC_CANCEL, // "Cancel"
        &VR_VoiceBoxAgentAudio::AudioCancel },
        { VBT_ACTION_TYPE_MUSIC_HELP, // "Help"
        &VR_VoiceBoxAgentAudio::AudioHelp },
        { VBT_ACTION_TYPE_MUSIC_START_OVER, // "Start Over"
        &VR_VoiceBoxAgentAudio::AudioStartOver },
        { VBT_ACTION_TYPE_MUSIC_SYSTEM_PAUSE, // "Start Pause"
        &VR_VoiceBoxAgentAudio::AudioStartPause },
        { VBT_ACTION_TYPE_MUSIC_SYSTEM_REPEAT, // "Start Repeat"
        &VR_VoiceBoxAgentAudio::AudioStartRepeat },
        { VBT_ACTION_TYPE_MUSIC_ESCALATING_ERROR, // "Escalating Error"
        &VR_VoiceBoxAgentAudio::EscalatingError },

        // Info Driver
        { VBT_DRIVER_REQUEST_ACTION_TYPE_MUSIC_GET_CURRENT_ALBUM_REQUEST,
        &VR_VoiceBoxAgentAudio::GetCurrentAlbumRequest }, // "Get Current Album Request"
        { VBT_DRIVER_REQUEST_ACTION_TYPE_MUSIC_GET_CURRENT_ARTIST_REQUEST,
        &VR_VoiceBoxAgentAudio::GetCurrentArtistRequest },  // "Get Current Artist Request"
        { VBT_DRIVER_REQUEST_ACTION_TYPE_MUSIC_GET_CURRENT_COMPOSER_REQUEST,
        &VR_VoiceBoxAgentAudio::GetCurrentComposerRequest },  // "Get Current Composer Request"
        { VBT_DRIVER_REQUEST_ACTION_TYPE_MUSIC_GET_CURRENT_GENRE_REQUEST,
        &VR_VoiceBoxAgentAudio::GetCurrentGenreRequest },  // "Get Current Genre Request"
        { VBT_DRIVER_REQUEST_ACTION_TYPE_MUSIC_GET_PODCAST_EPISODES_REQUEST,
        &VR_VoiceBoxAgentAudio::GetPodcastEpisodesRequest },  // "Get Podcast Episodes Request"
        // Radio
        { VBT_ACTION_TYPE_RADIO_ACTIVATE_RADIO,
        &VR_VoiceBoxAgentAudio::ActivateRadio },  // "Activate Radio" Task Complete
        { VBT_ACTION_TYPE_RADIO_CHANGE_HD_SUBCHANNEL,
        &VR_VoiceBoxAgentAudio::ChangeHDSubChannel },  // "Change HD Sub-channel" Task Complete
        { VBT_ACTION_TYPE_RADIO_HD_SUBCHANNEL_NOT_AVAILABLE,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "HD Sub-channel Not Available" Task Complete
        { VBT_ACTION_TYPE_RADIO_PRESET_NOT_DEFINED,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "Preset Not Defined" Task Complete
        { VBT_ACTION_TYPE_RADIO_PRESETS_NOT_AVAILABLE,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "Presets Not Available" Task Complete
        { VBT_ACTION_TYPE_RADIO_TUNE_AM_FREQUENCY,
        &VR_VoiceBoxAgentAudio::TuneAMFrequency },  // "Tune AM Frequency" Task Complete
        { VBT_ACTION_TYPE_RADIO_TUNE_FM_FREQUENCY,
        &VR_VoiceBoxAgentAudio::TuneFMFrequency },  // "Tune FM Frequency" Task Complete
        { VBT_ACTION_TYPE_RADIO_TUNE_FM_GENRE,
        &VR_VoiceBoxAgentAudio::TuneFMGenre },  // "Tune FM Genre Catch" Task Complete
        { VBT_ACTION_TYPE_RADIO_TUNE_FM_GENRE_STATIONS_NOT_AVAILABLE,
        &VR_VoiceBoxAgentAudio::TaskComplete },  // "Tune FM Genre - Stations Not Available" Task Complete
        { VBT_ACTION_TYPE_RADIO_TUNE_SATELLITE_CHANNEL_NAME,
        &VR_VoiceBoxAgentAudio::TuneSatelliteChannelName },  // "Tune Satellite Channel Name" Task Complete
        { VBT_ACTION_TYPE_RADIO_TUNE_SATELLITE_CHANNEL_NUMBER,
        &VR_VoiceBoxAgentAudio::TuneSatelliteChannelNumber },  // "Tune Satellite Channel Number" Task Complete
        { VBT_ACTION_TYPE_RADIO_TUNE_SATELLITE_GENRE,
        &VR_VoiceBoxAgentAudio::TuneSatelliteGenre },  // "Tune Satellite Genre" Task Complete
        { "VBT Send Command",
        &VR_VoiceBoxAgentAudio::VBTSendCommand },  // "VBT Send Command" Task Complete
        { VBT_ACTION_TYPE_RADIO_TUNE_FM_HD_FREQUENCY,
        &VR_VoiceBoxAgentAudio::TuneFMHDFrequency },  // "Tune FM HD Frequency" Task Complete
        { VBT_ACTION_TYPE_RADIO_TUNE_PRESET,
        &VR_VoiceBoxAgentAudio::TunePreset },  // "Tune Preset" Task Complete
        { VBT_ACTION_TYPE_RADIO_TUNE_FREQUENCY_BAND_SATELLITE_STATION,
        &VR_VoiceBoxAgentAudio::TuneFrequencyBandSatelliteStation },  // "Tune Frequency Band Satellite - Station" Task Complete
        { VBT_ACTION_TYPE_RADIO_TUNE_FREQUENCY_BAND_AM_FREQUENCY,
        &VR_VoiceBoxAgentAudio::TuneFrequencyBandAMFrequency },  // "Tune Frequency Band AM - Frequency" Task Complete
        { VBT_ACTION_TYPE_RADIO_TUNE_FREQUENCY_BAND_FM_FREQUENCY,
        &VR_VoiceBoxAgentAudio::TuneFrequencyBandFMFrequency },  // "Tune Frequency Band FM - Frequency" Task Complete

        { VBT_ACTION_TYPE_RADIO_GO_TO_RADIO,
        &VR_VoiceBoxAgentAudio::GoToRadio },  // "Go to Radio" Task Complete (re-activate VR)
        { VBT_ACTION_TYPE_RADIO_TUNE_RADIO_CATCH,
        &VR_VoiceBoxAgentAudio::TuneRadioCatch },  // "Tune Radio Catch" Task Complete (re-activate VR)
        { VBT_ACTION_TYPE_RADIO_TUNE_FREQUENCY_BAND_AM,
        &VR_VoiceBoxAgentAudio::TuneFrequencyBandAM },  // "Tune Frequency Band AM" Task Complete (re-activate VR)
        { VBT_ACTION_TYPE_RADIO_TUNE_FREQUENCY_BAND_FM,
        &VR_VoiceBoxAgentAudio::TuneFrequencyBandFM },  // "Tune Frequency Band FM" Task Complete (re-activate VR)
        { VBT_ACTION_TYPE_RADIO_TUNE_PRESET_CATCH,
        &VR_VoiceBoxAgentAudio::TunePresetCatch },  // "Tune Preset Catch" Follow-Up 4
        { VBT_ACTION_TYPE_RADIO_TUNE_FM_GENRE_CATCH,
        &VR_VoiceBoxAgentAudio::TuneFMGenreCatch },  // "Tune FM Genre Catch" Follow-Up 3
        { VBT_ACTION_TYPE_RADIO_TUNE_SATELLITE_GENRE_CATCH,
        &VR_VoiceBoxAgentAudio::TuneSatelliteGenreCatch },  // "Tune Satellite Genre Catch" Follow-Up 3
        { VBT_ACTION_TYPE_RADIO_TUNE_FREQUENCY_BAND_SATELLITE,
        &VR_VoiceBoxAgentAudio::TuneFrequencyBandSatellite },  // "Tune Frequency Band Satellite" Task Complete (re-activate VR) \  Follow-Up
        { VBT_ACTION_TYPE_RADIO_TUNE_FREQUENCY_CATCH,
        &VR_VoiceBoxAgentAudio::TuneFrequencyCatch },  // "Tune Frequency Catch" Task Complete (re-activate VR) \  Follow-Up
        { VBT_LIST_TYPE_RADIO_NBEST,
        &VR_VoiceBoxAgentAudio::AudioNBest }, // Radio nBest
        { VBT_ACTION_TYPE_RADIO_BACK, // "Back"
        &VR_VoiceBoxAgentAudio::AudioBack },
        { VBT_ACTION_TYPE_RADIO_CANCEL, // "Cancel"
        &VR_VoiceBoxAgentAudio::AudioCancel },
        { VBT_ACTION_TYPE_RADIO_HELP, // "Help"l
        &VR_VoiceBoxAgentAudio::AudioHelp },
        { VBT_ACTION_TYPE_RADIO_START_OVER, // "Start Over"
        &VR_VoiceBoxAgentAudio::AudioStartOver },
        { VBT_ACTION_TYPE_RADIO_SYSTEM_PAUSE, // "Start Pause"
        &VR_VoiceBoxAgentAudio::AudioStartPause },
        { VBT_ACTION_TYPE_RADIO_SYSTEM_REPEAT, // "Start Repeat"
        &VR_VoiceBoxAgentAudio::AudioStartRepeat },

        // Info Driver
        { VBT_DRIVER_REQUEST_ACTION_TYPE_RADIO_ACTIVE_FREQUENCY_BAND_REQUEST,
        &VR_VoiceBoxAgentAudio::ActiveFrequencyBandRequest }, // "Active Frequency Band Request",
        { VBT_DRIVER_REQUEST_ACTION_TYPE_RADIO_FREQUENCY_BAND_STATUS_REQUEST,
        &VR_VoiceBoxAgentAudio::FrequencyBandStatusRequest },  // "Frequency Band Status Request"
        { VBT_DRIVER_REQUEST_ACTION_TYPE_RADIO_HD_SUBCHANNEL_AVAILABLE_FOR_CURRENT_FREQUENCY_REQUEST,
        &VR_VoiceBoxAgentAudio::HDSubChannelAvailforCurFreRequest }, // "HD Sub-channel Available for Current Frequency Request"
        { VBT_DRIVER_REQUEST_ACTION_TYPE_RADIO_HD_SUBCHANNEL_AVAILABLE_FOR_FREQUENCY_REQUEST,
        &VR_VoiceBoxAgentAudio::HDSubchannelAvailableforFreRequest }, // "HD Sub-channel Available for Frequency Request"
        { VBT_DRIVER_REQUEST_ACTION_TYPE_RADIO_PRESET_DEFINED_REQUEST,
        &VR_VoiceBoxAgentAudio::PresetDefinedRequest }, // "Preset Defined Request"
        { VBT_DRIVER_REQUEST_ACTION_TYPE_RADIO_PRESETS_AVAILABLE_REQUEST,
        &VR_VoiceBoxAgentAudio::PresetsAvailableRequest }, // "Presets Available Request"
        { VBT_DRIVER_REQUEST_ACTION_TYPE_RADIO_RADIO_STATUS_REQUEST,
        &VR_VoiceBoxAgentAudio::RadioStatusRequest }, // "Radio Status Request"
        { VBT_DRIVER_REQUEST_ACTION_TYPE_RADIO_STATIONS_AVAILABLE_IN_FM_GENRE_REQUEST,
        &VR_VoiceBoxAgentAudio::StationsAvailableinFMGenreRequest }, // "Stations Available in FM Genre Request"
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

    m_audioMsgResult.insert(std::make_pair("getCurrentAlbum", &VR_VoiceBoxAgentAudio::GetCurrentAlbum));
    m_audioMsgResult.insert(std::make_pair("getCurrentArtist", &VR_VoiceBoxAgentAudio::GetCurrentArtist));
    m_audioMsgResult.insert(std::make_pair("getCurrentComposer", &VR_VoiceBoxAgentAudio::GetCurrentComposer));
    m_audioMsgResult.insert(std::make_pair("getCurrentGenre", &VR_VoiceBoxAgentAudio::GetCurrentGenre));

    m_audioMsgResult.insert(std::make_pair("queryActiveFrequencyBand", &VR_VoiceBoxAgentAudio::ActiveFrequencyBandReply));
    m_audioMsgResult.insert(std::make_pair("queryFreqBandStatus", &VR_VoiceBoxAgentAudio::FrequencyBandStatusReply));
    m_audioMsgResult.insert(std::make_pair("queryCurrentHDSubchannelAvailable", &VR_VoiceBoxAgentAudio::HDSubChannelAvailforCurFreReply));
    m_audioMsgResult.insert(std::make_pair("queryHDSubchannelAvailable", &VR_VoiceBoxAgentAudio::HDSubchannelAvailableforFreReply));
    m_audioMsgResult.insert(std::make_pair("queryPresetDefined", &VR_VoiceBoxAgentAudio::PresetDefinedReply));
    m_audioMsgResult.insert(std::make_pair("queryPresetsAvailable", &VR_VoiceBoxAgentAudio::PresetsAvailableReply));
    m_audioMsgResult.insert(std::make_pair("queryRadioStatus", &VR_VoiceBoxAgentAudio::RadioStatusReply));
    m_audioMsgResult.insert(std::make_pair("queryFMGenreAvailable", &VR_VoiceBoxAgentAudio::StationsAvailableinFMGenreReply));

    // radia command content
    m_radioCommandtoContent.insert(std::make_pair("Tune FM Genre", "na_media_radio_genre_select"));
    m_radioCommandtoContent.insert(std::make_pair("Radio Global Active Satellite - Tune Channel Name", "na_media_radio_satellite_channel_select"));
    m_radioCommandtoContent.insert(std::make_pair("Tune Channel Name", "na_media_radio_satellite_channel_select"));
    m_radioCommandtoContent.insert(std::make_pair("Tune Satellite Genre", "na_media_radio_genre_select"));
    m_radioCommandtoContent.insert(std::make_pair("Radio Home - Tune FM Genre", "na_media_radio_genre_select"));
    m_radioCommandtoContent.insert(std::make_pair("Radio Home Active Satellite - Tune Channel Name", "na_media_radio_satellite_channel_select"));
    m_radioCommandtoContent.insert(std::make_pair("Radio Home Satellite - Tune Channel Name", "na_media_radio_satellite_channel_select"));
    m_radioCommandtoContent.insert(std::make_pair("Radio Home Satellite - Tune Satellite Genre", "na_media_radio_genre_select"));
    m_radioCommandtoContent.insert(std::make_pair("Select FM Genre", "na_media_radio_genre_select"));
    m_radioCommandtoContent.insert(std::make_pair("Select Satellite Genre", "na_media_radio_genre_select"));

    return true;
}

void
VR_VoiceBoxAgentAudio::UnInitialize()
{
    m_audioMsgHandler.clear();
    m_audioMsgResult.clear();
    m_radioCommandtoContent.clear();
}

bool
VR_VoiceBoxAgentAudio::ReplyQueryInfo(
    VR_VoiceBoxXmlParser& parser
)
{
    std::string strActionKey = parser.getXmlKey();

    VR_LOG("VR_VoiceBoxAgentAudio ReplyQueryInfo strActionKey = %s \n", strActionKey.c_str());

    VoiceMap<std::string, AudioResultResult>::const_iterator iterMap = m_audioMsgResult.find(strActionKey);
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
VR_VoiceBoxAgentAudio::QueryAgentSupport(
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
VR_VoiceBoxAgentAudio::OnAgentResultMessage(
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
VR_VoiceBoxAgentAudio::OnAgentDriverMessage(
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
VR_VoiceBoxAgentAudio::ProcessAgentMessage(IVECIParsedMessage *pcMsg)
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
    VoiceMap<std::string, AudioMsgHandler>::const_iterator iterMap = m_audioMsgHandler.find(strAgentResult);
    if (m_audioMsgHandler.cend() != iterMap) {
        if (NULL != iterMap->second) {
            (this->*(iterMap->second))(pcMsg);
            return true;
        }
    }

    return true;
}

void
VR_VoiceBoxAgentAudio::SendMusicPlay(const std::string& strSourceID,
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
VR_VoiceBoxAgentAudio::PlayAlbum(IVECIParsedMessage *pcMsg)
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
VR_VoiceBoxAgentAudio::ShufflePlayAlbum(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    std::string strAlbumID = GetActionParaByName(pcMsg, "Album ID");

    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    SendMusicPlay(strSourceID, "PLAY_ALBUM", strAlbumID, "true", "");

    return true;
}

bool
VR_VoiceBoxAgentAudio::PlayAll(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    SendMusicPlay(strSourceID, "PLAY_ALL", "", "true", "");

    return true;
}

bool
VR_VoiceBoxAgentAudio::PlayArtist(IVECIParsedMessage *pcMsg)
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
VR_VoiceBoxAgentAudio::ShufflePlayArtist(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    std::string strArtistID = GetActionParaByName(pcMsg, "Artist ID");

    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    SendMusicPlay(strSourceID, "PLAY_ARTIST", strArtistID, "true", "");

    return true;
}

bool
VR_VoiceBoxAgentAudio::PlayAudioBook(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    std::string strAudioBookID = GetActionParaByName(pcMsg, "Audiobook ID");

    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    SendMusicPlay(strSourceID, "PLAY_AUDIO_BOOK", strAudioBookID, "false", "");

    return true;
}

bool
VR_VoiceBoxAgentAudio::PlayComposer(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    std::string strComposerID = GetActionParaByName(pcMsg, "Composer ID");

    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    SendMusicPlay(strSourceID, "PLAY_COMPOSER", strComposerID, "false", "");

    return true;
}

bool
VR_VoiceBoxAgentAudio::ShufflePlayComposer(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    std::string strComposerID = GetActionParaByName(pcMsg, "Composer ID");

    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    SendMusicPlay(strSourceID, "PLAY_COMPOSER", strComposerID, "true", "");

    return true;
}

bool
VR_VoiceBoxAgentAudio::PlayGenre(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    std::string strGenreID = GetActionParaByName(pcMsg, "Genre ID");

    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    SendMusicPlay(strSourceID, "PLAY_GENRE", strGenreID, "false", "");

    return true;
}

bool
VR_VoiceBoxAgentAudio::PlayPlaylist(IVECIParsedMessage *pcMsg)
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
VR_VoiceBoxAgentAudio::ShufflePlayPlaylist(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    std::string strPlaylistID = GetActionParaByName(pcMsg, "Playlist ID");

    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    SendMusicPlay(strSourceID, "PLAY_PLAYLIST", strPlaylistID, "true", "");

    return true;
}

bool
VR_VoiceBoxAgentAudio::PlayPodcast(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    std::string strPodcastID = GetActionParaByName(pcMsg, "Podcast ID");

    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    SendMusicPlay(strSourceID, "PLAY_PODCAST", strPodcastID, "true", "");

    return true;
}

bool
VR_VoiceBoxAgentAudio::PlayPodcastItem(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    std::string strPodcastID = GetActionParaByName(pcMsg, "Podcast ID");

    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    std::string strEpisodeID = GetActionParaByName(pcMsg, "Episode ID");

    SendMusicPlay(strSourceID, "PLAY_PODCAST_ITEM", strPodcastID, "false", strEpisodeID);

    return true;
}

bool
VR_VoiceBoxAgentAudio::PlaySong(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    VR_LOGP("DE: receive VBT processing data, send action playsong to DM...case 212-122-99 212-128-99");
    

    std::string strSongID = GetActionParaByName(pcMsg, "Song ID");

    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    SendMusicPlay(strSourceID, "PLAY_SONG", strSongID, "false", "");

    return true;
}

bool
VR_VoiceBoxAgentAudio::AudioOn(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("media", "turnAudioOn");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio::AudioOff(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("media", "turnAudioOff");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

pugi::xml_node
VR_VoiceBoxAgentAudio::BuildScreenDisplayFront(VR_VoiceBoxXmlBuilder& xmlBulder,
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
VR_VoiceBoxAgentAudio::BuildScreenDisplayAll(
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
VR_VoiceBoxAgentAudio::BuildListHeader(IVECIParsedMessage *pcMsg,
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
VR_VoiceBoxAgentAudio::BuildMusicInfo(VR_VoiceBoxXmlBuilder& xmlBulder, pugi::xml_node& xmlNode,
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
VR_VoiceBoxAgentAudio::BuildListInfo(IVECIParsedMessage *pcMsg,
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
VR_VoiceBoxAgentAudio::BrowseAlbum(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("BrowseAlbum: %s\n", xmlMsg.Get());

    std::string strAlbumName = GetActionParaByName(pcMsg, "Album");

    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode;

    xmlNode = BuildScreenDisplayFront(xmlBulder, "na_media_music_album_song_select", strAlbumName);

    pugi::xml_node nodeSelectList = xmlBulder.buildGivenElement(xmlNode, "selectList", "", "", "");

    CVECIOutStr strListType;
    (void)pcMsg->GetListType(&strListType);

    if (NULL != strListType.Get()) {
        BuildListInfo(pcMsg, xmlBulder, nodeSelectList, "Song", "Song ID", strSourceID);
    }

    std::string strTmp;
    m_engineCallback.GetPromptByScreenId("VR-MUS-06", strTmp);

    char buf[256] = { 0 };
    snprintf(buf, sizeof(buf), strTmp.c_str(), strAlbumName.c_str());
    m_strPrompt = std::string(buf);

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio::OnAlbums(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    BuildScreenDisplayAll("na_media_music_speak_album_name", "VR-MUS-05");

    return true;
}

bool
VR_VoiceBoxAgentAudio::BrowseArtist(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("BrowseArtist: %s\n", xmlMsg.Get());

    std::string strArtistName = GetActionParaByName(pcMsg, "Artist");
    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode;

    xmlNode = BuildScreenDisplayFront(xmlBulder, "na_media_music_artist_album_select", strArtistName);

    pugi::xml_node nodeSelectList = xmlBulder.buildGivenElement(xmlNode, "selectList", "", "", "");

    CVECIOutStr strListType;
    (void)pcMsg->GetListType(&strListType);

    if (NULL != strListType.Get()) {
        BuildListInfo(pcMsg, xmlBulder, nodeSelectList, "Album", "Album ID", strSourceID);
    }

    std::string strTmp;
    m_engineCallback.GetPromptByScreenId("VR-MUS-08", strTmp);

    char buf[256] = { 0 };
    snprintf(buf, sizeof(buf), strTmp.c_str(), strArtistName.c_str());
    m_strPrompt = std::string(buf);

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio::BrowseArtistSongs(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("BrowseArtistSongs: %s\n", xmlMsg.Get());

    std::string strArtistName = GetActionParaByName(pcMsg, "Artist");
    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode;

    xmlNode = BuildScreenDisplayFront(xmlBulder, "na_media_music_artist_song_select", strArtistName);

    pugi::xml_node nodeSelectList = xmlBulder.buildGivenElement(xmlNode, "selectList", "", "", "");

    CVECIOutStr strListType;
    (void)pcMsg->GetListType(&strListType);

    if (NULL != strListType.Get()) {
        BuildListInfo(pcMsg, xmlBulder, nodeSelectList, "Song", "Song ID", strSourceID);
    }

    std::string strTmp;
    m_engineCallback.GetPromptByScreenId("VR-MUS-09", strTmp);

    char buf[256] = { 0 };
    snprintf(buf, sizeof(buf), strTmp.c_str(), strArtistName.c_str());
    m_strPrompt = std::string(buf);

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio::BrowseComposer(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("BrowseComposer: %s\n", xmlMsg.Get());

    std::string strComposerName = GetActionParaByName(pcMsg, "Composer");
    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode;

    xmlNode = BuildScreenDisplayFront(xmlBulder, "na_media_music_composer_song_select", strComposerName);

    pugi::xml_node nodeSelectList = xmlBulder.buildGivenElement(xmlNode, "selectList", "", "", "");

    CVECIOutStr strListType;
    (void)pcMsg->GetListType(&strListType);

    if (NULL != strListType.Get()) {
        BuildListInfo(pcMsg, xmlBulder, nodeSelectList, "Song", "Song ID", strSourceID);
    }

    std::string strTmp;
    m_engineCallback.GetPromptByScreenId("VR-MUS-12", strTmp);

    char buf[256] = { 0 };
    snprintf(buf, sizeof(buf), strTmp.c_str(), strComposerName.c_str());
    m_strPrompt = std::string(buf);

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio::OnDisambiguateAlbumArtist(IVECIParsedMessage *pcMsg) // 1
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("OnDisambiguateAlbumArtist: %s\n", xmlMsg.Get());

    std::string strAlbumName = GetActionParaByName(pcMsg, "Album");
    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode;

    xmlNode = BuildScreenDisplayFront(xmlBulder, "na_media_music_which_artist_for_album", strAlbumName);

    pugi::xml_node nodeSelectList = xmlBulder.buildGivenElement(xmlNode, "selectList", "", "", "");

    CVECIOutStr strListType;
    (void)pcMsg->GetListType(&strListType);

   if (NULL != strListType.Get()) {
        BuildListInfo(pcMsg, xmlBulder, nodeSelectList, "Artist", "Artist ID", strSourceID);
    }

    std::string strTmp;
    m_engineCallback.GetPromptByScreenId("VR-MUS-01", strTmp);

    char buf[256] = { 0 };
    snprintf(buf, sizeof(buf), strTmp.c_str(), strAlbumName.c_str());
    m_strPrompt = std::string(buf);

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio::BrowseGenre(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("BrowseGenre: %s\n", xmlMsg.Get());

    std::string strGenreName = GetActionParaByName(pcMsg, "Genre");
    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode;

    xmlNode = BuildScreenDisplayFront(xmlBulder, "na_media_music_genre_artist_select", strGenreName);

    pugi::xml_node nodeSelectList = xmlBulder.buildGivenElement(xmlNode, "selectList", "", "", "");

    CVECIOutStr strListType;
    (void)pcMsg->GetListType(&strListType);

    if (NULL != strListType.Get()) {
        BuildListInfo(pcMsg, xmlBulder, nodeSelectList, "Artist", "Artist ID", strSourceID);
    }

    std::string strTmp;
    m_engineCallback.GetPromptByScreenId("VR-MUS-14", strTmp);
    char buf[256] = { 0 };
    snprintf(buf, sizeof(buf), strTmp.c_str(), strGenreName.c_str());
    m_strPrompt = std::string(buf);

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio::BrowsePlaylist(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("BrowsePlaylist: %s\n", xmlMsg.Get());

    std::string strPlaylistName = GetActionParaByName(pcMsg, "Playlist");
    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode;

    xmlNode = BuildScreenDisplayFront(xmlBulder, "na_media_music_play_list_song_select", strPlaylistName);

    pugi::xml_node nodeSelectList = xmlBulder.buildGivenElement(xmlNode, "selectList", "", "", "");

    CVECIOutStr strListType;
    (void)pcMsg->GetListType(&strListType);

    if (NULL != strListType.Get()) {
        BuildListInfo(pcMsg, xmlBulder, nodeSelectList, "Song", "Song ID", strSourceID);
    }

    std::string strTmp;
    m_engineCallback.GetPromptByScreenId("VR-MUS-17", strTmp);
    char buf[256] = { 0 };
    snprintf(buf, sizeof(buf), strTmp.c_str(), strPlaylistName.c_str());
    m_strPrompt = std::string(buf);

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio::BrowsePodcast(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("BrowsePodcast: %s\n", xmlMsg.Get());

    std::string strPodcastName = GetActionParaByName(pcMsg, "Podcast");
    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode;

    xmlNode = BuildScreenDisplayFront(xmlBulder, "na_media_music_podcast_item_select", strPodcastName);

    pugi::xml_node nodeSelectList = xmlBulder.buildGivenElement(xmlNode, "selectList", "", "", "");

    CVECIOutStr strListType;
    (void)pcMsg->GetListType(&strListType);

    if (NULL != strListType.Get()) {
        BuildListInfo(pcMsg, xmlBulder, nodeSelectList, "Episode", "Episode ID", strSourceID);
    }
    std::string strTmp;
    m_engineCallback.GetPromptByScreenId("VR-MUS-23", strTmp);

    char buf[256] = { 0 };
    snprintf(buf, sizeof(buf), strTmp.c_str(), strPodcastName.c_str());
    m_strPrompt = std::string(buf);

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio::ChangeSourceCatch(IVECIParsedMessage *pcMsg) // 21
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("ChangeSourceCatch: %s\n", xmlMsg.Get());

    VR_VoiceBoxXmlBuilder xmlBulder;

    pugi::xml_node xmlNode = BuildScreenDisplayFront(xmlBulder, "na_media_music_source_select", "");

    pugi::xml_node nodeSelectList = xmlBulder.buildGivenElement(xmlNode, "selectList", "", "", "");

    CVECIOutStr strListType;
    (void)pcMsg->GetListType(&strListType);

    if (NULL != strListType.Get()) {
        BuildListInfo(pcMsg, xmlBulder, nodeSelectList, "Name", "ID", "");
    }

    m_engineCallback.GetPromptByScreenId("VR-MUS-21", m_strPrompt);

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio::OnCatch(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    BuildScreenDisplayAll("na_media_music_speak_a_category", "VR-MUS-03");

    return true;
}

bool
VR_VoiceBoxAgentAudio::OnComposers(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    BuildScreenDisplayAll("na_media_music_speak_composer_name", "VR-MUS-11");

    return true;
}

bool
VR_VoiceBoxAgentAudio::PlayDisambiguateSongAlbum(IVECIParsedMessage *pcMsg) // 4
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

    xmlNode = BuildScreenDisplayFront(xmlBulder, "na_media_music_which_album_for_song", strSongName);

    pugi::xml_node nodeSelectList = xmlBulder.buildGivenElement(xmlNode, "selectList", "", "", "");

    CVECIOutStr strListType;
    (void)pcMsg->GetListType(&strListType);

    if (NULL != strListType.Get()) {
        BuildListInfo(pcMsg, xmlBulder, nodeSelectList, "Album", "Album ID", strSourceID);
    }

    std::string strTmp;
    m_engineCallback.GetPromptByScreenId("VR-MUS-04", strTmp);
    char buf[256] = { 0 };
    snprintf(buf, sizeof(buf), strTmp.c_str(), strSongName.c_str());
    m_strPrompt = std::string(buf);

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio::PlayDisambiguateSongArtist(IVECIParsedMessage *pcMsg) // 2
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

    xmlNode = BuildScreenDisplayFront(xmlBulder, "na_media_music_which_artist_for_song", strSongName);

    pugi::xml_node nodeSelectList = xmlBulder.buildGivenElement(xmlNode, "selectList", "", "", "");

    CVECIOutStr strListType;
    (void)pcMsg->GetListType(&strListType);

    if (NULL != strListType.Get()) {
        BuildListInfo(pcMsg, xmlBulder, nodeSelectList, "Artist", "Artist ID", strSourceID);
    }

    std::string strTmp;
    m_engineCallback.GetPromptByScreenId("VR-MUS-02", strTmp);
    char buf[256] = { 0 };
    snprintf(buf, sizeof(buf), strTmp.c_str(), strSongName.c_str());
    m_strPrompt = std::string(buf);

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio::OnGenres(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    BuildScreenDisplayAll("na_media_music_speak_genre_name", "VR-MUS-13");

    return true;
}

bool
VR_VoiceBoxAgentAudio::OnPlaylists(IVECIParsedMessage *pcMsg) // 16
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("OnPlaylists: %s", xmlMsg.Get());

    std::string strSourceID = GetActionParaByName(pcMsg, "Source ID");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode = BuildScreenDisplayFront(xmlBulder, "na_media_music_play_list_select", "");

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
VR_VoiceBoxAgentAudio::OnPodcasts(IVECIParsedMessage *pcMsg) // 18
{
    if (NULL == pcMsg) {
        return false;
    }

    BuildScreenDisplayAll("na_media_music_speak_podcastname", "VR-MUS-18");

    return true;
}

bool
VR_VoiceBoxAgentAudio::OnArtists(IVECIParsedMessage *pcMsg) // 7
{
    if (NULL == pcMsg) {
        return false;
    }

    BuildScreenDisplayAll("na_media_music_speak_artist_name", "VR-MUS-07");

    return true;
}

bool
VR_VoiceBoxAgentAudio::OnAudiobooks(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    BuildScreenDisplayAll("na_media_music_speak_audiobooks_name", "VR-MUS-10");

    return true;
}

bool
VR_VoiceBoxAgentAudio::OnSongs(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    BuildScreenDisplayAll("na_media_music_speak_song_name", "VR-MUS-20");

    return true;
}

bool
VR_VoiceBoxAgentAudio::MusicMenu(IVECIParsedMessage *pcMsg)
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
VR_VoiceBoxAgentAudio::ChangeSource(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    std::string strSourceName = GetActionParaByName(pcMsg, "Source");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("media", "changeSourceByName");

    if ("USB" == strSourceName && VR_VoiceBoxCatalogAudio::m_bIPodUSB2) {
        strSourceName = "USB2";
    }
    else if ("iPod" == strSourceName && VR_VoiceBoxCatalogAudio::m_bUSBIPod2) {
        strSourceName = "iPod2";
    }
    else {

    }

    (void)xmlBulder.buildGivenElement(node, "sourceName", strSourceName, "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio::MusicKeyboardDictation(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }
    // Client uses the ASR results to fill-in the keyboard input.
    std::string strUtterance = GetActionParaByName(pcMsg, "utterance");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("media", "dictationMusicKeyboard");
    (void)xmlBulder.buildGivenElement(node, "utterance", strUtterance, "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio::AudioNBest(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr strAgentName;
    (void)pcMsg->GetAgentName(&strAgentName);

    if (strAgentName.IsEqual("Music")) {
        MusicNBest(pcMsg);
    }
    else if (strAgentName.IsEqual("Radio")) {
        RadioNBest(pcMsg);
    }
    else {

    }

    return true;
}

bool
VR_VoiceBoxAgentAudio::MusicNBest(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("MusicNBest: %s", xmlMessage.Get());

    VR_VoiceBoxXmlBuilder xmlBulder;

    pugi::xml_node xmlNode = BuildScreenDisplayFront(xmlBulder, "na_media_music_nbest_select", "");
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

void
VR_VoiceBoxAgentAudio::OnInfoRequest(const std::string& strOp)
{
    VR_VoiceBoxXmlBuilder xmlBulder;

    pugi::xml_node node = xmlBulder.buildStartActionElement("media", strOp);
    (void)xmlBulder.buildGivenElement(node, "sourceid", VR_VoiceBoxCatalogAudio::m_strSourceId, "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnInfoQueryMessage(strReply);
}

bool
VR_VoiceBoxAgentAudio::GetCurrentAlbumRequest(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    OnInfoRequest("getCurrentAlbum");

    return true;
}

bool
VR_VoiceBoxAgentAudio::GetCurrentArtistRequest(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    OnInfoRequest("getCurrentArtist");

    return true;
}

bool
VR_VoiceBoxAgentAudio::GetCurrentComposerRequest(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    OnInfoRequest("getCurrentComposer");

    return true;
}

bool
VR_VoiceBoxAgentAudio::GetCurrentGenreRequest(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    OnInfoRequest("getCurrentGenre");

    return true;
}

bool
VR_VoiceBoxAgentAudio::GetPodcastEpisodesRequest(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    std::string strPodcastID = GetActionParaByName(pcMsg, "Podcast ID");

    VoiceMap<std::string, std::string>::type mapExternalId2Name;

    GetRelateInfoByBaseID("MusicAgentExEpisodes", "MusicAgentExPodcastsEpisodes",
            "nEpisodeId", "nPodcastId", strPodcastID, mapExternalId2Name);

    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    if (NULL == spParams.ptr()) {
        return false;
    }
    (void)spParams->AddParameter("Status", "Success", "", "");

    CVECIPtr<IVECIListItems> spListItems;
    m_client.CreateListItems(&spListItems);
    if (NULL == spListItems.ptr()) {
        return false;
    }

    VoiceMap<std::string, std::string>::iterator iter = mapExternalId2Name.begin();
    for (; mapExternalId2Name.end() != iter; ++iter) {
        CVECIPtr<IVECIParameterSet> spItem;
        m_client.CreateParameterSet(&spItem);
        if (NULL == spItem.ptr()) {
            continue;
        }

        (void)spItem->AddParameter("Episode ID", iter->first.c_str(), "", "");
        (void)spItem->AddParameter("Episode", iter->second.c_str(), "", "");

        VR_LOG("first: %s, second: %s", iter->first.c_str(), iter->second.c_str());

        spListItems->AddItem(spItem);
    }

    HRESULT retCode1 = m_engineCommand.DriverActionReply("Music", "Get Podcast Episodes Reply", spParams, "Episodes", spListItems);
    if (S_OK != retCode1) {
        return false;
    }

    return true;
}

int
VR_VoiceBoxAgentAudio::GetRelateInfoBySql(void* strName, int nCount, char** pValue, char** pName)
{
    VoiceMap<std::string, std::string>::type * mapTmp = (VoiceMap<std::string, std::string>::type *)strName;

    (*mapTmp).insert(std::make_pair(pValue[0], pValue[1]));

    return 0;
}

void
VR_VoiceBoxAgentAudio::GetRelateInfoByBaseID(const std::string& strBaseTableName,
    const std::string& strRelateTableName, const std::string& strRelateIndexName,
    const std::string& strIndexName, const std::string& strId,
    VoiceMap<std::string, std::string>::type& mapExternalId2Name)
{
    mapExternalId2Name.clear();

    sqlite3* conn = NULL;
    std::string strDBPath = VR_VoiceBoxCatalogAudio::m_strDBPath;
    VR_LOG("GetRelateInfoByBaseID strDBPath = %s", strDBPath.c_str());

    // open sqlite3 db
    int retCode = sqlite3_open(strDBPath.c_str(), &conn);
    if (SQLITE_OK != retCode) {
        VR_LOG("open sqlite db fail");
        sqlite3_close(conn);
        return;
    }
    else {
        VR_LOG("open sqlite db OK");
    }

    std::string strSql = "select nExternalId, cName from ";
    strSql.append(strBaseTableName);
    strSql.append(" where nExternalId in (select ");
    strSql.append(strRelateIndexName);
    strSql.append(" from ");
    strSql.append(strRelateTableName);
    strSql.append(" where (");
    strSql.append(strIndexName);
    strSql.append(" = ");
    strSql.append(strId);
    strSql.append(" and nSourceId = ");
    strSql.append(VR_VoiceBoxCatalogAudio::m_strSourceId);
    strSql.append("));");

    VR_LOG("strSql = %s", strSql.c_str());

    char * errorMsg = NULL;
    retCode = sqlite3_exec(conn, strSql.c_str(), GetRelateInfoBySql, &mapExternalId2Name, &errorMsg);

    if (SQLITE_OK != retCode) {
        sqlite3_close(conn);
        VR_LOG("sqlite3_exec: errorMsg = %s", errorMsg);
        sqlite3_free(errorMsg);
        return;
    }

    VR_LOG("mapExternalId2Name.size = %d", mapExternalId2Name.size());

    sqlite3_close(conn);
}

std::string
VR_VoiceBoxAgentAudio::GetStatusByErrCode(const std::string& strErrCode)
{
    if (strErrCode.empty()) {
        return "";
    }

    std::string strStatus;

    if ("0" == strErrCode) {
        strStatus = "Success";
    }
    else if ("20" == strErrCode) {
        strStatus = "No Song Playing";
    }
    else {
        strStatus = "Information Not Available";
    }

    return strStatus;
}

bool
VR_VoiceBoxAgentAudio::GetCurrentAlbum(VR_VoiceBoxXmlParser& parser)
{
    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    if (NULL == spParams.ptr()) {
        return false;
    }

    std::string strErrCode = parser.getValueByKey("errcode");
    std::string strStatus = GetStatusByErrCode(strErrCode);

    (void)spParams->AddParameter("Status", strStatus.c_str(), "", "");

    VR_LOG("status = %s", strStatus.c_str());

    if ("Success" == strStatus) {
        std::string strSourceId = VR_VoiceBoxCatalogAudio::m_strSourceId;
        std::string strSongId = parser.getValueByKey("songid");

        VoiceMap<std::string, std::string>::type mapExternalId2Name;

        GetRelateInfoByBaseID("MusicAgentAlbums", "MusicAgentAlbumsSongs",
            "nAlbumId", "nSongId", strSongId, mapExternalId2Name);

        if (!mapExternalId2Name.empty()) {
            (void)spParams->AddParameter("Source ID", strSourceId.c_str(), "", "");
            (void)spParams->AddParameter("Album ID", mapExternalId2Name.begin()->first.c_str(), "", "");
            (void)spParams->AddParameter("Album", mapExternalId2Name.begin()->second.c_str(), "", "");

            VR_LOG("first: %s, second: %s", mapExternalId2Name.begin()->first.c_str(), mapExternalId2Name.begin()->second.c_str());
        }
    }

    HRESULT retCode = m_engineCommand.DriverActionReply("Music", "Get Current Album Reply", spParams, NULL, NULL);
    if (S_OK != retCode) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentAudio::GetCurrentArtist(VR_VoiceBoxXmlParser& parser)
{
    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    if (NULL == spParams.ptr()) {
        return false;
    }

    std::string strErrCode = parser.getValueByKey("errcode");
    std::string strStatus = GetStatusByErrCode(strErrCode);

    (void)spParams->AddParameter("Status", strStatus.c_str(), "", "");

    VR_LOG("status = %s", strStatus.c_str());

    if ("Success" == strStatus) {
        std::string strSourceId = VR_VoiceBoxCatalogAudio::m_strSourceId;
        std::string strSongId = parser.getValueByKey("songid");

        VoiceMap<std::string, std::string>::type mapExternalId2Name;

        GetRelateInfoByBaseID("MusicAgentArtists", "MusicAgentArtistsSongs",
            "nArtistId", "nSongId", strSongId, mapExternalId2Name);

        if (!mapExternalId2Name.empty()) {
            (void)spParams->AddParameter("Source ID", strSourceId.c_str(), "", "");
            (void)spParams->AddParameter("Artist ID", mapExternalId2Name.begin()->first.c_str(), "", "");
            (void)spParams->AddParameter("Artist", mapExternalId2Name.begin()->second.c_str(), "", "");

            VR_LOG("first: %s, second: %s", mapExternalId2Name.begin()->first.c_str(), mapExternalId2Name.begin()->second.c_str());
        }
    }

    HRESULT retCode = m_engineCommand.DriverActionReply("Music", "Get Current Artist Reply", spParams, NULL, NULL);
    if (S_OK != retCode) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentAudio::GetCurrentComposer(VR_VoiceBoxXmlParser& parser)
{
    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    if (NULL == spParams.ptr()) {
        return false;
    }

    std::string strErrCode = parser.getValueByKey("errcode");
    std::string strStatus = GetStatusByErrCode(strErrCode);

    (void)spParams->AddParameter("Status", strStatus.c_str(), "", "");

    VR_LOG("status = %s", strStatus.c_str());

    if ("Success" == strStatus) {
        std::string strSourceId = VR_VoiceBoxCatalogAudio::m_strSourceId;
        std::string strSongId = parser.getValueByKey("songid");

        VoiceMap<std::string, std::string>::type mapExternalId2Name;

        GetRelateInfoByBaseID("MusicAgentComposers", "MusicAgentComposersSongs",
            "nComposerId", "nSongId", strSongId, mapExternalId2Name);

        if (!mapExternalId2Name.empty()) {
            (void)spParams->AddParameter("Source ID", strSourceId.c_str(), "", "");
            (void)spParams->AddParameter("Composer ID", mapExternalId2Name.begin()->first.c_str(), "", "");
            (void)spParams->AddParameter("Composer", mapExternalId2Name.begin()->second.c_str(), "", "");

            VR_LOG("first: %s, second: %s", mapExternalId2Name.begin()->first.c_str(), mapExternalId2Name.begin()->second.c_str());
        }
    }

    HRESULT retCode  = m_engineCommand.DriverActionReply("Music", "Get Current Composer Reply", spParams, NULL, NULL);
    if (S_OK != retCode) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentAudio::GetCurrentGenre(VR_VoiceBoxXmlParser& parser)
{
    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    if (NULL == spParams.ptr()) {
        return false;
    }

    std::string strErrCode = parser.getValueByKey("errcode");
    std::string strStatus = GetStatusByErrCode(strErrCode);

    (void)spParams->AddParameter("Status", strStatus.c_str(), "", "");

    if ("Success" == strStatus) {

        std::string strSourceId = VR_VoiceBoxCatalogAudio::m_strSourceId;
        std::string strSongId = parser.getValueByKey("songid");

        VoiceMap<std::string, std::string>::type mapExternalId2Name;

        GetRelateInfoByBaseID("MusicAgentGenres", "MusicAgentGenresSongs",
            "nGenreId", "nSongId", strSongId, mapExternalId2Name);

        if (!mapExternalId2Name.empty()) {
            (void)spParams->AddParameter("Source ID", strSourceId.c_str(), "", "");
            (void)spParams->AddParameter("Genre ID", mapExternalId2Name.begin()->first.c_str(), "", "");
            (void)spParams->AddParameter("Genre", mapExternalId2Name.begin()->second.c_str(), "", "");

            VR_LOG("first: %s, second: %s", mapExternalId2Name.begin()->first.c_str(), mapExternalId2Name.begin()->second.c_str());
        }
    }

    HRESULT retCode = m_engineCommand.DriverActionReply("Music", "Get Current Genre Reply", spParams, NULL, NULL);
    if (S_OK != retCode) {
        return false;
    }

    return true;
}



// ------------------------------------------------------------Radio
bool
VR_VoiceBoxAgentAudio::ActiveFrequencyBandRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("ActiveFrequencyBandRequest: %s", xmlMessage.Get());

    OnInfoRequest("queryActiveFrequencyBand");

    return true;
}

bool
VR_VoiceBoxAgentAudio::ActiveFrequencyBandReply(
    VR_VoiceBoxXmlParser& parser
)
{
    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    if (NULL == spParams.ptr()) {
        return false;
    }

    std::string strFrequencyBand = parser.getValueByKey("band");
    (void)spParams->AddParameter("Frequency Band", strFrequencyBand.c_str(), "", "");

    if ("XM" == strFrequencyBand) {
        strFrequencyBand = "Satellite";
    }

    VR_LOG("ActiveFrequencyBandReply FrequencyBand = %s", strFrequencyBand.c_str());

    HRESULT retCode = m_engineCommand.DriverActionReply("Radio", "Active Frequency Band Reply", spParams, NULL, NULL);
    if (S_OK != retCode) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentAudio::FrequencyBandStatusRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    std::string strFrequencyBand = GetActionParaByName(pcMsg, "Frequency Band");

    if ("Satellite" == strFrequencyBand) {
        strFrequencyBand = "XM";
    }

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("media", "queryFreqBandStatus");
    (void)xmlBulder.buildGivenElement(node, "band", strFrequencyBand, "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnInfoQueryMessage(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio::FrequencyBandStatusReply(
    VR_VoiceBoxXmlParser& parser
)
{
    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    if (NULL == spParams.ptr()) {
        return false;
    }

    std::string strFrequencyBandAlreadyactive = parser.getValueByKey("active");
    if ("True" == strFrequencyBandAlreadyactive) {
        (void)spParams->AddParameter("Frequency Band Already Active", "True", "", "");
    }
    else if ("False" == strFrequencyBandAlreadyactive) {
        (void)spParams->AddParameter("Frequency Band Already Active", "False", "", "");
        std::string strFrequencyBand = parser.getValueByKey("band");

        if ("XM" == strFrequencyBand) {
            (void)spParams->AddParameter("Frequency Band", "Satellite", "", "");

            std::string strStationName = parser.getValueByKey("stationName");

            (void)spParams->AddParameter("Station Name", strStationName.c_str(), "", "");
        }
        else {
            (void)spParams->AddParameter("Frequency Band", strFrequencyBand.c_str(), "", "");

            std::string strStationFrequency = parser.getValueByKey("frequency");

            (void)spParams->AddParameter("Station Frequency", strStationFrequency.c_str(), "", "");
        }
    }

    HRESULT retCode = m_engineCommand.DriverActionReply("Radio", "Frequency Band Status Reply", spParams, NULL, NULL);
    if (S_OK != retCode) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentAudio::HDSubChannelAvailforCurFreRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    std::string strHDSubchannel = GetActionParaByName(pcMsg, "HD Sub-channel");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("media", "queryCurrentHDSubchannelAvailable");
    (void)xmlBulder.buildGivenElement(node, "hdsubchannel", strHDSubchannel, "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnInfoQueryMessage(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio::HDSubChannelAvailforCurFreReply(
    VR_VoiceBoxXmlParser& parser
)
{
    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    if (NULL == spParams.ptr()) {
        return false;
    }

    std::string strCurrentFrequency = parser.getValueByKey("frequency");
    std::string strAvailable = parser.getValueByKey("available");

    if (!strCurrentFrequency.empty()) {
        (void)spParams->AddParameter("Current Frequency", strCurrentFrequency.c_str(), "", "");
    }

    (void)spParams->AddParameter("Available", strAvailable.c_str(), "", "");

    HRESULT retCode = m_engineCommand.DriverActionReply("Radio", "HD Sub-channel Available for Current Frequency Reply", spParams, NULL, NULL);
    if (S_OK != retCode) {
        return false;
    }

    return true;
}

// // HD Sub-channel Available for Frequency
bool
VR_VoiceBoxAgentAudio::HDSubchannelAvailableforFreRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    std::string strHDSubchannel = GetActionParaByName(pcMsg, "HD Sub-channel");
    std::string strFrequency = GetActionParaByName(pcMsg, "Frequency");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("media", "queryHDSubchannelAvailable");
    (void)xmlBulder.buildGivenElement(node, "hdsubchannel", strHDSubchannel, "", "");
    (void)xmlBulder.buildGivenElement(node, "frequency", strFrequency, "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnInfoQueryMessage(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio::HDSubchannelAvailableforFreReply(
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

    HRESULT retCode = m_engineCommand.DriverActionReply("Radio", "HD Sub-channel Available for Frequency Reply", spParams, NULL, NULL);
    if (S_OK != retCode) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentAudio::PresetDefinedRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    std::string strPreset = GetActionParaByName(pcMsg, "Preset");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("media", "queryPresetDefined");
    (void)xmlBulder.buildGivenElement(node, "number", strPreset, "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnInfoQueryMessage(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio::PresetDefinedReply(
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
VR_VoiceBoxAgentAudio::PresetsAvailableRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    OnInfoRequest("queryPresetsAvailable");

    return true;
}

bool
VR_VoiceBoxAgentAudio::PresetsAvailableReply(
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
VR_VoiceBoxAgentAudio::RadioStatusRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    OnInfoRequest("queryRadioStatus");

    return true;
}

bool
VR_VoiceBoxAgentAudio::RadioStatusReply(
    VR_VoiceBoxXmlParser& parser
)
{
    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    if (NULL == spParams.ptr()) {
        return false;
    }

    std::string strFrequencyBandAlreadyactive = parser.getValueByKey("active");
    if ("True" == strFrequencyBandAlreadyactive) {
        (void)spParams->AddParameter("Radio Already Active", "True", "", "");
    }
    else if ("False" == strFrequencyBandAlreadyactive) {
        (void)spParams->AddParameter("Radio Already Active", "False", "", "");
        std::string strFrequencyBand = parser.getValueByKey("band");

        if ("XM" == strFrequencyBand) {
            (void)spParams->AddParameter("Frequency Band", "Satellite", "", "");

            std::string strStationName = parser.getValueByKey("stationName");

            (void)spParams->AddParameter("Station Name", strStationName.c_str(), "", "");
        }
        else {
            (void)spParams->AddParameter("Frequency Band", strFrequencyBand.c_str(), "", "");

            std::string strStationFrequency = parser.getValueByKey("frequency");

            (void)spParams->AddParameter("Station Frequency", strStationFrequency.c_str(), "", "");
        }
    }

    HRESULT retCode = m_engineCommand.DriverActionReply("Radio", "Radio Status Reply", spParams, NULL, NULL);
    if (S_OK != retCode) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentAudio::StationsAvailableinFMGenreRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    std::string strID = GetActionParaByName(pcMsg, "ID");
    std::string strGenre = GetActionParaByName(pcMsg, "Genre");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("media", "queryFMGenreAvailable");
    (void)xmlBulder.buildGivenElement(node, "id", strID, "", "");
    (void)xmlBulder.buildGivenElement(node, "genre", strGenre, "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnInfoQueryMessage(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio::StationsAvailableinFMGenreReply(
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

    HRESULT retCode = m_engineCommand.DriverActionReply("Radio", "Stations Available in FM Genre Reply", spParams, NULL, NULL);
    if (S_OK != retCode) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentAudio::GoToRadio(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.GetPromptByScreenId("VR-HNT-01", m_strPrompt);

    m_engineCallback.OnRequest("<event name=\"startNextAgent\"><agent>radio</agent></event>");

    return true;
}

bool
VR_VoiceBoxAgentAudio::TuneRadioCatch(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.GetPromptByScreenId("VR-HNT-01", m_strPrompt);

    m_engineCallback.OnRequest("<event name=\"startNextAgent\"><agent>radio</agent></event>");

    return true;
}

bool
VR_VoiceBoxAgentAudio::TuneFrequencyBandAM(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.GetPromptByScreenId("VR-HNT-01", m_strPrompt);

    m_engineCallback.OnRequest("<event name=\"startNextAgent\"><agent>radio</agent></event>");

    return true;
}

bool
VR_VoiceBoxAgentAudio::TuneFrequencyBandFM(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.GetPromptByScreenId("VR-HNT-01", m_strPrompt);

    m_engineCallback.OnRequest("<event name=\"startNextAgent\"><agent>radio</agent></event>");

    return true;
}

bool
VR_VoiceBoxAgentAudio::TunePresetCatch(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    BuildScreenDisplayAll("na_media_radio_speak_preset_number", "VR-RAD-04");

    return true;
}

bool
VR_VoiceBoxAgentAudio::TuneFMGenreCatch(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    BuildScreenDisplayAll("na_media_radio_speak_radio_frequency", "VR-RAD-03");

    return true;
}

bool
VR_VoiceBoxAgentAudio::TuneSatelliteGenreCatch(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    BuildScreenDisplayAll("na_media_radio_speak_radio_frequency", "VR-RAD-03");

    return true;
}

bool
VR_VoiceBoxAgentAudio::TuneFrequencyBandSatellite(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.GetPromptByScreenId("VR-HNT-01", m_strPrompt);

    m_engineCallback.OnRequest("<event name=\"startNextAgent\"><agent>radio</agent></event>");

    return true;
}

bool
VR_VoiceBoxAgentAudio::TuneFrequencyCatch(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.GetPromptByScreenId("VR-HNT-01", m_strPrompt);

    m_engineCallback.OnRequest("<event name=\"startNextAgent\"><agent>radio</agent></event>");

    return true;
}

bool
VR_VoiceBoxAgentAudio::ActivateRadio( // ?
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("ActivateRadio: %s", xmlMessage.Get());

    std::string strFrequencyBand = GetActionParaByName(pcMsg, "Frequency Band");
    // std::string strStationFrequency = GetActionParaByName(pcMsg, "Station Frequency");

    std::string strOp;
    if ("FM" == strFrequencyBand) {
        strOp = "tuneLastFM";
    }
    else if ("AM" == strFrequencyBand) {
        strOp = "tuneLastAM";
    }
    else if ("Satellite" == strFrequencyBand) {
        strOp = "tuneLastSatellite";
    }
    else {

    }

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("media", strOp);
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio::ChangeHDSubChannel(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("ChangeHDSubChannel: %s", xmlMessage.Get());

    std::string strHDSubchannel = GetActionParaByName(pcMsg, "HD Sub-channel");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("media", "changeHDSubChannel");

    (void)xmlBulder.buildGivenElement(node, "subchannelNum", strHDSubchannel, "", "");

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

void
VR_VoiceBoxAgentAudio::TuneFrequency(
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
VR_VoiceBoxAgentAudio::TuneAMFrequency(
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
VR_VoiceBoxAgentAudio::TuneFMFrequency(
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
VR_VoiceBoxAgentAudio::TuneFMGenre(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("VR_VoiceBoxAgentAudio TuneFMGenre: %s", xmlMsg.Get());

    std::string strID = GetActionParaByName(pcMsg, "ID");
    std::string strGenre = GetActionParaByName(pcMsg, "Genre");

    PlayByGenre("FM", strGenre, strID);

    return true;
}

bool
VR_VoiceBoxAgentAudio::TuneSatelliteChannelName(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("TuneSatelliteChannelName: %s", xmlMsg.Get());

    std::string strID = GetActionParaByName(pcMsg, "ID");
    std::string strName = GetActionParaByName(pcMsg, "Name");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("media", "playByName");

    (void)xmlBulder.buildGivenElement(node, "band", "XM", "", "");

    (void)xmlBulder.buildGivenElement(node, "name", strName, "", "");
    (void)xmlBulder.buildGivenElement(node, "id", strID, "", "");

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio::TuneSatelliteChannelNumber(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("TuneSatelliteChannelNumber: %s", xmlMsg.Get());

    std::string strNumber = GetActionParaByName(pcMsg, "Number");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("media", "tuneSatelliteChannelNumber");

    (void)xmlBulder.buildGivenElement(node, "subchannelNum", strNumber, "", "");

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

void
VR_VoiceBoxAgentAudio::PlayByGenre(const std::string& strBand, const std::string& strGenre,
    const std::string& strID)
{
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("media", "playByGenre");

    (void)xmlBulder.buildGivenElement(node, "band", strBand, "", "");
    (void)xmlBulder.buildGivenElement(node, "genre", strGenre, "", "");
    (void)xmlBulder.buildGivenElement(node, "id", strID, "", "");

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

}

bool
VR_VoiceBoxAgentAudio::TuneSatelliteGenre(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("TuneSatelliteGenre: %s", xmlMsg.Get());

    std::string strID = GetActionParaByName(pcMsg, "ID");
    std::string strGenre = GetActionParaByName(pcMsg, "Genre");

    PlayByGenre("XM", strGenre, strID);

    return true;
}

bool
VR_VoiceBoxAgentAudio::VBTSendCommand(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    return false;
}

bool
VR_VoiceBoxAgentAudio::TuneFMHDFrequency(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    std::string strStationFrequency = GetActionParaByName(pcMsg, "Station Frequency");
    std::string strHDSubchannel = GetActionParaByName(pcMsg, "HD Sub-channel");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("media", "tuneFMHDFrequency");
    (void)xmlBulder.buildGivenElement(node, "frequency", strStationFrequency, "", "");
    (void)xmlBulder.buildGivenElement(node, "subchannelNum", strHDSubchannel, "", "");

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio::TunePreset(
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

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("media", "playByPreset");
    (void)xmlBulder.buildGivenElement(node, "number", strPreset, "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio::TuneFrequencyBandSatelliteStation(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    // std::string strStationName = GetActionParaByName(pcMsg, "Station Name");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("media", "tuneLastSatellite");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio::TuneFrequencyBandAMFrequency(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    // std::string strStationFrequency = GetActionParaByName(pcMsg, "Station Frequency");

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("media", "tuneLastAM");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio::TuneFrequencyBandFMFrequency(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("TuneFrequencyBandFMFrequency: %s", xmlMsg.Get());

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("media", "tuneLastFM");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio::RadioNBest(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMsg;
    (void)pcMsg->GetString(&xmlMsg);
    VR_LOG("RadioNBest: %s", xmlMsg.Get());

    VR_VoiceBoxXmlBuilder xmlBulder;

    CVECIOutStr strCommand;
    (void)pcMsg->GetValueByKey("Message.List.Execution.Parameter.Command.value", &strCommand);
    std::string strContent;
    if (NULL != strCommand.Get()) {
        VoiceMap<std::string, std::string>::iterator iter = m_radioCommandtoContent.find(strCommand.Get());
        if (m_radioCommandtoContent.end() != iter) {
            strContent = iter->second;
        }
    }

    pugi::xml_node xmlNode = BuildScreenDisplayFront(xmlBulder, strContent.c_str(), "");
    VBT_ULONG uSize = 0;
    (void)pcMsg->GetListSize(&uSize);
    uSize = (uSize > 5) ? 5 : uSize;
    pugi::xml_node nodeSelectList = xmlBulder.buildGivenElement(xmlNode, "selectList", "", "", "");
    pugi::xml_node nodeList = xmlBulder.buildGivenElement(nodeSelectList, "list", "", "", "");
    pugi::xml_node nodeHeader = xmlBulder.buildGivenElement(nodeList, "header", "", "", "");

    std::ostringstream oss;
    oss<<uSize;
    std::string strCount = oss.str();

    (void)xmlBulder.buildGivenElement(nodeHeader, "pageSize", "5", "", "");
    (void)xmlBulder.buildGivenElement(nodeHeader, "startIndex", "0", "", "");
    (void)xmlBulder.buildGivenElement(nodeHeader, "count", strCount, "", "");

    pugi::xml_node nodeItems = xmlBulder.buildGivenElement(nodeList, "items", "", "", "");

    std::string strFormalName;
    std::string strID;
    std::string strAlias;

    if ("na_media_radio_genre_select" == strContent) {
        strID = "Genre ID";
        strAlias = "Genre Alias";
        strFormalName = "Genre Formal Name";
    }
    else if ("na_media_radio_satellite_channel_select" == strContent) {
        strID = "Satellite Channel ID";
        strAlias = "Satellite Channel Alias";
        strFormalName = "Satellite Channel Formal Name";
    }

    for (VBT_ULONG i = 0; i < uSize; ++i) {
        CVECIOutStr strIDValue;
        CVECIOutStr strAliasValue;
        CVECIOutStr strFormalNameValue;

        (void)pcMsg->GetListItemParameterValue(i, strID.c_str(), &strIDValue);
        (void)pcMsg->GetListItemParameterValue(i, strAlias.c_str(), &strAliasValue);
        (void)pcMsg->GetListItemParameterValue(i, strFormalName.c_str(), &strFormalNameValue);

        VoiceVector<StructNode>::type attributeVector;
        StructNode node;

        node.Name = "formal_name";
        node.Value = strFormalNameValue;
        attributeVector.push_back(node);

        node.Name = "alias";
        node.Value = strAliasValue;
        attributeVector.push_back(node);

        node.Name = "id";
        node.Value = strIDValue;
        attributeVector.push_back(node);

        xmlBulder.buildListItemChildElement(nodeItems, "radioInfo", radioInfo, attributeVector);
    }

    m_engineCallback.GetPromptByScreenId("VR-SYS-01", m_strPrompt);

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentAudio::TaskComplete(
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
VR_VoiceBoxAgentAudio::AudioBack(
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
VR_VoiceBoxAgentAudio::AudioCancel(
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
VR_VoiceBoxAgentAudio::AudioHelp(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentAudio::AudioStartOver(
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
VR_VoiceBoxAgentAudio::AudioStartPause(
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
VR_VoiceBoxAgentAudio::AudioStartRepeat(
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
VR_VoiceBoxAgentAudio::EscalatingError(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.OnRequest("<event name=\"escalating-error\"></event>");
    return true;
}

/* EOF */
