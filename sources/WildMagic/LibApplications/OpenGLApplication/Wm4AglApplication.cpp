// Wild Magic Source Code
// David Eberly
// http://www.geometrictools.com
// Copyright (c) 1998-2009
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.  The license is available for reading at
// either of the locations:
//     http://www.gnu.org/copyleft/lgpl.html
//     http://www.geometrictools.com/License/WildMagicLicense.pdf
//
// Version: 4.0.0 (2006/06/28)

#include <Carbon/Carbon.h>
#include <unistd.h>
using namespace std;

#include "Wm4WindowApplication.h"
#include "Wm4AglRenderer.h"
using namespace Wm4;

const int WindowApplication::KEY_ESCAPE = kEscapeCharCode;
const int WindowApplication::KEY_LEFT_ARROW = kLeftArrowCharCode;
const int WindowApplication::KEY_RIGHT_ARROW = kRightArrowCharCode;
const int WindowApplication::KEY_UP_ARROW = kUpArrowCharCode;
const int WindowApplication::KEY_DOWN_ARROW = kDownArrowCharCode;
const int WindowApplication::KEY_HOME = kHomeCharCode;
const int WindowApplication::KEY_END = kEndCharCode;
const int WindowApplication::KEY_PAGE_UP = kPageUpCharCode;
const int WindowApplication::KEY_PAGE_DOWN = kPageDownCharCode;
const int WindowApplication::KEY_INSERT = kHelpCharCode;
const int WindowApplication::KEY_DELETE = kDeleteCharCode;
const int WindowApplication::KEY_BACKSPACE = kBackspaceCharCode;
const int WindowApplication::KEY_TAB = kTabCharCode;
const int WindowApplication::KEY_ENTER = kEnterCharCode;
const int WindowApplication::KEY_RETURN = kReturnCharCode;

// Function key values are actually key codes rather than char codes because
// MacOS passes all function key values as kFunctionKeyCharCode.
const int WindowApplication::KEY_F1 = 122;
const int WindowApplication::KEY_F2 = 120;
const int WindowApplication::KEY_F3 = 99;
const int WindowApplication::KEY_F4 = 118;
const int WindowApplication::KEY_F5 = 96;
const int WindowApplication::KEY_F6 = 97;
const int WindowApplication::KEY_F7 = 98;
const int WindowApplication::KEY_F8 = 100;
const int WindowApplication::KEY_F9 = 101;
const int WindowApplication::KEY_F10 = 109;
const int WindowApplication::KEY_F11 = 103;
const int WindowApplication::KEY_F12 = 111;

const int WindowApplication::KEY_SHIFT = shiftKey | rightShiftKey;
const int WindowApplication::KEY_CONTROL = controlKey | rightControlKey;
const int WindowApplication::KEY_ALT = optionKey | rightOptionKey;
const int WindowApplication::KEY_COMMAND = cmdKey;

const int WindowApplication::MOUSE_LEFT_BUTTON = kEventMouseButtonPrimary;
const int WindowApplication::MOUSE_MIDDLE_BUTTON = kEventMouseButtonTertiary;
const int WindowApplication::MOUSE_RIGHT_BUTTON = kEventMouseButtonSecondary;
const int WindowApplication::MOUSE_UP = 0;
const int WindowApplication::MOUSE_DOWN = 1;

const int WindowApplication::MODIFIER_CONTROL = controlKey | rightControlKey;
const int WindowApplication::MODIFIER_LBUTTON = kEventMouseButtonPrimary;
const int WindowApplication::MODIFIER_MBUTTON = kEventMouseButtonTertiary;
const int WindowApplication::MODIFIER_RBUTTON = kEventMouseButtonSecondary;
const int WindowApplication::MODIFIER_SHIFT   = shiftKey | rightShiftKey;

// Indices into the extra data storage for some system data.
#define AGLAPP_BACK   0
#define AGLAPP_FONT   AGLAPP_BACK + sizeof(GWorldPtr)
#define AGLAPP_SYSTEM AGLAPP_FONT + sizeof(SInt32)

//----------------------------------------------------------------------------
void WindowApplication::SetMousePosition (int iX, int iY)
{
    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;

    CGrafPtr pkCurrPort;
    GetPort(&pkCurrPort);
    SetPortWindowPort((WindowRef)(pkTheApp->GetWindowID()));
    Point kMouseLoc;
    kMouseLoc.h = (short)iX;
    kMouseLoc.v = (short)iY;
    LocalToGlobal(&kMouseLoc);
    SetPort(pkCurrPort);

    CGPoint kPoint = CGPointMake((float)kMouseLoc.h,(float)kMouseLoc.v);
    CGPostMouseEvent(kPoint,true,1,false,0);
}
//----------------------------------------------------------------------------
void WindowApplication::GetMousePosition (int& riX, int& riY) const
{
    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;

    CGrafPtr pkCurrPort;
    GetPort(&pkCurrPort);
    SetPortWindowPort((WindowRef)(pkTheApp->GetWindowID()));
    ::Point kMouseLoc;
    GetMouse(&kMouseLoc);
    SetPort(pkCurrPort);
    
    riX = (int)kMouseLoc.h;
    riY = (int)kMouseLoc.v;
}
//----------------------------------------------------------------------------
int WindowApplication::GetStringWidth (const char* acText) const
{
    GWorldPtr pkWorld;
    GDHandle kDevice;
    GetGWorld(&pkWorld,&kDevice);

    GWorldPtr pkBack;
    GetExtraData(AGLAPP_BACK,sizeof(GWorldPtr),&pkBack);
    SetGWorld(pkBack,0);

    int iWidth = TextWidth(acText,0,strlen(acText));

    SetGWorld(pkWorld,kDevice);
    return iWidth;
}
//----------------------------------------------------------------------------
int WindowApplication::GetCharacterWidth (const char cCharacter) const
{
    GWorldPtr pkWorld;
    GDHandle kDevice;
    GetGWorld(&pkWorld,&kDevice);

    GWorldPtr pkBack;
    GetExtraData(AGLAPP_BACK,sizeof(GWorldPtr),&pkBack);
    SetGWorld(pkBack,0);

    int iWidth = ::CharWidth(cCharacter);

    SetGWorld(pkWorld,kDevice);
    return iWidth;
}
//----------------------------------------------------------------------------
int WindowApplication::GetFontHeight () const
{
    GWorldPtr pkWorld;
    GDHandle kDevice;
    GetGWorld(&pkWorld,&kDevice);

    GWorldPtr pkBack;
    GetExtraData(AGLAPP_BACK,sizeof(GWorldPtr),&pkBack);
    SetGWorld(pkBack,0);
    
    FontInfo kFontInfo;
    GetFontInfo(&kFontInfo);
    
    SetGWorld(pkWorld,kDevice);
    return kFontInfo.ascent + kFontInfo.descent + kFontInfo.leading;
}
//----------------------------------------------------------------------------
static OSErr MacApplicationAEGetDescData (const AEDesc* pkDesc,
    DescType* piTypeCode, void* pvDataBuffer, ByteCount iMaximumSize,
    ByteCount* piActualSize)
{
    OSErr iTheErr = noErr;
    ByteCount iDataSize;

    if (piTypeCode && pkDesc)
    {
        *piTypeCode = pkDesc->descriptorType;
    }

    iDataSize = AEGetDescDataSize(pkDesc);
    if (piActualSize)
    {
        *piActualSize = iDataSize;
    }

    if (iDataSize > 0 && iMaximumSize > 0)
    {
        iTheErr = AEGetDescData(pkDesc,pvDataBuffer,iMaximumSize);
    }

    return iTheErr;
}
//----------------------------------------------------------------------------
static OSErr MacApplicationNavGetFile (FSSpecPtr pkTheFileP,
    Boolean* pbGotFile)
{   
    NavDialogOptions kDialogOptions; 
    OSErr iTheErr = NavGetDefaultDialogOptions(&kDialogOptions);
    kDialogOptions.dialogOptionFlags =
        (kNavDefaultNavDlogOptions | kNavNoTypePopup) &
        ~kNavAllowPreviews & ~kNavAllowMultipleFiles;

    NavReplyRecord kTheReply;
    NavEventUPP pkEventProc = 0;
    iTheErr = NavGetFile(0,&kTheReply,&kDialogOptions,pkEventProc,0,
        0,0,(NavCallBackUserData)0);

    if (kTheReply.validRecord && iTheErr == noErr)
    {
        long lCount;
        iTheErr = AECountItems(&kTheReply.selection,&lCount);
        if (iTheErr == noErr)
        {
            long lIndex = 1;
            AEDesc kResultDesc;
            AEKeyword kKeyWord;
            iTheErr = AEGetNthDesc(&kTheReply.selection,lIndex,typeFSS,
                &kKeyWord,&kResultDesc);

            if (iTheErr == noErr)
            {
                DescType iTypeCode = typeFSS;
                ByteCount iC;

                iTheErr = MacApplicationAEGetDescData(&kResultDesc,
                    &iTypeCode,pkTheFileP,sizeof(FSSpec),&iC);
                *pbGotFile = true;
                iTheErr = AEDisposeDesc(&kResultDesc);
            }
        }

        iTheErr = NavDisposeReply(&kTheReply);
    }
    else
    {
        *pbGotFile = false;
    }

    return iTheErr;
}
//----------------------------------------------------------------------------
static Boolean MacApplicationOpenFile (FSSpecPtr pkFile)
{
    FSSpec kTheFile;
    Boolean bGotFile = false;
    OSErr iErr = MacApplicationNavGetFile(&kTheFile,&bGotFile);
    if (iErr == noErr && bGotFile)
    {
        *pkFile = kTheFile;
    }

    return bGotFile;
}
//----------------------------------------------------------------------------
static string GetStringPathname (FSSpec& rkFile)
{
    FSRef kFileRef;
    FSpMakeFSRef(&rkFile,&kFileRef);

    CFURLRef pkURL = CFURLCreateFromFSRef(0,&kFileRef);
    CFStringRef kString = CFURLCopyFileSystemPath(pkURL,kCFURLPOSIXPathStyle);
    char acPath[256];
    CFStringGetCString(kString,acPath,256,CFStringGetSystemEncoding());
    CFRelease(kString);
    CFRelease(pkURL);
    std::string kFileName(acPath);
    return kFileName;
}
//----------------------------------------------------------------------------
static char* GetCommandLine ()
{
    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;

    OSErr iErr = noErr;
    SInt32 iSystemVersion;
    iErr = Gestalt(gestaltSystemVersion,&iSystemVersion);
    pkTheApp->SetExtraData(AGLAPP_SYSTEM,sizeof(SInt32),&iSystemVersion);
    if (iErr != noErr)
    {
        return 0;
    }

    FILE* pkFile = fopen ("cmdline.txt","rb");
    if (pkFile == 0)
    {
        FSSpec kFile;
        if (MacApplicationOpenFile(&kFile))
        {
            string kName = GetStringPathname(kFile);
            char* pcTheFile = WM4_NEW char[kName.length()+1];
            strcpy(pcTheFile,kName.c_str());
            return pcTheFile;
        }
        return 0;
    }

    // get size of file
    unsigned int uiFileSize = 0;
    while (fgetc(pkFile) != EOF)
    {
        uiFileSize++;
    }
    
    if (uiFileSize == 0)
    {
        fclose(pkFile);
        return 0;
    }
    
    // read file
    fseek(pkFile,0,SEEK_SET);
    char* pcCmdLine = WM4_NEW char[uiFileSize+1];
    char* pcIterator = pcCmdLine;
    char c;
    while ((c = (char)fgetc(pkFile)) != EOF)
    {
        // cull out non-printable characters
       *pcIterator++ = (isprint(c) ? c : ' ');
            
    }
    *pcIterator = '\0';
    
    fclose(pkFile);
    return pcCmdLine;
}
//----------------------------------------------------------------------------
static pascal OSStatus ProcessWindowClose (EventHandlerCallRef pkNextHandler, 
    EventRef pkEvent, void*)
{
    // allow standard handler to run
    OSStatus eResult = CallNextEventHandler(pkNextHandler,pkEvent);

    // quit the application
    QuitApplicationEventLoop();
    
    return eResult;
}
//----------------------------------------------------------------------------
static pascal OSStatus ProcessWindowBoundsChange (EventHandlerCallRef, 
    EventRef pkEvent, void*)
{
    UInt32 uiAttributes;
    GetEventParameter(pkEvent,kEventParamAttributes,typeUInt32,0,
        sizeof(uiAttributes),0,&uiAttributes);

    Rect kRect;
    GetEventParameter(pkEvent,kEventParamCurrentBounds,typeQDRectangle,0,
        sizeof(kRect),0,&kRect);

    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;

    if (uiAttributes & kWindowBoundsChangeUserDrag
    ||  uiAttributes & kWindowBoundsChangeOriginChanged)
    {
        // bounds are changing due to window moving
        pkTheApp->OnMove(kRect.top,kRect.left);
    }
    else if (uiAttributes & kWindowBoundsChangeUserResize
    ||  uiAttributes & kWindowBoundsChangeSizeChanged)
    {
        // bounds are changing due to window resizing
        pkTheApp->OnResize(kRect.right-kRect.left,kRect.bottom-kRect.top);
    }

    // allow standard handler to run
    return eventNotHandledErr;
}
//----------------------------------------------------------------------------
static pascal OSStatus ProcessWindowZoomed (EventHandlerCallRef, 
    EventRef pkEvent, void*)
{
    WindowRef kWindow;
    GetEventParameter(pkEvent,kEventParamDirectObject,typeWindowRef,0,
        sizeof(kWindow),0,&kWindow);

    Rect kRect;
    GetWindowBounds(kWindow,kWindowContentRgn,&kRect);

    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;

    pkTheApp->OnResize(kRect.right-kRect.left,kRect.bottom-kRect.top);

    // allow standard handler to run
    return eventNotHandledErr;
}
//----------------------------------------------------------------------------
static pascal OSStatus ProcessWindowRedraw (EventHandlerCallRef, EventRef,
    void*)
{
    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;

    pkTheApp->OnDisplay();

    // allow standard handler to run
    return eventNotHandledErr;
}
//----------------------------------------------------------------------------
static pascal OSStatus ProcessKeyDown (EventHandlerCallRef, EventRef pkEvent,
    void*)
{
    char cCharCode;
    GetEventParameter(pkEvent,kEventParamKeyMacCharCodes,typeChar,0, 
        sizeof(cCharCode),0,&cCharCode);

    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;
    
    CGrafPtr pkCurrPort;
    GetPort(&pkCurrPort);
    SetPortWindowPort((WindowRef)(pkTheApp->GetWindowID()));
    ::Point kMouseLoc;
    GetMouse(&kMouseLoc);
    SetPort(pkCurrPort);
    
    if (cCharCode == kEscapeCharCode)
    {
        // quit the application when 'esc' is pressed
        QuitApplicationEventLoop();
        return eventNotHandledErr;
    }

    if (isalnum(cCharCode) || isprint(cCharCode))
    {
        pkTheApp->OnKeyDown(cCharCode,kMouseLoc.h,kMouseLoc.v);
    }
    else
    {
       if (cCharCode == kFunctionKeyCharCode)
       {
           // function key - get key identity
           UInt32 uiKeyCode;
           GetEventParameter(pkEvent,kEventParamKeyCode,typeUInt32,0, 
               sizeof(uiKeyCode),0,&uiKeyCode);
           
           cCharCode = uiKeyCode & 0x000000FF; 
       }

       // Do not filter for specific keys.  This allows for keys such as tab,
       // delete, enter.
       pkTheApp->OnSpecialKeyDown(cCharCode,kMouseLoc.h,kMouseLoc.v);
    }

    // allow standard handler to run
    return eventNotHandledErr;
}
//----------------------------------------------------------------------------
static pascal OSStatus ProcessKeyUp (EventHandlerCallRef, EventRef pkEvent,
    void*)
{
    char cCharCode;
    GetEventParameter(pkEvent,kEventParamKeyMacCharCodes,typeChar,0, 
        sizeof(cCharCode),0,&cCharCode);

    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;
    
    CGrafPtr pkCurrPort;
    GetPort(&pkCurrPort);
    SetPortWindowPort((WindowRef)(pkTheApp->GetWindowID()));
    ::Point kMouseLoc;
    GetMouse(&kMouseLoc);
    SetPort(pkCurrPort);
    
    if (isalnum(cCharCode) || isprint(cCharCode))
    {
        pkTheApp->OnKeyUp(cCharCode,kMouseLoc.h,kMouseLoc.v);
    }
    else
    {
        if (cCharCode == kFunctionKeyCharCode)
        {
            // function key - get key identity
            UInt32 uiKeyCode;
            GetEventParameter(pkEvent,kEventParamKeyCode,typeUInt32,0, 
                sizeof(uiKeyCode),0,&uiKeyCode);
            
            cCharCode = uiKeyCode & 0x000000FF; 
        }
        
        // Do not filter for specific keys.  This allows for keys such as tab,
        // delete, enter.
        pkTheApp->OnSpecialKeyUp(cCharCode,kMouseLoc.h,kMouseLoc.v);
    }

    // allow standard handler to run
    return eventNotHandledErr;
}
//----------------------------------------------------------------------------
static pascal OSStatus ProcessMouseDown (EventHandlerCallRef,
    EventRef pkEvent, void*)
{
    ::Point kMouseLoc;    
    GetEventParameter(pkEvent,kEventParamMouseLocation,typeQDPoint,0,
        sizeof(kMouseLoc),0,&kMouseLoc);

    EventMouseButton eMouseButton;
    GetEventParameter(pkEvent,kEventParamMouseButton,typeMouseButton,0,
        sizeof(eMouseButton),0,&eMouseButton);

    UInt32 uiModifiers;
    GetEventParameter(pkEvent,kEventParamKeyModifiers,typeUInt32,0,
        sizeof(uiModifiers),0,&uiModifiers);

    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;

    CGrafPtr pkCurrPort;
    GetPort(&pkCurrPort);
    SetPortWindowPort((WindowRef)(pkTheApp->GetWindowID()));
    GlobalToLocal(&kMouseLoc);
    SetPort(pkCurrPort);

    pkTheApp->OnMouseClick(eMouseButton,WindowApplication::MOUSE_DOWN,
        kMouseLoc.h,kMouseLoc.v,uiModifiers);

    // allow standard handler to run
    return eventNotHandledErr;
}
//----------------------------------------------------------------------------
static pascal OSStatus ProcessMouseUp (EventHandlerCallRef, EventRef pkEvent,
    void*)
{
    ::Point kMouseLoc;    
    GetEventParameter(pkEvent,kEventParamMouseLocation,typeQDPoint,0,
        sizeof(kMouseLoc),0,&kMouseLoc);

    EventMouseButton eMouseButton;
    GetEventParameter(pkEvent,kEventParamMouseButton,typeMouseButton,0,
        sizeof(eMouseButton),0,&eMouseButton);

    UInt32 uiModifiers;
    GetEventParameter(pkEvent,kEventParamKeyModifiers,typeUInt32,0,
        sizeof(uiModifiers),0,&uiModifiers);

    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;

    CGrafPtr pkCurrPort;
    GetPort(&pkCurrPort);
    SetPortWindowPort((WindowRef)(pkTheApp->GetWindowID()));
    GlobalToLocal(&kMouseLoc);
    SetPort(pkCurrPort);

    pkTheApp->OnMouseClick(eMouseButton,WindowApplication::MOUSE_UP,
        kMouseLoc.h,kMouseLoc.v,uiModifiers);

    // allow standard handler to run
    return eventNotHandledErr;
}
//----------------------------------------------------------------------------
static pascal OSStatus ProcessMouseDragged (EventHandlerCallRef,
    EventRef pkEvent, void*)
{
    ::Point kMouseLoc;    
    GetEventParameter(pkEvent,kEventParamMouseLocation,typeQDPoint,0,
        sizeof(kMouseLoc),0,&kMouseLoc);

    EventMouseButton eMouseButton;
    GetEventParameter(pkEvent,kEventParamMouseButton,typeMouseButton,0,
        sizeof(eMouseButton),0,&eMouseButton);

    UInt32 uiModifiers;
    GetEventParameter(pkEvent,kEventParamKeyModifiers,typeUInt32,0,
        sizeof(uiModifiers),0,&uiModifiers);

    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;

    CGrafPtr pkCurrPort;
    GetPort(&pkCurrPort);
    SetPortWindowPort((WindowRef)(pkTheApp->GetWindowID()));
    GlobalToLocal(&kMouseLoc);
    SetPort(pkCurrPort);

    pkTheApp->OnMotion(eMouseButton,kMouseLoc.h,kMouseLoc.v,uiModifiers);

    // allow standard handler to run
    return eventNotHandledErr;
}
//----------------------------------------------------------------------------
static pascal OSStatus ProcessMouseMoved (EventHandlerCallRef,
    EventRef pkEvent, void*)
{
    ::Point kMouseLoc;    
    GetEventParameter(pkEvent,kEventParamMouseLocation,typeQDPoint,0,
        sizeof(kMouseLoc),0,&kMouseLoc);

    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;

    CGrafPtr pkCurrPort;
    GetPort(&pkCurrPort);
    SetPortWindowPort((WindowRef)(pkTheApp->GetWindowID()));
    GlobalToLocal(&kMouseLoc);
    SetPort(pkCurrPort);

    pkTheApp->OnPassiveMotion(kMouseLoc.h,kMouseLoc.v);
    
    // allow standard handler to run
    return eventNotHandledErr;
}
//----------------------------------------------------------------------------
static pascal void ProcessTimer (EventLoopTimerRef, void*)
{
    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;

    pkTheApp->OnIdle();
}
//----------------------------------------------------------------------------
static OSErr QuitAppleEventHandler(const AppleEvent*, AppleEvent*, UInt32)
{
    QuitApplicationEventLoop();

    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;

    GWorldPtr pkBack;
    pkTheApp->GetExtraData(AGLAPP_BACK,sizeof(GWorldPtr),&pkBack);
    DisposeGWorld(pkBack);
    return noErr;
}
//----------------------------------------------------------------------------
int WindowApplication::Main (int, char**)
{
    InitCursor();
    
    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;
    assert(pkTheApp);
    if (!pkTheApp)
    {
        return -1;
    }
    
    // allocate temporary back buffer to be used for font management    
    GDHandle kDevice = GetGDevice();
    PixMapHandle kPixmap = (**kDevice).gdPMap;
    Rect kArea;
    GetPixBounds(kPixmap, &kArea);
    int iDepth = GetPixDepth(kPixmap);
    GWorldPtr pkBack;
    OSErr iError = NewGWorld(&pkBack,iDepth,&kArea,0,0,useTempMem | pixPurge);
    if (iError != noErr || !pkBack)
    {
        assert(false);
        return -2;
    }

    SetExtraData(AGLAPP_BACK,sizeof(GWorldPtr),&pkBack);
    
    // assign desired font settings to back buffer
    unsigned char aucFontName[256];
    strcpy((char*)&aucFontName[1],"Monaco");
    aucFontName[0] = 6;
    short sFont;
    GetFNum(aucFontName,&sFont);
    SetExtraData(AGLAPP_FONT,sizeof(short),&sFont);

    GWorldPtr pkCurrentWorld;
    GDHandle kCurrentDevice;
    GetGWorld(&pkCurrentWorld,&kCurrentDevice);
    SetGWorld(pkBack,0);

    TextFont(sFont);
    TextSize(9);
    TextFace(normal);
    SetGWorld(pkCurrentWorld,kCurrentDevice);
    
    // add standard window menu
    MenuRef pkMenu = 0;
    CreateStandardWindowMenu(0,&pkMenu);
    InsertMenu(pkMenu,0);
    
    // change current directory into application bundle.
    FSRef kProcessRef;
    FSCatalogInfo kProcessInfo;
    ProcessSerialNumber kPSN = { 0, kCurrentProcess };
    GetProcessBundleLocation(&kPSN,&kProcessRef);
    FSSpec kFileSpec;
    FSGetCatalogInfo(&kProcessRef,kFSCatInfoNodeFlags,&kProcessInfo,0,
        &kFileSpec,0);
    string kAppFile = GetStringPathname(kFileSpec);
    const int iMaxPathLen = 1024;
    char acPath[iMaxPathLen];
    strcpy(acPath,kAppFile.c_str());
    char* pcLast = strrchr(acPath,'/');
    *pcLast = 0;
    int iResult = chdir(acPath);
    assert(iResult == 0);
    
    // If the application is packaged, we have to get back up a couple of
    // levels such as the current working directory is the same as the
    // application's.    
    char acBuf[iMaxPathLen];
    char* acCurDir = getcwd(acBuf,iMaxPathLen);
    if (strstr(acCurDir,"/Contents/MacOS"))
    {
        iResult = chdir("../../..");
        assert(iResult == 0);
        acCurDir = getcwd(acBuf,iMaxPathLen);
    }

    // Launch a file dialog, if requested, when the application needs to
    // select an input file.  The derived-class application should set
    // m_bLaunchFileDialog to 'true' in its constructor when the dialog is
    // needed.
    if (pkTheApp->LaunchFileDialog())
    {
        char* acArgument = GetCommandLine();
        if (acArgument)
        {
            WM4_DELETE pkTheApp->TheCommand;
            pkTheApp->TheCommand = WM4_NEW Command(acArgument);
            WM4_DELETE[] acArgument;
        }
    }

    if (!pkTheApp->OnPrecreate())
    {
        return -3;
    }
    
    // require the window to have the specified client area
    short sWindowTop = 60, sWindowLeft = 40;
    Rect kRect = { sWindowTop, sWindowLeft,
        pkTheApp->GetHeight() + sWindowTop-1,
        pkTheApp->GetWidth() + sWindowLeft-1 };

    // create the application window
    WindowRef pkWindow = 0;
    OSStatus eErr = CreateNewWindow(kDocumentWindowClass, 
        kWindowStandardDocumentAttributes | kWindowStandardHandlerAttribute,
        &kRect,&pkWindow);
    if (eErr != noErr)
    {
        fprintf(stderr,"Create window error\n");
        return -4;
    }
    
    // Grab GD from kRect - based on FindGDHandleFromRect in Carbon SetupGL
    GDHandle hGD = 0;
    GDHandle hgdNthDevice = GetDeviceList();
    unsigned int uiGreatestArea = 0;

    // Check window against all gdRects in gDevice list and remember which
    // gdRect contains largest area of window.
    while (hgdNthDevice)
    {
        if (TestDeviceAttribute(hgdNthDevice,screenDevice)
        &&  TestDeviceAttribute(hgdNthDevice,screenActive))
        {
            // The SectRect routine calculates the intersection of the window
            // rectangle and this gDevice rectangle and returns TRUE if the
            // rectangles intersect, FALSE if they don't.
            Rect kRectSect;
            SectRect(&kRect,&(**hgdNthDevice).gdRect,&kRectSect);

            // Determine which screen holds greatest window area first,
            // calculate area of rectangle on current device.
            unsigned int uiSectArea = 
                (unsigned int)(kRectSect.right - kRectSect.left) * 
                (kRectSect.bottom - kRectSect.top);
            if (uiSectArea > uiGreatestArea)
            {
                uiGreatestArea = uiSectArea; // set greatest area so far
                hGD = hgdNthDevice; // set zoom device
            }
            hgdNthDevice = GetNextDevice(hgdNthDevice);
        }
    }

    // set window title
    CFStringRef pkWindowTitle = CFStringCreateWithCString(0, 
        pkTheApp->GetWindowTitle(),kCFStringEncodingASCII);
    SetWindowTitleWithCFString(pkWindow,pkWindowTitle);
    CFRelease(pkWindowTitle);

    pkTheApp->SetWindowID((int)pkWindow);

    SetPortWindowPort(pkWindow);
    pkTheApp->SetRenderer(WM4_NEW AglRenderer((AGLDevice)hGD,pkWindow,
        m_eFormat,m_eDepth,m_eStencil,m_eBuffering,m_eMultisampling,0,0,
        pkTheApp->GetWidth(),pkTheApp->GetHeight()));

    // install quit apple event handler
    eErr = AEInstallEventHandler(kCoreEventClass,kAEQuitApplication,
        NewAEEventHandlerUPP((AEEventHandlerProcPtr)QuitAppleEventHandler),0,
        false);
    if (eErr != noErr)
    {
        ExitToShell();
    }

    // install window close handler
    EventTypeSpec kEventType;
    kEventType.eventClass = kEventClassWindow;
    kEventType.eventKind = kEventWindowClose;
    EventHandlerUPP pfnHandlerUPP = NewEventHandlerUPP(ProcessWindowClose); 
    InstallWindowEventHandler(pkWindow,pfnHandlerUPP,1,&kEventType,0,0);

    // install window bounds change handler
    kEventType.eventKind = kEventWindowBoundsChanged;
    pfnHandlerUPP = NewEventHandlerUPP(ProcessWindowBoundsChange);
    InstallWindowEventHandler(pkWindow,pfnHandlerUPP,1,&kEventType,0,0);

    // install window zoomed handler
    kEventType.eventKind = kEventWindowZoomed;
    pfnHandlerUPP = NewEventHandlerUPP(ProcessWindowZoomed);
    InstallWindowEventHandler(pkWindow,pfnHandlerUPP,1,&kEventType,0,0);

    // install window redraw handler
    kEventType.eventKind = kEventWindowDrawContent;
    pfnHandlerUPP = NewEventHandlerUPP(ProcessWindowRedraw);
    InstallWindowEventHandler(pkWindow,pfnHandlerUPP,1,&kEventType,0,0);

    // install key down handler
    EventTypeSpec akEventType[2];
    akEventType[0].eventClass = kEventClassKeyboard;
    akEventType[0].eventKind = kEventRawKeyDown;
    akEventType[1].eventClass = kEventClassKeyboard;
    akEventType[1].eventKind = kEventRawKeyRepeat;
    pfnHandlerUPP = NewEventHandlerUPP(ProcessKeyDown);
    InstallWindowEventHandler(pkWindow,pfnHandlerUPP,2,akEventType,0,0);

    // install key up handler
    kEventType.eventClass = kEventClassKeyboard;
    kEventType.eventKind = kEventRawKeyUp;
    pfnHandlerUPP = NewEventHandlerUPP(ProcessKeyUp);
    InstallWindowEventHandler(pkWindow,pfnHandlerUPP,1,&kEventType,0,0);

    // install mouse down handler
    kEventType.eventClass = kEventClassMouse;
    kEventType.eventKind = kEventMouseDown;
    pfnHandlerUPP = NewEventHandlerUPP(ProcessMouseDown);
    InstallWindowEventHandler(pkWindow,pfnHandlerUPP,1,&kEventType,0,0);

    // install mouse up handler
    kEventType.eventKind = kEventMouseUp;
    pfnHandlerUPP = NewEventHandlerUPP(ProcessMouseUp);
    InstallWindowEventHandler(pkWindow,pfnHandlerUPP,1,&kEventType,0,0);

    // install mouse drag handler
    kEventType.eventKind = kEventMouseDragged;
    pfnHandlerUPP = NewEventHandlerUPP(ProcessMouseDragged);
    InstallWindowEventHandler(pkWindow,pfnHandlerUPP,1,&kEventType,0,0);

    // install mouse move handler
    kEventType.eventKind = kEventMouseMoved;
    pfnHandlerUPP = NewEventHandlerUPP(ProcessMouseMoved);
    InstallWindowEventHandler(pkWindow,pfnHandlerUPP,1,&kEventType,0,0);

    // create timer
    EventLoopTimerRef pkTimer;
    InstallEventLoopTimer(GetMainEventLoop(),0,kEventDurationMillisecond, 
        NewEventLoopTimerUPP(ProcessTimer),0,&pkTimer);

    if (!pkTheApp->OnInitialize())
    {
        return -5;
    }

    // set auto-repeat key timing
    short sKeyRepeatThresh = LMGetKeyRepThresh();
    short sKeyThresh = LMGetKeyThresh();
    LMSetKeyRepThresh(1);
    LMSetKeyThresh(30);

    // display the window
    ShowWindow(pkWindow);

    // run event loop
    RunApplicationEventLoop();

    // reset auto-repeat key timing to initial value
    LMSetKeyRepThresh(sKeyRepeatThresh);
    LMSetKeyThresh(sKeyThresh);

    pkTheApp->OnTerminate();

    RemoveEventLoopTimer(pkTimer);
    DisposeMenu(pkMenu);

    return 0;
}
//----------------------------------------------------------------------------
void Application::LaunchTreeControl (Spatial*, int, int, int, int)
{
    // TO DO
}
//----------------------------------------------------------------------------
void Application::ShutdownTreeControl ()
{
    // TO DO
}
//----------------------------------------------------------------------------

