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
 * @file VR_VoiceBoxEngineIF.h
 * @brief interface for EngineWrapper callback.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VOICEBOXENGINEIF_H
#define VR_VOICEBOXENGINEIF_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

/* Standard Header */
#include <string>

/* Forward Declaration */
class VR_DialogEngineListener;
class VR_VoiceBoxController;
class VR_VoiceBoxCatalogManager;

/**
 * @brief The VR_VoiceBoxEngineIF class
 *
 * class declaration
 */
class VR_VoiceBoxEngineIF
{
public:
    /**
     * Factory Method
     *
     * Create a VoiceBox Engine instance
     *
     */
    static VR_VoiceBoxEngineIF* CreateInstance();

    /**
     * Destructor
     *
     */
    virtual ~VR_VoiceBoxEngineIF() {}

    /**
     * Initialize
     *
     * Initialize VoiceBox Engine
     *
     * @param pcExternalCallback  [IN] : External service's callback
     * @param pcMsgController     [IN] : XML message controller
     * @param pcCatalogController [IN] : XML message controller
     * @return bool
     * @attention none
     * @see none
     */
    virtual bool Initialize(
        VR_DialogEngineListener* pcExternalCallback,
        VR_VoiceBoxController*  pcMsgController,
        VR_VoiceBoxController*  pcCatalogController
        ) = 0;

    /**
     * Start
     *
     * Start VoiceBox Engine
     *
     * @param strCultureName [IN] : Culture name
     * @return bool
     * @attention none
     * @see none
     */
    virtual bool Start(const std::string &strCultureName) = 0;

    /**
     * Stop
     *
     * Stop VoiceBox Engine
     *
     * @param none
     * @return none
     * @attention none
     * @see none
     */
    virtual void Stop() = 0;

    /**
     * UnInitialize
     *
     * Shutdown VoiceBox Engine
     *
     * @param none
     * @return none
     * @attention none
     * @see none
     */
    virtual void UnInitialize() = 0;

    /**
     * ProcessMessage
     *
     * Process Voice Recognition related messages
     *
     * @param none
     * @return none
     * @attention none
     * @see none
     */
    virtual void ProcessMessage(const std::string& message, int actionSeqId  = -1) = 0;

    /**
     * GetCatalogManager
     *
     * Get the pointer for catalog manager
     *
     * @param none
     * @return VR_VoiceBoxCatalogManager*
     * @attention none
     * @see none
     */
    virtual VR_VoiceBoxCatalogManager* GetCatalogManager() = 0;

    // virtual void setPromptInterrupt(bool promptInterrupt) = 0;
    virtual void setPromptLevel(int promptLevel) = 0;

    virtual std::string getHints(const std::string& hintsParams) = 0;
};

#endif // VR_VOICEBOXENGINEIF_H
/* EOF */
