#pragma once
#include "CSpParticle.h"
#include "CSDataArray.h"

#define SPG_MAX_COUNT	0xFFFF
#define SPG_MAX_MAXLEVEL_STEP	0x4


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



struct GroupSummaryInfo
{
	SPos Position; //позиция центра
	SPos Speed_3D; //суммарная скорость 3D
	double Speed; //суммарная скорость
	SPos Acceleration_3D; //суммарное ускорение 3D
	double Acceleration; //суммарное ускорение
	int Count;
	int TraceCnt_Max;
	int TraceCnt_Min;
	double TimePoint;

	void CLEAR()
	{
		Position = { 0 };
		Speed_3D = { 0 };
		Speed = 0.0;
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
		Position.DIV(Count);
		Speed_3D.DIV(Count);
		Speed = Speed_3D.LEN();

		Acceleration_3D.DIV(Count);
		Acceleration = Acceleration_3D.LEN();
	};


	void SET(const SData& inData, int in_IdTrace)
	{
		CLEAR();
		ADD(inData, in_IdTrace);
		Position = inData.position;
	};

	void ADD(const SData& inData, int in_IdTrace)
	{
		Position.ADD(inData.position);
		Speed_3D.ADD(inData.Speed);
		Speed += inData.AbsSpeed;
		Acceleration_3D.ADD(inData.Accel3D);
		//Acceleration;
		Count++;
		TimePoint = inData.TimePoint;

		if (in_IdTrace > TraceCnt_Max)  TraceCnt_Max = in_IdTrace;
		if (in_IdTrace < TraceCnt_Min)  TraceCnt_Min = in_IdTrace;
	};

	void ADD(const GroupSummaryInfo& inInfo)
	{
		Position.ADD(inInfo.Position);
		Speed_3D.ADD(inInfo.Speed_3D);
		Speed += inInfo.Speed;
		Acceleration_3D.ADD(inInfo.Acceleration_3D);
		Acceleration += inInfo.Acceleration;
		Count += inInfo.Count;

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
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "Speed");
					if (in_StrLen - (ret += nOutput) > 0)		io_Str[ret++] = in_ValueDelim;
				}
				if (in_StrLen - ret > 0)
				{
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, in_Format, Speed);
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


class CSP_Group
{
	friend class CSpParticle;
	friend class CDrawDlg;
	friend class CFindDlg;
	friend void PaintTraceToCDC(CDC* DCMem, SP_Calc* mData, int in_DrawFlags, SPos& delta);

	friend UINT DoCommandThread(LPVOID pParam);

private:
	int m_innerState;

private:
	CSpParticle** m_Array;
	SPRelation* m_CurentRelations;
	SPRelation* m_NextRelations;
	SPRelation* m_PrevRelations;
	SPRelation* m_Relations;
	int m_Count;
	double m_CountDivider;
	int m_maxCount;
	double m_CurTime;
	char *m_name;
	char* m_tmpString;
	//void CalcDensity(SData* DataAtPoint, SData& ViewPointData);
	void CalcCountDevider();

	char** m_CmdArr;
	int m_CalcState;
	int m_DelayMS;

public:
	CRITICAL_SECTION m_cs;
	CRITICAL_SECTION m_cs_Command;
	int m_maxCountLimit;
	SP_Calc* m_pCalc;
	SP_CalcFind* m_pCalcFind;

public:
	int GetIdByMaxDistance(double in_RelDist);
	int GetCount() {return m_Count;	}
	int AddSpParticle(int in_flags, SData *in_Ptr, int in_Count, SP_Calc* pCalc);
	int RemoveSpParticle( int in_Idx);
	int SetParticleData(int in_Idx, int in_flags, SData *in_Ptr);
	//int RecalcDensityAt_(double atTime);
	void WriteState(FILE* in_Ptr, double in_StateTime);
	void WriteState(FILE* in_Ptr, int OutputType, int in_nPoints);
	double GetCurTime();
	int GetMinMaxData(SData* out_Min, SData* out_Max, SData_SD* out_MinSD, SData_SD* out_MaxSD);
	int GetMinMaxRelation(SMinMaxRelation* out_Min, SMinMaxRelation* out_Max);
	void Reset();
	void Clear();
	void DoDelay() { if (m_DelayMS> 0 && m_DelayMS < 10000) Sleep(m_DelayMS); };
	bool DoPause() 
	{
		bool ret = (m_CalcState & M_STATE_PAUSE) != 0;
		if (ret) Sleep(100);
		return ret;
	};

	int GetGroupSummaryInfo(GroupSummaryInfo *out_Info, int in_IdPos);
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


	int GetMaxTraceId();
	void GetMinMaxTracePos(SPos &min_Pos, SPos& max_Pos, int idx_Start, int idx_End);
	void SetViewPoint(double in_Percents, double in_Spread);
	void SetViewPointRange(int in_Start, int in_End);

	int CalcRelations(double in_NextTimePoint, SPRelation* in_CalcRelations, SPRelation* in_PrevRelations, int *OutData, int in_CalcType);
	int CalcSingleRelation(SData* in_PtrData, CSpParticle* in_PtrView, double in_ViewTime, SPRelation* in_PrevRelations, SPRelation* in_NewRelations, int in_CalcType);
	int CalcDataByRelations(SPRelation* in_CalcRelations);
	int CalcDataByCurRelations();
	int CalcSingleDataByRelation(SData* pCalcData, SPRelation* in_CalcRelations, SP_Calc* pCalc);

	
	//нужна процедура создания обратной траектории для добавляемых частиц
	//int CSP_Group::CreateHistory(int in_nTimes)

	int NextStep(double in_DeltaTime);
	CSP_Group();
	~CSP_Group();
	CSpParticle* GetItem(int in_Idx);

};
