////////////////////////////////////////////////////////////////////////////
//	Created		: 17.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_PATROL_GRAPH_PART_TRANSFORM_DATA_H_INCLUDED
#define OBJECT_PATROL_GRAPH_PART_TRANSFORM_DATA_H_INCLUDED

namespace xray
{
	namespace editor
	{
		using xray::editor_base::transform_control_base;
		ref class object_patrol_graph_part;

		ref class object_patrol_graph_part_transform_data : editor_base::transform_control_object
		{

		public:
							object_patrol_graph_part_transform_data	( object_patrol_graph_part^ obj );
			virtual			~object_patrol_graph_part_transform_data( );
							!object_patrol_graph_part_transform_data( );

		public:
			virtual	bool	attach									( transform_control_base^ transform )		override;
			virtual void	start_modify							( editor_base::transform_control_base^ )	override;
			virtual void	execute_preview							( editor_base::transform_control_base^ )	override;
			virtual void	end_modify								( editor_base::transform_control_base^ )	override;
			virtual math::float4x4	get_ancor_transform				( )											override;

		private:
			object_patrol_graph_part^			m_graph_part;
			math::float4x4*						m_start_transform;

		};//track_object_key_transform_data

	} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_PATROL_GRAPH_PART_TRANSFORM_DATA_H_INCLUDED