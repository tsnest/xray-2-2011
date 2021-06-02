////////////////////////////////////////////////////////////////////////////
//	Created		: 29.01.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PLATFORM_INITIALIZER_H_INCLUDED
#define PLATFORM_INITIALIZER_H_INCLUDED

namespace xray {
namespace network {
namespace lowlevel {


class platform_initializer
{
public:
	platform_initializer	();
	~platform_initializer	();
private:
	char XRAY_DEFAULT_ALIGN	m_object_store[8];
}; // class platform_initializer

} // namespace lowlevel
} // namespace network
} // namespace xray

#endif // #ifndef PLATFORM_INITIALIZER_H_INCLUDED