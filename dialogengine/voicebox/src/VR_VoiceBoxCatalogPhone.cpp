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

/* Standard Header */
#include <sstream>
#include "VR_Log.h"

/* VBT Header */
#ifndef IVBTENGINECLIENT_H
#    include "IVBTEngineClient.h"
#endif

#ifndef VBT_SDK_AGENT_H
#    include "VBT_SDK_Agent.h"
#endif

#ifndef VECICSTR_H
#    include "VECICStr.h"
#endif

/* Suntec Header */
#ifndef VR_VOICEBOXCATALOGPHONE_H
#    include "VR_VoiceBoxCatalogPhone.h"
#endif

#ifndef VR_VOICEBOXXMLPARSER_H
#    include "VR_VoiceBoxXmlParser.h"
#endif

#ifndef VR_VOICEBOXLOG_H
#    include "VR_VoiceBoxLog.h"
#endif

#ifndef VR_CONFIGUREIF_H
#include "VR_ConfigureIF.h"
#endif

#ifndef VR_VOICETAGIDMANAGER_H
#include "VR_VoiceTagIDManager.h"
#endif

#ifndef VR_VOICEBOXDATASTORAGE_H
#    include "VR_VoiceBoxDataStorage.h"
#endif

#ifndef VR_VOICEBOXPHONEDATA_H
#    include "VR_VoiceBoxPhoneData.h"
#endif

using namespace nutshell;

#define VR_VOICETAG_DB_PATH     "voicetagDB/"
#define VR_VOICETAG_PCM_PATH    "pcm/"

// voicetag value node name

#define VR_VOICETAG_PHONEME_NODE    "phoneme"
#define VR_VOICETAG_TEMPDATA        "tempData"
#define VR_VOICETAG_ID_PREFIX       "VoiceTagID"

VoiceMap<std::string, std::string>::type VR_VoiceBoxCatalogPhone::m_mapSaveTransactionId;
VR_VoiceBoxCatalogPhone::VR_VoiceBoxCatalogPhone(
    IVECIEngineClient& client,
    VR_VoiceBoxEngineCallback& engineCallback
    )
: m_client(client)
, m_engineCallback(engineCallback)
, m_isDisactiveFromDE(false)
{

}

VR_VoiceBoxCatalogPhone::~VR_VoiceBoxCatalogPhone()
{
}

bool
VR_VoiceBoxCatalogPhone::Initialize()
{
    HRESULT result = m_client.CreateAgentDispatcher(&m_agentDataCommand);
    if (S_OK != result) {
        return false;
    }
    if (NULL == m_agentDataCommand.ptr()) {
        return false;
    }

    result = m_agentDataCommand->Init(VBT_AGENT_HFD);
    if (S_OK != result) {
        return false;
    }

    return true;
}

void
VR_VoiceBoxCatalogPhone::UnInitialize()
{
    m_mapSaveTransactionId.clear();
}

void
VR_VoiceBoxCatalogPhone::CreateVoiceTag()
{

}

void
VR_VoiceBoxCatalogPhone::UpdateVoiceTagGrammar(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    std::string strOp = parser.getValueByKey("op");

    VR_LOG("strOp : %s", strOp.c_str());

    if ("addGrammar" == strOp) {
        std::string strFullName = parser.getValueByKey("fullName");
        std::string strPhoneme = parser.getValueByKey("phoneme");
        VR_LOG("strFullName : %s", strFullName.c_str());
        VR_LOG("strPhoneme : %s", strPhoneme.c_str());
        RecompileTheGrammar(strFullName, strPhoneme);
    }
    else if ("DeleteAllVoiceTagGrammar" == strOp) {
        RecompileTheGrammar();
    }
}

bool
VR_VoiceBoxCatalogPhone::RecompileTheGrammar(const std::string &strFullName, const std::string &phoneme)
{
    VR_LOGD_FUNC();

    CVECIPtr<IVECITransaction> spTrans;
    if (NULL == m_agentDataCommand.ptr()) {
        VR_LOG("m_agentDataCommand is NULL");
        return false;
    }
    HRESULT retCode = m_agentDataCommand->SetDataSynchronized(&spTrans, _T(""), _T("HFDAgentContacts"), _T(""), VBT_FALSE);
    if (S_OK != retCode) {
        VR_LOG("SetDataSynchronized: 0x%x", retCode);
        return false;
    }

    CVECIPtr<IVECIListItems> contactList;
    (void)m_client.CreateListItems(&contactList);
    CVECIPtr<IVECIListItems> deleteContactList;
    (void)m_client.CreateListItems(&deleteContactList);
    // GetContactNodeInfo(contactNode, phoneme, deleteContactList, contactList);
    GetContactNodeInfo(strFullName, deleteContactList);

    retCode = m_agentDataCommand->RemoveData(&spTrans, _T(""), _T("HFDAgentContacts"), deleteContactList);
    if (S_OK != retCode) {
        VR_LOG("RemoveData: 0x%x", retCode);
        return false;
    }
    GetContactNodeInfo(strFullName, phoneme, contactList);
    retCode = m_agentDataCommand->AddData(&spTrans, _T(""), _T("HFDAgentContacts"), contactList);
    if (S_OK != retCode) {
        VR_LOG("AddData: 0x%x", retCode);
        return false;
    }

    retCode = m_agentDataCommand->SetDataSynchronized(&spTrans, _T(""), _T("HFDAgentContacts"), _T(""), VBT_TRUE);
    VR_LOG("RecompileTheGrammar2");
    if (S_OK != retCode) {
        VR_LOG("SetDataSynchronized: 0x%x", retCode);
        return false;
    }

    retCode = m_agentDataCommand->ReloadGrammar(&spTrans, _T(""),  _T("HFDAgentContacts"));
    if (S_OK != retCode) {
        VR_LOG("ReloadGrammar: 0x%x", retCode);
        return false;
    }

    CVECIOutStr strTrans;
    retCode = spTrans->GetTransactionId(&strTrans);
    if (FAILED(retCode)) {
        return false;
    }

    std::string strTransaction;
    if (NULL != strTrans.Get()) {
        strTransaction = strTrans.Get();
        m_engineCallback.SetCurrentTransaction("hfd", strTransaction);
    }

    return true;
}

bool
VR_VoiceBoxCatalogPhone::RecompileTheGrammar()
{
    VR_LOGD_FUNC();

    if (!m_vecContact.empty()) {
        CVECIPtr<IVECIListItems> contactList;
        (void)m_client.CreateListItems(&contactList);
        LoadContactInfo(m_vecContact, contactList);

        HRESULT retCode = m_agentDataCommand->SetDataSynchronized(NULL, _T(""), _T("HFDAgentContacts"), _T(""), VBT_FALSE);
        if (S_OK != retCode) {
            VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
            return false;
        }
        retCode = m_agentDataCommand->ReloadData(NULL, _T(""), _T("HFDAgentContacts"), contactList);
        if (S_OK != retCode) {
            VR_ERROR("ReloadData: 0x%lx", retCode);
            return false;
        }
        retCode = m_agentDataCommand->SetDataSynchronized(NULL, _T(""), _T("HFDAgentContacts"), _T(""), VBT_TRUE);
        if (S_OK != retCode) {
            VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
            return false;
        }

        CVECIPtr<IVECITransaction> spTrans;
        retCode = m_agentDataCommand->ReloadGrammar(&spTrans, _T(""),  _T("HFDAgentContacts"));
        if (S_OK != retCode) {
            VR_ERROR("ReloadGrammar: 0x%lx", retCode);
            return false;
        }

        if (NULL == spTrans.ptr()) {
            return false;
        }
        CVECIOutStr strTrans;
        retCode = spTrans->GetTransactionId(&strTrans);
        if (FAILED(retCode)) {
            return false;
        }
        std::string strTransaction;
        if (NULL != strTrans.Get()) {
            strTransaction = strTrans.Get();
            m_engineCallback.SetCurrentTransaction("hfd", strTransaction);
        }
        else {
            return false;
        }
        VR_LOG("grammar : %s", strTransaction.c_str());
    }
    else {
        m_engineCallback.SetUpdateGammarFlg(true);
        return false;
    }
    return true;
}

void
VR_VoiceBoxCatalogPhone::GetContactNodeInfo(std::string strFullName,
    CVECIPtr<IVECIListItems>& deleteContactList)
{
    VR_LOGD_FUNC();

    if ("" == strFullName) {
        strFullName = " ";
    }
    VR_LOG("strFullName: %s", strFullName.c_str());
    // pugi::xml_node phoneItemNode = contactNode.first_child();
    for (size_t i = 0; i < m_vecContact.size(); ++i) {
        CVECIPtr<IVECIParameterSet> spContact;
        CVECICStr cFullName(&m_client);
        PersonDetailInfo perInfo = m_vecContact[i];

        cFullName.Format(_T("%s %s"), perInfo.firstName.c_str(), perInfo.lastName.c_str());
        std::string contactName = perInfo.firstName + " " + perInfo.lastName;
        // std::string strFullName = cFullName.Get();

        if (strFullName == contactName) {
            VR_LOG("equal");
            (void)m_client.CreateParameterSet(&spContact);
            if (NULL == spContact) {
                return;
            }
            VR_LOG("strId : %s", perInfo.strId.c_str());
            (void)spContact->AddParameter(_T("nId"), perInfo.strId.c_str(), NULL, NULL);
            (void)deleteContactList->AddItem(spContact);
            // (void)contactList->AddItem(spContact);
        }
    }
    return;
}

void
VR_VoiceBoxCatalogPhone::GetContactNodeInfo(std::string strFullName, const std::string &phoneme,
    CVECIPtr<IVECIListItems>& contactList)
{
    VR_LOGD_FUNC();

    if ("" == strFullName) {
        strFullName = " ";
    }
    VR_LOG("strFullName: %s", strFullName.c_str());
    VR_LOG("phoneme: %s", phoneme.c_str());
    // pugi::xml_node phoneItemNode = contactNode.first_child();
    for (size_t i = 0; i < m_vecContact.size(); ++i) {
        CVECIPtr<IVECIParameterSet> spContact;
        CVECICStr cFullName(&m_client);
        PersonDetailInfo perInfo = m_vecContact[i];

        cFullName.Format(_T("%s %s"), perInfo.firstName.c_str(), perInfo.lastName.c_str());
        std::string contactName = perInfo.firstName + " " + perInfo.lastName;
        // std::string strFullName = cFullName.Get();
        if (strFullName == contactName) {
            VR_LOG("equal");
            (void)m_client.CreateParameterSet(&spContact);
            if (NULL == spContact) {
                return;
            }
            VR_LOG("strId : %s", perInfo.strId.c_str());
            (void)spContact->AddParameter(_T("nId"), perInfo.strId.c_str(), NULL, NULL);
            (void)spContact->AddParameter(_T("cFirstName"), perInfo.firstName.c_str(), NULL, NULL);
            (void)spContact->AddParameter(_T("cLastName"), perInfo.lastName.c_str(), NULL, NULL);
            (void)spContact->AddParameter(_T("cFullName"), cFullName.Get(), NULL, NULL);
            (void)spContact->AddParameter(_T("nPhoneType"), perInfo.phonetype.c_str(), NULL, NULL);
            (void)spContact->AddParameter(_T("cNumber"), perInfo.number.c_str(), NULL, NULL);
            (void)spContact->AddParameter(_T("cVoiceTag"), phoneme.c_str(), NULL, NULL);
            (void)contactList->AddItem(spContact);
        }
    }
    return;
}

VoiceVector<PersonDetailInfo>::type
VR_VoiceBoxCatalogPhone::GetContactInfo()
{
    return m_vecContact;
}

void
VR_VoiceBoxCatalogPhone::GetVoiceTagNodeInfo(VoiceList<std::string>::type NameList,
    CVECIPtr<IVECIListItems>& contactList)
{
    VR_LOGD_FUNC();

    for (size_t i = 0; i < m_vecContact.size(); ++i) {
        CVECIPtr<IVECIParameterSet> spContact;
        CVECICStr cFullName(&m_client);
        PersonDetailInfo perInfo = m_vecContact[i];

        cFullName.Format(_T("%s %s"), perInfo.firstName.c_str(), perInfo.lastName.c_str());
        std::string contactName = perInfo.firstName + perInfo.lastName;

        VoiceList<std::string>::const_iterator it;
        std::string tempName;
        for (it = NameList.cbegin(); it != NameList.cend(); ++it) {
            tempName = (*it);
            if (tempName == contactName) {
                VR_LOG("equal");
                (void)m_client.CreateParameterSet(&spContact);
                if (NULL == spContact) {
                    return;
                }
                (void)spContact->AddParameter(_T("nId"), perInfo.strId.c_str(), NULL, NULL);
                (void)spContact->AddParameter(_T("cFirstName"), perInfo.firstName.c_str(), NULL, NULL);
                (void)spContact->AddParameter(_T("cLastName"), perInfo.lastName.c_str(), NULL, NULL);
                (void)spContact->AddParameter(_T("cFullName"), cFullName.Get(), NULL, NULL);
                (void)spContact->AddParameter(_T("nPhoneType"), perInfo.phonetype.c_str(), NULL, NULL);
                (void)spContact->AddParameter(_T("cNumber"), perInfo.number.c_str(), NULL, NULL);
                (void)spContact->AddParameter(_T("cVoiceTag"), "", NULL, NULL);
                (void)contactList->AddItem(spContact);
            }
        }
    }

    return;
}

void
VR_VoiceBoxCatalogPhone::LoadContactInfo(const VoiceVector<PersonDetailInfo>::type& vecContact,
    CVECIPtr<IVECIListItems>& contactList)
{
    VR_LOGD_FUNC();

    VoiceMap<std::string, std::string>::type mapNametoPhoneme;
    // VoiceVector<StructNode>::type tempVector;
    // grammar
    VR_LOG("m_deviceAddress : %s", m_deviceAddress.c_str());
    std::string deviceVoiceTagValueStr;
    VR_VoiceBoxDataStorage storage;
    int key = 0;
    if ("" != m_deviceAddress) {
        key = VR_VoiceTagIDManager::getInstance()->getVoiceTagStorageKey(m_deviceAddress);
        VR_LOG("key: %d", key);
        storage.GetValue(key, deviceVoiceTagValueStr);
    }
    VR_LOG("VoiceTag Value: %s", deviceVoiceTagValueStr.c_str());
    pugi::xml_document deviceVoiceTagValueDoc;
    deviceVoiceTagValueDoc.load_string(deviceVoiceTagValueStr.c_str());

    pugi::xml_node voiceTagValueNode = deviceVoiceTagValueDoc.first_child();
    std::string phoneme;
    std::string tempData;
    std::string tempContactName;
    pugi::xml_node contactNode;
    while (!voiceTagValueNode.empty()) {
        phoneme.clear();
        tempData.clear();
        tempContactName.clear();

        std::string voiceTagID = voiceTagValueNode.name();
        VR_LOG("voiceTagID: %s", voiceTagID.c_str());
        if (std::string::npos != voiceTagID.find(VR_VOICETAG_ID_PREFIX)) {
            contactNode = voiceTagValueNode.child("contact");
            if (contactNode) {
                tempContactName = contactNode.attribute("name").as_string();
                if ("" == tempContactName) {
                    tempContactName = " ";
                }
            }
            if (voiceTagValueNode.child(VR_VOICETAG_TEMPDATA)) {
                tempData = voiceTagValueNode.child(VR_VOICETAG_TEMPDATA).text().as_string();
            }
            VR_LOG("tempContactName : %s", tempContactName.c_str());
            VR_LOG("tempData : %s", tempData.c_str());
            if (voiceTagValueNode.child(VR_VOICETAG_PHONEME_NODE)) {
                phoneme = voiceTagValueNode.child(VR_VOICETAG_PHONEME_NODE).text().as_string();
            }
            VR_LOG("phoneme : %s", phoneme.c_str());
            if ("false" == tempData) {
                mapNametoPhoneme.insert(std::make_pair(tempContactName, phoneme));
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
    VR_LOG("new voicetag: %s", deviceVoiceTagValueStr.c_str());
    storage.PutValue(key, deviceVoiceTagValueStr);


    std::string contactName;
    for (size_t i = 0; i < vecContact.size(); ++i) {
        CVECIPtr<IVECIParameterSet> spContact;
        CVECICStr cFullName(&m_client);
        PersonDetailInfo perInfo = vecContact[i];

        cFullName.Format(_T("%s %s"), perInfo.firstName.c_str(), perInfo.lastName.c_str());
        contactName = perInfo.firstName + " " + perInfo.lastName;
        (void)m_client.CreateParameterSet(&spContact);
        if (NULL == spContact) {
            return;
        }
        (void)spContact->AddParameter(_T("nId"), perInfo.strId.c_str(), NULL, NULL);
        (void)spContact->AddParameter(_T("cFirstName"), perInfo.firstName.c_str(), NULL, NULL);
        (void)spContact->AddParameter(_T("cLastName"), perInfo.lastName.c_str(), NULL, NULL);
        (void)spContact->AddParameter(_T("cFullName"), cFullName.Get(), NULL, NULL);
        (void)spContact->AddParameter(_T("nPhoneType"), perInfo.phonetype.c_str(), NULL, NULL);
        (void)spContact->AddParameter(_T("cNumber"), perInfo.number.c_str(), NULL, NULL);
        for (VoiceMap<std::string, std::string>::iterator it = mapNametoPhoneme.begin();
            it != mapNametoPhoneme.end(); ++it) {
            if (contactName == it->first) {
                VR_LOG("equal");
                (void)spContact->AddParameter(_T("cVoiceTag"), (it->second).c_str(), NULL, NULL);
                break;
            }
        }
        (void)contactList->AddItem(spContact);
    }

    return;
}

void
VR_VoiceBoxCatalogPhone::LoadMessageInfo(const VoiceVector<FormalTwoItem>::type& vecMessageInfo,
    CVECIPtr<IVECIListItems>& messageInfoList)
{
    VR_LOGD_FUNC();

    for (size_t i = 0; i < vecMessageInfo.size(); ++i) {
        CVECIPtr<IVECIParameterSet> spMessageType;
        (void)m_client.CreateParameterSet(&spMessageType);

        (void)spMessageType->AddParameter(_T("nId"), vecMessageInfo[i].strId.c_str(), NULL, NULL);
        (void)spMessageType->AddParameter(_T("value"), vecMessageInfo[i].name.c_str(), NULL, NULL);
        (void)messageInfoList->AddItem(spMessageType);
    }

    return;
}

void
VR_VoiceBoxCatalogPhone::LoadMsgOrPhoneType(const VoiceVector<FormalInfo>::type& vecMsgorPhoneType,
    CVECIPtr<IVECIListItems>& msgorPhoneTypeList)
{
    VR_LOGD_FUNC();

    for (size_t i = 0; i < vecMsgorPhoneType.size(); ++i) {

        for (size_t index = 0; index < vecMsgorPhoneType[i].aliasVector.size(); ++index) {
            CVECIPtr<IVECIParameterSet> spType;
            (void)m_client.CreateParameterSet(&spType);

            (void)spType->AddParameter(_T("nId"), vecMsgorPhoneType[i].formalItem.strId.c_str(), NULL, NULL);
            (void)spType->AddParameter(_T("cFormal"), vecMsgorPhoneType[i].formalItem.name.c_str(), NULL, NULL);
            (void)spType->AddParameter(_T("cAlias"), vecMsgorPhoneType[i].aliasVector[index].c_str(), NULL, NULL);
            (void)msgorPhoneTypeList->AddItem(spType);
        }
    }

    return;
}

void
VR_VoiceBoxCatalogPhone::ContactGrammer(const VoiceVector<PersonDetailInfo>::type& vecPersonInfo)
{
    VR_LOGD_FUNC();
    VR_LOGP("DE: update phone grammar... case : 212-138 212-140");


    if (!vecPersonInfo.empty()) {
        CVECIPtr<IVECIListItems> contactList;
        (void)m_client.CreateListItems(&contactList);
        LoadContactInfo(vecPersonInfo, contactList);
        UpdateHFDGrammer("HFDAgentContacts", contactList, "true");
    }
    else {
        VR_LOG("the phonebook is null");
        std::string strGrammar = GetGrammarName();
        m_engineCallback.SendGrammarResult(strGrammar, "phone", "1", "0");
        m_engineCallback.SetUpdateGammarFlg(true);
    }
}

bool
VR_VoiceBoxCatalogPhone::UpdateHFDQuickReplyMessages(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();
    VoiceVector<FormalTwoItem>::type vecMessageInfo;
    parser.getMessageInfo(vecMessageInfo);
    std::string strLastFlg = parser.getValueByKey("isLast");

    VR_LOGP("DE: update phone grammar... case : 212-138 212-140");
    if (!vecMessageInfo.empty()) {
        CVECIPtr<IVECIListItems> messageInfoList;
        (void)m_client.CreateListItems(&messageInfoList);
        LoadMessageInfo(vecMessageInfo, messageInfoList);

        UpdateHFDGrammer("HFDAgentQuickReplyMessages", messageInfoList, strLastFlg);
    }
    else {
        RemoveGrammer("HFDAgentQuickReplyMessages", strLastFlg);
    }
}

bool
VR_VoiceBoxCatalogPhone::UpdateHFDPhoneTypes(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();
    VoiceVector<FormalInfo>::type vecPhoneType;
    parser.getPhoneType(vecPhoneType);
    std::string strLastFlg = parser.getValueByKey("isLast");

    VR_LOGP("DE: update phone grammar... case : 212-138 212-140");
    if (!vecPhoneType.empty()) {
        CVECIPtr<IVECIListItems> phoneTypeList;
        (void)m_client.CreateListItems(&phoneTypeList);
        LoadMsgOrPhoneType(vecPhoneType, phoneTypeList);

        UpdateHFDGrammer("HFDAgentPhoneTypes", phoneTypeList, strLastFlg);
    }
    else {
        RemoveGrammer("HFDAgentPhoneTypes", strLastFlg);
    }

}

bool
VR_VoiceBoxCatalogPhone::UpdateHFDMessageTypes(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();
    VoiceVector<FormalInfo>::type vecMessageType;
    parser.getMessageType(vecMessageType);
    std::string strLastFlg = parser.getValueByKey("isLast");

    VR_LOGP("DE: update phone grammar... case : 212-138 212-140");


    if (!vecMessageType.empty()) {
        CVECIPtr<IVECIListItems> messageTypeList;
        (void)m_client.CreateListItems(&messageTypeList);
        LoadMsgOrPhoneType(vecMessageType, messageTypeList);

        UpdateHFDGrammer("HFDAgentMessageTypes", messageTypeList, strLastFlg);
    }
    else {
        RemoveGrammer("HFDAgentMessageTypes", strLastFlg);
    }
}

void
VR_VoiceBoxCatalogPhone::UpdateHFDGrammer(
    const std::string& strTableName, const CVECIPtr<IVECIListItems>& sourceList, const std::string& strLastFlg)
{
    VR_LOGD_FUNC();

    if (NULL == m_agentDataCommand.ptr() || "" == strTableName) {
        return;
    }
    VR_LOG("strTableName: %s", strTableName.c_str());

    HRESULT retCode = m_agentDataCommand->SetDataSynchronized(NULL, _T(""), strTableName.c_str(), _T(""), VBT_FALSE);
    if (S_OK != retCode) {
        VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
        return;
    }

    retCode = m_agentDataCommand->ReloadData(NULL, _T(""), strTableName.c_str(), sourceList);
    if (S_OK != retCode) {
        VR_ERROR("ReloadData: 0x%lx", retCode);
        return;
    }


    retCode = m_agentDataCommand->SetDataSynchronized(NULL, _T(""), strTableName.c_str(), _T(""), VBT_TRUE);
    if (S_OK != retCode) {
        VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
        return;
    }

    CVECIPtr<IVECITransaction> spTrans;
    retCode = m_agentDataCommand->ReloadGrammar(&spTrans, _T(""),  strTableName.c_str());
    if (S_OK != retCode) {
        VR_ERROR("ReloadGrammar: 0x%lx", retCode);
        return;
    }

    if (NULL == spTrans.ptr()) {
        return;
    }
    CVECIOutStr strTrans;
    retCode = spTrans->GetTransactionId(&strTrans);
    if (FAILED(retCode)) {
        return;
    }
    std::string strTransaction;
    if (NULL != strTrans.Get()) {
        strTransaction = strTrans.Get();

        if ("HFDAgentContacts" == strTableName) {
            std::string strGrammar = GetGrammarName();
            VR_LOG("strGrammar : %s", strGrammar.c_str());
            m_mapSaveTransactionId.insert(std::make_pair(strTransaction, strGrammar));
        }
        m_engineCallback.SetCurrentTransaction("hfd", strTransaction);
    }
    else {
        return;
    }
    VR_LOG("grammar : %s", strTransaction.c_str());
}

void
VR_VoiceBoxCatalogPhone::RemoveGrammer(const std::string& strTableName, const std::string& strLastFlg)
{
    VR_LOGD_FUNC();

    if (NULL == m_agentDataCommand.ptr() || "" == strTableName) {
        return;
    }
    VR_LOG("strTableName: %s", strTableName.c_str());

    HRESULT retCode = m_agentDataCommand->SetDataSynchronized(NULL, _T(""), strTableName.c_str(), _T(""), VBT_FALSE);
    if (S_OK != retCode) {
        VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
        return;
    }
    retCode = m_agentDataCommand->RemoveAllData(NULL, _T(""), strTableName.c_str());
    if (S_OK != retCode) {
        VR_ERROR("RemoveAllData: 0x%lx", retCode);
        return;
    }
    retCode = m_agentDataCommand->SetDataSynchronized(NULL, _T(""), strTableName.c_str(), _T(""), VBT_TRUE);
    if (S_OK != retCode) {
        VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
        return;
    }
    CVECIPtr<IVECITransaction> spTrans;
    retCode = m_agentDataCommand->RemoveAllGrammars(&spTrans, _T(""), strTableName.c_str());

    if (S_OK != retCode) {
        VR_ERROR("RemoveAllGrammars: 0x%lx", retCode);
        return;
    }

    if (NULL == spTrans.ptr()) {
        return;
    }

    CVECIOutStr strTrans;
    retCode = spTrans->GetTransactionId(&strTrans);
    if (FAILED(retCode)) {
        return;
    }

    std::string strTransaction;
    if (NULL != strTrans.Get()) {
        strTransaction = strTrans.Get();

        if (!m_isDisactiveFromDE) {
            if ("HFDAgentContacts" == strTableName) {
                std::string strGrammar = GetGrammarName();
                VR_LOG("strGrammar : %s", strGrammar.c_str());
                m_mapSaveTransactionId.insert(std::make_pair(strTransaction, strGrammar));
            }
        }
        m_engineCallback.SetCurrentTransaction("hfd", strTransaction);
    }
    else {
        return;
    }
     VR_LOG("grammar : %s", strTransaction.c_str());
}

void
VR_VoiceBoxCatalogPhone::SetupPhoneBookData(
    VR_VoiceBoxXmlParser& parser
    )
{
    std::string path = parser.getValueByKey("path");
    m_strGrammar = parser.getXmlKey();
    VR_VoiceBoxPhoneData phoneData;
    m_vecContact.clear();
    phoneData.OpenPhoneBookDB(path, m_vecContact);
    ContactGrammer(m_vecContact);

}

void
VR_VoiceBoxCatalogPhone::PhoneGrammarDisActive(
    VR_VoiceBoxXmlParser& parser
    )
{
    m_strGrammar = parser.getXmlKey();
    std::string sender = parser.getValueByKey("sender");
    if ("DE" == sender) {
        m_isDisactiveFromDE = true;
    }
    else {
        m_isDisactiveFromDE = false;
    }
    m_vecContact.clear();
    RemoveGrammer("HFDAgentContacts", "true");
}

void
VR_VoiceBoxCatalogPhone::InitialHFDPersonData()
{
    VR_LOGD_FUNC();

    m_isDisactiveFromDE = true;
    m_vecContact.clear();

    if (NULL == m_agentDataCommand.ptr()) {
        return;
    }

    HRESULT retCode = m_agentDataCommand->SetDataSynchronized(NULL, _T(""), "HFDAgentContacts", _T(""), VBT_FALSE);
    if (S_OK != retCode) {
        VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
        return;
    }
    retCode = m_agentDataCommand->RemoveAllData(NULL, _T(""), "HFDAgentContacts");
    if (S_OK != retCode) {
        VR_ERROR("RemoveAllData: 0x%lx", retCode);
        return;
    }
    retCode = m_agentDataCommand->SetDataSynchronized(NULL, _T(""), "HFDAgentContacts", _T(""), VBT_TRUE);
    if (S_OK != retCode) {
        VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
        return;
    }
    CVECIPtr<IVECITransaction> spTrans;
    retCode = m_agentDataCommand->RemoveAllGrammars(&spTrans, _T(""), "HFDAgentContacts");
    if (S_OK != retCode || NULL == spTrans.ptr()) {
        VR_ERROR("RemoveAllGrammars: 0x%lx", retCode);
        return;
    }

    CVECIOutStr strTrans;
    retCode = spTrans->GetTransactionId(&strTrans);
    if (FAILED(retCode)) {
        return;
    }

    std::string strTransaction;
    if (NULL != strTrans.Get()) {
        strTransaction = strTrans.Get();
        m_engineCallback.SetCurrentTransaction("hfd", strTransaction);
    }

    VBT_BOOL bTimeout = VBT_FALSE;
    retCode = spTrans->WaitForCompletion(INFINITE, &bTimeout);
    if (FAILED(retCode) || bTimeout) {
        VR_LOG("Wait uninstall failed: %xl", retCode);
        return;
    }

    m_engineCallback.SetUpdateGammarFlg(true);

    if (S_OK != retCode) {
        VR_ERROR("WaitForCompletion for RemoveAllGrammars: 0x%lx", retCode);
        return;
    }

    return;
}

VoiceMap<std::string, std::string>::type
VR_VoiceBoxCatalogPhone::GetTransactionIdMap()
{
    VR_LOGD_FUNC();

    return m_mapSaveTransactionId;
}

void
VR_VoiceBoxCatalogPhone::SetTransactionIdMap(VoiceMap<std::string, std::string>::type mapTransactionId)
{
    VR_LOGD_FUNC();

    m_mapSaveTransactionId = mapTransactionId;
}

void
VR_VoiceBoxCatalogPhone::SetPersonInfo(VoiceVector<PersonDetailInfo>::type vecContact)
{
    VR_LOGD_FUNC();
    m_vecContact.clear();
    m_vecContact = vecContact;
}

std::string
VR_VoiceBoxCatalogPhone::GetGrammarName()
{
    std::string strGrammar;
    if ("grammar_active" == m_strGrammar) {
        strGrammar = "active";
    }
    else if ("grammar_disactive" == m_strGrammar) {
        strGrammar = "disactive";
    }
    else if ("grammar_init" == m_strGrammar) {
        strGrammar = "grammar";
    }
    else {
        strGrammar = m_strGrammar;
    }
    return strGrammar;
}

void
VR_VoiceBoxCatalogPhone::SetDeviceAddress(const std::string deviceAddress)
{
    VR_LOGD_FUNC();

    VR_LOG("deviceAddress: %s", deviceAddress.c_str());
    this->m_deviceAddress = deviceAddress;
}

void
VR_VoiceBoxCatalogPhone::PauseGrammarPhone()
{
    VR_LOGD_FUNC();
    if (NULL == m_agentDataCommand.ptr()) {
        return;
    }

    std::pair<std::string, std::string> pairAgent2TransId = m_engineCallback.GetCurrentTransaction();
    if (pairAgent2TransId.first.empty()) {
        VR_LOG("grammar has updated finish");
        return;
    }

    CVECIPtr<IVECITransaction> spTrans;
    CVECIOutStr strTrans;
    HRESULT retCode =  m_agentDataCommand->PauseGrammarUpdate(&spTrans);
    if (FAILED(retCode)  || NULL == spTrans.ptr()) {
        return;
    }

    retCode = spTrans->GetTransactionId(&strTrans);
    if (FAILED(retCode)) {
        return;
    }

    if (NULL != strTrans.Get()) {
        std::string strTransaction = strTrans.Get();
        VR_LOG("strTransaction = %s", strTransaction.c_str());
    }
}

void
VR_VoiceBoxCatalogPhone::ResumeGrammarPhone()
{
    VR_LOGD_FUNC();
    if (NULL == m_agentDataCommand.ptr()) {
        return;
    }
    CVECIPtr<IVECITransaction> spTrans;
    CVECIOutStr strTrans;
    HRESULT retCode = m_agentDataCommand->ResumeGrammarUpdate(&spTrans);
    if (FAILED(retCode)  || NULL == spTrans.ptr()) {
        return;
    }

    retCode = spTrans->GetTransactionId(&strTrans);
    if (FAILED(retCode)) {
        return;
    }

    if (NULL != strTrans.Get()) {
        std::string strTransaction = strTrans.Get();
        VR_LOG("strTransaction = %s", strTransaction.c_str());
    }
}

void
VR_VoiceBoxCatalogPhone::CancelGrammarUpdate()
{
    if (NULL == m_agentDataCommand.ptr()) {
        return;
    }

    CVECIPtr<IVECITransaction> spTrans;
    CVECIOutStr strTrans;
    HRESULT retCode = m_agentDataCommand->CancelGrammarUpdate(&spTrans);
    if (FAILED(retCode)) {
        return;
    }

    if (NULL != spTrans.ptr()) {
        retCode = spTrans->GetTransactionId(&strTrans);
        if (FAILED(retCode)) {
            return;
        }
        VBT_BOOL bSaveTimeout = VBT_FALSE;
        retCode = spTrans->WaitForCompletion(INFINITE, &bSaveTimeout);
        if (FAILED(retCode) || bSaveTimeout) {
            VR_LOG("CancelGrammarUpdate failed: %lx", retCode);
        }

        if (NULL != strTrans.Get()) {
            std::string strTransaction = strTrans.Get();
            VR_LOG("strTransaction = %s", strTransaction.c_str());
        }
    }
}
/* EOF */
