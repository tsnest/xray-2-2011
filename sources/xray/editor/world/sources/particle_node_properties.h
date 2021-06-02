////////////////////////////////////////////////////////////////////////////
//	Created		: 30.01.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_NODE_TYPE_H_INCLUDED
#define PARTICLE_NODE_TYPE_H_INCLUDED

#include "float_curve.h"

using namespace System::ComponentModel;

namespace xray {
	namespace editor {
	//	public delegate void particle_property_changed(particle_graph_node^ node);
		
		ref struct particle_node_properties
		{
		
		public:
			particle_node_properties(particle_graph_node^ owner)
			{
				type			= "";
				name			= "";
				m_length_time	= 600;
				m_start_time	= 0;	
				m_length_time_pos = m_start_time + m_length_time;
				m_owner			= owner;
				fc				= gcnew xray::editor::float_curve(); 
			}
			particle_node_properties(particle_graph_node^ owner ,String^ set_type, String^ set_name)
			{
				type			= set_type;
				name			= set_name;
				m_length_time	= 600;
				m_start_time	= 0;	
				m_length_time_pos = m_start_time + m_length_time;
				m_owner			= owner;
				
			}

		private:
			int					m_start_time;
			int					m_length_time;
			int					m_length_time_pos;
			particle_node_properties^ my_old_version;
			bool				m_transaction_active;

		private:
			void				set_new_start_time(int value);
			void				set_start_time_impl(int value);

			void				set_length_time_impl(int value);
		public:
			void				set_start_length_offsets(int offset);
			void				start_transaction();
			void				end_transaction();
			
		public:
//			event particle_property_changed^ on_property_changed;

			[ReadOnlyAttribute(true)]
			property String^		type;
			[ReadOnlyAttribute(true)]
			property String^		name;

			[DisplayNameAttribute("fc"), DescriptionAttribute("object fc"), CategoryAttribute("general"), 
				ReadOnlyAttribute(false), MergablePropertyAttribute(false)]
			property xray::editor::float_curve^ fc;

			property int			start_time{
				int					get(){return m_start_time;}
				void				set(int value){set_start_time_impl(value);}
			};
			property int			length_time{
				int					get(){return m_length_time;}
				void				set(int value){set_length_time_impl(value);}
			};

			[Browsable(false)]
			property particle_graph_node^	m_owner;
			[Browsable(false)]
			property int					time_limit;
		}; // class particle_node_type
	}; //namespace editor
}; //namespace xray

#endif // #ifndef PARTICLE_NODE_TYPE_H_INCLUDED