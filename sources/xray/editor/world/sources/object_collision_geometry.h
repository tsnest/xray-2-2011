////////////////////////////////////////////////////////////////////////////
//	Created		: 15.08.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_COLLISION_GEOMETRY_H_INCLUDED
#define OBJECT_COLLISION_GEOMETRY_H_INCLUDED

#include "object_base.h"

using namespace System;
using namespace System::ComponentModel;

using System::Collections::Generic::List;
using xray::editor::wpf_controls::control_containers::button;

namespace xray
{
	namespace collision{ struct geomerty; struct space_partitioning_tree; enum primitive_type; }

	namespace editor
	{
		ref class tool_misc;
		ref class object_collision_geometry_mesh;

		typedef	xray::editor::wpf_controls::property_container property_container;
		typedef	xray::collision::space_partitioning_tree space_partitioning_tree;

		enum class object_collision_geometry_mode
		{
			containment,
			intersection
		}; //enum class

		ref class object_collision_geometry: object_base
		{

		public:
			object_collision_geometry		( tool_misc^ t );
			virtual		~object_collision_geometry		( );
		
		public:
			[CategoryAttribute("collision"), DisplayNameAttribute("mode")]
			property object_collision_geometry_mode			mode;

		public:			
			virtual void 				load_contents				( ) override;
			virtual void 				unload_contents				( Boolean bdestroy ) override;
			virtual	void				load_props					( configs::lua_config_value const& t ) override;
			virtual	void				save						( configs::lua_config_value t ) override;
			virtual void				render						( ) override;
			virtual property_container^	get_property_container		( ) override;
			virtual void				set_transform				( float4x4 const& transform ) override;
			virtual aabb				get_aabb					( ) override;
			virtual void				on_selected					( bool bselected ) override;
			virtual bool				attach_transform			( transform_control_base^ ) override;

		internal:
			object_collision_geometry_mesh^	get_mesh				( Boolean is_anti_mesh, Int32 index );
			Int32						get_mesh_index				( object_collision_geometry_mesh^ mesh, Boolean is_anti_mesh );
			space_partitioning_tree*	get_collision_tree			( );
			void						set_modified				( );
			void						select_mesh					( object_collision_geometry_mesh^ obj );

			Int32						add_collision_mesh			( collision::primitive_type type );
			void						add_collision_mesh			( collision::primitive_type type, Float3% position, Float3% rotation, Float3% scale, Int32 index );
			void						remove_collision_mesh		( Int32 index, Float3% position, Float3% rotation, Float3% scale );
			void						remove_collision_mesh		( Int32 index );
			Int32						add_anti_mesh				( collision::primitive_type type );
			void						add_anti_mesh				( collision::primitive_type type, Float3% position, Float3% rotation, Float3% scale, Int32 index );
			void						remove_anti_mesh			( Int32 index, Float3% position, Float3% rotation, Float3% scale );
			void						remove_anti_mesh			( Int32 index );

		private:
			void						init_collision				( );
			void						update_collision			( );

			object_collision_geometry_mesh^	create_mesh				( collision::primitive_type type, List<object_collision_geometry_mesh^>^ to );
			object_collision_geometry_mesh^	create_mesh				( collision::primitive_type type, List<object_collision_geometry_mesh^>^ to, Int32 index );
			void						remove_mesh					( object_collision_geometry_mesh^ mesh, List<object_collision_geometry_mesh^>^ from );
			void						remove_mesh					( List<object_collision_geometry_mesh^>^ from, Int32 index );

			void						add_sphere_mesh_clicked		( button^ button );
			void						add_box_mesh_clicked		( button^ button );
			void						add_cylinder_mesh_clicked	( button^ button );
			void						add_capsule_mesh_clicked	( button^ button );
			void						add_truncated_sphere_mesh_clicked	( button^ button );
			void						remove_mesh_clicked			( button^ button );

			void						add_sphere_antimesh_clicked		( button^ button );
			void						add_box_antimesh_clicked		( button^ button );
			void						add_cylinder_antimesh_clicked	( button^ button );
			void						add_capsule_antimesh_clicked	( button^ button );
			void						add_truncated_sphere_antimesh_clicked	( button^ button );
			void						remove_antimesh_clicked			( button^ button );

		private:
			typedef object_base		super;

		private:
			collision::geometry_instance*				m_collision_geometry;
			List<object_collision_geometry_mesh^>^		m_collision_meshes;
			List<object_collision_geometry_mesh^>^		m_collision_anti_meshes;
			object_collision_geometry_mesh^				m_selected_mesh;
			Boolean										m_is_selected;
			
		}; // class object_collision_geometry
	} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_COLLISION_GEOMETRY_H_INCLUDED