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
#include "stdafx.h"
#include "VR_DialogEngineFactory.h"
#include <dlfcn.h>
#include <string>
#include <fstream>
#include <dirent.h>

#ifndef VR_DIALOGENGINEIF_H
#    include "VR_DialogEngineIF.h"
#endif

#ifndef VR_DIALOGENGINENULL_H
#    include "VR_DialogEngineNull.h"
#endif

#ifndef VR_LOG_H
#   include "VR_Log.h"
#endif

#ifndef VR_CONFIGUREIF_H
#    include "VR_ConfigureIF.h"
#endif

#include "MEM_map.h"

const VoiceMap<VR_ConfigureIF::VR_EngineType, std::string>::type::value_type init_value[] =
{
    VoiceMap<VR_ConfigureIF::VR_EngineType, std::string>::type::value_type(VR_ConfigureIF::VR_EngineType::SUNTEC, "libvr_dialogengine_suntec_nuance-navi.so"),
    VoiceMap<VR_ConfigureIF::VR_EngineType, std::string>::type::value_type(VR_ConfigureIF::VR_EngineType::IFLYTEK, "libvr_dialogengine_suntec_iflytek-navi.so"),
    VoiceMap<VR_ConfigureIF::VR_EngineType, std::string>::type::value_type(VR_ConfigureIF::VR_EngineType::VBT, "libvr_dialogengine_vbt-navi.so")
    // VoiceMap<VR_ConfigureIF::VR_EngineType, std::string>::type::value_type(VR_ConfigureIF::VR_EngineType::BARGEIN, "libvr_dialogengine_bargein-navi.so"),
};

const static VoiceMap<VR_ConfigureIF::VR_EngineType, std::string>::type VR_DE_LibraryNameMap(init_value, init_value + 3);

static VoiceMap<VR_ConfigureIF::VR_EngineType, void *>::type VR_DE_LibraryHandleMap;

#define VR_SYM_DIALOG_ENGINE "VR_CreateDialogEngine"
typedef VR_DialogEngineIF* (*fnCreateDialogEngine)();

static VOID* VR_LoadLibrary(const char* szPath)
{
    if (!szPath) {
        VR_ERROR("Load DialogEngine lib failed, szPath is NULL\n");
        return NULL;
    }

    VOID* handle;
    if (0 == strcmp(szPath, "libvr_dialogengine_vbt-navi.so")) {
        handle = dlopen(szPath, RTLD_LAZY | RTLD_GLOBAL);
    }
    else {
        handle = dlopen(szPath, RTLD_LAZY | RTLD_LOCAL);
    }
    if (!handle) {
        VR_ERROR("Load DialogEngine lib failed, szPath=%s, errmsg=%s\n", szPath, dlerror());
        return NULL;
    }
    return handle;
}

static VR_DialogEngineIF* VR_LoadDialogEngine(bool bargeinEngine)
{
    VR_LOGD_FUNC();
    VR_ConfigureIF* pcConfig = VR_ConfigureIF::Instance();
    if (NULL == pcConfig) {
        VR_ERROR("get configure handle failed");
        return NULL;
    }

    VOID* handle = NULL;
    if (bargeinEngine) {
        handle = VR_LoadLibrary("libvr_dialogengine_bargein-navi.so");
        if (NULL == handle) {
            VR_ERROR("can not find engine so: libvr_dialogengine_bargein-navi.so");
            return NULL;
        }
    }
    else {
        VoiceMap<VR_ConfigureIF::VR_EngineType, void *>::iterator it;
        it = VR_DE_LibraryHandleMap.find(pcConfig->getEngineType());
        if (it == VR_DE_LibraryHandleMap.end()) {
            VR_ERROR("can not find engine so: %s", VR_DE_LibraryNameMap.at(pcConfig->getEngineType()).c_str());
            return NULL;
        }
        handle = it->second;

        // close the other
        VR_DE_LibraryHandleMap.erase(it);
        it = VR_DE_LibraryHandleMap.begin();
        while (it != VR_DE_LibraryHandleMap.end()) {
            dlclose(it->second);
            it = VR_DE_LibraryHandleMap.erase(it);
        }
    }

    // Load so export function sym
    fnCreateDialogEngine func = (fnCreateDialogEngine)dlsym(handle, VR_SYM_DIALOG_ENGINE);
    if (!func) {
        VR_ERROR("dlsym failed, func=%s\n", VR_SYM_DIALOG_ENGINE);
        return NULL;
    }

    // Create dialog engine instance
    VR_DialogEngineIF* pEngineIF = (*func)();
    if (!pEngineIF) {
        VR_ERROR("Create dialog engine instance failed, func=%s\n", VR_SYM_DIALOG_ENGINE);
        return NULL;
    }

    VR_LOG("Load dialogengine ok");
    return pEngineIF;
}

bool VR_DialogEngineFactory::LoadAllLibraries()
{
    bool ok = false;
    for (VoiceMap<VR_ConfigureIF::VR_EngineType, std::string>::const_iterator it = VR_DE_LibraryNameMap.cbegin();
        it != VR_DE_LibraryNameMap.cend();
        ++it) {
        void* handle = VR_LoadLibrary(it->second.c_str());
        if (NULL != handle) {
            VR_DE_LibraryHandleMap[it->first] = handle;
            ok = true;
        }
    }
    return ok;
}

VR_DialogEngineIF* VR_DialogEngineFactory::CreateDialogEngine(bool mock)
{
    if (mock) {
        VR_DialogEngineIF* pEngineIF = new(MEM_Voice) VR_DialogEngineNull();
        return pEngineIF;
    }
    std::string debugTtsPath = "/data/vr_bargein_debug.txt";
    std::string debugWavPath = "/data/vr_bargein_debug.wav";
    std::string bargeinPath = "/data/vr_bargein_prompts";
    std::ifstream ifsDebugTts(debugTtsPath.c_str());
    std::ifstream ifsDebugWav(debugWavPath.c_str());

    bool bargeinPrompts = false;
    DIR *dir = opendir(bargeinPath.c_str());
    if (dir != NULL) {
        bargeinPrompts = true;
        VR_LOG("Load dialogengine with bargein prompts");
    }

    bool isBargeinEngine = (ifsDebugTts || ifsDebugWav || bargeinPrompts);

    VR_DialogEngineIF* pEngineIF = VR_LoadDialogEngine(isBargeinEngine);
    if (NULL == pEngineIF) {
        VR_ERROR("Load DialogEngine failed");
        pEngineIF = new(MEM_Voice) VR_DialogEngineNull();
    }
    return pEngineIF;
}

/* EOF */
