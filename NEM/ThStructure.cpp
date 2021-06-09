#include "pch.h"
#include "ThStructure.h"
void SetFlag(DWORD* flag_to_set, DWORD flag)
{
	*flag_to_set |= flag;
}

void CancelFlag(DWORD* flag_to_set, DWORD flag)
{
	*flag_to_set &= ~flag;
}