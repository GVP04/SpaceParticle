#pragma once
#include "CSpParticle.h"

#define DA_SHRINK_FLAG_NONE		0x0000
#define DA_SHRINK_FLAG_FREE		0x0001
#define DA_MAX_ITEMS			0x200000

#define DA_ADD_FLAG_ADD_AUTO				0x00000000
#define DA_ADD_FLAG_ADD_FORCE				0x00000001
#define DA_ADD_FLAG_UPDATE					0x00000002
#define DA_ADD_FLAG_SQUISE					0x00000004
#define DA_ADD_FLAG_CORRECT_ON_SQUISE		0x00000010

#define DA_ADD_RESULT_MOVECOUNT				0x00000010
#define DA_ADD_LEVEL_MOVECOUNT				0x00000004

#define DA_ADD_RESULT_NONE					0x00000000
#define DA_ADD_RESULT_ADDED					0x00010000
#define DA_ADD_RESULT_UPDATED				0x00020000
#define DA_ADD_RESULT_SQUISED				0x00040000
#define DA_ADD_RESULT_PULLOUT				0x00080000
#define DA_ADD_RESULT_CORRECTED_SQUISE		0x00100000


using SquizeFlag = int;
using T_TimeCompare = double;

SquizeFlag DA_ADD_FlagFromResult(SquizeFlag in_Flags);

template <typename T_Data>
struct QuickData
{
//		friend class SPTraceArray;
//	private:
	T_Data* Cur;
	T_Data* Prev;
	T_Data* PrevPrev;
	T_Data PulledOut;
	T_Data Temp;
//	public:
	QuickData()		{	RESET();	};
	void RESET()	{	Cur = Prev = PrevPrev = &Temp;	};
	void COPYCURTOPREV() { *Prev = *Cur; }
	void SHIFT() { PulledOut = *PrevPrev; PrevPrev = Prev; Prev = Cur; }
};

template <typename T_Data>
class SPTraceArray
{
public:

protected:
	T_Data* m_Array;
	T_Data UndefData;
	int CurPos;
	int CurPos_Real;
	int MaxItems;

	QuickData<T_Data> m_QuickData;
	QuickData<T_Data>* GetQuickDataPtr() {	return &m_QuickData;}

	void Init(bool in_bClear = false) {
		if (m_Array == nullptr)
			m_Array = new T_Data[MaxItems + 10];
		Clear(in_bClear);
	};

	T_Data* GetDataByIdUnsafe(int in_DataId) const
	{
		int id = in_DataId - CurPos + CurPos_Real;
		if (id < 0)
			id += CurPos + 1;
		return m_Array + id;
	};

	int GetRealIdx(int in_DataId) const
	{
		int id = in_DataId - CurPos + CurPos_Real;
		if (id < 0)
			id += CurPos + 1;
		return id;
	};


public:
	QuickData<T_Data>* const GetQuickDataPtr_C() { return &m_QuickData; }

	//const QuickData* GetQuickDataPtr_C() { return &m_QuickData; }

	SPTraceArray(int in_MaxItems) 
	{
		m_QuickData.RESET();

		m_Array = nullptr;
		if (in_MaxItems > DA_MAX_ITEMS || in_MaxItems < 10)
			in_MaxItems = DA_MAX_ITEMS;

		MaxItems = in_MaxItems;
		Init();
	};

	~SPTraceArray()
	{
		CurPos_Real = CurPos = -1;
		MaxItems = 0;
		if (m_Array) delete[] m_Array;
		m_Array = nullptr;
	};

	void Clear(int in_bClear) 
	{
		if (m_Array == NULL) Init();

		switch (in_bClear)
		{
		case SP_CLEAR_HARD:
			m_QuickData.RESET();
			CurPos_Real = CurPos = -1;
			std::memset(m_Array, 0, MaxItems * sizeof(T_Data));
			break;

		case SP_CLEAR_SOFT:
		default:
			m_QuickData.RESET();
			CurPos_Real = CurPos = 0;
			std::memset(m_Array, 0, sizeof(T_Data));
			m_QuickData.Cur = m_Array;

			break;
		}
	};

	T_TimeCompare GetCurTime() { return m_QuickData.Cur->TimePoint; }

	virtual int AddData(const T_Data& in_Data)
	{
		int DoAdd = DA_ADD_RESULT_ADDED;

		m_QuickData.SHIFT();

		if (CurPos < MaxItems)
		{
			CurPos++;
			CurPos_Real = CurPos;
		}
		else
		{
			CurPos_Real++;
			if (CurPos_Real > CurPos)
				CurPos_Real = 0;
			m_QuickData.PulledOut = m_Array[CurPos_Real];
			DoAdd |= DA_ADD_RESULT_PULLOUT;
		}
		m_Array[CurPos_Real] = in_Data;
		m_QuickData.Cur = m_Array + CurPos_Real;
		m_QuickData.Cur->TAD_OnAdd(m_QuickData.Prev);

		return DoAdd;
	};

	int UpdateData(const T_Data& in_Data)
	{
		int DoAdd = DA_ADD_RESULT_NONE;

		if (CurPos >= 0)
		{
			*m_QuickData.Cur = in_Data;
			m_QuickData.Cur->TAD_OnUpdate(m_QuickData.Prev);

			DoAdd |= DA_ADD_RESULT_UPDATED;
		}
		else
			DoAdd = AddData(in_Data);

		return DoAdd;
	};

	inline T_Data* const GetPrevData() { return m_QuickData.Prev; }
	inline T_Data* const GetCurData() { return m_QuickData.Cur; }
	inline T_Data* const GetPrevPrevData() { return m_QuickData.PrevPrev; }
	inline const T_Data& GetPullOutData() { return m_QuickData.PulledOut; }

	int GetCurPos() { return CurPos; }
	int GetCountMax() { return MaxItems; }

	T_Data* GetDataById(int in_DataId)
	{
		T_Data* ret = nullptr;

		if (in_DataId >= 0 && in_DataId <= CurPos && m_Array)
		{
			int id = in_DataId - CurPos + CurPos_Real;
			if (id < 0)
				id += CurPos + 1;
			ret = m_Array + id;
		}

		return ret;
	}

	int GetLowIdxByTime(T_TimeCompare in_ViewTime, int StrtIdx) const
	{
		int ret = -1;
		if (CurPos_Real >= 0)
		{
			if (StrtIdx < 0) StrtIdx = 0;

			if (m_Array[CurPos_Real].TimePoint <= in_ViewTime) ret = CurPos;
			else if (m_Array[CurPos_Real == CurPos ? 0 : CurPos_Real + 1].TimePoint <= in_ViewTime)
			{
				if (StrtIdx >= CurPos) StrtIdx = CurPos;

				double DateAtStrtIdx = GetDataByIdUnsafe(StrtIdx)->TimePoint;

				if (DateAtStrtIdx >= in_ViewTime)
				{
					if (DateAtStrtIdx > in_ViewTime)
					{
						StrtIdx--;
						while (StrtIdx >= 0 && GetDataByIdUnsafe(StrtIdx)->TimePoint > in_ViewTime)
							StrtIdx--;
					}
					ret = StrtIdx;
				}
				else if (GetDataByIdUnsafe(StrtIdx + 1)->TimePoint > in_ViewTime)
				{
					ret = StrtIdx;
				}
				else if (GetDataByIdUnsafe(StrtIdx + 1)->TimePoint == in_ViewTime)
				{
					ret = StrtIdx + 1;
				}
				else
				{
					int min = CurPos_Real - CurPos + StrtIdx + 1;
					int max = CurPos_Real;

					if (min < 0)
					{
						if (m_Array[0].TimePoint <= in_ViewTime) min = 0;
						else
						{
							min += CurPos + 1;
							max = CurPos;
						}
					}

					while (min < max)
					{
						ret = min + ((max - min + 1) >> 1);
						if (m_Array[ret].TimePoint > in_ViewTime)
							max = ret - 1;
						else
							min = ret;
					}

					if (min <= CurPos_Real)
						ret = min + (CurPos - CurPos_Real);
					else
						ret = min - CurPos_Real - 1;
				}
			}
		}
		return ret;
	}

	int GetLowIdxByTime(T_TimeCompare in_ViewTime) const
	{
		int ret = -1;
		if (CurPos_Real >= 0)
		{
			if (in_ViewTime >= m_Array[CurPos_Real].TimePoint) ret = CurPos;
			else if (m_Array[CurPos_Real == CurPos ? 0 : CurPos_Real + 1].TimePoint <= in_ViewTime)
			{
				int min = CurPos_Real - CurPos;
				int max = CurPos_Real;

				if (min < 0)
				{
					if (m_Array[0].TimePoint <= in_ViewTime) min = 0;
					else
					{
						min += CurPos + 1;
						max = CurPos;
					}
				}

				while (min < max)
				{
					ret = min + ((max - min + 1) >> 1);
					if (m_Array[ret].TimePoint > in_ViewTime)
						max = ret - 1;
					else
						min = ret;
				}

				if (min <= CurPos_Real)
					ret = min + (CurPos - CurPos_Real);
				else
					ret = min - CurPos_Real - 1;
			}
		}
		return ret;
	}


	void GetMinMaxTracePos(SPos& min_Pos, SPos& max_Pos, int idx_Start, int idx_End) const
	{
		if (idx_Start < 0 || idx_Start > CurPos) idx_Start = 0;
		if (idx_End < 0 || idx_End < idx_Start || idx_End > CurPos)  idx_End = CurPos;

		int delta = CurPos - CurPos_Real;
		idx_Start -= delta;
		idx_End -= delta;

		int strt1 = 1, strt2 = 1, end1 = 0, end2 = 0;


		if (idx_Start < 0 && idx_End < 0)
		{
			strt1 = idx_Start + CurPos + 1;
			end1 = idx_End + CurPos;
		}
		else
			if (idx_Start < 0)
			{
				strt1 = idx_Start + CurPos + 1;
				end1 = CurPos;

				strt2 = 0;
				end2 = idx_End;
			}
			else
			{
				strt1 = idx_Start;
				end1 = idx_End;
			}

		SData* pos = m_Array + strt1;

		for (int i = strt1; i <= end1; i++, pos++)
		{
			if (min_Pos.X > pos->Position.X)
				min_Pos.X = pos->Position.X;
			if (max_Pos.X < pos->Position.X)
				max_Pos.X = pos->Position.X;

			if (min_Pos.Y > pos->Position.Y)
				min_Pos.Y = pos->Position.Y;
			if (max_Pos.Y < pos->Position.Y)
				max_Pos.Y = pos->Position.Y;

			if (min_Pos.Z > pos->Position.Z)
				min_Pos.Z = pos->Position.Z;
			if (max_Pos.Z < pos->Position.Z)
				max_Pos.Z = pos->Position.Z;
		}

		pos = m_Array + strt2;

		for (int i = strt2; i <= end2; i++, pos++)
		{
			if (min_Pos.X > pos->Position.X)
				min_Pos.X = pos->Position.X;
			if (max_Pos.X < pos->Position.X)
				max_Pos.X = pos->Position.X;

			if (min_Pos.Y > pos->Position.Y)
				min_Pos.Y = pos->Position.Y;
			if (max_Pos.Y < pos->Position.Y)
				max_Pos.Y = pos->Position.Y;

			if (min_Pos.Z > pos->Position.Z)
				min_Pos.Z = pos->Position.Z;
			if (max_Pos.Z < pos->Position.Z)
				max_Pos.Z = pos->Position.Z;
		}
	}
};

// Инициализация статического члена SPTraceArray
//template <typename T_Data>
//const T_Data SPTraceArray<T_Data>::StaticTmp = T_Data();

template <typename T_Data, typename T_TimeCompare>
class SPTraceArrayDynamic : public SPTraceArray<T_Data>
{
private:
	T_TimeCompare SqueezeStep;
	SquizeFlag CompareFlag;

public:

	SPTraceArrayDynamic(int in_MaxItems, T_TimeCompare in_SqueezeStep) : SPTraceArray(in_MaxItems), SqueezeStep(in_SqueezeStep)
	{
		CompareFlag = 0;
	}

	void SetCompareFlag(SquizeFlag in_CompareFlag) { CompareFlag = in_CompareFlag; }

	void SetSqueezeStep(T_TimeCompare in_SqueezeStep) {	SqueezeStep = in_SqueezeStep;}
	T_TimeCompare GetSqueezeStep() {return SqueezeStep;	}

	int AddDataDynamic(const T_Data& in_Data, int in_Flags)
	{
		int DoAdd = DA_ADD_RESULT_NONE;

		if ((in_Flags & DA_ADD_FLAG_UPDATE))
			DoAdd = UpdateData(in_Data);
		else if ((in_Flags & DA_ADD_FLAG_ADD_FORCE) || GetCurPos() < 2)
			DoAdd = AddData(in_Data);
		else
		{
			if ((in_Flags & DA_ADD_FLAG_SQUISE) || GetPrevData()->TAD_IsToSquize(GetPrevPrevData(), SqueezeStep, CompareFlag))
			{
				m_QuickData.COPYCURTOPREV();
				DoAdd = UpdateData(in_Data);
				T_Data::TAD_CorrectData(*m_QuickData.PrevPrev, *m_QuickData.Prev);
				DoAdd |= DA_ADD_RESULT_SQUISED;
			}
			else
				DoAdd = AddData(in_Data);
		}
		return DoAdd;
	}


};

template <typename T_Data>
class SPTraceArrayEternal : public SPTraceArray<T_Data>
{
private:

public:

	SPTraceArrayEternal(int in_MaxItems) : SPTraceArray(in_MaxItems)
	{
		;
	}


	T_TimeCompare GetCurTime() { return GetCurData()->TimePoint; }
	//Добавляет элемент в массив. Если  массив заполнен, то сжимает его с шагом времени по элементам *2 без исключений.
	virtual int AddData(const T_Data& in_Data) override
	{
		int DoAdd = 0;

		if (CurPos < MaxItems)
		{
			m_QuickData.SHIFT();
			CurPos++;
			CurPos_Real = CurPos;
			m_Array[CurPos_Real] = in_Data;
			m_QuickData.Cur = m_Array + CurPos_Real;
			m_QuickData.Cur->TAD_OnAdd(m_QuickData.Prev);
		}
		else
		{
			double dTime = in_Data.TimePoint - m_Array[0].TimePoint;
			double Step = dTime /(double) CurPos; Step += Step;
			int Src = 1,Dst = 0;

			while (Src <= CurPos)
			{
				while (Src < CurPos && Step > m_Array[Src].TimePoint - m_Array[Dst].TimePoint)
					Src++;

				if (++Dst < Src)
				{
					m_Array[Dst] = m_Array[Src++];
					m_Array[Dst].TAD_OnUpdate(m_Array + Dst - 1);
				}
			}

			CurPos = CurPos_Real = Dst;
			DoAdd = AddData(in_Data);
		}
		return DoAdd;
	};
};


#define	TA_TYPE_MAIN		0x00000001
#define	TA_TYPE_SECONDATY	0x00000002
#define	TA_TYPE_ETERNAL		0x00000004


template <typename T_Data, typename T_TimeCompare>
class SPTraceArrayDynEx
{
private:
	SPTraceArrayDynamic<T_Data, T_TimeCompare>* m_SDArrayMain;
	SPTraceArrayDynamic<T_Data, T_TimeCompare>* m_SDArraySecond;
	SPTraceArrayEternal<T_Data>* m_SDArrayEternal;

	SPTime* m_pTime;
	QuickData<T_Data>* m_pQuickData;

	int CurPos;
	int IdDeltaMainSecond;
	int IdDeltaSecondEternal;

	int MaxItemsMain;
	int MaxItemsSecond;
	int MaxItemsEternal;


	int CalcCurPos()
	{
		IdDeltaSecondEternal = m_SDArrayEternal->GetCurPos() + 1;
		IdDeltaMainSecond = m_SDArraySecond->GetCurPos() + 1 + IdDeltaSecondEternal;
		return CurPos = m_SDArrayMain->GetCurPos() + IdDeltaMainSecond;
	}

	void SetMaxCountValue(int in_MaxItems, double in_ArvivePart = 0.0625)
	{
		if (in_MaxItems > DA_MAX_ITEMS || in_MaxItems < 100) in_MaxItems = DA_MAX_ITEMS;

		if (in_ArvivePart > 0.95 || in_ArvivePart < 0.01) in_ArvivePart = 0.01;

		int ArchCount = (int)((double)in_MaxItems * in_ArvivePart);
		if (ArchCount < 8) ArchCount = 8;
		MaxItemsMain = in_MaxItems - ArchCount;
		MaxItemsSecond = ArchCount;
		MaxItemsEternal = MaxItemsSecond / 4;
	};

	void SetMaxCountValue(int in_MaxItemsMain, int in_MaxItemsSecond)
	{
		if (in_MaxItemsMain > DA_MAX_ITEMS || in_MaxItemsMain < 100) in_MaxItemsMain = DA_MAX_ITEMS;
		if (in_MaxItemsSecond > DA_MAX_ITEMS || in_MaxItemsSecond < 100) in_MaxItemsMain = DA_MAX_ITEMS;

		MaxItemsMain = in_MaxItemsMain;
		MaxItemsSecond = in_MaxItemsSecond;
		MaxItemsEternal = MaxItemsSecond / 4;
	};

	void Init(int in_bClear)
	{
		if (m_SDArrayMain) delete m_SDArrayMain;
		m_SDArrayMain = new SPTraceArrayDynamic<T_Data, T_TimeCompare>(MaxItemsMain, m_pTime->GetTraceMinStep());
		if (m_SDArraySecond) delete m_SDArraySecond;
		m_SDArraySecond = new SPTraceArrayDynamic<T_Data, T_TimeCompare>(MaxItemsSecond, m_pTime->GetHistoryMinStep());
		if (m_SDArrayEternal) delete m_SDArrayEternal;
		m_SDArrayEternal = new SPTraceArrayEternal<T_Data>(MaxItemsEternal);

		m_pQuickData = m_SDArrayMain->GetQuickDataPtr_C();

		Clear(in_bClear);
	}

public:
	int GetEternalCount() const { return m_SDArrayEternal->GetCurPos() + 1; }
	int GetSecondaryCount() const { return m_SDArraySecond->GetCurPos() + 1; }
	int GetMainCount() const { return m_SDArraySecond->GetCurPos() + 1; }
	int GetStartIdSecondary()const { return IdDeltaSecondEternal - 1; }
	int GetStartIdMain()const { return IdDeltaMainSecond - 1; }

	SPTraceArrayDynEx(int in_MaxItems, SPTime* in_pTime)
	{
		m_pTime = in_pTime;
		IdDeltaMainSecond = 0;
		IdDeltaSecondEternal = 0;

		m_SDArrayMain = nullptr;
		m_SDArraySecond = nullptr;
		m_SDArrayEternal = nullptr;
		m_pQuickData = nullptr;

		SetMaxCountValue(in_MaxItems);
		Init(SP_CLEAR_HARD);
	}

	SPTraceArrayDynEx(int in_MaxItemsMain, int in_MaxItemsSecond, SPTime* in_pTime)
	{
		m_pTime = in_pTime;
		IdDeltaMainSecond = 0;
		IdDeltaSecondEternal = 0;

		m_SDArrayMain = nullptr;
		m_SDArraySecond = nullptr;
		m_SDArrayEternal = nullptr;
		m_pQuickData = nullptr;

		SetMaxCountValue(in_MaxItemsMain, in_MaxItemsSecond);
		Init(SP_CLEAR_HARD);
	}	
	
	~SPTraceArrayDynEx()
	{
		MaxItemsMain = 0;
		MaxItemsSecond = 0;
		MaxItemsEternal = 0;
		m_pQuickData = nullptr;

		if (m_SDArrayMain) delete m_SDArrayMain;
		m_SDArrayMain = nullptr;
		if (m_SDArraySecond) delete m_SDArraySecond;
		m_SDArraySecond = nullptr;
		if (m_SDArrayEternal) delete m_SDArrayEternal;
		m_SDArrayEternal = nullptr;
	}

	int SetMaxItemsCount(int in_nMaxItems)
	{
		SetMaxCountValue(in_nMaxItems);
		Init(SP_CLEAR_SOFT);

		return MaxItemsMain + MaxItemsSecond + MaxItemsEternal;
	}


	void Clear(int in_bClear)
	{
		if (!m_SDArrayMain || !m_SDArraySecond || !m_SDArrayEternal)
			Init(in_bClear);
		else
		{
			m_SDArrayMain->Clear(in_bClear);
			m_SDArraySecond->Clear(SP_CLEAR_HARD);
			m_SDArrayEternal->Clear(SP_CLEAR_HARD);
		}
		CalcCurPos();
	}

	int AddData(const T_Data& in_Data, int in_Flags)
	{
		int ret = m_SDArrayMain->AddDataDynamic(in_Data, in_Flags);

		if ((ret & DA_ADD_RESULT_PULLOUT))
		{
			int ret2 = m_SDArraySecond->AddDataDynamic(m_SDArrayMain->GetPullOutData(), in_Flags >> DA_ADD_LEVEL_MOVECOUNT);

			if ((ret2 & DA_ADD_RESULT_PULLOUT))
				m_SDArrayEternal->AddData(m_SDArraySecond->GetPullOutData());

			ret2 <<= DA_ADD_LEVEL_MOVECOUNT;
			ret |= ret2;
		}

		CalcCurPos();

		return ret;
	}

	
	inline T_Data* const GetCurData() { return m_SDArrayMain->GetCurData(); }
	inline T_Data* const GetPrevData() { return m_SDArrayMain->GetPrevData(); }
	int GetCurPos() { return CurPos; }
	T_TimeCompare GetCurTime() { return m_SDArrayMain->GetCurTime(); }
	int GetCountMax() { return m_SDArrayMain->GetCountMax() + m_SDArraySecond->GetCountMax() + m_SDArrayEternal->GetCountMax(); }

	T_Data& operator[](int in_DataId)
	{
		return *GetDataById(in_DataId);
	}

	T_Data* GetDataById(int in_DataId)
	{
		T_Data* ret = nullptr;
		if (in_DataId >= 0)
		{
			if (in_DataId < IdDeltaSecondEternal)
				ret = m_SDArrayEternal->GetDataById(in_DataId);
			else
				if (in_DataId < IdDeltaMainSecond)
					ret = m_SDArraySecond->GetDataById(in_DataId - IdDeltaSecondEternal);
			else
				ret = m_SDArrayMain->GetDataById(in_DataId - IdDeltaMainSecond);
		}

		return ret;
	}

	int GetLowIdxByTime(T_TimeCompare in_ViewTime, int StrtIdx)
	{
		int ret = -1;

		if (CurPos >= 0)
		{
			if ((ret = m_SDArrayMain->GetLowIdxByTime(in_ViewTime, StrtIdx - IdDeltaMainSecond)) >= 0)
				ret += IdDeltaMainSecond;
			else if ((ret = m_SDArraySecond->GetLowIdxByTime(in_ViewTime, StrtIdx - IdDeltaSecondEternal)) >= 0)
				ret += IdDeltaSecondEternal;
			else 
				ret = m_SDArrayEternal->GetLowIdxByTime(in_ViewTime, StrtIdx);
		}

		return ret;
	}

	int GetLowIdxByTime(T_TimeCompare in_ViewTime)
	{
		int ret = -1;

		if (CurPos >= 0)
		{
			if ((ret = m_SDArrayMain->GetLowIdxByTime(in_ViewTime)) >= 0)
				ret += IdDeltaMainSecond;
			else if ((ret = m_SDArraySecond->GetLowIdxByTime(in_ViewTime)) >= 0)
				ret += IdDeltaSecondEternal;
			else
				ret = m_SDArrayEternal->GetLowIdxByTime(in_ViewTime);
		}

		return ret;
	}

	void GetMinMaxTracePos(SPos& min_Pos, SPos& max_Pos, int idx_Start, int idx_End)
	{
		if (CurPos >= 0)
		{
			if (idx_End < 0) idx_End = CurPos;
			if (idx_Start < 0) idx_Start = 0;

			if (m_SDArrayEternal->GetCurPos() >= idx_Start)
				m_SDArrayEternal->GetMinMaxTracePos(min_Pos, max_Pos, idx_Start, idx_End);

			if (idx_Start < IdDeltaMainSecond && idx_End > IdDeltaSecondEternal)
				m_SDArraySecond->GetMinMaxTracePos(min_Pos, max_Pos, idx_Start - IdDeltaSecondEternal, idx_End - IdDeltaSecondEternal);

			if (idx_End > IdDeltaMainSecond)
				m_SDArrayMain->GetMinMaxTracePos(min_Pos, max_Pos, idx_Start - IdDeltaMainSecond, idx_End - IdDeltaMainSecond);

		}
	}

};


