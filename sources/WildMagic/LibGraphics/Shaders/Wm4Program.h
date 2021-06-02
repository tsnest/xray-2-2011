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
// Version: 4.0.6 (2009/02/27)

#ifndef WM4PROGRAM_H
#define WM4PROGRAM_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Object.h"
#include "Wm4Attributes.h"
#include "Wm4Bindable.h"
#include "Wm4NumericalConstant.h"
#include "Wm4RendererConstant.h"
#include "Wm4SamplerInformation.h"
#include "Wm4UserConstant.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM Program : public Object, public Bindable
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // Abstract base class.
    virtual ~Program ();

    // *** Member read-only access ***

    // Access to the program text string.
    const std::string& GetProgramText () const;

    // Access to the program input and output attributes.
    const Attributes& GetInputAttributes () const;
    const Attributes& GetOutputAttributes () const;

    // Access to renderer constants.
    int GetRendererConstantQuantity () const;
    RendererConstant* GetRendererConstant (int i);
    RendererConstant* GetRendererConstant (RendererConstant::Type eType);

    // Access to numerical constants.
    int GetNumericalConstantQuantity () const;
    NumericalConstant* GetNumericalConstant (int i);

    // Access to user constants.
    int GetUserConstantQuantity () const;
    UserConstant* GetUserConstant (int i);
    UserConstant* GetUserConstant (const std::string& rkName);

    // Access to samplers.
    int GetSamplerInformationQuantity () const;
    SamplerInformation* GetSamplerInformation (int i);
    SamplerInformation* GetSamplerInformation (const std::string& rkName);

    // This information is used to load and parse programs.
    static std::string RendererType;
    static char CommentChar;

    static bool Load (const std::string& rkFilename, Program* pkProgram);

    // *** Member write access ***
    // TO DO.  Allow the users to construct a shader program at run time (as
    // compared to forcing all programs to be loaded from disk).

protected:
    // Streaming Support.
    Program ();

    // The program itself, stored as a text string.
    std::string m_kProgramText;

    // The format of the input and output parameters to the shader program.
    Attributes m_kInputAttributes;
    Attributes m_kOutputAttributes;

    // The renderer constants required by the shader program.
    std::vector<RendererConstant> m_kRendererConstants;

    // The numerical constants required by the shader program.
    std::vector<NumericalConstant> m_kNumericalConstants;

    // The user constants required by the shader program.  These are set by
    // the applications as needed.
    std::vector<UserConstant> m_kUserConstants;

    // Information about the sampler units required by a shader program.
    std::vector<SamplerInformation> m_kSamplerInformation;

    // For use by the constructor for loading and parsing a shader program.
    static const std::string ms_kFloatStr;
    static const std::string ms_kFloat1Str;
    static const std::string ms_kFloat1x1Str;
    static const std::string ms_kFloat1x2Str;
    static const std::string ms_kFloat1x3Str;
    static const std::string ms_kFloat1x4Str;
    static const std::string ms_kFloat2Str;
    static const std::string ms_kFloat2x1Str;
    static const std::string ms_kFloat2x2Str;
    static const std::string ms_kFloat2x3Str;
    static const std::string ms_kFloat2x4Str;
    static const std::string ms_kFloat3Str;
    static const std::string ms_kFloat3x1Str;
    static const std::string ms_kFloat3x2Str;
    static const std::string ms_kFloat3x3Str;
    static const std::string ms_kFloat3x4Str;
    static const std::string ms_kFloat4Str;
    static const std::string ms_kFloat4x1Str;
    static const std::string ms_kFloat4x2Str;
    static const std::string ms_kFloat4x3Str;
    static const std::string ms_kFloat4x4Str;
    static const std::string ms_kSampler1DStr;
    static const std::string ms_kSampler2DStr;
    static const std::string ms_kSampler3DStr;
    static const std::string ms_kSamplerCubeStr;
    static const std::string ms_kSamplerProjStr;
    static const std::string ms_kPositionStr;
    static const std::string ms_kBlendWeightStr;
    static const std::string ms_kNormalStr;
    static const std::string ms_kColorStr;
    static const std::string ms_kColor0Str;
    static const std::string ms_kColor1Str;
    static const std::string ms_kFogCoordStr;
    static const std::string ms_kFogStr;
    static const std::string ms_kPSizeStr;
    static const std::string ms_kBlendIndicesStr;
    static const std::string ms_kTexCoordStr;
    static const std::string ms_kTangentStr;
    static const std::string ms_kBinormalStr;
    static const std::string ms_kWPosStr;
    static const std::string ms_kInStr;
    static const std::string ms_kEOL;
};

WM4_REGISTER_STREAM(Program);
typedef Pointer<Program> ProgramPtr;
#include "Wm4Program.inl"

}

#endif
