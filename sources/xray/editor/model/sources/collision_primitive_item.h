////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef COLLISION_PRIMITIVE_ITEM_H_INCLUDED
#define COLLISION_PRIMITIVE_ITEM_H_INCLUDED

#include <xray/collision/collision_object.h>

namespace xray {
namespace physics{ struct bt_rigid_body_construction_info; }

namespace model_editor {

ref class edit_object_mesh;
class collision_primitive_item_collision;
class mass_center_item_collision;

ref class collision_primitive_item : public editor_base::transform_control_object
{
public:
						collision_primitive_item( edit_object_mesh^ parent );
						~collision_primitive_item( );
	void				render					( render::scene_ptr const& scene, render::debug::renderer& r, math::color const& clr  );
	void				load					( configs::lua_config_value const& t, float3 const& mass_center );
	void				save					( configs::lua_config_value& t, float3 const& mass_center );
	void				activate				( bool value );

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

	collision::primitive*			m_primitive;
	math::float4x4*					m_matrix;
	edit_object_mesh^				m_parent;
	xray::editor::wpf_controls::property_descriptor^			m_current_desc;
public: //transform_control_object
	virtual void					start_modify			( editor_base::transform_control_base^ control ) override;
	virtual void					execute_preview			( editor_base::transform_control_base^ control ) override;
	virtual void					end_modify				( editor_base::transform_control_base^ control ) override;
	virtual float4x4				get_ancor_transform		( ) override;
	virtual u32						get_collision			( System::Collections::Generic::List<editor_base::collision_object_wrapper>^% ) override;

public: //
	collision_primitive_item_collision*	m_active_collision_object;
	collision::geometry_instance*	create_geometry_instance( );
	void							update_collision		( );
	void							set_selected			( bool value );
	bool							is_selected				( );
};

ref class mass_center_item : public editor_base::transform_control_object
{
public:
						mass_center_item		( edit_object_mesh^ parent );
						~mass_center_item		( );
	void				render					( render::scene_ptr const& scene, render::debug::renderer& r );
	void				activate				( bool value );

	edit_object_mesh^				m_parent;
//	xray::editor::wpf_controls::property_descriptor^			m_current_desc;
public: //transform_control_object
	virtual void					start_modify			( editor_base::transform_control_base^ control ) override;
	virtual void					execute_preview			( editor_base::transform_control_base^ control ) override;
	virtual void					end_modify				( editor_base::transform_control_base^ control ) override;
	virtual float4x4				get_ancor_transform		( ) override;
	virtual u32						get_collision			( System::Collections::Generic::List<editor_base::collision_object_wrapper>^% ) override;

public:
	void							collision_item_moved	( );
//	mass_center_item_collision*		m_collision_object;
	collision::geometry_instance*	create_geometry_instance( );
//	void							update_collision		( );
	void							set_selected			( bool value );
	bool							is_selected				( );
};

class collision_primitive_item_collision : public collision::collision_object
{
	typedef collision::collision_object	super;
public:
					collision_primitive_item_collision	( collision_primitive_item^ parent );
	virtual bool	touch				( ) const;
private:
	gcroot<collision_primitive_item^>	m_parent;
};

//class mass_center_item_collision : public collision::collision_object
//{
//	typedef collision::collision_object	super;
//public:
//					mass_center_item_collision	( mass_center_item^ parent );
//	virtual bool	touch						( ) const;
//private:
//	gcroot<mass_center_item^>	m_parent;
//};

typedef System::Collections::Generic::List<collision_primitive_item^>				collision_list;


ref struct rigid_body_construction_info_ref
{

	typedef System::ComponentModel::CategoryAttribute		CategoryAttribute;
	typedef System::ComponentModel::DefaultValueAttribute	DefaultValueAttribute;
	typedef System::ComponentModel::DescriptionAttribute	DescriptionAttribute;
	typedef System::ComponentModel::DisplayNameAttribute	DisplayNameAttribute;

	rigid_body_construction_info_ref		( edit_object_mesh^parent );
	~rigid_body_construction_info_ref		( );
	void	save_to							( configs::lua_config_ptr config );
	void	load_from						( configs::lua_config_ptr const config );

	[DisplayNameAttribute("Mass center"), CategoryAttribute("Rigid Body Settings")]
	[ValueAttribute(ValueAttribute::value_type::e_def_val, 0.0f, 0.0f, 0.0f), ReadOnlyAttribute(false)]
	property Float3					mass_center{
		Float3						get( );
		void						set( Float3 p );
	}

	[CategoryAttribute("Rigid Body Settings")]
	[DisplayNameAttribute("Mass")]
	property float	mass
	{
		float			get	( );
		void			set	( float value );
	}

	[CategoryAttribute("Rigid Body Settings")]
	[DisplayNameAttribute("Linear damping")]
	[DefaultValueAttribute(0.0f)]
	property float	linear_damping
	{
		float			get	( );
		void			set	( float value );
	}

	[CategoryAttribute("Rigid Body Settings")]
	[DisplayNameAttribute("Angular damping")]
	[DefaultValueAttribute(0.0f)]
	property float	angular_damping
	{
		float			get	( );
		void			set	( float value );
	}

	[CategoryAttribute("Rigid Body Settings")]
	[DisplayNameAttribute("Friction")]
	[DescriptionAttribute("best simulation results when friction is non-zero")]
	[DefaultValueAttribute(0.5f)]
	property float	friction
	{
		float			get	( );
		void			set	( float value );
	}

	[CategoryAttribute("Rigid Body Settings")]
	[DisplayNameAttribute("Restitution")]
	[DescriptionAttribute("best simulation results using zero restitution")]
	[DefaultValueAttribute(0.0f)]
	property float	restitution
	{
		float			get	( );
		void			set	( float value );
	}

	[CategoryAttribute("Rigid Body Settings")]
	[DisplayNameAttribute("Linear sleeping threshold")]
	[DefaultValueAttribute(0.8f)]
	property float	linear_sleeping_threshold
	{
		float			get	( );
		void			set	( float value );
	}
	[CategoryAttribute("Rigid Body Settings")]
	[DisplayNameAttribute("Angular sleeping threshold")]
	[DefaultValueAttribute(1.0f)]
	property float	angular_sleeping_threshold
	{
		float			get	( );
		void			set	( float value );
	}

	[CategoryAttribute("Rigid Body Settings")]
	[DisplayNameAttribute("Additional damping")]
	[DescriptionAttribute("Additional damping can help avoiding lowpass jitter motion, help stability for ragdolls etc. Such damping is undesirable, so once the overall simulation quality of the rigid body dynamics system has improved, this should become obsolete")]
	[DefaultValueAttribute(false)]
	property bool	additional_damping
	{
		bool			get	( );
		void			set	( bool value );
	}

	[CategoryAttribute("Rigid Body Settings")]
	[DisplayNameAttribute("Additional damping factor")]
	[DefaultValueAttribute(0.005f)]
	property float	additional_damping_factor
	{
		float			get	( );
		void			set	( float value );
	}
	[CategoryAttribute("Rigid Body Settings")]
	[DisplayNameAttribute("Additional linear damping threshold")]
	[DefaultValueAttribute(0.01f)]
	property float	additional_linear_damping_threshold_sqr
	{
		float			get	( );
		void			set	( float value );
	}
	[CategoryAttribute("Rigid Body Settings")]
	[DisplayNameAttribute("Additional angular damping threshold")]
	[DefaultValueAttribute(0.01f)]
	property float	additional_angular_damping_threshold_sqr
	{
		float			get	( );
		void			set	( float value );
	}
	[CategoryAttribute("Rigid Body Settings")]
	[DisplayNameAttribute("Additional angular damping factor")]
	[DefaultValueAttribute(0.01f)]
	property float	additional_angular_damping_factor
	{
		float			get	( );
		void			set	( float value );
	}


	edit_object_mesh^							m_parent;
	physics::bt_rigid_body_construction_info*	m_info;
};

} // namespace model_editor
} // namespace xray

#endif // #ifndef COLLISION_PRIMITIVE_ITEM_H_INCLUDED