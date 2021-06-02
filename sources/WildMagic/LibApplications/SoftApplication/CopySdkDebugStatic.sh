#!/bin/tcsh
cd ${SRCROOT}
set HEADERS = `ls ../*.h ../*.inl`
foreach header (${HEADERS})
    cp -fp "${SRCROOT}"/$header ../../SDK/Include
end

cp -fp "${SRCROOT}"/build/Default/libWm4MacSoftApplicationd.a ../../SDK/Library/Debug/libWm4MacSoftApplicationd.a
ranlib ../../SDK/Library/Debug/libWm4MacSoftApplicationd.a

