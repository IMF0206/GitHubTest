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
 * @file VR_VoiceTagIDManager.h
 * @brief Declaration file of VR_VoiceTagIDManager.
 *
 * This file includes the declaration of VR_VoiceTagIDManager.
 *
 * @attention used for C++ only.
 */

#ifndef VR_VOICE_TAG_ID_MANAGER_H
#define VR_VOICE_TAG_ID_MANAGER_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include "pugixml.hpp"

#include "MEM_set.h"
#include "MEM_list.h"

/**
 * @brief The VR_VoiceTagIDManager class
 *
 * manage voicetag id for dialogengine
 */
class VR_VoiceTagIDManager
{
public:
    virtual ~VR_VoiceTagIDManager();

    static VR_VoiceTagIDManager* getInstance();

    void saveVoiceTagID(const std::string &voiceTagID);
    void deleteVoiceTagID(const VoiceList<std::string>::type &voiceTagIDList);
    void clearVoiceTagID();
    std::string getVoiceTagID(const std::string &voiceTagID);

    int getVoiceTagStorageKey(const std::string &deviceAddress);

private:
    VoiceSet<short int>::type m_voiceTagIDSet;

    VR_VoiceTagIDManager();
    VR_VoiceTagIDManager(const VR_VoiceTagIDManager &copy);
    VR_VoiceTagIDManager & operator=(const VR_VoiceTagIDManager &copy);
};

#endif /* VR_VOICE_TAG_ID_MANAGER_H */
/* EOF */
