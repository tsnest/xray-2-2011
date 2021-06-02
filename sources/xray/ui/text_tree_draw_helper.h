////////////////////////////////////////////////////////////////////////////
//	Created		: 17.08.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef TEXT_TREE_DRAW_HELPER_H_INCLUDED
#define TEXT_TREE_DRAW_HELPER_H_INCLUDED

#include <xray/ui/world.h>
#include <xray/text_tree_item.h>

using namespace xray::strings;

namespace xray {
namespace ui {

	struct world;
	struct text;
	struct window;
	enum enum_font;
	struct text_tree_draw_helper_params
	{
		u32			color1;
		u32			color2;
		enum_font	fnt;
		bool		is_multipaged;
		float		row_height;
		float		space_between_pages;
		float2		start_pos;
	};

	class text_tree_draw_helper: private boost::noncopyable
	{
		typedef vectora<float> columns;

	public:
		text_tree_draw_helper(world& w, window* wnd, text_tree_draw_helper_params p, memory::base_allocator& a)
		:m_world(w),
		m_window(wnd),
		m_params(p),
		m_allocator(a)
		{
			m_font = m_world.default_font(); // replace this when font by enum will be implemented
			m_space_width = str_length(m_font, " ");
			m_page_width = 0.0f;
		};

		void output(text_tree_item* itm, u32 indent=4, char separator=':')
		{
			m_cur_row = 0;
			columns cols(m_allocator);
			prepare(itm, cols, indent, 0, separator);
			show(itm, cols, indent, 0, separator);
		}

	private:
		f32 str_length(ui::font const* f, pcstr str)
		{
			pcstr p = str;
			f32	result = 0.0f;
			while (*p)
			{
				result += f->get_char_tc(*p).z;
				++p;
			}

			return result;
		}

		void prepare(text_tree_item* itm, columns& cols, u32 indent, u32 cur_indent, char separator)
		{
			pcstr name = itm->get_name();
			sub_items_type const& sub_items = itm->get_sub_items();
			for(text_tree_item* child=sub_items.front(); child; child=sub_items.get_next_of_object(child))
			{
				if(child->get_visibility())
				{
					u32 new_indent = cur_indent + (name ? indent : 0);
					prepare(child, cols, indent, new_indent, separator);
				}
			}

			column_items_type const& column_items = itm->get_columns();
			u32 column_items_size = column_items.back() - column_items.front() + 1;
			if(cols.size() < column_items_size)
				cols.resize(column_items_size);

			// only count as column if theres more then 1 on the line!
			if(name && !column_items.empty())
			{
				columns::iterator c = cols.begin();
				float string_size = str_length(m_font, name) + cur_indent*m_space_width;
				*c = std::max(string_size, *c);
				++c;
				for(text_tree_column_item* column=column_items.front(); column; ++c, column=column_items.get_next_of_object(column))
					*c = std::max(str_length(m_font, column->value), *c);
			}
		}

		void create_window(pcstr text, float2 pos, float2 sz)
		{
			ui::text* wnd = m_world.create_text();
			wnd->set_color((m_cur_row%2) ? m_params.color1 : m_params.color2);
			wnd->set_font(m_params.fnt);
			wnd->set_align(al_left);
			wnd->set_text(text);
			wnd->w()->set_position(pos);
			wnd->w()->set_size(sz);
			wnd->w()->set_visible(true);
			m_window->add_child(wnd->w(), true);
		}

		void show(text_tree_item* itm, columns& cols, u32 indent, u32 cur_indent, char separator)
		{
			float offset = cur_indent * m_space_width;
			pcstr name = itm->get_name();
			if(name)
			{
				++m_cur_row;
				float2 pos = float2(m_params.start_pos.x + offset, m_params.start_pos.y);
				float string_size = str_length(m_font, name);
				float2 sz = float2(string_size, m_params.row_height);
				m_page_width = std::max(string_size+offset, m_page_width);
				create_window(name, pos, sz);
				columns::iterator c = cols.begin();
				column_items_type const& column_items = itm->get_columns();
				if(!column_items.empty())
				{
					offset = *c;
					++c;
					for(text_tree_column_item* column=column_items.front(); column; ++c, column=column_items.get_next_of_object(column))
					{
						fixed_string<8> buffer;
						buffer += " ";
						buffer += separator;
						buffer += " ";
						float2 pos = float2(m_params.start_pos.x + offset, m_params.start_pos.y);
						float string_size = str_length(m_font, buffer.c_str());
						float2 sz = float2(string_size, m_params.row_height);
						create_window(buffer.c_str(), pos, sz);
						offset += string_size;

						pos = float2(m_params.start_pos.x + offset, m_params.start_pos.y);
						string_size = str_length(m_font, column->value);
						sz = float2(string_size, m_params.row_height);
						create_window(column->value, pos, sz);
						offset += *c;
					}
	
					m_page_width = std::max(offset, m_page_width);
				}
			}

			m_params.start_pos.y += m_params.row_height;
			if(m_params.is_multipaged)
			{
				if(m_params.start_pos.y>=m_window->get_size().y || itm->is_page_breaker())
				{
					m_params.start_pos.x += m_page_width + m_params.space_between_pages;
					m_params.start_pos.y = 0;
					m_page_width = 0;
				}
			}

			sub_items_type const& sub_items = itm->get_sub_items();
			for(text_tree_item* child=sub_items.front(); child; child=sub_items.get_next_of_object(child))
			{
				if(child->get_visibility())
				{
					u32 new_indent = cur_indent + (name ? indent : 0);
					show(child, cols, indent, new_indent, separator);
				}
			}
		}

		world&							m_world;
		window*							m_window;
		text_tree_draw_helper_params	m_params;
		font const*						m_font;
		float							m_space_width;
		float							m_page_width;
		u32								m_cur_row;
		memory::base_allocator&			m_allocator;
	}; // class text_tree_draw_helper
} // namespace ui
} // namespace xray
#endif // #ifndef TEXT_TREE_DRAW_HELPER_H_INCLUDED