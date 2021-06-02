////////////////////////////////////////////////////////////////////////////
//	Created		: 26.11.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_NPC_H_INCLUDED
#define OBJECT_NPC_H_INCLUDED


#include "object_logic.h"
#include "i_job_resource.h"


namespace xray {
	namespace editor {
		
		ref class tool_scripting;

		public ref class object_npc : public object_logic , public i_job_resource
		{
			typedef object_logic		super;
		public:
			object_npc				(tool_scripting^ t);

			virtual void			load_props				( configs::lua_config_value const& t )	override;
			virtual void			save					( configs::lua_config_value t )			override;
			virtual void			render					( ) override;
			virtual void			on_selected				( bool bselected )								override;

			virtual void			setup_job				( object_job^ job );

			virtual bool			is_npc					() override { return true; }

		private:

			object_job^				m_job;

		}; // class object_npc


	} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_NPC_H_INCLUDED