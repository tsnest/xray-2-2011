#include <windows.h>
#include <direct.h>
#include <io.h>
#include <ctime>
#include "Command.h"
#include <cassert>
#include <iostream>
using namespace std;

// Disable the warnings about not using the "security" functions.
#pragma warning(disable : 4996)

const int g_iStringQuantity = 11;
const char* g_acUsage[g_iStringQuantity] =
{
"The Wild Magic Installer must be executed from the WildMagic4 folder in",
"the GeometricTools distribution.  The usage is:",
"    Wm4Installer",
"        -m [msvc71, msvc80, msvc80e, msvc90, msvc90e]",
"        -g [opengl, directx, soft]",
"        -c [debug, debugdll, debugmem, release, releasedll, releasemem]",
"All options are required.  For example, to use Microsoft Visual C++ 9.0,",
"OpenGL, static libraries, and release mode, you must execute:",
"    Wm4Installer -m msvc90 -g opengl -c release",
"The compiler location is looked up in the Windows Registry.  If it cannot",
"be found, the installer will display a message to that effect."
};

// compiler versions
#define M_INVALID -1
#define M_MSVC60   0
#define M_MSVC70   1
#define M_MSVC71   2
#define M_MSVC80   3
#define M_MSVC80E  4
#define M_MSVC90   5
#define M_MSVC90E  6

// graphics APIs
#define G_INVALID -1
#define G_OPENGL   0
#define G_DIRECTX  1
#define G_SOFT     2

// configurations
#define C_INVALID   -1
#define C_DEBUG      0
#define C_DEBUGDLL   1
#define C_DEBUGMEM   2
#define C_RELEASE    3
#define C_RELEASEDLL 4
#define C_RELEASEMEM 5
#define C_NOPCH      6

static const char* gs_acGrfs[3] = { "Wgl", "Dx9", "Sft" };
static const char* gs_acCfgs[7] =
{
    "Debug",
    "Debug DLL",
    "Debug Memory",
    "Release",
    "Release DLL",
    "Release Memory",
    "NoPCH"
};

static const char gs_acLogfile[] = "Wm4Installer.log";

//----------------------------------------------------------------------------
void Usage ()
{
    for (int i = 0; i < g_iStringQuantity; i++)
    {
        cout << g_acUsage[i] << endl;
    }
}
//----------------------------------------------------------------------------
int GetCompilerVersion (Command& rkCmd)
{
    char* acCompiler = 0;
    int iLocation = rkCmd.String("M",acCompiler);
    if (iLocation == 0)
    {
        iLocation = rkCmd.String("m",acCompiler);
        if (iLocation == 0)
        {
            return M_INVALID;
        }
    }

    int iLength = (int)strlen(acCompiler);
    for (int i = 0; i < iLength; i++)
    {
        tolower(acCompiler[i]);
    }

    // DEPRECATED.  To enable automatic compilation for these compilers, just
    // uncomment the relevant code blocks.

    //if (strcmp(acCompiler,"msvc60") == 0)
    //{
    //    delete[] acCompiler;
    //    return M_MSVC60;
    //}

    //if (strcmp(acCompiler,"msvc70") == 0)
    //{
    //    delete[] acCompiler;
    //    return M_MSVC70;
    //}

    if (strcmp(acCompiler,"msvc71") == 0)
    {
        delete[] acCompiler;
        return M_MSVC71;
    }

    if (strcmp(acCompiler,"msvc80") == 0)
    {
        delete[] acCompiler;
        return M_MSVC80;
    }

    if (strcmp(acCompiler,"msvc80e") == 0)
    {
        delete[] acCompiler;
        return M_MSVC80E;
    }

    if (strcmp(acCompiler,"msvc90") == 0)
    {
        delete[] acCompiler;
        return M_MSVC90;
    }

    if (strcmp(acCompiler,"msvc90e") == 0)
    {
        delete[] acCompiler;
        return M_MSVC90E;
    }

    delete[] acCompiler;
    return M_INVALID;
}
//----------------------------------------------------------------------------
int GetGraphicsAPI (Command& rkCmd)
{
    char* acGraphics = 0;
    int iLocation = rkCmd.String("G",acGraphics);
    if (iLocation == 0)
    {
        iLocation = rkCmd.String("g",acGraphics);
        if (iLocation == 0)
        {
            return G_INVALID;
        }
    }

    int iLength = (int)strlen(acGraphics);
    for (int i = 0; i < iLength; i++)
    {
        tolower(acGraphics[i]);
    }

    if (strcmp(acGraphics,"opengl") == 0)
    {
        delete[] acGraphics;
        return G_OPENGL;
    }

    if (strcmp(acGraphics,"directx") == 0)
    {
        delete[] acGraphics;
        return G_DIRECTX;
    }

    if (strcmp(acGraphics,"soft") == 0)
    {
        delete[] acGraphics;
        return G_SOFT;
    }

    delete[] acGraphics;
    return G_INVALID;
}
//----------------------------------------------------------------------------
int GetConfiguration (Command& rkCmd)
{
    char* acConfiguration = 0;
    int iLocation = rkCmd.String("C",acConfiguration);
    if (iLocation == 0)
    {
        iLocation = rkCmd.String("c",acConfiguration);
        if (iLocation == 0)
        {
            return C_INVALID;
        }
    }

    int iLength = (int)strlen(acConfiguration);
    for (int i = 0; i < iLength; i++)
    {
        tolower(acConfiguration[i]);
    }

    if (strcmp(acConfiguration,"debug") == 0)
    {
        delete[] acConfiguration;
        return C_DEBUG;
    }

    if (strcmp(acConfiguration,"debugdll") == 0)
    {
        delete[] acConfiguration;
        return C_DEBUGDLL;
    }

    if (strcmp(acConfiguration,"debugmem") == 0)
    {
        delete[] acConfiguration;
        return C_DEBUGMEM;
    }

    if (strcmp(acConfiguration,"release") == 0)
    {
        delete[] acConfiguration;
        return C_RELEASE;
    }

    if (strcmp(acConfiguration,"releasedll") == 0)
    {
        delete[] acConfiguration;
        return C_RELEASEDLL;
    }

    if (strcmp(acConfiguration,"releasemem") == 0)
    {
        delete[] acConfiguration;
        return C_RELEASEMEM;
    }

    if (strcmp(acConfiguration,"nopch") == 0)
    {
        delete[] acConfiguration;
        return C_NOPCH;
    }

    delete[] acConfiguration;
    return C_INVALID;
}
//----------------------------------------------------------------------------
bool GetPathToCompiler (int iCompiler, char* acPath)
{
    HKEY hKey;
    LONG lResult;
    DWORD dwType, dwSize;

    if (iCompiler == M_MSVC60)
    {
        lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
            "Software\\Microsoft\\VisualStudio\\6.0\\Setup",
            0,KEY_QUERY_VALUE,&hKey);
        if (lResult != ERROR_SUCCESS)
        {
            return false;
        }

        dwType = REG_SZ;
        dwSize = (DWORD)_MAX_PATH;
        lResult = RegQueryValueEx(hKey,"VsCommonDir",0,&dwType,
            (LPBYTE)acPath,&dwSize);
        if (lResult != ERROR_SUCCESS || dwSize > (DWORD)_MAX_PATH)
        {
            return false;
        }

        RegCloseKey(hKey);

        strcat(acPath,"\\MSDev98\\Bin\\msdev.com");
    }
    else if (iCompiler == M_MSVC70)
    {
        lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
            "Software\\Microsoft\\VisualStudio\\7.0\\Setup\\VS",
            0,KEY_QUERY_VALUE,&hKey);
        if (lResult != ERROR_SUCCESS)
        {
            return false;
        }

        dwType = REG_SZ;
        dwSize = (DWORD)_MAX_PATH;
        lResult = RegQueryValueEx(hKey,"EnvironmentPath",0,&dwType,
            (LPBYTE)acPath,&dwSize);
        if (lResult != ERROR_SUCCESS || dwSize > (DWORD)_MAX_PATH)
        {
            return false;
        }

        RegCloseKey(hKey);
    }
    else
    if (iCompiler == M_MSVC71)
    {
        lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
            "Software\\Microsoft\\VisualStudio\\7.1\\Setup\\VS",
            0,KEY_QUERY_VALUE,&hKey);
        if (lResult != ERROR_SUCCESS)
        {
            return false;
        }

        dwType = REG_SZ;
        dwSize = (DWORD)_MAX_PATH;
        lResult = RegQueryValueEx(hKey,"EnvironmentPath",0,&dwType,
            (LPBYTE)acPath,&dwSize);
        if (lResult != ERROR_SUCCESS || dwSize > (DWORD)_MAX_PATH)
        {
            return false;
        }

        RegCloseKey(hKey);
    }
    else if (iCompiler == M_MSVC80)
    {
        lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
            "Software\\Microsoft\\VisualStudio\\8.0\\Setup\\VS",
            0,KEY_QUERY_VALUE,&hKey);
        if (lResult != ERROR_SUCCESS)
        {
            return false;
        }

        dwType = REG_SZ;
        dwSize = (DWORD)_MAX_PATH;
        lResult = RegQueryValueEx(hKey,"EnvironmentPath",0,&dwType,
            (LPBYTE)acPath,&dwSize);
        if (lResult != ERROR_SUCCESS || dwSize > (DWORD)_MAX_PATH)
        {
            return false;
        }

        RegCloseKey(hKey);
    }
    else if (iCompiler == M_MSVC80E)
    {
        lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
            "Software\\Microsoft\\VisualStudio\\8.0\\Setup",
            0,KEY_QUERY_VALUE,&hKey);
        if (lResult != ERROR_SUCCESS)
        {
            return false;
        }

        dwType = REG_SZ;
        dwSize = (DWORD)_MAX_PATH;
        lResult = RegQueryValueEx(hKey,"Dbghelp_path",0,&dwType,
            (LPBYTE)acPath,&dwSize);
        if (lResult != ERROR_SUCCESS || dwSize > (DWORD)_MAX_PATH)
        {
            return false;
        }

        strcat(acPath,"VCExpress.exe");

        RegCloseKey(hKey);
    }
    else if (iCompiler == M_MSVC90)
    {
        lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
            "Software\\Microsoft\\VisualStudio\\9.0\\Setup\\VS",
            0,KEY_QUERY_VALUE,&hKey);
        if (lResult != ERROR_SUCCESS)
        {
            return false;
        }

        dwType = REG_SZ;
        dwSize = (DWORD)_MAX_PATH;
        lResult = RegQueryValueEx(hKey,"EnvironmentPath",0,&dwType,
            (LPBYTE)acPath,&dwSize);
        if (lResult != ERROR_SUCCESS || dwSize > (DWORD)_MAX_PATH)
        {
            return false;
        }

        RegCloseKey(hKey);
    }
    else if (iCompiler == M_MSVC90E)
    {
        lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
            "Software\\Microsoft\\VisualStudio\\9.0\\Setup",
            0,KEY_QUERY_VALUE,&hKey);
        if (lResult != ERROR_SUCCESS)
        {
            return false;
        }

        dwType = REG_SZ;
        dwSize = (DWORD)_MAX_PATH;
        lResult = RegQueryValueEx(hKey,"Dbghelp_path",0,&dwType,
            (LPBYTE)acPath,&dwSize);
        if (lResult != ERROR_SUCCESS || dwSize > (DWORD)_MAX_PATH)
        {
            return false;
        }

        strcat(acPath,"VCExpress.exe");

        RegCloseKey(hKey);
    }
    else
    {
        assert(false);
    }

    return true;
}
//----------------------------------------------------------------------------
void BuildSamples60 (const char* pcExe, const char* pcGrf, const char* pcCfg)
{
    char acString[_MAX_PATH];

    struct _finddata_t kFileInfo;
    intptr_t hFile = _findfirst("*",&kFileInfo);
    while (hFile != -1)
    {
        if (kFileInfo.attrib & _A_SUBDIR)
        {
            char* acName = kFileInfo.name;
            if (strcmp(acName,".") != 0 && strcmp(acName,"..") != 0)
            {
                _chdir(acName);

                sprintf(acString,"%s_VC60.dsw",acName);
                if (_access(acString,0) == 0)
                {
                    sprintf(acString,"del /f %s_VC60.dsw",acName);
                    system(acString);
                }

                sprintf(acString,"%s_VC60.opt",acName);
                if (_access(acString,0) == 0)
                {
                    sprintf(acString,"del /f %s_VC60.opt",acName);
                    system(acString);
                }

                sprintf(acString,"%s_VC60.ncb",acName);
                if (_access(acString,0) == 0)
                {
                    sprintf(acString,"del /f %s_VC60.ncb",acName);
                    system(acString);
                }

                sprintf(acString,
                    "%s %s_VC60.dsp /MAKE \"%s_VC60 - Win32 %s %s\"",
                    pcExe,acName,acName,pcGrf,pcCfg);
                system(acString);
                _chdir("..");
            }
        }

        if (_findnext(hFile,&kFileInfo) != 0)
        {
            break;
        }
    }
}
//----------------------------------------------------------------------------
void BuildSamples70 (const char* pcExe, const char* pcGrf, const char* pcCfg)
{
    char acString[_MAX_PATH];

    struct _finddata_t kFileInfo;
    intptr_t hFile = _findfirst("*",&kFileInfo);
    while (hFile != -1)
    {
        if (kFileInfo.attrib & _A_SUBDIR)
        {
            char* acName = kFileInfo.name;
            if (strcmp(acName,".") != 0 && strcmp(acName,"..") != 0)
            {
                _chdir(acName);

                sprintf(acString,"%s_VC70.sln",acName);
                if (_access(acString,0) == 0)
                {
                    sprintf(acString,"del /f %s_VC70.sln",acName);
                    system(acString);
                }

                sprintf(acString,"%s_VC70.suo",acName);
                if (_access(acString,0) == 0)
                {
                    sprintf(acString,"del /f %s_VC70.suo",acName);
                    system(acString);
                }

                sprintf(acString,"%s_VC70.ncb",acName);
                if (_access(acString,0) == 0)
                {
                    sprintf(acString,"del /f %s_VC70.ncb",acName);
                    system(acString);
                }

                sprintf(acString,"%s %s_VC70.vcproj /build \"%s %s\"",
                    pcExe,acName,pcGrf,pcCfg);
                system(acString);
                _chdir("..");
            }
        }

        if (_findnext(hFile,&kFileInfo) != 0)
        {
            break;
        }
    }
}
//----------------------------------------------------------------------------
void BuildSamples71 (const char* pcExe, const char* pcGrf, const char* pcCfg)
{
    char acString[_MAX_PATH];

    struct _finddata_t kFileInfo;
    intptr_t hFile = _findfirst("*",&kFileInfo);
    while (hFile != -1)
    {
        if (kFileInfo.attrib & _A_SUBDIR)
        {
            char* acName = kFileInfo.name;
            if (strcmp(acName,".") != 0 && strcmp(acName,"..") != 0)
            {
                _chdir(acName);

                sprintf(acString,"%s_VC71.sln",acName);
                if (_access(acName,0) == 0)
                {
                    sprintf(acString,"del /f %s_VC71.sln",acName);
                    system(acString);
                }

                sprintf(acString,"%s_VC71.suo",acName);
                if (_access(acName,0) == 0)
                {
                    sprintf(acString,"del /f %s_VC71.suo",acName);
                    system(acString);
                }

                sprintf(acString,"%s_VC71.ncb",acName);
                if (_access(acName,0) == 0)
                {
                    sprintf(acString,"del /f %s_VC71.ncb",acName);
                    system(acString);
                }

                sprintf(acString,"%s %s_VC71.vcproj /build \"%s %s\"",
                    pcExe,acName,pcGrf,pcCfg);
                system(acString);
                _chdir("..");
            }
        }

        if (_findnext(hFile,&kFileInfo) != 0)
        {
            break;
        }
    }
}
//----------------------------------------------------------------------------
void BuildSamples80 (const char* pcExe, const char* pcGrf, const char* pcCfg)
{
    char acString[_MAX_PATH];

    struct _finddata_t kFileInfo;
    intptr_t hFile = _findfirst("*",&kFileInfo);
    while (hFile != -1)
    {
        if (kFileInfo.attrib & _A_SUBDIR)
        {
            char* acName = kFileInfo.name;
            if (strcmp(acName,".") != 0 && strcmp(acName,"..") != 0)
            {
                _chdir(acName);

                sprintf(acString,"%s_VC80.sln",acName);
                if (_access(acName,0) == 0)
                {
                    sprintf(acString,"del /f %s_VC80.sln",acName);
                    system(acString);
                }

                sprintf(acString,"%s_VC80.suo",acName);
                if (_access(acName,0) == 0)
                {
                    sprintf(acString,"del /f %s_VC80.suo",acName);
                    system(acString);
                }

                sprintf(acString,"%s_VC80.ncb",acName);
                if (_access(acName,0) == 0)
                {
                    sprintf(acString,"del /f %s_VC80.ncb",acName);
                    system(acString);
                }

                sprintf(acString,"%s %s_VC80.vcproj /build \"%s %s\"",
                    pcExe,acName,pcGrf,pcCfg);
                system(acString);
                _chdir("..");
            }
        }

        if (_findnext(hFile,&kFileInfo) != 0)
        {
            break;
        }
    }
}
//----------------------------------------------------------------------------
void BuildSamples90 (const char* pcExe, const char* pcGrf, const char* pcCfg)
{
    char acString[_MAX_PATH];

    struct _finddata_t kFileInfo;
    intptr_t hFile = _findfirst("*",&kFileInfo);
    while (hFile != -1)
    {
        if (kFileInfo.attrib & _A_SUBDIR)
        {
            char* acName = kFileInfo.name;
            if (strcmp(acName,".") != 0 && strcmp(acName,"..") != 0)
            {
                _chdir(acName);

                sprintf(acString,"%s_VC90.sln",acName);
                if (_access(acName,0) == 0)
                {
                    sprintf(acString,"del /f %s_VC90.sln",acName);
                    system(acString);
                }

                sprintf(acString,"%s_VC90.suo",acName);
                if (_access(acName,0) == 0)
                {
                    sprintf(acString,"del /f %s_VC90.suo",acName);
                    system(acString);
                }

                sprintf(acString,"%s_VC90.ncb",acName);
                if (_access(acName,0) == 0)
                {
                    sprintf(acString,"del /f %s_VC90.ncb",acName);
                    system(acString);
                }

                sprintf(acString,"%s %s_VC90.vcproj /build \"%s %s\"",
                    pcExe,acName,pcGrf,pcCfg);
                system(acString);
                _chdir("..");
            }
        }

        if (_findnext(hFile,&kFileInfo) != 0)
        {
            break;
        }
    }
}
//----------------------------------------------------------------------------
void Build60 (const char* pcExe, const char* pcGrf, const char* pcCfg)
{
    char acString[_MAX_PATH];

    _chdir("LibFoundation");
    sprintf(acString,
        "%s LibFoundation_VC60.dsp /MAKE \"LibFoundation_VC60 - Win32 %s\"",
        pcExe,pcCfg);
    system(acString);
    _chdir("..");

    _chdir("LibImagics");
    sprintf(acString,
        "%s LibImagics_VC60.dsp /MAKE \"LibImagics_VC60 - Win32 %s\"",
        pcExe,pcCfg);
    system(acString);
    _chdir("..");

    _chdir("LibGraphics");
    sprintf(acString,
        "%s LibGraphics_VC60.dsp /MAKE \"LibGraphics_VC60 - Win32 %s\"",
        pcExe,pcCfg);
    system(acString);
    _chdir("..");

    _chdir("LibPhysics");
    sprintf(acString,
        "%s LibPhysics_VC60.dsp /MAKE \"LibPhysics_VC60 - Win32 %s\"",
        pcExe,pcCfg);
    system(acString);
    _chdir("..");

    if (strcmp("Wgl",pcGrf) == 0)
    {
        _chdir("LibRenderers\\OpenGLRenderer");
        sprintf(acString,
            "%s WglRenderer_VC60.dsp /MAKE \"WglRenderer_VC60 - Win32 %s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");

        _chdir("LibApplications\\OpenGLApplication");
        sprintf(acString,
            "%s WglApplication_VC60.dsp /MAKE \"WglApplication_VC60 - Win32 %s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");
    }
    else if (strcmp("Dx9",pcGrf) == 0)
    {
        _chdir("LibRenderers\\Dx9Renderer");
        sprintf(acString,
            "%s Dx9Renderer_VC60.dsp /MAKE \"Dx9Renderer_VC60 - Win32 %s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");

        _chdir("LibApplications\\Dx9Application");
        sprintf(acString,
            "%s Dx9Application_VC60.dsp /MAKE \"Dx9Application_VC60 - Win32 %s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");
    }
    else // Sft
    {
        _chdir("LibRenderers\\SoftRenderer");
        sprintf(acString,
            "%s WinSoftRenderer_VC60.dsp /MAKE \"WinSoftRenderer_VC60 - Win32 %s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");

        _chdir("LibApplications\\SoftApplication");
        sprintf(acString,
            "%s WinSoftApplication_VC60.dsp /MAKE \"WinSoftApplication_VC60 - Win32 %s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");
    }

    _chdir("SampleFoundation");
    BuildSamples60(pcExe,pcGrf,pcCfg);
    _chdir("..");

    _chdir("SampleImagics");
    BuildSamples60(pcExe,pcGrf,pcCfg);
    _chdir("..");

    _chdir("SampleGraphics");
    BuildSamples60(pcExe,pcGrf,pcCfg);
    _chdir("..");

    _chdir("SamplePhysics");
    BuildSamples60(pcExe,pcGrf,pcCfg);
    _chdir("..");
}
//----------------------------------------------------------------------------
void Build70 (const char* pcExe, const char* pcGrf, const char* pcCfg)
{
    char acString[_MAX_PATH];

    _chdir("LibFoundation");
    sprintf(acString,
        "%s LibFoundation_VC70.vcproj /build \"%s\"",
        pcExe,pcCfg);
    system(acString);
    _chdir("..");

    _chdir("LibImagics");
    sprintf(acString,
        "%s LibImagics_VC70.vcproj /build \"%s\"",
        pcExe,pcCfg);
    system(acString);
    _chdir("..");

    // VC70 has a bug in its precompiled header system.  It generates error
    // C2908 in the SharedArrays cpp files.  If you build a second time, the
    // cpp files compile.
    _chdir("LibGraphics");

    // Error C2908 generated on first pass.
    sprintf(acString,
        "%s LibGraphics_VC70.vcproj /build \"%s\"",
        pcExe,pcCfg);
    system(acString);

    // Files compile correctly on second pass.
    sprintf(acString,
        "%s LibGraphics_VC70.vcproj /build \"%s\"",
        pcExe,pcCfg);
    system(acString);
    _chdir("..");

    _chdir("LibPhysics");
    sprintf(acString,
        "%s LibPhysics_VC70.vcproj /build \"%s\"",
        pcExe,pcCfg);
    system(acString);
    _chdir("..");

    if (strcmp("Wgl",pcGrf) == 0)
    {
        _chdir("LibRenderers\\OpenGLRenderer");
        sprintf(acString,
            "%s WglRenderer_VC70.vcproj /build \"%s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");

        _chdir("LibApplications\\OpenGLApplication");
        sprintf(acString,
            "%s WglApplication_VC70.vcproj /build \"%s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");
    }
    else if (strcmp("Dx9",pcGrf) == 0)
    {
        _chdir("LibRenderers\\Dx9Renderer");
        sprintf(acString,
            "%s Dx9Renderer_VC70.vcproj /build \"%s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");

        _chdir("LibApplications\\Dx9Application");
        sprintf(acString,
            "%s Dx9Application_VC70.vcproj /build \"%s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");
    }
    else  // Sft
    {
        _chdir("LibRenderers\\SoftRenderer");
        sprintf(acString,
            "%s WinSoftRenderer_VC70.vcproj /build \"%s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");

        _chdir("LibApplications\\SoftApplication");
        sprintf(acString,
            "%s WinSoftApplication_VC70.vcproj /build \"%s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");
    }

    _chdir("SampleFoundation");
    BuildSamples70(pcExe,pcGrf,pcCfg);
    _chdir("..");

    _chdir("SampleImagics");
    BuildSamples70(pcExe,pcGrf,pcCfg);
    _chdir("..");

    _chdir("SampleGraphics");
    BuildSamples70(pcExe,pcGrf,pcCfg);
    _chdir("..");

    _chdir("SamplePhysics");
    BuildSamples70(pcExe,pcGrf,pcCfg);
    _chdir("..");
}
//----------------------------------------------------------------------------
void Build71 (const char* pcExe, const char* pcGrf, const char* pcCfg)
{
    char acString[_MAX_PATH];

    _chdir("LibFoundation");
    sprintf(acString,
        "%s LibFoundation_VC71.vcproj /build \"%s\"",
        pcExe,pcCfg);
    system(acString);
    _chdir("..");

    _chdir("LibImagics");
    sprintf(acString,
        "%s LibImagics_VC71.vcproj /build \"%s\"",
        pcExe,pcCfg);
    system(acString);
    _chdir("..");

    _chdir("LibGraphics");
    sprintf(acString,
        "%s LibGraphics_VC71.vcproj /build \"%s\"",
        pcExe,pcCfg);
    system(acString);
    _chdir("..");

    _chdir("LibPhysics");
    sprintf(acString,
        "%s LibPhysics_VC71.vcproj /build \"%s\"",
        pcExe,pcCfg);
    system(acString);
    _chdir("..");

    if (strcmp("Wgl",pcGrf) == 0)
    {
        _chdir("LibRenderers\\OpenGLRenderer");
        sprintf(acString,
            "%s WglRenderer_VC71.vcproj /build \"%s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");

        _chdir("LibApplications\\OpenGLApplication");
        sprintf(acString,
            "%s WglApplication_VC71.vcproj /build \"%s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");
    }
    else if (strcmp("Dx9",pcGrf) == 0)
    {
        _chdir("LibRenderers\\Dx9Renderer");
        sprintf(acString,
            "%s Dx9Renderer_VC71.vcproj /build \"%s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");

        _chdir("LibApplications\\Dx9Application");
        sprintf(acString,
            "%s Dx9Application_VC71.vcproj /build \"%s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");
    }
    else  // Sft
    {
        _chdir("LibRenderers\\SoftRenderer");
        sprintf(acString,
            "%s WinSoftRenderer_VC71.vcproj /build \"%s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");

        _chdir("LibApplications\\SoftApplication");
        sprintf(acString,
            "%s WinSoftApplication_VC71.vcproj /build \"%s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");
    }

    _chdir("SampleFoundation");
    BuildSamples71(pcExe,pcGrf,pcCfg);
    _chdir("..");

    _chdir("SampleImagics");
    BuildSamples71(pcExe,pcGrf,pcCfg);
    _chdir("..");

    _chdir("SampleGraphics");
    BuildSamples71(pcExe,pcGrf,pcCfg);
    _chdir("..");

    _chdir("SamplePhysics");
    BuildSamples71(pcExe,pcGrf,pcCfg);
    _chdir("..");
}
//----------------------------------------------------------------------------
void Build80 (const char* pcExe, const char* pcGrf, const char* pcCfg)
{
    char acString[_MAX_PATH];

    _chdir("LibFoundation");
    sprintf(acString,
        "%s LibFoundation_VC80.vcproj /build \"%s\"",
        pcExe,pcCfg);
    system(acString);
    _chdir("..");

    _chdir("LibImagics");
    sprintf(acString,
        "%s LibImagics_VC80.vcproj /build \"%s\"",
        pcExe,pcCfg);
    system(acString);
    _chdir("..");

    _chdir("LibGraphics");
    sprintf(acString,
        "%s LibGraphics_VC80.vcproj /build \"%s\"",
        pcExe,pcCfg);
    system(acString);
    _chdir("..");

    _chdir("LibPhysics");
    sprintf(acString,
        "%s LibPhysics_VC80.vcproj /build \"%s\"",
        pcExe,pcCfg);
    system(acString);
    _chdir("..");

    if (strcmp("Wgl",pcGrf) == 0)
    {
        _chdir("LibRenderers\\OpenGLRenderer");
        sprintf(acString,
            "%s WglRenderer_VC80.vcproj /build \"%s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");

        _chdir("LibApplications\\OpenGLApplication");
        sprintf(acString,
            "%s WglApplication_VC80.vcproj /build \"%s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");
    }
    else if (strcmp("Dx9",pcGrf) == 0)
    {
        _chdir("LibRenderers\\Dx9Renderer");
        sprintf(acString,
            "%s Dx9Renderer_VC80.vcproj /build \"%s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");

        _chdir("LibApplications\\Dx9Application");
        sprintf(acString,
            "%s Dx9Application_VC80.vcproj /build \"%s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");
    }
    else  // Sft
    {
        _chdir("LibRenderers\\SoftRenderer");
        sprintf(acString,
            "%s WinSoftRenderer_VC80.vcproj /build \"%s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");

        _chdir("LibApplications\\SoftApplication");
        sprintf(acString,
            "%s WinSoftApplication_VC80.vcproj /build \"%s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");
    }

    _chdir("SampleFoundation");
    BuildSamples80(pcExe,pcGrf,pcCfg);
    _chdir("..");

    _chdir("SampleImagics");
    BuildSamples80(pcExe,pcGrf,pcCfg);
    _chdir("..");

    _chdir("SampleGraphics");
    BuildSamples80(pcExe,pcGrf,pcCfg);
    _chdir("..");

    _chdir("SamplePhysics");
    BuildSamples80(pcExe,pcGrf,pcCfg);
    _chdir("..");
}
//----------------------------------------------------------------------------
void Build90 (const char* pcExe, const char* pcGrf, const char* pcCfg)
{
    char acString[_MAX_PATH];

    _chdir("LibFoundation");
    sprintf(acString,
        "%s LibFoundation_VC90.vcproj /build \"%s\"",
        pcExe,pcCfg);
    system(acString);
    _chdir("..");

    _chdir("LibImagics");
    sprintf(acString,
        "%s LibImagics_VC90.vcproj /build \"%s\"",
        pcExe,pcCfg);
    system(acString);
    _chdir("..");

    _chdir("LibGraphics");
    sprintf(acString,
        "%s LibGraphics_VC90.vcproj /build \"%s\"",
        pcExe,pcCfg);
    system(acString);
    _chdir("..");

    _chdir("LibPhysics");
    sprintf(acString,
        "%s LibPhysics_VC90.vcproj /build \"%s\"",
        pcExe,pcCfg);
    system(acString);
    _chdir("..");

    if (strcmp("Wgl",pcGrf) == 0)
    {
        _chdir("LibRenderers\\OpenGLRenderer");
        sprintf(acString,
            "%s WglRenderer_VC90.vcproj /build \"%s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");

        _chdir("LibApplications\\OpenGLApplication");
        sprintf(acString,
            "%s WglApplication_VC90.vcproj /build \"%s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");
    }
    else if (strcmp("Dx9",pcGrf) == 0)
    {
        _chdir("LibRenderers\\Dx9Renderer");
        sprintf(acString,
            "%s Dx9Renderer_VC90.vcproj /build \"%s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");

        _chdir("LibApplications\\Dx9Application");
        sprintf(acString,
            "%s Dx9Application_VC90.vcproj /build \"%s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");
    }
    else  // Sft
    {
        _chdir("LibRenderers\\SoftRenderer");
        sprintf(acString,
            "%s WinSoftRenderer_VC90.vcproj /build \"%s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");

        _chdir("LibApplications\\SoftApplication");
        sprintf(acString,
            "%s WinSoftApplication_VC90.vcproj /build \"%s\"",
            pcExe,pcCfg);
        system(acString);
        _chdir("..\\..");
    }

    _chdir("SampleFoundation");
    BuildSamples90(pcExe,pcGrf,pcCfg);
    _chdir("..");

    _chdir("SampleImagics");
    BuildSamples90(pcExe,pcGrf,pcCfg);
    _chdir("..");

    _chdir("SampleGraphics");
    BuildSamples90(pcExe,pcGrf,pcCfg);
    _chdir("..");

    _chdir("SamplePhysics");
    BuildSamples90(pcExe,pcGrf,pcCfg);
    _chdir("..");
}
//----------------------------------------------------------------------------
double GetMegabytesAvailable ()
{
    _diskfree_t kDriveInfo;
    _getdiskfree((unsigned int)_getdrive(),&kDriveInfo);

    double dBytesPerSector = (double)kDriveInfo.bytes_per_sector;
    double dSectorsPerCluster = (double)kDriveInfo.sectors_per_cluster;
    double dAvailClusters = (double)kDriveInfo.avail_clusters;
    double dBytes = dBytesPerSector*dSectorsPerCluster*dAvailClusters;
    dBytes /= 1048576.0;  // dBytes /= (1024*1024)

    return dBytes;
}
//----------------------------------------------------------------------------
int main (int iArgQuantity, char** apcArgument)
{
    int iErrorCode = 0;

    if (iArgQuantity <= 1)
    {
        Usage();
        return ++iErrorCode;
    }

    // Make sure the current working directory is WildMagic4.
    char acCWDPath[_MAX_PATH];
    if (!_getcwd(acCWDPath,_MAX_PATH))
    {
        cout << "Cannot find current working directory." << endl;
        return ++iErrorCode;
    }

    const char* pcCWD = strrchr(acCWDPath,'\\');
    if (!pcCWD || *(++pcCWD) == 0)
    {
        cout << "Cannot find current working directory." << endl;
        return ++iErrorCode;
    }

    if (stricmp("WildMagic4",pcCWD) != 0)
    {
        cout << "The current working directory must be WildMagic4." << endl;
        return ++iErrorCode;
    }

    Command kCmd(iArgQuantity,apcArgument);

    int iCmp = GetCompilerVersion(kCmd);
    if (iCmp == M_INVALID)
    {
        cout << "Either -M was not specified or the parameter is incorrect."
             << endl;
        return ++iErrorCode;
    }

    int iGrf = GetGraphicsAPI(kCmd);
    if (iGrf == G_INVALID)
    {
        cout << "Either -G was not specified or the parameter is incorrect."
             << endl;
        return ++iErrorCode;
    }

    int iCfg = GetConfiguration(kCmd);
    if (iCfg == C_INVALID)
    {
        cout << "Either -C was not specified or the parameter is incorrect."
             << endl;
        return ++iErrorCode;
    }

    if (iCmp == M_MSVC60 && (iCfg == C_DEBUGDLL || iCfg == C_RELEASEDLL))
    {
        cout << "The VC6 builds do not support dynamic libraries." << endl;
        return ++iErrorCode;
    }

    char acPath[_MAX_PATH];
    if (!GetPathToCompiler(iCmp,acPath))
    {
        cout << "Cannot find the specified compiler." << endl;
        return ++iErrorCode;
    }

    // Convert the path to a short name to keep the command line statements
    // to a minimum in length.
    char acShortPath[_MAX_PATH];
    DWORD dwResult = GetShortPathName(acPath,acShortPath,_MAX_PATH);
    if (dwResult == 0 || dwResult > _MAX_PATH)
    {
        cout << "Failed to get compiler's short path name." << endl;
        return ++iErrorCode;
    }

    if (iCmp != M_MSVC60 && iCmp != M_MSVC80E && iCmp != M_MSVC90E)
    {
        // Truncate the .exe so that devenv.com runs.  This allows output to
        // the console window.  The VC8.0/9.0 Express executables are
        // VCExpress.exe and cannot have their extension truncated.
        int iLength = (int)strlen(acShortPath);
        assert(acShortPath[iLength-4] == '.');
        acShortPath[iLength-4] = 0;
    }

    const char* pcGrf = gs_acGrfs[iGrf];
    const char* pcCfg = gs_acCfgs[iCfg];

    FILE* pkLog;
    time_t iTime;
    char* acTime;
    int iLengthM1;

    double dMegabytesStart = GetMegabytesAvailable();

    if (iCmp == M_MSVC60)
    {
        time(&iTime);
        acTime = ctime(&iTime);
        iLengthM1 = (int)(strlen(acTime) - 1);
        if (acTime[iLengthM1] == '\n')
        {
            acTime[iLengthM1] = 0;
        }
        pkLog = fopen(gs_acLogfile,"at");
        fprintf(pkLog,"%s Start build for MSVC60 %s %s\n",acTime,pcGrf,pcCfg);
        fclose(pkLog);

        Build60(acShortPath,pcGrf,pcCfg);

        time(&iTime);
        acTime = ctime(&iTime);
        iLengthM1 = (int)(strlen(acTime) - 1);
        if (acTime[iLengthM1] == '\n')
        {
            acTime[iLengthM1] = 0;
        }
        pkLog = fopen(gs_acLogfile,"at");
        fprintf(pkLog,"%s End build for MSVC60 %s %s\n",acTime,pcGrf,pcCfg);
        fclose(pkLog);
    }
    else if (iCmp == M_MSVC70)
    {
        time(&iTime);
        acTime = ctime(&iTime);
        iLengthM1 = (int)(strlen(acTime) - 1);
        if (acTime[iLengthM1] == '\n')
        {
            acTime[iLengthM1] = 0;
        }
        pkLog = fopen(gs_acLogfile,"at");
        fprintf(pkLog,"%s Start build for MSVC70 %s %s\n",acTime,pcGrf,pcCfg);
        fclose(pkLog);

        Build70(acShortPath,pcGrf,pcCfg);

        time(&iTime);
        acTime = ctime(&iTime);
        iLengthM1 = (int)(strlen(acTime) - 1);
        if (acTime[iLengthM1] == '\n')
        {
            acTime[iLengthM1] = 0;
        }
        pkLog = fopen(gs_acLogfile,"at");
        fprintf(pkLog,"%s End build for MSVC70 %s %s\n",acTime,pcGrf,pcCfg);
        fclose(pkLog);
    }
    else if (iCmp == M_MSVC71)
    {
        time(&iTime);
        acTime = ctime(&iTime);
        iLengthM1 = (int)(strlen(acTime) - 1);
        if (acTime[iLengthM1] == '\n')
        {
            acTime[iLengthM1] = 0;
        }
        pkLog = fopen(gs_acLogfile,"at");
        fprintf(pkLog,"%s Start build for MSVC71 %s %s\n",acTime,pcGrf,pcCfg);
        fclose(pkLog);

        Build71(acShortPath,pcGrf,pcCfg);

        time(&iTime);
        acTime = ctime(&iTime);
        iLengthM1 = (int)(strlen(acTime) - 1);
        if (acTime[iLengthM1] == '\n')
        {
            acTime[iLengthM1] = 0;
        }
        pkLog = fopen(gs_acLogfile,"at");
        fprintf(pkLog,"%s End build for MSVC71 %s %s\n",acTime,pcGrf,pcCfg);
        fclose(pkLog);
    }
    else if (iCmp == M_MSVC80)
    {
        time(&iTime);
        acTime = ctime(&iTime);
        iLengthM1 = (int)(strlen(acTime) - 1);
        if (acTime[iLengthM1] == '\n')
        {
            acTime[iLengthM1] = 0;
        }
        pkLog = fopen(gs_acLogfile,"at");
        fprintf(pkLog,"%s Start build for MSVC80 %s %s\n",acTime,pcGrf,pcCfg);
        fclose(pkLog);

        Build80(acShortPath,pcGrf,pcCfg);

        time(&iTime);
        acTime = ctime(&iTime);
        iLengthM1 = (int)(strlen(acTime) - 1);
        if (acTime[iLengthM1] == '\n')
        {
            acTime[iLengthM1] = 0;
        }
        pkLog = fopen(gs_acLogfile,"at");
        fprintf(pkLog,"%s End build for MSVC80 %s %s\n",acTime,pcGrf,pcCfg);
        fclose(pkLog);
    }
    else if (iCmp == M_MSVC80E)
    {
        time(&iTime);
        acTime = ctime(&iTime);
        iLengthM1 = (int)(strlen(acTime) - 1);
        if (acTime[iLengthM1] == '\n')
        {
            acTime[iLengthM1] = 0;
        }
        pkLog = fopen(gs_acLogfile,"at");
        fprintf(pkLog,"%s Start build for MSVC80E %s %s\n",acTime,pcGrf,pcCfg);
        fclose(pkLog);

        Build80(acShortPath,pcGrf,pcCfg);

        time(&iTime);
        acTime = ctime(&iTime);
        iLengthM1 = (int)(strlen(acTime) - 1);
        if (acTime[iLengthM1] == '\n')
        {
            acTime[iLengthM1] = 0;
        }
        pkLog = fopen(gs_acLogfile,"at");
        fprintf(pkLog,"%s End build for MSVC80E %s %s\n",acTime,pcGrf,pcCfg);
        fclose(pkLog);
    }
    else if (iCmp == M_MSVC90)
    {
        time(&iTime);
        acTime = ctime(&iTime);
        iLengthM1 = (int)(strlen(acTime) - 1);
        if (acTime[iLengthM1] == '\n')
        {
            acTime[iLengthM1] = 0;
        }
        pkLog = fopen(gs_acLogfile,"at");
        fprintf(pkLog,"%s Start build for MSVC90 %s %s\n",acTime,pcGrf,pcCfg);
        fclose(pkLog);

        Build90(acShortPath,pcGrf,pcCfg);

        time(&iTime);
        acTime = ctime(&iTime);
        iLengthM1 = (int)(strlen(acTime) - 1);
        if (acTime[iLengthM1] == '\n')
        {
            acTime[iLengthM1] = 0;
        }
        pkLog = fopen(gs_acLogfile,"at");
        fprintf(pkLog,"%s End build for MSVC90 %s %s\n",acTime,pcGrf,pcCfg);
        fclose(pkLog);
    }
    else if (iCmp == M_MSVC90E)
    {
        time(&iTime);
        acTime = ctime(&iTime);
        iLengthM1 = (int)(strlen(acTime) - 1);
        if (acTime[iLengthM1] == '\n')
        {
            acTime[iLengthM1] = 0;
        }
        pkLog = fopen(gs_acLogfile,"at");
        fprintf(pkLog,"%s Start build for MSVC90 %s %s\n",acTime,pcGrf,pcCfg);
        fclose(pkLog);

        Build90(acShortPath,pcGrf,pcCfg);

        time(&iTime);
        acTime = ctime(&iTime);
        iLengthM1 = (int)(strlen(acTime) - 1);
        if (acTime[iLengthM1] == '\n')
        {
            acTime[iLengthM1] = 0;
        }
        pkLog = fopen(gs_acLogfile,"at");
        fprintf(pkLog,"%s End build for MSVC90 %s %s\n",acTime,pcGrf,pcCfg);
        fclose(pkLog);
    }
    else
    {
        assert(false);
    }

    double dMegabytesFinal = GetMegabytesAvailable();
    int iMegabytesUsed = (int)(dMegabytesStart - dMegabytesFinal + 0.5);
    pkLog = fopen(gs_acLogfile,"at");
    fprintf(pkLog,"disk megabytes used = %d\n\n",iMegabytesUsed);
    fclose(pkLog);

    return 0;
}
//----------------------------------------------------------------------------
