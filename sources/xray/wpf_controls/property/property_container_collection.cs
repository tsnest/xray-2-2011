////////////////////////////////////////////////////////////////////////////
//	Created		: 11.10.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;

namespace xray.editor.wpf_controls
{
	public class property_container_collection: property_container_base
	{

		#region | Initialize |


		public property_container_collection( String item_name_template )
		{
			properties		= new property_collection( this );
			this.item_name_template = item_name_template;
		}


		#endregion

		#region | Properties |


		public new property_collection properties
		{
			get
			{
				return (property_collection)base.properties;
			}
			private set
			{
				base.properties = value;
			}
		}
		public String item_name_template;

		#endregion

		#region |   Events   |


		public event			Action<Int32>			property_added;
		public event			Action<Int32>			property_removed;


		#endregion

		#region |  Method   |


		private			void			on_property_added		( Int32 index )
		{
			if( property_added != null )
				property_added( index );
		}
		private			void			on_property_removed		( Int32 index )
		{
			if( property_removed != null )
				property_removed( index );
		}


		#endregion

		#region | InnerTypes |


		public class property_collection: property_collection_base, IList<property_descriptor>
		{

			#region | Initialize |


			internal	property_collection		( property_container_collection owner ):base( owner )	 
			{
				m_properties	= new List<property_descriptor>( );
			}


			#endregion

			#region |   Fields   |


			private readonly		List< property_descriptor >			m_properties;


			#endregion

			#region | Properties |


			public override			Int32					count						
			{
				get
				{
					return m_properties.Count;
				}
			}
		

			#endregion

			#region |  Methods   |


			public static 		implicit operator			List<property_descriptor>	( property_collection collection )		
			{
				return collection.m_properties;
			}

			public override		property_descriptor			add				( property_descriptor desc )	
			{
				m_properties.Add( desc );
				desc.owner		= m_owner;

				( (property_container_collection)m_owner ).on_property_added( m_properties.Count - 1 );

				return desc;
			}
			public override		void						remove			( property_descriptor desc )												
			{
				var index = m_properties.IndexOf( desc );
				remove( index );
			}
			public				void						remove			( Int32 property_id )
			{
				var desc				= m_properties[property_id];
				desc.owner				= null;
				desc.owner_property		= null;
				m_properties.RemoveAt	( property_id );

				( (property_container_collection)m_owner ).on_property_removed( property_id );
			}
			public override		void						clear			( )																			
			{
				foreach ( var descriptor in m_properties )
				{
					descriptor.owner			= null;
					descriptor.owner_property	= null;
				}
				m_properties.Clear			( );
				m_owner.refresh_properties	( );
			}

			public override		Boolean						contains		( property_descriptor desc )												
			{
				return m_properties.IndexOf( desc ) >= 0;
			}

			public override		IEnumerator<property_descriptor> GetEnumerator	( )		
			{
				return m_properties.GetEnumerator();
			}


			#endregion

			#region | IListImpl  |


			public		Int32					Count				
			{
				get { return m_properties.Count; }
			}
			public		Boolean					IsReadOnly			
			{
				get { return false; }
			}
			public		property_descriptor		this[Int32 index]	
			{
				get { return m_properties[ index ]; }
				set { m_properties[ index ] = value; }
			}

			public		void					Add			( property_descriptor item )						
			{
				add( item );
			}
			public		void					Clear		( )													
			{
				m_properties.Clear();
			}
			public		Boolean					Contains	( property_descriptor item )						
			{
				return m_properties.Contains( item );
			}
			public		void					CopyTo		( property_descriptor[] array, Int32 arrayIndex )	
			{
				m_properties.CopyTo( array, arrayIndex );
			}
			public		Boolean					Remove		( property_descriptor item )						
			{
				remove( item );
				return true;
			}
			public		Int32					IndexOf		( property_descriptor item )						
			{
				return m_properties.IndexOf( item );
			}
			public		void					Insert		( Int32 index, property_descriptor item )			
			{
				m_properties.Insert( index, item );
				( (property_container_collection)m_owner ).on_property_added( index );
			}
			public		void					RemoveAt	( Int32 index )										
			{
				remove( index );
			}
			

			#endregion

		}


		#endregion

	}
}
