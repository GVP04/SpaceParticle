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


SData* CSpParticle::GetDataById(int in_Id)
{
	return m_Data == NULL || m_Data->m_Array == NULL || m_Data->CurPos < 0 || in_Id  < 0 || in_Id > m_Data->CurPos ? NULL : m_Data->m_Array +in_Id;
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
	return m_Data && m_Data->CurPos >=0 ? m_Data->GetCurData()->TimePoint : 0.0;
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
		if (m_NextData.position == m_Data->m_Array[m_Data->CurPos].position)
			ret = 1;

		m_Data->AddData( in_maxCountLimit, &m_NextData);
		NewPoint_Clear();
		ret = 1;
	}
	else
		ret = 0;

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
					out_Str = GetFlagsDescription(out_Str, 0);
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
				out_Str = GetFlagsDescription(out_Str, ret);
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

int CSpParticle::FindViewPointEx(SData* out_Data, SData* in_Pos, double in_ViewTime)
{
	int idMax = 0;
	//if (in_LastRelation) // && in_LastRelation->State != SP_STATE_FAR_AWAY)
	//{
	//idMin = m_Data->GetLowIdxByTime(in_StartTime);
	//idMax = m_Data->GetLowIdxByTime(in_EndTime) + 1;
	//if (idMax > m_Data->CurPos) idMax = m_Data->CurPos;
	//}
	//else
	//{
	//	idMax = m_Data->GetLowIdxByTime(in_ViewTime) + 1;  
	//	if (idMax > m_Data->CurPos) idMax = m_Data->CurPos;
	//	idMin = -1;

	//	if (idMax > 0)
	//	{
	//		double tmpDistance = in_Pos->position.GetDistance(m_Data->m_Array[idMax].position);
	//		if ((idMax = m_Data->GetLowIdxByTime(m_Data->m_Array[idMax].TimePoint - 0.5 * tmpDistance)) > 0)
	//		{
	//			idMin = m_Data->GetLowIdxByTime(m_Data->m_Array[idMax].TimePoint - 2.5 * tmpDistance);
	//			if (idMin < 0) idMin = 0;
	//		}
	//	}
	//}
	return 0;
}

//напоминание -- время изменения плотности == расстоянию
int CSpParticle::FindViewPoint(SData* out_Data, SData* in_Pos, double in_ViewTime, double in_StartTime, double in_EndTime)
{
	int ret = SP_STATE_ОК;
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
		int idMin = m_Data->CurPos;
		
		if (m_Data->CurPos > 0)
		{
			int idMax = 0;
			idMin = m_Data->GetLowIdxByTime(in_StartTime);
			int idMinTmp = idMin;
			idMax = m_Data->GetLowIdxByTime(in_EndTime) + 1;
			if (idMax > m_Data->CurPos) idMax = m_Data->CurPos;

			if (idMin < 0 && idMax > 0) idMin = 0;

			if (idMin < m_Data->CurPos && idMin >= 0)
			{
				double dltTpwr2 = 0.0;

				for(int kk = 1; dltTpwr2 >= 0.0 && idMin + kk < idMax; kk*=2)
				{
						dltTpwr2 = (in_ViewTime - m_Data->m_Array[idMin + kk].TimePoint);
						dltTpwr2 = dltTpwr2 * dltTpwr2 - in_Pos->position.GetDistancePwr2(m_Data->m_Array[idMin + kk].position);
						if (dltTpwr2 <= 0.0) idMax = idMin + kk;
						else idMin += kk;
				}

				while (idMax > idMin)
				{
					int idMid = ((idMax - idMin + 1) >> 1) + idMin;

					dltTpwr2 = (in_ViewTime - m_Data->m_Array[idMid].TimePoint);
					dltTpwr2 = dltTpwr2 * dltTpwr2 - in_Pos->position.GetDistancePwr2(m_Data->m_Array[idMid].position);
					if (dltTpwr2 > 0.0)
						idMin = idMid;
					else
						if (idMax != idMid) idMax = idMid;
						else idMax--;
				}

				if (idMin < 0 && idMax > 0)
				{
					idMax = 0;
					idMin = idMinTmp;
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

				//tSwap = (in_ViewTime - Dn.TimePoint);
				//double dltDn = tSwap * tSwap - in_Pos->position.GetDistancePwr2(Dn.position);
				//tSwap = (in_ViewTime - Up.TimePoint);
				//double dltUp = tSwap * tSwap - in_Pos->position.GetDistancePwr2(Up.position);


				while (ret == SP_STATE_INPROC)
				{
					double newTime = Dn.TimePoint + (Up.TimePoint - Dn.TimePoint) * 0.5; // возможно ускорить за счет линейной интерполяции
					switch(CalcMidData(&Mid, Dn, Up, newTime))
					{
					case SP_STATE_UP:
					case SP_STATE_DOWN:
						ret = SP_STATE_ОК;
						break;
					case SP_STATE_MID:
						tSwap = (in_ViewTime - Mid.TimePoint);
						tSwap = tSwap * tSwap - in_Pos->position.GetDistancePwr2(Mid.position);
						if (tSwap < 0.0) Up = Mid;	else Dn = Mid;
						if (Up.TimePoint - Dn.TimePoint < maxDelta) ret = SP_STATE_ОК;
						break;
					default: //error
						ret = SP_STATE_ERROR_MAIN;
						break;
					}
				}
				if (!(ret & SP_STATE_ERROR_MAIN)) *out_Data = Mid;
			}
			else
			{ //выход за пределы первой точки траектории. Эмуляция прямолинейного равномерного движения. Если относительная скорость выше допустипой меняем ret
				ret = SP_STATE_LOW_VIEW|SP_STATE_ERROR_MINOR;
				*out_Data = m_Data->m_Array[0];
				SData Up;
				Up = m_Data->m_Array[0];

				if (Up.Speed.X == 0.0 && Up.Speed.Y == 0.0 && Up.Speed.Z == 0.0)
				{
					ret |= SP_STATE_ABS_0_SPEED;
					out_Data->TimePoint = in_ViewTime - in_Pos->position.GetDistance(m_Data->m_Array[0].position);
				}
				else
				{
					double newTimeDlt;
					newTimeDlt = Up.TimePoint - in_StartTime;

					double tSwap, maxDelta = newTimeDlt * 0.00001;
					if (maxDelta < m_pCalc->Time.MinStep) maxDelta = m_pCalc->Time.MinStep;
					//оптимизировать по делителю. Пока здесь производительность не важна
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
						tSwap = tSwap * tSwap - in_Pos->position.GetDistancePwr2(Mid.position);
						if (tSwap < 0.0)
							Up = Mid;
					}
					*out_Data = Mid;
				}
			}
		}
		else
		{//время больше времени в последней позиции масиива траектории
			ret = SP_STATE_HIGH_VIEW | SP_STATE_ERROR_MINOR;
			double KeyPoint = 0.0;
			GetNextDataByTime(out_Data, in_ViewTime, &KeyPoint);
		}

		if (ret > 0 && fabs(out_Data->GetRelativeSpeed(*in_Pos, NULL)) >= m_pCalc->maxRelativeSpeed)
			ret = SP_STATE_REL_SPEED_AL | SP_STATE_ERROR_MINOR;
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

int CSpParticle::GetLowIdxByTime(double in_ViewTime)
{
	int ret = -1;
	if (m_Data && m_Data->CurPos >= 0)
		ret = m_Data->GetLowIdxByTime(in_ViewTime);

	return ret;
}

int CSpParticle::GetDataById(int in_Idx, SData* inout_Reseult)
{
	int ret = -1;
	if (inout_Reseult)
	{
		if (m_Data && m_Data->m_Array && in_Idx >= 0 && in_Idx <= m_Data->CurPos)
			*inout_Reseult = m_Data->m_Array[ret = in_Idx];
		else
			inout_Reseult->CLEAR();
	}

	return ret;
}

int CSpParticle::GetMaxIdx()
{
	int ret = -2;
	if (m_Data)
		ret = m_Data->CurPos;

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
		{//позиция после последней в траектории
			ret = SP_STATE_ОК;

			ItgrlFull(out_Data, m_Data->m_Array[m_Data->CurPos], in_ViewTime, m_pCalc);

			int SpeedPwr1 = (m_Data->m_Array + m_Data->CurPos)->AbsSpeed >= m_pCalc->maxAbsSpeed ? 1 : 0;
			int SpeedPwr2 = out_Data->AbsSpeed >= m_pCalc->maxAbsSpeed ? 1 : 0;

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
					if (SpeedPwr2)
					{
						if (Mid.AbsSpeed >= m_pCalc->maxAbsSpeed) Hi = Mid; else Lo = Mid;
					}
					else if (Mid.AbsSpeed >= m_pCalc->maxAbsSpeed) Lo = Mid; else Hi = Mid;
				}
				*out_KeyPoint = Hi.TimePoint;
			}
		}
		else 
			if (idDn >= 0)
		{//Позиция между первой и последней точками траектории.
			ret = SP_STATE_ERROR_MAIN;
		}
		else
		{//Позиция до первой в траектории
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
		if (idDn == m_Data->CurPos)
		{
			if (m_Data->m_Array[idDn].TimePoint == in_ViewTime)
			{
				ret = SP_STATE_ОК;
				*out_Data = m_Data->m_Array[idDn];
			}
		}
		else
			if (idDn > m_Data->CurPos)
				*out_Data = { 0 };
		else
		if (idDn >= 0)
		{//Позиция между первой и последней точками траектории.
			ret = CalcMidData(out_Data, m_Data->m_Array[idDn], m_Data->m_Array[idDn + 1], in_ViewTime);
			int SpeedPwr1 = m_Data->m_Array[idDn].AbsSpeed >= m_pCalc->maxAbsSpeed ? 1 : 0;
			int SpeedPwr2 = out_Data->AbsSpeed >= m_pCalc->maxAbsSpeed ? 1 : 0;
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
					if (SpeedPwr2)
					{
						if (Mid.AbsSpeed >= m_pCalc->maxAbsSpeed) Hi = Mid; else Lo = Mid;
					}
					else if (Mid.AbsSpeed >= m_pCalc->maxAbsSpeed) Lo = Mid; else Hi = Mid;
				}
				*out_KeyPoint = Hi.TimePoint;
			}
		}
		else
		{//Позиция до первой в траектории
			ret = CalcPrevData(out_Data, in_ViewTime);
			int SpeedPwr1 = m_Data->m_Array[0].AbsSpeed >= m_pCalc->maxAbsSpeed ? 1 : 0;
			int SpeedPwr2 = out_Data->AbsSpeed >= m_pCalc->maxAbsSpeed ? 1 : 0;
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
					if (SpeedPwr2)
					{
						if (Mid.AbsSpeed >= m_pCalc->maxAbsSpeed) Hi = Mid; else Lo = Mid;
					}
					else if (Mid.AbsSpeed >= m_pCalc->maxAbsSpeed) Lo = Mid; else Hi = Mid;
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
			//Правильно посчитать с учетом ускорения

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

SPos CSpParticle::GetCurPosition()
{
	SPos ret = { 0 };
	if (m_Data && m_Data->CurPos >= 0) ret = m_Data->GetCurData()->position;
	return ret;
}

int CSpParticle::SetParticleData(int in_flags, SData *in_Ptr)
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
				curData->AbsSpeed = curData->Speed.LEN();
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
		ret = SP_STATE_ОК;
		if (DataSrc.TimePoint < in_Time)
		{
			*Result = DataSrc;
			Result->TimePoint = in_Time;

			double deltaTime = in_Time - DataSrc.TimePoint;
			double deltaTimeD2 = deltaTime * 0.5;

			Result->Accel3D = DataSrc.Accel3D;

			if (in_pCalcParams->DensityDerivLevel >= 3)
			{
				ASSERT(0);
				/// Надо реализовать для ускорения. Пока нет реализации расчета на превышение максимальной скорости. Gthatrwbjybpv jnls[ftn gjrf/ 
				Result->Accel3D.X += DataSrc.AccelDeriv3D.X * deltaTime;
				Result->Accel3D.Y += DataSrc.AccelDeriv3D.Y * deltaTime;
				Result->Accel3D.Z += DataSrc.AccelDeriv3D.Z * deltaTime;

				Result->Speed.X += deltaTime * (DataSrc.Accel3D.X + DataSrc.AccelDeriv3D.X * deltaTimeD2);
				Result->Speed.Y += deltaTime * (DataSrc.Accel3D.Y + DataSrc.AccelDeriv3D.Y * deltaTimeD2);
				Result->Speed.Z += deltaTime * (DataSrc.Accel3D.Z + DataSrc.AccelDeriv3D.Z * deltaTimeD2);

				Result->AbsSpeed = Result->Speed.LEN();

				if (DataSrc.AbsSpeed < in_pCalcParams->maxAbsSpeed)
					ret |= SP_STATE_ABS_CHANGE_LS;

				double deltaTimeD3 = deltaTime * 0.33333333333333333;

				if (Result->AbsSpeed >= in_pCalcParams->maxAbsSpeed)
				{
					ret |= SP_STATE_ABS_SPEED_AL;
					if (Result->AbsSpeed > in_pCalcParams->maxAbsSpeed)
					{
						Result->Speed.SET_LENGHT(in_pCalcParams->maxAbsSpeed);
					}

					Result->position.X += deltaTime * (DataSrc.Speed.X + deltaTimeD2 * (DataSrc.Accel3D.X + DataSrc.AccelDeriv3D.X * deltaTimeD3));
					Result->position.Y += deltaTime * (DataSrc.Speed.Y + deltaTimeD2 * (DataSrc.Accel3D.Y + DataSrc.AccelDeriv3D.Y * deltaTimeD3));
					Result->position.Z += deltaTime * (DataSrc.Speed.Z + deltaTimeD2 * (DataSrc.Accel3D.Z + DataSrc.AccelDeriv3D.Z * deltaTimeD3));
				}
				else
				{
					Result->position.X += deltaTime * (DataSrc.Speed.X + deltaTimeD2 * (DataSrc.Accel3D.X + DataSrc.AccelDeriv3D.X * deltaTimeD3));
					Result->position.Y += deltaTime * (DataSrc.Speed.Y + deltaTimeD2 * (DataSrc.Accel3D.Y + DataSrc.AccelDeriv3D.Y * deltaTimeD3));
					Result->position.Z += deltaTime * (DataSrc.Speed.Z + deltaTimeD2 * (DataSrc.Accel3D.Z + DataSrc.AccelDeriv3D.Z * deltaTimeD3));
				}
			}
			else //if (in_pCalcParams->DensityDerivLevel == 2)
			{
				//V = V0 + a * t
				//S = S0 + V0 * t + (a * t^2)/2, >>>  S = S0 + t * (V0 + a * t /2),

				Result->Speed.X += DataSrc.Accel3D.X * deltaTime;
				Result->Speed.Y += DataSrc.Accel3D.Y * deltaTime;
				Result->Speed.Z += DataSrc.Accel3D.Z * deltaTime;

				Result->AbsSpeed = Result->Speed.LEN();

				if (Result->AbsSpeed >= in_pCalcParams->maxAbsSpeed)
				{
					ret |= SP_STATE_ABS_SPEED_AL;
					if (DataSrc.AbsSpeed >= in_pCalcParams->maxAbsSpeed)
					{
						Result->Speed.SET_LENGHT(in_pCalcParams->maxAbsSpeed);
						//грубовато
						Result->position.X += deltaTimeD2 * (DataSrc.Speed.X + Result->Speed.X);
						Result->position.Y += deltaTimeD2 * (DataSrc.Speed.Y + Result->Speed.Y);
						Result->position.Z += deltaTimeD2 * (DataSrc.Speed.Z + Result->Speed.Z);
					}
					else
					{
						//грубовато
						double dlt = deltaTimeD2;
						double dltStop = deltaTimeD2 * 0.001;
						double MaxSpeedTime = deltaTimeD2 * 0.5;
						SPos tmpPos;
						do
						{
							tmpPos = DataSrc.Accel3D;
							tmpPos.MULT(MaxSpeedTime);
							tmpPos.ADD(DataSrc.Speed);
							double lencmp = tmpPos.LENPWR2();
							dlt *= 0.5;

							if (lencmp > in_pCalcParams->maxAbsSpeedPwr2)			MaxSpeedTime -= dlt;
							else if (lencmp > in_pCalcParams->maxAbsSpeedPwr2)		MaxSpeedTime += dlt;
								else		dlt = 0.0;

						} while (dlt > dltStop);

						Result->position.X += MaxSpeedTime * (DataSrc.Speed.X + Result->Speed.X);
						Result->position.Y += MaxSpeedTime * (DataSrc.Speed.Y + Result->Speed.Y);
						Result->position.Z += MaxSpeedTime * (DataSrc.Speed.Z + Result->Speed.Z);

						Result->Speed.SET_LENGHT(in_pCalcParams->maxAbsSpeed);

						dlt = deltaTime - MaxSpeedTime - MaxSpeedTime;
						Result->position.X += dlt * Result->Speed.X;
						Result->position.Y += dlt * Result->Speed.Y;
						Result->position.Z += dlt * Result->Speed.Z;
					}
					Result->AbsSpeed = in_pCalcParams->maxAbsSpeed;
				}
				else
				{
					if (DataSrc.AbsSpeed < in_pCalcParams->maxAbsSpeed)
					{
						Result->position.X += deltaTime * (DataSrc.Speed.X + DataSrc.Accel3D.X * deltaTimeD2);
						Result->position.Y += deltaTime * (DataSrc.Speed.Y + DataSrc.Accel3D.Y * deltaTimeD2);
						Result->position.Z += deltaTime * (DataSrc.Speed.Z + DataSrc.Accel3D.Z * deltaTimeD2);
					}
					else
					{
						ret |= SP_STATE_ABS_CHANGE_LS;
						/// ???? Огрубление вверх
						Result->position.X += deltaTime * (DataSrc.Speed.X + DataSrc.Accel3D.X * deltaTimeD2);
						Result->position.Y += deltaTime * (DataSrc.Speed.Y + DataSrc.Accel3D.Y * deltaTimeD2);
						Result->position.Z += deltaTime * (DataSrc.Speed.Z + DataSrc.Accel3D.Z * deltaTimeD2);
					}
				}
			}
		}
		else *Result = DataSrc;
	}
	return ret;
}

int CSpParticle::CalcMidData(SData* Result, const SData& Ptr1, const SData& Ptr2, double absMidPoint)
{
	int ret = SP_STATE_ОК;

	if (Result)
	{
		*Result = { 0 };
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