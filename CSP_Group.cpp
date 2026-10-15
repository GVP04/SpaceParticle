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
		int tmpCount = PartCntr.GetCount();
		for (int i = 0; i < tmpCount; i++)
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
		int tmpCount = PartCntr.GetCount();

		if (tmpCount < m_ThreadsMax) m_ThreadsMax = tmpCount;

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
		int tmpCount = PartCntr.GetCount();

		for (int i = 0; i < tmpCount; i++)
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
	int tmpCount = PartCntr.GetCount();

	if (m_Array)
		for (int i = 0; i < tmpCount; i++)
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
	int tmpCount = PartCntr.GetCount();
	int ret = tmpCount;
	
	if (m_Array && tmpCount + 1 < SPG_MAX_COUNT)
	{
		if (tmpCount + 1 > m_maxCount)
		{
			int newCount = m_maxCount + m_maxCount;
			if (newCount > SPG_MAX_COUNT) newCount = SPG_MAX_COUNT;

			CSpParticle ** tmp = new CSpParticle* [newCount + 10];
			CopyMemory(tmp, m_Array, tmpCount * sizeof(CSpParticle*));
			delete[] m_Array;
			m_Array = tmp;

			m_Relations.SET_COUNT(newCount);

			m_maxCount = newCount;
		}

		if ((m_Array[tmpCount] = new CSpParticle(*m_pCalc, in_Type)))
		{
			PartCntr.AddCurrent(in_Type);
			m_Array[tmpCount]->m_pCalc = pCalc;
			m_Array[tmpCount]->SetParticleData(SP_DATA_ALL, in_Ptr);
			m_Array[tmpCount]->m_TraceSettings.SET(tmpCount);
		}
		CalcCountDevider();
	}
	return PartCntr.GetCount() - ret;
}

int CSP_Group::GetMaxTraceId()
{
	int ret = 0;
	int tmpCount = PartCntr.GetCount();
	if (m_Array && tmpCount)
	{
		for(int i = 0; i < tmpCount; i++)
		{
				int tmp = m_Array[i]->GetCurPos();
				if (tmp > ret) ret = tmp;
		}
	}
	return ret;
}


int CSP_Group::RemoveSpParticle(int in_Idx)
{
	int tmpCount = PartCntr.GetCount();

	if (m_Array && in_Idx >= 0 && in_Idx < tmpCount)
	{
		int ParticleType = 0;
		if (m_Array[in_Idx])
		{
			ParticleType = m_Array[in_Idx]->m_TypeFlags;
			delete m_Array[in_Idx];
		}

		m_Array[in_Idx] = nullptr;

		int MoveCount = tmpCount - in_Idx - 1;

		if (MoveCount > 0)
		{
			CopyMemory(m_Array + in_Idx, m_Array + in_Idx + 1, MoveCount * sizeof(CSpParticle*));
			m_Relations.REMOVE_ITEM(in_Idx);
		}

		PartCntr.AddCurrent(ParticleType, -1);

		CheckThreads();
		//pBS_Corrected = m_Relations.GetRelated(GR_PS_RELATION_CORRECTED);
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

void CSP_Group::CollectStat(SP_CalcStat& out_Stat, int in_StatType)
{
	if (m_Array)
	{
		int tmpCount = PartCntr.GetCount();

		if ((in_StatType & STAT_TYPE_ALL_DATA))
		{
			if ((in_StatType & STAT_TYPE_MAX_RELATION))
			{
				out_Stat.v_Max.Velocity.Z = out_Stat.v_Max.Velocity.Y = out_Stat.v_Max.Velocity.X = -SP_MAX_TIME_DISTANCE;
				out_Stat.v_Max.Velocity_Linear.Z = out_Stat.v_Max.Velocity_Linear.Y = out_Stat.v_Max.Velocity_Linear.X = -SP_MAX_TIME_DISTANCE;
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
				out_Stat.v_Min.Velocity_Linear.Z = out_Stat.v_Min.Velocity_Linear.Y = out_Stat.v_Min.Velocity_Linear.X = SP_MAX_TIME_DISTANCE;
				out_Stat.v_Min.Position.Z = out_Stat.v_Min.Position.Y = out_Stat.v_Min.Position.X = SP_MAX_TIME_DISTANCE;
				out_Stat.v_Min.Accel_2_3D.Z = out_Stat.v_Min.Accel_2_3D.Y = out_Stat.v_Min.Accel_2_3D.X = SP_MAX_TIME_DISTANCE;
				out_Stat.v_Min.Jerk_3_3D.Z = out_Stat.v_Min.Jerk_3_3D.Y = out_Stat.v_Min.Jerk_3_3D.X = SP_MAX_TIME_DISTANCE;
				out_Stat.v_Min.AbsSpeed = 1.0;
				out_Stat.v_Min.Density = 1.0;
				out_Stat.v_Min.TimePoint = SP_MAX_TIME_DISTANCE;
			}

			for (int i = 0; i < tmpCount; i++)
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
				out_Stat.Rel_Min.RelVelocity.MMValue = 1.0e30;
			}
			if ((in_StatType & STAT_TYPE_MAX_RELATION))
			{
				ZeroMemory(&out_Stat.Rel_Max, sizeof(SMinMaxRelation));
				out_Stat.Rel_Max.Real_Distance.MMValue = -1.0e30;
				out_Stat.Rel_Max.Phantom_Distance.MMValue = -1.0e30;
				out_Stat.Rel_Max.RelVelocity.MMValue = -1.0e30;
			}

			if ((in_StatType & STAT_TYPE_SETPARTICLE))
				for (int idx1 = 0; idx1 < tmpCount; idx1++)
					m_Array[idx1]->m_CountRelated = 0;

			for (int idx1 = 0; idx1 < tmpCount; idx1++)
			{
				SPRelation* curRel = calcRelations + idx1;
				for (int idx2 = 0; idx2 < tmpCount; idx2++, curRel += m_maxCount)
					if (idx1 != idx2)
					{
						if ((curRel->State & GR_PS_COLLISION_IS_ULTIMATE) && (in_StatType & STAT_TYPE_SETPARTICLE))
								m_Array[idx1]->m_CountRelated++;

						if ((in_StatType & STAT_TYPE_MIN_RELATION))
						{
							if (out_Stat.Rel_Min.RelVelocity.MMValue > curRel->RelativeSpeedAbs)
							{
								out_Stat.Rel_Min.RelVelocity.MMValue = curRel->RelativeSpeedAbs;
								out_Stat.Rel_Min.RelVelocity.idxParticle1 = idx1;
								out_Stat.Rel_Min.RelVelocity.idxParticle2 = idx2;
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
							if (out_Stat.Rel_Max.RelVelocity.MMValue < curRel->RelativeSpeedAbs)
							{
								out_Stat.Rel_Max.RelVelocity.MMValue = curRel->RelativeSpeedAbs;
								out_Stat.Rel_Max.RelVelocity.idxParticle1 = idx1;
								out_Stat.Rel_Max.RelVelocity.idxParticle2 = idx2;
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
				SData tmpVal; tmpVal.CLEAR();

				if ((in_StatType & STAT_TYPE_MIN_RELATION))
				{
					tmpPtr = m_Array[out_Stat.Rel_Min.RelVelocity.idxParticle1]->GetCurData();
					out_Stat.Rel_Min.RelVelocity.Part1 = tmpPtr ? *tmpPtr : tmpVal;
					tmpPtr = m_Array[out_Stat.Rel_Min.RelVelocity.idxParticle2]->GetCurData();
					out_Stat.Rel_Min.RelVelocity.Part2 = tmpPtr ? *tmpPtr : tmpVal;
					out_Stat.Rel_Min.RelVelocity.relation = calcRelations[out_Stat.Rel_Min.RelVelocity.idxParticle1 + m_maxCount * out_Stat.Rel_Min.RelVelocity.idxParticle2];

					tmpPtr = m_Array[out_Stat.Rel_Min.Phantom_Distance.idxParticle1]->GetCurData();
					out_Stat.Rel_Min.Phantom_Distance.Part1 = tmpPtr ? *tmpPtr : tmpVal;
					tmpPtr = m_Array[out_Stat.Rel_Min.Phantom_Distance.idxParticle2]->GetCurData();
					out_Stat.Rel_Min.Phantom_Distance.Part2 = tmpPtr ? *tmpPtr : tmpVal;
					out_Stat.Rel_Min.Phantom_Distance.relation = calcRelations[out_Stat.Rel_Min.Phantom_Distance.idxParticle1 + m_maxCount * out_Stat.Rel_Min.Phantom_Distance.idxParticle2];
				}

				if ((in_StatType & STAT_TYPE_MAX_RELATION))
				{
					tmpPtr = m_Array[out_Stat.Rel_Max.RelVelocity.idxParticle1]->GetCurData();
					out_Stat.Rel_Max.RelVelocity.Part1 = tmpPtr ? *tmpPtr : tmpVal;
					tmpPtr = m_Array[out_Stat.Rel_Max.RelVelocity.idxParticle2]->GetCurData();
					out_Stat.Rel_Max.RelVelocity.Part2 = tmpPtr ? *tmpPtr : tmpVal;
					out_Stat.Rel_Max.RelVelocity.relation = calcRelations[out_Stat.Rel_Max.RelVelocity.idxParticle1 + m_maxCount * out_Stat.Rel_Max.RelVelocity.idxParticle2];

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
		int tmpCount = PartCntr.GetCount();

		if (m_Array && tmpCount > 0)
		{
			fprintf(in_Ptr, "\nGroup %s\tCount=\t%d\tTime=\t%g\n", m_name, tmpCount, m_pCalc->Time.GetCurTime());

			for (int i = 0; i < tmpCount; i++)
			{
				m_Array[i]->GetStateDescription(m_tmpString, SPG_TEMPSTRING_LEN, in_StateTime);
				fprintf(in_Ptr, "\t\tSParticle\t%d\t%s\n", i, m_tmpString);
			}
		}
		else
			fprintf(in_Ptr, "m_Array %p\tCSP_Group.m_Count = %d\n", m_Array, tmpCount);
	}
}

void CSP_Group::WriteState(FILE* in_Ptr, int OutputType, int in_nPoints)
{
	if (in_Ptr)
	{
		int tmpCount = PartCntr.GetCount();

		if (m_Array  && m_pCalc && m_pCalcFind && tmpCount > 0)
		{
			fprintf(in_Ptr, "\nGroup %s\tCount=\t%d\tTime=\t%10.10g\tsRandSeed=\t%d\tsStat.Count=\t%d\n", m_name, tmpCount, m_pCalc->Time.GetCurTime(), m_pCalcFind->sRandSeed, m_pCalcFind->stat.Count);
			switch (OutputType)
			{
			case 0:
				for (int i = 0; i < tmpCount; i++)
				{
					m_Array[i]->GetStateDescription(m_tmpString, SPG_TEMPSTRING_LEN, in_nPoints);
					fprintf(in_Ptr, "\tSParticle\t%d\t%s\n", i, m_tmpString);
				}
				break;
			case 1:
				for (int i = 0; i < tmpCount; i++)
				{
					m_Array[i]->GetStateDescription(m_tmpString, SPG_TEMPSTRING_LEN, 1);
					fprintf(in_Ptr, "\tSParticle\t%d\t%s\n", i, m_tmpString);

					for (int j = 0; j < tmpCount; j++)
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
			fprintf(in_Ptr, "m_Array %p\tCSP_Group.m_Count = %d\n", m_Array, tmpCount);
	}
}

void CSP_Group::SetViewPoint(double in_Percents, double in_Spread, int ArrTypes)
{
	if (in_Percents > 100.0) in_Percents = 100.0;
	if (in_Percents < 0.0) in_Percents = 100.0;
	if (in_Spread > 100.0) in_Spread = 100.0;
	if (in_Spread < 0.000001) in_Spread = 0.000001;
	

	if (m_pCalc && m_ParticleInfoArray)
	{
		STracePosInfo* PInfoStart;
		STracePosInfo* PInfoEnd;

		int idxStartSecondary = m_ParticleInfoArray->GetStartIdSecondary();
		int idxStartMain = m_ParticleInfoArray->GetStartIdMain();
		int idxEndMain = m_ParticleInfoArray->GetCurPos();

		int resIdStart, resIdEnd;

		switch ((ArrTypes & SP_DRAW_PART_ALL))
		{
		case SP_DRAW_PART_MAIN:
			resIdStart = idxStartMain;
			resIdEnd = idxEndMain;
			break;
		case SP_DRAW_PART_SECONDARY:
			resIdStart = idxStartSecondary;
			resIdEnd = idxStartMain;
			break;
		case SP_DRAW_PART_ETERNAL:
			resIdStart = 0;
			resIdEnd = idxStartSecondary;
			break;
		case SP_DRAW_PART_MS:
			resIdStart = idxStartSecondary;
			resIdEnd = idxEndMain;
			break;
		case (SP_DRAW_PART_ETERNAL| SP_DRAW_PART_SECONDARY):
			resIdStart = 0;
			resIdEnd = idxStartMain;
			break;
		case SP_DRAW_PART_ALL:
		default:
			resIdStart = 0;
			resIdEnd = idxEndMain;
			break;
			break;
		}

		if (resIdStart < 0) resIdStart = 0;
		if (resIdEnd <= 0) resIdEnd = idxEndMain;

		PInfoStart = m_ParticleInfoArray->GetDataById(resIdStart);
		PInfoEnd = m_ParticleInfoArray->GetDataById(resIdEnd);

		m_pCalc->DrawSet.ViewPoint.TimeInPc = -1.0;
		m_pCalc->DrawSet.ViewPoint.Spread = 100.0;

		if (PInfoEnd && PInfoStart) //&& (GetCalcState()& (M_STATE_READY| M_STATE_CALC | M_STATE_PAUSE | M_STATE_BREAK)) != 123456)
		{
			double vpSpread = (PInfoEnd->TimePoint - PInfoStart->TimePoint) * 0.01; // == 1%
			double vpTime = PInfoStart->TimePoint + vpSpread * in_Percents; //  vpTime +- vpSpread
			vpSpread *= in_Spread;

			int id_Start, id_End;
			// , id_Mid;
			//id_Mid = m_ParticleInfoArray->GetLowIdxByTime(vpTime);

			if (vpTime + vpSpread > PInfoEnd->TimePoint) id_End = m_ParticleInfoArray->GetLowIdxByTime(PInfoEnd->TimePoint);
			else	id_End = m_ParticleInfoArray->GetLowIdxByTime(vpTime + vpSpread);

			if (vpTime - vpSpread <= PInfoStart->TimePoint) id_Start = m_ParticleInfoArray->GetLowIdxByTime(PInfoStart->TimePoint);
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


void CSP_Group::GetMinMaxTracePos(SPos& min_Pos, SPos& max_Pos, const ViewPointData& VPData)
{
	min_Pos.X = min_Pos.Y = min_Pos.Z = 1.0e30;
	max_Pos.X = max_Pos.Y = max_Pos.Z = -1.0e30;
	int tmpCount = PartCntr.GetCount();

	for (int i = 0; i < tmpCount; i++) 
		m_Array[i]->GetMinMaxTracePos(min_Pos, max_Pos, VPData.StartIdx, VPData.EndIdx);
}

int CSP_Group::GetIdByMaxDistance(double in_RelDist)
{
	int ret = -1;
	int tmpCount = PartCntr.GetCount();

	if (m_Array && tmpCount > 2 && m_Array[0] && m_Array[1])
	{
		for (int i = 0; i < tmpCount && ret < 0; i++)
		{
			int j;
			for (j = 0; j < tmpCount && (i == j || m_Relations.GetCurentRelation( i, j)->DistancePwr1 > in_RelDist); j++);

			if (j >= tmpCount) 
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
			int tmpCount = PartCntr.GetCount();
			SData tmpData;
			for (int i = 0; i < tmpCount; i++)
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
	int tmpCount = PartCntr.GetCount();
	if (m_pCalc->Time.GetCurTime() > 0.0)
		for (i = 0; i < tmpCount; i++)
		{
			const SData *cpCurdata = m_Array[i]->GetCurData_C();
			const SData* cpPrevData = m_Array[i]->GetPrevData_C();

			if (cpCurdata->AbsSpeed >= m_pCalc->Critical.Speed)
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
	int tmpCount = PartCntr.GetCount();


	for (int i = 1; i < tmpCount; i++)
	{
		if (m_Array[i]->GetCurTime() != m_Array[0]->GetCurTime())
			ASSERT(0);
	}

	for (int i = 1; i < tmpCount; i++)
	{
		if (m_Array[i]->GetNextData().TimePoint != m_Array[0]->GetNextData().TimePoint)
			ASSERT(0);
	}

	return ret;
}

void CSP_Group::SetRelationByUltimate(SData& NextData, const SData& CurData, RelatedInfo& R_Info)
{
	//расчет relation для каждого из nMastersCur (цикл снаружи)
	{
		R_Info.RelationNext->Distance_3D.SET_AS_DELTA(NextData.Position, R_Info.RelationNext->ViewPointData.Position);

		R_Info.RelationNext->DistancePwr2 = R_Info.RelationNext->Distance_3D.LENPWR2();
		R_Info.RelationNext->DistancePwr1 = sqrt(R_Info.RelationNext->DistancePwr2);

		R_Info.RelationNext->RelativeSpeed = SData::GetRelativeSpeedFromVelocity_Linear(R_Info.RelationNext->ViewPointData, NextData, &R_Info.RelationNext->RelativeVelocity_3D);
		R_Info.RelationNext->RelativeSpeedAbs = fabs(R_Info.RelationNext->RelativeSpeed);
		R_Info.RelationNext->Wind_3D = { 0 };
		R_Info.RelationNext->LastViewPointTime = R_Info.RelationCurent->ViewPointData.TimePoint;

		R_Info.RelationNext->RealDistancePwr1 = CurData.Position.GetDistance(R_Info.RelationNext->ViewParticlePtr->GetCurPosition());
		R_Info.RelationNext->RealRelativeSpeed = SData::GetRelativeSpeedFromVelocity_Linear(CurData, *R_Info.RelationNext->ViewParticlePtr->GetCurData_C(), NULL);


		if ((R_Info.RelationNext->State & GR_PS_COLLISION_IS_ULTIMATE))
		{
			ASSERT(R_Info.RelationNext->DistancePwr1 >= m_pCalc->Critical.DistanceMinusEps -1.0e-12 && R_Info.RelationNext->DistancePwr1 <= m_pCalc->Critical.DistancePlusEps + 1.0e-12);
			R_Info.RelationNext->DensityAtBasePoint = 0.0;
			R_Info.RelationNext->DistancePwr1 = m_pCalc->Critical.Distance;
			R_Info.RelationNext->DistancePwr2 = m_pCalc->Critical.DistancePwr2;
			R_Info.RelationNext->DistancePwr3 = m_pCalc->Critical.DistancePwr3;
		}
		else
		{
			R_Info.RelationNext->State &= ~GR_PS_COLLISION_PRESENT;

			if (R_Info.RelationNext->DistancePwr1 < m_pCalc->Critical.Distance)
			{
				R_Info.RelationNext->State |= (GR_PS_COLLISION_DISTANCE | GR_PS_COLLISION_IS_INSIDE);
				R_Info.RelationNext->State |= (R_Info.RelationCurent->State & (GR_PS_COLLISION_BESTUCK | GR_PS_COLLISION_EXPELLED));//просто копирование предыдущего типа 
			}
			/////////////РАСЧЕТ ПЛОТНОСТИ
			R_Info.RelationNext->Calc_RelativeDensity(m_pCalc->DensityCalcType);
			//расчет ускорения
			R_Info.RelationNext->Calc_RelativeAcceleration(m_pCalc->AccCalcType);

			////switch (m_pCalc->WindType) // Fantasy !!!!!
		}
		R_Info.RelationNext->DistancePwr3 = R_Info.RelationNext->DistancePwr1 * R_Info.RelationNext->DistancePwr2;
	}

}


int CSP_Group::ComputeParticleDataAtPhantomBarrier(SData& out_Data, int in_IdParticle, double in_NextTime, double* const out_KeyPoint)
{
	int ret = 0;

	RelatedParticles* pBSP;
	const RelatedList* pBSList;
	int tmpCount = PartCntr.GetCount();
	out_Data.CLEAR();
	out_Data.TimePoint = in_NextTime;
	
	if (in_IdParticle < tmpCount && (pBSP = m_Relations.GetRelated_Current()) && (pBSList = pBSP->GetRelatedListById(in_IdParticle)))
	{
		const double CriticalSpeedPlusEps = m_pCalc->Critical.SpeedPlusEps;
		const SData* pCurData = m_Array[in_IdParticle]->GetCurData_C();

		double DeltaT = in_NextTime - pCurData->TimePoint;
		double DeltaTPwr2 = DeltaT * DeltaT;
		double TimeOfView = in_NextTime - m_pCalc->Critical.Distance;
		int nMastersCur = pBSList->nMasters;
		ASSERT(nMastersCur > 0);
		if (in_IdParticle < 0)
			ASSERT(0);
		
		if (nMastersCur == 1) //раздельно значительно иыстрее 	//пока предположительно не может быть единственного хозяина AP
		{//////////(nMastersCur == 1)///////////////////////////////////////////////////////////////////////////////////////////
			RelatedInfo* R_Info = pBSList->List;
			SPRelation* pRelationNext = R_Info->RelationNext;
			SPRelation* pRelationCurent = R_Info->RelationCurent;

			pRelationNext->State = (R_Info->State | GR_PS_RELATION_NONEEDRECALC | GR_PS_COLLISION_IS_ULTIMATE);
			out_Data.State |= pRelationNext->State;

			// Новое положение центра сферы фантома
			pRelationNext->ViewParticlePtr = m_Array[R_Info->IdMaster];
			pRelationNext->ViewParticlePtr->GetPrevDataByTimeLinear(pRelationNext->ViewPointData, pRelationNext->LowIdxOfViewPont_Near, pRelationCurent->LowIdxOfViewPont_Near, TimeOfView);
			pRelationNext->LastViewPointTime = pRelationCurent->ViewPointData.TimePoint;

			if ((R_Info->State & GR_PS_COLLISION_EXPELLED))
			{ //AP
				out_Data.Velocity.SET_AS_DELTA(pCurData->Position, pRelationCurent->ViewPointData.Position);

				//Velocity_Linear == 0 ????????
				if (out_Data.Velocity.DOT(pRelationCurent->ViewPointData.Velocity_Linear) < 0) out_Data.Velocity.INVERT();

				out_Data.Velocity.SET_LENGHT(m_pCalc->Critical.Speed);

				out_Data.Accel_2_3D = { 0 };
				out_Data.Jerk_3_3D = { 0 };
				out_Data.AbsSpeed = 1.0;
				out_Data.Velocity_Linear = out_Data.Velocity;
				out_Data.Position.SET_AS_IntegralOf(pCurData->Position, out_Data.Velocity_Linear, DeltaT);

				if (!m_pCalc->Time.IsMinimalStep())
					*out_KeyPoint = pCurData->TimePoint + m_pCalc->Time.GetMinStep();

				pRelationNext->State &= ~GR_PS_COLLISION_IS_ULTIMATE;//исключение из хозяев 
				out_Data.TimePoint = in_NextTime;

				SetRelationByUltimate(out_Data, *pCurData, *R_Info);
				out_Data.State |= pRelationNext->State;
			}
			else
			{//NP
				//предварительное перемещение только за счет мастера
				compute_1_Master_LinearMovement(pRelationCurent->ViewPointData, *pCurData, out_Data.Position, out_Data.Velocity, DeltaT);		//перемещение и скорость от мастера

				double MoveLenPwr2 = out_Data.Position.LENPWR2();

				if (MoveLenPwr2 < DeltaTPwr2)
				{//ниже предельной. Должно быть учтено влияние иных
					SPos FrgnAcc = ((RelationAndRelated*)m_Relations.GetCurentRelation(in_IdParticle, in_IdParticle))->AsSelf.ByNormalData.Accel_2_3D;
					FrgnAcc.SET_AS_PROJECTION_ONTO_PLANE(out_Data.Velocity);
					SPos FrgnVelocity = pCurData->Velocity_Linear;
					FrgnVelocity.SET_AS_PROJECTION_ONTO_PLANE(out_Data.Velocity);

					//с учетом СТО итоговый вектор нормируется к 1 при неизменном векторе от мастера
					SPos FrgnNewPos = { 0 };
					FrgnNewPos.ADDIntegralStep(FrgnVelocity, FrgnAcc, DeltaT);

					SPos tmp; tmp.SET_AS_SUMM(out_Data.Position, FrgnNewPos);
					if (tmp.LENPWR2() > DeltaTPwr2)
						FrgnNewPos.SET_LENGHT(sqrt(DeltaTPwr2 - MoveLenPwr2));

					out_Data.Position += FrgnNewPos;
					//это конечно усредненно и с опозданием на полшага
					out_Data.Velocity.SET_AS_MULT(out_Data.Position, 1.0 / DeltaT);
					out_Data.Accel_2_3D = { 0 };
				}

				out_Data.Position += pCurData->Position;
				///позицию жестко на критическом расстоянии 
				out_Data.Position -= pRelationNext->ViewPointData.Position;
				//установка вектора дистанции
				out_Data.Position.SET_LENGHT_TO_1();
				//			pRelationNext->Distance_3D = out_Data.Position;

				out_Data.Position += pRelationNext->ViewPointData.Position;

				out_Data.AbsSpeed = out_Data.Velocity.LEN();
				if (out_Data.AbsSpeed > m_pCalc->Critical.Speed)
				{
					out_Data.AbsSpeed = m_pCalc->Critical.Speed;
					out_Data.Velocity.SET_LENGHT_TO_1();
				}
				out_Data.Velocity_Linear = out_Data.Velocity;

				SetRelationByUltimate(out_Data, *pCurData, pBSList->List[0]);
				out_Data.State |= pRelationNext->State;
			}
		}//////////////(nMastersCur == 1)///////////////////////////////////////////////////////////////////////////////////////
		else if (nMastersCur == 2) 
		{//////////////(nMastersCur == 2)///////////////////////////////////////////////////////////////////////////////////////
			double tmp_KP = in_NextTime;
			double* const out_KeyPointTmp = out_KeyPoint ? out_KeyPoint : &tmp_KP;
			RelatedInfo* pR_Info0 = &pBSList->List[0];
			RelatedInfo* pR_Info1 = &pBSList->List[1];

			int typeOfCombination = 0;
			if ((pR_Info0->State & GR_PS_COLLISION_EXPELLED))
			{
				typeOfCombination++;
				if ((pR_Info1->State & GR_PS_COLLISION_EXPELLED)) typeOfCombination++;
				else
				{ //Если есть AP, то pR_Info1 всегда AP
					pR_Info0 = &pBSList->List[1];
					pR_Info1 = &pBSList->List[0];
				}
			}
			else
				if ((pR_Info1->State & GR_PS_COLLISION_EXPELLED)) typeOfCombination++;

			SPRelation* pRelationCurent0 = pR_Info0->RelationCurent;
			SPRelation* pRelationCurent1 = pR_Info1->RelationCurent;
			SPRelation* pRelationNext0 = pR_Info0->RelationNext;
			SPRelation* pRelationNext1 = pR_Info1->RelationNext;

			pRelationNext0->State = (pR_Info0->State | GR_PS_RELATION_NONEEDRECALC | GR_PS_COLLISION_IS_ULTIMATE);
			pRelationNext1->State = (pR_Info1->State | GR_PS_RELATION_NONEEDRECALC | GR_PS_COLLISION_IS_ULTIMATE);

			out_Data.State |= pRelationNext0->State;

			// Новое положение центра сферы фантома 0
			pRelationNext0->ViewParticlePtr = m_Array[pR_Info0->IdMaster];
			pRelationNext0->ViewParticlePtr->GetPrevDataByTimeLinear(pRelationNext0->ViewPointData, pRelationNext0->LowIdxOfViewPont_Near, pRelationCurent0->LowIdxOfViewPont_Near, TimeOfView);
			pRelationNext0->LastViewPointTime = pRelationCurent0->ViewPointData.TimePoint;

			// Новое положение центра сферы фантома 1
			pRelationNext1->ViewParticlePtr = m_Array[pR_Info1->IdMaster];
			pRelationNext1->ViewParticlePtr->GetPrevDataByTimeLinear(pRelationNext1->ViewPointData, pRelationNext1->LowIdxOfViewPont_Near, pRelationCurent1->LowIdxOfViewPont_Near, TimeOfView);
			pRelationNext1->LastViewPointTime = pRelationCurent1->ViewPointData.TimePoint;

			int ComputeResult;

			switch (typeOfCombination)
			{
			case 0://2NP
				ComputeResult = compute_2NP_master_LinearMove_R1(pRelationCurent0->ViewPointData, pRelationCurent1->ViewPointData, pRelationNext0->ViewPointData, pRelationNext1->ViewPointData, *pCurData, out_Data, DeltaT);
				if (out_Data.AbsSpeed < m_pCalc->Critical.SpeedMinusEps)
				{//учет стремлений от не хозяев. Для двух хозяев одна степень свободы. Вырожденные случаи с двумя степенями свободы можно игнорировать
					// Строим два вектора в плоскости
					SPos Movement; Movement.SET_AS_DELTA(out_Data.Position, pCurData->Position);
					double MoveLenPwr2 = Movement.LENPWR2();
					if (MoveLenPwr2 < DeltaTPwr2)
					{
						SPos AB; AB.SET_AS_DELTA(pRelationNext0->ViewPointData.Position, out_Data.Position);
						SPos MoveLine; MoveLine.SET_AS_DELTA(pRelationNext1->ViewPointData.Position, out_Data.Position);
						MoveLine.CROSS(AB);


						SPos FrgnAcc = ((RelationAndRelated*)m_Relations.GetCurentRelation(in_IdParticle, in_IdParticle))->AsSelf.ByNormalData.Accel_2_3D;
						FrgnAcc.SET_AS_PROJECTION_ONTO_LINE(MoveLine);
						SPos FrgnVelocity = pCurData->Velocity_Linear;
						FrgnVelocity.SET_AS_PROJECTION_ONTO_LINE(MoveLine);

						//с учетом СТО итоговый вектор нормируется к 1 при неизменном векторе от мастера
						SPos FrgnNewPos = { 0 };
						FrgnNewPos.ADDIntegralStep(FrgnVelocity, FrgnAcc, DeltaT);

						SPos tmp; tmp.SET_AS_SUMM(Movement, FrgnNewPos);
						if (tmp.LENPWR2() > DeltaTPwr2)
							FrgnNewPos.SET_LENGHT(sqrt(DeltaTPwr2 - Movement.LENPWR2()));

						out_Data.Position += FrgnNewPos;
						MoveToClosestEequiDistantPoint_R1_FAST(pRelationNext0->ViewPointData.Position, pRelationNext1->ViewPointData.Position, out_Data.Position);

						Movement.SET_AS_DELTA(out_Data.Position, pCurData->Position);

						//это конечно усредненно и с опозданием на полшага
						out_Data.Velocity.SET_AS_MULT(Movement, 1.0 / DeltaT);
						out_Data.Velocity_Linear = out_Data.Velocity;
						out_Data.Accel_2_3D = { 0 };
						out_Data.AbsSpeed = out_Data.Velocity.LEN();
					}
				}
				break;
			case 1://NP+AP
				ComputeResult = compute_NPAP_master_LinearMove(pRelationCurent0->ViewPointData, pRelationNext0->ViewPointData, pRelationCurent1->ViewPointData, *pCurData, out_Data, DeltaT);
				break;
			case 2://2AP
				ComputeResult = compute_2AP_master_Motion(pRelationCurent0->ViewPointData, pRelationCurent1->ViewPointData, *pCurData, out_Data, DeltaT);
				if (out_Data.AbsSpeed == 0.0)
				{
					//настолько невероятно, что не имеет смысла реализовывать учет внешнего стремления
					;
				}
				break;
			}

			if ((ComputeResult & 0x11))
			{
				if (!m_pCalc->Time.IsMinimalStep())
					*out_KeyPointTmp = pCurData->TimePoint + m_pCalc->Time.GetMinStep();

				if ((ComputeResult & 0x01))
					pRelationNext0->State &= ~GR_PS_COLLISION_IS_ULTIMATE;//исключение из хозяев 
				if ((ComputeResult & 0x10))
					pRelationNext1->State &= ~GR_PS_COLLISION_IS_ULTIMATE;//исключение из хозяев 
			}

			if (out_Data.AbsSpeed > m_pCalc->Critical.Speed)
			{
				if (out_Data.AbsSpeed < CriticalSpeedPlusEps)
					out_Data.AbsSpeed = m_pCalc->Critical.Speed;
				else
				{
					//ASSERT(0);
					out_Data.AbsSpeed = m_pCalc->Critical.Speed;
					out_Data.Velocity.NORMALIZE();
				}
				out_Data.Velocity_Linear = out_Data.Velocity;
			}

			out_Data.TimePoint = in_NextTime;

			SetRelationByUltimate(out_Data, *pCurData, *pR_Info0);
			out_Data.State |= pRelationNext0->State;

			SetRelationByUltimate(out_Data, *pCurData, *pR_Info1);
			out_Data.State |= pRelationNext1->State;

		}
		else
		{//////////////(nMastersCur > 2)///////////////////////////////////////////////////////////////////////////////////////
			ASSERT(nMastersCur < 3);
			double SpedOnDirection[10];
			double tmp_KP = in_NextTime;
			double* const out_KeyPointTmp = out_KeyPoint ? out_KeyPoint : &tmp_KP;
			SPos DIrectionStartVector = { 0 };
			int DoBy0 = 0;

			//определение начальных условий
			for (int i = 0; i < nMastersCur; i++)
			{
				RelatedInfo& R_Info = pBSList->List[i];
				SPRelation* pRelationNext = R_Info.RelationNext;
				SPRelation* pRelationCurent = R_Info.RelationCurent;

				// Новые положения центра сферы фантомов
				pRelationNext->ViewParticlePtr = m_Array[R_Info.IdMaster];
				pRelationNext->ViewParticlePtr->GetPrevDataByTimeLinear(pRelationNext->ViewPointData, pRelationNext->LowIdxOfViewPont_Near, pRelationCurent->LowIdxOfViewPont_Near, TimeOfView);

				//временное использование переменных для данных по направлению и величине относительной скорости
				pRelationNext->RelativeSpeed = SData::GetRelativeSpeedFromVelocity_Linear(pRelationCurent->ViewPointData, pCurData->Position, &pRelationNext->RelativeVelocity_3D);
				pRelationNext->Acceleration_3D = { 0 };
				pRelationNext->Jerk_3_3D = { 0 };
				pRelationNext->Wind_3D.SET_AS_DELTA(pCurData->Position, pRelationCurent->ViewPointData.Position); //направление от VP к частице
				pRelationNext->State = R_Info.State | GR_PS_RELATION_NONEEDRECALC | GR_PS_COLLISION_IS_ULTIMATE;

				//определение направления движения
				if (R_Info.RelationNext->RelativeSpeed > 1.0e-14)//убегает !!!!!!!!!!! заменить 0 на минимальное значение???
				{
					if ((R_Info.State & GR_PS_COLLISION_EXPELLED) == 0)//притягивает
						DIrectionStartVector += R_Info.RelationNext->Wind_3D;
					else //отталкивает
						DIrectionStartVector -= R_Info.RelationNext->Wind_3D;
				}
				else if (R_Info.RelationNext->RelativeSpeed < -1.0e-14)//приближается 
				{
					if ((R_Info.State & GR_PS_COLLISION_EXPELLED) == 0)	//отталкивает
						DIrectionStartVector -= R_Info.RelationNext->Wind_3D;
					else //притягивает
						DIrectionStartVector += R_Info.RelationNext->Wind_3D;
				}
				else DoBy0++;
			}

			if (DoBy0)//сопротивление движению. гасит вектор по проекции
				for (int i = 0; i < nMastersCur; i++)
				{
					ASSERT(0);
					RelatedInfo& R_Info = pBSList->List[i];
					if (R_Info.RelationNext->RelativeSpeed <= 1.0e-14 && R_Info.RelationNext->RelativeSpeed >= -1.0e-14)
					{
						SPos sss = DIrectionStartVector;
						sss.SET_AS_PROJECTION_OF(R_Info.RelationNext->Wind_3D);
						double direction = sss.DOT(DIrectionStartVector);
						double dirLen = DIrectionStartVector.LEN();
						double sssLen = sss.LEN();

						if (direction > 0.0)
						{//убегает 
							if ((R_Info.State & GR_PS_COLLISION_EXPELLED))//AP
								DIrectionStartVector += sss;
							else
								if (sssLen >= dirLen) DIrectionStartVector = { 0 };
								else DIrectionStartVector -= sss;
						}
						else if (direction < 0.0)
						{//приближается 
							if ((pBSList->List[i].State & GR_PS_COLLISION_EXPELLED) == 0)//NP
								DIrectionStartVector += sss;
							else
								if (sssLen >= dirLen) DIrectionStartVector = { 0 };
								else DIrectionStartVector -= sss;
						}
					}
				}

			//определение начальной скорости движения
			double maxSpeed = 0.0;
			int nMoveMasters = 0;

			for (int i = 0; i < nMastersCur; i++)
			{
				double curSpeed = 0;// SpeedByPhantom(pBSList->List[i].RelationCurent->ViewPointData, pCurData->Position, DIrectionStartVector, nullptr);

				if (curSpeed > -1.0e-14)
				{
					if (curSpeed < 1.0e-14) SpedOnDirection[i] = 0.0;
					else
					{
						nMoveMasters++;
						if (curSpeed > maxSpeed) maxSpeed = curSpeed;
						SpedOnDirection[i] = curSpeed;
					}
				}
				else
					SpedOnDirection[i] = 0.0;
			}


			int nSize = 2;

			for (int i = 0; i < nMastersCur; i++)
			{
				const double curSpeed = SpedOnDirection[i];
				if (curSpeed > 1.0e-14)
				{
					if (curSpeed > CriticalSpeedPlusEps)
					{
						pBSList->List[i].RelationNext->State &= ~GR_PS_COLLISION_IS_ULTIMATE;//исключение из хозяев
					}
				}
				else if (curSpeed < -1.0e-14)
				{
					pBSList->List[i].RelationNext->State &= ~GR_PS_COLLISION_IS_ULTIMATE;//исключение из хозяев
				}
			}


			if (maxSpeed > m_pCalc->Critical.SpeedMinusEps)
				maxSpeed = m_pCalc->Critical.Speed;


			out_Data.Position = DIrectionStartVector;
			double moveLenghtTmp = maxSpeed * DeltaT;
			out_Data.Position.SET_LENGHT(moveLenghtTmp);

			if (maxSpeed < m_pCalc->Critical.Speed && tmpCount > nMastersCur + 1)
			{//учет стремлений от не хозяев
				RelationAndRelated* SelfRel = (RelationAndRelated*)m_Relations.GetCurentRelation(in_IdParticle, in_IdParticle);
				SData ByNormal;
				ByNormal.CLEAR();
				ByNormal.Accel_2_3D = SelfRel->AsSelf.ByNormalData.Accel_2_3D;
				ASSERT(fabs(ByNormal.Accel_2_3D.X) < SP_MAX_TIME_DISTANCE);

				SData SelfMove;
				CSpParticle::ItgrlFull(SelfMove, ByNormal, DeltaT, *m_pCalc, nullptr, ITGL_CALC_USELINEAR);
				if (SelfMove.Position.LEN() > moveLenghtTmp * 1.0e-14)
				{

					//определение количества степеней свободы
					//pBSList->List[nn].RelationNext->Wind_3D  >> //направление от VP к частице
					SPos normFree = pBSList->List[0].RelationNext->Wind_3D;

					if (nMastersCur == 2)
					{
						if (!pBSList->List[0].RelationNext->Wind_3D.IsCollinear(pBSList->List[1].RelationNext->Wind_3D))
						{// не на одной прямой
							nSize = 1;
							normFree.SET_AS_CROSS(pBSList->List[0].RelationNext->Wind_3D, pBSList->List[1].RelationNext->Wind_3D);
						}
					}
					else if (nMastersCur > 2)
					{
						int i;
						for (i = 1; i < nMastersCur && pBSList->List[i].RelationNext->Wind_3D.IsCollinear(pBSList->List[i - 1].RelationNext->Wind_3D); i++);

						if (i < nMastersCur)
						{//не все коллинеарны
							nSize = 1;
							normFree.CROSS(pBSList->List[i].RelationNext->Wind_3D);

							while (++i < nMastersCur && normFree.GETCROSS(pBSList->List[i].RelationNext->Wind_3D).IsZero());

							if (i < nMastersCur)
								nSize = 0;
						}
					}

					if (nSize)
					{
						SPos SelfMovePos;
						if (nSize == 1)
						{
							SelfMovePos = normFree;
							SelfMovePos.SET_AS_PROJECTION_OF(SelfMove.Position);
						}
						else
						{
							SelfMovePos = SelfMove.Position;
							SelfMovePos.SET_AS_PROJECTION_ONTO_PLANE(normFree);
						}

						double maxSelfPwr2 = out_Data.Position.LENPWR2() + m_pCalc->Critical.SpeedPwr2 * DeltaT * DeltaT;

						if (SelfMovePos.LENPWR2() > maxSelfPwr2)
							SelfMovePos.SET_LENGHT(sqrt(maxSelfPwr2));

						out_Data.Position += SelfMovePos;
						ASSERT(out_Data.Position.LENPWR2() <= m_pCalc->Critical.SpeedPwr2);
					}
				}
			}

			out_Data.Velocity = out_Data.Position;
			out_Data.Velocity /= DeltaT;
			out_Data.AbsSpeed = out_Data.Velocity.LEN();

			if (out_Data.AbsSpeed > m_pCalc->Critical.Speed)
			{
				if (out_Data.AbsSpeed < CriticalSpeedPlusEps)
					out_Data.AbsSpeed = m_pCalc->Critical.Speed;
				else
				{
					ASSERT(0);
					out_Data.AbsSpeed = m_pCalc->Critical.Speed;
				}
				out_Data.Velocity.SET_LENGHT(m_pCalc->Critical.Speed);
			}

			out_Data.Velocity_Linear = out_Data.Velocity;


			out_Data.Position += pCurData->Position;
			out_Data.TimePoint = in_NextTime;

			//расчет relation для каждого из nMastersCur
			for (int i = 0; i < nMastersCur; i++)
			{
				RelatedInfo& R_Info = pBSList->List[i];

				R_Info.RelationNext->Distance_3D.SET_AS_DELTA(out_Data.Position, R_Info.RelationNext->ViewPointData.Position);


				///!!!!!!подумать как иначе уточнить позицию
				if ((R_Info.RelationNext->State & GR_PS_COLLISION_IS_ULTIMATE))
				{
					if (nMastersCur > 1)
					{
						if (fabs(R_Info.RelationNext->DistancePwr1 - m_pCalc->Critical.Distance) > m_pCalc->Time.GetPrecDistance())
							R_Info.RelationNext->State ^= GR_PS_COLLISION_IS_ULTIMATE;
					}
					else
					{///!!!!!!подумать как иначе уточнить позицию
						out_Data.Position -= R_Info.RelationNext->ViewPointData.Position;
						out_Data.Position.SET_LENGHT(m_pCalc->Critical.Distance);
						out_Data.Position += R_Info.RelationNext->ViewPointData.Position;
						R_Info.RelationNext->Distance_3D.SET_AS_DELTA(out_Data.Position, R_Info.RelationNext->ViewPointData.Position);
						R_Info.RelationNext->DistancePwr1 = m_pCalc->Critical.Distance;
						R_Info.RelationNext->DistancePwr2 = m_pCalc->Critical.DistancePwr2;
					}
				}
				else
				{
					R_Info.RelationNext->DistancePwr2 = R_Info.RelationNext->Distance_3D.LENPWR2();
					R_Info.RelationNext->DistancePwr1 = sqrt(R_Info.RelationNext->DistancePwr2);
				}

				R_Info.RelationNext->RelativeSpeed = SData::GetRelativeSpeedFromVelocity_Linear(R_Info.RelationNext->ViewPointData, out_Data, &R_Info.RelationNext->RelativeVelocity_3D);
				R_Info.RelationNext->RelativeSpeedAbs = fabs(R_Info.RelationNext->RelativeSpeed);
				R_Info.RelationNext->Wind_3D = { 0 };
				R_Info.RelationNext->LastViewPointTime = R_Info.RelationCurent->ViewPointData.TimePoint;

				R_Info.RelationNext->RealDistancePwr1 = pCurData->Position.GetDistance(R_Info.RelationNext->ViewParticlePtr->GetCurPosition());
				R_Info.RelationNext->RealRelativeSpeed = SData::GetRelativeSpeedFromVelocity_Linear(*pCurData, *R_Info.RelationNext->ViewParticlePtr->GetCurData_C(), NULL);


				if ((R_Info.RelationNext->State & GR_PS_COLLISION_IS_ULTIMATE))
				{
					ASSERT(R_Info.RelationNext->DistancePwr1 == m_pCalc->Critical.Distance);
					R_Info.RelationNext->DensityAtBasePoint = 0.0;
					out_Data.Accel_2_3D = { 0 };
					out_Data.Jerk_3_3D = { 0 };
					out_Data.Density = 0;
				}
				else
				{
					R_Info.RelationNext->State &= (0xFFFFFFFF ^ GR_PS_COLLISION_PRESENT);

					if (R_Info.RelationNext->DistancePwr1 < m_pCalc->Critical.Distance)
					{
						R_Info.RelationNext->State |= (GR_PS_COLLISION_DISTANCE | GR_PS_COLLISION_IS_INSIDE);
						R_Info.RelationNext->State |= (R_Info.RelationCurent->State & (GR_PS_COLLISION_BESTUCK | GR_PS_COLLISION_EXPELLED));
					}
					*out_KeyPointTmp = pCurData->TimePoint + m_pCalc->Time.GetMinStep();
					/////////////РАСЧЕТ ПЛОТНОСТИ
					R_Info.RelationNext->Calc_RelativeDensity(m_pCalc->DensityCalcType);
					//расчет ускорения
					R_Info.RelationNext->Calc_RelativeAcceleration(m_pCalc->AccCalcType);

					////switch (m_pCalc->WindType) // Fantasy !!!!!
				}
				R_Info.RelationNext->DistancePwr3 = R_Info.RelationNext->DistancePwr1 * R_Info.RelationNext->DistancePwr2;
				out_Data.State |= R_Info.RelationNext->State;
			}
		}
	}
	return ret;
}

int CSP_Group::NextStep()
{
	int ret = SP_STATE_ERROR_MAIN;
	int tmpCount = PartCntr.GetCount();

	if (m_Array && tmpCount > 1 && m_Array[0] && m_Array[1])
	{
		int prev_ParticlesState = m_ParticlesState;
		m_pCalc->Time.PrepareNextStep(m_pCalc->Stat);

		if ((prev_ParticlesState & GR_PS_COLLISION_IS_ULTIMATE))
			m_pCalc->Time.CheckByRelatedStep();

		m_Relations.FillNRelated(GR_PS_RELATION_CURRENT, 0, tmpCount, GR_PS_COLLISION_IS_ULTIMATE);
		if (0)
			ASSERT(0);

		if ((prev_ParticlesState & GR_PS_MAXABSSPEED) != GR_PS_NORM &&  m_pCalc->Time.GetStep() > m_pCalc->Time.GetMinStep() + m_pCalc->Time.GetMinStep())
		{
			double deltaT = CalcStepUpLimit();
			if (m_pCalc->Time.GetStep() > deltaT)
				m_pCalc->Time.SetStep(deltaT);
		}

		int doByKeyPoint;
		double KeyPoint, NextTime;

		const RelatedParticles* pBSP = m_Relations.GetRelated_Current();

		do
		{
			m_pCalc->Time.DelFlag(SPTIME_NEXT_STEP_MIN);

			do	//расчет новой позиции
			{
				m_Relations.ClearNextState(0, tmpCount - 1, SP_STATE_RELATION_MASK_CLR_ALL);
				NextTime = KeyPoint = m_pCalc->Time.GetNextTime();
				m_ParticlesState = GR_PS_NORM;

				for (int i = 0; i < tmpCount && KeyPoint == NextTime; i++)
				{
					m_Array[i]->NewPoint_Begin(NextTime);

					int tmpRet;
					int BS_num = pBSP->GetNumById(i);

					if (BS_num >= 0)
						tmpRet = ComputeParticleDataAtPhantomBarrier(m_Array[i]->m_NextData, i, NextTime, &KeyPoint);
					else
						tmpRet = m_Array[i]->GetNextDataByTime(m_Array[i]->m_NextData, NextTime, &KeyPoint);

					if ((tmpRet& SP_STATE_NEXT_STEP_MIN))
						m_pCalc->Time.AddFlag(SPTIME_NEXT_STEP_MIN);

					if (m_Array[i]->m_NextData.AbsSpeed >= m_pCalc->Critical.Speed)
						m_ParticlesState |= GR_PS_MAXABSSPEED;

					ASSERT(m_pCalc->Time.GetCurTime() != KeyPoint);
					if (NextTime != KeyPoint)
						KeyPoint = m_pCalc->Time.SetNextTimeEx(KeyPoint);
				}
			} while (NextTime != KeyPoint);

			m_Relations.ClearNextState(0, tmpCount - 1, (GR_PS_COLLISION_PRESENT | GR_PS_RELATION_NONEEDRECALC));

			//уточнение времени, пока есть предупреждения
			doByKeyPoint = CalcRelations(NextTime, m_Relations.GetNext(), m_Relations.GetCurent(), m_pCalc->Time.IsMinimalStep() ? SP_CALCRELATION_FIND_VIEWPOINT : SP_CALCRELATION_BREAK_ON_MINOR_ERROR | SP_CALCRELATION_FIND_VIEWPOINT, KeyPoint);
			//ASSERT(fabs(m_Relations.GetNext()[1].Acceleration_3D.X) < 1.0e80);
			ASSERT(NextTime >= KeyPoint);
			if (NextTime != KeyPoint)
			{
				if ((doByKeyPoint & SP_STATE_PHANTOM_CRITICAL_CHANGED) && NextTime - KeyPoint < 1.0e-15)//m_pCalc->Time.GetMinStep())
					KeyPoint = NextTime;
				else
					KeyPoint = m_pCalc->Time.SetNextTimeEx(KeyPoint);
				if (NextTime != KeyPoint)
					doByKeyPoint |= SP_STATE_NEED_RECALC;
			}

		} while (NextTime != KeyPoint);

		if ((doByKeyPoint & SP_STATE_NEED_RECALC))// FINAL if need
		{
			m_Relations.ClearNextState(0, tmpCount - 1, (GR_PS_COLLISION_PRESENT | GR_PS_RELATION_NONEEDRECALC));
			doByKeyPoint = CalcRelations(NextTime, m_Relations.GetNext(), m_Relations.GetCurent(), SP_CALCRELATION_FIND_VIEWPOINT, KeyPoint);
		}

		if ((doByKeyPoint & (SP_STATE_PHANTOM_DIST_LE_CRITICAL| SP_STATE_PHANTOM_DENSITY_LE_0)))
			m_Relations.FillNRelated(GR_PS_RELATION_NEXT, 0, tmpCount, GR_PS_COLLISION_IS_ULTIMATE);
		else
			m_Relations.clear_Related(GR_PS_RELATION_NEXT);



		CalcDataByRelations(GR_PS_RELATION_NEXT);
		ASSERT(m_Array[0]->m_NextData.TimePoint == m_Array[1]->m_NextData.TimePoint);
		ASSERT(m_Array[0]->m_NextData.TimePoint != m_Array[0]->GetCurData_C()->TimePoint);
		double dTime =  m_pCalc->Time.GetStep();
		for (int i = 0; i < tmpCount; i++)
		{
			//ASSERT(m_Array[i]->m_NextData.Velocity.Z== 0.0 && m_Array[i]->m_NextData.Position.Z == 0.0 && m_Array[i]->m_NextData.Accel_2_3D.Z == 0.0 && m_Array[i]->m_NextData.Jerk_3_3D.Z == 0.0 && m_Array[i]->m_NextData.Velocity_Linear.Z == 0.0);
			ASSERT(m_Array[i]->m_NextData.Velocity.LEN() <= 1.00000000000001);
			SPos tmp; 
			tmp.SET_AS_DELTA(m_Array[i]->m_NextData.Position, m_Array[i]->GetCurData_C()->Position);
			ASSERT(dTime > 0.0);

			tmp /= dTime;
			double tmplen = tmp.LEN();
			ASSERT(tmplen - sqrt(dTime) <= 1.0e20 + 1000000.90000000000);
		}

		double cmpDist = m_Array[0]->m_Data->GetCurData()->Position.GetDistance(m_Array[0]->m_NextData.Position);

		ASSERT(!(CSP_Group::AssertionVar & 1) || cmpDist < m_pCalc->Time.GetStep() + 1.0e-15);

		nextStepInfo = { 0 };
		nextStepInfo.TimePoint = NextTime;
		nextStepInfo.RelationsS_Prev = m_ParticleInfoArray->GetCurData()->RelationsState_Cur;

		FiilParticlesState();

		EnterCriticalSection(&m_cs);

		int AddType = DA_ADD_FlagFromResult(m_ParticleInfoArray->AddData(nextStepInfo, DA_ADD_FLAG_ADD_AUTO));

		ret = SP_STATE_ОК;
		//фиксация результата и переход на следующую итерацию
		for (int i = 0; i < tmpCount; i++)
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
	Params.Ret_Flags = SP_STATE_ОК;
	Params.out_KeyPoint = Params.ViewTime;
	
	if (!Params.PtrViewParticle || !Params.NewRelations)
	{
		Params.Ret_Flags = SP_STATE_ERROR_MAIN;
	}
	else if ((Params.NewRelations->State & GR_PS_RELATION_NONEEDRECALC))
	{
		if ((Params.NewRelations->State & GR_PS_COLLISION_IS_ULTIMATE))
		{
			Params.Ret_Flags |= SP_STATE_PHANTOM_DIST_LE_CRITICAL;
			m_ParticlesState |= GR_PS_COLLISION_DISTANCE | GR_PS_COLLISION_IS_ULTIMATE;
		}
	}
	else
	{
		const SData* curParticleData = Params.PtrParticle->GetCurData_C();
		const SData* curViewParticleData = Params.PtrViewParticle->GetCurData_C();

		Params.NewRelations->ViewParticlePtr = Params.PtrViewParticle;
		Params.NewRelations->RealDistancePwr1 = curParticleData->Position.GetDistance(curViewParticleData->Position);
		Params.NewRelations->RealRelativeSpeed = SData::GetRelativeSpeedFromVelocity_Linear(*curParticleData, *curViewParticleData, NULL);

		int tmpRet = 0;
		int prevState = Params.PrevRelations->State;
		ASSERT(Params.PrevRelations);

		if ((Params.CalcType & SP_CALCRELATION_FIND_VIEWPOINT))
		{
			double in_StartTime = -99999.0, EndTime;

			EndTime = Params.PtrViewParticle->GetCurTime();
			in_StartTime = Params.NewRelations->LastViewPointTime = Params.PrevRelations->LastViewPointTime;

			if (EndTime > Params.ViewTime) EndTime = Params.ViewTime;

			ASSERT(EndTime >= 0.0);

			ASSERT(Params.ViewTime - m_pCalc->Time.GetCurTime() > 0.0);

			tmpRet = Params.PtrViewParticle->FindViewPointByTime(Params.NewRelations->ViewPointData, Params.DataParticle, Params.out_KeyPoint, in_StartTime, EndTime, &Params.NewRelations->LowIdxOfViewPont_Near);

			if ((tmpRet & SP_STATE_FAR_AWAY) == SP_STATE_FAR_AWAY && ((tmpRet ^ prevState) & SP_STATE_FAR_AWAY) == 0 && Params.NewRelations->RealDistancePwr1 < m_pCalc->Critical.Distance) ///&& m_pCalc->Time.IsMinimalStep()
			{ //возможно немного оптимизировать по расстоянию и скорости
				SData dataM1;
				int IdxTmp = 0;
				//Params.PtrViewParticle->GetPrevDataByTime(dataM1, Params.out_KeyPoint - m_pCalc->Critical.Distance, NULL);
				Params.PtrViewParticle->GetPrevDataByTimeLinear(dataM1, IdxTmp, 0, Params.out_KeyPoint - m_pCalc->Critical.Distance);

				double dist = dataM1.Position.GetDistance(Params.PtrParticle->GetCurPosition()) - m_pCalc->Critical.Distance;
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
			{ // ПЕРЕХОД В НЕВИДИМОСТЬ
				//ASSERT((Params.PrevRelations->State & GR_PS_COLLISION_DISTANCE) == 0 );
				if (!m_pCalc->Time.IsMinimalStep())
				{
					m_pCalc->Time.SetUpLimit(m_pCalc->Time.GetNextTime());
					Params.out_KeyPoint = m_pCalc->Time.GetNextTime_ByCorrectedDelta(m_pCalc->Time.GetStep4321());
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
			int RelParticleType = (Params.PtrViewParticle->m_TypeFlags & SP_TYPE_FORM_MASK);


			if (((tmpRet ^ prevState) & SP_STATE_FAR_AWAY))
			{//ВЫХОД ИЗ НЕВИДИМОСТИ
				Params.Ret_Flags = tmpRet;
				Params.Ret_Flags |= SP_STATE_AFTER_FAR_AWAY;
				m_ParticlesState |= GR_PS_FAR_AWAY_PRESENT_OFF;

				if (!m_pCalc->Time.IsMinimalStep())
				{
					m_pCalc->Time.SetUpLimit(m_pCalc->Time.GetNextTime());
					Params.out_KeyPoint = m_pCalc->Time.GetNextTime_ByCorrectedDelta(m_pCalc->Time.GetStep() * 0.5);
				}
			}

			Params.NewRelations->LastViewPointTime = Params.NewRelations->ViewPointData.TimePoint;

			/////////SPEED
			Params.NewRelations->RelativeSpeed = SData::GetRelativeSpeedFromVelocity_Linear(Params.DataParticle, Params.NewRelations->ViewPointData, &Params.NewRelations->RelativeVelocity_3D);
			Params.NewRelations->RelativeSpeedAbs = fabs(Params.NewRelations->RelativeSpeed);

			/////////DISTANCE
			Params.NewRelations->Distance_3D.SET_AS_DELTA(Params.DataParticle.Position, Params.NewRelations->ViewPointData.Position);

			double DistancePwr1tmp = Params.NewRelations->Distance_3D.LEN();

			if (m_pCalc->ParentSpaceThickness != 0.0)
			{
				Params.NewRelations->Distance_3D *= (DistancePwr1tmp + m_pCalc->ParentSpaceThickness) / DistancePwr1tmp;
				DistancePwr1tmp += m_pCalc->ParentSpaceThickness;
			}

			Params.NewRelations->DistancePwr1 = DistancePwr1tmp;
			Params.NewRelations->DistancePwr2 = DistancePwr1tmp * DistancePwr1tmp;
			Params.NewRelations->DistancePwr3 = Params.NewRelations->DistancePwr2 * DistancePwr1tmp;

			double DeltaDistPB = Params.NewRelations->DistancePwr1 - m_pCalc->Critical.Distance;

			int tmpFlags = 0x0;
			Params.NewRelations->State &= ~GR_PS_COLLISION_PRESENT;

			if (fabs(DeltaDistPB) <= 1.0e-14)//////////////!!!!!!ввести параметр - толщина барьера
			{ //в зоне барьера . Выход из зоны в другой процедуре
				if ((Params.PrevRelations->State & GR_PS_COLLISION_PRESENT) == 0) 
					tmpFlags = 0x11; //попал в зону барьера снаружи. сброс шага 4321
				else
					tmpFlags = 0x11;//попал в зону барьера изнутри. сброс шага 4321
			}
			else if (DeltaDistPB < 0)
			{//внутри барьера  0x2;
				if ((Params.PrevRelations->State & GR_PS_COLLISION_PRESENT) == 0)
					tmpFlags = 0x11;//проскочили барьер снаружи и возврат на него, сброс шага 4321
				else 
					tmpFlags = 0x02; //остается во внутренней области
			}
			else
			{//вне барьера
				if ((Params.PrevRelations->State & GR_PS_COLLISION_IS_INSIDE))
					tmpFlags = 0x11;//проскочили барьер изнутри и возврат на него, сброс шага 4321
				else
					tmpFlags = 0x04;// свободен 
			}

			if ((tmpFlags & 0x01))
			{// перешел на фантомный барьер. Пока не соскочит будет рассчитываться не здесь
				Params.Ret_Flags |= SP_STATE_PHANTOM_DIST_LE_CRITICAL;
				Params.NewRelations->State |= GR_PS_COLLISION_DISTANCE | GR_PS_COLLISION_IS_ULTIMATE;
				m_ParticlesState |= GR_PS_COLLISION_DISTANCE | GR_PS_COLLISION_IS_ULTIMATE;
				//явное определение
				Params.NewRelations->DistancePwr1 = m_pCalc->Critical.Distance;
				Params.NewRelations->DistancePwr2 = m_pCalc->Critical.DistancePwr2;
				Params.NewRelations->DistancePwr3 = m_pCalc->Critical.DistancePwr3;

				if (!m_pCalc->Time.IsMinimalStep() && (tmpFlags & 0x10))
				{/////////// РАСЧЕТ ТОЧКИ ДОСТИЖЕНИЯ КРИТИЧЕСКОЙ ДИСТАНЦИИ
					if (!Params.PrevRelations->RelativeVelocity_3D.IsZero() || !Params.PrevRelations->Acceleration_3D.IsZero())
					{
						double prev = Params.out_KeyPoint;
						Params.out_KeyPoint = CalcCriticalDistTime(*Params.PrevRelations, Params.out_KeyPoint);
						if (prev <= Params.out_KeyPoint)
							Params.out_KeyPoint = (m_pCalc->Time.GetCurTime() + Params.out_KeyPoint) * 0.5;
					}
					else
						Params.out_KeyPoint = m_pCalc->Time.GetNextTime_ByCorrectedDelta(m_pCalc->Time.GetStep() * 0.4321);
				}

				if ((Params.PrevRelations->State & GR_PS_COLLISION_PRESENT) == 0)
					Params.Ret_Flags |= SP_STATE_PHANTOM_DIST_CRITICAL_CHG;

				if (RelParticleType == SP_TYPE_NEGATIVE)
				{
					Params.Ret_Flags |= SP_STATE_PHANTOM_EXPELLED;
					Params.NewRelations->State |= GR_PS_COLLISION_EXPELLED;
					m_ParticlesState |= GR_PS_COLLISION_EXPELLED;
				}
				else
				{
					Params.Ret_Flags |= SP_STATE_PHANTOM_BESTUCK;
					Params.NewRelations->State |= GR_PS_COLLISION_BESTUCK;
					m_ParticlesState |= GR_PS_COLLISION_BESTUCK;
				}
			}

			if ((tmpFlags & 0x02))
			{//внутри барьера
				Params.Ret_Flags |= SP_STATE_PHANTOM_DIST_LE_CRITICAL;
				Params.NewRelations->State |= (GR_PS_COLLISION_DISTANCE | GR_PS_COLLISION_IS_INSIDE);
				m_ParticlesState |= (GR_PS_COLLISION_DISTANCE | GR_PS_COLLISION_IS_INSIDE);

				if (RelParticleType == SP_TYPE_NEGATIVE)
				{
					Params.Ret_Flags |= SP_STATE_PHANTOM_EXPELLED;
					Params.NewRelations->State |= GR_PS_COLLISION_EXPELLED;
					m_ParticlesState |= GR_PS_COLLISION_EXPELLED;
				}
				else
				{
					Params.Ret_Flags |= SP_STATE_PHANTOM_BESTUCK;
					Params.NewRelations->State |= GR_PS_COLLISION_BESTUCK;
					m_ParticlesState |= GR_PS_COLLISION_BESTUCK;
				}
			}
			//else //снаружи. Вся обработка колизий и времени в другом месте

			/////////////РАСЧЕТ ПЛОТНОСТИ
			Params.NewRelations->Calc_RelativeDensity(m_pCalc->DensityCalcType);

			if (Params.NewRelations->DensityAtBasePoint <= 0.0 && RelParticleType != SP_TYPE_NEGATIVE)
			{
				Params.NewRelations->State |= (GR_PS_COLLISION_DENSITY | GR_PS_COLLISION_BESTUCK);
				Params.Ret_Flags |= SP_STATE_PHANTOM_DENSITY_LE_0 | SP_STATE_PHANTOM_BESTUCK;
				m_ParticlesState |= (GR_PS_COLLISION_DENSITY| GR_PS_COLLISION_BESTUCK);
			}
			else if (Params.NewRelations->DensityAtBasePoint >= 1.0 && RelParticleType == SP_TYPE_NEGATIVE)
			{
				Params.NewRelations->State |= (GR_PS_COLLISION_DENSITY | GR_PS_COLLISION_EXPELLED);
				Params.Ret_Flags |= SP_STATE_PHANTOM_DENSITY_LE_0 | SP_STATE_PHANTOM_EXPELLED;
				m_ParticlesState |= (GR_PS_COLLISION_DENSITY | GR_PS_COLLISION_EXPELLED);
			}

			if (((Params.NewRelations->State ^ Params.PrevRelations->State) & GR_PS_COLLISION_DENSITY))
			{/////////// расчет точки достижения критической дистанции
				Params.Ret_Flags |= SP_STATE_PHANTOM_DENSITY_LE_CHG;

				if (!m_pCalc->Time.IsMinimalStep() && Params.out_KeyPoint - m_pCalc->Time.GetCurTime() > m_pCalc->Time.GetMinStep())
				{
					if (!((Params.NewRelations->State ^ Params.PrevRelations->State) & GR_PS_COLLISION_DISTANCE)) //!!!! идентично, если было
					{
						if (!Params.PrevRelations->RelativeVelocity_3D.IsZero() || !Params.PrevRelations->Acceleration_3D.IsZero())
						{	/////////// расчет точки достижения нулевой плотности 
							Params.out_KeyPoint = CalcCriticalDistTime(*Params.PrevRelations, Params.out_KeyPoint);
						}
						else
						{
							if (Params.out_KeyPoint - m_pCalc->Time.GetCurTime() > 0.00001)
								Params.out_KeyPoint = m_pCalc->Time.GetNextTime_ByCorrectedDelta(0.00001);
						}
					}
				}
			}

			//расчет ускорения
			Params.NewRelations->Calc_RelativeAcceleration(m_pCalc->AccCalcType);
			ASSERT(fabs(Params.NewRelations->Acceleration_3D.X) < 1.0e80);


			if (m_pCalc->WindType && Params.NewRelations->RelativeSpeedAbs > 0.0000000000001)
			{///???????????????? продумать при переходах и движении прилипших
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
	if (Params.pInterruptionFlag && Params.out_KeyPoint != Params.ViewTime)
		*Params.pInterruptionFlag = 1;
	return Params.Ret_Flags;
}

int CSP_Group::CalcRelationsSingle(double in_TimePoint, SPRelation* const in_CalcRelations, SPRelation* const in_PrevRelations, int in_CalcType, int in_TimeType, double & out_KeyPoint)
{
	CalceRelationParams curParams;
	int tmpCount = PartCntr.GetCount();

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
		int maskBreak = (in_CalcType & SP_CALCRELATION_BREAK_ON_MINOR_ERROR);
		volatile INT64 InterruptionFlag = 0;
		if (maskBreak) curParams.pInterruptionFlag = &InterruptionFlag;
		int BP_Id;
		for (BP_Id = 0; BP_Id < tmpCount && !(curRet_Flags & maskBreak) && !InterruptionFlag; BP_Id++)
			if ((in_CalcType == -1 || !(in_CalcType & SP_CALCRELATION_NEED_UPDATE_ONLY)) || (in_CalcRelations[BP_Id].State & SP_STATE_NEED_UPDATE))
			{
				curParams.PtrParticle = m_Array[BP_Id];
				curParams.Particle_ID = BP_Id;
				if (in_TimeType == 0) 
					curParams.DataParticle = m_Array[BP_Id]->m_NextData;
				else if (curParams.ViewTime > m_Array[0]->GetCurTime())
					m_Array[BP_Id]->GetNextDataByTime(curParams.DataParticle, curParams.ViewTime, &tmpDbl);
				else
				{
					//m_Array[BP_Id]->GetPrevDataByTime(curParams.DataParticle, curParams.ViewTime, &tmpDbl);
					int IdxTmp = 0;
					m_Array[BP_Id]->GetPrevDataByTimeLinear(curParams.DataParticle, IdxTmp, 0, curParams.ViewTime);
				}
				int VP_Id, RelationPos = BP_Id;
				for (VP_Id = 0; VP_Id < tmpCount && !(curRet_Flags & maskBreak) && !InterruptionFlag; VP_Id++, RelationPos += m_maxCount)
					if (BP_Id != VP_Id)
					{
						curParams.NewRelations = in_CalcRelations + RelationPos;

						curParams.PtrViewParticle = m_Array[VP_Id];
						curParams.ViewParticle_ID = VP_Id;
						curParams.PrevRelations = in_PrevRelations + RelationPos;
						curRet_Flags |= CalcSingleRelation(curParams);

						if (curParams.out_KeyPoint < out_KeyPoint)
							out_KeyPoint = curParams.out_KeyPoint;
					}

				if (VP_Id < tmpCount)
					curRet_Flags |=SP_STATE_NEED_RECALC;
			}
		if (BP_Id < tmpCount)
			curRet_Flags |=SP_STATE_NEED_RECALC;
	}
	else curRet_Flags = SP_STATE_ERROR_MAIN;

	return curRet_Flags;
}
void CSP_Group::FiilParticlesState()
{
	if (m_Array)
	{
		//Params.DataParticle.State |= Params.NewRelations->State;
		int tmpCount = PartCntr.GetCount();
		SPRelation * Reation = m_Relations.GetNext();

		for (int BP_Id = 0; BP_Id < tmpCount ; BP_Id++)
		{
			SData& ParticleData = m_Array[BP_Id]->m_NextData;
			int VP_Id, RelationPos = BP_Id;
			for (VP_Id = 0; VP_Id < tmpCount; VP_Id++, RelationPos += m_maxCount)
				if (BP_Id != VP_Id)
					ParticleData.State |= Reation[RelationPos].State;

			nextStepInfo.RelationsState_Cur |= ParticleData.State;
			nextStepInfo.ParticlesState |= m_Array[BP_Id]->m_StateNext;
		}
	}
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
						maskBreak = SP_STATE_ERROR_MINOR;
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
									curParams.NewRelations = m_Thread->pCalcRelations + RelationPos;
									curParams.ViewParticle_ID = j;
									curParams.PrevRelations = m_Thread->pPrevRelations ? m_Thread->pPrevRelations + RelationPos : nullptr;
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
		int tmpCount = GetCount();
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
		for (int i = 0, Delta = tmpCount % m_ThreadsCur, iStep = (tmpCount - 1) / m_ThreadsCur + 1, curPtrId = 0; i < m_ThreadsCur && curPtrId < tmpCount; i++)
		{
			if (Delta > 0 && Delta < tmpCount && i == Delta) iStep--;
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
	SData tmpData;
	tmpData.CLEAR();

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
	int tmpCount = PartCntr.GetCount();

	for (int i = 0; i < tmpCount; i++)
	{
		SData PtrData = *(m_Array[i]->GetDataById(0));
		//PtrData.Velocity = { 0 };//???????????????????????????

		for (int j = 0; j < tmpCount; j++)
			if (i != j)
			{
				SPRelation* pRelation = m_Relations.GetPrevRelation(i, j);
				pRelation->ViewParticlePtr = m_Array[j];
				SPRelation *pRelationCur = m_Relations.GetCurentRelation(i, j);
				pRelationCur->ViewParticlePtr = m_Array[j];
				pRelation->ViewPointData = *(pRelation->ViewParticlePtr->GetDataById(0));


				double  LinearViewTime = GetLinearViewTime(PtrData, pRelation->ViewPointData);

				double newVP = PtrData.TimePoint + LinearViewTime;

				if (fabs(LinearViewTime) != SP_MAX_TIME_DISTANCE) //&& newVP <= UpBorder.TimePoint)
				{
					pRelation->ViewPointData.LinearMove(newVP - pRelation->ViewPointData.TimePoint);

					double dDelta, dDeltaPrev = 1.0e90, prev_newVP = 0.0;
					int AssertCntr = 0;

					while ((dDelta = fabs(- newVP - PtrData.Position.GetDistance(pRelation->ViewPointData.Position))) > 1.0e-14 && dDelta < dDeltaPrev)
					{//коррекция результатов сжатия траектории (здесь лишнее)
						ASSERT(AssertCntr++ < 20);
						dDeltaPrev = dDelta;
						prev_newVP = newVP;

						newVP = PtrData.TimePoint + GetLinearViewTime(PtrData, pRelation->ViewPointData);
						pRelation->ViewPointData.LinearMove(newVP - pRelation->ViewPointData.TimePoint);
					}

					if (dDelta > 1.0e-14 && dDelta != dDeltaPrev)
						pRelation->ViewPointData.LinearMove(prev_newVP - pRelation->ViewPointData.TimePoint);
				}

				pRelation->LastViewPointTime = PtrData.TimePoint + LinearViewTime;
				if (fabs(pRelation->LastViewPointTime) >= SP_MAX_TIME_DISTANCE)
				{
					pRelation->LastViewPointTime = -10000.0;
					pRelation->SET_AS_FARAWAY(pRelation->ViewParticlePtr->m_TypeFlags);
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

					pRelation->RelativeSpeed = SData::GetRelativeSpeedFromVelocity(PtrData, pRelation->ViewPointData, &pRelation->RelativeVelocity_3D);
					pRelation->RelativeSpeedAbs = fabs(pRelation->RelativeSpeed);


					pRelation->Calc_RelativeDensity(m_pCalc->DensityCalcType);
					pRelation->Calc_RelativeAcceleration(m_pCalc->AccCalcType);
				}

				pRelation->RealDistancePwr1 = PtrData.Position.GetDistance(pRelation->ViewPointData.Position);
				pRelation->RealRelativeSpeed = SData::GetRelativeSpeedFromVelocity(PtrData, pRelation->ViewPointData, nullptr);

				*pRelationCur = *pRelation;
			}
	}
}

int CSP_Group::CalcRelations(double in_TimePoint, SPRelation* const in_CalcRelations, SPRelation* const in_PrevRelations, int in_CalcType, double& out_KeyPoint)
{
	int AddResult = 0;
	int tmpCount = GetCount();

	if (m_Array && *m_Array)
	{
		m_ParticlesState &= (GR_PS_MAXABSSPEED| GR_PS_MAXABSSPEED_ON| GR_PS_MAXABSSPEED_OFF| GR_PS_POSITION_CHANGED| GR_PS_POSITION_LOW_PREC);

		int TimeType = 2;
		if (in_TimePoint == m_Array[0]->GetCurTime()) TimeType = 1;
		else
			if (in_TimePoint == m_Array[0]->m_NextData.TimePoint) TimeType = 0;

		if ((in_CalcType & SP_CALCRELATION_NEED_UPDATE_ONLY))
			for (int i = 0; i < tmpCount; i++)
				for (int j = 0, RelationPos = i; j < tmpCount; j++, RelationPos += m_maxCount)
					if ((in_CalcRelations[RelationPos].State & SP_STATE_NEED_UPDATE))
					{
						j = 0;
						RelationPos = i;
						for (; j < tmpCount; j++, RelationPos += m_maxCount)
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

		if (temp_Stat.Rel_Min.Phantom_Distance.MMValue <= m_pCalc->Critical.Distance)
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
		SData initData;
		initData.CLEAR();

		initData.Position.X = (in_Settings.maxPos.X - in_Settings.minPos.X) * (dblHaf - (double)rand()) * dblDiv;
		initData.Position.Y = (in_Settings.maxPos.Y - in_Settings.minPos.Y) * (dblHaf - (double)rand()) * dblDiv;
		initData.Position.Z = (in_Settings.maxPos.Z - in_Settings.minPos.Z) * (dblHaf - (double)rand()) * dblDiv;

		initData.AbsSpeed = (double)rand() * dblDiv;

		initData.Velocity.X = (in_Settings.maxSpeed.X - in_Settings.minSpeed.X) * (dblHaf - (double)rand());
		initData.Velocity.Y = (in_Settings.maxSpeed.Y - in_Settings.minSpeed.Y) * (dblHaf - (double)rand());
		initData.Velocity.Z = (in_Settings.maxSpeed.Z - in_Settings.minSpeed.Z) * (dblHaf - (double)rand());

		initData.Velocity.SET_LENGHT(initData.AbsSpeed);
		initData.Velocity_Linear = initData.Velocity;
		SetParticleData(i, SP_DATA_ALL, initData);
	}
}

int CSP_Group::CalcDataByRelations(int in_Type)
{
	int KeyPointFlag = SP_STATE_ERROR_MAIN;
	int tmpCount = GetCount();

	SPRelation * pRelations = m_Relations.GetRelation(in_Type);
	const RelatedParticles* pBSP = (in_Type & GR_PS_RELATION_NEEDRECALC) ? m_Relations.FillNRelated(in_Type, 0, tmpCount, GR_PS_COLLISION_IS_ULTIMATE) : m_Relations.GetRelated(in_Type);
	SData* pCalcData;

	if (m_Array && pRelations)
	{
		KeyPointFlag = 0;

		for (int i = 0; i < tmpCount; i++)
			if ((pCalcData = (in_Type & GR_PS_RELATION_NEXT) ? &m_Array[i]->m_NextData : (in_Type & GR_PS_RELATION_CURRENT) ? m_Array[i]->GetCurData() : nullptr))
			{
				const RelatedList* BSList = pBSP ? pBSP->GetRelatedListById(i) : nullptr;
				pCalcData->Accel_2_3D = { 0 };
				pCalcData->Jerk_3_3D = { 0 };
				pCalcData->Density = 0.0;
				int tmpRet = 0;
				SPRelation* pSingleRel = pRelations + i;
				RelationAndRelated* pSelfRel = (RelationAndRelated *) (pRelations + i + i * m_maxCount);
				pSelfRel->AsSelf = { 0 };

				if (BSList == nullptr)
				{
					for (int j = 0; j < tmpCount; j++, pSingleRel += m_maxCount)
						if (i != j)
							tmpRet |= CalcSingleDataByRelation(*pCalcData, *pSingleRel, m_pCalc);

					pSelfRel->AsSelf.ByNormalData = *pCalcData;
				}
				else
				{
					ASSERT(BSList->nMasters > 0);

					for (int j = 0; j < tmpCount; j++, pSingleRel += m_maxCount)
						if (i != j && !(pSingleRel->State & GR_PS_COLLISION_IS_ULTIMATE))
							tmpRet |= CalcSingleDataByRelation(pSelfRel->AsSelf.ByNormalData, *pSingleRel, m_pCalc);
					//те, что на барьере рассчитаны при вычислении новой позиции
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
		double tmpVal = in_CalcRelations.DistancePwr2 - m_pCalc->Critical.Distance;
			
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
				int tmpInt;
				sscanf_s(in_Command, "%i", &tmpInt);
				if (tmpInt < 0 || tmpInt > 10000) tmpInt = 0;
				m_pCalc->Time.Delay.SetDelay(tmpInt);

			}
			else if (strstr(in_Command, "ADD") == in_Command && in_Command[3] <= ' ')
			{
				in_Command += 3;
				while (*in_Command == ' ' || *in_Command == '\t' || *in_Command == '=') in_Command++;

				SData initData;initData.CLEAR();

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

void CSP_Group::DoDelay()
{
	if (m_pCalc->Time.CheckDelay())
	{
		m_pCalc->Time.DoDelay();
		m_pCalc->Time.SetTimeOfLastDelay();
	}
}

int CSP_Group::ReadSettingsGroup(char* in_Settings, int in_SettingsGroup, int in_UseDefs)
{
	int ret = 0; 
	
	if (m_pCalc && m_pCalcFind && in_Settings && *in_Settings)
	{
		if ((in_SettingsGroup & SGROUP_TRACE))
		{
			int tmpInt;
			double tmpDbl;
			ret += ReadCalcParam(in_Settings, "STEP_DELAY_MS", tmpInt, 0, in_UseDefs);			m_pCalc->Time.Delay.SetDelay(tmpInt);
			ret += ReadCalcParam(in_Settings, "STEP_DELAY_EVERY", tmpDbl, 1.0, in_UseDefs);		m_pCalc->Time.Delay.SetDelayEveryTimeDelta(tmpDbl);
			ret += ReadCalcParam(in_Settings, "TRACE_SPEED_ARR", m_pCalc->DrawSet.ShowSpeedFor, 0xFFFFFFFF, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "TRACE_ACC_ARR", m_pCalc->DrawSet.ShowAccFor, 0xFFFFFFFF, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "TRACE_GROUP_ARR", m_pCalc->DrawSet.ShowGroupArrows, 0xFFFFFFFF, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "TRACE_SHOWITEMS", m_pCalc->DrawSet.ShowItems, 0xFFFFFFFF, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "TRACE_SHOWLINKSFROM", m_pCalc->DrawSet.ShowLinksFrom, 0x0001, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "TRACE_SHOWLINKSTO", m_pCalc->DrawSet.ShowLinksTo, 0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "TRACE_LINKS", m_pCalc->DrawSet.nLinks, 10, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "TRACE_REFRESH", m_pCalc->refresh.Delta, 50, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "TRACE_SHOTREFRESH", m_pCalc->ShotRefresh.Delta, 10, in_UseDefs);
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

			ret += ReadCalcParam(in_Settings, "ASSERT_STEP", tmpInt, -1, in_UseDefs);
			m_pCalc->CalcCounter.SetAssertStep(tmpInt);

			if ((tmpInt = ReadCalcParam(in_Settings, "MAXSTEP_VAL", tmpVal, 0.1, in_UseDefs))|| !(in_UseDefs & READPARAMS_NOT_USE_DEFS))
				m_pCalc->Time.SetMaxStep(tmpVal);
			ret += tmpInt;

			if ((tmpInt = ReadCalcParam(in_Settings, "MINSTEP_VAL", tmpVal, SP_PREC_TIME_MLTPL, in_UseDefs))|| !(in_UseDefs & READPARAMS_NOT_USE_DEFS))
				m_pCalc->Time.SetMinStep(tmpVal);
			ret += tmpInt;

			if ((tmpInt = ReadCalcParam(in_Settings, "BESTUCKMAXSTEP_VAL", tmpVal, SP_PREC_TIME_MLTPL, in_UseDefs))|| !(in_UseDefs & READPARAMS_NOT_USE_DEFS))
				m_pCalc->Time.SetRelatedMaxStep(tmpVal);
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
			ret += ReadCalcParam(in_Settings, "INITIALCHECK", m_pCalc->InitialCheckFlags, 1, in_UseDefs);

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
			ret += ReadCalcParam(in_Settings, "DENSITYDERIVLEVEL", m_pCalc->DensityDerivLevel, 2, in_UseDefs);
				if (m_pCalc->DensityDerivLevel > 3 || m_pCalc->DensityDerivLevel < 1) m_pCalc->DensityDerivLevel = 2;
			ret += ReadCalcParam(in_Settings, "ACCCALCTYPE", m_pCalc->AccCalcType, 0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "CLASSICTYPE", m_pCalc->ClassicType, 0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "ADDITIVETYPE", m_pCalc->AdditiveType, 0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "DENSITYCALCTYPE", m_pCalc->DensityCalcType, 0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "MAXABSSPEED", m_pCalc->Critical.Speed,  1.0, in_UseDefs);

			ret += ReadCalcParam(in_Settings, "PARENTSPACETHICKNESS", m_pCalc->ParentSpaceThickness, 0.0, in_UseDefs);
			ret += ReadCalcParam(in_Settings, "CRITICALDISTANCE", m_pCalc->Critical.Distance, 1.0, in_UseDefs);

			m_pCalc->Critical.Prepare();
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

void CSP_Group::CheckBeforeCalc()
{
	m_ThreadsMax = m_ThreadsMax_Initial;
	if (GetCount() < m_ThreadsMax) m_ThreadsMax = GetCount();
	CheckThreads();
};


void CSP_Group::ReadParticleData(const char* in_Settings, int in_ParticleId)
{
	int IdStart = 0, IdEnd = GetCount() - 1;
	if (in_ParticleId >= IdStart && in_ParticleId <= IdEnd)
		IdStart = IdEnd = in_ParticleId;

	const char* prefix = "PARTICLEDATA[";
	char tmps[50];
	const char* posTmp = strstr(in_Settings, prefix);
	if (posTmp)
	{
		strcpy_s(m_tmpString, SPG_TEMPSTRING_LEN, posTmp);

		char* posPrev = m_tmpString, *pos;
		while ((pos = strstr(posPrev, prefix)))
			posPrev = pos + 1;

		while (posPrev != m_tmpString && *posPrev && *posPrev != '\r' && *posPrev != '\n')
			posPrev++;

		*posPrev = 0;

		for (int i = IdStart; i <= IdEnd; i++)
		{
			SData initData;
			initData.CLEAR();
			sprintf_s(tmps, 50, "%s%d]", prefix, i);

			if ((pos = strstr(m_tmpString, tmps)))
			{
				pos += 15;
				char* posEnd = pos, charAtEnd;
				while (*posEnd && *posEnd != '\r' && *posEnd != '\n')
					posEnd++;

				charAtEnd = *posEnd;
				*posEnd = 0;

				ReadCalcParam(pos, "POSITION", initData.Position, 0.0);
				ReadCalcParam(pos, "VELOCITY", initData.Velocity, 0.0);
				ReadCalcParam(pos, "ACCELERATION", initData.Accel_2_3D, 0.0);
				ReadCalcParam(pos, "JERK", initData.Jerk_3_3D, 0.0);
				initData.AbsSpeed = initData.Velocity.LEN();
				initData.Velocity_Linear = initData.Velocity;

				*posEnd = charAtEnd;
			}

			SetParticleData(i, SP_DATA_ALL, initData);
		}
	}
}

void CSP_Group::ReadParticleLook(const char* in_Settings, int in_ParticleId)
{
	int IdStart = 0, IdEnd = GetCount() - 1;
	if (in_ParticleId >= IdStart && in_ParticleId <= IdEnd)
		IdStart = IdEnd = in_ParticleId;
	const char* prefix = "TRACE_LOOK[";

	const char* posTmp = in_Settings;
	while ((posTmp = strstr(posTmp, prefix)))
	{
		posTmp += 11;
		int idx = atoi(posTmp);

		if (idx >= IdStart && idx <= IdEnd)
		{
			char* tmpDst = m_tmpString;
			while (*posTmp && *posTmp != '\r' && *posTmp != '\n')
				*(tmpDst++) = *(posTmp++);

			*tmpDst = 0;
			m_Array[idx]->m_TraceSettings.SET(m_tmpString);
		}
	}
}


void CSP_Group::PrepareForCalc(int in_CalcOrFind)
{
	EnterCriticalSection(&m_cs);
	EnterCriticalSection(&m_cs_Command);

	ClearCalcState(M_STATE_READY);

	while (GetCount() > 0)
		RemoveSpParticle(GetCount() - 1);


	SData initData; initData.CLEAR();

	while (PartCntr.GetCount(SP_TYPE_POSITIVE) < PartCntr.GetInitial().GetCount(SP_TYPE_POSITIVE))
	{
		AddSpParticle(SP_TYPE_POSITIVE, initData, m_pCalc);
	}

	while (PartCntr.GetCount(SP_TYPE_NEGATIVE) < PartCntr.GetInitial().GetCount(SP_TYPE_NEGATIVE))
	{
		AddSpParticle(SP_TYPE_NEGATIVE, initData, m_pCalc);
	}

	InitItems(SP_CLEAR_SOFT);


	if (in_CalcOrFind == 0)
	{   //Calc
		if (m_pCalc->sRandSeed_Calc < 0)
		{

			ReadParticleData(m_pCalc->CalcParamsDecoded, -1);
			CalcRelationsInit();
		}
		else
		{
			InitPosSpeedRandom(m_pCalc->sRandSeed_Calc, m_pCalc->CalcFindSet);
			if ((m_pCalc->InitialCheckFlags & 0x0001))
				RepairPositions();
		}

		CalcDataByRelations(GR_PS_RELATION_CURRENT);
		CalcRelationsInit();//??????
		CalcDataByRelations(GR_PS_RELATION_CURRENT);

		m_pCalc->CalcCounter.InitLoop();
		m_pCalc->Time.FixStartTime();
	}
	else if (in_CalcOrFind == 1)
	{//Find
		InitPosSpeedRandom(m_pCalcFind->sRandSeed == -1 ? m_pCalcFind->stat.Count : -1, m_pCalcFind->Settings);
		if ((m_pCalc->InitialCheckFlags & 0x0001))
			RepairPositions();

		CalcRelationsInit();//??????
		CalcDataByRelations(GR_PS_RELATION_CURRENT);
		CalcRelationsInit();//??????

		SetCalcState(M_STATE_READY);

		ZeroMemory(&m_pCalc->Stat, sizeof(SP_CalcStat));

		CollectStat(STAT_TYPE_ALL_CURRENT | STAT_TYPE_SETPARTICLE);

		m_pCalcFind->FindCounter.InitLoop();
		m_pCalc->Time.FixStartTimeCur();
	}

	CheckBeforeCalc();

	ReadParticleLook(m_pCalc->CalcParamsDecoded, -1);

	LeaveCriticalSection(&m_cs_Command);
	LeaveCriticalSection(&m_cs);
};



int CSP_Group::SetCalcState(int in_State)
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
int CSP_Group::ClearCalcState(int in_State)
{
	m_CalcState &= (0xFFFFFFFF ^ in_State);

	return m_CalcState;
};

double CSP_Group::CalcCriticalDistTime(SPRelation& PrevRelation, double in_CalcTime, double* newTimePoint)
{
	double RetKeyPoint;
	TwoDoubleRoots result;
	SPos dV = PrevRelation.Distance_3D;
	dV.SET_AS_PROJECTION_OF(PrevRelation.RelativeVelocity_3D);
	SPos dA = PrevRelation.Distance_3D;
	dA.SET_AS_PROJECTION_OF(PrevRelation.Acceleration_3D);
	dA.MULT(0.5);

	result = QuadraticEequationSolution(dA.LEN(), dV.LEN(), m_pCalc->Critical.Distance - PrevRelation.DistancePwr1);
	double newDeltaTime = result.root1.Dbl;
	if ((isnan(newDeltaTime) || newDeltaTime < 0.0) && (isnan(newDeltaTime = result.root2.Dbl) || newDeltaTime < 0.0))
	{
		RetKeyPoint = m_pCalc->Time.GetNextTime_ByCorrectedDelta(m_pCalc->Time.GetStep() * 0.15);
	}
	else if (newDeltaTime == 0.0)
	{
		RetKeyPoint = m_pCalc->Time.GetNextTime_ByCorrectedDelta(m_pCalc->Time.GetStep()) - m_pCalc->Time.GetMinStep() * 0.1;
	}
	else if (m_pCalc->Time.GetCurTime() + newDeltaTime <= in_CalcTime)
	{
		RetKeyPoint = m_pCalc->Time.GetNextTime_ByCorrectedDelta(newDeltaTime);
	}
	else
	{
		RetKeyPoint = m_pCalc->Time.GetNextTime_ByCorrectedDelta(m_pCalc->Time.GetStep() * 0.4321);
	}

	if (newTimePoint) *newTimePoint = newDeltaTime;

	return RetKeyPoint;
}


int CSP_Group::CheckHealth()
{
	int ret = 0;
	int m_Count = PartCntr.GetCount();
	if (m_Array)
		for (int i = 0; i < m_Count; i++)
			if (m_Array[i]) ret++;

	return m_Count - ret;
}
