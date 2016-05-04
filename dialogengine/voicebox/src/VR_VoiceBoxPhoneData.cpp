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


#include <sqlite3.h>
#include <string>
#include "MEM_map.h"
#include "MEM_list.h"
#include <sstream>

/* Suntec Header */
#ifndef VR_VOICEBOXPHONEDATA_H
#    include "VR_VoiceBoxPhoneData.h"
#endif

#ifndef VR_CONFIGUREIF_H
#    include "VR_ConfigureIF.h"
#endif

#include "VR_Log.h"

#define VR_DB_TABLENAME_PHONENUMBER             "phoneNumber"
#define VR_DB_TABLENAME_CONTACT                 "contact"

// Constructor
VR_VoiceBoxPhoneData::VR_VoiceBoxPhoneData()
    : m_dbHandler(NULL)
{
}

void
VR_VoiceBoxPhoneData::OpenPhoneBookDB(const std::string strPath, VoiceVector<PersonDetailInfo>::type& personDetailInfo)
{

    std::string path = VR_ConfigureIF::Instance()->getUsrPath() + strPath.c_str();
    VR_LOG("strPath : %s", strPath.c_str());
    m_dbResult = sqlite3_open(path.c_str(), &m_dbHandler);

    if (SQLITE_OK != m_dbResult) {
        VR_ERROR("Open DB file %s failed, resultID: [%d]",
            strPath.c_str(),
            m_dbResult);
        return;
    }
    else {
        GetPhoneBookInfo(personDetailInfo);
    }
}

void
VR_VoiceBoxPhoneData::GetPhoneBookInfo(VoiceVector<PersonDetailInfo>::type& personDetailInfo)
{
    std::string sqlRequest("SELECT contact.id,contact.first,contact.last,phonerecords.typeid,phonerecords.indexid,phonerecords.number FROM phonerecords,contact WHERE contact.id=phonerecords.id");

    char * errorMsg = NULL;
    int result = sqlite3_exec(m_dbHandler, sqlRequest.c_str(), GetPhoneBookInfoCallback, &personDetailInfo, &errorMsg);
    if (SQLITE_OK != result) {
        VR_ERROR("Run SQL request [%s] failed, error code: [%d], error msg: [%s]", sqlRequest.c_str(), result, errorMsg);
        return;
    }
}

std::string
VR_VoiceBoxPhoneData::GetPhoneType(const std::string& contactID, const std::string& phoneNumber, const std::string& phoneType, const std::string& strPath)
{
    if ("" == contactID) {
        return phoneType;
    }
    else {
        if (("0" == phoneType) || ("1" == phoneType) || ("2" == phoneType) || ("3" == phoneType)) {
            std::string path = VR_ConfigureIF::Instance()->getUsrPath() + strPath.c_str();
            VR_LOG("strPath : %s", strPath.c_str());
            m_dbResult = sqlite3_open(path.c_str(), &m_dbHandler);

            if (SQLITE_OK != m_dbResult) {
                VR_ERROR("Open DB file %s failed, resultID: [%d]",
                    strPath.c_str(),
                    m_dbResult);
                return phoneType;
            }

            std::string sqlRequest = "SELECT phonerecords.typeid,phonerecords.indexid FROM phonerecords WHERE phonerecords.id=";
            sqlRequest.append(contactID);
            sqlRequest.append(" and phonerecords.number=");
            sqlRequest.append(phoneNumber);

            std::string strPhoneType;
            char * errorMsg = NULL;
            int result = sqlite3_exec(m_dbHandler, sqlRequest.c_str(), GetPhoneTypeCallback, &strPhoneType, &errorMsg);
            if (SQLITE_OK != result) {
                VR_ERROR("Run SQL request [%s] failed, error code: [%d], error msg: [%s]", sqlRequest.c_str(), result, errorMsg);
                return phoneType;
            }
            if ("" == strPhoneType) {
                return phoneType;
            }
            else {
                return strPhoneType;
            }
        }
        else {
            return phoneType;
        }
    }
}

int
VR_VoiceBoxPhoneData::GetPhoneTypeCallback(void *phoneType, int columnNum, char **columnValue, char **columnName)
{
    if ((NULL != phoneType) && (columnNum >= 2)) {
        std::string indexID;
        std::string typeID;
        if (NULL != columnValue[0]) {
            indexID = columnValue[0];
            typeID = columnValue[1];
            *((std::string*)phoneType) = GetPhoneTypeToUI(indexID, typeID);
        }
    }
}

int
VR_VoiceBoxPhoneData::GetPhoneBookInfoCallback(void *personDetailInfo, int columnNum, char **columnValue, char **columnName)
{
    if ((NULL != personDetailInfo) && (columnNum >= 6)) {
        PersonDetailInfo personInfo;
        std::string indexID;
        std::string typeID;
        if (NULL != columnValue[0]) {
            personInfo.strId = columnValue[0];
            personInfo.firstName = columnValue[1];
            personInfo.lastName = columnValue[2];
            typeID = columnValue[3];
            indexID = columnValue[4];
            personInfo.phonetype = GetPhoneTypeID(typeID, indexID);
            personInfo.number = columnValue[5];
        }
        ((VoiceVector<PersonDetailInfo>::type*)personDetailInfo)->push_back(personInfo);
    }
    return 0;
}

std::string
VR_VoiceBoxPhoneData::GetPhoneTypeID(const std::string typeID, const std::string indexID)
{
    int iTypeID = atoi(typeID.c_str());
    int iIndexID = atoi(indexID.c_str());

    if (1 == iIndexID) {
        iIndexID = 0;
    }
    int iPhoneType = iTypeID + iIndexID * 4;
    std::string phoneType;
    std::stringstream stream;
    stream << iPhoneType;
    stream >> phoneType;

    return phoneType;
}

std::string
VR_VoiceBoxPhoneData::GetPhoneTypeToUI(const std::string typeID, const std::string indexID)
{
    VR_LOG("typeID : %s", typeID.c_str());
    VR_LOG("indexID : %s", indexID.c_str());

    int iTypeID = atoi(typeID.c_str());
    int iIndexID = atoi(indexID.c_str());
    int iPhoneType = iTypeID + iIndexID * 4;

    std::string phoneType;
    std::stringstream stream;
    stream << iPhoneType;
    stream >> phoneType;

    return phoneType;
}

// Destructor
VR_VoiceBoxPhoneData::~VR_VoiceBoxPhoneData()
{
    sqlite3_close(m_dbHandler);
    m_dbHandler = NULL;
}
/* EOF */