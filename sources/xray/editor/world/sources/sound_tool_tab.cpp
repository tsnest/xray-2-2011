#include "pch.h"
#include "sound_tool_tab.h"
#include "tool_base.h"
#include "level_editor.h"
#include "library_objects_tree.h"
#include "sound_debug_mode_strip_button.h"
#include <xray/sound/world.h>
#include <xray/sound/world_user.h>

namespace xray{
namespace editor{

void sound_tool_tab::post_constructor( )
{
	m_library_objects_tree							= gcnew library_objects_tree( m_tool );
	m_library_objects_tree->show_empty_folders		= true;
	m_library_objects_tree->sync_tool_selected_name = true;
	m_library_objects_tree->allow_folder_creation	= false;
	m_library_objects_tree->allow_items_reparent	= false;
	m_library_objects_tree->allow_items_rename		= false;
	m_library_objects_tree->allow_items_delete		= false;
	m_library_objects_tree->Dock					= System::Windows::Forms::DockStyle::Fill;
	objects_tree_place->Controls->Add				( m_library_objects_tree );

	m_library_objects_tree->m_double_clicked		+= gcnew double_clicked( this, &sound_tool_tab::item_double_clicked);

	ToolStrip^ strip				= m_tool_strip;
	editor_base::gui_binder^ binder	= m_tool->get_level_editor()->get_gui_binder();

	ToolStripButton^ button		= nullptr;
	System::String^ action_name	= nullptr;

	action_name					= gcnew System::String("debug stream writing");
	button						= gcnew ToolStripButton;
	strip->Items->Add			( button);
	binder->bind				( button, action_name );
	button->Image				= binder->get_image( action_name );
	button->DisplayStyle		= System::Windows::Forms::ToolStripItemDisplayStyle::Image;
	button->ImageScaling		= System::Windows::Forms::ToolStripItemImageScaling::SizeToFit;

	action_name					= gcnew System::String("dump debug stream writing");
	button						= gcnew ToolStripButton;
	strip->Items->Add			( button );
	binder->bind				( button, action_name );
	button->Image				= binder->get_image( action_name );
	button->DisplayStyle		= System::Windows::Forms::ToolStripItemDisplayStyle::Image;
	button->ImageScaling		= System::Windows::Forms::ToolStripItemImageScaling::SizeToFit;

	strip->Items->Add							( gcnew System::Windows::Forms::ToolStripLabel("debug:"));
	m_debug_options_button						= gcnew sound_debug_mode_strip_button( m_editor_world );
	m_debug_options_button->ToolTipText			= "select debug configuration";
	m_debug_options_button->can_iterate_modes	= false;
	System::Collections::Generic::List<System::String^>^ modes = gcnew ( System::Collections::Generic::List<System::String^> )();
	modes->Add									( "off" );
	modes->Add									( "base" );
	modes->Add									( "hdr" );
	m_debug_options_button->modes				= modes;
	m_debug_options_button->current_mode		= 0;
	strip->Items->Add							( m_debug_options_button );
}

void sound_tool_tab::fill_objects_list( System::String^ initial )
{
	m_library_objects_tree->fill_objects_list( initial );
}

void sound_tool_tab::on_activate( )
{
	m_tool->get_level_editor()->set_active_tool( m_tool );
}

void sound_tool_tab::on_deactivate( )
{
	m_tool->get_level_editor()->set_active_tool( nullptr );
}

void sound_tool_tab::item_double_clicked( lib_item_base^ item )
{
	if(dynamic_cast<lib_item^>(item))
		m_tool->item_double_clicked(item);
}

System::Void sound_tool_tab::time_factor_ValueChanged	(System::Object^, System::EventArgs^)
{
	m_editor_world.engine().get_sound_world().get_editor_world_user().set_time_scale_factor( float(time_factor->Value) );
}

} //namespace xray
} //namespace editor
