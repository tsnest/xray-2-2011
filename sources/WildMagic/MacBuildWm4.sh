#!/bin/tcsh

if ("$1" == "") then
    echo "Usage: $0 grf cfg lib opr"
    echo "grf in {Agl,Sft}"
    echo "cfg in {Debug,Release}"
    echo "lib in {Static,Dynamic,Memory}"
    echo "opr in {build,clean}"
    exit
endif

set libTarget = ''
set appTarget = ''

if ("$1" == "Agl") then
    set appTarget = Agl
else if ("$1" == "Sft") then
    set appTarget = Sft
else
    echo "Usage: $0 grf cfg lib opr"
    echo "grf in {Agl,Sft}"
    echo "cfg in {Debug,Release}"
    echo "lib in {Static,Dynamic,Memory}"
    echo "opr in {build,clean}"
    exit
endif

if ("$2" == "Debug") then
    set libTarget =  Debug
    set appTarget = "$appTarget"' Debug'
else if ("$2" == "Release") then
    set libTarget = Release
    set appTarget = "$appTarget"' Release'
else
    echo "Usage: $0 grf cfg lib opr"
    echo "grf in {Agl,Sft}"
    echo "cfg in {Debug,Release}"
    echo "lib in {Static,Dynamic,Memory}"
    echo "opr in {build,clean}"
    exit
endif

if ("$3" == "Static") then
    set libTarget = "$libTarget"' Static'
    set appTarget = "$appTarget"' Static'
else if ("$3" == "Dynamic") then
    set libTarget = "$libTarget"' Dynamic'
    set appTarget = "$appTarget"' Dynamic'
else if ("$3" == "Memory") then
    set libTarget = "$libTarget"' Memory'
    set appTarget = "$appTarget"' Memory'
else
    echo "Usage: $0 grf cfg lib opr"
    echo "grf in {Agl,Sft}"
    echo "cfg in {Debug,Release}"
    echo "lib in {Static,Dynamic,Memory}"
    echo "opr in {build,clean}"
    exit
endif

set opr = ''
if ("$4" == "build") then
    set opr = build
else if ("$4" == "clean") then
    set opr = clean
else
    echo "Usage: $0 grf cfg lib opr"
    echo "grf in {Agl,Sft}"
    echo "cfg in {Debug,Release}"
    echo "lib in {Static,Dynamic,Memory}"
    echo "opr in {build,clean}"
    exit
endif

cd LibFoundation
xcodebuild -project LibFoundation.xcodeproj -configuration Default -target "${libTarget}" $opr
cd ..

cd LibImagics
xcodebuild -project LibImagics.xcodeproj -configuration Default -target "${libTarget}" $opr
cd ..

cd LibGraphics
xcodebuild -project LibGraphics.xcodeproj -configuration Default -target "$libTarget" $opr
cd ..

cd LibPhysics
xcodebuild -project LibPhysics.xcodeproj -configuration Default -target "$libTarget" $opr
cd ..

if ("$1" == "Agl") then
    cd LibRenderers/OpenGLRenderer
    xcodebuild -project AglRenderer.xcodeproj -configuration Default -target "$libTarget" $opr
    cd ../..

    cd LibApplications/OpenGLApplication
    xcodebuild -project AglApplication.xcodeproj -configuration Default -target "$libTarget" $opr
    cd ../..
else
    cd LibRenderers/SoftRenderer
    xcodebuild -project MacSoftRenderer.xcodeproj -configuration Default -target "$libTarget" $opr
    cd ../..

    cd LibApplications/SoftApplication
    xcodebuild -project MacSoftApplication.xcodeproj -configuration Default -target "$libTarget" $opr
    cd ../..
endif

cd SampleFoundation
set DIRS = `ls`
foreach dir (${DIRS})
    if (-d $dir) then
        echo $dir
        cd $dir
        xcodebuild -project $dir.xcodeproj -configuration Default -target "$appTarget" $opr
        cd ..
    endif
end
cd ..

cd SampleGraphics
set DIRS = `ls`
foreach dir (${DIRS})
    if (-d $dir) then
        echo $dir
        cd $dir
        xcodebuild -project $dir.xcodeproj -configuration Default -target "$appTarget" $opr
        cd ..
    endif
end
cd ..

cd SampleImagics
set DIRS = `ls`
foreach dir (${DIRS})
    if (-d $dir) then
        echo $dir
        cd $dir
        xcodebuild -project $dir.xcodeproj -configuration Default -target "$appTarget" $opr
        cd ..
    endif
end
cd ..

cd SamplePhysics
set DIRS = `ls`
foreach dir (${DIRS})
    if (-d $dir) then
        echo $dir
        cd $dir
        xcodebuild -project $dir.xcodeproj -configuration Default -target "$appTarget" $opr
        cd ..
    endif
end
cd ..
