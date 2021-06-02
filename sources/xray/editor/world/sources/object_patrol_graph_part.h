////////////////////////////////////////////////////////////////////////////
//	Created		: 17.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_PATROL_GRAPH_PART_H_INCLUDED
#define OBJECT_PATROL_GRAPH_PART_H_INCLUDED

namespace xray
{
	namespace collision { class geometry_instance; }
	namespace editor
	{
		using	System::ComponentModel::BrowsableAttribute;
		using	xray::editor_base::type_converters::Float3_to_Vector3D_converter;
		using	xray::editor::wpf_controls::property_editors::attributes::type_converter_attribute;
		using	System::Windows::Media::Media3D::Vector3D;

		ref class object_patrol_graph;
		class object_patrol_graph_part_collision;

		typedef object_patrol_graph_part_collision part_collision;

		public ref class object_patrol_graph_part abstract
		{
		public:
			object_patrol_graph_part( object_patrol_graph^ graph, collision::geometry_instance& geometry );
			~object_patrol_graph_part( );

		public:
			[BrowsableAttribute(false)]
			[type_converter_attribute( Vector3D::typeid, Float3_to_Vector3D_converter::typeid )]
			virtual property	Float3		position;

		public:
			object_patrol_graph^	get_parent_graph	( );
			void					set_parent_graph	( object_patrol_graph^ graph );
					aabb			get_aabb			( );
			virtual void			select				( ) abstract;

		internal:
			part_collision*			get_collision		( );
			virtual float4x4		get_matrix			( ) abstract;

		protected:
			void					attach_collision	( );
			void					detach_collision	( );

		protected:
			object_patrol_graph^	m_parent_graph;
			part_collision*			m_collision;
			
		}; // class object_patrol_graph_part

	} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_PATROL_GRAPH_PART_H_INCLUDED