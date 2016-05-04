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
 * @file VR_VoiceIDConverter.h
 * @brief Declaration file of VR_VoiceIDConverter.
 *
 * This file includes the declaration of VR_VoiceIDConverter.
 *
 * @attention used for C++ only.
 */

#ifndef VR_VOICE_ID_CONVERTER_H
#define VR_VOICE_ID_CONVERTER_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <string>

enum class VR_MappingType
{
    FULL,
    TILL_STREET,
    TILL_CITY,
    TILL_STATE,
    EMPTY
};

struct VR_GlobalVoiceIDResult
{
    VR_MappingType type;
    std::string zoneid;
    std::string cityid;
    std::string streetid;
    std::string houseNumber;
};

/**
 * @brief The VR_VoiceIDConverter class
 *
 * convert voiceid from asr to global voiceid
 *
 */

class VR_VoiceIDConverter
{
public:
    VR_VoiceIDConverter() {}
    virtual ~VR_VoiceIDConverter() {}

    VR_GlobalVoiceIDResult getGlobalVoiceID(const std::string &zoneidStr, const std::string &cityidStr, const std::string &streetidStr, const std::string &houseNumberStr);
    std::string getGlobalZoneID(const std::string &zoneidStr);
    std::string getGlobalCityID(const std::string &globalZoneidStr, const std::string &cityidStr);
    std::string getGlobalStreetID(const std::string &globalCityidStr, const std::string &streetidStr);

    std::string getZoneID(const std::string &globalVoiceidStr);
    std::string getCityID(const std::string &globalVoiceidStr);
    std::string getStreetID(const std::string &globalVoiceidStr);
};

#endif /* VR_VOICE_ID_CONVERTER_H */
/* EOF */