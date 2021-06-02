#!/bin/tcsh
mkdir -p ../SDK/Include
mkdir -p ../SDK/Library
mkdir -p ../SDK/Library/ReleaseMemory

set DIRS = `ls`
foreach dir (${DIRS})
    if (-d $dir && $dir != LibIMagics.xcodeproj && $dir != build) then
        echo $dir
        cd $dir
        set CURDIR = `pwd`
        set HEADERS = `ls *.h *.inl`
        foreach header (${HEADERS})
            cp -fp "${CURDIR}"/$header ../../SDK/Include
        end
        cd ..
    endif
end

set RHEADERS = `ls *.h`
foreach rheader (${RHEADERS})
    cp -fp "${SRCROOT}"/$rheader ../SDK/Include
end

cp -fp "${SRCROOT}"/build/Default/libWm4Imagicsm.a ../SDK/Library/ReleaseMemory/libWm4Imagics.a
ranlib ../SDK/Library/ReleaseMemory/libWm4Imagics.a
