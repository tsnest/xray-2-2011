///////////////////////////////////////////////////////////////////////  
//  CoordSys.h
//
//  *** INTERACTIVE DATA VISUALIZATION (IDV) CONFIDENTIAL AND PROPRIETARY INFORMATION ***
//
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Interactive Data Visualization, Inc. and
//  may not be copied, disclosed, or exploited except in accordance with 
//  the terms of that agreement.
//
//      Copyright (c) 2003-2010 IDV, Inc.
//      All rights reserved in all media.
//
//      IDV, Inc.
//      http://www.idvinc.com
//
//  *** Release version 5.2.0 ***


///////////////////////////////////////////////////////////////////////  
//  Preprocessor

#pragma once
#include <speedtree/core/exportbegin.h>
#include <speedtree/core/matrix.h>
#include <cassert>

#ifdef ST_SUPPORTS_PRAGMA_PACK
    #pragma pack(push, 8)
#endif


///////////////////////////////////////////////////////////////////////  
//  All SpeedTree SDK classes and variables are under the namespace "SpeedTree"

namespace SpeedTree
{

    ///////////////////////////////////////////////////////////////////////  
    //  Class CCoordSysBase

    class ST_STORAGE_CLASS CCoordSysBase
    {
    public:
    virtual                         ~CCoordSysBase( )                                   { }

    virtual st_bool                 IsLeftHanded(void) const = 0;
    virtual st_bool                 IsYAxisUp(void) const = 0;

    virtual Vec3                    ConvertToStd(st_float32 x, st_float32 y, st_float32 z) const = 0;
            Vec3                    ConvertToStd(const st_float32 afCoord[3]) const     { return ConvertToStd(afCoord[0], afCoord[1], afCoord[2]); }
    virtual Vec3                    ConvertFromStd(st_float32 x, st_float32 y, st_float32 z) const = 0;
            Vec3                    ConvertFromStd(const st_float32 afCoord[3]) const   { return ConvertFromStd(afCoord[0], afCoord[1], afCoord[2]); }

    virtual const Vec3&             OutAxis(void) const = 0;
    virtual const Vec3&             RightAxis(void) const = 0;
    virtual const Vec3&             UpAxis(void) const = 0;

    virtual st_float32              OutComponent(st_float32 x, st_float32 y, st_float32 z) const = 0;
    virtual st_float32              RightComponent(st_float32 x, st_float32 y, st_float32 z) const = 0;
    virtual st_float32              UpComponent(st_float32 x, st_float32 y, st_float32 z) const = 0;

    virtual void                    RotateUpAxis(Mat3x3& mMatrix, st_float32 fRadians) const = 0;
    virtual void                    RotateUpAxis(Mat4x4& mMatrix, st_float32 fRadians) const = 0;
    virtual void                    RotateOutAxis(Mat3x3& mMatrix, st_float32 fRadians) const = 0;
    virtual void                    RotateOutAxis(Mat4x4& mMatrix, st_float32 fRadians) const = 0;
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Class CCoordSys

    class ST_STORAGE_CLASS CCoordSys
    {
    public:
            enum ECoordSysType
            {
                COORD_SYS_RIGHT_HANDED_Z_UP,
                COORD_SYS_RIGHT_HANDED_Y_UP,
                COORD_SYS_LEFT_HANDED_Z_UP,
                COORD_SYS_LEFT_HANDED_Y_UP,
                COORD_SYS_CUSTOM
            };

    static  void                    SetCoordSys(ECoordSysType eType, const CCoordSysBase* pCustomConverter = NULL);
    static  ECoordSysType           GetCoordSysType(void);
    static  st_bool                 IsDefaultCoordSys(void);
    static  const CCoordSysBase*    GetBuiltInConverter(ECoordSysType eType);

    static  st_bool                 IsLeftHanded(void);
    static  st_bool                 IsYAxisUp(void);

    static  Vec3                    ConvertToStd(const st_float32 afCoord[3]);
    static  Vec3                    ConvertToStd(st_float32 x, st_float32 y, st_float32 z);
    static  Vec3                    ConvertFromStd(const st_float32 afCoord[3]);
    static  Vec3                    ConvertFromStd(st_float32 x, st_float32 y, st_float32 z);

    static  const Vec3&             OutAxis(void);
    static  const Vec3&             RightAxis(void);
    static  const Vec3&             UpAxis(void);

    static  st_float32              OutComponent(const st_float32 afCoord[3]);
    static  st_float32              OutComponent(st_float32 x, st_float32 y, st_float32 z);
    static  st_float32              RightComponent(const st_float32 afCoord[3]);
    static  st_float32              RightComponent(st_float32 x, st_float32 y, st_float32 z);
    static  st_float32              UpComponent(const st_float32 afCoord[3]);
    static  st_float32              UpComponent(st_float32 x, st_float32 y, st_float32 z);

    static  void                    RotateUpAxis(Mat3x3& mMatrix, st_float32 fRadians);
    static  void                    RotateUpAxis(Mat4x4& mMatrix, st_float32 fRadians);
    static  void                    RotateOutAxis(Mat3x3& mMatrix, st_float32 fRadians);
    static  void                    RotateOutAxis(Mat4x4& mMatrix, st_float32 fRadians);

    static  const CCoordSysBase*    m_pCoordSys;
    static  ECoordSysType           m_eCoordSysType;
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Class CDefaultCoordinateSystem
    //
    //  The default coordinate system is right-handed with the +Z pointing "up."
    //  The conversion functions in this class will essentially be no-ops.

    // CRHCS_Zup
    class CDefaultCoordinateSystem : public CCoordSysBase
    {
    public:
    virtual                         ~CDefaultCoordinateSystem( )                                            { }

            st_bool                 IsLeftHanded(void) const                                                { return false; }
            st_bool                 IsYAxisUp(void) const                                                   { return false; }

            // conversions for this coordinate system are symmetric (it's the default)
            Vec3                    ConvertToStd(st_float32 x, st_float32 y, st_float32 z) const            { return Vec3(x, y, z); }
            Vec3                    ConvertFromStd(st_float32 x, st_float32 y, st_float32 z) const          { return ConvertToStd(x, y, z); }

            const Vec3&             OutAxis(void) const                                                     { return m_vOut; }
            const Vec3&             RightAxis(void) const                                                   { return m_vRight; }
            const Vec3&             UpAxis(void) const                                                      { return m_vUp; }

            // first assignment in these functions is to quiet unreferenced parameter warnings
            st_float32              OutComponent(st_float32 /*x*/, st_float32 y, st_float32 /*z*/) const    { return y; }
            st_float32              RightComponent(st_float32 x, st_float32 /*y*/, st_float32 /*z*/) const  { return x; }
            st_float32              UpComponent(st_float32 /*x*/, st_float32 /*y*/, st_float32 z) const     { return z; }

            void                    RotateUpAxis(Mat3x3& mMatrix, st_float32 fRadians) const                { mMatrix.RotateZ(fRadians); }
            void                    RotateUpAxis(Mat4x4& mMatrix, st_float32 fRadians) const                { mMatrix.RotateZ(fRadians); }
            void                    RotateOutAxis(Mat3x3& mMatrix, st_float32 fRadians) const               { mMatrix.RotateY(fRadians); }
            void                    RotateOutAxis(Mat4x4& mMatrix, st_float32 fRadians) const               { mMatrix.RotateY(fRadians); }

    private:
            static  const Vec3      m_vOut;
            static  const Vec3      m_vRight;
            static  const Vec3      m_vUp;
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Class CRHCS_Yup
    //
    //  Right-handed coordinate system, with +Y axis pointing up

    static const Vec3 c_vRHCS_Yup_Out = Vec3(0.0f, 0.0f, -1.0f);
    static const Vec3 c_vRHCS_Yup_Right = Vec3(1.0f, 0.0f, 0.0f);
    static const Vec3 c_vRHCS_Yup_Up = Vec3(0.0f, 1.0f, 0.0f);

    class CRHCS_Yup : public CCoordSysBase
    {
    public:
    virtual                 ~CRHCS_Yup( )                                                           { }

            st_bool         IsLeftHanded(void) const                                                { return false; }
            st_bool         IsYAxisUp(void) const                                                   { return true; }

            // conversions for this coordinate system are NOT symmetric
            Vec3            ConvertToStd(st_float32 x, st_float32 y, st_float32 z) const            { return Vec3(x, -z, y); }
            Vec3            ConvertFromStd(st_float32 x, st_float32 y, st_float32 z) const          { return Vec3(x, z, -y); }

            const Vec3&     OutAxis(void) const                                                     { return c_vRHCS_Yup_Out; }
            const Vec3&     RightAxis(void) const                                                   { return c_vRHCS_Yup_Right; }
            const Vec3&     UpAxis(void) const                                                      { return c_vRHCS_Yup_Up; }

            float           OutComponent(st_float32 /*x*/, st_float32 /*y*/, st_float32 z) const    { return -z; }
            float           RightComponent(st_float32 x, st_float32 /*y*/, st_float32 /*z*/) const  { return x; }
            float           UpComponent(st_float32 /*x*/, st_float32 y, st_float32 /*z*/) const     { return y; }

            void            RotateUpAxis(Mat3x3& mMatrix, st_float32 fRadians) const                { mMatrix.RotateY(fRadians); }
            void            RotateUpAxis(Mat4x4& mMatrix, st_float32 fRadians) const                { mMatrix.RotateY(fRadians); }
            void            RotateOutAxis(Mat3x3& mMatrix, st_float32 fRadians) const               { mMatrix.RotateZ(fRadians); }
            void            RotateOutAxis(Mat4x4& mMatrix, st_float32 fRadians) const               { mMatrix.RotateZ(fRadians); }
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Class CLHCS_Yup
    //
    //  Left-handed coordinate system, with +Y axis pointing up

    static const Vec3 c_vLHCS_Yup_Out = Vec3(0.0f, 0.0f, 1.0f);
    static const Vec3 c_vLHCS_Yup_Right = Vec3(1.0f, 0.0f, 0.0f);
    static const Vec3 c_vLHCS_Yup_Up = Vec3(0.0f, 1.0f, 0.0f);

    class CLHCS_Yup : public CCoordSysBase
    {
    public:
    virtual                 ~CLHCS_Yup( )                                                           { }

            st_bool         IsLeftHanded(void) const                                                { return true; }
            st_bool         IsYAxisUp(void) const                                                   { return true; }

            // conversions for this coordinate system are symmetric
            Vec3            ConvertToStd(st_float32 x, st_float32 y, st_float32 z) const            { return Vec3(x, z, y); }
            Vec3            ConvertFromStd(st_float32 x, st_float32 y, st_float32 z) const          { return ConvertToStd(x, y, z); }

            const Vec3&     OutAxis(void) const                                                     { return c_vLHCS_Yup_Out; }
            const Vec3&     RightAxis(void) const                                                   { return c_vLHCS_Yup_Right; }
            const Vec3&     UpAxis(void) const                                                      { return c_vLHCS_Yup_Up; }

            st_float32      OutComponent(st_float32 /*x*/, st_float32 /*y*/, st_float32 z) const    { return z; }
            st_float32      RightComponent(st_float32 x, st_float32 /*y*/, st_float32 /*z*/) const  { return x; }
            st_float32      UpComponent(st_float32 /*x*/, st_float32 y, st_float32 /*z*/) const     { return y; }

            void            RotateUpAxis(Mat3x3& mMatrix, st_float32 fRadians) const                { mMatrix.RotateY(fRadians); }
            void            RotateUpAxis(Mat4x4& mMatrix, st_float32 fRadians) const                { mMatrix.RotateY(fRadians); }
            void            RotateOutAxis(Mat3x3& mMatrix, st_float32 fRadians) const               { mMatrix.RotateZ(fRadians); }
            void            RotateOutAxis(Mat4x4& mMatrix, st_float32 fRadians) const               { mMatrix.RotateZ(fRadians); }
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Class CLHCS_Zup
    //
    //  Left-handed coordinate system, with +Z axis pointing up

    static const Vec3 c_vLHCS_Zup_Out = Vec3(0.0f, -1.0f, 0.0f);
    static const Vec3 c_vLHCS_Zup_Right = Vec3(1.0f, 0.0f, 0.0f);
    static const Vec3 c_vLHCS_Zup_Up = Vec3(0.0f, 0.0f, 1.0f);

    class CLHCS_Zup : public CCoordSysBase
    {
    public:
    virtual                 ~CLHCS_Zup( )                                                           { }

            st_bool         IsLeftHanded(void) const                                                { return true; }
            st_bool         IsYAxisUp(void) const                                                   { return false; }

            // conversions for this coordinate system are symmetric
            Vec3            ConvertToStd(st_float32 x, st_float32 y, st_float32 z) const            { return Vec3(x, -y, z); }
            Vec3            ConvertFromStd(st_float32 x, st_float32 y, st_float32 z) const          { return ConvertToStd(x, y, z); }

            const Vec3&     OutAxis(void) const                                                     { return c_vLHCS_Zup_Out; }
            const Vec3&     RightAxis(void) const                                                   { return c_vLHCS_Zup_Right; }
            const Vec3&     UpAxis(void) const                                                      { return c_vLHCS_Zup_Up; }

            st_float32      OutComponent(st_float32 /*x*/, st_float32 y, st_float32 /*z*/) const    { return -y; }
            st_float32      RightComponent(st_float32 x, st_float32 /*y*/, st_float32 /*z*/) const  { return x; }
            st_float32      UpComponent(st_float32 /*x*/, st_float32 /*y*/, st_float32 z) const     { return z; }

            void            RotateUpAxis(Mat3x3& mMatrix, st_float32 fRadians) const                { mMatrix.RotateZ(fRadians); }
            void            RotateUpAxis(Mat4x4& mMatrix, st_float32 fRadians) const                { mMatrix.RotateZ(fRadians); }
            void            RotateOutAxis(Mat3x3& mMatrix, st_float32 fRadians) const               { mMatrix.RotateY(fRadians); }
            void            RotateOutAxis(Mat4x4& mMatrix, st_float32 fRadians) const               { mMatrix.RotateY(fRadians); }
    };

} // end namespace SpeedTree


#ifdef ST_SUPPORTS_PRAGMA_PACK
    #pragma pack(pop)
#endif

#include <speedtree/core/ExportEnd.h>

