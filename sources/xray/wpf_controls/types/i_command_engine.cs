////////////////////////////////////////////////////////////////////////////
//	Created		: 15.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

namespace xray.editor.wpf_controls
{
	public interface i_command_engine
	{
		bool	run				( i_command command );
		int		rollback		( int steps );

		bool	run_preview		( i_command command );
		bool	preview			( );
		void	end_preview		( );
		
		void	clear_stack		( );
		bool	undo_stack_empty( );
		bool	redo_stack_empty( );
		bool	stack_empty		( );

		void	set_saved_flag	( );
		bool	saved_flag		( );
	}
}
