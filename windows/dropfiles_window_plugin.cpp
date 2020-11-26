#include "include/dropfiles_window/dropfiles_window_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <map>
#include <memory>
#include <sstream>

namespace {
HWND GetRootWindow(flutter::FlutterView *view) {
  return GetAncestor(view->GetNativeWindow(), GA_ROOT);
}
class DropfilesWindowPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  DropfilesWindowPlugin(flutter::PluginRegistrarWindows *registrar, std::unique_ptr<flutter::MethodChannel<>> channel);




  virtual ~DropfilesWindowPlugin();

 private:
  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
  // The ID of the WindowProc delegate registration.
  int window_proc_id_ = -1;
  // The registrar for this plugin, for accessing the window.
  flutter::PluginRegistrarWindows *registrar_;
  std::unique_ptr<flutter::MethodChannel<>> channel_;
  // Called for top-level WindowProc delegation.
  std::optional<LRESULT> HandleWindowProc(HWND hwnd, UINT message,
                                          WPARAM wparam, LPARAM lparam);

  void DropfilesWindowPlugin::HandleDropFiles(WPARAM wParam);
};

// static
void DropfilesWindowPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "dropfiles_window",
          &flutter::StandardMethodCodec::GetInstance());
  auto *channel_pointer = channel.get();
  auto plugin = std::make_unique<DropfilesWindowPlugin>(registrar, std::move(channel));

  channel_pointer->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

DropfilesWindowPlugin::DropfilesWindowPlugin(flutter::PluginRegistrarWindows *registrar, std::unique_ptr<flutter::MethodChannel<>> channel) {
  registrar_ = registrar;
  channel_ = std::move(channel);
  window_proc_id_ = registrar_->RegisterTopLevelWindowProcDelegate(
      [this](HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
        return HandleWindowProc(hwnd, message, wparam, lparam);
      });

}

DropfilesWindowPlugin::~DropfilesWindowPlugin() {
  registrar_->UnregisterTopLevelWindowProcDelegate(window_proc_id_);
}

void DropfilesWindowPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  // Replace "getPlatformVersion" check with your plugin's method.
  // See:
  // https://github.com/flutter/engine/tree/master/shell/platform/common/cpp/client_wrapper/include/flutter
  // and
  // https://github.com/flutter/engine/tree/master/shell/platform/glfw/client_wrapper/include/flutter
  // for the relevant Flutter APIs.
  if (method_call.method_name().compare("getPlatformVersion") == 0) {
    std::ostringstream version_stream;
    version_stream << "Windows ";
    if (IsWindows10OrGreater()) {
      version_stream << "10+";
    } else if (IsWindows8OrGreater()) {
      version_stream << "8";
    } else if (IsWindows7OrGreater()) {
      version_stream << "7";
    }
    result->Success(flutter::EncodableValue(version_stream.str()));
  } else if (method_call.method_name().compare("modifyWindowAcceptFiles") == 0) {
    HWND hWnd = GetRootWindow(registrar_->GetView());
    LONG nValue = GetWindowLong(hWnd, GWL_EXSTYLE);
    nValue |= WS_EX_ACCEPTFILES ;
    SetWindowLong(hWnd, GWL_EXSTYLE, nValue);  
    result->Success();
  }
  else {
    result->NotImplemented();
  }
}

void DropfilesWindowPlugin::HandleDropFiles(WPARAM wParam)
{
  char szName[MAX_PATH];
  TCHAR wszName[MAX_PATH];
  std::ostringstream strName;

  HDROP hDrop = (HDROP)wParam;

  int count = DragQueryFile(hDrop, 0xFFFFFFFF, wszName, MAX_PATH);

  for (int i = 0; i < count; i++)
  {
    DragQueryFile(hDrop, i, wszName, MAX_PATH);

    if (i == 0) 
    {
      WideCharToMultiByte(CP_ACP, 0, wszName, MAX_PATH, szName, MAX_PATH, NULL, NULL);
      strName << szName;
      channel_->InvokeMethod("onDropFile", std::make_unique<flutter::EncodableValue>(strName.str()));
    }
  }

  DragFinish(hDrop);
}

std::optional<LRESULT> DropfilesWindowPlugin::HandleWindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
  std::optional<LRESULT> result;
  switch (message) {
    case WM_DROPFILES:
      HandleDropFiles(wparam);
      result = 0;
      break;
  }
  return result;
}
}  // namespace

void DropfilesWindowPluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  DropfilesWindowPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
