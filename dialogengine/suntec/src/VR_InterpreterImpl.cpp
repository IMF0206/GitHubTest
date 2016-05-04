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
#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <BL_NDataFile.h>
#include <BL_String.h>
#include <BL_UnzipFile.h>

#include "VR_InterpreterImpl.h"
#include "VR_VerboseMonitor.h"
#include "VR_PerformanceLog.h"

#include <DOM/Simple/DOMImplementation.hpp>
#include <SAX/helpers/InputSourceResolver.hpp>
#include <DOM/io/Stream.hpp>
#include "uscxml/config.h"
#include "uscxml/DOMUtils.h"
#include "uscxml/util/xmlutility.h"
#include <chrono>

#include <glog/logging.h>

#include "uscxml/Factory.h"
#include "uscxml/plugins/invoker/vr/VRInvoker.h"

#ifdef HAS_SIGNAL_H
#include <signal.h>
#endif

#ifdef HAS_EXECINFO_H
#include <execinfo.h>
#endif

#ifdef HAS_DLFCN_H
#include <dlfcn.h>
#endif

using namespace std;
using namespace uscxml;
using namespace voice;

VR_InterpreterImpl::VR_InterpreterImpl(VR_MonitorForBack* pMonitor, const std::string& name, const std::string& scxmlPath)
    : m_name(name)
    , m_scxmlPath(scxmlPath)
    , m_state(NONE)
    , m_pInterpreter(NULL)
{
    VR_LOGD_FUNC();
    m_pMonitor = VR_new VR_VerboseMonitor(pMonitor);
}

VR_InterpreterImpl::~VR_InterpreterImpl()
{
    VR_LOGD_FUNC();
    doStop();

    if (NULL != m_pInterpreter) {
        delete m_pInterpreter;
    }

    delete m_pMonitor;
}

std::string VR_InterpreterImpl::getName()
{
    VR_LOGD_FUNC();
    return m_name;
}

void VR_InterpreterImpl::setPreInit(bool isPreInit)
{
    VR_LOGD_FUNC();
    VR_LOGD("agent name=%s", this->getName().c_str());
    std::unique_lock<std::mutex> lk(m_mutex);
    VR_LOGD("setPreInit enter lock");

    if (m_state == State::PREPARE) {
        if (isPreInit) {
            m_state = State::READY;
            VR_LOGD("m_state is READY");
        }
    }
}

// only invoke in uscxml thread
bool VR_InterpreterImpl::updateGlobalStates(const std::string& globalStates)
{
    VR_LOGD_FUNC();
    VR_LOGD("agentName=%s", this->getName().c_str());

    VR_LOG_TO_FILE("assignDataName:", "globalStates");
    VR_LOG_TO_FILE("assignDataContent:", globalStates);
    VR_LOG_TO_FILE("------------------------------", "");
    try {
        m_pInterpreter->getDataModel().assignDirectly("$globalState/g", globalStates);
    }
    catch (...) {
        VR_LOGD("warnning: assign globalState error");
    }
    return true;
}

bool VR_InterpreterImpl::bindServiceRequestor(VRServiceRequestor* requestor)
{
    VR_LOGD_FUNC();
    VRInvoker::getInstance()->setServiceRequestor(requestor);
    return true;
}

bool VR_InterpreterImpl::loadInterpreter()
{
    VR_LOGD_FUNC();
    try
    {
        // sure the interpreter is stopped , and no interface invoke
        if (NULL != m_pInterpreter) {
            int loop = 500;
            while (m_pInterpreter->isRunning() && loop > 0) {
                usleep(20*1000);
                --loop;
            }
            VR_LOG("delete interpreter, loop:%d", 500 - loop);
            delete m_pInterpreter;
            m_pInterpreter = NULL;
            VR_LOG("delete interpreter done");
        }

        char* urlCahr = const_cast<char*>(m_scxmlPath.c_str());
        XCHAR* filePath = XTEXT(urlCahr);
        std::size_t pos = m_scxmlPath.find_last_of("/");
        std::size_t endPos = m_scxmlPath.find(".zip");

        std::size_t len = endPos - (pos + 1);
        std::string scxmlName = m_scxmlPath.substr(pos + 1, len)  + ".xml";

        char* fileNameChar = const_cast<char*>(scxmlName.c_str());
        XCHAR* fileName = XTEXT(fileNameChar);
        XCHAR* password = XTEXT("SUNTEC_SCXML");

        BYTE* xmlBuf = NULL;
        BL_String xmlFileFullPath(filePath);
        int bufSize = UnzipSelectFileToBuffer(xmlFileFullPath.GetString(), fileName, password, xmlBuf);
        if (0 == bufSize || NULL == xmlBuf) {
            VR_ERROR("Unzip scxml failed, bufSize=%d, xmlBuf=%p", bufSize, xmlBuf);
            return false;
        }

        char* str = reinterpret_cast<char*>(xmlBuf);
        m_pInterpreter = VR_new Interpreter(Interpreter::fromXML(str, m_scxmlPath));
        delete[] xmlBuf;

        if (!m_pInterpreter->getImpl()) {
            VR_ERROR("Can't create interpreter");
            return false;
        }

        m_pInterpreter->addMonitor(m_pMonitor);
        m_pInterpreter->getImpl()->init();
        uscxml::Invoker vrInvoke = uscxml::Invoker(VRInvoker::getInstance());
        m_pInterpreter->setInvoker("id.vrservice", vrInvoke);
        VR_LOGP("interpreter has been initeed");
        return true;
    }
    catch (uscxml::Event& ex)
    {
        VR_ERROR("load interpreter error(event):%s", ex.getData().getCompound().at("cause").getAtom().c_str());
    }
    catch (std::exception& e)
    {
        VR_ERROR("load interpreter error:%s", e.what());
    }
    return false;
}

void VR_InterpreterImpl::load()
{
    VR_LOGD_FUNC();
    VR_LOGP("load interpreter:%s", m_scxmlPath.c_str());
    std::unique_lock<std::mutex> lk(m_mutex);
    VR_LOGD("load enter lock");
    if (m_state != State::NONE) {
        VR_LOG("warnning: the interpreter is LOADING or LOADED");
        return;
    }

    m_state = State::LOADING;
    VR_LOGD("m_state is LOADING");
    lk.unlock();
    loadInterpreter();

    lk.lock();
    m_state = State::LOADED;
    VR_LOGD("m_state is LOADED");
    m_cv.notify_all();
    VR_LOGP("load interpreter:%s ok", m_scxmlPath.c_str());
}

// invoke in DE thread
bool VR_InterpreterImpl::start(int nice)
{
    VR_LOGD_FUNC();
    VR_LOG("start agent name=%s, nice:%d", this->getName().c_str(), nice);
    if (m_state == State::NONE || m_state == State::LOADING
        || m_state == State::LOADED || m_state == State::PREPARE) {
        {
            std::unique_lock<std::mutex> lk(m_mutex);
            VR_LOGD("start enter lock");
            if (m_state == State::LOADING) {
                VR_LOG("interpreter waiting for be loaded");
                std::cv_status ret = m_cv.wait_for(lk, std::chrono::seconds(5)); // wait for 5000 ms
                if (ret == std::cv_status::timeout) {
                    VR_ERROR("waiting for interrpeter load timeout");
                    RETURN(false);
                }
            }
            else if (m_state == State::NONE) {
                VR_LOG("interpreter not be loaded");

                m_state = State::LOADING;
                VR_LOGD("m_state is LOADING");
                lk.unlock();
                loadInterpreter();
            }
        }

        {
            std::unique_lock<std::mutex> lk(m_mutex);
            VR_LOGD("start enter state PENDING lock");
            m_state = State::PENDING;
            VR_LOGD("m_state is PENDING");
            m_cv.notify_all();
        }

        m_pInterpreter->start(m_name, nice);
        return true;
    }
    else {
        VR_ERROR("Already Pending, Error! m_state is %d", m_state);
        return false;
    }
}

bool VR_InterpreterImpl::start(const std::string& xmlContent, int nice)
{
    VR_LOGD_FUNC();
    if (m_state == State::READY) {
        // ready
        VR_LOGD("Interpreter State is Ready");
        run("done.preInit", xmlContent);
    }
    else if (m_state == State::PREPARE) {
        // prepare
        VR_LOGD("Interpreter State is Prepare");
        runAfterPerpare();
    }
    else {
        VR_LOGD("Interpreter State is %d", m_state);
        // loaded
        bool isStarted = isInterpreterStarted();
        if (!isStarted) {
            VR_LOGD("Interpreter State is not Started");
            bool startState = start(nice);
            if (!startState) {
                VR_ERROR("Interpreter State Fail");
                return false;
            }
        }
        else {
            VR_ERROR("Already Started , Impossible!!!");
            return false;
        }
    }
    return true;
}

bool VR_InterpreterImpl::prepare(int nice)
{
    VR_LOGD_FUNC();
    VR_LOG("start agent name=%s, nice:%d", this->getName().c_str(), nice);
    if (m_state == State::NONE || m_state == State::LOADING || m_state == State::LOADED) {
        {
            std::unique_lock<std::mutex> lk(m_mutex);
            VR_LOGD("start enter lock");
            if (m_state == State::LOADING) {
                VR_LOG("interpreter waiting for be loaded");
                std::cv_status ret = m_cv.wait_for(lk, std::chrono::seconds(10));
                if (ret == std::cv_status::timeout) {
                    VR_ERROR("waiting for interrpeter load timeout");
                    RETURN(false);
                }
            }
            else if (m_state == State::NONE) {
                VR_LOG("interpreter not be loaded");
                m_state = State::LOADING;
                VR_LOGD("m_state is LOADING");
                lk.unlock();
                loadInterpreter();
            }
        }

        {
            std::unique_lock<std::mutex> lk(m_mutex);
            VR_LOGD("start enter state PREPARE lock");
            m_state = State::PREPARE;
            VR_LOGD("m_state is PREPARE");
            m_cv.notify_all();
        }

        m_pInterpreter->start(m_name, nice);
        VR_LOGD("prepare end %d", m_state);
        return true;
    }
    else {
        VR_ERROR("Already Prepared, Error! m_state is %d", m_state);
        return false;
    }
}

bool VR_InterpreterImpl::doStop()
{
    VR_LOG("stop agent name=%s", this->getName().c_str());

    {
        std::unique_lock<std::mutex> lk(m_mutex);
        if (!isInterpreterStarted()) {
            VR_LOG("Invalid state for stop, state:%d", m_state);
            return false;
        }

        m_listOpResult.clear();
    }
    m_pInterpreter->stop();
    std::string cancelEvt = "cancel.invoke." + m_pInterpreter->getSessionId();
    m_pInterpreter->receive(buildEventByConetent(cancelEvt, ""));
    {
        std::unique_lock<std::mutex> lk(m_mutex);
        m_state = State::NONE;
        VR_LOGD("m_state is NONE");
    }

    return true;
}

// invoke in DE thread
bool VR_InterpreterImpl::stop()
{
    VR_LOGD_FUNC();
    return doStop();
}

// only invoke in uscxml thread
bool VR_InterpreterImpl::setTargetName(const std::string& value)
{
    VR_LOGD_FUNC();
    VR_LOGD("Set target for agent[%s], value:%s", this->getName().c_str(), value.c_str());
    try
    {
        Arabica::DOM::Document<std::string> document = m_pInterpreter->getDocument();
        Arabica::DOM::Element<std::string> root = document.getDocumentElement(); // get root
        // get events
        // FIXME: optimize
        Arabica::XPath::NodeSet<std::string> sends = uscxml::InterpreterImpl::filterChildElements(m_pInterpreter->getNameSpaceInfo().xmlNSPrefix + "transition", root, true);
        for (int i = 0; i < sends.size(); ++i) {
            Arabica::DOM::Element<std::string> send = Arabica::DOM::Element<std::string>(sends[i]);
            if (HAS_ATTR(send, "backtran")) {
                send.setAttribute("target", value);
                VR_LOGD("set target = [%s]", value.c_str());
                return true;
            }
        }
        return true;
    }
    catch (std::exception& e)
    {
        VR_ERROR("build DM error: %s\n", e.what());
        return false;
    }
}

// invoke in uscxml thread
bool VR_InterpreterImpl::assignData(const std::string& keyExpr, const std::string& valueDataStr)
{
    VR_LOGD_FUNC();
    VR_LOGD("agentName=%s", this->getName().c_str());
    VR_LOG_TO_FILE("assignDataName:", keyExpr);
    VR_LOG_TO_FILE("assignDataContent:", valueDataStr);
    VR_LOG_TO_FILE("------------------------------", "");
    try
    {
        m_pInterpreter->getDataModel().assignDirectly("$" + keyExpr + "/g", valueDataStr);
        return true;
    }
    catch (...)
    {
        VR_LOGD("warnning: assign data = %s error", keyExpr.c_str());
        return false;
    }
}

void VR_InterpreterImpl::runAfterPerpare()
{
    if (m_state == State::PREPARE) {
        m_state = State::PENDING;
        VR_LOGD("m_state is PENDING");
    }
    else {
        VR_LOGD("can't runAfterPerpare, m_state is %d", m_state);
    }
}

void VR_InterpreterImpl::run(const std::string& evt, const std::string& xmlcontent)
{
    VR_LOGD_FUNC();

    if (m_state == State::READY || m_state == State::PENDING) {
        m_state = State::RUNNING;
        VR_LOGD("m_state is RUNNING");
        m_pInterpreter->receive(buildEventByConetent(evt, xmlcontent));
        VR_LOGD("done.preinit is send");
    }
    else {
        VR_LOGD("can't run, m_state is %d", m_state);
    }
}

// invoke in DE thread
bool VR_InterpreterImpl::receiveEvent(const std::string& evt, const std::string& xmlcontent)
{
    VR_LOGD_FUNC();
    std::unique_lock<std::mutex> lk(m_mutex);
    VR_LOGD("receiveEvent enter lock");
    if (State::RUNNING != m_state) {
        VR_ERROR("cann't handle event, interpreter is invalid state:%d", m_state);
        return false;
    }
    m_pInterpreter->receive(buildEventByConetent(evt, xmlcontent));
    return true;
}

bool VR_InterpreterImpl::isInterpreterStarted()
{
    if (m_state == State::RUNNING || m_state == State::PENDING
        || m_state == State::READY || m_state == State::PREPARE) {
        return true;
    }
    else {
        return false;
    }
}

// only can invoke in uscxml thread
bool VR_InterpreterImpl::receiveOpResult()
{
    VR_LOGD_FUNC();
    EventInfo evtInfo;
    {
        std::unique_lock<std::mutex> lk(m_mutex);
        if (m_listOpResult.empty()) {
            return false;
        }
        evtInfo = m_listOpResult.front();
        m_listOpResult.pop_front();
    }

    pugi::xml_document doc;
    doc.load_string(evtInfo.content.c_str());

    // remove items
    pugi::xpath_node_set itemNodeSet = doc.select_nodes("//list[@id]/items/item");

    int count = itemNodeSet.size();
    int index = 5;
    while (index < count) {
        itemNodeSet[index].node().parent().remove_child(itemNodeSet[index].node());
        ++index;
    }

    std::stringstream ss;
    doc.print(ss);

    std::string dataName(evtInfo.name + "_result");
    assignData(dataName, ss.str().c_str());

    std::string evtName("done." + evtInfo.name);
    if (evtInfo.isContentInputEvent
        || evtName == "done.queryGBookStatus") { // REVIEW: GBook need all action-result
        m_pInterpreter->receive(buildEventByConetent(evtName, evtInfo.content));
    }
    else {
        pugi::xml_node actionRetNode = doc.select_node("//action-result").node();
        pugi::xml_node childNode = actionRetNode.first_child();
        while (childNode) {
            actionRetNode.remove_child(childNode);
            childNode = actionRetNode.first_child();
        }
        std::stringstream ss;
        actionRetNode.print(ss);
        m_pInterpreter->receive(buildEventByConetent(evtName, ss.str()));
    }
    return true;
}

// invoke in DE thread
bool VR_InterpreterImpl::notifyOpResult(const std::string &op, const std::string &result, bool isContentInputEvent)
{
    VR_LOGD_FUNC();
    {
        std::unique_lock<std::mutex> lk(m_mutex);
        if (State::RUNNING != m_state) {
            VR_ERROR("cann't update data model, interpreter is invalid state:%d", m_state);
            return false;
        }
        // remove items
        pugi::xml_document doc;
        doc.load_string(result.c_str());
        pugi::xml_node node = doc.select_node("//items").node();
        while (node) {
            node.parent().remove_child("items");
            node = doc.select_node("//items").node();
        }

        m_listOpResult.push_back(EventInfo(op, result, isContentInputEvent));
    }
    receiveEvent("notifyOpResult", "");
    return true;
}

Event VR_InterpreterImpl::buildEventByConetent(const std::string& evtName, const std::string& xmlContent)
{
    VR_LOGD_FUNC();
    VR_LOGD("eventName = %s, eventContent = %s", evtName.c_str(), xmlContent.c_str());
    VR_LOG_TO_FILE("receiveEventName:", evtName);
    VR_LOG_TO_FILE("receiveEventContent:", xmlContent);
    VR_LOG_TO_FILE("------------------------------", "");
    Event evt(evtName);

    // construct dom from xmlcontent
    if (xmlContent.empty()) {
        VR_LOGD("content is empty");
        return evt;
    }
    cxh::created_result ret = cxh::create_arabica_doc_from_xml(xmlContent.c_str(), "");

    evt.setDOM(ret.doc.getFirstChild());
    return evt;
}

void VR_InterpreterImpl::resumeInterpreter()
{
    VR_LOGD_FUNC();
    if (m_state == State::RUNNING) {
        m_pInterpreter->receive(buildEventByConetent("stateResume", ""));
    }
    else {
        VR_ERROR("Resume to a not running interpreter.");
    }
}

bool VR_InterpreterImpl::receiveInitEvent(const string &xmlcontent)
{
    VR_LOGD_FUNC();
    if (m_state == State::PREPARE) {
        setPreInit(true);
    }
    else if (m_state == State::PENDING) {
        run("done.preInit", xmlcontent);
    }
    else {
        // error
        VR_LOGD("Interpreter State is %d", m_state);
        return false;
    }
    return true;
}

/* EOF */
