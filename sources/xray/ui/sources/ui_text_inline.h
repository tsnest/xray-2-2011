
template < class T >
ui_text<T>::ui_text(ui_world& w )
:m_ui_world		( w ),
ui_window		( w.allocator() ),
m_font			( NULL ),
m_color			( 0xffffffff )
//m_text_length	( 0 )
{
	this->subscribe_event	(ev_text_changed, ui_event_handler(this, &ui_text::on_text_changed));
}

template < class T >
ui_text<T>::~ui_text()
{}

template < class T >
void ui_text<T>::set_font(enum_font fnt)
{
	XRAY_UNREFERENCED_PARAMETER	(fnt);
	m_font = m_ui_world.get_font_manager().get_font();
}

template < class T >
void ui_text<T>::set_color(u32 clr)
{
	m_color = clr;
}

template < class T >
void ui_text<T>::set_text(pcstr text)
{
	if( NULL==m_text.c_str() || 0!=strings::compare(m_text.c_str(), text) )
	{
		m_text.set		( text );
		process_event	( ev_text_changed, 0, 0 );
	}
}

template < class T >
bool ui_text<T>::on_text_changed(window* w, int p1, int p2)
{
	XRAY_UNREFERENCED_PARAMETERS	(w, p1, p2);

//	m_text_length	= (u16)strings::length(get_text());
	return false;//non-exclusive handler
}

template < class T >
void ui_text<T>::set_align(enum_text_align al)
{
	m_align			= al;
}

template < class T >
void ui_text<T>::set_text_mode(enum_text_mode tm)
{
	m_mode			= tm;
}

template < class T >
void ui_text<T>::fit_height_to_text()
{
	if(m_mode==tm_multiline)
	{
		pcstr ch				= get_text();
		u32 symb_count			= m_text.length();
		float const height		= m_font->get_height();
		float curr_word_len		= 0.0f;
		pcstr next_word			= NULL;
		float2 new_size			(0.0f, height);

		parse_word				(ch, m_font, curr_word_len, next_word);
		for(u32 i=0; i<symb_count; ++i, ++ch)
		{
			float3 const& tc	= m_font->get_char_tc(*ch);
			if(ch==next_word)
			{
				parse_word		(ch, m_font, curr_word_len, next_word);
				if(new_size.x+curr_word_len > get_size().x)
				{
					new_size.x	= 0.0f;
					new_size.y	+= height;
				}
			}
			new_size.x			+= tc.z;
		}
		w()->set_size(float2(get_size().x, new_size.y));
	}
}

template < class T >
void ui_text<T>::split_and_set_text(pcstr str, float const width, pcstr& ret_str)
{
	pcstr tmp_str			= str;
	float curr_word_len		= 0.0f;
	string1024				text;
	pcstr next_word			= NULL;
	float size				= 0.0f;

	parse_word				(str, m_font, curr_word_len, next_word);
	while(size+curr_word_len < width && strings::length(tmp_str))
	{
		if(tmp_str==next_word)
		{
			size			+= curr_word_len;
			parse_word		(tmp_str, m_font, curr_word_len, next_word);
		}
		else
			++tmp_str;
	}
	strings::copy_n			(text, sizeof(text), str, strings::length(str)-strings::length(tmp_str));
	set_text				( text );
	(strings::length(next_word)>0) ? ret_str = tmp_str : ret_str = NULL;
}

template < class T >
void ui_text<T>::draw( xray::render::ui::renderer& render, xray::render::scene_view_ptr const& scene_view )
{
	draw_internal( render, scene_view, 0, 0, 0xffff0000 );
}

template < class T >
float2 ui_text<T>::measure_string() const
{
	float2			result;
	result.y		= get_size().y;
	result.x		= calc_string_length(*m_font, get_text());
	return			result;
}

template < class T >
void ui_text<T>::draw_internal(
		xray::render::ui::renderer& render, 
		xray::render::scene_view_ptr const& scene_view, 
		u32 const idx1, 
		u32 const idx2, 
		u32 const sel_clr
	)
{
	ui_window::draw			(render, scene_view);
	
	if(0==m_text.length())
		return;

	ASSERT					(m_font, "font not assigned");

	float2  pos				= get_position();
	float2 pos_rt			(0.0f, 0.0f);
	client_to_screen		(*this, pos);		//need absolute position
	
	render.draw_text		(
		scene_view,
		get_text(), 
		*m_font, 
		pos, 
		math::color(m_color), 
		math::color(sel_clr), 
		math::floor( get_size().x ),
		m_mode==tm_multiline, 
		idx1, 
		idx2
	);
	
	return;
/*


	u32 symb_count			= m_text.length();
	u32 max_verts			= symb_count*4; // use TriangleList+ IndexBuffer(0-1-2, 3-2-1)

	pcstr ch				= get_text();
	float const height		= m_font->get_height();
	float height_ts			= m_font->get_height_ts();

	float curr_word_len		= 0.0f;
	pcstr next_word			= NULL;

	if(m_mode==tm_multiline)
		parse_word			(ch, m_font, curr_word_len, next_word);

	
	xray::render::ui::command* cmd = render.create_command(max_verts, prim_type, point_type);
	for(u32 i=0; i<symb_count; ++i, ++ch)
	{
		u32 clr = (i>=(u32)idx1 && i<(u32)idx2)?sel_clr:m_color;

		float3 uv			= m_font->get_char_tc_ts(*ch);
		float3 const& tc	= m_font->get_char_tc(*ch);

		if((m_mode==tm_multiline) && (ch==next_word))
		{
			parse_word		(ch, m_font, curr_word_len, next_word);
			if(pos_rt.x+curr_word_len >get_size().x)
			{
				pos_rt.x		= 0.0f;
				pos_rt.y		+= height;
			}
		}

		cmd->push_point		(pos.x+pos_rt.x,		pos.y+pos_rt.y+height,	0.0f, clr, uv.x,		uv.y+height_ts);
		cmd->push_point		(pos.x+pos_rt.x,		pos.y+pos_rt.y,			0.0f, clr, uv.x,		uv.y);
		cmd->push_point		(pos.x+pos_rt.x+tc.z,	pos.y+pos_rt.y+height,	0.0f, clr, uv.x+uv.z,	uv.y+height_ts);
		cmd->push_point		(pos.x+pos_rt.x+tc.z,	pos.y+pos_rt.y,			0.0f, clr, uv.x+uv.z,	uv.y);

		pos_rt.x			+= tc.z;
	}
//	cmd->set_shader();
//	cmd->set_geom();
//	w.ui().destroy_command(cmd);
	render.push_command		(cmd);
	*/
}
