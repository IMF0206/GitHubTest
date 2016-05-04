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
 * @file VR_DialogEngineFactory.h
 * @brief interface for DialogEngine Factory.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_DIALOGENGINEFACTORY_H
#define VR_DIALOGENGINEFACTORY_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

class VR_DialogEngineIF;

/**
 * @brief The VR_DialogEngineFactory class
 *
 * class declaration
 */
class VR_DialogEngineFactory
{
public:
    static VR_DialogEngineIF* CreateDialogEngine(bool mock = false);
    static bool LoadAllLibraries();
};

#endif // VR_DIALOGENGINEFACTORY_H
/* EOF */
