////////////////////////////////////////////////////////////////////////////
//	Created		: 27.04.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows.Input;
using System.Windows.Shapes;

namespace xray.editor.wpf_controls.curve_editor.tools
{
	internal class add_tool: tool_base
	{
		public					add_tool		( curve_editor_panel panel ): base( panel )		
		{
			
		}

		public override			Boolean			mouse_down		( MouseButtonEventArgs e )		
		{
			if( e.ChangedButton == MouseButton.Left && Keyboard.PrimaryDevice.Modifiers == ModifierKeys.None )
			{
				var picked_element = m_parent_panel.curves_panel.InputHitTest( e.GetPosition( m_parent_panel.curves_panel ) );
				if( picked_element is Path && ((Path)picked_element).Parent is visual_curve )
				{
					m_parent_panel.deselect_all_keys( );
					var curve = (visual_curve)((Path)picked_element).Parent;
					curve.add_key( m_parent_panel.visual_to_logical_point( e.GetPosition( curve ) ).X );

					m_is_in_action = true;

					return true;
				}

				return false;
			}
			return false;
		}
		public override			Boolean			mouse_move		( MouseEventArgs e )			
		{
			if( m_is_in_action )
				return true;

			return false;
		}
		public override			Boolean			mouse_up		( MouseButtonEventArgs e )		
		{
			if( !m_is_in_action )
				return false;

			m_is_in_action = false;
			return true;
		}
	}
}
