#!/bin/tcsh
cd ${SRCROOT}
set HEADERS = `ls *.h *.inl`
foreach header (${HEADERS})
    cp -fp "${SRCROOT}"/$header ../../SDK/Include
end

cp -fp "${SRCROOT}"/build/Default/libWm4MacSoftRenderer.a ../../SDK/Library/Release/libWm4MacSoftRenderer.a
ranlib ../../SDK/Library/Release/libWm4MacSoftRenderer.a

