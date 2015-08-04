

#ifndef _TREGISTRY_
#define _TREGISTRY_

#include "C_SmartPointer.h"
#include <map> 

template<typename key, typename T>
class T_Registry {
private: 
	typedef std::map<key, C_SmartPointer<T>> RegistryMap;
	RegistryMap d_registry;

public:
	
	T_Registry() {}
	~T_Registry() { d_registry.clear(); }

	enum RegResult { E_FAILED, E_SUCCESS, E_DOUBLEENTRY };
	
	void clear() {
		d_registry.clear();
	}

	C_SmartPointer<T> get(const key& k) {
		RegistryMap::iterator i = d_registry.find(k);
		if(i == d_registry.end()) {
			C_SmartPointer<T> null;
			return null;
		}

		return (*i).second;
	}
	
	RegResult insert(key k, C_SmartPointer<T> p) {
			// Make sure it doesnt already exist
		RegistryMap::iterator itr = d_registry.find(k);
		RegistryMap::iterator endItr = d_registry.end(); 

		if(itr == endItr) {	// No duplicates
			std::pair<RegistryMap::iterator, bool> insertResult = 
				d_registry.insert(std::pair<key, C_SmartPointer<T>>(k, p));

			if(!insertResult.second)
				return E_FAILED;

			if(insertResult.first == endItr)
				return E_FAILED;

			return E_SUCCESS;
		} else {
			return E_DOUBLEENTRY;
		}
	}

	void remove(const key& k) {
		d_registry.erase(k);
	}
};


#endif // _TREGISTRY_
