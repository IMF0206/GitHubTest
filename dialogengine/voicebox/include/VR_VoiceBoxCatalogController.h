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
 * @file VR_VoiceBoxCatalogController.h
 * @brief calss for cataloguing data
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VOICEBOXCATALOGCONTROLLER_H
#define VR_VOICEBOXCATALOGCONTROLLER_H

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
class VR_VoiceBoxCatalogManager;

/**
 * @brief The VR_VoiceBoxCatalogController class
 *
 * class declaration
 */
class VR_VoiceBoxCatalogController : public VR_VoiceBoxController, public BL_Thread
{
public:
    /**
     * Constructor
     *
     */
    VR_VoiceBoxCatalogController();

    /**
     * Destructor
     *
     */
    virtual ~VR_VoiceBoxCatalogController();

    /**
     * Initialize
     *
     * Initialize Catalog Controller
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
     * Start the Catalog Controller thread
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
     * Stop the Catalog Controller thread
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
     * Post message to the Controller
     *
     * @param message [IN] : XML format message
     * @return bool
     * @attention none
     * @see none
     */
    virtual bool PostMessage(const std::string& message, int actionSeqId = -1);

    virtual void ClearMessage();

    virtual void ProcessOpMessage(const std::string& strOpMsg);

    virtual std::string GetCurrentMessage();

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
    VoiceList<std::string>::type  m_msglist;      ///< Message list
    VR_VoiceBoxEngineIF*    m_voiceBoxEngine;     ///< VoiceBox dialog engine
    VR_VoiceBoxCatalogManager*  m_pcCatalogManager;   ///< Catalog manager
    bool                                           m_bUpdateGrammar;
    int                                              m_totalindex;
    int                                              m_index;
    std::string                                  m_strAudioSource;
    std::string                                  m_strCurrentMsg;
    std::string                                  m_strUpdateAgainMsg;
    bool                                           m_bNeedPauseGrammar;
    bool                                           m_bNeedResumeGrammar;

    /**
     * Copy Constructor
     *
     */
    VR_VoiceBoxCatalogController(const VR_VoiceBoxCatalogController&);

    /**
     * Copy Assignment Operator
     *
     */
    VR_VoiceBoxCatalogController &operator=(const VR_VoiceBoxCatalogController&);

    void DeletePhoneGrammarBuf();

};

#endif // VR_VOICEBOXCATALOGCONTROLLER_H
/* EOF */
