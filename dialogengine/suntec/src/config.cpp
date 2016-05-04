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
 * @file configure.cpp
 * @brief configure
 *
 *
 * @attention used for C++ only.
 */
#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <chrono>
#include "config.h"
#include "BL_SystemTime.h"
#include <mutex>

int VR_VOICETAG_RECORD_CONFIDENCE_THRESHOLD = VR_CONFIDENCE_THRESHOLD;
int VR_RECOGNIZE_CONFIDENCE_THRESHOLD = 4000;

static const char* const kDumpTestcaseFile = "/var/vr_de_testcases.txt";
static std::fstream s_dumpStream;
std::mutex s_dumpMutex;

void dumpTestcaseSwitch(bool on)
{
    if (on) {
        std::ifstream fin(kDumpTestcaseFile, std::ios::in);
        if (fin) {
            fin.close();
            s_dumpStream.open(kDumpTestcaseFile, std::fstream::in | std::fstream::out | std::fstream::app);
        }
    }
    else {
        if (s_dumpStream.is_open()) {
            s_dumpStream.close();
        }
    }
}

void dumpTestcase(const std::string& name, const std::string& content)
{
    if (s_dumpStream.is_open()) {
        char buffer[100] = { 0 };

        BL_SystemTime cTime;
        BL_TimeSystem currentTime = { 0 };
        if (BL_NOERROR == cTime.GetSystemTime(&currentTime)) {
            snprintf(buffer, sizeof(buffer), "%02d%02d%02d%03d",
                currentTime.Time.wHour, currentTime.Time.wMinute,
                currentTime.Time.wSecond, currentTime.Time.wMilliseconds);
        }
        else {
            timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            snprintf(buffer, sizeof(buffer), "%02d%02d%03d",
                (int)(ts.tv_sec/60), (int)(ts.tv_sec%60), (int)(ts.tv_nsec/1000000));
        }

        std::lock_guard<std::mutex> lk(s_dumpMutex);
        if (s_dumpStream.is_open()) {
            s_dumpStream << buffer << "\t" << name << "\n" << content << "\n\n";            
        }
    }
}

void writeToFile(const std::string& name, const std::string& content)
{
    std::ifstream fin("/pdata/VR_LOG_FILE", std::ios::in);
    if (fin.is_open()) {
        std::fstream fs;
        fs.open("/tmp/VRtest.log", std::fstream::in | std::fstream::out | std::fstream::app);
        fs << name << ":\n" << content << "\n";
        fs.close();
    }
}


/* EOF */

