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
 * @file VR_InterpreterImpl.h
 * @brief realize the interface of VR_Interpreter
 *
 *
 * @attention used for C++ only.
 */
#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#ifndef _VR_INTERPRETERImpl_H_
#define _VR_INTERPRETERImpl_H_


#include <condition_variable>
#include <mutex>
#include "VR_Interpreter.h"
#include "uscxml/Interpreter.h"
#include <string.h>
#include "MEM_list.h"

namespace voice
{
/**
 * @brief The VR_InterpreterImpl class
 *
 * for VR_InterpreterImpl
 */
class VR_InterpreterImpl: public voice::VR_Interpreter
{
public:
    struct EventInfo
    {
        EventInfo(std::string na, std::string con, bool is)
        {
            name = na;
            content = con;
            isContentInputEvent = is;
        }

        EventInfo()
        {
            name = "";
            content = "";
            isContentInputEvent = true;
        }

        std::string name;
        std::string content;
        bool isContentInputEvent;
    };

    enum State
    {
        NONE = 0,
        LOADING,
        LOADED,
        PENDING,
        READY,
        RUNNING,
        PREPARE
    };

    explicit VR_InterpreterImpl(VR_MonitorForBack* dialogEngine, const std::string& name, const std::string& scxmlPath);
    virtual ~VR_InterpreterImpl();

    virtual bool bindServiceRequestor(VRServiceRequestor* pRequestor);
    virtual std::string getName();
    virtual void load();
    virtual bool start(const std::string& xmlContent, int nice);
    virtual bool start(int nice);
    virtual bool prepare(int nice);
    virtual bool stop();
    virtual void setPreInit(bool isPreInit);
    virtual bool receiveEvent(const std::string& evt, const std::string& xmlContent);
    virtual bool notifyOpResult(const std::string &op, const std::string &result, bool isContentInputEvent);
    virtual bool receiveOpResult();
    virtual bool setTargetName(const std::string& value);
    virtual bool assignData(const std::string& keyExpr, const std::string& valueDataStr);
    virtual bool updateGlobalStates(const std::string& globalStates);
    virtual bool isInterpreterStarted();
    virtual void resumeInterpreter();
    virtual bool receiveInitEvent(const std::string& xmlcontent);
protected:
    uscxml::Event buildEventByConetent(const std::string& evt, const std::string& xmlContent);
    bool doStop();
    bool loadInterpreter();

private:
    virtual void run(const std::string& evt, const std::string& xmlcontent);
    virtual void runAfterPerpare();
    explicit VR_InterpreterImpl(VR_InterpreterImpl& copy);
    VR_InterpreterImpl& operator= (const VR_InterpreterImpl& source);

    std::mutex m_mutex;
    std::condition_variable m_cv;

    State m_state;
    std::string m_name;
    std::string m_scxmlPath;
    std::mutex m_mutexLoad;

    VoiceList<EventInfo>::type m_listOpResult;
    uscxml::Interpreter* m_pInterpreter;
    uscxml::InterpreterMonitor* m_pMonitor;
};
}
#endif // _VR_INTERPRETERImpl_H_
/* EOF */
