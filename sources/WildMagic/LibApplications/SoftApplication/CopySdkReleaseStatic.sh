#!/bin/tcsh
cd ${SRCROOT}
set HEADERS = `ls ../*.h ../*.inl`
foreach header (${HEADERS})
    cp -fp "${SRCROOT}"/$header ../../SDK/Include
end

cp -fp "${SRCROOT}"/build/Default/libWm4MacSoftApplication.a ../../SDK/Library/Release/libWm4MacSoftApplication.a
ranlib ../../SDK/Library/Release/libWm4MacSoftApplication.a

