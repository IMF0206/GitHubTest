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
#    include "VR_VoiceBoxCatalogApps.h"
#endif

#ifndef VR_VOICEBOXLOG_H
#    include "VR_VoiceBoxLog.h"
#endif

VR_VoiceBoxCatalogApps::VR_VoiceBoxCatalogApps(
    IVECIEngineClient& client,
    VR_VoiceBoxEngineCallback& engineCallback
    )
: m_client(client)
, m_engineCallback(engineCallback)
{
}

VR_VoiceBoxCatalogApps::~VR_VoiceBoxCatalogApps()
{
}

bool
VR_VoiceBoxCatalogApps::Initialize()
{
    HRESULT result = m_client.CreateAgentDispatcher(&m_agentDataCommandApps);
    if (S_OK != result) {
        return false;
    }

    if (NULL == m_agentDataCommandApps.ptr()) {
        return false;
    }

    result = m_agentDataCommandApps->Init(VBT_AGENT_APPS);
    if (S_OK != result) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxCatalogApps::Cataloguing(
    VR_VoiceBoxXmlParser& parser
    )
{
    if (NULL == m_agentDataCommandApps.ptr()) {
        return false;
    }

    CVECIPtr<IVECITransaction> spTrans;

    HRESULT retCode = m_agentDataCommandApps->SetDataSynchronized(NULL, _T(""), _T(""),
        _T(""), VBT_FALSE);
    if (S_OK != retCode) {
        return false;
    }

    VoiceVector<AppCategory>::type vecAppCategory;
    parser.getApps(vecAppCategory);
    std::string strTableName;

    VoiceVector<AppCategory>::const_iterator citor = vecAppCategory.cbegin();
    for (; citor != vecAppCategory.cend(); ++citor) {
        if (citor->strName == "apps") {
            strTableName = "AppsAgentApps";
        }
        else if (citor->strName == "appsoffboard") {
            strTableName = "AppsAgentAppsOffboard";
        }
        else {
            continue;
        }

        CVECIPtr<IVECIListItems> contactList;
        m_client.CreateListItems(&contactList);

        LoadAppsInfo(citor->vecFormal, *contactList);

        retCode = m_agentDataCommandApps->ReloadData(NULL, _T(""),
            strTableName.c_str(), contactList);
        if (S_OK != retCode) {
            return false;
        }
    }

    retCode = m_agentDataCommandApps->SetDataSynchronized(NULL, _T(""), _T(""),
        _T(""), VBT_TRUE);
    if (S_OK != retCode) {
        return false;
    }

    retCode = m_agentDataCommandApps->ReloadGrammar(&spTrans, _T(""),  _T(""));
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
        m_engineCallback.SetCurrentTransaction("apps", strTransaction);
    }
    else {
        return false;
    }
    VR_LOG("grammar : %s", strTransaction.c_str());

    return true;
}

void
VR_VoiceBoxCatalogApps::LoadAppsInfo(
    const VoiceVector<AppFormal>::type& vecAppsInfo,
    IVECIListItems& listItems
)
{
    VoiceVector<AppFormal>::const_iterator citorFormal = vecAppsInfo.cbegin();
    VoiceVector<Alias>::const_iterator citorAlias;
    CVECIPtr<IVECIParameterSet> spContact;
    while (citorFormal != vecAppsInfo.cend()) {
        for (citorAlias = citorFormal->vecAlias.cbegin();
            citorAlias != citorFormal->vecAlias.cend();
            ++citorAlias) {
            m_client.CreateParameterSet(&spContact);
            if (NULL == spContact) {
                return;
            }
            spContact->AddParameter(_T("nId"), citorFormal->strId.c_str(), NULL, NULL);
            spContact->AddParameter(_T("formal"), citorFormal->strName.c_str(), NULL, NULL);
            spContact->AddParameter(_T("shortcut"), citorFormal->strShortCut.c_str(), NULL, NULL);
            spContact->AddParameter(_T("alias"), citorAlias->strName.c_str(), NULL, NULL);
            spContact->AddParameter(_T("pronunciation"), citorAlias->strPron.c_str(), NULL, NULL);
            listItems.AddItem(spContact);
        }
        ++citorFormal;
    }
}

void
VR_VoiceBoxCatalogApps::PauseGrammarApps()
{
    if (NULL == m_agentDataCommandApps.ptr()) {
        return;
    }
    std::pair<std::string, std::string> pairAgent2TransId = m_engineCallback.GetCurrentTransaction();
    if (pairAgent2TransId.first.empty()) {
        VR_LOG("grammar has updated finish");
        return;
    }
    CVECIPtr<IVECITransaction> spTrans;
    CVECIOutStr strTrans;
    HRESULT retCode =  m_agentDataCommandApps->PauseGrammarUpdate(&spTrans);
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
VR_VoiceBoxCatalogApps::ResumeGrammarApps()
{
    if (NULL == m_agentDataCommandApps.ptr()) {
        return;
    }

    CVECIPtr<IVECITransaction> spTrans;
    CVECIOutStr strTrans;
    HRESULT retCode = m_agentDataCommandApps->ResumeGrammarUpdate(&spTrans);
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
VR_VoiceBoxCatalogApps::CancelGrammarUpdate()
{
    if (NULL == m_agentDataCommandApps.ptr()) {
        return;
    }
    CVECIPtr<IVECITransaction> spTrans;
    CVECIOutStr strTrans;
    HRESULT retCode = m_agentDataCommandApps->CancelGrammarUpdate(&spTrans);
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