////////////////////////////////////////////////////////////////////////////
//	Created		: 15.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

namespace xray.editor.wpf_controls
{
	public interface i_command
	{
		bool	commit			( );
		void	rollback		( );

		void	start_preview	( );
		bool	end_preview		( );
	}
}
