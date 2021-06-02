////////////////////////////////////////////////////////////////////////////
//	Created		: 22.04.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCE_EDITOR_OPTIONS_WRAPPER_H_INCLUDED
#define RESOURCE_EDITOR_OPTIONS_WRAPPER_H_INCLUDED

namespace xray
{
	namespace editor
	{
		public ref class resource_editor_options_wrapper: IDisposable
		{
		public:
			resource_editor_options_wrapper(editor_world& world, const resources::unmanaged_resource_ptr& set_resource, property_holder* holder):
				m_property_holder(holder), m_editor_world(world)
			{
				m_resource		= NEW(resources::unmanaged_resource_ptr)(set_resource);
				m_resource_name = gcnew String(static_cast_checked<resource_options*>(m_resource->c_ptr())->m_resource_name.c_str());
			}

			!resource_editor_options_wrapper()
			{
				this->~resource_editor_options_wrapper();
			}

			~resource_editor_options_wrapper()
			{
				if(!m_is_disposed)
				{
					DELETE						(m_resource);
					property_holder*	temp	=  dynamic_cast<property_holder*>(m_property_holder);
					DELETE						(temp);
					m_resource					= NULL;
					m_property_holder			= NULL;
					m_is_disposed				= true;
				}
			}
		private:
			editor_world&						m_editor_world;
			Boolean								m_is_disposed;

		public:
			resources::unmanaged_resource_ptr * m_resource;
			property_holder*					m_property_holder;
			Boolean								m_is_changed;
			String^								m_resource_name;

		}; // class resource_editor_options_wrapper
	} // namespace editor
} // namespace xray

#endif // #ifndef RESOURCE_EDITOR_OPTIONS_WRAPPER_H_INCLUDED