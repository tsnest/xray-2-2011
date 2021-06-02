////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef FIXED_STRING_INLINE_H_INCLUDED
#define FIXED_STRING_INLINE_H_INCLUDED

namespace xray {

//-----------------------------------------------------------------------------------
// initializations
//-----------------------------------------------------------------------------------

template <int Size>
fixed_string<Size>::fixed_string () : buffer_string(m_buffer, Size)
{
	if ( identity(Size) ) 
	{
		m_buffer[0]					=	NULL;
	}
}

template <int Size>
fixed_string<Size>::fixed_string (fixed_string const& src) : buffer_string(m_buffer, Size, src.begin(), src.end())
{
}

template <int Size>
fixed_string<Size>::fixed_string (buffer_string const& src) : buffer_string(m_buffer, Size, src.begin(), src.end())
{
}

template <int Size>
fixed_string<Size>::fixed_string (value_type const* src) : buffer_string(m_buffer, Size, src)
{
}

template <int Size>
fixed_string<Size>::fixed_string (value_type const src) : buffer_string(m_buffer, Size)
{
	char buff[]						=	{ src, 0 };
	append								((char*)buff, buff+1);
}

template <int Size>
fixed_string<Size>::fixed_string (value_type const* src, size_type const& count) : buffer_string(m_buffer, Size)
{
	strings::copy_n					(m_buffer, Size, src, (u32)count);
}

template <int Size>
template <typename input_iterator>
fixed_string<Size>::fixed_string (input_iterator const& begin_src, input_iterator const& end_src) : 
					buffer_string(m_buffer, Size, begin_src, end_src)
{
}

template <int Size>
fixed_string<Size> const&   fixed_string<Size>::operator = (fixed_string const& src)
{
	(buffer_string&)*this			=	src;
	return								*this;
}

template <int Size>
template <class src_type>
fixed_string<Size> const&   fixed_string<Size>::operator = (src_type const& s)
{
	(buffer_string & ) * this		=	s;
	return								*this;
}

template <int Size>
void   fixed_string<Size>::substr (size_type pos, size_type count, buffer_string * out_dest) const
{
	this->buffer_string::substr			(pos, count, out_dest);
}

template <int Size>
fixed_string<Size>   fixed_string<Size>::substr (size_type pos, size_type count) const
{
	fixed_string<Size> res;
	this->buffer_string::substr			(pos, count, & res);
	return								res;
}

template <int Size>
void   fixed_string<Size>::verify_self	()
{
	R_ASSERT							(begin() == m_buffer);
	R_ASSERT							(get_max_end() == m_buffer + Size);
}

template <int Size>
fixed_string<Size>   fixed_string<Size>::createf	(pcstr format, ...)
{
	fixed_string<Size>					result;
	va_list								argptr;
	va_start 							(argptr, format);
	result.appendf_va_list				(format, argptr);
	va_end	 							(argptr);
	return								result;
}

} // namespace xray

#endif // FIXED_STRING_INLINE_H_INCLUDED