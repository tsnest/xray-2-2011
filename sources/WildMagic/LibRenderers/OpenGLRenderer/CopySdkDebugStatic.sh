#!/bin/tcsh
cd ${SRCROOT}
set HEADERS = `ls *.h *.inl`
foreach header (${HEADERS})
    cp -fp "${SRCROOT}"/$header ../../SDK/Include
end

cp -fp "${SRCROOT}"/build/Default/libWm4AglRendererd.a ../../SDK/Library/Debug/libWm4AglRendererd.a
ranlib ../../SDK/Library/Debug/libWm4AglRendererd.a

