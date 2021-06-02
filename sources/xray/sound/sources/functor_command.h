////////////////////////////////////////////////////////////////////////////
//	Created		: 13.04.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef FUNCTOR_COMMAND_H_INCLUDED
#define FUNCTOR_COMMAND_H_INCLUDED

#include <xray/sound/sound.h>
#include <xray/sound/sound_object_commands.h>
#include <xray/sound/functor_command.h>

namespace xray {
namespace sound {

class world_user;

template < typename SuperClass, typename T >
class functor_command_with_data : public SuperClass
{
public:
	typedef boost::function< void ( T const& data ) >	functor_type;

public:
			functor_command_with_data	( functor_type const& receiver_function, T const& data );
	virtual	void	execute	( );

private:
	functor_type						m_functor;
	T									m_data;
}; // class functor_order

class notify_receiver_command : public sound_response
{
public:
	typedef boost::function< void ( ) >	functor_type;

public:
			notify_receiver_command		(
											functor_type const& receiver_function,
											world_user& user,
											u64 producer_address,
											u64 receiver_address
										);

	virtual	void	execute	( );

private:
	functor_type						m_functor;
	u64									m_producer_address;
	u64									m_receiver_address;
	world_user&							m_user;
}; // class notify_receiver_command

typedef functor_command< sound_order >		functor_order;
typedef functor_command< sound_response >	functor_response;


typedef intrusive_spsc_queue< sound_order, sound_order, &sound_order::m_next_for_orders >	sound_orders_queue_type;
typedef intrusive_spsc_queue< sound_response, sound_response, &sound_response::m_next >		sound_responses_queue_type;

} // namespace sound
} // namespace xray

#include "functor_command_inline.h"


#endif // #ifndef FUNCTOR_COMMAND_H_INCLUDED