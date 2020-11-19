// Copyright (c) 2019 The FydeOS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROMEOS_FYDEOS_SWITCHES_H_
#define CHROMEOS_FYDEOS_SWITCHES_H_

#include <string>
#include "base/feature_list.h"
#include "chromeos/chromeos_export.h"
#include "ash/ash_export.h"
#include "ui/display/manager/display_manager_export.h"

namespace fydeos {
namespace switches {
CHROMEOS_EXPORT bool IsFlintAccountEnabled();
CHROMEOS_EXPORT bool IsFydeminaEnabled();
CHROMEOS_EXPORT int64_t GetFydeOSArcDelay();
CHROMEOS_EXPORT bool IsFydeExtendAccountEnabled();
CHROMEOS_EXPORT bool IsFydeAccountEnabled();
CHROMEOS_EXPORT int GetAccelConfig();
CHROMEOS_EXPORT int GetAccelRightMoveBits();
CHROMEOS_EXPORT bool IsAccelRevertX();
CHROMEOS_EXPORT bool IsAccelRevertY();
CHROMEOS_EXPORT bool IsAccelRevertZ();
CHROMEOS_EXPORT bool IsAccelRightMove();
CHROMEOS_EXPORT bool IsFydeOSAccelerometer();
CHROMEOS_EXPORT bool IsPolicyManagedByFyde();

CHROMEOS_EXPORT extern const char kFydeOSGaiaUrl[];
CHROMEOS_EXPORT extern const char kFydeOSApisUrl[];
CHROMEOS_EXPORT extern const char kFydeOSDeviceManagementUrl[];
CHROMEOS_EXPORT extern const char kFydeOSSyncServiceURL[];
CHROMEOS_EXPORT bool NeedResetWifiDriver();

CHROMEOS_EXPORT bool IsRotate_90();
CHROMEOS_EXPORT bool IsRotate_180();
CHROMEOS_EXPORT bool IsRotate_270();

extern bool HasAppStoreURL();
extern std::string GetAppStoreURL();

CHROMEOS_EXPORT int GetAccelDataPattern();

CHROMEOS_EXPORT float GetDefaultScreenDpi(float default_value);
CHROMEOS_EXPORT float GetDefaultDSF(float default_value);
CHROMEOS_EXPORT bool ForceCursorCompositing();

}
}

#endif
