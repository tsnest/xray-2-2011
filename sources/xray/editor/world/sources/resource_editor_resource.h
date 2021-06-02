////////////////////////////////////////////////////////////////////////////
//	Created		: 22.04.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCE_EDITOR_OPTIONS_WRAPPER_H_INCLUDED
#define RESOURCE_EDITOR_OPTIONS_WRAPPER_H_INCLUDED

typedef xray::editor::wpf_controls::property_container wpf_property_container;

namespace xray {
namespace editor {

class editor_world;
ref class resource_editor_resource;

public delegate void resource_load_delegate( resource_editor_resource^, bool );
public delegate void resource_changed_delegate( resource_editor_resource^ );

public ref class resource_editor_resource abstract
{
public:
			resource_editor_resource( )	
				:m_is_deleted			( false ), 
				m_is_new				( false ), 
				m_is_renamed			( false ),
				m_is_broken				( false )
				{
					loaded				= false;
				}

protected:
	resource_load_delegate^			m_loaded_delegate;
	resource_changed_delegate^		m_changed_delegate;

public:
	virtual void					load				( System::String^ name )		= 0;
	virtual void					save				( )								= 0;
	virtual void					apply_changes		( )								= 0;
	virtual void					preview_changes		( )								= 0;
	virtual void					reset_to_default	( )								= 0;
	virtual void					init_new			( )								= 0;
	virtual void					copy_from			( resource_editor_resource^ other ) = 0;

	virtual System::String^			resource_name		( System::String^ short_name )	= 0;
	virtual System::String^			source_path			( System::String^ short_name, System::String^ resources_path )	= 0;;

	void						subscribe_on_loaded		( resource_load_delegate^ d )		{ m_loaded_delegate+=d; }
	void						subscribe_on_changed	( resource_changed_delegate^ d )	{ m_changed_delegate+=d; }
	void						properties_changed		( )									{ if(m_changed_delegate) m_changed_delegate(this); }

	bool							m_is_deleted;
	bool							m_is_renamed;
	bool							m_is_new;
	bool							m_is_broken;

	System::String^					m_name;
	System::String^					m_old_name;

	virtual wpf_property_container^	get_property_container	( bool chooser_mode )							= 0;
	
	
	property bool					loaded;
}; // class resource_editor_resource

typedef System::Collections::Generic::List<System::String^>										names_list;
typedef System::Collections::Generic::Dictionary<System::String^, resource_editor_resource^>	resources_dict;
typedef System::Collections::Generic::List<resource_editor_resource^>							resources_list;

} // namespace editor
} // namespace xray

#endif // #ifndef RESOURCE_EDITOR_OPTIONS_WRAPPER_H_INCLUDED