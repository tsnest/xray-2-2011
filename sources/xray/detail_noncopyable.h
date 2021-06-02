////////////////////////////////////////////////////////////////////////////
//	Created 	: 31.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_DETAIL_NONCOPYABLE_H_INCLUDED
#define XRAY_DETAIL_NONCOPYABLE_H_INCLUDED

namespace xray {
namespace core {

class XRAY_CORE_API noncopyable {
	protected:
		inline						noncopyable	( ) { }
		inline						~noncopyable( ) { }

	private:  // emphasize the following members are private
									noncopyable	( const noncopyable& );
		inline	noncopyable const&	operator=	( const noncopyable& );
};

} // namespace core
} // namespace xray

#endif // #ifndef XRAY_DETAIL_NONCOPYABLE_H_INCLUDED