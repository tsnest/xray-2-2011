
//-----------------------------------
// CASE 2
//-----------------------------------

#define PROPERTY(type)	struct property ## type ## __LINE__

#define PROPERTY_BODY		template <class T>							\
							void assign_helper (type const& value)		\
							{											\
								m_value	=	value;						\
							}											\
							void operator = (type const& value)			\
							{											\
								assign_helper<int> (value);				\
							}											\
							template <class T>							\
							type const&	conversion_helper ()			\
							{											\
								return		m_value;					\
							};											\
							operator type ()							\
							{											\
								return		conversion_helper<int> ();	\
							}											\
							type m_value;

#define PROPERTY_SET		template <>									\
							void assign_helper<int> (int const& value)

#define PROPERTY_GET		template <>									\
							int const&   conversion_helper<int> ()

class C2
{
public:
	
	PROPERTY(int)
	{
		PROPERTY_BODY

		PROPERTY_SET
		{
			LOG_INFO	("setting property");
			m_value	=	value;
		}
		
		PROPERTY_GET
		{
			LOG_INFO	("getting property");
			return		m_value;		
		}

	} m_prop;

};

template <class T, class Derived>
struct property
{
	typedef T type;
	typedef property derived;
	void operator = (T const& value)
	{
		((Derived*)this)->set	(value);
	}

	operator T ()
	{
		return			((Derived*)this)->get();
	}

	void set (T const& value)
	{
		m_value		=	value;
	}

	T const& get ()
	{
		return			m_value;
	}


	T m_value;
};

class A
{
public:
	struct my : public property<int, my>
	{
		void operator = (type const& value) { derived::operator = (value); }

		void set (int const& value)
		{
			LOG_INFO	("setting property");
			m_value	=	value;
		}

		int const& get ()
		{
			LOG_INFO	("getting property");
			return		m_value;
		}

	};
};

	A c;

	c.m_prop		=	5;
	int a1			=	c.m_prop;

	printf				("%d", (int)c.m_prop);
	bool catecher	=	true;


// 	C1 c;
// 	c.m_prop		=	5;
// 	int a1			=	c.m_prop;
// 	printf				("%d", (int)c.m_prop);
// 	bool catecher	=	true;
// 
// 	C2 c2;
// 	c2.m_prop		=	5;
// 	int a2			=	c2.m_prop;
// 	printf				("%d", (int)c2.m_prop);
// 	bool catecher2	=	true;