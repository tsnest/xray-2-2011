////////////////////////////////////////////////////////////////////////////
//	Created		: 27.04.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_TRACK_H_INCLUDED
#define OBJECT_TRACK_H_INCLUDED

#include "object_base.h"
#include <xray/collision/collision_object.h>

#pragma managed( push, off )
#include <xray/render/facade/model.h>
#include <xray/render/engine/base_classes.h>
#pragma managed( pop )

using xray::editor::wpf_controls::animation_curve_editor;

using namespace System;
using namespace System::Collections::Generic;

namespace xray
{
	namespace animation
	{
		enum enum_channel_id;
		struct anm_track; 
		struct EtCurve; 
		struct EtKey; 
	}

	namespace ui
	{
		struct text;
	}

	namespace editor
	{
		typedef wpf_controls::property_container									property_container;
		typedef xray::editor::wpf_controls::float_curve								float_curve;
		typedef xray::editor::wpf_controls::float_curve_key							float_curve_key;
		typedef xray::editor::wpf_controls::float_curve_key_type					curve_key_type;
		typedef xray::editor::wpf_controls::curve_editor::visual_curve				visual_curve;
		typedef xray::editor::wpf_controls::curve_editor::visual_curve_key			visual_curve_key;
		typedef xray::editor::wpf_controls::curve_editor::curves_link				curves_link;
		typedef System::ComponentModel::BrowsableAttribute							BrowsableAttribute;
		typedef System::Windows::Media::Media3D::Vector3D							Vector3D;
		typedef xray::animation::enum_channel_id									anim_channel;

		class		collision_object_track_key;
		class		collision_object_track_key_tangent;
		ref class	collision_object_track;
		ref class	tool_misc;

		public ref class object_track: public object_base 
		{
		
		#pragma region | Initialize |


		private:
			typedef object_base		super;

		public:
						object_track		( tool_misc^ t, render::scene_ptr const& scene );
			virtual		~object_track		( );

		private:
			ref class	curve_to_channel_binder;


		#pragma endregion

		#pragma region |   Fields   |


		private:
			static const float						c_key_size		= 5;
			static const float						c_tangent_size	= 3;
			static object_track^					m_selected_animation_track;

			timing::timer*							m_test_timer;
			render::scene_ptr*						m_scene;
			bool									m_is_test_playing;
			float									m_test_time_scale;
			float									m_test_current_time;
			int										m_selected_key;
			animation::anm_track*					m_track;
			tool_misc^								m_tool_misc;
			property_container^						m_temp_container;
			xray::ui::text*							m_text;
			bool									m_is_selected;
			List<float_curve^>^						m_animation_curves;
			List<curve_to_channel_binder^>^			m_binders;
			collision_object_track_key_tangent*		m_input_tangent_collision;
			collision_object_track_key_tangent*		m_output_tangent_collision;
			render::static_model_ptr*				m_camera_model;
			bool									m_is_camera_model_in_scene;
			

		public:
			vectora<collision_object_track_key*>*	m_keys_collision;
			int										m_selected_tangent;


		#pragma endregion

		#pragma region | Properties |


		public:
			[DisplayNameAttribute("keys"), DescriptionAttribute("path keys count"), CategoryAttribute("general")]
			[ReadOnlyAttribute(true)]
			property		u32			keys
			{
				u32			get		( )					{ return keys_count(); }
			}

			[BrowsableAttribute( false )]
			[DisplayNameAttribute("current"), DescriptionAttribute("currently focused key"), CategoryAttribute("general")]
			property		int			selected_key
			{
				int			get		( )					{ return m_selected_key; }
				void		set		( int idx );
			}

			[DisplayNameAttribute("UseTime"), DescriptionAttribute(""), CategoryAttribute("general")]
			property		bool		use_time;

			[DisplayNameAttribute("ShowFrames"), DescriptionAttribute(""), CategoryAttribute("general")]
			property		bool		show_frames;

			[DisplayNameAttribute("ShowFrameNumbers"), DescriptionAttribute(""), CategoryAttribute("general")]
			property		bool		show_frame_numbers;
			
			[BrowsableAttribute( false )]
			property		float		test_current_time
			{
				float		get		( )					{ return m_test_current_time; }
				void		set		( float value )		{ m_test_current_time = value; }
			}

			[BrowsableAttribute( false )]
			property		bool		is_selected	
			{
				bool		get		( )					{ return m_is_selected; }
			}


		#pragma endregion

		#pragma region |   Methods  |


		private:
			void			selected_to_camera				( );
			void			camera_to_selected				( );

			void			animation_curves_key_addedd		( int key_index );
			void			animation_curves_key_changed	( int key_index );
			void			animation_curves_key_removed	( int key_index );

			float			min_track_time					( );
			float			max_track_time					( );
			void			editor_track_time_changed		( );
			void			curve_editor_selection_changed	( int index );
			void			tick							( );
			void			tick_animation_curve_editor		( Single elapsed_time );
			u32				keys_count						( );
			math::float3	key_position					( u32 idx );
			math::float3	key_angles						( u32 idx );
			float			key_time						( u32 idx );
			curve_key_type	key_type						( xray::animation::enum_channel_id channel, u32 idx );
			math::float2	channel_key_tangent				( xray::animation::enum_channel_id channel, u32 idx, s32 tangent_idx );
			void			fill_animation_curve_editor		( );
			void			clear_animation_curve_editor	( );
			void			init_collision					( );
			void			init_keys_collision				( );
			void			init_key_tangents_collision		( );
			void			update_collisions				( );
			void			destroy_keys_collision			( );
			void			destroy_key_tangents_collision	( );
			void			insert_collision_in_tree		( collision::object* collision, float4x4 const& tranform );
			void			on_camera_model_ready			( resources::queries_result& data );

		public:

			animation_curve_editor^			get_animation_curve_editor	( );
			virtual		void				load_contents				( )										override;
			virtual		void				unload_contents				( bool bdestroy )						override;
			virtual		void				load_props					( configs::lua_config_value const& t )	override;
			virtual		void				save						( configs::lua_config_value t )			override;
			virtual		void				destroy_collision			( )										override;
			virtual		void				set_transform				( float4x4 const& transform )			override;
			
						void				set_key_transform			( u32 idx, math::float4x4 const& t );
						math::float4x4		get_key_transform			( u32 idx );
						void				insert_key					( float time, math::float3 const& pos, math::float3 const& rot );
						void				remove_key					( u32 idx );

						math::float3		get_key_tangent_position	( int idx, int tangent_idx );
						void				set_key_tangent_position	( int idx, int tangent_idx, math::float3 position );
						void				get_camera_transform		( Vector3D% position, Vector3D% rotation, Vector3D% scale );

			virtual		void				render						( )										override;
			virtual		aabb				get_aabb					( )										override;

			virtual		property_container^	get_property_container		( )										override;
			virtual		void				on_selected					( bool bselected )						override;

		#pragma endregion

		private:
			
			ref class curve_to_channel_binder
			{
			public:
				curve_to_channel_binder( object_track^ track_obj, curves_link^ link );
				curve_to_channel_binder( object_track^ track_obj, visual_curve^ curve, int channel_index );

			private:
				visual_curve^	m_curve;
				int				m_channel_index;

				curves_link^	m_curves_link;
				object_track^	m_track_obj;

			private:
				void			curve_key_added			( visual_curve_key^ key );
				void			curve_key_changed		( int key_index );
				void			curve_key_removed		( visual_curve_key^ key );

				void			link_key_added			( int key_index );
				void			link_key_changed		( int key_index );
				void			link_key_removed		( int key_index );

				void			read_key				( animation::EtKey* channel_key, float_curve_key^ key, bool need_convert );
				void			write_key				( animation::EtKey* channel_key, float_curve_key^ key, bool need_convert );

			public:
				void			update_key				( Int32 key_index );
				void			raise_key_changed		( Int32 key_index );
				
			}; //ref class curve_to_channel_binder

		}; // class object_track

		class collision_object_track_key: public collision::collision_object
		{
		private:
			typedef collision::collision_object	super;

		public:
			collision_object_track_key		( collision::geometry_instance* geometry, object_track^ o, int key_idx );

		private:
			int						m_key_idx;
			gcroot<object_track^>	m_owner;

		public:
			void					set_key_index( int new_key_index );
			virtual bool			touch	( ) const;
		
		}; //collision_object_track_key

		class collision_object_track_key_tangent: public collision::collision_object
		{
		private:
			typedef collision::collision_object	super;

		public:
			collision_object_track_key_tangent		( collision::geometry_instance* geometry, object_track^ o, int key_idx, int tangent_index );

		private:
			int						m_key_idx;
			int						m_tangent_idx;
			gcroot<object_track^>	m_owner;

		public:
			virtual bool			touch	( ) const;
		
		}; //collision_object_track_key_tangent

	} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_TRACK_H_INCLUDED
