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
 * @file VR_NCNetworkAccessAdaptor.h
 * @brief adapter class and interface defines for NCNetworkAccess
 *
 *
 * @attention used for C++ only.
 */

#ifndef VR_NCNETWORKACCESSADAPTOR_H
#define VR_NCNETWORKACCESSADAPTOR_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <networkmanager/NCNetworkAccess.h>

using nutshell::ncsp;
using nutshell::NEReplyReceiver;
using nutshell::NCString;
using nutshell::NC_BOOL;
using nutshell::NCNetworkAccess;
using nutshell::AccessMode;
using nutshell::INT32;
using nutshell::UINT32;

/**
 * VR_INCNetworkAccessAdaptor
 *
 * Adaptor interface definition for NCNetworkAccess
 */
class VR_INCNetworkAccessAdaptor
{
public:
    virtual ~VR_INCNetworkAccessAdaptor() {}
    
    virtual VOID setReplyReceiver(ncsp<NEReplyReceiver>::sp callback) = 0;

    /// Binder service and register notify ID.
    virtual NC_BOOL bindService() = 0;

    /// Unbind service and unregister notify ID.
    virtual VOID unbindService() = 0;

    virtual INT32 request(
          const NCString& resourceID
        , const NCString& deviceType
        , const AccessMode& mode) = 0;

    virtual VOID release(const NCString& resourceID) = 0;
};

/**
 * VR_NCNetworkAccessAdaptor
 *
 * Adaptor class definition for NCNetworkAccess
 */
class VR_NCNetworkAccessAdaptor : public VR_INCNetworkAccessAdaptor
{
public:
    VR_NCNetworkAccessAdaptor(NCNetworkAccess* pNCNetworkAccess)
        : m_spNCNetworkAccess(pNCNetworkAccess)
    {
    }

    VOID setReplyReceiver(ncsp<NEReplyReceiver>::sp callback)
    {
        m_spNCNetworkAccess->setReplyReceiver(callback);
    }

    NC_BOOL bindService()
    {
        return m_spNCNetworkAccess->bindService();
    }

    VOID unbindService()
    {
        m_spNCNetworkAccess->unbindService();
    }

    INT32 request(const NCString& resourceID, const NCString& deviceType, const AccessMode& mode)
    {
        return m_spNCNetworkAccess->request(resourceID, deviceType, mode);
    }

    VOID release(const NCString& resourceID)
    {
        m_spNCNetworkAccess->release(resourceID);
    }

private:
    ncsp<NCNetworkAccess>::sp m_spNCNetworkAccess;
};

#endif /* VR_NCNETWORKACCESSADAPTOR_H */
/* EOF */
