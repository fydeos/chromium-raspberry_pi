// Copyright (c) 2018 The FlintOS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Author: Simon Tsao(yang@flintos.io)

#include "fydeos/extensions/browser/api/app_management/app_management_api.h"

#include <stddef.h>
#include <algorithm>
#include <limits>
#include <utility>
#include <vector>

#include "chrome/browser/ui/app_list/arc/arc_app_list_prefs.h"
#include "chrome/browser/profiles/profile.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/common/extension_set.h"
#include "fydeos/extensions/common/api/app_management.h"
#include "chrome/browser/chromeos/arc/arc_util.h"

namespace extensions{
  const char kAppIdName[] = "appId";
  const char kAppTypeName[] = "appType";
  //const char kPackageName[] = "package";
  const char kWebApp[] = "web_app";
  const char kArcApp[] = "arc_app";
  namespace {
    std::vector<std::string> GetArcAppIdList(content::BrowserContext* context) {
      return ArcAppListPrefs::Get(context)->GetAppIds();
    }

    ExtensionIdSet GetExtensionIdSet(content::BrowserContext* context) {
      return extensions::ExtensionRegistry::Get(context)->enabled_extensions().GetIDs();
    }

    std::unique_ptr<base::DictionaryValue> CreateWebAppInfo(std::string app_id) {
      std::unique_ptr<base::DictionaryValue> result(new base::DictionaryValue());
      result->SetString(kAppIdName, app_id);
      result->SetString(kAppTypeName, kWebApp);
      return result;
    }

    std::unique_ptr<base::DictionaryValue> CreateArcAppInfo(std::string app_id) {
      std::unique_ptr<base::DictionaryValue> result(new base::DictionaryValue());
      result->SetString(kAppIdName, app_id);
      result->SetString(kAppTypeName, kArcApp);
      return result;
    }

  } //exit internel namespace

  ExtensionFunction::ResponseAction AppManagementGetAppListFunction::Run() {
    Profile* profile = Profile::FromBrowserContext(browser_context());
    std::unique_ptr<base::ListValue> app_list(new base::ListValue());
    for(std::string app_id: GetExtensionIdSet(browser_context()))
      app_list->Append(CreateWebAppInfo(app_id));
    if (arc::IsArcAllowedForProfile(profile))
      for(std::string app_id: GetArcAppIdList(browser_context()))
        app_list->Append(CreateArcAppInfo(app_id));
    return RespondNow(OneArgument(std::move(app_list)));
  }

  ExtensionFunction::ResponseAction AppManagementInstallWebAppFunction::Run() {
    return RespondNow(NoArguments());
  }

  ExtensionFunction::ResponseAction AppManagementInstallArcAppFunction::Run() {
    return RespondNow(NoArguments());
  }
} //exit namespace extensions
