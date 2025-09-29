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

int CSDataArray::AddData(int in_maxCountLimit, const SData *in_Data)
{
	if (m_Array && in_Data)
	{
		CurPos++;
		if (in_maxCountLimit >5000 && CurPos >= in_maxCountLimit)
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
		m_Array[CurPos].AbsSpeed = CSpParticle::Get3D_Pwr1(in_Data->Speed);
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


void CSDataArray::GetMinMaxTracePos(SPos& min_Pos, SPos& max_Pos)
{

	if (m_Array)
	{
		SData *pos = m_Array;
		for (int i = 0; i <= CurPos; i++, pos++)
		{
			if (min_Pos.X > pos->position.X)
				if (pos->position.X < -100000.0)
					min_Pos.X = -100000.0;
				else
				min_Pos.X = pos->position.X;

			//else
			if (max_Pos.X < pos->position.X) 
				if (pos->position.X > 100000.0)
					max_Pos.X = 100000.0;
				else
					max_Pos.X = pos->position.X;

			if (min_Pos.Y > pos->position.Y) 
				min_Pos.Y = pos->position.Y;
			//else
			if (max_Pos.Y < pos->position.Y)
				max_Pos.Y = pos->position.Y;

			if (min_Pos.Z > pos->position.Z)
				min_Pos.Z = pos->position.Z;
			//else
			if (max_Pos.Z < pos->position.Z)
				max_Pos.Z = pos->position.Z;
		}
	}
}