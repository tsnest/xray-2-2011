////////////////////////////////////////////////////////////////////////////
//	Created		: 31.03.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TEXTURE_TGA_TO_ARGB_COOK_H_INCLUDED
#define TEXTURE_TGA_TO_ARGB_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray
{
	namespace resources
	{
		class cook;
	} // namespace resources

	namespace editor
	{
		struct texture_tga_argb_buffers
		{
			texture_tga_argb_buffers(resources::pinned_ptr_const<u8>* tga_buffer, void* rgba_buffer):
				m_tga_buffer(tga_buffer), m_rgba_buffer(rgba_buffer), m_need_process_flag(0){}

			~texture_tga_argb_buffers()
			{
				DELETE(m_tga_buffer);
			}

			resources::pinned_ptr_const<u8>*	m_tga_buffer;
			void*								m_rgba_buffer;
			long volatile						m_need_process_flag;
		};	// struct texture_tga_to_argb_cook_buffer

		struct texture_tga_to_argb_cook : public resources::inplace_unmanaged_cook
		{
		public:
				texture_tga_to_argb_cook();

				void	create_resource_in_creation_data (resources::query_result_for_cook & in_out_query,
														  mutable_buffer					 in_out_unmanaged_resource_buffer);

				virtual create_resource_inplace_delegate_type	get_create_resource_inplace_in_creation_data_delegate	() 
				{
					return create_resource_inplace_delegate_type(this, 
						& texture_tga_to_argb_cook::create_resource_in_creation_data); 
				}

				virtual	mutable_buffer	allocate_resource	(resources::query_result_for_cook &	in_query, 
															 u32								file_size, 
															 u32 &								out_offset_to_file, 
															 bool								file_exist)
				{
					XRAY_UNREFERENCED_PARAMETERS		(& in_query, file_size, out_offset_to_file, file_exist);
					NOT_IMPLEMENTED						(return mutable_buffer::zero());
				}

				virtual void		create_resource		(resources::query_result_for_cook & in_out_query, 
														 mutable_buffer						in_out_unmanaged_resource_buffer)
				{
					XRAY_UNREFERENCED_PARAMETERS		(& in_out_query, & in_out_unmanaged_resource_buffer);
					NOT_IMPLEMENTED						();
//					in_out_query.finish_query			(result_error);
				}

				virtual void			destroy_resource (resources::unmanaged_resource * resource)
				{
					XRAY_UNREFERENCED_PARAMETER			(resource);
					NOT_IMPLEMENTED						();
				}
				
				virtual void			deallocate_resource (pvoid buffer)
				{
					XRAY_UNREFERENCED_PARAMETER			(buffer);
					NOT_IMPLEMENTED						();
				}
		};	// class texture_tga_to_argb_cook

	} // namespace editor
} // namespace xray

#endif // #ifndef TEXTURE_TGA_TO_ARGB_COOK_H_INCLUDED