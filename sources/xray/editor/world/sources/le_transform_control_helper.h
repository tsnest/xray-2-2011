////////////////////////////////////////////////////////////////////////////
//	Created		: 03.11.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef LE_TRANSFORM_CONTROL_HELPER_H_INCLUDED
#define LE_TRANSFORM_CONTROL_HELPER_H_INCLUDED

namespace xray{

namespace collision{ class object; }

namespace editor{

// custom impls
ref class command_apply_control_transform;
ref class level_editor;
ref class object_track;
ref class composite_object_item;
ref class project_item_folder;

using xray::editor_base::transform_control_base;

public ref class le_helper :public editor_base::transform_control_helper
{
public:
						le_helper					( level_editor^ le );
	virtual float4x4	get_inverted_view_matrix	( ) override;
	virtual float4x4	get_projection_matrix		( ) override;
	virtual void		get_mouse_ray				( float3& origin, float3& direction) override;
private:
	level_editor^		m_level_editor;
};

public ref class object_transform_data :public editor_base::transform_control_object
{
	typedef editor_base::transform_control_object super;
protected:
	typedef System::Collections::Generic::List<editor_base::collision_object_wrapper> collision_objects_list;
public:
					object_transform_data			( level_editor^ );
	virtual			~object_transform_data			( );
	
	virtual bool	attach							( editor_base::transform_control_base^ )	override;
	virtual void	detach							( editor_base::transform_control_base^ )	override;
	virtual void	start_modify					( editor_base::transform_control_base^ )	override;
	virtual void	execute_preview					( editor_base::transform_control_base^ )	override;
	virtual void	end_modify						( editor_base::transform_control_base^ )	override;
	virtual math::float4x4	get_ancor_transform		( )											override;
	virtual u32		get_collision					( collision_objects_list^% r)				override;
	
protected:
	level_editor^						m_level_editor;
	editor_base::command^				m_apply_command;
};//object_transform_data

public ref class folder_transform_data :public object_transform_data
{
	typedef object_transform_data super;
public:
					folder_transform_data			( level_editor^ le, project_item_folder^ f );
	
	virtual void	start_modify					( editor_base::transform_control_base^ )	override;
	virtual math::float4x4	get_ancor_transform		( )											override;
	virtual u32		get_collision					( collision_objects_list^% r)				override;

protected:
	project_item_folder^	m_folder;
};//folder_transform_data

public ref class track_object_key_transform_data :public editor_base::transform_control_object
{
protected:
	typedef System::Collections::Generic::List<editor_base::collision_object_wrapper> collision_objects_list;
public:
					track_object_key_transform_data	( object_track^ o, u32 key_idx );
	virtual			~track_object_key_transform_data( );
					!track_object_key_transform_data( );
	
	virtual	bool	attach							( transform_control_base^ transform )		override;
	virtual void	start_modify					( editor_base::transform_control_base^ )	override;
	virtual void	execute_preview					( editor_base::transform_control_base^ )	override;
	virtual void	end_modify						( editor_base::transform_control_base^ )	override;
	virtual math::float4x4	get_ancor_transform		( )											override;
	virtual u32		get_collision					( collision_objects_list^% r)				override;
private:
	object_track^						m_object_path;
	u32									m_key_index;
	math::float4x4*						m_start_transform;
};//track_object_key_transform_data

public ref class track_object_key_tangent_transform_data :public editor_base::transform_control_object
{
public:
					track_object_key_tangent_transform_data	( object_track^ o, u32 key_idx, int tangent_idx );
	virtual			~track_object_key_tangent_transform_data( );
					!track_object_key_tangent_transform_data( );

	virtual	bool	attach							( transform_control_base^ transform )		override;
	virtual void	start_modify					( editor_base::transform_control_base^ )	override;
	virtual void	execute_preview					( editor_base::transform_control_base^ )	override;
	virtual void	end_modify						( editor_base::transform_control_base^ )	override;
	virtual math::float4x4	get_ancor_transform		( )											override;
private:
	object_track^						m_object_path;
	u32									m_key_index;
	int									m_tangent_index;
	math::float4x4*						m_start_transform;
};//track_object_key_tangent_transform_data

public ref class object_incompound_transform_data :public editor_base::transform_control_object
{
public:
					object_incompound_transform_data	( composite_object_item^ o );
	virtual			~object_incompound_transform_data	( );
					!object_incompound_transform_data	( );

	virtual void	start_modify					( editor_base::transform_control_base^ )	override;
	virtual void	execute_preview					( editor_base::transform_control_base^ )	override;
	virtual void	end_modify						( editor_base::transform_control_base^ )	override;
	virtual math::float4x4	get_ancor_transform		( )											override;
private:
	composite_object_item^				m_compound_object_item;
	math::float4x4*						m_start_transform;
};//object_incompound_transform_data

} //namespace editor
} //namespace xray


#endif // #ifndef LE_TRANSFORM_CONTROL_HELPER_H_INCLUDED
