////////////////////////////////////////////////////////////////////////////
//	Created		: 03.08.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TESTING_H_INCLUDED
#define TESTING_H_INCLUDED

namespace xray {
namespace rtp {

class rtp_world;
class base_controller;

class testing: boost::noncopyable
{

public:
			testing						( rtp_world& w );
			~testing					( );

void		test						( );
void		save_test					( );

private:
	void	load_test_controller		( );
	void	save_test_controller		( rtp::base_controller	*ctrl );
	void	calculate					( );

private:
	fs_new::virtual_path_string	value_controller_path	( )const;
	pcstr			value_test_dir			( )const;
	pcstr			controller_name			( )const;

private:
	void	on_resources_loaded			( resources::queries_result& data );
	void	load						( );


private:
	rtp::base_controller					*m_test_controller;
	rtp::base_controller					*m_controller;
	configs::binary_config_ptr				m_settings;
	rtp_world								&m_rtp_world;

}; // class testing

} // namespace rtp
} // namespace xray

#endif // #ifndef TESTING_H_INCLUDED

