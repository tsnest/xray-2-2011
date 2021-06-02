////////////////////////////////////////////////////////////////////////////
//	Created		: 12.02.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_CONSOLE_COMMAND_H_INCLUDED
#define XRAY_CONSOLE_COMMAND_H_INCLUDED

namespace xray {
namespace console_commands {

enum command_type {
	command_type_engine_internal,
	command_type_user_specific,
}; // enum command_type

enum execution_filter {
	execution_filter_early,
	execution_filter_general,
	execution_filter_all,
}; // enum command_type

typedef boost::function< void (pcstr) >			functor_type;

#pragma warning( push )
#pragma warning( disable : 4231 )
template class XRAY_CORE_API boost::function< void (pcstr) >;
#pragma warning( pop )

#pragma warning(push)
#pragma warning(disable:4251)

class XRAY_CORE_API save_storage : private core::noncopyable
{
public:
						save_storage	( memory::base_allocator& a );
						~save_storage	( );
			void		add_line		( pcstr );
			void		save_to			( memory::writer& f );
private:
	vectora<pcstr>				m_lines;
	memory::base_allocator&		m_allocator;
};

class XRAY_CORE_API console_command : private core::noncopyable
{
public:
	typedef	string512	info_str;
	typedef	string512	status_str;
	typedef	string512	syntax_str;

public:
						console_command			( pcstr name, bool serializable, command_type const command_type, execution_filter const execution_filter );
	virtual				~console_command		( );
	virtual void		execute					( pcstr args )			= 0;
	virtual void		status					( status_str& dest )	const;
	virtual void		info					( info_str& dest )		const;
	virtual void		syntax					( syntax_str& dest )	const;
	virtual void		fill_command_args_list	( vectora<pcstr>& )		const			{};
	virtual void		save_to					( save_storage& f, memory::base_allocator* a )	const;
			pcstr		name					( ) const				{ return m_name; }
			bool		need_args				( ) const				{ return m_need_args; }
			bool		is_serializable			( ) const				{ return m_serializable; }
			void		set_serializable		( bool b )				{ m_serializable = b; }
	command_type		get_command_type		( ) const				{ return m_command_type; }
	execution_filter		get_execution_type		( ) const				{ return m_execution_type; }
			void		subscribe_on_change		( functor_type functor ) {m_on_change_event = functor; }

	console_command*	prev					( ) const				{ return m_prev; }
	console_command*	next					( ) const				{ return m_next; }

protected:
			void		on_invalid_syntax		( pcstr args );
			void		on_changed				( pcstr args );
			
	console_command*	m_next;
	console_command*	m_prev;
	pcstr				m_name;
	command_type const	m_command_type;
	execution_filter const m_execution_type;
	bool				m_need_args;
	bool				m_serializable;
	functor_type		m_on_change_event;

}; // class console_command


class XRAY_CORE_API cc_delegate : public console_command
{
public:
						cc_delegate				(pcstr name, functor_type const& functor, bool need_args);
	virtual void		execute					(pcstr args);
	virtual void		info					(info_str& dest) const;
	virtual void		save_to					( console_commands::save_storage& , memory::base_allocator* )	const {};

protected:
	functor_type		m_functor;

private:
	typedef console_command	super;
};// cc_string

class XRAY_CORE_API cc_string :public console_command
{
	typedef console_command		super;
public:
						cc_string				( pcstr name, pstr value, u32 size, bool serializable, command_type const command_type, execution_filter const execution_filter = execution_filter_general );
	virtual void		execute					( pcstr args );
	virtual void		status					( status_str& dest )	const;
	virtual void		info					( info_str& dest )		const;
	virtual void		syntax					( syntax_str& dest )	const;
protected:
	pstr				m_value;
	u32					m_size;
};// cc_string

class XRAY_CORE_API cc_bool :public console_command {
	typedef console_command		super;

public:
						cc_bool					( pcstr name, bool& value, bool serializable, command_type const command_type, execution_filter const execution_filter = execution_filter_general );
	virtual void		execute					( pcstr args );
	virtual void		status					( status_str& dest )	const;
	virtual void		info					( info_str& dest )		const;
	virtual void		syntax					( syntax_str& dest )	const;
	virtual void		fill_command_args_list	( vectora<pcstr>& dest ) const;
protected:
	bool&				m_value;
};// cc_bool

template<class T>
class XRAY_CORE_API cc_value : public console_command
{
	typedef console_command		super;
public:
	inline			cc_value					( pcstr name, T& value, T const min, T const max, bool serializable, command_type const command_type, execution_filter const execution_filter = execution_filter_general );
protected:
			T&		m_value;
			T		m_min;
			T		m_max;
};//cc_value
#pragma warning(pop)

class XRAY_CORE_API cc_float :public cc_value<float>
{
	typedef cc_value<float>		super;
public:
						cc_float				( pcstr name, float& value, float const min, float const max, bool serializable, command_type const command_type, execution_filter const execution_filter = execution_filter_general );
	virtual void		execute					( pcstr args );
	virtual void		status					( status_str& dest )	const;
	virtual void		info					( info_str& dest )		const;
	virtual void		syntax					( syntax_str& des )		const;
};//cc_float 

class XRAY_CORE_API cc_u32 :public cc_value<u32>
{
	typedef cc_value<u32>		super;
public:
						cc_u32					( pcstr name, u32& value, u32 const min, u32 const max, bool serializable, command_type const command_type, execution_filter const execution_filter = execution_filter_general );
	virtual void		execute					( pcstr args );
	virtual void		status					( status_str& dest )	const;
	virtual void		info					( info_str& dest )		const;
	virtual void		syntax					( syntax_str& dest )	const;
};//cc_u32

class XRAY_CORE_API cc_float2 :public cc_value<math::float2>{
	typedef cc_value<math::float2>	super;
public:
						cc_float2				( pcstr name, math::float2& value, math::float2 const min, math::float2 const max, bool serializable, command_type const command_type, execution_filter const execution_filter = execution_filter_general );
	virtual void		execute					( pcstr args );
	virtual void		status					( status_str& dest ) const;
	virtual void		info					( info_str& dest ) const;
	virtual void		syntax					( syntax_str& dest ) const;
}; // class cc_float2

class XRAY_CORE_API cc_float3 :public cc_value<math::float3>
{
	typedef cc_value<math::float3>	super;
public:
						cc_float3				( pcstr name, math::float3& value, math::float3 const min, math::float3 const max, bool serializable, command_type const command_type, execution_filter const execution_filter = execution_filter_general );
	virtual void		execute					( pcstr args );
	virtual void		status					( status_str& dest ) const;
	virtual void		info					( info_str& dest ) const;
	virtual void		syntax					( syntax_str& dest ) const;
}; // class cc_float3


struct command_token
{
	u32		id;
	pcstr	name;

} ; // command_token

class XRAY_CORE_API cc_token :public console_commands::console_command
{

	typedef console_command	super;

public:
		cc_token( pcstr name, u32 &value, const command_token commands[ ], u32 size, bool serializable, command_type const command_type, execution_filter const execution_filter = execution_filter_general );

	virtual void		execute					( pcstr args );
	virtual void		status					( status_str& dest )	const;
	virtual void		info					( info_str& dest )	const;
	virtual void		syntax					( syntax_str& dest )	const;
	virtual void		fill_command_args_list	( vectora<pcstr>& dest ) const;

private:
			u32			num_commands		( )	const { return m_num_commands; };
			u32			find_id				( pcstr args ) const;
			pcstr		find_name			( u32 id )	const;
			void		all_methods_names	( string512 names )const;

private:
	const		command_token	*m_commands;
	const		u32				m_num_commands;
				u32				&m_value;

}; // class cc_token



} // namespace console_commands
} // namespace xray

#include <xray/console_command_inline.h>

#endif // #ifndef XRAY_CONSOLE_COMMAND_H_INCLUDED