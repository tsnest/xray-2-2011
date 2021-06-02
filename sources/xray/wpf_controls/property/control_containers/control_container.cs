////////////////////////////////////////////////////////////////////////////
//	Created		: 19.07.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Windows.Controls;

namespace xray.editor.wpf_controls.control_containers
{
	public abstract class control_container
	{
		protected			control_container	( property_descriptor descriptor )			
		{
			m_descriptor		= descriptor;
			controls			= new Dictionary<String, control>( );
		}

		private readonly	property_descriptor				m_descriptor;
		protected readonly	Dictionary<String, control>		controls;

		public				String							category	
		{
			get
			{
				return m_descriptor.category;
			}
			set
			{
				m_descriptor.category = value;
			}
		}
		public				String							description	
		{
			get
			{
				return m_descriptor.description;
			}
			set
			{
				m_descriptor.description = value;
			}
		}
		public				button							add_button				( String caption, Action<button> click_callback )						
		{
			var button			= new button( caption )			{ content = caption };
			button.click		+= click_callback;
			controls.Add		( button.name, button );

			return button;
		}
		public				toggle_button					add_toggle_button		( String caption, Action<toggle_button> click_callback )		
		{
			var button			= new toggle_button( caption )	{ content = caption };
			button.toggle		+= click_callback;
			controls.Add		( button.name, button );

			return button;
		}

		protected internal abstract	Panel					generate_panel			( );
		public						void					merge_to				( control_container other )
		{
			foreach( var pair in controls )
			{
				if( other.controls.ContainsKey( pair.Key ) && other.controls[pair.Key].GetType( ) == pair.Value.GetType( ) )
					pair.Value.merge_to( other.controls[pair.Key].wpf_control );
			}
		}
	}
}