#!/bin/tcsh
cd ${SRCROOT}
set HEADERS = `ls ../*.h ../*.inl`
foreach header (${HEADERS})
    cp -fp "${SRCROOT}"/$header ../../SDK/Include
end

cp -fp "${SRCROOT}"/build/Default/libWm4AglApplicationmd.a ../../SDK/Library/DebugMemory/libWm4AglApplicationd.a
ranlib ../../SDK/Library/DebugMemory/libWm4AglApplicationd.a

