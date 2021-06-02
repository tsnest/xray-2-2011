////////////////////////////////////////////////////////////////////////////
//	Created		: 05.09.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_SOUND_FUNCTOR_COMMAND_H_INCLUDED
#define XRAY_SOUND_FUNCTOR_COMMAND_H_INCLUDED

namespace xray {
namespace sound {

template < typename SuperClass >
class functor_command : public SuperClass
{
public:
	typedef boost::function< void ( ) >	functor_type;

public:
			functor_command				( functor_type const& receiver_function );
	virtual	void	execute	( );

private:
	functor_type						m_functor;
}; // class functor_order

template < typename SuperClass >
inline functor_command< SuperClass >::functor_command	( functor_type const& functor ) :
	m_functor											( functor )
{
}

template < typename SuperClass >
void functor_command< SuperClass >::execute	( )
{
	return m_functor										( );
}

typedef functor_command< sound_order >		functor_order;
typedef functor_command< sound_response >	functor_response;

typedef intrusive_spsc_queue< sound_order, sound_order, &sound_order::m_next_for_orders >	sound_orders_queue_type;
typedef intrusive_spsc_queue< sound_response, sound_response, &sound_response::m_next >		sound_responses_queue_type;

} // namespace sound
} // namespace xray

#endif // #ifndef XRAY_SOUND_FUNCTOR_COMMAND_H_INCLUDED