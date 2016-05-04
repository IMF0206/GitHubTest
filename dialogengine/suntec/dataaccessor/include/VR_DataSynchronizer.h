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
 * @file VR_DataSynchronizer.h
 * @brief Declaration file of VR_DataSynchronizer.
 *
 * This file includes the declaration of VR_DataSynchronizer.
 *
 * @attention used for C++ only.
 */

#ifndef VR_DATA_SYNCHRONIZER_H
#define VR_DATA_SYNCHRONIZER_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include "MEM_list.h"
#include <string>
#include <boost/shared_array.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

namespace N_Vr
{
namespace N_Asr
{
class C_Term;
class C_Engine;
class C_Event_Phase;
class C_Event_Notify;
class C_Request_Context_List_Update;
}
}

class VR_AsrRequestor;

typedef boost::shared_ptr<N_Vr::N_Asr::C_Term> spC_Term;

/**
 * @brief The VR_DataSynchronizer class
 *
 * interface to update grammar for VR_DataAccessorManager
 */

class VR_DataSynchronizer
{
public:
    VR_DataSynchronizer(VR_AsrRequestor* asrRequestor);
    virtual ~VR_DataSynchronizer();

    static spC_Term getCTerm(
        unsigned int id,
        const std::string &name,
        const std::string &pron = std::string());

    bool deleteGrammar(const std::string &contextID);
    bool deleteGrammar(
        const std::string &contextID,
        boost::function<void(N_Vr::N_Asr::C_Event_Phase const &)> phaseCallbackFuncPoint);
    bool deleteGrammar(
        const std::string &contextID,
        VoiceList<spC_Term>::type &delList,
        boost::function<void(N_Vr::N_Asr::C_Event_Phase const &)> phaseCallbackFuncPoint);
    bool updateGrammar(
        const std::string &contextID,
        VoiceList<spC_Term>::type &addList,
        bool clear = false);
    bool updateGrammar(
        const std::string &contextID,
        VoiceList<spC_Term>::type &addList,
        boost::function<void(N_Vr::N_Asr::C_Event_Phase const &)> phaseCallbackFuncPoint,
        bool clear = false);
    bool updateGrammar(
        const std::string &contextID,
        VoiceList<spC_Term>::type &addList,
        VoiceList<spC_Term>::type &delList);
    bool updateGrammar(
        const std::string &contextID,
        VoiceList<spC_Term>::type &addList,
        VoiceList<spC_Term>::type &delList,
        boost::function<void(N_Vr::N_Asr::C_Event_Phase const &)> phaseCallbackFuncPoint);
    bool updateGrammar(
        const std::string &contextID,
        VoiceList<spC_Term>::type &addList,
        VoiceList<spC_Term>::type &delList,
        boost::function<void(N_Vr::N_Asr::C_Event_Phase const &)> phaseCallbackFuncPoint,
        boost::function<void(N_Vr::N_Asr::C_Event_Notify const &)> notifyCallbackFuncPoint,
        bool clear = false);

    bool stopGrammar();

    void genVoiceTagPhoneme();
    void setGrammarActive(const std::string &contextID, bool isActive, const VoiceList<std::string>::type &ruleIDList = VoiceList<std::string>::type());
    void notifyUpdateGrammarCategoryFinish(const std::string &category);
    void updateGrammarCategoryFinish(const std::string &category);

    bool isAsrIdle();

    void phaseCallback(const N_Vr::N_Asr::C_Event_Phase &phaseEvent);
    void notifyCallback(const N_Vr::N_Asr::C_Event_Notify &notifyEvent);
private:
    VR_AsrRequestor* m_asrRequestor;
    VoiceList<unsigned int>::type m_asrRequestIdList;
};


#endif /* VR_DATA_SYNCHRONIZER_H */
/* EOF */
