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

/**
 * @file VR_DEMonitorIF.h
 * @brief dialog engine class interface
 *
 *
 * @attention used for C++ only.


 */
#ifndef VR_INTERPRETERMANAGER_H
#define VR_INTERPRETERMANAGER_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <string>
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "MEM_map.h"
#include "MEM_queue.h"
#include "MEM_deque.h"
#include "VR_Interpreter.h"
#include "VR_InterpreterImpl.h"
#include "VR_ConfigureIF.h"
#include "VR_DECommonIF.h"

class VR_DialogEngine;

/**
 * @brief The VR_InterpreterManager class
 *
 * VR_InterpreterManager class
 */
class VR_InterpreterManager
{
public:
    VR_InterpreterManager(VR_MonitorForBack* pMonitor, VRServiceRequestor* pRequestor, VR_DECommonIF* pCommon);
    virtual ~VR_InterpreterManager();

    bool start();

    bool changeAgent(const std::string& agentName);

    bool startAgent(const std::string& agentName);
    bool startAgent(const std::string& agentName, const std::string& initParm);
    bool backAgent(const std::string& agentName);
    bool stopAgent();
    bool setPreInit(bool isPreInit);
    bool receiveEvent(const std::string& evt, const std::string& xmlcontent);
    bool receiveInitEvent(const std::string& evt, const std::string& xmlcontent);

    bool setTargetName(const std::string& value);
    bool notifyUpdateGlobalStates(const std::string& updateStr);
    bool updateGlobalStates();

    bool receiveOpResult();
    bool notifyOpResult(const std::string &op, const std::string &result, bool isContentInputEvent = true);

    /**
     * Stop current agent
     */
    bool stop();

    bool prepareAgent(const std::string& agentName);

protected:
    static void threadProc(void* instance);

    VR_InterpreterManager(const VR_InterpreterManager& other);
    VR_InterpreterManager& operator= (const VR_InterpreterManager& other);

    void loadAgents();
    void pushLoadAgent(const std::string& agentName);

    void initScxml();
    void initScxmlByRegion(const std::string& region);
    void initScxmlFromData();
    void initScxmlFromEtc();

    bool isAgentNeedStop(const std::string& agentName);
private:
    VR_MonitorForBack* m_pMonitorForBack;
    VRServiceRequestor* m_pServiceRequestor;
    VR_DECommonIF* m_pCommon;

    VoiceMap<std::string, std::string>::type m_scxmls;
    VoiceMap<std::string, std::shared_ptr<voice::VR_Interpreter> >::type m_interpreters;

    std::shared_ptr<voice::VR_Interpreter> m_spInterpreterActive;
    std::string m_globalStates;
    std::mutex m_mutexGlobalState;
    VoiceDeque<std::string>::type m_queueLoad;
    std::mutex m_mutexLoad;
    std::condition_variable m_cvLoad;
    std::atomic<bool> m_running;
    tthread::thread* m_pThreadLoad;
    bool m_isFirstReadyLoadAgent;
};

#endif // VR_INTERPRETERMANAGER_H
/* EOF */
