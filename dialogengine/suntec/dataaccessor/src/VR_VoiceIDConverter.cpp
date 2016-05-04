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

#include "VR_VoiceIDConverter.h"
#include <sstream>

VR_GlobalVoiceIDResult VR_VoiceIDConverter::getGlobalVoiceID(const std::string &zoneidStr, const std::string &cityidStr, const std::string &streetidStr, const std::string &houseNumberStr)
{
    VR_GlobalVoiceIDResult result;
    result.type = VR_MappingType::EMPTY;

    if (!zoneidStr.empty()) {
        result.type = VR_MappingType::TILL_STATE;
        std::stringstream ss;
        unsigned long long int zoneid;
        ss << zoneidStr;
        ss >> zoneid;
        ss.clear();
        ss.str("");
        zoneid = zoneid << 48;
        ss << zoneid;
        result.zoneid = ss.str();
        ss.clear();
        ss.str("");

        if (!cityidStr.empty()) {
            result.type = VR_MappingType::TILL_CITY;
            unsigned long long int cityid;
            ss << cityidStr;
            ss >> cityid;
            ss.clear();
            ss.str("");
            cityid = cityid << 32;
            cityid = zoneid | cityid;
            ss << cityid;
            result.cityid = ss.str();
            ss.clear();
            ss.str("");

            if (!streetidStr.empty()) {
                result.type = VR_MappingType::TILL_STREET;
                unsigned long long int streetid;
                ss << streetidStr;
                ss >> streetid;
                ss.clear();
                ss.str("");
                streetid = cityid | streetid;
                ss << streetid;
                result.streetid = ss.str();
                ss.clear();
                ss.str("");

                if (!houseNumberStr.empty()) {
                    result.type = VR_MappingType::FULL;
                    result.houseNumber = houseNumberStr;
                }
            }
        }
    }

    return result;
}

std::string VR_VoiceIDConverter::getGlobalZoneID(const std::string &zoneidStr)
{
    std::stringstream ss;
    unsigned long long int zoneid;
    ss << zoneidStr;
    ss >> zoneid;
    ss.clear();
    ss.str("");
    zoneid = zoneid << 48;
    ss << zoneid;
    return ss.str();
}

std::string VR_VoiceIDConverter::getGlobalCityID(const std::string &globalZoneidStr, const std::string &cityidStr)
{
    std::stringstream ss;
    unsigned long long int globalZoneid;
    unsigned long long int cityid;
    ss << globalZoneidStr;
    ss >> globalZoneid;
    ss.clear();
    ss.str("");
    ss << cityidStr;
    ss >> cityid;
    ss.clear();
    ss.str("");
    cityid = cityid << 32;
    cityid = globalZoneid | cityid;
    ss << cityid;
    return ss.str();
}

std::string VR_VoiceIDConverter::getGlobalStreetID(const std::string &globalCityidStr, const std::string &streetidStr)
{
    std::stringstream ss;
    unsigned long long int globalCityid;
    unsigned long long int streetid;
    ss << globalCityidStr;
    ss >> globalCityid;
    ss.clear();
    ss.str("");
    ss << streetidStr;
    ss >> streetid;
    ss.clear();
    ss.str("");
    streetid = globalCityid | streetid;
    ss << streetid;
    return ss.str();
}

std::string VR_VoiceIDConverter::getZoneID(const std::string &globalVoiceidStr)
{
    std::stringstream ss;
    unsigned long long int globalVoiceid;
    unsigned long long int zoneid;
    ss << globalVoiceidStr;
    ss >> globalVoiceid;
    ss.clear();
    ss.str("");
    zoneid = 0x00ff000000000000 & globalVoiceid;
    zoneid = zoneid >> 48;
    ss << zoneid;
    return ss.str();
}

std::string VR_VoiceIDConverter::getCityID(const std::string &globalVoiceidStr)
{
    std::stringstream ss;
    unsigned long long int globalVoiceid;
    unsigned long long int cityid;
    ss << globalVoiceidStr;
    ss >> globalVoiceid;
    ss.clear();
    ss.str("");
    cityid = 0x0000ffff00000000 & globalVoiceid;
    cityid = cityid >> 32;
    ss << cityid;
    return ss.str();
}

std::string VR_VoiceIDConverter::getStreetID(const std::string &globalVoiceidStr)
{
    std::stringstream ss;
    unsigned long long int globalVoiceid;
    unsigned long long int streetid;
    ss << globalVoiceidStr;
    ss >> globalVoiceid;
    ss.clear();
    ss.str("");
    streetid = 0x00000000ffffffff & globalVoiceid;
    ss << streetid;
    return ss.str();
}

/* EOF */