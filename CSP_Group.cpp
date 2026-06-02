#include "pch.h"
#include "CDrawDlg.h"
#include "CSP_Group.h"
#include "CSpParticle.h"
#include "CommonFunc.h"

int CSP_Group::AssertionVar;

CSP_Group::CSP_Group()
{

	//STracePosInfo rrr = { 0 };
	//
	//for (int i = 0; i < 1000000; i++)
	//{
	//	rrr.state_Prev = rrr.state_Cur;
	//	rrr.state_Cur = i;
	//	rrr.TimePoint += 0.001;
	//	m_ParticleInfoArray->AddDataDynamic(rrr,0);

	//}
	
	//SPtrInfo ttt = SPArrayTemplate<SPtrInfo>::StaticTmp;


	for (int i = 0; i < GR_MAX_THREADS_COUNT; i++)
		Wait_Before[i] = Wait_After[i] = StateArray[i] = 0;

	m_innerState = M_INNER_STATE_STARTING;
	AfxBeginThread(DoCommandThread, (LPVOID)this);

	m_DelayMS = 0;
	m_CalcState = M_STATE_OFF;
	m_name = new char[SPG_TEMPSTRING_LEN +1000];
	sprintf_s(m_name, 1000, "GROUP_0");
	m_tmpString = m_name + 1000;
	*m_tmpString = 0;
	m_maxCount = 10;

	m_ThreadsMax_Initial = 0;
	m_ThreadsMax = 0;
	m_ThreadsCur = 0;
	m_Thr_Calc = new GR_Thread * [GR_MAX_THREADS_COUNT + 10];
	ZeroMemory(m_Thr_Calc, sizeof(GR_Thread*) * GR_MAX_THREADS_COUNT);

	m_pCalc = nullptr;
	m_pCalcFind = nullptr;

	m_CmdArr = new char* [M_CMD_MAXCOUNT + 100];
	ZeroMemory(m_CmdArr, sizeof(char*) * M_CMD_MAXCOUNT);

	InitializeCriticalSection(&m_cs);
	InitializeCriticalSection(&m_cs_Command);

	m_Array = new CSpParticle * [m_maxCount + 10];
	ZeroMemory(m_Array,  sizeof(CSpParticle *) * m_maxCount);

	m_ParticleInfoArray = nullptr;

	m_Relations.INIT(m_maxCount);

	PartCntr.INIT();

	m_CalcState = M_STATE_READY;

	m_innerState = M_INNER_STATE_READY;

	CalcCountDevider();
	CheckThreads();
}
CSP_Group::~CSP_Group()
{

	m_ThreadsMax = 0;
	if (m_Thr_Calc)
	{
		StopThreads();
		delete[] m_Thr_Calc; 
		m_Thr_Calc = nullptr;
	}
	m_CalcState |= M_STATE_BREAK;

	if (m_ParticleInfoArray) delete m_ParticleInfoArray;
	m_ParticleInfoArray = nullptr;

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


	if (m_Array)
	{
		int m_Count = PartCntr.GetCount();
		for (int i = 0; i < m_Count; i++)
			if (m_Array[i])
			{
				delete m_Array[i]; 
				m_Array[i] = nullptr; 
			}
		delete[] m_Array;
		PartCntr.InitCurrent();
	}

	delete[] m_name;
	m_Array = nullptr;


	if (m_CmdArr)
	{
		for (int i = 0; i < M_CMD_MAXCOUNT && m_CmdArr[i]; i++)
		{
			delete m_CmdArr[i];
			m_CmdArr[i] = nullptr;
		}
	}

	if (m_CmdArr) delete[] m_CmdArr;
	m_CmdArr = nullptr;

	LeaveCriticalSection(&m_cs_Command);
	Sleep(0);
	DeleteCriticalSection(&m_cs_Command);
	DeleteCriticalSection(&m_cs);
}



void CSP_Group::CheckThreads()
{
	if (m_Thr_Calc)
	{
		int m_Count = PartCntr.GetCount();

		if (m_Count < m_ThreadsMax) m_ThreadsMax = m_Count;

		if (m_ThreadsMax >= GR_MAX_THREADS_COUNT) m_ThreadsMax = GR_MAX_THREADS_COUNT;
		if (m_ThreadsMax < 2) m_ThreadsMax = 0;

		for (int i = m_ThreadsCur; i < m_ThreadsMax; i++)
			if (m_Thr_Calc[i] == nullptr)
				m_Thr_Calc[i] = new GR_Thread(this, i);

		m_ThreadsCur = m_ThreadsMax;
	}
}

void CSP_Group::StopThreads()
{
	if (m_Thr_Calc)
	{
		CSP_Group::InterruptionFlag = 1;
		for (int i = 0; i < GR_MAX_THREADS_COUNT; i++)
			if (m_Thr_Calc[i])
			{
				m_Thr_Calc[i]->StopFlag = 1;
				WakeByAddressAll((volatile PVOID)&Wait_After[i]);
				WakeByAddressAll((volatile PVOID)&Wait_Before[i]);
				std::atomic_thread_fence(std::memory_order_release);
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}

		for (int i = 0; i < GR_MAX_THREADS_COUNT; i++)
			if (m_Thr_Calc[i])
			{
				try
				{
					delete m_Thr_Calc[i];
					m_Thr_Calc[i] = nullptr;
				}
				catch (...) { ; }
			}
	}
}

void CSP_Group::Reset()
{
	EnterCriticalSection(&m_cs_Command);
	EnterCriticalSection(&m_cs);

	m_ParticlesState = GR_PS_NORM;

	if (m_Array)
	{
		int m_Count = PartCntr.GetCount();

		for (int i = 0; i < m_Count; i++)
			if (m_Array[i])
			{
				delete m_Array[i];
				m_Array[i] = nullptr;
			}
		delete[] m_Array;
		m_Array = nullptr;
	}
	PartCntr.INIT();

	if (m_CmdArr)
		for (int i = 0; i < M_CMD_MAXCOUNT && m_CmdArr[i]; i++)
		{
			delete m_CmdArr[i];
			m_CmdArr[i] = nullptr;
		}

	sprintf_s(m_name, 1000, "GROUP_0");

	m_maxCount = 10;
	m_Array = new CSpParticle * [m_maxCount + 10];
	ZeroMemory(m_Array, sizeof(CSpParticle*) * m_maxCount);

	m_Relations.INIT(m_maxCount);

	m_DelayMS = 0;

	if (m_pCalc)
	{
		m_pCalc->Time.RESET();
		m_pCalc->CalcCounter.InitLoop();
	}
	if (m_pCalcFind) m_pCalcFind->FindCounter.InitLoop();

	m_CalcState = M_STATE_READY;

	LeaveCriticalSection(&m_cs);
	Sleep(0);
	LeaveCriticalSection(&m_cs_Command);
	Sleep(0);
	CalcCountDevider();
	CheckThreads();
}

void CSP_Group::Clear()
{
	m_ParticlesState = GR_PS_NORM;
	int m_Count = PartCntr.GetCount();

	if (m_Array)
		for (int i = 0; i < m_Count; i++)
			if (m_Array[i])
				m_Array[i]->Clear(SP_CLEAR_SOFT);

	m_Relations.clear();

	if (m_pCalc) m_pCalc->Time.CLEAR();
}

void CSP_Group::CalcCountDevider()
{
	if (PartCntr.GetCount() < 2) m_CountDivider = 1.0;
	else m_CountDivider = 1.0 / (double(PartCntr.GetCount() - 1));
}


int CSP_Group::AddSpParticle(int in_Type, const SData& in_Ptr, SP_Calc* pCalc)
{
	int m_Count = PartCntr.GetCount();
	int ret = m_Count;
	
	if (m_Array && m_Count + 1 < SPG_MAX_COUNT)
	{
		if (m_Count + 1 > m_maxCount)
		{
			int newCount = m_maxCount + m_maxCount;
			if (newCount > SPG_MAX_COUNT) newCount = SPG_MAX_COUNT;

			CSpParticle ** tmp = new CSpParticle* [newCount + 10];
			CopyMemory(tmp, m_Array, m_Count * sizeof(CSpParticle*));
			delete[] m_Array;
			m_Array = tmp;

			m_Relations.SET_COUNT(newCount);

			m_maxCount = newCount;
		}

		if ((m_Array[m_Count] = new CSpParticle(*m_pCalc, in_Type)))
		{
			PartCntr.AddCurrent(in_Type);
			m_Array[m_Count]->m_pCalc = pCalc;
			m_Array[m_Count]->SetParticleData(SP_DATA_ALL, in_Ptr);
		}
		CalcCountDevider();
	}
	return PartCntr.GetCount() - ret;
}

int CSP_Group::GetMaxTraceId()
{
	int ret = 0;
	int m_Count = PartCntr.GetCount();
	if (m_Array && m_Count)
	{
		for(int i = 0; i < m_Count; i++)
		{
				int tmp = m_Array[i]->GetCurPos();
				if (tmp > ret) ret = tmp;
		}
	}
	return ret;
}


int CSP_Group::RemoveSpParticle(int in_Idx)
{
	int m_Count = PartCntr.GetCount();

	if (m_Array && in_Idx >= 0 && in_Idx < m_Count)
	{
		int ParticleType = 0;
		if (m_Array[in_Idx])
		{
			ParticleType = m_Array[in_Idx]->m_TypeFlags;
			delete m_Array[in_Idx];
		}

		m_Array[in_Idx] = nullptr;

		int MoveCount = m_Count - in_Idx - 1;

		if (MoveCount > 0)
		{
			CopyMemory(m_Array + in_Idx, m_Array + in_Idx + 1, MoveCount * sizeof(CSpParticle*));
			m_Relations.REMOVE_ITEM(in_Idx);
		}

		PartCntr.AddCurrent(ParticleType, -1);

		CheckThreads();
		pBS_Corrected = m_Relations.GetBeStuck(GR_PS_RELATION_CORRECTED);
		CalcCountDevider();
	}

	return GetCount();
}

CSpParticle* CSP_Group::GetItem(int in_Idx)
{
	CSpParticle* ret = nullptr;

	if (m_Array && in_Idx >= 0 && in_Idx < GetCount())
		ret = m_Array[in_Idx];

	return ret;
}

int CSP_Group::SetParticleData(int in_Idx, int in_flags, const SData& in_Ptr)
{
	int ret = 0;
	if (m_Array && in_Idx >= 0 && in_Idx < GetCount())
		ret = m_Array[in_Idx]->SetParticleData(in_flags,  in_Ptr);

	return ret;
}

double CSP_Group::GetCurTime()
{
	return 	m_pCalc->Time.GetCurTime();
}
/*
int CSP_Group::GetMinMaxData(SData* out_Min, SData* out_Max, SData_SD* out_MinSD, SData_SD* out_MaxSD, int in_NextFlag)
{
	int ret = SP_STATE_ERROR_MAIN;
	SData* pData;

	if (out_Min && out_Max && out_MinSD && out_MaxSD && m_Count && m_Array[0] && (pData = in_NextFlag == 0 ? m_Array[0]->GetCurData() : &m_Array[0]->m_NextData) && pData)
	{
		double* pDoubleMin, * pDoubleMax;
		*out_Min = *pData;
		*out_Max = *pData;
		// с 1 потому что проинициализировано выше значением idx = 0
		for (int i = 1; i < m_Count; i++)
		{
			if ((pData = in_NextFlag == 0 ? m_Array[i]->GetCurData() : &m_Array[i]->m_NextData))
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
		pDoubleMax++; pDoubleMin++; pDoubleMaxSD++; pDoubleMinSD++;
		if (*pDoubleMax > *pDoubleMaxSD)  *pDoubleMaxSD = *pDoubleMax;
		if (*pDoubleMin < *pDoubleMinSD)  *pDoubleMinSD = *pDoubleMin;
	}

	return ret;
}
*/

void CSP_Group::CollectStat(SP_CalcStat& out_Stat, int in_StatType)
{
	if (m_Array)
	{
		int m_Count = PartCntr.GetCount();

		if ((in_StatType & STAT_TYPE_ALL_DATA))
		{
			if ((in_StatType & STAT_TYPE_MAX_RELATION))
			{
				out_Stat.v_Max.Velocity.Z = out_Stat.v_Max.Velocity.Y = out_Stat.v_Max.Velocity.X = -SP_MAX_TIME_DISTANCE;
				out_Stat.v_Max.Position.Z = out_Stat.v_Max.Position.Y = out_Stat.v_Max.Position.X = -SP_MAX_TIME_DISTANCE;
				out_Stat.v_Max.Accel_2_3D.Z = out_Stat.v_Max.Accel_2_3D.Y = out_Stat.v_Max.Accel_2_3D.X = -SP_MAX_TIME_DISTANCE;
				out_Stat.v_Max.Jerk_3_3D.Z = out_Stat.v_Max.Jerk_3_3D.Y = out_Stat.v_Max.Jerk_3_3D.X = -SP_MAX_TIME_DISTANCE;
				out_Stat.v_Max.AbsSpeed = 0.0;
				out_Stat.v_Max.Density = 0.0;
				out_Stat.v_Max.TimePoint = -SP_MAX_TIME_DISTANCE;
			}

			if ((in_StatType & STAT_TYPE_MIN_RELATION))
			{
				out_Stat.v_Min.Velocity.Z = out_Stat.v_Min.Velocity.Y = out_Stat.v_Min.Velocity.X = SP_MAX_TIME_DISTANCE;
				out_Stat.v_Min.Position.Z = out_Stat.v_Min.Position.Y = out_Stat.v_Min.Position.X = SP_MAX_TIME_DISTANCE;
				out_Stat.v_Min.Accel_2_3D.Z = out_Stat.v_Min.Accel_2_3D.Y = out_Stat.v_Min.Accel_2_3D.X = SP_MAX_TIME_DISTANCE;
				out_Stat.v_Min.Jerk_3_3D.Z = out_Stat.v_Min.Jerk_3_3D.Y = out_Stat.v_Min.Jerk_3_3D.X = SP_MAX_TIME_DISTANCE;
				out_Stat.v_Min.AbsSpeed = 1.0;
				out_Stat.v_Min.Density = 1.0;
				out_Stat.v_Min.TimePoint = SP_MAX_TIME_DISTANCE;
			}

			for (int i = 0; i < m_Count; i++)
			{
				const SData* pData;
				switch ((in_StatType & STAT_TYPE_TIMEMASK))
				{
				case STAT_TYPE_CURRENT:
					pData = m_Array[i]->GetCurData();	break;
				case STAT_TYPE_PREV:
					pData = m_Array[i]->GetPrevData_C();	break;
				case STAT_TYPE_NEXT:
					pData = &m_Array[i]->m_NextData;	break;
				case STAT_TYPE_TEMP:
				default:
					pData = m_Array[i]->GetCurData();	break;
				}

				double* pDoubleMin = (double*)&out_Stat.v_Min;
				double* pDoubleMax = (double*)&out_Stat.v_Max;
				double* pCmp = (double*)pData;

				for (int j = 0; j < 13; j++, pDoubleMax++, pDoubleMin++, pCmp++)
				{
					double value = fabs(*pCmp);
					if ((in_StatType & STAT_TYPE_MAX_RELATION) && value > *pDoubleMax)  *pDoubleMax = value;
					if ((in_StatType & STAT_TYPE_MIN_RELATION) && value < *pDoubleMin)  *pDoubleMin = value;
				}
			}

			if ((in_StatType & STAT_TYPE_MIN_RELATION))
			{
				double* pDoubleMinSD;
				pDoubleMinSD = (double*)&out_Stat.v_MinSD;

				for (int i = 0; i < 6; i++)
					*(pDoubleMinSD++) = 1.0e30;

				double* pDoubleMin = (double*)&out_Stat.v_Min;
				double* pDoubleMax = (double*)&out_Stat.v_Max;

				pDoubleMinSD = (double*)&out_Stat.v_MinSD;

				for (int j = 0; j < 4; j++, pDoubleMinSD++)
					for (int k = 0; k < 3; k++, pDoubleMax++, pDoubleMin++)
						if (*pDoubleMin < *pDoubleMinSD)  *pDoubleMinSD = *pDoubleMin;

				if (*pDoubleMin < *pDoubleMinSD)  *pDoubleMinSD = *pDoubleMin;
				pDoubleMax++; pDoubleMin++; pDoubleMinSD++;
				if (*pDoubleMin < *pDoubleMinSD)  *pDoubleMinSD = *pDoubleMin;
			}

			if ((in_StatType & STAT_TYPE_MAX_RELATION))
			{
				double* pDoubleMaxSD;
				pDoubleMaxSD = (double*)&out_Stat.v_MaxSD;

				for (int i = 0; i < 6; i++)
					*(pDoubleMaxSD++) = -1.0e30;

				double* pDoubleMin = (double*)&out_Stat.v_Min;
				double* pDoubleMax = (double*)&out_Stat.v_Max;

				pDoubleMaxSD = (double*)&out_Stat.v_MaxSD;

				for (int j = 0; j < 4; j++, pDoubleMaxSD++)
					for (int k = 0; k < 3; k++, pDoubleMax++, pDoubleMin++)
						if (*pDoubleMax > *pDoubleMaxSD)  *pDoubleMaxSD = *pDoubleMax;

				if (*pDoubleMax > *pDoubleMaxSD)  *pDoubleMaxSD = *pDoubleMax;
				pDoubleMax++; pDoubleMin++; pDoubleMaxSD++;
				if (*pDoubleMax > *pDoubleMaxSD)  *pDoubleMaxSD = *pDoubleMax;
			}
		}

		if ((in_StatType & STAT_TYPE_ALL_RELATION))
		{
			SPRelation* calcRelations;
			switch ((in_StatType & STAT_TYPE_TIMEMASK))
			{
			case STAT_TYPE_CURRENT:
				calcRelations = m_Relations.GetCurent();	break;
			case STAT_TYPE_PREV:
				calcRelations = m_Relations.GetPrev();	break;
			case STAT_TYPE_NEXT:
				calcRelations = m_Relations.GetNext();	break;
			case STAT_TYPE_TEMP:
				calcRelations = m_Relations.GetTemp();	break;
			default:
				calcRelations = m_Relations.GetCurent();	break;
			}

			if ((in_StatType & STAT_TYPE_MIN_RELATION))
			{
				ZeroMemory(&out_Stat.Rel_Min, sizeof(SMinMaxRelation));
				out_Stat.Rel_Min.Real_Distance.MMValue = 1.0e30;
				out_Stat.Rel_Min.Phantom_Distance.MMValue = 1.0e30;
				out_Stat.Rel_Min.Velocity.MMValue = 1.0e30;
			}
			if ((in_StatType & STAT_TYPE_MAX_RELATION))
			{
				ZeroMemory(&out_Stat.Rel_Max, sizeof(SMinMaxRelation));
				out_Stat.Rel_Max.Real_Distance.MMValue = -1.0e30;
				out_Stat.Rel_Max.Phantom_Distance.MMValue = -1.0e30;
				out_Stat.Rel_Max.Velocity.MMValue = -1.0e30;
			}

			if ((in_StatType & STAT_TYPE_SETPARTICLE))
				for (int idx1 = 0; idx1 < m_Count; idx1++)
					m_Array[idx1]->m_CountBeStuck = 0;

			for (int idx1 = 0; idx1 < m_Count; idx1++)
			{
				SPRelation* curRel = calcRelations + idx1;
				for (int idx2 = 0; idx2 < m_Count; idx2++, curRel += m_maxCount)
					if (idx1 != idx2)
					{
						if ((curRel->State & GR_PS_COLLISION_BESTUCK) && (in_StatType & STAT_TYPE_SETPARTICLE))
								m_Array[idx1]->m_CountBeStuck++;

						if ((in_StatType & STAT_TYPE_MIN_RELATION))
						{
							if (out_Stat.Rel_Min.Velocity.MMValue > curRel->RelativeSpeedAbs)
							{
								out_Stat.Rel_Min.Velocity.MMValue = curRel->RelativeSpeedAbs;
								out_Stat.Rel_Min.Velocity.idxParticle1 = idx1;
								out_Stat.Rel_Min.Velocity.idxParticle2 = idx2;
							}
							if (out_Stat.Rel_Min.Phantom_Distance.MMValue > curRel->DistancePwr1)
							{
								out_Stat.Rel_Min.Phantom_Distance.MMValue = curRel->DistancePwr1;
								out_Stat.Rel_Min.Phantom_Distance.idxParticle1 = idx1;
								out_Stat.Rel_Min.Phantom_Distance.idxParticle2 = idx2;
							}

							if (out_Stat.Rel_Min.Real_Distance.MMValue > curRel->RealDistancePwr1)
							{
								out_Stat.Rel_Min.Real_Distance.MMValue = curRel->RealDistancePwr1;
								out_Stat.Rel_Min.Real_Distance.idxParticle1 = idx1;
								out_Stat.Rel_Min.Real_Distance.idxParticle2 = idx2;
							}

						}
						if ((in_StatType & STAT_TYPE_MAX_RELATION))
						{
							if (out_Stat.Rel_Max.Velocity.MMValue < curRel->RelativeSpeedAbs)
							{
								out_Stat.Rel_Max.Velocity.MMValue = curRel->RelativeSpeedAbs;
								out_Stat.Rel_Max.Velocity.idxParticle1 = idx1;
								out_Stat.Rel_Max.Velocity.idxParticle2 = idx2;
							}
							if (out_Stat.Rel_Max.Phantom_Distance.MMValue < curRel->DistancePwr1)
							{
								out_Stat.Rel_Max.Phantom_Distance.MMValue = curRel->DistancePwr1;
								out_Stat.Rel_Max.Phantom_Distance.idxParticle1 = idx1;
								out_Stat.Rel_Max.Phantom_Distance.idxParticle2 = idx2;
							}

							if (out_Stat.Rel_Max.Real_Distance.MMValue < curRel->RealDistancePwr1)
							{
								out_Stat.Rel_Max.Real_Distance.MMValue = curRel->RealDistancePwr1;
								out_Stat.Rel_Max.Real_Distance.idxParticle1 = idx1;
								out_Stat.Rel_Max.Real_Distance.idxParticle2 = idx2;
							}
						}
					}
			}

			if ((in_StatType & STAT_TYPE_ALL_RELATION) == STAT_TYPE_CURRENT)
			{
				SData* tmpPtr;
				SData tmpVal = { 0 };

				if ((in_StatType & STAT_TYPE_MIN_RELATION))
				{
					tmpPtr = m_Array[out_Stat.Rel_Min.Velocity.idxParticle1]->GetCurData();
					out_Stat.Rel_Min.Velocity.Part1 = tmpPtr ? *tmpPtr : tmpVal;
					tmpPtr = m_Array[out_Stat.Rel_Min.Velocity.idxParticle2]->GetCurData();
					out_Stat.Rel_Min.Velocity.Part2 = tmpPtr ? *tmpPtr : tmpVal;
					out_Stat.Rel_Min.Velocity.relation = calcRelations[out_Stat.Rel_Min.Velocity.idxParticle1 + m_maxCount * out_Stat.Rel_Min.Velocity.idxParticle2];

					tmpPtr = m_Array[out_Stat.Rel_Min.Phantom_Distance.idxParticle1]->GetCurData();
					out_Stat.Rel_Min.Phantom_Distance.Part1 = tmpPtr ? *tmpPtr : tmpVal;
					tmpPtr = m_Array[out_Stat.Rel_Min.Phantom_Distance.idxParticle2]->GetCurData();
					out_Stat.Rel_Min.Phantom_Distance.Part2 = tmpPtr ? *tmpPtr : tmpVal;
					out_Stat.Rel_Min.Phantom_Distance.relation = calcRelations[out_Stat.Rel_Min.Phantom_Distance.idxParticle1 + m_maxCount * out_Stat.Rel_Min.Phantom_Distance.idxParticle2];
				}

				if ((in_StatType & STAT_TYPE_MAX_RELATION))
				{
					tmpPtr = m_Array[out_Stat.Rel_Max.Velocity.idxParticle1]->GetCurData();
					out_Stat.Rel_Max.Velocity.Part1 = tmpPtr ? *tmpPtr : tmpVal;
					tmpPtr = m_Array[out_Stat.Rel_Max.Velocity.idxParticle2]->GetCurData();
					out_Stat.Rel_Max.Velocity.Part2 = tmpPtr ? *tmpPtr : tmpVal;
					out_Stat.Rel_Max.Velocity.relation = calcRelations[out_Stat.Rel_Max.Velocity.idxParticle1 + m_maxCount * out_Stat.Rel_Max.Velocity.idxParticle2];

					tmpPtr = m_Array[out_Stat.Rel_Max.Phantom_Distance.idxParticle1]->GetCurData();
					out_Stat.Rel_Max.Phantom_Distance.Part1 = tmpPtr ? *tmpPtr : tmpVal;
					tmpPtr = m_Array[out_Stat.Rel_Max.Phantom_Distance.idxParticle2]->GetCurData();
					out_Stat.Rel_Max.Phantom_Distance.Part2 = tmpPtr ? *tmpPtr : tmpVal;
					out_Stat.Rel_Max.Phantom_Distance.relation = calcRelations[out_Stat.Rel_Max.Phantom_Distance.idxParticle1 + m_maxCount * out_Stat.Rel_Max.Phantom_Distance.idxParticle2];
				}
			}
		}
	}
}

void CSP_Group::CollectStat(int in_StatType)
{
	if (m_pCalc)	CollectStat(m_pCalc->Stat, in_StatType);
}

void CSP_Group::WriteState(FILE* in_Ptr, double in_StateTime)
{
	if (in_Ptr)
	{
		int m_Count = PartCntr.GetCount();

		if (m_Array && m_Count > 0)
		{
			fprintf(in_Ptr, "\nGroup %s\tCount=\t%d\tTime=\t%g\n", m_name, m_Count, m_pCalc->Time.GetCurTime());

			for (int i = 0; i < m_Count; i++)
			{
				m_Array[i]->GetStateDescription(m_tmpString, SPG_TEMPSTRING_LEN, in_StateTime);
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
		int m_Count = PartCntr.GetCount();

		if (m_Array  && m_pCalc && m_pCalcFind && m_Count > 0)
		{
			fprintf(in_Ptr, "\nGroup %s\tCount=\t%d\tTime=\t%10.10g\tsRandSeed=\t%d\tsStat.Count=\t%d\n", m_name, m_Count, m_pCalc->Time.GetCurTime(), m_pCalcFind->sRandSeed, m_pCalcFind->stat.Count);
			switch (OutputType)
			{
			case 0:
				for (int i = 0; i < m_Count; i++)
				{
					m_Array[i]->GetStateDescription(m_tmpString, SPG_TEMPSTRING_LEN, in_nPoints);
					fprintf(in_Ptr, "\tSParticle\t%d\t%s\n", i, m_tmpString);
				}
				break;
			case 1:
				for (int i = 0; i < m_Count; i++)
				{
					m_Array[i]->GetStateDescription(m_tmpString, SPG_TEMPSTRING_LEN, 1);
					fprintf(in_Ptr, "\tSParticle\t%d\t%s\n", i, m_tmpString);

					for (int j = 0; j < m_Count; j++)
						if (i != j)
						{
							SPRelation* tmpRel = m_Relations.GetCurentRelation( i, j );

							char* out_Str = m_tmpString;
							out_Str += sprintf_s(out_Str, 1000, "\tTime=\t%10.10g\t", tmpRel->ViewPointData.TimePoint);
							out_Str += sprintf_s(out_Str, 1000, "\tVP_Point=\t%10.10g\t%10.10g\t%g", tmpRel->ViewPointData.Position.X, tmpRel->ViewPointData.Position.Y, tmpRel->ViewPointData.Position.Z);
							out_Str += sprintf_s(out_Str, 1000, "\tVP_Speed=\t%10.10g\t%10.10g\t%10.10g", tmpRel->ViewPointData.Velocity.X, tmpRel->ViewPointData.Velocity.Y, tmpRel->ViewPointData.Velocity.Z);
							sprintf_s(out_Str, 1000, "\tSpeedRel=\t%10.10g\tDistRel=\t%10.10g", tmpRel->RelativeSpeed, tmpRel->DistancePwr1);
							fprintf(in_Ptr, "\tRelation\t%d\t%s\n", j, m_tmpString);
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
	if (in_Percents > 100.0) in_Percents = 100.0;
	if (in_Percents < 0.0) in_Percents = 100.0;
	if (in_Spread > 100.0) in_Spread = 100.0;
	if (in_Spread < 0.000001) in_Spread = 0.000001;
	

	if (m_pCalc && m_ParticleInfoArray)
	{
		STracePosInfo* PInfoStart = m_ParticleInfoArray->GetDataById(0);
		STracePosInfo* PInfoEnd = m_ParticleInfoArray->GetCurData();
		m_pCalc->DrawSet.ViewPoint.TimeInPc = -1.0;
		m_pCalc->DrawSet.ViewPoint.Spread = 100.0;

		if (PInfoEnd && PInfoStart && (GetCalcState()& (M_STATE_READY| M_STATE_CALC | M_STATE_PAUSE | M_STATE_BREAK)))
		{
			double vpSpread = (PInfoEnd->TimePoint - PInfoStart->TimePoint) * 0.01; // == 1%
			double vpTime = PInfoStart->TimePoint + vpSpread * in_Percents; //  vpTime +- vpSpread
			vpSpread *= in_Spread;

			int id_Start, id_End;
			// , id_Mid;
			//id_Mid = m_ParticleInfoArray->GetLowIdxByTime(vpTime);

			if (vpTime + vpSpread > PInfoEnd->TimePoint) id_End = SP_DRAW_MAX_ID;
			else	id_End = m_ParticleInfoArray->GetLowIdxByTime(vpTime + vpSpread);

			if (vpTime - vpSpread <= PInfoStart->TimePoint) id_Start = 0;
			else	id_Start = m_ParticleInfoArray->GetLowIdxByTime(vpTime - vpSpread);

			SetViewPointRange(id_Start, id_End);
		}
		else 
			SetViewPointRange(0, SP_DRAW_MAX_ID);
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
	int m_Count = PartCntr.GetCount();

	for (int i = 0; i < m_Count; i++) 
		m_Array[i]->GetMinMaxTracePos(min_Pos, max_Pos, idx_Start, idx_End);
}

int CSP_Group::GetIdByMaxDistance(double in_RelDist)
{
	int ret = -1;
	int m_Count = PartCntr.GetCount();

	if (m_Array && m_Count > 2 && m_Array[0] && m_Array[1])
	{
		for (int i = 0; i < m_Count && ret < 0; i++)
		{
			int j;
			for (j = 0; j < m_Count && (i == j || m_Relations.GetCurentRelation( i, j)->DistancePwr1 > in_RelDist); j++);

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
		if (m_Array)
		{
			int m_Count = PartCntr.GetCount();
			SData tmpData = {0};
			for (int i = 0; i < m_Count; i++)
			{
				int IdIten = in_IdPos;
				if (IdIten < 0) IdIten = m_Array[i]->GetCurPos();

				if (IdIten >= 0 && m_Array[i]->GetDataById(in_IdPos, &tmpData) >= 0)
					out_Info->ADD(&tmpData, in_IdPos);
			}
			if (out_Info->Count > 0)
				out_Info->CALC();
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


double CSP_Group::CalcStepUpLimit()
{
	double ret = 1.0;
	int i;
	//поиск точки перехода с предельной скорости на допредельную
	int m_Count = PartCntr.GetCount();
	if (m_pCalc->Time.GetCurTime() > 0.0)
		for (i = 0; i < m_Count; i++)
		{
			const SData *cpCurdata = m_Array[i]->GetCurData_C();
			const SData* cpPrevData = m_Array[i]->GetPrevData_C();

			if (cpCurdata->AbsSpeed >= m_pCalc->maxAbsSpeed)
			{
				SPos DeltaAcc;
				DeltaAcc.SET_AS_DELTA(cpCurdata->Accel_2_3D, cpPrevData->Accel_2_3D); //m_Array[i]->GetLastDeltaAcc(DeltaAcc); 
				SPos SpeedTmp = cpCurdata->Velocity; SpeedTmp.NORMALIZE();

				double deltaAccOnV = SpeedTmp.DOT(DeltaAcc); 
				if (deltaAccOnV < 0)
				{
					double deltaT = deltaAccOnV / (cpCurdata->Velocity.LENPWR2() * deltaAccOnV * (cpCurdata->TimePoint- cpCurdata->TimePoint));
					if (deltaT < ret) ret = deltaT;
				}
			}
		}

	return ret;
}

int CSP_Group::CheckTimes()
{
	int ret = 0;
	int m_Count = PartCntr.GetCount();


	for (int i = 1; i < m_Count; i++)
	{
		if (m_Array[i]->GetCurTime() != m_Array[0]->GetCurTime())
			ASSERT(0);
	}

	for (int i = 1; i < m_Count; i++)
	{
		if (m_Array[i]->GetNextData().TimePoint != m_Array[0]->GetNextData().TimePoint)
			ASSERT(0);
	}

	return ret;
}

int CSP_Group::CorrectBeStuckParticles(int in_Flags, double& KeyPoint)
{
	int ret = GR_PS_NORM;

	const BeStuckParticles* pBSP = (in_Flags& GR_PS_RELATION_NEEDRECALC) ? m_Relations.FillNBeStuck(in_Flags, 0, GetCount()) : m_Relations.GetBeStuck(in_Flags);

	if (pBS_Corrected->GetCount())	pBS_Corrected->clear();

	if (pBSP)
	{

		for (int StuckNum = 0; StuckNum < pBSP->GetCount(); StuckNum++)
		{
			const BeStuckList* BSList = pBSP->GetBeStuckListByNum(StuckNum);
			if (BSList)
			{

				switch (BSList->nMasters)//коррекция позиции по связке
				{
				case 0:
					break;
				case 1:
				{
					int VP_ID = BSList->List[0].IdMaster;
					if (VP_ID >= 0)
					{
						double PrecDist = m_pCalc->Time.GetPrecDistance();
						SPRelation* NextRelation = m_Relations.GetRelation(GR_PS_RELATION_NEXT, BSList->IdParticle, VP_ID);
						SPRelation* curRelation = m_Relations.GetRelation( GR_PS_RELATION_CURRENT, BSList->IdParticle, VP_ID);
						double CriticalDelta = 0.0, DistPwr1 = 0.0;
						SData VP, * pm_NextData = &m_Array[BSList->IdParticle]->m_NextData;
						double NewViewPointTime = curRelation->ViewPointData.TimePoint + m_pCalc->Time.GetStep();

						m_Array[VP_ID]->GetPrevDataByTimeLinear(VP, NextRelation->LowIdxOfViewPont_Near, curRelation->LowIdxOfViewPont_Near, NewViewPointTime);


						DistPwr1 = VP.Position.GetDistance(pm_NextData->Position); ///!!!!!!!!!!Убрать лишние операции и итерацию, есл
						CriticalDelta = fabs(DistPwr1 - m_pCalc->CriticalDistance);
						//if (DistPwr1 < m_pCalc->CriticalDistance)
						ASSERT(DistPwr1 < 1000.0 && DistPwr1 > 0.50);
						if (CriticalDelta > 5.0e-14)
						{
							//пока простое выправниевание
							if (CriticalDelta > PrecDist)
							{
								ret |= GR_PS_POSITION_CHANGED;
								pm_NextData->Position.MINUS(VP.Position);
								pm_NextData->Position.SET_LENGHT(m_pCalc->CriticalDistance);
								pm_NextData->Position.ADD(VP.Position);
								pBS_Corrected->AddItem(BSList->IdParticle, VP_ID, GR_PS_POSITION_CHANGED);
							}
							else
								ret |= GR_PS_POSITION_LOW_PREC;
						}
						if (CriticalDelta > 0.0)
							ret |= GR_PS_POSITION_LOW_PREC;

						SPos normal = pm_NextData->Position;
						normal.MINUS(VP.Position);
						//!!!!normal.SET_LENGHT(m_pCalc->CriticalDistance); //Уже !!!!!!!

						//корректировка скорости
						//!!!!скорость равна проекции VP на нормаль (прямая [ViewPoint,Particle])
						pm_NextData->Velocity = normal;
						pm_NextData->Velocity.SET_AS_PROJECTION_OF(VP.Velocity);

						if (in_Flags & GR_PS_RELATION_NEXT)
						{
							NextRelation->DistancePwr1 = m_pCalc->CriticalDistance;
							NextRelation->DistancePwr2 = m_pCalc->CriticalDistancePwr2;
							NextRelation->DistancePwr3 = m_pCalc->CriticalDistancePwr3;
							NextRelation->DensityAtBasePoint = 0.0;
							NextRelation->RelativeSpeedAbs = 0.0;
							NextRelation->RelativeVelocity_3D.SET_AS_DELTA(pm_NextData->Velocity, VP.Velocity);
						}

						//!!!!корректировка ускорения на уровне расчета данных по относительным данным
					}
					else
					{
						ASSERT(0);
					}
				}
				break;
				case 2:
				{
					/*			int RelationPos_0 = ID_CollissionRel[0].RelationPos;
								int RelationPos_1 = ID_CollissionRel[1].RelationPos;
								ASSERT(0);
								SPos PointVPMiddle;
								PointVPMiddle.SET_AS_MIDDLE_POINT(in_CalcRelations[RelationPos_0].ViewPointData.Position, in_CalcRelations[RelationPos_1].ViewPointData.Position);

								SPos NormalVP = in_CalcRelations[RelationPos_0].ViewPointData.Position;
								NormalVP.MINUS(in_CalcRelations[RelationPos_1].ViewPointData.Position);

								double DistanceVP_PWR2 = NormalVP.LENPWR2();
								double DistParticleToMVP = sqrt(m_pCalc->CriticalDistancePwr2 - DistanceVP_PWR2 * 0.25);

								SPos PartPprojToPlane;
								PartPprojToPlane.SET_AS_PROJECTION_ON_PLANE(pCalcData->Position, PointVPMiddle, in_CalcRelations[RelationPos_0].ViewPointData.Position);///!!!!!!!!!!!!!!!!!!   NormalVP
								PartPprojToPlane.MINUS(PointVPMiddle);
								PartPprojToPlane.SET_LENGHT(DistParticleToMVP);
								PartPprojToPlane.ADD(PointVPMiddle);

								//корректировка позиции
								////////pCalcData->Position = PartPprojToPlane;

								//корректировка скорости
								//скорость равна проекции VP на нормаль (прямая [ViewPoint,Particle])
								SPos Norm0 = in_CalcRelations[RelationPos_0].ViewPointData.Position;
								Norm0.MINUS(pCalcData->Position);
								SPos Norm1 = in_CalcRelations[RelationPos_1].ViewPointData.Position;
								Norm1.MINUS(pCalcData->Position);

								/////SPos CrossLine = Norm0;
								/////CrossLine.CROSS(Norm1);
								//////CrossLine.MULT((pCalcData->Velocity.DOT(CrossLine) / CrossLine.LENPWR2()));
								/////pCalcData->Velocity = CrossLine;

								////////SPos tmpSpeed = Norm0;
								/////tmpSpeed.SET_AS_PROJECTION_OF(in_CalcRelations[RelationPos_1].ViewPointData.Velocity);
								/////pCalcData->Velocity.ADD(tmpSpeed);

								//////tmpSpeed = Norm1;
								///////tmpSpeed.SET_AS_PROJECTION_OF(in_CalcRelations[RelationPos_0].ViewPointData.Velocity);
								///////pCalcData->Velocity.ADD(tmpSpeed);



								//корректировка ускорения
								//ускорение равно проекции ускорения VP на нормаль (прямая [ViewPoint,Particle]) + проекция остальных на плоскость

								SPos CrossLine = Norm0;
								CrossLine.CROSS(Norm1);
								CrossLine.MULT((pCalcData->Accel_2_3D.DOT(CrossLine) / CrossLine.LENPWR2()));
								pCalcData->Accel_2_3D = CrossLine;

								SPos tmpAccel3D = Norm0;
								tmpAccel3D.SET_AS_PROJECTION_OF(in_CalcRelations[RelationPos_1].ViewPointData.Accel_2_3D);
								pCalcData->Accel_2_3D.ADD(tmpAccel3D);

								tmpAccel3D = Norm1;
								tmpAccel3D.SET_AS_PROJECTION_OF(in_CalcRelations[RelationPos_0].ViewPointData.Accel_2_3D);
								pCalcData->Accel_2_3D.ADD(tmpAccel3D);

								if (m_pCalc->DensityDerivLevel >= 3)
								{
									CrossLine = Norm0;
									CrossLine.CROSS(Norm1);
									CrossLine.MULT((pCalcData->Jerk_3_3D.DOT(CrossLine) / CrossLine.LENPWR2()));
									pCalcData->Jerk_3_3D = CrossLine;

									SPos tmpAccelDeriv3D = Norm0;
									tmpAccelDeriv3D.SET_AS_PROJECTION_OF(in_CalcRelations[RelationPos_1].ViewPointData.Jerk_3_3D);
									pCalcData->Jerk_3_3D.ADD(tmpAccelDeriv3D);

									tmpAccelDeriv3D = Norm1;
									tmpAccelDeriv3D.SET_AS_PROJECTION_OF(in_CalcRelations[RelationPos_0].ViewPointData.Jerk_3_3D);
									pCalcData->Jerk_3_3D.ADD(tmpAccelDeriv3D);
								}
					*/
				}
				ASSERT(0);
				break;
				default:
					ASSERT(0);
					break;
				}
			}
		}
	}
	return ret;
}

int CSP_Group::CalcBeStuckNextPos(SData& out_Data, int in_IdParticle, double in_NextTime, double* const out_KeyPoint)
{
	int ret = SP_STATE_ERROR_MAIN;

	const BeStuckParticles* pBSP;
	const BeStuckList* pBSList;
	int m_Count = PartCntr.GetCount();
	
	if (in_IdParticle < m_Count && (pBSP = m_Relations.GetBeStuck_Current()) && (pBSList = pBSP->GetBeStuckListById(in_IdParticle)))
	{
		const SData *pCurData = m_Array[in_IdParticle]->GetCurData_C();
		double DeltaT = in_NextTime - pCurData->TimePoint;
		double tmp_KP = in_NextTime;
		double* const out_KeyPointTmp = out_KeyPoint ? out_KeyPoint : &tmp_KP;

		switch (pBSList->nMasters)
		{
		case 1:
		{
			int IdMaster = pBSList->List[0].IdMaster;
			SPRelation* curRelation = m_Relations.GetCurentRelation(in_IdParticle, IdMaster);
			SPRelation* nextRelation = m_Relations.GetNextRelation(in_IdParticle, IdMaster);
			RelationAndBeStuck* SelfRel = (RelationAndBeStuck*)curRelation + in_IdParticle + in_IdParticle * m_Count;

			// Новое положение центра сферы фантома

			m_Array[IdMaster]->GetPrevDataByTimeLinear(nextRelation->ViewPointData, nextRelation->LowIdxOfViewPont_Near, curRelation->LowIdxOfViewPont_Near, in_NextTime - m_pCalc->maxAbsSpeed);
//			m_Array[IdMaster]->GetPrevDataByTimeFast(nextRelation->ViewPointData, nextRelation->LowIdxOfViewPont_Near, curRelation->LowIdxOfViewPont_Near, in_NextTime - m_pCalc->maxAbsSpeed, out_KeyPoint);
//			m_Array[IdMaster]->GetMovement(nextRelation->ViewPointData, curRelation->ViewPointData, curRelation->ViewPointData.TimePoint + DeltaT, out_KeyPoint);
//			nextRelation->LowIdxOfViewPont_Near = m_Array[IdMaster]->GetLowIdxByTime(nextRelation->ViewPointData.TimePoint, curRelation->LowIdxOfViewPont_Near);

			SData ByNormal = { 0 };
			ByNormal.Accel_2_3D = SelfRel->AsSelf.ByNormalData.Accel_2_3D;
			ASSERT(fabs(ByNormal.Accel_2_3D.X) < SP_MAX_TIME_DISTANCE);

			ByNormal.TimePoint = pCurData->TimePoint;
			/////Accel3D_DERIV !!!!!!!

			// Изменение положения точки только за счет ускорения от не мастера (в виде проекции на плоскость нормали)
			ret = CSpParticle::ItgrlFull(out_Data, ByNormal, in_NextTime, *m_pCalc, out_KeyPoint);
			out_Data.Position.ADD(pCurData->Position);

			//новое положение точки относительно центра сферы
			out_Data.Position.SET_AS_NORMAL(nextRelation->ViewPointData.Position, m_pCalc->CriticalDistance);

			//новое абсолютное положение точки
			out_Data.Position.ADD(nextRelation->ViewPointData.Position);

			//далее все показатели кроме позиции с отставанием на шаг, так как имеют только информативный характер и не учитываются в расчетах
			//скорость как изменение позиции S = T * (V1 + V2) / 2  >> 2*S / T - V1 = V2 = (S+S)/T - V1
			out_Data.Velocity = out_Data.Position;
			out_Data.Velocity -= pCurData->Position;
			out_Data.AbsSpeed = out_Data.Velocity.LEN() / DeltaT;

			if (out_Data.AbsSpeed >= m_pCalc->maxAbsSpeed)
			{
				ret |= SP_STATE_ABS_SPEED_AL;
				if (out_Data.AbsSpeed > m_pCalc->maxAbsSpeed)
					*out_KeyPointTmp = pCurData->TimePoint + DeltaT * 0.75;
				out_Data.AbsSpeed = m_pCalc->maxAbsSpeed;
				if (pCurData->AbsSpeed < m_pCalc->maxAbsSpeed)
				{
					ret |= SP_STATE_ABS_CHANGE_LS;
					*out_KeyPointTmp = pCurData->TimePoint + DeltaT * 0.75;
				}
			}
			else
			{
				if (pCurData->AbsSpeed >= m_pCalc->maxAbsSpeed)
				{
					ret |= SP_STATE_ABS_CHANGE_LS;
					*out_KeyPointTmp = pCurData->TimePoint + DeltaT * 0.75;
				}
			}

			out_Data.Velocity.SET_LENGHT(out_Data.AbsSpeed);
			out_Data.Density = 0.0;
			nextRelation->State |= SP_STATE_PHANTOM_BESTUCK;
			
			//ускорение и тд не здесь
		}
			break;
		case 2:
			ASSERT(0);
			{
				SPos midPoint1, midPoint2;
				SPos sphCenter11, sphCenter12, sphCenter21, sphCenter22;
				int IdMaster1 = pBSList->List[0].IdMaster;
				int IdMaster2 = pBSList->List[1].IdMaster;

				SPRelation* curRelation1 = m_Relations.GetCurentRelation(in_IdParticle, IdMaster1);
				SPRelation* curRelation2 = m_Relations.GetCurentRelation(in_IdParticle, IdMaster2);
				
				sphCenter11 = curRelation1->ViewPointData.Position;
				sphCenter21 = curRelation2->ViewPointData.Position;

				SData SD_Sphere12, SD_Sphere22;
				m_Array[IdMaster1]->GetMovement(SD_Sphere12, curRelation1->ViewPointData, in_NextTime, out_KeyPoint);
				m_Array[IdMaster2]->GetMovement(SD_Sphere22, curRelation2->ViewPointData, in_NextTime, out_KeyPoint);


				midPoint1.SET_AS_MIDDLE_POINT(sphCenter11, sphCenter21);
				midPoint2.SET_AS_MIDDLE_POINT(sphCenter12, sphCenter22);

				SPos midPointMovement = midPoint2; midPointMovement.MINUS(midPoint1);
			}
			break;

		default:
			ASSERT(0);
			break;
		}

	}
	return ret;
}

int CSP_Group::NextStep()
{
	int ret = SP_STATE_ERROR_MAIN;
	int m_Count = PartCntr.GetCount();

	if (m_Array && m_Count > 1 && m_Array[0] && m_Array[1])
	{
		int prev_ParticlesState = m_ParticlesState;
		m_pCalc->Time.PrepareNextStep(m_pCalc->Stat);
		if ((prev_ParticlesState & GR_PS_COLLISION_BESTUCK))
			m_pCalc->Time.CheckByBeStuckStep();

		if ((prev_ParticlesState & GR_PS_MAXABSSPEED) != GR_PS_NORM &&  m_pCalc->Time.GetStep() > m_pCalc->Time.GetMinStep() + m_pCalc->Time.GetMinStep())
		{
			double deltaT = CalcStepUpLimit();
			if (m_pCalc->Time.GetStep() > deltaT)
				m_pCalc->Time.SetStep(deltaT);
		}

		int doByKeyPoint;
		double KeyPoint, NextTime;

		const BeStuckParticles* pBSP = m_Relations.GetBeStuck_Current();

		do
		{
			if (pBS_Corrected->GetCount())	pBS_Corrected->clear();
			doByKeyPoint = 0;

			do	//расчет новой позиции
			{
				m_Relations.ClearNextState(0, m_Count - 1, SP_STATE_RELATION_MASK_CLR_ALL);
				NextTime = KeyPoint = m_pCalc->Time.GetNextTime();
				m_ParticlesState = GR_PS_NORM;

				for (int i = 0; i < m_Count && KeyPoint == NextTime; i++)
				{
					m_Array[i]->NewPoint_Begin(NextTime);

					int BS_num = pBSP->GetNumById(i);

					if (BS_num >= 0)
					{
						CalcBeStuckNextPos(m_Array[i]->m_NextData, i, NextTime, &KeyPoint);
						//ASSERT(KeyPoint == NextTime);
						if ((CSP_Group::AssertionVar & 1))
						{//проверка на непревышение предельной скорости
							double cmpDist = m_Array[i]->m_Data->GetCurData()->Position.GetDistance(m_Array[i]->m_NextData.Position);

							double cmpSpeed = cmpDist / m_pCalc->Time.GetStep();

							ASSERT(!(CSP_Group::AssertionVar & 1) || cmpSpeed < 1.000000001);
							ASSERT(!(CSP_Group::AssertionVar & 1) || cmpDist < m_pCalc->Time.GetStep() + 1.0e-15);
						}
					}
					else
					{
						m_Array[i]->GetNextDataByTime(m_Array[i]->m_NextData, NextTime, &KeyPoint);
						//ASSERT(KeyPoint == NextTime);
					}

					if (m_Array[i]->m_NextData.AbsSpeed >= m_pCalc->maxAbsSpeed)
						m_ParticlesState |= GR_PS_MAXABSSPEED;

					if (NextTime != KeyPoint)
						KeyPoint = m_pCalc->Time.SetNextTimeEx(KeyPoint);
				}

				if (NextTime == KeyPoint && (prev_ParticlesState & GR_PS_COLLISION_BESTUCK))
				{//коррекция BeStuck частиц с возможным изменением времени
					CorrectBeStuckParticles(GR_PS_RELATION_NEXT, KeyPoint);
				}

			} while (NextTime != KeyPoint);

			int CicleCntr = 0;

			do
			{
				CicleCntr++;
				ASSERT(CicleCntr < 100);
				m_Relations.ClearNextState(0, m_Count - 1, SP_STATE_PHANTOM_BESTUCK);

				//уточнение времени, пока есть предупреждения
				doByKeyPoint = CalcRelations(NextTime, m_Relations.GetNext(), m_Relations.GetCurent(), SP_CALCRELATION_BREAK_ON_MINOR_ERROR | SP_CALCRELATION_FIND_VIEWPOINT, KeyPoint);
				//ASSERT(fabs(m_Relations.GetNext()[1].Acceleration_3D.X) < 1.0e80);
				ASSERT(NextTime >= KeyPoint);
				if (NextTime != KeyPoint)
				{
					KeyPoint = m_pCalc->Time.SetNextTimeEx(KeyPoint);
					doByKeyPoint |= SP_STATE_NEED_RECALC;
				}
				if (NextTime == KeyPoint && (doByKeyPoint & SP_STATE_PHANTOM_CRITICAL_CHANGED))
				{

					m_Relations.clear_BeStuck(GR_PS_RELATION_CORRECTED);
					doByKeyPoint |= CorrectBeStuckParticles(GR_PS_RELATION_NEXT| GR_PS_RELATION_NEEDRECALC, KeyPoint);
					if (NextTime != KeyPoint) KeyPoint = m_pCalc->Time.SetNextTimeEx(KeyPoint);
					if ((doByKeyPoint & GR_PS_POSITION_CHANGED))
						doByKeyPoint |= SP_STATE_NEED_RECALC;
				} 
			} while (NextTime == KeyPoint && (doByKeyPoint & GR_PS_POSITION_CHANGED) && m_pCalc->Time.IsMinimalStep() == false && (CicleCntr <50 || m_pCalc->Time.GetCurTime() < 1000.0));

		} while (NextTime != KeyPoint);

		if ((doByKeyPoint & SP_STATE_NEED_RECALC))// FINAL if need
		{
			m_Relations.ClearNextState(0, m_Count - 1, SP_STATE_PHANTOM_BESTUCK);
			doByKeyPoint = CalcRelations(NextTime, m_Relations.GetNext(), m_Relations.GetCurent(), SP_CALCRELATION_FIND_VIEWPOINT, KeyPoint);
		}

		if ((doByKeyPoint & (SP_STATE_PHANTOM_DIST_LE_CRITICAL| SP_STATE_PHANTOM_DENSITY_LE_0)))
			m_Relations.FillNBeStuck(GR_PS_RELATION_NEXT, 0, m_Count);
		else
			m_Relations.clear_BeStuck(GR_PS_RELATION_NEXT);

		CalcDataByRelations(GR_PS_RELATION_NEXT);


		ASSERT(m_Array[0]->m_NextData.TimePoint == m_Array[1]->m_NextData.TimePoint);
		ASSERT(m_Array[0]->m_NextData.TimePoint != m_Array[0]->GetCurData_C()->TimePoint);

		double cmpDist = m_Array[0]->m_Data->GetCurData()->Position.GetDistance(m_Array[0]->m_NextData.Position);

		ASSERT(!(CSP_Group::AssertionVar & 1) || cmpDist < m_pCalc->Time.GetStep() + 1.0e-15);

		STracePosInfo nextStepInfo = { 0 };

		nextStepInfo.TimePoint = NextTime;
		nextStepInfo.ParticlesState = m_ParticlesState;
		nextStepInfo.RelationsS_Prev = m_ParticleInfoArray->GetCurData()->RelationsState_Cur;
		nextStepInfo.RelationsState_Cur = doByKeyPoint;

		EnterCriticalSection(&m_cs);

		int AddType = DA_ADD_FlagFromResult(m_ParticleInfoArray->AddData(nextStepInfo, DA_ADD_FLAG_ADD_AUTO));

		ret = SP_STATE_ОК;
		//фиксация результата и переход на следующую итерацию
		for (int i = 0; i < m_Count; i++)
		{
			m_Array[i]->MoveToNewPoint(AddType);
			ASSERT(m_Array[i]->GetCurPos() == m_ParticleInfoArray->GetCurPos());
			ASSERT(m_Array[i]->GetCurTime() == m_ParticleInfoArray->GetCurTime());
		}
		
		ASSERT(m_Array[0]->GetCurTime() == m_Array[1]->GetCurTime());

		m_pCalc->Time.SetCurTime(NextTime);

		m_Relations.DoStep();

		//CalcDataByRelations(GR_PS_RELATION_CURRENT);//!!!!!!!!!!!!!!!!!!!!!!!!
		CollectStat(STAT_TYPE_ALL_CURRENT|STAT_TYPE_SETPARTICLE);

		LeaveCriticalSection(&m_cs);
	}

	return ret;
}

int CSP_Group::CalcSingleRelation(CalceRelationParams& Params)
{
	Params.Ret_Flags = SP_STATE_ERROR_MAIN;
	
	if (Params.PtrViewParticle && Params.NewRelations)
	{
		const SData* curParticleData = Params.PtrParticle->GetCurData_C();
		const SData* curViewParticleData = Params.PtrViewParticle->GetCurData_C();

		Params.NewRelations->RealDistancePwr1 = curParticleData->Position.GetDistance(curViewParticleData->Position);
		Params.NewRelations->RealRelativeSpeed = SData::GetRelativeSpeed(*curParticleData, *curViewParticleData, NULL);

		int tmpRet = 0;
		int prevState = 0;
		ASSERT(Params.PrevRelations);

		prevState = Params.PrevRelations->State;

		if ((Params.CalcType & SP_CALCRELATION_FIND_VIEWPOINT))
		{
			if ((Params.NewRelations->State & SP_STATE_PHANTOM_BESTUCK))
			{
				;
			}
			else
			{
				double in_StartTime = -99999.0, EndTime;

				EndTime = Params.PtrViewParticle->GetCurTime();
				in_StartTime = Params.NewRelations->LastViewPointTime = Params.PrevRelations->LastViewPointTime;

				if (EndTime > Params.ViewTime) EndTime = Params.ViewTime;

				ASSERT(EndTime >= 0.0);

				Params.out_KeyPoint = Params.ViewTime;
				ASSERT(Params.ViewTime - m_pCalc->Time.GetCurTime() > 0.0);

				tmpRet = Params.PtrViewParticle->FindViewPointByTime(Params.NewRelations->ViewPointData, Params.DataParticle, Params.out_KeyPoint, in_StartTime, EndTime, &Params.NewRelations->LowIdxOfViewPont_Near);

				if ((tmpRet & SP_STATE_FAR_AWAY) == SP_STATE_FAR_AWAY && ((tmpRet ^ prevState) & SP_STATE_FAR_AWAY) == 0 && Params.NewRelations->RealDistancePwr1 < m_pCalc->CriticalDistance) ///&& m_pCalc->Time.IsMinimalStep()
				{ //возможно немного оптимизировать по расстоянию и скорости
					SData dataM1;
					Params.PtrViewParticle->GetPrevDataByTime(dataM1, Params.out_KeyPoint - m_pCalc->CriticalDistance, NULL);

					double dist = dataM1.Position.GetDistance(Params.PtrParticle->GetCurPosition()) - m_pCalc->CriticalDistance;
					if (dist < m_pCalc->Time.GetStep() + m_pCalc->Time.GetMinStep())
					{
						if (m_pCalc->Time.IsMinimalStep())
						{ //На предыдущем шаге была невидимость и шаг минимальный
							Params.NewRelations->ViewPointData = dataM1;
							tmpRet = SP_STATE_ОК;
						}
						else
						{ //возможно немного оптимизировать за счет интерполяции
							Params.out_KeyPoint = m_pCalc->Time.GetNextTime_ByCorrectedDelta(m_pCalc->Time.GetStep4321());
							if (Params.pInterruptionFlag) *Params.pInterruptionFlag = 1;
						}
					}
				}
			}
			ASSERT(!isnan(Params.NewRelations->ViewPointData.TimePoint));
		}

		if ((tmpRet & SP_STATE_ERROR_MINOR) == SP_STATE_ERROR_MAIN)
		{
			if (Params.pInterruptionFlag) *Params.pInterruptionFlag = 1;
			Params.Ret_Flags = SP_STATE_ERROR_MAIN;
		}
		else if ((tmpRet & SP_STATE_ERROR_MINOR) && (tmpRet & SP_STATE_HIGH_VIEW))
		{
			if (Params.pInterruptionFlag) *Params.pInterruptionFlag = 1;
			Params.Ret_Flags = tmpRet;
			Params.NewRelations->SET_AS_FARAWAY(Params.PtrViewParticle->m_TypeFlags);
		}
		else if ((tmpRet & SP_STATE_FAR_AWAY))
		{
			Params.Ret_Flags = tmpRet;
			Params.NewRelations->SET_AS_FARAWAY(Params.PtrViewParticle->m_TypeFlags);
			m_ParticlesState |= GR_PS_FAR_AWAY_PRESENT;

			if (((tmpRet ^ prevState) & SP_STATE_FAR_AWAY))
			{ // Переход в невидимость
				if (!m_pCalc->Time.IsMinimalStep())
				{
					m_pCalc->Time.SetUpLimit(m_pCalc->Time.GetNextTime());
					Params.out_KeyPoint = m_pCalc->Time.GetNextTime_ByCorrectedDelta(m_pCalc->Time.GetStep4321());
					if (Params.pInterruptionFlag) *Params.pInterruptionFlag = 1;
				}
				m_ParticlesState |= GR_PS_FAR_AWAY_PRESENT_ON;
			}
		}
		else if ((tmpRet & SP_STATE_ERROR_MINOR) && (tmpRet & SP_STATE_LOW_VIEW) && Params.NewRelations->DistancePwr1 > 1.0e10)
		{
			if (Params.pInterruptionFlag) *Params.pInterruptionFlag = 1;
			Params.Ret_Flags = tmpRet;
			Params.NewRelations->SET_AS_FARAWAY(Params.PtrViewParticle->m_TypeFlags);
		}
		else
		{
			Params.Ret_Flags = SP_STATE_ОК;
			int RelParticleType = (Params.PtrViewParticle->m_TypeFlags & SP_TYPE_FORM_MASK);


			if (((tmpRet ^ prevState) & SP_STATE_FAR_AWAY))
			{//выход из невидимости
				Params.Ret_Flags = tmpRet;
				Params.Ret_Flags |= SP_STATE_AFTER_FAR_AWAY;
				m_ParticlesState |= GR_PS_FAR_AWAY_PRESENT_OFF;

				if (!m_pCalc->Time.IsMinimalStep())
				{
					m_pCalc->Time.SetUpLimit(m_pCalc->Time.GetNextTime());
					Params.out_KeyPoint = m_pCalc->Time.GetNextTime_ByCorrectedDelta(m_pCalc->Time.GetStep() * 0.5);
					if (Params.pInterruptionFlag) *Params.pInterruptionFlag = 1;
				}
			}

			Params.NewRelations->LastViewPointTime = Params.NewRelations->ViewPointData.TimePoint;

			/////////SPEED
			if ((Params.NewRelations->State & SP_STATE_PHANTOM_BESTUCK))
			{
				Params.NewRelations->RelativeSpeedAbs = Params.NewRelations->RelativeSpeed = 0.0;
				Params.NewRelations->RelativeVelocity_3D = { 0 };
			}
			else
			{
				Params.NewRelations->RelativeSpeed = SData::GetRelativeSpeed(Params.DataParticle, Params.NewRelations->ViewPointData, &Params.NewRelations->RelativeVelocity_3D);
				Params.NewRelations->RelativeSpeedAbs = fabs(Params.NewRelations->RelativeSpeed);
			}
			Params.NewRelations->Distance_3D.X = Params.DataParticle.Position.X - Params.NewRelations->ViewPointData.Position.X;
			Params.NewRelations->Distance_3D.Y = Params.DataParticle.Position.Y - Params.NewRelations->ViewPointData.Position.Y;
			Params.NewRelations->Distance_3D.Z = Params.DataParticle.Position.Z - Params.NewRelations->ViewPointData.Position.Z;

			{
				double DistancePwr1tmp = Params.NewRelations->Distance_3D.LEN();

				if (m_pCalc->ParentSpaceThickness != 0.0)
				{
					double tmpMltplr = (DistancePwr1tmp + m_pCalc->ParentSpaceThickness) / DistancePwr1tmp;
					Params.NewRelations->Distance_3D.X *= tmpMltplr;
					Params.NewRelations->Distance_3D.Y *= tmpMltplr;
					Params.NewRelations->Distance_3D.Z *= tmpMltplr;
					DistancePwr1tmp += m_pCalc->ParentSpaceThickness;
				}

				Params.NewRelations->DistancePwr1 = DistancePwr1tmp;
				Params.NewRelations->DistancePwr2 = DistancePwr1tmp * DistancePwr1tmp;
				Params.NewRelations->DistancePwr3 = Params.NewRelations->DistancePwr2 * DistancePwr1tmp;
			}

			if (RelParticleType == SP_TYPE_NEGATIVE)
			{
				ASSERT(!(Params.NewRelations->State& SP_STATE_PHANTOM_BESTUCK));

				if (Params.NewRelations->DistancePwr1 <= m_pCalc->CriticalDistance)
				{
					Params.Ret_Flags |= SP_STATE_ERROR_MINOR | SP_STATE_PHANTOM_DIST_LE_CRITICAL;
					Params.NewRelations->State |= GR_PS_COLLISION_DISTANCE;
					m_ParticlesState |= GR_PS_COLLISION_DISTANCE;
					Params.NewRelations->DistancePwr1 = m_pCalc->CriticalDistance;
					Params.NewRelations->DistancePwr2 = m_pCalc->CriticalDistancePwr2;
					Params.NewRelations->DistancePwr3 = m_pCalc->CriticalDistancePwr3;
					Params.out_KeyPoint = m_pCalc->Time.GetNextTime_ByCorrectedDelta(m_pCalc->Time.GetStep() * 0.231);
				}
			}
			else
			{
				if ((Params.NewRelations->State & SP_STATE_PHANTOM_BESTUCK) || fabs(Params.NewRelations->DistancePwr1 - m_pCalc->CriticalDistance) <= 5.0e-14)
				{
					Params.NewRelations->DistancePwr1 = m_pCalc->CriticalDistance;
					Params.NewRelations->DistancePwr2 = m_pCalc->CriticalDistancePwr2;
					Params.NewRelations->DistancePwr3 = m_pCalc->CriticalDistancePwr3;
				}

				if (Params.NewRelations->DistancePwr1 <= m_pCalc->CriticalDistance)
				{
					Params.Ret_Flags |= SP_STATE_ERROR_MINOR | SP_STATE_PHANTOM_DIST_LE_CRITICAL;
					Params.NewRelations->State |= (GR_PS_COLLISION_DISTANCE | GR_PS_COLLISION_BESTUCK);
					m_ParticlesState |= (GR_PS_COLLISION_DISTANCE | GR_PS_COLLISION_BESTUCK);
				}
			}
			
			if (((Params.NewRelations->State ^ Params.PrevRelations->State) & GR_PS_COLLISION_DISTANCE))
			{	/////////// расчет точки достижения критической дистанции
				if (RelParticleType == SP_TYPE_NEGATIVE)
				{
					Params.Ret_Flags |= SP_STATE_ANTIPHANTOM_DIST_CRITICAL_CHG;
					Params.NewRelations->State |= SP_STATE_ANTIPHANTOM_DIST_CRITICAL_CHG;
				}
				else
				{
					Params.Ret_Flags |= SP_STATE_PHANTOM_DIST_CRITICAL_CHG;
					Params.NewRelations->State |= SP_STATE_PHANTOM_DIST_CRITICAL_CHG;
				}

				if (!m_pCalc->Time.IsMinimalStep())
				{
					if (Params.pInterruptionFlag) *Params.pInterruptionFlag = 1;

					if (!Params.PrevRelations->RelativeVelocity_3D.IsZero() || !Params.PrevRelations->Acceleration_3D.IsZero())
					{
						TwoDoubleRoots result;
						SPos dV = Params.PrevRelations->Distance_3D;
						dV.SET_AS_PROJECTION_OF(Params.PrevRelations->RelativeVelocity_3D);
						SPos dA = Params.PrevRelations->Distance_3D;
						dA.SET_AS_PROJECTION_OF(Params.PrevRelations->Acceleration_3D);
						dA.MULT(0.5);

						result = QuadraticEequationSolution(dA.LEN(), dV.LEN(), m_pCalc->CriticalDistance - Params.PrevRelations->DistancePwr1);
						double newDeltaTime = result.root1.Dbl;
						if ((isnan(newDeltaTime) || newDeltaTime < 0.0) && (isnan(newDeltaTime = result.root2.Dbl) || newDeltaTime < 0.0))
						{
							//ASSERT(0);
							Params.out_KeyPoint = m_pCalc->Time.GetNextTime_ByCorrectedDelta(m_pCalc->Time.GetStep() * 0.15);
						}
						else if (newDeltaTime == 0.0)
						{
							Params.out_KeyPoint = m_pCalc->Time.GetNextTime_ByCorrectedDelta(m_pCalc->Time.GetStep()) - m_pCalc->Time.GetMinStep() * 0.1;
							Params.NewRelations->DistancePwr1 = m_pCalc->CriticalDistance;
							Params.NewRelations->DistancePwr2 = m_pCalc->CriticalDistancePwr2;
							Params.NewRelations->DistancePwr3 = m_pCalc->CriticalDistancePwr3;
						}
						else if (m_pCalc->Time.GetCurTime() + newDeltaTime <= Params.out_KeyPoint)
						{
							Params.out_KeyPoint = m_pCalc->Time.GetNextTime_ByCorrectedDelta(newDeltaTime);
							if (Params.out_KeyPoint >= m_pCalc->Time.GetNextTime())
							{
								Params.NewRelations->DistancePwr1 = m_pCalc->CriticalDistance;
								Params.NewRelations->DistancePwr2 = m_pCalc->CriticalDistancePwr2;
								Params.NewRelations->DistancePwr3 = m_pCalc->CriticalDistancePwr3;
							}
						}
						else
						{
							//ASSERT(0);
							Params.out_KeyPoint = m_pCalc->Time.GetNextTime_ByCorrectedDelta(m_pCalc->Time.GetStep() * 0.4321);
						}
					}
					else
						Params.out_KeyPoint = m_pCalc->Time.GetNextTime_ByCorrectedDelta(m_pCalc->Time.GetStep() * 0.4321);
				}
				else
				{
					Params.NewRelations->DistancePwr1 = m_pCalc->CriticalDistance;
					Params.NewRelations->DistancePwr2 = m_pCalc->CriticalDistancePwr2;
					Params.NewRelations->DistancePwr3 = m_pCalc->CriticalDistancePwr3;
				}
			}
			//расчет плотности
			Params.NewRelations->DensityAtBasePoint = Calc_RelativeDensity(RelParticleType, m_pCalc->DensityCalcType, Params.NewRelations->DistancePwr1, Params.NewRelations->DistancePwr2);

			if (Params.NewRelations->DensityAtBasePoint <= 0.0 && RelParticleType != SP_TYPE_NEGATIVE)
			{
				Params.NewRelations->State |= (GR_PS_COLLISION_DENSITY | GR_PS_COLLISION_BESTUCK);
				Params.Ret_Flags |= SP_STATE_ERROR_MINOR | SP_STATE_PHANTOM_DENSITY_LE_0;
				m_ParticlesState |= (GR_PS_COLLISION_DENSITY| GR_PS_COLLISION_BESTUCK);
			}
			//else
			//	if (Params.NewRelations->DensityAtBasePoint >= 1.0 && RelParticleType == SP_TYPE_NEGATIVE)
			//	{
			//		Params.NewRelations->State |= (GR_PS_COLLISION_DENSITY );
			//		Params.Ret_Flags |= SP_STATE_ERROR_MINOR | SP_STATE_PHANTOM_DENSITY_LE_0;
			//		m_ParticlesState |= (GR_PS_COLLISION_DENSITY);
			//	}


			if (((Params.NewRelations->State ^ Params.PrevRelations->State) & GR_PS_COLLISION_DENSITY))
			{/////////// расчет точки достижения критической дистанции
				if (RelParticleType == SP_TYPE_NEGATIVE)
				{
					Params.Ret_Flags |= SP_STATE_ANTIPHANTOM_DENSITY_LE_CHG;
					Params.NewRelations->State |= SP_STATE_ANTIPHANTOM_DENSITY_LE_CHG;
				}
				else
				{
					Params.Ret_Flags |= SP_STATE_PHANTOM_DENSITY_LE_CHG;
					Params.NewRelations->State |= SP_STATE_PHANTOM_DENSITY_LE_CHG;
				}

				if (!m_pCalc->Time.IsMinimalStep())
				{
					if (Params.pInterruptionFlag) *Params.pInterruptionFlag = 1;

					if (!((Params.NewRelations->State ^ Params.PrevRelations->State) & GR_PS_COLLISION_DISTANCE)) //!!!! идентично, если было
						if (!Params.PrevRelations->RelativeVelocity_3D.IsZero() || !Params.PrevRelations->Acceleration_3D.IsZero())
						{	/////////// расчет точки достижения нулевой плотности 
							TwoDoubleRoots result;
							SPos dV = Params.PrevRelations->Distance_3D;
							dV.SET_AS_PROJECTION_OF(Params.PrevRelations->RelativeVelocity_3D);
							SPos dA = Params.PrevRelations->Distance_3D;
							dA.SET_AS_PROJECTION_OF(Params.PrevRelations->Acceleration_3D);
							dA.MULT(0.5);

							result = QuadraticEequationSolution(dA.LEN(), dV.LEN(), m_pCalc->CriticalDistance - Params.PrevRelations->DistancePwr1);
							double newDeltaTime = result.root1.Dbl;
							if ((isnan(newDeltaTime) || newDeltaTime < 0.0) && (isnan(newDeltaTime = result.root2.Dbl) || newDeltaTime < 0.0))
							{
								//ASSERT(0);
								Params.out_KeyPoint = m_pCalc->Time.GetNextTime_ByCorrectedDelta(m_pCalc->Time.GetStep() * 0.15);
							}
							else if (newDeltaTime == 0.0)
							{
								Params.out_KeyPoint = m_pCalc->Time.GetNextTime_ByCorrectedDelta(m_pCalc->Time.GetStep()) - m_pCalc->Time.GetMinStep() * 0.1;
								Params.NewRelations->DistancePwr1 = m_pCalc->CriticalDistance;
								Params.NewRelations->DistancePwr2 = m_pCalc->CriticalDistancePwr2;
								Params.NewRelations->DistancePwr3 = m_pCalc->CriticalDistancePwr3;
							}
							else if (m_pCalc->Time.GetCurTime() + newDeltaTime <= Params.out_KeyPoint)
							{
								Params.out_KeyPoint = m_pCalc->Time.GetNextTime_ByCorrectedDelta(newDeltaTime);
								if (Params.out_KeyPoint >= m_pCalc->Time.GetNextTime())
								{
									Params.NewRelations->DistancePwr1 = m_pCalc->CriticalDistance;
									Params.NewRelations->DistancePwr2 = m_pCalc->CriticalDistancePwr2;
									Params.NewRelations->DistancePwr3 = m_pCalc->CriticalDistancePwr3;
								}
							}
							else
							{
								//ASSERT(0);
								Params.out_KeyPoint = m_pCalc->Time.GetNextTime() - m_pCalc->Time.GetMinStep() * 0.1;
							}
						}
						else
						{
							if (Params.out_KeyPoint - m_pCalc->Time.GetCurTime() > 0.00001)
								Params.out_KeyPoint = m_pCalc->Time.GetNextTime_ByCorrectedDelta(0.00001);
						}
				}
			}

			//расчет ускорения
			Params.NewRelations->AccelerationValue = Calc_RelativeAcceleration(RelParticleType, m_pCalc->AccCalcType, Params.NewRelations->DensityAtBasePoint);

			if (Params.NewRelations->AccelerationValue < 0.0)
			{
				if (RelParticleType != SP_TYPE_NEGATIVE)
				{
					//это если DensityAtBasePoint > 1 или < 0.0, что пока не рассматривается
					Params.Ret_Flags |= SP_STATE_ERROR_MINOR | SP_STATE_REL_ACCELERATION_INF;
					Params.NewRelations->State |= SP_STATE_REL_ACCELERATION_INF;

					if (!(Params.NewRelations->State & GR_PS_COLLISION_DENSITY) && Params.pInterruptionFlag && !Params.pInterruptionFlag)
					{
						ASSERT(0);
					}
				}
				else
				{
					Params.NewRelations->Acceleration_3D = Params.NewRelations->Distance_3D;
					Params.NewRelations->Acceleration_3D.SET_LENGHT(-Params.NewRelations->AccelerationValue);
				}

			}
			else
			{
				if ((Params.PtrViewParticle->m_TypeFlags & SP_TYPE_FORM_MASK) != SP_TYPE_NEGATIVE)
				{

					if ((Params.NewRelations->State & (GR_PS_COLLISION_DISTANCE | GR_PS_COLLISION_BESTUCK)))
					{
						Params.NewRelations->Acceleration_3D = { 0 };
					}
					else
					{
						Params.NewRelations->Acceleration_3D = Params.NewRelations->Distance_3D;
						Params.NewRelations->Acceleration_3D.SET_LENGHT(-Params.NewRelations->AccelerationValue);
					}
				}
				else
				{
					Params.Ret_Flags |= SP_STATE_ERROR_MINOR | SP_STATE_REL_ACCELERATION_INF;
					Params.NewRelations->State |= SP_STATE_REL_ACCELERATION_INF;

					if (!(Params.NewRelations->State & GR_PS_COLLISION_DENSITY) && Params.pInterruptionFlag && !Params.pInterruptionFlag)
					{
						ASSERT(0);
					}
				}
			}

			ASSERT(fabs(Params.NewRelations->Acceleration_3D.X) < 1.0e80);

			if (m_pCalc->WindType && Params.NewRelations->RelativeSpeedAbs > 0.0000000000001)
			{
				switch (m_pCalc->WindType) // Fantasy !!!!!
				{
				case 0://OFF
				default:
					break;
				case 1: // ветер влияет прямо пропорционально относительной скорости 
					Params.NewRelations->Wind_3D.X = (SP_DENSITY_MAX - Params.NewRelations->DensityAtBasePoint) * Params.NewRelations->RelativeVelocity_3D.X;
					Params.NewRelations->Wind_3D.Y = (SP_DENSITY_MAX - Params.NewRelations->DensityAtBasePoint) * Params.NewRelations->RelativeVelocity_3D.Y;
					Params.NewRelations->Wind_3D.Z = (SP_DENSITY_MAX - Params.NewRelations->DensityAtBasePoint) * Params.NewRelations->RelativeVelocity_3D.Z;
					break;
				case 2:	// ветер влияет прямо пропорционально относительной скорости 
					Params.NewRelations->Wind_3D.X = (SP_DENSITY_MAX - Params.NewRelations->DensityAtBasePoint) * Params.NewRelations->RelativeVelocity_3D.X * 0.5;
					Params.NewRelations->Wind_3D.Y = (SP_DENSITY_MAX - Params.NewRelations->DensityAtBasePoint) * Params.NewRelations->RelativeVelocity_3D.Y * 0.5;
					Params.NewRelations->Wind_3D.Z = (SP_DENSITY_MAX - Params.NewRelations->DensityAtBasePoint) * Params.NewRelations->RelativeVelocity_3D.Z * 0.5;
					break;
				case 3: // ветер влияет прямо пропорционально относительной скорости 
					Params.NewRelations->Wind_3D.X = Params.NewRelations->RelativeVelocity_3D.X / Params.NewRelations->DensityAtBasePoint;
					Params.NewRelations->Wind_3D.Y = Params.NewRelations->RelativeVelocity_3D.Y / Params.NewRelations->DensityAtBasePoint;
					Params.NewRelations->Wind_3D.Z = Params.NewRelations->RelativeVelocity_3D.Z / Params.NewRelations->DensityAtBasePoint;
					break;
				}
			}
		}
		Params.PtrParticle->m_StateNext |= Params.Ret_Flags;
	}
	return Params.Ret_Flags;
}

int CSP_Group::CalcRelationsSingle(double in_TimePoint, SPRelation* const in_CalcRelations, SPRelation* const in_PrevRelations, int in_CalcType, int in_TimeType, double & out_KeyPoint)
{
	CalceRelationParams curParams;
	int m_Count = PartCntr.GetCount();

	curParams.DataParticle = {0};
	out_KeyPoint = curParams.out_KeyPoint = curParams.ViewTime = in_TimePoint;
	curParams.CalcType = in_CalcType;
	curParams.ThreadID = 0;
	curParams.Ret_Flags = 0;
	int curRet_Flags = 0;
	curParams.pInterruptionFlag = nullptr;
	curParams.TimeType = in_TimeType;
	curParams.BSI.IdMaster = -1;
	//curParams.PtrParticle = nullptr;
	//curParams.PtrViewParticle = nullptr;
	//curParams.PrevRelations = nullptr;
	//curParams.NewRelations = nullptr;

	if (m_Array)
	{
		double tmpDbl = curParams.ViewTime;
		int maskBreak = (in_CalcType & SP_CALCRELATION_BREAK_ON_MINOR_ERROR ) ? SP_STATE_PHANTOM_CRITICAL_CHANGED : 0;
		volatile INT64 InterruptionFlag = 0;
		if (maskBreak) curParams.pInterruptionFlag = &InterruptionFlag;
		int i;
		for (i = 0; i < m_Count && !(curRet_Flags & maskBreak) && !InterruptionFlag; i++)
			if ((in_CalcType == -1 || !(in_CalcType & SP_CALCRELATION_NEED_UPDATE_ONLY)) || (in_CalcRelations[i].State & SP_STATE_NEED_UPDATE))
			{
				curParams.PtrParticle = m_Array[i];
				curParams.Particle_ID = i;
				if (in_TimeType == 0) 
					curParams.DataParticle = m_Array[i]->m_NextData;
				else if (curParams.ViewTime > m_Array[0]->GetCurTime())
					m_Array[i]->GetNextDataByTime(curParams.DataParticle, curParams.ViewTime, &tmpDbl);
				else
					m_Array[i]->GetPrevDataByTime(curParams.DataParticle, curParams.ViewTime, &tmpDbl);
				int j, RelationPos = i;
				for (j = 0; j < m_Count && !(curRet_Flags & maskBreak) && !InterruptionFlag; j++, RelationPos += m_maxCount)
					if (i != j)
					{
						curParams.PtrViewParticle = m_Array[j];
						curParams.ViewParticle_ID = j;
						curParams.PrevRelations = in_PrevRelations + RelationPos;
						curParams.NewRelations = in_CalcRelations + RelationPos;

						curRet_Flags |= CalcSingleRelation(curParams);

						if (curParams.out_KeyPoint < out_KeyPoint)
							out_KeyPoint = curParams.out_KeyPoint;
					}

				if (j < m_Count)
					curRet_Flags |=SP_STATE_NEED_RECALC;
			}
		if (i < m_Count)
			curRet_Flags |=SP_STATE_NEED_RECALC;
	}
	else curRet_Flags = SP_STATE_ERROR_MAIN;

	return curRet_Flags;
}

UINT Do_RelationThread(LPVOID pParam)
{
	int InnerFlag = 0;
	GR_Thread* m_Thread = (GR_Thread*)pParam;
	int ret = -1;
	if (m_Thread)
	{
		m_Thread->ThreadID = GetCurrentThreadId();
		m_Thread->ThreadInnerFlagPtr = &InnerFlag;

		CalceRelationParams curParams = {0};

		if (m_Thread->Group)
		{
			ret = 1;
			m_Thread->StateBySlave |= GR_THREAD_READY;
			InnerFlag = 1;

			while (InnerFlag && !m_Thread->StopFlag)
			{
				m_Thread->Slave_WaitToCalc();
				m_Thread->Result = 0;

				REPORTER_THREAD_CALC(m_Thread->Group->m_reporter.OUTPUT("Do_RelationThread Begin", m_Thread->IdParticleStart, m_Thread->IdParticleEnd);)

				curParams.ViewTime = m_Thread->KeyPointTime = m_Thread->TimePoint;
				curParams.CalcType = m_Thread->CalcType;
				curParams.ThreadID = m_Thread->ID;
				curParams.TimeType = 0;


				curParams.out_KeyPoint = curParams.ViewTime;
				curParams.Ret_Flags = 0;

				if (InnerFlag && !m_Thread->StopFlag)
				{
					int PtCount = m_Thread->Group->GetCount();
					int m_maxCount = m_Thread->Group->GetCountMax();

					int maskBreak;
					if ((m_Thread->CalcType & SP_CALCRELATION_BREAK_ON_MINOR_ERROR))
					{
						maskBreak = SP_STATE_WARNING_MASK;
						curParams.pInterruptionFlag = &m_Thread->Group->InterruptionFlag;
					}
					else
					{
						maskBreak = 0;
						curParams.pInterruptionFlag = nullptr;
					}

					bool bNotFullDone = true;

					while ((bNotFullDone = m_Thread->NEXT()) && !m_Thread->StopFlag && !(m_Thread->Result & maskBreak) && !m_Thread->Group->InterruptionFlag)
					{
						int ParticleID = m_Thread->GetIdParticleCurrent();
						REPORTER_THREAD_CALC(m_Thread->Group->m_reporter.OUTPUT("Do_RelationThread Do", m_Thread->ID, ParticleID);)

						CSpParticle* pParticle = m_Thread->Group->GetParticleById(ParticleID);
						curParams.Particle_ID = ParticleID;

						if (!m_Thread->StopFlag && pParticle && (!(m_Thread->CalcType & SP_CALCRELATION_NEED_UPDATE_ONLY) || (m_Thread->pCalcRelations[ParticleID].State & SP_STATE_NEED_UPDATE))  && !m_Thread->Group->InterruptionFlag && !(m_Thread->Result & maskBreak))
						{
							curParams.DataParticle = *pParticle->GetNextDataPtr();
							curParams.PtrParticle = pParticle;
							//!!!!!!!!!!!!!!!!!!
							int RelationPos = ParticleID;
							int j;
							for (j = 0; !m_Thread->StopFlag && j < PtCount && !m_Thread->Group->InterruptionFlag && !(m_Thread->Result & maskBreak); j++, RelationPos += m_maxCount)
								if (ParticleID != j && (curParams.PtrViewParticle = m_Thread->Group->GetParticleById(j)))
								{
									curParams.ViewParticle_ID = j;
									curParams.PrevRelations = m_Thread->pPrevRelations ? m_Thread->pPrevRelations + RelationPos : nullptr;
									curParams.NewRelations = m_Thread->pCalcRelations + RelationPos;
									curParams.out_KeyPoint = m_Thread->TimePoint;
									m_Thread->Result |= m_Thread->Group->CalcSingleRelation(curParams);
									if (m_Thread->KeyPointTime > curParams.out_KeyPoint) m_Thread->KeyPointTime = curParams.out_KeyPoint;
								}

							if (j < PtCount)
								curParams.Ret_Flags |= SP_STATE_NEED_RECALC;

						}
						curParams.Ret_Flags = m_Thread->Result;
					}
					if (bNotFullDone)
						curParams.Ret_Flags |= SP_STATE_NEED_RECALC;
				}
				m_Thread->Slave_CalcCompleted();
			}
		}
		m_Thread->ThreadInnerFlagPtr = nullptr;
		m_Thread->StateBySlave = GR_THREAD_OFF;
	}
	return ret;
}

int CSP_Group::CalcRelationsMulti(double in_TimePoint, SPRelation* const in_CalcRelations, SPRelation* const in_PrevRelations, int in_CalcType, double& out_KeyPoint)
{
	int AddResult = 0;
	REPORTER_THREAD_CALC(m_reporter.OUTPUT("CalcRelationsMulti Begin", (int)GetTickCount());)

	if (m_Array && !(m_CalcState & M_STATE_BREAK))
	{
		int m_Count = GetCount();
		int nDone = 0;

		std::atomic_thread_fence(std::memory_order_acquire);

		//Проверяем - все ли потоки добежали до ожидания начала расчета
		do
		{
			nDone = 0;
			for (int i = 0; i < m_ThreadsCur; i++)
			{
				if (StateArray[i] == 0)
					nDone++;
				else
				{
					Wait_After[i]++;
					REPORTER_THREAD_CALC(m_reporter.OUTPUT("Master\t CalcRelationsMulti WakeByAddress Wait_After", Wait_After[i], i);)
					WakeByAddressSingle((volatile PVOID)&Wait_After[i]);
				}
			}
			if (nDone < m_ThreadsCur)
				std::this_thread::yield();
		} while (nDone < m_ThreadsCur);


		CSP_Group::InterruptionFlag = 0;

		out_KeyPoint = in_TimePoint;
		for (int i = 0, Delta = m_Count % m_ThreadsCur, iStep = (m_Count - 1) / m_ThreadsCur + 1, curPtrId = 0; i < m_ThreadsCur && curPtrId < m_Count; i++)
		{
			if (Delta > 0 && Delta < m_Count && i == Delta) iStep--;
			m_Thr_Calc[i]->MasterSetCalc(in_TimePoint, curPtrId, curPtrId + iStep - 1, in_CalcType, in_PrevRelations, in_CalcRelations);
			curPtrId += iStep;
		}

		std::atomic_thread_fence(std::memory_order_release);

		if (!(m_CalcState & M_STATE_BREAK))
		{
			//Пробуждаем всех чтобы не перенапрягать std::atomic_thread_fence(std::memory_order_release);
			for (int i = 0; i < m_ThreadsCur; i++)
			{
				Wait_Before[i]++;
				REPORTER_THREAD_CALC(m_reporter.OUTPUT("Master\t CalcRelationsMulti WakeByAddress Wait_Before", Wait_Before[i], i);)
				WakeByAddressSingle((volatile PVOID)&Wait_Before[i]);
			}

			//Проверяем - все ли потоки пробудились и работают или уже посчитали
			do
			{
				nDone = 0;
				for (int i = 0; i < m_ThreadsCur; i++)
				{
					if (StateArray[i] != 0)
						nDone++;
					else
					{
						Wait_Before[i]++;
						REPORTER_THREAD_CALC(m_reporter.OUTPUT("Master\t CalcRelationsMulti WakeByAddress Wait_Before !!!!", Wait_Before[i], i);)
						WakeByAddressSingle((volatile PVOID)&Wait_Before[i]);
					}
				}
				if (nDone < m_ThreadsCur)
					std::this_thread::yield();
			} while (nDone < m_ThreadsCur);

			//ожидаем окончания расчета потоками
			for (int i = 0; i < m_ThreadsCur; i++)
			{
				REPORTER_THREAD_CALC(m_reporter.OUTPUT("Master\t CalcRelationsMulti Wait Result TRY ", i);)
				while(StateArray[i] != 2)
					std::this_thread::yield();

				AddResult |= m_Thr_Calc[i]->Result;
				if (m_Thr_Calc[i]->KeyPointTime < out_KeyPoint) out_KeyPoint = m_Thr_Calc[i]->KeyPointTime;

				REPORTER_THREAD_CALC(m_reporter.OUTPUT("Master\t CalcRelationsMulti Wait Result DONE", i);)
			}

			//Проталкиваем потоки из ожидания по окончании на ожидание начала расчета
			for (int i = 0; i < m_ThreadsCur; i++)
			{
				Wait_After[i]++;
				REPORTER_THREAD_CALC(m_reporter.OUTPUT("Master\t CalcRelationsMulti WakeByAddress Wait_After", Wait_After[i], i);)
				WakeByAddressSingle((volatile PVOID)&Wait_After[i]);
			}
		}
		else
		{
			AddResult = SP_STATE_ERROR_MAIN;
			REPORTER_THREAD_CALC(m_reporter.OUTPUT("!!!!!ERROR!!!ERROR!!ERROR!!ERROR!!ERROR!!ERROR!!ERROR!!ERROR!!!!!!!!!!!!!!CalcRelationsMulti CSP_Group::WaitFlagAfter <= m_ThreadsCur\n");)
		}

	}
	REPORTER_THREAD_CALC(m_reporter.OUTPUT("CalcRelationsMulti End  AddResult=", AddResult, (int)GetTickCount());)

	return AddResult;
}

void CSP_Group::InitItems(int in_Flags)
{
	SData tmpData = { 0 };

	for (int i = 0; i < GetCount(); i++)
	if (m_Array[i])
	{
		m_Array[i]->Clear(in_Flags);
		m_Array[i]->SetParticleData(0, tmpData);
	}

	m_ParticleInfoArray->Clear(in_Flags);

	STracePosInfo tmpInfo = { 0 };
	m_ParticleInfoArray->AddData(tmpInfo, DA_ADD_FLAG_UPDATE);

}

void CSP_Group::CalcRelationsInit()
{
	int m_Count = PartCntr.GetCount();

	if (m_Array && *m_Array)
		for (int i = 0; i < m_Count; i++)
		{
			SData PtrData = *(m_Array[i]->GetDataById(0));
			//PtrData.Velocity = { 0 };//???????????????????????????

			for (int j = 0; j < m_Count; j++)
				if (i != j)
				{
					SPRelation* pRelation = m_Relations.GetPrevRelation(i, j);
					SPRelation *pRelationCur = m_Relations.GetCurentRelation(i, j);
					pRelation->ViewPointData = *(m_Array[j]->GetDataById(0));

					double  LinearViewTime = GetLinearViewTime(PtrData, pRelation->ViewPointData);
					pRelation->LastViewPointTime = PtrData.TimePoint + LinearViewTime;
					if (fabs(pRelation->LastViewPointTime) >= SP_MAX_TIME_DISTANCE)
					{
						pRelation->LastViewPointTime = -10000.0;
						pRelation->SET_AS_FARAWAY(m_Array[j]->m_TypeFlags);
					}
					else
					{
						pRelation->ViewPointData.LinearMove(pRelation->LastViewPointTime, LINEARMOVECLEARFLAG_ACCEL3D| LINEARMOVECLEARFLAG_ACCELDERIV3D);
						pRelation->State = 0;

						pRelation->Distance_3D = PtrData.Position;
						pRelation->Distance_3D.MINUS(pRelation->ViewPointData.Position);
						pRelation->DistancePwr2 = pRelation->Distance_3D.LENPWR2();
						pRelation->DistancePwr1 = sqrt(pRelation->DistancePwr2);
						pRelation->DistancePwr3 = pRelation->DistancePwr1 * pRelation->DistancePwr2;

						pRelation->RelativeSpeed = SData::GetRelativeSpeed(PtrData, pRelation->ViewPointData, &pRelation->RelativeVelocity_3D);
						pRelation->RelativeSpeedAbs = fabs(pRelation->RelativeSpeed);


						pRelation->DensityAtBasePoint = Calc_RelativeDensity(m_Array[j]->m_TypeFlags, m_pCalc->DensityCalcType, pRelation->DistancePwr1, pRelation->DistancePwr2);
						pRelation->AccelerationValue = Calc_RelativeAcceleration(m_Array[j]->m_TypeFlags, m_pCalc->AccCalcType, pRelation->DensityAtBasePoint);

						double tmpAddMlpl = pRelation->AccelerationValue / pRelation->DistancePwr1;
						pRelation->Acceleration_3D = pRelation->Distance_3D;
						pRelation->Acceleration_3D.MULT(-tmpAddMlpl);
						ASSERT(fabs(pRelation->Acceleration_3D.X) < 1.0e80);
					}

					pRelation->RealDistancePwr1 = PtrData.Position.GetDistance(pRelation->ViewPointData.Position);
					pRelation->RealRelativeSpeed = SData::GetRelativeSpeed(PtrData, pRelation->ViewPointData, NULL);

					*pRelationCur = *pRelation;
				}
		}
}

int CSP_Group::CalcRelations(double in_TimePoint, SPRelation* const in_CalcRelations, SPRelation* const in_PrevRelations, int in_CalcType, double& out_KeyPoint)
{
	int AddResult = 0;
	int m_Count = GetCount();

	if (m_Array && *m_Array)
	{
		m_ParticlesState &= (GR_PS_MAXABSSPEED| GR_PS_MAXABSSPEED_ON| GR_PS_MAXABSSPEED_OFF| GR_PS_POSITION_CHANGED| GR_PS_POSITION_LOW_PREC);

		int TimeType = 2;
		if (in_TimePoint == m_Array[0]->GetCurTime()) TimeType = 1;
		else
			if (in_TimePoint == m_Array[0]->m_NextData.TimePoint) TimeType = 0;

		if ((in_CalcType & SP_CALCRELATION_NEED_UPDATE_ONLY))
			for (int i = 0; i < m_Count; i++)
				for (int j = 0, RelationPos = i; j < m_Count; j++, RelationPos += m_maxCount)
					if ((in_CalcRelations[RelationPos].State & SP_STATE_NEED_UPDATE))
					{
						j = 0;
						RelationPos = i;
						for (; j < m_Count; j++, RelationPos += m_maxCount)
							in_CalcRelations[RelationPos].State |= SP_STATE_NEED_UPDATE;
					}

		if (TimeType != 0 || m_ThreadsCur < 2 || m_Thr_Calc == nullptr || *m_Thr_Calc == nullptr)
			AddResult = CalcRelationsSingle(in_TimePoint, in_CalcRelations , in_PrevRelations, in_CalcType, TimeType, out_KeyPoint);
		else
			AddResult = CalcRelationsMulti(in_TimePoint, in_CalcRelations, in_PrevRelations, in_CalcType, out_KeyPoint);
	}

	return AddResult;
}

void CSP_Group::RepairPositions()
{
	int NeedTuneRelation;
	SP_CalcStat temp_Stat;
	temp_Stat.CLEAR();
	double delta = 1.1;

	do
	{
		NeedTuneRelation = 0;
		
		CalcRelationsInit();
		CollectStat(temp_Stat, STAT_TYPE_MIN_RELATION | STAT_TYPE_CURRENT | STAT_TYPE_SETPARTICLE);

		if (temp_Stat.Rel_Min.Phantom_Distance.MMValue <= m_pCalc->CriticalDistance)
		{
			SData* cmpData1 = GetItem(temp_Stat.Rel_Min.Phantom_Distance.idxParticle1)->GetCurData();
			SData* cmpData2 = GetItem(temp_Stat.Rel_Min.Phantom_Distance.idxParticle2)->GetCurData();
			if (cmpData1->Position != cmpData2->Position)
				cmpData1->Position.MULT(delta);

			cmpData2->Position.MULT(delta);
			NeedTuneRelation++;
			delta += 0.001;
		}
	} while (NeedTuneRelation);
}

void CSP_Group::InitPosSpeedRandom(int in_InitialSeed, SP_CalcFindSet& in_Settings)
{
	if (in_InitialSeed != -1) srand(in_InitialSeed);

	double dblHaf = RAND_MAX * 0.5;
	double dblDiv = 1.0 / (double)RAND_MAX;

	Clear();

	for (int i = 0; i < GetCount(); i++)
	{
		SData initData = { 0 };

		initData.Position.X = (in_Settings.maxPos.X - in_Settings.minPos.X) * (dblHaf - (double)rand()) * dblDiv;
		initData.Position.Y = (in_Settings.maxPos.Y - in_Settings.minPos.Y) * (dblHaf - (double)rand()) * dblDiv;
		initData.Position.Z = (in_Settings.maxPos.Z - in_Settings.minPos.Z) * (dblHaf - (double)rand()) * dblDiv;

		initData.AbsSpeed = (double)rand() * dblDiv;

		initData.Velocity.X = (in_Settings.maxSpeed.X - in_Settings.minSpeed.X) * (dblHaf - (double)rand());
		initData.Velocity.Y = (in_Settings.maxSpeed.Y - in_Settings.minSpeed.Y) * (dblHaf - (double)rand());
		initData.Velocity.Z = (in_Settings.maxSpeed.Z - in_Settings.minSpeed.Z) * (dblHaf - (double)rand());

		initData.Velocity.SET_LENGHT(initData.AbsSpeed);

		SetParticleData(i, SP_DATA_ALL, initData);
	}
}

int CSP_Group::CalcDataByRelations(int in_Type)
{
	int KeyPointFlag = SP_STATE_ERROR_MAIN;
	int m_Count = GetCount();

	SPRelation * pRelations = m_Relations.GetRelation(in_Type);
	const BeStuckParticles* pBSP = (in_Type & GR_PS_RELATION_NEEDRECALC) ? m_Relations.FillNBeStuck(in_Type, 0, m_Count) : m_Relations.GetBeStuck(in_Type);
	SData* pCalcData;

	if (m_Array && pRelations)
	{
		KeyPointFlag = 0;

		for (int i = 0; i < m_Count; i++)
			if ((pCalcData = (in_Type & GR_PS_RELATION_NEXT) ? &m_Array[i]->m_NextData : (in_Type & GR_PS_RELATION_CURRENT) ? m_Array[i]->GetCurData() : nullptr))
			{
				const BeStuckList* BSList = pBSP ? pBSP->GetBeStuckListById(i) : nullptr;
				pCalcData->Accel_2_3D = { 0 };
				pCalcData->Jerk_3_3D = { 0 };
				pCalcData->Density = 0.0;
				int tmpRet = 0;
				SPRelation* pSingleRel = pRelations + i;

				if (BSList == nullptr)
				{
					for (int j = 0; j < m_Count; j++, pSingleRel += m_maxCount)
						if (i != j)
							tmpRet |= CalcSingleDataByRelation(*pCalcData, *pSingleRel, m_pCalc);
				}
				else
				{
					ASSERT(BSList->nMasters > 0);
					RelationAndBeStuck* pSelfRel = (RelationAndBeStuck *) (pRelations + i + i * m_maxCount);

					pSelfRel->AsSelf = {0};

					for (int j = 0; j < m_Count; j++, pSingleRel += m_maxCount)
						if (i != j && !(pSingleRel->State & GR_PS_COLLISION_BESTUCK))
							tmpRet |= CalcSingleDataByRelation(pSelfRel->AsSelf.ByNormalData, *pSingleRel, m_pCalc);


					if (BSList->nMasters == 1)
					{
						pSingleRel = pRelations + m_Relations.GetRelationPos(i, BSList->List[0].IdMaster);

						SPos normal; 
						normal.SET_AS_NORMAL(pCalcData->Position, pSingleRel->ViewPointData.Position, m_pCalc->CriticalDistance);

						ASSERT((pSingleRel->State & GR_PS_COLLISION_DISTANCE) && pSingleRel->DistancePwr1 == m_pCalc->CriticalDistance);

						//!!!!!!!!!!!!!!!!!корректировка позиции и скоростина данном этапе недопустима

						//корректировка ускорения
						////?????7 ускорение равно проекции ускорения VP на нормаль (прямая [ViewPoint,Particle]) + проекция остальных на плоскость
						//??????ускорение равно ускорению VP + проекция остальных на плоскость
						pSelfRel->AsSelf.ByNormalData.Accel_2_3D.SET_AS_PROJECTION_ON_PLANE_NORMAL(normal);
						pCalcData->Accel_2_3D = pSelfRel->AsSelf.ByNormalData.Accel_2_3D;

						ASSERT(fabs(pCalcData->Accel_2_3D.X) < 1.0e80);

						SPos tmp = normal;
						tmp.SET_AS_PROJECTION_OF_TO_NORMAL(pSingleRel->ViewPointData.Accel_2_3D);
						pCalcData->Accel_2_3D.ADD(tmp);
						ASSERT(fabs(pCalcData->Accel_2_3D.X) < 1.0e80);

						pCalcData->Jerk_3_3D = pSelfRel->AsSelf.ByNormalData.Jerk_3_3D;
						pCalcData->Jerk_3_3D.SET_AS_PROJECTION_OF_TO_NORMAL(normal);
						tmp = normal;
						tmp.SET_AS_PROJECTION_OF_TO_NORMAL(pSingleRel->ViewPointData.Jerk_3_3D);
						pCalcData->Jerk_3_3D.ADD(tmp);
						//pCalcData->Jerk_3_3D = { 0 };
					}
					else if (BSList->nMasters == 2)
					{
						SPRelation* pSingleRel_0 = pRelations + m_Relations.GetRelationPos(i, BSList->List[0].IdMaster);
						SPRelation* pSingleRel_1 = pRelations + m_Relations.GetRelationPos(i, BSList->List[1].IdMaster);

						ASSERT(0);
						SPos PointVPMiddle;
						PointVPMiddle.SET_AS_MIDDLE_POINT(pSingleRel_0->ViewPointData.Position, pSingleRel_1->ViewPointData.Position);

						SPos NormalVP = pSingleRel_0->ViewPointData.Position;
						NormalVP.MINUS(pSingleRel_1->ViewPointData.Position);

						double DistanceVP_PWR2 = NormalVP.LENPWR2();
						double DistParticleToMVP = sqrt(m_pCalc->CriticalDistancePwr2 - DistanceVP_PWR2 * 0.25);

						SPos PartPprojToPlane;
						PartPprojToPlane.SET_AS_PROJECTION_ON_PLANE(pCalcData->Position, PointVPMiddle, pSingleRel_0->ViewPointData.Position);///!!!!!!!!!!!!!!!!!!   NormalVP
						PartPprojToPlane.MINUS(PointVPMiddle);
						PartPprojToPlane.SET_LENGHT(DistParticleToMVP);
						PartPprojToPlane.ADD(PointVPMiddle);

						SPos Norm0;  Norm0.SET_AS_NORMAL(pSingleRel_0->ViewPointData.Position, pCalcData->Position, m_pCalc->CriticalDistance);
						SPos Norm1;  Norm1.SET_AS_NORMAL(pSingleRel_1->ViewPointData.Position, pCalcData->Position, m_pCalc->CriticalDistance);

						//корректировка ускорения
						//ускорение равно проекции ускорения VP на нормаль (прямая [ViewPoint,Particle]) + проекция остальных на плоскость

						SPos CrossLine = Norm0;
						CrossLine.CROSS(Norm1);
						CrossLine.MULT((pCalcData->Accel_2_3D.DOT(CrossLine) / CrossLine.LENPWR2()));
						pCalcData->Accel_2_3D = CrossLine;
						ASSERT(fabs(pCalcData->Accel_2_3D.X) < 1.0e80);

						SPos tmpAccel3D = Norm0;
						tmpAccel3D.SET_AS_PROJECTION_OF_TO_NORMAL(pSingleRel_1->ViewPointData.Accel_2_3D);
						pCalcData->Accel_2_3D.ADD(tmpAccel3D);
						ASSERT(fabs(pCalcData->Accel_2_3D.X) < 1.0e80);

						tmpAccel3D = Norm1;
						tmpAccel3D.SET_AS_PROJECTION_OF_TO_NORMAL(pSingleRel_0->ViewPointData.Accel_2_3D);
						pCalcData->Accel_2_3D.ADD(tmpAccel3D);
						ASSERT(fabs(pCalcData->Accel_2_3D.X) < 1.0e80);

						CrossLine = Norm0;
						CrossLine.CROSS(Norm1);
						CrossLine.MULT((pCalcData->Jerk_3_3D.DOT(CrossLine) / CrossLine.LENPWR2()));
						pCalcData->Jerk_3_3D = CrossLine;

						SPos tmpAccelDeriv3D = Norm0;
						tmpAccelDeriv3D.SET_AS_PROJECTION_OF_TO_NORMAL(pSingleRel_1->ViewPointData.Jerk_3_3D);
						pCalcData->Jerk_3_3D.ADD(tmpAccelDeriv3D);

						tmpAccelDeriv3D = Norm1;
						tmpAccelDeriv3D.SET_AS_PROJECTION_OF_TO_NORMAL(pSingleRel_0->ViewPointData.Jerk_3_3D);
						pCalcData->Jerk_3_3D.ADD(tmpAccelDeriv3D);
						//pCalcData->Jerk_3_3D = { 0 };
					}
					else if (BSList->nMasters > 2)
					{
						ASSERT(0);
					}
				}
				KeyPointFlag |= tmpRet;
				pCalcData->Density *= m_CountDivider;
			}
	}

	return KeyPointFlag;
}

int CSP_Group::CalcSingleDataByRelation(SData& pCalcData, SPRelation& in_CalcRelations, SP_Calc* const pCalc)
{
	int ret = in_CalcRelations.State;

	if ((in_CalcRelations.State & SP_STATE_FAR_AWAY) == 0)
	{
		pCalcData.Accel_2_3D.X += in_CalcRelations.Acceleration_3D.X;
		pCalcData.Accel_2_3D.Y += in_CalcRelations.Acceleration_3D.Y;
		pCalcData.Accel_2_3D.Z += in_CalcRelations.Acceleration_3D.Z;

		ASSERT(fabs(pCalcData.Accel_2_3D.X) < 1.0e80);

		in_CalcRelations.CALC_JERK_AS_RELATIVE();
		pCalcData.Jerk_3_3D -= in_CalcRelations.Jerk_3_3D;

/*
		//Jerk_3_3D  == -2.0 * S * v / pow(S * S - 1.0, 2);
		double Jerk_3_3D;
		double tmpVal = in_CalcRelations.DistancePwr2 - m_pCalc->CriticalDistance;
			
		Jerk_3_3D  = in_CalcRelations.DistancePwr1 * in_CalcRelations.RelativeSpeed / (tmpVal * tmpVal);
		SPos tmpDeriv = in_CalcRelations.RelativeVelocity_3D;
		tmpDeriv.SET_LENGHT(Jerk_3_3D + Jerk_3_3D);
		pCalcData.Jerk_3_3D -= tmpDeriv;

		//AccelDerivDeriv3D   
		//double denom1 = pow(S * S - 1.0, 2);  // (S² - 1)²
		//double denom2 = pow(S * S - 1.0, 3);  // (S² - 1)³
		//AccelDerivDeriv3D =  -2.0 * ((v * v + S * a) / denom1 - 4.0 * S * S * v * v / denom2);

		//tmpVal = in_CalcRelations.DistancePwr2 - 1.0;
		//double denom1 = tmpVal * tmpVal;  // (S² - 1)²
		//double denom2 = denom1 * tmpVal;  // (S² - 1)³
		//double relVelocityPwr2 = in_CalcRelations.RelativeSpeed * in_CalcRelations.RelativeSpeed;
		//double Snap_4 =  (relVelocityPwr2 + in_CalcRelations.DistancePwr1 * in_CalcRelations.AccelerationValue) / denom1 - 4.0 * in_CalcRelations.DistancePwr2 * relVelocityPwr2 / denom2;
		//tmpDeriv = in_CalcRelations.RelativeVelocity_3D; // Acceleration_3D;
		//tmpDeriv.SET_LENGHT(Snap_4 + Snap_4);
		//pCalcData.Snap_4_3D -= tmpDeriv;

*/

		pCalcData.Density += in_CalcRelations.DensityAtBasePoint;
	}

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
				m_CmdArr[ret] = new char[iLen + 10];
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
	EnterCriticalSection(&m_cs);
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

				ReadCalcParam(in_Command, "POS", initData.Position, 0.0);
				ReadCalcParam(in_Command, "SPEED", initData.Velocity, 0.0);

				AddSpParticle(SP_DATA_ALL, initData, m_pCalc);
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
				ClearCalcState(M_STATE_CALC);
			else if (strstr(in_Command, "NEXT") == in_Command && in_Command[4] <= ' ')
				SetCalcState(M_STATE_DO_NEXT);
			else if (strstr(in_Command, "SAVE") == in_Command && in_Command[4] <= ' ')
			{
				in_Command += 4;
				ReplaceChar(in_Command, '\t', ' ');
				TrimChar(in_Command, ' ');

				CMD_Param_File tmpfile;

				if (m_Array && GetCount() && tmpfile.ReadParams(in_Command) && *tmpfile.FileName)
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
								for(int ParticleID = 0; ParticleID < GetCount(); ParticleID++)
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
				SetCalcState(M_STATE_DO_REPORT);
			else if (strstr(in_Command, "REDRAW") == in_Command && in_Command[6] <= ' ')
			{
				if (m_pCalc->DlgTrace)
				{
					m_pCalc->DlgTrace->ShowWindow(SW_SHOW);
					m_pCalc->DlgTrace->RedrawMe();
				}
			}
			else if (strstr(in_Command, "SET") == in_Command && in_Command[3] <= ' ')
				ReadSettingsGroup(in_Command, SGROUP_ALL, READPARAMS_NOT_USE_DEFS);
			else if (strstr(in_Command, "?") == in_Command && in_Command[1] <= ' ')
				MessageBox(m_pCalc->DlgTrace ? m_pCalc->DlgTrace->m_hWnd : GetDesktopWindow(), L"PAUSE \nCONTINUE \nDELAY msec\nADD POS.X POS.Y POS.Z SPEED.X SPEED.Y SPEED.Z\nDEL ParticleID\nSTOP \nNEXT \nREPORT", L"Commands list", MB_OK | MB_APPLMODAL);
		}
	}
	LeaveCriticalSection(&m_cs);
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
			m_CmdArr[M_CMD_MAXCOUNT - 1] = nullptr;
		}

		LeaveCriticalSection(&m_cs_Command);
	}
	return ret;
}

int CSP_Group::ReadSettingsGroup(char* in_Settings, int in_SettingsGroup, int in_UseDefs)
{
	int ret = 0; 
	
	if (m_pCalc && m_pCalcFind && in_Settings && *in_Settings)
	{
		if ((in_SettingsGroup & SGROUP_TRACE))
		{
			ret += ReadCalcParam(in_Settings, "TRACE_SPEED_ARR", m_pCalc->DrawSet.ShowSpeedFor, 0xFFFFFFFF, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "TRACE_ACC_ARR", m_pCalc->DrawSet.ShowAccFor, 0xFFFFFFFF, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "TRACE_GROUP_ARR", m_pCalc->DrawSet.ShowGroupArrows, 0xFFFFFFFF, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "TRACE_SHOWITEMS", m_pCalc->DrawSet.ShowItems, 0xFFFFFFFF, in_UseDefs);
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
			int tmpInt = 0;
			double tmpVal = 0.0;

			ret += ReadCalcParam(in_Settings, "NSTEPS", m_pCalc->nIterations, 2000000, in_UseDefs);

			ret += ReadCalcParam(in_Settings, "MAXLOOP_CALC", tmpInt, 1, in_UseDefs);
			m_pCalc->CalcCounter.SetData(m_pCalc->nIterations, tmpInt);

			ret += ReadCalcParam(in_Settings, "MAXLOOP_FIND", tmpInt, -1, in_UseDefs);
			m_pCalcFind->FindCounter.SetData(m_pCalc->nIterations, tmpInt);


			if ((tmpInt = ReadCalcParam(in_Settings, "MAXSTEP_VAL", tmpVal, 0.1, in_UseDefs))|| !(in_UseDefs & READPARAMS_NOT_USE_DEFS))
				m_pCalc->Time.SetMaxStep(tmpVal);
			ret += tmpInt;

			if ((tmpInt = ReadCalcParam(in_Settings, "MINSTEP_VAL", tmpVal, SP_PREC_TIME_MLTPL, in_UseDefs))|| !(in_UseDefs & READPARAMS_NOT_USE_DEFS))
				m_pCalc->Time.SetMinStep(tmpVal);
			ret += tmpInt;

			if ((tmpInt = ReadCalcParam(in_Settings, "BESTUCKMAXSTEP_VAL", tmpVal, SP_PREC_TIME_MLTPL, in_UseDefs))|| !(in_UseDefs & READPARAMS_NOT_USE_DEFS))
				m_pCalc->Time.SetBeStuckMaxStep(tmpVal);
			ret += tmpInt;

			if ((tmpInt = ReadCalcParam(in_Settings, "PRECISSION_MLTPL_ACCEL_VAL", tmpVal, 1.0, in_UseDefs)) || !(in_UseDefs & READPARAMS_NOT_USE_DEFS))
				m_pCalc->Time.SetPrecAccelMltpl(tmpVal);
			ret += tmpInt;

			if ((tmpInt = ReadCalcParam(in_Settings, "PRECISSION_MLTPL_DISTANCE_VAL", tmpVal, 1.0, in_UseDefs)) || !(in_UseDefs & READPARAMS_NOT_USE_DEFS))
				m_pCalc->Time.SetPrecDistanceMltpl(tmpVal);
			ret += tmpInt;

			if ((tmpInt = ReadCalcParam(in_Settings, "PRECISSION_MLTPL_SPEED_VAL", tmpVal, 1.0, in_UseDefs)) || !(in_UseDefs & READPARAMS_NOT_USE_DEFS))
				m_pCalc->Time.SetPrecSpeedMltpl(tmpVal);
			ret += tmpInt;

			if ((tmpInt = ReadCalcParam(in_Settings, "TRACEMINSTEP_VAL", tmpVal, 0.0000001, in_UseDefs)) || !(in_UseDefs & READPARAMS_NOT_USE_DEFS))
				m_pCalc->Time.SetTraceMinStep(tmpVal);
			ret += tmpInt;

			if ((tmpInt = ReadCalcParam(in_Settings, "HISTORYMINSTEP_VAL", tmpVal, 0.01, in_UseDefs)) || !(in_UseDefs & READPARAMS_NOT_USE_DEFS))
				m_pCalc->Time.SetHistoryMinStep(tmpVal);
			ret += tmpInt;

			if (m_ParticleInfoArray) delete m_ParticleInfoArray;
			m_ParticleInfoArray = new SPTraceArrayDynEx<STracePosInfo, double>(m_pCalc->nIterations, &m_pCalc->Time);
		}
		if ((in_SettingsGroup & SGROUP_MISC))
		{
			m_ThreadsMax = 0;
			ret += ReadCalcParam(in_Settings, "NTHREADS", m_ThreadsMax_Initial, 0, in_UseDefs);
			if (m_ThreadsMax_Initial < 1 || m_ThreadsMax_Initial > GR_MAX_THREADS_COUNT)
				m_ThreadsMax_Initial = 0;

			m_ThreadsMax = m_ThreadsMax_Initial;
			
			ret += ReadCalcParam(in_Settings, "OUTPUTFLAGS", m_pCalc->OutPutFlags, 0, in_UseDefs);

			if (ReadCalcParam(in_Settings, "FIND_REFRESH", m_pCalcFind->m_FIND_REFRESH, 5, in_UseDefs) > 0)
				ret++;

			if (m_pCalcFind->m_FIND_REFRESH < 100) m_pCalcFind->m_FIND_REFRESH *= 1000; // sec / frame
			else
			if (m_pCalcFind->m_FIND_REFRESH < 500) m_pCalcFind->m_FIND_REFRESH = 500; // 0.5sec / frame
			if (m_pCalcFind->m_FIND_REFRESH > 100000) m_pCalcFind->m_FIND_REFRESH = 100000;

			ret += ReadCalcParam(in_Settings, "CALC_SRANDSEED", m_pCalc->sRandSeed_Calc, 0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "FIND_SRANDSEED", m_pCalcFind->sRandSeed, -1, in_UseDefs);


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

				m_pCalc->CalcFindSet.minSpeed.CUT(nullptr, &m_pCalc->CalcFindSet.maxSpeed);
				m_pCalc->CalcFindSet.maxSpeed.CUT(&m_pCalc->CalcFindSet.minSpeed, nullptr);
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

				m_pCalcFind->Settings.minSpeed.CUT(nullptr, &m_pCalcFind->Settings.maxSpeed);
				m_pCalcFind->Settings.maxSpeed.CUT(&m_pCalcFind->Settings.minSpeed, nullptr);
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
			PartCntr.INIT();
			int TmpData;
			if (ReadCalcParam(in_Settings, "PARTICLE_COUNT_POSITIVE", TmpData, 0, in_UseDefs) > 0)
			{
				ret++;
				if (TmpData > 0 && TmpData < 5000)
					PartCntr.AddInitial(SP_TYPE_POSITIVE, TmpData);
			}
			if (ReadCalcParam(in_Settings, "PARTICLE_COUNT_NEGATIVE", TmpData, 0, in_UseDefs) > 0)
			{
				ret++;
				if (TmpData > 0 && TmpData < 5000)
					PartCntr.AddInitial(SP_TYPE_NEGATIVE, TmpData);
			}

			if (PartCntr.GetInitial().GetCount() < 2 )
				PartCntr.AddInitial(SP_TYPE_POSITIVE, 2 - PartCntr.GetInitial().GetCount());

			ret += ReadCalcParam(in_Settings, "WINDTYPE", m_pCalc->WindType, 0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "COLLISIONTYPE", m_pCalc->CollisionType, 0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "DENSITYDERIVLEVEL", m_pCalc->DensityDerivLevel, 2, in_UseDefs);
				if (m_pCalc->DensityDerivLevel > 3 || m_pCalc->DensityDerivLevel < 1) m_pCalc->DensityDerivLevel = 2;
			ret += ReadCalcParam(in_Settings, "ACCCALCTYPE", m_pCalc->AccCalcType, 0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "CLASSICTYPE", m_pCalc->ClassicType, 0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "ADDITIVETYPE", m_pCalc->AdditiveType, 0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "DENSITYCALCTYPE", m_pCalc->DensityCalcType, 0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "MAXABSSPEED", m_pCalc->maxAbsSpeed,  1.0, in_UseDefs);
			m_pCalc->maxAbsSpeedPwr2 = m_pCalc->maxAbsSpeed * m_pCalc->maxAbsSpeed;

			ret += ReadCalcParam(in_Settings, "PARENTSPACETHICKNESS", m_pCalc->ParentSpaceThickness, 0.0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "CRITICALDISTANCE", m_pCalc->CriticalDistance, 1.0, in_UseDefs);
			m_pCalc->CriticalDistancePwr2 = m_pCalc->CriticalDistance * m_pCalc->CriticalDistance;
			m_pCalc->CriticalDistancePwr3 = m_pCalc->CriticalDistance * m_pCalc->CriticalDistancePwr2;
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

