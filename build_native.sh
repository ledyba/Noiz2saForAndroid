#!/bin/bash

if [ -z $NDK_ROOT ]; then
		NDK_ROOT=/opt/android-ndk-r9c
fi

echo NDK_ROOT=$NDK_ROOT
# For compatibility of android-ndk-r9, 4.7 was removed from r9
if [ -d "${NDK_ROOT}/toolchains/arm-linux-androideabi-4.8" ]; then
	export NDK_TOOLCHAIN_VERSION=4.8
	echo "The Selected NDK toolchain version was 4.8 !"
else
	if [ -d "${NDK_ROOT}/toolchains/arm-linux-androideabi-4.7" ]; then
		export NDK_TOOLCHAIN_VERSION=4.7
		echo "The Selected NDK toolchain version was 4.7 !"
	else
		echo "Couldn't find the gcc toolchain."
		exit 1
	fi
fi

APP_ROOT=$(cd $(dirname $0); pwd -LP)
echo "APP_ROOT = $APP_ROOT"

echo "Using prebuilt externals"
echo + "$NDK_ROOT"/ndk-build -C "$APP_ROOT" $*
"$NDK_ROOT"/ndk-build -C "$APP_ROOT" $*

