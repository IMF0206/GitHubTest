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
 * @file VR_InterpreterImpl.cpp
 * @brief
 *
 * This file includes the define of class VR_InterpreterImpl.
 *
 * @attention used for C++ only.
 */

#include <fstream>
#include <chrono>
#include "stdafx.h"
#include "pugixml.hpp"
#include "VR_InterpreterManager.h"
#include "VR_DialogEngine.h"
#include "VR_InterpreterImpl.h"
#include "uscxml/util/xmlutility.h"


#define CHECK_NULL() if (!m_spInterpreterActive) { \
VR_LOGD("warnning: interpreterActive is false"); \
RETURN(false); \
} \


using namespace voice;

VR_InterpreterManager::VR_InterpreterManager(VR_MonitorForBack* pMonitor, VRServiceRequestor* pRequestor, VR_DECommonIF* pCommon)
: m_pMonitorForBack(pMonitor), m_pServiceRequestor(pRequestor), m_pCommon(pCommon), m_pThreadLoad(NULL), m_running(false), m_isFirstReadyLoadAgent(true)
{
}

VR_InterpreterManager::~VR_InterpreterManager()
{
    VR_LOGD_FUNC();
    {
        std::unique_lock<std::mutex> lk(m_mutexLoad);
        m_running = false;
        while (!m_queueLoad.empty()) {
            m_queueLoad.pop_front();
        }
    }

    m_cvLoad.notify_all();
    if (m_pThreadLoad != NULL) {
    //    if (m_pThreadLoad->joinable()) {
            m_pThreadLoad->join();
    //    }
        delete m_pThreadLoad;
    }
}

void VR_InterpreterManager::initScxml()
{
    VR_LOGD_FUNC();
    bool readFromData = m_pCommon->isGetFromData();
    if (readFromData) { // read scxmls from vdata direcotry
        initScxmlFromData();
    }
    else {
        initScxmlFromEtc();
    }
}

void VR_InterpreterManager::initScxmlFromData()
{
    VR_LOGD_FUNC();
    VR_DECommonIF::DE_Country enCountry = m_pCommon->getCountryType();

    switch (enCountry) {
    case VR_DECommonIF::DE_Country::eu:
    {
        initScxmlByRegion("eu");
        break;
    }
    case VR_DECommonIF::DE_Country::cn:
    {
        initScxmlByRegion("cn");
        break;
    }
    case VR_DECommonIF::DE_Country::vt:
    {
        initScxmlByRegion("vt");
        break;
    }
    case VR_DECommonIF::DE_Country::gen:
    {
        initScxmlByRegion("gen");
        break;
    }
    default:
    {
        initScxmlByRegion("default");
        break;
    }
    }
}

void VR_InterpreterManager::initScxmlFromEtc()
{
    VR_LOGD_FUNC();
    VR_DECommonIF::DE_Country enCountry = m_pCommon->getCountryType();

    switch (enCountry) {
    case VR_DECommonIF::DE_Country::eu:
    {
        m_scxmls["topmenu"] = "/system/etc/voicerecog/eu/topmenu.zip";
        break;
    }
    case VR_DECommonIF::DE_Country::cn:
    {
        if (VR_HEADUNIT_DCU == m_pCommon->getHeadUnitType()) {
            // dcu
            m_scxmls["topmenu"] = "/system/etc/voicerecog/cn/topmenu.zip";
            m_scxmls["media"] = "/system/etc/voicerecog/cn/media.zip";
            m_scxmls["phone"] = "/system/etc/voicerecog/cn/phone.zip";
            m_scxmls["climate"] = "/system/etc/voicerecog/cn/climate.zip";
        }
        else {
            // meu
            m_scxmls["topmenu"] = "/system/etc/voicerecog/cn/topmenu.zip";
            m_scxmls["navi"] = "/system/etc/voicerecog/cn/navi.zip";
            m_scxmls["media"] = "/system/etc/voicerecog/cn/media.zip";
            m_scxmls["phone"] = "/system/etc/voicerecog/cn/phone.zip";
            m_scxmls["climate"] = "/system/etc/voicerecog/cn/climate.zip";
            m_scxmls["information"] = "/system/etc/voicerecog/cn/information.zip";
        }
        break;
    }
    case VR_DECommonIF::DE_Country::vt:
    {
        m_scxmls["voicetag"] = "/system/etc/voicerecog/voicetag/voicetag.zip";
        break;
    }
    case VR_DECommonIF::DE_Country::gen:
    default:
    {
        m_scxmls["topmenu"] = "/system/etc/voicerecog/genel/topmenu.zip";
        m_scxmls["navi"] = "/system/etc/voicerecog/genel/navi.zip";
        m_scxmls["media"] = "/system/etc/voicerecog/genel/media.zip";
        m_scxmls["phone"] = "/system/etc/voicerecog/genel/phone.zip";
    }
    }
}

void VR_InterpreterManager::initScxmlByRegion(const std::string& region)
{
    VR_LOGD_FUNC();
    pugi::xml_document doc;
    std::string pathStr = m_pCommon->getDataPath() + "agent_configure.xml";
    doc.load_file(pathStr.c_str());

    std::string xpathRegion = "//" + region + "/*";
    VR_LOGD("select %s", xpathRegion.c_str());
    pugi::xpath_node_set regionNodes = doc.select_nodes(xpathRegion.c_str());
    int num = regionNodes.size();
    VR_LOGD("regionNodes.size = %d", num);
    for (int i = 0; i < num; i++) {
        std::string name = regionNodes[i].node().name();
        std::string path = m_pCommon->getDataPath() + "engine/" + name + ".zip";

        std::ifstream scxmlZip(path);
        if (scxmlZip) {
            VR_LOGD("add [%s ] [%s]into scxml_path_map ", name.c_str(), path.c_str());
            m_scxmls[name] = path;
        }
    }
}

bool VR_InterpreterManager::start()
{
    VR_LOGD_FUNC();
    // Read scxml files
    initScxml();
    VR_LOGD("VR_InterpreterManager : start");
    for (VoiceMap<std::string, std::string>::iterator it = m_scxmls.begin(); it != m_scxmls.end(); ++it) {
        std::string agentName = it->first;
        std::shared_ptr<VR_Interpreter> sp(VR_new VR_InterpreterImpl(m_pMonitorForBack, agentName, m_scxmls[agentName]));
        sp->bindServiceRequestor(m_pServiceRequestor);
        m_interpreters[agentName] = sp;
        VR_LOGD("add agent name [%s] into map", agentName.c_str());
    }

    int nice = m_pCommon->getTargetNice("vr_load");
    VR_LOGD("set vr_load thread nice %d", nice);
    m_running = true;
    m_pThreadLoad = VR_new tthread::thread(threadProc, (void*)this, "vr_load", nice);
    RETURN(true);
}

void VR_InterpreterManager::threadProc(void* pArg)
{
    VR_LOGD_FUNC();
    VR_InterpreterManager* me = (VR_InterpreterManager*)pArg;
    me->loadAgents();
}

void VR_InterpreterManager::loadAgents()
{
    VR_LOGD_FUNC();

    // load all agents

    for (VoiceMap<std::string, std::shared_ptr<VR_Interpreter> >::iterator it = m_interpreters.begin();
        it != m_interpreters.end(); ++it) {
            if ("topmenu" == it->first) {
                m_queueLoad.push_front(it->first);
            }
            else {
                m_queueLoad.push_back(it->first);             
            }
    }

    while (m_running) {
        std::string agentName;
        {
            std::unique_lock<std::mutex> lk(m_mutexLoad);
            VR_LOGD("VR_InterpreterManager : loadAgents enter lock");
            while (m_queueLoad.empty()) {
                m_cvLoad.wait(lk);
                if (!m_running) {
                    VR_LOGD("VR_InterpreterManager : loadAgents return");
                    return;
                }
            }
            agentName = m_queueLoad.front();
            m_queueLoad.pop_front();
        }

        VoiceMap<std::string, std::shared_ptr<voice::VR_Interpreter>>::iterator it = m_interpreters.find(agentName);
        if (it != m_interpreters.end()) {
            VR_LOGD("VR_InterpreterManager : it->second->load()");
            it->second->load();
            if (m_isFirstReadyLoadAgent) {
                m_isFirstReadyLoadAgent = false;
                uscxml::Event notifyEvent;
                notifyEvent.setName("agentReadyFirstTime");
                m_pServiceRequestor->requestService(notifyEvent);
              }
        }
        else {
            VR_ERROR("can't find interpreter:%s in the map", agentName.c_str());
        }
    }
    VR_LOGD("load thread exit!");
}

void VR_InterpreterManager::pushLoadAgent(const std::string& agentName)
{
    VR_LOGD_FUNC();
    std::unique_lock<std::mutex> lk(m_mutexLoad);
    if ("topmenu" == agentName) {
        m_queueLoad.push_front(agentName);
    }
    else {
        m_queueLoad.push_back(agentName);       
    }
    m_cvLoad.notify_all();
}

bool VR_InterpreterManager::startAgent(const std::string& agentName, const std::string& initParm)
{
    VR_LOGD_FUNC();
    VR_LOGP("start agent:%s", agentName.c_str());
    VoiceMap<std::string, std::shared_ptr<VR_Interpreter> >::iterator it = m_interpreters.find(agentName);
    if (it == m_interpreters.end()) {
        VR_ERROR("cann't find agent, name %s", agentName.c_str());
        RETURN(false);
    }
    else {
        m_spInterpreterActive = m_interpreters[agentName];

        bool isStart = m_spInterpreterActive->start(initParm, m_pCommon->getTargetNice(agentName));
        if (isStart) {
            VR_LOGD("start agent success");
            RETURN(true);
        }
        else {
            VR_LOGD("start agent fail");
            RETURN(false);
        }
    }
}

bool VR_InterpreterManager::backAgent(const std::string &agentName)
{
   VR_LOGD_FUNC();

   bool isNeedStop = isAgentNeedStop(m_spInterpreterActive->getName());
   if (isNeedStop) {
       VR_LOGD("Interpreter %s need stop", m_spInterpreterActive->getName().c_str());
       m_spInterpreterActive->stop();
       pushLoadAgent(m_spInterpreterActive->getName());
   }

   VoiceMap<std::string, std::shared_ptr<VR_Interpreter> >::iterator it = m_interpreters.find(agentName);
   if (it == m_interpreters.end()) {
       VR_ERROR("cann't find Interpreter, name %s", agentName.c_str());
       RETURN(false);
   }
   else {
       m_spInterpreterActive = m_interpreters[agentName];
       VR_LOGD("Interpreter %s is Active", agentName.c_str());

       bool isStarted = m_spInterpreterActive->isInterpreterStarted();
       if (!isStarted) {
           // this is a abnormal case
           VR_ERROR("Back Interpreter must be started, but this [%s] is stopped!!!", agentName.c_str());
           m_spInterpreterActive->start(m_pCommon->getTargetNice(agentName));
       }
       else {
           m_spInterpreterActive->resumeInterpreter();
           VR_LOGD("Resume Interpreter Send");
       }
       RETURN(true);
   }
}

// invoke in DE
bool VR_InterpreterManager::startAgent(const std::string& agentName)
{
    VR_LOGD_FUNC();
    VR_LOGP("start agent:%s", agentName.c_str());
    VoiceMap<std::string, std::shared_ptr<VR_Interpreter> >::iterator it = m_interpreters.find(agentName);
    if (it == m_interpreters.end()) {
        VR_ERROR("cann't find agent, name %s", agentName.c_str());
        RETURN(false);
    }
    else {
        m_spInterpreterActive = m_interpreters[agentName];

        bool isStarted = m_spInterpreterActive->isInterpreterStarted();
        if (!isStarted) {
            m_spInterpreterActive->start(m_pCommon->getTargetNice(agentName));
        }
        else {
            // forward to the destination agent, do nothing now. because this case can not be in.
            VR_ERROR("To start a started agent, this is not correct now!!!");
        }
        RETURN(true);
    }
}

bool VR_InterpreterManager::isAgentNeedStop(const std::string& agentName)
{
    return agentName != "topmenu";
}

// invoke in DE
bool VR_InterpreterManager::changeAgent(const std::string& agentName)
{
    VR_LOGD_FUNC();
    VR_LOGP("changeAgent %s", agentName.c_str());
    CHECK_NULL();

    bool isNeedStop = isAgentNeedStop(m_spInterpreterActive->getName());
    if (isNeedStop) {
        m_spInterpreterActive->stop();
        pushLoadAgent(m_spInterpreterActive->getName());
    }
    else {
        // current agent stay, active interpreter will change
        VR_LOG("stay agent is  [%s]", agentName.c_str());
    }

    return startAgent(agentName);
}

bool VR_InterpreterManager::receiveInitEvent(const std::string& evt, const std::string& xmlcontent)
{
    VR_LOGD_FUNC();
    VR_LOGP("interpreter receive event, name:%s", evt.c_str());
    CHECK_NULL();

    bool isInit = m_spInterpreterActive->receiveInitEvent(xmlcontent);
    if (isInit) {
        VR_LOGP("receiveInitEvent sucess");
    }
    else {
        VR_LOGP("receiveInitEvent fail");
        RETURN(false);
    }

    RETURN(true);
}

// invoke in DE and asr thread
bool VR_InterpreterManager::receiveEvent(const std::string& evt, const std::string& xmlcontent)
{
    VR_LOGD_FUNC();
    VR_LOGP("interpreter receive event, name:%s", evt.c_str());
    CHECK_NULL();
    bool ret = m_spInterpreterActive->receiveEvent(evt, xmlcontent);
    RETURN(ret);
}

// only invoke in uscxml thread
bool VR_InterpreterManager::receiveOpResult()
{
    VR_LOGD_FUNC();
    CHECK_NULL();
    m_spInterpreterActive->receiveOpResult();
    return true;
}

// invoke in DE thread
bool VR_InterpreterManager::notifyOpResult(const std::string &op, const std::string &result, bool isContentInputEvent)
{
    VR_LOGD_FUNC();
    CHECK_NULL();
    m_spInterpreterActive->notifyOpResult(op, result, isContentInputEvent);
    return true;
}

// REVIEW: stop Agent
// invoke in DE thread
bool VR_InterpreterManager::stopAgent()
{
    VR_LOGD_FUNC();
    CHECK_NULL();

    VoiceMap<std::string, std::shared_ptr<voice::VR_Interpreter>>::iterator it;
    for (it = m_interpreters.begin(); it != m_interpreters.end(); ++it) {
        std::shared_ptr<voice::VR_Interpreter> interpreterSp = it->second;
        std::string agentName = interpreterSp->getName();
        if (interpreterSp->isInterpreterStarted()) {
            VR_LOGD("agent %s is started , need stop", agentName.c_str());
            interpreterSp->stop();
            pushLoadAgent(agentName);
        }
        else {
            VR_LOGD("agent %s not start", agentName.c_str());
        }
    }

    RETURN(true);
}

bool VR_InterpreterManager::stop()
{
    VR_LOGD_FUNC();
    std::unique_lock<std::mutex> lk(m_mutexLoad);
    m_running = false;
    m_cvLoad.notify_all();
    return true;
}

bool VR_InterpreterManager::prepareAgent(const std::string &agentName)
{
    VR_LOGD_FUNC();
    VoiceMap<std::string, std::shared_ptr<VR_Interpreter> >::iterator it = m_interpreters.find(agentName);
    if (it == m_interpreters.end()) {
        VR_ERROR("cann't find agent, name %s", agentName.c_str());
        RETURN(false);
    }
    else {
        m_spInterpreterActive = m_interpreters[agentName];

        bool isPrepareOK = m_spInterpreterActive->prepare(m_pCommon->getTargetNice(agentName));

        RETURN(isPrepareOK);
    }
}

// invoke in DE thread
bool VR_InterpreterManager::setTargetName(const std::string& value)
{
    VR_LOGD_FUNC();
    CHECK_NULL();
    return m_spInterpreterActive->setTargetName(value);
}

// invoke in DE thread
bool VR_InterpreterManager::notifyUpdateGlobalStates(const std::string& updateStr)
{
    VR_LOGD_FUNC();
    {
        std::unique_lock<std::mutex> lk(m_mutexGlobalState);
        m_globalStates = updateStr;
    }
    VR_LOG_TO_FILE("receive globalState", m_globalStates);
    CHECK_NULL();
    m_spInterpreterActive->receiveEvent("updateDataModel", "");
    RETURN(true);
}

// invoke in DE thread
bool VR_InterpreterManager::setPreInit(bool isPreInit)
{
    VR_LOGD_FUNC();
    CHECK_NULL();
    m_spInterpreterActive->setPreInit(isPreInit);
    return true;
}

// REVIEW: update key-value
// invoke in uscxml thread
bool VR_InterpreterManager::updateGlobalStates()
{
    VR_LOGD_FUNC();
    CHECK_NULL();
    std::string globalStates;
    {
        std::unique_lock<std::mutex> lk(m_mutexGlobalState);
        globalStates = m_globalStates;
    }

    // Only for debug log
    VR_LOG("updateGlobalStateToInterpreter[%s]", globalStates.c_str());
    VR_LOG_TO_FILE("updateGlobalStateToInterpreter:", globalStates);
    //

    return m_spInterpreterActive->updateGlobalStates(globalStates);
}

/* EOF */
