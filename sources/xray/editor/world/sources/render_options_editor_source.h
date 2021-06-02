////////////////////////////////////////////////////////////////////////////
//	Created		: 01.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef RENDER_OPTIONS_EDITOR_SOURCE_H_INCLUDED
#define RENDER_OPTIONS_EDITOR_SOURCE_H_INCLUDED

using xray::editor::controls::tree_view_source;
using xray::editor::controls::tree_view;

namespace xray{
namespace editor{

// ref class render_options_editor_source_filter
// {
// public:
// 	render_options_editor_source_filter(xray::render::texture_options::ETType* filter_type)
// 	:m_filter_type(filter_type)
// 	{}
// 
// 	xray::render::texture_options::ETType* m_filter_type;
// };//ref class texture_editor_source_filter


ref class render_options_editor_source: public xray::editor::controls::tree_view_source
{

#pragma region | Initialize | 


public:
						render_options_editor_source	( );
						render_options_editor_source	( Object^ filter );


#pragma endregion 

#pragma region |   Fields   | 


private:
	tree_view^								m_parent;
	static System::String^		c_source_textures_path = "resources/render";


#pragma endregion 

#pragma region | Properties | 


public:
	virtual property	tree_view^		parent
	{
		tree_view^			get	()					{ return m_parent; }
		void				set	(tree_view^ value)	{ m_parent = value; }
	};

#pragma endregion

#pragma region |   Methods  | 


private:
	void	on_fs_iterator_ready	( vfs::vfs_locked_iterator const & fs_it );
	void	process_fs				( vfs::vfs_iterator const & it,  System::String^ file_path );

public:
	virtual void refresh();


#pragma endregion 

}; // class render_options_editor_source

}//namespace editor
}//namespace xray

#endif // #ifndef RENDER_OPTIONS_EDITOR_SOURCE_H_INCLUDED