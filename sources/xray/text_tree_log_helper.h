////////////////////////////////////////////////////////////////////////////
//	Created		: 17.08.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef TEXT_TREE_LOG_HELPER_H_INCLUDED
#define TEXT_TREE_LOG_HELPER_H_INCLUDED

#include <xray/text_tree_item.h>

namespace xray {
namespace strings {
	class text_tree_log_helper
	{
		typedef xray::debug::vector<u32> columns;
	public:
		void output(text_tree_item* itm, u32 indent=4, char separator=':')
		{
			columns cols;
			prepare(itm, cols, indent, 0, separator);
			log(itm, cols, indent, 0, separator);
		}

	private:
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
				*c = std::max(strings::length(name) + cur_indent, *c);
				++c;
				for(text_tree_column_item* column=column_items.front(); column; ++c, column=column_items.get_next_of_object(column))
					*c = std::max(strings::length(column->value), *c);
			}
		}

		void log(text_tree_item* itm, columns& cols, u32 indent, u32 cur_indent, char separator)
		{
			pcstr name = itm->get_name();
			if(name)
			{
				fixed_string4096 buffer;
				for(u32 j=0; j<cur_indent; ++j)
					buffer += " ";

				buffer += name;
				columns::iterator c = cols.begin();
				u32 num_padding_spaces = *c - (cur_indent + strings::length(name));
				++c;

				column_items_type const& column_items = itm->get_columns();
				if(!column_items.empty())
				{
					for(float j=0; j<num_padding_spaces; ++j)
						buffer += " ";

					for(text_tree_column_item* column=column_items.front(); column; ++c, column=column_items.get_next_of_object(column))
					{
						buffer += " ";
						buffer += separator;
						buffer += " ";
						buffer += column->value;
						num_padding_spaces = *c - strings::length(column->value);
						for(float j=0; j<num_padding_spaces; ++j)
							buffer += " ";
					}
				}

				LOG_INFO(buffer.c_str());
			}

			sub_items_type const& sub_items = itm->get_sub_items();
			for(text_tree_item* child=sub_items.front(); child; child=sub_items.get_next_of_object(child))
			{
				if(child->get_visibility())
				{
					u32 new_indent = cur_indent + (name ? indent : 0);
					log(child, cols, indent, new_indent, separator);
				}
			}
		}

	}; // class text_tree_log_helper
} // namespace strings
} // namespace xray
#endif // #ifndef TEXT_TREE_LOG_HELPER_H_INCLUDED