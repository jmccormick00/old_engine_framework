/*=========================================================================
/ James McCormick - C_SmartPointer.h
/ The header file for the SmarPointer system.  Any object that wants to use
/ a smart pointer must use this base class. 
/==========================================================================*/

#ifndef _SMARTPOINTER_
#define _SMARTPOINTER_

/*=========================================================================
/ C_Object - Class
/ An object class that will be used with the smart pointer class
/ This is a super class for any class that wants to use the smart 
/ pointer class
/==========================================================================*/
class C_SmartPointerObject {
public:
	// Constructor
	C_SmartPointerObject() : d_sNumRef(0) {}
	virtual ~C_SmartPointerObject() {}

private:
	unsigned short d_sNumRef;		
	void DecrementRef();
	void IncrementRef() { d_sNumRef++; }

	template <class T> friend class C_SmartPointer;
};

//------------------------------------------------------------------------------
// Decrements the reference count for the object
// If the count has reached zero, the object deletes itself
//------------------------------------------------------------------------------
inline void C_SmartPointerObject::DecrementRef() {
	if(--d_sNumRef == 0) 
		delete this;
}

/*=========================================================================
/ A class that will do dynamic memory deletion if the object that the pointer
/ points to is no longer referenced.  Can be used with any class that 
/ inherents from the C_SmartPointerObject class
/==========================================================================*/
template <class T> class C_SmartPointer {
private:
		T* d_ObjectPtr;		// The pointer to the object
	
public:
		// Constructor
		C_SmartPointer(T* obj = 0);
		
		// Copy Constructor
		C_SmartPointer(const C_SmartPointer<T> &obj);
		
		// Destructor
		~C_SmartPointer();
	
		// Implicit Conversion
		operator T* () const { return d_ObjectPtr; }
		T& operator* () const { return *d_ObjectPtr; }
		T* operator-> () const { return d_ObjectPtr; }
	
		// Assigment operations
		C_SmartPointer& operator= (T* obj);
		C_SmartPointer& operator= (const C_SmartPointer<T> obj);
	
		// Comparisons
		bool operator== (const T* obj) const { return d_ObjectPtr == obj; }
		bool operator!= (const T* obj) const { return d_ObjectPtr != obj; }
		bool operator== (const C_SmartPointer<T> obj) const { return d_ObjectPtr == obj.d_ObjectPtr; }
		bool operator!= (const C_SmartPointer<T> obj) const { return d_ObjectPtr != obj.d_ObjectPtr; }
};


//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
template <class T>
C_SmartPointer<T>::C_SmartPointer(T* obj) {
	d_ObjectPtr = obj;
	if( d_ObjectPtr )
		d_ObjectPtr->IncrementRef();
}

//------------------------------------------------------------------------------
// Copy Constructor
//------------------------------------------------------------------------------
template <class T>
C_SmartPointer<T>::C_SmartPointer(const C_SmartPointer<T> &obj) {
	d_ObjectPtr = obj.d_ObjectPtr;
	if( d_ObjectPtr )
		d_ObjectPtr->IncrementRef();
}


//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
template <class T> 
C_SmartPointer<T>::~C_SmartPointer() {
	if(d_ObjectPtr) 
		d_ObjectPtr->DecrementRef();
}


template <class T>
C_SmartPointer<T>& C_SmartPointer<T>::operator= (T* obj) {
	if( d_ObjectPtr != obj) {
		// If the new pointer is valid, then increment the ref count 
		// for the object
		if(obj)
			obj->IncrementRef();
		
		// If the classes pointer points to an object, then decrement the count
		// for that object
		if(d_ObjectPtr)
			d_ObjectPtr->DecrementRef();
		
		d_ObjectPtr = obj;
	}
	return *this;
}


template <class T>
C_SmartPointer<T>& C_SmartPointer<T>::operator= (const C_SmartPointer<T> obj) {
	if(d_ObjectPtr != obj.d_ObjectPtr) {
		// If there is an object being pointed to by the SmartPtr on the right 
		// hand side, then increment the count
		if(obj.d_ObjectPtr)
			obj->IncrementRef();
		
		// If the classes pointer points to an object, then decrement the count
		// for that object
		if(d_ObjectPtr)
			d_ObjectPtr->DecrementRef();
		
		d_ObjectPtr = obj.d_ObjectPtr;
	}
	return *this;
}


/*=========================================================================
/ A class that will create a 1D array.  
/ The size is passed to the constructor.  Can be used with smartpointers.
/==========================================================================*/
template <class T>
class C_1DArray : public C_SmartPointerObject {
public:

	typedef T* iterator;
	typedef const T* const_iterator;

	// Constructor
	C_1DArray(unsigned size) : C_SmartPointerObject() 
	{
		d_pArray = new T[size];
		d_uiSize = size;
	}

	virtual ~C_1DArray() { delete[] d_pArray; }

	// Element Access
	T& At(unsigned i)						{ return d_pArray[i]; }
	const T& At(unsigned i) const			{ return d_pArray[i]; }
	T& operator[](unsigned i)				{ return d_pArray[i]; }
	const T& operator[](unsigned i) const	{ return d_pArray[i]; }
	const T* Data() const { return d_pArray; }
	T* CArray() { return d_pArray; }

	iterator begin() { return &d_pArray[0]; }
	const_iterator begin() const { return &d_pArray[0]; }
	T& front() { return d_pArray[0]; } 
	const T& front() const { return d_pArray[0]; }

	iterator end() { return &d_pArray[d_uiSize-1]; }
	const_iterator end() const { return &d_pArray[d_uiSize-1]; }
	T& back() { return d_pArray[d_uiSize-1]; } 
	const T& back() const { return d_pArray[d_uiSize-1]; }

	unsigned Size() { return d_uiSize; }

private:
	T* d_pArray;
	unsigned d_uiSize;

	C_1DArray();
	C_1DArray(C_1DArray&);
};



/*=========================================================================
/ A class that will create a 2D array.  
/ The size is passed to the constructor.  Can be used with smartpointers.
/ Uses Row major order.
/==========================================================================*/
template <class T>
class C_2DArray : public C_SmartPointerObject {
public:
	// Constructor
	C_2DArray(unsigned sizeI, unsigned sizeJ) : C_SmartPointerObject() {
		d_pArray = new T[sizeI*sizeJ];
		d_uiSizeI = sizeI;
		d_uiSizeJ = sizeJ;
	}

	virtual ~C_2DArray() { delete[] d_pArray; }

	// Element Access
	T& At(unsigned i, unsigned j)						{ return d_pArray[i*d_uiSizeJ + j]; }
	const T& At(unsigned i, unsigned j) const			{ return d_pArray[i*d_uiSizeJ + j]; }
	T& operator()(unsigned i, unsigned j)				{ return d_pArray[i*d_uiSizeJ + j]; }
	const T& operator()(unsigned i, unsigned j) const	{ return d_pArray[i*d_uiSizeJ + j]; }
	const T* Data() const { return d_pArray; }
	T* CArray() { return d_pArray; }

	unsigned SizeI() { return d_uiSizeI; }
	unsigned SizeJ() { return d_uiSizeJ; }

private:
	T* d_pArray;
	unsigned d_uiSizeI;
	unsigned d_uiSizeJ;

	C_2DArray();
	C_2DArray(C_2DArray&);
};

#endif
