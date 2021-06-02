#!/bin/tcsh
cd ${SRCROOT}
set HEADERS = `ls *.h *.inl`
foreach header (${HEADERS})
    cp -fp "${SRCROOT}"/$header ../../SDK/Include
end

cp -fp "${SRCROOT}"/build/Default/libWm4AglRendererm.a ../../SDK/Library/ReleaseMemory/libWm4AglRenderer.a
ranlib ../../SDK/Library/ReleaseMemory/libWm4AglRenderer.a

