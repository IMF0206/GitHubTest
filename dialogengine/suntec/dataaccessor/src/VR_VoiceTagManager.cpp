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

#include "VR_VoiceTagManager.h"
#include "VR_VoiceTagIDManager.h"

#include "VR_ConfigureIF.h"
#include "VR_Log.h"
#include "config.h"

#include "BL_Dir.h"
#include "BL_File.h"
#include "BL_DataStorage.h"

#include <sstream>

// voicetag value node name
#define VR_VOICETAG_REC_PATH_NODE   "recPath"
#define VR_VOICETAG_PHONEME_NODE    "phoneme"
#define VR_VOICETAG_CONTACT_NODE    "contact"

#define VR_VOICETAG_REC_PATH        "rec/"
#define VR_VOICETAG_ID_PREFIX       "VoiceTagID"


bool VR_VoiceTagManager::saveVoiceTag(const std::string &deviceAddress, const std::string &voiceTagID, const std::string &pcmPath, const std::string &phoneme, const std::string &contactMsg, bool update)
{
    if (m_currentDeviceAddress != deviceAddress) {
        VR_LOG("Warning: current device is [%s] and voiceTag save to device [%s]", m_currentDeviceAddress.c_str(), deviceAddress.c_str());
    }

    if (deviceAddress.empty()) {
        return false;
    }

    // get contact node
    pugi::xml_document contactDoc;
    contactDoc.load_string(contactMsg.c_str());
    pugi::xml_node contactNode = contactDoc.select_node((std::string("//") + VR_VOICETAG_CONTACT_NODE).c_str()).node();
    std::string name = contactNode.attribute("name").as_string();
    contactNode.append_attribute("asrText").set_value(name.c_str());

    std::string contactID = contactNode.attribute("contactID").as_string();
    if (!contactID.empty()) {
        m_voiceTagContactIDMap[voiceTagID] = contactID;
        contactNode.remove_attribute("contactID");
    }

    // move pcmfile to new path
    std::string dirPath = VR_VOICETAG_REC_PATH;
    m_configure->makeDir(dirPath);
    dirPath += deviceAddress;
    m_configure->makeDir(dirPath);
    std::string fileName = dirPath +  "/" + voiceTagID + ".wav";
    std::string fullPath(m_configure->getUsrPath() + fileName);
    VR_LOG("pcmfile path : %s", fullPath.c_str());

    if (m_configure->isFileExist(fileName)) {
        bool isRemoved = m_configure->removeFile(fileName);
        VR_LOG("Remove File: %s, %s", fullPath.c_str(), isRemoved ? "true" : "false");
    }
    m_configure->moveFile(pcmPath, fileName);

    // create voicetag value
    pugi::xml_document voiceTagValueDoc;
    voiceTagValueDoc.load_string("");
    pugi::xml_node voiceTagValueNode = voiceTagValueDoc.append_child((std::string(VR_VOICETAG_ID_PREFIX) + voiceTagID).c_str());
    voiceTagValueNode.append_child(VR_VOICETAG_REC_PATH_NODE).text().set(fullPath.c_str());
    voiceTagValueNode.append_child(VR_VOICETAG_PHONEME_NODE).text().set(phoneme.c_str());
    voiceTagValueNode.append_copy(contactNode);

    // save voicetag
    BL_DATA_KEY_NAME key = BL_DATA_KEY_NAME(VR_VoiceTagIDManager::getInstance()->getVoiceTagStorageKey(deviceAddress));
    if (BL_DATA_KEY_VR_END == key) {
        VR_ERROR("can not get correct voicetagid");
        return false;
    }

    std::string deviceVoiceTagValueStr;
    BL_DataStorage storage;
    BL_DataValue dataValue;
    VR_LOGD("Save VoiceTag Begin");
    if (storage.GetValue(key, dataValue)) {
        VR_LOGD("GetValue OK");
    }
    else {
        VR_LOGD("GetValue Failed");
    }
    const void * dataPoint = dataValue.GetData();
    if (nullptr != dataPoint) {
        deviceVoiceTagValueStr.assign((char*)dataPoint);
    }
    VR_LOGD("VoiceTag Value: %s", deviceVoiceTagValueStr.c_str());
    VR_LOG_TO_FILE("VoiceTag Value:", deviceVoiceTagValueStr);

    pugi::xml_document deviceVoiceTagValueDoc;
    deviceVoiceTagValueDoc.load_string(deviceVoiceTagValueStr.c_str());

    // remove the last one
    VoiceList<std::pair<std::string, std::string>>::type deleteList;
    std::string asrText = deviceVoiceTagValueDoc.child((std::string(VR_VOICETAG_ID_PREFIX) + voiceTagID).c_str()).child(VR_VOICETAG_CONTACT_NODE).attribute("asrText").as_string();
    if (!asrText.empty()) {
        deleteList.push_back(std::pair<std::string, std::string>(voiceTagID, asrText));
        deleteVoiceTagGrammar(deleteList, deviceAddress);
    }
    deviceVoiceTagValueDoc.remove_child((std::string(VR_VOICETAG_ID_PREFIX) + voiceTagID).c_str());

    deviceVoiceTagValueDoc.append_copy(voiceTagValueNode);

    std::ostringstream oss;
    deviceVoiceTagValueDoc.print(oss);
    deviceVoiceTagValueStr = oss.str();
    VR_LOGD("VoiceTag Value: %s", deviceVoiceTagValueStr.c_str());
    VR_LOG_TO_FILE("VoiceTag Value:", deviceVoiceTagValueStr);

    if (storage.PutValue(key, (void*)(deviceVoiceTagValueStr.c_str()), deviceVoiceTagValueStr.size() + 1)) {
        VR_LOGD("PutValue OK");
    }
    else {
        VR_LOGD("PutValue Failed");
    }
    VR_VoiceTagIDManager::getInstance()->saveVoiceTagID(voiceTagID);
    VR_LOGD("Save VoiceTag End");

    // generate grammar
    return saveVoiceTagGrammar(voiceTagID, name, phoneme, deviceAddress);
}

std::string VR_VoiceTagManager::getVoiceTagPCMPath(const std::string &voiceTagID)
{
    if (m_currentDeviceAddress.empty()) {
        VR_ERROR("Current device is empty, can not get pcm path of VoiceTagID [%s]", voiceTagID.c_str());
        return std::string();
    }
    BL_DATA_KEY_NAME key = BL_DATA_KEY_NAME(VR_VoiceTagIDManager::getInstance()->getVoiceTagStorageKey(m_currentDeviceAddress));
    if (BL_DATA_KEY_VR_END == key) {
        VR_ERROR("can not get correct voicetagid");
        return std::string();
    }

    std::string deviceVoiceTagValueStr;
    BL_DataStorage storage;
    BL_DataValue dataValue;
    storage.GetValue(key, dataValue);
    const void * dataPoint = dataValue.GetData();
    if (nullptr != dataPoint) {
        deviceVoiceTagValueStr.assign((char*)dataPoint);
    }

    VR_LOGD("VoiceTag Value: %s", deviceVoiceTagValueStr.c_str());

    pugi::xml_document deviceVoiceTagValueDoc;
    deviceVoiceTagValueDoc.load_string(deviceVoiceTagValueStr.c_str());

    pugi::xml_node voiceTagValueNode = deviceVoiceTagValueDoc.select_node((std::string("//") + VR_VOICETAG_ID_PREFIX + voiceTagID).c_str()).node();
    std::ostringstream oss;
    voiceTagValueNode.print(oss);
    VR_LOGD("play VoiceTag Value : [%s]", oss.str().c_str());
    std::string pcmPath = voiceTagValueNode.child(VR_VOICETAG_REC_PATH_NODE).text().as_string();

    VR_LOG("pcmPath: %s", pcmPath.c_str());

    return pcmPath;
}

bool VR_VoiceTagManager::deleteVoiceTag(const std::string &deviceAddress, const std::string &voiceTagID)
{
    VR_LOGD_FUNC();
    if (m_currentDeviceAddress != deviceAddress) {
        VR_LOG("Warning: current device is [%s] and voiceTag delete from device [%s]", m_currentDeviceAddress.c_str(), deviceAddress.c_str());
    }

    if (deviceAddress.empty()) {
        return false;
    }

    BL_DATA_KEY_NAME key = BL_DATA_KEY_NAME(VR_VoiceTagIDManager::getInstance()->getVoiceTagStorageKey(deviceAddress));
    if (BL_DATA_KEY_VR_END == key) {
        VR_ERROR("can not get correct voicetagid");
        return false;
    }

    std::string deviceVoiceTagValueStr;
    BL_DataStorage storage;
    BL_DataValue dataValue;
    VR_LOGD("Delete VoiceTag Begin");
    storage.GetValue(key, dataValue);
    const void * dataPoint = dataValue.GetData();
    if (nullptr != dataPoint) {
        deviceVoiceTagValueStr.assign((char*)dataPoint);
    }

    VR_LOGD("VoiceTag Value: %s", deviceVoiceTagValueStr.c_str());
    VR_LOG_TO_FILE("VoiceTag Value:", deviceVoiceTagValueStr);

    pugi::xml_document deviceVoiceTagValueDoc;
    deviceVoiceTagValueDoc.load_string(deviceVoiceTagValueStr.c_str());

    VoiceList<std::pair<std::string, std::string>>::type deleteList;

    std::string relativePath = VR_VOICETAG_REC_PATH + deviceAddress + "/" + voiceTagID + ".wav";
    m_configure->removeFile(relativePath);

    std::string asrText = deviceVoiceTagValueDoc.child((std::string(VR_VOICETAG_ID_PREFIX) + voiceTagID).c_str()).child(VR_VOICETAG_CONTACT_NODE).attribute("asrText").as_string();
    deviceVoiceTagValueDoc.remove_child((std::string(VR_VOICETAG_ID_PREFIX) + voiceTagID).c_str());
    deleteList.push_back(std::pair<std::string, std::string>(voiceTagID, asrText));

    std::ostringstream oss;
    deviceVoiceTagValueDoc.print(oss);
    deviceVoiceTagValueStr = oss.str();
    VR_LOG_TO_FILE("VoiceTag Value:", deviceVoiceTagValueStr);

    storage.PutValue(key, (void*)(deviceVoiceTagValueStr.c_str()), deviceVoiceTagValueStr.size() + 1);
    VoiceList<std::string>::type voiceTagIDList;
    voiceTagIDList.push_back(voiceTagID);
    VR_VoiceTagIDManager::getInstance()->deleteVoiceTagID(voiceTagIDList);
    VR_LOGD("Delete VoiceTag End");

    return deleteVoiceTagGrammar(deleteList, deviceAddress);
}

bool VR_VoiceTagManager::deleteAllVoiceTag(const std::string &deviceAddress)
{
    if (m_currentDeviceAddress != deviceAddress) {
        VR_LOG("Warning: current device is [%s] and voiceTag delete from device [%s]", m_currentDeviceAddress.c_str(), deviceAddress.c_str());
    }

    if (deviceAddress.empty()) {
        return false;
    }

    std::string pcmDir = VR_VOICETAG_REC_PATH + deviceAddress;
    m_configure->removeDir(pcmDir);

    BL_DATA_KEY_NAME key = BL_DATA_KEY_NAME(VR_VoiceTagIDManager::getInstance()->getVoiceTagStorageKey(deviceAddress));
    if (BL_DATA_KEY_VR_END == key) {
        VR_ERROR("can not get correct voicetagid");
        return false;
    }
    BL_DataStorage storage;
    BL_DataValue dataValue;
    std::string deviceVoiceTagValueStr;
    storage.GetValue(key, dataValue);
    const void * dataPoint = dataValue.GetData();
    if (nullptr != dataPoint) {
        deviceVoiceTagValueStr.assign((char*)dataPoint);
    }
    VR_LOGD("VoiceTag Value: %s", deviceVoiceTagValueStr.c_str());
    VR_LOG_TO_FILE("VoiceTag Value:", deviceVoiceTagValueStr);

    pugi::xml_document deviceVoiceTagValueDoc;
    deviceVoiceTagValueDoc.load_string(deviceVoiceTagValueStr.c_str());
    pugi::xml_node tempNode = deviceVoiceTagValueDoc.first_child();
    VoiceList<std::string>::type voiceTagIDList;
    while (!tempNode.empty()) {
        voiceTagIDList.push_back(std::string(tempNode.name()).substr(std::string(VR_VOICETAG_ID_PREFIX).size()));
        tempNode = tempNode.next_sibling();
    }
    deviceVoiceTagValueStr.clear();

    storage.DeleteValue(key);

    VR_VoiceTagIDManager::getInstance()->deleteVoiceTagID(voiceTagIDList);

    return deleteAllVoiceTagGrammar(deviceAddress);
}

bool VR_VoiceTagManager::clearVoiceTag()
{
    m_configure->removeDir(VR_VOICETAG_REC_PATH);

    BL_DataStorage storage;
    storage.DeleteValue(BL_DATA_KEY_VR_VOICETAG_1);
    storage.DeleteValue(BL_DATA_KEY_VR_VOICETAG_2);
    storage.DeleteValue(BL_DATA_KEY_VR_VOICETAG_3);
    storage.DeleteValue(BL_DATA_KEY_VR_VOICETAG_4);
    storage.DeleteValue(BL_DATA_KEY_VR_VOICETAG_5);

    VR_VoiceTagIDManager::getInstance()->clearVoiceTagID();

    return clearVoiceTagGrammar();
}

bool VR_VoiceTagManager::syncVoiceTag(const std::string &deviceAddress, const std::string &btContactIDStr, std::string &btVoiceTagIDStr)
{
    VoiceList<std::string>::type btVoiceTagIDList;
    VoiceList<std::string>::type btContactIDList;
    getIDListFromString(btVoiceTagIDStr, btVoiceTagIDList);
    getIDListFromString(btContactIDStr, btContactIDList);

    initVoiceTagContactIDMap(btVoiceTagIDList, btContactIDList);

    if (btVoiceTagIDList.empty()) {
        return deleteAllVoiceTag(deviceAddress);
    }

    VoiceList<std::string>::type voiceTagIDList;
    if (!getVoiceTagIDList(deviceAddress, voiceTagIDList)) {
        return false;
    }

    btVoiceTagIDList.sort();
    voiceTagIDList.sort();

    VoiceList<std::string>::iterator btIt = btVoiceTagIDList.begin();
    VoiceList<std::string>::iterator it = voiceTagIDList.begin();
    for (; btIt != btVoiceTagIDList.end() && it != voiceTagIDList.end();) {
        if (*btIt < *it) {
            ++btIt;
        }
        else if (*it < *btIt) {
            ++it;
        }
        else {
            btIt = btVoiceTagIDList.erase(btIt);
            it = voiceTagIDList.erase(it);
        }
    }

    it = voiceTagIDList.begin();
    while (it != voiceTagIDList.end()) {
        deleteVoiceTag(deviceAddress, *it);
        ++it;
    }

    // return the voicetagIds that VR do not have
    btVoiceTagIDStr.clear();
    btIt = btVoiceTagIDList.begin();
    while (btVoiceTagIDList.end() != btIt) {
        btVoiceTagIDStr += (*btIt + ",");
        ++btIt;
    }
    if (!btVoiceTagIDStr.empty()) {
        btVoiceTagIDStr.pop_back();
    }

    return true;
}

bool VR_VoiceTagManager::updateVoiceTag(const std::string &voiceTagID, const pugi::xml_node &contactNode)
{
    if (m_currentDeviceAddress.empty()) {
        VR_ERROR("Current device is empty");
        return false;
    }

    BL_DATA_KEY_NAME key = BL_DATA_KEY_NAME(VR_VoiceTagIDManager::getInstance()->getVoiceTagStorageKey(m_currentDeviceAddress));
    if (BL_DATA_KEY_VR_END == key) {
        VR_ERROR("can not get correct voicetag BL_Data Key");
        return false;
    }
    BL_DataStorage storage;
    BL_DataValue dataValue;
    std::string deviceVoiceTagValueStr;
    storage.GetValue(key, dataValue);
    const void * dataPoint = dataValue.GetData();
    if (nullptr != dataPoint) {
        deviceVoiceTagValueStr.assign((char*)dataPoint);
    }
    VR_LOGD("VoiceTag Value: %s", deviceVoiceTagValueStr.c_str());
    VR_LOG_TO_FILE("VoiceTag Value:", deviceVoiceTagValueStr);

    pugi::xml_document deviceVoiceTagValueDoc;
    deviceVoiceTagValueDoc.load_string(deviceVoiceTagValueStr.c_str());

    pugi::xml_node currentVoiceTagNode = deviceVoiceTagValueDoc.child((std::string(VR_VOICETAG_ID_PREFIX) + voiceTagID).c_str());
    if (currentVoiceTagNode.empty()) {
        VR_ERROR("Current device [%s] do not have the voicetag [%s]", m_currentDeviceAddress.c_str(), voiceTagID.c_str());
        return false;
    }
    std::string asrText = currentVoiceTagNode.child(VR_VOICETAG_CONTACT_NODE).attribute("asrText").as_string();
    currentVoiceTagNode.remove_child(VR_VOICETAG_CONTACT_NODE);
    pugi::xml_node newContactNode = currentVoiceTagNode.append_copy(contactNode);
    if (!asrText.empty()) {
        newContactNode.attribute("asrText").set_value(asrText.c_str());
    }

    std::ostringstream oss;
    deviceVoiceTagValueDoc.print(oss);
    deviceVoiceTagValueStr = oss.str();
    VR_LOGD("VoiceTag Value: %s", deviceVoiceTagValueStr.c_str());
    VR_LOG_TO_FILE("VoiceTag Value:", deviceVoiceTagValueStr);

    if (storage.PutValue(key, (void*)(deviceVoiceTagValueStr.c_str()), deviceVoiceTagValueStr.size() + 1)) {
        VR_LOGD("PutValue OK");
    }
    else {
        VR_LOGD("PutValue Failed");
    }
    return true;
}

void VR_VoiceTagManager::setCurrentDevice(const std::string &deviceAddress)
{
    if (deviceAddress.empty()) {
        return;
    }
    m_currentDeviceAddress = deviceAddress;
}

void VR_VoiceTagManager::clearCurrentDevice()
{
    m_currentDeviceAddress.clear();
}

std::string VR_VoiceTagManager::getContactInfo(const std::string &voiceTagID)
{
    std::string response;
    if (m_currentDeviceAddress.empty()) {
        VR_ERROR("Current device is empty, can not get voiceTag grammar param");
        return response;
    }

    BL_DATA_KEY_NAME key = BL_DATA_KEY_NAME(VR_VoiceTagIDManager::getInstance()->getVoiceTagStorageKey(m_currentDeviceAddress));
    if (BL_DATA_KEY_VR_END == key) {
        VR_ERROR("can not get correct voicetagid");
        return response;
    }
    BL_DataStorage storage;
    BL_DataValue dataValue;
    std::string deviceVoiceTagValueStr;
    storage.GetValue(key, dataValue);
    const void * dataPoint = dataValue.GetData();
    if (nullptr != dataPoint) {
        deviceVoiceTagValueStr.assign((char*)dataPoint);
    }
    VR_LOGD("VoiceTag Value: %s", deviceVoiceTagValueStr.c_str());

    pugi::xml_document deviceVoiceTagValueDoc;
    deviceVoiceTagValueDoc.load_string(deviceVoiceTagValueStr.c_str());
    pugi::xml_node voiceTagIDNode = deviceVoiceTagValueDoc.select_node((std::string("//") + VR_VOICETAG_ID_PREFIX + voiceTagID).c_str()).node();
    pugi::xml_node contactNode;
    if (!voiceTagIDNode.empty()) {
        contactNode = voiceTagIDNode.child(VR_VOICETAG_CONTACT_NODE);
    }
    else {
        return response;
    }

    std::ostringstream oss;
    contactNode.print(oss);
    response = oss.str();
    return response;
}

int VR_VoiceTagManager::getVoiceTagCount()
{
    if (m_currentDeviceAddress.empty()) {
        VR_ERROR("Current device is empty, can not get voiceTag grammar param");
        return 0;
    }

    BL_DATA_KEY_NAME key = BL_DATA_KEY_NAME(VR_VoiceTagIDManager::getInstance()->getVoiceTagStorageKey(m_currentDeviceAddress));
    if (BL_DATA_KEY_VR_END == key) {
        VR_ERROR("can not get correct voicetagid");
        return 0;
    }
    BL_DataStorage storage;
    BL_DataValue dataValue;
    std::string deviceVoiceTagValueStr;
    storage.GetValue(key, dataValue);
    const void * dataPoint = dataValue.GetData();
    if (nullptr != dataPoint) {
        deviceVoiceTagValueStr.assign((char*)dataPoint);
    }
    VR_LOGD("VoiceTag Value: %s", deviceVoiceTagValueStr.c_str());
    pugi::xml_document deviceVoiceTagValueDoc;
    deviceVoiceTagValueDoc.load_string(deviceVoiceTagValueStr.c_str());
    pugi::xml_node tempNode = deviceVoiceTagValueDoc.first_child();
    int count = 0;
    while (!tempNode.empty()) {
        ++count;
        tempNode = tempNode.next_sibling();
    }
    return count;
}

void VR_VoiceTagManager::getVoiceTagGrammarParam(VoiceList<VoiceTagGrammarParam>::type &paramList)
{
    VR_LOGD_FUNC();
    if (m_currentDeviceAddress.empty()) {
        VR_ERROR("Current device is empty, can not get voiceTag grammar param");
        return;
    }

    BL_DATA_KEY_NAME key = BL_DATA_KEY_NAME(VR_VoiceTagIDManager::getInstance()->getVoiceTagStorageKey(m_currentDeviceAddress));

    if (BL_DATA_KEY_VR_END == key) {
        VR_ERROR("can not get correct voicetagid");
        return;
    }

    std::string deviceVoiceTagValueStr;
    BL_DataStorage storage;
    BL_DataValue dataValue;
    storage.GetValue(key, dataValue);
    const void * dataPoint = dataValue.GetData();
    if (nullptr != dataPoint) {
        deviceVoiceTagValueStr.assign((char*)dataPoint);
    }

    pugi::xml_document deviceVoiceTagValueDoc;
    deviceVoiceTagValueDoc.load_string(deviceVoiceTagValueStr.c_str());

    pugi::xml_node voiceTagValueNode = deviceVoiceTagValueDoc.first_child();

    while (!voiceTagValueNode.empty()) {
        VoiceTagGrammarParam param;
        param.voiceTagID = std::string(voiceTagValueNode.name()).substr(std::string(VR_VOICETAG_ID_PREFIX).size());
        param.contactName = voiceTagValueNode.child(VR_VOICETAG_CONTACT_NODE).attribute("name").as_string();
        param.phoneme = voiceTagValueNode.child(VR_VOICETAG_PHONEME_NODE).text().as_string();
        paramList.push_back(param);

        voiceTagValueNode = voiceTagValueNode.next_sibling();
    }
}

std::string VR_VoiceTagManager::getContactIDByVoiceTagID(const std::string &voiceTagID)
{
    VR_LOGD_FUNC();
    VoiceMap<std::string, std::string>::iterator it = m_voiceTagContactIDMap.find(voiceTagID);
    if (it != m_voiceTagContactIDMap.end()) {
        return it->second;
    }
    else {
        return std::string();
    }
}

std::string VR_VoiceTagManager::getVoiceTagIDByContactID(const std::string &contactID)
{
    VR_LOGD_FUNC();
    VoiceMap<std::string, std::string>::const_iterator it = m_voiceTagContactIDMap.cbegin();
    while (it != m_voiceTagContactIDMap.cend()) {
        if (it->second == contactID) {
            return it->first;
        }
        ++it;
    }
    return std::string();
}


bool VR_VoiceTagManager::getVoiceTagIDList(const std::string &deviceAddress, VoiceList<std::string>::type &voiceTagIDList)
{
    VR_LOGD_FUNC();

    BL_DATA_KEY_NAME key = BL_DATA_KEY_NAME(VR_VoiceTagIDManager::getInstance()->getVoiceTagStorageKey(deviceAddress));
    if (BL_DATA_KEY_VR_END == key) {
        VR_ERROR("can not get correct voicetagid");
        return false;
    }
    BL_DataStorage storage;
    BL_DataValue dataValue;
    std::string deviceVoiceTagValueStr;
    storage.GetValue(key, dataValue);
    const void * dataPoint = dataValue.GetData();
    if (nullptr != dataPoint) {
        deviceVoiceTagValueStr.assign((char*)dataPoint);
    }
    VR_LOGD("VoiceTag Value: %s", deviceVoiceTagValueStr.c_str());
    VR_LOG_TO_FILE("VoiceTag Value:", deviceVoiceTagValueStr);

    pugi::xml_document deviceVoiceTagValueDoc;
    deviceVoiceTagValueDoc.load_string(deviceVoiceTagValueStr.c_str());
    pugi::xml_node tempNode = deviceVoiceTagValueDoc.first_child();
    size_t prefixSize = std::string(VR_VOICETAG_ID_PREFIX).size();
    while (!tempNode.empty()) {
        std::string nodeName(tempNode.name());
        if (nodeName.size() > prefixSize) {
            voiceTagIDList.push_back(std::string(tempNode.name()).substr(prefixSize));
        }
        else {
            VR_ERROR("voicetag Node not correct");
        }
        tempNode = tempNode.next_sibling();
    }

    return true;
}

bool VR_VoiceTagManager::getIDListFromString(const std::string &IDStr, VoiceList<std::string>::type &IDList)
{
    if (IDStr.empty()) {
        return false;
    }

    size_t startPos = 0;
    size_t pos = IDStr.find(",");
    while (std::string::npos != pos) {
        IDList.push_back(IDStr.substr(startPos, pos - startPos));
        startPos = pos + 1;
        pos = IDStr.find(",", startPos);
    }
    IDList.push_back(IDStr.substr(startPos));
    return true;
}

void VR_VoiceTagManager::initVoiceTagContactIDMap(const VoiceList<std::string>::type &voiceTagIDList, const VoiceList<std::string>::type &contactIDList)
{
    VR_LOGD_FUNC();
    m_voiceTagContactIDMap.clear();

    if (voiceTagIDList.empty() || contactIDList.empty()) {
        return;
    }

    VoiceList<std::string>::const_iterator voiceTagIt = voiceTagIDList.cbegin();
    VoiceList<std::string>::const_iterator contactIt = contactIDList.cbegin();

    while (voiceTagIt != voiceTagIDList.cend() && contactIt != contactIDList.cend()) {
        m_voiceTagContactIDMap[*voiceTagIt] = *contactIt;
        VR_LOGD("voiceTag ID: %s, contact ID: %s", voiceTagIt->c_str(), contactIt->c_str());
        ++voiceTagIt;
        ++contactIt;
    }
}

/* EOF */