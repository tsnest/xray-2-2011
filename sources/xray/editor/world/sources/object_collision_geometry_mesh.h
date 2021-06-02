////////////////////////////////////////////////////////////////////////////
//	Created		: 19.08.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_COLLISION_GEOMETRY_MESH_H_INCLUDED
#define OBJECT_COLLISION_GEOMETRY_MESH_H_INCLUDED

#include <xray/collision/collision_object.h>
#include "attribute.h"

using System::Boolean;
using System::Collections::Generic::List;
using xray::editor::wpf_controls::property_descriptor;

namespace xray
{
	namespace editor
	{
		ref class object_collision_geometry;
		class object_collision_geometry_mesh_collision;

		typedef	xray::editor::wpf_controls::property_container property_container;

		ref class object_collision_geometry_mesh : public editor_base::transform_control_object
		{
		public:
			object_collision_geometry_mesh	( object_collision_geometry^ parent );
			object_collision_geometry_mesh	( object_collision_geometry^ parent, Boolean is_anti_mesh );
			~object_collision_geometry_mesh	( );
			
			[DisplayNameAttribute("position"), CategoryAttribute("transform")]
			[ValueAttribute(ValueAttribute::value_type::e_def_val, 0.0f, 0.0f, 0.0f), ReadOnlyAttribute(false)]
			property Float3					position{
				Float3						get( );
				void						set( Float3 p );
			}

			[DisplayNameAttribute("rotation"), CategoryAttribute("transform")]
			[ValueAttribute(ValueAttribute::value_type::e_def_val, 0.0f, 0.0f, 0.0f), ReadOnlyAttribute(false)]
			property Float3					rotation{
				Float3						get( );
				void						set( Float3 p );
			}
			[DisplayNameAttribute("scale"), CategoryAttribute("transform")]
			[ValueAttribute(ValueAttribute::value_type::e_def_val, 1.0f, 1.0f, 1.0f), ReadOnlyAttribute(false)]
			property Float3					scale{
				Float3						get( );
				void						set( Float3 p );
			}

			property int					type{
				int							get( );
				void						set( int p );
			}

			property float4x4				matrix{
				float4x4					get( );
				void						set( float4x4 matrix );
			}

			property collision::primitive const&	primitive
			{
				collision::primitive const&		get( );
			}

			property object_collision_geometry^		collision_object
			{
				object_collision_geometry^ get( )
				{
					return m_parent;
				}
			}
			property property_descriptor^			last_property_descriptor;

		public:
			void				render					( render::scene_ptr const& scene, render::debug::renderer& r );
			void				dbg_render_nearest_point( render::scene_ptr const& scene, render::debug::renderer& r, float3 const& point );
			void				load					( configs::lua_config_value const& t );
			void				save					( configs::lua_config_value& t );
			void				activate				( bool value );

		public: //transform_control_object
			virtual void					start_modify			( editor_base::transform_control_base^ control ) override;
			virtual void					execute_preview			( editor_base::transform_control_base^ control ) override;
			virtual void					end_modify				( editor_base::transform_control_base^ control ) override;
			virtual float4x4				get_ancor_transform		( ) override;
					property_container^		get_property_container	( );

		public: //
			collision::geometry_instance*	create_geometry			( );
			collision::geometry_instance*	get_geometry_instance	( );
			void							update_collision		( );
			void							update_position			( float4x4 const& matrix );
			void							set_selected			( bool value );
			aabb							get_aabb				( );
			void							set_plane				( float4 const& plane );
			float3							get_absolute_position	( );

		private:
			object_collision_geometry_mesh_collision*	m_collision_object;
			collision::primitive*						m_primitive;
			math::float4x4*								m_matrix;
			object_collision_geometry^					m_parent;
			Object^										m_additional_data;
			Boolean										m_is_anti_mesh;

		}; // class object_collision_geometry_mesh

		class object_collision_geometry_mesh_collision : public collision::collision_object
		{
			typedef collision::collision_object	super;
		public:
							object_collision_geometry_mesh_collision	( object_collision_geometry_mesh^ parent );
					void	dbg_render_nearest_point					( render::scene_ptr const& scene, render::debug::renderer& r, float3 const& point ) { collision::collision_object::dbg_render_nearest_point( scene, r,  point ); }
			virtual bool	touch				( ) const;
		private:
			gcroot<object_collision_geometry_mesh^>	m_parent;
		}; // class collision_primitive_item_collision

	} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_COLLISION_GEOMETRY_MESH_H_INCLUDED