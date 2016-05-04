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

#include "VR_DataAccessorMedia.h"
#include "VR_Log.h"

#include "MEM_list.h"

#include <pugixml.hpp>
#include <sqlite3.h>
#include <sstream>

#define VR_MSG_MEDIA_PLAYLIST_ID    "id"
#define VR_MSG_MEDIA_GENRE_ID       "genreId"
#define VR_MSG_MEDIA_ARTIST_ID      "artistId"
#define VR_MSG_MEDIA_ALBUM_ID       "albumId"
#define VR_MSG_MEDIA_TYPE           "type"
#define VR_MSG_MEDIA_TYPE_ALL       "ALL"
#define VR_MSG_MEDIA_TYPE_GENRE     "GENRE"
#define VR_MSG_MEDIA_TYPE_ARTIST    "ARTIST"
#define VR_MSG_MEDIA_TYPE_ALBUM     "ALBUM"
#define VR_MSG_MEDIA_TYPE_PLAYLIST  "PLAYLIST"
#define VR_MSG_MEDIA_TYPE_SONG      "SONG"

#define VR_MSG_MEDIA_ONESHOT_ARTIST     "artist"
#define VR_MSG_MEDIA_ONESHOT_ARTIST_ID  "id"

#define VR_MSG_RESPONSE_NODENAME                    "data"
#define VR_MSG_RESPONSE_NUMBER                      "number"
#define VR_MSG_RESPONSE_LIST                        "list"
#define VR_MSG_RESPONSE_LIST_ID                     "id"
#define VR_MSG_RESPONSE_LIST_ID_PREFIX              "list_"
#define VR_MSG_RESPONSE_LIST_HEADER                 "header"
#define VR_MSG_RESPONSE_LIST_HEADER_PAGESIZE        "pageSize"
#define VR_MSG_RESPONSE_LIST_HEADER_STARTINDEX      "startIndex"
#define VR_MSG_RESPONSE_LIST_HEADER_COUNT           "count"
#define VR_MSG_RESPONSE_LIST_ITEMS                  "items"
#define VR_MSG_RESPONSE_LIST_ITEMS_ITEM             "item"

#define VR_MSG_RESPONSE_MUSIC_INFO                  "musicInfo"
#define VR_MSG_RESPONSE_MUSIC_INFO_NAME             "name"
#define VR_MSG_RESPONSE_MUSIC_INFO_TYPE_NAME        "typeName"
#define VR_MSG_RESPONSE_MUSIC_INFO_SOURCE_ID        "sourceId"
#define VR_MSG_RESPONSE_MUSIC_INFO_ID               "id"
#define VR_MSG_RESPONSE_MUSIC_INFO_TEMP             "type"
#define VR_MSG_RESPONSE_MUSIC_INFO_PLAYTYPE         "playType"
#define VR_MSG_RESPONSE_MUSIC_INFO_PLAYTYPE_SONG    "PLAY_SONG"
#define VR_MSG_RESPONSE_MUSIC_INFO_PLAYTYPE_ALBUM   "PLAY_ALBUM"

#define VR_MSG_RESPONSE_STATION         "station"
#define VR_MSG_RESPONSE_STATION_NAME    "name"
#define VR_MSG_RESPONSE_STATION_BAND    "band"
#define VR_MSG_RESPONSE_STATION_ID      "id"

#define VR_MEDIA_MAX_PLAYLIST   25
#define VR_MEDIA_MAX_MUSICINFO  12

VR_DataAccessorMedia::VR_DataAccessorMedia()
    : m_dbHandler(nullptr)
{
    m_operationMap[VR_OPERATION_GET_PLAYLISTS] = MediaOperation::GetPlaylists;
    m_operationMap[VR_OPERATION_GET_PLAYLIST_SONGS_NUMBER] = MediaOperation::GetPlaylistSongsNumber;
    m_operationMap[VR_OPERATION_QUERY_GRAMMAR_UPDATE_STATUS] = MediaOperation::QueryGrammarUpdateStatus;
    // for EU begin
    m_operationMap[VR_OPERATION_GET_RADIO_STATIONS] = MediaOperation::GetRadioStations;
    m_operationMap[VR_OPERATION_GET_MUSIC_BY_TYPE] = MediaOperation::GetMusicByType;
    m_operationMap[VR_OPERATION_SELECT_SONGS] = MediaOperation::SelectSongs;
    m_operationMap[VR_OPERATION_QUERY_ONESHOT_NAME] = MediaOperation::QueryOneshotName;
    m_operationMap[VR_OPERATION_QUERY_ONESHOT_DETAIL] = MediaOperation::QueryOneshotDetail;
    // for EU end
}

bool VR_DataAccessorMedia::getInfo(const std::string &operation, const std::string &reqMsg, std::string &response)
{
    VR_LOGD_FUNC();
    VR_LOGD("Operation [%s]\nreqMsg [%s]", operation.c_str(), reqMsg.c_str());
    MediaOperation op = MediaOperation::None;
    if (m_operationMap.end() != m_operationMap.find(operation)) {
        op = m_operationMap[operation];
    }
    switch (op) {
    case MediaOperation::GetPlaylists:
        {
            return getPlaylists(response);
        }
    case MediaOperation::GetPlaylistSongsNumber:
        {
            return getPlaylistSongsNumber(reqMsg, response);
        }
    case MediaOperation::QueryGrammarUpdateStatus:
        {
            return queryGrammarUpdateStatus(reqMsg, response);
        }
    // for EU begin
    case MediaOperation::GetRadioStations:
        {
            return getRadioStations(response);
        }
    case MediaOperation::GetMusicByType:
        {
            return getMusicByType(reqMsg, response);
        }
    case MediaOperation::SelectSongs:
        {
            return selectSongs(reqMsg);
        }
    case MediaOperation::QueryOneshotName:
        {
            return queryOneshotName(reqMsg, response);
        }
    case MediaOperation::QueryOneshotDetail:
        {
            return queryOneshotDetail(reqMsg, response);
        }
    // for EU end
    default:
        {
            return false; // can not find the operation
        }
    }
}

bool VR_DataAccessorMedia::isOperationHandled(const std::string &operation)
{
    return (m_operationMap.end() != m_operationMap.find(operation));
}

void VR_DataAccessorMedia::openDB(const std::string &path)
{
    m_dbResult = sqlite3_open_v2(path.c_str(), &m_dbHandler, SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX, NULL);

    if (SQLITE_OK != m_dbResult) {
        closeDB();
        VR_ERROR("Open Musice DB file %s failed, resultID: [%d]", path.c_str(), m_dbResult);
    }
}

void VR_DataAccessorMedia::closeDB()
{
    sqlite3_close(m_dbHandler);
    m_dbResult = SQLITE_ERROR;
    m_dbHandler = nullptr;
}

void VR_DataAccessorMedia::setGrammarUpdateStatus(const std::string &status)
{
    m_grammarUpdateStatus = status;
}

std::string VR_DataAccessorMedia::getSongNumber()
{
    return getRecordCount("MusicAgentSongs");
}

std::string VR_DataAccessorMedia::getArtistNumber()
{
    return getRecordCount("MusicAgentArtists");
}

std::string VR_DataAccessorMedia::getAlbumNumber()
{
    return getRecordCount("MusicAgentAlbums");
}

std::string VR_DataAccessorMedia::getPlaylistNumber()
{
    return getRecordCount("MusicAgentPlaylists");
}

// void VR_DataAccessorMedia::requestService(const std::string &agent, const std::string &reqName)
// {

// }

// count MAX return to script 25, to ASR full
bool VR_DataAccessorMedia::getPlaylists(std::string &response)
{
    VR_LOGD_FUNC();

    // create response message
    pugi::xml_document responseDoc;
    responseDoc.load_string("");
    pugi::xml_node responseNode = responseDoc.append_child(VR_MSG_RESPONSE_NODENAME);
    responseNode.append_child(VR_MSG_RESPONSE_LIST).append_attribute(VR_MSG_RESPONSE_LIST_ID).set_value((std::string(VR_MSG_RESPONSE_LIST_ID_PREFIX) + VR_OPERATION_GET_PLAYLISTS).c_str());
    responseNode.child(VR_MSG_RESPONSE_LIST).append_child(VR_MSG_RESPONSE_LIST_HEADER);
    responseNode.child(VR_MSG_RESPONSE_LIST).append_child(VR_MSG_RESPONSE_LIST_ITEMS);

    pugi::xml_node headerNode = responseNode.child(VR_MSG_RESPONSE_LIST).child(VR_MSG_RESPONSE_LIST_HEADER);
    headerNode.append_child(VR_MSG_RESPONSE_LIST_HEADER_PAGESIZE).text().set("5");
    headerNode.append_child(VR_MSG_RESPONSE_LIST_HEADER_STARTINDEX).text().set("0");
    headerNode.append_child(VR_MSG_RESPONSE_LIST_HEADER_COUNT).text().set("0");

    pugi::xml_node itemsNode = responseNode.child(VR_MSG_RESPONSE_LIST).child(VR_MSG_RESPONSE_LIST_ITEMS);

    std::ostringstream oss;
    responseNode.print(oss);
    response = oss.str();
    oss.str("");

    // query database
    if (SQLITE_OK == m_dbResult) {
        std::string sqlRequest("SELECT cName,nSourceId,nExternalId FROM MusicAgentPlaylists");
        char * errmsg = NULL;
        int result = sqlite3_exec(m_dbHandler, sqlRequest.c_str(), getPlaylistsCallback, &itemsNode, &errmsg);
        if (SQLITE_OK != result) {
            VR_ERROR("Run SQL request [%s] failed, error code: [%d], error msg: [%s]", sqlRequest.c_str(), result, errmsg);
            return false;
        }
    }
    else {
        return false;
    }

    // generate response
    int playlistCount = 0;
    pugi::xml_node itemNode = itemsNode.first_child();
    while (!itemNode.empty()) {
        pugi::xml_node currentNode = itemNode;
        itemNode = itemNode.next_sibling();
        if (playlistCount < VR_MEDIA_MAX_PLAYLIST) {
            ++playlistCount;
        }
        else {
            itemsNode.remove_child(currentNode);
        }
    }

    oss << playlistCount;
    std::string playlistCountStr = oss.str();
    oss.str("");
    headerNode.child(VR_MSG_RESPONSE_LIST_HEADER_COUNT).text().set(playlistCountStr.c_str());

    responseNode.print(oss);
    response = oss.str();
    VR_LOGD("response msg is [%s]", response.c_str());

    return true;
}

bool VR_DataAccessorMedia::getPlaylistSongsNumber(const std::string &reqMsg, std::string &response)
{
    VR_LOGD_FUNC();

    // create response message
    pugi::xml_document responseDoc;
    responseDoc.load_string("");
    pugi::xml_node responseNode = responseDoc.append_child(VR_MSG_RESPONSE_NODENAME);
    responseNode.append_child(VR_MSG_RESPONSE_NUMBER).text().set("0");

    std::ostringstream oss;
    responseNode.print(oss);
    response = oss.str();
    oss.str("");

    // process request message
    VR_LOGD("reqMsg [%s]", reqMsg.c_str());
    pugi::xml_document msgDoc;
    msgDoc.load_string(reqMsg.c_str());
    std::string playlistID = msgDoc.select_node((std::string("//") + VR_MSG_MEDIA_PLAYLIST_ID).c_str()).node().text().as_string();

    // query database
    int number = 0;
    if (SQLITE_OK == m_dbResult) {
        std::string sqlRequest(std::string("SELECT count(*) FROM MusicAgentPlaylistsSongs WHERE nPlaylistId = ") + playlistID);
        char * errmsg = NULL;
        int result = sqlite3_exec(m_dbHandler, sqlRequest.c_str(), getRecordCountCallback, &number, &errmsg);
        if (SQLITE_OK != result) {
            VR_ERROR("Run SQL request [%s] failed, error code: [%d], error msg: [%s]", sqlRequest.c_str(), result, errmsg);
            return false;
        }
    }
    else {
        return false;
    }

    // generate response
    oss << number;
    std::string numberStr = oss.str();
    oss.str("");

    responseNode.child(VR_MSG_RESPONSE_NUMBER).text().set(numberStr.c_str());
    responseNode.print(oss);
    response = oss.str();

    return true;
}

bool VR_DataAccessorMedia::queryGrammarUpdateStatus(const std::string &reqMsg, std::string &response)
{
    pugi::xml_document responseDoc;
    responseDoc.load_string("");
    pugi::xml_node responseNode = responseDoc.append_child("status");
    responseNode.text().set(m_grammarUpdateStatus.c_str());

    std::ostringstream oss;
    responseNode.print(oss);
    response = oss.str();
    return true;
}

// for EU begin

// count MAX return to script 12, to ASR full
bool VR_DataAccessorMedia::getRadioStations(std::string &response)
{
    VR_LOGD_FUNC();
    // TODO: for EU

    return true;
}

// count MAX return to script 12, to ASR full
bool VR_DataAccessorMedia::getMusicByType(const std::string &reqMsg, std::string &response)
{
    VR_LOGD_FUNC();

    // create response message
    pugi::xml_document responseDoc;
    responseDoc.load_string("");
    pugi::xml_node responseNode = responseDoc.append_child(VR_MSG_RESPONSE_NODENAME);
    responseNode.append_child(VR_MSG_RESPONSE_LIST).append_attribute(VR_MSG_RESPONSE_LIST_ID).set_value((std::string(VR_MSG_RESPONSE_LIST_ID_PREFIX) + VR_OPERATION_GET_MUSIC_BY_TYPE).c_str());
    responseNode.child(VR_MSG_RESPONSE_LIST).append_child(VR_MSG_RESPONSE_LIST_HEADER);
    responseNode.child(VR_MSG_RESPONSE_LIST).append_child(VR_MSG_RESPONSE_LIST_ITEMS);

    pugi::xml_node headerNode = responseNode.child(VR_MSG_RESPONSE_LIST).child(VR_MSG_RESPONSE_LIST_HEADER);
    headerNode.append_child(VR_MSG_RESPONSE_LIST_HEADER_PAGESIZE).text().set("5");
    headerNode.append_child(VR_MSG_RESPONSE_LIST_HEADER_STARTINDEX).text().set("0");
    headerNode.append_child(VR_MSG_RESPONSE_LIST_HEADER_COUNT).text().set("0");

    pugi::xml_node itemsNode = responseNode.child(VR_MSG_RESPONSE_LIST).child(VR_MSG_RESPONSE_LIST_ITEMS);

    std::ostringstream oss;
    responseNode.print(oss);
    response = oss.str();
    oss.str("");

    // process request message
    VR_LOGD("reqMsg [%s]", reqMsg.c_str());
    pugi::xml_document msgDoc;
    msgDoc.load_string(reqMsg.c_str());
    std::string selectType = msgDoc.select_node((std::string("//") + VR_MSG_MEDIA_TYPE).c_str()).node().text().as_string();

    // query database
    std::string sqlRequest;
    if (VR_MSG_MEDIA_TYPE_GENRE == selectType) {
        sqlRequest.assign((m_selectedArtistID.empty() ? "" : (std::string("SELECT nGenreId FROM MusicAgentGenresArtists WHERE nArtistId=\"") + m_selectedArtistID + "\")"))
                           + (m_selectedAlbumID.empty() ? "" : (std::string("INTERSECT SELECT nGenreId FROM MusicAgentGenresAlbums WHERE nAlbumId=\"") + m_selectedAlbumID + "\"")));
        if (std::size_t(0) == sqlRequest.find("INTERSECT")) {
            sqlRequest.erase(0, 10);
        }
        sqlRequest.assign(std::string("SELECT cName,nSourceId,nExternalId FROM MusicAgentGenres") + (sqlRequest.empty() ? "" : (std::string(" WHERE nExternalId in (") + sqlRequest + ")")));
    }
    else if (VR_MSG_MEDIA_TYPE_ARTIST == selectType) {
        sqlRequest.assign((m_selectedGenreID.empty() ? "" : (std::string("SELECT nArtistId FROM MusicAgentGenresArtists WHERE nGenreId=\"") + m_selectedGenreID + "\")"))
                           + (m_selectedAlbumID.empty() ? "" : (std::string("INTERSECT SELECT nArtistId FROM MusicAgentAlbumsArtists WHERE nAlbumId=\"") + m_selectedAlbumID + "\"")));
        if (std::size_t(0) == sqlRequest.find("INTERSECT")) {
            sqlRequest.erase(0, 10);
        }
        sqlRequest.assign(std::string("SELECT cName,nSourceId,nExternalId FROM MusicAgentArtists") + (sqlRequest.empty() ? "" : (std::string(" WHERE nExternalId in (") + sqlRequest + ")")));
    }
    else if (VR_MSG_MEDIA_TYPE_ALBUM == selectType) {
        sqlRequest.assign((m_selectedGenreID.empty() ? "" : (std::string("SELECT nAlbumId FROM MusicAgentGenresAlbums WHERE nGenreId=\"") + m_selectedGenreID + "\")"))
                           + (m_selectedArtistID.empty() ? "" : (std::string("INTERSECT SELECT nAlbumId FROM MusicAgentAlbumsArtists WHERE nArtistId=\"") + m_selectedArtistID + "\"")));
        if (std::size_t(0) == sqlRequest.find("INTERSECT")) {
            sqlRequest.erase(0, 10);
        }
        sqlRequest.assign(std::string("SELECT cName,nSourceId,nExternalId FROM MusicAgentAlbums") + (sqlRequest.empty() ? "" : (std::string(" WHERE nExternalId in (") + sqlRequest + ")")));
    }
    else if (VR_MSG_MEDIA_TYPE_SONG == selectType) {
        // sqlRequest.assign((m_selectedArtistID.empty() ? "" : (std::string("SELECT nSongId FROM MusicAgentArtistsSongs WHERE nArtistId=\"") + m_selectedArtistID + "\")"))
        //                    + (m_selectedGenreID.empty() ? "" : (std::string("INTERSECT SELECT nSongId FROM MusicAgentGenresSongs WHERE nGenreId=\"") + m_selectedGenreID + "\")"))
        //                    + (m_selectedAlbumID.empty() ? "" : (std::string("INTERSECT SELECT nSongId FROM MusicAgentAlbumsSongs WHERE nAlbumId=\"") + m_selectedAlbumID + "\"")));
        // if (std::size_t(0) == sqlRequest.find("INTERSECT")) {
        //     sqlRequest.erase(0, 10);
        // }
        // sqlRequest.assign(std::string("SELECT cName,nSourceId,nExternalId FROM MusicAgentSongs") + sqlRequest.empty() ? "" : (std::string(" WHERE nExternalId in (") + sqlRequest + ")"));
        sqlRequest.assign("SELECT cName,nSourceId,nExternalId FROM MusicAgentSongs");
    }
    else if (VR_MSG_MEDIA_TYPE_PLAYLIST == selectType) {
        sqlRequest.assign("SELECT cName,nSourceId,nExternalId FROM MusicAgentPlaylists");
    }
    else {
        VR_ERROR("Invalid type to query");
        return false;
    }

    if (SQLITE_OK == m_dbResult) {
        char * errmsg = NULL;
        itemsNode.append_attribute(VR_MSG_RESPONSE_MUSIC_INFO_TEMP).set_value(selectType.c_str());
        int result = sqlite3_exec(m_dbHandler, sqlRequest.c_str(), getMusicByTypeCallback, &itemsNode, &errmsg);
        if (SQLITE_OK != result) {
            VR_ERROR("Run SQL request [%s] failed, error code: [%d], error msg: [%s]", sqlRequest.c_str(), result, errmsg);
            return false;
        }
        itemsNode.remove_attribute(VR_MSG_RESPONSE_MUSIC_INFO_TEMP);
    }
    else {
        return false;
    }

    // generate response
    int playlistCount = 0;
    pugi::xml_node itemNode = itemsNode.first_child();
    while (!itemNode.empty()) {
        pugi::xml_node currentNode = itemNode;
        itemNode = itemNode.next_sibling();
        if (playlistCount < VR_MEDIA_MAX_MUSICINFO) {
            ++playlistCount;
        }
        else {
            itemsNode.remove_child(currentNode);
        }
    }

    oss << playlistCount;
    std::string playlistCountStr = oss.str();
    oss.str("");
    headerNode.child(VR_MSG_RESPONSE_LIST_HEADER_COUNT).text().set(playlistCountStr.c_str());

    responseNode.print(oss);
    response = oss.str();
    VR_LOGD("response msg is [%s]", response.c_str());

    return true;
}

// count MAX return to script 12, to ASR full
bool VR_DataAccessorMedia::selectSongs(const std::string &reqMsg)
{
    VR_LOGD_FUNC();

    // process request message
    VR_LOGD("reqMsg [%s]", reqMsg.c_str());
    pugi::xml_document msgDoc;
    msgDoc.load_string(reqMsg.c_str());
    std::string selectType = msgDoc.select_node((std::string("//") + VR_MSG_MEDIA_TYPE).c_str()).node().text().as_string();

    if (VR_MSG_MEDIA_TYPE_ALL == selectType) {
        m_selectedGenreID.clear();
        m_selectedArtistID.clear();
        m_selectedAlbumID.clear();
    }
    else if (VR_MSG_MEDIA_TYPE_GENRE == selectType) {
        m_selectedGenreID = msgDoc.select_node((std::string("//") + VR_MSG_MEDIA_GENRE_ID).c_str()).node().text().as_string();
    }
    else if (VR_MSG_MEDIA_TYPE_ARTIST == selectType) {
        m_selectedArtistID = msgDoc.select_node((std::string("//") + VR_MSG_MEDIA_ARTIST_ID).c_str()).node().text().as_string();
    }
    else if (VR_MSG_MEDIA_TYPE_ALBUM == selectType) {
        m_selectedAlbumID = msgDoc.select_node((std::string("//") + VR_MSG_MEDIA_ALBUM_ID).c_str()).node().text().as_string();
    }
    else {
        VR_ERROR("Invalid type to select");
        return false;
    }

    // generate grammar
    return true;
}

bool VR_DataAccessorMedia::queryOneshotName(const std::string &reqMsg, std::string &response)
{
    VR_LOGD_FUNC();

    // process request message
    VR_LOGD("reqMsg [%s]", reqMsg.c_str());
    pugi::xml_document msgDoc;
    msgDoc.load_string(reqMsg.c_str());
    pugi::xml_node countNode = msgDoc.select_node((std::string("//") + VR_MSG_RESPONSE_LIST_HEADER_COUNT).c_str()).node();
    int countNum = atoi(countNode.text().as_string());
    pugi::xml_node itemsNode = msgDoc.select_node((std::string("//") + VR_MSG_RESPONSE_LIST_ITEMS).c_str()).node();
    pugi::xml_node itemNode = itemsNode.first_child();
    while (!itemNode.empty()) {
        pugi::xml_node currentNode = itemNode;
        itemNode = itemNode.next_sibling();
        if (currentNode.child(VR_MSG_MEDIA_ONESHOT_ARTIST).empty()) {
            itemsNode.remove_child(currentNode);
            --countNum;        m_selectedGenreID.clear();
        m_selectedArtistID.clear();
        m_selectedAlbumID.clear();
        }
    }
    std::string countStr;
    std::ostringstream oss;
    oss << countNum;
    countStr = oss.str();
    oss.str("");
    countNode.text().set(countStr.c_str());

    // create response message
    pugi::xml_document responseDoc;
    responseDoc.load_string("");
    pugi::xml_node responseNode = responseDoc.append_child(VR_MSG_RESPONSE_NODENAME);
    pugi::xml_node listNode = msgDoc.select_node((std::string("//") + VR_MSG_RESPONSE_LIST).c_str()).node();
    responseNode.append_copy(listNode);

    responseNode.print(oss);
    response = oss.str();
    VR_LOGD("response msg is [%s]", response.c_str());
    return true;
}

// count MAX return to script 12, to ASR full
bool VR_DataAccessorMedia::queryOneshotDetail(const std::string &reqMsg, std::string &response)
{
    VR_LOGD_FUNC();

    // create response message
    pugi::xml_document responseDoc;
    responseDoc.load_string("");
    pugi::xml_node responseNode = responseDoc.append_child(VR_MSG_RESPONSE_NODENAME);
    responseNode.append_child(VR_MSG_RESPONSE_LIST).append_attribute(VR_MSG_RESPONSE_LIST_ID).set_value((std::string(VR_MSG_RESPONSE_LIST_ID_PREFIX) + VR_OPERATION_QUERY_ONESHOT_DETAIL).c_str());
    responseNode.child(VR_MSG_RESPONSE_LIST).append_child(VR_MSG_RESPONSE_LIST_HEADER);
    responseNode.child(VR_MSG_RESPONSE_LIST).append_child(VR_MSG_RESPONSE_LIST_ITEMS);

    pugi::xml_node headerNode = responseNode.child(VR_MSG_RESPONSE_LIST).child(VR_MSG_RESPONSE_LIST_HEADER);
    headerNode.append_child(VR_MSG_RESPONSE_LIST_HEADER_PAGESIZE).text().set("5");
    headerNode.append_child(VR_MSG_RESPONSE_LIST_HEADER_STARTINDEX).text().set("0");
    headerNode.append_child(VR_MSG_RESPONSE_LIST_HEADER_COUNT).text().set("0");

    pugi::xml_node itemsNode = responseNode.child(VR_MSG_RESPONSE_LIST).child(VR_MSG_RESPONSE_LIST_ITEMS);

    std::ostringstream oss;
    responseNode.print(oss);
    response = oss.str();
    oss.str("");

    // process request message
    VR_LOGD("reqMsg [%s]", reqMsg.c_str());
    pugi::xml_document msgDoc;
    msgDoc.load_string(reqMsg.c_str());
    pugi::xml_node artistNode = msgDoc.select_node((std::string("//") + VR_MSG_MEDIA_ONESHOT_ARTIST).c_str()).node();
    std::string artistID = artistNode.child(VR_MSG_MEDIA_ONESHOT_ARTIST_ID).text().as_string();

    if (SQLITE_OK == m_dbResult) {
        char * errmsg = NULL;
        std::string sqlRequest;
        sqlRequest.assign(std::string("SELECT cName,nExternalId FROM MusicAgentAlbums WHERE nExternalId in (SELECT nAlbumId FROM MusicAgentAlbumsArtists WHERE nArtistId=\"") + artistID + "\")");
        itemsNode.append_attribute(VR_MSG_RESPONSE_MUSIC_INFO_TEMP).set_value(VR_MSG_RESPONSE_MUSIC_INFO_PLAYTYPE_ALBUM);
        int result = sqlite3_exec(m_dbHandler, sqlRequest.c_str(), queryOneshotDetailCallback, &itemsNode, &errmsg);
        if (SQLITE_OK != result) {
            VR_ERROR("Run SQL request [%s] failed, error code: [%d], error msg: [%s]", sqlRequest.c_str(), result, errmsg);
            return false;
        }
        sqlRequest.assign(std::string("SELECT cName,nExternalId FROM MusicAgentSongs WHERE nExternalId in (SELECT nSongId FROM MusicAgentArtistsSongs WHERE nArtistId=\"") + artistID + "\")");
        itemsNode.attribute(VR_MSG_RESPONSE_MUSIC_INFO_TEMP).set_value(VR_MSG_RESPONSE_MUSIC_INFO_PLAYTYPE_SONG);
        result = sqlite3_exec(m_dbHandler, sqlRequest.c_str(), queryOneshotDetailCallback, &itemsNode, &errmsg);
        if (SQLITE_OK != result) {
            VR_ERROR("Run SQL request [%s] failed, error code: [%d], error msg: [%s]", sqlRequest.c_str(), result, errmsg);
            return false;
        }
        itemsNode.remove_attribute(VR_MSG_RESPONSE_MUSIC_INFO_TEMP);
    }
    else {
        return false;
    }

    // generate response
    int playlistCount = 0;
    pugi::xml_node itemNode = itemsNode.first_child();
    while (!itemNode.empty()) {
        pugi::xml_node currentNode = itemNode;
        itemNode = itemNode.next_sibling();
        if (playlistCount < VR_MEDIA_MAX_MUSICINFO) {
            ++playlistCount;
        }
        else {
            itemsNode.remove_child(currentNode);
        }
    }

    oss << playlistCount;
    std::string playlistCountStr = oss.str();
    oss.str("");
    headerNode.child(VR_MSG_RESPONSE_LIST_HEADER_COUNT).text().set(playlistCountStr.c_str());

    responseNode.print(oss);
    response = oss.str();
    VR_LOGD("response msg is [%s]", response.c_str());

    return true;
}
// for EU end

std::string VR_DataAccessorMedia::getRecordCount(const std::string &tableName)
{
    int number = 0;
    if (SQLITE_OK == m_dbResult) {
        std::string sqlRequest(std::string("SELECT count(*) FROM ") + tableName);
        char * errmsg = NULL;
        int result = sqlite3_exec(m_dbHandler, sqlRequest.c_str(), getRecordCountCallback, &number, &errmsg);
        if (SQLITE_OK != result) {
            VR_ERROR("Run SQL request [%s] failed, error code: [%d], error msg: [%s]", sqlRequest.c_str(), result, errmsg);
            return "0";
        }
    }
    else {
        return "0";
    }
    std::ostringstream oss;
    oss << number;
    return oss.str();
}

int VR_DataAccessorMedia::getPlaylistsCallback(void *itemsNode, int columnNum, char **columnValue, char **columnName)
{
    if (columnValue[0] && columnValue[1] && columnValue[2]) {
        pugi::xml_node * itemsNodePoint = (pugi::xml_node *)itemsNode;
        pugi::xml_node musicInfoNode = itemsNodePoint->append_child(VR_MSG_RESPONSE_LIST_ITEMS_ITEM).append_child(VR_MSG_RESPONSE_MUSIC_INFO);
        musicInfoNode.append_child(VR_MSG_RESPONSE_MUSIC_INFO_NAME).text().set(columnValue[0]);
        musicInfoNode.append_child(VR_MSG_RESPONSE_MUSIC_INFO_SOURCE_ID).text().set(columnValue[1]);
        musicInfoNode.append_child(VR_MSG_RESPONSE_MUSIC_INFO_ID).text().set(columnValue[2]);
    }
    return 0;
}

int VR_DataAccessorMedia::getRecordCountCallback(void *number, int columnNum, char **columnValue, char **columnName)
{
    if (columnValue[0]) {
        *(int *)number = atoi(columnValue[0]);
    }
    return 0;
}

// for EU
int VR_DataAccessorMedia::getMusicByTypeCallback(void *itemsNode, int columnNum, char **columnValue, char **columnName)
{
    if (columnValue[0] && columnValue[1] && columnValue[2]) {
        pugi::xml_node * itemsNodePoint = (pugi::xml_node *)itemsNode;
        pugi::xml_node musicInfoNode = itemsNodePoint->append_child(VR_MSG_RESPONSE_LIST_ITEMS_ITEM).append_child(VR_MSG_RESPONSE_MUSIC_INFO);
        musicInfoNode.append_child(VR_MSG_RESPONSE_MUSIC_INFO_NAME).text().set(columnValue[0]);
        musicInfoNode.append_child(VR_MSG_RESPONSE_MUSIC_INFO_TYPE_NAME).text().set(itemsNodePoint->attribute(VR_MSG_RESPONSE_MUSIC_INFO_TEMP).value());
        musicInfoNode.append_child(VR_MSG_RESPONSE_MUSIC_INFO_SOURCE_ID).text().set(columnValue[1]);
        musicInfoNode.append_child(VR_MSG_RESPONSE_MUSIC_INFO_ID).text().set(columnValue[2]);
    }
    return 0;
}

// for EU
int VR_DataAccessorMedia::queryOneshotDetailCallback(void *itemsNode, int columnNum, char **columnValue, char **columnName)
{
    if (columnValue[0] && columnValue[1]) {
        pugi::xml_node * itemsNodePoint = (pugi::xml_node *)itemsNode;
        pugi::xml_node musicInfoNode = itemsNodePoint->append_child(VR_MSG_RESPONSE_LIST_ITEMS_ITEM).append_child(VR_MSG_RESPONSE_MUSIC_INFO);
        musicInfoNode.append_child(VR_MSG_RESPONSE_MUSIC_INFO_NAME).text().set(columnValue[0]);
        musicInfoNode.append_child(VR_MSG_RESPONSE_MUSIC_INFO_PLAYTYPE).text().set(itemsNodePoint->attribute(VR_MSG_RESPONSE_MUSIC_INFO_TEMP).value());
        musicInfoNode.append_child(VR_MSG_RESPONSE_MUSIC_INFO_ID).text().set(columnValue[1]);
    }
    return 0;
}

/* EOF */
