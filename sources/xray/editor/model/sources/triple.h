////////////////////////////////////////////////////////////////////////////
//	Created		: 26.07.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef TRIPLE_H_INCLUDED
#define TRIPLE_H_INCLUDED

namespace xray {
namespace model_editor {

template <typename T>
class triple
{
public:
	triple(T one, T two, T three)
	{
		m_elements[0] = one;
		m_elements[1] = two;
		m_elements[2] = three;
	}
	bool contains( T value ) const { return value == m_elements[0] || value == m_elements[1] || value == m_elements[2]; }
	T operator[]( u32 index ) const { return m_elements[ index ]; }
	T& operator[]( u32 index ) { return m_elements[ index ]; }
private:
	T m_elements[3];
};
 // struct triple

} // namespace model_editor
} // namespace xray

#endif // #ifndef TRIPLE_H_INCLUDED