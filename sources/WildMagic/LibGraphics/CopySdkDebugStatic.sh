#!/bin/tcsh
mkdir -p ../SDK/Include
mkdir -p ../SDK/Library
mkdir -p ../SDK/Library/Debug

set DIRS = `ls`
foreach dir (${DIRS})
    if (-d $dir && $dir != LibGraphics.xcodeproj && $dir != build) then
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

cp -fp "${SRCROOT}"/build/Default/libWm4Graphicsd.a ../SDK/Library/Debug/libWm4Graphicsd.a
ranlib ../SDK/Library/Debug/libWm4Graphicsd.a
