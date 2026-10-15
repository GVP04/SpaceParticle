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
	m_CountRelated = 0;
}

CSpParticle::CSpParticle(int in_MaxItems, int in_TypeFlags)
{
	m_TypeFlags = in_TypeFlags;
	m_StateNext = m_StateCurr = m_StatePrev = 0;
	m_NewPointState = SP_NEWPOINT_STATE_ON;
	NewPoint_Clear();
	m_NextData.CLEAR();
	int nItms = in_MaxItems < 5 ? 1000 : in_MaxItems + 100;
	m_Data = new SPTraceArrayDynEx<SData, double>(nItms, &m_pCalc->Time);
}

CSpParticle::~CSpParticle()
{
	if (m_Data) delete m_Data;
	m_Data = nullptr;
}

void CSpParticle::Clear(int in_Flags)
{
	m_StateNext = m_StateCurr = m_StatePrev = 0;
	m_CountRelated = 0;

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
	m_NextData.State = 0;
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
				SData tmpData;
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
	ASSERT(in_StartTime < in_EndTime || in_StartTime == 0.0);

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
			ret |= SP_STATE_DATA_LINEAR;
			int FlagFind = 1;
			if (in_StartTime <= VPDataID0->TimePoint)
			{
				double distanceTMP = VPDataID0->Position.GetDistance(in_Pos.Position);
				double periodTMP = in_ViewTime - VPDataID0->TimePoint;// -m_pCalc->Critical.Distance;
				if (distanceTMP >= periodTMP)
				{//VP до сохраненной траектории
					ret |= FindViewPointByTimeLinear_LE0(out_Data, in_Pos, in_ViewTime, in_StartTime);
					if (out_Data.TimePoint >= VPDataID0->TimePoint)
						FlagFind = 1;
					else
						FlagFind = 0;
				}
			}

			if (FlagFind)
			{
				int idMinRet = FindViewPointMinIdByTime(in_Pos.Position, in_ViewTime, in_StartTime, in_EndTime);
				int idMin = idMinRet;
				if ((idMinRet & 0xF0000000) == 0x90000000)
					idMin = (idMinRet & 0x0FFFFFFF);

				if (idMinRet < 0)
				{
					ret |= SP_STATE_FAR_AWAY;
				}
				else
				{
					if (out_LowIdx)
						*out_LowIdx = idMin;

					if (idMin < GetCurPos())
					{
						if ((idMinRet & 0xF0000000) == 0x90000000)
							out_Data = *GetDataById(idMin);
						else
							if (idMin >= 0)
							{// есть пересечение в диапазоне [idMin]...[idMin+1] /// простой линйный расчет без учета ускорения
								//правильно с учетом ускорения. ускорение ускорения пропорционально квадрату скорости.
								//пока расчет позиции по спрямленной  GetDataByIdLinear(int in_Id)
								SData LowBorder = *GetDataById(idMin);
								SData UpBorder = *GetDataById(idMin + 1);

								double dt = GetLinearViewTime(in_Pos, LowBorder);
								double newVP = in_Pos.TimePoint + dt;

								if (fabs(dt) != SP_MAX_TIME_DISTANCE) //&& newVP <= UpBorder.TimePoint)
								{
									if (newVP >= UpBorder.TimePoint)
									{
										//ASSERT(newVP <= UpBorder.TimePoint + 0.00001 || UpBorder.AbsSpeed >= m_pCalc->Critical.Speed); //следствие спрямления 
										out_Data = UpBorder;
										out_Data.LinearMove(m_pCalc->Time.GetMinStep() * -0.1);
									}
									else
										if (newVP >= LowBorder.TimePoint)
										{
											out_Data = LowBorder;
											out_Data.LinearMove(newVP - out_Data.TimePoint);
											double dDelta, dDeltaPrev = 1.0e90;	////dDelta < 0 разница во времени меньше расстояния. Не успеет добежать информация.
											int AssertCntr = 0;
											double prev_newVP;

											while ((dDelta = fabs(in_ViewTime - newVP - in_Pos.Position.GetDistance(out_Data.Position))) > 1.0e-14 && dDelta < dDeltaPrev)
											{//коррекция результатов сжатия траектории
												ASSERT(AssertCntr++ < 20);
												dDeltaPrev = dDelta;
												prev_newVP = newVP;

												newVP = in_Pos.TimePoint + GetLinearViewTime(in_Pos, out_Data);
												out_Data.LinearMove(newVP - out_Data.TimePoint);
											}

											if (dDelta > 1.0e-14 && dDelta != dDeltaPrev)
												out_Data.LinearMove(prev_newVP - out_Data.TimePoint);
										}
										else
										{
											out_Data = LowBorder;
											out_Data.LinearMove(m_pCalc->Time.GetMinStep() * -0.1);
										}
								}
								else
									ret |= SP_STATE_FAR_AWAY;
							}
							else
								ret |= SP_STATE_FAR_AWAY;
					}
					else
					{//время больше времени в последней позиции масиива траектории
						ret = SP_STATE_HIGH_VIEW | SP_STATE_ERROR_MINOR;
						double KeyPoint = 0.0;
						GetNextDataByTime(out_Data, in_ViewTime, &KeyPoint);
					}
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


//для получения точки видимости на прямолмнейной равномерной траектории
int CSpParticle::FindViewPointByTimeLinear_LE0(SData & out_Data, const SData & in_Pos, double in_ViewTime, double in_StartTime)
{
	int ret = -1;
	SData* VPDataID0 = GetDataById(0);
	if (in_StartTime <= VPDataID0->TimePoint)
	{
		double LinearViewTime = GetLinearViewTime(in_Pos, *VPDataID0);
		double newVP = in_Pos.TimePoint + LinearViewTime;

		if (fabs(LinearViewTime) < SP_MAX_TIME_DISTANCE)
		{
			ret = 0;
			out_Data = *VPDataID0;
			out_Data.LinearMove(newVP - out_Data.TimePoint, LINEARMOVECLEARFLAG_ACCEL3D | LINEARMOVECLEARFLAG_ACCELDERIV3D);
			double dDelta, dDeltaPrev = 1.0e90, prev_newVP;
			int AssertCntr = 0;

			//if (newVP > 0.0)
			{
				while (!isinf(newVP) && (dDelta = fabs(in_ViewTime - newVP - in_Pos.Position.GetDistance(out_Data.Position))) > 1.0e-14 && dDelta < dDeltaPrev)
				{//коррекция результатов сжатия траектории
					ASSERT(AssertCntr++ < 20);
					dDeltaPrev = dDelta;
					prev_newVP = newVP;

					newVP = in_Pos.TimePoint + GetLinearViewTime(in_Pos, out_Data);
					if (isinf(newVP))
						dDelta = 0.0;
					else
						out_Data.LinearMove(newVP - out_Data.TimePoint);

				}

				if (dDelta > 1.0e-14 && dDelta != dDeltaPrev)
					out_Data.LinearMove(prev_newVP - out_Data.TimePoint);
			}
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

	if (m_pCalc->ClassicType)
	{
		ret = m_Data->GetLowIdxByTime(in_ViewTime);
	}
	else
	{
		int MaxID = in_ViewTime >= m_Data->GetCurTime() ? m_Data->GetCurPos() : m_Data->GetLowIdxByTime(in_ViewTime > in_EndTime ? in_EndTime : in_ViewTime);

		if (MaxID >= 0)
		{
			int MinID = m_Data->GetLowIdxByTime(in_StartTime);
			if (MinID < 0) MinID = 0;

			SData* tmpData = GetDataById(MinID);
			//dDelta < 0 разница во времени меньше расстояния. Не успеет добежать информация.
			double dDelta = in_ViewTime - tmpData->TimePoint - in_Pos.GetDistance(tmpData->Position);

			if (dDelta > 0)
			{
				ret = MinID;
				tmpData = GetDataById(MinID + 1); //чаще всего это верхняя граница
				if (tmpData)
				{
					//dDelta < 0 разница во времени меньше расстояния. Не успеет добежать информация.
					dDelta = in_ViewTime - tmpData->TimePoint - in_Pos.GetDistance(tmpData->Position);

					if (dDelta <= 0) 
						MaxID = MinID;
					else
						ret = MinID = MinID + 1;

					if (MinID < MaxID)
					{
						int idMid = MinID + 2; //еще одна быстрая попытка
						if (idMid > MaxID)
							idMid = ((MaxID - MinID + 1) >> 1) + MinID;

						while (MinID < MaxID)
						{
							tmpData = GetDataById(idMid);
							ASSERT(tmpData);
							//dDelta < 0 разница во времени меньше расстояния. Не успеет добежать информация.
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
					}
				}
			}
			else if (dDelta < 0)
			{//!!!??????как результат сжатия массива траектории 
				ret = -2;
				SData* tmpDataPrev = GetDataById(MinID - 1);
				if (tmpDataPrev && tmpData->TimePoint - tmpDataPrev->TimePoint > dDelta * 128.0)
					ret = (0x90000000 | MinID);
			}
			else //if (dDelta == 0)
			{
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

int CSpParticle::GetNextDataByTime(SData& out_Data, double in_ViewTime, double* const out_KeyPoint)
{
	int ret = SP_STATE_ERROR_MAIN;
	const SData* CurData = GetCurData_C();
	if (CurData && in_ViewTime >= CurData->TimePoint)
		ret = ItgrlFull(out_Data, *CurData, in_ViewTime, *m_pCalc, out_KeyPoint, 0);

//	ASSERT(fabs(out_Data.Position.X) < 1000);

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
			SData* Sdata0 = GetDataById(0);
			if (Sdata0)
			{//Правильно посчитать с учетом ускорения
				out_Data = *Sdata0;
				out_Data.LinearMove(in_ViewTime - Sdata0->TimePoint);
			}
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
					out_Data = { 0 };
		}
		else if (idDn >= 0)
		{//Позиция между первой и последней точками траектории.
			out_Idx = idDn;
			out_Data.SetLinearFromVelocity_Linear(*GetDataById(idDn), *GetDataById(idDn + 1), in_ViewTime);
		}
		else
		{//Позиция до первой в траектории
			SData* Sdata0 = GetDataById(0);
			if (Sdata0)
			{//Правильно посчитать с учетом ускорения
				out_Data = *Sdata0;
				out_Data.LinearMove(in_ViewTime - Sdata0->TimePoint);
			}
		}
	}

	return ret;
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
					curData->Velocity_Linear = curData->Velocity;
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

	if ((in_CalcFlag & ITGL_CALC_TIME))
		Result.TimePoint = DataSrc.TimePoint + in_TimeDelta;

	const SPos* SrcVelocity = ((in_CalcFlag & ITGL_CALC_USELINEAR)) ? &DataSrc.Velocity_Linear : &DataSrc.Velocity;


	if ((in_CalcFlag & ITGL_CALC_ACCELERATION))
	{
		Result.Accel_2_3D.X = DataSrc.Accel_2_3D.X + DataSrc.Jerk_3_3D.X * in_TimeDelta;
		Result.Accel_2_3D.Y = DataSrc.Accel_2_3D.Y + DataSrc.Jerk_3_3D.Y * in_TimeDelta;
		Result.Accel_2_3D.Z = DataSrc.Accel_2_3D.Z + DataSrc.Jerk_3_3D.Z * in_TimeDelta;
		ASSERT(fabs(Result.Accel_2_3D.X) < 1.0e80);
	}

	if ((in_CalcFlag & ITGL_CALC_SPEED))
	{
		double deltaTimeD2 = in_TimeDelta * 0.5;
		Result.Velocity.X = SrcVelocity->X + in_TimeDelta * (DataSrc.Accel_2_3D.X + DataSrc.Jerk_3_3D.X * deltaTimeD2);
		Result.Velocity.Y = SrcVelocity->Y + in_TimeDelta * (DataSrc.Accel_2_3D.Y + DataSrc.Jerk_3_3D.Y * deltaTimeD2);
		Result.Velocity.Z = SrcVelocity->Z + in_TimeDelta * (DataSrc.Accel_2_3D.Z + DataSrc.Jerk_3_3D.Z * deltaTimeD2);
		Result.Velocity.SET_LENGHT(in_pCalcParams.Critical.Speed);
		Result.Velocity_Linear = Result.Velocity;
	}

	//SPos MidVelocity; MidVelocity.SET_AS_MIDDLE_POINT(DataSrc.Velocity, Result.Velocity);
	//так быстрее
	Result.Position.X += in_TimeDelta * (SrcVelocity->X + Result.Velocity.X) * 0.5;
	Result.Position.Y += in_TimeDelta * (SrcVelocity->Y + Result.Velocity.Y) * 0.5;
	Result.Position.Z += in_TimeDelta * (SrcVelocity->Z + Result.Velocity.Z) * 0.5;

	return ret;
}

/*
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
		Result.Velocity.X = SrcVelocity->X + in_TimeDelta * (DataSrc.Accel_2_3D.X + DataSrc.Jerk_3_3D.X * deltaTimeD2);
		Result.Velocity.Y = SrcVelocity->Y + in_TimeDelta * (DataSrc.Accel_2_3D.Y + DataSrc.Jerk_3_3D.Y * deltaTimeD2);
		Result.Velocity.Z = SrcVelocity->Z + in_TimeDelta * (DataSrc.Accel_2_3D.Z + DataSrc.Jerk_3_3D.Z * deltaTimeD2);
		Result.Velocity.SET_LENGHT(in_pCalcParams.Critical.Speed);
	}

	//ASSERT(DataSrc.Jerk_3_3D.Z == 0.0);

	//Разбиение на два отрезка. Разница дрины проекции половинного результата на половину итогового приблизительно является 1/4 величины перелета по прямой вместо дуги
	SPos SpeedDelta05 = DataSrc.Velocity;

	SpeedDelta05.X += deltaTimeD2 * (DataSrc.Accel_2_3D.X + DataSrc.Jerk_3_3D.X * deltaTimeD4);
	SpeedDelta05.Y += deltaTimeD2 * (DataSrc.Accel_2_3D.Y + DataSrc.Jerk_3_3D.Y * deltaTimeD4);
	SpeedDelta05.Z += deltaTimeD2 * (DataSrc.Accel_2_3D.Z + DataSrc.Jerk_3_3D.Z * deltaTimeD4);

	SpeedDelta05.SET_LENGHT(in_pCalcParams.Critical.Speed);

	SpeedDelta05 *= deltaTimeD2;
	Result.Velocity.SET_AS_PROJECTION_OF(SpeedDelta05);

	double NewLenTime = Result.Velocity.LEN() * 4.0 - in_TimeDelta;

	Result.Velocity.SET_LENGHT(in_pCalcParams.Critical.Speed);
	//так быстрее
	Result.Position.X += NewLenTime * Result.Velocity.X;
	Result.Position.Y += NewLenTime * Result.Velocity.Y;
	Result.Position.Z += NewLenTime * Result.Velocity.Z;

	return ret;
}
*/

int CSpParticle::ItgrlFull(SData& Result, const SData& DataSrc, double in_Time, const SP_Calc& in_pCalcParams, double* const out_KeyPoint, int in_CalcFlag)
{
	int ret = SP_STATE_ОК;

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

	const SPos* SrcVelocity = ((in_CalcFlag & ITGL_CALC_USELINEAR)) ? &DataSrc.Velocity_Linear : &DataSrc.Velocity;

	if (Result.AbsSpeed >= in_pCalcParams.Critical.Speed)
	{
		ret |= SP_STATE_ABS_SPEED_AL;
		Result.AbsSpeed = in_pCalcParams.Critical.Speed;
		Result.Velocity.SET_LENGHT(in_pCalcParams.Critical.Speed);
		if (DataSrc.AbsSpeed < in_pCalcParams.Critical.Speed)
			ret |= SP_STATE_ABS_CHANGE_LS;

		if (deltaTime - 5.0e-14 <= in_pCalcParams.Time.GetMinStep())
		{
			//упрощение для минимального шага и только для точки перехода. Считаем скорость предельной
			//с большой вероятностью точка посчитана с предельной точностью на предыдущем шаге
			//CSpParticle::ItgrlFullMoveOnMaxSpeedOnly(Result, DataSrc, deltaTime, in_pCalcParams, ITGL_CALC_NONE);

			SPos tmpVelocity = DataSrc.Velocity;
			tmpVelocity.SET_LENGHT_TO_1();

			Result.Position.X = DataSrc.Position.X + deltaTime * (tmpVelocity.X + Result.Velocity.X) * 0.5;
			Result.Position.Y = DataSrc.Position.Y + deltaTime * (tmpVelocity.Y + Result.Velocity.Y) * 0.5;
			Result.Position.Z = DataSrc.Position.Z + deltaTime * (tmpVelocity.Z + Result.Velocity.Z) * 0.5;

			////!!!!Из-за потери точности расстояние может быть больше допустимого. Можно скорректировать немного при превышении
		}
		else if (DataSrc.AbsSpeed >= in_pCalcParams.Critical.Speed) //До этого была предельная
		{
			CSpParticle::ItgrlFullMoveOnMaxSpeedOnly(Result, DataSrc, deltaTime, in_pCalcParams, ITGL_CALC_NONE);
		}
		else
		{
			//// ключевые точки с минимальным шагом позволяют выделить участки траектории с постоянными свойствами. переход между блоками
			//// !!!!! оптимизировать производительность за счет решения уравнения 4степени и/или Метод Ньютона
			////ЗИИ - для точки в пространстве известна скорость V0, ускорение A0 и рывок J0 в момент T0. Определить когда модуль скорости достигнет заданного значения после T0, если рывок J0 постоянен.
			SPos tmpSpeed;
			double StepDeltaTime = deltaTimeD2;
			double MaxSpeedDeltaTime = deltaTimeD2;
			double CurSpeed;

			SPos tmpJerk = DataSrc.Jerk_3_3D;
			tmpJerk *= 0.5;
			//поиск точки перехода
			tmpSpeed.X = SrcVelocity->X + MaxSpeedDeltaTime * (DataSrc.Accel_2_3D.X + tmpJerk.X * MaxSpeedDeltaTime);
			tmpSpeed.Y = SrcVelocity->Y + MaxSpeedDeltaTime * (DataSrc.Accel_2_3D.Y + tmpJerk.Y * MaxSpeedDeltaTime);
			tmpSpeed.Z = SrcVelocity->Z + MaxSpeedDeltaTime * (DataSrc.Accel_2_3D.Z + tmpJerk.Z * MaxSpeedDeltaTime);
			CurSpeed = tmpSpeed.LEN();

			do
			{
				StepDeltaTime *= 0.5;
				if (CurSpeed > in_pCalcParams.Critical.Speed)
					MaxSpeedDeltaTime -= StepDeltaTime;
				else
					MaxSpeedDeltaTime += StepDeltaTime;
				tmpSpeed.X = SrcVelocity->X + MaxSpeedDeltaTime * (DataSrc.Accel_2_3D.X + tmpJerk.X * MaxSpeedDeltaTime);
				tmpSpeed.Y = SrcVelocity->Y + MaxSpeedDeltaTime * (DataSrc.Accel_2_3D.Y + tmpJerk.Y * MaxSpeedDeltaTime);
				tmpSpeed.Z = SrcVelocity->Z + MaxSpeedDeltaTime * (DataSrc.Accel_2_3D.Z + tmpJerk.Z * MaxSpeedDeltaTime);
				CurSpeed = tmpSpeed.LEN();
			}
			while (StepDeltaTime > 1.0e-14);

			if (CurSpeed > in_pCalcParams.Critical.Speed && MaxSpeedDeltaTime > 2.0e-14)
			{
				MaxSpeedDeltaTime -= StepDeltaTime;
				tmpSpeed.X = SrcVelocity->X + MaxSpeedDeltaTime * (DataSrc.Accel_2_3D.X + tmpJerk.X * MaxSpeedDeltaTime);
				tmpSpeed.Y = SrcVelocity->Y + MaxSpeedDeltaTime * (DataSrc.Accel_2_3D.Y + tmpJerk.Y * MaxSpeedDeltaTime);
				tmpSpeed.Z = SrcVelocity->Z + MaxSpeedDeltaTime * (DataSrc.Accel_2_3D.Z + tmpJerk.Z * MaxSpeedDeltaTime);
				CurSpeed = tmpSpeed.LEN();
			}
			if (out_KeyPoint)
			{
				*out_KeyPoint = DataSrc.TimePoint + MaxSpeedDeltaTime;
				if (*out_KeyPoint == DataSrc.TimePoint)
					*out_KeyPoint = nextafter(*out_KeyPoint, 1.0e100);

			}

			tmpSpeed.SET_LENGHT_TO_1();
			Result.Velocity = tmpSpeed;

			double dtDiv2 = 0.5 * MaxSpeedDeltaTime;
			double dtDiv3 = (1.0 / 3.0) * MaxSpeedDeltaTime;

			//перемещение до точки перехода
			Result.Position.X += MaxSpeedDeltaTime * (SrcVelocity->X + dtDiv2 * (DataSrc.Accel_2_3D.X + dtDiv3 * DataSrc.Jerk_3_3D.X));
			Result.Position.Y += MaxSpeedDeltaTime * (SrcVelocity->Y + dtDiv2 * (DataSrc.Accel_2_3D.Y + dtDiv3 * DataSrc.Jerk_3_3D.Y));
			Result.Position.Z += MaxSpeedDeltaTime * (SrcVelocity->Z + dtDiv2 * (DataSrc.Accel_2_3D.Z + dtDiv3 * DataSrc.Jerk_3_3D.Z));

			//перемещение после точки перехода
			// ITGL_CALC_NONE потому, что все кроме позиции уже посчитано
			CSpParticle::ItgrlFullMoveOnMaxSpeedOnly(Result, DataSrc, deltaTime - MaxSpeedDeltaTime, in_pCalcParams, ITGL_CALC_NONE);
			if (CurSpeed < in_pCalcParams.Critical.Speed)
				ret |= SP_STATE_NEXT_STEP_MIN;
		}
		Result.AbsSpeed = in_pCalcParams.Critical.Speed;
	}
	else
	{
		double dtDiv2 = (1.0 / 2.0) * deltaTime;
		double dtDiv3 = (1.0 / 3.0) * deltaTime;

		Result.Position.X += deltaTime * (SrcVelocity->X + dtDiv2 * (DataSrc.Accel_2_3D.X + dtDiv3 * DataSrc.Jerk_3_3D.X));
		Result.Position.Y += deltaTime * (SrcVelocity->Y + dtDiv2 * (DataSrc.Accel_2_3D.Y + dtDiv3 * DataSrc.Jerk_3_3D.Y));
		Result.Position.Z += deltaTime * (SrcVelocity->Z + dtDiv2 * (DataSrc.Accel_2_3D.Z + dtDiv3 * DataSrc.Jerk_3_3D.Z));

		if (DataSrc.AbsSpeed >= in_pCalcParams.Critical.Speed)
			ret |= SP_STATE_ABS_CHANGE_LS;
	}
	Result.Velocity_Linear = Result.Velocity;

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
		ItgrlFull(Result, Ptr1, absMidPoint, *m_pCalc, nullptr, ITGL_CALC_USELINEAR);
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


//////////////////////////////////////// SPRelation
double SPRelation::Calc_RelativeDensity(int in_CalcType)
{
	//расчет плотности
	switch (ViewParticlePtr->GetTypeFlags())
	{
	case SP_TYPE_NEGATIVE://простая античастица - отталкивает
		switch (in_CalcType)
		{
		case 0:
		default: 
			if (DistancePwr1 >= SP_DENSITY_MAX)
				DensityAtBasePoint = SP_DENSITY_MAX / DistancePwr2;
			else
				DensityAtBasePoint = DistancePwr2;
			break;
		case 1://dens = 1 - 1 / (1 + S) ^ 2      0.0 < DensityAtBasePoint < 1
			DensityAtBasePoint = SP_DENSITY_MAX / ((SP_DENSITY_MAX + DistancePwr1) * (SP_DENSITY_MAX + DistancePwr1));
			break;
		case 2://dens = 1 - 1 / (1 + S^2)
			DensityAtBasePoint = SP_DENSITY_MAX / (SP_DENSITY_MAX + DistancePwr2);
			break;
		}
		break;
	case SP_TYPE_POSITIVE://простая частица - притягивает
	default:
		switch (in_CalcType)
		{
		case 0:
		default: 
			if (DistancePwr1 >= SP_DENSITY_MAX)
				DensityAtBasePoint = SP_DENSITY_MAX - SP_DENSITY_MAX / DistancePwr2;
			else
				DensityAtBasePoint = SP_DENSITY_MAX - DistancePwr2;
			break;
		case 1://dens = 1 - 1 / (1 + S) ^ 2      0.0 < DensityAtBasePoint < 1
			DensityAtBasePoint = SP_DENSITY_MAX - SP_DENSITY_MAX / ((SP_DENSITY_MAX + DistancePwr1) * (SP_DENSITY_MAX + DistancePwr1));
			break;
		case 2://dens = 1 - 1 / (1 + S^2)
			DensityAtBasePoint = SP_DENSITY_MAX - SP_DENSITY_MAX / (SP_DENSITY_MAX + DistancePwr2);
			break;
		}
		break;
	}

	return DensityAtBasePoint;
}

double SPRelation::Calc_RelativeAcceleration(int in_CalcType)
{
	double BarierDistance = DistancePwr1 - SP_DENSITY_MAX;
	//расчет ускорения. Меньше нуля притягивает, больше нуля отталкивает
	switch ((ViewParticlePtr->GetTypeFlags() & SP_TYPE_FORM_MASK))
	{
	case SP_TYPE_NEGATIVE://простая античастица - отталкивает
		switch (in_CalcType)
		{
		case 0:
		default:
			if (BarierDistance < 1.0e-14 && BarierDistance > -1.0e-14)
				AccelerationValue = 1.0e-50;
			else
			{
				if (BarierDistance > 0.0)
					AccelerationValue = SP_DENSITY_MAX / (BarierDistance * BarierDistance);
				else
					AccelerationValue = -SP_DENSITY_MAX / (BarierDistance * BarierDistance);
			}
			break;
		case 1: //AccelerationValue   !!!-inf>>>!!! 0 >>> +inf
			AccelerationValue = 1.0 / (1.0 - DensityAtBasePoint);
			break;
		case 2: //AccelerationValue !!!-inf>>>!!! 0 >>> +inf
			if (DensityAtBasePoint == 1.0)
				AccelerationValue = 1.0e50;
			else
				AccelerationValue = 1.0 / (1.0 - DensityAtBasePoint) + 1.0;
			break;
		}
		break;
	case SP_TYPE_POSITIVE://простая частица - притягивает
	default:
		switch (in_CalcType)
		{
		case 0:
		default: 
			if (BarierDistance < 1.0e-14 && BarierDistance > -1.0e-14)
				AccelerationValue = -1.0e-50;
			else
			{
				if (BarierDistance > 0.0)
					AccelerationValue = -SP_DENSITY_MAX / (BarierDistance * BarierDistance);
				else
					AccelerationValue = SP_DENSITY_MAX / (BarierDistance * BarierDistance);
			}
			break;
		case 1: //AccelerationValue   !!!-inf>>>!!! 0 >>> +inf
			AccelerationValue = -1.0 / (1.0 - DensityAtBasePoint);
			break;
		case 2://AccelerationValue !!!-inf>>>!!! 0 >>> +inf
			if (DensityAtBasePoint == 0.0)
				AccelerationValue = -1.0e50;
			else
				AccelerationValue =  1.0- (1.0 / DensityAtBasePoint);
			break;
		}
		break;
	}

	Acceleration_3D = Distance_3D;
	Acceleration_3D.SET_LENGHT(AccelerationValue);

	return AccelerationValue;
}

// Расчёт третьей производной (рывка)
void SPRelation::CALC_JERK_AS_RELATIVE()
{
	if (DistancePwr2 != 1.0 && DistancePwr1 > 0.0)
	{
		double DistPwr2minus1 = DistancePwr2 - 1.0;
		double a_magnitude = 1.0 / DistPwr2minus1;
		double DistDotVelocity = Distance_3D.DOT(RelativeVelocity_3D);
		double d_a_dt = -2.0 * DistancePwr1 * DistDotVelocity / (DistPwr2minus1 * DistPwr2minus1);


		// Производная единичного вектора
		SPos dn_dt = RelativeVelocity_3D; dn_dt.DIV(DistancePwr1);
		SPos dn_dt2 = Distance_3D;  dn_dt2.MULT(DistDotVelocity / DistancePwr3);
		dn_dt.MINUS(dn_dt2);
		dn_dt.MULT(a_magnitude);

		// Рывок
		// Единичный вектор направления
		Jerk_3_3D = Distance_3D;
		Jerk_3_3D.NORMALIZE();
		//
		Jerk_3_3D.MULT(d_a_dt);
		Jerk_3_3D += dn_dt;

		ASSERT(fabs(Jerk_3_3D.X) < 1.0e80);
	}
	else
	{
		Jerk_3_3D = { 0 };
	}
}

// Расчёт четвёртой производной (snap) численно
void SPRelation::CALC_SNAP_AS_RELATIVE()
{
	double DistPwr2minus1 = DistancePwr2 - 1.0;
	double DistPwr2minus1Pwr2 = DistPwr2minus1 * DistPwr2minus1;
	double DistDotVelocity = Distance_3D.DOT(RelativeVelocity_3D);
	double dot_S = DistDotVelocity / DistancePwr1;  // dS/dt
	double v_dot_vPLUSr_dot_a = RelativeVelocity_3D.LENPWR2() + Distance_3D.DOT(Acceleration_3D);  // |v|^2
	double ddv_dotS = DistDotVelocity * dot_S;

	// Компонент A и его производная dA/dt
	double A = -2.0 * DistDotVelocity / (DistPwr2minus1Pwr2 * DistancePwr1);
	double numerator_A = v_dot_vPLUSr_dot_a * DistPwr2minus1 * DistancePwr1 - ddv_dotS * (4.0 * DistancePwr2 + DistPwr2minus1);
	double denominator_A = 1.0 / (DistPwr2minus1Pwr2 * DistancePwr2);
	double dA_dt = -2.0 * numerator_A * denominator_A;

	// Компонент B и его производная dB/dt
	double B = 1.0 / (DistPwr2minus1 * DistancePwr1);
	double dB_dt = -(2.0 * DistancePwr2 * dot_S * DistPwr2minus1 + DistPwr2minus1Pwr2 * dot_S) * denominator_A;

	double  ddv_dotSdivDistancePwr3 = ddv_dotS / DistancePwr3;
	double factor_dC = v_dot_vPLUSr_dot_a / DistancePwr3 - 3.0 * ddv_dotSdivDistancePwr3;

	SPos tmp = Distance_3D; tmp *= (-ddv_dotSdivDistancePwr3 - ddv_dotSdivDistancePwr3 / DistancePwr1);
	SPos dC_dt = RelativeVelocity_3D; dC_dt *= factor_dC; dC_dt += tmp;


	// Итоговая четвёртая производная
	// Термы для расчёта snap
	Snap_4_3D = Distance_3D;  Snap_4_3D *= dA_dt;                             // dA/dt * r

	tmp = RelativeVelocity_3D; tmp *= A;                      // A * v
	Snap_4_3D += tmp;

	// Вектор C и его производная dC/dt
	SPos C = Distance_3D; C *= -DistDotVelocity;  C /= DistancePwr3; C += RelativeVelocity_3D;  C *= dB_dt;// dB/dt * (v + C)
	Snap_4_3D += C;

	tmp = Acceleration_3D;  tmp *= B;  // B * a
	Snap_4_3D += tmp;

	tmp = dC_dt;  tmp *= B;  // B * dC/dt
	Snap_4_3D += tmp;

}