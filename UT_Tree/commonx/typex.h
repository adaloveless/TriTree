#ifndef _TYPEX_H_
#define _TYPEX_H_
#include <functional>

#define ABSTRACT_FUNCTION =0

#define GREATER_OF(_a_,_b_) ((_a_)>(_b_)?(_a_):(_b_))
#define LESSER_OF(_a_,_b_) ((_a_)>(_b_)?(_a_):(_b_))
#define ABS(_a_) ((_a_)<0?(0-(_a_)):(_a_))

typedef int64_t NativeInt;
typedef NativeInt ni;

//-----------------------------------------------------------------------------------------
template<class T, class TGetter = std::function<T& ()>, class TSetter = std::function<void(T)>>
struct PROPERTY
{
	template<class X, class Y, class Z> friend struct PROPERTY; // makes this a friend to all instanciations
private:
	TGetter m_Getter;
	TSetter m_Setter;
	T m_ValueIfNoAccessors;

	T& get()
	{
		if (!m_Getter.empty())
			return m_Getter();
		else
			return m_ValueIfNoAccessors;
	}

	const T& get() const
	{
		if (!m_Getter.empty())
			return m_Getter();
		else
			return m_ValueIfNoAccessors;
	}

	void set(T newValue)
	{
		if (!m_Setter.empty())
			m_Setter(newValue);
		else
			m_ValueIfNoAccessors = newValue;
	}

	typename typedef PROPERTY<T, TGetter, TSetter> prop_type;

public:
	PROPERTY() : m_Getter(), m_Setter(), m_ValueIfNoAccessors() { set(T()); }
	PROPERTY(T initValue) : m_Getter(), m_Setter(), m_ValueIfNoAccessors() { set(initValue); }
	PROPERTY(TGetter getter) : m_Getter(getter), m_Setter(), m_ValueIfNoAccessors() { set(T()); }
	PROPERTY(TGetter getter, TSetter setter) : m_Getter(getter), m_Setter(setter), m_ValueIfNoAccessors() { set(T()); }

	operator T() { return get(); }

	template<class X, class Y, class Z>
	prop_type& operator=(PROPERTY<X, Y, Z>& other)
	{
		set(other.get());

		return *this;
	}

	template<class TI>
	prop_type& operator=(const TI& other)
	{
		set(other);

		return *this;
	}

	/*
		This operator was introduced for simplifying access to call operators:

		PROPERTY<boost::function<int (int)>> myFunc;

		myFunc(345); // ERROR!
		myFunc()(345); // now is valid...
	*/
	T& operator()() { return get(); }
	const T& operator()() const { return get(); }

	/*
		Beaware of the fact, that anyone really wanting to change the backing value
		of a property sure can, by simply doing:

		PROPERTY<T> myProp;
		T* myRef = myProp.operator->();

		So do not rely on Getter/Setter safety in case of any security critical scenario.
	*/
	T* operator->() { return &get(); }
	const T* operator->() const { return &get(); }

	template<class TI> prop_type& operator/=(TI newValue) { set(get() / newValue); return *this; }
	template<class TI> prop_type& operator+=(TI newValue) { set(get() + newValue); return *this; }
	template<class TI> prop_type& operator-=(TI newValue) { set(get() - newValue); return *this; }
	template<class TI> prop_type& operator*=(TI newValue) { set(get() * newValue); return *this; }

	template<class TI> auto operator/(TI newValue) const -> decltype(T() / TI()) { return get() / newValue; }
	template<class TI> auto operator+(TI newValue) const -> decltype(T() + TI()) { return get() + newValue; }
	template<class TI> auto operator-(TI newValue) const -> decltype(T() - TI()) { return get() - newValue; }
	template<class TI> auto operator*(TI newValue) const -> decltype(T() * TI()) { return get() * newValue; }
};


//---------------------------------------------------------------------TRY..FINALLY for C++
class never_thrown_exception {};
#define begin_try    with_finally([&](){ try
#define finally      catch(never_thrown_exception){throw;} },[&]()
#define end_try      ) // sorry (NOT SORRY) for "pascalish" style 

//----------------------------------------------------------------------------------------

class BetterObject
{
public:
	BetterObject() {};
	~BetterObject() {};
};

template <typename TCode, typename TFinallyCode>
inline void with_finally(const TCode &code, const TFinallyCode &finally_code)
{
	try
	{
		code();
	}
	catch (...)
	{
		try
		{
			finally_code();
		}
		catch (...) // Maybe stupid check that finally_code mustn't throw.
		{
			std::terminate();
		}
		throw;
	}
	finally_code();
}
//------------------------------------------------------------------------------------------
#endif