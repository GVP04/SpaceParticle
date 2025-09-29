#include "pch.h"
#include "CSpParticle.h"
#include "CSDataArray.h"
#include "CSP_Group.h"


CSpParticle::CSpParticle(SP_Calc* in_pCalc)
{
	m_pCalc = in_pCalc;
	m_NewPointState = SP_NEWPOINT_STATE_ON;
	NewPoint_Clear();
	m_NextData = { 0 };
	m_Data = new CSDataArray;
}

CSpParticle::CSpParticle(int in_MaxItems)
{
	m_NewPointState = SP_NEWPOINT_STATE_ON;
	NewPoint_Clear();
	m_NextData = { 0 };
	m_Data = new CSDataArray(in_MaxItems);
}

CSpParticle::~CSpParticle()
{
	if (m_Data) delete m_Data;
}

void CSpParticle::Clear()
{
	if (!m_Data) m_Data = new CSDataArray;
	m_Data->Init();
}


SData* CSpParticle::GetCurData()
{
	return m_Data == NULL || m_Data->CurPos < 0 ? &CSDataArray::EmptyData : m_Data->GetCurData();
}

int CSpParticle::NewPoint_Begin(double in_NewTimePoint)
{
	int ret = 0;
	NewPoint_Clear();
	if (m_Data && m_Data->GetCurData() && m_Data->GetCurData()->TimePoint < in_NewTimePoint)
	{
		ret = 1;
		m_NewPointState = SP_NEWPOINT_STATE_ON;
		m_NextData.TimePoint = in_NewTimePoint;
	}
	return ret;
}

double CSpParticle::GetCurTime() 
{
	return m_Data ? m_Data->GetCurData()->TimePoint : 0.0; 
}

double CSpParticle::GetPrevTime(int n_StepsBefore)
{
	double ret = -1.0;

	if (m_Data && m_Data->CurPos >= 0)
	{
		int PointsIdx = m_Data->CurPos - n_StepsBefore;

		if (PointsIdx <= m_Data->CurPos && PointsIdx >= 0)  ret = m_Data->m_Array[ PointsIdx].TimePoint;
	}

	return ret;
}


int CSpParticle::DeletePoints(double TimeFromGE, double TimeToLE)
{
	int ret = 0;
	if (m_Data) ret = m_Data->DeletePoints(TimeFromGE, TimeToLE);

	return ret;
}

int CSpParticle::MoveToNewPoint(int in_maxCountLimit, bool in_ClearData)
{
	int ret = 0;
	if (m_Data && (m_NewPointState & SP_NEWPOINT_STATE_ON))
	{
		m_Data->AddData( in_maxCountLimit, &m_NextData);
		NewPoint_Clear();
		ret = 1;
	}

	return ret;
}

void CSpParticle::NewPoint_Clear()
{
	m_NewPointState = SP_NEWPOINT_STATE_OFF;
	m_NextData = { 0 };
}


char* CSpParticle::GetStateDescription(char* out_Str, int in_nPoints)
{
	if (out_Str)
	{
		*out_Str = 0;
		if (m_Data)
		{
			if (m_Data->CurPos >= 0)
			{
				int PointsIdx = m_Data->CurPos - in_nPoints + 1;

				if (PointsIdx > m_Data->CurPos || PointsIdx < 0 || in_nPoints < 0)  PointsIdx = 0;

				while (PointsIdx <= m_Data->CurPos)
				{
					SData *tmpData = m_Data->m_Array + PointsIdx;
					out_Str = CSpParticle::GetFlagsDescription(out_Str, 0);
					out_Str += sprintf_s(out_Str, 1000, "\tTime=\t%g\tTraceCount=%d", tmpData->TimePoint, m_Data->CurPos);
					out_Str += sprintf_s(out_Str, 1000, "\tPosition=\t%g\t%g\t%g", tmpData->position.X, tmpData->position.Y, tmpData->position.Z);
					out_Str += sprintf_s(out_Str, 1000, "\tSpeed=\t%g\t%g\t%g", tmpData->Speed.X, tmpData->Speed.Y, tmpData->Speed.Z);
					out_Str += sprintf_s(out_Str, 1000, "\tDensity3D=\t%g\t%g\t%g", tmpData->Accel3D.X, tmpData->Accel3D.Y, tmpData->Accel3D.Z);
					out_Str += sprintf_s(out_Str, 1000, "\tDensityDeriv3D=\t%g\t%g\t%g", tmpData->AccelDeriv3D.X, tmpData->AccelDeriv3D.Y, tmpData->AccelDeriv3D.Z);

					if (PointsIdx++ < m_Data->CurPos)
						out_Str += sprintf_s(out_Str, 1000, "\n");
				}
			}
			else sprintf_s(out_Str, 1000, "CurPos<0");
		}
		else sprintf_s(out_Str, 1000, "m_Data==NULL");
	}

	return out_Str;
}


char* CSpParticle::GetStateDescription(char* out_Str, double in_ReportTime)
{
	if (out_Str)
	{
		*out_Str = 0;
		if (m_Data)
		{
			if (m_Data->CurPos >= 0)
			{
				SData tmpData = { 0 };
				double tmpTime = 0.0;
				int ret = GetPrevDataByTime(&tmpData, in_ReportTime, &tmpTime);
				out_Str = CSpParticle::GetFlagsDescription(out_Str, ret);
				if ((ret & SP_STATE_ERROR_MINOR) != SP_STATE_ERROR_MAIN)
				{
					out_Str += sprintf_s(out_Str, 1000, "\tTime=\t%.10g\tTraceCount=%d", tmpData.TimePoint, m_Data->CurPos);
					out_Str += sprintf_s(out_Str, 1000, "\tPosition=\t%.10g\t%.10g\t%.10g", tmpData.position.X, tmpData.position.Y, tmpData.position.Z);
					out_Str += sprintf_s(out_Str, 1000, "\tSpeed=\t%.10g\t%.10g\t%.10g", tmpData.Speed.X, tmpData.Speed.Y, tmpData.Speed.Z);
					out_Str += sprintf_s(out_Str, 1000, "\tDensity3D=\t%.10g\t%.10g\t%.10g", tmpData.Accel3D.X, tmpData.Accel3D.Y, tmpData.Accel3D.Z);
					out_Str += sprintf_s(out_Str, 1000, "\tDensityDeriv3D=\t%.10g\t%.10g\t%.10g", tmpData.AccelDeriv3D.X, tmpData.AccelDeriv3D.Y, tmpData.AccelDeriv3D.Z);
				}
			}
			else sprintf_s(out_Str, 1000, "CurPos<0");
		}
		else sprintf_s(out_Str, 1000, "m_Data==NULL");
	}

	return out_Str;
}


//напоминание -- врем€ изменени€ плотности == рассто€нию
int CSpParticle::FindViewPoint(SData* out_Data, SData* in_Pos, double in_ViewTime, SPRelation* in_LastRelation)
{
	int ret = SP_STATE_ќ ;
	if (!m_Data || !m_Data->m_Array || m_Data->CurPos < 0 || !in_Pos || !out_Data)
		ret = SP_STATE_ERROR_MAIN;
	else
	if (m_pCalc->ClassicType)
	{
		if (m_Data->m_Array[m_Data->CurPos].TimePoint <= in_ViewTime)
		{
			if (m_pCalc->ClassicType == 2 && (m_NewPointState & SP_NEWPOINT_STATE_ON))
				*out_Data = m_NextData;
			else
				*out_Data = m_Data->m_Array[m_Data->CurPos];
		}
		else
		{
			///добавить вычисление данных если  m_Data->m_Array[iii].TimePoint < in_ViewTime
			int iii = m_Data->GetLowIdxByTime(in_ViewTime);
			if (iii >= 0)
				*out_Data = m_Data->m_Array[iii];
		}
	}
	else
	{
		ret = SP_STATE_ќ ;
		double dltTpwr2 = -1.0;
		int idMin = m_Data->CurPos;

		if (in_LastRelation == NULL)
		{
			idMin = m_Data->GetLowIdxByTime(in_ViewTime);
			int dlt = 16;
			if (idMin >= 0)
			do
			{
				dlt += dlt >> 1;
				//возможно ускорить при больших отклонени€х 
				dltTpwr2 = (in_ViewTime - m_Data->m_Array[idMin].TimePoint);
				dltTpwr2 = dltTpwr2 * dltTpwr2 - CSpParticle::GetDistancePwr2(in_Pos->position, m_Data->m_Array[idMin].position);
			} while (dltTpwr2 < 0.0 && (idMin-= dlt) >= 0);
		}
		
		if (m_Data->CurPos > 0)
		{
			if (in_LastRelation) idMin = m_Data->GetLowIdxByTime(in_LastRelation->ViewPointData.TimePoint);
			if (idMin < m_Data->CurPos && idMin >= 0)
			{

				int idMax = m_Data->CurPos;

				while (idMax > idMin)
				{
					int idMid = ((idMax - idMin + 1) >> 1) + idMin;

					dltTpwr2 = (in_ViewTime - m_Data->m_Array[idMid].TimePoint);
					dltTpwr2 = dltTpwr2 * dltTpwr2 - CSpParticle::GetDistancePwr2(in_Pos->position, m_Data->m_Array[idMid].position);
					if (dltTpwr2 > 0.0)
						idMin = idMid;
					else
						if (idMax != idMid) idMax = idMid;
						else idMax--;
				}
			}
		}
		else idMin = -1;

		if (idMin < m_Data->CurPos)
		{
			if (idMin >= 0 )
			{// есть пересечение в диапазоне [idMin]...[idMin+1]
				SData Up, Dn, Mid;
				Dn = m_Data->m_Array[idMin];
				Up = m_Data->m_Array[idMin + 1];

				double tSwap, maxDelta = (Up.TimePoint - Dn.TimePoint) / 1000.0;
				ret = SP_STATE_INPROC;

				while (ret == SP_STATE_INPROC)
				{
					double newTime = Dn.TimePoint + (Up.TimePoint - Dn.TimePoint) * 0.5; // возможно ускорить за счет линейной экстрапол€ции
					switch(CalcMidData(&Mid, Dn, Up, newTime))
					{
					case SP_STATE_UP:
					case SP_STATE_DOWN:
						ret = SP_STATE_ќ ;
						break;
					case SP_STATE_MID:
						tSwap = (in_ViewTime - Mid.TimePoint);
						tSwap = tSwap * tSwap - CSpParticle::GetDistancePwr2(in_Pos->position, Mid.position);
						if (tSwap < 0.0) Up = Mid;	else Dn = Mid;
						if (Up.TimePoint - Dn.TimePoint < maxDelta) ret = SP_STATE_ќ ;
						break;
					default: //error
						ret = SP_STATE_ERROR_MAIN;
						break;
					}
				}
				if (!(ret & SP_STATE_ERROR_MAIN)) *out_Data = Mid;
			}
			else
			{ //выход за пределы первой точки траектории. Ёмул€ци€ пр€молинейного равномерного движени€. ≈сли относительна€ скорость выше допустипой мен€ем ret
				ret = SP_STATE_LOW_VIEW|SP_STATE_ERROR_MINOR;
				*out_Data = m_Data->m_Array[0];
				SData Up, Dn;
				Up = m_Data->m_Array[0];
				double dDistance0 = CSpParticle::GetDistance(in_Pos->position, m_Data->m_Array[0].position);

				if (Up.Speed.X == 0.0 && Up.Speed.Y == 0.0 && Up.Speed.Z == 0.0)
				{
					ret |= SP_STATE_ABS_0_SPEED;
					out_Data->TimePoint = in_ViewTime - dDistance0;
				}
				else
				{
					Dn = m_Data->m_Array[0];
					Dn.TimePoint = in_ViewTime - dDistance0 - dDistance0 * sqrt(Up.Speed.X * Up.Speed.X + Up.Speed.Y * Up.Speed.Y + Up.Speed.Z * Up.Speed.Z);
					double newTimeDlt = Up.TimePoint - Dn.TimePoint;

					Dn.position.X -= Dn.Speed.X * newTimeDlt;
					Dn.position.Y -= Dn.Speed.Y * newTimeDlt;
					Dn.position.Z -= Dn.Speed.Z * newTimeDlt;

					double tSwap, maxDelta = newTimeDlt / 10000.0;
					if (maxDelta < m_pCalc->Time.MinStep) maxDelta = m_pCalc->Time.MinStep;
					//оптимизировать по делителю. ѕока здесь производительность не важна
					double ntDelim = 0.5;
					SData Mid = Up;

					while (newTimeDlt > maxDelta)
					{
						newTimeDlt *= ntDelim;
						Mid = Up;
						Mid.position.X -= Mid.Speed.X * newTimeDlt;
						Mid.position.Y -= Mid.Speed.Y * newTimeDlt;
						Mid.position.Z -= Mid.Speed.Z * newTimeDlt;
						Mid.TimePoint -= newTimeDlt;

						tSwap = (in_ViewTime - Mid.TimePoint);
						tSwap = tSwap * tSwap - CSpParticle::GetDistancePwr2(in_Pos->position, Mid.position);
						if (tSwap < 0.0)
							Up = Mid;
						else
							Dn = Mid;
					}
					*out_Data = Mid;
				}
			}
		}
		else
		{//врем€ больше времени в последней позиции масиива траектории
			ret = SP_STATE_HIGH_VIEW | SP_STATE_ERROR_MINOR;
			double KeyPoint = 0.0;
			GetNextDataByTime(out_Data, in_ViewTime, &KeyPoint);
		}

		if (ret > 0 && fabs(GetRelativeSpeed(*out_Data, *in_Pos, NULL, m_pCalc)) >= m_pCalc->maxRelativeSpeed)
			ret = SP_STATE_REL_SPEED_AL | SP_STATE_ERROR_MINOR;

	//	if (in_LastRelation)
	//		out_Data->Density = in_LastRelation->ViewPointData.Density;
	}

	return ret;
}

int CSpParticle::GetDataAtLowPoint(SData* out_Data, double in_ViewTime)
{
	int ret = -1;
	if (out_Data)
	{
		SData* tmp = GetDataAtLowPoint(in_ViewTime);
		if (tmp)
		{
			*out_Data = *tmp;
			ret = 1;
		}
	}
	return ret;
}


double CSpParticle::GetAbsSpeedPwr2()
{
	double ret = 0.0;
	if (m_Data && m_Data->CurPos >= 0)
	{
		SData* tmp = GetCurData();
		ret = tmp->Speed.X * tmp->Speed.X +  tmp->Speed.Y * tmp->Speed.Y + tmp->Speed.Z * tmp->Speed.Z;
	}
	return ret;
}

SData* CSpParticle::GetDataAtLowPoint(double in_ViewTime)
{
	SData* ret = NULL;
	if (m_Data && m_Data->CurPos >= 0)
	{
		int LoIdx = m_Data->GetLowIdxByTime(in_ViewTime);
		if (LoIdx >= 0)
			ret = m_Data->m_Array + LoIdx;
	}
	return ret;
}

int CSpParticle::GetNextDataByTime(SData* out_Data, double in_ViewTime, double* out_KeyPoint)
{
	int ret = SP_STATE_ERROR_MAIN;
	if (m_Data && out_Data && out_KeyPoint)
	{
		int idDn;
		idDn = m_Data->GetLowIdxByTime(in_ViewTime);
		if (idDn >= m_Data->CurPos && m_Data->m_Array[idDn].TimePoint < in_ViewTime)
		{//позици€ после последней в траектории
			ret = SP_STATE_ќ ;

			ItgrlFull(out_Data, m_Data->m_Array[m_Data->CurPos], in_ViewTime, m_pCalc);

			int SpeedPwr1 = CSpParticle::Get3D_Pwr2((m_Data->m_Array + m_Data->CurPos)->Speed) >= m_pCalc->maxAbsSpeed ? 1 : 0;
			int SpeedPwr2 = CSpParticle::Get3D_Pwr2(out_Data->Speed) >= m_pCalc->maxAbsSpeed ? 1 : 0;

			if (SpeedPwr1 ^ SpeedPwr2)
			{//был переход с/на предельную скорость
				SData Lo = *(m_Data->m_Array + m_Data->CurPos);
				SData Hi = *out_Data;
				SData Mid = Hi;
				double deltaT;

				ret |= SP_STATE_ABS_CHANGE_LS;

				while ((deltaT = (Hi.TimePoint - Lo.TimePoint) / 2.0) > m_pCalc->Time.MinStep)
				{
					ItgrlFull(&Mid, Lo, deltaT, m_pCalc);

					Mid.TimePoint = Lo.TimePoint + deltaT;
					double SpeedPwrMidPwr2 = CSpParticle::Get3D_Pwr2(Mid.Speed);
					if (SpeedPwr2)
					{
						if (SpeedPwrMidPwr2 >= m_pCalc->maxAbsSpeed) Hi = Mid; else Lo = Mid;
					}
					else if (SpeedPwrMidPwr2 >= m_pCalc->maxAbsSpeed) Lo = Mid; else Hi = Mid;
				}
				*out_KeyPoint = Hi.TimePoint;
			}
		}
		else 
			if (idDn >= 0)
		{//ѕозици€ между первой и последней точками траектории.
			ret = SP_STATE_ERROR_MAIN;
		}
		else
		{//ѕозици€ до первой в траектории
			ret = SP_STATE_ERROR_MAIN;
		}
	}
	return ret;
}

int CSpParticle::GetPrevDataByTime(SData* out_Data, double in_ViewTime, double* out_KeyPoint)
{
	int ret = SP_STATE_ERROR_MAIN;
	if (m_Data && out_Data && out_KeyPoint)
	{
		int idDn;
		idDn = m_Data->GetLowIdxByTime(in_ViewTime);
		if (idDn < m_Data->CurPos || m_Data->m_Array[idDn].TimePoint < in_ViewTime)
			ret = SP_STATE_ERROR_MAIN;
		else
		if (idDn >= 0)
		{//ѕозици€ между первой и последней точками траектории.
			ret = CalcMidData(out_Data, m_Data->m_Array[idDn], m_Data->m_Array[idDn + 1], in_ViewTime);
			int SpeedPwr1 = CSpParticle::Get3D_Pwr2(m_Data->m_Array[idDn].Speed) >= m_pCalc->maxAbsSpeed ? 1 : 0;
			int SpeedPwr2 = CSpParticle::Get3D_Pwr2(out_Data->Speed) >= m_pCalc->maxAbsSpeed ? 1 : 0;
			if (SpeedPwr1 ^ SpeedPwr2)
			{//был переход с/на предельную скорость
				//!!! не должно срабатывать так как точки траектории учитывают переходы
				ret |= SP_STATE_ABS_CHANGE_LS;
				SData Lo = m_Data->m_Array[idDn];
				SData Hi = m_Data->m_Array[idDn + 1];
				SData Mid = Hi;
				double deltaT;

				while ((deltaT = (Hi.TimePoint - Lo.TimePoint) / 2.0) > m_pCalc->Time.MinStep)
				{
					CalcMidData(&Mid, Lo, Hi, Lo.TimePoint + deltaT);
					double SpeedPwrMidPwr2 = CSpParticle::Get3D_Pwr2(Mid.Speed);
					if (SpeedPwr2)
					{
						if (SpeedPwrMidPwr2 >= m_pCalc->maxAbsSpeed) Hi = Mid; else Lo = Mid;
					}
					else if (SpeedPwrMidPwr2 >= m_pCalc->maxAbsSpeed) Lo = Mid; else Hi = Mid;
				}
				*out_KeyPoint = Hi.TimePoint;
			}
		}
		else
		{//ѕозици€ до первой в траектории
			ret = CalcPrevData(out_Data, in_ViewTime);
			int SpeedPwr1 = CSpParticle::Get3D_Pwr2(m_Data->m_Array[0].Speed) >= m_pCalc->maxAbsSpeed ? 1 : 0;
			int SpeedPwr2 = CSpParticle::Get3D_Pwr2(out_Data->Speed) >= m_pCalc->maxAbsSpeed ? 1 : 0;
			if (SpeedPwr1 ^ SpeedPwr2)
			{//был переход с/на предельную скорость
				ret |= SP_STATE_ABS_CHANGE_LS;
				SData Lo = *out_Data;
				SData Hi = m_Data->m_Array[0];
				SData Mid = Hi;
				double deltaT;

				while ((deltaT = (Hi.TimePoint - Lo.TimePoint) / 2.0) > m_pCalc->Time.MinStep)
				{
					CalcPrevData(out_Data, Lo.TimePoint + deltaT);
					double SpeedPwrMidPwr2 = CSpParticle::Get3D_Pwr2(Mid.Speed);
					if (SpeedPwr2)
					{
						if (SpeedPwrMidPwr2 >= m_pCalc->maxAbsSpeed) Hi = Mid; else Lo = Mid;
					}
					else if (SpeedPwrMidPwr2 >= m_pCalc->maxAbsSpeed) Lo = Mid; else Hi = Mid;
				}
				*out_KeyPoint = Lo.TimePoint;
			}
		}
	}
		
	return ret;
}

int CSpParticle::CalcPrevData(SData* out_Data, double in_DataTime)
{
	int ret = -1;
	if (m_Data && out_Data && m_Data->m_Array && m_Data->CurPos >= 0)
	{
		double KeyPoint = 0.0;
		if (in_DataTime >= m_Data->m_Array[0].TimePoint) ret = GetPrevDataByTime(out_Data, in_DataTime, &KeyPoint);
		else
		{
			*out_Data = m_Data->m_Array[0];
			//ѕравильно посчитать с учетом ускорени€

			out_Data->TimePoint = in_DataTime;
			double dltTime = m_Data->m_Array[0].TimePoint - in_DataTime;
			out_Data->position.X -= m_Data->m_Array[0].Speed.X * dltTime;
			out_Data->position.Y -= m_Data->m_Array[0].Speed.Y * dltTime;
			out_Data->position.Z -= m_Data->m_Array[0].Speed.Z * dltTime;
		}
	}
	return ret;
}

int CSpParticle::GetPointCount()
{
	return m_Data ? m_Data->GetCurPos() + 1 : 0;
}

int CSpParticle::NewPoint_CheckLevel()
{
	int ret = 0;


	return ret;
}


SPos CSpParticle::GetCurPosition()
{
	SPos ret = { 0 };
	if (m_Data && m_Data->CurPos >= 0) ret = m_Data->GetCurData()->position;
	return ret;
}

int CSpParticle::SetParticleData(int in_flags, const SData *in_Ptr)
{
	int ret = 0;

	if (m_Data && in_Ptr)
	{
		SData* curData = m_Data->GetCurData();
		if (curData)
		{
			if ((in_flags & SP_DATA_DENCITY))
			{
				curData->Accel3D = in_Ptr->Accel3D;
				curData->AccelDeriv3D = in_Ptr->AccelDeriv3D;
				ret |= SP_DATA_DENCITY; 
			}
			if ((in_flags & SP_DATA_SPEED)) 
			{
				curData->Speed = in_Ptr->Speed;
				ret |= SP_DATA_SPEED;
				curData->AbsSpeed = CSpParticle::Get3D_Pwr1(curData->Speed);
			}
			if ((in_flags & SP_DATA_POSITION))
			{
				curData->position = in_Ptr->position;
				ret |= SP_DATA_POSITION;
			}
			if ((in_flags & SP_DATA_TIMEPOINT))
			{ 
				curData->TimePoint = in_Ptr->TimePoint;
				ret |= SP_DATA_TIMEPOINT;
			}
		}
		else
		{
			m_Data->AddData(0, in_Ptr);
		}
	}

	return ret;
}

/////////////////////// STATIC ///////////////////////

double CSpParticle::Get3D_Pwr2(const SPos& Ptr)
{
	return Ptr.X * Ptr.X + Ptr.Y * Ptr.Y + Ptr.Z * Ptr.Z;
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
			if ((in_Flags & SP_STATE_REL_SPEED_AL) == SP_STATE_REL_SPEED_AL) out_Str += sprintf_s(out_Str, 1000, "REL_SPEED_AL|");
			if ((in_Flags & SP_STATE_REL_CHANGE_LS) == SP_STATE_REL_CHANGE_LS) out_Str += sprintf_s(out_Str, 1000, "REL_CHANGE_LS|");
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


double CSpParticle::GetDistance(CSpParticle *Ptr1, CSpParticle *Ptr2) {	return CSpParticle::GetDistance(*Ptr1->GetCurData(), *Ptr2->GetCurData());}
double CSpParticle::GetDistance(const SData& Ptr1, const SData& Ptr2) {	return CSpParticle::GetDistance(Ptr1.position, Ptr2.position);}
double CSpParticle::GetDistance(const SPos &Ptr1, const SPos& Ptr2)   {	return sqrt(CSpParticle::GetDistancePwr2(Ptr1, Ptr2));}

double CSpParticle::GetDistancePwr2(CSpParticle* Ptr1, CSpParticle* Ptr2) {	return CSpParticle::GetDistancePwr2(*Ptr1->GetCurData(), *Ptr2->GetCurData());}
double CSpParticle::GetDistancePwr2(const SData& Ptr1, const SData& Ptr2) {	return CSpParticle::GetDistancePwr2(Ptr1.position, Ptr2.position);}
double CSpParticle::GetDistancePwr2(const SPos& Ptr1, const SPos& Ptr2)   
{
	double ret, tmp;
	tmp = Ptr1.X - Ptr2.X;
	ret = tmp * tmp;
	tmp = Ptr1.Y - Ptr2.Y;
	ret += tmp * tmp;
	tmp = Ptr1.Z - Ptr2.Z;
	ret += tmp * tmp;

	return ret;

	//return (Ptr1.X - Ptr2.X) * (Ptr1.X - Ptr2.X) + (Ptr1.Y - Ptr2.Y) * (Ptr1.Y - Ptr2.Y) + (Ptr1.Z - Ptr2.Z) * (Ptr1.Z - Ptr2.Z);
}

double CSpParticle::GetRelativeSpeed(CSpParticle *Ptr1, CSpParticle *Ptr2, SPos* RSpeed3D, SP_Calc* in_pCalcParams) {	return CSpParticle::GetRelativeSpeed(*Ptr1->GetCurData(), *Ptr2->GetCurData(), RSpeed3D, in_pCalcParams);}
double CSpParticle::GetRelativeSpeed(const SData& Ptr1, const SData& Ptr2, SPos* RSpeed3D, SP_Calc* in_pCalcParams)
{
	///дурацкое решение. Ќадо переделать
	SData Next1, Next2;

	CSpParticle::ItgrlFull(&Next1, Ptr1, Ptr1.TimePoint + 0.00000001, in_pCalcParams);
	CSpParticle::ItgrlFull(&Next2, Ptr2, Ptr2.TimePoint + 0.00000001, in_pCalcParams);

	double dist1 = CSpParticle::GetDistance(Ptr1.position, Ptr2.position);
	double dist2 = CSpParticle::GetDistance(Next1.position, Next2.position);

	if (RSpeed3D)
	{
		RSpeed3D->X = (fabs(Next1.position.X - Next2.position.X) - fabs(Ptr1.position.X - Ptr2.position.X)) * 100000000.0; // >0 разбегаютс€
		RSpeed3D->Y = (fabs(Next1.position.Y - Next2.position.Y) - fabs(Ptr1.position.Y - Ptr2.position.Y)) * 100000000.0; // >0 разбегаютс€
		RSpeed3D->Z = (fabs(Next1.position.Z - Next2.position.Z) - fabs(Ptr1.position.Z - Ptr2.position.Z)) * 100000000.0; // >0 разбегаютс€
	}

	return (dist2 - dist1) * 100000000.0;
}

//
//int CSpParticle::FindDensityDerLevel(const SPos* Ptr)
//{
//	int ret = SP_MAX_DIRIV_LEVEL - 1;
//
//	while (ret > 0 && Ptr[ret].X == 0 && Ptr[ret].Y == 0 && Ptr[ret].Z == 0) ret--;
//
//	return ret;
//}

int CSpParticle::Itgrl(SPos* D_Res, SPos* D1, int iMaxLev, double Tx, double DensMltpl)
{
	int ret = 0;
	if (D_Res && D1 && Tx >= 0.0)
		ret = Itgrl(D_Res, D1, Tx, 0, iMaxLev, Tx, DensMltpl);

	return ret;
}

int CSpParticle::Itgrl(SPos* D_Res, SPos* D1, double in_Mltpl, int iLev, int iMaxLev, double Tx, double DensMltpl)
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


int CSpParticle::Itgrl_s(SPos* D_Res, SPos* D1, int iMaxLev, double Tx)
{
	int ret = 0;
	if (D_Res && D1 && Tx >= 0.0)
		ret = CSpParticle::Itgrl_s(D_Res, D1, Tx, 0, iMaxLev, Tx);

	return ret;
}

int CSpParticle::Itgrl_s(SPos* D_Res, SPos* D1, double in_Mltpl, int iLev, int iMaxLev, double Tx)
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

int CSpParticle::ItgrlFull(SData* Result, const SData& DataSrc, double in_Time, SP_Calc* in_pCalcParams)
{
	int ret = SP_STATE_ERROR_MAIN;

	if (in_pCalcParams && Result && DataSrc.TimePoint <= in_Time)
	{
		ret = SP_STATE_ќ ;
		if (DataSrc.TimePoint < in_Time)
		{
			*Result = { 0 };
			Result->TimePoint = in_Time;
			double deltaTime = in_Time - DataSrc.TimePoint;
			double deltaTimeSpeed = deltaTime;
			double deltaTimeSpeedD2 = deltaTimeSpeed * 0.5;
			double deltaTimeSpeedD3 = deltaTimeSpeed * 0.33333333333333333;

			double SrcSpeedPwr2 = CSpParticle::Get3D_Pwr2(DataSrc.Speed);

			if (in_pCalcParams->DensityDerivLevel == 3)
			{
				Result->Accel3D.X = DataSrc.Accel3D.X + DataSrc.AccelDeriv3D.X * deltaTime;
				Result->Accel3D.Y = DataSrc.Accel3D.Y + DataSrc.AccelDeriv3D.Y * deltaTime;
				Result->Accel3D.Z = DataSrc.Accel3D.Z + DataSrc.AccelDeriv3D.Z * deltaTime;

				///!!!!!!!!!!!!!!!!!!!!!!!!!!!
				Result->Density = 1.0 - CSpParticle::Get3D_Pwr1(Result->Accel3D);

				//Speed = SrcSpeed + SrcDensity3D * t + SrcDensityDeriv3D * t^2 / 2,

				Result->Speed.X = DataSrc.Speed.X + deltaTimeSpeed * (DataSrc.Accel3D.X + DataSrc.AccelDeriv3D.X * deltaTimeSpeedD2);
				Result->Speed.Y = DataSrc.Speed.Y + deltaTimeSpeed * (DataSrc.Accel3D.Y + DataSrc.AccelDeriv3D.Y * deltaTimeSpeedD2);
				Result->Speed.Z = DataSrc.Speed.Z + deltaTimeSpeed * (DataSrc.Accel3D.Z + DataSrc.AccelDeriv3D.Z * deltaTimeSpeedD2);

				double tmpSpeedPwr2 = CSpParticle::Get3D_Pwr2(Result->Speed);
				Result->AbsSpeed = sqrt(tmpSpeedPwr2);

				if (tmpSpeedPwr2 >= in_pCalcParams->maxAbsSpeed)
				{
					ret |= SP_STATE_ABS_SPEED_AL;
					if (tmpSpeedPwr2 > in_pCalcParams->maxAbsSpeed)
					{
						double mltpl = in_pCalcParams->maxAbsSpeed / CSpParticle::Get3D_Pwr1(Result->Speed);
						Result->Speed.X *= mltpl;
						Result->Speed.Y *= mltpl;
						Result->Speed.Z *= mltpl;
					}
					if (SrcSpeedPwr2 < in_pCalcParams->maxAbsSpeed)
						ret |= SP_STATE_ABS_CHANGE_LS;
				}
				else
					if (SrcSpeedPwr2 >= in_pCalcParams->maxAbsSpeed)
						ret |= SP_STATE_ABS_CHANGE_LS;

				Result->position.X = DataSrc.position.X + deltaTimeSpeed * (DataSrc.Speed.X + deltaTimeSpeedD2 * (DataSrc.Accel3D.X + DataSrc.AccelDeriv3D.X * deltaTimeSpeedD3));
				Result->position.Y = DataSrc.position.Y + deltaTimeSpeed * (DataSrc.Speed.Y + deltaTimeSpeedD2 * (DataSrc.Accel3D.Y + DataSrc.AccelDeriv3D.Y * deltaTimeSpeedD3));
				Result->position.Z = DataSrc.position.Z + deltaTimeSpeed * (DataSrc.Speed.Z + deltaTimeSpeedD2 * (DataSrc.Accel3D.Z + DataSrc.AccelDeriv3D.Z * deltaTimeSpeedD3));
			}
			else if (in_pCalcParams->DensityDerivLevel == 2)
			{
				//S = S0 + V0 * t + (a * t^2)/2,
				Result->Speed.X = DataSrc.Speed.X + DataSrc.Accel3D.X * deltaTimeSpeed;
				Result->Speed.Y = DataSrc.Speed.Y + DataSrc.Accel3D.Y * deltaTimeSpeed;
				Result->Speed.Z = DataSrc.Speed.Z + DataSrc.Accel3D.Z * deltaTimeSpeed;

				///!!!!!!!!!!!!!!!!!!!!!!!!!!!
				Result->Density = 1.0 - CSpParticle::Get3D_Pwr1(Result->Accel3D);

				if (CSpParticle::Get3D_Pwr2(Result->Speed) >= in_pCalcParams->maxAbsSpeed)
				{
					ret |= SP_STATE_ABS_SPEED_AL;
					if (CSpParticle::Get3D_Pwr2(Result->Speed) > in_pCalcParams->maxAbsSpeed)
					{
						double mltpl = 1.0 / CSpParticle::Get3D_Pwr1(Result->Speed);
						Result->Speed.X *= mltpl;
						Result->Speed.Y *= mltpl;
						Result->Speed.Z *= mltpl;
					}
					if (SrcSpeedPwr2 < 1.0)
						ret |= SP_STATE_ABS_CHANGE_LS;
				}
				else if (SrcSpeedPwr2 >= 1.0)
					ret |= SP_STATE_ABS_CHANGE_LS;

				Result->position.X = DataSrc.position.X + deltaTimeSpeed * (DataSrc.Speed.X + DataSrc.Accel3D.X * deltaTimeSpeedD2);
				Result->position.Y = DataSrc.position.Y + deltaTimeSpeed * (DataSrc.Speed.Y + DataSrc.Accel3D.Y * deltaTimeSpeedD2);
				Result->position.Z = DataSrc.position.Z + deltaTimeSpeed * (DataSrc.Speed.Z + DataSrc.Accel3D.Z * deltaTimeSpeedD2);
			}
		}
		else *Result = DataSrc;
	}
	return ret;
}




//T1 = 0  Tx = absMidPoint - Ptr1.TimePoint K=D1
//9--D[0](Tx) = D1[0]
//8--D[1](Tx) = D1[1] + D1[0] * Tx
//7--D[2](Tx) = D1[2] + D1[1] * Tx + D1[0] * Tx ^ 2 / 2
//6--D[3](Tx) = D1[3] + D1[2] * Tx + D1[1] * Tx ^ 2 / 2 + D1[0] * Tx ^ 3 / 6
//5--D[4](Tx) = D1[4] + D1[3] * Tx + D1[2] * Tx ^ 2 / 2 + D1[1] * Tx ^ 3 / 6 + D1[0] * Tx ^ 4 / 24
//4--D[5](Tx) = D1[5] + D1[4] * Tx + D1[3] * Tx ^ 2 / 2 + D1[2] * Tx ^ 3 / 6 + D1[1] * Tx ^ 4 / 24 + D1[0] * Tx ^ 5 / 120
//3--D[6](Tx) = D1[6] + D1[5] * Tx + D1[4] * Tx ^ 2 / 2 + D1[3] * Tx ^ 3 / 6 + D1[2] * Tx ^ 4 / 24 + D1[1] * Tx ^ 5 / 120 + D1[0] * Tx ^ 6 / 720
//2--D[7](Tx) = D1[7] + D1[6] * Tx + D1[5] * Tx ^ 2 / 2 + D1[4] * Tx ^ 3 / 6 + D1[3] * Tx ^ 4 / 24 + D1[2] * Tx ^ 5 / 120 + D1[1] * Tx ^ 6 / 720 + D1[0] * Tx ^ 7 / 5040
//1--D[8](Tx) = D1[8] + D1[7] * Tx + D1[6] * Tx ^ 2 / 2 + D1[5] * Tx ^ 3 / 6 + D1[4] * Tx ^ 4 / 24 + D1[3] * Tx ^ 5 / 120 + D1[2] * Tx ^ 6 / 720 + D1[1] * Tx ^ 7 / 5040 + D1[0] * Tx ^ 8 / 40320
//0--D[9](Tx) = D1[9] + D1[8] * Tx + D1[7] * Tx ^ 2 / 2 + D1[6] * Tx ^ 3 / 6 + D1[5] * Tx ^ 4 / 24 + D1[4] * Tx ^ 5 / 120 + D1[3] * Tx ^ 6 / 720 + D1[2] * Tx ^ 7 / 5040 + D1[1] * Tx ^ 8 / 40320 + D1[0] * Tx ^ 9 / 362880
//...........

//D[0](Tx) = K[0]
//D[1](Tx) = K[1] + K[0] * Tx
//D[2](Tx) = K[2] + K[1] * Tx + K[0] * Tx ^ 2 / 2
//D[3](Tx) = K[3] + K[2] * Tx + K[1] * Tx ^ 2 / 2 + K[0] * Tx ^ 3 / 6
//D[4](Tx) = K[4] + K[3] * Tx + K[2] * Tx ^ 2 / 2 + K[1] * Tx ^ 3 / 6 + K[0] * Tx ^ 4 / 24
//D[5](Tx) = K[5] + K[4] * Tx + K[3] * Tx ^ 2 / 2 + K[2] * Tx ^ 3 / 6 + K[1] * Tx ^ 4 / 24 + K[0] * Tx ^ 5 / 120
//D[6](Tx) = K[6] + K[5] * Tx + K[4] * Tx ^ 2 / 2 + K[3] * Tx ^ 3 / 6 + K[2] * Tx ^ 4 / 24 + K[1] * Tx ^ 5 / 120 + K[0] * Tx ^ 6 / 720
//D[7](Tx) = K[7] + K[6] * Tx + K[5] * Tx ^ 2 / 2 + K[4] * Tx ^ 3 / 6 + K[3] * Tx ^ 4 / 24 + K[2] * Tx ^ 5 / 120 + K[1] * Tx ^ 6 / 720 + K[0] * Tx ^ 7 / 5040
//D[8](Tx) = K[8] + K[7] * Tx + K[6] * Tx ^ 2 / 2 + K[5] * Tx ^ 3 / 6 + K[4] * Tx ^ 4 / 24 + K[3] * Tx ^ 5 / 120 + K[2] * Tx ^ 6 / 720 + K[1] * Tx ^ 7 / 5040 + K[0] * Tx ^ 8 / 40320
//D[9](Tx) = K[9] + K[8] * Tx + K[7] * Tx ^ 2 / 2 + K[6] * Tx ^ 3 / 6 + K[5] * Tx ^ 4 / 24 + K[4] * Tx ^ 5 / 120 + K[3] * Tx ^ 6 / 720 + K[2] * Tx ^ 7 / 5040 + K[1] * Tx ^ 8 / 40320 + K[0] * Tx ^ 9 / 362880

//T1 != 0  Tx = absMidPoint
//K[0] = D[0][1]
//K[1] = D[1][1] - K[0] * T1
//K[2] = D[2][1] - K[1] * T1 - K[0] * T1 ^ 2 / 2
//K[3] = D[3][1] - K[2] * T1 - K[1] * T1 ^ 2 / 2 - K[0] * T1 ^ 3 / 6
//K[4] = D[4][1] - K[3] * T1 - K[2] * T1 ^ 2 / 2 - K[1] * T1 ^ 3 / 6 - K[0] * T1 ^ 4 / 24
//K[5] = D[5][1] - K[4] * T1 - K[3] * T1 ^ 2 / 2 - K[2] * T1 ^ 3 / 6 - K[1] * T1 ^ 4 / 24 - K[0] * T1 ^ 5 / 120
//K[6] = D[6][1] - K[5] * T1 - K[4] * T1 ^ 2 / 2 - K[3] * T1 ^ 3 / 6 - K[2] * T1 ^ 4 / 24 - K[1] * T1 ^ 5 / 120 - K[0] * T1 ^ 6 / 720
//K[7] = D[7][1] - K[6] * T1 - K[5] * T1 ^ 2 / 2 - K[4] * T1 ^ 3 / 6 - K[3] * T1 ^ 4 / 24 - K[2] * T1 ^ 5 / 120 - K[1] * T1 ^ 6 / 720 - K[0] * T1 ^ 7 / 5040
//K[8] = D[8][1] - K[7] * T1 - K[6] * T1 ^ 2 / 2 - K[5] * T1 ^ 3 / 6 - K[4] * T1 ^ 4 / 24 - K[3] * T1 ^ 5 / 120 - K[2] * T1 ^ 6 / 720 - K[1] * T1 ^ 7 / 5040 - K[0] * T1 ^ 8 / 40320
//K[9] = D[9][1] - K[8] * T1 - K[7] * T1 ^ 2 / 2 - K[6] * T1 ^ 3 / 6 - K[5] * T1 ^ 4 / 24 - K[4] * T1 ^ 5 / 120 - K[3] * T1 ^ 6 / 720 - K[2] * T1 ^ 7 / 5040 - K[1] * T1 ^ 8 / 40320 - K[0] * T1 ^ 9 / 362880

//T1 = 0  Tx = absMidPoint - Ptr1.TimePoint
//K[0] = D[0][1]  
//K[1] = D[1][1]  
//K[2] = D[2][1]  
//K[3] = D[3][1]  
//K[4] = D[4][1]  
//K[5] = D[5][1]  
//K[6] = D[6][1]  
//K[7] = D[7][1]  
//K[8] = D[8][1]  
//K[9] = D[9][1]  

/*
P0 - Pn - параметры
K[0] = K = (R[0]2 - R[0]1) / (T2 - T1)
K[1] = N = R[0]1 Ц K[0] * T1
K[2] = W = R[1]1 Ц R[0]1 * T1 + K[0] * (T1 ^ 2) / 2

R[0] = A(t) = K[0] * T + K[1]
R[1] = V(t) = K[2] + T * K[1] + K[0] * (T ^ 2) / 2
R[2] = S(t) = R[2]1 + K[2] * (T - T1) + K[1] * (T ^ 2 - T1 ^ 2) / 2 + K[0] * (T ^ 3 - T1 ^ 3) / 6


R[2] = S(t) = K[2] * (T - T1) + K[1] * (T ^ 2 - T1 ^ 2) / 2 + K[0] * (T ^ 3 - T1 ^ 3) / 6 + R[2]1
R[2] = S(t) = K[2] * T - K[2] * T1 + K[1] * T ^ 2 / 2 - K[1] * T1 ^ 2 / 2 + K[0] * T ^ 3 / 6 - K[0] * T1 ^ 3 / 6 + R[2]1
R[2] = S(t) = K[2] * T - K[2] * T1 + K[1] * T ^ 2 / 2 - K[1] * T1 ^ 2 / 2 + K[0] * T ^ 3 /6- K[0] * T1 ^ 3 / 6 + R[2]1


D[0](Tx) = D[0][1] = D[0][2] 

D[1](Tx) = D[1][1] + itg(T1,Tx, D[0][1]) = D[1][1] + dtx(T1, Tx, D[0][1]*Tx) = D[1][1] + D[0][1] * Tx - D[0][1] * T1 = D[1][1] - D[0][1] * T1 + D[0][1] * Tx = K[1] + D[0][1] * Tx    
/// K[1] = D[1][1] - D[0][1] * T1

D[2](Tx) = D[2][1] + itg(T1, Tx, K[1] + D[0][1] * Tx) = D[2][1] + dtx(T1, Tx, K[1]*Tx + D[0][1]*Tx^2/2) = D[2][1] + K[1] * Tx + D[0][1] * Tx ^ 2 / 2 - K[1] * T1 - D[0][1] * T1 ^ 2 / 2 = D[2][1] - K[1] * T1 - D[0][1] * T1 ^ 2 / 2 + K[1] * Tx + D[0][1] * Tx ^ 2 / 2 = K[2] + K[1] * Tx + D[0][1] * Tx ^ 2 / 2 
/// K[2] = D[2][1] - K[1] * T1 - D[0][1] * T1 ^ 2 / 2

D[3](Tx) = D[3][1] + itg(T1, Tx, K[2] + K[1] * Tx + D[0][1] * Tx ^ 2 / 2) = D[3][1] + dtx(T1, Tx, K[2]*Tx + K[1]*Tx^2/2 + D[0][1]*Tx^3/6) = D[3][1] + K[2] * Tx + K[1] * Tx ^ 2 / 2 + D[0][1] * Tx ^ 3 / 6  - (K[2] * T1 + K[1] * T1 ^ 2 / 2 + D[0][1] * T1 ^ 3 / 6) = K[3] + K[2] * Tx + K[1] * Tx ^ 2 / 2 + D[0][1] * Tx ^ 3 / 6
// K[3] = D[3][1]  - K[2] * T1 - K[1] * T1 ^ 2 / 2 - D[0][1] * T1 ^ 3 / 6 

D[4](Tx) = D[4][1] + itg(T1, Tx, K[3] + K[2]*Tx + K[1]*Tx^2/2 + D[0][1]*Tx^3/6) = D[4][1] + dtx(T1, Tx, K[3]*Tx + K[2]*Tx^2/2 + K[1]*Tx^3/6 + D[0][1]*Tx^4/24) = D[4][1] + ( K[3] * Tx + K[2] * Tx ^ 2 / 2 + K[1] * Tx ^ 3 / 6 + D[0][1] * Tx ^ 4 / 24) - ( K[3] * T1 + K[2] * T1 ^ 2 / 2 + K[1] * T1 ^ 3 / 6 + D[0][1] * T1 ^ 4 / 24) = K[4] + K[3] * Tx + K[2] * Tx ^ 2 / 2 + K[1] * Tx ^ 3 / 6 + D[0][1] * Tx ^ 4 / 24
// K[4] = D[4][1] -  K[3] * T1 - K[2] * T1 ^ 2 / 2 - K[1] * T1 ^ 3 / 6 - D[0][1] * T1 ^ 4 / 24


//



D[-](Tx) = D[-][1] + itg(T1, Tx, ) = D[-][1] + dtx(T1, Tx, ) =


D[0](Tx) = D[0][1]
D[1](Tx) = K[1] + D[0][1] * Tx
D[2](Tx) = K[2] + K[1] * Tx + D[0][1] * Tx ^ 2 / 2
D[3](Tx) = K[3] + K[2] * Tx + K[1] * Tx ^ 2 / 2 + D[0][1] * Tx ^ 3 / 6
D[4](Tx) = K[4] + K[3] * Tx + K[2] * Tx ^ 2 / 2 + K[1] * Tx ^ 3 / 6 + D[0][1] * Tx ^ 4 / 24
D[5](Tx) = K[5] + K[4] * Tx + K[3] * Tx ^ 2 / 2 + K[2] * Tx ^ 3 / 6 + K[1] * Tx ^ 4 / 24 + D[0][1] * Tx ^ 5 / 120
D[6](Tx) = K[6] + K[5] * Tx + K[4] * Tx ^ 2 / 2 + K[3] * Tx ^ 3 / 6 + K[2] * Tx ^ 4 / 24 + K[1] * Tx ^ 5 / 120 + D[0][1] * Tx ^ 6 / 720
D[7](Tx) = K[7] + K[6] * Tx + K[5] * Tx ^ 2 / 2 + K[4] * Tx ^ 3 / 6 + K[3] * Tx ^ 4 / 24 + K[2] * Tx ^ 5 / 120 + K[1] * Tx ^ 6 / 720 + D[0][1] * Tx ^ 7 / 5040
D[8](Tx) = K[8] + K[7] * Tx + K[6] * Tx ^ 2 / 2 + K[5] * Tx ^ 3 / 6 + K[4] * Tx ^ 4 / 24 + K[3] * Tx ^ 5 / 120 + K[2] * Tx ^ 6 / 720 + K[1] * Tx ^ 7 / 5040 + D[0][1] * Tx ^ 8 / 40320
D[9](Tx) = K[9] + K[8] * Tx + K[7] * Tx ^ 2 / 2 + K[6] * Tx ^ 3 / 6 + K[5] * Tx ^ 4 / 24 + K[4] * Tx ^ 5 / 120 + K[3] * Tx ^ 6 / 720 + K[2] * Tx ^ 7 / 5040 + K[1] * Tx ^ 8 / 40320 + D[0][1] * Tx ^ 9 / 362880



K[1] = D[1][1] - D[0][1] * T1
K[2] = D[2][1] - K[1] * T1 - D[0][1] * T1 ^ 2 / 2
K[3] = D[3][1] - K[2] * T1 - K[1] * T1 ^ 2 / 2 - D[0][1] * T1 ^ 3 / 6 
K[4] = D[4][1] - K[3] * T1 - K[2] * T1 ^ 2 / 2 - K[1] * T1 ^ 3 / 6 - D[0][1] * T1 ^ 4 / 24
K[5] = D[5][1] - K[4] * T1 - K[3] * T1 ^ 2 / 2 - K[2] * T1 ^ 3 / 6 - K[1] * T1 ^ 4 / 24 - D[0][1] * T1 ^ 5 / 120
K[6] = D[6][1] - K[5] * T1 - K[4] * T1 ^ 2 / 2 - K[3] * T1 ^ 3 / 6 - K[2] * T1 ^ 4 / 24 - K[1] * T1 ^ 5 / 120 - D[0][1] * T1 ^ 6 / 720
K[7] = D[7][1] - K[6] * T1 - K[5] * T1 ^ 2 / 2 - K[4] * T1 ^ 3 / 6 - K[3] * T1 ^ 4 / 24 - K[2] * T1 ^ 5 / 120 - K[1] * T1 ^ 6 / 720 - D[0][1] * T1 ^ 7 / 5040
K[8] = D[8][1] - K[7] * T1 - K[6] * T1 ^ 2 / 2 - K[5] * T1 ^ 3 / 6 - K[4] * T1 ^ 4 / 24 - K[3] * T1 ^ 5 / 120 - K[2] * T1 ^ 6 / 720 - K[1] * T1 ^ 7 / 5040 - D[0][1] * T1 ^ 8 / 40320
K[9] = D[9][1] - K[8] * T1 - K[7] * T1 ^ 2 / 2 - K[6] * T1 ^ 3 / 6 - K[5] * T1 ^ 4 / 24 - K[4] * T1 ^ 5 / 120 - K[3] * T1 ^ 6 / 720 - K[2] * T1 ^ 7 / 5040 - K[1] * T1 ^ 8 / 40320 - D[0][1] * T1 ^ 9 / 362880


*/


int CSpParticle::CalcMidData(SData* Result, const SData& Ptr1, const SData& Ptr2, double absMidPoint)
{
	int ret = SP_STATE_ќ ;

	if (Result)
	{
		ZeroMemory(Result, sizeof(SData));
		if (Ptr1.TimePoint >= absMidPoint)
		{
			ret = SP_STATE_DOWN;
			if (Ptr1.TimePoint == absMidPoint) *Result = Ptr1; else ret |= SP_STATE_ERROR_MAIN;
		}
		else if (Ptr2.TimePoint <= absMidPoint)
		{
			ret = SP_STATE_UP;
			if (Ptr2.TimePoint == absMidPoint) *Result = Ptr2; else ret |= SP_STATE_ERROR_MAIN;
		}
		else
		{
			ret = SP_STATE_MID;
			ItgrlFull(Result, Ptr1, absMidPoint, m_pCalc);

			Result->TimePoint = absMidPoint;
		}
	}
	return ret;
}



void CSpParticle::GetMinMaxTracePos(SPos& min_Pos, SPos& max_Pos)
{
	m_Data->GetMinMaxTracePos(min_Pos, max_Pos);
}