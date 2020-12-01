#ifndef _MJ_V2_H_
#define _MJ_V2_H_
#include "list.h"

#define MJ_COLOR_COUNT    (4)
#define MJ_NUMBER_COUNT    (9)

#define MJ_WEIGHT_LOW    (1)
#define MJ_WEIGHT_HIGH    (2)

typedef struct hu_info
{
	stListEntry entry;
	int cardkey;
	int needkey_nojiang;
	int needkey;
	long long probability;
	long long prob_jiang;
	char card[MJ_NUMBER_COUNT];
	char needcard[MJ_NUMBER_COUNT];
}stHuInfo;

typedef struct mj_card
{
	int cardValue;
	int cardCount;
	int cardWeight;
}stMjCard;

long long calc_probability(char cards[], int *needkey);
long long calc_probability_withjiang(char cards[], int *needkey);
#endif
