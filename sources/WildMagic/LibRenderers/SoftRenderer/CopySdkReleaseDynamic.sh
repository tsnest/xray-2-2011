#!/bin/tcsh
cd ${SRCROOT}
set HEADERS = `ls *.h *.inl`
foreach header (${HEADERS})
    cp -fp "${SRCROOT}"/$header ../../SDK/Include
end

cp -fp "${SRCROOT}"/build/Default/libWm4MacSoftRenderer.dylib ../../SDK/Library/ReleaseDLL/libWm4MacSoftRenderer.dylib

