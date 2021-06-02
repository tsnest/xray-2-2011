////////////////////////////////////////////////////////////////////////////
//	Created		: 24.03.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "main_menu.h"
#include "game.h"

#include <xray/ui/world.h>
#include <xray/input/keyboard.h>
#include <xray/input/world.h>

namespace stalker2 {

struct main_menu_ui :public boost::noncopyable
{
main_menu_ui(xray::ui::world& ui_world)
:m_ui_world(ui_world)
{
	m_ui_dialog						= m_ui_world.create_dialog( );
	m_ui_dialog->w()->set_position	( float2(100, 100) );
	m_ui_dialog->w()->set_size		( float2(500,500) );

	ui::image* img					= m_ui_world.create_image( );
	img->init_texture				( "ui_rect" );
	img->set_color					( 0xff409040 );
	img->w()->set_size				( m_ui_dialog->w()->get_size() );
	img->w()->set_position			( float2(0, 0) );
	img->w()->set_visible			( true );
	m_ui_dialog->w()->add_child		( img->w(), true );

	ui::text* text					= m_ui_world.create_text( );
	text->set_font					( ui::fnt_arial );
	text->set_text					( "Main menu" );
	text->w()->set_size				( float2(200, 40) );
	text->w()->set_position			( float2(20, 120) );
	text->w()->set_visible			( true );
	m_ui_dialog->w()->add_child		( text->w(), true );

	text							= m_ui_world.create_text( );
	text->set_font					( ui::fnt_arial );
	text->set_text					( "ESC - switch to Game" );
	text->w()->set_size				( float2(200, 40) );
	text->w()->set_position			( float2(20, 140) );
	text->w()->set_visible			( true );
	m_ui_dialog->w()->add_child		( text->w(), true );

	text							= m_ui_world.create_text( );
	text->set_font					( ui::fnt_arial );
	text->set_text					( "Q - quit" );
	text->w()->set_size				( float2(200, 40) );
	text->w()->set_position			( float2(20, 160) );
	text->w()->set_visible			( true );
	m_ui_dialog->w()->add_child		( text->w(), true );

	text							= m_ui_world.create_text( );
	text->set_font					( ui::fnt_arial );
	text->set_text					( "H/J- attach/detach hud camera" );
	text->w()->set_size				( float2(200, 40) );
	text->w()->set_position			( float2(20, 180) );
	text->w()->set_visible			( true );
	m_ui_dialog->w()->add_child		( text->w(), true );
}
	~main_menu_ui()
	{
		m_ui_world.destroy_window(m_ui_dialog->w());
	}
	xray::ui::world&		m_ui_world;
	ui::dialog*				m_ui_dialog;
};

main_menu::main_menu( game& game ) 
:m_game	( game )
{
}

main_menu::~main_menu( )
{
}

input::handler*	main_menu::dialog_input_handler	()
{
	return m_ui->m_ui_dialog->input_handler();
}

void main_menu::on_activate( )
{
	super::on_activate					( );
	m_game.input_world().add_handler	( *this );

	m_ui	= NEW(main_menu_ui)(m_game.ui_world());
}

void main_menu::on_deactivate( )
{
	super::on_deactivate				( );
	m_game.input_world().remove_handler	( *this );
	DELETE	(m_ui);
}

void main_menu::tick( )
{
	m_ui->m_ui_dialog->w()->tick( );
	m_ui->m_ui_dialog->w()->draw( m_game.ui_world().get_renderer(), m_game.ui_world().get_scene_view() );
}

} // namespace stalker2