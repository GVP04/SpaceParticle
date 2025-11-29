#include "pch.h"
#include "CSDataArray.h"

SData CSDataArray::EmptyData = { 0 };


CSDataArray::CSDataArray()
{
	m_Array = NULL;
	MaxItems = SDA_DEF_AMOUNT;
	Init();
}

CSDataArray::CSDataArray(int in_MaxItems)
{
	m_Array = NULL;

	if (in_MaxItems > 0) MaxItems = in_MaxItems;
	else MaxItems = SDA_DEF_AMOUNT;

	Init();
}

CSDataArray::~CSDataArray()
{
	CurPos = -1;
	MaxItems = 0;
	if (m_Array) delete[] m_Array;
	m_Array = NULL;
}

void CSDataArray::Init()
{
	CurPos = -1;
	if (MaxItems < 1) MaxItems = 1;

	if (!m_Array) m_Array = new SData[MaxItems];
	ZeroMemory(m_Array, MaxItems * sizeof(SData));
}

int CSDataArray::AddData(int in_maxCountLimit, SData *in_Data)
{
	if (m_Array && in_Data)
	{
		CurPos++;
		if ((in_maxCountLimit >5000 && CurPos >= in_maxCountLimit) || CurPos > DA_MAX_ITEMS)
		{
			int deltaMove = CurPos >> 4;
			CurPos -= deltaMove;
			CopyMemory(m_Array, m_Array + deltaMove, (CurPos + 1) * sizeof(SData));
		}
		else if (CurPos >= MaxItems)
		{
			MaxItems += MaxItems + 1;
			SData *m_Array2 = new SData[MaxItems + 5000];
			CopyMemory(m_Array2, m_Array, CurPos * sizeof(SData));
			delete[] m_Array;
			m_Array = m_Array2;
		}
		m_Array[CurPos] = *in_Data;
		m_Array[CurPos].AbsSpeed = in_Data->Speed.LEN();
	}
	return CurPos;
}

SData *CSDataArray::GetCurData()
{
	return (m_Array && CurPos >= 0) ? m_Array + CurPos : NULL;
	//return (m_Array && CurPos >= 0) ? m_Array + CurPos : &CSDataArray::EmptyData;
}

int CSDataArray::ShrinkData(int in_nItems, int in_Flags)// = DA_SHRINK_FLAG_NONE
{
	if ((in_Flags & DA_SHRINK_FLAG_FREE)) ShrinkVolune(in_nItems);
	else 
	{
		if (!m_Array) Init();

		if (m_Array && in_nItems > 0 && CurPos + 1 - in_nItems > 0)
		{
			CopyMemory(m_Array, m_Array + CurPos + 1 - in_nItems, in_nItems * sizeof(SData));
			CurPos = in_nItems - 1;
		}
	}
	return MaxItems;
}

int CSDataArray::ShrinkVolune(int in_nItems)
{
	if (!m_Array) Init();

	if (m_Array && in_nItems > 0 && CurPos + 1 - in_nItems > 0)
	{
		SData* m_Array2 = new SData[MaxItems];
		CopyMemory(m_Array2, m_Array + CurPos + 1 - in_nItems, in_nItems * sizeof(SData));
		CurPos = in_nItems - 1;
		MaxItems = in_nItems;
		delete[] m_Array;
		m_Array = m_Array2;
	}
	return MaxItems;
}

int CSDataArray::GetFloatPos(double in_Time, double *midPosition)
{
	int ret = -1;
	if (midPosition && CurPos >= 0 && m_Array && in_Time >= 0.0 && in_Time <= m_Array[CurPos].TimePoint)
	{
		*midPosition = 0.0;
		int low = 0, hi = CurPos;
		if (m_Array[0].TimePoint < in_Time)
		{
			while (low < hi + 1)
			{
				int mid = ((low + hi) >> 1);
				if (m_Array[mid].TimePoint < in_Time) 
					low = mid;
				else 
					if (m_Array[mid].TimePoint > in_Time) 
						hi = mid;
				else 
						low = hi = mid;
			}
			ret = low;
			*midPosition = (in_Time - m_Array[low].TimePoint) / (m_Array[hi].TimePoint - m_Array[low].TimePoint);
		}
		else
			if (m_Array[0].TimePoint == in_Time) 
				ret = 0;
	}
	return ret;
}


double CSDataArray::GetFloatPos(double in_Time)
{
	double ret = -1.0;

	int idx = GetFloatPos(in_Time, &ret);

	return ret + (double)idx;
}

int CSDataArray::GetLowIdxByTime(double in_ViewTime)
{
	int ret = -1;

	if (m_Array && CurPos >= 0)
		if (in_ViewTime >= m_Array[CurPos].TimePoint) 
			ret = CurPos;
		else 
			if (in_ViewTime < m_Array[0].TimePoint) ret = -1;
		else 
				ret = GetLowIdxByTime(in_ViewTime, 0, CurPos);

	return ret;
}

//private вызов только из GetLowIdxByTime(double in_ViewTime), значение однозначно в диапазоне
int CSDataArray::GetLowIdxByTime(double in_ViewTime, int min, int max)
{
	int ret = min;

	while (min < max)
	{
		ret = min + ((max - min + 1) >> 1);
		if (m_Array[ret].TimePoint > in_ViewTime) 
			max = ret - 1;
		else 
			min = ret;
	}

	return min;
}


int CSDataArray::DeletePoints(double TimeFromGE, double TimeToLE)
{
	int ret = 0;
	if (m_Array)
	{
		int idxLo = GetLowIdxByTime(TimeFromGE);
		int idxHi = GetLowIdxByTime(TimeToLE);
		if (idxHi == CurPos)
		{
			CurPos = idxLo - 1;
			if (CurPos < 0) CurPos = 0;
		}
		else
		if (idxLo >= 0 && idxHi >= 0 && idxHi >= idxLo && idxHi < CurPos)
		{
			CopyMemory(m_Array + idxLo, m_Array + idxHi + 1, (CurPos - idxHi) * sizeof(SData));
			ret = idxHi - idxLo + 1;
			CurPos -= ret;
		}
	}

	return ret;
}


void CSDataArray::GetMinMaxTracePos(SPos& min_Pos, SPos& max_Pos, int idx_Start, int idx_End)
{
	if (m_Array)
	{
		if (idx_Start < 0 || idx_Start > CurPos) idx_Start = 0;
		if (idx_End < 0 || idx_End < idx_Start || idx_End > CurPos)  idx_End = CurPos;

		SData *pos = m_Array + idx_Start;

		for (int i = idx_Start; i <= idx_End; i++, pos++)
		{
			if (min_Pos.X > pos->position.X)
				min_Pos.X = pos->position.X;
			if (max_Pos.X < pos->position.X) 
				max_Pos.X = pos->position.X;

			if (min_Pos.Y > pos->position.Y) 
				min_Pos.Y = pos->position.Y;
			if (max_Pos.Y < pos->position.Y)
				max_Pos.Y = pos->position.Y;

			if (min_Pos.Z > pos->position.Z)
				min_Pos.Z = pos->position.Z;
			if (max_Pos.Z < pos->position.Z)
				max_Pos.Z = pos->position.Z;
		}
	}
}