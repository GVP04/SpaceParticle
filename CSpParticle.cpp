#include "pch.h"
#include "CSpParticle.h"
#include "CSDataArray.h"
#include "CSP_Group.h"


CSpParticle::CSpParticle(SP_Calc& in_pCalc, int in_TypeFlags)
{
	m_TypeFlags = in_TypeFlags;
	m_StateNext = m_StateCurr = m_StatePrev = 0;
	m_pCalc = &in_pCalc;

	m_NewPointState = SP_NEWPOINT_STATE_ON;
	NewPoint_Clear();
	m_NextData = { 0 };
	m_Data = new SPTraceArrayDynEx<SData, double>(m_pCalc->nIterations < 5 ? SDA_DEF_AMOUNT : m_pCalc->nIterations, &m_pCalc->Time);
	m_CountBeStuck = 0;
}

CSpParticle::CSpParticle(int in_MaxItems, int in_TypeFlags)
{
	m_TypeFlags = in_TypeFlags;
	m_StateNext = m_StateCurr = m_StatePrev = 0;
	m_NewPointState = SP_NEWPOINT_STATE_ON;
	NewPoint_Clear();
	m_NextData.CLEAR();
	m_Data = new SPTraceArrayDynEx<SData, double>(in_MaxItems < 5 ? 1000 : in_MaxItems + 100, &m_pCalc->Time);
}

CSpParticle::~CSpParticle()
{
	if (m_Data) delete m_Data;
	m_Data = nullptr;
}

void CSpParticle::Clear(int in_Flags)
{
	m_StateNext = m_StateCurr = m_StatePrev = 0;
	m_CountBeStuck = 0;

	if (m_Data) m_Data->Clear(in_Flags);
	else
	if (m_pCalc)
		m_Data = new SPTraceArrayDynEx<SData, double>(m_pCalc->nIterations < 5 ? SDA_DEF_AMOUNT + 100: m_pCalc->nIterations + 100, &m_pCalc->Time);

	NewPoint_Clear();
	m_NextData.CLEAR();
}

SData* CSpParticle::GetDataById(int in_Id)
{
	return m_Data == nullptr ? nullptr : m_Data->GetDataById(in_Id);
}

void CSpParticle::GetLastDeltaAcc(SPos& in_DeltaAcc)
{
	in_DeltaAcc.SET_AS_DELTA(m_Data->GetCurData()->Accel_2_3D, m_Data->GetPrevData()->Accel_2_3D);
}


int CSpParticle::NewPoint_Begin(double in_NewTimePoint)
{
	int ret = 0;
	NewPoint_Clear();
	ASSERT(m_Data->GetCurTime() < in_NewTimePoint || in_NewTimePoint == 0.0);
	if (m_Data)
	{
		ret = 1;
		m_NewPointState = SP_NEWPOINT_STATE_ON;
		m_NextData.TimePoint = in_NewTimePoint;
	}
	return ret;
}

double CSpParticle::GetCurTime() 
{
	return m_Data->GetCurTime();
}

double CSpParticle::GetPrevTime(int n_StepsBefore)
{
	double ret = -1.0;
	int CurPos;
	if (m_Data && (CurPos = m_Data->GetCurPos()) >= 0)
	{
		int PointsIdx = CurPos - n_StepsBefore;

		SData* sd = m_Data->GetDataById(PointsIdx);
		if (sd)
			ret = sd->TimePoint;
	}

	return ret;
}

int CSpParticle::MoveToNewPoint(int AddType)
{
	int ret = 0;
	if (m_Data && (m_NewPointState & SP_NEWPOINT_STATE_ON))
	{
		m_Data->AddData(m_NextData, AddType);
		m_StatePrev = m_StateCurr;
		m_StateCurr = m_StateNext;

		NewPoint_Clear();
		ret = 1;
	}
	else
	{
		ASSERT(0);
		ret = 0;//!!!!!!check
	}

	return ret;
}

void CSpParticle::NewPoint_Clear()
{
	m_NewPointState = SP_NEWPOINT_STATE_OFF;
	m_StateNext = 0;

	//m_NextData = { 0 };
}


char* CSpParticle::GetStateDescription(char* out_Str, int in_StrLen, int in_nPoints)
{
	if (out_Str)
	{
		*out_Str = 0;
		char* initialPos = out_Str;
		if (m_Data)
		{
			if (m_Data->GetCurPos() >= 0)
			{
				int tmpCurPos = m_Data->GetCurPos();
				int PointsIdx = tmpCurPos - in_nPoints + 1;

				if (PointsIdx > tmpCurPos || PointsIdx < 0 || in_nPoints < 0)  PointsIdx = 0;

				while (PointsIdx <= tmpCurPos)
				{
					SData *tmpData = GetDataById(PointsIdx);
					out_Str = GetFlagsDescription(out_Str, 0);
					out_Str += sprintf_s(out_Str, in_StrLen - ((int)(out_Str - initialPos)), "\tTime=\t%g\tTraceCount=%d\t", tmpData ? tmpData->TimePoint : 0.0, tmpCurPos);
					if (tmpData)
						out_Str += tmpData->PRINT(out_Str, in_StrLen - ((int)(out_Str - initialPos)), SDATA_ALL);
					else
						out_Str += tmpData->PRINT_AS_NULL(out_Str, in_StrLen - ((int)(out_Str - initialPos)), SDATA_ALL);


					if (PointsIdx++ < tmpCurPos)
						out_Str += sprintf_s(out_Str, in_StrLen - ((int)(out_Str - initialPos)), "\n");
				}
			}
			else sprintf_s(out_Str, in_StrLen - ((int)(out_Str - initialPos)), "CurPos<0");
		}
		else sprintf_s(out_Str, in_StrLen - ((int)(out_Str - initialPos)), "m_Data==nullptr");
	}

	return out_Str;
}


char* CSpParticle::GetStateDescription(char* out_Str, int in_StrLen, double in_ReportTime)
{
	if (out_Str)
	{
		*out_Str = 0;
		if (m_Data)
		{
			int tmpCurPos = m_Data->GetCurPos();
			if (tmpCurPos >= 0)
			{
				SData tmpData = { 0 };
				double tmpTime = 0.0;
				int ret = GetPrevDataByTime(tmpData, in_ReportTime, &tmpTime);
				out_Str = GetFlagsDescription(out_Str, ret);
				if ((ret & SP_STATE_ERROR_MINOR) != SP_STATE_ERROR_MAIN)
				{
					out_Str += sprintf_s(out_Str, 1000, "\tTime=\t%.10g\tTraceCount=%d", tmpData.TimePoint, tmpCurPos);
					out_Str += sprintf_s(out_Str, 1000, "\tPosition=\t%.10g\t%.10g\t%.10g", tmpData.Position.X, tmpData.Position.Y, tmpData.Position.Z);
					out_Str += sprintf_s(out_Str, 1000, "\tSpeed=\t%.10g\t%.10g\t%.10g", tmpData.Velocity.X, tmpData.Velocity.Y, tmpData.Velocity.Z);
					out_Str += sprintf_s(out_Str, 1000, "\tDensity3D=\t%.10g\t%.10g\t%.10g", tmpData.Accel_2_3D.X, tmpData.Accel_2_3D.Y, tmpData.Accel_2_3D.Z);
					out_Str += sprintf_s(out_Str, 1000, "\tDensityDeriv3D=\t%.10g\t%.10g\t%.10g", tmpData.Jerk_3_3D.X, tmpData.Jerk_3_3D.Y, tmpData.Jerk_3_3D.Z);
				}
			}
			else sprintf_s(out_Str, 1000, "CurPos<0");
		}
		else sprintf_s(out_Str, 1000, "m_Data==nullptr");
	}

	return out_Str;
}

//напоминание -- время изменения плотности == расстоянию
int CSpParticle::FindViewPointByTime(SData& out_Data, const SData& in_Pos, double in_ViewTime, double in_StartTime, double in_EndTime, int* const out_LowIdx)
{
	ASSERT(in_StartTime < in_EndTime);

	int ret = SP_STATE_ОК;
	SData* VPDataID0 = GetDataById(0);

	if (VPDataID0)
	{
		if (m_pCalc->ClassicType)
		{
			const SData* VPDataCur = GetCurData_C();
			if (VPDataCur->TimePoint <= in_ViewTime)
			{
				if (m_pCalc->ClassicType == 2 && (m_NewPointState & SP_NEWPOINT_STATE_ON))
					out_Data = m_NextData;
				else
					out_Data = *VPDataCur;
				if (out_LowIdx) *out_LowIdx = GetCurPos();
			}
			else
			{	///добавить вычисление данных если  m_Data->m_Array[iii].TimePoint < in_ViewTime
				SData* tmpData;
				int iii = m_Data->GetLowIdxByTime(in_ViewTime);
				if (out_LowIdx) *out_LowIdx = iii;

				if (iii >= 0 && (tmpData = GetDataById(iii)))
					out_Data = *tmpData;
			}
		}
		else
		{
			int FlagFind = 1;
			if (in_StartTime <= VPDataID0->TimePoint)
			{
				double distanceTMP = VPDataID0->Position.GetDistance(in_Pos.Position);
				double periodTMP = in_ViewTime - VPDataID0->TimePoint;// -m_pCalc->CriticalDistance;
				if (distanceTMP >= periodTMP)
				{//VP до сохраненной траектории
					ret |= FindViewPointByTimeLinear(out_Data, in_Pos, in_ViewTime, in_StartTime);
					if (out_Data.TimePoint >= VPDataID0->TimePoint)
						FlagFind = 1;
					else
						FlagFind = 0;
				}
			}

			if (FlagFind)
			{
				int idMin = FindViewPointMinIdByTime(in_Pos.Position, in_ViewTime, in_StartTime, in_EndTime);
				if (out_LowIdx)
					*out_LowIdx = idMin;

				int CurPos = GetCurPos();

				if (idMin < CurPos)
				{
					if (idMin >= 0)
					{// есть пересечение в диапазоне [idMin]...[idMin+1] /// простой линйный расчет без учета ускорения
						//правильно с учетом ускорения. ускорение ускорения пропорционально квадрату скорости.
						//пока расчет позиции по спрямленной 
						SData LowBorder = *GetDataById(idMin);
						SData UpBorder = *GetDataById(idMin + 1);
						double UpDelta, LowDelta;

						LowDelta = in_ViewTime - LowBorder.TimePoint - LowBorder.Position.GetDistance(in_Pos.Position);
						UpDelta = in_ViewTime - UpBorder.TimePoint - UpBorder.Position.GetDistance(in_Pos.Position);
						
						ASSERT(UpDelta <= 0.0 || UpBorder.AbsSpeed > 0.9999);		//расстояние больше времени. 
						ASSERT(LowDelta >= 0.0);		//расстояние меньше времени. 

						while (UpDelta < - m_pCalc->Time.GetMinStep() && LowDelta > m_pCalc->Time.GetMinStep())
						{
							SPos DeltaPos = UpBorder.Position; 
							DeltaPos -= LowBorder.Position;
							double RelMltpl = LowDelta / (LowDelta - UpDelta);
							DeltaPos *= RelMltpl;
							DeltaPos += LowBorder.Position;
							double MidTime = UpBorder.TimePoint - LowBorder.TimePoint;
							MidTime *= RelMltpl;
							MidTime += LowBorder.TimePoint;

							double MidDelta = in_ViewTime - MidTime - DeltaPos.GetDistance(in_Pos.Position);
							if (MidDelta < 0.0)
							{
								UpDelta = MidDelta;
								UpBorder.Position = DeltaPos;
								UpBorder.TimePoint = MidTime;
							}
							else
							{
								LowDelta = MidDelta;
								LowBorder.Position = DeltaPos;
								LowBorder.TimePoint = MidTime;
							}
						}

						if (UpDelta >= -m_pCalc->Time.GetMinStep())
						{
							out_Data = UpBorder;
						}
						else
						{
							out_Data = LowBorder;
						}
						//Через квадратное уравнение
						//SData Up, Dn, Mid;
						//Dn = LowBorder;
						//Up = UpBorder;
						//Dn.Accel_2_3D = {0}; Up.Accel_2_3D = { 0 };
						//Dn.Velocity.SET_AS_DELTA(UpBorder.Position, LowBorder.Position);
						//Dn.Velocity.DIV((UpBorder.TimePoint - LowBorder.TimePoint));
						//Up.Velocity = Dn.Velocity;
						//Mid = Dn;
						//double tSwap, maxDelta = m_pCalc->Time.GetMinStep();
						//ret = SP_STATE_INPROC;
						//double DeltaT13 = in_ViewTime - Mid.TimePoint;
						//while (ret == SP_STATE_INPROC)
						//{
						//	double newTime2;
						//	SPos DP = Mid.Position; 
						//	DP.MINUS(in_Pos.Position);
						//	double C = DP.LENPWR2() - DeltaT13 * DeltaT13;
						//	double B =  DeltaT13 + DP.DOT(Mid.Velocity);
						//	B += B;
						//	double A = Mid.Velocity.LENPWR2() - 1.0;
						//	TwoDoubleRoots result = QuadraticEequationSolution( A, B, C);
						//	newTime2 = Mid.TimePoint + result.root1.Dbl;
						//	switch (CalcMidData(Mid, Dn, Up, newTime2))
						//	{
						//	case SP_STATE_UP:
						//	case SP_STATE_DOWN:
						//		ret = SP_STATE_ОК;
						//		break;
						//	case SP_STATE_MID:
						//		DeltaT13 = in_ViewTime - Mid.TimePoint;
						//		tSwap = DeltaT13 - in_Pos.Position.GetDistance(Mid.Position);
						//		if (tSwap < 0.0)
						//			Up = Mid;	
						//		else
						//			Dn = Mid;
						//		if (Up.TimePoint - Dn.TimePoint < maxDelta || fabs(tSwap) < maxDelta)
						//			ret = SP_STATE_ОК;
						//		break;
						//	default: //error
						//		ret = SP_STATE_ERROR_MAIN;
						//		break;
						//	}
						//}
						//if (!(ret & SP_STATE_ERROR_MAIN))
						//{
						//	out_Data = Mid;
						//	if (out_LowIdx && UpBorder.TimePoint == Mid.TimePoint)
						//		*out_LowIdx = idMin + 1;
						//}
					}
				}
				else
				{//время больше времени в последней позиции масиива траектории
					ret = SP_STATE_HIGH_VIEW | SP_STATE_ERROR_MINOR;
					double KeyPoint = 0.0;
					GetNextDataByTime(out_Data, in_ViewTime, &KeyPoint);
				}

				if (out_Data.TimePoint < in_StartTime && !(ret & SP_STATE_FAR_AWAY))
				{
					ret |= SP_STATE_FAR_AWAY;
					//out_Data->SET_AS_FARAWAY();
				}
			}
			ASSERT(!isnan(out_Data.TimePoint));
		}
	}
	else
	{
		ret = SP_STATE_ERROR_MAIN;
	}


	return ret;
}



int CSpParticle::FindViewPointByTimeLinear(SData & out_Data, const SData & in_Pos, double in_ViewTime, double in_StartTime)
{
	int ret = -1;
	SData* VPDataID0 = GetDataById(0);
	if (in_StartTime <= VPDataID0->TimePoint)
	{
		double LinearViewTime = GetLinearViewTime(in_Pos, *VPDataID0);
		if (fabs(LinearViewTime) < SP_MAX_TIME_DISTANCE)
		{
			ret = 0;
			out_Data = *VPDataID0;
			out_Data.LinearMove(in_ViewTime + LinearViewTime - VPDataID0->TimePoint, LINEARMOVECLEARFLAG_ACCEL3D | LINEARMOVECLEARFLAG_ACCELDERIV3D);
		}
		else
		{
			ret = SP_STATE_FAR_AWAY;
			out_Data.SET_AS_FARAWAY();
		}
	}
	return ret;
}

int CSpParticle::FindViewPointMinIdByTime(const SPos& in_Pos, double in_ViewTime, double in_StartTime, double in_EndTime)
{
	int ret = -1;
	int MinIDAtViewTime;
	if (in_ViewTime >= GetCurTime())
		MinIDAtViewTime = GetCurPos();
	else
		MinIDAtViewTime = m_Data->GetLowIdxByTime(in_ViewTime);

	if (m_pCalc->ClassicType || MinIDAtViewTime < 0)
		ret = MinIDAtViewTime;
	else
	{
		SData* pDataAtViewTime  = GetDataById(MinIDAtViewTime);
		double distAtViewTime = in_Pos.GetDistance(pDataAtViewTime->Position);
		if (distAtViewTime < m_pCalc->CriticalDistance) distAtViewTime = m_pCalc->CriticalDistance;

		double maxTime = in_ViewTime - distAtViewTime * 0.5;
		if (maxTime > in_EndTime) maxTime = in_EndTime;

		int MaxID = m_Data->GetLowIdxByTime(maxTime);

		if (MaxID >= 0)
		{
			//double minTime = in_ViewTime - distAtViewTime - distAtViewTime;
			//if (minTime < in_StartTime) minTime = in_StartTime;
			//int MinID = m_Data->GetLowIdxByTime(minTime);

			int MinID = m_Data->GetLowIdxByTime(in_StartTime);
			if (MinID < 0)
			{
				SData* tmpData = GetDataById(0);
				if (tmpData)
				{
					double dDelta = in_ViewTime - tmpData->TimePoint - in_Pos.GetDistance(tmpData->Position);

					if (dDelta >= 0.0) //(dT > dDist)
						MinID = 0;
					else
						MaxID = MinID;
				}
			}

			int idMid = MinID + 3;
			double dDelta = -1.0;

			while (MinID < MaxID)
			{
				SData* tmpData = GetDataById(idMid);
				ASSERT(tmpData);
				//double dT = in_ViewTime - tmpData->TimePoint;
				//double dDist = in_Pos.GetDistance(tmpData->Position);
				//double dDelta = dT - dDist;
				dDelta = in_ViewTime - tmpData->TimePoint - in_Pos.GetDistance(tmpData->Position);

				if (dDelta >= 0.0) //(dT > dDist)
					ret = MinID = idMid;
				else // if (dT < dDist) так как вероятносьт попадания в равенство стремится к 0
				{
					if (MaxID != idMid)
						MaxID = idMid;
					else 
						MaxID--;
				}
				//else MaxID = MinID = idMid;
				idMid = ((MaxID - MinID + 1) >> 1) + MinID;
			}
			//!!!!Пока непонятно почему так бывает
			if (ret < 0)
			{
				MinID++;
				while (dDelta < 0.0 && --MinID >= 0)
				{
					SData* tmpData = GetDataById(MinID);
					dDelta = in_ViewTime - tmpData->TimePoint - in_Pos.GetDistance(tmpData->Position);
				}

				if (dDelta >= 0.0)
					ret = MinID;

			}

		}
	}

	return ret;
}

int CSpParticle::GetDataAtLowPoint(SData& out_Data, double in_ViewTime)
{
	int ret = -1;
	SData* tmp = GetDataAtLowPoint(in_ViewTime);
	if (tmp)
	{
		out_Data = *tmp;
		ret = 1;
	}
	return ret;
}


double CSpParticle::GetAbsSpeedPwr2()
{
	double ret = 0.0;
	const SData* tmp = GetCurData_C();
	if (tmp)
		ret = tmp->Velocity.X * tmp->Velocity.X +  tmp->Velocity.Y * tmp->Velocity.Y + tmp->Velocity.Z * tmp->Velocity.Z;

	return ret;
}

SData* CSpParticle::GetDataAtLowPoint(double in_ViewTime)
{
	return GetDataById(m_Data->GetLowIdxByTime(in_ViewTime));
}

int CSpParticle::GetLowIdxByTime(double in_ViewTime)
{
	return m_Data ? m_Data->GetLowIdxByTime(in_ViewTime) : -1;
}

int CSpParticle::GetLowIdxByTime(double in_ViewTime, int in_IdxStart)
{
	return m_Data ? m_Data->GetLowIdxByTime(in_ViewTime, in_IdxStart) : -1;
}
int CSpParticle::GetDataById(int in_Idx, SData* inout_Reseult)
{
	SData* tmp = nullptr;
	if (inout_Reseult && m_Data)
	{
		tmp = m_Data->GetDataById(in_Idx);
		if (tmp)
			*inout_Reseult = *tmp;
		else
			inout_Reseult->CLEAR();
	}

	return tmp ? in_Idx : -1;
}

int CSpParticle::GetMovement(SData& out_Data, const SData& cur_Data, double in_NewTime, double* const out_KeyPoint)
{
	int ret = SP_STATE_ERROR_MAIN;
	//double dTime = in_NewTime - cur_Data.TimePoint;
	int  LowIdxStart = m_Data->GetLowIdxByTime(in_NewTime);

	if (LowIdxStart >= 0) // && LowIdxStart < m_Data->GetCurPos())
	{
		SData* DataAtLowIdx = m_Data->GetDataById(LowIdxStart);
		if (DataAtLowIdx->TimePoint > cur_Data.TimePoint)
			ret = ItgrlFull(out_Data, *DataAtLowIdx, in_NewTime, *m_pCalc, out_KeyPoint);
		else
			ret = ItgrlFull(out_Data, cur_Data, in_NewTime, *m_pCalc, out_KeyPoint);
	}
	else
		ret = GetPrevDataByTime(out_Data, in_NewTime, out_KeyPoint);

	return ret;
}


int CSpParticle::GetNextDataByTime(SData& out_Data, double in_ViewTime, double* const out_KeyPoint)
{
	int ret = SP_STATE_ERROR_MAIN;
	const SData* CurData = GetCurData_C();
	int CurPos = GetCurPos();
	if (CurData && CurPos >=0)
	{
		int idDn;
		idDn = m_Data->GetLowIdxByTime(in_ViewTime);

		if (idDn >= CurPos && in_ViewTime > GetCurTime())
			ret = ItgrlFull(out_Data, *CurData, in_ViewTime, *m_pCalc, out_KeyPoint);
	}

	ASSERT(fabs(out_Data.Position.X) < 1000);

	return ret;
}

int CSpParticle::GetPrevDataByTime(SData& out_Data, double in_ViewTime, double* const out_KeyPoint)
{
	int ret = SP_STATE_ERROR_MAIN;
	const SData* CurData = GetCurData_C();
	int CurPos = GetCurPos();
	if (CurData)
	{
		int idDn;
		idDn = m_Data->GetLowIdxByTime(in_ViewTime);

		if (idDn == CurPos)
		{
			if (CurData->TimePoint == in_ViewTime)
			{
				ret = SP_STATE_ОК;
				out_Data = *CurData;
			}
		}
		else
			if (idDn > CurPos)
			{
				out_Data = { 0 };
			}
		else
		if (idDn >= 0)
		{//Позиция между первой и последней точками траектории.
			SData* SdataIdn = GetDataById(idDn);
			if (SdataIdn)
			{
				SData* SdataIdnP1 = GetDataById(idDn + 1);
				if (SdataIdnP1)
					ret = CalcMidData(out_Data, *SdataIdn, *SdataIdnP1, in_ViewTime);
			}
		}
		else
		{//Позиция до первой в траектории
			ret = CalcPrevData(out_Data, in_ViewTime);
		}
	}
		
	return ret;
}

int CSpParticle::GetPrevDataByTimeFast_(SData& out_Data, int& out_Idx, int StartIdx, double in_ViewTime, double* const out_KeyPoint)
{
	int ret = SP_STATE_ERROR_MAIN;
	out_Idx = -1;
	const SData* CurData = GetCurData_C();
	int CurPos = GetCurPos();
	if (CurData)
	{
		int idDn;
		idDn = m_Data->GetLowIdxByTime(in_ViewTime, StartIdx);

		if (idDn == CurPos)
		{
			if (CurData->TimePoint == in_ViewTime)
			{
				ret = SP_STATE_ОК;
				out_Data = *CurData;
			}
		}
		else
			if (idDn > CurPos)
			{
				out_Data = { 0 };
			}
		else
		if (idDn >= 0)
		{//Позиция между первой и последней точками траектории.
			SData* SdataIdn = GetDataById(idDn);
			out_Idx = idDn;
			ItgrlFull(out_Data, *SdataIdn, in_ViewTime, *m_pCalc, out_KeyPoint);
		}
		else
		{//Позиция до первой в траектории
			ret = CalcPrevData(out_Data, in_ViewTime);
		}
	}
		
	return ret;
}


int CSpParticle::GetPrevDataByTimeLinear(SData& out_Data, int& out_Idx, int StartIdx, double in_ViewTime)
{
	int ret = SP_STATE_ERROR_MAIN;
	out_Idx = -1;
	const SData* CurData = GetCurData_C();
	int CurPos = GetCurPos();
	if (CurData)
	{
		int idDn;
		idDn = m_Data->GetLowIdxByTime(in_ViewTime, StartIdx);

		if (idDn >= CurPos)
		{
			if (CurData->TimePoint == in_ViewTime)
			{
				ret = SP_STATE_ОК;
				out_Data = *CurData;
			}
			else
				if (idDn > CurPos)
				{
					out_Data = { 0 };
				}
		}
		else if (idDn >= 0)
			{//Позиция между первой и последней точками траектории.
				out_Idx = idDn;
				out_Data.SetLinearFrom(*GetDataById(idDn), *GetDataById(idDn + 1), in_ViewTime);
			}
			else
			{//Позиция до первой в траектории
				ret = CalcPrevData(out_Data, in_ViewTime);
			}
	}

	return ret;
}



int CSpParticle::CalcPrevData(SData& out_Data, double in_DataTime)
{
	int ret = -1;
	int CurPos = GetCurPos();
	if (CurPos >= 0)
	{
		double KeyPoint = 0.0;
		SData* Sdata0 = GetDataById(0);
		if (Sdata0)
		{
			if (in_DataTime >= Sdata0->TimePoint)
				ret = GetPrevDataByTime(out_Data, in_DataTime, &KeyPoint);
			else
			{
				out_Data = *Sdata0;
				//Правильно посчитать с учетом ускорения
				out_Data.LinearMove(in_DataTime - Sdata0->TimePoint);
			}
		}
	}
	return ret;
}

int CSpParticle::GetPointCount()
{
	return m_Data ? m_Data->GetCurPos() + 1 : 0;
}

int CSpParticle::GetCurPos()
{
	return m_Data ? m_Data->GetCurPos() : -2;
}

SData* CSpParticle::GetCurData()
{
	return m_Data ? m_Data->GetCurData() : nullptr;
}

const SData* CSpParticle::GetCurData_C() 
{
	return m_Data->GetCurData(); 
}

const SData* CSpParticle::GetPrevData_C()
{
	return m_Data->GetPrevData(); 
}

int CSpParticle::GetCurData(SData* out_Data)
{
	int ret = 0;
	if (out_Data) *out_Data = *GetCurData_C();
	else ret = -1;
	return ret;
}

int CSpParticle::SetParticleData(int in_flags, const SData& in_Ptr)
{
	int ret = 0;

	if (m_Data)
	{
		if (m_Data->GetCurPos() >= 0)
		{
			SData* curData = GetCurData();

			if ((in_flags & SP_DATA_ALL) == SP_DATA_ALL)
			{
				*curData = in_Ptr;
			}
			else
			{
				if ((in_flags & SP_DATA_DENSITY))
				{
					curData->Accel_2_3D = in_Ptr.Accel_2_3D;
					curData->Jerk_3_3D = in_Ptr.Jerk_3_3D;
					ret |= SP_DATA_DENSITY;
				}
				if ((in_flags & SP_DATA_SPEED))
				{
					curData->Velocity = in_Ptr.Velocity;
					ret |= SP_DATA_SPEED;
					curData->AbsSpeed = curData->Velocity.LEN();
				}
				if ((in_flags & SP_DATA_POSITION))
				{
					curData->Position = in_Ptr.Position;
					ret |= SP_DATA_POSITION;
				}
				if ((in_flags & SP_DATA_TIMEPOINT))
				{
					curData->TimePoint = in_Ptr.TimePoint;
					ret |= SP_DATA_TIMEPOINT;
				}
			}
		}
		else
		{
			m_Data->AddData(in_Ptr, DA_ADD_FLAG_ADD_FORCE);
		}
	}

	return ret;
}

/////////////////////// STATIC ///////////////////////

int CSpParticle::Itgrl(SPos* const D_Res, SPos* const D1, int iMaxLev, double Tx, double DensMltpl)
{
	return Itgrl(D_Res, D1, Tx, 0, iMaxLev, Tx, DensMltpl);
}

int CSpParticle::Itgrl(SPos* const D_Res, SPos* const D1, double in_Mltpl, int iLev, int iMaxLev, double Tx, double DensMltpl)
{
	if (iLev <= 0)
	{
		for(int i = 0; i <= iMaxLev; i++)
			D_Res[i] = D1[i];

		Itgrl(D_Res, D1, Tx, 1, iMaxLev, Tx, DensMltpl);
	}
	else if (iLev <= iMaxLev)
	{
		for (int i = iLev, idx = 0; i <= iMaxLev; i++, idx++)
		{
			if (idx == 1 && m_pCalc->AccCalcType == 1 )
			{
				D_Res[idx].X += D1[i].X * in_Mltpl * (1.0 - DensMltpl);
				D_Res[idx].Y += D1[i].Y * in_Mltpl * (1.0 - DensMltpl);
				D_Res[idx].Z += D1[i].Z * in_Mltpl * (1.0 - DensMltpl);
			}
			else
				if (idx == 1 && m_pCalc->AccCalcType == 2) 
				{
					if (DensMltpl != 1.0)
					{
						D_Res[idx].X += D1[i].X * in_Mltpl / (1.0 - DensMltpl);
						D_Res[idx].Y += D1[i].Y * in_Mltpl / (1.0 - DensMltpl);
						D_Res[idx].Z += D1[i].Z * in_Mltpl / (1.0 - DensMltpl);
					}
					else
					{
						D_Res[idx].X += D1[i].X * in_Mltpl * 1.0e100;
						D_Res[idx].Y += D1[i].Y * in_Mltpl * 1.0e100;
						D_Res[idx].Z += D1[i].Z * in_Mltpl * 1.0e100;
					}
				}
				else
				{
					D_Res[idx].X += D1[i].X * in_Mltpl;
					D_Res[idx].Y += D1[i].Y * in_Mltpl;
					D_Res[idx].Z += D1[i].Z * in_Mltpl;
				}
		}

		in_Mltpl *= Tx;
		in_Mltpl /= (double)(++iLev);
		Itgrl(D_Res, D1, in_Mltpl, iLev, iMaxLev, Tx, DensMltpl);
	}

	return iLev;
}


int CSpParticle::Itgrl_s(SPos* const D_Res, SPos* const D1, int iMaxLev, double Tx)
{
	return CSpParticle::Itgrl_s(D_Res, D1, Tx, 0, iMaxLev, Tx);
}

int CSpParticle::Itgrl_s(SPos* const D_Res, SPos* const D1, double in_Mltpl, int iLev, int iMaxLev, double Tx)
{
	if (iLev <= 0)
	{
		for(int i = 0; i <= iMaxLev; i++)
			D_Res[i] = D1[i];

		CSpParticle::Itgrl_s(D_Res, D1, Tx, 1, iMaxLev, Tx);
	}
	else if (iLev <= iMaxLev)
	{
		for (int i = iLev, idx = 0; i <= iMaxLev; i++, idx++)
		{
			D_Res[idx].X += D1[i].X * in_Mltpl;
			D_Res[idx].Y += D1[i].Y * in_Mltpl;
			D_Res[idx].Z += D1[i].Z * in_Mltpl;
		}

		in_Mltpl *= Tx;
		in_Mltpl /= (double)(++iLev);
		CSpParticle::Itgrl_s(D_Res, D1, in_Mltpl, iLev, iMaxLev, Tx);
	}

	return iLev;
}


int CSpParticle::ItgrlFullMoveOnMaxSpeedOnly(SData& Result, const SData& DataSrc, double in_TimeDelta, const SP_Calc& in_pCalcParams, int in_CalcFlag)
{
	int ret = SP_STATE_ОК;
	double deltaTimeD2 = in_TimeDelta * 0.5;
	double deltaTimeD4 = deltaTimeD2 * 0.5;

	if ((in_CalcFlag & ITGL_CALC_TIME))
		Result.TimePoint = DataSrc.TimePoint + in_TimeDelta;

	if ((in_CalcFlag & ITGL_CALC_ACCELERATION))
	{
		Result.Accel_2_3D.X = DataSrc.Accel_2_3D.X + DataSrc.Jerk_3_3D.X * in_TimeDelta;
		Result.Accel_2_3D.Y = DataSrc.Accel_2_3D.Y + DataSrc.Jerk_3_3D.Y * in_TimeDelta;
		Result.Accel_2_3D.Z = DataSrc.Accel_2_3D.Z + DataSrc.Jerk_3_3D.Z * in_TimeDelta;
		ASSERT(fabs(Result.Accel_2_3D.X) < 1.0e80);
	}

	if ((in_CalcFlag & ITGL_CALC_SPEED))
	{
		Result.Velocity.X = DataSrc.Velocity.X + in_TimeDelta * (DataSrc.Accel_2_3D.X + DataSrc.Jerk_3_3D.X * deltaTimeD2);
		Result.Velocity.Y = DataSrc.Velocity.Y + in_TimeDelta * (DataSrc.Accel_2_3D.Y + DataSrc.Jerk_3_3D.Y * deltaTimeD2);
		Result.Velocity.Z = DataSrc.Velocity.Z + in_TimeDelta * (DataSrc.Accel_2_3D.Z + DataSrc.Jerk_3_3D.Z * deltaTimeD2);
	}

	//ASSERT(DataSrc.Jerk_3_3D.Z == 0.0);

	//Разбиение на два отрезка. Разница дрины проекции половинного результата на половину итогового приблизительно является 1/4 величины перелета по прямой вместо дуги
	SPos SpeedDelta05 = DataSrc.Velocity;

	SpeedDelta05.X += deltaTimeD2 * (DataSrc.Accel_2_3D.X + DataSrc.Jerk_3_3D.X * deltaTimeD4);
	SpeedDelta05.Y += deltaTimeD2 * (DataSrc.Accel_2_3D.Y + DataSrc.Jerk_3_3D.Y * deltaTimeD4);
	SpeedDelta05.Z += deltaTimeD2 * (DataSrc.Accel_2_3D.Z + DataSrc.Jerk_3_3D.Z * deltaTimeD4);

	SpeedDelta05.SET_LENGHT(in_pCalcParams.maxAbsSpeed);

	SpeedDelta05 *= deltaTimeD2;
	Result.Velocity.SET_AS_PROJECTION_OF(SpeedDelta05);

	double NewLenTime = Result.Velocity.LEN() * 4.0 - in_TimeDelta;

	Result.Velocity.SET_LENGHT(in_pCalcParams.maxAbsSpeed);
	//так быстрее
	Result.Position.X += NewLenTime * Result.Velocity.X;
	Result.Position.Y += NewLenTime * Result.Velocity.Y;
	Result.Position.Z += NewLenTime * Result.Velocity.Z;

	return ret;
}

int CSpParticle::ItgrlFull(SData& Result, const SData& DataSrc, double in_Time, const SP_Calc& in_pCalcParams, double* const out_KeyPoint)
{
	int ret = SP_STATE_ERROR_MAIN;

	ret = SP_STATE_ОК;
	Result = DataSrc;
	Result.TimePoint = in_Time;

	double deltaTime = in_Time - DataSrc.TimePoint;
	double deltaTimeD2 = deltaTime * 0.5;

	Result.Accel_2_3D.X += DataSrc.Jerk_3_3D.X * deltaTime;
	Result.Accel_2_3D.Y += DataSrc.Jerk_3_3D.Y * deltaTime;
	Result.Accel_2_3D.Z += DataSrc.Jerk_3_3D.Z * deltaTime;

	ASSERT(fabs(Result.Accel_2_3D.X) < SP_MAX_TIME_ACC);

	Result.Velocity.X += deltaTime * (DataSrc.Accel_2_3D.X + DataSrc.Jerk_3_3D.X * deltaTimeD2);
	Result.Velocity.Y += deltaTime * (DataSrc.Accel_2_3D.Y + DataSrc.Jerk_3_3D.Y * deltaTimeD2);
	Result.Velocity.Z += deltaTime * (DataSrc.Accel_2_3D.Z + DataSrc.Jerk_3_3D.Z * deltaTimeD2);

	Result.AbsSpeed = Result.Velocity.LEN();

	if (Result.AbsSpeed >= in_pCalcParams.maxAbsSpeed - in_pCalcParams.Time.GetPrecSpeed())
	{
		ret |= SP_STATE_ABS_SPEED_AL;
		Result.AbsSpeed = in_pCalcParams.maxAbsSpeed;
		Result.Velocity.SET_LENGHT(in_pCalcParams.maxAbsSpeed);

		if (deltaTime <= in_pCalcParams.Time.GetMinStep())
		{
			//упрощение для минимального шага и только для точки перехода.
			SPos DeltaS; DeltaS.SET_AS_DELTA(DataSrc.Velocity, Result.Velocity);

			Result.Position.X += deltaTimeD2 * DeltaS.X;
			Result.Position.Y += deltaTimeD2 * DeltaS.Y;
			Result.Position.Z += deltaTimeD2 * DeltaS.Z;
		}
		else if (DataSrc.AbsSpeed >= in_pCalcParams.maxAbsSpeed) //До этого была предельная
		{
			CSpParticle::ItgrlFullMoveOnMaxSpeedOnly(Result, DataSrc, deltaTime, in_pCalcParams, ITGL_CALC_NONE);

			////Разбиение на два отрезка. Разница дрины проекции половинного результата на половину итогового приблизительно является 1/4 величины перелета по прямой вместо дуги
			//double deltaTimeD4 = deltaTimeD2 * 0.5;
			//SPos SpeedDelta05 = DataSrc.Velocity;

			//SpeedDelta05.X += deltaTimeD2 * (DataSrc.Accel_2_3D.X + DataSrc.Jerk_3_3D.X * deltaTimeD4);
			//SpeedDelta05.Y += deltaTimeD2 * (DataSrc.Accel_2_3D.Y + DataSrc.Jerk_3_3D.Y * deltaTimeD4);
			//SpeedDelta05.Z += deltaTimeD2 * (DataSrc.Accel_2_3D.Z + DataSrc.Jerk_3_3D.Z * deltaTimeD4);

			//SpeedDelta05.SET_LENGHT(in_pCalcParams.maxAbsSpeed);

			//SpeedDelta05 *= deltaTimeD2;
			//Result.Velocity; Result.Velocity.SET_AS_PROJECTION_OF(SpeedDelta05);

			//double NewLenTime = Result.Velocity.LEN() * 4.0 - deltaTime;

			//Result.Velocity.SET_LENGHT(in_pCalcParams.maxAbsSpeed);
			////так быстрее
			//Result.Position.X += NewLenTime * Result.Velocity.X;
			//Result.Position.Y += NewLenTime * Result.Velocity.Y;
			//Result.Position.Z += NewLenTime * Result.Velocity.Z;
		}
		else 
		{
			//// ключевые точки с минимальным шагом позволяют выделить участки траектории с постоянными свойствами. переход между блоками
			SPos tmpSpeed;
			double StepDeltaTime = deltaTimeD2;
			double MaxSpeedDeltaTime = deltaTimeD2;
			double CurSpeedPwr2;

			//поиск точки перехода
			do
			{
				StepDeltaTime *= 0.5;
				tmpSpeed.X = DataSrc.Velocity.X + MaxSpeedDeltaTime * (DataSrc.Accel_2_3D.X + DataSrc.Jerk_3_3D.X * StepDeltaTime);
				tmpSpeed.Y = DataSrc.Velocity.Y + MaxSpeedDeltaTime * (DataSrc.Accel_2_3D.Y + DataSrc.Jerk_3_3D.Y * StepDeltaTime);
				tmpSpeed.Z = DataSrc.Velocity.Z + MaxSpeedDeltaTime * (DataSrc.Accel_2_3D.Z + DataSrc.Jerk_3_3D.Z * StepDeltaTime);

				if ((CurSpeedPwr2 = tmpSpeed.LENPWR2())> in_pCalcParams.maxAbsSpeedPwr2)
					MaxSpeedDeltaTime -= StepDeltaTime;
				else
					MaxSpeedDeltaTime += StepDeltaTime;
			}
			while (StepDeltaTime > in_pCalcParams.Time.GetMinStep());

			if (CurSpeedPwr2 > in_pCalcParams.maxAbsSpeedPwr2)
				MaxSpeedDeltaTime -= StepDeltaTime;

			if (out_KeyPoint)	*out_KeyPoint = DataSrc.TimePoint + in_pCalcParams.Time.CorrectDelta_ByMinStep(MaxSpeedDeltaTime);

			double dtDiv2 = (1.0 / 2.0) * MaxSpeedDeltaTime;
			double dtDiv3 = (1.0 / 3.0) * MaxSpeedDeltaTime;

			//перемещение до точки перехода
			Result.Position.X += MaxSpeedDeltaTime * (DataSrc.Velocity.X + dtDiv2 * (DataSrc.Accel_2_3D.X + dtDiv3 * DataSrc.Jerk_3_3D.X));
			Result.Position.Y += MaxSpeedDeltaTime * (DataSrc.Velocity.Y + dtDiv2 * (DataSrc.Accel_2_3D.Y + dtDiv3 * DataSrc.Jerk_3_3D.Y));
			Result.Position.Z += MaxSpeedDeltaTime * (DataSrc.Velocity.Z + dtDiv2 * (DataSrc.Accel_2_3D.Z + dtDiv3 * DataSrc.Jerk_3_3D.Z));

			//перемещение после точки перехода
			CSpParticle::ItgrlFullMoveOnMaxSpeedOnly(Result, DataSrc, deltaTime - MaxSpeedDeltaTime, in_pCalcParams, ITGL_CALC_NONE);
		}
		//else
		//{ //////Квадратное уравнение не точно и отключено
		//	//// ключевые точки с минимальным шагом позволяют выделить участки траектории с постоянными свойствами. переход между блоками
		//	double MaxSpeedDeltaTime = DataSrc.GetTimeToSpeed(in_pCalcParams.maxAbsSpeed);
		//	ASSERT(!isnan(MaxSpeedDeltaTime));

		//	MaxSpeedDeltaTime = in_pCalcParams.Time.CorrectDelta_ByMinStep(MaxSpeedDeltaTime);

		//	if (out_KeyPoint)	*out_KeyPoint = DataSrc.TimePoint + MaxSpeedDeltaTime;

		//	Result.Velocity.SET_LENGHT(in_pCalcParams.maxAbsSpeed);

		//	double MaxSpeedDeltaTime05 = MaxSpeedDeltaTime * 0.5; //в интеграле /2

		//	Result.Position.X += MaxSpeedDeltaTime05 * (DataSrc.Velocity.X + Result.Velocity.X);
		//	Result.Position.Y += MaxSpeedDeltaTime05 * (DataSrc.Velocity.Y + Result.Velocity.Y);
		//	Result.Position.Z += MaxSpeedDeltaTime05 * (DataSrc.Velocity.Z + Result.Velocity.Z);

		//	double dlt = deltaTime - MaxSpeedDeltaTime;
		//	Result.Position.X += dlt * Result.Velocity.X;
		//	Result.Position.Y += dlt * Result.Velocity.Y;
		//	Result.Position.Z += dlt * Result.Velocity.Z;
		//}
		Result.AbsSpeed = in_pCalcParams.maxAbsSpeed;
	}
	else
	{
		double dtDiv2 = (1.0 / 2.0) * deltaTime;
		double dtDiv3 = (1.0 / 3.0) * deltaTime;

		Result.Position.X += deltaTime * (DataSrc.Velocity.X + dtDiv2 * (DataSrc.Accel_2_3D.X + dtDiv3 * DataSrc.Jerk_3_3D.X));
		Result.Position.Y += deltaTime * (DataSrc.Velocity.Y + dtDiv2 * (DataSrc.Accel_2_3D.Y + dtDiv3 * DataSrc.Jerk_3_3D.Y));
		Result.Position.Z += deltaTime * (DataSrc.Velocity.Z + dtDiv2 * (DataSrc.Accel_2_3D.Z + dtDiv3 * DataSrc.Jerk_3_3D.Z));

		if (DataSrc.AbsSpeed >= in_pCalcParams.maxAbsSpeed)
			ret |= SP_STATE_ABS_CHANGE_LS;
	}
	return ret;
}

int CSpParticle::CalcMidData(SData& Result, const SData& Ptr1, const SData& Ptr2, double absMidPoint)
{
	int ret = SP_STATE_ОК;

	Result = { 0 };
	if (Ptr1.TimePoint >= absMidPoint)
	{
		ret = SP_STATE_DOWN;
		if (Ptr1.TimePoint == absMidPoint) Result = Ptr1; else ret |= SP_STATE_ERROR_MAIN;
	}
	else if (Ptr2.TimePoint <= absMidPoint)
	{
		ret = SP_STATE_UP;
		if (Ptr2.TimePoint == absMidPoint) Result = Ptr2; else ret |= SP_STATE_ERROR_MAIN;
	}
	else
	{
		ret = SP_STATE_MID;
		ItgrlFull(Result, Ptr1, absMidPoint, *m_pCalc, nullptr);
	}
	return ret;
}



void CSpParticle::GetMinMaxTracePos(SPos& min_Pos, SPos& max_Pos, int idxStart, int idx_End)
{
	m_Data->GetMinMaxTracePos(min_Pos, max_Pos, idxStart, idx_End);
}


char* CSpParticle::GetFlagsDescription(char* out_Str, int in_Flags)
{
	if (out_Str)
	{
		if (in_Flags == 0) out_Str += sprintf_s(out_Str, 1000, "Ok");
		else
		{
			if ((in_Flags & SP_STATE_ERROR_MINOR) == SP_STATE_ERROR_MINOR) out_Str += sprintf_s(out_Str, 1000, "Warning|");
			else  if ((in_Flags & SP_STATE_ERROR_MINOR) == SP_STATE_ERROR_MAIN) out_Str += sprintf_s(out_Str, 1000, "Error|");
			if ((in_Flags & SP_STATE_INPROC) == SP_STATE_INPROC) out_Str += sprintf_s(out_Str, 1000, "INPROC|");
			if ((in_Flags & SP_STATE_DOWN) == SP_STATE_DOWN) out_Str += sprintf_s(out_Str, 1000, "DOWN|");
			if ((in_Flags & SP_STATE_UP) == SP_STATE_UP) out_Str += sprintf_s(out_Str, 1000, "UP|");
			if ((in_Flags & SP_STATE_MID) == SP_STATE_MID) out_Str += sprintf_s(out_Str, 1000, "MID|");
			if ((in_Flags & SP_STATE_ABS_SPEED_AL) == SP_STATE_ABS_SPEED_AL) out_Str += sprintf_s(out_Str, 1000, "ABS_SPEED_AL|");
			if ((in_Flags & SP_STATE_ABS_CHANGE_LS) == SP_STATE_ABS_CHANGE_LS) out_Str += sprintf_s(out_Str, 1000, "ABS_CHANGE_LS|");
			if ((in_Flags & SP_STATE_LOW_VIEW) == SP_STATE_LOW_VIEW) out_Str += sprintf_s(out_Str, 1000, "LOW_VIEW|");
			if ((in_Flags & SP_STATE_HIGH_VIEW) == SP_STATE_HIGH_VIEW) out_Str += sprintf_s(out_Str, 1000, "HIGH_VIEW|");
			if ((in_Flags & SP_STATE_ABS_0_SPEED) == SP_STATE_ABS_0_SPEED) out_Str += sprintf_s(out_Str, 1000, "ABS_0_SPEED|");
			if ((in_Flags & SP_STATE_REL_0_SPEED) == SP_STATE_REL_0_SPEED) out_Str += sprintf_s(out_Str, 1000, "REL_0_SPEED|");
		}
	}

	return out_Str;
}



////////////////////////////////////////////////////////////////////////////////////


double Calc_RelativeAcceleration(int in_RelPartType, int in_CalcType, double in_DensityAtBasePoint)
{
	double AccelerationValue;
	//расчет ускорения
	switch ((in_RelPartType & SP_TYPE_FORM_MASK))
	{
	case SP_TYPE_NEGATIVE://простая античастица - отталкивает
		switch (in_CalcType)
		{
		case 0:
		default: //AccelerationValue !!!-inf>>>!!! 0 >>> +inf
			if (in_DensityAtBasePoint >= 1.0)
				AccelerationValue = -1.0e50;
			else
				AccelerationValue = -1.0 / (1.0 - in_DensityAtBasePoint) + 1.0;
			break;
		case 1: //AccelerationValue   !!!-inf>>>!!! 0 >>> +inf
			AccelerationValue = in_DensityAtBasePoint - 1.0;
			break;
		}
		break;
	case SP_TYPE_POSITIVE://простая частица - притягивает
	default:
		switch (in_CalcType)
		{
		case 0:
		default: //AccelerationValue !!!-inf>>>!!! 0 >>> +inf
			if (in_DensityAtBasePoint <= 0.0)
				AccelerationValue = 1.0e50;
			else
				AccelerationValue = (1.0 / in_DensityAtBasePoint) - 1.0;
			break;
		case 1: //AccelerationValue   !!!-inf>>>!!! 0 >>> +inf
			AccelerationValue = 1.0 - in_DensityAtBasePoint;
			break;
		}
		break;
	}
	return AccelerationValue;
}

double Calc_RelativeDensity(int in_RelPartType, int in_CalcType, double in_DistancePwr1, double in_DistancePwr2)
{
	double DensityValue;
	//расчет плотности


	switch (in_RelPartType)
	{
	case SP_TYPE_NEGATIVE://простая античастица - отталкивает
		switch (in_CalcType)
		{
		case 0:
		default: //dens = 1 / S^2        DensityAtBasePoint [-inf] 0 >>>> 1
			if (in_DistancePwr2 == 0.0)
				DensityValue = 1.0e-50;
			else
				DensityValue = SP_DENSITY_MAX / in_DistancePwr2;
			break;
		case 1://dens = 1 - 1 / (1 + S^2)
			DensityValue = SP_DENSITY_MAX / (SP_DENSITY_MAX + in_DistancePwr2);
			break;
		case 2://dens = 1 - 1 / (1 + S) ^ 2      0.0 < DensityAtBasePoint < 1
			DensityValue = SP_DENSITY_MAX / ((SP_DENSITY_MAX + in_DistancePwr1) * (SP_DENSITY_MAX + in_DistancePwr1));
			break;
		}
		break;
	case SP_TYPE_POSITIVE://простая частица - притягивает
	default:
		switch (in_CalcType)
		{
		case 0:
		default: //dens = 1 - 1 / S^2        DensityAtBasePoint [-inf] 0 >>>> 1
			if (in_DistancePwr2 == 0.0)
				DensityValue = 1.0e-50;
			else
				DensityValue = SP_DENSITY_MAX - SP_DENSITY_MAX / in_DistancePwr2;
			break;
		case 1://dens = 1 - 1 / (1 + S^2)
			DensityValue = SP_DENSITY_MAX - SP_DENSITY_MAX / (SP_DENSITY_MAX + in_DistancePwr2);
			break;
		case 2://dens = 1 - 1 / (1 + S) ^ 2      0.0 < DensityAtBasePoint < 1
			DensityValue = SP_DENSITY_MAX - SP_DENSITY_MAX / ((SP_DENSITY_MAX + in_DistancePwr1) * (SP_DENSITY_MAX + in_DistancePwr1));
			break;
		}
		break;
	}

	return DensityValue;
}

