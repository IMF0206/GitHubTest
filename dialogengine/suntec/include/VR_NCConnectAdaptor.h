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
 * @file VR_NCConnectAdaptor.h
 * @brief adapter class and interface defines for NCConnect
 *
 *
 * @attention used for C++ only.
 */

#ifndef VR_NCCONNECTADAPTOR_H
#define VR_NCCONNECTADAPTOR_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <connectmanager/NCConnect.h>

using nutshell::ncsp;
using nutshell::NEReplyReceiver;
using nutshell::NCString;
using nutshell::NC_BOOL;
using nutshell::NCConnect;

/**
 * VR_INCConnectAdaptor
 *
 * Adaptor interface definition for NCConnect
 */
class VR_INCConnectAdaptor
{
public:
    virtual ~VR_INCConnectAdaptor() {}
    
    virtual VOID setReplyReceiver(ncsp<NEReplyReceiver>::sp callback) = 0;

    /// Binder service and register notify ID.
    virtual NC_BOOL bindService() = 0;

    /// Unbind service and unregister notify ID.
    virtual VOID unbindService() = 0;

    virtual VOID request(const NCString& resource, const NCString& use,
            const NCString& role, NCString& resourceID) = 0;

    virtual VOID request(const NCString& resource, const NCString& use,
            const NCString& role, const NCString& netType, NCString& resourceID) = 0;

    virtual VOID release(const NCString& resourceID) = 0;

    virtual VOID replyInterrupt(const NCString& resourceID, const NCString& result) = 0;

    virtual VOID stopWait(const NCString& resourceID) = 0;
};

/**
 * VR_NCConnectAdaptor
 *
 * Adaptor class definition for NCConnect
 */
class VR_NCConnectAdaptor : public VR_INCConnectAdaptor
{
public:
    VR_NCConnectAdaptor(NCConnect* pNCConnect)
        : m_spNCConnect(pNCConnect)
    {
    }

    VOID setReplyReceiver(ncsp<NEReplyReceiver>::sp callback)
    {
        m_spNCConnect->setReplyReceiver(callback);
    }

    NC_BOOL bindService()
    {
        return m_spNCConnect->bindService();
    }

    VOID unbindService()
    {
        m_spNCConnect->unbindService();
    }

    VOID request(const NCString& resource, const NCString& use,
            const NCString& role, NCString& resourceID)
    {
        m_spNCConnect->request(resource, use, role, resourceID);
    }

    VOID request(const NCString& resource, const NCString& use,
            const NCString& role, const NCString& netType, NCString& resourceID)
    {
        m_spNCConnect->request(resource, use, role, netType, resourceID);
    }

    VOID release(const NCString& resourceID)
    {
        m_spNCConnect->release(resourceID);
    }

    VOID replyInterrupt(const NCString& resourceID, const NCString& result)
    {
        m_spNCConnect->replyInterrupt(resourceID, result);
    }

    VOID stopWait(const NCString& resourceID)
    {
        m_spNCConnect->stopWait(resourceID);
    }

private:
    ncsp<NCConnect>::sp m_spNCConnect;
};

#endif /* VR_NCCONNECTADAPTOR_H */
/* EOF */
