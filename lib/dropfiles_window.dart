import 'dart:async';

import 'package:flutter/services.dart';

typedef void OnDropFileCallback(String strName);

class DropfilesWindow {
  static const MethodChannel _channel = const MethodChannel('dropfiles_window');

  static Future<String> get platformVersion async {
    final String version = await _channel.invokeMethod('getPlatformVersion');
    return version;
  }

  /// to remove the callback call the function again with null as parameter
  static void modifyWindowAcceptFiles(OnDropFileCallback? callback) async {
    if (callback != null) {
      _channel.setMethodCallHandler((MethodCall call) async {
        // print("method=${call.method} argumemts=${call.arguments}");
        assert(call.method == 'onDropFile');
        callback(call.arguments);
      });
      await _channel.invokeMethod('modifyWindowAcceptFiles');
    } else {
      _channel.setMethodCallHandler(null);
      await _channel.invokeMethod('resetWindowAcceptFiles');
    }
  }
}
