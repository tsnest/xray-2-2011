////////////////////////////////////////////////////////////////////////////
//	Created		: 24.03.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_PATROL_GRAPH_H_INCLUDED
#define OBJECT_PATROL_GRAPH_H_INCLUDED

#include "object_base.h"
#include <xray/collision/collision_object.h>

using namespace System;
using namespace System::Collections::Generic;

namespace xray 
{
	namespace editor 
	{
		using System::ComponentModel::BrowsableAttribute;
		using System::Collections::ObjectModel::ReadOnlyCollection;
		using System::Windows::Media::Media3D::Vector3D;	
		using wpf_controls::property_container;
		using editor_base::actions_layer;

		ref class	tool_misc;
		ref class	object_patrol_graph;
		ref class	object_patrol_graph_part;
		ref class	object_patrol_graph_node;
		ref class	object_patrol_graph_edge;

		typedef object_patrol_graph_node						node;
		typedef object_patrol_graph_edge						edge;


		public ref class object_patrol_graph: object_base
		{
		internal:
			static		void				initialize_in_tool			( tool_misc^ tool );

		public:
						object_patrol_graph			( tool_misc^ t );
			virtual		~object_patrol_graph		( );

		public:
			property object_patrol_graph_part^		selected_graph_part			
			{
				object_patrol_graph_part^		get( ){ return m_selected_graph_part; }
			internal: void set( object_patrol_graph_part^ value ){ m_selected_graph_part = value; }
			}
			property ReadOnlyCollection<node^>^		nodes				
			{
				ReadOnlyCollection<node^>^		get( ){ return m_nodes_readonly; }
			}

		public:
			virtual		void				load_contents				( )										override;
			virtual		void				unload_contents				( bool bdestroy )						override;
			virtual		void				load_props					( configs::lua_config_value const& t )	override;
			virtual		void				save						( configs::lua_config_value t )			override;

			virtual		aabb				get_aabb					( )										override;
			virtual		void				destroy_collision			( )										override;
			
			virtual		void				set_transform				( float4x4 const& transform )			override;

						node^				create_node					( float3 position );
						node^				create_node					( float3 position, u32 index );

						edge^				create_edge					( node^ source_node, node^ destination_node );
						edge^				create_edge					( node^ source_node, node^ destination_node, u32 index );
						edge^				create_edge_to_node			( node^ node );

			virtual		void				render						( )										override;

			virtual		property_container^	get_property_container		( )										override;
			virtual		void				on_selected					( bool bselected )						override;
						List<String^>^		get_signals_list			( );

		internal:
						void				add_node					( node^ node );
						void				insert_node					( u32 index, node^ node );
						void				remove_node					( node^ node );
						node^				get_node_at					( u32 index );
						void				select_graph_part			( object_patrol_graph_part^ part );
						void				insert_graph_in_collision	( );
						void				remove_graph_from_collision	( );
						void				select_patrol_graph			( );
						void				deselect_patrol_graph		( );

			static		void				toggle_edit_mode			( object_patrol_graph^ graph );
			static		bool				is_in_edit_mode				( );
			static		void				toggle_node_adding_mode		( );
			static		bool				is_in_node_adding_mode		( );

		private:
						void				init_collision				( );
						void				run_command					( editor_base::command^ command );

		private:
			tool_misc^						m_tool_misc;
			property_container^				m_temp_container;
			Boolean							m_is_selected;

			List<node^>^					m_nodes;
			ReadOnlyCollection<node^>^		m_nodes_readonly;
			object_patrol_graph_part^		m_selected_graph_part;

		internal:
			static List<object_patrol_graph^>^	s_selected_patrol_graphs;

		private:
			static Boolean						s_is_in_node_adding_mode;
			static actions_layer^				s_actions_layer;

		private:
			typedef object_base		super;

		}; // class object_patrol_graph

	} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_PATROL_GRAPH_H_INCLUDED