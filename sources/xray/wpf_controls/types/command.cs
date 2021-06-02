////////////////////////////////////////////////////////////////////////////
//	Created		: 15.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

namespace xray.editor.wpf_controls
{
	abstract class command: i_command
	{
		public virtual bool		commit			( )		
		{
			return false;
		}
		public virtual void		rollback		( )		
		{
			
		}
		public virtual void		start_preview	( )		
		{
			
		}
		public virtual bool		end_preview		( )		
		{
			return false;
		}
	}
}
