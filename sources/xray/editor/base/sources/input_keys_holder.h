////////////////////////////////////////////////////////////////////////////
//	Created		: 21.05.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef INPUT_KEYS_HOLDER_H_INCLUDED
#define INPUT_KEYS_HOLDER_H_INCLUDED

using namespace System;
using namespace System::Collections;
using namespace System::Windows::Forms;

namespace xray{
namespace editor_base{

public delegate void	keys_combination_changed	( String^ status );

public interface class input_handler
{
public:
	bool			on_key_down						( KeyEventArgs^ k );
	bool			on_key_up						( KeyEventArgs^ k );
};

value struct input_event
{
	KeyEventArgs^	m_key;
	bool			m_key_down;
};

public ref class input_keys_holder : IMessageFilter
{
private:
					input_keys_holder				( );
					~input_keys_holder				( );

public:
	static	void	create							( );
	ref class		keys_iterator;

public:
	event			keys_combination_changed^		m_changed_delegate;

private:
	System::Collections::Generic::List<input_handler^>	m_external_input_handlers;

	System::Collections::Generic::List<Keys>			m_held_keys;
	System::Collections::Generic::Queue<input_event>	m_input_events;
	KeysConverter										m_key_converter;
	static input_keys_holder^							m_ref;
	Form^												m_last_active_form;
	keys_iterator^										m_iterator;
	String^												m_last_combination_string;

	Boolean						m_l_button_last_down;
	Boolean						m_r_button_last_down;
	Boolean						m_m_button_last_down;
	Boolean						m_x1_button_last_down;
	Boolean						m_x2_button_last_down;

public:
	static property	input_keys_holder^				ref
	{
		input_keys_holder^		get( ){ return m_ref; }
	}
	property keys_iterator^							key_combinations
	{
		keys_iterator^			get(){ return m_iterator; }
	}

private:
	void			register_down_key_event			( Keys key );
	void			register_up_key_event			( Keys key );
	void			register_mouse_down_event		( Keys key );
	void			register_mouse_up_event			( Keys key );

	bool			peek_registered_key				( );
	String^			combine_string					( Keys modifiers, ArrayList^ keys );
	void			combination_changed				( );

public:
	void			subscribe_on_changed			( keys_combination_changed^ d );
	void			subscribe_input_handler			( input_handler^ h, bool bsubscribe );
	void			reset							( );
	void			reset							( Object^, EventArgs^ );
	virtual bool	PreFilterMessage				( Message% msg );
	System::Collections::Generic::List<Keys>^	get_held_keys					( );
	String^			get_current_pressed_keys		( );
	Int32			get_current_pressed_keys		( String^& str );
	bool			has_unprocessed_keys			( );

private:
	int				get_current_keys_string			( String^& combination );


private:
	enum class input_events
	{
		key_down			= 0x100,
		sys_key_down		= 0x0104,
		key_up				= 0x101,
		sys_key_up			= 0x0105,
		mouse_move 			= 0x0200,
		left_mouse_down		= 0x201,
		left_mouse_up		= 0x202,
		left_mouse_dbl		= 0x203,
		middle_mouse_down	= 0x0207,
		middle_mouse_up		= 0x208,
		middle_mouse_dbl	= 0x209,
		right_mouse_down 	= 0x0204,
		right_mouse_up 		= 0x0205,
		right_mouse_dbl 	= 0x0206,
		x_mouse_down 		= 0x020b,
		x_mouse_up 			= 0x020c,
		x_mouse_dbl 		= 0x020d
	}; // enum class input_events

private:
	typedef System::Collections::IEnumerable IEnumerable;
	typedef System::Collections::IEnumerator IEnumerator;

public:
	ref class keys_iterator: IEnumerable, IEnumerator
	{
	internal:
		keys_iterator( ){}

	private:
		String^			m_current_string;

	public:
		virtual property Object^ Current 
		{
			Object^		get		( );
		}
	
		virtual bool			MoveNext		( );
		virtual void			Reset			( );
		virtual IEnumerator^	GetEnumerator	( ){ return this; }

	};

}; // class input_keys_holder

} // namespace editor_base
} // namespace xray

#endif // #ifndef INPUT_KEYS_HOLDER_H_INCLUDED