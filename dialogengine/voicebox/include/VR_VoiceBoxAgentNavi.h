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
 * @file VR_VoiceBoxAgentNavi.h
 * @brief interface for EngineWrapper callback.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VOICEBOXAGENTNAVI_H
#define VR_VOICEBOXAGENTNAVI_H

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

/* Forward Declaration */
class VR_VoiceBoxAgentNavi;
class VR_NaviAddressCommon;

// Define the agent message handler pointer
typedef bool (VR_VoiceBoxAgentNavi::*NaviMsgHandler)(IVECIParsedMessage*);
typedef bool (VR_VoiceBoxAgentNavi::*NaviQueryMsgHandler)(VR_VoiceBoxXmlParser&);

// poi id and name
struct VR_POIInfo
{
    std::string      POIId;     // poi id
    std::string      POIName;   // poi name
    std::string      POIType;   // poi type
};

// Destination point id and name
struct VR_DestinationPoint
{
    std::string      pointId;     // point id
    std::string      pointName;   // point name
    std::string      pointType;   // point type
};

// parameter of house number valide reply
struct VR_HouseNumValidePara
{
    std::string      strResult;     // Result
    // std::string      strAddressType;   // Address Type
    std::string      strHouseNumberFormat;   // House Number Format
};

// parameter of IDMapping reply
struct VR_IDMappingPara
{
    std::string      strStateID;
    std::string      strState;
    std::string      strCityID;
    std::string      strCity;
    std::string      strStreetID;
    std::string      strStreet;
    std::string      strFullName;
};

/**
 * @brief The VR_VoiceBoxAgentNavi class
 *
 * class declaration
 */
class VR_VoiceBoxAgentNavi : public VR_VoiceBoxEventSink
{
public:
    /**
     * Constructor
     *
     */
    VR_VoiceBoxAgentNavi(
        IVECIEngineClient& client,
        IVECIEngineCommand& engineCommand,
        VR_VoiceBoxEngineCallback& engineCallback
        );

    /**
     * Destructor
     *
     */
    virtual ~VR_VoiceBoxAgentNavi();

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

private:
    VoiceMap<std::string, NaviMsgHandler>::type m_naviMsgHandler;
    VoiceMap<std::string, NaviMsgHandler>::type m_naviMsgDriverHandler;
    VoiceMap<std::string, NaviQueryMsgHandler>::type m_naviQueryMsgHandler;
    VoiceMap<std::string, std::string>::type m_naviCurPointPOIInfo;
    VoiceMap<std::string, std::string>::type m_naviAlongRoutePOIInfo;
    VoiceMap<std::string, std::string>::type m_naviNearDestPOIInfo;
    VoiceMap<std::string, std::string>::type m_naviPOIInfo;
    VoiceMap<std::string, std::string>::type m_naviPoint;
    VoiceMap<std::string, std::string>::type m_naviDropPoint;
    VoiceVector<std::string>::type m_vecDropPoint;
    VoiceMap<std::string, std::string>::type m_vecAddressId;
    std::string m_strAddressIdList;
    std::string m_strFirstSearchAddress;

    std::string m_strCurrAgent;
    std::string m_strCurrActionResult;

    std::string m_strLocation;
    std::string m_strPOIID;
    std::string m_strPOIName;
    std::string m_strCurGuide;
    std::string m_strPOIList;

    std::string m_strAddress;

    std::string m_strPresetGuide;

    std::string m_strRouteGuide;
    std::string m_strRouteGuideNotNormal;
    std::string m_strDropPointID;

    std::string m_strPrevDisplay;
    std::string m_strCurDisplay;

    VR_HouseNumValidePara m_structHouseNumValidePara;

    bool m_bNBest;
    bool m_bIDMapping;
    bool m_bHaveAddressQueried;
    bool m_bCurAddressQueryisIDMapping;
    bool m_bisAddToRoute;
    bool m_bPOISpecialAddressType;
    bool m_bWayPointRequest;

    // status of IDMapping, which 1 meanings include querystate, 2 include querycity, 4 include querystreet
    int m_iIDMappingRequestType;
    VR_IDMappingPara m_srtIDMappingPara;

    VR_NaviAddressCommon* m_pNaviAddressCommon;

    enum VR_QueryAddressStep
    {
        VR_Step_None = 0,
        VR_Step_State = 1,
        VR_Step_City = 2,
        VR_Step_Street = 3,
        VR_Step_HouseNum = 4,
        VR_Step_Done = 5
    };

    VR_QueryAddressStep m_curQueryAddressStep;

    void GetInfo(const std::string& strActionOp);

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

    /* Agent: POI */
    // Agent Result Messages
    bool NaviBack(IVECIParsedMessage *pcMsg);
    bool NaviCancel(IVECIParsedMessage *pcMsg);
    bool NaviHelp(IVECIParsedMessage *pcMsg);
    bool NaviStartOver(IVECIParsedMessage *pcMsg);
    bool NaviStartPause(IVECIParsedMessage *pcMsg);
    bool NaviStartRepeat(IVECIParsedMessage *pcMsg);

    bool CallPOI(IVECIParsedMessage *pcMsg);
    bool ConfirmPOI(IVECIParsedMessage *pcMsg);
    bool FindPOINoRouteActive(IVECIParsedMessage *pcMsg);
    bool GetAssignment(IVECIParsedMessage *pcMsg);
    bool GetDirectionsToPOI(IVECIParsedMessage *pcMsg);
    bool GetPOICategory(IVECIParsedMessage *pcMsg);
    bool HideAllIcons(IVECIParsedMessage *pcMsg);
    bool InternetNotAvailable(IVECIParsedMessage *pcMsg);
    bool ListPOIs(IVECIParsedMessage *pcMsg);
    bool MaxIconsShown(IVECIParsedMessage *pcMsg);
    bool MaxWaypoints(IVECIParsedMessage *pcMsg);
    bool NoIconsShown(IVECIParsedMessage *pcMsg);
    bool NoPOIsAvailable(IVECIParsedMessage *pcMsg);
    bool NoPOIsReturned(IVECIParsedMessage *pcMsg);
    bool NoSearchApp(IVECIParsedMessage *pcMsg);
    bool OffboardPOISearch(IVECIParsedMessage *pcMsg);
    bool PhoneNotConnected(IVECIParsedMessage *pcMsg);
    bool PhoneNumberNotAvailable(IVECIParsedMessage *pcMsg);
    bool POIIconsNotAvailable(IVECIParsedMessage *pcMsg);
    // bool POIKeyboardDictation(IVECIParsedMessage *pcMsg);
    bool RejectDestination(IVECIParsedMessage *pcMsg);
    bool RejectPOINavigation(IVECIParsedMessage *pcMsg);
    bool SearchTheWeb(IVECIParsedMessage *pcMsg);
    bool ShowOffboardPOISearchResults(IVECIParsedMessage *pcMsg);
    bool ShowPOIIcons(IVECIParsedMessage *pcMsg);
    // bool VBTSendCommand(IVECIParsedMessage *pcMsg);
    // Info Driver Messages
    bool POIAddWaypointRequest(IVECIParsedMessage *pcMsg);
    bool IconsShownRequest(IVECIParsedMessage *pcMsg);
    bool PhoneNumAvailableRequest(IVECIParsedMessage *pcMsg);
    bool POIRequest(IVECIParsedMessage *pcMsg);

    /* Agent: Destination */
    // Agent Result Messages
    bool AddPresetAsWaypoint(IVECIParsedMessage *pcMsg);
    bool DestinationAssistPrompt(IVECIParsedMessage *pcMsg);
    bool AddPrevDestinationAsWaypoint(IVECIParsedMessage *pcMsg);
    bool CannotAddWaypoint(IVECIParsedMessage *pcMsg);
    bool ConfirmDeleteWaypoint(IVECIParsedMessage *pcMsg);
    bool ConfirmPreset(IVECIParsedMessage *pcMsg);
    bool ConfirmPresetReject(IVECIParsedMessage *pcMsg);
    bool ConfirmWaypointForDeletion(IVECIParsedMessage *pcMsg);
    bool DeleteDestination(IVECIParsedMessage *pcMsg);
    bool DeleteWaypoint(IVECIParsedMessage *pcMsg);
    bool DestAssCatch(IVECIParsedMessage *pcMsg);
    bool DestAssConfirmCall(IVECIParsedMessage *pcMsg);
    bool DestAssConfirmCallReject(IVECIParsedMessage *pcMsg);
    bool DestAssNotAvailable(IVECIParsedMessage *pcMsg);
    bool HomePresetNotDefined(IVECIParsedMessage *pcMsg);
    bool MapDataNotInArea(IVECIParsedMessage *pcMsg);
    bool NavigationHome(IVECIParsedMessage *pcMsg);
    bool NavigationMenu(IVECIParsedMessage *pcMsg);
    bool PresetNotValid(IVECIParsedMessage *pcMsg);
    bool PrevDestNotValid(IVECIParsedMessage *pcMsg);
    bool PrevDestNotAvailable(IVECIParsedMessage *pcMsg);
    bool SelectPresetAssignment(IVECIParsedMessage *pcMsg);
    bool SelectPrevDest(IVECIParsedMessage *pcMsg);
    bool SelectPrevDestAssignment(IVECIParsedMessage *pcMsg);
    bool SelectWaypointForDeletion(IVECIParsedMessage *pcMsg);
    bool SetPresetAsDestination(IVECIParsedMessage *pcMsg);
    bool SetPrevDestAsDestination(IVECIParsedMessage *pcMsg);
    bool WaypointDeletionCanceled(IVECIParsedMessage *pcMsg);
    bool WaypointsNotAvailable(IVECIParsedMessage *pcMsg);
    // Info Driver Messages
    bool IDMappingRequest(IVECIParsedMessage *pcMsg);
    bool CanAddWaypointRequest(IVECIParsedMessage *pcMsg);
    bool HomePresetDefinedRequest(IVECIParsedMessage *pcMsg);
    bool MapDataInAreaRequest(IVECIParsedMessage *pcMsg);
    bool PresetValidRequest(IVECIParsedMessage *pcMsg);
    bool PresetsAvailableRequest(IVECIParsedMessage *pcMsg);
    bool PrevDestValidRequest(IVECIParsedMessage *pcMsg);
    bool PrevDestsRequest(IVECIParsedMessage *pcMsg);
    bool PrevStartPointDefinedRequest(IVECIParsedMessage *pcMsg);
    bool WaypointRequest(IVECIParsedMessage *pcMsg);

    /* Agent: Address Entry */
    // Agent Result Messages
    bool ActiveRounteConfirmation(IVECIParsedMessage *pcMsg);
    bool AddRouteConfirmArrivalPoint(IVECIParsedMessage *pcMsg);
    bool AddRouteMaxWaypoint(IVECIParsedMessage *pcMsg);
    bool AddRouteRequest(IVECIParsedMessage *pcMsg);
    bool AddressConfirmationBridge(IVECIParsedMessage *pcMsg);
    bool AddressConfirmationFerry(IVECIParsedMessage *pcMsg);
    bool AddressConfirmationFreeway(IVECIParsedMessage *pcMsg);
    bool AddressConfirmationTollRoad(IVECIParsedMessage *pcMsg);
    bool AddressConfirmationTunnel(IVECIParsedMessage *pcMsg);
    bool ConfirmAddressConfirmSuccess(IVECIParsedMessage *pcMsg);
    bool ConfirmAddressWithoutHouseNumber(IVECIParsedMessage *pcMsg);
    bool ConfirmArrivalPointAddWaypoint(IVECIParsedMessage *pcMsg);
    bool ConfirmArrivalPointRejectSuccess(IVECIParsedMessage *pcMsg);
    bool ConfirmArrivalPointSuccess(IVECIParsedMessage *pcMsg);
    bool ReplaceRouteConfirmArrivalPoint(IVECIParsedMessage *pcMsg);

    bool AddAddressToActiveRoute(IVECIParsedMessage *pcMsg);
    bool AddWaypoint(IVECIParsedMessage *pcMsg);
    bool AddWaypointCarParked(IVECIParsedMessage *pcMsg);
    bool AddWaypointRequest(IVECIParsedMessage *pcMsg);
    bool AddressConfirmed(IVECIParsedMessage *pcMsg);
    bool AddressRejected(IVECIParsedMessage *pcMsg);
    bool ChangeAddress(IVECIParsedMessage *pcMsg);
    bool ChangeAddressCity(IVECIParsedMessage *pcMsg);
    bool ChangeAddressHouseNumber(IVECIParsedMessage *pcMsg);
    bool ChangeAddressState(IVECIParsedMessage *pcMsg);
    bool ChangeAddressStreet(IVECIParsedMessage *pcMsg);
    bool ChangeCityOrState(IVECIParsedMessage *pcMsg);
    bool ChangeCityOrStateCity(IVECIParsedMessage *pcMsg);
    bool ChangeCityOrStateHouseNumber(IVECIParsedMessage *pcMsg);
    bool ChangeCityOrStateState(IVECIParsedMessage *pcMsg);
    bool ChangeCityOrStateStreet(IVECIParsedMessage *pcMsg);
    bool ChangeDefaultStateCatch(IVECIParsedMessage *pcMsg);
    bool ChangeStateInNBestList(IVECIParsedMessage *pcMsg);
    bool ChangeStreetCityOrState(IVECIParsedMessage *pcMsg);
    bool ChangeStreetCityOrStateCity(IVECIParsedMessage *pcMsg);
    bool ChangeStreetCityOrStateHouseNumber(IVECIParsedMessage *pcMsg);
    bool ChangeStreetCityOrStateState(IVECIParsedMessage *pcMsg);
    bool ChangeStreetCityOrStateStreet(IVECIParsedMessage *pcMsg);
    // Cities Keyboard Dictation
    bool CityWithNoStreetsRejected(IVECIParsedMessage *pcMsg);
    bool ConfirmAddress(IVECIParsedMessage *pcMsg);
    bool ConfirmAddressCity(IVECIParsedMessage *pcMsg);
    bool ConfirmAddressHouseNumber(IVECIParsedMessage *pcMsg);
    bool ConfirmAddressState(IVECIParsedMessage *pcMsg);
    bool ConfirmAddressStreet(IVECIParsedMessage *pcMsg);
    bool ConfirmAnywhere(IVECIParsedMessage *pcMsg);
    bool ConfirmAnywhereCity(IVECIParsedMessage *pcMsg);
    bool ConfirmAnywhereHouseNumber(IVECIParsedMessage *pcMsg);
    bool ConfirmAnywhereState(IVECIParsedMessage *pcMsg);
    bool ConfirmAnywhereStreet(IVECIParsedMessage *pcMsg);
    bool ConfirmCityWithNoStreets(IVECIParsedMessage *pcMsg);
    bool ConfirmCityWithNoStreetsCity(IVECIParsedMessage *pcMsg);
    bool ConfirmCityWithNoStreetsState(IVECIParsedMessage *pcMsg);
    bool GetCity(IVECIParsedMessage *pcMsg);
    bool GetDefaultState(IVECIParsedMessage *pcMsg);
    bool GetDirectionsToAddress(IVECIParsedMessage *pcMsg);
    bool GetDirectionsToCountrywide(IVECIParsedMessage *pcMsg);
    bool GetDirectionsToCountrywideNoDefaultState(IVECIParsedMessage *pcMsg);
    bool GetDirectionsToMultiStepStartingWithState(IVECIParsedMessage *pcMsg);
    bool GetDirectionsToUnsupported(IVECIParsedMessage *pcMsg);
    bool GetDirectionsViaCountrywide(IVECIParsedMessage *pcMsg);
    bool GetDirectionsViaMultiStepStartingWithInfo(IVECIParsedMessage *pcMsg);
    bool GetDirectionsViaUnsupported(IVECIParsedMessage *pcMsg);
    bool GetHouseNumber(IVECIParsedMessage *pcMsg);
    bool GetStreetBeforeHouseNumber(IVECIParsedMessage *pcMsg);
    bool InvalidHouseNumber(IVECIParsedMessage *pcMsg);
    bool MultiStepStartingWithState(IVECIParsedMessage *pcMsg);
    bool NativeAddressConfirmCountrywide(IVECIParsedMessage *pcMsg);
    bool NativeAddressRejectedStateReplace(IVECIParsedMessage *pcMsg);
    bool ReplaceWaypoint(IVECIParsedMessage *pcMsg);
    bool ReplaceWaypointRequest(IVECIParsedMessage *pcMsg);
    bool SaveAddress(IVECIParsedMessage *pcMsg);
    bool SaveAddressIsFull(IVECIParsedMessage *pcMsg);
    bool SelectUserIntent(IVECIParsedMessage *pcMsg);
    bool ShowOnMap(IVECIParsedMessage *pcMsg);
    // States Keyboard Dictation
    // Streets Keyboard Dictation
    // VBT Error
    // VBT Send Command
    bool WaypointConfirmed(IVECIParsedMessage *pcMsg);

    // State Driver Messages
    bool CarParkedRequest(IVECIParsedMessage *pcMsg);
    bool SaveAddrRequest(IVECIParsedMessage *pcMsg);
    bool ValidateHouseNum(IVECIParsedMessage *pcMsg);
    bool VerifyCityContainsStreets(IVECIParsedMessage *pcMsg);
    bool NBest(IVECIParsedMessage *pcMsg);

    bool NullOperation(IVECIParsedMessage *pcMsg);

    bool POICurRequest(
        VR_VoiceBoxXmlParser& parser
        );
    bool HomePresetDefinedReply(
        VR_VoiceBoxXmlParser& parser
        );
    bool PointDataAreaRequest(
        VR_VoiceBoxXmlParser& parser
        );
    bool PreviousDestinationsReply(
        VR_VoiceBoxXmlParser& parser
        );
    bool WaypointReply(
        VR_VoiceBoxXmlParser& parser
        );
    bool AddWaypointReply(
        VR_VoiceBoxXmlParser& parser
        );
    bool IconsShownReply(
        VR_VoiceBoxXmlParser& parser
        );
    bool POICollectionNearby(
        VR_VoiceBoxXmlParser& parser
        );
    bool POICollectionAlongRoute(
        VR_VoiceBoxXmlParser& parser
        );
    bool POICollectionNearDestination(
        VR_VoiceBoxXmlParser& parser
        );
    bool CarParked(
        VR_VoiceBoxXmlParser& parser
        );
    bool HouseNumValidation(
        VR_VoiceBoxXmlParser& parser
        );
    bool CityVerification(
        VR_VoiceBoxXmlParser& parser
        );
    bool HandlePointType(
        VR_VoiceBoxXmlParser& parser
        );

    bool SimplyInfoQueryMessage(const std::string& strOp);

    bool POIReply(
        VR_VoiceBoxXmlParser& parser
        );

    bool IDMappingReply(
        VR_VoiceBoxXmlParser& parser
        );

    bool SetCountryID(
        VR_VoiceBoxXmlParser& parser
        );

    bool GetHouseNumType(
        const std::string& strHouseNumber,
        std::string& strHouseNumType
        );

    bool AddressMapping(IVECIParsedMessage *pcMsg);
    bool BuildAddress(IVECIParsedMessage *pcMsg);
    bool AddressMapping(const ListNodeInfo& listNodeInfo);
    bool BuildAddress(const ListNodeInfo& listNodeInfo, const std::string& strCommand);

    bool JumpScreenSpeakEntriesAddress();
    bool JumpScreenSpeakState();
    bool JumpScreenSpeakCity();
    bool JumpScreenSpeakStreet();
    bool JumpScreenSpeakHouseNum();
    bool JumpScreenAddressConfirm();
    bool JumpScreenSetRoute();
    bool JumpScreenSetRouteConfirm(const std::string& strGuideType);

    bool JumpScreenSpeakPOI();
    bool JumpScreenPOIList(const std::string& strScreen, IVECIParsedMessage *pcMsg);
    bool JumpScreenPOIConfirm(IVECIParsedMessage *pcMsg);

    bool JumpScreenConfirmDestination(const std::string& strDestinationType);
    bool JumpScreenConfirmDelDestination(const std::string& strNameNode);
    bool JumpScreenConfirmAssist();
    bool JumpScreenDestinationSelect(IVECIParsedMessage *pcMsg);
    bool JumpScreenDestDelSelect(IVECIParsedMessage *pcMsg);

    bool JumpScreenNBest(const VoiceVector<std::string>::type& vecAddress);

    void ChangeValue(std::string& strValue, bool bToHex);
    std::string GetAddress(const std::string& strKey);

    void QueryPointCollection(const std::string& strType);
    std::string GetChildNodeString(const std::string& strMessage, const std::string& strChildName);

    void GetChildNodeStringVector(
        const std::string& strMessage,
        const std::string& strChildName,
        VoiceVector<std::string>::type& vecInfo);

    std::string GetKey(const std::string& strMessage, const std::string& strKeyName);
    void CalculateRoute(const std::string& strCalculateType);
    bool ChangeNodeValueInXML(std::string& strxml, const std::string& strNodeName, const std::string& strDestValue);

    /**
     * Copy Constructor
     *
     */
    VR_VoiceBoxAgentNavi(const VR_VoiceBoxAgentNavi&);

    /**
     * Copy Assignment Operator
     *
     */
    VR_VoiceBoxAgentNavi &operator=(const VR_VoiceBoxAgentNavi&);
};


#endif // VR_VOICEBOXAGENTNAVI_H
/* EOF */
