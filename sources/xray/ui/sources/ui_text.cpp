#include "pch.h"
#include "ui_font.h"

namespace xray {
namespace ui {


#define IsSpace(ch)       ((ch) == ' ' || (ch) == '\t' || (ch) == '\r' || (ch) == '\n' || (ch) == ',' || (ch) == '.' || (ch) == ':' || (ch) == '!')
void parse_word(pcstr str, ui_font const* font, float& length, pcstr& next_word)
{
	length = 0.0f;
	while(*str)
	{
		length	+= font->get_char_tc(*str).z;
		if(IsSpace(*str))
			break;

		++str;
	}
	next_word = str+1;
}

} // namespace xray
} // namespace ui
