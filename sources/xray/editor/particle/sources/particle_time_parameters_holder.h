////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_TIME_PARAMETERS_HOLDER_H_INCLUDED
#define PARTICLE_TIME_PARAMETERS_HOLDER_H_INCLUDED

namespace xray {
	namespace particle_editor {	

		ref class particle_graph_node;

		public ref class particle_time_parameters_holder {
		public:
			particle_time_parameters_holder(particle_graph_node^ owner, configs::lua_config_value const& prop_config)
				:m_owner(owner)
			{
				m_config = CRT_NEW(configs::lua_config_value) (prop_config);	
				m_changes_config = CRT_NEW(configs::lua_config_value)(configs::create_lua_config()->get_root());
			}
			!particle_time_parameters_holder()
			{
				CRT_DELETE(m_config);
				CRT_DELETE(m_changes_config);
			}
			~particle_time_parameters_holder()
			{
				this->!particle_time_parameters_holder();
			}

		private:

#pragma region | InnerTypes |

#pragma endregion

#pragma region | Initialize |

#pragma endregion

#pragma region | Fields |
		private:
			float						m_start_time;
			float						m_length_time;
			particle_graph_node^		m_owner;
			configs::lua_config_value*	m_config;
			configs::lua_config_value*	m_changes_config;
#pragma endregion

#pragma region | Methods |

		public:
			[CategoryAttribute("Time Parameters")]
			property float Delay {
				float get(){return get_start_time_impl();}
				void set(float value){set_start_time_impl(value);}				
			}
			[CategoryAttribute("Time Parameters")]
			property float Duration {
				float get(){return get_length_time_impl();}
				void set(float value){set_length_time_impl(value);}				
			}

		public:
			void					set_start_time_impl(float value);
			float					get_start_time_impl();

			void					set_length_time_impl(float value);
			float					get_length_time_impl();
#pragma endregion


	}; // class particle_node_time_parameters_holder


} // namespace particle_editor
} // namespace xray

#endif // #ifndef PARTICLE_TIME_PARAMETERS_HOLDER_H_INCLUDED