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

#include "VR_Def.h" // contains MEM_map.h
#include "VR_CommuMediation.h"
#include <unistd.h> // usleep
#include <boost/assign/list_of.hpp>

using namespace nutshell;

#define RESOURCE_NAME "wireless"
#define USE_NAME      "voice"
#define ROLE_NAME     "centervr"
#define WIFI_NET_TYPE "wifiSta"
#define WIFI_NET_TYPE_FAKE "wifista"
#define RESOURCE_ID   RESOURCE_NAME "#" USE_NAME "#" ROLE_NAME

#define EXPECT_STATE(statevar, stateenum, retval) do {\
	if ((stateenum) != (statevar)) {\
	    VR_LOG("should in %s state, however in %s instead",\
	  	    VR_CommuMediation::GetStateString(stateenum),\
	  	    VR_CommuMediation::GetStateString(statevar));\
	    return retval;\
	}\
} while (0)

#define EXPECT_STATE_VOID(statevar, stateenum) do {\
    if ((stateenum) != (statevar)) {\
        VR_LOG("should in %s state, however in %s instead",\
            VR_CommuMediation::GetStateString(stateenum),\
            VR_CommuMediation::GetStateString(statevar));\
        return;\
    }\
} while (0)

#define COMMED_LOGD(expr) do {\
    VR_LOGD("->" #expr);\
    expr;\
    VR_LOGD("<-" #expr);\
} while (0)

#define CONFIRM_RESOURCEID_AND_ACTION(p,action) do {\
    if (!p) {\
        VR_ERROR("m_pCommuMediation is NULL");\
        return;\
    }\
    if (resourceID != p->m_resourceID) {\
        VR_LOGD("got resourceID [%s], expect [%s], bypass.", resourceID.getString(), p->m_resourceID.getString());\
        return;\
    }\
    if (action != VR_CommuMediation::Any) {\
        if (action != p->m_actionState) {\
            VR_LOGD("WARNING: detect actionState [%d], expect [%d], cancel", p->m_actionState, action);\
            return;\
        }\
    }\
} while (0)

// invoke the macro only after checks passed, including EXPECT_STATE
#define RESET_ACTION(p) do {\
    p->m_actionState = VR_CommuMediation::Idle;\
} while (0)

#define SLEEP_MS(n) usleep((n)*1000)
#define RETRY_PERIOD_MS 500
#define DTOR_CHECK_INTV 50
#define DTOR_WAIT_TIMEOUT 5000

static VoiceMap<VR_CommuMediation::StateEnum, std::string>::type s_states = boost::assign::map_list_of
        (VR_CommuMediation::StateUninitialized, "StateUninitialized")
        (VR_CommuMediation::StateConnectReject, "StateConnectReject")
        (VR_CommuMediation::StateConnectPermit, "StateConnectPermit")
        (VR_CommuMediation::StateConnectWait,   "StateConnectWait")
        (VR_CommuMediation::StateNetworkReady,  "StateNetworkReady");

const char *
VR_CommuMediation::GetStateString(VR_CommuMediation::StateEnum state)
{
    if (s_states.find(state) != s_states.end()) {
        return s_states[state].c_str();
    }
    else {
        return "<State UNKNOWN>";
    }
}

VR_CommuMediation::VR_CommuMediation(
      VR_INCConnectAdaptor*        pINCConnectAdapter
    , VR_INCNetworkAccessAdaptor*  pINCNetworkAccessAdaptor
    , VR_CommuMediationReplier*    pCommuMediationReplier
    , VR_CommuMediationNetReplier* pCommuMediationNetReplier)
: m_replier(pCommuMediationReplier)
, m_netreplier(pCommuMediationNetReplier)
, m_connectproxy(pINCConnectAdapter)
, m_networkproxy(pINCNetworkAccessAdaptor)
, m_resourceID(RESOURCE_ID)
, m_resource(RESOURCE_NAME)
, m_use(USE_NAME)
, m_role(ROLE_NAME)
, m_netType("")
, m_state(VR_CommuMediation::StateUninitialized)
, m_OnStatusChanged(NULL)
, m_interruptFlag(false)
, m_disconnectFlag(false)
, m_netRefCount(0)
, m_actionState(VR_CommuMediation::Idle)
{
    VR_LOGD_FUNC();
    m_replier->setCommuMediation(this);
    m_netreplier->setCommuMediation(this);
    m_connectproxy->setReplyReceiver(m_replier);
    m_networkproxy->setReplyReceiver(m_netreplier);
}

VR_CommuMediation::VR_CommuMediation()
: VR_CommuMediation(VR_new VR_NCConnectAdaptor(VR_new NCConnect())
    , VR_new VR_NCNetworkAccessAdaptor(VR_new NCNetworkAccess())
    , VR_new VR_CommuMediation::VR_CommuMediationReplier()
    , VR_new VR_CommuMediation::VR_CommuMediationNetReplier())
{
}

VR_CommuMediation::~VR_CommuMediation()
{
    VR_LOGD_FUNC();
    int cnt = (DTOR_WAIT_TIMEOUT) / (DTOR_CHECK_INTV);
    if (VR_CommuMediation::StateUninitialized == m_state) {
        return;
    }

    if (VR_CommuMediation::StateConnectReject != m_state) {
        Release();
    }
    while (VR_CommuMediation::StateConnectReject != m_state) {
        SLEEP_MS(DTOR_CHECK_INTV);
        if (--cnt <= 0) {
            VR_LOGD("not changed to StateConnectReject state by %d ms, Deinit forcibly", DTOR_WAIT_TIMEOUT);
            break;
        }
    }
    Deinit();
}

bool
VR_CommuMediation::Init()
{
    VR_LOGD_FUNC();
    EXPECT_STATE(m_state, VR_CommuMediation::StateUninitialized, false);

    if (NC_FALSE == m_connectproxy->bindService()) { // bind service failure.
        VR_LOGD("***> bind connection service failure.");
        return false;
    }

    if (NC_FALSE == m_networkproxy->bindService()) { // bind service failure.
        VR_LOGD("***> bind network service failure.");
        m_connectproxy->unbindService();
        return false;
    }
    VR_LOGD("-->bind connection & network service success.");
    SetState(VR_CommuMediation::StateConnectReject);
    return true;
}

VOID
VR_CommuMediation::Deinit()
{
    VR_LOGD_FUNC();
    EXPECT_STATE_VOID(m_state, VR_CommuMediation::StateConnectReject);

    m_networkproxy->unbindService();
    m_connectproxy->unbindService();
    SetState(VR_CommuMediation::StateUninitialized);
    m_OnStatusChanged = NULL;
}

bool
VR_CommuMediation::Request()
{
    VR_LOGD_FUNC();
    AccessMode mode = { DialType_Auto, CommunicatePriority_AutoHigh };
    if (VR_CommuMediation::RequestIssued == m_actionState) {
        VR_LOGD("WARNING: already sent reQuest, no need to send again.");
        return false;
    }
    else if (VR_CommuMediation::ReleaseIssued == m_actionState) {
        VR_LOGD("WARNING: please reQuest until release done.");
        // StateEnum state = m_state;
        // m_state = state + m_actionState;
        return false;
    }
    m_actionState = VR_CommuMediation::RequestIssued;

    switch (m_state) {
    case VR_CommuMediation::StateUninitialized:
        Init();
        // do init first, then issue request
    case VR_CommuMediation::StateConnectReject:
        m_disconnectFlag = false;
        VR_LOGD("---------------->>---------------->>---------------->>---------------->>");
#if 1
        COMMED_LOGD(m_connectproxy->request(m_resource, m_use, m_role, m_resourceID));
#else
        COMMED_LOGD(m_connectproxy->request(m_resource, m_use, m_role, WIFI_NET_TYPE, m_resourceID));
#endif
        VR_LOGD("connect request: (resource,use,role)=(%s,%s,%s), resourceID=%s"
            , m_resource.getString()
            , m_use.getString()
            , m_role.getString()
            , m_resourceID.getString());
        break;
    case VR_CommuMediation::StateConnectPermit:
        m_disconnectFlag = false;
        ++m_netRefCount; // replyRequest() may be notified before request() invocation returns
        COMMED_LOGD(m_networkproxy->request(m_resourceID, m_netType, mode));
        VR_LOGD("network request: %s/%s"
            , m_resourceID.getString()
            , m_netType.getString());
        break;
    default:
        m_actionState = VR_CommuMediation::Idle;
        return false;
    }
    return true;
}

bool
VR_CommuMediation::Release()
{
    VR_LOGD_FUNC();
    if (VR_CommuMediation::ReleaseIssued == m_actionState) {
        VR_LOGD("WARNING: already sent reLease, no need to send again.");
        return false;
    }
    else if (VR_CommuMediation::RequestIssued == m_actionState) {
        if ((VR_CommuMediation::StateConnectReject == m_state)
            || (VR_CommuMediation::StateConnectPermit == m_state)) {
            StateEnum state = m_state;
            m_state = StateEnum(state + m_actionState);
            VR_LOGD("INFO: conservatively promote state, %s -> %s", GetStateString(state), GetStateString(m_state));
        }
    }
    m_actionState = VR_CommuMediation::ReleaseIssued;

    switch (m_state) {
    case VR_CommuMediation::StateNetworkReady:
        m_disconnectFlag = true;
        COMMED_LOGD(m_networkproxy->release(m_resourceID));
        break;
    case VR_CommuMediation::StateConnectPermit:
        m_disconnectFlag = true;
        COMMED_LOGD(m_connectproxy->release(m_resourceID));
        break;
    case VR_CommuMediation::StateConnectWait:
        m_disconnectFlag = true;
        COMMED_LOGD(m_connectproxy->stopWait(m_resourceID));
        break;
    default:
        m_actionState = VR_CommuMediation::Idle;
        break;
    }
    return true;
}

int 
VR_CommuMediation::OnStatusChanged(VR_CommuMediation::ConnStatusEnum status)
{
    if (!m_OnStatusChanged.empty()) { // binded
        VR_LOGD("status = %d", status);
        return m_OnStatusChanged((int)status);
    }
    VR_LOGD("*** no OnStatusChanged callback binded ***");
    return true;
}

VR_CommuMediation::VR_CommuMediationReplier::VR_CommuMediationReplier()
: m_pCommuMediation(NULL)
{
}

VR_CommuMediation::VR_CommuMediationReplier::~VR_CommuMediationReplier()
{
}

VOID
VR_CommuMediation::VR_CommuMediationReplier::replyRequest(
      const NCString& resourceID
    , const NCString& netType
    , const NCString& result)
{
    VR_LOGD_FUNC();
    CONFIRM_RESOURCEID_AND_ACTION(m_pCommuMediation, VR_CommuMediation::RequestIssued);
    EXPECT_STATE_VOID(m_pCommuMediation->m_state, VR_CommuMediation::StateConnectReject);
    RESET_ACTION(m_pCommuMediation);
    NCString netType_l = netType;
    if (netType_l == WIFI_NET_TYPE_FAKE) {
        VR_LOGD("netType is " WIFI_NET_TYPE_FAKE ", forced to " WIFI_NET_TYPE);
        netType_l = WIFI_NET_TYPE;
    }

    VR_LOGD("(resourceID,netType,result)=(%s, %s, %s)"
        , resourceID.getString()
        , netType_l.getString()
        , result.getString());

    if ("permit" == result) {
        m_pCommuMediation->SetState(VR_CommuMediation::StateConnectPermit);
        m_pCommuMediation->m_netType = netType_l;
        m_pCommuMediation->Request();
    }
    else if ("wait" == result) {
        m_pCommuMediation->SetState(VR_CommuMediation::StateConnectWait);
    }
    else {
        SLEEP_MS(RETRY_PERIOD_MS);
        if (!m_pCommuMediation->m_disconnectFlag) { // issue request again when failed and not disconnecting
            m_pCommuMediation->Request();
        }
        else {
            VR_LOGD("disconnecting, no retry for request again");
        }
    }
}

VOID 
VR_CommuMediation::VR_CommuMediationReplier::replyWaitPermit(const NCString& resourceID)
{
    VR_LOGD_FUNC();
    CONFIRM_RESOURCEID_AND_ACTION(m_pCommuMediation, VR_CommuMediation::Idle);
    EXPECT_STATE_VOID(m_pCommuMediation->m_state, VR_CommuMediation::StateConnectWait);
    if (!m_pCommuMediation->m_disconnectFlag) { // assure haven't issue stopwait command
        m_pCommuMediation->SetState(VR_CommuMediation::StateConnectReject);
        m_pCommuMediation->Request();
    }
}

VOID 
VR_CommuMediation::VR_CommuMediationReplier::replyStopWait(const NCString& resourceID)
{
    VR_LOGD_FUNC();
    CONFIRM_RESOURCEID_AND_ACTION(m_pCommuMediation, VR_CommuMediation::RequestIssued);
    EXPECT_STATE_VOID(m_pCommuMediation->m_state, VR_CommuMediation::StateConnectWait);
    RESET_ACTION(m_pCommuMediation);
    m_pCommuMediation->SetState(VR_CommuMediation::StateConnectReject);
}

VOID 
VR_CommuMediation::VR_CommuMediationReplier::requestInterrupt(const NCString& resourceID, const NCString& content)
{
    VR_LOGD_FUNC();
    CONFIRM_RESOURCEID_AND_ACTION(m_pCommuMediation, VR_CommuMediation::Any);
    if (VR_CommuMediation::StateNetworkReady == m_pCommuMediation->m_state
        || VR_CommuMediation::StateConnectPermit == m_pCommuMediation->m_state) {
        m_pCommuMediation->m_interruptFlag = true;
        m_pCommuMediation->Release();
    }
    else {
        VR_LOGD("state is %d, replyInterrupt directly", m_pCommuMediation->m_state);
        COMMED_LOGD(m_pCommuMediation->m_connectproxy->replyInterrupt(resourceID, "succeed"));
    }
}

VOID 
VR_CommuMediation::VR_CommuMediationReplier::replyRelease(const NCString& resourceID, const NCString& result)
{
    VR_LOGD_FUNC();
    CONFIRM_RESOURCEID_AND_ACTION(m_pCommuMediation, VR_CommuMediation::ReleaseIssued);
    EXPECT_STATE_VOID(m_pCommuMediation->m_state, VR_CommuMediation::StateConnectPermit);
    RESET_ACTION(m_pCommuMediation);
    VR_LOGD("(resourceID,result)=(%s, %s)", resourceID.getString(), result.getString());
    if ("succeed" == result) {
        m_pCommuMediation->SetState(VR_CommuMediation::StateConnectReject);
        if (m_pCommuMediation->m_interruptFlag) {
            m_pCommuMediation->m_interruptFlag = false;
            COMMED_LOGD(m_pCommuMediation->m_connectproxy->replyInterrupt(resourceID, "succeed"));
            m_pCommuMediation->OnStatusChanged(StatusInterrupted);
        }
        // m_pCommuMediation->m_resourceID = ""; // do not reset, since NCConnect.request() may return after replyRequest() is invoked
        m_pCommuMediation->m_netType = "";
        VR_LOGD("--------------- || -------------- || -------------- || ---------------");
    }
    else {
        SLEEP_MS(RETRY_PERIOD_MS);
        if (m_pCommuMediation->m_disconnectFlag) { // assure no fresh Request command issued.
            m_pCommuMediation->Release();
        }
    }
}

VR_CommuMediation::VR_CommuMediationNetReplier::VR_CommuMediationNetReplier()
: m_pCommuMediation(NULL)
{
}

VR_CommuMediation::VR_CommuMediationNetReplier::~VR_CommuMediationNetReplier()
{
}

VOID
VR_CommuMediation::VR_CommuMediationNetReplier::replyRequest(const NCString& resourceID, UINT32 result)
{
    VR_LOGD_FUNC();
    if (m_pCommuMediation && RESOURCE_ID == resourceID) {
        int count = m_pCommuMediation->m_netRefCount;
        if (0 < count) {
            --m_pCommuMediation->m_netRefCount;
        }
        if (1 < count) {
            VR_LOGD("WARNING: got response of previous network request. [by ref]");
            // return;
        }
        else if (0 == count) {
            VR_LOGD("WARNING: got response WITHOUT ANY request. [by ref]");
            // return;
        }
    }
    CONFIRM_RESOURCEID_AND_ACTION(m_pCommuMediation, VR_CommuMediation::RequestIssued);
    EXPECT_STATE_VOID(m_pCommuMediation->m_state, VR_CommuMediation::StateConnectPermit);
    RESET_ACTION(m_pCommuMediation);
    VR_LOGD("(resourceID,result)=(%s,%d)", resourceID.getString(), result);
    if (0 == result) {
        m_pCommuMediation->SetState(VR_CommuMediation::StateNetworkReady);
        m_pCommuMediation->OnStatusChanged(VR_CommuMediation::StatusConnected);
        VR_LOGD("--------------- R -------------- R -------------- R ---------------");
    }
    else {
        SLEEP_MS(RETRY_PERIOD_MS);
        if (!m_pCommuMediation->m_disconnectFlag) { // issue request again when failed and not disconnecting
            m_pCommuMediation->Request();
        }
        else {
            VR_LOGD("disconnecting, no retry for request again");
        }
    }
}

VOID
VR_CommuMediation::VR_CommuMediationNetReplier::replyRelease(const NCString& resourceID, UINT32 result)
{
    VR_LOGD_FUNC();
    CONFIRM_RESOURCEID_AND_ACTION(m_pCommuMediation, VR_CommuMediation::ReleaseIssued);
    EXPECT_STATE_VOID(m_pCommuMediation->m_state, VR_CommuMediation::StateNetworkReady);
    RESET_ACTION(m_pCommuMediation);
    VR_LOGD("(resourceID,result)=(%s,%d)", resourceID.getString(), result);
    if (0 == result) {
        m_pCommuMediation->SetState(VR_CommuMediation::StateConnectPermit);
    }
    else { // issue request again when failed
        SLEEP_MS(RETRY_PERIOD_MS);
    }
    if (m_pCommuMediation->m_disconnectFlag) { // assure no fresh Request command issued.
        m_pCommuMediation->Release();
    }
}

/* EOF */
