#! /bin/bash
NDK_ROOT=/opt/android-ndk-r9
cd $(dirname $0)
$NDK_ROOT/ndk-build NDK_DEBUG=1 $@
