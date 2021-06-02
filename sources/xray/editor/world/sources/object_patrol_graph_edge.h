////////////////////////////////////////////////////////////////////////////
//	Created		: 17.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_PATROL_GRAPH_EDGE_H_INCLUDED
#define OBJECT_PATROL_GRAPH_EDGE_H_INCLUDED

#include "object_patrol_graph_part.h"

namespace xray
{
	namespace editor
	{
		using	xray::editor::wpf_controls::property_editors::attributes::value_range_and_format_attribute;

		ref class object_patrol_graph;

		ref class object_patrol_graph_edge: object_patrol_graph_part
		{
		public:
			object_patrol_graph_edge	( object_patrol_graph^ parent );
			object_patrol_graph_edge	( node^ source_node, configs::lua_config_value const config_value );
			~object_patrol_graph_edge	(  );

		public:
			[value_range_and_format_attribute( 0, 100, 0.01, 2 )]
			property Single		probability
			{
				Single		get( );
				void		set( Single value );
			}

			[BrowsableAttribute(false)]
			property node^		source_node	
			{
				node^		get( ){ return m_source_node; }
				void		set( node^ value ){ m_source_node = value; }
			}
			[BrowsableAttribute(false)]
			property node^		destination_node	
			{
				node^		get( ){ return m_destination_node; }
				void		set( node^ value ){ m_destination_node = value; }
			}
			[BrowsableAttribute(false)]
			property Int32		index			
			{
				Int32		get( );
			}

		public:
			virtual void		select				( ) override;

					void		load				( configs::lua_config_value const config_value );
					void		save				( configs::lua_config_value config_value );
					void		attach				( node^ source, node^ destination );
					void		attach				( node^ source, u32 index, node^ destination );
					void		detach				( );

		internal:
					void		fix_position		( );
					float4x4	fixed_position		( );
			virtual float4x4	get_matrix			( ) override;

		private:
			Single				m_probability;
			node^				m_source_node;
			node^				m_destination_node;

		private:
			typedef object_patrol_graph_part super;
			
		}; // class object_patrol_graph_edge

	} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_PATROL_GRAPH_EDGE_H_INCLUDED