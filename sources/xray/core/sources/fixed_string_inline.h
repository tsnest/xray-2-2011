#ifndef FIXED_STRING_INLINE_H_INCLUDED
#define FIXED_STRING_INLINE_H_INCLUDED

namespace xray {
namespace core {

//-----------------------------------------------------------------------------------
// initializations
//-----------------------------------------------------------------------------------

template <int Size>
fixed_string<Size>::fixed_string () : buffer_string(m_buffer, Size)
{
	if ( Size ) m_buffer[0] = NULL;
}

template <int Size>
fixed_string<Size>::fixed_string (const fixed_string& src) : buffer_string(m_buffer, Size, src.begin(), src.end())
{
}

template <int Size>
template <int SrcSize>
fixed_string<Size>::fixed_string (const fixed_string<SrcSize>& src) : buffer_string(m_buffer, Size, src.begin(), src.end())
{
}

template <int Size>
fixed_string<Size>::fixed_string (const value_type* src) : buffer_string(m_buffer, Size, src)
{
}

template <int Size>
fixed_string<Size>::fixed_string (const value_type src) : buffer_string(m_buffer, Size)
{
	char buff[]	=	{src, 0};
	append((char*)buff, buff+1);
}

template <int Size>
fixed_string<Size>::fixed_string (const value_type* src, size_type const& count) : buffer_string(m_buffer, Size)
{
	strncpy_s(m_buffer, Size, src, count);
}

template <int Size>
template <typename input_iterator>
fixed_string<Size>::fixed_string (input_iterator const& begin_src, input_iterator const& end_src) : 
					buffer_string(m_buffer, Size, begin_src, end_src)
{
}

template <int Size>
fixed_string<Size>&   fixed_string<Size>::operator = (const value_type*	src)
{
	this->~fixed_string();
	this->fixed_string::fixed_string(src);	
	return *this;
}

template <int Size>
fixed_string<Size>&   fixed_string<Size>::operator = (const value_type src)
{
	this->~fixed_string();
	this->fixed_string::fixed_string(src);	
	return *this;
}

template <int Size>
fixed_string<Size>&   fixed_string<Size>::operator = (const fixed_string& src)
{
	this->~fixed_string();
	this->fixed_string::fixed_string(src);
	return *this;
}

template <int Size>
template <int SrcSize>
fixed_string<Size>&   fixed_string<Size>::operator = (const fixed_string<SrcSize>& src)
{
	this->~fixed_string();
	this->fixed_string::fixed_string(src);
	return *this;
}

template <int Size>
void   fixed_string<Size>::substr (size_t pos, size_t count, fixed_string& dest) const
{
	this->buffer_string::substr(pos, count, dest);
}

template <int Size>
fixed_string<Size>   fixed_string<Size>::substr (size_t pos, size_t count) const
{
	fixed_string<Size> res;
	this->buffer_string::substr(pos, count, res);
	return res;
}

} // namespace core
} // namespace xray

#endif // FIXED_STRING_INLINE_H_INCLUDED