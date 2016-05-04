/**
 * Copyright @ 2013 - 2015 Suntec Software(Shanghai) Co., Ltd.
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
 * @file VR_ParamConfig.h
 * @brief Interface file of VR_ParamConfig.
 *
 * This file includes the interface of VR_ParamConfig.
 *
 * @attention used for C++ only.
 */

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#ifndef _VR_PARAMCONFIOG_H_
#define _VR_PARAMCONFIOG_H_

#include <string>
#include <fstream>
#include <iostream>
#include "VR_Log.h"
#include "MEM_map.h"

/**
 * Class ParamConfig
 *
 * ParamConfig for paramter setting
 *
 */
class ParamConfig
{
public:
    // clear all values
    void Clear()
    {
        _data.clear();
    }

    // load a configuration file
    bool Load(const std::string& File);

    // check if value associated with given key exists
    bool Contains(const std::string& key) const
    {
        return _data.find(key) != _data.end();
    }

    // get value associated with given key
    bool Get(const std::string& key, std::string& value) const
    {
        VoiceMap<std::string, std::string>::const_iterator iter = _data.find(key);
        // std::map<std::string, std::string>::const_iterator

        if (iter != _data.end()) {
            value = iter->second;
            VR_LOGD("ParamConfig get value sucess! key = %s, value = %s.", key.c_str(), value.c_str());
            return true;
        }
        else {
            return false;
        }
    }

    bool Get(const std::string& key, int& value) const
    {
        std::string str;

        if (Get(key, str)) {
            value = atoi(str.c_str());
            return true;
        }
        else {
            return false;
        }
    }

private:
    // the container
    // std::map<std::string, std::string> _data;
    VoiceMap<std::string, std::string>::type _data;

    // remove leading and trailing tabs and spaces
    static std::string Trim(const std::string& str)
    {
        std::string::size_type first = str.find_first_not_of(" \t");

        if (first != std::string::npos) {
            std::string::size_type last = str.find_last_not_of(" \t");

            return str.substr(first, last - first + 1);
        }
        else {
            return "";
        }
    }
};

#endif // _VR_PARAMCONFIOG_H_
/* EOF */
