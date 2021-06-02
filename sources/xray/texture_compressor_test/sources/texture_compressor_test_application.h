////////////////////////////////////////////////////////////////////////////
//	Created		: 19.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TEXTURE_COMPRESSOR_TEST_APPLICATION_H_INCLUDED
#define TEXTURE_COMPRESSOR_TEST_APPLICATION_H_INCLUDED

namespace xray {
namespace texture_compressor_test {

class application {
public:
			void	initialize		( );
			void	execute			( );
			void	finalize		( );
	inline	u32		get_exit_code	( ) const	{ return m_exit_code; }

private:
	u32		m_exit_code;
}; // class application

} // namespace texture_compressor_test
} // namespace xray

#endif // #ifndef TEXTURE_COMPRESSOR_TEST_APPLICATION_H_INCLUDED