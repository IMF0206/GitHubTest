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

/* Suntec Header */
#include "VR_Log.h"

#ifndef VR_VOICEBOXVOICETAG_H
#    include "VR_VoiceBoxVoiceTag.h"
#endif

#ifndef VR_VOICEBOXDATASTORAGE_H
#    include "VR_VoiceBoxDataStorage.h"
#endif

#ifndef VR_VOICETAGIDMANAGER_H
#include "VR_VoiceTagIDManager.h"
#endif

#ifndef VR_CONFIGUREIF_H
#    include "VR_ConfigureIF.h"
#endif

#define VR_VOICETAG_ID_PREFIX       "VoiceTagID"
#define VR_VOICETAG_TEMPDATA        "tempData"
#define VR_VOICETAG_REC_PATH        "rec/"
#define VR_VOICETAG_CONTACT_NODE    "contact"
#define VR_VOICETAG_PCM_PATH_NODE   "pcmPath"
#define VR_VOICETAG_PHONEME_NODE    "phoneme"

// Constructor
VR_VoiceBoxVoiceTag::VR_VoiceBoxVoiceTag()
{
}

// Destructor
VR_VoiceBoxVoiceTag::~VR_VoiceBoxVoiceTag()
{
}

void
VR_VoiceBoxVoiceTag::CheckTempVoiceTag(const std::string &deviceAddress)
{
    VR_LOGD_FUNC();

    int key = VR_VoiceTagIDManager::getInstance()->getVoiceTagStorageKey(deviceAddress);

    std::string deviceVoiceTagValueStr;
    VR_VoiceBoxDataStorage storage;
    // VR_LOG("Delete VoiceTag Begin");
    storage.GetValue(key, deviceVoiceTagValueStr);
    VR_LOG("deviceVoiceTagValueStr : %s", deviceVoiceTagValueStr.c_str());
    pugi::xml_document deviceVoiceTagValueDoc;
    deviceVoiceTagValueDoc.load_string(deviceVoiceTagValueStr.c_str());

    pugi::xml_node voiceTagValueNode = deviceVoiceTagValueDoc.first_child();
    VoiceList<std::string>::type voiceTagIDList;
    while (!voiceTagValueNode.empty()) {
        std::string voiceTagID = voiceTagValueNode.name();
        VR_LOG("voiceTagID: %s", voiceTagID.c_str());
        if (std::string::npos != voiceTagID.find(VR_VOICETAG_ID_PREFIX)) {
            std::string tempData = voiceTagValueNode.select_node((std::string("//") + VR_VOICETAG_TEMPDATA).c_str()).node().text().as_string();
            VR_LOG("tempData: %s", tempData.c_str());
            if ("true" == tempData) {
                deviceVoiceTagValueDoc.remove_child(voiceTagID.c_str());
                voiceTagIDList.push_back(std::string(voiceTagValueNode.name()).substr(std::string(VR_VOICETAG_ID_PREFIX).size()));
            }
        }
        else {
            deviceVoiceTagValueDoc.remove_child(voiceTagID.c_str());
        }
        voiceTagValueNode = voiceTagValueNode.next_sibling();
    }

    std::ostringstream oss;
    deviceVoiceTagValueDoc.print(oss);
    deviceVoiceTagValueStr = oss.str();
    VR_LOG("deviceVoiceTagValueStr : %s", deviceVoiceTagValueStr.c_str());
    if ("" == deviceVoiceTagValueStr) {
        storage.DeleteValue(key);
        std::string pcmDir = VR_VOICETAG_REC_PATH + deviceAddress;
        VR_ConfigureIF::Instance()->removeDir(pcmDir);
    }
    else {
        storage.PutValue(key, deviceVoiceTagValueStr);
    }
    VoiceList<std::string>::iterator it;
    for (it = voiceTagIDList.begin(); it != voiceTagIDList.end(); ++it) {
        VR_LOG("voiceTagIDList");
        VR_LOG("voiceTagIDList: %s", (*it).c_str());
    }
    VR_VoiceTagIDManager::getInstance()->deleteVoiceTagID(voiceTagIDList);

}

bool
VR_VoiceBoxVoiceTag::SaveVoiceTag(std::string &strAddGrammar, const std::string &deviceAddress, VoiceTagInfo voiceTagPara, const std::string tempData, bool update)
{
    VR_LOGD_FUNC();

    VR_LOG("deviceAddress : %s", deviceAddress.c_str());
    if (deviceAddress.empty()) {
        return false;
    }
    pugi::xml_document contactDoc;
    contactDoc.load_string(voiceTagPara.contactMsg.c_str());
    pugi::xml_node contactNode = contactDoc.select_node((std::string("//") + VR_VOICETAG_CONTACT_NODE).c_str()).node();
    // std::string name = contactNode.attribute("name").as_string();
    VR_LOG("pcmfile path : %s", voiceTagPara.pcmPath.c_str());

    std::string dirPath = VR_VOICETAG_REC_PATH;
    VR_ConfigureIF::Instance()->makeDir(dirPath);
    dirPath = dirPath + deviceAddress;
    VR_ConfigureIF::Instance()->makeDir(dirPath);
    VR_LOG("pcmfile path : %s", dirPath.c_str());
    std::string relativePath(dirPath + "/" + voiceTagPara.voiceTagID + ".wav");
    VR_LOG("relativePath %s", relativePath.c_str());
    VR_VoiceBoxDataStorage storage;
    std::string newPath(VR_ConfigureIF::Instance()->getUsrPath() + VR_VOICETAG_REC_PATH + deviceAddress + "/" + voiceTagPara.voiceTagID + ".wav");
    VR_LOG("pcmfile newPath path : %s", newPath.c_str());
    if ("false" == tempData) {
        if (VR_ConfigureIF::Instance()->isFileExist(relativePath)) {
            bool isRemoved = VR_ConfigureIF::Instance()->removeFile(relativePath);
            VR_LOG("Remove File: %s", isRemoved ? "true" : "false");
        }
        VR_LOG("moveFile");
        VR_ConfigureIF::Instance()->moveFile("voiceTag.wav", relativePath.c_str());
    }

    // create voicetag value
    pugi::xml_document voiceTagValueDoc;
    voiceTagValueDoc.load_string("");
    pugi::xml_node voiceTagValueNode = voiceTagValueDoc.append_child((std::string(VR_VOICETAG_ID_PREFIX) + voiceTagPara.voiceTagID).c_str());
    if ("true" == tempData) {
        newPath = voiceTagPara.pcmPath;
    }
    VR_LOG("pcmfile newPath path : %s", newPath.c_str());
    voiceTagValueNode.append_child(VR_VOICETAG_PCM_PATH_NODE).text().set(newPath.c_str());
    voiceTagValueNode.append_child(VR_VOICETAG_PHONEME_NODE).text().set(voiceTagPara.phoneme.c_str());
    voiceTagValueNode.append_child(VR_VOICETAG_TEMPDATA).text().set(tempData.c_str());
    voiceTagValueNode.append_copy(contactNode);

    std::ostringstream osss;
    voiceTagValueNode.print(osss);
    VR_LOG("voiceTag Value: %s", osss.str().c_str());

    // save voicetag
    int key = VR_VoiceTagIDManager::getInstance()->getVoiceTagStorageKey(deviceAddress);
    VR_LOG("key: %d", key);
    std::string deviceVoiceTagValueStr;

    storage.GetValue(key, deviceVoiceTagValueStr);

    pugi::xml_document deviceVoiceTagValueDoc;
    deviceVoiceTagValueDoc.load_string(deviceVoiceTagValueStr.c_str());
    if ("true" == tempData) {
        pugi::xml_node VoicetagIdNode = deviceVoiceTagValueDoc.select_node((std::string("//") + VR_VOICETAG_ID_PREFIX + voiceTagPara.voiceTagID).c_str()).node();
        if (VoicetagIdNode) {
            std::string name = VoicetagIdNode.name();
            VR_LOG("name :%s", name.c_str());
            deviceVoiceTagValueDoc.remove_child((std::string(VR_VOICETAG_ID_PREFIX) + voiceTagPara.voiceTagID).c_str());
        }
    }
    if (update) {
        VR_LOG("update");
        deviceVoiceTagValueDoc.remove_child((std::string(VR_VOICETAG_ID_PREFIX) + voiceTagPara.voiceTagID).c_str());
    }
    if ("false" == tempData) {
        VR_LOG("remove_child");
        deviceVoiceTagValueDoc.remove_child((std::string(VR_VOICETAG_ID_PREFIX) + "-2").c_str());
    }
    deviceVoiceTagValueDoc.append_copy(voiceTagValueNode);

    std::ostringstream oss;
    deviceVoiceTagValueDoc.print(oss);
    deviceVoiceTagValueStr = oss.str();
    VR_LOG("VoiceTag Value: %s", deviceVoiceTagValueStr.c_str());
    storage.PutValue(key, deviceVoiceTagValueStr);
    if ("false" == tempData) {
        VR_VoiceTagIDManager::getInstance()->saveVoiceTagID(voiceTagPara.voiceTagID);
    }
    VR_LOG("Save VoiceTag End");

    deviceVoiceTagValueStr.clear();
    storage.GetValue(key, deviceVoiceTagValueStr);
    VR_LOG("VoiceTag Value: %s", deviceVoiceTagValueStr.c_str());
    if ("false" == tempData) {
        std::string strFullName = contactNode.attribute("name").as_string();
        VR_LOG("FullName: %s", strFullName.c_str());
        strAddGrammar = "<grammar_voicetag agent=\"phone\" op=\"addGrammar\"><fullName>";
        strAddGrammar.append(strFullName);
        strAddGrammar.append("</fullName><phoneme>");
        strAddGrammar.append(voiceTagPara.phoneme);
        strAddGrammar.append("</phoneme></grammar_voicetag>");
        VR_LOG("strAddGrammar : %s", strAddGrammar.c_str());
    }
    return true;
}

bool
VR_VoiceBoxVoiceTag::DeleteAllVoiceTag(std::string &strAddGrammar, const std::string &deviceAddress)
{
    VR_LOGD_FUNC();

    if (deviceAddress.empty()) {
        return false;
    }
    std::string pcmDir = VR_VOICETAG_REC_PATH + deviceAddress;
    VR_ConfigureIF::Instance()->removeDir(pcmDir);

    return RemoveAllRegisteredVoiceTag(strAddGrammar, deviceAddress);
}

bool
VR_VoiceBoxVoiceTag::RemoveAllRegisteredVoiceTag(std::string &strAddGrammar, const std::string &deviceAddress)
{
    VR_LOGD_FUNC();

    int key = VR_VoiceTagIDManager::getInstance()->getVoiceTagStorageKey(deviceAddress);

    std::string deviceVoiceTagValueStr;
    VR_VoiceBoxDataStorage storage;
    VR_LOG("Delete VoiceTag Begin");
    storage.GetValue(key, deviceVoiceTagValueStr);
    VR_LOG("VoiceTag Value: %s", deviceVoiceTagValueStr.c_str());
    pugi::xml_document deviceVoiceTagValueDoc;
    deviceVoiceTagValueDoc.load_string(deviceVoiceTagValueStr.c_str());

    pugi::xml_node voiceTagValueNode = deviceVoiceTagValueDoc.first_child();
    pugi::xml_node contactNode;
    VoiceList<std::string>::type NameList;
    VoiceList<std::string>::type voiceTagIDList;
    std::string tempContactName;
    while (!voiceTagValueNode.empty()) {

        std::string voiceTagID = voiceTagValueNode.name();
        VR_LOG("voiceTagID: %s", voiceTagID.c_str());
        if (std::string::npos != voiceTagID.find(VR_VOICETAG_ID_PREFIX)) {
            voiceTagIDList.push_back(std::string(voiceTagValueNode.name()).substr(std::string(VR_VOICETAG_ID_PREFIX).size()));
            contactNode = voiceTagValueNode.child("contact");
            if (contactNode) {
                tempContactName = contactNode.attribute("name").as_string();
                VR_LOG("NameList name : %s", tempContactName.c_str());
                NameList.push_back(tempContactName);
            }
        }
        else {
            deviceVoiceTagValueDoc.remove_child(voiceTagID.c_str());
        }

        voiceTagValueNode = voiceTagValueNode.next_sibling();
    }
    deviceVoiceTagValueStr.clear();
    storage.DeleteValue(key);
    VoiceList<std::string>::iterator it;
    for (it = voiceTagIDList.begin(); it != voiceTagIDList.end(); ++it) {
        VR_LOG("voiceTagIDList");
        VR_LOG("voiceTagIDList: %s", (*it).c_str());
    }
    VR_VoiceTagIDManager::getInstance()->deleteVoiceTagID(voiceTagIDList);
    if (voiceTagIDList.empty()) {
        return true;
    }
    strAddGrammar = "<grammar_voicetag agent=\"phone\" op=\"DeleteAllVoiceTagGrammar\"></grammar_voicetag>";
    VR_LOG("strAddGrammar : %s", strAddGrammar.c_str());

    return true;
}

bool
VR_VoiceBoxVoiceTag::DeleteVoiceTag(std::string &strAddGrammar, const std::string &deviceAddress, const VoiceList<std::string>::type &voiceTagIDList)
{
    VR_LOGD_FUNC();

    if (deviceAddress.empty()) {
        return false;
    }
    VR_LOG("DeleteVoiceTag");
    int key = VR_VoiceTagIDManager::getInstance()->getVoiceTagStorageKey(deviceAddress);

    std::string deviceVoiceTagValueStr;
    VR_VoiceBoxDataStorage storage;
    VR_LOG("Delete VoiceTag Begin");
    storage.GetValue(key, deviceVoiceTagValueStr);
    VR_LOG("VoiceTag Value: %s", deviceVoiceTagValueStr.c_str());

    pugi::xml_document deviceVoiceTagValueDoc;
    deviceVoiceTagValueDoc.load_string(deviceVoiceTagValueStr.c_str());
    std::string relativePath;
    pugi::xml_node contactNode;
    pugi::xml_node VoicetagIdNode;
    for (VoiceList<std::string>::const_iterator it = voiceTagIDList.cbegin(); it != voiceTagIDList.cend(); ++it) {
        VR_LOG("(*it): %s", (*it).c_str());
        relativePath = std::string("VR/VBT/") + VR_VOICETAG_REC_PATH + deviceAddress + "/" + (*it) + ".wav";
        VR_LOG("relativePath %s", relativePath.c_str());
        storage.RemoveFile(relativePath);

        VoicetagIdNode = deviceVoiceTagValueDoc.select_node((std::string("//") + std::string(VR_VOICETAG_ID_PREFIX) + (*it)).c_str()).node();
        if (VoicetagIdNode) {
            std::string name = VoicetagIdNode.name();
            VR_LOG("name :%s", name.c_str());
            contactNode = VoicetagIdNode.child(VR_VOICETAG_CONTACT_NODE);
            deviceVoiceTagValueDoc.remove_child((std::string(VR_VOICETAG_ID_PREFIX) + (*it)).c_str());
        }
    }

    std::ostringstream oss;
    deviceVoiceTagValueDoc.print(oss);
    deviceVoiceTagValueStr = oss.str();

    storage.PutValue(key, deviceVoiceTagValueStr);

    deviceVoiceTagValueStr.clear();
    storage.GetValue(key, deviceVoiceTagValueStr);
    VR_LOG("VoiceTag Value: %s", deviceVoiceTagValueStr.c_str());

    VR_VoiceTagIDManager::getInstance()->deleteVoiceTagID(voiceTagIDList);
    VR_LOG("Delete VoiceTag End");
    if (contactNode) {
        std::string strFullName = contactNode.attribute("name").as_string();
        VR_LOG("FullName: %s", strFullName.c_str());
        strAddGrammar = "<grammar_voicetag agent=\"phone\" op=\"addGrammar\"><fullName>";
        strAddGrammar.append(strFullName);
        strAddGrammar.append("</fullName><phoneme>");
        strAddGrammar.append("");
        strAddGrammar.append("</phoneme></grammar_voicetag>");
        VR_LOG("strAddGrammar : %s", strAddGrammar.c_str());
    }

    return true;
}

std::string
VR_VoiceBoxVoiceTag::GetVoiceTagPCMPath(const std::string &voiceTagID, const std::string &deviceAddress)
{
    VR_LOGD_FUNC();

    VR_LOG("deviceAddress : %s", deviceAddress.c_str());
    if (deviceAddress.empty()) {
        VR_ERROR("Current device is empty, can not get pcm path of VoiceTagID [%s]", voiceTagID.c_str());
        return std::string();
    }
    int key = VR_VoiceTagIDManager::getInstance()->getVoiceTagStorageKey(deviceAddress);

    std::string deviceVoiceTagValueStr;
    VR_VoiceBoxDataStorage storage;
    storage.GetValue(key, deviceVoiceTagValueStr);
    VR_LOG("VoiceTag Value: %s", deviceVoiceTagValueStr.c_str());

    pugi::xml_document deviceVoiceTagValueDoc;
    deviceVoiceTagValueDoc.load_string(deviceVoiceTagValueStr.c_str());

    pugi::xml_node voiceTagValueNode = deviceVoiceTagValueDoc.select_node((std::string("//") + VR_VOICETAG_ID_PREFIX + voiceTagID).c_str()).node();

    std::ostringstream oss;
    voiceTagValueNode.print(oss);
    VR_LOG("play VoiceTag Value : [%s]", oss.str().c_str());
    // std::string pcmPath = voiceTagValueNode.select_node((std::string("//") + VR_VOICETAG_PCM_PATH_NODE).c_str()).node().text().as_string();
    std::string pcmPath = voiceTagValueNode.child(VR_VOICETAG_PCM_PATH_NODE).text().as_string();
    VR_LOG("pcmPath: %s", pcmPath.c_str());

    return pcmPath;
}

/* EOF */
