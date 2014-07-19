#! /bin/bash

CDIR=$(cd $(dirname $0); pwd -LP)

if [ -z $ANDROID_SDK_ROOT ]; then
		ANDROID_SDK_ROOT=/opt/android
fi

function ex() {
echo -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
echo $*
echo =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
$*
}

cd ${CDIR}
sh ${CDIR}/build_native.sh
ex ${ANDROID_SDK_ROOT}/tools/android update project --path ${CDIR} -s --target "android-17"
ex ant debug -Dsdk.dir=${ANDROID_SDK_ROOT}
ex ant installd -Dsdk.dir=${ANDROID_SDK_ROOT}

