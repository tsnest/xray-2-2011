///////////////////////////////////////////////////////////////////////  
//  Vector.inl
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
//  Vec3::Vec3

inline Vec3::Vec3( ) :
    x(0.0f),
    y(0.0f),
    z(0.0f)
{
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::Vec3

inline Vec3::Vec3(st_float32 _x, st_float32 _y, st_float32 _z) :
    x(_x),
    y(_y),
    z(_z)
{
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::Vec3

inline Vec3::Vec3(st_float32 _x, st_float32 _y) :
    x(_x),
    y(_y),
    z(0.0f)
{
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::Vec3

inline Vec3::Vec3(const st_float32 afPos[3]) :
    x(afPos[0]),
    y(afPos[1]),
    z(afPos[2])
{
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::operator[]

inline st_float32& Vec3::operator[](int nIndex)
{
    return *(&x + nIndex);
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::operator st_float32*

inline Vec3::operator st_float32*(void)
{
    return &x;
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::operator st_float32*

inline Vec3::operator const st_float32*(void) const
{
    return &x;
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::operator<

inline bool Vec3::operator<(const Vec3& vIn) const
{
    if (x == vIn.x)
    {
        if (y == vIn.y)
            return z < vIn.z;
        else 
            return y < vIn.y;
    }
    else
        return x < vIn.x;
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::operator==

inline bool Vec3::operator==(const Vec3& vIn) const
{
    return (x == vIn.x && y == vIn.y && z == vIn.z);
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::operator!=

inline bool Vec3::operator!=(const Vec3& vIn) const
{
    return (x != vIn.x || y != vIn.y || z != vIn.z);    
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::operator-

inline Vec3 Vec3::operator-(const Vec3& vIn) const
{
    return Vec3(x - vIn.x, y - vIn.y, z - vIn.z);
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::operator+

inline Vec3 Vec3::operator+(const Vec3& vIn) const
{
    return Vec3(x + vIn.x, y + vIn.y, z + vIn.z);
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::operator+=

inline Vec3 Vec3::operator+=(const Vec3& vIn)
{
    x += vIn.x;
    y += vIn.y;
    z += vIn.z;

    return *this;
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::operator*

inline Vec3 Vec3::operator*(const Vec3& vIn) const
{
    return Vec3(x * vIn.x, y * vIn.y, z * vIn.z);
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::operator*=

inline Vec3 Vec3::operator*=(const Vec3& vIn)
{
    x *= vIn.x;
    y *= vIn.y;
    z *= vIn.z;

    return *this;
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::operator*

inline Vec3 Vec3::operator*(st_float32 fScale) const
{
    return Vec3(x * fScale, y * fScale, z * fScale);
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::operator*=

inline Vec3 Vec3::operator*=(st_float32 fScale)
{
    x *= fScale;
    y *= fScale;
    z *= fScale;

    return *this;
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::operator-

inline Vec3 Vec3::operator-(void) const
{
    return Vec3(-x, -y, -z);
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::Set

inline void Vec3::Set(st_float32 _x, st_float32 _y, st_float32 _z)
{
    x = _x;
    y = _y;
    z = _z;
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::Set

inline void Vec3::Set(st_float32 _x, st_float32 _y)
{
    x = _x;
    y = _y;
    z = 0.0f;
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::Set

inline void Vec3::Set(const st_float32 afPos[3])
{
    x = afPos[0];
    y = afPos[1];
    z = afPos[2];
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::Cross

inline Vec3 Vec3::Cross(const Vec3& vIn) const
{
    return Vec3(y * vIn.z - z * vIn.y, z * vIn.x - x * vIn.z, x * vIn.y - y * vIn.x);
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::Distance

inline st_float32 Vec3::Distance(const Vec3& vIn) const
{
    return ((*this - vIn).Magnitude( ));    
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::DistanceSquared

inline st_float32 Vec3::DistanceSquared(const Vec3& vIn) const
{
    return ((*this - vIn).MagnitudeSquared( )); 
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::Dot

inline st_float32 Vec3::Dot(const Vec3& vIn) const
{
    return (x * vIn.x + y * vIn.y + z * vIn.z); 
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::Magnitude

inline st_float32 Vec3::Magnitude(void) const
{
    return st_float32(sqrt(MagnitudeSquared( )));
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::MagnitudeSquared

inline st_float32 Vec3::MagnitudeSquared(void) const
{
    return (x * x + y * y + z * z); 
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::Normalize

inline void Vec3::Normalize(void)
{
    st_float32 fMagnitude = Magnitude( );
    if (fMagnitude > FLT_EPSILON)
        Scale(1.0f / fMagnitude);
}


///////////////////////////////////////////////////////////////////////  
//  Vec3::Scale

inline void Vec3::Scale(st_float32 fScale)
{
    x *= fScale;
    y *= fScale;
    z *= fScale;
}


///////////////////////////////////////////////////////////////////////  
//  Vec4::Vec4

inline Vec4::Vec4( ) :
    x(0.0f),
    y(0.0f),
    z(0.0f),
    w(1.0f)
{
}


///////////////////////////////////////////////////////////////////////  
//  Vec4::Vec4

inline Vec4::Vec4(st_float32 _x, st_float32 _y, st_float32 _z, st_float32 _w) :
    x(_x),
    y(_y),
    z(_z),
    w(_w)
{
}


///////////////////////////////////////////////////////////////////////  
//  Vec4::Vec4

inline Vec4::Vec4(st_float32 _x, st_float32 _y, st_float32 _z) :
    x(_x),
    y(_y),
    z(_z),
    w(1.0f)
{
}


///////////////////////////////////////////////////////////////////////  
//  Vec4::Vec4

inline Vec4::Vec4(st_float32 _x, st_float32 _y) :
    x(_x),
    y(_y),
    z(0.0f),
    w(1.0f)
{
}


///////////////////////////////////////////////////////////////////////  
//  Vec4::Vec4

inline Vec4::Vec4(const st_float32 afPos[4]) :
    x(afPos[0]),
    y(afPos[1]),
    z(afPos[2]),
    w(afPos[3])
{
}


///////////////////////////////////////////////////////////////////////  
//  Vec4::operator[]

inline st_float32& Vec4::operator[](int nIndex)
{
    return *(&x + nIndex);
}


///////////////////////////////////////////////////////////////////////  
//  Vec4::operator st_float32*

inline Vec4::operator st_float32*(void)
{
    return &x;
}


///////////////////////////////////////////////////////////////////////  
//  Vec4::operator st_float32*

inline Vec4::operator const st_float32*(void) const
{
    return &x;
}


///////////////////////////////////////////////////////////////////////  
//  Vec4::operator==

inline bool Vec4::operator==(const Vec4& vIn) const
{
    return (x == vIn.x && y == vIn.y && z == vIn.z && w == vIn.w);  
}


///////////////////////////////////////////////////////////////////////  
//  Vec4::operator!=

inline bool Vec4::operator!=(const Vec4& vIn) const
{
    return (x != vIn.x || y != vIn.y || z != vIn.z || w != vIn.w);  
}


///////////////////////////////////////////////////////////////////////  
//  Vec4::operator*

inline Vec4 Vec4::operator*(const Vec4& vIn) const
{
    return Vec4(x * vIn.x, y * vIn.y, z * vIn.z, w * vIn.w);
}


///////////////////////////////////////////////////////////////////////  
//  Vec4::operator*

inline Vec4 Vec4::operator*(st_float32 fScale) const
{
    return Vec4(x * fScale, y * fScale, z * fScale, w * fScale);
}


///////////////////////////////////////////////////////////////////////  
//  Vec4::Set

inline Vec4 Vec4::operator+(const Vec4& vIn) const
{
    return Vec4(x + vIn.x, y + vIn.y, z + vIn.z, w + vIn.w);
}


///////////////////////////////////////////////////////////////////////  
//  Vec4::Set

inline void Vec4::Set(st_float32 _x, st_float32 _y, st_float32 _z, st_float32 _w)
{
    x = _x;
    y = _y;
    z = _z;
    w = _w;
}


///////////////////////////////////////////////////////////////////////  
//  Vec4::Set

inline void Vec4::Set(st_float32 _x, st_float32 _y, st_float32 _z)
{
    x = _x;
    y = _y;
    z = _z;
}


///////////////////////////////////////////////////////////////////////  
//  Vec4::Set

inline void Vec4::Set(st_float32 _x, st_float32 _y)
{
    x = _x;
    y = _y;
}


///////////////////////////////////////////////////////////////////////  
//  Vec4::Set

inline void Vec4::Set(const st_float32 afPos[4])
{
    x = afPos[0];
    y = afPos[1];
    z = afPos[2];
    w = afPos[3];
}


