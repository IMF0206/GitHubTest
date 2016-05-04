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
#ifndef VR_VOICEBOXSYNCOBJECT_H
#    include "VR_VoiceBoxSyncObject.h"
#endif

#ifndef CXX_BL_SyncObject_H
#   include "BL_SyncObject.h"
#endif

#ifndef VR_DEF_H
#    include "VR_Def.h"
#endif

// Constructor
VR_VoiceBoxSyncObject::VR_VoiceBoxSyncObject()
: m_pSyncObject(NULL)
{
    m_pSyncObject = VR_new BL_SyncObject;
}

// Destructor
VR_VoiceBoxSyncObject::~VR_VoiceBoxSyncObject()
{
    delete m_pSyncObject;
    m_pSyncObject = NULL;
}

void
VR_VoiceBoxSyncObject::SynchronizeStart()
{
    if (NULL != m_pSyncObject) {
        m_pSyncObject->SynchronizeStart();
    }
}

void
VR_VoiceBoxSyncObject::SynchronizeEnd()
{
    if (NULL != m_pSyncObject) {
        m_pSyncObject->SynchronizeEnd();
    }
}
/* EOF */
