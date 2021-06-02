////////////////////////////////////////////////////////////////////////////
//	Created		: 24.11.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.ComponentModel;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace xray.editor.wpf_controls.animation_playback
{
	public class animation_event: INotifyPropertyChanged
	{
		public event PropertyChangedEventHandler PropertyChanged;

		public animation_event(animation_item parent)
		{
			m_parent = parent;
		}
		private void on_property_changed( String property_name )
		{
			if( PropertyChanged != null )
				PropertyChanged( this, new PropertyChangedEventArgs( property_name ) );
		}
		internal void update()
		{
			on_property_changed("position");
			on_property_changed("text");
		}

  
		private		animation_item	m_parent;
		internal	Single			m_position;
		private		String			m_text;

		public String text 
		{
			get
			{
				return m_text;
			}
			set
			{
				m_text = value;
				on_property_changed("text");
			}
		}

		public Single position
		{
			get
			{
				return m_position * m_parent.m_panel.time_layout_scale - 10;
			}
			set
			{
				m_position = value;
				on_property_changed("position");
			}
		}
		public Single time
		{
			get
			{
				return m_position;
			}
			set{}
		}
	}
}
