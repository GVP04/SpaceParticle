#pragma once
#include "CSpParticle.h"
#include "CSDataArray.h"
#include "CommonFunc.h"

#include <atomic>   
#include <thread>


//#define REPORTER_THREAD_CALC(X) X	
#define REPORTER_THREAD_CALC(X)

#define SPG_MAX_COUNT	0xFFFF
#define SPG_MAX_MAXLEVEL_STEP	0x4

#define SPG_TEMPSTRING_LEN	0x8000


#define M_STATE_OFF			0x00000000

#define M_STATE_READY		0x00000001
#define M_STATE_CALC		0x00000002
#define M_STATE_PAUSE		0x00000004
#define M_STATE_BREAK		0x00000008


#define M_STATE_DO_NEXT		0x00000100
#define M_STATE_DO_REPORT	0x00000200

#define M_STATE_CHANGED		0x00001000

#define M_STATE_FULL		0x0000FFFF


#define M_CMD_MAXCOUNT		0x1000

extern int mRGB[16];
extern int mRGBDark[16];

UINT DoCommandThread(LPVOID pParam);

#define M_INNER_STATE_MASK		0x0000000F
#define M_INNER_STATE_NULL		0x00000000
#define M_INNER_STATE_STARTING	0x00000001
#define M_INNER_STATE_READY		0x00000002
#define M_INNER_STATE_RUNING	0x00000003
#define M_INNER_STATE_BREAK_ON	0x00000004
#define M_INNER_STATE_STOPPED	0x00000005
#define M_INNER_STATE_END_IS	0x00000006


#define SGROUP_ALL				0x00FF
#define SGROUP_TRACE			0x0001
#define SGROUP_STEP				0x0002
#define SGROUP_CALC_PARAM		0x0004
#define SGROUP_MISC				0x0008
#define SGROUP_LIMITS			0x0010


void PaintTraceToCDC(CDC* DCMem, SP_Calc* mData, int in_DrawFlags, SPos& delta);


struct GroupSummaryInfo
{
	SPos Position; //позиция центра
	SPos Speed_3D; //суммарная скорость 3D
	SPos Acceleration_3D; //суммарное ускорение 3D
	double Velocity; //суммарная скорость
	double Acceleration; //суммарное ускорение
	double TimePoint;
	double VP_TimePoint_Min;
	double VP_TimePoint_Max;
	int Count;
	int TraceCnt_Max;
	int TraceCnt_Min;
	int VP_Id_Min;
	int VP_Id_Max;
	int AlignmentRes1;



	void CLEAR()
	{
		Position = { 0 };
		Speed_3D = { 0 };
		Velocity = 0.0;
		Acceleration_3D = { 0 };
		Acceleration = 0.0;

		Count = 0;
		TraceCnt_Max = -1;
		TraceCnt_Min = 0x40000000;
		TimePoint = -9.0e99;
	};

	GroupSummaryInfo()
	{
		CLEAR();
	}

	void CALC()
	{
		if (Count > 0)
		{
			Position.DIV(Count);
			Speed_3D.DIV(Count);
			Velocity = Speed_3D.LEN();

			Acceleration_3D.DIV(Count);
			Acceleration = Acceleration_3D.LEN();
			ASSERT(fabs(Acceleration_3D.X) < 1.0e80);

		}
	};


	void SET(const SData* inData, int in_IdTrace)
	{
		if (inData)
		{
			CLEAR();
			ADD(inData, in_IdTrace);
		}
	};

	void ADD(const SData* inData, int in_IdTrace)
	{
		if (inData)
		{
			Position.ADD(inData->Position);
			Speed_3D.ADD(inData->Velocity);
			Velocity += inData->AbsSpeed;
			Acceleration_3D.ADD(inData->Accel_2_3D);
			ASSERT(fabs(Acceleration_3D.X) < 1.0e80);

			//Acceleration;
			Count++;
			TimePoint = inData->TimePoint;

			if (in_IdTrace > TraceCnt_Max)  TraceCnt_Max = in_IdTrace;
			if (in_IdTrace < TraceCnt_Min)  TraceCnt_Min = in_IdTrace;
		}
	};

	void ADD(const GroupSummaryInfo& inInfo)
	{
		Position.ADD(inInfo.Position);
		Speed_3D.ADD(inInfo.Speed_3D);
		Velocity += inInfo.Velocity;
		Acceleration_3D.ADD(inInfo.Acceleration_3D);
		Acceleration += inInfo.Acceleration;
		Count += inInfo.Count;
		ASSERT(fabs(Acceleration_3D.X) < 1.0e80);

		if (inInfo.TraceCnt_Max > TraceCnt_Max)  TraceCnt_Max = inInfo.TraceCnt_Max;
		if (inInfo.TraceCnt_Min < TraceCnt_Min)  TraceCnt_Min = inInfo.TraceCnt_Min;

		//TimePoint;
	};


	int PRINT(char* io_Str, int in_StrLen, int in_ValueDelim = '\t', int in_SubValueDelim = '\t', const char* in_Format = "%g")
	{
		int ret = 0;
		if (io_Str)
		{
			if (in_Format && *in_Format)
			{
				int nOutput;
				if (in_StrLen - ret > 0)
				{
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "GroupSummaryInfo");
					if (in_StrLen - (ret += nOutput) > 0)		io_Str[ret++] = in_ValueDelim;
				}

				if (in_StrLen - ret > 0)
				{
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "Count");
					if (in_StrLen - (ret += nOutput) > 0)		io_Str[ret++] = in_ValueDelim;
				}
				if (in_StrLen - ret > 0)
				{
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "%d", Count);
					if (in_StrLen - (ret += nOutput) > 0)		io_Str[ret++] = in_ValueDelim;
				}

				if (in_StrLen - ret > 0)
				{
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "TimePoint");
					if (in_StrLen - (ret += nOutput) > 0)		io_Str[ret++] = in_ValueDelim;
				}
				if (in_StrLen - ret > 0)
				{
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, in_Format, TimePoint);
					if (in_StrLen - (ret += nOutput) > 0)		io_Str[ret++] = in_ValueDelim;
				}

				if (in_StrLen - ret > 0)
				{
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "Position");
					if (in_StrLen - (ret += nOutput) > 0)		io_Str[ret++] = in_ValueDelim;
				}
				if (in_StrLen - ret > 0)
				{
					nOutput = Position.PRINT(io_Str + ret, in_StrLen - ret, in_SubValueDelim, in_Format);
					if (in_StrLen - (ret += nOutput) > 0)		io_Str[ret++] = in_ValueDelim;
				}

				if (in_StrLen - ret > 0)
				{
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "Velocity");
					if (in_StrLen - (ret += nOutput) > 0)		io_Str[ret++] = in_ValueDelim;
				}
				if (in_StrLen - ret > 0)
				{
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, in_Format, Velocity);
					if (in_StrLen - (ret += nOutput) > 0)		io_Str[ret++] = in_ValueDelim;
				}

				if (in_StrLen - ret > 0)
				{
					nOutput = Speed_3D.PRINT(io_Str + ret, in_StrLen - ret, in_SubValueDelim, in_Format);
					if (in_StrLen - (ret += nOutput) > 0)		io_Str[ret++] = in_ValueDelim;
				}

				if (in_StrLen - ret > 0)
				{
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "Acceleration");
					if (in_StrLen - (ret += nOutput) > 0)		io_Str[ret++] = in_ValueDelim;
				}
				if (in_StrLen - ret > 0)
				{
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, in_Format, Acceleration);
					if (in_StrLen - (ret += nOutput) > 0)		io_Str[ret++] = in_ValueDelim;
				}

				if (in_StrLen - ret > 0)
				{
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "Acceleration_3D");
					if (in_StrLen - (ret += nOutput) > 0)		io_Str[ret++] = in_ValueDelim;
				}
				if (in_StrLen - ret > 0)
				{
					nOutput = Acceleration_3D.PRINT(io_Str + ret, in_StrLen - ret, in_SubValueDelim, in_Format);
					if (in_StrLen - (ret += nOutput) > 0)		io_Str[ret++] = in_ValueDelim;
				}

				if (in_StrLen - ret > 0)
				{
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "TraceCnt_Min");
					if (in_StrLen - (ret += nOutput) > 0)		io_Str[ret++] = in_ValueDelim;
				}
				if (in_StrLen - ret > 0)
				{
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, in_Format, TraceCnt_Min);
					if (in_StrLen - (ret += nOutput) > 0)		io_Str[ret++] = in_ValueDelim;
				}

				if (in_StrLen - ret > 0)
				{
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "TraceCnt_Max");
					if (in_StrLen - (ret += nOutput) > 0)		io_Str[ret++] = in_ValueDelim;
				}
				if (in_StrLen - ret > 0)
				{
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, in_Format, TraceCnt_Max);
					if (in_StrLen - (ret += nOutput) > 0)		io_Str[ret++] = in_ValueDelim;
				}

			}
			io_Str[ret] = 0;
		}

		return ret;
	};


};

struct GR_Thread;

#define GR_MAX_THREADS_COUNT	0x20

#define GR_THREAD_OFF			0x0000

#define GR_THREAD_READY			0x0001
#define GR_THREAD_STOP			0x0002

#define GR_THREAD_WAITING_STRT		0x0010
#define GR_THREAD_BUSY				0x0040
#define GR_THREAD_STOPPED			0x0080

#define GR_THREAD_ALL				0x00FF

#define GR_PS_NORM					0x00000000
#define GR_PS_MAXABSSPEED			0x00000001
#define GR_PS_MAXRELATIVESPEED		0x00000002
#define GR_PS_FAR_AWAY_PRESENT		0x00000004
#define GR_PS_COLLISION_PRESENT		0x000000F0
#define GR_PS_COLLISION_BESTUCK		0x00000010
#define GR_PS_COLLISION_DENSITY		0x00000020
#define GR_PS_COLLISION_DISTANCE	0x00000040
#define GR_PS_COLLISION_DISTANCE0	0x00000080

#define GR_PS_MAX_IS_PRESENT		0x00000F00
#define GR_PS_MAXABSSPEED_ON		0x00000100
#define GR_PS_MAXABSSPEED_OFF		0x00000200
#define GR_PS_MAXRELATIVESPEED_ON	0x00000400
#define GR_PS_MAXRELATIVESPEED_OFF	0x00000800

#define GR_PS_FAR_AWAY_PRESENT_ON	0x00001000
#define GR_PS_FAR_AWAY_PRESENT_OFF	0x00002000
#define GR_PS_POSITION_CHANGED		0x00004000
#define GR_PS_POSITION_LOW_PREC		0x00008000

#define GR_PS_RELATION_NEEDRECALC	0x10000000


#define GR_PS_RELATION_CURRENT		0x01000000
#define GR_PS_RELATION_NEXT			0x02000000
#define GR_PS_RELATION_PREV			0x04000000
#define GR_PS_RELATION_TEMP			0x08000000
#define GR_PS_RELATION_CORRECTED	0x00800000
#define GR_PS_RELATION_TYPEMASK		0x0F800000



struct BeStuckInfo
{
	int IdMaster;
	int State;
};

struct BeStuckList
{
	int nMasters;
	int IdParticle;
	BeStuckInfo* List;
	int* PosByIdx;
};




struct BSP_PosByIdx
{
private:
	int LineCount;
	int SizeFull;
	int SizeLine;
	int* PosByIdIndex;
	int* PBI_First;
	int* PBI_Second;
	int* IdxIdx;

	void Reset()
	{
		if (PosByIdIndex) delete[] PosByIdIndex;
		LineCount = 0;
		SizeFull = 0;
		SizeLine = 0;
		PosByIdIndex = PBI_First = PBI_Second = IdxIdx = nullptr;
	}

public:
	BSP_PosByIdx()
	{
		PosByIdIndex = nullptr;
		Reset();
	}
	~BSP_PosByIdx()
	{
		Reset();
	}

	void SetCount(int in_Count)
	{
		Reset();

		if (in_Count < 10) in_Count = 10;
		LineCount = in_Count;
		if (PosByIdIndex) delete[] PosByIdIndex;

		SizeLine = LineCount * sizeof(int);
		SizeFull = SizeLine * (LineCount + 2);

		PosByIdIndex = new int[SizeFull + 100];
		PBI_First = PosByIdIndex;
		IdxIdx = PosByIdIndex + LineCount;
		PBI_Second = IdxIdx + LineCount;

		for (int i = 0, j = LineCount + LineCount; i < LineCount; i++, j += LineCount)
			IdxIdx[i] = j;

		Clear();
	}

	void Clear()
	{
		memset(PosByIdIndex, 0xFF, SizeLine);
		for (int i = 0; i < LineCount; i++)
			memset(PosByIdIndex + IdxIdx[i], 0xFF, SizeLine);
	}


};

struct BeStuckParticles
{
private:
	int Count;
	int nParticles;
	BeStuckList* List;
	int* PosByIdIndex;
	//BSP_PosByIdx Idx;
public:

	static int CalcPoolSize(int in_MaxCount)
	{
		return in_MaxCount * ( sizeof(BeStuckList) + SP_MAX_STUCK_MASTERS * sizeof(BeStuckInfo) + sizeof(int));
	}

	inline int GetCount() const { return nParticles; };

	const BeStuckList* GetBeStuckListByNum(int in_Num) const
	{
		return in_Num >= 0 && in_Num < nParticles ? List + in_Num : nullptr;
	};

	const BeStuckList* GetBeStuckListById(int in_Idx) const
	{
		return in_Idx >= Count || in_Idx < 0 || PosByIdIndex[in_Idx] < 0 ? nullptr : List + PosByIdIndex[in_Idx];
	};

	inline int GetNumById(int in_Idx) const
	{
		return in_Idx >= Count || in_Idx < 0 ? -1 : PosByIdIndex[in_Idx];
	};

	BeStuckParticles()
	{
		List = nullptr;
		PosByIdIndex = nullptr;
		RESET();
	};

	~BeStuckParticles()
	{
		RESET();
	};

	void RESET()
	{
		nParticles = Count = 0;
		if (List) delete[] List;
		List = nullptr;
		if (PosByIdIndex) delete[] PosByIdIndex;
		PosByIdIndex = nullptr;
	};

	int INIT(int in_Count)
	{
		RESET();
		if (in_Count < 2) in_Count = 10;
		Count = in_Count;

		List = (BeStuckList*) new BYTE[BeStuckParticles::CalcPoolSize(Count) + 100];
		PosByIdIndex = new int[Count + 100];

		clear();

		return Count;
	};

	void clear()
	{
		if (List)
		{
			nParticles = 0;
			List[0].List = (BeStuckInfo*)(&List[Count+1]);
			List[0].IdParticle = 0;
			List[0].nMasters = 0;
			List[0].List[0].IdMaster = 0;
			List[0].List[0].State = 0;
			memset(PosByIdIndex, 0xFF, Count * sizeof(int));
		}
	};

	int GetItemState(int in_IdParticle, int in_IdMaster)
	{
		int ret = 0;
		
		ASSERT(in_IdParticle < Count && in_IdMaster < Count && in_IdParticle >= 0 && in_IdMaster >= 0);

		int NumPtr = PosByIdIndex[in_IdParticle];

		if (NumPtr >= 0)
		{
			int NumMaster;

			for (NumMaster = 0; NumMaster < List[NumPtr].nMasters && List[NumPtr].List[NumMaster].IdMaster < in_IdMaster; NumMaster++);

			if (List[NumPtr].List[NumMaster].IdMaster == in_IdMaster)
				ret = List[NumPtr].List[NumMaster].State;
		}

		return ret;
	}

	int AddItem(int in_IdParticle, int in_IdMaster, int in_State)
	{
		int ret = -1;

		if (in_IdParticle < Count && in_IdMaster < Count)
		{
			BeStuckList* pItem;
			if (nParticles == 0)
			{
				pItem = List;
				nParticles++;
				pItem->nMasters = 0;
				pItem->IdParticle = in_IdParticle;
				PosByIdIndex[in_IdParticle] = 0;
			}
			else if (List[nParticles - 1].IdParticle == in_IdParticle)
			{
				pItem = List + nParticles - 1;
			}
			else
			{
				PosByIdIndex[in_IdParticle] = nParticles;
				pItem = List + nParticles;
				nParticles++;
				pItem->nMasters = 0;

				BeStuckList* pItemPrev = pItem - 1;
				pItem->List = pItemPrev->List + pItemPrev->nMasters;
				pItem->IdParticle = in_IdParticle;
			}
			while (pItem->nMasters && pItem->List[pItem->nMasters - 1].IdMaster >= in_IdMaster)
				pItem->nMasters--;
			pItem->List[pItem->nMasters].IdMaster = in_IdMaster;
			pItem->List[pItem->nMasters].State = in_State;
			pItem->nMasters++;
			ret = nParticles;
		}

		return ret;
	};

	////////////	int InsertItem(int in_IdParticle, int in_IdMaster, int in_State)
	////////////	{
	////////////		int ret = -1;
	////////////		if (toAdd && in_IdParticle < Count && in_IdMaster < Count)
	////////////		{
	////////////			int IdxItem;
	////////////			if (toAdd->nParticles < 1) ret = AddItem(toAdd, in_IdParticle, in_IdMaster, in_State);
	////////////			else
	////////////			{
	////////////				for (IdxItem = 0; IdxItem < toAdd->nParticles && toAdd->List[IdxItem].IdParticle < in_IdParticle; IdxItem++);
	////////////				if (IdxItem >= toAdd->nParticles) ret = AddItem(toAdd, in_IdParticle, in_IdMaster, in_State);
	////////////				else
	////////////				{
	////////////					if (toAdd->List[IdxItem].IdParticle > in_IdParticle)
	////////////					{
	////////////						//ins new
	////////////					}
	///////////////////////////////надо ли???????????
	////////////				}
	////////////			}
	////////////		}
	////////////		return ret;
	////////////	};
};

struct BeStuckPool
{
private:
	BeStuckParticles* Curent;
	BeStuckParticles* Next;
	BeStuckParticles* Prev;
	BeStuckParticles* Temp; ///????????????????
	BeStuckParticles* Corrected;

public:

	BeStuckPool()
	{
		Corrected = Curent = Next = Prev = Temp = nullptr;
	};

	~BeStuckPool()
	{
		RESET();
	};

	inline BeStuckParticles* GetCurent() { return Curent; };
	inline BeStuckParticles* GetNext() { return Next; };
	inline BeStuckParticles* GetPrev() { return Prev; };
	inline BeStuckParticles* GetTemp() { return Temp; };
	inline BeStuckParticles* GetCorrected() { return Corrected; };

	BeStuckParticles* GetBeStuck(int in_Type)
	{
		BeStuckParticles* ret;
		switch ((in_Type & GR_PS_RELATION_TYPEMASK))
		{
		case GR_PS_RELATION_CURRENT:	ret = GetCurent();	break;
		case GR_PS_RELATION_NEXT:		ret = GetNext();	break;
		case GR_PS_RELATION_PREV:		ret = GetPrev();	break;
		case GR_PS_RELATION_TEMP:		ret = GetTemp();	break;
		case GR_PS_RELATION_CORRECTED:	ret = GetCorrected();	break;
		default:					ret = GetCurent();	break;
		}
		return ret;
	};

	void DoStep() {
		BeStuckParticles* Swap;
		Swap = Prev;
		Prev = Curent;
		Curent = Next;
		Next = Swap;
	};

	void RESET()
	{
		if (Prev) delete Prev;
		Prev = nullptr;
		if (Curent) delete Curent;
		Curent = nullptr;
		if (Next) delete Next;
		Next = nullptr;
		if (Temp) delete Temp;
		Temp = nullptr;
		if (Corrected) delete Corrected;
		Corrected = nullptr;
	};

	void clear()
	{
		if (Prev) Prev->clear();
		if (Curent) Curent->clear();
		if (Next) Next->clear();
		if (Temp) Temp->clear();
		if (Corrected) Corrected->clear();
	};

	BeStuckParticles* clear(int in_Type)
	{
		BeStuckParticles* toClear = GetBeStuck(in_Type);
		if (toClear) toClear->clear();

		return toClear;
	};

	int AddItem(int in_Type, int in_IdParticle, int in_IdMaster, int in_State)
	{
		int ret = -1;

		BeStuckParticles* toAdd = GetBeStuck(in_Type);
		if (toAdd) ret = toAdd->AddItem(in_IdParticle, in_IdMaster, in_State);

		return ret;
	};

	void INIT(int in_Count)
	{
		RESET();
		if (in_Count < 2) in_Count = 10;
		Curent = (BeStuckParticles*) new BeStuckParticles;
		Curent->INIT(in_Count);
		Next = (BeStuckParticles*) new BeStuckParticles;
		Next->INIT(in_Count);
		Prev = (BeStuckParticles*) new BeStuckParticles;
		Prev->INIT(in_Count);
		Temp = (BeStuckParticles*) new BeStuckParticles;
		Temp->INIT(in_Count);
		Corrected = (BeStuckParticles*) new BeStuckParticles;
		Corrected->INIT(in_Count);
		clear();
	};
};

struct Relations
{
private:
	SPRelation* Curent;
	SPRelation* Next;
	SPRelation* Prev;
	SPRelation* Temp;
	SPRelation* m_Array;
	int Count;
	int nItems;
	BeStuckPool BSP;

public:
	Relations()
	{
		nItems = Count = 0;
		Curent = Next = Prev = Temp = m_Array = nullptr;
	};

	~Relations()
	{
		RESET();
	};

	inline SPRelation* GetCurent() { return Curent; };
	inline SPRelation* GetNext() { return Next; };
	inline SPRelation* GetPrev() { return Prev; };
	inline SPRelation* GetTemp() { return Temp; };

	SPRelation* GetNext(const SPRelation* in_OfRelation) { return in_OfRelation == Curent ? Next : Curent; };
	SPRelation* GetPrev(const SPRelation* in_OfRelation) { return in_OfRelation == Curent ? Prev : Curent; };

	BeStuckParticles* FillNBeStuck(int in_Type, int IdxSrt, int IdxEnd)
	{
		if (IdxSrt > Count || IdxSrt < 0) IdxSrt = 0;
		if (IdxEnd > Count || IdxEnd < IdxSrt) IdxEnd = Count;

		SPRelation* Src = GetRelation(in_Type);
		BeStuckParticles* pBSP = BSP.clear(in_Type);

		for (int i = IdxSrt; i < IdxEnd; i++)
		{
			SPRelation* pos = Src + i + IdxSrt * Count;
			for (int j = IdxSrt; j < IdxEnd; j++, pos += Count)
				if (i != j && (pos->State & GR_PS_COLLISION_BESTUCK))
					pBSP->AddItem(i, j, pos->State);
		}

		return pBSP;
	};

	inline BeStuckParticles* GetBeStuck(int in_Type) { return BSP.GetBeStuck(in_Type); };
	inline BeStuckParticles* GetBeStuck_Current() { return BSP.GetCurent(); };
	inline BeStuckParticles* GetBeStuck_Next() { return BSP.GetNext(); };
	inline BeStuckParticles* GetBeStuck_Prev() { return BSP.GetPrev(); };
	inline BeStuckParticles* GetBeStuck_Temp() { return BSP.GetTemp(); };

	inline int GetRelationPos(int in_First, int in_Second) { return in_First + in_Second * Count; };


	SPRelation* GetRelation(int in_Type)
	{
		SPRelation* ret;
		switch ((in_Type & GR_PS_RELATION_TYPEMASK))
		{
		case GR_PS_RELATION_CURRENT:			ret = Curent;		break;
		case GR_PS_RELATION_NEXT:				ret = Next;			break;
		case GR_PS_RELATION_PREV:				ret = Prev;			break;
		case GR_PS_RELATION_TEMP:				ret = Temp;			break;
		default:							ret = Curent;		break;
		}
		return ret;
	};

	SPRelation* GetRelation(int in_Type, int in_First, int in_Second)
	{
		SPRelation* ret;
		switch ((in_Type & GR_PS_RELATION_TYPEMASK))
		{
		case GR_PS_RELATION_CURRENT:
			ret = GetCurentRelation(in_First, in_Second);
			break;
		case GR_PS_RELATION_NEXT:
			ret = GetNextRelation(in_First, in_Second);
			break;
		case GR_PS_RELATION_PREV:
			ret = GetPrevRelation(in_First, in_Second);
			break;
		case GR_PS_RELATION_TEMP:
			ret = GetTempRelation(in_First, in_Second);
			break;
		default:
			ret = GetCurentRelation(in_First, in_Second);
			break;
		}

		return ret;
	};

	inline SPRelation* GetCurentRelation(int in_First, int in_Second)
	{
		return Curent + in_First + in_Second * Count;
	};
	inline SPRelation* GetNextRelation(int in_First, int in_Second)
	{
		return Next + in_First + in_Second * Count;
	};
	inline SPRelation* GetPrevRelation(int in_First, int in_Second)
	{
		return Prev + in_First + in_Second * Count;
	};
	inline SPRelation* GetTempRelation(int in_First, int in_Second)
	{
		return Temp + in_First + in_Second * Count;
	};

	int GetIdByState(int in_Type, int in_State, int ParticleId, int StartId, int EndId)
	{
		int ret = -1;
		SPRelation* pos = GetRelation(in_Type, ParticleId, StartId);

		if (EndId > Count || EndId < 0) EndId = Count;

		for (int i = StartId; i < EndId && ret < 0; i++, pos += Count)
			if ((pos->State & in_State) && i != ParticleId)
				ret = i;

		return ret;
	};

	void ClearNextState(int in_IdxStart, int in_IdxEnd, int in_NotClearMask = 0)
	{
		if (in_IdxStart < 0) in_IdxStart = 0;
		if (in_IdxEnd < 0 || in_IdxEnd >= Count) in_IdxEnd = Count - 1;
		if (in_IdxEnd < in_IdxStart) in_IdxEnd = in_IdxStart;

		int RelationPos = in_IdxStart * Count;
		for (int i = in_IdxStart; i <= in_IdxEnd; i++, RelationPos += Count)
			for (int j = RelationPos + in_IdxStart, pos = RelationPos+ in_IdxEnd; j <= pos; j++)
				if (Next[j].State)
					Next[j].State &= in_NotClearMask;
	};

	void DoStep() {
		SPRelation* Swap;
		Swap = Prev;
		Prev = Curent;
		Curent = Next;
		Next = Swap;
		BSP.DoStep();
	};

	void RESET()
	{
		nItems = Count = 0;
		if (m_Array) delete[] m_Array;
		Curent = Next = Prev = Temp = m_Array = nullptr;
		BSP.RESET();
	};

	void clear()
	{
		BSP.clear();

		if (nItems > 0 && m_Array)
			ZeroMemory(m_Array, nItems * 4 * sizeof(SPRelation));
	};

	void clear_BeStuck(int in_Type)
	{
		BSP.clear(in_Type);
	};

	int INIT(int in_Count)
	{
		RESET();

		if (in_Count < 2) in_Count = 10;
		Count = in_Count;
		nItems = Count * Count;

		m_Array = new SPRelation[nItems * 4 + 100];

		Curent = m_Array;
		Next = Curent + nItems;
		Prev = Next + nItems;
		Temp = Prev + nItems;

		BSP.INIT(Count);

		clear();

		return Count;
	};

	int SET_COUNT(int in_newCount)
	{
		if (in_newCount < 3) in_newCount = 10;

		if (Count <= 0)
			INIT(in_newCount);
		else if (Count != in_newCount)
		{
			BSP.INIT(in_newCount);
			int nRelations = in_newCount * in_newCount;
			SPRelation* tmp_Relations = new SPRelation[nRelations * 4 + 100];
			ZeroMemory(tmp_Relations, nRelations * 4 * sizeof(SPRelation));

			SPRelation* tmpCurent = Curent;
			SPRelation* tmpNext = Next;
			SPRelation* tmpPrev = Prev;
			SPRelation* tmpTemp = Temp;

			Curent = tmp_Relations;
			Next = Curent + nRelations;
			Prev = Next + nRelations;
			Temp = Prev + nRelations;

			int nCpy = in_newCount;
			if (nCpy > Count) nCpy = Count;

			int byteCpy = nCpy * sizeof(SPRelation);
			int newDelta = in_newCount * sizeof(SPRelation);
			int oldDelta = Count * sizeof(SPRelation);

			for (int j = 0, idxNew = 0, idxOld = 0; j < nCpy; j++, idxNew += newDelta, idxOld += oldDelta)
			{
				CopyMemory(((BYTE*)Curent) + idxNew, tmpCurent + idxOld, byteCpy);
				CopyMemory(((BYTE*)Next) + idxNew, tmpNext + idxOld, byteCpy);
				CopyMemory(((BYTE*)Prev) + idxNew, tmpPrev + idxOld, byteCpy);
				CopyMemory(((BYTE*)Temp) + idxNew, tmpTemp + idxOld, byteCpy);
			}

			if (m_Array) delete[] m_Array;

			m_Array = tmp_Relations;
			Count = in_newCount;
			nItems = nRelations;
		}
		return Count;
	};

	int REMOVE_ITEM(int in_ItemId)
	{
		if (m_Array && in_ItemId >= 0 && in_ItemId + 1 < Count)
		{
			int LineInBytes = Count * sizeof(SPRelation);
			int byteCpy = (Count - in_ItemId - 1) * LineInBytes;
			int posCpyDst = in_ItemId * LineInBytes;
			int posCpySrc = posCpyDst + LineInBytes;

			CopyMemory(((BYTE*)Curent) + posCpyDst, (BYTE*)Curent + posCpySrc, byteCpy);
			CopyMemory(((BYTE*)Next) + posCpyDst, (BYTE*)Next + posCpySrc, byteCpy);
			CopyMemory(((BYTE*)Prev) + posCpyDst, (BYTE*)Prev + posCpySrc, byteCpy);
			CopyMemory(((BYTE*)Temp) + posCpyDst, (BYTE*)Temp + posCpySrc, byteCpy);

			posCpySrc = (Count - 1) * LineInBytes;
			ZeroMemory((BYTE*)Curent + posCpySrc, LineInBytes);
			ZeroMemory((BYTE*)Next + posCpySrc, LineInBytes);
			ZeroMemory((BYTE*)Prev + posCpySrc, LineInBytes);
			ZeroMemory((BYTE*)Temp + posCpySrc, LineInBytes);

			byteCpy = (Count - in_ItemId - 1) * sizeof(SPRelation);
			posCpyDst = in_ItemId * sizeof(SPRelation);
			posCpySrc = posCpyDst + sizeof(SPRelation);

			int ZeroItemId = LineInBytes - sizeof(SPRelation);
			for (int j = 0; j < Count; j++)
			{
				CopyMemory((BYTE*)Curent + posCpyDst, (BYTE*)Curent + posCpySrc, byteCpy);
				CopyMemory((BYTE*)Next + posCpyDst, (BYTE*)Next + posCpySrc, byteCpy);
				CopyMemory((BYTE*)Prev + posCpyDst, (BYTE*)Prev + posCpySrc, byteCpy);
				CopyMemory((BYTE*)Temp + posCpyDst, (BYTE*)Temp + posCpySrc, byteCpy);

				ZeroMemory((BYTE*)Curent + ZeroItemId, sizeof(SPRelation));
				ZeroMemory((BYTE*)Next + ZeroItemId, sizeof(SPRelation));
				ZeroMemory((BYTE*)Prev + ZeroItemId, sizeof(SPRelation));
				ZeroMemory((BYTE*)Temp + ZeroItemId, sizeof(SPRelation));

				ZeroItemId += LineInBytes;
				posCpyDst += LineInBytes;
				posCpySrc += LineInBytes;
			}
		}
		return Count;
	};

	void CLEAR_ITEM(int in_ItemId)
	{
		if (m_Array && in_ItemId >= 0 && in_ItemId < Count)
		{
			int LineInBytes = Count * sizeof(SPRelation);
			int posDst = in_ItemId * LineInBytes;

			ZeroMemory((BYTE*)Curent + posDst, LineInBytes);
			ZeroMemory((BYTE*)Next + posDst, LineInBytes);
			ZeroMemory((BYTE*)Prev + posDst, LineInBytes);
			ZeroMemory((BYTE*)Temp + posDst, LineInBytes);

			posDst = in_ItemId * sizeof(SPRelation);

			for (int j = 0; j < Count; j++)
			{
				ZeroMemory((BYTE*)Curent + posDst, sizeof(SPRelation));
				ZeroMemory((BYTE*)Next + posDst, sizeof(SPRelation));
				ZeroMemory((BYTE*)Prev + posDst, sizeof(SPRelation));
				ZeroMemory((BYTE*)Temp + posDst, sizeof(SPRelation));

				posDst += LineInBytes;
			}
		}
	};


};


struct CalceRelationParams
{
	SData DataParticle;
	CSpParticle* PtrParticle;
	CSpParticle* PtrViewParticle;
	double ViewTime;
	SPRelation* PrevRelations;
	SPRelation* NewRelations;
	BeStuckInfo BSI;
	int CalcType;
	int ThreadID;
	double out_KeyPoint;
	int Ret_Flags;
	int TimeType;
	int Particle_ID;
	int ViewParticle_ID;
	volatile INT64* pInterruptionFlag;
};


struct STracePosInfo
{
public:
	double TimePoint;
	int RelationsState_Cur;
	int RelationsS_Prev;
	int ParticlesState;
	int Reserve;

	bool TAD_IsToSquize(const STracePosInfo * PrevPrevData, double SqueezeStep, int CompareFlag) const
	{
		//CompareFlag не используется здесь. А так можно по флагу определить какие переменные сравнивать
		return !(ParticlesState & SP_STATE_ABS_CHANGE_LS) && !(RelationsState_Cur& SP_STATE_PHANTOM_CRITICAL_CHANGED) && TimePoint - PrevPrevData->TimePoint < SqueezeStep; //если разница больше 
	}

	static bool TAD_CorrectData(STracePosInfo & PrevPrevData, const STracePosInfo & PrevData, int CorrectionFlag = 0) 
	{
		return true; //если разница больше 
	}
};


#define STAT_TYPE_TIMEMASK		0xF0000000
#define STAT_TYPE_ALL			0x0FFFFFFF
#define STAT_TYPE_ALL_CURRENT 	0x0FFFFFFF
#define STAT_TYPE_ALL_PREV	 	0x1FFFFFFF
#define STAT_TYPE_ALL_NEXT	 	0x2FFFFFFF
#define STAT_TYPE_ALL_TEMP	 	0x4FFFFFFF
#define STAT_TYPE_CURRENT		0x00000000
#define STAT_TYPE_PREV			0x10000000
#define STAT_TYPE_NEXT			0x20000000
#define STAT_TYPE_TEMP			0x40000000
#define STAT_TYPE_ALL_DATA		0x0000000F
#define STAT_TYPE_MIN_DATA		0x00000001
#define STAT_TYPE_MAX_DATA		0x00000002
#define STAT_TYPE_ALL_RELATION	0x000000F0
#define STAT_TYPE_MIN_RELATION	0x00000010
#define STAT_TYPE_MAX_RELATION	0x00000020
#define STAT_TYPE_SETPARTICLE	0x01000000




struct ParticleCountByType
{
	friend struct ParticleCount;
private:
	int Antiparticle;
	int Particle;
	int All;

	int Add(int in_Type, int Count)
	{
		if ((in_Type & SP_TYPE_FORM_MASK) == SP_TYPE_POSITIVE)
			Particle += Count;
		if ((in_Type & SP_TYPE_FORM_MASK) == SP_TYPE_NEGATIVE)
			Antiparticle += Count;

		if (Particle < 0) Particle = 0;
		if (Antiparticle < 0) Antiparticle = 0;

		return All = Particle + Antiparticle;
	}

public:
	void INIT()
	{
		Antiparticle = Particle	= All = 0;
	}

	int GetCount() const { return All; }
	int GetCount(int in_Type) const
	{
		int ret = All;
		if ((in_Type & SP_TYPE_FORM_MASK) == SP_TYPE_POSITIVE)
			ret = Particle;
		if ((in_Type & SP_TYPE_FORM_MASK) == SP_TYPE_NEGATIVE)
			ret = Antiparticle;


		return ret; 
	}
};


struct ParticleCount
{
private:
	ParticleCountByType Initial;
	ParticleCountByType Current;

public:
	ParticleCount()
	{
		INIT();
	}

	const ParticleCountByType GetInitial() const {	return Initial;	}
	const ParticleCountByType GetCurrent() const { return Current; }
	inline int GetCount() const {	return Current.All;	}
	int GetCount(int in_Type) const {	return Current.GetCount(in_Type);	}

	void InitCurrent() { Current.INIT(); }
	void InitInitial() { Initial.INIT(); }

	int AddInitial(int in_Type, int Count = 1)
	{
		Initial.Add(in_Type, Count);
		return Initial.GetCount();
	}
	int AddCurrent(int in_Type, int Count = 1)
	{
		Current.Add(in_Type, Count);
		return Current.GetCount();
	}
	void INIT() 
	{
		Initial.INIT();
		Current.INIT();
	}

};





class CSP_Group
{
	friend class CSpParticle;
	friend class CDrawDlg;
	friend class CFindDlg;

	friend void PaintTraceToCDC(CDC* DCMem, SP_Calc* mData, int in_DrawFlags, SPos& delta);

	friend UINT DoCommandThread(LPVOID pParam);
	friend UINT DoCalc_1(LPVOID pParam);
public:

	volatile INT64 InterruptionFlag;
	volatile INT64 StateArray[GR_MAX_THREADS_COUNT];
	volatile INT64 Wait_After[GR_MAX_THREADS_COUNT];
	volatile INT64 Wait_Before[GR_MAX_THREADS_COUNT];

	volatile int m_CalcState;


private:
	ParticleCount PartCntr;

	int m_innerState;
	int m_ParticlesState;
	CSpParticle** m_Array;
#ifdef UseTemplates_in_H
	SPTraceArrayDynEx<STracePosInfo, double> *m_ParticleInfoArray;
#else
	SPTraceArrayDynEx *m_ParticleInfoArray;
#endif // UseTemplates_in_H

	Relations m_Relations;

	BeStuckParticles* pBS_Corrected;

	///////////
	static int AssertionVar;
	///////////

	GR_Thread** m_Thr_Calc;
	int m_ThreadsMax_Initial;
	int m_ThreadsMax;
	int m_ThreadsCur;

	double m_CountDivider;
	int m_maxCount;
	//double m_CurTime;
	char* m_name;
	char* m_tmpString;
	//void CalcDensity(SData* DataAtPoint, SData* ViewPointData);
	void CalcCountDevider();

	char** m_CmdArr;
	int m_DelayMS;
	void CheckThreads();
	void StopThreads();

public:
	void RepairPositions();
	void InitPosSpeedRandom(int in_InitialSeed, SP_CalcFindSet & in_Settings);

	REPORTER_THREAD_CALC(CMD_Param_File m_reporter;)

	CRITICAL_SECTION m_cs;
	CRITICAL_SECTION m_cs_Command;

	SP_Calc* m_pCalc;
	SP_CalcFind* m_pCalcFind;
	CSpParticle* GetParticleById(int in_IdParticle) { return m_Array && in_IdParticle >= 0 && in_IdParticle < GetCount() ? m_Array[in_IdParticle] : nullptr; };

public:
	int GetIdByMaxDistance(double in_RelDist);
	inline int GetCount() { return PartCntr.GetCount(); }
	int GetCountMax() { return m_maxCount; }
	int AddSpParticle(int in_Type, const SData& in_Ptr, SP_Calc* pCalc);
	int RemoveSpParticle(int in_Idx);
	int SetParticleData(int in_Idx, int in_flags, const SData& in_Ptr);
	//int RecalcDensityAt_(double atTime);
	void WriteState(FILE* in_Ptr, double in_StateTime);
	void WriteState(FILE* in_Ptr, int OutputType, int in_nPoints);
	double GetCurTime();
	void CollectStat(int in_StatType);
	void CollectStat(SP_CalcStat& out_Stat, int in_StatType);
	//int GetMinMaxData(SData* out_Min, SData* out_Max, SData_SD* out_MinSD, SData_SD* out_MaxSD, int in_NextFlag = 0);
	void Reset();
	void Clear();
	void DoDelay() { if (m_DelayMS > 0 && m_DelayMS < 10000) Sleep(m_DelayMS); };
	bool DoPause()
	{
		bool ret = (m_CalcState & M_STATE_PAUSE) != 0;
		if (ret) Sleep(100);
		return ret;
	};

	void ResumeThreadsAfter()
	{
		for (int i = 0; i < m_ThreadsCur; i++)
		{
			Wait_After[i]++;
			WakeByAddressSingle((volatile PVOID)&Wait_After[i]);
		}
	}

	int GetGroupSummaryInfo(GroupSummaryInfo* out_Info, int in_IdPos);
	int GetGroupSummaryInfo(GroupSummaryInfo* out_Info, double in_TimePoint);
	int GetGroupSummaryInfo(GroupSummaryInfo* out_Info);

	int ReadSettingsGroup(char* in_Settings, int in_SettingsGroup, int in_UseDefs = READPARAMS_USE_DEFS);

	int Cmd_Add(const char* in_Command);
	int Cmd_Execute(char* in_Command);
	int Cmd_Do(int in_Count = 0);
	int GetCalcState() { return m_CalcState; };
	int SetCalcState(int in_State)
	{
		if (in_State == M_STATE_OFF) m_CalcState = M_STATE_OFF;
		else
		{
			if ((in_State & M_STATE_READY))
				m_CalcState |= M_STATE_READY;

			if ((in_State & M_STATE_CALC))
				m_CalcState |= M_STATE_CALC;

			if ((in_State & M_STATE_BREAK))
				m_CalcState |= M_STATE_BREAK;

			if ((in_State & M_STATE_PAUSE))
				m_CalcState |= M_STATE_PAUSE;

			if ((in_State & M_STATE_DO_NEXT))
				m_CalcState |= M_STATE_DO_NEXT;

			if ((in_State & M_STATE_DO_REPORT))
				m_CalcState |= M_STATE_DO_REPORT;

			if ((in_State & M_STATE_CHANGED))
				m_CalcState |= M_STATE_CHANGED;
		}

		return m_CalcState;
	};
	int ClearCalcState(int in_State)
	{
		m_CalcState &= (0xFFFFFFFF ^ in_State);

		return m_CalcState;
	};


	int CheckHealth()
	{
		int ret = 0;
		int m_Count = PartCntr.GetCount();
		if (m_Array)
			for (int i = 0; i < m_Count; i++)
				if (m_Array[i]) ret++;

		return m_Count - ret;
	}

	int GetMaxTraceId();
	void GetMinMaxTracePos(SPos & min_Pos, SPos & max_Pos, int idx_Start, int idx_End);
	void SetViewPoint(double in_Percents, double in_Spread);
	void SetViewPointRange(int in_Start, int in_End);

	int CalcRelationsSingle(double in_NextTimePoint, SPRelation* const in_CalcRelations, SPRelation* const in_PrevRelations, int in_CalcType, int in_TimeType, double& out_KeyPoint);
	int CalcRelationsMulti(double in_NextTimePoint, SPRelation* const in_CalcRelations, SPRelation* const in_PrevRelations, int in_CalcType, double& out_KeyPoint);
	int CalcRelations(double in_NextTimePoint, SPRelation* const in_CalcRelations, SPRelation* const in_PrevRelations, int in_CalcType, double& out_KeyPoint);
	void CalcRelationsInit();
	void InitItems(int in_Flags);
	int CalcSingleRelation(CalceRelationParams & Params);
	int CalcDataByRelations(int in_Type);
	int CalcSingleDataByRelation(SData&  pCalcData, SPRelation& in_CalcRelations, SP_Calc* const pCalc);


	//нужна процедура создания обратной траектории для добавляемых частиц
	//int CSP_Group::CreateHistory(int in_nTimes)

	double CalcStepUpLimit();

	int NextStep();

	int CorrectBeStuckParticles(int in_Flags, double & KeyPoint);

	int CalcBeStuckNextPos(SData & out_Data, int in_IdParticle, double in_NextTime, double* const out_KeyPoint);


	int CheckTimes();

	CSP_Group();
	~CSP_Group();
	CSpParticle* GetItem(int in_Idx);
	void CheckBeforeCalc()
	{
		m_ThreadsMax = m_ThreadsMax_Initial;
		if (GetCount() < m_ThreadsMax) m_ThreadsMax = GetCount();
		CheckThreads();
		pBS_Corrected = m_Relations.GetBeStuck(GR_PS_RELATION_CORRECTED);
	};



	void PrepareForCalc(int in_CalcOrFind)
	{
		EnterCriticalSection(&m_cs);
		EnterCriticalSection(&m_cs_Command);

		ClearCalcState(M_STATE_READY);



		while (GetCount() > 0)
			RemoveSpParticle(GetCount() - 1);


		while (PartCntr.GetCount(SP_TYPE_POSITIVE) < PartCntr.GetInitial().GetCount(SP_TYPE_POSITIVE))
		{
			SData initData = { 0 };
			AddSpParticle(SP_TYPE_POSITIVE, initData, m_pCalc);
		}

		while (PartCntr.GetCount(SP_TYPE_NEGATIVE) < PartCntr.GetInitial().GetCount(SP_TYPE_NEGATIVE))
		{
			SData initData = { 0 };
			AddSpParticle(SP_TYPE_NEGATIVE, initData, m_pCalc);
		}

		InitItems(SP_CLEAR_SOFT);


		if (in_CalcOrFind == 0)
		{
			if (m_pCalc->sRandSeed_Calc < 0)
			{
				for (int i = 0; i < GetCount(); i++)
				{
					char tmps[500];
					SData initData = { 0 };
					sprintf_s(tmps, 500, "p%d_POS", i);
					ReadCalcParam(m_pCalc->CalcParams, tmps, initData.Position, 0.0);

					sprintf_s(tmps, 500, "p%d_SPEED", i);
					ReadCalcParam(m_pCalc->CalcParams, tmps, initData.Velocity, 0.0000001);
					initData.AbsSpeed = initData.Velocity.LEN();

					SetParticleData(i, SP_DATA_ALL, initData);
				}
				CalcRelationsInit();
			}
			else
			{
				InitPosSpeedRandom(m_pCalc->sRandSeed_Calc, m_pCalc->CalcFindSet);
				RepairPositions();
			}

			CalcDataByRelations(GR_PS_RELATION_CURRENT);
			CalcRelationsInit();//??????
			CalcDataByRelations(GR_PS_RELATION_CURRENT);

			m_pCalc->CalcCounter.InitLoop();
			m_pCalc->Time.FixStartTime();
		}
		else if (in_CalcOrFind == 1)
		{
			InitPosSpeedRandom(m_pCalcFind->sRandSeed == -1 ? m_pCalcFind->stat.Count : -1, m_pCalcFind->Settings);
			RepairPositions();

			CalcRelationsInit();//??????
			CalcDataByRelations(GR_PS_RELATION_CURRENT);
			CalcRelationsInit();//??????

			SetCalcState(M_STATE_READY);

			ZeroMemory(&m_pCalc->Stat, sizeof(SP_CalcStat));

			CollectStat(STAT_TYPE_ALL_CURRENT | STAT_TYPE_SETPARTICLE);

			m_pCalcFind->FindCounter.InitLoop();
			m_pCalc->Time.FixStartTimeCur();
		}

		CheckBeforeCalc();

		LeaveCriticalSection(&m_cs_Command);
		LeaveCriticalSection(&m_cs);
	};

};



UINT Do_RelationThread(LPVOID pParam);

struct GR_Thread
{
public:
	volatile int Result;

	volatile int IdParticleStart;
	volatile int IdParticleEnd;
	volatile int IdParticleCurrent;
	volatile int CalcType;
	volatile double KeyPointTime;

	volatile int StateBySlave;
	volatile DWORD ThreadID;

	volatile int* ThreadInnerFlagPtr;

	double TimePoint;

	int ID;

	CSP_Group* Group;
	volatile int StopFlag;

	SPRelation* pPrevRelations;
	SPRelation* pCalcRelations;

	GR_Thread(CSP_Group* in_pGroup, int in_ID)
	{
		ThreadInnerFlagPtr = nullptr;

		ID = in_ID;

		Group = in_pGroup;
		ThreadID = 0;

		StopFlag = 0;
		StateBySlave = 0;
		TimePoint = 0.0;

		Result = 0;
		CalcType = 0;
		KeyPointTime = 0.0;
		IdParticleCurrent = -1;
		IdParticleStart = -1;
		IdParticleEnd = -1;
		pPrevRelations = nullptr;
		pCalcRelations = nullptr;

		StartRelationThread();
	};

	~GR_Thread()
	{
		StopFlag = 1;
	};

	void StartRelationThread()
	{
		REPORTER_THREAD_CALC(Group->m_reporter.OUTPUT("GR_Thread AfxBeginThread Try", ID);)
		AfxBeginThread(&Do_RelationThread, (LPVOID)this);
		REPORTER_THREAD_CALC(Group->m_reporter.OUTPUT("GR_Thread AfxBeginThread Done", ID);)
	};

	void Slave_WaitToCalc()
	{
		StateBySlave |= GR_THREAD_WAITING_STRT;

		Group->StateArray[ID] = 0;
		REPORTER_THREAD_CALC(Group->m_reporter.OUTPUT("Slave\t Slave_WaitToCalc WaitOnAddress Try", CSP_Group::WaitFlagBefore, ID);)

		WaitOnAddress((volatile PVOID)&Group->Wait_Before[ID], (volatile PVOID)&Group->Wait_Before[ID], sizeof(INT64), INFINITE);

		Group->StateArray[ID] = 1;

		std::atomic_thread_fence(std::memory_order_acquire);

		REPORTER_THREAD_CALC(Group->m_reporter.OUTPUT("Slave\t Slave_WaitToCalc WaitOnAddress Done", ID);)
		StateBySlave &= (0xFFFFFFFF ^ GR_THREAD_WAITING_STRT);
	}

	void Slave_CalcCompleted()
	{
		IdParticleStart = -1;
		IdParticleEnd = -1;

		StateBySlave |= GR_THREAD_BUSY;
		StateBySlave &= (0xFFFFFFFF ^ GR_THREAD_BUSY);
		REPORTER_THREAD_CALC(Group->m_reporter.OUTPUT("Slave\t CalcCompleted WaitOnAddress Try", CSP_Group::WaitFlagAfter, ID);)

		Group->StateArray[ID] = 2;

		std::atomic_thread_fence(std::memory_order_release);
		WaitOnAddress((volatile PVOID)&Group->Wait_After[ID], (volatile PVOID)&Group->Wait_After[ID], sizeof(INT64), INFINITE);
		REPORTER_THREAD_CALC(Group->m_reporter.OUTPUT("Slave\t CalcCompleted WaitOnAddress Done", ID);)

	}

	void MasterSetCalc(double in_TimePoint, int in_IdParticleStart, int in_IdParticleEnd, int in_CalcType, SPRelation* in_pPrevRelations, SPRelation* in_pCalcRelations)
	{
		int MaxCount;
		if (Group && (MaxCount = Group->GetCount()) > 1)
		{
			CalcType = in_CalcType;
			Result = 0;
			KeyPointTime = TimePoint = in_TimePoint;

			pPrevRelations = in_pPrevRelations;
			pCalcRelations = in_pCalcRelations;

			if (in_IdParticleStart < 0) in_IdParticleStart = 0;
			if (in_IdParticleEnd < 0 || in_IdParticleEnd >= MaxCount) in_IdParticleEnd = MaxCount - 1;
			if (in_IdParticleStart > in_IdParticleEnd) in_IdParticleStart = in_IdParticleEnd;

			IdParticleEnd = in_IdParticleEnd;
			IdParticleStart = in_IdParticleStart;
			IdParticleCurrent = IdParticleStart - 1;
		}
	}

	bool NEXT() { return  IdParticleCurrent <= IdParticleEnd && (++IdParticleCurrent) <= IdParticleEnd; }
	int GetIdParticleCurrent() { return IdParticleCurrent; }
};

