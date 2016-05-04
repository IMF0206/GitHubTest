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
 * @file VR_Interpreter.h
 * @brief inteface
 *
 *
 * @attention used for C++ only.
 */
#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#ifndef _VR_INTERPRETER_H_
#define _VR_INTERPRETER_H_

#include "config.h"

#include <DOM/Document.hpp>
#include <string.h>


class VRServiceRequestor;
class VR_MonitorForBack;
namespace voice
{
/**
 * @brief The VR_Interpreter class
 *
 * for VR_Interpreter
 */
class VR_Interpreter
{
public:
    virtual ~VR_Interpreter() {}
    virtual bool bindServiceRequestor(VRServiceRequestor* requestor) = 0;
    virtual void load() = 0;
    virtual bool start(int nice) = 0;
    virtual bool start(const std::string& xmlcontent, int nice) = 0;
    virtual bool prepare(int nice) = 0;
    virtual bool receiveEvent(const std::string& evt, const std::string& xmlcontent) = 0;
    virtual bool stop() = 0;
    virtual void setPreInit(bool isPre) = 0;
    // some interface need manipulation data shuold be offered by interpreter
    virtual bool setTargetName(const std::string& value) = 0; // for back
    // assign query action data to interpreter,ps:$keyExpr/g/valueDataStr.childrenNodes
    virtual bool assignData(const std::string& keyExpr, const std::string& valueDataStr) = 0;
    virtual std::string getName() = 0;
    virtual bool updateGlobalStates(const std::string& globalStates) = 0; // update location globalstate to interpreter
    virtual bool receiveOpResult() = 0;
    virtual bool notifyOpResult(const std::string &op, const std::string &result, bool isContentInputEvent) = 0;

    /**
    *  function : check Interpreter is start or not
    *  param    : none
    *  return   : trun  , when Interpreter was started
    *             false , when Interpreter isn't start
    */
    virtual bool isInterpreterStarted() = 0;

    virtual void resumeInterpreter() = 0;

    virtual bool receiveInitEvent(const std::string& xmlcontent) = 0;
};

}

#endif // _VR_INTERPRETER_H_
/* EOF */
