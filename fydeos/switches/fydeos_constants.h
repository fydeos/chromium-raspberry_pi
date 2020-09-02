// Copyright (c) 2019 The FydeOS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROMEOS_FYDEOS_CONSTANTS_H_
#define CHROMEOS_FYDEOS_CONSTANTS_H_

#include "base/feature_list.h"
#include "chromeos/chromeos_export.h"

namespace fydeos {
namespace constants {
CHROMEOS_EXPORT extern const char kDefaultTestUrl[]; //used by chrome/browser/chromeos/net/network_portal_detector_impl.cc
CHROMEOS_EXPORT extern const char kFydeOSStoreBaseUrl[];
CHROMEOS_EXPORT extern const char kFydeOSHomePageUrl[];
CHROMEOS_EXPORT extern const uint8_t kFydeOSPolicyVerificationKey[];

CHROMEOS_EXPORT extern const char kDefaultFydeOSGaiaUrl[];
CHROMEOS_EXPORT extern const char kDefaultFydeOSApisBaseUrl[];
CHROMEOS_EXPORT extern const char kDefaultFydeOSDeviceManagementServerUrl[];
CHROMEOS_EXPORT extern const char kFydeOSSyncDevServerUrl[];
CHROMEOS_EXPORT extern const char kFydeOSSyncServerUrl[];
CHROMEOS_EXPORT extern const char kFydeOSDefaultTimeZoneId[];
CHROMEOS_EXPORT extern const char kFydeOSReloadWifiCmd[];

extern const char kFydeOSBetaForumURL[];
extern const char kFydeOSHelpURL[];
extern const char kMultiDeviceLearnMoreURL[];
extern const char kLanguageSettingsLearnMoreUrl[];
extern const char kLinuxAppsLearnMoreURL[];
extern const char kOnlineEulaURLPath[];
extern const char kFydeAccessibilityHelpURL[];
extern const char kSmbSharesLearnMoreURL[];
extern const char kCupsPrintLearnMoreURL[];
extern const char kNaturalScrollHelpURL[];

}//constants
}//fydeos

#endif
