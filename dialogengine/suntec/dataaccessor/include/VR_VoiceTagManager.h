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
 * @file VR_VoiceTagManager.h
 * @brief Declaration file of VR_VoiceTagManager.
 *
 * This file includes the declaration of VR_VoiceTagManager.
 *
 * @attention used for C++ only.
 */

#ifndef VR_VOICE_TAG_MANAGER_H
#define VR_VOICE_TAG_MANAGER_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include "MEM_list.h"
#include "MEM_map.h"

#include <pugixml.hpp>
#include <string>

enum class VR_VoiceTagResult
{
    VOICETAG_OK = 0,
    VOICETAG_NOSPEECH = 1,
    VOICETAG_TOOSHORT = 2,
    VOICETAG_SIMILAR = 3,
    VOICETAG_FAILURE = 4
};

/**
 * @brief The VR_VoiceTagManager class
 *
 * provide interface for query voicetag
 *
 */
class VR_VoiceTagQuerier
{
public:
    virtual ~VR_VoiceTagQuerier() {}

    virtual std::string getContactInfo(const std::string &voiceTagID) = 0;
    virtual int getVoiceTagCount() = 0;
    virtual std::string getContactIDByVoiceTagID(const std::string &voiceTagID) = 0;
    virtual std::string getVoiceTagIDByContactID(const std::string &contactID) = 0;
};

class VR_ConfigureIF;

/**
 * @brief The VR_VoiceTagManager class
 *
 * manage voicetag
 *
 */
class VR_VoiceTagManager : public VR_VoiceTagQuerier
{
public:
    struct VoiceTagGrammarParam
    {
        std::string voiceTagID;
        std::string contactName;
        std::string phoneme;
    };

    VR_VoiceTagManager(VR_ConfigureIF* config)
        : m_configure(config)
    {
    }

    virtual ~VR_VoiceTagManager() {}

    // pcmpath is relativePath, e.g. relativePath to (BL_FILE_PREFIX_RW)/VR/SUNTEC/
    virtual bool saveVoiceTag(const std::string &deviceAddress, const std::string &voiceTagID, const std::string &pcmPath, const std::string &phoneme, const std::string &contactMsg, bool update = false);
    // play voice tag
    virtual std::string getVoiceTagPCMPath(const std::string &voiceTagID);
    virtual bool deleteVoiceTag(const std::string &deviceAddress, const std::string &voiceTagID);
    // delete deviceAddress's all voicetag
    virtual bool deleteAllVoiceTag(const std::string &deviceAddress);
    // delete all voicetag
    virtual bool clearVoiceTag();
    // sync voicetag with bt service
    virtual bool syncVoiceTag(const std::string &deviceAddress, const std::string &btContactIDStr, std::string &btVoiceTagIDStr);
    // update voicetag when contact changed
    virtual bool updateVoiceTag(const std::string &voiceTagID, const pugi::xml_node &contactNode);

    virtual void setCurrentDevice(const std::string &deviceAddress);
    virtual void clearCurrentDevice();

    virtual std::string getContactInfo(const std::string &voiceTagID);
    virtual int getVoiceTagCount();

    virtual void getVoiceTagGrammarParam(VoiceList<VoiceTagGrammarParam>::type &paramList);

    virtual bool saveVoiceTagGrammar(const std::string &voiceTagID, const std::string &name, const std::string &phoneme, const std::string &deviceAddress) = 0;
    virtual bool deleteVoiceTagGrammar(const VoiceList<std::pair<std::string, std::string>>::type &deleteList, const std::string &deviceAddress) = 0;
    // delete deviceAddress's all voicetag grammar
    virtual bool deleteAllVoiceTagGrammar(const std::string &deviceAddress) = 0;
    // delete all voicetag grammar
    virtual bool clearVoiceTagGrammar() = 0;

    virtual std::string getContactIDByVoiceTagID(const std::string &voiceTagID);
    virtual std::string getVoiceTagIDByContactID(const std::string &contactID);

private:
    VR_ConfigureIF* m_configure;
    std::string m_currentDeviceAddress;
    VoiceMap<std::string, std::string>::type m_voiceTagContactIDMap;

    bool getVoiceTagIDList(const std::string &deviceAddress, VoiceList<std::string>::type &voiceTagIDList);
    bool getIDListFromString(const std::string &IDStr, VoiceList<std::string>::type &IDList);
    void initVoiceTagContactIDMap(const VoiceList<std::string>::type &voiceTagIDList, const VoiceList<std::string>::type &contactIDList);
};

#endif // VR_VOICE_TAG_MANAGER_H
/* EOF */