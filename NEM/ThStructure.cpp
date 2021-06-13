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

void SetAllPlayerOpOnTickFunc()
{//fix 2un's playerOp
	if (PPLAYER)
	{
		for (int i = 5; i <= 16; i++)
		{
			VALUED(PPLAYER + 0x580 + 0xF0 * i + 0xE8) = 0;
		}
	}
}
