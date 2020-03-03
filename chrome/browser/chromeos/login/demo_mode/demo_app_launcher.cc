// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/chromeos/login/demo_mode/demo_app_launcher.h"

#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/logging.h"
#include "chrome/browser/apps/app_service/app_launch_params.h"
#include "chrome/browser/apps/app_service/app_service_proxy.h"
#include "chrome/browser/apps/app_service/app_service_proxy_factory.h"
#include "chrome/browser/apps/app_service/browser_app_launcher.h"
#include "chrome/browser/chromeos/app_mode/kiosk_app_manager.h"
#include "chrome/browser/chromeos/app_mode/kiosk_app_types.h"
#include "chrome/browser/chromeos/login/ui/login_display_host.h"
#include "chrome/browser/extensions/component_loader.h"
#include "chrome/browser/extensions/extension_service.h"
#include "chrome/browser/lifetime/application_lifetime.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/common/chrome_switches.h"
#include "chrome/common/extensions/extension_constants.h"
#include "chrome/grit/browser_resources.h"
#include "chromeos/network/network_handler.h"
#include "chromeos/network/network_state_handler.h"
#include "components/account_id/account_id.h"
#include "components/session_manager/core/session_manager.h"
#include "components/user_manager/user_names.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/browser/extension_system.h"
#include "extensions/common/constants.h"
#include "extensions/common/extension.h"
#include "ui/base/window_open_disposition.h"
//---***FYDEOS BEGIN***---
#include "base/lazy_instance.h"
#include "base/files/file_util.h"
#include "base/json/json_file_value_serializer.h"
//---***FYDEOS END***---

namespace chromeos {

const char DemoAppLauncher::kDemoAppId[] = "klimoghijjogocdbaikffefjfcfheiel";
const base::FilePath::CharType kDefaultDemoAppPath[] =
    FILE_PATH_LITERAL("/usr/share/chromeos-assets/demo_app");

//---***FYDEOS BEGIN***---
namespace {
const base::FilePath::CharType kDemoBasePath[] =
    FILE_PATH_LITERAL("/usr/local/share/kiosk_app");
const base::FilePath::CharType kDemoConfig[] =
    FILE_PATH_LITERAL("config.json");
const char kAppId[] = "AppId";
const char kAppPath[] = "AppPath";
const char kEnable[] = "Enable";
const base::FilePath::CharType kDefaultManifest[] =
    FILE_PATH_LITERAL("manifest.json");
} // namespace
//---***FYDEOS END***---
// static
base::FilePath* DemoAppLauncher::demo_app_path_ = NULL;
//---***FYDEOS BEGIN***---
static base::LazyInstance<DemoAppLauncher>::DestructorAtExit instance =
    LAZY_INSTANCE_INITIALIZER;
DemoAppLauncher* DemoAppLauncher::Get() {
    return instance.Pointer();
}
//---***FYDEOS END***---

DemoAppLauncher::DemoAppLauncher() {
  //---***FYDEOS BEGIN***---
  ConfigDemo();
  //---***FYDEOS END***---
  if (!demo_app_path_) {
    demo_app_path_ = new base::FilePath(kDefaultDemoAppPath);
    app_id_ = std::string(kDemoAppId);
  }
}

DemoAppLauncher::~DemoAppLauncher() {
  delete demo_app_path_;
}

void DemoAppLauncher::StartDemoAppLaunch() {
  DVLOG(1) << "Launching demo app...";
  // user_id = DemoAppUserId, force_emphemeral = true, delegate = this.
  kiosk_profile_loader_.reset(new KioskProfileLoader(
      user_manager::DemoAccountId(), KioskAppType::CHROME_APP, true, this));
  kiosk_profile_loader_->Start();
}

// static
bool DemoAppLauncher::IsDemoAppSession(const AccountId& account_id) {
  return account_id == user_manager::DemoAccountId();
}

// static
void DemoAppLauncher::SetDemoAppPathForTesting(const base::FilePath& path) {
  delete demo_app_path_;
  demo_app_path_ = new base::FilePath(path);
}

void DemoAppLauncher::OnProfileLoaded(Profile* profile) {
  DVLOG(1) << "Profile loaded... Starting demo app launch.";

  kiosk_profile_loader_.reset();

  // Load our demo app, then launch it.
  extensions::ExtensionService* extension_service =
      extensions::ExtensionSystem::Get(profile)->extension_service();
  CHECK(demo_app_path_);
  //---***FYDEOS BEGIN***---
  std::string manifest;
  if (kiosk_mode_) {
    base::FilePath manifest_path = demo_app_path_->Append(kDefaultManifest);
    if (!base::PathExists(manifest_path)) {
      VLOG(1) << "No found " << manifest_path;
      chrome::AttemptUserExit();
      return;
    }
    if (!base::ReadFileToString(manifest_path, &manifest)) {
      VLOG(1) << "Read error of: " << manifest_path;
      chrome::AttemptUserExit();
      return;
    }
  }
  const std::string extension_id = kiosk_mode_ ? 
      extension_service->component_loader()->Add(manifest, *demo_app_path_, true) :
			extension_service->component_loader()->Add(IDR_DEMO_APP_MANIFEST, *demo_app_path_);
  //---***FYDEOS END***---

  extensions::ExtensionRegistry* extension_registry =
      extensions::ExtensionRegistry::Get(profile);
  const extensions::Extension* extension =
      extension_registry->enabled_extensions().GetByID(extension_id);
  if (!extension) {
    // We've already done too much setup at this point to just return out, it
    // is safer to just restart.
//---***FYDEOS BEGIN***---
    VLOG(1) << "No extension found:" << extension_id ;
//---***FYDEOS END***---
    chrome::AttemptUserExit();
    return;
  }

  // Disable network before launching the app.
/*
  LOG(WARNING) << "Disabling network before launching demo app..";
  NetworkHandler::Get()->network_state_handler()->SetTechnologyEnabled(
      NetworkTypePattern::Physical(), false,
      chromeos::network_handler::ErrorCallback());
*/

  apps::AppServiceProxyFactory::GetForProfile(profile)
      ->BrowserAppLauncher()
      ->LaunchAppWithParams(apps::AppLaunchParams(
          extension_id, apps::mojom::LaunchContainer::kLaunchContainerWindow,
          WindowOpenDisposition::NEW_WINDOW,
          apps::mojom::AppLaunchSource::kSourceChromeInternal, true));
  KioskAppManager::Get()->InitSession(profile, extension_id);

  session_manager::SessionManager::Get()->SessionStarted();

  LoginDisplayHost::default_host()->Finalize(base::OnceClosure());
}

void DemoAppLauncher::OnProfileLoadFailed(KioskAppLaunchError::Error error) {
  LOG(ERROR) << "Loading the Kiosk Profile failed: "
             << KioskAppLaunchError::GetErrorMessage(error);
}

void DemoAppLauncher::OnOldEncryptionDetected(const UserContext& user_context) {
  NOTREACHED();
}

//---***FYDEOS BEGIN***---
void DemoAppLauncher::ConfigDemo() {
  kiosk_mode_ = false;
  base::FilePath base_path(kDemoBasePath);
  base::FilePath config_path = base_path.Append(kDemoConfig);
  if (!base::PathExists(config_path))
    return;
  JSONFileValueDeserializer deserializer(config_path);
  std::unique_ptr<base::DictionaryValue> config =
    base::DictionaryValue::From(deserializer.Deserialize(nullptr, nullptr));
  if (!config)
    return;
  if (!config->GetString(kAppId, &app_id_))
    return;
  std::string app_rel_path;
  if (!config->GetString(kAppPath, &app_rel_path))
    return;
   base::FilePath app_path = base_path.Append(app_rel_path);
  if (!base::PathExists(app_path))
    return;
  if (!config->GetBoolean(kEnable, &kiosk_mode_))
    return;
  if (kiosk_mode_)
    demo_app_path_ = new base::FilePath(app_path);
}
//---***FYDEOS END***---

}  // namespace chromeos
