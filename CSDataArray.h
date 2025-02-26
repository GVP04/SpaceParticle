#pragma once

#include "CSpParticle.h"


class CSDataArray
{
private:
	SData* m_Array;
	int CurPos;
	int MaxItems;
	SData NextData;
	void Init();

public:
	CSDataArray();
	CSDataArray(int in_MaxItems);
	~CSDataArray();
	void Reset();
	int AddData(SData *in_Data);
	SData* GetCurData();

	static SData EmptyData;
};


