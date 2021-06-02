#!/bin/tcsh
cd ${SRCROOT}
set HEADERS = `ls ../*.h ../*.inl`
foreach header (${HEADERS})
    cp -fp "${SRCROOT}"/$header ../../SDK/Include
end

cp -fp "${SRCROOT}"/build/Default/libWm4AglApplicationd.a ../../SDK/Library/Debug/libWm4AglApplicationd.a
ranlib ../../SDK/Library/Debug/libWm4AglApplicationd.a

