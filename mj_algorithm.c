#include <stdio.h>
#include <string.h>
#include "mjv2.h"

int g_cardWeight[4] = {6,8,12,24};
int g_jiang_centain_card = 0;    //是否258作将0-不是,1-是

int check_ke(stMjCard cards[], int index)                                   
{
	if (index >= MJ_NUMBER_COUNT)
		return 0;

	if (cards[index].cardCount >= 3)
	{
		cards[index].cardCount -= 3;
		cards[index].cardWeight = MJ_WEIGHT_HIGH;
		return 1;
	}

	return 0;
}

int check_shun(stMjCard cards[], int index)
{
	if (index > MJ_NUMBER_COUNT - 3)
		return 0;

	if (cards[index].cardCount >= 1 &&
		cards[index+1].cardCount >= 1 &&
		cards[index+2].cardCount >=1)
	{
		cards[index].cardCount -= 1;
		cards[index+1].cardCount -= 1;
		cards[index+2].cardCount -= 1;
		if (index != 0 && index != 6)
		{
		    cards[index].cardWeight = MJ_WEIGHT_HIGH;
		    if (cards[index+1].cardWeight != MJ_WEIGHT_HIGH)
				cards[index+1].cardWeight = MJ_WEIGHT_LOW;
		    cards[index+2].cardWeight = MJ_WEIGHT_HIGH;
		}
		return 1;
	}

	return 0;
}

int check_jiang(stMjCard cards[], int index)
{
	if (g_jiang_centain_card == 1 && index != 1 && index != 4 && index != 7)
		return 0;
	if (cards[index].cardCount >= 2)
	{
		cards[index].cardCount -= 2;
		cards[index].cardWeight = MJ_WEIGHT_LOW;
		return 1;
	}

	return 0;
}

void getCardWeight(stMjCard src[], stMjCard dst[])
{
	for (int i = 0; i < MJ_NUMBER_COUNT; i++)
		dst[i].cardWeight = src[i].cardWeight;
}

int judge_hu_without_jiang(stMjCard cards[])
{
	stMjCard tmpCards[MJ_NUMBER_COUNT] = {0};

	int hu = 1;
	memcpy(tmpCards, cards, sizeof(stMjCard) * MJ_NUMBER_COUNT);
	for (int i = 0; i < MJ_NUMBER_COUNT; i++)
	{
		if (tmpCards[i].cardCount < 1)
			continue;
		hu = 0;
		if (check_ke(tmpCards, i) == 1)
		{
			if(judge_hu_without_jiang(tmpCards) == 1)
			{
				getCardWeight(tmpCards, cards);
				return 1;
			}
			else
				return 0;
		}
		else if (check_shun(tmpCards, i) == 1)
		{
			if(judge_hu_without_jiang(tmpCards) == 1)
			{
				getCardWeight(tmpCards, cards);
				return 1;
			}
			else
				return 0;
		}
	}

	return hu;
}

int judge_hu_with_jiang(stMjCard cards[], int hasjiang)
{
	stMjCard tmp_cards[MJ_NUMBER_COUNT] = {0};

	int hu = 1;
	memcpy(tmp_cards, cards, sizeof(stMjCard) * MJ_NUMBER_COUNT);
	for (int i = 0; i < MJ_NUMBER_COUNT; i++)
	{
		if (tmp_cards[i].cardCount < 1)
			continue;

		hu = 0;
		/*
		if (hasjiang == 0 && tmp_cards[1].cardCount >= 2)
		{
			for (int usedforjiang = 0; usedforjiang <= 1; usedforjiang++)
			{
				hasjiang = usedforjiang;
				if (hasjiang == 1)
				{
					if (check_jiang(tmp_cards, i) != 1)
					{
						hasjiang = 0;
						break;
					}
				    else
					    return judge_hu_with_jiang(tmp_cards, hasjiang);
				}

				if (check_ke(tmp_cards, i) == 1)
					return judge_hu_with_jiang(tmp_cards, hasjiang);
				else if (check_shun(tmp_cards, i) == 1)
					return judge_hu_with_jiang(tmp_cards, hasjiang);
			}

		}
		*/
		if (hasjiang == 1)
		{
			if (check_ke(tmp_cards, i) == 1)
			{
				if (judge_hu_with_jiang(tmp_cards, hasjiang) == 1)
				{
					getCardWeight(tmp_cards, cards);
					return 1;
				}
				else
					return 0;
			}
			else if (check_shun(tmp_cards, i) == 1)
			{
				if (judge_hu_with_jiang(tmp_cards, hasjiang) == 1)
				{
					getCardWeight(tmp_cards, cards);
					return 1;
				}
				else
					return 0;
			}
		}
		else if(check_jiang(tmp_cards, i) == 1)
		{
			hasjiang = 1;
			if (judge_hu_with_jiang(tmp_cards, hasjiang) == 1)
			{
				getCardWeight(tmp_cards, cards);
				return 1;
			}
			tmp_cards[i].cardCount += 2;
			hasjiang = 0;
		}
	}

	return (hu && hasjiang);
}

int get_card_count(stMjCard cards[])
{
	int count = 0;
	for (int i = 0; i < MJ_NUMBER_COUNT; i++)
		count += cards[i].cardCount;

	return count;
}

int check_hu(stMjCard cards[], int needjiang, int *needcard, int iterationcount, stHuInfo *huInfo)
{
	stMjCard tmp_cards[MJ_NUMBER_COUNT] = {0};
	long long prob = huInfo->probability;
	long long minprob = 1;
	char tmp_needcard[MJ_NUMBER_COUNT] = {0};
	char min_needcard[MJ_NUMBER_COUNT] = {0};
	stMjCard minWeight[MJ_NUMBER_COUNT] = {0};

	if (++iterationcount > 6)
		return 0;
	if (get_card_count(cards) > 14)
		return 0;

	(*needcard)++;
	memcpy(tmp_cards, cards, sizeof(stMjCard) * MJ_NUMBER_COUNT);
	memcpy(tmp_needcard, huInfo->needcard, sizeof(tmp_needcard));
	for (int i = 0; i < MJ_NUMBER_COUNT; i++)
	{
		huInfo->probability = prob;
		memcpy(huInfo->needcard, tmp_needcard, sizeof(tmp_needcard));
		getCardWeight(cards, tmp_cards);
		if (tmp_cards[i].cardCount > 3)
			continue;
		tmp_cards[i].cardCount += 1;
		huInfo->needcard[i] += 1;
		huInfo->probability += g_cardWeight[tmp_cards[i].cardCount - 1];

		if (needjiang == 1)
		{
			if (judge_hu_with_jiang(tmp_cards, 0) == 1)
			{
				if (tmp_cards[i].cardWeight == MJ_WEIGHT_HIGH)
					huInfo->probability -= g_cardWeight[tmp_cards[i].cardCount - 1] / tmp_cards[i].cardWeight;
				//huInfo->probability += (g_cardWeight[tmp_cards[i].cardCount - 1]) / tmp_cards[i].cardWeight;
				getCardWeight(tmp_cards, cards);
				return 1;
			}
		}
		else
		{
			if (judge_hu_without_jiang(tmp_cards) == 1)
			{
				if (tmp_cards[i].cardWeight == MJ_WEIGHT_HIGH)
					huInfo->probability -= g_cardWeight[tmp_cards[i].cardCount - 1] / tmp_cards[i].cardWeight;
				//huInfo->probability += (g_cardWeight[tmp_cards[i].cardCount - 1]) / tmp_cards[i].cardWeight;
				getCardWeight(tmp_cards, cards);
				return 1;
			}
		}

		int lneedcard = 0;
		if (check_hu(tmp_cards, needjiang, &lneedcard, iterationcount, huInfo) == 1)
		{
			if (tmp_cards[i].cardWeight == MJ_WEIGHT_HIGH)
				huInfo->probability -= g_cardWeight[tmp_cards[i].cardCount - 1] / tmp_cards[i].cardWeight;
			if (minprob == 1)
			{
				minprob = huInfo->probability;
				memcpy(min_needcard, huInfo->needcard, sizeof(min_needcard));
				getCardWeight(tmp_cards, minWeight);
			}
			else if (minprob > huInfo->probability)
			{
				minprob = huInfo->probability;
				memcpy(min_needcard, huInfo->needcard, sizeof(min_needcard));
				getCardWeight(tmp_cards, minWeight);
			}
		}

		tmp_cards[i].cardCount -= 1;
		huInfo->needcard[i] -= 1;
	}

	if (minprob != 1)
	{
		huInfo->probability = minprob;
		memcpy(huInfo->needcard, min_needcard, sizeof(min_needcard));
		getCardWeight(minWeight, cards);
		return 1;
	}

	return 0;
}

int getkey(char cards[])
{
	int key = 0;
	for (int i = 0; i < MJ_NUMBER_COUNT; i++)
		key = key * 10 + cards[i];

	return key;
}

long long calc_probability(char cards[], int *needkey)
{
	int needcard = 0;
	long long prob = 1;
	long long total = 9 * 3;
	long long allprob = 0;
	int iterationcount = 0;
	stHuInfo huInfo;
	memset(huInfo.needcard, 0, sizeof(huInfo.needcard));
	huInfo.probability = 1;
	*needkey = 0;
	stMjCard mjCards[MJ_NUMBER_COUNT];

	for (int i = 0; i < MJ_NUMBER_COUNT; i++)
	{
		mjCards[i].cardCount = cards[i];
		mjCards[i].cardWeight = MJ_WEIGHT_LOW;
	}

	if (judge_hu_without_jiang(mjCards) == 1)
		return 1;

	if (check_hu(mjCards, 0, &needcard, iterationcount, &huInfo) == 1)
	{
		allprob = huInfo.probability;
		if (allprob > 1)
			allprob -= 1;
		*needkey = getkey(huInfo.needcard);
	}
	else
		allprob = 0;

	return allprob;
}

long long calc_probability_withjiang(char cards[], int *needkey)
{
	int needcard = 0;
	long long total = 9 * 3;
	long long allprob = 0;
	int iterationcount = 0;
	stHuInfo huInfo;
	huInfo.probability = 1;
	memset(huInfo.needcard, 0, sizeof(huInfo.needcard));
	*needkey = 0;

	stMjCard mjCards[MJ_NUMBER_COUNT];

	for (int i = 0; i < MJ_NUMBER_COUNT; i++)
	{
		mjCards[i].cardCount = cards[i];
		mjCards[i].cardWeight = MJ_WEIGHT_LOW;
	}

	if (judge_hu_with_jiang(mjCards, 0) == 1)
		return 1;

	if (check_hu(mjCards, 1, &needcard, iterationcount, &huInfo) == 1)
	{
		allprob = huInfo.probability;
		if (allprob > 1)
			allprob -= 1;
		*needkey = getkey(huInfo.needcard);
	}
	else
		allprob = 0;

	return allprob;
}


