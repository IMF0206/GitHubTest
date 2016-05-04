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
 * @file VR_VoiceBoxPhoneData.h
 * @brief interface for EngineWrapper callback.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VOICEBOXPHONEDATA_H
#define VR_VOICEBOXPHONEDATA_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

/* Suntec Header */
#ifndef VR_VOICEBOXXMLPARSER_H
#    include "VR_VoiceBoxXmlParser.h"
#endif

class sqlite3;

/**
 * @brief The VR_VoiceBoxPhoneData class
 *
 * class declaration
 */
class VR_VoiceBoxPhoneData
{
public:
    VR_VoiceBoxPhoneData();
    // message
    virtual ~VR_VoiceBoxPhoneData();

    void GetPhoneBookInfo(VoiceVector<PersonDetailInfo>::type& personDetailInfo);
    void OpenPhoneBookDB(const std::string strPath, VoiceVector<PersonDetailInfo>::type& personDetailInfo);
    std::string GetPhoneType(const std::string& contactID, const std::string& phoneNumber, const std::string& phoneType, const std::string& strPath);

private:
    sqlite3 * m_dbHandler;
    int m_dbResult;
    static int GetPhoneBookInfoCallback(void *personDetailInfo, int columnNum, char **columnValue, char **columnName);
    static int GetPhoneTypeCallback(void *phoneType, int columnNum, char **columnValue, char **columnName);
    static std::string GetPhoneTypeID(const std::string typeID, const std::string indexID);
    static std::string GetPhoneTypeToUI(const std::string typeID, const std::string indexID);
};

#endif // VR_VOICEBOXPHONEDATA_H
/* EOF */
