#pragma once

#include "CSpParticle.h"

#define DA_SHRINK_FLAG_NONE		0x0000
#define DA_SHRINK_FLAG_FREE		0x0001
#define DA_MAX_ITEMS			0x1000000

class CSDataArray
{
	friend class CSpParticle;
	friend class CSP_Group;
	friend class CDrawDlg;
	friend void PaintTraceToCDC(CDC* DCMem, SP_Calc* mData, int in_DrawFlags, SPos& delta);

private:
	SData* m_Array;
	int CurPos;
	int MaxItems;
	SData NextData;
	void Init();
	int GetLowIdxByTime(double in_ViewTime, int min, int max);

public:
	CSDataArray();
	CSDataArray(int in_MaxItems);
	~CSDataArray();
	int AddData(int in_maxCountLimit, SData *in_Data);
	int ShrinkData(int in_nItems, int in_Flags = DA_SHRINK_FLAG_NONE);
	int ShrinkVolune(int in_nItems);
	SData* GetCurData();
	//int GetData(SData* out_Data, int in_Idx);
	//int GetInterpolatedData(SData* out_Data, double in_Time);
	//int GetCurData(SData* out_Data);
	int GetCurPos() { return CurPos; }
	int GetCountMax() { return MaxItems; }
	int GetLowIdxByTime(double in_ViewTime);
	double GetFloatPos(double in_Time);
	int GetFloatPos(double in_Time, double* midPosition);
	int DeletePoints(double TimeFromGE, double TimeToLE);
	void GetMinMaxTracePos(SPos& min_Pos, SPos& max_Pos, int idxStart, int idx_End);


	static SData EmptyData;
};


