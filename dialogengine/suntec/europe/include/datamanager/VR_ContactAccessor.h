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
 * @file VR_ContactAccessor.h
 * @brief Declaration file of VR_ContactAccessor.
 *
 * This file includes the declaration of VR_ContactAccessor.
 *
 * @attention used for C++ only.
 */

#ifndef VR_CONTACT_ACCESSOR_H
#define VR_CONTACT_ACCESSOR_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include "VR_DataAccessor.h"

#define VR_OPERATION_GET_PHONE_INFO             "_getPhoneInfo"
#define VR_OPERATION_GET_INFO_BY_TYPE           "_getInfoByType"
#define VR_OPERATION_GET_CONTACT_IDS            "getContactIds"

class sqlite3;

/**
 * @brief The VR_ContactAccessor class
 *
 * provide interface for VR_ContactAccessor
 */

class VR_ContactAccessor
{
public:
    VR_ContactAccessor();
    virtual ~VR_ContactAccessor() {}

    bool queryPhonebookDatabase(const std::string &contactID,const std::string &phonetypeID,const std::string &number);

    bool queryDatabase_PhoneInfo(bool isTypeFound,pugi::xml_node itemsNode,std::ostringstream oss,const std::string &fullname,const std::string &contactId,const std::string queryType);
    
    int getContactCount();
    // get phone type string used for script, used in buildInfoList and getInfoByType
    std::string getPhoneTypeString(int id);

// protected:
//     void requestService(const std::string &agent, const std::string &reqName) override;

private:

    sqlite3 * m_dbHandler;
    int m_dbResult;
    bool m_isContactInPhonebook;

    std::string m_phoneListStr; // used in getInfoByType

};
#endif /* VR_CONTACT_ACCESSOR_H */
/* EOF */
