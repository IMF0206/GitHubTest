/**
 * Copyright @ 2014 - 2017 Suntec Software(Shanghai) Co., Ltd.
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
 * @file VR_VoiceBoxSyncObject.h
 * @brief Declaration file of log macro.
 *
 * This file includes the declaration of sync object.
 *
 * @attention used on *.cpp only.
 */

#ifndef VR_VOICEBOXSYNCOBJECT_H
#define VR_VOICEBOXSYNCOBJECT_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

/* Forward Declaration */
class BL_SyncObject;

/**
 * @brief The VR_VoiceBoxSyncObject class
 *
 * class declaration
 */
class VR_VoiceBoxSyncObject
{
public:
    VR_VoiceBoxSyncObject();
    ~VR_VoiceBoxSyncObject();

    void SynchronizeStart();
    void SynchronizeEnd();

private:
    BL_SyncObject* m_pSyncObject;
};

#endif /* VR_VOICEBOXSYNCOBJECT_H */
/* EOF */
