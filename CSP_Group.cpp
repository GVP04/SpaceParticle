#include "pch.h"
#include "CDrawDlg.h"
#include "CSP_Group.h"
#include "CommonFunc.h"


CSP_Group::CSP_Group()
{
	m_innerState = M_INNER_STATE_STARTING;
	AfxBeginThread(DoCommandThread, (LPVOID)this);

	m_DelayMS = 0;
	m_CalcState = M_STATE_OFF;
	m_name = new char[20000];
	sprintf_s(m_name, 1000, "GROUP_0");
	m_tmpString = m_name + 1000;
	*m_tmpString = 0;
	m_maxCount = 10;
	m_maxCountLimit = 0;
	m_pCalc = NULL;
	m_pCalcFind = NULL;

	m_CmdArr = new char* [M_CMD_MAXCOUNT];
	ZeroMemory(m_CmdArr, sizeof(char*) * M_CMD_MAXCOUNT);

	InitializeCriticalSection(&m_cs);
	InitializeCriticalSection(&m_cs_Command);


	m_Array = new CSpParticle * [m_maxCount];
	ZeroMemory(m_Array,  sizeof(CSpParticle *) * m_maxCount);

	int nRelations = m_maxCount * m_maxCount;

	m_Relations = new SPRelation [nRelations * 4];
	
	m_CurentRelations = m_Relations;
	m_NextRelations = m_CurentRelations + nRelations;
	m_PrevRelations = m_NextRelations + nRelations;

	m_Count = 0;
	CalcCountDevider();
	m_CurTime = 0.0;
	m_CalcState = M_STATE_READY;

	m_innerState = M_INNER_STATE_READY;

}
CSP_Group::~CSP_Group()
{
	m_CalcState |= M_STATE_BREAK;

	switch ((m_innerState & M_INNER_STATE_MASK))
	{
	case M_INNER_STATE_NULL:
	case M_INNER_STATE_STOPPED:
	case M_INNER_STATE_END_IS:
		break;
	case M_INNER_STATE_BREAK_ON:
	case M_INNER_STATE_STARTING:
	case M_INNER_STATE_READY:
	case M_INNER_STATE_RUNING:
		m_innerState = M_INNER_STATE_BREAK_ON;
		for(int cntr = 0; m_innerState != M_INNER_STATE_STOPPED && m_innerState != M_INNER_STATE_END_IS && cntr < 1000; cntr++)
			Sleep(10);
	}

	for (int cntr = 0; (m_CalcState & M_STATE_CALC) && cntr < 1000; cntr++)
		Sleep(10);


	EnterCriticalSection(&m_cs_Command);

	if (m_Relations)
	{
		delete [] m_Relations;
		m_Relations = NULL;
	}

	if (m_Array)
	{
		for (int i = 0; i < m_Count; i++) 
			if (m_Array[i])
			{
				delete m_Array[i]; 
				m_Array[i] = NULL; 
			}
		delete[] m_Array;
	}

	delete[] m_name;
	m_Array = NULL;


	if (m_CmdArr)
	{
		for (int i = 0; i < M_CMD_MAXCOUNT && m_CmdArr[i]; i++)
		{
			delete m_CmdArr[i];
			m_CmdArr[i] = NULL;
		}
	}

	if (m_CmdArr) delete[] m_CmdArr;
	m_CmdArr = NULL;

	LeaveCriticalSection(&m_cs_Command);
	DeleteCriticalSection(&m_cs_Command);
	DeleteCriticalSection(&m_cs);
}

void CSP_Group::Reset()
{
	EnterCriticalSection(&m_cs_Command);

	if (m_Array)
	{
		for (int i = 0; i < m_Count; i++)
			if (m_Array[i])
			{
				delete m_Array[i];
				m_Array[i] = NULL;
			}
	}
	if (m_Relations)
	{
		delete [] m_Relations;
		m_Relations = NULL;
	}

	if (m_CmdArr)
		for (int i = 0; i < M_CMD_MAXCOUNT && m_CmdArr[i]; i++)
		{
			delete m_CmdArr[i];
			m_CmdArr[i] = NULL;
		}

	sprintf_s(m_name, 1000, "GROUP_0");

	m_maxCount = 10;
	int nRelations = m_maxCount * m_maxCount;

	m_Relations = new SPRelation[nRelations * 4];

	m_CurentRelations = m_Relations;
	m_NextRelations = m_CurentRelations + nRelations;
	m_PrevRelations = m_NextRelations + nRelations;

	m_DelayMS = 0;
	m_Count = 0;
	CalcCountDevider();
	m_CurTime = 0.0;
	m_maxCountLimit = 0;
	m_CalcState = M_STATE_READY;

	LeaveCriticalSection(&m_cs_Command);
}

void CSP_Group::Clear()
{
	if (m_Array)
	{
		for (int i = 0; i < m_Count; i++)
			if (m_Array[i])
				m_Array[i]->Clear();
	}

	if (m_Relations)
	{
		delete[] m_Relations;
		m_Relations = NULL;
	}

	int nRelations = m_maxCount * m_maxCount;

	m_Relations = new SPRelation[nRelations * 4];

	m_CurentRelations = m_Relations;
	m_PrevRelations = m_CurentRelations + nRelations;
	m_NextRelations = m_PrevRelations + nRelations;

	m_CurTime = 0.0;
}

void CSP_Group::CalcCountDevider()
{
	if (m_Count < 2) m_CountDivider = 1.0;
	else m_CountDivider = 1.0 / (double(m_Count - 1));
}


int CSP_Group::AddSpParticle(int in_flags, SData *in_Ptr, int in_Count, SP_Calc* pCalc)
{
	int ret = m_Count;
	if (m_Array && in_Ptr)
	{
		if (m_Count + in_Count > m_maxCount && m_maxCount < SPG_MAX_COUNT)
		{
			int newCount = m_maxCount + in_Count;
			if (newCount > SPG_MAX_COUNT) newCount = SPG_MAX_COUNT;

			CSpParticle ** tmp = new CSpParticle* [newCount];
			CopyMemory(tmp, m_Array, m_maxCount * sizeof(CSpParticle*));
			delete[] m_Array;
			m_Array = tmp;
			m_maxCount = newCount;
		}

		while (in_Count-- > 0 && m_Count < m_maxCount)
			if ((m_Array[m_Count] = new CSpParticle(m_pCalc)))
			{
				m_Array[m_Count]->m_pCalc = pCalc;
				m_Array[m_Count++]->SetParticleData(SP_DATA_ALL, in_Ptr);
			}
		CalcCountDevider();
	}
	return m_Count - ret;
}

int CSP_Group::GetMaxTraceId()
{
	int ret = 0;
	if (m_Array && m_Count)
	{
		for(int i = 0; i < m_Count; i++)
			if (m_Array[i])
			{
				int tmp = m_Array[i]->GetMaxIdx();
				if (tmp > ret) ret = tmp;
			}
	}
	return ret;
}


int CSP_Group::RemoveSpParticle(int in_Idx)
{
	if (m_Array && in_Idx >= 0 && in_Idx < m_Count)
	{
		if (m_Array[in_Idx]) delete m_Array[in_Idx];
		int i, MoveCount = m_Count - in_Idx - 1;

		if (MoveCount)
		{
			CopyMemory(m_Array + in_Idx, m_Array + in_Idx + 1, MoveCount * sizeof(CSpParticle*));
			if (m_CurentRelations)
			{
				CopyMemory(m_PrevRelations + m_maxCount * in_Idx, m_PrevRelations + m_maxCount * (in_Idx + 1), sizeof(SPRelation) * m_maxCount * MoveCount);
				CopyMemory(m_CurentRelations + m_maxCount * in_Idx, m_CurentRelations + m_maxCount * (in_Idx + 1), sizeof(SPRelation) * m_maxCount * MoveCount);
				CopyMemory(m_NextRelations + m_maxCount * in_Idx, m_NextRelations + m_maxCount * (in_Idx + 1), sizeof(SPRelation) * m_maxCount * MoveCount);

				int tmpLen = (MoveCount) * sizeof(SPRelation);
				SPRelation* Pos = m_CurentRelations + in_Idx;

				for (i = 1; i < m_Count; i++, Pos += m_maxCount)
					CopyMemory(Pos, Pos + 1, tmpLen);

				Pos = m_NextRelations + in_Idx;

				for (i = 1; i < m_Count; i++, Pos += m_maxCount)
					CopyMemory(Pos, Pos + 1, tmpLen);

				Pos = m_PrevRelations + in_Idx;

				for (i = 1; i < m_Count; i++, Pos += m_maxCount)
					CopyMemory(Pos, Pos + 1, tmpLen);
			}
		}

		m_Count--;
		CalcCountDevider();
	}

	return m_Count;
}

CSpParticle* CSP_Group::GetItem(int in_Idx)
{
	CSpParticle* ret = NULL;

	if (m_Array && in_Idx >= 0 && in_Idx < m_Count)
		ret = m_Array[in_Idx];

	return ret;
}

int CSP_Group::SetParticleData(int in_Idx, int in_flags, SData *in_Ptr)
{
	int ret = 0;
	if (m_Array && in_Idx >= 0 && in_Idx < m_Count && in_Ptr)
		ret = m_Array[in_Idx]->SetParticleData(in_flags,  in_Ptr);

	return ret;
}

double CSP_Group::GetCurTime()
{
	double ret = m_CurTime;
	if (m_Array && m_Count > 0 && m_Array[0])
		ret = m_Array[0]->GetCurTime();

	return ret;
}

int CSP_Group::GetMinMaxData(SData* out_Min, SData* out_Max, SData_SD* out_MinSD, SData_SD* out_MaxSD)
{
	int ret = SP_STATE_ERROR_MAIN;
	SData* pData;

	if (out_Min && out_Max && out_MinSD && out_MaxSD && m_Count && m_Array[0] && (pData = m_Array[0]->GetCurData()))
	{
		double* pDoubleMin, * pDoubleMax;
		*out_Min = *pData;
		*out_Max = *pData;
		// с 1 потому что проинициализировано выше значением idx = 0
		for (int i = 1; i < m_Count; i++)
		{
			if (m_Array[i] && (pData = m_Array[i]->GetCurData()))
			{
				pDoubleMin = (double*)out_Min;
				pDoubleMax = (double*)out_Max;

				double* pCmp = (double*)pData;

				for (int j = 0; j < 13; j++, pDoubleMax++, pDoubleMin++, pCmp++)
				{
					double value = fabs(*pCmp);
					if (value > *pDoubleMax)  *pDoubleMax = value; 
					if (value < *pDoubleMin)  *pDoubleMin = value; 
				}
			}
		}

		double* pDoubleMinSD, * pDoubleMaxSD;
		pDoubleMinSD = (double*)out_MinSD;
		pDoubleMaxSD = (double*)out_MaxSD;

		for (int i = 0; i < 6; i++)
		{
			*(pDoubleMinSD++) = 1.0e30;
			*(pDoubleMaxSD++) = -1.0e30;
		}

		pDoubleMin = (double*)out_Min;
		pDoubleMax = (double*)out_Max;
		pDoubleMinSD = (double*)out_MinSD;
		pDoubleMaxSD = (double*)out_MaxSD;

		for (int j = 0; j < 4; j++, pDoubleMaxSD++, pDoubleMinSD++)
			for (int k = 0; k < 3; k++, pDoubleMax++, pDoubleMin++)
			{
				if (*pDoubleMax > *pDoubleMaxSD)  *pDoubleMaxSD = *pDoubleMax;
				if (*pDoubleMin < *pDoubleMinSD)  *pDoubleMinSD = *pDoubleMin;
			}
		if (*pDoubleMax > *pDoubleMaxSD)  *pDoubleMaxSD = *pDoubleMax;
		if (*pDoubleMin < *pDoubleMinSD)  *pDoubleMinSD = *pDoubleMin;

	}

	return ret;
}

int CSP_Group::GetMinMaxRelation(SMinMaxRelation* out_Min, SMinMaxRelation* out_Max)
{
	int ret = SP_STATE_ERROR_MAIN;

	if (out_Min && out_Max && m_Count > 1 && m_Array[0] && m_CurentRelations)
	{
		out_Min->CLEAR();
		out_Max->CLEAR();

		out_Min->Distance.MMValue = 1.0e30;
		out_Min->Speed.MMValue = 1.0e30;

		out_Max->Distance.MMValue = -1.0e30;
		out_Max->Speed.MMValue = -1.0e30;


		if (m_Array && m_CurentRelations && m_Count > 1)
		{
			for (int idx1 = 0; idx1 < m_Count; idx1++)
				for (int idx2 = 0; idx2 < m_Count; idx2++)
					if (idx1 != idx2)
					{
						SPRelation* curRel = m_CurentRelations + idx1 + m_maxCount * idx2;

						double abRelativeSpeed = fabs(curRel->RelativeSpeedValue);

						if (out_Min->Speed.MMValue > abRelativeSpeed)
						{ 
							out_Min->Speed.MMValue = abRelativeSpeed;
							out_Min->Speed.idxParticle1 = idx1;
							out_Min->Speed.idxParticle2 = idx2;
						}
						if (out_Min->Distance.MMValue > curRel->DistancePwr1)
						{ 
							out_Min->Distance.MMValue = curRel->DistancePwr1; 
							out_Min->Distance.idxParticle1 = idx1;
							out_Min->Distance.idxParticle2 = idx2;
						}

						if (out_Max->Speed.MMValue < abRelativeSpeed)
						{
							out_Max->Speed.MMValue = abRelativeSpeed;
							out_Max->Speed.idxParticle1 = idx1;
							out_Max->Speed.idxParticle2 = idx2;
						}
						if (out_Max->Distance.MMValue < curRel->DistancePwr1)
						{
							out_Max->Distance.MMValue = curRel->DistancePwr1;
							out_Max->Distance.idxParticle1 = idx1;
							out_Max->Distance.idxParticle2 = idx2;
						}
					}

			out_Min->Speed.Part1 = *(m_Array[out_Min->Speed.idxParticle1]->GetCurData());
			out_Min->Speed.Part2 = *(m_Array[out_Min->Speed.idxParticle2]->GetCurData());
			out_Min->Speed.relation = m_CurentRelations[out_Min->Speed.idxParticle1 + m_maxCount * out_Min->Speed.idxParticle2];

			out_Max->Speed.Part1 = *(m_Array[out_Max->Speed.idxParticle1]->GetCurData());
			out_Max->Speed.Part2 = *(m_Array[out_Max->Speed.idxParticle2]->GetCurData());
			out_Max->Speed.relation = m_CurentRelations[out_Max->Speed.idxParticle1 + m_maxCount * out_Max->Speed.idxParticle2];

			out_Min->Distance.Part1 = *(m_Array[out_Min->Distance.idxParticle1]->GetCurData());
			out_Min->Distance.Part2 = *(m_Array[out_Min->Distance.idxParticle2]->GetCurData());
			out_Min->Distance.relation = m_CurentRelations[out_Min->Distance.idxParticle1 + m_maxCount * out_Min->Distance.idxParticle2];

			out_Max->Distance.Part1 = *(m_Array[out_Max->Distance.idxParticle1]->GetCurData());
			out_Max->Distance.Part2 = *(m_Array[out_Max->Distance.idxParticle2]->GetCurData());
			out_Max->Distance.relation = m_CurentRelations[out_Max->Distance.idxParticle1 + m_maxCount * out_Max->Distance.idxParticle2];
		}
	}

	return ret;
}

void CSP_Group::WriteState(FILE* in_Ptr, double in_StateTime)
{
	if (in_Ptr)
	{
		if (m_Array && m_Count > 0)
		{
			fprintf(in_Ptr, "\nGroup %s\tCount=\t%d\tTime=\t%g\n", m_name, m_Count, m_CurTime);

			for (int i = 0; i < m_Count; i++)
				if (m_Array[i])
				{
					m_Array[i]->GetStateDescription(m_tmpString, in_StateTime);
					fprintf(in_Ptr, "\t\tSParticle\t%d\t%s\n", i, m_tmpString);
				}
		}
		else
			fprintf(in_Ptr, "m_Array %p\tCSP_Group.m_Count = %d\n", m_Array, m_Count);
	}
}

void CSP_Group::WriteState(FILE* in_Ptr, int OutputType, int in_nPoints)
{
	if (in_Ptr)
	{
		if (m_Array && m_Count > 0)
		{
			switch (OutputType)
			{
			case 0:
				{
					fprintf(in_Ptr, "\nGroup %s\tCount=\t%d\tTime=\t%10.10g\n", m_name, m_Count, m_CurTime);

					for (int i = 0; i < m_Count; i++)
						if (m_Array[i])
						{
							m_Array[i]->GetStateDescription(m_tmpString, in_nPoints);
							fprintf(in_Ptr, "\tSParticle\t%d\t%s\n", i, m_tmpString);
						}
				}
				break;
			case 1:
				{
					fprintf(in_Ptr, "\nGroup %s\tCount=\t%d\tTime=\t%10.10g\n", m_name, m_Count, m_CurTime);

					for (int i = 0; i < m_Count; i++)
					{
						m_Array[i]->GetStateDescription(m_tmpString);
						fprintf(in_Ptr, "\tSParticle\t%d\t%s\n", i, m_tmpString);

						for (int j = 0; j < m_Count; j++)
							if (i != j)
							{
								SPRelation* tmpRel = m_CurentRelations + i + j * m_maxCount;
								char* out_Str = m_tmpString;
								out_Str += sprintf_s(out_Str, 1000, "\tTime=\t%10.10g\t", tmpRel->ViewPointData.TimePoint);
								out_Str += sprintf_s(out_Str, 1000, "\tVP_Point=\t%10.10g\t%10.10g\t%g", tmpRel->ViewPointData.position.X, tmpRel->ViewPointData.position.Y, tmpRel->ViewPointData.position.Z);
								out_Str += sprintf_s(out_Str, 1000, "\tVP_Speed=\t%10.10g\t%10.10g\t%10.10g", tmpRel->ViewPointData.Speed.X, tmpRel->ViewPointData.Speed.Y, tmpRel->ViewPointData.Speed.Z);
								out_Str += sprintf_s(out_Str, 1000, "\tSpeedRel=\t%10.10g\tDistRel=\t%10.10g", tmpRel->RelativeSpeedValue, tmpRel->DistancePwr1);
								fprintf(in_Ptr, "\tRelation\t%d\t%s\n", j, m_tmpString);
							}
					}
				}
				break;
			}



		}
		else
			fprintf(in_Ptr, "m_Array %p\tCSP_Group.m_Count = %d\n", m_Array, m_Count);
	}
}

void CSP_Group::SetViewPoint(double in_Percents, double in_Spread)
{
	if (m_pCalc)
	{
		if (in_Percents > 100.0) in_Percents = 100.0;
		if (in_Spread > 100.0) in_Spread = 100.0;
		if (in_Percents < 0.0) in_Percents = 100.0;
		if (in_Spread < 0.000001) in_Spread = 0.000001;

		if ((in_Percents < 0.0 && in_Spread >= 100.0 ) || !(m_pCalc->Group->GetCalcState()& M_STATE_READY) || !m_Array || !m_Array[0] || !m_Array[0]->m_Data || (m_Array[0]->m_Data->CurPos < 0) || !m_Array[1])
		{
			m_pCalc->DrawSet.ViewPoint.TimeInPc = -1.0;
			m_pCalc->DrawSet.ViewPoint.Spread = 100.0;
			SetViewPointRange(0, SP_DRAW_MAX_ID);
		}
		else
		{
			double curTime = m_Array[0]->GetCurTime();
			double vpTime = curTime * 0.01;
			double vpSpread = vpTime;
	
			vpTime *= in_Percents;
			vpSpread *= in_Spread;

			int id_Start, id_End, id_Mid;

			id_Mid = m_Array[0]->m_Data->GetLowIdxByTime(vpTime);
			if (vpTime + vpSpread > curTime) id_End = SP_DRAW_MAX_ID;
			else	id_End = m_Array[0]->m_Data->GetLowIdxByTime(vpTime + vpSpread);

			if (vpTime - vpSpread <= 0.0) id_Start = 0;
			else	id_Start = m_Array[0]->m_Data->GetLowIdxByTime(vpTime - vpSpread);

			SetViewPointRange(id_Start, id_End);
		}
	}
}

void CSP_Group::SetViewPointRange(int in_Start, int in_End)
{
	if (m_pCalc)
	{
		if (in_Start < 0) in_Start = 0;
		if (in_End < in_Start) in_End = SP_DRAW_MAX_ID;

		m_pCalc->DrawSet.ViewPoint.StartIdx = in_Start;
		m_pCalc->DrawSet.ViewPoint.EndIdx = in_End;
	}
}


void CSP_Group::GetMinMaxTracePos(SPos& min_Pos, SPos& max_Pos, int idx_Start, int idx_End)
{
	min_Pos.X = min_Pos.Y = min_Pos.Z = 1.0e30;
	max_Pos.X = max_Pos.Y = max_Pos.Z = -1.0e30;

	for (int i = 0; i < m_Count; i++) 
		if (m_Array[i]) 
			m_Array[i]->GetMinMaxTracePos(min_Pos, max_Pos, idx_Start, idx_End);
}

int CSP_Group::GetIdByMaxDistance(double in_RelDist)
{
	int ret = -1;
	if (m_Array && m_Count > 2 && m_Array[0] && m_Array[1])
	{
		for (int i = 0; i < m_Count && ret < 0; i++)
		{
			int j;
			for (j = 0; j < m_Count && (i == j || m_CurentRelations[i + m_maxCount * j].DistancePwr1 > in_RelDist); j++)
				;

			if (j >= m_Count) 
				ret = i; 
		}
	}
	return ret;
}


int CSP_Group::GetGroupSummaryInfo(GroupSummaryInfo* out_Info, int in_IdPos)
{
	int ret = -1;
	if (out_Info)
	{
		out_Info->CLEAR();
		if (m_Array && *m_Array)
		{
			if (in_IdPos < 0) in_IdPos = m_Array[0]->GetMaxIdx();

			if (in_IdPos >= 0)
			{
				SData tmpData;
				tmpData.CLEAR();

				for (int i = 0; i < m_Count; i++)
					if (m_Array[i])
					{
						if (m_Array[i]->GetDataById(in_IdPos, &tmpData) >= 0)
							out_Info->ADD(tmpData, in_IdPos);
					}
				if (out_Info->Count < 1)
					out_Info->CLEAR();
				else
					out_Info->CALC();
			}
		}

	}

	return ret;
}

int CSP_Group::GetGroupSummaryInfo(GroupSummaryInfo* out_Info, double in_TimePoint)
{
	int ret = -1;
	if (out_Info)
	{
		if (m_Array && *m_Array)
		{
			ret = m_Array[0]->GetLowIdxByTime(in_TimePoint);
			if (ret >= 0)
				ret = GetGroupSummaryInfo(out_Info, ret);
		}
		else out_Info->CLEAR();
	}

	return ret;
}

int CSP_Group::GetGroupSummaryInfo(GroupSummaryInfo* out_Info)
{
	return GetGroupSummaryInfo(out_Info, -1);
}




int CSP_Group::NextStep(double in_DeltaTime)
{
	int ret = SP_STATE_ERROR_MAIN;
	if (in_DeltaTime < m_pCalc->Time.MinStep) in_DeltaTime = m_pCalc->Time.MinStep;

	if (m_Array && m_Count > 1 && m_Array[0] && m_Array[1])
	{
		int doByKeyPoint = SP_STATE_INPROC;
		double m_NextTime, KeyPoint, KeyPointmin;

		while (doByKeyPoint == SP_STATE_INPROC)
		{
			KeyPoint = KeyPointmin = m_NextTime = m_CurTime + in_DeltaTime;

			for (int i = 0; i < m_Count; i++)
				if (m_Array[i])	m_Array[i]->NewPoint_Begin(m_NextTime);


			for (int i = 0; i < m_Count; i++)
				if (m_Array[i])
				{
					SData* curData = m_Array[i]->GetCurData();
					SData* pNextData = &(m_Array[i]->m_NextData);

					//расчет новой позиции
					m_Array[i]->GetNextDataByTime(pNextData, m_NextTime, &KeyPoint);
  					if (KeyPointmin > KeyPoint) KeyPointmin = KeyPoint;
				}

			if (m_NextTime > KeyPointmin)
			{
				m_NextTime = KeyPointmin + m_pCalc->Time.MinStep * 0.01;

				for (int i = 0; i < m_Count; i++)
					if (m_Array[i])	m_Array[i]->NewPoint_Begin(m_NextTime);

				for (int i = 0; i < m_Count; i++)
					if (m_Array[i])
					{
						SData* curData = m_Array[i]->GetCurData();
						SData* pNextData = &(m_Array[i]->m_NextData);
						//расчет новой позиции
						m_Array[i]->GetNextDataByTime(pNextData, m_NextTime, &KeyPoint);
					}
			}

			int RelationPos = 0, BalanceCounter = 0, CalcType = 0;
			while (
				((doByKeyPoint = CalcRelations(m_NextTime, m_NextRelations, m_CurentRelations, &RelationPos, CalcType = (in_DeltaTime >= m_pCalc->Time.MinStep ? 0 : BalanceCounter + 1))) & SP_STATE_PHANTOM_DIST_LECRITICAL) == SP_STATE_PHANTOM_DIST_LECRITICAL
				&& CalcType > 0
				&& in_DeltaTime < m_pCalc->Time.MinStep && BalanceCounter++ < 100 
				&& (m_pCalc->CollisionType & COLISSION_TYPEMASK) != COLISSION_NONE
				)
			{
				for (int i = 0; i < m_Count; i++)
					if (m_Array[i])
					{
						RelationPos = i;

						for (int j = 0; j < m_Count; j++, RelationPos += m_maxCount)
						{
							m_NextRelations[RelationPos].State &= (0xFFFFFFFF ^ SP_STATE_NEED_UPDATE);

							if (m_Array[j] && i != j && (m_NextRelations[RelationPos].State & SP_STATE_PHANTOM_DIST_LECRITICAL))
							{
								m_NextRelations[RelationPos].State ^= SP_STATE_PHANTOM_DIST_LECRITICAL;
								m_NextRelations[RelationPos].State |= SP_STATE_NEED_UPDATE;

								SPRelation* curRelation = m_NextRelations + RelationPos;
								SData* curData = &(m_Array[i]->m_NextData);
								double deltaS = m_pCalc->CriticalDistance - curRelation->DistancePwr1;

								if ( (m_pCalc->OutPutFlags & 0x0002))
								{
									FILE* tmpf;
									fopen_s(&tmpf, "TTTTTT.TXT", "at");
									if (tmpf)
									{
										fprintf_s(tmpf, "DistancePwr1\t%e\t%e\t%e\t%e\t", curRelation->DistancePwr1, curRelation->Distance_3D.X, curRelation->Distance_3D.Y, curRelation->Distance_3D.Z);
										fprintf_s(tmpf, "\tCur->Position\t%e\t%e\t%e\t", curData->position.X, curData->position.Y, curData->position.Z);
										fprintf_s(tmpf, "\tVP->Position\t%e\t%e\t%e\t", curRelation->ViewPointData.position.X, curRelation->ViewPointData.position.Y, curRelation->ViewPointData.position.Z);
										fprintf_s(tmpf, "\tdeltaS\t%e\t", deltaS);
										fprintf_s(tmpf, "\tRelSpeed\t%e\t%e\t%e\t%e\n", curRelation->RelativeSpeedValue, curRelation->RelativeSpeed_3D.X, curRelation->RelativeSpeed_3D.Y, curRelation->RelativeSpeed_3D.Z);
										fclose(tmpf);
									}
								}


								if ((m_pCalc->CollisionType & COLISSION_POS_SET1))
								{
									SPos deltapos = curRelation->Distance_3D;
									deltapos.SET_LENGHT(deltaS * 1.0000001);
									curData->position.ADD(deltapos);
								}


								switch (m_pCalc->CollisionType & COLISSION_TYPEMASK)
								{
								case COLISSION_ELASTIC:

									switch (m_pCalc->CollisionType & COLISSION_SPEED_MASK)
									{
									case COLISSION_SPEED_ADD:
									{
										SPos newSpeed;
										newSpeed.SET_AS_REFLECT(curData->Speed, curRelation->ViewPointData.Speed, curRelation->Distance_3D);
										if (newSpeed.LENPWR2() > m_pCalc->maxAbsSpeedPwr2)
											newSpeed.SET_LENGHT(m_pCalc->maxAbsSpeed);
										curData->Speed = newSpeed;
									}
									break;
									case COLISSION_SPEED_NOCHANGE:
									default:
									{
										SPos newSpeed;
										newSpeed.SET_AS_REFLECT(curData->Speed, curRelation->ViewPointData.Speed, curRelation->Distance_3D);
										newSpeed.SET_LENGHT(curData->Speed.LEN());
										curData->Speed = newSpeed;
									}
									break;
									}
									break;
								case COLISSION_INELASTIC:
									switch (m_pCalc->CollisionType & COLISSION_SPEED_MASK)
									{
									case COLISSION_SPEED_VIEWPOINT:
									default:
										curData->Speed = curRelation->ViewPointData.Speed;
										break;
									}
									break;
								case COLISSION_SLIPPERY:
									switch (m_pCalc->CollisionType & COLISSION_SPEED_MASK)
									{
									case COLISSION_SPEED_OPPOSITE:
									default:
									{
										SPos newDirection;
										newDirection.SET_AS_OPPOSITE(curRelation->RelativeSpeed_3D, curRelation->Distance_3D);
										newDirection.NORMALIZE();
										curData->Speed = newDirection;
										curData->Speed.MULT(curData->AbsSpeed);
										break;
									}
									}
									break;
								}
							}

						}
					}
			}

			if ((doByKeyPoint & SP_STATE_WARNING_MASK) && in_DeltaTime >= m_pCalc->Time.MinStep)
			{
				in_DeltaTime *= 0.5;
				doByKeyPoint = SP_STATE_INPROC;
			}
			else
				doByKeyPoint = SP_STATE_ОК;
		}

		EnterCriticalSection(&m_cs);

		if ( m_Array[0]->m_NextData.TimePoint != m_Array[1]->m_NextData.TimePoint)
			ret = SP_STATE_ОК;



		ret = SP_STATE_ОК;
		//фиксация результата и переход на следующую итерацию
		for (int i = 0; i < m_Count; i++)
			if (m_Array[i]) m_Array[i]->MoveToNewPoint(m_maxCountLimit);


		if (m_Array[0]->GetCurData()->TimePoint != m_Array[1]->GetCurData()->TimePoint)
			ret = SP_STATE_ОК;

		m_CurTime = m_NextTime;

		SPRelation* m_SwapRelations = m_CurentRelations;
		m_CurentRelations = m_NextRelations;
		m_NextRelations = m_PrevRelations;
		m_PrevRelations = m_SwapRelations;

		CalcDataByCurRelations();
		//CalcDataByRelations(m_CurentRelations);

		LeaveCriticalSection(&m_cs);

	}
	return ret;
}

//in_CalcType == 0 - новый расчет. 1 - балансировка
int CSP_Group::CalcSingleRelation(SData* in_PtrData, CSpParticle* in_PtrView, double in_ViewTime, SPRelation* in_PrevRelations, SPRelation* in_NewRelations, int in_CalcType)
{
	int ret = SP_STATE_ERROR_MAIN;

	if (in_PtrData && in_PtrView && in_NewRelations)
	{
		int tmpRet = 0;
		if (in_CalcType <= 1 || (in_NewRelations->State & SP_STATE_NEED_UPDATE) == SP_STATE_NEED_UPDATE)
		{
			double in_StartTime = -99999.0, in_EndTime = in_ViewTime;

			in_NewRelations->CLEAR();
			
			if (in_PrevRelations)
			{
				in_NewRelations->LastViewPointTime = in_PrevRelations->LastViewPointTime;
				in_StartTime = in_NewRelations->LastViewPointTime;
				in_EndTime = in_PtrView->GetCurTime();
			}

			if (in_EndTime > in_ViewTime) in_EndTime = in_ViewTime;
			tmpRet = in_PtrView->FindViewPoint(&in_NewRelations->ViewPointData, in_PtrData, in_ViewTime, in_StartTime, in_EndTime);
		}
		else
		{
			in_NewRelations->State = 0;
		}

		if ((tmpRet & SP_STATE_ERROR_MINOR) == SP_STATE_ERROR_MAIN)
		{
			ret = SP_STATE_ERROR_MAIN;
		}
		else if ((tmpRet & SP_STATE_ERROR_MINOR) && (tmpRet & SP_STATE_HIGH_VIEW))
		{
			ret = tmpRet;
			in_NewRelations->SET_AS_FARAWAY();
		}
		else if ((tmpRet & SP_STATE_ERROR_MINOR) && (tmpRet & SP_STATE_LOW_VIEW) && in_NewRelations->DistancePwr1 > 1.0e90)
		{
			ret = tmpRet;
			in_NewRelations->SET_AS_FARAWAY();
		}
		else
		{
			ret = SP_STATE_ОК;
			in_NewRelations->LastViewPointTime = in_NewRelations->ViewPointData.TimePoint;

			in_NewRelations->RelativeSpeedValue = SData::GetRelativeSpeed(*in_PtrData, in_NewRelations->ViewPointData, &in_NewRelations->RelativeSpeed_3D);
			in_NewRelations->RelativeSpeedValueAbs = fabs(in_NewRelations->RelativeSpeedValue);

			in_NewRelations->Distance_3D.X = in_PtrData->position.X - in_NewRelations->ViewPointData.position.X;
			in_NewRelations->Distance_3D.Y = in_PtrData->position.Y - in_NewRelations->ViewPointData.position.Y;
			in_NewRelations->Distance_3D.Z = in_PtrData->position.Z - in_NewRelations->ViewPointData.position.Z;

			double DistancePwr1tmp = in_NewRelations->Distance_3D.LEN();
			if (m_pCalc->ParentSpaceThickness != 0.0)
			{
				double tmpMltplr = (DistancePwr1tmp + m_pCalc->ParentSpaceThickness) / DistancePwr1tmp;
				in_NewRelations->Distance_3D.X *= tmpMltplr;
				in_NewRelations->Distance_3D.Y *= tmpMltplr;
				in_NewRelations->Distance_3D.Z *= tmpMltplr;
				DistancePwr1tmp += m_pCalc->ParentSpaceThickness;
			}

			in_NewRelations->DistancePwr2 = DistancePwr1tmp * DistancePwr1tmp;
			in_NewRelations->DistancePwr1 = DistancePwr1tmp;

			if (DistancePwr1tmp <= m_pCalc->CriticalDistance)
			{
				ret |= SP_STATE_ERROR_MINOR | SP_STATE_PHANTOM_DIST_LECRITICAL;
				in_NewRelations->State |= SP_STATE_PHANTOM_DIST_LECRITICAL;
				DistancePwr1tmp = 0.00000000000001 + m_pCalc->CriticalDistance;
				//in_NewRelations->Distance_3D.SET_LENGHT(DistancePwr1tmp);
			}

			if (in_PrevRelations &&
				((in_NewRelations->RelativeSpeedValueAbs >= m_pCalc->maxRelativeSpeed && in_PrevRelations->RelativeSpeedValueAbs < m_pCalc->maxRelativeSpeed)
					|| (in_NewRelations->RelativeSpeedValueAbs < m_pCalc->maxRelativeSpeed && in_PrevRelations->RelativeSpeedValueAbs >= m_pCalc->maxRelativeSpeed)))
			{
				ret |= SP_STATE_ERROR_MINOR | SP_STATE_REL_CHANGE_LS;
				in_NewRelations->State |= SP_STATE_REL_CHANGE_LS;
			}


			if (in_NewRelations->DistancePwr2 > 0.0)
			{
				double tmpDistancePwr2 = DistancePwr1tmp * DistancePwr1tmp;
				switch (m_pCalc->DensityCalcType)
				{
				case 0:
				default: //dens = 1 - 1 / S^2
					in_NewRelations->DensityAtBasePoint = SP_DENCITY_MAX - SP_DENCITY_MAX / tmpDistancePwr2;
					break;
				case 1://dens = 1 - 1 / (1 + S^2)
					in_NewRelations->DensityAtBasePoint = SP_DENCITY_MAX - SP_DENCITY_MAX / (SP_DENCITY_MAX + tmpDistancePwr2);
					break;
				case 2://dens = 1 - 1 / (1 + S) ^ 2    ??????????????????????????????????????
					in_NewRelations->DensityAtBasePoint = SP_DENCITY_MAX - SP_DENCITY_MAX / ((SP_DENCITY_MAX + DistancePwr1tmp) * (SP_DENCITY_MAX + DistancePwr1tmp));
					break;
				}

				if (in_NewRelations->DensityAtBasePoint <= 0.0)
				{
					if (in_NewRelations->DensityAtBasePoint < 0.0)
					{
						in_NewRelations->State |= SP_STATE_PHANTOM_DIST_LECRITICAL;
						ret |= SP_STATE_ERROR_MINOR | SP_STATE_PHANTOM_DIST_LECRITICAL;
					}
					else in_NewRelations->DensityAtBasePoint = 0.0000000000001;
				}

				if (in_NewRelations->RelativeSpeedValueAbs < m_pCalc->maxRelativeSpeed) ///!!!!Очень спорное условие, учитывая скважность расчета
				{
					switch (m_pCalc->AccCalcType)
					{
					case 0:
					default: //AccelerationValue не может быть больше 1.0
						in_NewRelations->AccelerationValue = 1.0 - in_NewRelations->DensityAtBasePoint;
						break;
					case 1: //AccelerationValue больше 0.0nan
						in_NewRelations->AccelerationValue = (1.0 / in_NewRelations->DensityAtBasePoint) - 1.0;
						break;
					}

					if (in_NewRelations->AccelerationValue < 0.0)
					{
						ret |= SP_STATE_ERROR_MINOR | SP_STATE_REL_ACCELERATION_L0;
						in_NewRelations->State |= SP_STATE_REL_ACCELERATION_L0;
					}

					double tmpAddMlpl = in_NewRelations->AccelerationValue / in_NewRelations->DistancePwr1;
					in_NewRelations->Acceleration_3D = in_NewRelations->Distance_3D;
					in_NewRelations->Acceleration_3D.MULT(-tmpAddMlpl);

					if (m_pCalc->WindType && in_NewRelations->RelativeSpeedValueAbs > 0.0000000000001)
					{
						switch (m_pCalc->WindType) // Fantasy 
						{
						case 0://OFF
						default:
							break;
						case 1: // ветер влияет прямо пропорционально относительной скорости 
							in_NewRelations->Wind_3D.X = (SP_DENCITY_MAX - in_NewRelations->DensityAtBasePoint) * in_NewRelations->RelativeSpeed_3D.X;
							in_NewRelations->Wind_3D.Y = (SP_DENCITY_MAX - in_NewRelations->DensityAtBasePoint) * in_NewRelations->RelativeSpeed_3D.Y;
							in_NewRelations->Wind_3D.Z = (SP_DENCITY_MAX - in_NewRelations->DensityAtBasePoint) * in_NewRelations->RelativeSpeed_3D.Z;
							break;
						case 2:	// ветер влияет прямо пропорционально относительной скорости 
							in_NewRelations->Wind_3D.X = (SP_DENCITY_MAX - in_NewRelations->DensityAtBasePoint) * in_NewRelations->RelativeSpeed_3D.X * 0.5;
							in_NewRelations->Wind_3D.Y = (SP_DENCITY_MAX - in_NewRelations->DensityAtBasePoint) * in_NewRelations->RelativeSpeed_3D.Y * 0.5;
							in_NewRelations->Wind_3D.Z = (SP_DENCITY_MAX - in_NewRelations->DensityAtBasePoint) * in_NewRelations->RelativeSpeed_3D.Z * 0.5;
							break;
						case 3: // ветер влияет прямо пропорционально относительной скорости 
							in_NewRelations->Wind_3D.X = in_NewRelations->RelativeSpeed_3D.X / in_NewRelations->DensityAtBasePoint;
							in_NewRelations->Wind_3D.Y = in_NewRelations->RelativeSpeed_3D.Y / in_NewRelations->DensityAtBasePoint;
							in_NewRelations->Wind_3D.Z = in_NewRelations->RelativeSpeed_3D.Z / in_NewRelations->DensityAtBasePoint;
							break;
						}
					}

					if (m_pCalc->DensityDerivLevel > 2)
					{
						in_NewRelations->AccelerationDerivValue = DistancePwr1tmp - in_NewRelations->RelativeSpeedValue; //если сближаются, то отнн. скорость меньше нуля, а обратная плотность увеличивается

						if (in_NewRelations->AccelerationDerivValue == 0.0) in_NewRelations->AccelerationDerivValue = 1.0e-30;

						double DensDerivPwr2 = in_NewRelations->AccelerationDerivValue * in_NewRelations->AccelerationDerivValue;
						in_NewRelations->AccelerationDerivValue = (DensDerivPwr2 - tmpDistancePwr2) / (tmpDistancePwr2 * DensDerivPwr2);

						in_NewRelations->AccelerationDeriv_3D.X = in_NewRelations->RelativeSpeed_3D.X * in_NewRelations->AccelerationDerivValue;
						in_NewRelations->AccelerationDeriv_3D.Y = in_NewRelations->RelativeSpeed_3D.Y * in_NewRelations->AccelerationDerivValue;
						in_NewRelations->AccelerationDeriv_3D.Z = in_NewRelations->RelativeSpeed_3D.Z * in_NewRelations->AccelerationDerivValue;
					}
				}
			}
		}
	}
	return ret;
}
//ret -3 если шаг >  m_pCalc->Time.MinStep и плотность < 0 или > 1
//in_CalcType == 0 - новый расчет. 1 - балансировка
int CSP_Group::CalcRelations(double in_TimePoint, SPRelation* in_CalcRelations, SPRelation* in_PrevRelations, int* OutData, int in_CalcType)
{
	int AddResult = 0;

	if (in_CalcRelations == NULL) in_CalcRelations = m_CurentRelations;

	if (m_Array && *m_Array)
	{
		if (in_CalcType > 1)
			for (int i = 0; i < m_Count; i++)
			{
				int RelationPos = i;
				for (int j = 0; j < m_Count; j++, RelationPos += m_maxCount)
				{
					if ((in_CalcRelations[RelationPos].State & SP_STATE_NEED_UPDATE))
						for (RelationPos = i, j = 0; j < m_Count; j++, RelationPos += m_maxCount)
						{
							in_CalcRelations[RelationPos].State |= SP_STATE_NEED_UPDATE;
						}

				}
			}


		int doNext = SP_STATE_INPROC;
		int TimeType = 2;
		if (in_TimePoint == m_Array[0]->GetCurTime()) TimeType = 1;
		else 
			if (in_TimePoint == m_Array[0]->m_NextData.TimePoint) TimeType = 0;

		for (int i = 0; i < m_Count && (doNext == SP_STATE_INPROC || in_CalcType); i++)
			if (m_Array[i] && (in_CalcType < 2 || (in_CalcRelations[i].State & SP_STATE_NEED_UPDATE)))
			{
				double out_KeyPoint = in_TimePoint;
				SData DataAtPoint = { 0 };
				if (TimeType == 0) DataAtPoint = m_Array[i]->m_NextData;
				else if (in_TimePoint > m_Array[0]->GetCurTime())
					m_Array[i]->GetNextDataByTime( &DataAtPoint, in_TimePoint, &out_KeyPoint);
				else
					m_Array[i]->GetPrevDataByTime(&DataAtPoint, in_TimePoint, &out_KeyPoint);

				int RelationPos = i;

				for (int j = 0; j < m_Count && (doNext == SP_STATE_INPROC || in_CalcType); j++, RelationPos += m_maxCount)
					if (i != j && m_Array[j])
					{
						AddResult |= CalcSingleRelation( &DataAtPoint, m_Array[j], in_TimePoint, in_PrevRelations ? in_PrevRelations + RelationPos : NULL, in_CalcRelations + RelationPos, in_CalcType);
						if ((AddResult & SP_STATE_WARNING_MASK))
							doNext = SP_STATE_ОК;
					}
			}
	}

	return AddResult;
}

int CSP_Group::CalcDataByCurRelations()
{
	return CalcDataByRelations(m_CurentRelations);
}


int CSP_Group::CalcDataByRelations(SPRelation* in_CalcRelations)
{
	int ret = SP_STATE_ERROR_MAIN;

	if (m_Array && m_Array[0] && m_Array[1] && in_CalcRelations)
	{
		SData *pCalcData = NULL;
		int RelationPos;

		for (int i = 0; i < m_Count ; i++)
			if (m_Array[i] && (pCalcData = in_CalcRelations == m_NextRelations ? &m_Array[i]->m_NextData : in_CalcRelations == m_CurentRelations ? m_Array[i]->GetCurData() : NULL))
			{
				RelationPos = i;
				pCalcData->Accel3D = { 0 };
				pCalcData->AccelDeriv3D = { 0 };
				pCalcData->Density = 0.0;

				for (int j = 0; j < m_Count; j++, RelationPos += m_maxCount)
					if (i != j && m_Array[j])
					{
						ret = CalcSingleDataByRelation(pCalcData, in_CalcRelations + RelationPos, m_pCalc);
					}
			}
	}

	return ret;
}

int CSP_Group::CalcSingleDataByRelation(SData* pCalcData, SPRelation* in_CalcRelations, SP_Calc* pCalc)
{
	int ret;

	if (pCalcData)
	{
		ret = SP_STATE_ОК;
		pCalcData->Accel3D.X += in_CalcRelations->Acceleration_3D.X;
		pCalcData->Accel3D.Y += in_CalcRelations->Acceleration_3D.Y;
		pCalcData->Accel3D.Z += in_CalcRelations->Acceleration_3D.Z;

		if (pCalc->DensityDerivLevel >= 3)
		{
			pCalcData->AccelDeriv3D.X += in_CalcRelations->AccelerationDeriv_3D.X;
			pCalcData->AccelDeriv3D.Y += in_CalcRelations->AccelerationDeriv_3D.Y;
			pCalcData->AccelDeriv3D.Z += in_CalcRelations->AccelerationDeriv_3D.Z;
		}

		pCalcData->Density += in_CalcRelations->DensityAtBasePoint;
	}
	else ret = SP_STATE_ERROR_MAIN;

	return ret;
}


int CSP_Group::Cmd_Add(const char* in_Command)
{
	int ret = 0;
	if (m_CmdArr)
	{
		EnterCriticalSection(&m_cs_Command);

		if (m_CmdArr && in_Command && *in_Command)
		{
			for (ret = 0; ret < M_CMD_MAXCOUNT && m_CmdArr[ret]; ret++);

			if (ret < M_CMD_MAXCOUNT)
			{
				int iLen = (int)strlen(in_Command);
				m_CmdArr[ret] = new char[iLen + 2];
				strcpy_s(m_CmdArr[ret], iLen + 1, in_Command);
				m_CmdArr[ret][iLen] = 0;
			}
		}

		LeaveCriticalSection(&m_cs_Command);
	}
	return ret;
}


int CSP_Group::Cmd_Execute(char* in_Command)
{
	int ret = 0;
	EnterCriticalSection(&m_cs_Command);
	if (in_Command && *in_Command)
	{
		TrimChar(in_Command, ' ');
		StrUpr_my(in_Command);

		if (*in_Command)
		{
			if (strstr(in_Command, "PAUSE") == in_Command && in_Command[5] <= ' ')
			{
				if ((m_CalcState & M_STATE_PAUSE)) ClearCalcState(M_STATE_PAUSE);
				else SetCalcState(M_STATE_PAUSE);
			}
			else if (strstr(in_Command, "CONTINUE") == in_Command && in_Command[8] <= ' ')
			{
				if ((m_CalcState & M_STATE_PAUSE)) ClearCalcState(M_STATE_PAUSE);
				else SetCalcState(M_STATE_PAUSE);
			}
			else if (strstr(in_Command, "DELAY") == in_Command && in_Command[5] <= ' ')
			{
				in_Command += 6;
				while (*in_Command == ' ' || *in_Command == '\t' || *in_Command == '=') in_Command++;
				sscanf_s(in_Command, "%i", &m_DelayMS);
				if (m_DelayMS < 0 || m_DelayMS > 10000) m_DelayMS = 0;
			}
			else if (strstr(in_Command, "ADD") == in_Command && in_Command[3] <= ' ')
			{
				in_Command += 3;
				while (*in_Command == ' ' || *in_Command == '\t' || *in_Command == '=') in_Command++;

				SData initData = { 0 };

				ReadCalcParam(in_Command, "POS", initData.position, 0.0);
				ReadCalcParam(in_Command, "SPEED", initData.Speed, 0.0);

				AddSpParticle(SP_DATA_ALL, &initData, 1, m_pCalc);
			}
			else if ((strstr(in_Command, "DEL") == in_Command && in_Command[3] <= ' ') || (strstr(in_Command, "DELETE") == in_Command && in_Command[6] <= ' '))
			{
				in_Command += 3; 
				if (*in_Command >= ' ') in_Command += 3;
				while (*in_Command == ' ' || *in_Command == '\t' || *in_Command == '=') in_Command++;
				
				int idParticle = atoi(in_Command);
				RemoveSpParticle(idParticle);
			}
			else if (strstr(in_Command, "STOP") == in_Command && in_Command[4] <= ' ')
			{
				ClearCalcState(M_STATE_CALC);
			}
			else if (strstr(in_Command, "NEXT") == in_Command && in_Command[4] <= ' ')
			{
				SetCalcState(M_STATE_DO_NEXT);
			}
			else if (strstr(in_Command, "SAVE") == in_Command && in_Command[4] <= ' ')
			{
				in_Command += 4;
				ReplaceChar(in_Command, '\t', ' ');
				TrimChar(in_Command, ' ');

				CMD_Param_File tmpfile;

				if (m_Array && *m_Array && m_Count && tmpfile.ReadParams(in_Command) && *tmpfile.FileName)
				{
					if (!tmpfile.OPEN())
					{
						CMD_Param_DataType tmpWhat;
						if (!tmpWhat.ReadParams(in_Command)) tmpWhat.SET_ALL_ON();

						char* StrOut = new char[10000];
						tmpWhat.CHECK(GetMaxTraceId());

						for (int Id = tmpWhat.StartId; Id < tmpWhat.EndId; Id += tmpWhat.StepId)
						{
							fprintf(tmpfile.pPile, "\nTraceID\t%d\t", Id);

							if (tmpWhat.GroupData)
							{
								GroupSummaryInfo GrInfo;
								GetGroupSummaryInfo(&GrInfo, Id);

								GrInfo.PRINT(StrOut, 10000);
								fprintf(tmpfile.pPile, "%s\n", StrOut);
							}

							if (tmpWhat.Trace)
							{
								for(int ParticleID = 0; ParticleID < m_Count; ParticleID++)
									if (m_Array[ParticleID] && m_Array[ParticleID]->m_Data)
									{
										fprintf(tmpfile.pPile, "ParticleID\t%d\t", ParticleID);
										SData *dt = m_Array[ParticleID]->GetDataById(Id);
										if (dt)	dt->PRINT(StrOut, 10000, SDATA_ALL);
										else	SData::PRINT_AS_NULL(StrOut, 10000, SDATA_ALL);

										fprintf(tmpfile.pPile, "%s\n", StrOut);
									}
							}
						}

						delete[] StrOut;
						tmpfile.CLOSE();
					}
				}
			}
			else if (strstr(in_Command, "REPORT") == in_Command && in_Command[6] <= ' ')
			{
				SetCalcState(M_STATE_DO_REPORT);
			}
			else if (strstr(in_Command, "REDRAW") == in_Command && in_Command[6] <= ' ')
			{
				if (m_pCalc->DlgTrace)
				{
					m_pCalc->DlgTrace->ShowWindow(SW_SHOW);
					m_pCalc->DlgTrace->RedrawMe();
				}
			}
			else if (strstr(in_Command, "SET") == in_Command && in_Command[3] <= ' ')
			{
				ReadSettingsGroup(in_Command, SGROUP_ALL, READPARAMS_NOT_USE_DEFS);
			}
			else if (strstr(in_Command, "?") == in_Command && in_Command[1] <= ' ')
			{
				MessageBox(m_pCalc->DlgTrace ? m_pCalc->DlgTrace->m_hWnd : GetDesktopWindow(), L"PAUSE \nCONTINUE \nDELAY msec\nADD POS.X POS.Y POS.Z SPEED.X SPEED.Y SPEED.Z\nDEL ParticleID\nSTOP \nNEXT \nREPORT", L"Commands list", MB_OK | MB_APPLMODAL);
			}


		}
	}
	LeaveCriticalSection(&m_cs_Command);
	return ret;
}
int CSP_Group::Cmd_Do(int in_Count)
{
	int ret = 0;
	if (m_CmdArr)
	{
		EnterCriticalSection(&m_cs_Command);

		while (*m_CmdArr && (in_Count--) != 0)
		{
			Cmd_Execute(*m_CmdArr);
			delete[] * m_CmdArr;
			CopyMemory(m_CmdArr, m_CmdArr + 1, sizeof(char*) * (M_CMD_MAXCOUNT - 1));
			m_CmdArr[M_CMD_MAXCOUNT - 1] = NULL;
		}

		LeaveCriticalSection(&m_cs_Command);
	}
	return ret;
}

int CSP_Group::ReadSettingsGroup(char* in_Settings, int in_SettingsGroup, int in_UseDefs)
{
	int ret = 0; 
	
	if (m_pCalc && in_Settings && *in_Settings)
	{
		if ((in_SettingsGroup & SGROUP_TRACE))
		{
			ret += ReadCalcParam(in_Settings, "TRACE_SPEED_ARR", m_pCalc->DrawSet.ShowSpeedFor, 0xFFFFF, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "TRACE_ACC_ARR", m_pCalc->DrawSet.ShowAccFor, 0xFFFFF, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "TRACE_GROUP_ARR", m_pCalc->DrawSet.ShowGroupArrows, 0xFFFFF, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "TRACE_SHOWITEMS", m_pCalc->DrawSet.ShowItems, 0xFFFFF, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "TRACE_SHOWLINKSFROM", m_pCalc->DrawSet.ShowLinksFrom, 0x0001, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "TRACE_SHOWLINKSTO", m_pCalc->DrawSet.ShowLinksTo, 0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "TRACE_LINKS", m_pCalc->DrawSet.nLinks, 10, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "TRACE_REFRESH", m_pCalc->refresh.Delta, 5, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "TRACE_00_POS", m_pCalc->DrawSet.m_00_Pos, 0, in_UseDefs);

			if (ReadCalcParam(in_Settings, "TRACE_MAGN", m_pCalc->DrawSet.ViewMltpl_Ext, 0, READPARAMS_NOT_USE_DEFS) != 0 && m_pCalc->DlgTrace)
			{
					ret++;
					m_pCalc->DlgTrace->SetMltplSliderPos();//!!!!!!!!!!!!!!!!!!уйти от mfc из-зи проблемы с разделением потоков
			}

			if (ReadCalcParam(in_Settings, "TRACE_WPOS", m_pCalc->DrawSet.WindowPos, 0, READPARAMS_NOT_USE_DEFS) != 0 && m_pCalc->DlgTrace)
			{
					ret++;
					m_pCalc->DlgTrace->SetWindowPos(m_pCalc->DrawSet.WindowPos);//!!!!!!!!!!!!!!!!!!уйти от mfc из-зи проблемы с разделением потоков
			}
		}
		if ((in_SettingsGroup & SGROUP_STEP))
		{
			ret += ReadCalcParam(in_Settings, "NSTEPS", m_pCalc->nIterations, 2000000, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "MAXNSTEPS", m_maxCountLimit, 0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "MAXSTEP_VAL", m_pCalc->Time.MaxStep, 1000.0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "MINSTEP_VAL", m_pCalc->Time.MinStep, SP_PREC_TIME_MINSTEP, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "TIMESTEP", m_pCalc->Time.BaseStep, 0.001, in_UseDefs);



		}
		if ((in_SettingsGroup & SGROUP_MISC))
		{
			ret += ReadCalcParam(in_Settings, "OUTPUTFLAGS", m_pCalc->OutPutFlags, 0, in_UseDefs);

			if (ReadCalcParam(in_Settings, "FIND_REFRESH", m_pCalcFind->m_FIND_REFRESH, 5, in_UseDefs) > 0)
			{
				ret++;
				if (m_pCalcFind->m_FIND_REFRESH < 2000) m_pCalcFind->m_FIND_REFRESH *= 1000;
				if (m_pCalcFind->m_FIND_REFRESH > 300000) m_pCalcFind->m_FIND_REFRESH = 10000;
			}

			ret += ReadCalcParam(in_Settings, "CALC_SRANDSEED", m_pCalc->sRandSeed_Calc, -1, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "FIND_SRANDSEED", m_pCalcFind->sRandSeed, 0, in_UseDefs);


			int tmpret = 0;

			tmpret = ReadCalcParam(in_Settings, "CALC_MINPOS", m_pCalc->CalcFindSet.minPos, -100.0, in_UseDefs);
			tmpret += ReadCalcParam(in_Settings, "CALC_MAXPOS", m_pCalc->CalcFindSet.maxPos, 100.0, in_UseDefs);
			tmpret += ReadCalcParam(in_Settings, "CALC_MINSPEED", m_pCalc->CalcFindSet.minSpeed, -1.0, in_UseDefs);
			tmpret += ReadCalcParam(in_Settings, "CALC_MAXSPEED", m_pCalc->CalcFindSet.maxSpeed, 1.0, in_UseDefs);
			if (tmpret)
			{
				ret += tmpret;
				m_pCalc->CalcFindSet.maxSpeed.CUT(-1.0, 1.0);
				m_pCalc->CalcFindSet.minSpeed.CUT(-1.0, 1.0);

				m_pCalc->CalcFindSet.minSpeed.CUT(NULL, &m_pCalc->CalcFindSet.maxSpeed);
				m_pCalc->CalcFindSet.maxSpeed.CUT(&m_pCalc->CalcFindSet.minSpeed, NULL);
			}

			ret += ReadCalcParam(in_Settings, "CALC_STOP_TIME", m_pCalc->CalcFindSet.StopIF.Time, 1.0e7, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "CALC_STOP_MINRELDIST", m_pCalc->CalcFindSet.StopIF.minRelDist, 2000.0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "CALC_STOP_MINMAXRELSPEED", m_pCalc->CalcFindSet.StopIF.minMaxRelSpeed, 0.001, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "CALC_STOP_CURSTEP", m_pCalc->CalcFindSet.StopIF.TimeStep, 1000.0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "CALC_STOP_ITERATION", m_pCalc->CalcFindSet.StopIF.Iteration, 0x7FFF0000, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "CALC_EXCLUDEDISTANCE", m_pCalc->CalcFindSet.ExcludeDistance, 1000.0, in_UseDefs);


			tmpret = ReadCalcParam(in_Settings, "FIND_MINPOS", m_pCalcFind->Settings.minPos, -100.0, in_UseDefs);
			tmpret += ReadCalcParam(in_Settings, "FIND_MAXPOS", m_pCalcFind->Settings.maxPos, 100.0, in_UseDefs);
			tmpret += ReadCalcParam(in_Settings, "FIND_MINSPEED", m_pCalcFind->Settings.minSpeed, -1.0, in_UseDefs);
			tmpret += ReadCalcParam(in_Settings, "FIND_MAXSPEED", m_pCalcFind->Settings.maxSpeed, 1.0, in_UseDefs);
			if (tmpret)
			{
				ret += tmpret;
				m_pCalcFind->Settings.maxSpeed.CUT(-1.0, 1.0);
				m_pCalcFind->Settings.minSpeed.CUT(-1.0, 1.0);

				m_pCalcFind->Settings.minSpeed.CUT(NULL, &m_pCalcFind->Settings.maxSpeed);
				m_pCalcFind->Settings.maxSpeed.CUT(&m_pCalcFind->Settings.minSpeed, NULL);
			}

			ret += ReadCalcParam(in_Settings, "FIND_STOP_TIME", m_pCalcFind->Settings.StopIF.Time, 1.0e7, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "FIND_STOP_MINRELDIST", m_pCalcFind->Settings.StopIF.minRelDist, 2000.0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "FIND_STOP_MINMAXRELSPEED", m_pCalcFind->Settings.StopIF.minMaxRelSpeed, 0.001, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "FIND_STOP_CURSTEP", m_pCalcFind->Settings.StopIF.TimeStep, 1000.0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "FIND_STOP_ITERATION", m_pCalcFind->Settings.StopIF.Iteration, 0x7FFF0000, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "FIND_EXCLUDEDISTANCE", m_pCalcFind->Settings.ExcludeDistance, 1000.0, in_UseDefs);
		}

		if ((in_SettingsGroup & SGROUP_CALC_PARAM))
		{
			if (ReadCalcParam(in_Settings, "PARTICLE_COUNT", m_pCalc->InitialParticleCount, 2, in_UseDefs) > 0)
			{
				ret++;
				if (m_pCalc->InitialParticleCount < 2 || m_pCalc->InitialParticleCount > 500)
					m_pCalc->InitialParticleCount = 2;
			}

			ret += ReadCalcParam(in_Settings, "WINDTYPE", m_pCalc->WindType, 0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "COLLISIONTYPE", m_pCalc->CollisionType, 0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "DENCITYDERIVLEVEL", m_pCalc->DensityDerivLevel, 3, in_UseDefs);
				if (m_pCalc->DensityDerivLevel > 3 || m_pCalc->DensityDerivLevel < 1) m_pCalc->DensityDerivLevel = 3;
			ret += ReadCalcParam(in_Settings, "ACCCALCTYPE", m_pCalc->AccCalcType, 0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "CLASSICTYPE", m_pCalc->ClassicType, 0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "ADDITIVETYPE", m_pCalc->AdditiveType, 0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "DENSITYCALCTYPE", m_pCalc->DensityCalcType, 0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "MAXABSSPEED", m_pCalc->maxAbsSpeed,  1.0, in_UseDefs);
				if (ReadCalcParam(in_Settings, "MAXRELATIVESPEED", m_pCalc->maxRelativeSpeed, 2.0, in_UseDefs) > 0)
				{
					ret++;
					m_pCalc->maxAbsSpeedPwr2 = m_pCalc->maxAbsSpeed * m_pCalc->maxAbsSpeed;
				}
			ret += ReadCalcParam(in_Settings, "PARENTSPACETHICKNESS", m_pCalc->ParentSpaceThickness, 0.0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "CRITICALDISTANCE", m_pCalc->CriticalDistance, 1.0, in_UseDefs);
		}
	}

	return ret;
}




UINT DoCommandThread(LPVOID pParam)
{
	if (pParam)
	{
		CSP_Group* pGroup = (CSP_Group*)pParam;

		while (pGroup->m_innerState == M_INNER_STATE_STARTING)
			Sleep(1);

		if (pGroup->m_innerState == M_INNER_STATE_READY)
			pGroup->m_innerState = M_INNER_STATE_RUNING;

		while (pGroup->m_innerState == M_INNER_STATE_RUNING)
		{
			pGroup->Cmd_Do(-1);

			Sleep(100);
		}


		pGroup->m_innerState = M_INNER_STATE_STOPPED;
	}
	return 0;
}

