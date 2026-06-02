#include "pch.h"
#include "CSDataArray.h"


SquizeFlag DA_ADD_FlagFromResult(SquizeFlag in_Flags)
{
	int ret, retSecond;
	if ((in_Flags & DA_ADD_RESULT_SQUISED)) ret = DA_ADD_FLAG_SQUISE;
	else
		if ((in_Flags & DA_ADD_RESULT_ADDED)) ret = DA_ADD_FLAG_ADD_FORCE;
		else
			ret = DA_ADD_FLAG_UPDATE;

	in_Flags >>= DA_ADD_LEVEL_MOVECOUNT;
	if ((in_Flags & DA_ADD_RESULT_SQUISED)) retSecond = DA_ADD_FLAG_SQUISE;
	else
		if ((in_Flags & DA_ADD_RESULT_ADDED)) retSecond = DA_ADD_FLAG_ADD_FORCE;
		else
			retSecond = DA_ADD_FLAG_UPDATE;

	return ret | (retSecond << DA_ADD_LEVEL_MOVECOUNT);
};










