#pragma once
#include "CSpParticle.h"

#define SPG_MAX_COUNT	0xFFFF
#define SPG_MAX_MAXLEVEL_STEP	0x4


#define M_STATE_OFF			0x00000000
#define M_STATE_ON			0x00000001
#define M_STATE_CALC		0x00000002
#define M_STATE_DO_NEXT		0x00000004
#define M_STATE_DO_REPORT	0x00000008
#define M_STATE_CHANGED		0x00000010
#define M_STATE_FULL		0x000000FF


extern int mRGB[16];

class CSP_Group
{
	friend class CSpParticle;
	friend class CDrawDlg;
	friend class CFindDlg;
	friend void PaintTraceToCDC(CDC* DCMem, SP_Calc* mData, int in_DrawFlags, SPos& delta);

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
	void CalcDensity(SData* DataAtPoint, SData& ViewPointData);
	void CalcCountDevider();

public:
	CRITICAL_SECTION m_cs;
	int m_maxCountLimit;
	SP_Calc* m_pCalc;
	SP_CalcFind* m_pCalcFind;

public:
	int GetIdByMaxDistance(double in_RelDist);
	int GetCount() {return m_Count;	}
	int AddSpParticle(int in_flags, const SData *in_Ptr, int in_Count, SP_Calc* pCalc);
	int RemoveSpParticle( int in_Idx);
	int SetParticleData(int in_Idx, int in_flags, const SData *in_Ptr);
	int RecalcDensityAt_(double atTime);
	void WriteState(FILE* in_Ptr, double in_StateTime);
	void WriteState(FILE* in_Ptr, int OutputType, int in_nPoints);
	double GetCurTime();
	int GetMinMaxData(SData* out_Min, SData* out_Max, SData_SD* out_MinSD, SData_SD* out_MaxSD);
	int GetMinMaxRelation(SMinMaxRelation* out_Min, SMinMaxRelation* out_Max);
	void Reset();
	void Clear();

	void GetMinMaxTracePos(SPos &min_Pos, SPos& max_Pos);

	int CalcRelations(double in_NextTimePoint, SPRelation* in_CalcRelations, SPRelation* in_PrevRelations);
	int CalcSingleRelation(SData* in_PtrData, CSpParticle* in_PtrView, double in_ViewTime, SPRelation* in_PrevRelations, SPRelation* in_NewRelations);
	int CalcDataByleRelations(SPRelation* in_CalcRelations);
	int CalcSingleDataByleRelation(SData* pCalcData, SPRelation* in_CalcRelations, SP_Calc* pCalc);


	//нужна процедура создания обратной траектории для добавляемых частиц
	//int CSP_Group::CreateHistory(int in_nTimes)

	int NextStep(double in_DeltaTime);
	CSP_Group();
	~CSP_Group();
	CSpParticle* GetItem(int in_Idx);

};
