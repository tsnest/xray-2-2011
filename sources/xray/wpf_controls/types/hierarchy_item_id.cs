////////////////////////////////////////////////////////////////////////////
//	Created		: 04.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using xray.editor.wpf_controls.helpers;

namespace xray.editor.wpf_controls
{
	public class hierarchy_item_id
	{
		public hierarchy_item_id( params Int32[] ids )
		{
			m_ids = ids;
		}
		public hierarchy_item_id( Int32[] present, params Int32[] additional )
		{
			debug.assert( additional.Length > 0 );

			m_ids = new int[present.Length + additional.Length];
			present.CopyTo		( m_ids, 0 );
			additional.CopyTo	( m_ids, present.Length );
		}
		public hierarchy_item_id( hierarchy_item_id other_id, params Int32[] additional ): this( other_id.ids, additional )
		{	
		}

		private readonly	Int32[]		m_ids;

		public				Int32[]		ids			
		{
			get
			{
				return m_ids;
			}
		}
		public				Int32		last_index	
		{
			get
			{
				if( m_ids.Length == 0 )
					return -1;

				return m_ids[ m_ids.Length - 1 ];
			}
			set
			{
				if( m_ids.Length == 0 )
					return;

				m_ids[ m_ids.Length - 1 ] = value;
			}
		}
	}
}
