#! /bin/bash

CDIR=$(cd $(dirname $0); pwd -LP)

if [ -z $SDK_ROOT ]; then
		SDK_ROOT=/opt/android
fi

function ex() {
echo -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
echo $*
echo =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
$*
}

cd ${CDIR}
sh ${CDIR}/build_native.sh
ex ${SDK_ROOT}/tools/android update project --path ${CDIR} -s --target "android-12"
ex ant debug -Dsdk.dir=${SDK_ROOT}
ex ant installd -Dsdk.dir=${SDK_ROOT}

