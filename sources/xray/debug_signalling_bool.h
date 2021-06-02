#ifndef XRAY_SIGNALLING_BOOL_H_INCLUDED
#define XRAY_SIGNALLING_BOOL_H_INCLUDED

namespace xray {
namespace detail {

class signalling_bool_impl
{
public:
	signalling_bool_impl	(bool const value)
	{
		m_value				=	(value ? u8(1) : u8(0)) | unchecked_flag;		
	}

	operator bool			() const
	{
		m_value				&=	~unchecked_flag;
		return					m_value != 0;
	}

	bool operator !			() const
	{
		return					!(bool)(*this);
	}

	~signalling_bool_impl	()
	{
		R_ASSERT				((m_value & unchecked_flag) == 0, 
								"signalling_bool was not checked! check the return value!");
	}

private:
	enum {
		unchecked_flag		=	u8(128),
	};

private:
	mutable u8				m_value;
};

} // namespace detail

#ifdef MASTER_GOLD
typedef		bool								signalling_bool;
#else // #ifdef MASTER_GOLD
typedef		detail::signalling_bool_impl		signalling_bool;
#endif // #ifdef MASTER_GOLD

} // namespace xray

#endif // #ifndef XRAY_SIGNALLING_BOOL_H_INCLUDED