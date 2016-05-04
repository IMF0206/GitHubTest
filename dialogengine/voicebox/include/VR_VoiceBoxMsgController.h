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
 * @file VR_VoiceBoxMsgController.h
 * @brief interface for EngineWrapper callback.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VOICEBOXMSGCONTROLLER_H
#define VR_VOICEBOXMSGCONTROLLER_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

/* Standard Header */
#include <string>

/* Suntec Header */
#ifndef MEM_LIST_H
#    include "MEM_list.h"
#endif

#ifndef CXX_BL_THREAD_H
#    include "BL_Thread.h"
#endif

#ifndef CXX_BL_SyncObject_H
#   include "BL_SyncObject.h"
#endif

#ifndef VR_VOICEBOXCONTROLLER_H
#    include "VR_VoiceBoxController.h"
#endif

/* Forward Declaration */
class VR_VoiceBoxEngineIF;

/**
 * @brief The VR_VoiceBoxMsgController class
 *
 * class declaration
 */
class VR_VoiceBoxMsgController : public VR_VoiceBoxController, public BL_Thread
{
public:
    /**
     * Constructor
     *
     */
    VR_VoiceBoxMsgController();

    /**
     * Destructor
     *
     */
    virtual ~VR_VoiceBoxMsgController();

    /**
     * Initialize
     *
     * Initialize Message Controller
     *
     * @param pcEngine [IN] : Voice recognized engine
     * @return bool
     * @attention none
     * @see none
     */
    bool Initialize(VR_VoiceBoxEngineIF* pcEngine);

    /**
     * Start
     *
     * Start the Message Controller thread
     *
     * @param none
     * @return none
     * @attention none
     * @see none
     */
    void Start();

    /**
     * Start
     *
     * Stop the Message Controller thread
     *
     * @param none
     * @return none
     * @attention none
     * @see none
     */
    void Stop();

    /**
     * Post Message
     *
     * Post message to the Message Controller
     *
     * @param message [IN] : XML format message
     * @return bool
     * @attention none
     * @see none
     */
    bool PostMessage(const std::string& message, int actionSeqId = -1);

    void ClearMessage();

    void ProcessOpMessage(const std::string& strOpMsg);

    std::string GetCurrentMessage();

protected:
    /**
     * Run
     *
     * Override base class's Run() function
     *
     * @param none
     * @return none
     * @attention none
     * @see none
     */
    virtual void Run();

private:
    static BL_SyncObject            s_cSync;      ///< Sync object
    struct VR_MsgInfo
    {
        int iMsgId;
        std::string strMsg;
    };

    VoiceList<VR_MsgInfo>::type  m_msglist;      ///< Message list
    VR_VoiceBoxEngineIF*    m_voiceBoxEngine;     ///< VoiceBox dialog engine

    /**
     * Copy Constructor
     *
     */
    VR_VoiceBoxMsgController(const VR_VoiceBoxMsgController&);

    /**
     * Copy Assignment Operator
     *
     */
    VR_VoiceBoxMsgController &operator=(const VR_VoiceBoxMsgController&);
};

#endif // VR_VOICEBOXMSGCONTROLLER_H
/* EOF */
