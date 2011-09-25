#!/bin/bash

source tools.sh

$ADB -d install -r build/bin/KiwiViewer-debug.apk
$ADB shell am start -a android.intent.action.MAIN -n com.kitware.KiwiViewer/.KiwiViewerActivity
