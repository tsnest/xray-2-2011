#!/bin/tcsh
cd ${SRCROOT}
set HEADERS = `ls *.h *.inl`
foreach header (${HEADERS})
    cp -fp "${SRCROOT}"/$header ../../SDK/Include
end

cp -fp "${SRCROOT}"/build/Default/libWm4MacSoftRendererm.a ../../SDK/Library/ReleaseMemory/libWm4MacSoftRenderer.a
ranlib ../../SDK/Library/ReleaseMemory/libWm4MacSoftRenderer.a

