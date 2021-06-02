////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef VECTOR_PORTABLE_H_INCLUDED
#define VECTOR_PORTABLE_H_INCLUDED


template<typename T>
struct 	vector_portable:
	private boost::noncopyable
{
	vector_portable():m_p(0),m_num(0){}
	~vector_portable(){
		clear ( );
	}
	T								*m_p;
	u32								m_num;
	void resize( u32 num )
	{
		clear ( );
		m_p = (T*)MALLOC(sizeof(T)*num,"vector_portable");
		m_num = num;
		for( u32 i=0; i<num; ++i )
			new	(&m_p[i])T;
		
		
	}
	void clear ( )
	{
		for( u32 i=0; i<m_num; ++i )
			m_p[i].~T();
		FREE(m_p);m_num=0;
	}
inline u32 size( )const{return m_num;}
inline T &operator [] (u32 i)
{
#ifdef	DEBUG 
	ASSERT(i<m_num);
#endif
	return m_p[i];
}
inline const T &operator [] (u32 i)const
{
#ifdef	DEBUG 
	ASSERT(i<m_num);
#endif
	return m_p[i];
}
};

#endif // #ifndef VECTOR_PORTABLE_H_INCLUDED