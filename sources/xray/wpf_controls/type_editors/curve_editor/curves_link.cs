////////////////////////////////////////////////////////////////////////////
//	Created		: 29.03.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;

namespace xray.editor.wpf_controls.curve_editor
{
	/// <summary>
	/// Interaction logic for key_link.xaml
	/// </summary>
	public class curves_link
	{
		
		public				curves_link				( IList<curve_editor.visual_curve> curves )	
		{
			m_curves			= curves;
			m_readonly_curves	= new ReadOnlyCollection<curve_editor.visual_curve>( m_curves );
			initialize			( );
		}
		private				void					initialize		( )				
		{
			foreach( var link_curve in m_curves )
			{
				var curve = link_curve;

				link_curve.key_changed		+= key =>				curve_key_changed	( curve, key );
				link_curve.keys_swaped		+= ( key1, key2 ) =>	curve_key_swaped	( curve, key1, key2 );
				link_curve.key_added		+= key =>				curve_key_added		( curve, key );
				link_curve.key_removed		+= key =>				curve_key_removed	( curve, key );
			}
		}

		private readonly	IList<curve_editor.visual_curve>						m_curves;
		private readonly	ReadOnlyCollection<curve_editor.visual_curve>		m_readonly_curves;
		private				Boolean									m_is_setting_value;

		public				ReadOnlyCollection<curve_editor.visual_curve>		curves		
		{
			get
			{
				return m_readonly_curves;
			}
		}

		public event		Action<Int32>			key_added;
		public event		Action<Int32>			key_removed;
		public event		Action<Int32>			key_changed;

		private				void					curve_key_swaped		( curve_editor.visual_curve curve, Int32 index1, Int32 index2 )	
		{
			if( m_is_setting_value )
				return;

			m_is_setting_value = true;

			foreach( var link_curve in m_curves.Where( link_curve => link_curve != curve ) )
			{
				visual_curve_key.swap					( link_curve.keys[index1], link_curve.keys[index2] );
				var tem_pos_x							= link_curve.keys[index1].position_x;
				link_curve.keys[index1].position_x		= link_curve.keys[index2].position_x;
				link_curve.keys[index2].position_x		= tem_pos_x;
			}

			m_is_setting_value = false;
		}
		private				void					curve_key_changed		( curve_editor.visual_curve curve, Int32 index )					
		{
			if( m_is_setting_value )
				return;

			m_is_setting_value = true;

			foreach( var link_curve in m_curves.Where( link_curve => link_curve != curve ) )
				link_curve.keys[index].position_x = curve.keys[index].position_x;

			m_is_setting_value = false;

			if( key_changed != null )
				key_changed( index ); 
		}
		private				void					curve_key_added			( curve_editor.visual_curve curve, visual_curve_key key )		
		{
			if( m_is_setting_value )
				return;

			m_is_setting_value = true;

			foreach( var link_curve in m_curves.Where( link_curve => link_curve != curve ) )
				link_curve.add_key( key.position_x );

			m_is_setting_value = false;

			if( key_added != null )
				key_added( key.index ); 
		}
		private				void					curve_key_removed		( curve_editor.visual_curve curve, visual_curve_key key )		
		{
			if( m_is_setting_value )
				return;

			m_is_setting_value = true;

			foreach( var link_curve in m_curves.Where( link_curve => link_curve != curve ) )
				link_curve.remove_key( link_curve.keys[key.index] );

			m_is_setting_value = false;

			if( key_removed != null )
				key_removed( key.index ); 
		}

	}
}
