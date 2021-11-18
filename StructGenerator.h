#ifndef STRUCT_NAME
#error "no name"
#endif // !STRUCT_NAME

#ifndef STRUCT_BODY
#error "no body"
#endif

#define STYPE(addr,type,name) type* name(){return (type*)(((char*)this)+(addr));};
#define ARRTYPESZ(addr,type,name,size) type* name(int index){return (type*)(((char*)this)+(addr)+(size)*(index));};

struct STRUCT_NAME
{
	STRUCT_BODY
};

#undef STRUCT_NAME
#undef STRUCT_BODY

#undef STYPE
#undef ARRTYPESZ