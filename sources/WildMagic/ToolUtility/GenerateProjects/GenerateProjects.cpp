#include <cassert>
#include <fstream>
#include <iostream>
using namespace std;

#pragma warning( disable : 4996 )

extern int g_iTemplateVC60Length;
extern char* g_aacTemplateVC60[];

extern int g_iTemplateVC70Length;
extern char* g_aacTemplateVC70[];

extern int g_iTemplateVC71Length;
extern char* g_aacTemplateVC71[];

extern int g_iTemplateVC80Length;
extern char* g_aacTemplateVC80[];

extern int g_iTemplateVC90Length;
extern char* g_aacTemplateVC90[];

extern int g_iTemplateXC23Length;
extern char* g_aacTemplateXC23[];

extern int g_iTemplateXC30Length;
extern char* g_aacTemplateXC30[];

//----------------------------------------------------------------------------
void Usage ()
{
    cout << "usage: GenerateProjects projectname" << endl;
}
//----------------------------------------------------------------------------
bool GenerateVC60 (const char* acProjectName)
{
    const size_t uiSize = 512;
    char acProjectFile[uiSize];
    sprintf(acProjectFile,"%s_VC60.dsp",acProjectName);
    ofstream kOStr(acProjectFile);
    if (!kOStr)
    {
        return false;
    }

    char acString[uiSize];

    for (int iLine = 0; iLine < g_iTemplateVC60Length; iLine++)
    {
        strcpy(acString,g_aacTemplateVC60[iLine]);

        char* acSub = strstr(acString,"PROJECTNAME");
        if (acSub)
        {
            int iIndex = (int)(acSub - acString);
            for (int i = 0; i < iIndex; i++)
            {
                kOStr << acString[i];
            }

            kOStr << acProjectName;
            acSub += strlen("PROJECTNAME");
            kOStr << acSub << endl;
            continue;
        }

        kOStr << acString << endl;
    }

    kOStr.close();
    return true;
}
//----------------------------------------------------------------------------
bool GenerateVC70 (const char* acProjectName)
{
    const size_t uiSize = 512;
    char acProjectFile[uiSize];
    sprintf(acProjectFile,"%s_VC70.vcproj",acProjectName);
    ofstream kOStr(acProjectFile);
    if (!kOStr)
    {
        return false;
    }

    char acString[uiSize];

    for (int iLine = 0; iLine < g_iTemplateVC70Length; iLine++)
    {
        strcpy(acString,g_aacTemplateVC70[iLine]);

        char* acSub;
        int iIndex, i;

        acSub = strstr(acString,"PROJECTNAME");
        if (acSub)
        {
            iIndex = (int)(acSub - acString);
            for (i = 0; i < iIndex; i++)
            {
                kOStr << acString[i];
            }

            kOStr << acProjectName;
            acSub += strlen("PROJECTNAME");
            kOStr << acSub << endl;
            continue;
        }

        kOStr << acString << endl;
    }

    kOStr.close();
    return true;
}
//----------------------------------------------------------------------------
bool GenerateVC71 (const char* acProjectName)
{
    const size_t uiSize = 512;
    char acProjectFile[uiSize];
    sprintf(acProjectFile,"%s_VC71.vcproj",acProjectName);
    ofstream kOStr(acProjectFile);
    if (!kOStr)
    {
        return false;
    }

    char acString[uiSize];

    for (int iLine = 0; iLine < g_iTemplateVC71Length; iLine++)
    {
        strcpy(acString,g_aacTemplateVC71[iLine]);

        char* acSub;
        int iIndex, i;

        acSub = strstr(acString,"PROJECTNAME");
        if (acSub)
        {
            iIndex = (int)(acSub - acString);
            for (i = 0; i < iIndex; i++)
            {
                kOStr << acString[i];
            }

            kOStr << acProjectName;
            acSub += strlen("PROJECTNAME");
            kOStr << acSub << endl;
            continue;
        }

        kOStr << acString << endl;
    }

    kOStr.close();
    return true;
}
//----------------------------------------------------------------------------
bool GenerateVC80 (const char* acProjectName)
{
    const size_t uiSize = 512;
    char acProjectFile[uiSize];
    sprintf(acProjectFile,"%s_VC80.vcproj",acProjectName);
    ofstream kOStr(acProjectFile);
    if (!kOStr)
    {
        return false;
    }

    char acString[uiSize];

    for (int iLine = 0; iLine < g_iTemplateVC80Length; iLine++)
    {
        strcpy(acString,g_aacTemplateVC80[iLine]);

        char* acSub;
        int iIndex, i;

        acSub = strstr(acString,"PROJECTNAME");
        if (acSub)
        {
            iIndex = (int)(acSub - acString);
            for (i = 0; i < iIndex; i++)
            {
                kOStr << acString[i];
            }

            kOStr << acProjectName;
            acSub += strlen("PROJECTNAME");
            kOStr << acSub << endl;
            continue;
        }

        kOStr << acString << endl;
    }

    kOStr.close();
    return true;
}
//----------------------------------------------------------------------------
bool GenerateVC90 (const char* acProjectName)
{
    const size_t uiSize = 512;
    char acProjectFile[uiSize];
    sprintf(acProjectFile,"%s_VC90.vcproj",acProjectName);
    ofstream kOStr(acProjectFile);
    if (!kOStr)
    {
        return false;
    }

    char acString[uiSize];

    for (int iLine = 0; iLine < g_iTemplateVC90Length; iLine++)
    {
        strcpy(acString,g_aacTemplateVC90[iLine]);

        char* acSub;
        int iIndex, i;

        acSub = strstr(acString,"PROJECTNAME");
        if (acSub)
        {
            iIndex = (int)(acSub - acString);
            for (i = 0; i < iIndex; i++)
            {
                kOStr << acString[i];
            }

            kOStr << acProjectName;
            acSub += strlen("PROJECTNAME");
            kOStr << acSub << endl;
            continue;
        }

        kOStr << acString << endl;
    }

    kOStr.close();
    return true;
}
//----------------------------------------------------------------------------
bool GenerateXC23 (const char* acProjName)
{
    // open the output project file
    ofstream kOStr("project.pbxproj");
    assert(kOStr);
    if (!kOStr)
    {
        return false;
    }

    // substitute
    const size_t uiSize = 2048;
    char acString[uiSize];
    for (int iLine = 0; iLine < g_iTemplateXC23Length; iLine++)
    {
        strcpy(acString,g_aacTemplateXC23[iLine]);

        char* acSub = strstr(acString,"WM4APPNAME");
        int iIndex, i;
        if (acSub)
        {
            iIndex = (int)(acSub - acString);
            for (i = 0; i < iIndex; i++)
            {
                kOStr << acString[i];
            }
            kOStr << acProjName;

            acSub += strlen("WM4APPNAME");
            char* acSub2 = strstr(acSub,"WM4APPNAME");
            if (acSub2)
            {
                iIndex = (int)(acSub2 - acSub);
                for (i = 0; i < iIndex; i++)
                {
                    kOStr << acSub[i];
                }
                kOStr << acProjName;

                acSub2 += strlen("WM4APPNAME");
                acSub = acSub2;
            }
            kOStr << acSub << endl;
            continue;
        }

        kOStr << acString << endl;
    }

    kOStr.close();
    return true;
}
//----------------------------------------------------------------------------
bool GenerateXC30 (const char* acProjName)
{
    // open the output project file
    ofstream kOStr("project.pbxproj");
    assert(kOStr);
    if (!kOStr)
    {
        return false;
    }

    // substitute
    const size_t uiSize = 2048;
    char acString[uiSize];
    for (int iLine = 0; iLine < g_iTemplateXC30Length; iLine++)
    {
        strcpy(acString,g_aacTemplateXC30[iLine]);

        char* acSub = strstr(acString,"WM4APPNAME");
        int iIndex, i;
        if (acSub)
        {
            iIndex = (int)(acSub - acString);
            for (i = 0; i < iIndex; i++)
            {
                kOStr << acString[i];
            }
            kOStr << acProjName;

            acSub += strlen("WM4APPNAME");
            char* acSub2 = strstr(acSub,"WM4APPNAME");
            if (acSub2)
            {
                iIndex = (int)(acSub2 - acSub);
                for (i = 0; i < iIndex; i++)
                {
                    kOStr << acSub[i];
                }
                kOStr << acProjName;

                acSub2 += strlen("WM4APPNAME");
                acSub = acSub2;
            }
            kOStr << acSub << endl;
            continue;
        }

        kOStr << acString << endl;
    }

    kOStr.close();
    return true;
}
//----------------------------------------------------------------------------
int main (int iQuantity, char** apcArgument)
{
    if (iQuantity != 2)
    {
        cout << "Incorrect number of command line arguments." << endl << endl;
        Usage();
        return -1;
    }

    const char* acSourceName = apcArgument[1];

    // DEPRECATED.  If you want to generate projects for these platforms,
    // uncomment the relevant lines.
    //GenerateVC60(acSourceName);
    //GenerateVC70(acSourceName);
    //GenerateXC23(acSourceName);

    GenerateVC71(acSourceName);
    GenerateVC80(acSourceName);
    GenerateVC90(acSourceName);
    GenerateXC30(acSourceName);

    return 0;
}
//----------------------------------------------------------------------------
