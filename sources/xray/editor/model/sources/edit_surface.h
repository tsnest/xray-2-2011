////////////////////////////////////////////////////////////////////////////
//	Created		: 28.01.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef EDIT_SURFACE_H_INCLUDED
#define EDIT_SURFACE_H_INCLUDED

namespace xray {
namespace model_editor {

ref class edit_object_mesh;
ref class lod_descriptor;

ref class edit_surface 
{
	typedef xray::editor::wpf_controls::hypergraph::properties_node	hypergraph_node;
public:
				edit_surface	( edit_object_mesh^ parent, System::String^ folder_name );
				~edit_surface	( );


	System::String^				m_mesh_name;
	property System::String^	m_base_texture_name;
	math::aabb*					m_bbox;

	property int			Triangles;
	property int			Vertices;
	property int			Indices
	{
		int		get()		{return m_indices;}
	}
	property System::String^ name{
		System::String^		get ( )							{ return m_surface_exported_name;}
	}
	property System::String^ material_name{
		System::String^		get	( )							{ return m_material_name;}
		void				set	( System::String^ value );
	}

	property bool			Visible{
		bool				get	( )							{ return m_visible;}
		void				set	( bool value );
	}

	property bool			Selected{
		bool				get	( )							{ return m_selected;}
		void				set	( bool value );
	}

	void					on_selected					( System::Boolean value );
	void					query_export_properties		( );
	void					load_material_settings		( configs::lua_config_value const& cfg );
	void					save_material_settings		( configs::lua_config_value& cfg );
	void					setup_default_material_settings( );

	resources::unmanaged_resource_ptr preview_material	( ) { return *m_preview_material; }

	property edit_surface^	next_lod{
		edit_surface^		get ( )							{ return m_next_lod_surface;}
		void				set	( edit_surface^ s);
	}
	hypergraph_node^		m_hypergraph_node;
private:
	void					requery_material			( );
	void					on_export_properties_ready	( resources::queries_result& data );
	void					material_ready				( resources::queries_result& data );

	System::String^						m_surface_exported_name;
	System::String^						m_material_name;
	query_result_delegate*				m_material_query;
	resources::unmanaged_resource_ptr*	m_preview_material;
	bool								m_visible;
	bool								m_selected;
	edit_surface^						m_next_lod_surface;
	edit_object_mesh^					m_parent_object;
	int									m_indices;
}; // class edit_surface

typedef System::Collections::Generic::Dictionary<System::String^, edit_surface^>	surfaces_dict;
typedef System::Collections::Generic::List<edit_surface^>							surfaces_list;

} // namespace model_editor
} // namespace xray

#endif // #ifndef EDIT_SURFACE_H_INCLUDED