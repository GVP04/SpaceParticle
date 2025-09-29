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


int CSP_Group::AddSpParticle(int in_flags, const SData *in_Ptr, int in_Count, SP_Calc* pCalc)
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

int CSP_Group::SetParticleData(int in_Idx, int in_flags, const SData *in_Ptr)
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
		*out_Min = { 0 };
		*out_Max = { 0 };

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

void CSP_Group::CalcDensity(SData* DataAtPoint, SData& ViewPointData)
{
	if (m_Array && *m_Array)
	{
		SPos Distance = { 0 };
		SPos AddAcc = { 0 };

		//	if (m_pCalc->AdditiveType != 0)

		Distance.X = ViewPointData.position.X - DataAtPoint->position.X;
		Distance.Y = ViewPointData.position.Y - DataAtPoint->position.Y;
		Distance.Z = ViewPointData.position.Z - DataAtPoint->position.Z;

		if (m_pCalc->ParentSpaceThickness != 0.0)
		{
			double DistanceTmp = CSpParticle::Get3D_Pwr1(Distance);
			double tmpMltplr = (DistanceTmp + m_pCalc->ParentSpaceThickness) / DistanceTmp;
			Distance.X *= tmpMltplr;
			Distance.Y *= tmpMltplr;
			Distance.Z *= tmpMltplr;
		}

		double DistancePwr2 = CSpParticle::Get3D_Pwr2(Distance);
		double DistancePwr1 = sqrt(DistancePwr2);
		double DistancePwr3 = DistancePwr1 * DistancePwr2;

		if (DistancePwr2 > 0.0)
		{
			double DensityAtPoint;

			switch (m_pCalc->DensityCalcType)
			{
			case 0:
			default: //dens = 1 - 1 / S^2
				DensityAtPoint = 1.0 - 1.0 / DistancePwr2;
				if (DensityAtPoint < 0.0)
					DensityAtPoint -= 0.00000000001;

				break;
			case 1://dens = 1 - 1 / (1 + S^2)
				DensityAtPoint = 1.0 - 1.0 / (1.0 + DistancePwr2);
				break;
			case 2://dens = 1 - 1 / (1 + S) ^ 2
				DensityAtPoint = 1.0 - 1.0 / ((1.0 + DistancePwr1) * (1.0 + DistancePwr1));
				break;
			}


			if (m_pCalc->AdditiveType)
				DensityAtPoint *= m_CountDivider;

			DataAtPoint->Density += DensityAtPoint;

			SPos RelativeSpeed3D = { 0 };
			double RelativeSpeedValue = (*m_Array)->GetRelativeSpeed(*DataAtPoint, ViewPointData, &RelativeSpeed3D, m_pCalc);
			double RelativeSpeedValueAbs = fabs(RelativeSpeedValue);

			if (RelativeSpeedValueAbs < m_pCalc->maxRelativeSpeed) ///!!!!Очень спорное условие, учитывая скважность расчета
			{
				double AccAtPoint;

				switch (m_pCalc->AccCalcType)
				{
				case 0:
				default:
					AccAtPoint = 1.0 - DensityAtPoint;
					break;
				case 1: 
					AccAtPoint = 1.0 - DensityAtPoint * DensityAtPoint;
					break;
				//case 2:	
				//	AccAtPoint = 1.0 / DensityAtPoint;
				//	break;
				//case 3:
				//	AccAtPoint = 1.0 / (DensityAtPoint * DensityAtPoint);
				//	break;
				}

				double tmpAddMlpl = AccAtPoint / DistancePwr1;

				AddAcc.X = tmpAddMlpl * Distance.X;
				AddAcc.Y = tmpAddMlpl * Distance.Y;
				AddAcc.Z = tmpAddMlpl * Distance.Z;
				if (m_pCalc->WindType && RelativeSpeedValueAbs > 0.000000000001)
				{
					switch (m_pCalc->WindType)
					{
					case 0://OFF
					default:
						break;
					case 1: // ветер влияет прямо пропорционально относительной скорости 
						AddAcc.X -= AccAtPoint * RelativeSpeed3D.X;
						AddAcc.Y -= AccAtPoint * RelativeSpeed3D.Y;
						AddAcc.Z -= AccAtPoint * RelativeSpeed3D.Z;
						break;
					case 2:	// ветер влияет прямо пропорционально относительной скорости 
						AddAcc.X -= AccAtPoint * RelativeSpeed3D.X * 0.5;
						AddAcc.Y -= AccAtPoint * RelativeSpeed3D.Y * 0.5;
						AddAcc.Z -= AccAtPoint * RelativeSpeed3D.Z * 0.5;
						break;
					}
				}

				DataAtPoint->Accel3D.X += AddAcc.X;
				DataAtPoint->Accel3D.Y += AddAcc.Y;
				DataAtPoint->Accel3D.Z += AddAcc.Z;

				if (m_pCalc->DensityDerivLevel > 444)
				{
					double DensDeriv = DistancePwr1 - RelativeSpeedValue; //если сближаются, то отнн. скорость меньше нуля, а обратная плотность увеличивается

					if (DensDeriv == 0.0) DensDeriv = 1.0e-30;

					double DensDerivPwr2 = DensDeriv * DensDeriv;
					if (m_pCalc->AdditiveType != 0)
						DensDeriv = m_CountDivider * (DensDerivPwr2 - DistancePwr2) / (DistancePwr2 * DensDerivPwr2);
					else
						DensDeriv = (DensDerivPwr2 - DistancePwr2) / (DistancePwr2 * DensDerivPwr2);

					DataAtPoint->AccelDeriv3D.X += (DataAtPoint->Speed.X - ViewPointData.Speed.X) * DensDeriv;
					DataAtPoint->AccelDeriv3D.Y += (DataAtPoint->Speed.Y - ViewPointData.Speed.Y) * DensDeriv;
					DataAtPoint->AccelDeriv3D.Z += (DataAtPoint->Speed.Z - ViewPointData.Speed.Z) * DensDeriv;
				}
			}
		}
	}
}

int CSP_Group::RecalcDensityAt_(double atTime)
{
	int ret = 0;
	SData* DataAtPoint;
	if (m_Array && m_Count > 1 && m_Array[0] && m_Array[1])
	{
		if (m_CurTime == atTime && atTime > 0.0)
		{
			//////////!!!!!!!!!!!!!!!!!!!!!!!Если текущая или предыдущая и atTime > 0 то брать из Relations
			for (int i = 0; i < m_Count; i++)
				if (m_Array[i] && (DataAtPoint = m_Array[i]->GetCurData()))
				{
					DataAtPoint->Accel3D = { 0 };
					DataAtPoint->AccelDeriv3D = { 0 };
					DataAtPoint->Density = 0.0;

					for (int j = 0; j < m_Count; j++)
						if (i != j)
							CalcDensity(DataAtPoint, m_CurentRelations[i + m_maxCount * j].ViewPointData);
				}
		}
		else
		{
			for (int i = 0; i < m_Count; i++)
				if (m_Array[i] && (DataAtPoint = m_Array[i]->GetDataAtLowPoint(atTime)))
				{
					SData tmpData2;
					DataAtPoint->Accel3D = { 0 };
					DataAtPoint->AccelDeriv3D = { 0 };
					DataAtPoint->Density = 0.0;

					for (int j = 0; j < m_Count; j++)
						if (i != j && m_Array[j] && (m_Array[j]->FindViewPoint(&tmpData2, DataAtPoint, atTime, NULL) & SP_STATE_ERROR_MINOR) != SP_STATE_ERROR_MAIN)
							CalcDensity(DataAtPoint, tmpData2);
				}
		}
	}
	return m_Count;
}


void CSP_Group::GetMinMaxTracePos(SPos& min_Pos, SPos& max_Pos)
{
	min_Pos.X = min_Pos.Y = min_Pos.Z = 1.0e30;
	max_Pos.X = max_Pos.Y = max_Pos.Z = -1.0e30;

	for (int i = 0; i < m_Count; i++) 
		if (m_Array[i]) 
			m_Array[i]->GetMinMaxTracePos(min_Pos, max_Pos);
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
		int doNext = 1;
		int ABS_CHANGE_LS_flag = -1;

		while (doNext > 0)
		{
			double m_NextTime = m_CurTime + in_DeltaTime;
			double KeyPoint = m_NextTime;

			for (int i = 0; i < m_Count && doNext > 0; i++)
				if (m_Array[i])
				{
					m_Array[i]->NewPoint_Begin(m_NextTime);
					SData* curData = m_Array[i]->GetCurData();
					SData* pNextData = &(m_Array[i]->m_NextData);

					//расчет новой позиции
					int tmpRet = m_Array[i]->GetNextDataByTime(pNextData, m_NextTime, &KeyPoint);

					if ((tmpRet & SP_STATE_ABS_CHANGE_LS) || (m_NextTime != KeyPoint && m_NextTime - KeyPoint > m_pCalc->Time.MinStep))
					{
						if (ABS_CHANGE_LS_flag == i)
						{
							i = m_Count;
						}
						else
						{
							doNext = -3;
							ABS_CHANGE_LS_flag = i;
						}
					}
				}

			if (doNext > 0)
				doNext = CalcRelations(m_NextTime, m_NextRelations, m_CurentRelations);

			if (doNext < 0 && in_DeltaTime >= m_pCalc->Time.MinStep)
			{ //перезапуск с уменьшением шага
				for (int i = 0; i < m_Count; i++)
					if (m_Array[i]) m_Array[i]->NewPoint_Clear();

				if (doNext == -3)
				{
					doNext = 1;
					if (in_DeltaTime == KeyPoint - m_CurTime) KeyPoint -= m_pCalc->Time.MinStep;
					if (KeyPoint <= m_CurTime) KeyPoint = m_CurTime + m_pCalc->Time.MinStep;
					in_DeltaTime = KeyPoint - m_CurTime;
				}
				else
				{
					in_DeltaTime *= 0.6;
					doNext = 1;
				}
			}
			else
			{
				EnterCriticalSection(&m_pCalc->Group->m_cs);
				ret = SP_STATE_ОК;
				//фиксация результата и переход на следующую итерацию
				for (int i = 0; i < m_Count; i++)
					if (m_Array[i]) m_Array[i]->MoveToNewPoint(m_maxCountLimit);

				m_CurTime = m_NextTime;

				SPRelation* m_SwapRelations = m_CurentRelations;
				m_CurentRelations = m_NextRelations;
				m_NextRelations = m_PrevRelations;
				m_PrevRelations = m_SwapRelations;

				CalcDataByleRelations(m_CurentRelations);

				LeaveCriticalSection(&m_pCalc->Group->m_cs);

				doNext = -1;
			}
		}
	}
	return ret;
}



int CSP_Group::CalcSingleRelation(SData* in_PtrData, CSpParticle* in_PtrView, double in_ViewTime, SPRelation* in_PrevRelations, SPRelation* in_NewRelations)
{
	int ret = SP_STATE_ERROR_MAIN;

	if (in_PtrData && in_PtrView && in_NewRelations)
	{
		*in_NewRelations = { 0 };

		int tmpRet = in_PtrView->FindViewPoint(&in_NewRelations->ViewPointData, in_PtrData, in_ViewTime, in_PrevRelations);
		//if (in_PrevRelations && in_NewRelations)
		//	in_NewRelations->ViewPointData.Density = in_PrevRelations->ViewPointData.Density;

		if ((tmpRet & SP_STATE_ERROR_MINOR) == SP_STATE_ERROR_MAIN)
		{
			ret = SP_STATE_ERROR_MAIN;
		}
		else
		{
			in_NewRelations->RelativeSpeed_3D = in_PtrData->Speed;
			in_NewRelations->RelativeSpeed_3D.X -= in_NewRelations->ViewPointData.Speed.X;
			in_NewRelations->RelativeSpeed_3D.Y -= in_NewRelations->ViewPointData.Speed.Y;
			in_NewRelations->RelativeSpeed_3D.Z -= in_NewRelations->ViewPointData.Speed.Z;

			in_NewRelations->RelativeSpeedValue = in_PtrView->GetRelativeSpeed(*in_PtrData, in_NewRelations->ViewPointData, NULL, m_pCalc);
			in_NewRelations->RelativeSpeedValueAbs = fabs(in_NewRelations->RelativeSpeedValue);

			in_NewRelations->Distance_3D.X = in_NewRelations->ViewPointData.position.X - in_PtrData->position.X;
			in_NewRelations->Distance_3D.Y = in_NewRelations->ViewPointData.position.Y - in_PtrData->position.Y;
			in_NewRelations->Distance_3D.Z = in_NewRelations->ViewPointData.position.Z - in_PtrData->position.Z;

			double DistancePwr1tmp = CSpParticle::Get3D_Pwr1(in_NewRelations->Distance_3D);
			if (m_pCalc->ParentSpaceThickness != 0.0)
			{
				double tmpMltplr = (DistancePwr1tmp + m_pCalc->ParentSpaceThickness) / DistancePwr1tmp;
				in_NewRelations->Distance_3D.X *= tmpMltplr;
				in_NewRelations->Distance_3D.Y *= tmpMltplr;
				in_NewRelations->Distance_3D.Z *= tmpMltplr;
				DistancePwr1tmp += m_pCalc->ParentSpaceThickness;
			}

			if (1.0 - m_pCalc->ParentSpaceThickness >= DistancePwr1tmp)
				ret |= SP_STATE_ERROR_MINOR | SP_STATE_PHANTOM_DIST_LE1;

			in_NewRelations->DistancePwr2 = DistancePwr1tmp * DistancePwr1tmp;
			in_NewRelations->DistancePwr1 = DistancePwr1tmp;
			in_NewRelations->DistancePwr3 = in_NewRelations->DistancePwr1 * in_NewRelations->DistancePwr2;

			if (in_PrevRelations &&
				((in_NewRelations->RelativeSpeedValueAbs >= m_pCalc->maxRelativeSpeed && in_PrevRelations->RelativeSpeedValueAbs < m_pCalc->maxRelativeSpeed)
					|| (in_NewRelations->RelativeSpeedValueAbs < m_pCalc->maxRelativeSpeed && in_PrevRelations->RelativeSpeedValueAbs >= m_pCalc->maxRelativeSpeed)))
				ret |= SP_STATE_ERROR_MINOR | SP_STATE_REL_CHANGE_LS;


			if (in_NewRelations->DistancePwr2 > 0.0)
			{
				switch (m_pCalc->DensityCalcType)
				{
				case 0:
				default: //dens = 1 - 1 / S^2
					in_NewRelations->DensityAtBasePoint = 1.0 - 1.0 / in_NewRelations->DistancePwr2;
					if (in_NewRelations->DensityAtBasePoint < 0.0)
						in_NewRelations->DensityAtBasePoint -= 0.00000000001;

					break;
				case 1://dens = 1 - 1 / (1 + S^2)
					in_NewRelations->DensityAtBasePoint = 1.0 - 1.0 / (1.0 + in_NewRelations->DistancePwr2);
					break;
				case 2://dens = 1 - 1 / (1 + S) ^ 2
					in_NewRelations->DensityAtBasePoint = 1.0 - 1.0 / ((1.0 + in_NewRelations->DistancePwr1) * (1.0 + in_NewRelations->DistancePwr1));
					break;
				case 3:			 //dens = 1 - 1 / S^2
					in_NewRelations->DensityAtBasePoint = 1.0 - 1.0 / in_NewRelations->DistancePwr1;
					if (in_NewRelations->DensityAtBasePoint < 0.0)
						in_NewRelations->DensityAtBasePoint -= 0.00000000001;
				case 4:			 //dens = 1 - 1 / S^2
					in_NewRelations->DensityAtBasePoint = 1.0 - 1.0 / (1.0 + in_NewRelations->DistancePwr1);
					if (in_NewRelations->DensityAtBasePoint < 0.0)
						in_NewRelations->DensityAtBasePoint -= 0.00000000001;

					break;
				}


				if (in_NewRelations->RelativeSpeedValueAbs < m_pCalc->maxRelativeSpeed) ///!!!!Очень спорное условие, учитывая скважность расчета
				{
					switch (m_pCalc->AccCalcType)
					{
					case 0:
					default:
						in_NewRelations->AccelerationValue = 1.0 - in_NewRelations->DensityAtBasePoint;
						break;
					case 1:
						in_NewRelations->AccelerationValue = 1.0 - in_NewRelations->DensityAtBasePoint * in_NewRelations->DensityAtBasePoint;
						break;
					case 2:
						in_NewRelations->AccelerationValue = 1.0 / in_NewRelations->DensityAtBasePoint;
						break;
					}

					double tmpAddMlpl = in_NewRelations->AccelerationValue / in_NewRelations->DistancePwr1;

					in_NewRelations->Acceleration_3D.X = tmpAddMlpl * in_NewRelations->Distance_3D.X;
					in_NewRelations->Acceleration_3D.Y = tmpAddMlpl * in_NewRelations->Distance_3D.Y;
					in_NewRelations->Acceleration_3D.Z = tmpAddMlpl * in_NewRelations->Distance_3D.Z;

					if (m_pCalc->WindType && in_NewRelations->RelativeSpeedValueAbs > 0.0000000000001)
					{
						switch (m_pCalc->WindType)
						{
						case 0://OFF
						default:
							break;
						case 1: // ветер влияет прямо пропорционально относительной скорости 
							in_NewRelations->Wind_3D.X = (1.0 - in_NewRelations->DensityAtBasePoint) * in_NewRelations->RelativeSpeed_3D.X;
							in_NewRelations->Wind_3D.Y = (1.0 - in_NewRelations->DensityAtBasePoint) * in_NewRelations->RelativeSpeed_3D.Y;
							in_NewRelations->Wind_3D.Z = (1.0 - in_NewRelations->DensityAtBasePoint) * in_NewRelations->RelativeSpeed_3D.Z;
							break;
						case 2:	// ветер влияет прямо пропорционально относительной скорости 
							in_NewRelations->Wind_3D.X = (1.0 - in_NewRelations->DensityAtBasePoint) * in_NewRelations->RelativeSpeed_3D.X * 0.5;
							in_NewRelations->Wind_3D.Y = (1.0 - in_NewRelations->DensityAtBasePoint) * in_NewRelations->RelativeSpeed_3D.Y * 0.5;
							in_NewRelations->Wind_3D.Z = (1.0 - in_NewRelations->DensityAtBasePoint) * in_NewRelations->RelativeSpeed_3D.Z * 0.5;
							break;
						case 3: // ветер влияет прямо пропорционально относительной скорости 
							in_NewRelations->Wind_3D.X = in_NewRelations->RelativeSpeed_3D.X / in_NewRelations->DensityAtBasePoint;
							in_NewRelations->Wind_3D.Y = in_NewRelations->RelativeSpeed_3D.Y / in_NewRelations->DensityAtBasePoint;
							in_NewRelations->Wind_3D.Z = in_NewRelations->RelativeSpeed_3D.Z / in_NewRelations->DensityAtBasePoint;
							break;
						}
					}

					if (m_pCalc->DensityDerivLevel > 444)
					{
						in_NewRelations->AccelerationDerivValue = in_NewRelations->DistancePwr1 - in_NewRelations->RelativeSpeedValue; //если сближаются, то отнн. скорость меньше нуля, а обратная плотность увеличивается

						if (in_NewRelations->AccelerationDerivValue == 0.0) in_NewRelations->AccelerationDerivValue = 1.0e-30;

						double DensDerivPwr2 = in_NewRelations->AccelerationDerivValue * in_NewRelations->AccelerationDerivValue;
						in_NewRelations->AccelerationDerivValue = (DensDerivPwr2 - in_NewRelations->DistancePwr2) / (in_NewRelations->DistancePwr2 * DensDerivPwr2);

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

int CSP_Group::CalcRelations(double in_TimePoint, SPRelation* in_CalcRelations, SPRelation* in_PrevRelations)
{
	int doNext = 1;

	if (in_CalcRelations == NULL) in_CalcRelations = m_CurentRelations;

	if (m_Array && *m_Array && in_CalcRelations)
	{
		int TimeType = 2;
		if (in_TimePoint == m_Array[0]->m_NextData.TimePoint) TimeType = 0;
		else if (in_TimePoint == m_Array[0]->GetCurTime()) TimeType = 1;


		double DeltaTime = 0.0;
		if (in_PrevRelations) DeltaTime = in_TimePoint - in_PrevRelations[1].ViewPointData.TimePoint;


		for (int i = 0; i < m_Count && doNext > 0; i++)
			if (m_Array[i])
			{
				double out_KeyPoint = in_TimePoint;
				SData DataAtPoint = { 0 };
				if (TimeType == 0) DataAtPoint = m_Array[i]->m_NextData;
				else if (in_TimePoint > m_Array[0]->GetCurTime())
					m_Array[i]->GetNextDataByTime( &DataAtPoint, in_TimePoint, &out_KeyPoint);
				else
					m_Array[i]->GetPrevDataByTime(&DataAtPoint, in_TimePoint, &out_KeyPoint);

				int relationPos = i;

				for (int j = 0; j < m_Count && doNext > 0; j++, relationPos += m_maxCount)
					if (i != j && m_Array[j])
					{
						int AddResult = CalcSingleRelation( &DataAtPoint, m_Array[j], in_TimePoint, in_PrevRelations ? in_PrevRelations + relationPos : NULL, in_CalcRelations + relationPos);

						if ((AddResult & SP_STATE_ERROR_MINOR) == SP_STATE_ERROR_MINOR && DeltaTime > m_pCalc->Time.MinStep)
						{//выход за пределы вычисленной траектории, изменение относительной или абсолютной скрости с/на предельную, нижняя граница фантомного расстояния
							if ((AddResult & SP_STATE_REL_CHANGE_LS))
								doNext = -4;
							else
								if ((AddResult & SP_STATE_HIGH_VIEW))
									doNext = -5;
								else
									if ((AddResult & SP_STATE_PHANTOM_DIST_LE1) && (m_CurentRelations + relationPos)->DistancePwr1 > 1.0 - m_pCalc->ParentSpaceThickness)
										doNext = -6;
									else
										doNext = 1;
						}
						//Добавить контроль на точки перегиба значения дистанции между точкой и точками видимости
					}
				if (doNext > 0 && m_Array[i]->NewPoint_CheckLevel() < 0) doNext = -2;
			}
	}

	return doNext;
}


int CSP_Group::CalcDataByleRelations(SPRelation* in_CalcRelations)
{
	int ret = SP_STATE_ERROR_MAIN;

	if (m_Array && m_Array[0] && m_Array[1] && in_CalcRelations)
	{
		SData *pCalcData = NULL;
		int relationPos;

		for (int i = 0; i < m_Count ; i++)
			if (m_Array[i] && (pCalcData = in_CalcRelations == m_NextRelations ? &m_Array[i]->m_NextData : in_CalcRelations == m_CurentRelations ? m_Array[i]->GetCurData() : NULL))
			{
				relationPos = i;

				for (int j = 0; j < m_Count; j++, relationPos += m_maxCount)
					if (i != j && m_Array[j])
					{
						ret = CalcSingleDataByleRelation(pCalcData, in_CalcRelations + relationPos, m_pCalc);
					}
			}

		//relationPos = 0;
		//int deltaPos = m_maxCount + 1;
		//for (int i = 0; i < m_Count; i++, relationPos += deltaPos)
		//	if ((pCalcData = in_CalcRelations == m_NextRelations ? &m_Array[i]->m_NextData : in_CalcRelations == m_CurentRelations ? m_Array[i]->GetCurData() : NULL)))
		//		in_CalcRelations[relationPos].ViewPointData = *pCalcData;

	}

	return ret;
}

int CSP_Group::CalcSingleDataByleRelation(SData* pCalcData, SPRelation* in_CalcRelations, SP_Calc* pCalc)
{
	int ret = SP_STATE_ERROR_MAIN;

	if (pCalcData)
	{
		ret = SP_STATE_ОК;
		pCalcData->Accel3D.X += in_CalcRelations->Acceleration_3D.X;
		pCalcData->Accel3D.Y += in_CalcRelations->Acceleration_3D.Y;
		pCalcData->Accel3D.Z += in_CalcRelations->Acceleration_3D.Z;

		pCalcData->AccelDeriv3D.X += in_CalcRelations->AccelerationDeriv_3D.X;
		pCalcData->AccelDeriv3D.Y += in_CalcRelations->AccelerationDeriv_3D.Y;
		pCalcData->AccelDeriv3D.Z += in_CalcRelations->AccelerationDeriv_3D.Z;

		pCalcData->Density += in_CalcRelations->DensityAtBasePoint;
	}

	return ret;
}
