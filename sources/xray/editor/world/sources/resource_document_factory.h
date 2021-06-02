////////////////////////////////////////////////////////////////////////////
//	Created		: 24.02.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCE_DOCUMENT_FACTORY_H_INCLUDED
#define RESOURCE_DOCUMENT_FACTORY_H_INCLUDED

namespace xray
{
	namespace editor
	{
		// move to resource_editor_base
		// void	set_view_panel_caption		(String^ caption);
		// void	set_properties_panel_caption(String^ caption);
		// void	set_editor_caption			(String^ caption);

		// pass tree_view_source on constructor
		// controls::tree_view_source^ get_resources_source		();	// => tree_view_source

		// rename class resource_document_factory
		public interface class resource_document_factory
		{
			virtual controls::document_base^	new_document	(controls::document_editor_base^ editor) = 0;
		}; // interface class resource_document_factory

		//public interface class resource_base
		//{
		//public:
		//	virtual controls::document_base^	get_new_document			(controls::document_editor_base^ editor);
		//	virtual controls::tree_view_source^ get_resources_source		();
		//	virtual String^						get_view_panel_name			();
		//	virtual String^						get_properties_panel_name	();
		//	virtual String^						get_editor_name				();
		//}; // class resource_base			
	} // namespace editor
} // namespace xray

#endif // #ifndef RESOURCE_DOCUMENT_FACTORY_H_INCLUDED