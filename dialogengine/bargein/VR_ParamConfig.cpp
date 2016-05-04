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

#include "VR_ParamConfig.h"

bool ParamConfig::Load(const std::string& file)
{
    std::ifstream inFile(file.c_str());

    if (!inFile.good()) {
        VR_LOGD("Cannot read configuration file: %s", file.c_str());
        return false;
    }

    while (inFile.good() && !inFile.eof()) {
        std::string line;
        getline(inFile, line);

        // filter out comments
        if (!line.empty()) {
            std::string::size_type pos = line.find('#');

            if (pos != std::string::npos) {
                line = line.substr(0, pos);
            }
        }

        // split line into key and value
        if (!line.empty()) {
            std::string::size_type pos = line.find('=');

            if (pos != std::string::npos) {
                std::string key     = Trim(line.substr(0, pos));
                std::string value   = Trim(line.substr(pos + 1));

                if (!key.empty() && !value.empty()) {
                    _data[key] = value;
                }
            }
        }  // if (!line.empty()) {
    }  // while

    return true;
}

/* EOF */
