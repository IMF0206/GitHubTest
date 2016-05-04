/**
 * Copyright @ 2014 - 2017 Suntec Software(Shanghai) Co., Ltd.
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
 * @file VR_DataAccessorMedia.h
 * @brief Declaration file of VR_DataAccessorMedia.
 *
 * This file includes the declaration of VR_DataAccessorMedia.
 *
 * @attention used for C++ only.
 */

#ifndef VR_DATA_ACCESSOR_MEDIA_H
#define VR_DATA_ACCESSOR_MEDIA_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include "VR_DataAccessor.h"

#define VR_OPERATION_GET_PLAYLISTS                  "_getPlayLists"
#define VR_OPERATION_GET_PLAYLIST_SONGS_NUMBER      "_getPlaylistSongsNumber"
#define VR_OPERATION_GET_RADIO_STATIONS             "_getRadioStations"
#define VR_OPERATION_GET_MUSIC_BY_TYPE              "_getMusicByType"
#define VR_OPERATION_SELECT_SONGS                   "_selectSongs"
#define VR_OPERATION_QUERY_ONESHOT_NAME             "_queryOneshotName"
#define VR_OPERATION_QUERY_ONESHOT_DETAIL           "_queryOneshotDetail"
#define VR_OPERATION_QUERY_GRAMMAR_UPDATE_STATUS    "_queryGrammarUpdateStatus"


class sqlite3;

/**
 * @brief The VR_DataAccessorMedia class
 *
 * provide interface for query media info
 */

class VR_DataAccessorMedia : public VR_DataAccessor
{
public:
    VR_DataAccessorMedia();
    virtual ~VR_DataAccessorMedia() {}

    bool getInfo(const std::string &operation, const std::string &reqMsg, std::string &response) override;
    bool isOperationHandled(const std::string &operation) override;

    void openDB(const std::string &path);
    void closeDB();

    void setGrammarUpdateStatus(const std::string &status);

    std::string getSongNumber();
    std::string getArtistNumber();
    std::string getAlbumNumber();
    std::string getPlaylistNumber();

// protected:
//     void requestService(const std::string &agent, const std::string &reqName) override;

private:
    enum class MediaOperation
    {
        None,
        GetPlaylists,
        GetPlaylistSongsNumber,
        QueryGrammarUpdateStatus,
        GetRadioStations,
        GetMusicByType,
        SelectSongs,
        QueryOneshotName,
        QueryOneshotDetail
    };

    sqlite3 * m_dbHandler;
    int m_dbResult;
    std::string m_selectedGenreID;
    std::string m_selectedArtistID;
    std::string m_selectedAlbumID;

    std::string m_grammarUpdateStatus;

    VoiceMap<std::string, MediaOperation>::type m_operationMap;

    bool getPlaylists(std::string &response);
    bool getPlaylistSongsNumber(const std::string &reqMsg, std::string &response);
    bool queryGrammarUpdateStatus(const std::string &reqMsg, std::string &response);
    // for EU begin
    bool getRadioStations(std::string &response);
    bool getMusicByType(const std::string &reqMsg, std::string &response);
    bool selectSongs(const std::string &reqMsg);
    bool queryOneshotName(const std::string &reqMsg, std::string &response);
    bool queryOneshotDetail(const std::string &reqMsg, std::string &response);
    // for EU end

    std::string getRecordCount(const std::string &tableName);

    // sqlite3 callback
    static int getPlaylistsCallback(void *itemsNode, int columnNum, char **columnValue, char **columnName);
    static int getRecordCountCallback(void *number, int columnNum, char **columnValue, char **columnName);
    // for EU begin
    static int getMusicByTypeCallback(void *itemsNode, int columnNum, char **columnValue, char **columnName);
    static int queryOneshotDetailCallback(void *itemsNode, int columnNum, char **columnValue, char **columnName);
    // for EU end
};

#endif /* VR_DATA_ACCESSOR_MEDIA_H */
/* EOF */