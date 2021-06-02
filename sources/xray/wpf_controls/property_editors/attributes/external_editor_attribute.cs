////////////////////////////////////////////////////////////////////////////
//	Created		: 21.10.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls.property_editors.attributes
{
	/// <summary>
	/// Describes that this property will be editied external editor
	/// </summary>
	[AttributeUsage(AttributeTargets.Property)]
	public class external_editor_attribute : Attribute
	{
		/// <summary>
		/// Constructs an instance of ExternalEditorAttribute
		/// </summary>
		public external_editor_attribute(external_editor_event_handler external_editor_delegate):this(external_editor_delegate, true){}
		/// <summary>
		/// Constructs an instance of ExternalEditorAttribute
		/// </summary>
		/// <param name="external_editor_delegete"> delegate that invokes when user click button </param>
		/// <param name="is_clear_visible"> is clear button visible </param>
		public external_editor_attribute(external_editor_event_handler external_editor_delegate, Boolean is_clear_visible)
		{
			m_is_clear_visible = is_clear_visible;
			if (external_editor_delegate == null)
				throw new NullReferenceException("externalEditorDelegete parameter can not be null");
			this.external_editor_delegate = external_editor_delegate;
			m_filter = null;
		}
		public external_editor_attribute(external_editor_event_handler external_editor_delegate, Boolean is_clear_visible, Object filter)
		{
			m_is_clear_visible = is_clear_visible;
			if (external_editor_delegate == null)
				throw new NullReferenceException("externalEditorDelegete parameter can not be null");
			this.external_editor_delegate = external_editor_delegate;
			m_filter = filter;
		}
		/// <summary>
		/// Constructs an instance of ExternalEditorAttribute
		/// </summary>
		/// <param name="editor_type"> typeof editor that implement i_external_property_editor </param>
		public external_editor_attribute( Type editor_type )
		{
			m_is_clear_visible = false;
			if ( editor_type == null || !typeof(i_external_property_editor).IsAssignableFrom( editor_type ))
				throw new ArgumentException(" Editor Type not valid. Editor Type must implement i_external_property_editor. ");
			external_editor_delegate	= null;
			m_external_editor_type		= editor_type;
		}
		public external_editor_attribute( Type editor_type, String additional_data )
		{
			m_is_clear_visible = false;
			if ( editor_type == null || !typeof(i_external_property_editor).IsAssignableFrom( editor_type ))
				throw new ArgumentException(" Editor Type not valid. Editor Type must implement i_external_property_editor. ");
			external_editor_delegate	= null;
			m_external_editor_type		= editor_type;
			m_additional_data			= additional_data;
		}
		public external_editor_attribute( Type editor_type, Boolean can_directly_set )
		{
			m_can_directly_set = can_directly_set;
			m_is_clear_visible = false;
			if ( editor_type == null || !typeof(i_external_property_editor).IsAssignableFrom( editor_type ))
				throw new ArgumentException(" Editor Type not valid. Editor Type must implement i_external_property_editor. ");
			external_editor_delegate	= null;
			m_external_editor_type		= editor_type;
		}

		/// <summary>
		/// Type of External Editor that will be modify specified proprty
		/// </summary>
		public readonly external_editor_event_handler	external_editor_delegate;
		public readonly Type							m_external_editor_type;
		public readonly Boolean							m_is_clear_visible;
		public readonly	Object							m_filter;
		public readonly	Boolean							m_can_directly_set;
		public readonly	String							m_additional_data;

	}

	/// <summary>
	/// Represents a Delegate that will be raised when external editor edit button down
	/// </summary>
	/// <param name="prop"> object that wraps source property </param>
	/// <param name="filter"> filter tha can be used for external editor </param>
	public delegate void external_editor_event_handler(property prop, Object filter);
}
