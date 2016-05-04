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
 * @file VR_VervoseMonitor.h
 * @brief
 *
 *
 * @attention used for C++ only.
 */
#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#ifndef _VR_VERBOSEMONITOR_
#define _VR_VERBOSEMONITOR_

#include "VR_MonitorForBack.h"
#include "uscxml/plugins/datamodel/xpath/XPathDataModel.h"
#include "uscxml/DOMUtils.h"

#define BACKATTR    "sback"
#define PUSHLAST    "pushLast"
#define NOTPUSHLAST "notPushLast"
#define AGENTIDLE   "agentIdle"

/**
 * @brief The VR_VerboseMonitor class
 *
 * verbose monitor class
 */
class VR_VerboseMonitor : public uscxml::InterpreterMonitor
{
public:
    VR_VerboseMonitor(VR_MonitorForBack* dialogEngine)
    {
        _dialogEngine = dialogEngine;
    }

    VR_VerboseMonitor(VR_VerboseMonitor& copyVm)
    {
        _dialogEngine = copyVm._dialogEngine;
    }

    VR_VerboseMonitor& operator =(const VR_VerboseMonitor& vMSource)
    {
        _dialogEngine = vMSource._dialogEngine;
        return *this;
    }

    // add tag for back, when exit the state push stack
    virtual void afterExitingState(uscxml::Interpreter interpreter, const Arabica::DOM::Element<std::string>& state, bool moreComing)
    {
        uscxml::Event currEvent = interpreter.getCurrentEvent();
        VR_LOGP("Exit state, name:%s, state:%s", currEvent.getName().c_str(), state.getAttribute("id").c_str());
        if (!strcmp(currEvent.getName().c_str(), "back")) {
            return;
        }

        // all sback save into tmp
        if (HAS_ATTR(state, BACKATTR)) {
            // push stack
            std::string str = state.getAttribute("id");
            std::string attr = state.getAttribute(BACKATTR);
            _dialogEngine->saveTmpState(str, attr);
        }
    }

    virtual void beforeEnteringState(uscxml::Interpreter interpreter, const Arabica::DOM::Element<std::string>& state, bool moreComing)
    {
        uscxml::Event currEvent = interpreter.getCurrentEvent();
        VR_LOGP("Enter state, name:%s, state:%s", currEvent.getName().c_str(), state.getAttribute("id").c_str());
        if (!strcmp(currEvent.getName().c_str(), "back")) {
            if (HAS_ATTR(state, BACKATTR)) {
                std::string str = state.getAttribute("id");
                std::string attr = state.getAttribute(BACKATTR);
                _dialogEngine->saveCurrentState(str, attr);
            }
            return;
        }

        std::string str = state.getAttribute("id");
        std::string attr = state.getAttribute(BACKATTR);
        std::string lastBackAttr = _dialogEngine->getTmpAttr();
        if (HAS_ATTR(state, BACKATTR)) {
            VR_LOG("beforeEnteringState[%s],[%s]", str.c_str(), attr.c_str());
            if (ATTR(state, BACKATTR) == PUSHLAST) {
                _dialogEngine->pushTmpToStack();
            }
            else if (ATTR(state, BACKATTR) == NOTPUSHLAST
                && lastBackAttr == NOTPUSHLAST) {
                // clear tmpstack or do nothing
                _dialogEngine->clearTmpState();
            }
            else if (ATTR(state, BACKATTR) == NOTPUSHLAST
                && lastBackAttr != NOTPUSHLAST) {
                // clear tmpstack or do nothing
                _dialogEngine->pushTmpToStack();
            }
            else if (ATTR(state, BACKATTR) == AGENTIDLE) {
                // clear the state(agent==_currentAgentName)
                _dialogEngine->pushTmpToStack();
                _dialogEngine->clearSameAgentState();
            }
            _dialogEngine->saveCurrentState(str, attr);
        }
    }

protected:
    VR_MonitorForBack* _dialogEngine;
};



/**
 * @brief The VerboseMonitorDEBUG class
 *
 * verbose monitor class
 */
class VerboseMonitorDEBUG : public uscxml::InterpreterMonitor
{
public:
    void beforeProcessingEvent(uscxml::Interpreter interpreter, const uscxml::Event& event)
    {
        std::string str = event.getName().c_str();
        VR_LOGD("beforeProcessingEvent eventName=%s", str.c_str());
    }

    void beforeMicroStep(uscxml::Interpreter interpreter)
    {
        std::string str = interpreter.getCurrentEvent().getName();
        VR_LOGD("beforeMicroStep, eventName=%s", str.c_str());
    }

    void beforeExitingState(uscxml::Interpreter interpreter, const Arabica::DOM::Element<std::string>& state, bool moreComing)
    {
        VR_LOGD("beforeExitingState, stateName=%s", state.getAttribute("id").c_str());
    }

    void afterExitingState(uscxml::Interpreter interpreter, const Arabica::DOM::Element<std::string>& state, bool moreComing)
    {
        VR_LOGD("afterExitingState, stateName=%s", state.getAttribute("id").c_str());
    }

    void beforeExecutingContent(uscxml::Interpreter interpreter, const Arabica::DOM::Element<std::string>& element)
    {
        VR_LOGD("beforeExecutingContent, content=%s", element.getNodeName().c_str());
    }

    void afterExecutingContent(uscxml::Interpreter interpreter, const Arabica::DOM::Element<std::string>& element)
    {
        VR_LOGD("afterExecutingContent, content=%s", element.getNodeName().c_str());
    }

    void beforeUninvoking(uscxml::Interpreter interpreter, const Arabica::DOM::Element<std::string>& invokeElem, const std::string& invokeid)
    {
        VR_LOGD("beforeUninvoking, invokeElem=%s", invokeElem.getAttribute("id").c_str());
    }

    void afterUninvoking(uscxml::Interpreter interpreter, const Arabica::DOM::Element<std::string>& invokeElem, const std::string& invokeid)
    {
        VR_LOGD("afterUninvoking, invokeElem=%s", invokeElem.getAttribute("id").c_str());
    }

    void beforeTakingTransition(uscxml::Interpreter interpreter, const Arabica::DOM::Element<std::string>& transition, bool moreComing)
    {
        VR_LOGD("beforeTakingTransition, transition.event=%s", transition.getAttribute("event").c_str());
    }

    void afterTakingTransition(uscxml::Interpreter interpreter, const Arabica::DOM::Element<std::string>& transition, bool moreComing)
    {
        VR_LOGD("afterTakingTransition, transition.event=%s", transition.getAttribute("event").c_str());
    }

    void beforeEnteringState(uscxml::Interpreter interpreter, const Arabica::DOM::Element<std::string>& state, bool moreComing)
    {
        VR_LOGD("beforeEnteringState, stateName=%s", state.getAttribute("id").c_str());
    }

    void afterEnteringState(uscxml::Interpreter interpreter, const Arabica::DOM::Element<std::string>& state, bool moreComing)
    {
        VR_LOGD("afterEnteringState, stateName=%s", state.getAttribute("id").c_str());
    }

    void beforeInvoking(uscxml::Interpreter interpreter, const Arabica::DOM::Element<std::string>& invokeElem, const std::string& invokeid)
    {
        VR_LOGD("beforeInvoking, invokeElem=%s", invokeElem.getAttribute("id").c_str());
    }

    void afterInvoking(uscxml::Interpreter interpreter, const Arabica::DOM::Element<std::string>& invokeElem, const std::string& invokeid)
    {
        VR_LOGD("afterInvoking, invokeElem=%s", invokeElem.getAttribute("id").c_str());
    }

    void afterMicroStep(uscxml::Interpreter interpreter)
    {
        std::string str = interpreter.getCurrentEvent().getName();
        VR_LOGD("afterMicroStep, eventName=%s", str.c_str());
    }

    void onStableConfiguration(uscxml::Interpreter interpreter)
    {
        VR_LOGD("onStableConfiguration");
    }

    void beforeCompletion(uscxml::Interpreter interpreter)
    {
        VR_LOGD("beforeCompletion");
    }

    void afterCompletion(uscxml::Interpreter interpreter)
    {
        VR_LOGD("afterCompletion");
    }

//    void reportIssue(uscxml::Interpreter interpreter, const InterpreterIssue& issue)
//    {
//        printf("reportIssue");
//    }

};

#endif
/* EOF */
