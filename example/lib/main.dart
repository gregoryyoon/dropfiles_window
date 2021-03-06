import 'package:flutter/material.dart';
import 'dart:async';
import 'dart:io';
import 'package:flutter/services.dart';
import 'package:dropfiles_window/dropfiles_window.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatefulWidget {
  @override
  _MyAppState createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  String _platformVersion = 'Unknown';
  String _dropFileName = '';

  @override
  void initState() {
    super.initState();
    initPlatformState();
  }

  // Platform messages are asynchronous, so we initialize in an async method.
  Future<void> initPlatformState() async {
    String platformVersion;
    // Platform messages may fail, so we use a try/catch PlatformException.
    try {
      platformVersion = await DropfilesWindow.platformVersion;
    } on PlatformException {
      platformVersion = 'Failed to get platform version.';
    }
    if (Platform.isWindows == true) {
      // Platform messages may fail, so we use a try/catch PlatformException.
      try {
        DropfilesWindow.modifyWindowAcceptFiles((String strName) {
          // print("fileName=$strName");
          setState(() {
            _dropFileName = strName;
          });
        });
      } on PlatformException {
        _dropFileName = 'Failed to modifyDropFilesWindow.';
      }
    }

    // If the widget was removed from the tree while the asynchronous platform
    // message was in flight, we want to discard the reply rather than calling
    // setState to update our non-existent appearance.
    if (!mounted) return;

    setState(() {
      _platformVersion = platformVersion;
    });
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Plugin example app'),
        ),
        body: Center(
          child:
              Text('Running on: $_platformVersion, DropFile=$_dropFileName\n'),
        ),
      ),
    );
  }
}
