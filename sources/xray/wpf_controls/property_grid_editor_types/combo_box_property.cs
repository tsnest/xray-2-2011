using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace xray.editor.wpf_controls.property_grid_editor_types
{
	/// <summary>
	/// Describes type that displays as combo box
	/// </summary>
	public class combo_box_property
	{

		#region | Initialize |


		/// <summary>
		/// Creates an instance of ComboBoxProperty type width defoult parameters
		/// </summary>
		public combo_box_property()
		{
			Value = 0;
			StringValue = "";
		}

		/// <summary>
		/// Creates an instance of ComboBoxProperty type and sets it
		/// </summary>
		/// <param name="value"> numeric value of instance </param>
		/// <param name="stringValue"> string value of instane </param>
		public combo_box_property(Int32 value, String stringValue)
		{
			Value = value;
			StringValue = stringValue;
		}


		#endregion

		#region | properties |


		/// <summary>
		/// Gest or Sets numeric representation of this instance
		/// </summary>
		public Int32 Value { get; set; }

		/// <summary>
		/// Gets or Sets string representation of this instance
		/// </summary>
		public String StringValue { get; set; }


		#endregion

		#region |  Methods   |


		/// <summary>
		/// Returns a string representation of this instance
		/// </summary>
		/// <returns></returns>
		public override string ToString()
		{
			return StringValue;
		}

		/// <summary>
		/// Determines whether the specified ComboBoxProperty is equal to the current ComboBoxProperty.
		/// </summary>
		/// <param name="obj"> Other ComboBoxProperty of check </param>
		/// <returns> Returns true if ComboBoxPropertyes is equal? otherwise false </returns>
		public override bool Equals(object obj)
		{
			return obj is combo_box_property && Value.Equals(((combo_box_property)obj).Value);
		}

		/// <summary>
		/// Compute hash for this ComboBoxProperty
		/// </summary>
		/// <returns></returns>
		public override int GetHashCode()
		{
			return Value.GetHashCode();
		}


		#endregion

	}
}
