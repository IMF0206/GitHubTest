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

#include "VR_DataSynchronizer.h"
#include "VR_AsrRequestor.h"
#include "VR_Log.h"
#include "VR_Def.h"
#include "Vr_Asr_Engine.h"

using namespace N_Vr::N_Asr;

VR_DataSynchronizer::VR_DataSynchronizer(VR_AsrRequestor* asrRequestor)
    :m_asrRequestor(asrRequestor)
{
}

VR_DataSynchronizer::~VR_DataSynchronizer()
{
}

spC_Term VR_DataSynchronizer::getCTerm(
    unsigned int id,
    const std::string &name,
    const std::string &pron)
{
    spC_Term item(VR_new N_Vr::N_Asr::C_Term);
    item->m_i_User_Data_Lo = id;
    if (!name.empty()) {
        item->m_string_Orthography.assign(name.c_str());
    }
    if (!pron.empty()) {
        unsigned char * buffer = NULL;
        buffer = VR_new unsigned char[pron.size()];
        memcpy(buffer, pron.c_str(), pron.size());

        C_Buffer pronBuffer;
        pronBuffer.m_i_Size = pron.size();
        pronBuffer.m_sai_Buffer.reset(buffer);

        item->m_vector_buffer_Transcriptions.push_back(pronBuffer);
    }
    return item;
}

bool VR_DataSynchronizer::deleteGrammar(const std::string &contextID)
{
    VoiceList<spC_Term>::type list;
    return updateGrammar(
        contextID,
        list,
        list,
        boost::bind(&VR_DataSynchronizer::phaseCallback, this, _1),
        boost::bind(&VR_DataSynchronizer::notifyCallback, this, _1),
        true);
}

bool VR_DataSynchronizer::deleteGrammar(
    const std::string &contextID,
    boost::function<void(C_Event_Phase const &)> phaseCallbackFuncPoint)
{
    VoiceList<spC_Term>::type list;
    return updateGrammar(
        contextID,
        list,
        list,
        phaseCallbackFuncPoint,
        boost::bind(&VR_DataSynchronizer::notifyCallback, this, _1),
        true);
}

bool VR_DataSynchronizer::deleteGrammar(
    const std::string &contextID,
    VoiceList<spC_Term>::type &delList,
    boost::function<void(C_Event_Phase const &)> phaseCallbackFuncPoint)
{
    VoiceList<spC_Term>::type list;
    return updateGrammar(
        contextID,
        list,
        delList,
        phaseCallbackFuncPoint,
        boost::bind(&VR_DataSynchronizer::notifyCallback, this, _1));
}

bool VR_DataSynchronizer::updateGrammar(
    const std::string &contextID,
    VoiceList<spC_Term>::type &addList,
    bool clear)
{
    return updateGrammar(
        contextID,
        addList,
        boost::bind(&VR_DataSynchronizer::phaseCallback, this, _1),
        clear);
}

bool VR_DataSynchronizer::updateGrammar(
    const std::string &contextID,
    VoiceList<spC_Term>::type &addList,
    boost::function<void(C_Event_Phase const &)> phaseCallbackFuncPoint,
    bool clear)
{
    VoiceList<spC_Term>::type list;
    return updateGrammar(
        contextID,
        addList,
        list,
        phaseCallbackFuncPoint,
        boost::bind(&VR_DataSynchronizer::notifyCallback, this, _1),
        clear);
}

bool VR_DataSynchronizer::updateGrammar(
    const std::string &contextID,
    VoiceList<spC_Term>::type &addList,
    VoiceList<spC_Term>::type &delList)
{
    return updateGrammar(
        contextID,
        addList,
        delList,
        boost::bind(&VR_DataSynchronizer::phaseCallback, this, _1));
}

bool VR_DataSynchronizer::updateGrammar(
    const std::string &contextID,
    VoiceList<spC_Term>::type &addList,
    VoiceList<spC_Term>::type &delList,
    boost::function<void(C_Event_Phase const &)> phaseCallbackFuncPoint)
{
    return updateGrammar(
        contextID,
        addList,
        delList,
        phaseCallbackFuncPoint,
        boost::bind(&VR_DataSynchronizer::notifyCallback, this, _1));
}

bool VR_DataSynchronizer::updateGrammar(
    const std::string &contextID,
    VoiceList<spC_Term>::type &addList,
    VoiceList<spC_Term>::type &delList,
    boost::function<void(C_Event_Phase const &)> phaseCallbackFuncPoint,
    boost::function<void(C_Event_Notify const &)> notifyCallbackFuncPoint,
    bool clear)
{
    VR_LOGD("Grammar contextID: %s", contextID.c_str());
    C_Request_Context_List_Update updateMsg;
    updateMsg.m_string_Id_Party.assign("Origin");
    updateMsg.m_string_Id_Context.assign(contextID);

    if (!addList.empty()) {
        updateMsg.m_list_spo_Term_For_Add.swap(addList);
    }
    if (!delList.empty()) {
        updateMsg.m_list_spo_Term_For_Delete.swap(delList);
    }

    updateMsg.m_function_On_Event_Phase = phaseCallbackFuncPoint;
    updateMsg.m_function_On_Event_Notify = notifyCallbackFuncPoint;

    if (!updateMsg.m_function_On_Event_Phase) {
        VR_LOGD("PhaseCallBack function point is NULL");
    }
    if (!updateMsg.m_function_On_Event_Notify) {
        VR_LOGD("NotifyCallBack function point is NULL");
    }

    updateMsg.m_b_Clear = clear;
    bool result = m_asrRequestor->updateGrammar(updateMsg);
    m_asrRequestIdList.push_back(updateMsg.m_i_Id_Request);
    return result;
}

bool VR_DataSynchronizer::stopGrammar()
{
    bool result = true;
    C_Request_Context_List_Update updateMsg;
    updateMsg.m_b_Cancel = true;
    for (VoiceList<unsigned int>::iterator it = m_asrRequestIdList.begin();
        it != m_asrRequestIdList.end();
        ++it) {
        updateMsg.m_i_Id_Request = *it;
        result = result && m_asrRequestor->updateGrammar(updateMsg);
        updateMsg.m_function_On_Event_Phase = boost::bind(&VR_DataSynchronizer::phaseCallback, this, _1);
        updateMsg.m_function_On_Event_Notify = boost::bind(&VR_DataSynchronizer::notifyCallback, this, _1);
    }
    m_asrRequestIdList.clear();
    return result;
}

void VR_DataSynchronizer::genVoiceTagPhoneme()
{
    m_asrRequestor->genVoiceTagPhoneme();
}

void VR_DataSynchronizer::setGrammarActive(const std::string &contextID, bool isActive, const VoiceList<std::string>::type &ruleIDList)
{
    m_asrRequestor->setGrammarActive(contextID, isActive, ruleIDList);
}

// invoke in asr thread
void VR_DataSynchronizer::notifyUpdateGrammarCategoryFinish(const std::string &category)
{
    m_asrRequestor->updateGrammarCategoryFinish(category);
}

// invoke in DE thread
void VR_DataSynchronizer::updateGrammarCategoryFinish(const std::string &category)
{
    // TODO: map category/contextID to asrrequestID
    if (!m_asrRequestIdList.empty()) {
        m_asrRequestIdList.pop_front();
    }
}

bool VR_DataSynchronizer::isAsrIdle()
{
    return m_asrRequestIdList.empty();
}

void VR_DataSynchronizer::phaseCallback(N_Vr::N_Asr::C_Event_Phase const &phaseEvent)
{
}

void VR_DataSynchronizer::notifyCallback(N_Vr::N_Asr::C_Event_Notify const &notifyEvent)
{
}

/* EOF */
