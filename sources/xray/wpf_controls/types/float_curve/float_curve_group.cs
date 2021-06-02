////////////////////////////////////////////////////////////////////////////
//	Created		: 30.06.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;

namespace xray.editor.wpf_controls
{
	public class float_curve_group
	{
		public				float_curve_group			( String name )				
		{
			this.name	= name;

			m_curves	= new List<float_curve>( );
			m_groups	= new List<float_curve_group>( );

			curves		= new ReadOnlyCollection<float_curve>( m_curves );
			groups		= new ReadOnlyCollection<float_curve_group>( m_groups );

			effects		= new ObservableCollection<float_curve_effect>( );
			effects.CollectionChanged += effects_collection_changed;
		}

		private				List<float_curve>			m_curves;
		private				List<float_curve_group>		m_groups;

		public readonly		ReadOnlyCollection<float_curve>			curves;
		public readonly		ReadOnlyCollection<float_curve_group>	groups;

		public				String									name			
		{
			get;set;
		}
		public				ObservableCollection<float_curve_effect>effects			
		{
			get;
			private set;
		}

		public event		Action		group_changed;
		public event		Action		group_change_complete;
		public event		Action		group_hierarchy_changed;
		public event		Action		group_hierarchy_change_complete;

		private				void		effects_collection_changed			( Object sender, NotifyCollectionChangedEventArgs e )
		{
			on_group_changed			( );
			on_group_change_complete	( );
		}
		private				void		on_group_hierarchy_change_complete	( Object sender, EventArgs e )
		{
			on_group_hierarchy_change_complete( );
		}

		protected			void		on_group_changed		( )		
		{
			if( group_changed != null )
				group_changed( );
		}
		protected			void		on_group_change_complete( )		
		{
			if( group_change_complete != null )
				group_change_complete( );
		}
		protected			void		on_group_hierarchy_changed			( )		
		{
			if( group_hierarchy_changed != null )
				group_hierarchy_changed( );
		}
		protected			void		on_group_hierarchy_change_complete	( )		
		{
			if( group_hierarchy_change_complete != null )
				group_hierarchy_change_complete( );
		}
		
		public				void		add_group				( float_curve_group group )		
		{
			m_groups.Add				( group );
			
			group.group_changed						+= on_group_hierarchy_changed;
			group.group_change_complete				+= on_group_hierarchy_change_complete;
			group.group_hierarchy_changed			+= on_group_hierarchy_changed;
			group.group_hierarchy_change_complete	+= on_group_hierarchy_change_complete;

			on_group_changed			( );
			on_group_change_complete	( );
		}
		public				void		remove_group			( float_curve_group group )		
		{
			m_groups.Remove			( group );
			group.group_changed		-= on_group_changed;
			on_group_changed		( );
		}
		public				void		add_curve				( float_curve curve )			
		{
			m_curves.Add				( curve );
			
			curve.curve_changed			+= on_group_hierarchy_changed;
			curve.edit_completed		+= on_group_hierarchy_change_complete;
			
			on_group_changed			( );
			on_group_change_complete	( );
		}

		public				void		remove_curve			( float_curve curve )			
		{
			m_curves.Remove			( curve );
			curve.curve_changed		-= on_group_changed;
			on_group_changed		( );
		}
	}
}