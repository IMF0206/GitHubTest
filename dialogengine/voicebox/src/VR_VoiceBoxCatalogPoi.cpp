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
#include <fstream>

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
#ifndef VR_VOICEBOXCATALOGAPPS_H
#    include "VR_VoiceBoxCatalogPoi.h"
#endif

#ifndef VR_VOICEBOXLOG_H
#    include "VR_VoiceBoxLog.h"
#endif

#ifndef VR_CONFIGUREIF_H
#    include "VR_ConfigureIF.h"
#endif

VR_VoiceBoxCatalogPoi::VR_VoiceBoxCatalogPoi(
    IVECIEngineClient& client,
    VR_VoiceBoxEngineCallback& engineCallback
    )
: m_client(client)
, m_engineCallback(engineCallback)
{
}

VR_VoiceBoxCatalogPoi::~VR_VoiceBoxCatalogPoi()
{
}

bool
VR_VoiceBoxCatalogPoi::Initialize()
{
    VR_LOGD_FUNC();

    HRESULT result = m_client.CreateAgentDispatcher(&m_agentDataCommandPoi);
    if (S_OK != result) {
        return false;
    }

    if (NULL == m_agentDataCommandPoi.ptr()) {
        return false;
    }

    result = m_agentDataCommandPoi->Init("POI");
    if (S_OK != result) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxCatalogPoi::Cataloguing(
    VR_VoiceBoxXmlParser& parser
    )
{
    VR_LOGD_FUNC();

    if (NULL == m_agentDataCommandPoi.ptr()) {
        return false;
    }

    CVECIPtr<IVECITransaction> spTrans;

    HRESULT retCode = m_agentDataCommandPoi->SetDataSynchronized(NULL, _T(""), _T(""),
        _T(""), VBT_FALSE);
    if (S_OK != retCode) {
        return false;
    }

    if (!ReadPoiData()) {
        m_engineCallback.SetUpdateGammarFlg(true);
        return false;
    }

    CVECIPtr<IVECIListItems> contactList;
    m_client.CreateListItems(&contactList);

    LoadInfo(*contactList);

    retCode = m_agentDataCommandPoi->ReloadData(NULL, _T(""),
        _T("POIAgentPOIData"), contactList);
    if (S_OK != retCode) {
        return false;
    }

    retCode = m_agentDataCommandPoi->SetDataSynchronized(NULL, _T(""), _T(""),
        _T(""), VBT_TRUE);
    if (S_OK != retCode) {
        return false;
    }

    retCode = m_agentDataCommandPoi->ReloadGrammar(&spTrans, _T(""),  _T(""));
    if (S_OK != retCode) {
        return false;
    }

    if (NULL == spTrans) {
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
        m_engineCallback.SetCurrentTransaction("poi", strTransaction);
    }
    else {
        return false;
    }
    VR_LOG("grammar : %s", strTransaction.c_str());

    return true;
}

bool
VR_VoiceBoxCatalogPoi::DeleteGrammar()
{
    VR_LOGD_FUNC();

    if (NULL == m_agentDataCommandPoi.ptr()) {
        return false;
    }

    HRESULT retCode = m_agentDataCommandPoi->SetDataSynchronized(NULL, _T(""), _T(""),
        _T(""), VBT_TRUE);
    if (S_OK != retCode) {
        return false;
    }

    CVECIPtr<IVECITransaction> spTrans;

    retCode = m_agentDataCommandPoi->RemoveAllGrammars(&spTrans, _T(""), _T("POIAgentPOIData"));
    if (S_OK != retCode) {
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
        m_engineCallback.SetCurrentTransaction("poi", strTransaction);
    }
    else {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxCatalogPoi::ReadPoiData()
{
    VR_LOGD_FUNC();

    m_vecItemData.clear();

    std::string strMapdata;
    if (NULL != VR_ConfigureIF::Instance()) {
        strMapdata = VR_ConfigureIF::Instance()->getMapDataPath();
    }

    std::string strFilePath = strMapdata + "/POI_DATA_AU.dat";
    std::ifstream input;
    input.open(strFilePath);
    if (!input.is_open()) {
        VR_LOG("file open error");
        return false;
    }

    VoiceVector<std::string>::type vecItem;
    const std::string cstPattern = ";";
    std::string strLine;

    while (!input.eof()) {
        strLine = "";
        getline(input, strLine);

        if (GetItem(strLine, cstPattern, vecItem)) {
            m_vecItemData.push_back(vecItem);
        }
    }

    input.close();

    return true;
}

bool
VR_VoiceBoxCatalogPoi::GetItem(const std::string& strLine,
        const std::string strPattern,
        VoiceVector<std::string>::type& vecItem)
{
    vecItem.clear();

    if (strPattern.empty()) {
        return false;
    }

    size_t start = 0;
    size_t index = strLine.find_first_of(strPattern, 0);

    while (index != strLine.npos) {
        if (start != index) {
            vecItem.push_back(strLine.substr(start, (index - start)));
        }
        else {
            vecItem.push_back("");
        }
        start = index + 1;
        index = strLine.find_first_of(strPattern, start);
    }

    if (!strLine.substr(start).empty()) {
        vecItem.push_back(strLine.substr(start));
    }
    else {
        vecItem.push_back("");
    }

    if (vecItem.size() < 5) {
        return false;
    }

    return true;
}

void
VR_VoiceBoxCatalogPoi::LoadInfo(
    IVECIListItems& listItems
)
{
    VR_LOGD_FUNC();

    VoiceVector<VecString>::const_iterator citor = m_vecItemData.cbegin();

    const int iItemSize = 5;
    CVECIPtr<IVECIParameterSet> spContact;
    while (citor != m_vecItemData.cend()) {
        if (iItemSize != citor->size()) {
            VR_LOG("item size error : %d", citor->size());
            ++citor;
            continue;
        }

        m_client.CreateParameterSet(&spContact);
        if (NULL == spContact) {
            continue;
        }

        spContact->AddParameter(_T("nId"), citor->at(0).c_str(), NULL, NULL);
        spContact->AddParameter(_T("formal"), citor->at(1).c_str(), NULL, NULL);
        spContact->AddParameter(_T("alias"), citor->at(2).c_str(), NULL, NULL);
        spContact->AddParameter(_T("pronunciation"), citor->at(3).c_str(), NULL, NULL);
        spContact->AddParameter(_T("shortcut"), citor->at(4).c_str(), NULL, NULL);
        listItems.AddItem(spContact);

        ++citor;
    }
}

void
VR_VoiceBoxCatalogPoi::PauseGrammarPoi()
{
    if (NULL == m_agentDataCommandPoi.ptr()) {
        return;
    }
    std::pair<std::string, std::string> pairAgent2TransId = m_engineCallback.GetCurrentTransaction();
    if (pairAgent2TransId.first.empty()) {
        VR_LOG("grammar has updated finish");
        return;
    }
    CVECIPtr<IVECITransaction> spTrans;
    CVECIOutStr strTrans;
    HRESULT retCode =  m_agentDataCommandPoi->PauseGrammarUpdate(&spTrans);
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
VR_VoiceBoxCatalogPoi::ResumeGrammarPoi()
{
    CVECIPtr<IVECITransaction> spTrans;
    CVECIOutStr strTrans;
    HRESULT retCode = m_agentDataCommandPoi->ResumeGrammarUpdate(&spTrans);
    if (FAILED(retCode) || NULL == spTrans.ptr()) {
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
VR_VoiceBoxCatalogPoi::CancelGrammarUpdate()
{
    if (NULL == m_agentDataCommandPoi.ptr()) {
        return;
    }

    CVECIPtr<IVECITransaction> spTrans;
    CVECIOutStr strTrans;
    HRESULT retCode = m_agentDataCommandPoi->CancelGrammarUpdate(&spTrans);
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