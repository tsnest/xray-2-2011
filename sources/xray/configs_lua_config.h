////////////////////////////////////////////////////////////////////////////
//	Created		: 01.04.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_CONFIGS_LUA_CONFIG_H_INCLUDED
#define XRAY_CONFIGS_LUA_CONFIG_H_INCLUDED

#include <xray/resources.h>
#include <xray/configs_lua_config_value.h>
#include <xray/configs_binary_config.h>

namespace xray {

namespace memory { class stream; } // namespace memory

namespace configs {

class binary_config;

XRAY_CORE_API void				create_binary_config_buffer( lua_config_value value, xray::memory::stream& fat );
XRAY_CORE_API binary_config_ptr	create_binary_config( lua_config_value value );
XRAY_CORE_API binary_config_ptr	create_binary_config( mutable_buffer const& buffer );

class XRAY_CORE_API lua_config : public resources::unmanaged_resource {
public:
								lua_config			( pcstr file_name, ::luabind::object const& object );
	virtual						~lua_config			( );

	inline						lua_config			( lua_config const& other ) : m_root( other.m_root ){ }
	inline	lua_config&			operator =			( lua_config_value const& value )					{ m_root = value; return *this; }
	inline	lua_config_value&	assign_lua_value	( lua_config_value const& value )					{ m_root.assign_lua_value( value ); return m_root; }
	inline	void				swap				( lua_config& other )								{ m_root.swap( other.m_root ); }

			void				save				( save_target target, lua_config_value::save_as_option option = lua_config_value::leave_old_value ) const;
			void				save_as				( pcstr file_name, save_target target, lua_config_value::save_as_option option = lua_config_value::leave_old_value ) const;
			void				save_as_binary		( pcstr file_name ) const;
			void				save				( xray::strings::stream& stream ) const;

	inline  pstr				get_file_name		( ) const											{ return m_file_name; }
	inline	binary_config_ptr	get_binary_config	( ) const											{ return create_binary_config( get_root() ); }

	inline	lua_config_value const&	get_root		( ) const											{ return m_root; }
	inline	lua_config_value&	get_root			( )													{ return m_root; }

	inline	bool				value_exists		( pcstr field_id ) const							{ return m_root.value_exists(field_id); }

	inline	bool				empty				( ) const											{ return m_root.empty(); }
	inline	u32					size				( ) const											{ return m_root.size(); }

	typedef lua_config_iterator						iterator;
	typedef iterator								const_iterator;

	inline	iterator			begin				( )													{ return m_root.begin(); }
	inline	iterator			end					( )													{ return m_root.end(); }

	inline	const_iterator		begin				( ) const											{ return m_root.begin(); }
	inline	const_iterator		end					( ) const											{ return m_root.end(); }

	inline	lua_config_value const	operator[ ]		( u32 index ) const									{ return m_root[index]; }
	inline	lua_config_value const	operator[ ]		( int index ) const									{ return m_root[index]; }
	inline	lua_config_value const	operator[ ]		( pcstr field_id ) const							{ return m_root[field_id]; }

	inline	lua_config_value	operator[ ]			( u32 index )										{ return m_root[index]; }
	inline	lua_config_value	operator[ ]			( int index )										{ return m_root[index]; }
	inline	lua_config_value	operator[ ]			( pcstr field_id )									{ return m_root[field_id]; }

	inline	operator lua_config_value				( ) const											{ return m_root; }
	inline	operator lua_config_value				( )													{ return m_root; }

	inline	void				erase				( pcstr const field_id )							{ m_root.erase(field_id); }
	inline	void				erase				( u32 const index )									{ m_root.erase(index); }
	inline	void				insert				(lua_config_value const& val, u32 const index )		{ m_root.insert(val, index); }

	inline	void				copy_super_tables	( lua_config_value const& from_value )						{ m_root.copy_super_tables(from_value); }
	inline	void				add_super_table		( lua_config_value const& super_table )				{ m_root.add_super_table( super_table ); }
	inline	void				add_super_table		( pcstr file_table, pcstr super_table )				{ m_root.add_super_table( file_table, super_table ); }
	inline	void				add_super_table		( lua_config_value const& config_root, pcstr const super_table )	{ m_root.add_super_table( config_root, super_table ); }

	inline	void				remove_super_table	( lua_config_value const& super_table )				{ m_root.remove_super_table( super_table ); }
	inline	void				remove_super_table	( pcstr file_table, pcstr super_table )				{ m_root.remove_super_table( file_table, super_table ); }

	inline	bool are_all_super_tables_loaded_correctly	( ) const										{ return m_root.are_all_super_tables_loaded_correctly(); }

private:
	lua_config_value			m_root;
	pstr						m_file_name;
}; // class lua_config

typedef	xray::resources::resource_ptr <
	lua_config, 
	resources::unmanaged_intrusive_base
> lua_config_ptr;

XRAY_CORE_API	lua_config_ptr  create_lua_config	( mutable_buffer const & buffer, pcstr file_name );
XRAY_CORE_API	lua_config_ptr	create_lua_config	( pcstr file_name = 0 );
XRAY_CORE_API	lua_config_ptr	create_lua_config	( pcstr file_name, lua_config_value const& value );
XRAY_CORE_API	lua_config_ptr	create_lua_config_from_string ( pcstr string );
XRAY_CORE_API	lua_config_ptr	create_lua_config	( binary_config_ptr const& config );

typedef std::pair<pcstr, u32>	enum_tuple;
XRAY_CORE_API	void			export_enum			( pcstr const table_id, enum_tuple const* begin, enum_tuple const* end );

} // namespace configs
} // namespace xray

	inline	void				swap				( xray::configs::lua_config& left, xray::configs::lua_config& right ) { left.swap(right); }

#ifdef MASTER_GOLD
#	error class must not be used in MASTER_GOLD
#endif

#endif // #ifndef XRAY_CONFIGS_LUA_CONFIG_H_INCLUDED