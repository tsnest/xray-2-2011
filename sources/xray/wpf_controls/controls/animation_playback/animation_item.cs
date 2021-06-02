////////////////////////////////////////////////////////////////////////////
//	Created		: 02.11.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.ComponentModel;
using System.Collections.Generic;

namespace xray.editor.wpf_controls.animation_playback
{
	public class animation_item: INotifyPropertyChanged
	{

		public event PropertyChangedEventHandler PropertyChanged;

        #region | Initilaize |
        
        
        public animation_item   () 
        {
            height			= 50;
			m_is_leading	= false;
			m_events		= new List<animation_event>();
			m_weights_by_time = new Dictionary<UInt32, Single>();
			m_scales_by_time = new Dictionary<UInt32, Single>();
            bindings		= new item_bindings( this );
        }
        
        
        #endregion
        
        #region |   Fields   |


        private String	m_name;
        private Single	m_position;
        private Single	m_offset;
        private Single	m_length;
        private Single	m_weight;
        private Single	m_time_scale;
		private Single	m_time_scaled_length;
		private Int32	m_sync_group;
		private UInt32	m_user_data;
		private Boolean	m_is_leading;
		private List<animation_event> m_events;
		private Dictionary<UInt32, Single> m_weights_by_time;
		private Dictionary<UInt32, Single> m_scales_by_time;
		internal animation_playback_panel m_panel;


        #endregion

        #region | Properties |


        public String	name          
        {
            get
            {
                return m_name;
            }
            set
            {
                m_name = value;
                on_property_changed("name");
            }
        }
        public Single	height        
        {
            get;
            set;
        }
        public Single	position      
        {
            get
            {
                return m_position * m_panel.time_layout_scale;
            }
            set
            {
                m_position = value;
                on_property_changed("position");
            }
        }
        public Single	offset        
        {
            get
            {
				return m_offset * m_panel.time_layout_scale;
            }
            set
            {
                m_offset = value;
                on_property_changed("offset");
            }
        }
        public Single	length        
        {
            get
            {
				return m_length * m_panel.time_layout_scale;
            }
            set
            {
                m_length = value;
                on_property_changed("length");
            }
        }
        public Single	weight        
        {
            get
            {
                return m_weight;
            }
            set
            {
                m_weight = value;
                on_property_changed("weight");
            }
        }
        public Single	time_scale    
        {
            get
            {
                return m_time_scale;
            }
            set
            {
                m_time_scale = value;
                on_property_changed("time_scale");
            }
        }
		public Single	time_scaled_length
		{
			get
			{
				return m_time_scaled_length;
			}
			set
			{
				m_time_scaled_length = value;
				on_property_changed("time_scaled_length");
			}
		}
		public Int32	sync_group    
        {
            get
            {
                return m_sync_group;
            }
            set
            {
                m_sync_group = value;
                on_property_changed("sync_group");
            }
        }
		public String	font_weight	
		{
			get
			{
				if(m_is_leading)
					return "Bold";
				else
					return "Normal";
			}
		}

		public UInt32 user_data    
        {
            get
            {
                return m_user_data;
            }
            set
            {
                m_user_data = value;
                on_property_changed("user_data");
            }
        }
		public Boolean is_leading    
        {
            get
            {
                return m_is_leading;
            }
            set
            {
                m_is_leading = value;
                on_property_changed("is_leading");
                on_property_changed("font_weight");
            }
        }
		public item_bindings bindings   
        {
            get;
            set;
        }

        public List<animation_event> events
        {
            get
            {
                return m_events;
            }
            set
            {
                m_events = value;
                on_property_changed("events");
            }
        }
        
        public Dictionary<UInt32, Single> weights_by_time
        {
            get
            {
                return m_weights_by_time;
            }
            set
            {
                m_weights_by_time = value;
                on_property_changed("weights_by_time");
            }
        }

        public Dictionary<UInt32, Single> scales_by_time
        {
            get
            {
                return m_scales_by_time;
            }
            set
            {
                m_scales_by_time = value;
                on_property_changed("scales_by_time");
            }
        }

		internal	animation_item_view		view
		{
			get;set;
		}


		#endregion

        #region |   Methods  |


        protected void on_property_changed(String property_name)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(property_name));
        }

		internal void update()
		{
			on_property_changed("m_position");
			on_property_changed("m_offset");
			on_property_changed("m_length");
			on_property_changed("time_scaled_length");
			foreach (var ev in m_events)
				ev.update();

			bindings.update_bindings( );
			if(view!=null)
				view.fill_curves( );
		}

        
        #endregion

        #region | InnerTypes |


        public class item_bindings: INotifyPropertyChanged
        {

			public event PropertyChangedEventHandler PropertyChanged;

			#region | Initialize |


            public item_bindings( animation_item item )
            {
                m_item = item;
            }


			#endregion

			#region |   Fields   |


            private		animation_item				m_item;


			#endregion

            #region | Properties |

            
            public Single top_offset            
            {
                get
                {
                    return m_item.height - m_item.height * m_item.weight;
                }
                set
                {
                }
            }
            public Single time_scaled_length    
            {
                get
                {
					return m_item.m_time_scaled_length * m_item.m_panel.time_layout_scale;//m_item.time_scale * m_item.length;
                }
                set
                {
                }
            }
            public Single weighted_height       
            {
                get
                {
                    return m_item.height * m_item.weight;
                }
                set
                {
                }
            }
			public Single animation_time
			{
				get
				{
					return m_item.m_panel.animation_time * m_item.m_panel.time_layout_scale;
				}
				set
				{
				}
			}
			public Single animation_time_width	
			{
				get
				{
					//return Math.Min(m_item.m_panel.animation_time*m_item.m_panel.time_layout_scale - ( m_item.position + m_item.offset ), time_scaled_length - m_item.offset );
					return m_item.m_panel.animation_time * m_item.m_panel.time_layout_scale - (m_item.position + m_item.offset);
				}
				set
				{
				}
			}
			public Single offset_position
			{
				get
				{
					return m_item.position + m_item.offset;
				}
				set
				{
				}
			}


            #endregion

			#region |   Methods  |


			private void on_property_changed( String property_name )
			{
				if( PropertyChanged != null )
					PropertyChanged( this, new PropertyChangedEventArgs( property_name ) );
			}

			internal void update_bindings()
			{
				on_property_changed( "top_offset" );
				on_property_changed( "time_scaled_length" );
				on_property_changed( "weighted_height" );
				on_property_changed( "animation_time" );
				on_property_changed( "animation_time_width" );
				on_property_changed( "offset_position" );
			}


			#endregion
			
		}


        #endregion

	}
}
