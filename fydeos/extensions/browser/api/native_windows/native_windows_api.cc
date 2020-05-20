// Copyright (c) 2018 The FlintOS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Author: Simon Tsao(yang@flintos.io)

#include "fydeos/extensions/browser/api/native_windows/native_windows_api.h"

#include <stddef.h>
#include <algorithm>
#include <limits>
#include <utility>
#include <vector>
#include "chrome/browser/extensions/api/tabs/tabs_constants.h"
#include "fydeos/extensions/common/api/native_windows.h"
#include "chrome/common/extensions/extension_constants.h"
#include "ash/public/cpp/window_pin_type.h"
#include "ash/public/cpp/window_properties.h"
#include "chrome/browser/ui/browser_command_controller.h"
#include "ui/aura/window.h"
#include "ash/shell.h"
#include "ash/public/cpp/shelf_types.h"
#include "ash/public/cpp/shell_window_ids.h"
#include "ash/wm/window_cycle_controller.h"
#include "ash/wm/window_cycle_list.h"
#include "ash/wm/window_state.h"
#include "ash/wm/window_util.h"
#include "ash/wm/window_state_util.h"
#include "ash/wm/mru_window_tracker.h"
#include "ash/wm/wm_event.h"
#include "components/arc/arc_util.h"
#include "chrome/browser/ui/ash/launcher/arc_app_window_launcher_controller.h"
#include "chrome/browser/ui/app_list/arc/arc_app_utils.h"
#include "chrome/browser/ui/ash/launcher/chrome_launcher_controller.h"


namespace extensions{
  using WindowList = ash::WindowCycleList::WindowList;
  namespace windows = api::native_windows;
  using Window = aura::Window;
  using WindowState = ash::WindowState;
  namespace keys = tabs_constants;
  using ShelfID = ash::ShelfID;

  const char kAppIdKey[] = "appId";
  const char kLaunchIdKey[] = "launchId";
  const char kAppIdIsEmptyError[] = "appId is empty.";

  namespace {
    int baseWindowId = ash::kShellWindowId_PhantomWindow + 2;
    void ArrangementWindowsIDs(WindowList window_list) {
      std::vector<Window*> need_id_list;
      for (Window * aura_window : window_list) {
        if (aura_window->id() == ash::kShellWindowId_Invalid)
          need_id_list.push_back(aura_window);
        else if (aura_window->id() >= baseWindowId)
          baseWindowId = aura_window->id() + 1;
      }
      for (Window * aura_window: need_id_list)
        aura_window->set_id(baseWindowId++);
    }

    WindowList BuildWindowsForCycleList() {
      WindowList window_list =
          ash::Shell::Get()->mru_window_tracker()->BuildMruWindowList(ash::kAllDesks);
      auto window_is_ineligible = [](Window* window) {
        WindowState* state = WindowState::Get(window);
        return !state->IsUserPositionable() || state->is_dragged() ||
                window->GetRootWindow()
                    ->GetChildById(ash::kShellWindowId_AppListContainer)
                    ->Contains(window) ||
                window->GetProperty(ash::kHideInOverviewKey);
      };
      window_list.erase(std::remove_if(window_list.begin(), window_list.end(),
                                        window_is_ineligible),
                        window_list.end());
      ArrangementWindowsIDs(window_list);
      return window_list;
    }

    std::unique_ptr<base::DictionaryValue> CreateWindowValueFromNativeWindow(Window* aura_window)
    {
      std::unique_ptr<base::DictionaryValue> result(new base::DictionaryValue());
      result->SetInteger(keys::kIdKey, aura_window->id());
      result->SetString(keys::kWindowTypeKey, keys::kWindowTypeValueApp);
      ShelfID shelfId = ShelfID::Deserialize(aura_window->GetProperty(ash::kShelfIDKey));
      if (!shelfId.IsNull()) {
        result->SetString(kAppIdKey, shelfId.app_id);
        result->SetString(kLaunchIdKey, shelfId.launch_id);
      }
      std::string window_state;
      WindowState* aura_window_state = WindowState::Get(aura_window);
      if (aura_window_state->IsMinimized()) {
        window_state = keys::kShowStateValueMinimized;
      } else if (aura_window_state->IsFullscreen()) {
        window_state = keys::kShowStateValueFullscreen;
        if (aura_window_state->IsPinned() || aura_window_state->IsTrustedPinned())
          window_state = keys::kShowStateValueLockedFullscreen;
      } else if (aura_window_state->IsMaximized()) {
        window_state = keys::kShowStateValueMaximized;
      } else {
        window_state = keys::kShowStateValueNormal;
      }
      result->SetString(keys::kShowStateKey, window_state);
      result->SetBoolean(keys::kFocusedKey, aura_window_state->IsActive());
      gfx::Rect bounds = aura_window->GetBoundsInScreen();
      result->SetInteger(keys::kLeftKey, bounds.x());
      result->SetInteger(keys::kTopKey, bounds.y());
      result->SetInteger(keys::kWidthKey, bounds.width());
      result->SetInteger(keys::kHeightKey, bounds.height());

      return result;
    }

    bool GetWindowFromWindowID(int window_id, Window** window) {
      WindowList window_list = BuildWindowsForCycleList();
      for (Window * tmp_window : window_list){
        if (tmp_window->id() == window_id){
          *window = tmp_window;
          break;
        }
      }
      return *window;
    }

  } // exit internel namespace

  ExtensionFunction::ResponseAction NativeWindowsGetFunction::Run() {
    std::unique_ptr<windows::Get::Params> params(
        windows::Get::Params::Create(*args_));
    EXTENSION_FUNCTION_VALIDATE(params.get());
    Window* window = NULL;
    if (!GetWindowFromWindowID(params->window_id,
                                              &window)) {
      return RespondNow(Error(keys::kNoCurrentWindowError));
    }
    return RespondNow(OneArgument(CreateWindowValueFromNativeWindow(window)));
  }

  ExtensionFunction::ResponseAction NativeWindowsGetAllFunction::Run() {

    std::unique_ptr<base::ListValue> window_list(new base::ListValue());
    for(Window* tmp_window: BuildWindowsForCycleList())
      window_list->Append(CreateWindowValueFromNativeWindow(tmp_window));
    return RespondNow(OneArgument(std::move(window_list)));
  }


  ExtensionFunction::ResponseAction NativeWindowsUpdateFunction::Run() {
    std::unique_ptr<windows::Update::Params> params(
        windows::Update::Params::Create(*args_));
    EXTENSION_FUNCTION_VALIDATE(params);

    Window* target_window = NULL;
    if (!GetWindowFromWindowID(params->window_id, &target_window))
      return RespondNow(Error(keys::kNoCurrentWindowError));
    WindowState* target_window_state = WindowState::Get(target_window);
    if (params->update_info.state != windows::WINDOW_STATE_LOCKED_FULLSCREEN &&
        params->update_info.state != windows::WINDOW_STATE_NONE) {
      target_window_state->Restore();
    } else if (params->update_info.state ==
                  windows::WINDOW_STATE_LOCKED_FULLSCREEN) {
      if (!target_window_state->IsMaximizedOrFullscreenOrPinned()){
        const ash::WMEvent event(ash::WM_EVENT_TOGGLE_FULLSCREEN);
        target_window_state->OnWMEvent(&event);
      }
      ash::window_util::PinWindow(target_window, true);
    }
    return RespondNow(OneArgument(CreateWindowValueFromNativeWindow(target_window)));
  }

  ExtensionFunction::ResponseAction NativeWindowsRemoveFunction::Run() {
    std::unique_ptr<windows::Remove::Params> params(
      windows::Remove::Params::Create(*args_));
    EXTENSION_FUNCTION_VALIDATE(params);
    Window* target_window = NULL;
    if (!GetWindowFromWindowID(params->window_id, &target_window))
      return RespondNow(Error(keys::kNoCurrentWindowError));

    if (arc::IsArcAppWindow(target_window)){
      int task_id = arc::GetWindowTaskId(target_window);
      if (task_id == -1)
        return RespondNow(Error(keys::kNoCurrentWindowError));
      arc::CloseTask(task_id);
    }else{
      ash::window_util::CloseWidgetForWindow(target_window);
    }
    return RespondNow(NoArguments());
  }

  ExtensionFunction::ResponseAction NativeWindowsCreateFunction::Run() {
    std::unique_ptr<windows::Create::Params> params(
      windows::Create::Params::Create(*args_));
    EXTENSION_FUNCTION_VALIDATE(params);
    if(params->app_id.empty())
      return RespondNow(Error(kAppIdIsEmptyError));
    ChromeLauncherController::instance()->ActivateApp(params->app_id,
                                                  ash::LAUNCH_FROM_UNKNOWN,
                                                  0,
                                                  0);
    return RespondNow(NoArguments());
  }

} // exit namesapce extensions
