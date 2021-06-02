////////////////////////////////////////////////////////////////////////////
//	Created		: 01.12.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_PATROL_GRAPH_LOOK_POINT_H_INCLUDED
#define OBJECT_PATROL_GRAPH_LOOK_POINT_H_INCLUDED

#include "object_patrol_graph_part.h"

using namespace System;

namespace xray
{
	namespace editor
	{
		using		System::ComponentModel::BrowsableAttribute;
		using		xray::editor_base::resource_chooser_external_editor;
		using		xray::editor::wpf_controls::property_editors::attributes::external_editor_attribute;

		ref class	object_patrol_graph;
		ref class	object_patrol_graph_node;

		typedef		object_patrol_graph			graph;
		typedef		object_patrol_graph_node	node;

		using		xray::editor::wpf_controls::property_editors::attributes::value_range_and_format_attribute;

		ref class object_patrol_graph_look_point: object_patrol_graph_part
		{
		private:
			typedef object_patrol_graph_part super;

		public:
			object_patrol_graph_look_point	( object_patrol_graph^ parent, Float3 position );
			object_patrol_graph_look_point	( node^ parent_node, configs::lua_config_value const config_value );
			~object_patrol_graph_look_point	( );

		public:
			property u32				index				
			{
				u32							get		( );
			}
			property Boolean			is_absolute_position;
			[BrowsableAttribute(true)]
			property Float3				position		
			{
				virtual Float3				get		( ) override;
				virtual void				set		( Float3 value ) override;
			}
			property Single				duration;
			property String^			signal_on_begin;
			property String^			signal_on_end;
			[external_editor_attribute( resource_chooser_external_editor::typeid, "animations_list" )]
			property String^			animation;
			[value_range_and_format_attribute( 0, 100, 0.01, 2 )]
			property Single				probability
			{
				Single						get( );
				void						set( Single value );
			}
			[BrowsableAttribute(false)]
			property node^				parent_node	
			{
				node^						get( ){ return m_parent_node; }
				void						set( node^ value ){ m_parent_node = value; }
			}

		public:
			virtual void		select				( ) override;

					void		load				( configs::lua_config_value const config_value );
					void		save				( configs::lua_config_value config_value );
					void		attach				( node^ parent );
					void		attach				( u32 index, node^ parent );
					void		detach				( );

		internal:
			virtual float4x4	get_matrix			( ) override;
					
		private:
			node^				m_parent_node;
			Float3				m_position;
			Single				m_probability;

		}; // class object_patrol_graph_look_point

	} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_PATROL_GRAPH_LOOK_POINT_H_INCLUDED