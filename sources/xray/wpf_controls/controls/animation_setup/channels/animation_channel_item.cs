using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;

namespace xray.editor.wpf_controls.animation_setup
{
	public class animation_channel_item : INotifyPropertyChanged
	{
		public event PropertyChangedEventHandler PropertyChanged;

		public					animation_channel_item	(animation_channel ch)	
		{
			channel = ch;
			id = Guid.NewGuid();
		}
		protected		void	on_property_changed		(String property_name)	
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(property_name));
        }
		public virtual	void	update					()						
		{
		}
		public virtual	void	snap_to_frames			()						
		{
		}
		public virtual	void	change_property			(String property_name, Object value)
		{
		}
		public virtual	Object	get_property			(String property_name)	
		{
			return null;
		}

		public			Guid							id;
		public			animation_channel				channel;
	}
}
