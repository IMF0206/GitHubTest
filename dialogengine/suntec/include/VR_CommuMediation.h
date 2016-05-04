/**
 * Copyright @ 2013 - 2015 Suntec Software(Shanghai) Co., Ltd.
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
 * @file VR_CommuMediation.h
 * @brief communication mediation class define
 *
 *
 * @attention used for C++ only.
 */

#ifndef VR_COMMUMEDIATION_H
#define VR_COMMUMEDIATION_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <connectmanager/NCConnectReplier.h>
#include <networkmanager/NCNetworkAccessReplier.h>
#include "VR_NCConnectAdaptor.h"
#include "VR_NCNetworkAccessAdaptor.h"
#include "VR_Log.h"

using nutshell::NCConnectReplier;
using nutshell::NCNetworkAccessReplier;

/**
 * VR_CommuMediation
 *
 * Provide communication mediation implementation.
 */
class VR_CommuMediation
{
public:
    typedef enum {
        StatusConnected = 1,
        StatusInterrupted = 2
    } ConnStatusEnum;

    typedef enum {
        StateUninitialized = 0,
        StateConnectReject = 1,
        StateConnectPermit = 2,
        StateConnectWait = 4,
        StateNetworkReady = 3
    } StateEnum;

    typedef enum {
        Idle = 0,
        RequestIssued = 1,
        ReleaseIssued = -1,
        Any = 10 // add the value won't result in a valid state
    } ActionEnum;

    typedef boost::function<int(int)> OnStatusChangedFuncPtr;
    friend class VR_CommuMediationReplier;
    friend class VR_CommuMediationNetReplier;

    virtual ~VR_CommuMediation();

    VOID SetOnStatusChanged(OnStatusChangedFuncPtr funcPtr)
    {
        m_OnStatusChanged = funcPtr;
    }

    bool Init();
    bool Request();
    bool Release();
    VOID Deinit();
    int OnStatusChanged(ConnStatusEnum status);

    StateEnum GetState()
    {
        return m_state;
    }

    /**
     * VR_CommuMediationReplier
     *
     * Replier for connect proxy
     */
    class VR_CommuMediationReplier : public NCConnectReplier
    {
    public:
        VR_CommuMediationReplier();
        virtual ~VR_CommuMediationReplier();

        VOID setCommuMediation(VR_CommuMediation* pCommuMediation)
        {
            m_pCommuMediation = pCommuMediation;
        }

        /// Reply for IF request
        virtual VOID replyRequest(
              const NCString& resourceID
            , const NCString& netType
            , const NCString& result);

        /// Reply for wait.
        virtual VOID replyWaitPermit(const NCString& resourceID);

        /// Reply for release.
        virtual VOID replyRelease(const NCString& resourceID, const NCString& result);

        /// Interrupt current application.
        virtual VOID requestInterrupt(const NCString& resourceID, const NCString& content);

        /// Reply for IF stopWait
        virtual VOID replyStopWait(const NCString& resourceID);

    private:
        NC_DISABLE_COPY(VR_CommuMediationReplier);
        VR_CommuMediation* m_pCommuMediation;
    };

    /**
     * VR_CommuMediationNetReplier
     *
     * Replier for network access proxy
     */
    class VR_CommuMediationNetReplier : public NCNetworkAccessReplier
    {
    public:
        VR_CommuMediationNetReplier();
        virtual ~VR_CommuMediationNetReplier();

        VOID setCommuMediation(VR_CommuMediation* pCommuMediation)
        {
            m_pCommuMediation = pCommuMediation;
        }

        /// Reply for Network request
        virtual VOID replyRequest(const NCString& resourceID, UINT32 result);

        /// Reply for NEtwork release
        virtual VOID replyRelease(const NCString& resourceID, UINT32 result);

    private:
        NC_DISABLE_COPY(VR_CommuMediationNetReplier);
        VR_CommuMediation* m_pCommuMediation;
    };

    VR_CommuMediation(
          VR_INCConnectAdaptor*        pINCConnectAdapter
        , VR_INCNetworkAccessAdaptor*  pINCNetworkAccessAdaptor
        , VR_CommuMediationReplier*    pCommuMediationReplier
        , VR_CommuMediationNetReplier* pCommuMediationNetReplier);

    VR_CommuMediation();

private:
    NC_DISABLE_COPY(VR_CommuMediation);

    VOID SetState(StateEnum state)
    {
        VR_LOGD("%s -> %s", GetStateString(m_state), GetStateString(state));
        m_state = state;
    }

    static const char* GetStateString(StateEnum state);

    ncsp<VR_CommuMediationReplier>::sp    m_replier;
    ncsp<VR_CommuMediationNetReplier>::sp m_netreplier;
    boost::shared_ptr<VR_INCConnectAdaptor>       m_connectproxy;
    boost::shared_ptr<VR_INCNetworkAccessAdaptor> m_networkproxy;
    NCString m_resourceID;
    NCString m_resource;
    NCString m_use;
    NCString m_role;
    NCString m_netType;
    StateEnum m_state;
    OnStatusChangedFuncPtr m_OnStatusChanged;
    bool m_interruptFlag;
    bool m_disconnectFlag;
    int  m_netRefCount; // handle the case that a long time wait is required for network request's response
    ActionEnum m_actionState;
};

#endif /* VR_COMMUMEDIATION_H */
/* EOF */
