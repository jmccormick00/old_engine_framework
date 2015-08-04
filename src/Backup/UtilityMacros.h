
#ifndef _UTILITYMACROS_
#define _UTILITYMACROS_

// Macro for casting a pointer
#define castPointer(ptr, typeCast, originalType) static_cast<typeCast*>((originalType*)ptr);

// Convenience macro for releasing COM objects.
#define ReleaseCOM(x) { if(x) { x->Release(); x = 0; } }

#endif // _UTILITYMACROS_
