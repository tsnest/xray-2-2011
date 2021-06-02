////////////////////////////////////////////////////////////////////////////
//	Created		: 01.12.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_LOGIC_H_INCLUDED
#define OBJECT_LOGIC_H_INCLUDED
#include "object_base.h"

using namespace System::Windows::Forms;

namespace xray {
	namespace editor {

		ref class tool_scripting;
		ref class object_scene;
		ref class object_composite;

		public delegate void on_parent_scene_changed( object_scene^ scene );

		public ref class object_logic abstract : public object_base 
		{
			typedef object_base		super;
		public:
			object_logic				(tool_scripting^ t);
			virtual						~object_logic( );

			virtual void			destroy_collision		( )										override;
			virtual void			load_props				( configs::lua_config_value const& t )	override;
			virtual void			save					( configs::lua_config_value t )			override;
			
			virtual void			load_contents			( )												override;
			virtual void			unload_contents			( bool )										override;
			virtual void			on_selected				( bool bselected )								override;

			virtual	void			set_name				( System::String^ name, bool refresh_ui )		override;
			//virtual bool			visible_for_project_tree( )												override { return false; }

			//virtual bool			visible_for_project_tree( ) override						{ return false;}
		
			virtual bool			is_scene				( ) { return false; }
			virtual bool			is_job					( ) { return false; }
			virtual bool			is_npc					( ) { return false; }
			virtual void			show_logic				( ) { };

		protected:
			void					initialize_collision	( );



		protected:
			tool_scripting^			m_tool_scripting;
			object_scene^			m_parent_scene;

		private:
			object_composite^		m_parent_composite; 

		//	u32						m_type;
			
		public:
			property tool_scripting^			tool
			{
				tool_scripting^					get( ){ return m_tool_scripting; };
			}
			property object_scene^				parent_scene
			{
				object_scene^					get( ){ return m_parent_scene; };
				void							set( object_scene^ scene );
			}

			property object_composite^			parent_composite 
			{
				object_composite^				get( );
				void							set( object_composite^ parent ) { ASSERT( is_scene() && get_persistent( ) , "You cant set composite parent to this object ( not a root scene!!! )" ); m_parent_composite = parent; }
			}


			event on_parent_scene_changed^		on_parent_scene_changed;

		}; // class object_npc


	} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_LOGIC_H_INCLUDED
