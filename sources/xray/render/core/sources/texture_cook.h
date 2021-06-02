////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TEXTURE_COOK_H_INCLUDED
#define TEXTURE_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace render {

struct texture_data_resource {
public:
							texture_data_resource	( u32 buffer_storage_size );
	const_buffer			buffer					( ) const;

private:
	u32						m_buffer_size;
}; //	struct texture_data_resource

struct texture_cook : public resources::inplace_managed_cook {
public:
							texture_cook 			( );
	virtual	u32				calculate_resource_size	( u32 file_size, u32 & out_offset_to_file, bool file_exist );
	virtual	void			create_resource			(
								resources::query_result_for_cook&	in_out_query,
								resources::managed_resource_ptr		in_out_resource,
								u32									raw_file_size,
								u32&								out_final_resource_size
							);
	virtual	void			destroy_resource		( resources::managed_resource * dying_resource );
}; // struct texture_cooker

	
struct texture_options_cooker: public resources::translate_query_cook
{
	typedef resources::translate_query_cook super;
public:
	texture_options_cooker	( resources::class_id_enum resource_class, reuse_enum reuse_type, u32 translate_query_thread, enum_flags<flags_enum> flags = 0);
	virtual void			translate_request_path	( pcstr request_path, fs_new::virtual_path_string& new_request) const;
	virtual void			delete_resource			( resources::resource_base* resource );

};//texture_options_cooker

#ifndef MASTER_GOLD
struct texture_options_lua_cooker: public texture_options_cooker
{
	typedef texture_options_cooker super;
public:
							texture_options_lua_cooker	( );

	virtual void			translate_query			( resources::query_result_for_cook& parent );
private:
	void					on_lua_config_loaded	( resources::queries_result& result );
	void					create_default_options	( resources::query_result_for_cook* parent );
}; // class texture_options_lua_cooker
#endif // #ifndef MASTER_GOLD

struct texture_options_binary_cooker: public texture_options_cooker
{
	typedef texture_options_cooker super;
public:
							texture_options_binary_cooker	( );

	virtual void			translate_query			( resources::query_result_for_cook& parent );

private:
	void					on_lua_options_loaded	( resources::queries_result& result );
	void					on_binary_config_loaded	( resources::queries_result& result );
}; // class texture_options_binary_cooker

} // namespace render
} // namespace xray

#endif // #ifndef TEXTURE_COOK_H_INCLUDED