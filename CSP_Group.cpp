#include "pch.h"


int mRGB[16]= {
	RGB(200, 0, 0),RGB(0, 200, 0),RGB(0, 0, 200),RGB(200, 200, 0),RGB(200, 0, 200),RGB(0, 200, 200),
	RGB(200, 100, 0),RGB(200, 0, 100),RGB(200, 100, 100),RGB(100, 200, 0),RGB(0, 200, 100),RGB(100, 0, 200),
	RGB(0, 100, 200),RGB(0, 100, 100),RGB(100, 0, 100),RGB(100, 100, 0),
};


#include "CSP_Group.h"


CSP_Group::CSP_Group()
{
	m_name = new char[20000];
	sprintf_s(m_name, 1000, "GROUP_0");
	m_tmpString = m_name + 1000;
	*m_tmpString = 0;
	m_maxCount = 10;
	m_maxCountLimit = 0;
	m_pCalc = NULL;
	InitializeCriticalSection(&m_cs);


	m_Array = new CSpParticle * [m_maxCount];
	ZeroMemory(m_Array,  sizeof(CSpParticle *) * m_maxCount);

	int nRelations = m_maxCount * m_maxCount;

	m_Relations = new SPRelation [nRelations * 4];
	ZeroMemory(m_Relations,  sizeof(SPRelation) * nRelations * 4);
	m_CurentRelations = m_Relations;
	m_NextRelations = m_CurentRelations + nRelations;
	m_PrevRelations = m_NextRelations + nRelations;

	m_Count = 0;
	CalcCountDevider();
	m_CurTime = 0.0;
}
CSP_Group::~CSP_Group()
{
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

	DeleteCriticalSection(&m_cs);
}

void CSP_Group::Reset()
{
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

	int nRelations = m_maxCount * m_maxCount;

	m_Relations = new SPRelation[nRelations * 4];
	ZeroMemory(m_Relations, sizeof(SPRelation) * nRelations * 4);
	m_CurentRelations = m_Relations;
	m_NextRelations = m_CurentRelations + nRelations;
	m_PrevRelations = m_NextRelations + nRelations;

	m_Count = 0;
	CalcCountDevider();
	m_CurTime = 0.0;
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
	ZeroMemory(m_Relations, sizeof(SPRelation) * nRelations * 4);
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

		if ((in_Percents < 0.0 && in_Spread >= 100.0 ) || !m_Array || !m_Array[0] || !m_Array[0]->m_Data || !m_Array[1])
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
			for (j = 0; j < m_Count && (i == j || m_CurentRelations[i + m_maxCount * j].DistancePwr1 > in_RelDist); j++);

			if (j >= m_Count) 
				ret = i; 
		}
	}
	return ret;
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

		EnterCriticalSection(&m_pCalc->Group->m_cs);

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

		LeaveCriticalSection(&m_pCalc->Group->m_cs);

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

			*in_NewRelations = { 0 };
			
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
