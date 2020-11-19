// Copyright (c) 2019 The FydeOS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "fydeos/switches/fydeos_switches.h"

#include <string>

#include "base/command_line.h"
#include "base/metrics/field_trial.h"
#include "third_party/icu/source/common/unicode/locid.h"
#include "extensions/common/extension_urls.h"
#include "chrome/common/chrome_switches.h"
#include "fydeos/switches/fydeos_constants.h"

namespace fydeos {
namespace switches {
namespace {
const char kFlintAccountEnable[] = "flint-account-enabled";
const char kFydemina[] = "fydemina";
const char kFydeOSArcDelay[] ="fydeos-arc-delay";
const char kFydeAccountEnable[] = "fyde-account-enabled";
const char kAccelConfigIndex[] = "fydeos-accel-config";
const char kAccelRightMoveBits[] = "fydeos-accel-right-move";
const char kAccelRevertX[] = "fydeos-accel-revert-x";
const char kAccelRevertY[] = "fydeos-accel-revert-y";
const char kAccelRevertZ[] = "fydeos-accel-revert-z";
const char kPolicyManagedByFyde[] = "policy-managed-by-fyde";
const char kResetWifiDriver[] = "fydeos-reset-wifi-driver";
const char kAccelDataPattern[] = "fydeos-accel-pattern";
const char kRotate_90[] = "fydeos-rotate-90";
const char kRotate_180[] = "fydeos-rotate-180";
const char kRotate_270[] = "fydeos-rotate-270";
const char kScreenDpi[] = "fydeos-default-screen-dpi";
const char kDefaultDSF[] = "fydeos-default-DSF";
const char KForceCursorCompositing[] = "fydeos-force-cursor-compositing";
} //end of namespace

const char kFydeOSGaiaUrl[] = "fydeos-gaia-url";
const char kFydeOSApisUrl[] = "fydeos-apis-url";
const char kFydeOSDeviceManagementUrl[] = "fydeos-device-management-url";
const char kFydeOSSyncServiceURL[] = "fydeos-sync-url";

bool IsFlintAccountEnabled() {
    return base::CommandLine::ForCurrentProcess()->HasSwitch(kFlintAccountEnable);
}

bool IsFydeAccountEnabled() {
    return base::CommandLine::ForCurrentProcess()->HasSwitch(kFydeAccountEnable);
}

bool IsFydeExtendAccountEnabled() {
    return IsFydeAccountEnabled() || IsFlintAccountEnabled();
}

int64_t GetFydeOSArcDelay() {
    std::string delayStr = base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(kFydeOSArcDelay);
    if (delayStr.empty())
        return 0;
    return (int64_t) std::stoi(delayStr);
}

bool IsFydeminaEnabled() {
    return base::CommandLine::ForCurrentProcess()->HasSwitch(kFydemina);
}

int GetAccelConfig() {
  std::string indexStr = base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(kAccelConfigIndex);
  if (indexStr.empty())
    return 0;
  return std::stoi(indexStr);
}

bool IsAccelRevertX() {
  return base::CommandLine::ForCurrentProcess()->HasSwitch(kAccelRevertX);
}

bool IsAccelRevertY() {
  return base::CommandLine::ForCurrentProcess()->HasSwitch(kAccelRevertY);
}

bool IsAccelRevertZ() {
  return base::CommandLine::ForCurrentProcess()->HasSwitch(kAccelRevertZ);
}

bool IsAccelRightMove() {
  return base::CommandLine::ForCurrentProcess()->HasSwitch(kAccelRightMoveBits);
}

bool IsFydeOSAccelerometer() {
  return base::CommandLine::ForCurrentProcess()->HasSwitch(kAccelConfigIndex);  
}

int GetAccelRightMoveBits() {
  int ret = 0;
  if (IsAccelRightMove()) {
      ret = 4;
      std::string rightBitsStr = base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(kAccelRightMoveBits);
      if (!rightBitsStr.empty())
          ret = std::stoi(rightBitsStr);
  }
  return ret;
}

bool IsPolicyManagedByFyde() {
  return base::CommandLine::ForCurrentProcess()->HasSwitch(kPolicyManagedByFyde);
}

bool HasAppStoreURL() {
  return base::CommandLine::ForCurrentProcess()->HasSwitch(::switches::kAppsGalleryURL);  
}

bool NeedResetWifiDriver() {
  return base::CommandLine::ForCurrentProcess()->HasSwitch(kResetWifiDriver);
}

bool IsRotate_90(){
  return base::CommandLine::ForCurrentProcess()->HasSwitch(kRotate_90);
}

bool IsRotate_180(){
  return base::CommandLine::ForCurrentProcess()->HasSwitch(kRotate_180);
}

bool IsRotate_270(){
  return base::CommandLine::ForCurrentProcess()->HasSwitch(kRotate_270);
}

std::string GetAppStoreURL() {
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  if (command_line->HasSwitch(::switches::kAppsGalleryURL))
    return command_line->GetSwitchValueASCII(::switches::kAppsGalleryURL); 
  else
    return std::string(fydeos::constants::kFydeOSStoreBaseUrl); 
}

int GetAccelDataPattern() {
  std::string patternStr = base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(kAccelDataPattern);
  if (patternStr.empty())
    return 0;
  return std::stoi(patternStr);
}

float GetDefaultScreenDpi(float default_value) {
  std::string factorStr = base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(kScreenDpi);
  if (factorStr.empty())
    return default_value;
  return std::stof(factorStr);
}

float GetDefaultDSF(float default_value) {
  std::string factorStr = base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(kDefaultDSF);
  if (factorStr.empty())
    return default_value;
  return std::stof(factorStr);  
}

bool ForceCursorCompositing() {
  return base::CommandLine::ForCurrentProcess()->HasSwitch(KForceCursorCompositing);
}

}// namespace switches
}// namespace fydeos
