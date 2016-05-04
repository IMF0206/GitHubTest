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

/* Standard Header */
#include <sstream>
#include "VR_Log.h"


/* VBT Header */
#ifndef VECIOUTSTR_H
#    include "VECIOutStr.h"
#endif

#ifndef VECICSTR_H
#    include "VECICStr.h"
#endif

#ifndef VBT_EXTERNAL_ERRORS_H
#    include "VBTExternalErrors.h"
#endif

#ifndef IVBTENGINECLIENT_H
#    include "IVBTEngineClient.h"
#endif

#ifndef VBT_SDK_AGENT_H
#    include "VBT_SDK_Agent.h"
#endif

#ifndef VBT_SDK_ACTION_TYPES_H
#    include "VBT_SDK_Action_Types.h"
#endif

/* Suntec Header */
#ifndef VR_VOICEBOXAGENTNAVI_H
#    include "VR_VoiceBoxAgentNavi.h"
#endif

#ifndef VR_VOICEBOXLOG_H
#    include "VR_VoiceBoxLog.h"
#endif

#ifndef VR_VOICEBOXXMLBUILDER_H
#    include "VR_VoiceBoxXmlBuilder.h"
#endif

#ifndef VR_VOICEBOXENGINECALLBACK_H
#    include "VR_VoiceBoxEngineCallback.h"
#endif

#ifndef VR_VOICEBOXXMLNODESTRING_H
#    include "VR_VoiceBoxXmlNodeString.h"
#endif

#ifndef VR_NAVI_ADDRESS_COMMON_H
#    include "VR_NaviAddressCommon.h"
#endif

#ifndef VBT_SDK_LIST_TYPES_H
#   include "VBT_SDK_List_Types.h"
#endif

#ifndef VR_CONFIGUREIF_H
#   include "VR_ConfigureIF.h"
#endif

#ifndef VR_DEF_H
#    include "VR_Def.h"
#endif

using namespace nutshell;

/**
 * @brief The NaviActionHandler class
 *
 * class declaration
 */
struct NaviActionHandler
{
    char*           szAction; // Action type
    NaviMsgHandler  handler;  // Handler pointer
};

/**
 * @brief The NaviQueryHandler class
 *
 * class declaration
 */
struct NaviQueryHandler
{
    char*                szAction; // Action type
    NaviQueryMsgHandler  handler;  // Handler pointer
};

const std::string cstPromptNoPreset = "No home address has been set up. "
                                      "Try again after setting a home location.";
const std::string cstPromptNoDestPreset = "There are no destinations set. ";
const std::string cstPromptOutSide = "The selected point is outside"
                                     "of the map data coverage.";
const std::string cstPromptMaxPOI = "No further selections are possible.";
const std::string cstPromptNoShowingPOI = "There are no icons shown.";
const std::string cstPromptNoPrevDestination = "There are no previous destinations.";
const std::string cstPromptNoSettingDestination = "There are no destinations set.";
const std::string cstPromptNoPOINearby = "There are no points of interest "
                                         "for the selected category near here.";
const std::string cstPromptNoPOIAlongRoute = "There are no points of interest "
                                             "for the selected category along the route.";
const std::string cstPromptNoPOINearDestination = "There are no points of interest "
                                             "for the selected category near your destination.";
const std::string cstPromptNoRoute = "That command is only available during route guidance.";

// Constructor
VR_VoiceBoxAgentNavi::VR_VoiceBoxAgentNavi(
    IVECIEngineClient& client,
    IVECIEngineCommand& engineCommand,
    VR_VoiceBoxEngineCallback& engineCallback
)
: VR_VoiceBoxEventSink(client, engineCommand, engineCallback)
, m_pNaviAddressCommon(NULL)
, m_curQueryAddressStep(VR_Step_None)
, m_bNBest(false)
, m_bWayPointRequest(false)
{
    if (NULL != VR_ConfigureIF::Instance()) {
        m_pNaviAddressCommon = VR_new VR_NaviAddressCommon(VR_ConfigureIF::Instance());
        if (m_pNaviAddressCommon) {
            m_pNaviAddressCommon->setCountryID("1");
        }
    }
}

// Destructor
VR_VoiceBoxAgentNavi::~VR_VoiceBoxAgentNavi()
{
    delete m_pNaviAddressCommon;
    m_pNaviAddressCommon = NULL;
}

bool
VR_VoiceBoxAgentNavi::Initialize()
{
    VR_LOGD_FUNC();

    const NaviActionHandler actionHandlerMap[] = {
        // Action Type : POI Agent
        { VBT_ACTION_TYPE_POI_CALL_POI,                      // "Call POI"
            &VR_VoiceBoxAgentNavi::CallPOI },
        { VBT_ACTION_TYPE_POI_CONFIRM_POI,                   // "Confirm POI"
            &VR_VoiceBoxAgentNavi::ConfirmPOI },
        { "Find POI No Route Active",                   // "Find POI No Route Active"
            &VR_VoiceBoxAgentNavi::FindPOINoRouteActive },
        { VBT_ACTION_TYPE_POI_GET_ASSIGNMENT,                // "Get Assignment"
            &VR_VoiceBoxAgentNavi::GetAssignment },
        { VBT_ACTION_TYPE_POI_GET_DIRECTIONS_TO_POI,         // "Get Directions to POI"
            &VR_VoiceBoxAgentNavi::GetDirectionsToPOI },
        { VBT_ACTION_TYPE_POI_GET_POI_CATEGORY,              // "Get POI Category"
            &VR_VoiceBoxAgentNavi::GetPOICategory },
        { VBT_ACTION_TYPE_POI_HIDE_ALL_ICONS,                // "Hide All Icons"
            &VR_VoiceBoxAgentNavi::HideAllIcons },
        { VBT_ACTION_TYPE_POI_INTERNET_NOT_AVAILABLE,        // "Internet Not Available"
            &VR_VoiceBoxAgentNavi::InternetNotAvailable },
        { VBT_ACTION_TYPE_POI_LIST_POIS,                     // "List POIs"
            &VR_VoiceBoxAgentNavi::ListPOIs },
        { "Max Icons Shown",                                 // "Max Icons Shown"
            &VR_VoiceBoxAgentNavi::MaxIconsShown },
        { "Max Waypoints",                                   // "Max Waypoints"
            &VR_VoiceBoxAgentNavi::MaxWaypoints },
        { "No Icons Shown",                                  // "No Icons Shown"
            &VR_VoiceBoxAgentNavi::NoIconsShown },
        { VBT_ACTION_TYPE_POI_NO_POIS_AVAILABLE,             // "No POIs Available"
            &VR_VoiceBoxAgentNavi::NoPOIsAvailable },
        { VBT_ACTION_TYPE_POI_NO_POIS_RETURNED,              // "No POIs Returned"
            &VR_VoiceBoxAgentNavi::NoPOIsReturned },
        { VBT_ACTION_TYPE_POI_NO_SEARCH_APP,                 // "No Search App"
            &VR_VoiceBoxAgentNavi::NoSearchApp },
        { VBT_ACTION_TYPE_POI_OFFBOARD_POI_SEARCH,           // "Off-board POI Search"
            &VR_VoiceBoxAgentNavi::OffboardPOISearch },
        { VBT_ACTION_TYPE_POI_PHONE_NOT_CONNECTED,           // "Phone Not Connected"
            &VR_VoiceBoxAgentNavi::PhoneNotConnected },
        { VBT_ACTION_TYPE_POI_PHONE_NUMBER_NOT_AVAILABLE,    // "Phone Number Not Available"
            &VR_VoiceBoxAgentNavi::PhoneNumberNotAvailable },
        { VBT_ACTION_TYPE_POI_POI_ICONS_NOT_AVAILABLE,       // "POI Icons Not Available"
            &VR_VoiceBoxAgentNavi::POIIconsNotAvailable },
        { "Reject Destination",                              // "Reject Destination"
            &VR_VoiceBoxAgentNavi::RejectDestination },
        { "Reject POI Navigation",                           // "Reject POI Navigation"
            &VR_VoiceBoxAgentNavi::RejectPOINavigation },
        { VBT_ACTION_TYPE_POI_SEARCH_THE_WEB,                // "Search the Web"
            &VR_VoiceBoxAgentNavi::SearchTheWeb },
        { VBT_ACTION_TYPE_POI_SHOW_OFFBOARD_POI_SEARCH_RESULTS, // "Show Off-board POI Search Results"
            &VR_VoiceBoxAgentNavi::ShowOffboardPOISearchResults },
        { VBT_ACTION_TYPE_POI_SHOW_POI_ICONS,                // "Show POI Icons"
            &VR_VoiceBoxAgentNavi::ShowPOIIcons },


        // Action Type : Destination Agent
        { VBT_ACTION_TYPE_DESTINATION_ADD_PRESET_AS_WAYPOINT, // "Add Preset as Waypoint"
            &VR_VoiceBoxAgentNavi::AddPresetAsWaypoint },
        { "Destination Assist Prompt", // "Destination Assist Prompt"
            &VR_VoiceBoxAgentNavi::DestinationAssistPrompt },
        { VBT_ACTION_TYPE_DESTINATION_ADD_PREVIOUS_DESTINATION_AS_WAYPOINT, // "Add Previous Destination as Waypoint"
            &VR_VoiceBoxAgentNavi::AddPrevDestinationAsWaypoint },
        { VBT_ACTION_TYPE_DESTINATION_CANNOT_ADD_WAYPOINT, // "Cannot Add Waypoint"
            &VR_VoiceBoxAgentNavi::CannotAddWaypoint },
        { VBT_ACTION_TYPE_DESTINATION_CONFIRM_DELETE_WAYPOINT, // "Confirm Delete Waypoint"
            &VR_VoiceBoxAgentNavi::ConfirmDeleteWaypoint },
        { VBT_ACTION_TYPE_DESTINATION_CONFIRM_PRESET, // "Confirm Preset"
            &VR_VoiceBoxAgentNavi::ConfirmPreset },
        { "Confirm Preset Reject", // "Confirm Preset Reject"
            &VR_VoiceBoxAgentNavi::ConfirmPresetReject },
        { "Confirm Waypoint for Deletion", // "Confirm Waypoint for Deletion"
            &VR_VoiceBoxAgentNavi::ConfirmWaypointForDeletion },
        { "Delete Destination - No Destination", // "Delete Destination - No Destination"
            &VR_VoiceBoxAgentNavi::DeleteDestination },
        { VBT_ACTION_TYPE_DESTINATION_DELETE_WAYPOINT, // "Delete Waypoint"
            &VR_VoiceBoxAgentNavi::DeleteWaypoint },
        { VBT_ACTION_TYPE_DESTINATION_DESTINATION_ASSIST_CATCH, // "Destination Assist Catch"
            &VR_VoiceBoxAgentNavi::DestAssCatch },
        { "Destination Assist Confirm Call", // "Destination Assist Confirm Call"
            &VR_VoiceBoxAgentNavi::DestAssConfirmCall },
        { "Destination Assist Confirm Call - Reject", // "Destination Assist Confirm Call - Reject"
            &VR_VoiceBoxAgentNavi::DestAssConfirmCallReject },
        { VBT_ACTION_TYPE_DESTINATION_HOME_PRESET_NOT_DEFINED, // "Home Preset Not Defined"
            &VR_VoiceBoxAgentNavi::HomePresetNotDefined },
        { "Map Data Not In Area", // "Map Data Not In Area"
            &VR_VoiceBoxAgentNavi::MapDataNotInArea },
        { "Navigation Home", // "Navigation Home"
            &VR_VoiceBoxAgentNavi::NavigationHome },
        { "Navigation Menu", // "Navigation Menu"
            &VR_VoiceBoxAgentNavi::NavigationMenu },
        { VBT_ACTION_TYPE_DESTINATION_PRESET_NOT_VALID, // "Preset Not Valid"
            &VR_VoiceBoxAgentNavi::PresetNotValid },
        { VBT_ACTION_TYPE_DESTINATION_PREVIOUS_DESTINATIONS_NOT_AVAILABLE, // "Previous Destinations Not Available"
            &VR_VoiceBoxAgentNavi::PrevDestNotAvailable },
        { VBT_ACTION_TYPE_DESTINATION_PREVIOUS_DESTINATION_NOT_VALID, // "Previous Destination Not Valid"
            &VR_VoiceBoxAgentNavi::PrevDestNotValid },
        { VBT_ACTION_TYPE_DESTINATION_SELECT_PRESET_ASSIGNMENT, // "Select Preset Assignment"
            &VR_VoiceBoxAgentNavi::SelectPresetAssignment },
        { VBT_ACTION_TYPE_DESTINATION_SELECT_PREVIOUS_DESTINATION, // "Select Previous Destination"
            &VR_VoiceBoxAgentNavi::SelectPrevDest },
        { VBT_ACTION_TYPE_DESTINATION_SELECT_PREVIOUS_DESTINATION_ASSIGNMENT, // "Select Previous Destination Assignment"
            &VR_VoiceBoxAgentNavi::SelectPrevDestAssignment },
        { VBT_ACTION_TYPE_DESTINATION_SELECT_WAYPOINT_FOR_DELETION, // "Select Waypoint for Deletion"
            &VR_VoiceBoxAgentNavi::SelectWaypointForDeletion },
        { VBT_ACTION_TYPE_DESTINATION_SET_PRESET_AS_DESTINATION, // "Set Preset as Destination"
            &VR_VoiceBoxAgentNavi::SetPresetAsDestination },
        { VBT_ACTION_TYPE_DESTINATION_SET_PREVIOUS_DESTINATION_AS_DESTINATION, // "Set Previous Destination as Destination"
            &VR_VoiceBoxAgentNavi::SetPrevDestAsDestination },
        { VBT_ACTION_TYPE_DESTINATION_WAYPOINTS_NOT_AVAILABLE, // "Waypoints Not Available"
            &VR_VoiceBoxAgentNavi::WaypointsNotAvailable },
        { VBT_ACTION_TYPE_DESTINATION_WAYPOINT_DELETION_CANCELED, // "Waypoint Deletion Canceled"
            &VR_VoiceBoxAgentNavi::WaypointDeletionCanceled },


        // Action Type : Address Entry Agent
        { "Active Route Confirmation",
            &VR_VoiceBoxAgentNavi::ActiveRounteConfirmation },
        { "Add To Route Confirm Arrival Point",
            &VR_VoiceBoxAgentNavi::AddRouteConfirmArrivalPoint },
        { "Add to Route Max Waypoint",
            &VR_VoiceBoxAgentNavi::AddRouteMaxWaypoint },
        { "Add To Route Request",
            &VR_VoiceBoxAgentNavi::AddRouteRequest },
        { "Address Confirmation Bridge",
            &VR_VoiceBoxAgentNavi::AddressConfirmationBridge },
        { "Address Confirmation Ferry",
            &VR_VoiceBoxAgentNavi::AddressConfirmationFerry },
        { "Address Confirmation Freeway",
            &VR_VoiceBoxAgentNavi::AddressConfirmationFreeway },
        { "Address Confirmation Toll Road",
            &VR_VoiceBoxAgentNavi::AddressConfirmationTollRoad },
        { "Address Confirmation Tunnel",
            &VR_VoiceBoxAgentNavi::AddressConfirmationTunnel },
        { VBT_ACTION_TYPE_ADDRESS_ENTRY_ADDRESS_CONFIRMED, // "Address Confirmed"
            &VR_VoiceBoxAgentNavi::AddressConfirmed },

        { VBT_ACTION_TYPE_ADDRESS_ENTRY_CONFIRM_ADDRESS,  // "Confirm Address"
            &VR_VoiceBoxAgentNavi::ConfirmAddress },
        { VBT_ACTION_TYPE_ADDRESS_ENTRY_CONFIRM_ADDRESS_CHANGE_STATE,  // "Confirm Address - Change State"
            &VR_VoiceBoxAgentNavi::ConfirmAddressState },
        { "Confirm Address Confirm Success",
            &VR_VoiceBoxAgentNavi::ConfirmAddressConfirmSuccess },
        { "Confirm Address Without House Number",
            &VR_VoiceBoxAgentNavi::ConfirmAddressWithoutHouseNumber },
        { VBT_ACTION_TYPE_ADDRESS_ENTRY_CONFIRM_ANYWHERE,  // "Confirm Anywhere"
            &VR_VoiceBoxAgentNavi::ConfirmAnywhere },
        { "Confirm Arrival Point Add Waypoint",
            &VR_VoiceBoxAgentNavi::ConfirmArrivalPointAddWaypoint },
        { "Confirm Arrival Point Reject Success",
            &VR_VoiceBoxAgentNavi::ConfirmArrivalPointRejectSuccess },
        { "Confirm Arrival Point Success",
            &VR_VoiceBoxAgentNavi::ConfirmArrivalPointSuccess },
        { VBT_ACTION_TYPE_ADDRESS_ENTRY_CONFIRM_CITY_WITH_NO_STREETS,  // "Confirm City with No Streets"
            &VR_VoiceBoxAgentNavi::ConfirmCityWithNoStreets },
        { VBT_ACTION_TYPE_ADDRESS_ENTRY_GET_CITY,  // "Get City"
            &VR_VoiceBoxAgentNavi::GetCity },
        { "Get Directions to an Address",  // "Get Directions to an Address"
            &VR_VoiceBoxAgentNavi::GetDirectionsToAddress },
        { VBT_ACTION_TYPE_ADDRESS_ENTRY_GET_HOUSE_NUMBER,  // "Get House Number"
            &VR_VoiceBoxAgentNavi::GetHouseNumber },
        { VBT_ACTION_TYPE_ADDRESS_ENTRY_GET_STREET_BEFORE_HOUSE_NUMBER,  // "Get Street Before House Number"
            &VR_VoiceBoxAgentNavi::GetStreetBeforeHouseNumber },
        { VBT_ACTION_TYPE_ADDRESS_ENTRY_MULTISTEP_STARTING_WITH_STATE,  // "Multi-step Starting with State"
            &VR_VoiceBoxAgentNavi::MultiStepStartingWithState },
        { "Replace Route Confirm Arrival Point",
            &VR_VoiceBoxAgentNavi::ReplaceRouteConfirmArrivalPoint },

        { VBT_ACTION_TYPE_ADDRESS_ENTRY_BACK, // "Back"
             &VR_VoiceBoxAgentNavi::NaviBack },
        { VBT_ACTION_TYPE_ADDRESS_ENTRY_CANCEL, // "Cancel"
             &VR_VoiceBoxAgentNavi::NaviCancel },
        { VBT_ACTION_TYPE_ADDRESS_ENTRY_HELP, // "Help"
             &VR_VoiceBoxAgentNavi::NaviHelp },
        { VBT_ACTION_TYPE_ADDRESS_ENTRY_START_OVER, // "Start Over"
             &VR_VoiceBoxAgentNavi::NaviStartOver },
        { VBT_ACTION_TYPE_ADDRESS_ENTRY_SYSTEM_PAUSE, // "System Pause"
             &VR_VoiceBoxAgentNavi::NaviStartPause },
        { VBT_ACTION_TYPE_ADDRESS_ENTRY_SYSTEM_REPEAT, // "System Repeat"
             &VR_VoiceBoxAgentNavi::NaviStartRepeat },

        { VBT_LIST_TYPE_ADDRESS_ENTRY_NBEST, // "n-Best"
             &VR_VoiceBoxAgentNavi::NBest },

        { NULL, NULL }
    };

    const NaviActionHandler driverHandlerMap[] = {
        // Driver Request Action Type : POI Agent
        { VBT_DRIVER_REQUEST_ACTION_TYPE_POI_ADD_WAYPOINT_REQUEST,
            &VR_VoiceBoxAgentNavi::POIAddWaypointRequest },
        { "Icons Shown Request",
            &VR_VoiceBoxAgentNavi::IconsShownRequest },
        { VBT_DRIVER_REQUEST_ACTION_TYPE_POI_PHONE_NUMBER_AVAILABLE_REQUEST,
            &VR_VoiceBoxAgentNavi::PhoneNumAvailableRequest },
        { VBT_DRIVER_REQUEST_ACTION_TYPE_POI_POI_REQUEST,
            &VR_VoiceBoxAgentNavi::POIRequest },

        // Driver Request Action Type : Destination Agent
        { VBT_DRIVER_REQUEST_ACTION_TYPE_DESTINATION_CAN_ADD_WAYPOINT_REQUEST,
            &VR_VoiceBoxAgentNavi::CanAddWaypointRequest },
        { VBT_DRIVER_REQUEST_ACTION_TYPE_DESTINATION_HOME_PRESET_DEFINED_REQUEST,
            &VR_VoiceBoxAgentNavi::HomePresetDefinedRequest },
        { "Map Data In Area Request",
            &VR_VoiceBoxAgentNavi::MapDataInAreaRequest },
        { VBT_DRIVER_REQUEST_ACTION_TYPE_DESTINATION_PRESET_VALID_REQUEST,
            &VR_VoiceBoxAgentNavi::PresetValidRequest },
        { VBT_DRIVER_REQUEST_ACTION_TYPE_DESTINATION_PRESETS_AVAILABLE_REQUEST,
            &VR_VoiceBoxAgentNavi::PresetsAvailableRequest },
        { VBT_DRIVER_REQUEST_ACTION_TYPE_DESTINATION_PREVIOUS_DESTINATION_VALID_REQUEST,
            &VR_VoiceBoxAgentNavi::PrevDestValidRequest },
        { VBT_DRIVER_REQUEST_ACTION_TYPE_DESTINATION_PREVIOUS_DESTINATIONS_REQUEST,
            &VR_VoiceBoxAgentNavi::PrevDestsRequest },
        { VBT_DRIVER_REQUEST_ACTION_TYPE_DESTINATION_PREVIOUS_START_POINT_DEFINED_REQUEST,
            &VR_VoiceBoxAgentNavi::PrevStartPointDefinedRequest },
        { VBT_DRIVER_REQUEST_ACTION_TYPE_DESTINATION_WAYPOINT_REQUEST,
            &VR_VoiceBoxAgentNavi::WaypointRequest },

        // Driver Request Action Type : Address Entry Agent
        { "ID Mapping Request", // ID Mapping Request
            &VR_VoiceBoxAgentNavi::IDMappingRequest },
        { VBT_DRIVER_REQUEST_ACTION_TYPE_ADDRESS_ENTRY_CAN_ADD_WAYPOINT_REQUEST,
            &VR_VoiceBoxAgentNavi::CanAddWaypointRequest },
        { VBT_DRIVER_REQUEST_ACTION_TYPE_ADDRESS_ENTRY_CAR_PARKED_REQUEST,
            &VR_VoiceBoxAgentNavi::CarParkedRequest },
        { VBT_DRIVER_REQUEST_ACTION_TYPE_ADDRESS_ENTRY_SAVE_ADDRESS_REQUEST,
            &VR_VoiceBoxAgentNavi::SaveAddrRequest },
        { VBT_DRIVER_REQUEST_ACTION_TYPE_ADDRESS_ENTRY_VALIDATE_HOUSE_NUMBER,
            &VR_VoiceBoxAgentNavi::ValidateHouseNum },
        { VBT_DRIVER_REQUEST_ACTION_TYPE_ADDRESS_ENTRY_VERIFY_CITY_CONTAINS_STREETS,
            &VR_VoiceBoxAgentNavi::VerifyCityContainsStreets },
        { NULL, NULL }
    };

    const NaviQueryHandler queryHandlerMap[] = {
        { "queryCurrentPosition", &VR_VoiceBoxAgentNavi::POICurRequest },
        { "queryNextDestination", &VR_VoiceBoxAgentNavi::POICurRequest },
        { "queryPointCollection", &VR_VoiceBoxAgentNavi::HomePresetDefinedReply },
        { "queryPointDataArea", &VR_VoiceBoxAgentNavi::PointDataAreaRequest },
        { "queryDestinationHistory", &VR_VoiceBoxAgentNavi::PreviousDestinationsReply },
        { "queryUnpassedDestinationList", &VR_VoiceBoxAgentNavi::WaypointReply },
        { "queryAllDestinationList", &VR_VoiceBoxAgentNavi::AddWaypointReply },
        { "queryShowingPOIIconNumber", &VR_VoiceBoxAgentNavi::IconsShownReply },
        { "queryPOICollectionNearby", &VR_VoiceBoxAgentNavi::POICollectionNearby },
        { "queryPOICollectionAlongRoute", &VR_VoiceBoxAgentNavi::POICollectionAlongRoute },
        { "queryPOICollectionNearDestination", &VR_VoiceBoxAgentNavi::POICollectionNearDestination },
        { "queryVehicleinMotion", &VR_VoiceBoxAgentNavi::CarParked },
        { "queryAddressInfo", &VR_VoiceBoxAgentNavi::HouseNumValidation },
        { "queryStreetInfo", &VR_VoiceBoxAgentNavi::HouseNumValidation },
        { "queryCityInfo", &VR_VoiceBoxAgentNavi::CityVerification },
        { "queryStateInfo", &VR_VoiceBoxAgentNavi::HouseNumValidation },
        { "queryPointType", &VR_VoiceBoxAgentNavi::HandlePointType },
        { "requestDefaultInfo", &VR_VoiceBoxAgentNavi::SetCountryID },
        { NULL, NULL }
    };

    int i = 0;
    while (true) {
        if (NULL == actionHandlerMap[i].szAction) {
            break;
        }

        m_naviMsgHandler.insert(std::make_pair(
            actionHandlerMap[i].szAction,
            actionHandlerMap[i].handler
            ));

        ++i;
    }

    i = 0;
    while (true) {
        if (NULL == driverHandlerMap[i].szAction) {
            break;
        }

        m_naviMsgDriverHandler.insert(std::make_pair(
            driverHandlerMap[i].szAction,
            driverHandlerMap[i].handler
            ));

        ++i;
    }

    i = 0;
    while (true) {
        if (NULL == queryHandlerMap[i].szAction) {
            break;
        }

        m_naviQueryMsgHandler.insert(std::make_pair(
            queryHandlerMap[i].szAction,
            queryHandlerMap[i].handler
            ));

        ++i;
    }

    return true;
}

void
VR_VoiceBoxAgentNavi::UnInitialize()
{
    VR_LOGD_FUNC();

    m_naviMsgHandler.clear();
    m_naviMsgDriverHandler.clear();
    m_naviQueryMsgHandler.clear();
}

bool
VR_VoiceBoxAgentNavi::ReplyQueryInfo(
    VR_VoiceBoxXmlParser& parser
)
{
    VR_LOGD_FUNC();

    std::string strActionKey = parser.getXmlKey();
    VR_LOG("ReplyQueryInfo: %s", strActionKey.c_str());

    // Dispatch the messages
    VoiceMap<std::string, NaviQueryMsgHandler>::const_iterator iterMap = m_naviQueryMsgHandler.find(strActionKey);
    if (m_naviQueryMsgHandler.cend() != iterMap) {
        if (NULL != iterMap->second) {
            (this->*(iterMap->second))(parser);
            return true;
        }
    }

    return true;
}

// QueryAgentSupport
HRESULT
VR_VoiceBoxAgentNavi::QueryAgentSupport(
    VBT_CSTR szAgent,
    VBT_BOOL *pbSupported
)
{
    VR_LOGD_FUNC();

    if (NULL == pbSupported) {
        VR_ERROR("QueryAgentSupport: NULL Parameter");
        return E_FAIL;
    }

    CVECICStr strAgent(&m_client, szAgent);
    if (strAgent.IsEqual(VBT_AGENT_ADDRESS_ENTRY)
        || strAgent.IsEqual(VBT_AGENT_DESTINATION)
        || strAgent.IsEqual(VBT_AGENT_POI)
        || strAgent.IsEqual("Navigation")) {
        m_strCurrAgent = strAgent.Get();
        VR_LOG("QueryAgentSupport: %s", szAgent);
        *pbSupported = VBT_TRUE;
    }
    else {
        *pbSupported = VBT_FALSE;
    }

    return S_OK;
}

// OnAgentResultMessage
HRESULT
VR_VoiceBoxAgentNavi::OnAgentResultMessage(
    IVECITransaction* pTrans,
    IVECIParsedMessage* pResultMsg
)
{
    VR_LOGD_FUNC();

    if ((NULL == pTrans) || (NULL == pResultMsg)) {
        VR_ERROR("OnAgentResultMessage: NULL Parameter");
        return E_FAIL;
    }

    if (!ProcessAgentMessage(pResultMsg)) {
        return E_FAIL;
    }

    return S_OK;
}

// OnAgentDriverMessage
HRESULT
VR_VoiceBoxAgentNavi::OnAgentDriverMessage(
    IVECIParsedMessage* pDriverMsg
)
{
    VR_LOGD_FUNC();

    if (NULL == pDriverMsg) {
        return E_FAIL;
    }

    if (!ProcessAgentMessage(pDriverMsg)) {
        return E_FAIL;
    }

    return S_OK;
}

bool
VR_VoiceBoxAgentNavi::ProcessAgentMessage(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    if (!SendActionMessage(pcMsg)) {
        return false;
    }

    std::string strMsgClassName;
    CVECIOutStr strMsgClass;
    pcMsg->GetMsgClass(&strMsgClass);
    VR_LOG("message class : %s", strMsgClass.Get());
    if (NULL != strMsgClass.Get()) {
        strMsgClassName = strMsgClass.Get();
    }

    bool bDriver = false;
    if ("Driver" == strMsgClassName) {
        bDriver = true;
    }

    std::string strAgentResult = "";
    do {
        CVECIOutStr strActionType;
        pcMsg->GetActionType(&strActionType);
        if (NULL != strActionType.Get()) {
            strAgentResult = strActionType.Get();
            break;
        }

        CVECIOutStr strListType;
        pcMsg->GetListType(&strListType);
        if (NULL != strListType.Get()) {
            strAgentResult = strListType.Get();
            break;
        }

        return false;

    } while (false);

    m_strCurrActionResult = strAgentResult;
    VR_LOG("agent result: %s", strAgentResult.c_str());

    // Dispatch the messages
    if (!bDriver) {
        VoiceMap<std::string, NaviMsgHandler>::const_iterator iterMap = m_naviMsgHandler.find(strAgentResult);
        if (m_naviMsgHandler.cend() != iterMap) {
            if (NULL != iterMap->second) {
                (this->*(iterMap->second))(pcMsg);
                return true;
            }
        }
    }
    else {
        VoiceMap<std::string, NaviMsgHandler>::const_iterator iterMap = m_naviMsgDriverHandler.find(strAgentResult);
        if (m_naviMsgDriverHandler.cend() != iterMap) {
            if (NULL != iterMap->second) {
                (this->*(iterMap->second))(pcMsg);
                return true;
            }
        }

    }

    return false;
}


bool
VR_VoiceBoxAgentNavi::NaviBack(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    m_engineCallback.OnRequest("<send event=\"back-internal\" target=\"de\"/>");
    return true;
}

bool
VR_VoiceBoxAgentNavi::NaviCancel(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.OnRequest("<send event=\"cancel\" target=\"dm\"/>");

    return true;
}

bool
VR_VoiceBoxAgentNavi::NaviHelp(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::NaviStartOver(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_curQueryAddressStep = VR_Step_None;
    m_engineCallback.OnRequest("<send event=\"startover-internal\" target=\"de\"/>");

    return true;
}

bool
VR_VoiceBoxAgentNavi::NaviStartPause(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.OnRequest("<send event=\"pause-internal\" target=\"de\"/>");

    return true;
}

bool
VR_VoiceBoxAgentNavi::NaviStartRepeat(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.OnRequest("<send event=\"repeat-internal\" target=\"de\"/>");

    return true;
}

/* Info Driver Messages */
// Agent: POI
bool
VR_VoiceBoxAgentNavi::CallPOI(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    return true;
}

bool
VR_VoiceBoxAgentNavi::ConfirmPOI(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    std::string strPOIID = GetActionParaByName(pcMsg, "POI ID");
    VR_LOG("select poi id: %s", strPOIID.c_str());

    VoiceMap<std::string, std::string>::const_iterator iter = m_naviPOIInfo.find(strPOIID);
    std::string strPoi;

    if (iter != m_naviPOIInfo.cend()) {
        VR_LOG("find poi info");
        strPoi = iter->second;
    }
    else {
        return false;
    }

    m_strRouteGuide = GetChildNodeString(strPoi, "guidePoint");
    std::string strAction = "<action agent=\"navi\" op=\"queryPointType\">";
    strAction.append(m_strRouteGuide);
    strAction.append("</action>");
    m_engineCallback.OnInfoQueryMessage(strAction);

    CVECIOutStr strWaypointAction;
    pcMsg->GetActionParameterValue("waypointAction", &strWaypointAction);

    if (NULL == strWaypointAction.Get()) {
        m_bPOISpecialAddressType = false;
        JumpScreenPOIConfirm(pcMsg);
    }
    else {
        VR_LOG("waypointAction is [%s]", strWaypointAction.Get());
        m_bPOISpecialAddressType = true;
    }


    return true;
}

bool
VR_VoiceBoxAgentNavi::FindPOINoRouteActive(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        VR_LOG("pcMsg is NULL");
        return false;
    }
    VR_LOG("show MSG");
    ShowPopupMsg(cstPromptNoRoute);

    return true;
}

bool
VR_VoiceBoxAgentNavi::GetAssignment(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return JumpScreenSetRoute();
}

bool
VR_VoiceBoxAgentNavi::GetDirectionsToPOI(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }
    std::string strWaypointAction = GetActionParaByName(pcMsg, "waypointAction");
    VR_LOG("Waypoint Action is : %s", strWaypointAction.c_str());
    if ("add" == strWaypointAction) {
        CalculateRoute("ADD_TO_ROUTE");
    }
    else if ("none" == strWaypointAction || "replace" == strWaypointAction) {
        CalculateRoute("REROUTE");
    }
    else {
        VR_LOG("error waypointAction param !!!");
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::GetPOICategory(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }
    VR_LOGP("DE: get POI category, will jump screen .... case : 212-124-99 212-130-99");

    return JumpScreenSpeakPOI();
}

bool
VR_VoiceBoxAgentNavi::HideAllIcons(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    std::string strPOIID = GetActionParaByName(pcMsg, "POI ID");
    VR_LOG("hide icon categoryId %s", strPOIID.c_str());

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("navi", "hidePOIIcon");
    xmlBulder.buildGivenElement(node, "categoryId", strPOIID, "", "");
    std::string strReply = xmlBulder.getXmlString();

    VR_LOG("xml: %s", strReply.c_str());

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentNavi::InternetNotAvailable(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ListPOIs(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    m_strLocation = GetActionParaByName(pcMsg, "Location");
    std::string strContent;

    if ("Nearby" == m_strLocation) {
        strContent = "navi_poi_list_select";
        m_engineCallback.GetPromptByScreenId("VR-POI-03", m_strPrompt);
    }
    else if ("Along Route" == m_strLocation) {
        strContent = "navi_poi_along_route_select";
        m_engineCallback.GetPromptByScreenId("VR-POI-51", m_strPrompt);
    }
    else if ("Destination" == m_strLocation) {
        strContent = "navi_poi_near_dest_select";
        m_engineCallback.GetPromptByScreenId("VR-POI-52", m_strPrompt);
    }
    else if ("City" == m_strLocation) {
        return true;
    }
    else {
        return false;
    }

    return JumpScreenPOIList(strContent, pcMsg);
}

bool
VR_VoiceBoxAgentNavi::MaxIconsShown(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }
    ShowPopupMsg(cstPromptMaxPOI);

    return true;
}

bool
VR_VoiceBoxAgentNavi::MaxWaypoints(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return JumpScreenSetRoute();
}

bool
VR_VoiceBoxAgentNavi::NoIconsShown(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    ShowPopupMsg(cstPromptNoShowingPOI);

    return true;
}

bool
VR_VoiceBoxAgentNavi::NoPOIsAvailable(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::NoPOIsReturned(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        VR_LOG("pcMsg is NULL");
        return false;
    }

    if ("Nearby" == m_strLocation) {
        VR_LOG("Nearby MSG");
        ShowPopupMsg(cstPromptNoPOINearby);
    }
    else if ("Along Route" == m_strLocation) {
        VR_LOG("Along Route MSG");
        ShowPopupMsg(cstPromptNoPOIAlongRoute);
    }
    else if ("Destination" == m_strLocation) {
        VR_LOG("Destination MSG");
        ShowPopupMsg(cstPromptNoPOINearDestination);
    }
    else {
        VR_LOG("error");
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::NoSearchApp(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::OffboardPOISearch(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::PhoneNotConnected(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::PhoneNumberNotAvailable(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::POIIconsNotAvailable(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

// bool
// VR_VoiceBoxAgentNavi::POIKeyboardDictation(IVECIParsedMessage *pcMsg)
// {
//     return true;
// }

bool
VR_VoiceBoxAgentNavi::RejectDestination(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::RejectPOINavigation(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    m_strCurDisplay = m_strPrevDisplay;
    m_engineCallback.OnRecognized(m_strCurDisplay);

    if ("Nearby" == m_strLocation) {
        m_engineCallback.GetPromptByScreenId("VR-POI-03", m_strPrompt);
    }
    else if ("Along Route" == m_strLocation) {
        m_engineCallback.GetPromptByScreenId("VR-POI-51", m_strPrompt);
    }
    else if ("Destination" == m_strLocation) {
        m_engineCallback.GetPromptByScreenId("VR-POI-52", m_strPrompt);
    }
    else if ("City" == m_strLocation) {
        return true;
    }
    else {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::SearchTheWeb(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ShowOffboardPOISearchResults(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ShowPOIIcons(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }
    std::string strPOIID = GetActionParaByName(pcMsg, "POI ID");
    VR_LOG("show icon categoryId %s", strPOIID.c_str());

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("navi", "showPOIIcon");
    xmlBulder.buildGivenElement(node, "categoryId", strPOIID, "", "");
    std::string strReply = xmlBulder.getXmlString();
    VR_LOG("xml: %s", strReply.c_str());

    m_engineCallback.OnRecognized(strReply);

    return true;
}

// bool
// VR_VoiceBoxAgentNavi::VBTSendCommand(IVECIParsedMessage *pcMsg)
// {
//     return true;
// }

bool
VR_VoiceBoxAgentNavi::POIAddWaypointRequest(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return SimplyInfoQueryMessage("queryAllDestinationList");
}

bool
VR_VoiceBoxAgentNavi::IconsShownRequest(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return SimplyInfoQueryMessage("queryShowingPOIIconNumber");
}

bool
VR_VoiceBoxAgentNavi::PhoneNumAvailableRequest(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::POIRequest(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("POIRequest: %s", xmlMessage.Get());

    m_strLocation = GetActionParaByName(pcMsg, "Location");
    m_strPOIID = GetActionParaByName(pcMsg, "POI ID");
    m_strPOIName = GetActionParaByName(pcMsg, "POI Name");

    if ("Destination" == m_strLocation) {
        SimplyInfoQueryMessage("queryNextDestination");
    }
    else {
        SimplyInfoQueryMessage("queryCurrentPosition");
    }

    return true;
}

// Agent: Destination
bool
VR_VoiceBoxAgentNavi::AddPresetAsWaypoint(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    CalculateRoute("ADD_TO_ROUTE");

    return true;
}

bool
VR_VoiceBoxAgentNavi::DestinationAssistPrompt(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return JumpScreenConfirmAssist();
}

bool
VR_VoiceBoxAgentNavi::AddPrevDestinationAsWaypoint(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    CalculateRoute("ADD_TO_ROUTE");

    return true;
}

bool
VR_VoiceBoxAgentNavi::CannotAddWaypoint(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ConfirmDeleteWaypoint(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    std::string strNum = GetActionParaByName(pcMsg, "Number");
    VR_LOG("select delete number: %s", strNum.c_str());

    int iIndex = atoi(strNum.c_str());

    if (iIndex > 0 && iIndex < 6) {
        iIndex = iIndex - 1;
    }
    else {
        VR_LOG("index error!");
        return false;
    }

    if (iIndex >= m_vecDropPoint.size()) {
        VR_LOG("out of vector! size:%d, index:%d", m_vecDropPoint.size(), iIndex);
        return false;
    }

    std::string strDropPoint = m_vecDropPoint.at(iIndex);

    m_strDropPointID = GetChildNodeString(strDropPoint, "dropPointId");

    std::string strDelDestination = "<action agent=\"navi\" op=\"deleteDestination\">";
    strDelDestination.append(m_strDropPointID);
    strDelDestination.append("</action>");

    m_engineCallback.OnRecognized(strDelDestination);
    m_vecDropPoint.clear();

    return true;
}

bool
VR_VoiceBoxAgentNavi::ConfirmPreset(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    std::string strTypeValue = GetActionParaByName(pcMsg, "Preset");
    VR_LOG("Destination Type is : %s", strTypeValue.c_str());

    std::string strDestinationType;
    if ("Home" == strTypeValue) {
        strDestinationType = "HOME";
    }
    else if ("1" == strTypeValue) {
        strDestinationType = "PRESET1";
    }
    else if ("2" == strTypeValue) {
        strDestinationType = "PRESET2";
    }
    else {
        VR_LOG("Error Destination Type !!! ");
        strDestinationType = "";
    }
    return JumpScreenConfirmDestination(strDestinationType);
}

bool
VR_VoiceBoxAgentNavi::ConfirmPresetReject(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    m_strCurDisplay = m_strPrevDisplay;
    m_engineCallback.OnRecognized(m_strCurDisplay);

    return true;
}

bool
VR_VoiceBoxAgentNavi::ConfirmWaypointForDeletion(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    std::string strID = GetActionParaByName(pcMsg, "ID");
    VR_LOG("select delete id: %s", strID.c_str());

    VoiceMap<std::string, std::string>::const_iterator iter = m_naviDropPoint.find(strID);

    m_strDropPointID = GetChildNodeString(iter->second, "dropPointId");
    m_strRouteGuide = GetChildNodeString(iter->second, "guidePoint");

    std::string strNameNode = GetChildNodeString(m_strRouteGuide, "name");
    return JumpScreenConfirmDelDestination(strNameNode);
}

bool
VR_VoiceBoxAgentNavi::DeleteDestination(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    ShowPopupMsg(cstPromptNoSettingDestination);

    return true;
}

bool
VR_VoiceBoxAgentNavi::DeleteWaypoint(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    std::string strDelDestination = "<action agent=\"navi\" op=\"deleteDestination\">";
    strDelDestination.append(m_strDropPointID);
    strDelDestination.append("</action>");

    m_engineCallback.OnRecognized(strDelDestination);

    return true;
}

bool
VR_VoiceBoxAgentNavi::DestAssCatch(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::DestAssConfirmCall(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }


    // action
    return true;
}

bool
VR_VoiceBoxAgentNavi::DestAssConfirmCallReject(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::DestAssNotAvailable(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::HomePresetNotDefined(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }
    ShowPopupMsg(cstPromptNoPreset);

    return true;
}

bool
VR_VoiceBoxAgentNavi::MapDataNotInArea(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }
    ShowPopupMsg(cstPromptOutSide);

    return true;
}

bool
VR_VoiceBoxAgentNavi::NavigationHome(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.GetPromptByScreenId("VR-HNT-01", m_strPrompt);

    m_engineCallback.OnRequest("<event name=\"startNextAgent\"><agent>navi</agent></event>");

    return true;
}

bool
VR_VoiceBoxAgentNavi::NavigationMenu(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.OnRequest("<event name=\"showHintScreen\"><agent>navi</agent></event>");

    return true;
}

bool
VR_VoiceBoxAgentNavi::PresetNotValid(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }
    ShowPopupMsg(cstPromptNoDestPreset);

    return true;
}

bool
VR_VoiceBoxAgentNavi::PrevDestNotValid(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return JumpScreenDestinationSelect(pcMsg);
}

bool
VR_VoiceBoxAgentNavi::PrevDestNotAvailable(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    ShowPopupMsg(cstPromptNoPrevDestination);

    return true;
}

bool
VR_VoiceBoxAgentNavi::SelectPresetAssignment(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return JumpScreenSetRoute();
}

bool
VR_VoiceBoxAgentNavi::SelectPrevDest(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return JumpScreenDestinationSelect(pcMsg);
}

bool
VR_VoiceBoxAgentNavi::SelectPrevDestAssignment(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    std::string strPrevDestination = GetActionParaByName(pcMsg, "Previous Destination");
    VR_LOG("point id: %s", strPrevDestination.c_str());

    VoiceMap<std::string, std::string>::const_iterator iter = m_naviPoint.find(strPrevDestination);
    std::string strPoint;

    if (iter != m_naviPoint.cend()) {
        VR_LOG("find point info");
        strPoint = iter->second;
    }
    else {
        return false;
    }

    m_strRouteGuide = GetChildNodeString(strPoint, "guidePoint");

    return JumpScreenSetRoute();
}

bool
VR_VoiceBoxAgentNavi::SelectWaypointForDeletion(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return JumpScreenDestDelSelect(pcMsg);
}

bool
VR_VoiceBoxAgentNavi::SetPresetAsDestination(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    CalculateRoute("REROUTE");

    return true;
}

bool
VR_VoiceBoxAgentNavi::SetPrevDestAsDestination(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    std::string strPrevDestination = GetActionParaByName(pcMsg, "Previous Destination");
    VR_LOG("point id: %s", strPrevDestination.c_str());

    VoiceMap<std::string, std::string>::const_iterator iter = m_naviPoint.find(strPrevDestination);
    std::string strPoint;

    if (iter != m_naviPoint.cend()) {
        VR_LOG("find point info");
        strPoint = iter->second;
    }
    else {
        return false;
    }

    m_strRouteGuide = GetChildNodeString(strPoint, "guidePoint");

    CalculateRoute("REROUTE");

    return true;
}

bool
VR_VoiceBoxAgentNavi::WaypointDeletionCanceled(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::WaypointsNotAvailable(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    ShowPopupMsg(cstPromptNoSettingDestination);

    return true;
}

bool
VR_VoiceBoxAgentNavi::IDMappingRequest(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    ListNodeInfo listNodeInfo;
    if (!GetListParameter(pcMsg, listNodeInfo)) {
        return false;
    }

    m_bIDMapping = true;
    m_iIDMappingRequestType = 0;
    std::string strTypeValue;
    std::string strID;

    VoiceVector<VoiceVector<NodeInfo>::type>::const_iterator itorItem = listNodeInfo.vecItems.cbegin();
    VoiceVector<NodeInfo>::const_iterator itor;
    for (; itorItem != listNodeInfo.vecItems.cend(); ++itorItem) {
        for (itor = itorItem->cbegin(); itor != itorItem->cend(); ++itor) {
            if ("Type" == itor->strName) {
                strTypeValue = itor->strValue;
                VR_LOG("Type : %s", strTypeValue.c_str());
            }
            else if ("ID" == itor->strName) {
                strID = itor->strValue;
                VR_LOG("ID : %s", strID.c_str());
            }
        }
        if ("State" == strTypeValue) {
            m_srtIDMappingPara.strStateID = strID;
            m_iIDMappingRequestType = m_iIDMappingRequestType | 1;
        }
        else if ("City" == strTypeValue) {
            m_srtIDMappingPara.strCityID = strID;
            m_iIDMappingRequestType = m_iIDMappingRequestType | 2;
        }
        else if ("Street" == strTypeValue) {
            m_srtIDMappingPara.strStreetID = strID;
            m_iIDMappingRequestType = m_iIDMappingRequestType | 4;
        }
    }

    if ((7 == m_iIDMappingRequestType) || (m_iIDMappingRequestType & 4)) {
        // current request is full or street, not need to delete info
    }
    else if ((m_iIDMappingRequestType & 2)) {
        // current request include city, need to delete street info
        m_srtIDMappingPara.strStreetID = "";
    }
    else if ((m_iIDMappingRequestType & 1)) {
        // current request is only state, need to delete city and street info
        m_srtIDMappingPara.strCityID = "";
        m_srtIDMappingPara.strStreetID = "";
    }

    m_bCurAddressQueryisIDMapping = false;
    if (m_bHaveAddressQueried) {
        return true;
    }

    // will request info caused by IDMapping
    m_bCurAddressQueryisIDMapping = true;
    std::string strReqMsg = "<data>"
                                "<index>0</index>"
                                "<list id=\"vbtNavi\">"
                                    "<items>"
                                        "<item>"
                                            "<zoneId>";
    strID = m_srtIDMappingPara.strStateID;
    ChangeValue(strID, false);
    strReqMsg.append(strID);
    strReqMsg.append("</zoneId>");
    strReqMsg.append("<cityId>");
    strID = m_srtIDMappingPara.strCityID;
    ChangeValue(strID, false);
    strReqMsg.append(strID);
    strReqMsg.append("</cityId>");
    strReqMsg.append("<streetId>");
    strID = m_srtIDMappingPara.strStreetID;
    ChangeValue(strID, false);
    strReqMsg.append(strID);
    strReqMsg.append("</streetId>");
    strReqMsg.append("<houseNumber>");
    strReqMsg.append("</houseNumber>");
    strReqMsg.append("</item></items></list></data>");

    std::string strResponse;
    if (NULL != m_pNaviAddressCommon) {
        m_pNaviAddressCommon->addressMapping(strReqMsg, strResponse);
    }

    VR_LOG("xml : %s", strResponse.c_str());

    VR_VoiceBoxXmlParser parser(strResponse);
    m_strAddressIdList = GetChildNodeString(strResponse, "list");

    std::string strMappingType = parser.getValueByKey("mappingType");
    std::string strActionOp;
    if ("TILL_STREET" == strMappingType) {
        strActionOp = "queryStreetInfo";
    }
    else if ("TILL_CITY" == strMappingType) {
        strActionOp = "queryCityInfo";
    }
    else if ("TILL_STATE" == strMappingType) {
        strActionOp = "queryStateInfo";
    }
    else {
        return false;
    }

    GetInfo(strActionOp);
    return true;

}

void
VR_VoiceBoxAgentNavi::QueryPointCollection(const std::string& strType)
{
    VR_LOGD_FUNC();

    if (m_engineCallback.GetUpdateMapState()) {
        VR_LOG("Update Map state : yes");
        std::string strReply;
        std::string strName;

        if (VBT_DRIVER_REQUEST_ACTION_TYPE_DESTINATION_HOME_PRESET_DEFINED_REQUEST == m_strCurrActionResult) {
            strReply = "Home Preset Defined Reply";
            strName = "Defined";
        }
        else if (VBT_DRIVER_REQUEST_ACTION_TYPE_DESTINATION_PRESET_VALID_REQUEST == m_strCurrActionResult) {
            strReply = "Preset Valid Reply";
            strName = "Valid";
        }

        CVECIPtr<IVECIParameterSet> spParams;
        m_client.CreateParameterSet(&spParams);
        spParams->AddParameter(strName.c_str(), _T("False"), _T(""), _T(""));

        m_engineCommand.DriverActionReply(
            m_strCurrAgent.c_str(),
            strReply.c_str(),
            spParams, NULL, NULL);
    }
    else {
        VR_LOG("Update Map state : no");
        std::string strAction = "<action agent=\"navi\" op=\"queryPointCollection\">"
                                    "<type>";
        strAction.append(strType);
        strAction.append("</type>");
        strAction.append("</action>");

        m_engineCallback.OnInfoQueryMessage(strAction);
    }
}

void
VR_VoiceBoxAgentNavi::ChangeValue(std::string& strValue, bool bToHex)
{
    VR_LOGD_FUNC();

    if ("" == strValue) {
        return;
    }

    std::string strTo = strValue;
    int iValue = 0;

    if (!bToHex) {
        iValue = strtol(strTo.c_str(), NULL, 16);
        VR_LOG("value is %d", iValue);
        std::stringstream ss;
        ss << iValue;
        ss >> strValue;
    }
    else {
        iValue = atoi(strTo.c_str());
        char buffer[5] = {};
        snprintf(buffer, 5, "%x", iValue);
        strValue = buffer;
    }
}

bool
VR_VoiceBoxAgentNavi::CanAddWaypointRequest(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return SimplyInfoQueryMessage("queryAllDestinationList");
}

bool
VR_VoiceBoxAgentNavi::HomePresetDefinedRequest(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }
    QueryPointCollection("HOME");

    return true;
}

bool
VR_VoiceBoxAgentNavi::MapDataInAreaRequest(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    if (m_engineCallback.GetUpdateMapState()) {
        VR_LOG("Update Map state : yes");
        CVECIPtr<IVECIParameterSet> spParams;
        m_client.CreateParameterSet(&spParams);
        spParams->AddParameter(_T("In Area"), _T("False"), _T(""), _T(""));

        m_engineCommand.DriverActionReply(
            m_strCurrAgent.c_str(),
            _T("Map Data In Area Reply"),
            spParams, NULL, NULL);
        return true;
    }

    VR_LOG("Update Map state : no");

    if ("" == m_strPresetGuide) {
        VR_LOG("no guide");

        CVECIPtr<IVECIParameterSet> spParams;
        m_client.CreateParameterSet(&spParams);
        spParams->AddParameter(_T("In Area"), "False", _T(""), _T(""));

        m_engineCommand.DriverActionReply(
            m_strCurrAgent.c_str(),
            _T("Map Data In Area Reply"),
            spParams, NULL, NULL);
        return false;
    }

    std::string strAction = "<action agent=\"navi\" op=\"queryPointDataArea\">";
    strAction.append(m_strPresetGuide);
    strAction.append("</action>");

    m_engineCallback.OnInfoQueryMessage(strAction);

    m_strRouteGuide = m_strPresetGuide;

    return true;
}

bool
VR_VoiceBoxAgentNavi::PresetValidRequest(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    std::string strNumber = GetActionParaByName(pcMsg, "Number");
    VR_LOG("preset number : %s", strNumber.c_str());
    std::string strType;

    if ("1" == strNumber) {
        strType = "PRESET1";
    }
    else if ("2" == strNumber) {
        strType = "PRESET2";
    }
    else {
        strType = "COMPANY";
    }

    QueryPointCollection(strType);

    return true;
}

bool
VR_VoiceBoxAgentNavi::PresetsAvailableRequest(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::PrevDestValidRequest(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    VR_LOG("this request is not used in Suntec!");

    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    spParams->AddParameter(_T("Valid"), _T("True"), _T(""), _T(""));

    m_engineCommand.DriverActionReply(
        m_strCurrAgent.c_str(),
        _T("Previous Destination Valid Reply"),
        spParams, NULL, NULL);

    return true;
}

bool
VR_VoiceBoxAgentNavi::PrevDestsRequest(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    if (m_engineCallback.GetUpdateMapState()) {
        VR_LOG("Update Map state : yes");
        CVECIPtr<IVECIParameterSet> spParams;
        m_client.CreateParameterSet(&spParams);

        spParams->AddParameter(_T("Available"), _T("False"), _T(""), _T(""));
        m_engineCommand.DriverActionReply(
            m_strCurrAgent.c_str(),
            _T("Previous Destinations Reply"),
            spParams, NULL, NULL);
    }
    else {
        VR_LOG("Update Map state : no");
        std::string strAction = "<action agent=\"navi\" op=\"queryDestinationHistory\">"
                                    "<count>25</count>"
                                "</action>";

        m_engineCallback.OnInfoQueryMessage(strAction);
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::PrevStartPointDefinedRequest(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::WaypointRequest(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }
    m_bWayPointRequest = true;
    return SimplyInfoQueryMessage("queryAllDestinationList");
}

// Agent: Address Entry
bool
VR_VoiceBoxAgentNavi::AddAddressToActiveRoute(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::AddWaypoint(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::AddWaypointCarParked(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::AddWaypointRequest(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ActiveRounteConfirmation(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }
    m_bisAddToRoute = false;
    return JumpScreenSetRoute();
}

bool
VR_VoiceBoxAgentNavi::AddRouteConfirmArrivalPoint(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }
    std::string strAddressType = GetActionParaByName(pcMsg, "Address Type");
    VR_LOG("Address Type : %s", strAddressType.c_str());
    std::string strGuideType;
    m_bisAddToRoute = true;
    // change address type for UI display
    if ("freeway" == strAddressType) {
        strGuideType = "FREEWAY";
    }
    else if ("ferry" == strAddressType) {
        strGuideType = "FERRY_ROUTE";
    }
    else if ("tunnel" == strAddressType) {
        strGuideType = "TUNNEL";
    }
    else if ("toll" == strAddressType) {
        strGuideType = "TOLL_ROAD";
    }
    else if ("bridge" == strAddressType) {
        strGuideType = "BRIDGE";
    }
    else {
        VR_LOG("Address Type is error");
        strGuideType = "";
    }
    return JumpScreenSetRouteConfirm(strGuideType);
}

bool
VR_VoiceBoxAgentNavi::AddRouteMaxWaypoint(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::AddRouteRequest(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    CalculateRoute("ADD_TO_ROUTE");
    return true;
}

bool
VR_VoiceBoxAgentNavi::AddressConfirmationBridge(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return JumpScreenSetRouteConfirm("BRIDGE");
}

bool
VR_VoiceBoxAgentNavi::AddressConfirmationFerry(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return JumpScreenSetRouteConfirm("FERRY_ROUTE");
}

bool
VR_VoiceBoxAgentNavi::AddressConfirmationFreeway(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return JumpScreenSetRouteConfirm("FREEWAY");
}

bool
VR_VoiceBoxAgentNavi::AddressConfirmationTollRoad(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return JumpScreenSetRouteConfirm("TOLL_ROAD");
}

bool
VR_VoiceBoxAgentNavi::AddressConfirmationTunnel(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return JumpScreenSetRouteConfirm("TUNNEL");
}

bool
VR_VoiceBoxAgentNavi::AddressConfirmed(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    CalculateRoute("REROUTE");
    return true;
}

bool
VR_VoiceBoxAgentNavi::AddressRejected(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ChangeAddress(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ChangeAddressCity(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ChangeAddressHouseNumber(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ChangeAddressState(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ChangeAddressStreet(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ChangeCityOrState(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ChangeCityOrStateCity(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ChangeCityOrStateHouseNumber(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ChangeCityOrStateState(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ChangeCityOrStateStreet(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ChangeDefaultStateCatch(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    return true;
}

bool
VR_VoiceBoxAgentNavi::ChangeStateInNBestList(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ChangeStreetCityOrState(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ChangeStreetCityOrStateCity(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ChangeStreetCityOrStateHouseNumber(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ChangeStreetCityOrStateState(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ChangeStreetCityOrStateStreet(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::CityWithNoStreetsRejected(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ConfirmAddress(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    // if (AddressMapping(pcMsg)) {
    //     return true;
    // }

    return JumpScreenAddressConfirm();
}

bool
VR_VoiceBoxAgentNavi::ConfirmAddressCity(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ConfirmAddressHouseNumber(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ConfirmAddressState(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return JumpScreenSpeakState();
}

bool
VR_VoiceBoxAgentNavi::ConfirmAddressConfirmSuccess(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    // need to do an action
    CalculateRoute("REROUTE");
    return true;
}

bool
VR_VoiceBoxAgentNavi::ConfirmAddressWithoutHouseNumber(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return JumpScreenAddressConfirm();
}

bool
VR_VoiceBoxAgentNavi::ConfirmArrivalPointAddWaypoint(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    // // curent arrival point is special, and route havv exist
    if ("" != m_strRouteGuideNotNormal) {
        m_strRouteGuide = m_strRouteGuideNotNormal;
    }
    CalculateRoute("ADD_TO_ROUTE");
    return true;
}

bool
VR_VoiceBoxAgentNavi::ConfirmArrivalPointRejectSuccess(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }
    VR_LOG("m_bisAddToRoute is : %d", m_bisAddToRoute);

    if (m_bisAddToRoute) {
        CalculateRoute("ADD_TO_ROUTE");
    }
    else {
        CalculateRoute("REROUTE");
    }
    return true;
}

bool
VR_VoiceBoxAgentNavi::ConfirmArrivalPointSuccess(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    // curent arrival point is special, and there is no route
    if ("" != m_strRouteGuideNotNormal) {
        m_strRouteGuide = m_strRouteGuideNotNormal;
    }
    CalculateRoute("REROUTE");
    return true;
}

bool
VR_VoiceBoxAgentNavi::ReplaceRouteConfirmArrivalPoint(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    std::string strAddressType = GetActionParaByName(pcMsg, "Address Type");
    // change address type for UI display
    std::string strGuideType;
    if ("freeway" == strAddressType) {
        strGuideType = "FREEWAY";
    }
    else if ("ferry" == strAddressType) {
        strGuideType = "FERRY_ROUTE";
    }
    else if ("tunnel" == strAddressType) {
        strGuideType = "TUNNEL";
    }
    else if ("toll" == strAddressType) {
        strGuideType = "TOLL_ROAD";
    }
    else if ("bridge" == strAddressType) {
        strGuideType = "BRIDGE";
    }
    else {
        VR_LOG("Address Type is error");
        strGuideType = "";
    }
    return JumpScreenSetRouteConfirm(strGuideType);
}

bool
VR_VoiceBoxAgentNavi::ConfirmAddressStreet(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ConfirmAnywhere(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    // if (AddressMapping(pcMsg)) {
    //     return true;
    // }

    return JumpScreenAddressConfirm();
}

bool
VR_VoiceBoxAgentNavi::ConfirmAnywhereCity(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ConfirmAnywhereHouseNumber(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ConfirmAnywhereState(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ConfirmAnywhereStreet(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ConfirmCityWithNoStreets(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return JumpScreenAddressConfirm();
}

bool
VR_VoiceBoxAgentNavi::ConfirmCityWithNoStreetsCity(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ConfirmCityWithNoStreetsState(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::GetCity(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    if (VR_Step_None == m_curQueryAddressStep || m_bNBest) {
        AddressMapping(pcMsg);
    }
    else {
        BuildAddress(pcMsg);
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::GetDefaultState(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::GetDirectionsToAddress(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }
    m_curQueryAddressStep = VR_Step_None;

    return JumpScreenSpeakEntriesAddress();
}

bool
VR_VoiceBoxAgentNavi::GetDirectionsToCountrywide(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return JumpScreenSpeakEntriesAddress();
}

bool
VR_VoiceBoxAgentNavi::GetDirectionsToCountrywideNoDefaultState(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return JumpScreenSpeakEntriesAddress();
}

bool
VR_VoiceBoxAgentNavi::GetDirectionsToMultiStepStartingWithState(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return JumpScreenSpeakState();
}

bool
VR_VoiceBoxAgentNavi::GetDirectionsToUnsupported(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::GetDirectionsViaCountrywide(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::GetDirectionsViaMultiStepStartingWithInfo(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::GetDirectionsViaUnsupported(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::GetHouseNumber(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return JumpScreenSpeakHouseNum();
}

bool
VR_VoiceBoxAgentNavi::GetStreetBeforeHouseNumber(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return JumpScreenSpeakStreet();
}

bool
VR_VoiceBoxAgentNavi::InvalidHouseNumber(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::MultiStepStartingWithState(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return JumpScreenSpeakState();
}

bool
VR_VoiceBoxAgentNavi::NativeAddressConfirmCountrywide(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::NativeAddressRejectedStateReplace(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ReplaceWaypoint(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ReplaceWaypointRequest(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::SaveAddress(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::SaveAddressIsFull(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::SelectUserIntent(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ShowOnMap(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::WaypointConfirmed(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::CarParkedRequest(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }
    std::string strCarParked = "<action agent=\"navi\" op=\"queryVehicleinMotion\"/>";
    m_engineCallback.OnRequest(strCarParked);

    return true;
}

bool
VR_VoiceBoxAgentNavi::SaveAddrRequest(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::ValidateHouseNum(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();
    VR_LOGP("DE: start query navi address info data start....");

    if (NULL == pcMsg) {
        return false;
    }
    VR_LOG("m_curQueryAddressStep is : [%d]", m_curQueryAddressStep);

    if (VR_Step_None == m_curQueryAddressStep || m_bNBest) {
        AddressMapping(pcMsg);
    }
    // else if (VR_Step_Done == m_curQueryAddressStep) {
    //     std::string strHouseNum = GetActionParaByName(pcMsg, "House Number");

    //     CVECIPtr<IVECIParameterSet> spParams;
    //     m_client.CreateParameterSet(&spParams);

    //     if (NULL == spParams.ptr()) {
    //         return false;
    //     }

    //     if ("" == strHouseNum) {
    //         spParams->AddParameter("Result", "House Numbers Not Available", "", "");
    //     }
    //     else {
    //         std::string strHouseFormat;

    //         if (GetHouseNumType(strHouseNum, strHouseFormat)) {
    //             spParams->AddParameter("Result", "Valid", "", "");
    //             spParams->AddParameter("House Number Format", strHouseFormat.c_str(), "", "");
    //             spParams->AddParameter("Address Type", "default", "", "");
    //         }
    //         else {
    //             spParams->AddParameter("Result", "Invalid", "", "");
    //         }
    //     }
    //     HRESULT retCode = m_engineCommand.DriverActionReply(m_strCurrAgent.c_str(),
    //             "House Number Validation", spParams, NULL, NULL);

    //     if (S_OK != retCode) {
    //         return false;
    //     }
    // }
    else {
        BuildAddress(pcMsg);
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::NBest(
    IVECIParsedMessage *pcMsg
)
{
    VR_LOGD_FUNC();
    VR_LOGP("DE: query navi address info data start....nbest data...");

    if (NULL == pcMsg) {
        return false;
    }

    ListNodeInfo listNodeInfo;
    if (!GetListParameter(pcMsg, listNodeInfo)) {
        return false;
    }

    CVECIOutStr strCommand;
    (void)pcMsg->GetValueByKey("Message.List.Execution.Parameter.Command.value", &strCommand);
    std::string strCommandContent = strCommand.Get();
    VR_LOG("Current Command Content : %s", strCommandContent.c_str());
    if ("Get Address" == strCommandContent) {
        AddressMapping(listNodeInfo);
    }
    else {
        BuildAddress(listNodeInfo, strCommandContent);
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::AddressMapping(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    m_bNBest = false;

    if (NULL == pcMsg) {
        return false;
    }

    std::string strHouseNum = GetActionParaByName(pcMsg, "House Number");
    std::string strStreet = GetActionParaByName(pcMsg, "Street");
    std::string strCity = GetActionParaByName(pcMsg, "City");
    std::string strState = GetActionParaByName(pcMsg, "State");

    VR_LOG("House Number: %s", strHouseNum.c_str());
    VR_LOG("Street Number: %s", strStreet.c_str());
    VR_LOG("City Number: %s", strCity.c_str());
    VR_LOG("State Number: %s", strState.c_str());

    m_srtIDMappingPara.strStateID = strState;
    m_srtIDMappingPara.strCityID = strCity;
    m_srtIDMappingPara.strStreetID = strStreet;

    ChangeValue(strStreet, false);
    ChangeValue(strCity, false);
    ChangeValue(strState, false);

    std::string strReqMsg = "<data>"
                                "<index>0</index>"
                                "<list id=\"vbtNavi\">"
                                    "<items>"
                                        "<item>"
                                            "<zoneId>";
    strReqMsg.append(strState);
    strReqMsg.append("</zoneId>");
    strReqMsg.append("<cityId>");
    strReqMsg.append(strCity);
    strReqMsg.append("</cityId>");
    strReqMsg.append("<streetId>");
    strReqMsg.append(strStreet);
    strReqMsg.append("</streetId>");
    strReqMsg.append("<houseNumber>");
    strReqMsg.append(strHouseNum);
    strReqMsg.append("</houseNumber>");
    strReqMsg.append("</item></items></list></data>");

    std::string strResponse;
    if (NULL != m_pNaviAddressCommon) {
        m_pNaviAddressCommon->addressMapping(strReqMsg, strResponse);
    }

    VR_LOG("xml : %s", strResponse.c_str());

    VR_VoiceBoxXmlParser parser(strResponse);
    m_strAddressIdList = GetChildNodeString(strResponse, "list");

    std::string strMappingType = parser.getValueByKey("mappingType");
    std::string strActionOp;
    if ("FULL" == strMappingType) {
        m_curQueryAddressStep = VR_Step_HouseNum;
        strActionOp = "queryAddressInfo";
    }
    else if ("TILL_STREET" == strMappingType) {
        m_curQueryAddressStep = VR_Step_Street;
        strActionOp = "queryStreetInfo";
    }
    else if ("TILL_CITY" == strMappingType) {
        m_curQueryAddressStep = VR_Step_City;
        strActionOp = "queryCityInfo";
    }
    else if ("TILL_STATE" == strMappingType) {
        m_curQueryAddressStep = VR_Step_State;
        strActionOp = "queryStateInfo";
    }
    else {
        m_curQueryAddressStep = VR_Step_None;
        return false;
    }

    GetInfo(strActionOp);

    return true;
}

bool
VR_VoiceBoxAgentNavi::AddressMapping(const ListNodeInfo& listNodeInfo)
{
    m_bNBest = true;

    std::string strReqMsg = "<data>"
                                "<index>0</index>"
                                "<list id=\"vbtNavi\">"
                                    "<items>";

    std::string strName;
    std::string strValue;
    VoiceVector<NodeInfo>::const_iterator itor;
    VoiceVector<VoiceVector<NodeInfo>::type>::const_iterator itorItem = listNodeInfo.vecItems.cbegin();
    for (; itorItem != listNodeInfo.vecItems.cend(); ++itorItem) {
        strReqMsg.append("<item>");
        for (itor = itorItem->cbegin(); itor != itorItem->cend(); ++itor) {
            strValue = itor->strValue;

            if ("House Number" == itor->strName) {
                strName = "houseNumber";
            }
            else if ("Street" == itor->strName) {
                strName = "streetId";
                ChangeValue(strValue, false);
            }
            else if ("City" == itor->strName) {
                strName = "cityId";
                ChangeValue(strValue, false);
            }
            else if ("State" == itor->strName) {
                strName = "zoneId";
                ChangeValue(strValue, false);
            }
            else {
                continue;
            }

            VR_LOG("%s, %s", strName.c_str(), strValue.c_str());

            strReqMsg.append("<");
            strReqMsg.append(strName);
            strReqMsg.append(">");
            strReqMsg.append(strValue);
            strReqMsg.append("</");
            strReqMsg.append(strName);
            strReqMsg.append(">");
        }
        strReqMsg.append("</item>");
    }
    strReqMsg.append("</items></list></data>");

    std::string strResponse;
    if (NULL != m_pNaviAddressCommon) {
        m_pNaviAddressCommon->addressMapping(strReqMsg, strResponse);
    }

    VR_LOG("xml : %s", strResponse.c_str());

    VR_VoiceBoxXmlParser parser(strResponse);
    m_strAddressIdList = GetChildNodeString(strResponse, "list");

    std::string strMappingType = parser.getValueByKey("mappingType");
    std::string strActionOp;
    if ("FULL" == strMappingType) {
        // this option need to be change bacause we need query full address list info
        m_curQueryAddressStep = VR_Step_Street;
        strActionOp = "queryStreetInfo";
    }
    else if ("TILL_STREET" == strMappingType) {
        m_curQueryAddressStep = VR_Step_Street;
        strActionOp = "queryStreetInfo";
    }
    else if ("TILL_CITY" == strMappingType) {
        m_curQueryAddressStep = VR_Step_City;
        strActionOp = "queryCityInfo";
    }
    else if ("TILL_STATE" == strMappingType) {
        m_curQueryAddressStep = VR_Step_State;
        strActionOp = "queryStateInfo";
    }
    else {
        m_curQueryAddressStep = VR_Step_None;
        return false;
    }

    GetInfo(strActionOp);
    return true;
}

bool
VR_VoiceBoxAgentNavi::BuildAddress(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    m_bNBest = false;

    if (NULL == pcMsg) {
        return false;
    }

    std::string strHouseNum = GetActionParaByName(pcMsg, "House Number");
    std::string strStreet = GetActionParaByName(pcMsg, "Street");
    std::string strCity = GetActionParaByName(pcMsg, "City");
    std::string strState = GetActionParaByName(pcMsg, "State");

    VR_LOG("m_curQueryAddressStep is :[%d]!", m_curQueryAddressStep);
    VR_LOG("House Number: %s", strHouseNum.c_str());
    VR_LOG("Street Number: %s", strStreet.c_str());
    VR_LOG("City Number: %s", strCity.c_str());
    VR_LOG("State Number: %s", strState.c_str());

    m_srtIDMappingPara.strStateID = strState;
    m_srtIDMappingPara.strCityID = strCity;
    m_srtIDMappingPara.strStreetID = strStreet;

    ChangeValue(strStreet, false);
    ChangeValue(strCity, false);
    ChangeValue(strState, false);

    std::string strBuildType;
    std::string strActionOp;
    if ("" == strState) {
        VR_LOG("state is NULL, error");
        return false;
    }
    else if ("" == strCity) {
        strBuildType = "STATE";
        m_curQueryAddressStep = VR_Step_State;
        strActionOp = "queryStateInfo";
    }
    else if ("" == strStreet) {
        strBuildType = "CITY";
        m_curQueryAddressStep = VR_Step_City;
        strActionOp = "queryCityInfo";
    }
    else if ("" == strHouseNum) {
        strBuildType = "STREET";
        m_curQueryAddressStep = VR_Step_Street;
        strActionOp = "queryStreetInfo";
    }
    else {
        switch (m_curQueryAddressStep) {
        case VR_Step_None:
        {
            strBuildType = "STATE";
            m_curQueryAddressStep = VR_Step_State;
            strActionOp = "queryStateInfo";
            break;
        }
        case VR_Step_State:
        {
            strBuildType = "CITY";
            m_curQueryAddressStep = VR_Step_City;
            strActionOp = "queryCityInfo";
            break;
        }
        case VR_Step_City:
        {
            strBuildType = "STREET";
            m_curQueryAddressStep = VR_Step_Street;
            strActionOp = "queryStreetInfo";
            break;
        }
        case VR_Step_Street:
        case VR_Step_HouseNum:
        case VR_Step_Done:
        {
            strBuildType = "NUMBER";
            m_curQueryAddressStep = VR_Step_HouseNum;
            strActionOp = "queryAddressInfo";
            break;
        }
        default:
        {
            return false;
        }
        }
    }

    std::string strReqMsg = "<buildType>";
    strReqMsg.append(strBuildType);
    strReqMsg.append("</buildType>");

    std::string strAddressInfo = GetChildNodeString(m_strFirstSearchAddress, "addressInfo");
    strReqMsg.append(strAddressInfo);

    strReqMsg.append("<asrResult><list><items><item>");
    strReqMsg.append("<zoneId>");
    strReqMsg.append(strState);
    strReqMsg.append("</zoneId>");
    strReqMsg.append("<cityId>");
    strReqMsg.append(strCity);
    strReqMsg.append("</cityId>");
    strReqMsg.append("<streetId>");
    strReqMsg.append(strStreet);
    strReqMsg.append("</streetId>");
    strReqMsg.append("<houseNumber>");
    strReqMsg.append(strHouseNum);
    strReqMsg.append("</houseNumber>");
    strReqMsg.append("</item></items></list></asrResult>");

    std::string strResponse;
    if (NULL != m_pNaviAddressCommon) {
        m_pNaviAddressCommon->buildAddress(strReqMsg, strResponse);
    }

    VR_VoiceBoxXmlParser parser(strResponse);
    m_strAddressIdList = GetChildNodeString(strResponse, "list");

    GetInfo(strActionOp);

    return true;
}

bool
VR_VoiceBoxAgentNavi::BuildAddress(const ListNodeInfo& listNodeInfo, const std::string& strCommand)
{
    VR_LOGD_FUNC();
    m_bNBest = true;

    std::string strBuildType;
    std::string strActionOp;
    if ("Get State" == strCommand) {
        strBuildType = "STATE";
        m_curQueryAddressStep = VR_Step_State;
        strActionOp = "queryStateInfo";
    }
    else if ("Get City" == strCommand) {
        strBuildType = "CITY";
        m_curQueryAddressStep = VR_Step_City;
        strActionOp = "queryCityInfo";
    }
    else if ("Get Street Before House Number" == strCommand) {
        strBuildType = "STREET";
        m_curQueryAddressStep = VR_Step_Street;
        strActionOp = "queryStreetInfo";
    }
    else if ("Get House Number" == strCommand) {
        // we have no interface to query list info of address with housenumber as style, and it is temporary because of VBT's bug
        strBuildType = "STREET";
        m_curQueryAddressStep = VR_Step_HouseNum;
        strActionOp = "queryStreetInfo";
    }
    else {
        return false;
    }

    std::string strReqMsg = "<buildType>";
    strReqMsg.append(strBuildType);
    strReqMsg.append("</buildType>");

    std::string strAddressInfo = GetChildNodeString(m_strFirstSearchAddress, "addressInfo");
    strReqMsg.append(strAddressInfo);

    strReqMsg.append("<asrResult><list><items>");
    std::string strName;
    std::string strValue;
    VoiceVector<NodeInfo>::const_iterator itor;
    VoiceVector<VoiceVector<NodeInfo>::type>::const_iterator itorItem = listNodeInfo.vecItems.cbegin();
    for (; itorItem != listNodeInfo.vecItems.cend(); ++itorItem) {
        strReqMsg.append("<item>");
        for (itor = itorItem->cbegin(); itor != itorItem->cend(); ++itor) {
            strValue = itor->strValue;
            if ("House Number" == itor->strName) {
                strName = "houseNumber";
            }
            else if ("Street" == itor->strName) {
                strName = "streetId";
                ChangeValue(strValue, false);
            }
            else if ("City" == itor->strName) {
                strName = "cityId";
                ChangeValue(strValue, false);
            }
            else if ("State" == itor->strName) {
                strName = "zoneId";
                ChangeValue(strValue, false);
            }
            else {
                continue;
            }

            strReqMsg.append("<");
            strReqMsg.append(strName);
            strReqMsg.append(">");
            strReqMsg.append(strValue);
            strReqMsg.append("</");
            strReqMsg.append(strName);
            strReqMsg.append(">");
        }
        strReqMsg.append("</item>");
    }

    strReqMsg.append("</items></list></asrResult>");

    std::string strResponse;
    if (NULL != m_pNaviAddressCommon) {
        m_pNaviAddressCommon->buildAddress(strReqMsg, strResponse);
    }

    VR_VoiceBoxXmlParser parser(strResponse);
    m_strAddressIdList = GetChildNodeString(strResponse, "list");

    GetInfo(strActionOp);

    return true;
}


void
VR_VoiceBoxAgentNavi::GetInfo(const std::string& strActionOp)
{
    VR_LOGD_FUNC();
    VR_LOG("m_curQueryAddressStep is :[%d]!", m_curQueryAddressStep);
    VR_LOG("action Op is :[%s]!", strActionOp.c_str());

    std::string strAction = "<action agent=\"navi\" op=\"";
    strAction.append(strActionOp);

    strAction.append("\" >");
    strAction.append("<languageId>''</languageId>");

    if ("queryStreetInfo" == strActionOp
        || "queryCityInfo" == strActionOp
        || "queryStateInfo" == strActionOp) {
        strAction.append(m_strAddressIdList);
    }
    else if ("queryAddressInfo" == strActionOp) {
        VoiceVector<std::string>::type vecAddressId;
        GetChildNodeStringVector(m_strAddressIdList, "addressInfo", vecAddressId);

        if (vecAddressId.empty()) {
            VR_LOG("addressInfo is empty! will get addressId...");
            GetChildNodeStringVector(m_strAddressIdList, "addressId", vecAddressId);
            // return;
        }
        std::string strAddressId = vecAddressId.at(0);
        strAction.append("<addressInfo>");
        strAction.append(GetChildNodeString(strAddressId, "zoneId"));
        strAction.append(GetChildNodeString(strAddressId, "cityId"));
        strAction.append(GetChildNodeString(strAddressId, "streetId"));
        strAction.append(GetChildNodeString(strAddressId, "houseNumber"));
        strAction.append("</addressInfo>");
    }
    else {
        VR_LOG("Error query aciton operation!!");
    }
    strAction.append("</action>");

    VR_LOG("strAction is :[%s]!", strAction.c_str());
    m_bHaveAddressQueried = true;
    m_engineCallback.OnInfoQueryMessage(strAction);
}

bool
VR_VoiceBoxAgentNavi::VerifyCityContainsStreets(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    if (VR_Step_None == m_curQueryAddressStep || m_bNBest) {
        AddressMapping(pcMsg);
    }
    else {
        BuildAddress(pcMsg);
    }
    return true;
}

bool
VR_VoiceBoxAgentNavi::NullOperation(
    IVECIParsedMessage *pcMsg
)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr strMsg;
    pcMsg->GetString(&strMsg);

    VR_LOG("Null Operation for the following message:");
    VR_LOG("%s", strMsg.Get());
    return false;
}

bool
VR_VoiceBoxAgentNavi::POICurRequest(
    VR_VoiceBoxXmlParser& parser
)
{
    VR_LOGD_FUNC();

    VR_LOG("get current guide");

    m_strCurGuide = GetChildNodeString(parser.getXmlString(), "guidePoint");

    VR_LOG("Location, %s", m_strLocation.c_str());

    std::string strOp;
    if ("Nearby" == m_strLocation) {
        strOp = "queryPOICollectionNearby";
    }
    else if ("Along Route" == m_strLocation) {
        strOp = "queryPOICollectionAlongRoute";
    }
    else if ("Destination" == m_strLocation) {
        strOp = "queryPOICollectionNearDestination";
    }
    else if ("City" == m_strLocation) {
        return true;
    }
    else {
        return false;
    }

    std::string strReply = "<action agent=\"navi\" op=\"";
    strReply.append(strOp);
    strReply.append("\">");
    strReply.append("<count>25</count>");
    strReply.append("<languageId>''</languageId>");
    strReply.append("<categoryId>");
    strReply.append(m_strPOIID);
    strReply.append("</categoryId>");
    strReply.append(m_strCurGuide);

    strReply.append("</action>");
    VR_LOG("xml : %s", strReply.c_str());
    m_engineCallback.OnInfoQueryMessage(strReply);
    return true;
}

bool
VR_VoiceBoxAgentNavi::HomePresetDefinedReply(
    VR_VoiceBoxXmlParser& parser
)
{
    VR_LOGD_FUNC();

    std::string strError = parser.getValueByKey("errcode");

    std::string strValue = "True";
    if ("ERR_NAVI_NO_POINT_COLLECTION" == strError) {
        strValue = "False";
    }

    std::string strReply;
    std::string strName;

    if (VBT_DRIVER_REQUEST_ACTION_TYPE_DESTINATION_HOME_PRESET_DEFINED_REQUEST == m_strCurrActionResult) {
        strReply = "Home Preset Defined Reply";
        strName = "Defined";
    }
    else if (VBT_DRIVER_REQUEST_ACTION_TYPE_DESTINATION_PRESET_VALID_REQUEST == m_strCurrActionResult) {
        strReply = "Preset Valid Reply";
        strName = "Valid";
    }
    else {
        return false;
    }

    m_naviPoint.clear();
    VoiceVector<std::string>::type vecPoint;
    GetChildNodeStringVector(parser.getXmlString(), "point", vecPoint);

    VoiceVector<std::string>::const_iterator it = vecPoint.cbegin();
    std::string strKey;
    for (; it != vecPoint.cend(); ++it) {
        strKey = GetKey(*it, "id");
        VR_LOG("key id: %s", strKey.c_str());
        m_naviPoint.insert(std::make_pair(strKey, *it));
    }

    std::string strPoint;
    if (1 != vecPoint.size()) {
        VR_LOG("point data error");
    }
    else {
        strPoint = vecPoint.at(0);
    }

    if ("True" == strValue) {
        m_strPresetGuide = GetChildNodeString(strPoint, "guidePoint");
    }
    else {
        m_strPresetGuide = "";
    }

    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    spParams->AddParameter(strName.c_str(), strValue.c_str(), _T(""), _T(""));

    m_engineCommand.DriverActionReply(
        m_strCurrAgent.c_str(),
        strReply.c_str(),
        spParams, NULL, NULL);
    return true;
}

bool
VR_VoiceBoxAgentNavi::PointDataAreaRequest(
    VR_VoiceBoxXmlParser& parser
)
{
    VR_LOGD_FUNC();

    std::string strValue = parser.getValueByKey("coverage");

    if ("true" == strValue) {
        strValue = "True";
    }
    else {
        strValue = "False";
    }

    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    spParams->AddParameter(_T("In Area"), strValue.c_str(), _T(""), _T(""));

    m_engineCommand.DriverActionReply(
        m_strCurrAgent.c_str(),
        _T("Map Data In Area Reply"),
        spParams, NULL, NULL);

    return true;
}

bool
VR_VoiceBoxAgentNavi::PreviousDestinationsReply(
    VR_VoiceBoxXmlParser& parser
)
{
    VR_LOGD_FUNC();

    m_naviPoint.clear();
    VoiceVector<std::string>::type vecPoint;
    VoiceVector<VR_DestinationPoint>::type vecPoint_id_name;      ///< store point id and name for sending to VBT
    GetChildNodeStringVector(parser.getXmlString(), "point", vecPoint);

    VoiceVector<std::string>::const_iterator it = vecPoint.cbegin();
    VR_DestinationPoint destinationPointNode;
    // add id and name to vecpoint_id_name, used for senting data to VBT
    for (; it != vecPoint.cend(); ++it) {
        destinationPointNode.pointId = GetKey(*it, "id");
        // save point info to local
        m_naviPoint.insert(std::make_pair(destinationPointNode.pointId, *it));

        destinationPointNode.pointName = GetKey(*it, "name");
        destinationPointNode.pointType = GetKey(*it, "displayGuideType");
        VR_LOG("key id:%s, key name:%s, key type:%s", destinationPointNode.pointId.c_str(), destinationPointNode.pointName.c_str(), destinationPointNode.pointType.c_str());
        vecPoint_id_name.push_back(destinationPointNode);
    }

    std::string strAvailable = "True";
    if (m_naviPoint.empty()) {
        strAvailable = "False";
    }

    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    if (NULL == spParams.ptr()) {
        return false;
    }
    spParams->AddParameter(_T("Available"), strAvailable.c_str(), _T(""), _T(""));

    if ("True" == strAvailable) {
        CVECIPtr<IVECIListItems> pListItems;
        m_client.CreateListItems(&pListItems);
        if (NULL == pListItems.ptr()) {
            return false;
        }

        CVECIPtr<IVECIParameterSet> spParamsList;
        VoiceVector<VR_DestinationPoint>::const_iterator it = vecPoint_id_name.cbegin();
        while (it != vecPoint_id_name.cend()) {
            m_client.CreateParameterSet(&spParamsList);
            if (NULL == spParamsList.ptr()) {
                return false;
            }
            spParamsList->AddParameter("ID", (*it).pointId.c_str(), "", "");
            spParamsList->AddParameter("Name", (*it).pointName.c_str(), "", "");
            spParamsList->AddParameter("Type", (*it).pointType.c_str(), "", "");
            VR_LOG("id:%s, name:%s, type:%s", (*it).pointId.c_str(), (*it).pointName.c_str(), (*it).pointType.c_str());
            pListItems->AddItem(spParamsList);
            ++it;
        }
        m_engineCommand.DriverActionReply(m_strCurrAgent.c_str(),
            "Previous Destinations Reply", spParams, "Previous Destinations", pListItems);
    }
    else {
        m_engineCommand.DriverActionReply(
            m_strCurrAgent.c_str(),
            _T("Previous Destinations Reply"),
            spParams, NULL, NULL);
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::WaypointReply(
    VR_VoiceBoxXmlParser& parser
)
{
    VR_LOGD_FUNC();

    m_naviDropPoint.clear();

    VoiceVector<std::string>::type vecPoint;
    VoiceVector<VR_DestinationPoint>::type vecPoint_id_name;      ///< store point id and name for sending to VBT
    GetChildNodeStringVector(parser.getXmlString(), "dropPoint", vecPoint);

    VoiceVector<std::string>::const_iterator it = vecPoint.cbegin();
    VR_DestinationPoint destinationPointNode;
    // add id and name to vecpoint_id_name, used for senting data to VBT
    VR_LOG("unpass point size %d", vecPoint.size());
    for (; it != vecPoint.cend(); ++it) {
        destinationPointNode.pointId = GetKey(*it, "id");
        if ("" == destinationPointNode.pointId) {
            VR_LOG("key id is not exist !!!");
            continue;
        }
        // save point info to local
        m_naviDropPoint.insert(std::make_pair(destinationPointNode.pointId, *it));

        destinationPointNode.pointName = GetKey(*it, "name");
        VR_LOG("key id:%s, key name:%s", destinationPointNode.pointId.c_str(), destinationPointNode.pointName.c_str());
        vecPoint_id_name.push_back(destinationPointNode);
    }

    // get route status and set it ot VBT
    std::string strAvailable = m_engineCallback.GetRouteStatus() ? "True" : "False";
    if (m_naviDropPoint.empty()) {
        strAvailable = "False";
    }

    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    spParams->AddParameter(_T("Available"), strAvailable.c_str(), _T(""), _T(""));

    if ("True" == strAvailable) {
        CVECIPtr<IVECIListItems> pListItems;
        m_client.CreateListItems(&pListItems);
        if (NULL == pListItems.ptr()) {
            return false;
        }
        CVECIPtr<IVECIParameterSet> spParamsList;

        VoiceVector<VR_DestinationPoint>::const_iterator citDropPoint = vecPoint_id_name.cbegin();
        while (citDropPoint != vecPoint_id_name.cend()) {
            m_client.CreateParameterSet(&spParamsList);
            if (NULL == spParamsList.ptr()) {
                return false;
            }
            spParamsList->AddParameter("ID", (*citDropPoint).pointId.c_str(), "", "");
            spParamsList->AddParameter("Name", (*citDropPoint).pointName.c_str(), "", "");
            VR_LOG("id:%s, name:%s", (*citDropPoint).pointId.c_str(), (*citDropPoint).pointName.c_str());
            pListItems->AddItem(spParamsList);
            ++citDropPoint;
        }
        m_engineCommand.DriverActionReply(m_strCurrAgent.c_str(),
            "Waypoint Reply", spParams, "Waypoints", pListItems);
    }
    else {
        m_engineCommand.DriverActionReply(
            m_strCurrAgent.c_str(),
            _T("Waypoint Reply"),
            spParams, NULL, NULL);
    }

    return true;
}

std::string
VR_VoiceBoxAgentNavi::GetChildNodeString(const std::string& strMessage, const std::string& strChildName)
{
    VR_LOGD_FUNC();

    VR_LOG("MSG: %s", strMessage.c_str());

    std::string strChildNode;
    pugi::xml_document doc;
    if (!doc.load_string(strMessage.c_str())) {
        return strChildNode;
    }

    pugi::xml_node firstNode = doc.first_child();

    pugi::xml_node childNode = firstNode.child(strChildName.c_str());
    if (NULL == childNode) {
        VR_LOG("cannot find this node in root, will get node in childnode deeply");
        firstNode = firstNode.first_child();
        childNode = firstNode.child(strChildName.c_str());
    }
    std::stringstream oss;
    childNode.print(oss);

    strChildNode = oss.str();

    VR_LOG("child %s: %s !", strChildName.c_str(), strChildNode.c_str());
    return strChildNode;
}

bool
VR_VoiceBoxAgentNavi::AddWaypointReply(
    VR_VoiceBoxXmlParser& parser
    )
{
    VR_LOGD_FUNC();

    // when queryAllDestinationList by WaypointRequest,
    if (m_bWayPointRequest) {
        m_bWayPointRequest = false;
        return WaypointReply(parser);
    }

    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    if (NULL == spParams.ptr()) {
        return false;
    }

    std::string strCount = parser.getValueByKey("count");
    int iCount = atoi(strCount.c_str());
    std::string strActionType;
    if ("Add Waypoint Request" == m_strCurrActionResult) {
        strActionType = "Add Waypoint Reply";
        VR_LOG("Add Waypoint Reply");

        if (iCount >= 5) {
            VR_LOG("Status->Max Waypoints");
            spParams->AddParameter("Status", "Max Waypoints", "", "");
        }
        else {
            VR_LOG("Status->Success");
            spParams->AddParameter("Status", "Success", "", "");
        }
    }
    else if ("Can Add Waypoint Request" == m_strCurrActionResult) {
        strActionType = "Can Add Waypoint Reply";
        VR_LOG("Can Add Waypoint Reply");
        if (iCount >= 5) {
            VR_LOG("Can Add->False");
            spParams->AddParameter("Can Add", "False", "", "");
        }
        else {
            VR_LOG("Can Add->True");
            spParams->AddParameter("Can Add", "True", "", "");
        }
    }
    else {
        VR_LOG("error: %s", m_strCurrActionResult.c_str());
        return false;
    }

    HRESULT retCode = m_engineCommand.DriverActionReply(m_strCurrAgent.c_str(),
        strActionType.c_str(), spParams, NULL, NULL);
    if (S_OK != retCode) {
        return false;
    }
    return true;
}

bool
VR_VoiceBoxAgentNavi::IconsShownReply(
    VR_VoiceBoxXmlParser& parser
    )
{
    VR_LOGD_FUNC();

    std::string strShowNum = parser.getValueByKey("number");
    VR_LOG("show icon num %s", strShowNum.c_str());

    int iShowNum = atoi(strShowNum.c_str());

    std::string strStatus;
    if (0 == iShowNum) {
        strStatus = "No Icons Shown";
    }
    else if ((0 < iShowNum) && (5 > iShowNum)) {
       strStatus = "Icons Shown";
    }
    else if (5 <= iShowNum) {
       strStatus = "Max Icons Shown";
    }
    else {
        return false;
    }

    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    if (NULL == spParams.ptr()) {
        return false;
    }
    spParams->AddParameter("Status", strStatus.c_str(), "", "");

    HRESULT retCode = m_engineCommand.DriverActionReply(m_strCurrAgent.c_str(),
        "Icons Shown Reply", spParams, NULL, NULL);
    if (S_OK != retCode) {
        return false;
    }
    return true;
}

bool
VR_VoiceBoxAgentNavi::POICollectionNearby(
    VR_VoiceBoxXmlParser& parser
    )
{
    VR_LOGD_FUNC();

    m_naviCurPointPOIInfo.clear();
    m_strPOIList = parser.getXmlString();
    POIReply(parser);

    m_naviCurPointPOIInfo = m_naviPOIInfo;
    return true;
}

bool
VR_VoiceBoxAgentNavi::POICollectionAlongRoute(
    VR_VoiceBoxXmlParser& parser
    )
{
    VR_LOGD_FUNC();

    m_naviAlongRoutePOIInfo.clear();
    m_strPOIList = parser.getXmlString();
    POIReply(parser);

    m_naviAlongRoutePOIInfo = m_naviPOIInfo;
    return true;
}

bool
VR_VoiceBoxAgentNavi::POICollectionNearDestination(
    VR_VoiceBoxXmlParser& parser
    )
{
    VR_LOGD_FUNC();

    m_naviNearDestPOIInfo.clear();
    m_strPOIList = parser.getXmlString();
    POIReply(parser);

    m_naviNearDestPOIInfo = m_naviPOIInfo;
    return true;
}

std::string
VR_VoiceBoxAgentNavi::GetKey(const std::string& strMessage, const std::string& strKeyName)
{
    std::string strKey;
    VR_VoiceBoxXmlParser parser(strMessage);

    strKey = parser.getValueByKey(strKeyName);

    if ("''" == strKey) {
        strKey = "";
    }

    return strKey;
}

void
VR_VoiceBoxAgentNavi::GetChildNodeStringVector(
    const std::string& strMessage,
    const std::string& strChildName,
    VoiceVector<std::string>::type& vecInfo
)
{
    vecInfo.clear();

    pugi::xml_document doc;
    if (!doc.load_string(strMessage.c_str())) {
        return;
    }

    pugi::xml_node node = doc.first_child();

    pugi::xml_node nodeChild = node;
    std::string strChildNode;

    while (node) {
        std::string strName = node.name();

        nodeChild = node.child(strChildName.c_str());
        VR_LOG("node Name: %s", strName.c_str());
        if (strName == strChildName) {
            std::stringstream oss;
            node.print(oss);

            strChildNode = oss.str();
            vecInfo.push_back(strChildNode);

            node = node.parent();
            node = node.next_sibling();
        }
        else if ("list" == strName) {
            node = node.last_child();
        }
        else if (nodeChild) {
            VR_LOG("the child is need!");
            node = nodeChild;
        }
        else {
            node = node.first_child();
        }
    }
}

bool
VR_VoiceBoxAgentNavi::CarParked(
    VR_VoiceBoxXmlParser& parser
    )
{
    VR_LOGD_FUNC();

    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);

    if (NULL == spParams.ptr()) {
        return false;
    }

    std::string strMotion = parser.getValueByKey("parser");

    if ("false" == strMotion) {
        spParams->AddParameter("Car Parked", "False", "", "");
    }
    else if ("true" == strMotion) {
        spParams->AddParameter("Car Parked", "True", "", "");
    }
    else {
        return false;
    }
    HRESULT retCode = m_engineCommand.DriverActionReply(m_strCurrAgent.c_str(),
            "Car Parked Reply", spParams, NULL, NULL);

    if (S_OK != retCode) {
        return false;
    }
    return true;
}

bool
VR_VoiceBoxAgentNavi::HouseNumValidation(
    VR_VoiceBoxXmlParser& parser
    )
{
    VR_LOGD_FUNC();
    // clear flag address query
    m_bHaveAddressQueried = false;

    m_srtIDMappingPara.strState = parser.getValueByKey("zone");
    m_srtIDMappingPara.strCity = parser.getValueByKey("city");
    m_srtIDMappingPara.strStreet = parser.getValueByKey("street");
    m_srtIDMappingPara.strFullName = parser.getValueByKey("name");
    if (m_bIDMapping) {
        // ID Mapping request need to reply
        IDMappingReply(parser);
        m_bIDMapping = false;
        // if current reponse is only caused by ID Mapping, return
        if (m_bCurAddressQueryisIDMapping) {
            m_bCurAddressQueryisIDMapping = false;
            return true;
        }
    }

    VoiceVector<std::string>::type vecAddress;
    GetChildNodeStringVector(parser.getXmlString(), "address", vecAddress);

    if (vecAddress.empty()) {
        m_strFirstSearchAddress = "";
    }
    else if (m_bNBest) {
        JumpScreenNBest(vecAddress);
        return true;
    }
    else {
        m_strFirstSearchAddress = vecAddress.at(0);
        m_strRouteGuide = GetChildNodeString(m_strFirstSearchAddress, "guidePoint");
    }

    std::string strHouseNumberFound = parser.getValueByKey("houseNumberFound");
    std::string strHouseNum = parser.getValueByKey("houseNumber");
    std::string strHouseNumberExist = parser.getValueByKey("houseNumberExist");
    VR_LOG("houseNumberFound : %s", strHouseNumberFound.c_str());
    VR_LOG("houseNumber : %s", strHouseNum.c_str());
    VR_LOG("houseNumberExist : %s", strHouseNumberExist.c_str());
    VR_LOG("Query Address Step is : %d", m_curQueryAddressStep);

    std::string strResult;
    std::string strHouseFormat = "NumericOnly";
    if (VR_Step_Street == m_curQueryAddressStep) {
        if ("true" == strHouseNumberExist) {
            strResult = "Valid";
        }
        else if ("false" == strHouseNumberExist) {
            strResult = "House Numbers Not Available";
        }
        else {
            VR_LOG("Error Parameter !!!");
            return false;
        }
    }
    else if (VR_Step_HouseNum == m_curQueryAddressStep || VR_Step_None == m_curQueryAddressStep) {
        if ("true" == strHouseNumberExist) {
            if ("false" == strHouseNumberFound) {
                strResult = "Invalid";
            }
            else if ("true" == strHouseNumberFound && GetHouseNumType(strHouseNum, strHouseFormat)) {
                VR_LOG("House Number Format : %s", strHouseFormat.c_str());
                strResult = "Valid";
            }
            else {
                strResult = "Valid";
            }
        }
        else if ("false" == strHouseNumberExist) {
            strResult = "House Numbers Not Available";
        }
        else {
            VR_LOG("Error Parameter !!!");
            return false;
        }
    }
    else if (VR_Step_State == m_curQueryAddressStep) {
        JumpScreenSpeakCity();
        return true;
    }
    else {
        VR_LOG("Error Query Address Step !!!");
        return false;
    }
    m_structHouseNumValidePara.strResult = strResult;
    m_structHouseNumValidePara.strHouseNumberFormat = strHouseFormat;

    std::string strAction = "<action agent=\"navi\" op=\"queryPointType\">";
    strAction.append(m_strRouteGuide);
    strAction.append("</action>");
    m_engineCallback.OnInfoQueryMessage(strAction);

    return true;
}

bool
VR_VoiceBoxAgentNavi::CityVerification(
    VR_VoiceBoxXmlParser& parser
    )
{
    VR_LOGD_FUNC();
    m_bHaveAddressQueried = false;

    m_srtIDMappingPara.strState = parser.getValueByKey("zone");
    m_srtIDMappingPara.strCity = parser.getValueByKey("city");
    m_srtIDMappingPara.strStreet = parser.getValueByKey("street");
    if (m_bIDMapping) {
        // ID Mapping request need to reply
        IDMappingReply(parser);
        m_bIDMapping = false;
        // if current reponse is only caused by ID Mapping, return
        if (m_bCurAddressQueryisIDMapping) {
            m_bCurAddressQueryisIDMapping = false;
            return true;
        }
    }

    VoiceVector<std::string>::type vecAddress;
    GetChildNodeStringVector(parser.getXmlString(), "address", vecAddress);

    if (vecAddress.empty()) {
        m_strFirstSearchAddress = "";
    }
    else {
        m_strFirstSearchAddress = vecAddress.at(0);
    }

    if (m_bNBest) {
        JumpScreenNBest(vecAddress);
        return true;
    }

    VR_VoiceBoxXmlParser parserAddress(m_strFirstSearchAddress);

    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);

    if (NULL == spParams.ptr()) {
        return false;
    }

    std::string strStreetExist = parserAddress.getValueByKey("streetExist");

    if ("false" == strStreetExist) {
        spParams->AddParameter("City Contains Streets", "False", "", "");
    }
    else if ("true" == strStreetExist) {
        spParams->AddParameter("City Contains Streets", "True", "", "");
    }
    else {
        return false;
    }

    HRESULT retCode = m_engineCommand.DriverActionReply(m_strCurrAgent.c_str(),
            "City Verification", spParams, NULL, NULL);

    if (S_OK != retCode) {
        return false;
    }
    return true;
}

bool
VR_VoiceBoxAgentNavi::HandlePointType(
    VR_VoiceBoxXmlParser& parser
    )
{
    VR_LOGD_FUNC();

    VoiceVector<std::string>::type vecGuidePoint;
    GetChildNodeStringVector(parser.getXmlString(), "guidePoint", vecGuidePoint);

    std::string strGuidePoint;
    std::string strGuideType;
    std::string strAddressType;
    m_strRouteGuideNotNormal = "";

    if (1 == vecGuidePoint.size()) {
        // the arrival point is only one type
        strGuidePoint = vecGuidePoint.at(0);
        VR_LOG("strGuidePoint[0] %s", strGuidePoint.c_str());
        // when only one arrival type, set 'default' to make VBT not judge arrival point.
        strGuideType = "DEFAULT";
        std::string str_linId = GetKey(strGuidePoint, "linkId");
        if ("0" != str_linId) {
            // when linkId is default, no need to change
            ChangeNodeValueInXML(m_strRouteGuide, "linkId", str_linId);
        }
    }
    else if (2 == vecGuidePoint.size()) {
        // when arrival point has two type, we must set the special type to make VBT judge arrival point type
        strGuidePoint = vecGuidePoint.at(0);
        VR_LOG("strGuidePoint[0] %s", strGuidePoint.c_str());
        if ("DEFAULT" == strGuideType) {
            // current guide point is normal, we must get another point's type
            m_strRouteGuide = strGuidePoint;
            strGuidePoint = vecGuidePoint.at(1);
            VR_LOG("strGuidePoint[1] %s", strGuidePoint.c_str());
            strGuideType = GetKey(strGuidePoint, "displayGuideType");
            m_strRouteGuideNotNormal = strGuidePoint;
        }
        else {
            // this guide point is not normal, save it's guide type
            m_strRouteGuideNotNormal = strGuidePoint;
            strGuideType = GetKey(strGuidePoint, "displayGuideType");
            strGuidePoint = vecGuidePoint.at(1);
            VR_LOG("strGuidePoint[1] %s", strGuidePoint.c_str());
            m_strRouteGuide = strGuidePoint;
        }
    }
    else {
        VR_LOG("ERROR !!! no guidePoint data");
    }
    VR_LOG("display Guide Type:  [%s]", strGuideType.c_str());

    VR_LOG("m_strCurrAgent : %s", m_strCurrAgent.c_str());
    if ("POI" == m_strCurrAgent) {
        if (!m_bPOISpecialAddressType) {
            VR_LOG("POI have no specail address type, will jump screen POI confirm...");
            return true;
        }
        else {
            VR_LOG("POI is specail address type, will jump screen set route confirm...");
            strGuidePoint = vecGuidePoint.at(0);
            strGuideType = GetKey(strGuidePoint, "displayGuideType");
            return JumpScreenSetRouteConfirm(strGuideType);
        }
    }

    if ("DEFAULT" == strGuideType) {
        strAddressType = "default";
    }
    else if ("FREEWAY" == strGuideType) {
        strAddressType = "freeway";
    }
    else if ("FERRY_ROUTE" == strGuideType) {
        strAddressType = "ferry";
    }
    else if ("TUNNEL" == strGuideType) {
        strAddressType = "tunnel";
    }
    else if ("TOLL_ROAD" == strGuideType) {
        strAddressType = "toll";
    }
    else if ("BRIDGE" == strGuideType) {
        strAddressType = "bridge";
    }
    else if ("URBAN" == strGuideType) {
        strAddressType = "default";
    }
    else {
        strAddressType = "default";
    }

    VR_LOG("Address Type : [%s]", strAddressType.c_str());
    VR_LOG("Result is : [%s]", m_structHouseNumValidePara.strResult.c_str());
    VR_LOG("House Number Format is : [%s]", m_structHouseNumValidePara.strHouseNumberFormat.c_str());

    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);

    if (NULL == spParams.ptr()) {
        return false;
    }

    spParams->AddParameter("Address Type", strAddressType.c_str(), "", "");
    spParams->AddParameter("House Number Format", m_structHouseNumValidePara.strHouseNumberFormat.c_str(), "", "");
    spParams->AddParameter("Result", m_structHouseNumValidePara.strResult.c_str(), "", "");

    HRESULT retCode = m_engineCommand.DriverActionReply(m_strCurrAgent.c_str(),
            "House Number Validation", spParams, NULL, NULL);
    VR_LOG("Reply Driver Action to VBT ........");
    if (S_OK != retCode) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::GetHouseNumType(
    const std::string& strHouseNumber,
    std::string& strHouseNumType
    )
{
    VR_LOGD_FUNC();
    VR_LOG("houseNumber : %s", strHouseNumber.c_str());

    char *cHouseNum = const_cast<char*>(strHouseNumber.c_str());

    bool bAllNum = true;
    strHouseNumType = "";

    while ('\0' != *cHouseNum) {
        if (('a' <= *cHouseNum && 'z' >= *cHouseNum)
            || ('A' <= *cHouseNum && 'Z' >= *cHouseNum)) {
            bAllNum = false;
        }
        else if ('0' <= *cHouseNum && '9' >= *cHouseNum) {

        }
        else if ('-' == *cHouseNum) {
            strHouseNumType = "Hyphenated";
        }
        else {
            VR_LOG("house number no reco");
            return false;
        }
        ++cHouseNum;
    }

    if ("" == strHouseNumType) {
        if (bAllNum) {
            strHouseNumType = "NumericOnly";
        }
        else {
            strHouseNumType = "AlphaNumeric";
        }
    }
    VR_LOG("house number type : %s", strHouseNumType.c_str());
    return true;
}

bool
VR_VoiceBoxAgentNavi::POIReply(
    VR_VoiceBoxXmlParser& parser
)
{
    VR_LOGD_FUNC();

    m_naviPOIInfo.clear();

    VoiceVector<std::string>::type vecPOI;
    VoiceVector<VR_POIInfo>::type vecPOIInfo;      ///< store POI_Name POI_ID POI_Type
    GetChildNodeStringVector(m_strPOIList, "poi", vecPOI);

    VoiceVector<std::string>::const_iterator it = vecPOI.cbegin();
    VR_POIInfo structPoiInfo;
    for (; it != vecPOI.cend(); ++it) {
        structPoiInfo.POIName = GetKey(*it, "alias");
        VR_LOG("key alias: %s", structPoiInfo.POIName.c_str());

        structPoiInfo.POIId = GetKey(*it, "id");
        VR_LOG("key id: %s", structPoiInfo.POIId.c_str());
        // save poi id and its detail data to local
        m_naviPOIInfo.insert(std::make_pair(structPoiInfo.POIId, *it));

        structPoiInfo.POIType = GetKey(*it, "displayGuideType");
        VR_LOG("key displayGuideType: %s", structPoiInfo.POIType.c_str());
        vecPOIInfo.push_back(structPoiInfo);
    }

    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);

    if (NULL == spParams.ptr()) {
        return false;
    }

    std::string strCount = parser.getValueByKey("count");
    int iCount = atoi(strCount.c_str());
    VR_LOG("count : %d", iCount);
    HRESULT retCode = S_OK;
    if (iCount > 0) {
        VR_LOG("Available, True");
        spParams->AddParameter("Available", "True", "", "");

        CVECIPtr<IVECIListItems> pListItems;
        m_client.CreateListItems(&pListItems);
        CVECIPtr<IVECIParameterSet> spParamsList;
        if (NULL == pListItems.ptr()) {
            return false;
        }

        VoiceVector<VR_POIInfo>::const_iterator cit = vecPOIInfo.cbegin();
        while (cit != vecPOIInfo.cend()) {
            m_client.CreateParameterSet(&spParamsList);
            if (NULL == spParamsList.ptr()) {
                return false;
            }

            VR_LOG("alias -> POI  : %s", (*cit).POIName.c_str());
            spParamsList->AddParameter("POI", (*cit).POIName.c_str(), "", "");
            VR_LOG("POI ID : %s", (*cit).POIId.c_str());
            spParamsList->AddParameter("POI ID", (*cit).POIId.c_str(), "", "");
            VR_LOG("POI Type : %s", (*cit).POIType.c_str());
            spParamsList->AddParameter("POI Type", (*cit).POIType.c_str(), "", "");
            ++cit;
            pListItems->AddItem(spParamsList);
        }

        retCode = m_engineCommand.DriverActionReply(m_strCurrAgent.c_str(),
            "POI Reply", spParams, "POIs", pListItems);
    }
    else {
        VR_LOG("Available, False");
        spParams->AddParameter("Available ", "False", "", "");
        retCode = m_engineCommand.DriverActionReply(m_strCurrAgent.c_str(),
            "POI Reply", spParams, NULL, NULL);
    }

    if (S_OK != retCode) {
        VR_LOG("DriverActionReply Error");
        return false;
    }
    VR_LOG("DriverActionReply Success");
    return true;
}

bool
VR_VoiceBoxAgentNavi::IDMappingReply(
    VR_VoiceBoxXmlParser& parser
)
{
    VR_LOGD_FUNC();

    CVECIPtr<IVECIListItems> pListItems;
    m_client.CreateListItems(&pListItems);
    CVECIPtr<IVECIParameterSet> spParamsList;
    m_client.CreateParameterSet(&spParamsList);

    if (NULL == pListItems.ptr() || NULL == spParamsList.ptr()) {
        return false;
    }
    VR_LOG("m_iIDMappingRequestType is : %d", m_iIDMappingRequestType);
    if (7 == m_iIDMappingRequestType) {
        spParamsList->AddParameter("Type", "State", "", "");
        spParamsList->AddParameter("ID", m_srtIDMappingPara.strStateID.c_str(), "", "");
        spParamsList->AddParameter("Result", "", "", "");
        pListItems->AddItem(spParamsList);

        m_client.CreateParameterSet(&spParamsList);
        spParamsList->AddParameter("Type", "City", "", "");
        spParamsList->AddParameter("ID", m_srtIDMappingPara.strCityID.c_str(), "", "");
        spParamsList->AddParameter("Result", "", "", "");
        pListItems->AddItem(spParamsList);

        m_client.CreateParameterSet(&spParamsList);
        spParamsList->AddParameter("Type", "Street", "", "");
        spParamsList->AddParameter("ID", m_srtIDMappingPara.strStreetID.c_str(), "", "");
        spParamsList->AddParameter("Result", m_srtIDMappingPara.strFullName.c_str(), "", "");
        VR_LOG("Street ID = [%s], Value = [%s]", m_srtIDMappingPara.strStreetID.c_str(), m_srtIDMappingPara.strFullName.c_str());
        pListItems->AddItem(spParamsList);

    }
    else {
        if ((m_iIDMappingRequestType & 1)) {
            spParamsList->AddParameter("Type", "State", "", "");
            spParamsList->AddParameter("ID", m_srtIDMappingPara.strStateID.c_str(), "", "");
            spParamsList->AddParameter("Result", m_srtIDMappingPara.strState.c_str(), "", "");
            VR_LOG("State ID = [%s], Value = [%s]", m_srtIDMappingPara.strStateID.c_str(), m_srtIDMappingPara.strState.c_str());
            pListItems->AddItem(spParamsList);
            m_client.CreateParameterSet(&spParamsList);
        }
        if ((m_iIDMappingRequestType & 2)) {
            spParamsList->AddParameter("Type", "City", "", "");
            spParamsList->AddParameter("ID", m_srtIDMappingPara.strCityID.c_str(), "", "");
            spParamsList->AddParameter("Result", m_srtIDMappingPara.strCity.c_str(), "", "");
            VR_LOG("City ID = [%s], Value = [%s]", m_srtIDMappingPara.strCityID.c_str(), m_srtIDMappingPara.strCity.c_str());
            pListItems->AddItem(spParamsList);
            m_client.CreateParameterSet(&spParamsList);
        }
        if ((m_iIDMappingRequestType & 4)) {
            spParamsList->AddParameter("Type", "Street", "", "");
            spParamsList->AddParameter("ID", m_srtIDMappingPara.strStreetID.c_str(), "", "");
            spParamsList->AddParameter("Result", m_srtIDMappingPara.strStreet.c_str(), "", "");
            VR_LOG("Street ID = [%s], Value = [%s]", m_srtIDMappingPara.strStreetID.c_str(), m_srtIDMappingPara.strStreet.c_str());
            pListItems->AddItem(spParamsList);
        }
    }

    HRESULT retCode = m_engineCommand.DriverListReply("Address Entry", "ID Mapping Reply", pListItems);
    if (S_OK != retCode) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentNavi::SetCountryID(
    VR_VoiceBoxXmlParser& parser
)
{
    VR_LOGD_FUNC();

    if (NULL == m_pNaviAddressCommon) {
        VR_LOG("point is NULL");
        return false;
    }

    std::string strCountryID = parser.getValueByKey("countryId");
    m_pNaviAddressCommon->setCountryID(strCountryID);
    return true;
}

bool
VR_VoiceBoxAgentNavi::SimplyInfoQueryMessage(
    const std::string& strOp
)
{
    VR_LOGD_FUNC();

    std::string strQueryMessage = "<action agent=\"navi\" op=\"";
    strQueryMessage.append(strOp);
    strQueryMessage.append("\" />");

    VR_LOG("xml : %s", strQueryMessage.c_str());
    m_engineCallback.OnInfoQueryMessage(strQueryMessage);

    return true;
}

bool
VR_VoiceBoxAgentNavi::JumpScreenSpeakEntriesAddress()
{
    VR_LOGD_FUNC();
    m_curQueryAddressStep = VR_Step_None;
    m_bHaveAddressQueried = false;
    m_bIDMapping = false;
    m_bCurAddressQueryisIDMapping = false;

    std::string strDisplay = "<display agent=\"Common\" content=\"ScreenDisplay\">"
                                "<agent>navi</agent>"
                                "<content>navi_speak_entries_address</content>"
                            "</display>";

    VR_LOG("xml : %s", strDisplay.c_str());
    m_strPrevDisplay = m_strCurDisplay;
    m_strCurDisplay = strDisplay;
    m_engineCallback.OnRecognized(strDisplay);

    m_engineCallback.GetPromptByScreenId("VR-VDE-01", m_strPrompt);

    return true;
}

bool
VR_VoiceBoxAgentNavi::JumpScreenSpeakState()
{
    VR_LOGD_FUNC();

    m_curQueryAddressStep = VR_Step_None;

    std::string strDisplay = "<display agent=\"Common\" content=\"ScreenDisplay\">"
                                "<agent>navi</agent>"
                                "<content>navi_speak_state</content>"
                            "</display>";

    VR_LOG("xml : %s", strDisplay.c_str());
    m_strPrevDisplay = m_strCurDisplay;
    m_strCurDisplay = strDisplay;
    m_engineCallback.OnRecognized(strDisplay);

    m_engineCallback.GetPromptByScreenId("VR-VDE-05", m_strPrompt);

    return true;
}

bool
VR_VoiceBoxAgentNavi::JumpScreenSpeakCity()
{
    VR_LOGD_FUNC();

    std::string strAddressInfo = GetChildNodeString(m_strFirstSearchAddress, "addressInfo");
    std::string strDisplay = "<display agent=\"Common\" content=\"ScreenDisplay\">"
                                "<agent>navi</agent>"
                                "<content>navi_speak_city</content>"
                                "<detail>";

    strDisplay.append(strAddressInfo);
    strDisplay.append("</detail>");
    strDisplay.append("</display>");
    VR_LOG("xml : %s", strDisplay.c_str());

    m_strPrevDisplay = m_strCurDisplay;
    m_strCurDisplay = strDisplay;
    m_engineCallback.OnRecognized(strDisplay);

    m_engineCallback.GetPromptByScreenId("VR-VDE-03", m_strPrompt);

    return true;
}

bool
VR_VoiceBoxAgentNavi::JumpScreenSpeakStreet()
{
    VR_LOGD_FUNC();

    std::string strAddressInfo = GetChildNodeString(m_strFirstSearchAddress, "addressInfo");
    std::string strDisplay = "<display agent=\"Common\" content=\"ScreenDisplay\">"
                                "<agent>navi</agent>"
                                "<content>navi_speak_street</content>"
                                "<detail>";

    strDisplay.append(strAddressInfo);
    strDisplay.append("</detail>");
    strDisplay.append("</display>");

    VR_LOG("xml : %s", strDisplay.c_str());

    m_strPrevDisplay = m_strCurDisplay;
    m_strCurDisplay = strDisplay;
    m_engineCallback.OnRecognized(strDisplay);

    m_engineCallback.GetPromptByScreenId("VR-VDE-06", m_strPrompt);

    return true;
}

bool
VR_VoiceBoxAgentNavi::JumpScreenSpeakHouseNum()
{
    VR_LOGD_FUNC();

    std::string strAddressInfo = GetChildNodeString(m_strFirstSearchAddress, "addressInfo");
    std::string strDisplay = "<display agent=\"Common\" content=\"ScreenDisplay\">"
                                "<agent>navi</agent>"
                                "<content>navi_speak_house_number</content>"
                                "<detail>";

    strDisplay.append(strAddressInfo);
    strDisplay.append("</detail>");
    strDisplay.append("</display>");

    VR_LOG("xml : %s", strDisplay.c_str());

    m_strPrevDisplay = m_strCurDisplay;
    m_strCurDisplay = strDisplay;
    m_engineCallback.OnRecognized(strDisplay);

    m_engineCallback.GetPromptByScreenId("VR-VDE-04", m_strPrompt);

    return true;
}

bool
VR_VoiceBoxAgentNavi::JumpScreenAddressConfirm()
{
    VR_LOGD_FUNC();

    m_curQueryAddressStep = VR_Step_Done;

    if ("Address Entry" != m_strCurrAgent) {
        VR_LOG("m_strCurrAgent is [%s]", m_strCurrAgent.c_str());
        m_strRouteGuide = GetChildNodeString(m_strFirstSearchAddress, "guidePoint");
        std::string strAction = "<action agent=\"navi\" op=\"queryPointType\">";
        strAction.append(m_strRouteGuide);
        strAction.append("</action>");
        m_engineCallback.OnInfoQueryMessage(strAction);
    }

    std::string strDisplay = "<display agent=\"Common\" content=\"ScreenDisplay\">"
                                "<agent>navi</agent>"
                                "<content>navi_address_confirm</content>"
                                "<detail>";
    strDisplay.append(m_strFirstSearchAddress);

    strDisplay.append("</detail>");
    strDisplay.append("</display>");

    VR_LOG("xml : %s", strDisplay.c_str());
    m_strPrevDisplay = m_strCurDisplay;
    m_strCurDisplay = strDisplay;
    m_engineCallback.OnRecognized(strDisplay);

    m_engineCallback.GetPromptByScreenId("VR-VDE-08", m_strPrompt);

    return true;
}

bool
VR_VoiceBoxAgentNavi::JumpScreenSetRoute()
{
    VR_LOGD_FUNC();

    std::string strEvent = "<send event=\"getAgentName\" currentAgent=\"";
    strEvent.append(m_strCurrAgent);
    strEvent.append("\" />");
    m_engineCallback.OnRecognized(strEvent);

    std::string strDisplay = "<display agent=\"Common\" content=\"ScreenDisplay\">"
                                "<agent>navi</agent>"
                                "<content>navi_set_route</content>"
                            "</display>";

    VR_LOG("xml : %s", strDisplay.c_str());

    m_strPrevDisplay = m_strCurDisplay;
    m_strCurDisplay = strDisplay;
    m_engineCallback.OnRecognized(strDisplay);

    m_engineCallback.GetPromptByScreenId("VR-DST-04", m_strPrompt);

    return true;
}

bool
VR_VoiceBoxAgentNavi::JumpScreenSetRouteConfirm(const std::string& strGuideType)
{
    VR_LOGD_FUNC();

    std::string strDisplay = "<display agent=\"Common\" content=\"ScreenDisplay\">"
                                "<agent>navi</agent>"
                                "<content>navi_set_route_confirm</content>"
                                "<detail>"
                                    "<displayGuideType>";
    strDisplay.append(strGuideType);
    strDisplay.append("</displayGuideType>");
    strDisplay.append("</detail>");
    strDisplay.append("</display>");

    VR_LOG("xml : %s", strDisplay.c_str());

    m_strPrevDisplay = m_strCurDisplay;
    m_strCurDisplay = strDisplay;
    m_engineCallback.OnRecognized(strDisplay);

    m_engineCallback.GetPromptByScreenId("VR-DST-51", m_strPrompt);

    return true;
}

bool
VR_VoiceBoxAgentNavi::JumpScreenSpeakPOI()
{
    VR_LOGD_FUNC();

    std::string strDisplay = "<display agent=\"Common\" content=\"ScreenDisplay\">"
                                "<agent>navi</agent>"
                                "<content>navi_speak_poi</content>"
                            "</display>";

    VR_LOG("xml : %s", strDisplay.c_str());

    m_strPrevDisplay = m_strCurDisplay;
    m_strCurDisplay = strDisplay;
    m_engineCallback.OnRecognized(strDisplay);

    m_engineCallback.GetPromptByScreenId("VR-POI-05", m_strPrompt);

    return true;
}

bool
VR_VoiceBoxAgentNavi::JumpScreenPOIList(const std::string& strScreen, IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    ListNodeInfo listNodeInfo;
    if (!GetListParameter(pcMsg, listNodeInfo)) {
        return false;
    }

    m_naviPOIInfo.clear();
    if ("Nearby" == m_strLocation) {
        m_naviPOIInfo = m_naviCurPointPOIInfo;
    }
    else if ("Along Route" == m_strLocation) {
        m_naviPOIInfo = m_naviAlongRoutePOIInfo;
    }
    else if ("Destination" == m_strLocation) {
        m_naviPOIInfo = m_naviNearDestPOIInfo;
    }
    else if ("City" == m_strLocation) {
        return false;
    }
    else {
        return false;
    }

    VR_LOG("screen content: %s", strScreen.c_str());

    std::string strStartIndex = GetStartIndex(pcMsg);
    std::string strListCount = GetActionParaByName(pcMsg, "List Count");
    VR_LOG("start index: %s", strStartIndex.c_str());
    VR_LOG("count: %s", strListCount.c_str());
    std::string strDisplay = "<display agent=\"Common\" content=\"ScreenDisplay\">"
                                "<agent>navi</agent>"
                                "<content>";
    strDisplay.append(strScreen);
    strDisplay.append("</content>");

    strDisplay.append("<selectList>");
    strDisplay.append("<list>");

    // header
    strDisplay.append("<header>");
    strDisplay.append("<pageSize>5</pageSize>");
    strDisplay.append("<startIndex>");
    strDisplay.append(strStartIndex);
    strDisplay.append("</startIndex>");
    strDisplay.append("<count>");
    strDisplay.append(strListCount);
    strDisplay.append("</count>");
    strDisplay.append("</header>");

    // items
    strDisplay.append("<items>");

    VoiceVector<NodeInfo>::const_iterator itor;
    VoiceMap<std::string, std::string>::const_iterator itorPoi;
    VoiceVector<VoiceVector<NodeInfo>::type>::const_iterator itorItem = listNodeInfo.vecItems.cbegin();
    for (; itorItem != listNodeInfo.vecItems.cend(); ++itorItem) {
        for (itor = itorItem->cbegin(); itor != itorItem->cend(); ++itor) {
            if ("POI ID" == itor->strName) {
                VR_LOG("poi id: %s", itor->strValue.c_str());
                itorPoi = m_naviPOIInfo.find(itor->strValue);
                if (itorPoi != m_naviPOIInfo.cend()) {
                    VR_LOG("find the poi");
                    strDisplay.append("<item>");
                    strDisplay.append(itorPoi->second);
                    strDisplay.append("</item>");
                }
                break;
            }
        }
    }

    strDisplay.append("</items>");

    strDisplay.append("</list>");
    strDisplay.append("</selectList>");
    strDisplay.append("</display>");

    m_strPrevDisplay = m_strCurDisplay;
    m_strCurDisplay = strDisplay;
    m_engineCallback.OnRecognized(strDisplay);

    return true;
}

bool
VR_VoiceBoxAgentNavi::JumpScreenPOIConfirm(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    std::string strPOIID = GetActionParaByName(pcMsg, "POI ID");
    VR_LOG("select poi id: %s", strPOIID.c_str());

    VoiceMap<std::string, std::string>::const_iterator iter = m_naviPOIInfo.find(strPOIID);
    std::string strPoi;

    if (iter != m_naviPOIInfo.cend()) {
        VR_LOG("find poi info");
        strPoi = iter->second;
    }
    else {
        VR_LOG("no poi!");
    }

    std::string strDisplay = "<display agent=\"Common\" content=\"ScreenDisplay\">"
                                "<agent>navi</agent>"
                                "<content>navi_poi_confirm</content>";
    strDisplay.append("<detail>");
    strDisplay.append(strPoi);
    strDisplay.append("</detail>");
    strDisplay.append("</display>");

    VR_LOG("xml : %s", strDisplay.c_str());

    m_strPrevDisplay = m_strCurDisplay;
    m_strCurDisplay = strDisplay;
    m_engineCallback.OnRecognized(strDisplay);

    m_engineCallback.GetPromptByScreenId("VR-POI-02", m_strPrompt);

    return true;
}

bool
VR_VoiceBoxAgentNavi::JumpScreenConfirmDestination(const std::string& strDestinationType)
{
    VR_LOGD_FUNC();

    std::string strDisplay = "<display agent=\"Common\" content=\"ScreenDisplay\">"
                                "<agent>navi</agent>"
                                "<content>navi_confirm_destination</content>";
    strDisplay.append("<type>");
    strDisplay.append(strDestinationType);
    strDisplay.append("</type");
    strDisplay.append("</display>");

    VR_LOG("xml : %s", strDisplay.c_str());

    m_strPrevDisplay = m_strCurDisplay;
    m_strCurDisplay = strDisplay;
    m_engineCallback.OnRecognized(strDisplay);

    m_engineCallback.GetPromptByScreenId("VR-DST-52", m_strPrompt);

    return true;
}

bool
VR_VoiceBoxAgentNavi::JumpScreenConfirmDelDestination(const std::string& strNameNode)
{
    VR_LOGD_FUNC();

    std::string strDisplay = "<display agent=\"Common\" content=\"ScreenDisplay\">"
                                "<agent>navi</agent>"
                                "<content>navi_confirm_delete_destination</content>";
    strDisplay.append(strNameNode);
    strDisplay.append("</display>");

    VR_LOG("xml : %s", strDisplay.c_str());

    m_engineCallback.GetPromptByScreenId("VR-DST-05", m_strPrompt);

    m_strPrevDisplay = m_strCurDisplay;
    m_strCurDisplay = strDisplay;
    m_engineCallback.OnRecognized(strDisplay);
    return true;
}

bool
VR_VoiceBoxAgentNavi::JumpScreenConfirmAssist()
{
    VR_LOGD_FUNC();

    std::string strDisplay = "<display agent=\"Common\" content=\"ScreenDisplay\">"
                                "<agent>navi</agent>"
                                "<content>navi_confirm_assist</content>"
                            "</display>";

    VR_LOG("xml : %s", strDisplay.c_str());

    m_strPrevDisplay = m_strCurDisplay;
    m_strCurDisplay = strDisplay;
    m_engineCallback.OnRecognized(strDisplay);

    m_engineCallback.GetPromptByScreenId("VR-HFD-06", m_strPrompt);

    return true;
}

bool
VR_VoiceBoxAgentNavi::JumpScreenDestinationSelect(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    if (NULL == pcMsg) {
        return false;
    }

    ListNodeInfo listNodeInfo;
    if (!GetListParameter(pcMsg, listNodeInfo)) {
        return false;
    }

    std::string strDisplay = "<display agent=\"Common\" content=\"ScreenDisplay\">"
                                "<agent>navi</agent>"
                                "<content>navi_destination_select</content>"
                                "<selectList>"
                                    "<list>"
                                         "<header>"
                                            "<pageSize>5</pageSize>";

    std::string strStartIndex = GetStartIndex(pcMsg);
    std::string strListCount = GetActionParaByName(pcMsg, "List Count");
    strDisplay.append("<startIndex>");
    strDisplay.append(strStartIndex);
    strDisplay.append("</startIndex>");

    strDisplay.append("<count>");
    strDisplay.append(strListCount);
    strDisplay.append("</count>");
    strDisplay.append("<header>");

    strDisplay.append("<items>");

    VoiceVector<NodeInfo>::const_iterator itor;
    VoiceMap<std::string, std::string>::const_iterator itorPoint;
    VoiceVector<VoiceVector<NodeInfo>::type>::const_iterator itorItem = listNodeInfo.vecItems.cbegin();
    for (; itorItem != listNodeInfo.vecItems.cend(); ++itorItem) {
        for (itor = itorItem->cbegin(); itor != itorItem->cend(); ++itor) {
            if ("ID" == itor->strName) {
                VR_LOG("drop point id: %s", itor->strValue.c_str());
                itorPoint = m_naviPoint.find(itor->strValue);
                if (itorPoint != m_naviPoint.cend()) {
                    VR_LOG("find the drop point");
                    strDisplay.append("<item>");
                    strDisplay.append(itorPoint->second);
                    strDisplay.append("</item>");
                }
                break;
            }
        }
    }

    strDisplay.append("</items>");
    strDisplay.append("</list>");
    strDisplay.append("</selectList>");
    strDisplay.append("</display>");
    VR_LOG("xml : %s", strDisplay.c_str());

    m_engineCallback.GetPromptByScreenId("VR-DST-03", m_strPrompt);

    m_strPrevDisplay = m_strCurDisplay;
    m_strCurDisplay = strDisplay;
    m_engineCallback.OnRecognized(strDisplay);
    return true;
}

bool
VR_VoiceBoxAgentNavi::JumpScreenDestDelSelect(IVECIParsedMessage *pcMsg)
{
    VR_LOGD_FUNC();

    m_vecDropPoint.clear();
    if (NULL == pcMsg) {
        return false;
    }

    ListNodeInfo listNodeInfo;
    if (!GetListParameter(pcMsg, listNodeInfo)) {
        return false;
    }

    std::string strDisplay = "<display agent=\"Common\" content=\"ScreenDisplay\">"
                                "<agent>navi</agent>"
                                "<content>navi_destination_delete_select</content>"
                                "<selectList>"
                                    "<list>"
                                         "<header>"
                                            "<pageSize>5</pageSize>";

    std::string strStartIndex = GetStartIndex(pcMsg);
    std::string strListCount = GetActionParaByName(pcMsg, "List Count");
    strDisplay.append("<startIndex>");
    strDisplay.append(strStartIndex);
    strDisplay.append("</startIndex>");

    strDisplay.append("<count>");
    strDisplay.append(strListCount);
    strDisplay.append("</count>");
    strDisplay.append("<header>");

    strDisplay.append("<items>");

    VoiceVector<NodeInfo>::const_iterator itor;
    VoiceMap<std::string, std::string>::const_iterator itorDropPoint;
    VoiceVector<VoiceVector<NodeInfo>::type>::const_iterator itorItem = listNodeInfo.vecItems.cbegin();
    for (; itorItem != listNodeInfo.vecItems.cend(); ++itorItem) {
        for (itor = itorItem->cbegin(); itor != itorItem->cend(); ++itor) {
            if ("ID" == itor->strName) {
                VR_LOG("drop point id: %s", itor->strValue.c_str());
                itorDropPoint = m_naviDropPoint.find(itor->strValue);
                if (itorDropPoint != m_naviDropPoint.cend()) {
                    VR_LOG("find the drop point");
                    strDisplay.append("<item>");
                    strDisplay.append(itorDropPoint->second);
                    strDisplay.append("</item>");
                    m_vecDropPoint.push_back(itorDropPoint->second);
                }
                break;
            }
        }
    }

    strDisplay.append("</items>");
    strDisplay.append("</list>");
    strDisplay.append("</selectList>");
    strDisplay.append("</display>");
    VR_LOG("xml : %s", strDisplay.c_str());

    m_engineCallback.GetPromptByScreenId("VR-DST-06", m_strPrompt);

    m_strPrevDisplay = m_strCurDisplay;
    m_strCurDisplay = strDisplay;
    m_engineCallback.OnRecognized(strDisplay);
    return true;
}

bool
VR_VoiceBoxAgentNavi::JumpScreenNBest(const VoiceVector<std::string>::type& vecAddress)
{
    VR_LOGD_FUNC();
    VR_LOGP("DE: query navi address info data end....nbest data..., will jump screen......case : 212-120 212-126");

    std::string strDisplay = "<display agent=\"Common\" content=\"ScreenDisplay\">"
                                "<agent>navi</agent>"
                                "<content>navi_destination_select</content>"
                                "<selectList>"
                                    "<list>"
                                         "<header>"
                                            "<pageSize>5</pageSize>"
                                            "<startIndex>0</startIndex>";

    int iCount = (vecAddress.size() > 5) ? 5 : vecAddress.size();
    strDisplay.append("<count>");
    strDisplay.append(std::to_string(iCount));
    strDisplay.append("</count>");

    strDisplay.append("<items>");

    for (int iIndex = 0; iIndex < iCount; ++iIndex) {
        strDisplay.append("<item>");
        strDisplay.append(vecAddress.at(iIndex));
        strDisplay.append("</item>");
    }

    strDisplay.append("</items></list></display>");
    VR_LOG("XML : %s", strDisplay.c_str());
    m_strPrevDisplay = m_strCurDisplay;
    m_strCurDisplay = strDisplay;
    m_engineCallback.OnRecognized(strDisplay);
    return true;
}

std::string
VR_VoiceBoxAgentNavi::GetAddress(const std::string& strKey)
{
    VR_LOGD_FUNC();

    VR_VoiceBoxXmlParser parser(m_strAddress);
    std::string strValue = parser.getValueByKey(strKey);

    return strValue;
}

void
VR_VoiceBoxAgentNavi::CalculateRoute(const std::string& strCalculateType)
{
    VR_LOGD_FUNC();

    std::string strAction = "<action agent=\"navi\" op=\"calculateRoute\">"
                                "<calculateType>";
    strAction.append(strCalculateType);
    strAction.append("</calculateType>");
    strAction.append(m_strRouteGuide);

    strAction.append("</action>");

    m_engineCallback.OnRecognized(strAction);
}

bool
VR_VoiceBoxAgentNavi::ChangeNodeValueInXML(std::string& strxml, const std::string& strNodeName, const std::string& strDestValue)
{
    VR_LOGD_FUNC();
    pugi::xml_document xmlDocument;
    if (!xmlDocument.load(strxml.c_str())) {
        VR_LOG("xml load fail !!");
        return false;
    }
    pugi::xml_node node = xmlDocument.first_child().child(strNodeName.c_str());
    node.text().set(strDestValue.c_str());

    std::stringstream oss;
    xmlDocument.first_child().print(oss);
    strxml = oss.str();
    VR_LOG("xml : %s", strxml.c_str());
    return true;
}

/* EOF */
