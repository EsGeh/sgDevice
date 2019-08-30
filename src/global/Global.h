#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "m_pd.h"
#include "Basic.h"
#include "DynArray.h"

#include <string.h>


#ifdef DEBUG
	#define DB_PRINT(message, ...) \
		post(message, ## __VA_ARGS__)
#else
	#define DB_PRINT(message, ...)
#endif

DECL_DYN_ARRAY(ByteDynA,unsigned char,getbytes,freebytes)
DEF_DYN_ARRAY(ByteDynA,unsigned char,getbytes,freebytes)
/*
DECL_LIST(ByteList,ByteEl,unsigned char,getbytes,freebytes,freebytes)
DEF_LIST(ByteList,ByteEl,unsigned char,getbytes,freebytes,freebytes)
*/

/*
DECL_DYN_ARRAY(AtomDynA,t_atom,getbytes,freebytes)
DEF_DYN_ARRAY(AtomDynA,t_atom,getbytes,freebytes)
*/


INLINE int compareAtoms(t_atom* atoml, t_atom* atomr)
{
	if(atoml -> a_type == A_SYMBOL)
	{
		if(atomr -> a_type == A_SYMBOL)
		{
			return atom_getsymbol( atoml ) == atom_getsymbol( atomr );
		}
	}
	else
	{
		if(atomr -> a_type == A_FLOAT)
		{
			return atom_getfloat(atoml)==atom_getfloat(atomr);
		}
	}
	return 0;
}

INLINE BOOL atomEqualsString(t_atom* pAtom, char* string)
{
	char buf[256];
	atom_string(pAtom,buf,256);
	if(!strncmp(buf,string,256))
		return TRUE;
	return FALSE;
}

#endif
