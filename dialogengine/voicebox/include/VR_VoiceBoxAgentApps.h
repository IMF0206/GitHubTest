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
 * @file VR_VoiceBoxAgentApps.h
 * @brief interface for EngineWrapper callback.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VOICEBOXAGENTAPPS_H
#define VR_VOICEBOXAGENTAPPS_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

/* Suntec Header */
#ifndef VR_VOICEBOXEVENTSINK_H
#    include "VR_VoiceBoxEventSink.h"
#endif

#ifndef MEM_MAP_H
#    include "MEM_map.h"
#endif

#ifndef MEM_VECTOR_H
#    include "MEM_vector.h"
#endif

#ifndef MEM_LIST_H
#    include "MEM_list.h"
#endif

// TSLInstallInfo
struct TSLInstallInfo
{
    std::string path;
    std::string culture;
};

// TSLUninstallInfo
struct TSLUninstallInfo
{
    std::string name;
    std::string culture;
};

/* Forward Declaration */
class VR_VoiceBoxAgentApps;

// Define the agent message handler pointer
typedef bool (VR_VoiceBoxAgentApps::*AppsMsgHandler)(IVECIParsedMessage*);

/**
 * @brief The VR_VoiceBoxAgentApps class
 *
 * class declaration
 */
class VR_VoiceBoxAgentApps : public VR_VoiceBoxEventSink
{
public:
    /**
     * Constructor
     *
     */
    VR_VoiceBoxAgentApps(
        IVECIEngineClient& client,
        IVECIEngineCommand& engineCommand,
        VR_VoiceBoxEngineCallback& engineCallback
        );

    /**
     * Destructor
     *
     */
    virtual ~VR_VoiceBoxAgentApps();

    /**
     * Initialize
     *
     * Initialize
     *
     * @param none
     * @return bool
     * @attention none
     * @see none
     */
    virtual bool Initialize();

    /**
     * UnInitialize
     *
     * UnInitialize
     *
     * @param none
     * @return none
     * @attention none
     * @see none
     */
    virtual void UnInitialize();

    /**
     * ReplyQueryInfo
     *
     * ReplyQueryInfo
     *
     * @param none
     * @return bool
     * @attention none
     * @see none
     */
    virtual bool ReplyQueryInfo(
        VR_VoiceBoxXmlParser& parser
        );

    // IVECIEventSink's pure virtual functions
    virtual HRESULT QueryAgentSupport(VBT_CSTR szAgent, VBT_BOOL *pbSupported);
    virtual HRESULT OnAgentResultMessage(IVECITransaction* pTrans, IVECIParsedMessage* pResultMsg);
    virtual HRESULT OnAgentDriverMessage(IVECIParsedMessage* pDriverMsg);

    virtual bool ProcessAgentMessage(IVECIParsedMessage *pcMsg);

    virtual bool ProcessMessage(const std::string& message);
private:
    VoiceMap<std::string, AppsMsgHandler>::type m_appsMsgHandler;
    VoiceVector<TSLInstallInfo>::type m_vecTSLInstallInfo;
    VoiceVector<TSLUninstallInfo>::type m_vecTSLUninstallInfo;

    /**
     * Process Agent Message
     *
     * Process message that was sent by VoiceBox
     *
     * @param pcMsg [IN] : Agent result message or info driver message
     * @return bool
     * @attention none
     * @see none
     */

    bool AppNotInstalled(IVECIParsedMessage *pcMsg);
    bool AppsNotAvailable(IVECIParsedMessage *pcMsg);
    bool NoAppsAvailable(IVECIParsedMessage *pcMsg);

    bool GotoApps(IVECIParsedMessage *pcMsg);
    bool LaunchApp(IVECIParsedMessage *pcMsg);
    bool ListApps(IVECIParsedMessage *pcMsg);

    bool NBest(IVECIParsedMessage *pcMsg);
    bool AppBack(IVECIParsedMessage *pcMsg);
    bool AppCancel(IVECIParsedMessage *pcMsg);
    bool AppHelp(IVECIParsedMessage *pcMsg);
    bool AppStartOver(IVECIParsedMessage *pcMsg);
    bool AppStartPause(IVECIParsedMessage *pcMsg);
    bool AppStartRepeat(IVECIParsedMessage *pcMsg);
    bool EscalatingError(IVECIParsedMessage *pcMsg);

    bool DoInstall(VR_VoiceBoxXmlParser& parser);
    bool DoUninstall(VR_VoiceBoxXmlParser& parser);
    bool DoUninstallAll();

    std::string GetCulture(std::string& language);

    /**
     * Copy Constructor
     *
     */
    VR_VoiceBoxAgentApps(const VR_VoiceBoxAgentApps&);

    /**
     * Copy Assignment Operator
     *
     */
    VR_VoiceBoxAgentApps &operator=(const VR_VoiceBoxAgentApps&);
};


#endif // VR_VOICEBOXAGENTAPPS_H
/* EOF */
