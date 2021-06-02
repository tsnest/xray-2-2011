////////////////////////////////////////////////////////////////////////////
//	Created		: 17.12.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TEXTURE_CONVERTER_COOK_H_INCLUDED
#define TEXTURE_CONVERTER_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace render {

struct texture_raw_file_data : public resources::unmanaged_resource
{
public:
	texture_raw_file_data				(mutable_buffer outer_buffer) : m_buffer(outer_buffer) {}
	~texture_raw_file_data				();

	mutable_buffer const &				buffer () const { return m_buffer; }
	mutable_buffer						m_buffer;
};

struct texture_converter_params
{
	texture_converter_params			(resources::unmanaged_resource_ptr	options, 
										 resources::managed_resource_ptr	buffer, 
										 fs_new::virtual_path_string 					dest_path, 
										 fs_new::virtual_path_string 					old_dest_path);
	
	resources::unmanaged_resource_ptr	m_options_cfg;
	resources::managed_resource_ptr		m_buffer;
	fs_new::virtual_path_string			m_dest_path;
	fs_new::virtual_path_string			m_old_dest_path;
};

struct texture_converter_cook : public resources::managed_cook
{
public:
									texture_converter_cook	();

	virtual void	create_resource			( resources::query_result_for_cook &	in_out_query, 
											  const_buffer							raw_file_data, 
 											  resources::managed_resource_ptr		out_resource);
	virtual u32		calculate_resource_size	( const_buffer							raw_file_data, 
											  bool									file_exist );
	virtual void	destroy_resource		( resources::managed_resource *			dying_resource );

private:
	u8*   create_texture					( u32 *									out_size,
											  bool									only_calculate_size,
											  mutable_buffer *						opt_destination_data,
											  const_buffer							raw_file_data );

}; // struct texture_cooker

} // namespace render
} // namespace xray

#endif // #ifndef TEXTURE_CONVERTER_COOK_H_INCLUDED