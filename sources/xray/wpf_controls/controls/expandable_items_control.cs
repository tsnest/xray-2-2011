////////////////////////////////////////////////////////////////////////////
//	Created		: 31.03.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;
using System.Windows.Controls;

namespace xray.editor.wpf_controls
{
	/// <summary>
	/// Interaction logic for expandable_items_control.xaml
	/// </summary>
	public class expandable_items_control: HeaderedItemsControl
	{
		static		expandable_items_control		( )								
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(expandable_items_control), new FrameworkPropertyMetadata(typeof(expandable_items_control)));
        }
		public		expandable_items_control		( )								
		{
			m_left_pocket	= new StackPanel{ Orientation = Orientation.Horizontal };
			m_right_pocket	= new StackPanel{ Orientation = Orientation.Horizontal };

			left_pocket		= m_left_pocket;
			right_pocket	= m_right_pocket;
		}

		private					Panel					m_left_pocket;
		private					Panel					m_right_pocket;

		public static readonly	DependencyProperty		is_expandedProperty				= DependencyProperty.Register( "is_expanded", typeof(Boolean), typeof(expandable_items_control), new PropertyMetadata( is_expanded_changed ) );
		public static readonly	DependencyProperty		expand_visibilityProperty		= DependencyProperty.Register( "expand_visibility", typeof(Visibility), typeof(expandable_items_control), new PropertyMetadata( Visibility.Visible ) );
		public static readonly	DependencyProperty		right_pocketProperty			= DependencyProperty.Register( "right_pocket", typeof(Panel), typeof(expandable_items_control), new PropertyMetadata( null ) );
		public static readonly	DependencyProperty		left_pocketProperty				= DependencyProperty.Register( "left_pocket", typeof(Panel), typeof(expandable_items_control), new PropertyMetadata( null ) );

		public static readonly	RoutedEvent				priview_expanded_changedEvent	= EventManager.RegisterRoutedEvent( "preview_expanded_changed", RoutingStrategy.Tunnel, typeof( RoutedEventArgs ), typeof(expandable_items_control) );
		public static readonly	RoutedEvent				expanded_changedEvent			= EventManager.RegisterRoutedEvent( "expanded_changed", RoutingStrategy.Bubble, typeof( RoutedEventArgs ), typeof(expandable_items_control) );
		
		public					Boolean					is_expanded					
		{
			get
			{
				return (Boolean)GetValue( is_expandedProperty );
			}
			set
			{
				SetValue( is_expandedProperty, value );
			}
		}
		public					Visibility				expand_visibility			
		{
			get 
			{
				return (Visibility)GetValue( expand_visibilityProperty );
			}
			set 
			{
				SetValue( expand_visibilityProperty, value );
			}
		}
		public					Double					inner_item_indnt			
		{
			get;set;
		}

		public					Panel					left_pocket					
		{
			get
			{
				return (Panel)GetValue( left_pocketProperty );
			}
			set
			{
				SetValue( left_pocketProperty, value );
			}
		}
		public					Panel					right_pocket				
		{
			get
			{
				return (Panel)GetValue( right_pocketProperty );
			}
			set
			{
				SetValue( right_pocketProperty, value );
			}
		}

		private static			void					is_expanded_changed			( DependencyObject d, DependencyPropertyChangedEventArgs e )	
		{
			var ctrl			= ((expandable_items_control)d);
			ctrl.on_expanded_changed( );
		}
		protected virtual		void					on_expanded_changed			( )							
		{
			var args = new RoutedEventArgs( priview_expanded_changedEvent );
			RaiseEvent( args );

			if( !args.Handled )
				RaiseEvent( new RoutedEventArgs( expanded_changedEvent ) );
		}
		
		public					void					add_right_pocket_inner		( UIElement element )		
		{
			m_right_pocket.Children.Insert( 0, element );
		}
		public					void					add_right_pocket_outer		( UIElement element )		
		{
			m_right_pocket.Children.Add( element );
		}
		public					void					add_left_pocket_inner		( UIElement element )		
		{
			m_left_pocket.Children.Add( element );
		}
		public					void					add_left_pocket_outer		( UIElement element )		
		{
			m_left_pocket.Children.Insert( 0, element );
		}
	}
}
