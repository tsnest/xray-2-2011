////////////////////////////////////////////////////////////////////////////
//	Created		: 26.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;

namespace xray.editor.wpf_controls.control_containers
{
	public abstract class control
	{
		public	control		( String name )		
		{
			this.name	= name;
			width		= Double.NaN;
			height		= Double.NaN;
		}

		public				String				name;
		public				Object				tag;
		public				FrameworkElement	wpf_control;
		public				Double				width;
		public				Double				height;

		public abstract		FrameworkElement	generate_control	( );

		public				Boolean		same_as				( control other_control )
		{
			return other_control.name == name && other_control.GetType( ) == GetType( );
		}

		public abstract		void		merge_to			( FrameworkElement ui_element );
	}
}
