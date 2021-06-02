////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Windows.Media;

namespace xray.editor.wpf_controls
{
	public class float_curve
	{

		#region | Initialize |


		public      float_curve  ( )									
		{
			keys	= new List<float_curve_key>();
            color	= Colors.Black;
			name	= "curve";
			effects	= new ObservableCollection<float_curve_effect>( );

			effects.CollectionChanged += effects_collection_changed;
		}
		public      float_curve  ( List<float_curve_key> keys_list )	
		{
			keys    = keys_list;
            color   = Colors.Black;
			name	= "curve";
			effects	= new ObservableCollection<float_curve_effect>( );
		}
            

		#endregion

		#region | Properties |


		public      List<float_curve_key>		keys			
		{
			get; set; 
		}
        public      Color						color			
		{
			get; set; 
		}
		public		Boolean						is_range		
		{
			get;
			set;
		}
		public		String						name			
		{
			get;
			set;
		}
		public		float_curve_monotone_type	monotone_type	
		{
			get;set;
		}
		public		ObservableCollection<float_curve_effect>	effects		
		{
			get;
			private set;
		}


		#endregion

		#region |   Events   |


		public event		Action				curve_changed;
		public event		EventHandler		edit_completed;


		#endregion

		#region |   Methods  |


		private		void			effects_collection_changed	( Object sender, NotifyCollectionChangedEventArgs e) 
		{
			on_curve_changed	( );
			on_edit_completed	( );
		}
		
		protected	void			on_curve_changed			( )		
		{
			if( curve_changed != null )
				curve_changed( );
		}
		protected	void			on_edit_completed			( )		
		{
			if( edit_completed != null )
				edit_completed( this, EventArgs.Empty );
		}
		
		public		void			fire_curve_changed			( )		
		{
			on_curve_changed( );
		}
		public		void			fire_edit_completed			( )		
		{
			on_edit_completed( );
		}
		
		public		float_curve		clone						( )		
		{
			var curve = new float_curve( );

			foreach( var key in keys )
				curve.keys.Add( new float_curve_key( key.position, key.key_type, key.left_tangent, key.right_tangent, key.range_delta ) );

			return curve;
		}


		#endregion

	}
}
