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

#include "VR_VoiceTagIDManager.h"
#include "VR_ConfigureIF.h"
#include "VR_Log.h"

#include "BL_DataStorage.h"

#include <sstream>

#define VR_VOICETAGID_KEY           BL_DATA_KEY_VR_VOICETAGIDSET
#define VR_VOICETAGID_BEGIN         0
#define VR_VOICETAGID_MAX           999

VR_VoiceTagIDManager* VR_VoiceTagIDManager::getInstance()
{
    static VR_VoiceTagIDManager manager;
    return &manager;
}

VR_VoiceTagIDManager::VR_VoiceTagIDManager()
{
    // get voiceTagIDSet
    BL_DataStorage storageDB;
    BL_DataValue dataValue;
    if (storageDB.GetValue(VR_VOICETAGID_KEY, dataValue)) {
        VR_LOGD("VoiceTagID GetValue OK");

        const void * dataPoint = dataValue.GetData();
        std::size_t dataValueSize = dataValue.GetSize();
        VR_LOGD("VoiceTag dataValueSize:[%d]", dataValueSize);

        if (nullptr != dataPoint) {
            std::size_t byteSize = sizeof(short int);
            std::size_t count = dataValueSize / byteSize;
            VR_LOGD("VoiceTag ID count:[%d]", count);
            for (std::size_t i = 0; i < count; ++i) {
                short int value = *((short int *)((char *)dataPoint + i * byteSize));
                m_voiceTagIDSet.insert(value);
            }
        }
    }
    else {
        VR_LOGD("VoiceTagID GetValue Failed");
    }
}

VR_VoiceTagIDManager::~VR_VoiceTagIDManager()
{
}

void VR_VoiceTagIDManager::saveVoiceTagID(const std::string &voiceTagID)
{
    m_voiceTagIDSet.insert((short int)atoi(voiceTagID.c_str()));
    std::size_t offset = 0;
    std::size_t byteSize = sizeof(short int);
    std::size_t storageSize = m_voiceTagIDSet.size() * byteSize;
    char * storage = new(MEM_Voice) char[storageSize];
    for (VoiceSet<short int>::iterator it = m_voiceTagIDSet.begin(); it != m_voiceTagIDSet.end(); ++it) {
        memcpy(storage + offset * byteSize, &(*it), byteSize);
        ++offset;
    }
    VR_LOGD("VoiceTagID save count:[%d]", m_voiceTagIDSet.size());

    BL_DataStorage storageDB;
    if (!storageDB.PutValue(VR_VOICETAGID_KEY, (void*)(storage), storageSize)) {
        VR_ERROR("BL_DataStorage PutValue failed, saveVoiceTagID failed");
    }
    else {
        VR_LOGD("VoiceTagID PutValue OK");
    }
    delete[] storage;
}

void VR_VoiceTagIDManager::deleteVoiceTagID(const VoiceList<std::string>::type &voiceTagIDList)
{
    for (VoiceList<std::string>::const_iterator it = voiceTagIDList.cbegin(); it != voiceTagIDList.cend(); ++it) {
        m_voiceTagIDSet.erase((short int)atoi((*it).c_str()));
    }

    std::size_t offset = 0;
    std::size_t byteSize = sizeof(short int);
    char * storage = new(MEM_Voice) char[(m_voiceTagIDSet.size() * byteSize)];
    for (VoiceSet<short int>::iterator it = m_voiceTagIDSet.begin(); it != m_voiceTagIDSet.end(); ++it) {
        memcpy(storage + offset * byteSize, &(*it), byteSize);
        ++offset;
    }
    std::string storageStr(storage, m_voiceTagIDSet.size() * byteSize);
    BL_DataStorage storageDB;
    if (!storageDB.PutValue(VR_VOICETAGID_KEY, (void*)(storageStr.c_str()), storageStr.size() + 1)) {
        VR_ERROR("BL_DataStorage PutValue failed, deleteVoiceTagID failed");
    }
    delete[] storage;
}

void VR_VoiceTagIDManager::clearVoiceTagID()
{
    m_voiceTagIDSet.clear();

    BL_DataStorage storageDB;
    storageDB.DeleteValue(VR_VOICETAGID_KEY);
}

std::string VR_VoiceTagIDManager::getVoiceTagID(const std::string &voiceTagID)
{
    VR_LOG("input voiceTagID: [%s]", voiceTagID.c_str());
    int id = -1;
    if (!voiceTagID.empty()) {
        id = std::stoi(voiceTagID.c_str());
    }

    if (id >= VR_VOICETAGID_BEGIN && id <= VR_VOICETAGID_MAX) {
        VR_LOG("output voiceTagID: [%s]", voiceTagID.c_str());
        return voiceTagID;
    }

    std::string voiceTagIDStr;
    for (short int i = VR_VOICETAGID_BEGIN; i < VR_VOICETAGID_MAX + 1; ++i) {
        if (m_voiceTagIDSet.end() == m_voiceTagIDSet.find(i)) {
            std::ostringstream oss;
            oss << i;
            voiceTagIDStr = oss.str();
            break;
        }
    }
    VR_LOG("output voiceTagID: [%s]", voiceTagIDStr.c_str());
    return voiceTagIDStr;
}

int VR_VoiceTagIDManager::getVoiceTagStorageKey(const std::string &deviceAddress)
{
    switch (atoi(deviceAddress.c_str())) {
    case 1:
        {
            return BL_DATA_KEY_VR_VOICETAG_1;
        }
    case 2:
        {
            return BL_DATA_KEY_VR_VOICETAG_2;
        }
    case 3:
        {
            return BL_DATA_KEY_VR_VOICETAG_3;
        }
    case 4:
        {
            return BL_DATA_KEY_VR_VOICETAG_4;
        }
    case 5:
        {
            return BL_DATA_KEY_VR_VOICETAG_5;
        }
    default:
        {
            return BL_DATA_KEY_VR_END;
        }
    }
}

/* EOF */