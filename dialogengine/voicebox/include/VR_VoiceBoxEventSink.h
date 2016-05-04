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
 * @file VR_VoiceBoxEventSink.h
 * @brief interface for EngineWrapper callback.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VOICEBOXEVENTSINK_H
#define VR_VOICEBOXEVENTSINK_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <string>

/* VBT Header */
#ifndef __VECIIface_h__
#    include "VECIIface.h"
#endif

#ifndef VECIPTR_H
#    include "VECIPtr.h"
#endif

/* Suntec Header */
#ifndef MEM_VECTOR_H
#    include "MEM_vector.h"
#endif

/* Forward Declaration */
class VR_VoiceBoxEngineCallback;
class VR_VoiceBoxXmlParser;
class IVECIEngineCommand;

/**
 * @brief The VR_VoiceBoxEventSink class
 *
 * class declaration
 */
class VR_VoiceBoxEventSink : public IVECIEventSink
{
public:
    /**
     * Constructor
     *
     */
    VR_VoiceBoxEventSink(
        IVECIEngineClient& client,
        IVECIEngineCommand& engineCommand,
        VR_VoiceBoxEngineCallback& engineCallback
        );

    /**
     * Destructor
     *
     */
    virtual ~VR_VoiceBoxEventSink();

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
    virtual bool Initialize() = 0;

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
    virtual void UnInitialize() = 0;

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
        VR_VoiceBoxXmlParser& parser) = 0;

    virtual bool ProcessMessage(
        const std::string& message
        );

    // IUnknown's pure virtual funcitons
    virtual HRESULT QueryInterface(REFIID riid, void** ppvObj);
    virtual VBT_ULONG AddRef();
    virtual VBT_ULONG Release();

    // IVECIEventSink's pure virtual functions
    virtual HRESULT OnCommandComplete(IVECITransaction* pTrans);
    virtual HRESULT OnClientMessage(IVECIParsedMessage* pClientMsg);
    virtual HRESULT OnAudioLevelUpdate(VBT_LONG lValue);
    virtual HRESULT OnExtendedEvent(VeciExtendedEvent type, IUnknown* pUnk);
    virtual HRESULT OnProcessStatus(VBT_CSTR szState);
    virtual HRESULT OnSystemNotification(unsigned long ulCode, IVECIParameterSet *pParameterSet);
    virtual HRESULT OnKeywordActivation();
    virtual HRESULT OnSpeak(IVECITransaction *pTrans, IVECIStringSet* pSpeakList);
    virtual HRESULT OnBeepSync();
    virtual bool ProcessAgentMessage(IVECIParsedMessage *pcMsg) = 0;

    static std::string         m_strPrompt;
protected:
    IVECIEngineClient&         m_client;       ///< EngineClient
    VR_VoiceBoxEngineCallback& m_engineCallback; ///< Callback for the dialog engine
    IVECIEngineCommand&        m_engineCommand; ///< Engine command

    struct NodeInfo
    {
        std::string strName;
        std::string strValue;
    };

    struct ListNodeInfo
    {
        std::string strListType;
        VoiceVector<VoiceVector<NodeInfo>::type>::type vecItems;
    };

    std::string GetActionParaByName(IVECIParsedMessage *pcMsg, 
            const VBT_STR& strElementName);

    std::string GetListParaByName(IVECIParsedMessage *pcMsg, 
            int index, const std::string& strListParaName);

    bool GetActionParameter(
        IVECIParsedMessage *pcMsg,
        VoiceVector<NodeInfo>::type& vecActionParameter
    );

    bool GetListParameter(
        IVECIParsedMessage *pcMsg,
        ListNodeInfo& listParameter
    );

    bool ReStartRecoSession();

    std::string GetStartIndex(IVECIParsedMessage *pcMsg);

    bool SendActionMessage(IVECIParsedMessage *pcMsg);

    void ShowPopupMsg(const std::string& strPrompt);

private:
    long                       m_reference;    ///< Reference count

    /**
     * Copy Constructor
     *
     */
    VR_VoiceBoxEventSink(const VR_VoiceBoxEventSink&);

    /**
     * Copy Assignment Operator
     *
     */
    VR_VoiceBoxEventSink &operator=(const VR_VoiceBoxEventSink&);
};


#endif // VR_VOICEBOXEVENTSINK_H
/* EOF */
