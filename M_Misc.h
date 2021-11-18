#pragma once
#include "ThStructure.h"

void InjectShop();
void InjectPlayerPositionTest();

CardInShopInfo* GetpCardInfoFromId(int id);
bool IsCardEquipped(int id);

void M_OpenSuperShop();