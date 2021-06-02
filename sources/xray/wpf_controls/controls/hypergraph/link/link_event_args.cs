////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls.hypergraph
{
	public class link_event_args: EventArgs
	{
		public link_event_args( link_id link_id, link_point source, link_point destination )
		{
			m_link_id				= link_id;
			m_source_point			= source;
			m_destination_point		= destination;
		}

		private		link_id					m_link_id;
		private		link_point				m_source_point;
		private		link_point				m_destination_point;

		public		link_id					link_id					
		{
			get
			{
				return m_link_id;
			}
		}
		public		Object					source_tag				
		{
			get
			{
				return m_source_point.tag;
			}
		}
		public		Object					destination_tag			
		{
			get
			{
				return m_destination_point.tag;
			}
		}
	}
}
