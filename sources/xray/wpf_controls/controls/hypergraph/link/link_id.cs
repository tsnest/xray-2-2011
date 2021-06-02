////////////////////////////////////////////////////////////////////////////
//	Created		: 09.03.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls.hypergraph
{
	public class link_id
	{
		public link_id( String output_node_id, String  output_link_point_id, String  input_node_id, String  input_link_point_id )
		{
			this.output_node_id			= output_node_id;
			this.input_node_id			= input_node_id;
			this.output_link_point_id	= output_link_point_id;
			this.input_link_point_id	= input_link_point_id;

			id							= output_node_id;
			if( !String.IsNullOrEmpty	( output_link_point_id ) )
				id						+= "/" + output_link_point_id;

			id							+= "^" + input_node_id;
			if( !String.IsNullOrEmpty	( input_link_point_id ) )
				id						+= "/" + input_link_point_id;
		}

		public				String		output_node_id			
		{
			get; private set;
		}
		public				String		input_node_id			
		{
			get; private set;
		}
		public				String		output_link_point_id	
		{
			get; private set;
		}
		public				String		input_link_point_id		
		{
			get; private set;
		}
		public				String		id						
		{
			get; private set;
		}

		public override		Int32		GetHashCode		( )		
		{
			var code = id.GetHashCode( );
			return code;
		}
		public override		String		ToString		( )		
		{
			return  id;
		}
	}
}
