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
	if (m_Array) delete[] m_Array;

	m_Array = new SData[MaxItems];
	Reset();
}

void CSDataArray::Reset()
{
	CurPos = -1;

	if (!m_Array) Init();
	else ZeroMemory(m_Array, sizeof(SData));
}

int CSDataArray::AddData(SData *in_Data)
{
	if (m_Array && in_Data)
	{
		CurPos++;
		if (CurPos >= MaxItems) CurPos = 0;

		m_Array[CurPos] = *in_Data;
	}
	return CurPos;
}


SData *CSDataArray::GetCurData()
{
	return (m_Array && CurPos >= 0) ? m_Array + CurPos : &CSDataArray::EmptyData;
}
