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
 * @file VR_VoiceBoxDataStorage.h
 * @brief interface for EngineWrapper callback.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VOICEBOXDATASTORAGE_H
#define VR_VOICEBOXDATASTORAGE_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <string>

/**
 * @brief The VR_VoiceBoxDataStorage class
 *
 * class declaration
 */
class VR_VoiceBoxDataStorage
{
public:
    VR_VoiceBoxDataStorage();
    ~VR_VoiceBoxDataStorage();
    void GetValue(std::string& deviceVoiceTagValueStr);
    void PutValue(std::string& deviceVoiceTagValueStr);
    void PutValue(int key, void* pData, int size);
    void GetValue(int key, void* pData, int& size);
    void GetValue(int key, std::string& deviceVoiceTagValueStr);
    void PutValue(int key, std::string& deviceVoiceTagValueStr);
    void DeleteValue();
    void DeleteValue(int key);
    void RemoveDir(const std::string &cDirName);
    void RemoveFile(const std::string &cDirName);

};


#endif // CXX_BL_DATA_STORE_H
/* EOF */
