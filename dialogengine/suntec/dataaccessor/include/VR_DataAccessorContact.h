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
 * @file VR_DataAccessorContact.h
 * @brief Declaration file of VR_DataAccessorContact.
 *
 * This file includes the declaration of VR_DataAccessorContact.
 *
 * @attention used for C++ only.
 */

#ifndef VR_DATA_ACCESSOR_CONTACT_H
#define VR_DATA_ACCESSOR_CONTACT_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include "VR_DataAccessor.h"

#define VR_OPERATION_CHECK_CONTACT_IN_PHONEBOOK "_checkContactInPhonebook"
#define VR_OPERATION_GET_PHONE_INFO             "_getPhoneInfo"
#define VR_OPERATION_BUILD_INFO_LIST            "_buildInfoList"
#define VR_OPERATION_GET_INFO_BY_TYPE           "_getInfoByType"
#define VR_OPERATION_GET_CONTACT_IDS            "getContactIds"

class sqlite3;

/**
 * @brief The VR_DataAccessorContact class
 *
 * provide interface for query contacts
 */

class VR_DataAccessorContact : public VR_DataAccessor
{
public:
    VR_DataAccessorContact();
    virtual ~VR_DataAccessorContact() {}

    bool getInfo(const std::string &operation, const std::string &reqMsg, std::string &response) override;
    bool isOperationHandled(const std::string &operation) override;

    void openDB(const std::string &path);
    void closeDB();

    int getContactCount();

// protected:
//     void requestService(const std::string &agent, const std::string &reqName) override;

private:
    enum class ContactOperation
    {
        None,
        CheckContactInPhonebook,
        GetPhoneInfo,
        BuildInfoList,
        GetInfoByType,
        GetContactIds
    };

    sqlite3 * m_dbHandler;
    int m_dbResult;
    bool m_isContactInPhonebook;
    VoiceMap<std::string, ContactOperation>::type m_operationMap;

    std::string m_phoneListStr; // used in buildInfoList and getInfoByType

    bool checkContactInPhonebook(const std::string &reqMsg, std::string &response);
    bool getPhoneInfo(const std::string &reqMsg, std::string &response);

    // filter screenList by phoneList
    bool buildInfoList(const std::string &reqMsg, std::string &response);

    // invoke after buildInfoList, need to save phoneInfoList of buildInfoList reqMsg
    bool getInfoByType(const std::string &reqMsg, std::string &response);

    // get contactId by fullname
    bool getContactIds(const std::string &reqMsg, std::string &response);

    // sqlite3 callback
    static int checkContactInPhonebookCallback(void *resultNode, int columnNum, char **columnValue, char **columnName);
    static int getPhoneInfoCallback(void *itemsNode, int columnNum, char **columnValue, char **columnName);
    static int getRecordCountCallback(void *number, int columnNum, char **columnValue, char **columnName);
    static int getContactIdsCallback(void *idListPoint, int columnNum, char **columnValue, char **columnName);

    // get phone type string used for script, used in buildInfoList and getInfoByType
    std::string getPhoneTypeString(int id);
};

#endif /* VR_DATA_ACCESSOR_CONTACT_H */
/* EOF */
