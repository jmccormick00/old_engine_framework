
#ifndef _CHASHSTRING_
#define _CHASHSTRING_

#include <string>

typedef unsigned long HashedValue;

class C_HashedString
{
private:

	// note: m_ident is stored as a void* not an int, so that in
	// the debugger it will show up as hex-values instead of
	// integer values. 
	void*             d_ident;
	std::string		  d_identStr;
	
	C_HashedString() {}

public:
	C_HashedString( char const* const pIdentString )
		: d_ident( hashName( pIdentString ) )
	, d_identStr( pIdentString )
	{}

	C_HashedString(const C_HashedString& d) : d_ident(d.d_ident), d_identStr(d.d_identStr) {}
	~C_HashedString() {}

	HashedValue getHashValue() const {
		return reinterpret_cast<HashedValue>(d_ident);
	}

	const std::string& getStr() const {
		return d_identStr;
	}

	static void* hashName(char const*  pIdentStr);

	bool operator< (C_HashedString const& o) const {
		return (getHashValue() < o.getHashValue());
	}

	bool operator== (C_HashedString const& o) const {
		return (getHashValue() == o.getHashValue());
	}
};

#endif // _CHASHSTRING_
