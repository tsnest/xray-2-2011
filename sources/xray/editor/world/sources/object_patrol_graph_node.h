////////////////////////////////////////////////////////////////////////////
//	Created		: 17.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_PATROL_GRAPH_NODE_H_INCLUDED
#define OBJECT_PATROL_GRAPH_NODE_H_INCLUDED

#include "object_patrol_graph_part.h"

using namespace System;
using namespace System::Collections::Generic;

namespace xray
{
	namespace editor
	{
		using		System::ComponentModel::BrowsableAttribute;

		ref class	object_patrol_graph;
		ref class	object_patrol_graph_edge;
		ref class	object_patrol_graph_look_point;

		typedef		object_patrol_graph				graph;
		typedef		object_patrol_graph_edge		edge;
		typedef		object_patrol_graph_look_point	look_point;
		typedef		System::Collections::ObjectModel::ReadOnlyCollection<edge^>	read_only_edges;
		typedef		System::Collections::ObjectModel::ReadOnlyCollection<look_point^>	read_only_look_points;

		enum class	object_patrol_look_point_selection_type
		{
			random,
			sequential
		}; //enum class	object_patrol_look_point_selection_type

		typedef		object_patrol_look_point_selection_type				look_point_selection_type;

		ref class object_patrol_graph_node: object_patrol_graph_part
		{
		private:
			typedef object_patrol_graph_part super;

		public:
			object_patrol_graph_node	( object_patrol_graph^ graph, Float3 position );
			object_patrol_graph_node	( object_patrol_graph^ graph, configs::lua_config_value const config_value );
			~object_patrol_graph_node	( );

		public:
			property u32						index				
			{
				u32							get		( );
			}
			property Boolean					is_absolute_position;
			[BrowsableAttribute(true)]
			property Float3						position		
			{
				virtual Float3				get		( ) override;
				virtual void				set		( Float3 value ) override;
			}
			property String^					signal;

			[DescriptionAttribute( "look point selection type" )]
			property look_point_selection_type	look_point_selection;

			[BrowsableAttribute(false)]
			property read_only_edges^			outgoing_edges	
			{
				read_only_edges^			get		( ){ return m_readonly_out_edges; }
			}
			[BrowsableAttribute(false)]
			property read_only_edges^			incoming_edges	
			{
				read_only_edges^			get		( ){ return m_readonly_in_edges; }
			}
			[BrowsableAttribute(false)]
			property read_only_look_points^		look_points	
			{
				read_only_look_points^		get		( ){ return m_readonly_look_points; }
			}

		public:
					void		fix_up_position		( float3 delta_position );
			virtual void		select				( ) override;

					void		load				( configs::lua_config_value const config_value );
					void		save				( configs::lua_config_value config_value );
					void		attach				( );
					void		attach				( u32 index );
					void		detach				( );

		internal:
					void		load_edges			( configs::lua_config_value const config_value );
					void		add_outgoing_edge	( edge^ edge );
					void		add_outgoing_edge	( edge^ edge, u32 index );
					void		remove_outgoing_edge( edge^ edge );
					void		add_incoming_edge	( edge^ edge );
					void		remove_incoming_edge( edge^ edge );
					void		add_look_point		( look_point^ look_point );
					void		add_look_point		( u32 index, look_point^ look_point );
					void		remove_look_point	( look_point^ look_point );
			virtual float4x4	get_matrix			( ) override;
					
		private:
			Float3					m_position;
			List<edge^>^			m_outgoing_edges;
			List<edge^>^			m_incoming_edges;
			List<look_point^>^		m_look_points;
			read_only_edges^		m_readonly_in_edges;
			read_only_edges^		m_readonly_out_edges;
			read_only_look_points^	m_readonly_look_points;

		}; // class object_patrol_graph_node

	} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_PATROL_GRAPH_NODE_H_INCLUDED