#include "pch.h"
#include "SpaceOM.h"
#include "SpaceOMDlg.h"

#include "CSP_Group.h"
#include "CDrawDlg.h"
#include "CFindDlg.h"

#include "CSpParticle.h"
#include "CSDataArray.h"

UINT DoCalc_1(LPVOID pParam)
{
	if (pParam)
	{
		int nIter = 400;
		SP_Calc* m_Calc = (SP_Calc*)pParam;
		FILE* ptrFile = NULL;

		m_Calc->Group->Reset();
		m_Calc->Time.BaseStep = 0.002;
		m_Calc->Time.MinStep = SP_PREC_TIME_MINSTEP;
		m_Calc->Time.MaxStep = 1000;
		m_Calc->State |= M_STATE_CALC;
		///////// ////////////////////////////////////////////////

		ReadCalcParam(m_Calc->CalcParams, "MAXRELATIVESPEED", m_Calc->maxRelativeSpeed, 1.0);
		ReadCalcParam(m_Calc->CalcParams, "MAXABSSPEED", m_Calc->maxAbsSpeed, 1.0);
		m_Calc->maxAbsSpeedPwr2 = m_Calc->maxAbsSpeed * m_Calc->maxAbsSpeed;
		ReadCalcParam(m_Calc->CalcParams, "ACCCALCTYPE", m_Calc->AccCalcType, 0);
		ReadCalcParam(m_Calc->CalcParams, "ADDITIVETYPE", m_Calc->AdditiveType, 0);
		ReadCalcParam(m_Calc->CalcParams, "CLASSICTYPE", m_Calc->ClassicType, 0);
		ReadCalcParam(m_Calc->CalcParams, "DENSITYCALCTYPE", m_Calc->DensityCalcType, 0);
		ReadCalcParam(m_Calc->CalcParams, "DENCITYDERIVLEVEL", m_Calc->DensityDerivLevel, 3); if (m_Calc->DensityDerivLevel > 3 || m_Calc->DensityDerivLevel < 1) m_Calc->DensityDerivLevel = 3;
		ReadCalcParam(m_Calc->CalcParams, "PARENTSPACETHICKNESS", m_Calc->ParentSpaceThickness, 0.0);
		ReadCalcParam(m_Calc->CalcParams, "CRITICALDISTANCE", m_Calc->CriticalDistance, 1.0);
		ReadCalcParam(m_Calc->CalcParams, "WINDTYPE", m_Calc->WindType, 0);
		ReadCalcParam(m_Calc->CalcParams, "COLLISIONTYPE", m_Calc->CollisionType, 0);

		///////// ////////////////////////////////////////////////
		ReadCalcParam(m_Calc->CalcParams, "NSTEPS", nIter, 5000000);
		ReadCalcParam(m_Calc->CalcParams, "TIMESTEP", m_Calc->Time.BaseStep, 0.001);
		ReadCalcParam(m_Calc->CalcParams, "MINSTEP_VAL", m_Calc->Time.MinStep, 0.0001);
		ReadCalcParam(m_Calc->CalcParams, "MAXSTEP_VAL", m_Calc->Time.MaxStep, 1000.0);
		ReadCalcParam(m_Calc->CalcParams, "MAXNSTEPS", m_Calc->Group->m_maxCountLimit, 5000000);
		///////// ////////////////////////////////////////////////
		ReadCalcParam(m_Calc->CalcParams, "OUTPUTFLAGS", m_Calc->OutPutFlags, 0);


		///////// ////////////////////////////////////////////////
		if ((m_Calc->OutPutFlags & 0x0001) == 0 || !fopen_s(&ptrFile, "C:\\SParticle\\report.txt", "wt"))
		{
			int nParticle = 2;
			ReadCalcParam(m_Calc->CalcParams, "PARTICLE_COUNT", nParticle, 2);

			for (int i = 1; i <= nParticle; i++)
			{
				char tmps[500];
				SData initData = { 0 };
				sprintf_s(tmps, 500, "p%d_POS", i);
				ReadCalcParam(m_Calc->CalcParams, tmps, initData.position, 0.0);
				sprintf_s(tmps, 500, "p%d_SPEED", i);
				ReadCalcParam(m_Calc->CalcParams, tmps, initData.Speed, 0.0000001);

				m_Calc->Group->AddSpParticle(SP_DATA_ALL, &initData, 1, m_Calc);
			}
			if (ptrFile && (m_Calc->OutPutFlags & 0x0001)) m_Calc->Group->WriteState(ptrFile, m_Calc->Group->GetCurTime());

			
			m_Calc->Group->CalcRelations(0.0, NULL, NULL, NULL, 0);
			m_Calc->Group->CalcDataByCurRelations();

			if (ptrFile && (m_Calc->OutPutFlags & 0x0001)) m_Calc->Group->WriteState(ptrFile, m_Calc->Group->GetCurTime());

			int i = 0;

			double tmpTimeDelta = m_Calc->Time.MinStep;

			while ((i++ < nIter || m_Calc->Group->m_maxCountLimit > 5000) && (m_Calc->State & M_STATE_ON))
			{

				//EnterCriticalSection(&m_Calc->Group->m_cs);
				{
					if (ptrFile && (m_Calc->OutPutFlags & 0x0001)) fprintf(ptrFile, "%d\t", i);

					SData v_Min, v_Max;
					SData_SD v_MinSD, v_MaxSD;
					SMinMaxRelation r_Min, r_Max;

					m_Calc->Group->GetMinMaxData(&v_Min, &v_Max, &v_MinSD, &v_MaxSD);
					m_Calc->Group->GetMinMaxRelation(&r_Min, &r_Max);


					m_Calc->Time.CurStep = m_Calc->Time.BaseStep;
					//if (v_MinSD.Density != 0.0) m_Calc->Time.CurStep = m_Calc->Time.BaseStep /(1.0 - v_MinSD.Density);
					m_Calc->Time.CurStep = m_Calc->Time.BaseStep * r_Min.Distance.MMValue;// / (m_Calc->maxRelativeSpeed + 0.1 - fabs(r_Min.Speed.MMValue));

					if (m_Calc->Time.CurStep < m_Calc->Time.MinStep) m_Calc->Time.CurStep = m_Calc->Time.MinStep;

					if (tmpTimeDelta < m_Calc->Time.CurStep) m_Calc->Time.CurStep = tmpTimeDelta;

					if (m_Calc->Time.CurStep > m_Calc->Time.MaxStep)
						m_Calc->Time.CurStep = m_Calc->Time.MaxStep;

					double LastCurTime = m_Calc->Group->GetCurTime();
					m_Calc->Group->NextStep(m_Calc->Time.CurStep);
					m_Calc->Time.CurTime = m_Calc->Group->GetCurTime();
					tmpTimeDelta = 10.0 * (m_Calc->Time.CurTime - LastCurTime);
					if (ptrFile && (m_Calc->OutPutFlags & 0x0001)) m_Calc->Group->WriteState(ptrFile, 1, 1);
				}

				//LeaveCriticalSection(&m_Calc->Group->m_cs);

			}

			EnterCriticalSection(&m_Calc->Group->m_cs);
			if (ptrFile) fclose(ptrFile);

			m_Calc->State &= (0xFFFFFFFF ^ M_STATE_CALC);
			m_Calc->State &= (0xFFFFFFFF ^ M_STATE_ON);
			m_Calc->State |= M_STATE_CHANGED;
			LeaveCriticalSection(&m_Calc->Group->m_cs);
		}

	}

	return 0;
}

UINT DoCalc_Find(LPVOID pParam)
{
	if (pParam)
	{
		SP_CalcFind* m_Calc = (SP_CalcFind*)pParam;
		FILE* ptrFile = NULL;

		m_Calc->curCalc.Group->Reset();
		m_Calc->FindState |= M_STATE_CALC;
		///////// ////////////////////////////////////////////////
		// 


		int nIter = 1000000;
		ReadCalcParam(m_Calc->FindParams, "FIND_STOP_STEP", nIter, 100000000);

		m_Calc->curCalc.Time.BaseStep = 0.002;
		m_Calc->curCalc.Time.MinStep = SP_PREC_TIME_MINSTEP;
		m_Calc->curCalc.Time.MaxStep = 1000;
		m_Calc->curCalc.State |= M_STATE_CALC;
		///////// ////////////////////////////////////////////////
		ReadCalcParam(m_Calc->FindParams, "MAXRELATIVESPEED", m_Calc->curCalc.maxRelativeSpeed, 1.0);
		ReadCalcParam(m_Calc->FindParams, "MAXABSSPEED", m_Calc->curCalc.maxAbsSpeed, 1.0);
		m_Calc->curCalc.maxAbsSpeedPwr2 = m_Calc->curCalc.maxAbsSpeed * m_Calc->curCalc.maxAbsSpeed;

		ReadCalcParam(m_Calc->FindParams, "ACCCALCTYPE", m_Calc->curCalc.AccCalcType, 0);
		ReadCalcParam(m_Calc->FindParams, "ADDITIVETYPE", m_Calc->curCalc.AdditiveType, 0);
		ReadCalcParam(m_Calc->FindParams, "CLASSICTYPE", m_Calc->curCalc.ClassicType, 0);
		ReadCalcParam(m_Calc->FindParams, "DENSITYCALCTYPE", m_Calc->curCalc.DensityCalcType, 0);
		ReadCalcParam(m_Calc->FindParams, "DENCITYDERIVLEVEL", m_Calc->curCalc.DensityDerivLevel, 3); if (m_Calc->curCalc.DensityDerivLevel > 3 || m_Calc->curCalc.DensityDerivLevel < 1) m_Calc->curCalc.DensityDerivLevel = 3;
		ReadCalcParam(m_Calc->FindParams, "PARENTSPACETHICKNESS", m_Calc->curCalc.ParentSpaceThickness, 0.0);
		ReadCalcParam(m_Calc->FindParams, "CRITICALDISTANCE", m_Calc->curCalc.CriticalDistance, 1.0);
		ReadCalcParam(m_Calc->FindParams, "WINDTYPE", m_Calc->curCalc.WindType, 0);
		ReadCalcParam(m_Calc->FindParams, "COLLISIONTYPE", m_Calc->curCalc.CollisionType, 0);
		///////// ////////////////////////////////////////////////
		ReadCalcParam(m_Calc->FindParams, "TIMESTEP", m_Calc->curCalc.Time.BaseStep, 0.001);
		ReadCalcParam(m_Calc->FindParams, "MINSTEP_VAL", m_Calc->curCalc.Time.MinStep, 0.0001);
		ReadCalcParam(m_Calc->FindParams, "MAXSTEP_VAL", m_Calc->curCalc.Time.MaxStep, 1000.0);
		ReadCalcParam(m_Calc->FindParams, "MAXNSTEPS", m_Calc->curCalc.Group->m_maxCountLimit, 5000000);
		///////// ////////////////////////////////////////////////
		ReadCalcParam(m_Calc->FindParams, "OUTPUTFLAGS", m_Calc->curCalc.OutPutFlags, 0);



		///////// ////////////////////////////////////////////////
		ReadCalcParam(m_Calc->FindParams, "FIND_REFRESH", m_Calc->m_FIND_REFRESH, 5);
		if (m_Calc->m_FIND_REFRESH < 2000) m_Calc->m_FIND_REFRESH *= 1000;
		if (m_Calc->m_FIND_REFRESH > 300000 ) m_Calc->m_FIND_REFRESH = 5000;

		///////// ////////////////////////////////////////////////
		ReadCalcParam(m_Calc->FindParams, "DENCITYDERIVLEVEL", m_Calc->curCalc.DensityDerivLevel, 3); if (m_Calc->curCalc.DensityDerivLevel > 3 || m_Calc->curCalc.DensityDerivLevel < 1) m_Calc->curCalc.DensityDerivLevel = 3;
		///////// ////////////////////////////////////////////////
		// 
		m_Calc->Settings.minPos = { 0 };
		m_Calc->Settings.maxPos = { 0 };
		m_Calc->Settings.minPos.X = m_Calc->Settings.minPos.Y = m_Calc->Settings.minPos.Z = -1000.0;
		m_Calc->Settings.maxPos.X = m_Calc->Settings.maxPos.Y = m_Calc->Settings.maxPos.Z = 1000.0;
		m_Calc->Settings.minSpeed = { 0 };
		m_Calc->Settings.maxSpeed = { 0 };
		m_Calc->Settings.minSpeed.X = m_Calc->Settings.minSpeed.Y = m_Calc->Settings.minSpeed.Z = -1.0;
		m_Calc->Settings.maxSpeed.X = m_Calc->Settings.maxSpeed.Y = m_Calc->Settings.maxSpeed.Z = 1.0;

		int sRandSeed = 0;
		ReadCalcParam(m_Calc->FindParams, "SRANDSEED", sRandSeed, 0);

		m_Calc->Settings.Stop_Time = 1.0e10;
		m_Calc->Settings.Stop_minRelDist = 10000.0;
		m_Calc->Settings.Stop_minMaxRelSpeed = 0.001;
		m_Calc->Settings.Stop_CurStep = 1000.0;
		m_Calc->Settings.ExcludeDistance = 1000.0;

		ReadCalcParam(m_Calc->FindParams, "FIND_MINPOS", m_Calc->Settings.minPos, -100.0);
		ReadCalcParam(m_Calc->FindParams, "FIND_MAXPOS", m_Calc->Settings.maxPos, 100.0);
		ReadCalcParam(m_Calc->FindParams, "FIND_MINSPEED", m_Calc->Settings.minSpeed, -1.0);
		ReadCalcParam(m_Calc->FindParams, "FIND_MAXSPEED", m_Calc->Settings.maxSpeed, 1.0);
		ReadCalcParam(m_Calc->FindParams, "FIND_STOP_TIME", m_Calc->Settings.Stop_Time, 1.0e7);
		ReadCalcParam(m_Calc->FindParams, "FIND_STOP_MINRELDIST", m_Calc->Settings.Stop_minRelDist, 2000.0);
		ReadCalcParam(m_Calc->FindParams, "FIND_STOP_MINMAXRELSPEED", m_Calc->Settings.Stop_minMaxRelSpeed, 0.001);
		ReadCalcParam(m_Calc->FindParams, "FIND_STOP_CURSTEP", m_Calc->Settings.Stop_CurStep, 1000.0);
		ReadCalcParam(m_Calc->FindParams, "FIND_EXCLUDEDISTANCE", m_Calc->Settings.ExcludeDistance, 1000.0);


		if (m_Calc->Settings.maxSpeed.X > 1.0)  m_Calc->Settings.maxSpeed.X = 1.0;
		if (m_Calc->Settings.maxSpeed.Y > 1.0)  m_Calc->Settings.maxSpeed.Y = 1.0;
		if (m_Calc->Settings.maxSpeed.Z > 1.0)  m_Calc->Settings.maxSpeed.Z = 1.0;
		if (m_Calc->Settings.maxSpeed.X < -1.0) m_Calc->Settings.maxSpeed.X = -1.0;
		if (m_Calc->Settings.maxSpeed.Y < -1.0) m_Calc->Settings.maxSpeed.Y = -1.0;
		if (m_Calc->Settings.maxSpeed.Z < -1.0) m_Calc->Settings.maxSpeed.Z = -1.0;

		if (m_Calc->Settings.minSpeed.X > 1.0)  m_Calc->Settings.minSpeed.X = 1.0;
		if (m_Calc->Settings.minSpeed.Y > 1.0)  m_Calc->Settings.minSpeed.Y = 1.0;
		if (m_Calc->Settings.minSpeed.Z > 1.0)  m_Calc->Settings.minSpeed.Z = 1.0;
		if (m_Calc->Settings.minSpeed.X < -1.0) m_Calc->Settings.minSpeed.X = -1.0;
		if (m_Calc->Settings.minSpeed.Y < -1.0) m_Calc->Settings.minSpeed.Y = -1.0;
		if (m_Calc->Settings.minSpeed.Z < -1.0) m_Calc->Settings.minSpeed.Z = -1.0;

		if (m_Calc->Settings.minSpeed.X > m_Calc->Settings.maxSpeed.X) m_Calc->Settings.minSpeed.X = m_Calc->Settings.maxSpeed.X;
		if (m_Calc->Settings.minSpeed.Y > m_Calc->Settings.maxSpeed.Y) m_Calc->Settings.minSpeed.Y = m_Calc->Settings.maxSpeed.Y;
		if (m_Calc->Settings.minSpeed.Z > m_Calc->Settings.maxSpeed.Z) m_Calc->Settings.minSpeed.Z = m_Calc->Settings.maxSpeed.Z;


		{
			double dblHaf = RAND_MAX * 0.5;
			double dblDiv = 1.0 / (double)RAND_MAX;
			char* tmpOutput = new char[10000];

			char fileMane[500];
			{
				char tmpCamtion[500];
				sprintf_s(tmpCamtion, "Find NP%d_C%d_ADD%d_Wind%d_Clls%X_DCT%d_ACT%d_DL%d_nc%0.3d_PT%g_%d", 
					m_Calc->curCalc.Group->GetCount(), m_Calc->curCalc.ClassicType, m_Calc->curCalc.AdditiveType, 
					m_Calc->curCalc.WindType,m_Calc->curCalc.CollisionType,m_Calc->curCalc.DensityCalcType, m_Calc->curCalc.AccCalcType,m_Calc->curCalc.DensityDerivLevel, m_Calc->stat.Count,
					m_Calc->curCalc.ParentSpaceThickness, GetTickCount());
				::SetWindowTextA( m_Calc->DlgFind->m_hWnd , tmpCamtion);
				sprintf_s(fileMane, "C:\\SParticle\\%s.txt", tmpCamtion);
			}

			for (m_Calc->stat.Count = 0; (m_Calc->FindState & M_STATE_ON); m_Calc->stat.Count++)
			{
				if (sRandSeed == -1) srand(m_Calc->stat.Count);

				{
					int nParticle = 2;
					ReadCalcParam(m_Calc->curCalc.CalcParams, "PARTICLE_COUNT", nParticle, 2);

					for (int i = 0; i < nParticle; i++)
					{
						SData initData = { 0 };
						m_Calc->curCalc.Group->AddSpParticle(SP_DATA_ALL, &initData, 1, &m_Calc->curCalc);
					}
				}

				if ((m_Calc->curCalc.OutPutFlags & 0x0001) == 0 || !fopen_s(&ptrFile, fileMane, "wt"))
				{
					m_Calc->curCalc.Group->Clear();
					m_Calc->curCalc.Time.CurTime = 0.0;

					char* tmppos = tmpOutput;
					tmppos += sprintf_s(tmppos, 1000, "Iteration\t%d\tReported\t%d\n", m_Calc->stat.Count, m_Calc->stat.Reported);
					{
						for (int i = 0; i < m_Calc->curCalc.Group->GetCount(); i++)
						{
							SData initData = { 0 };

							initData.position.X = (m_Calc->Settings.maxPos.X - m_Calc->Settings.minPos.X) * (dblHaf - (double)rand()) * dblDiv;
							initData.position.Y = (m_Calc->Settings.maxPos.Y - m_Calc->Settings.minPos.Y) * (dblHaf - (double)rand()) * dblDiv;
							initData.position.Z = (m_Calc->Settings.maxPos.Z - m_Calc->Settings.minPos.Z) * (dblHaf - (double)rand()) * dblDiv;

							if (m_Calc->curCalc.ParentSpaceThickness < 1.0)
							{
								int bDo = 1;
								while (bDo)
								{
									bDo = 0;
									for (int j = 0; j < i && !bDo; j++)
									{
										SPos cmpPos = m_Calc->curCalc.Group->GetItem(i)->GetCurPosition();
										cmpPos.X -= initData.position.X;
										cmpPos.Y -= initData.position.Y;
										cmpPos.Z -= initData.position.Z;
										if (cmpPos.LEN() + m_Calc->curCalc.ParentSpaceThickness < 1.0)
										{
											initData.position.X += 0.1;
											initData.position.Y += 0.1;
											initData.position.Z += 0.1;
											bDo = 1;
										}
									}
								}
							}

							double SpeedPwr1 = (double)rand() * dblDiv;

							initData.Speed.X = (m_Calc->Settings.maxSpeed.X - m_Calc->Settings.minSpeed.X) * (dblHaf - (double)rand()) * dblDiv;
							initData.Speed.Y = (m_Calc->Settings.maxSpeed.Y - m_Calc->Settings.minSpeed.Y) * (dblHaf - (double)rand()) * dblDiv;
							initData.Speed.Z = (m_Calc->Settings.maxSpeed.Z - m_Calc->Settings.minSpeed.Z) * (dblHaf - (double)rand()) * dblDiv;

							double Speed3dPwr1 = initData.Speed.LEN();
							if (Speed3dPwr1 == 0.0) Speed3dPwr1 = 1.0e-30;
							double mlplr = SpeedPwr1 / Speed3dPwr1;

							initData.Speed.X *= mlplr;
							initData.Speed.Y *= mlplr;
							initData.Speed.Z *= mlplr;

							m_Calc->curCalc.Group->SetParticleData(i, SP_DATA_ALL, &initData);
							tmppos = m_Calc->curCalc.Group->GetItem(i)->GetStateDescription(tmppos);
							tmppos += sprintf_s(tmppos, 1000, "\n");
						}
					}

					if (ptrFile && (m_Calc->curCalc.OutPutFlags & 0x0001)) m_Calc->curCalc.Group->WriteState(ptrFile, m_Calc->curCalc.Group->GetCurTime());
					m_Calc->curCalc.Group->CalcRelations(0.0, NULL, NULL, NULL, 0);
					m_Calc->curCalc.Group->CalcDataByCurRelations();

					if (ptrFile && (m_Calc->curCalc.OutPutFlags & 0x0001)) m_Calc->curCalc.Group->WriteState(ptrFile, m_Calc->curCalc.Group->GetCurTime());

					double tmpTimeDelta = m_Calc->curCalc.Time.MinStep;

					m_Calc->curCalc.Stat.v_Min = { 0 }; 
					m_Calc->curCalc.Stat.v_Max = { 0 };
					m_Calc->curCalc.Stat.v_MinSD = { 0 };
					m_Calc->curCalc.Stat.v_MaxSD = { 0 };
					m_Calc->curCalc.Stat.Rel_Min.CLEAR();
					m_Calc->curCalc.Stat.Rel_Max.CLEAR();

					m_Calc->curCalc.Time.CurStep = m_Calc->curCalc.Time.BaseStep;

					m_Calc->curCalc.Group->GetMinMaxData(&m_Calc->curCalc.Stat.v_Min, &m_Calc->curCalc.Stat.v_Max, &m_Calc->curCalc.Stat.v_MinSD, &m_Calc->curCalc.Stat.v_MaxSD);
					m_Calc->curCalc.Group->GetMinMaxRelation(&m_Calc->curCalc.Stat.Rel_Min, &m_Calc->curCalc.Stat.Rel_Max);
					m_Calc->curCalc.CurStep = 0;
					m_Calc->curCalc.Stat.Prev_Rel_Min.CLEAR();
					m_Calc->curCalc.Stat.Prev_Rel_Max.CLEAR();


					while (
						(m_Calc->curCalc.CurStep++ < nIter/* || m_Calc->curCalc.Group->m_maxCountLimit > 5000*/) && (m_Calc->FindState & M_STATE_ON)
						&& m_Calc->curCalc.Time.CurTime < m_Calc->Settings.Stop_Time  //Ok
						&& !(m_Calc->FindState & M_STATE_DO_NEXT)

						&& m_Calc->curCalc.Time.CurStep < m_Calc->Settings.Stop_CurStep
						&& m_Calc->curCalc.Stat.Rel_Min.Distance.MMValue < m_Calc->Settings.Stop_minRelDist
						)
					{
						{
							if (ptrFile && (m_Calc->curCalc.OutPutFlags & 0x0001)) fprintf(ptrFile, "%d\t", m_Calc->curCalc.CurStep);

							m_Calc->curCalc.Group->GetMinMaxData(&m_Calc->curCalc.Stat.v_Min, &m_Calc->curCalc.Stat.v_Max, &m_Calc->curCalc.Stat.v_MinSD, &m_Calc->curCalc.Stat.v_MaxSD);

							m_Calc->curCalc.Time.CurStep = m_Calc->curCalc.Time.BaseStep;

							//if (m_Calc->curCalc.Stat.v_MinSD.Density != 0.0) m_Calc->curCalc.Time.CurStep = m_Calc->curCalc.Time.BaseStep / (1.0 - m_Calc->curCalc.Stat.v_MinSD.Density);

							if (m_Calc->curCalc.Stat.Rel_Min.Distance.MMValue != 0.0) m_Calc->curCalc.Time.CurStep = m_Calc->curCalc.Time.BaseStep * m_Calc->curCalc.Stat.Rel_Min.Distance.MMValue * m_Calc->curCalc.Stat.Rel_Min.Distance.MMValue;



							if (m_Calc->curCalc.Time.CurStep < m_Calc->curCalc.Time.MinStep) m_Calc->curCalc.Time.CurStep = m_Calc->curCalc.Time.MinStep;
							if (tmpTimeDelta < m_Calc->curCalc.Time.CurStep) m_Calc->curCalc.Time.CurStep = tmpTimeDelta;

							if (m_Calc->curCalc.Time.CurStep > m_Calc->curCalc.Time.MaxStep)
								m_Calc->curCalc.Time.CurStep = m_Calc->curCalc.Time.MaxStep;

							double LastCurTime = m_Calc->curCalc.Group->GetCurTime();
							m_Calc->curCalc.Group->NextStep(m_Calc->curCalc.Time.CurStep);
							m_Calc->curCalc.Time.CurTime = m_Calc->curCalc.Group->GetCurTime();
							tmpTimeDelta = 10.0 * (m_Calc->curCalc.Time.CurTime - LastCurTime + 1.0e-10);
							if (ptrFile && (m_Calc->curCalc.OutPutFlags & 0x0001)) m_Calc->curCalc.Group->WriteState(ptrFile, 1, 1);

							m_Calc->curCalc.Stat.Prev_Rel_Min = m_Calc->curCalc.Stat.Rel_Min;
							m_Calc->curCalc.Stat.Prev_Rel_Max = m_Calc->curCalc.Stat.Rel_Max;

							m_Calc->curCalc.Group->GetMinMaxRelation(&m_Calc->curCalc.Stat.Rel_Min, &m_Calc->curCalc.Stat.Rel_Max);

							if (m_Calc->curCalc.Stat.Rel_Min.Distance.MMValue < 1.000002)
								m_Calc->curCalc.Stat.Rel_Min.Distance.MMValue += 0.000000000000001;

							if (m_Calc->curCalc.Group->GetCount() > 2 && m_Calc->curCalc.Stat.Rel_Max.Distance.MMValue > m_Calc->Settings.ExcludeDistance)
							{
								int idToExclude = m_Calc->curCalc.Group->GetIdByMaxDistance(m_Calc->Settings.ExcludeDistance);
								if (idToExclude >= 0)
									m_Calc->curCalc.Group->RemoveSpParticle(idToExclude);
							}
						}

					}
					if (ptrFile) fclose(ptrFile);

//					if (m_Calc->curCalc.Stat.Rel_Min.Distance.MMValue < m_Calc->Settings.Stop_minRelDist)
			//			MessageBox(m_Calc->DlgFind->m_hWnd,L"??????????",L"FFFFFFFFFFFF",MB_OK);

					if (
						(m_Calc->FindState & M_STATE_DO_REPORT) == M_STATE_DO_REPORT
						||
						(
							(
								m_Calc->curCalc.Time.CurTime >= m_Calc->Settings.Stop_Time	
								|| m_Calc->curCalc.Time.CurStep >= m_Calc->Settings.Stop_CurStep
								|| m_Calc->curCalc.CurStep >= nIter
							)
							&& (m_Calc->FindState & M_STATE_DO_NEXT) != M_STATE_DO_NEXT

							)
						)
					{
						m_Calc->stat.Reported++;
						if (!fopen_s(&ptrFile, fileMane, "at"))
						{
							fputs(tmpOutput, ptrFile);
							m_Calc->curCalc.Group->WriteState(ptrFile, 1,3);
							fclose(ptrFile);
						}
					}
					if (m_Calc->FindState) m_Calc->FindState &= (0xFFFFFFFF ^ (M_STATE_DO_NEXT| M_STATE_DO_REPORT));
				}
				m_Calc->curCalc.Group->Reset();
			}
			delete[] tmpOutput;
		}
		if (m_Calc->FindState)
		{
			m_Calc->FindState &= (0xFFFFFFFF ^ M_STATE_CALC);
			m_Calc->FindState &= (0xFFFFFFFF ^ M_STATE_ON);
			m_Calc->FindState |= M_STATE_CHANGED;
		}

	}

	return 0;
}


void PaintTraceToCDC(CDC* DCMem, SP_Calc* mData, int in_DrawFlags, SPos& delta00)
{
	if (DCMem && mData && mData->Group)
	{
		EnterCriticalSection(&mData->Group->m_cs);

		if ((in_DrawFlags & SP_DRAW_XYZ) == SP_DRAW_XYZ)
		{
			SDrawSettings oldSet = mData->DrawSet;

			if ((in_DrawFlags & SP_DRAW_CLEARBKG))
				DCMem->FillSolidRect(0, 0, (int)mData->DrawSet.BMP_Size.X, (int)mData->DrawSet.BMP_Size.Y, RGB(255, 255, 255));

			mData->DrawSet.ViewMltpl *= 0.5;
			mData->DrawSet.BMP_Size.X *= 0.5;
			mData->DrawSet.BMP_Size.Y *= 0.5;

			in_DrawFlags &= (0xFFFFFFFF ^ (SP_DRAW_XYZ | SP_DRAW_TEXT | SP_DRAW_CLEARBKG));

			mData->DrawSet.m_00_Pos.X = (mData->DrawSet.BMP_Size.X - (mData->DrawSet.PosMax.X - mData->DrawSet.PosMin.X) * mData->DrawSet.ViewMltpl) * 0.5 - oldSet.PosMin.X * mData->DrawSet.ViewMltpl;
			mData->DrawSet.m_00_Pos.Y = (mData->DrawSet.BMP_Size.Y - (mData->DrawSet.PosMax.Y - mData->DrawSet.PosMin.Y) * mData->DrawSet.ViewMltpl) * 0.5 - oldSet.PosMin.Y * mData->DrawSet.ViewMltpl;
			PaintTraceToCDC(DCMem, mData, in_DrawFlags | SP_DRAW_XY | SP_DRAW_TEXT, delta00);

			//mData->DrawSet.m_00_Pos.X += mData->DrawSet.BMP_Size.X;

			mData->DrawSet.m_00_Pos.X = mData->DrawSet.BMP_Size.X + (mData->DrawSet.BMP_Size.X - (mData->DrawSet.PosMax.Z - mData->DrawSet.PosMin.Z) * mData->DrawSet.ViewMltpl) * 0.5 - oldSet.PosMin.Z * mData->DrawSet.ViewMltpl;
			mData->DrawSet.m_00_Pos.Y = (mData->DrawSet.BMP_Size.Y - (mData->DrawSet.PosMax.Y - mData->DrawSet.PosMin.Y) * mData->DrawSet.ViewMltpl) * 0.5 - oldSet.PosMin.Y * mData->DrawSet.ViewMltpl;
			PaintTraceToCDC(DCMem, mData, in_DrawFlags | SP_DRAW_YZ, delta00);


			//mData->DrawSet.m_00_Pos.X -= mData->DrawSet.BMP_Size.X;
			//mData->DrawSet.m_00_Pos.Y += mData->DrawSet.BMP_Size.Y;


			mData->DrawSet.m_00_Pos.X = (mData->DrawSet.BMP_Size.X - (mData->DrawSet.PosMax.X - mData->DrawSet.PosMin.X) * mData->DrawSet.ViewMltpl) * 0.5 - oldSet.PosMin.X * mData->DrawSet.ViewMltpl;
			mData->DrawSet.m_00_Pos.Y = mData->DrawSet.BMP_Size.Y + (mData->DrawSet.BMP_Size.Y - (mData->DrawSet.PosMax.Z - mData->DrawSet.PosMin.Z) * mData->DrawSet.ViewMltpl) * 0.5 - oldSet.PosMin.Z * mData->DrawSet.ViewMltpl;

			PaintTraceToCDC(DCMem, mData, in_DrawFlags | SP_DRAW_XZ, delta00);

			mData->DrawSet = oldSet;
		}
		else
		{
			int idxCrd1 = 0, idxCrd2 = 1;
			if ((in_DrawFlags & SP_DRAW_XYZ) == SP_DRAW_XZ) idxCrd2 = 2;
			else
				if ((in_DrawFlags & SP_DRAW_XYZ) == SP_DRAW_YZ) { idxCrd1 = 2; idxCrd2 = 1;}

			DCMem->SetBkMode(TRANSPARENT);

			if ((in_DrawFlags & SP_DRAW_CLEARBKG))
				DCMem->FillSolidRect(0, 0, (int)mData->DrawSet.BMP_Size.X, (int)mData->DrawSet.BMP_Size.Y, RGB(255, 255, 255));
		
			DCMem->SetBkColor(RGB(255, 255, 255));
			DCMem->SetTextColor(RGB(0, 0, 0));

			try
			{
				int nTrace = mData->Group->m_Count;
				if (nTrace > 1 && mData->Group->m_Array[0] && mData->Group->m_Array[0]->m_Data)
				{
					HDC hDC = DCMem->m_hDC;

					if ((in_DrawFlags & SP_DRAW_TEXT))
					{
						char tmps[300];
						SData* pData = mData->Group->m_Array[0]->m_Data->m_Array;
						int nPoints = mData->Group->m_Array[0]->m_Data->CurPos + 1;
						int txtlen = sprintf_s(tmps, 300, "nPoints = %d", nPoints);
						TextOutA(hDC, 1, 10, tmps, txtlen);
						txtlen = sprintf_s(tmps, 300, "Time = %15.15g  | TimeStep = %15.15g ", pData[nPoints - 1].TimePoint, mData->Group->m_pCalc->Time.CurStep);
						TextOutA(hDC, 1, 30, tmps, txtlen);

						int iLastY = 0;
						try
						{

						for (int i = 0; i < nTrace; i++)
						{
							pData = mData->Group->m_Array[i]->m_Data->m_Array;
							nPoints = mData->Group->m_Array[i]->m_Data->CurPos + 1;
							if (nPoints > 0)
							{
								DCMem->SetTextColor(mRGB[(i + 8) % 16]);
								TextOutA(hDC, 1, 50 + i * 50, "Speed", 5);
								DCMem->SetTextColor(RGB(0, 0, 0));

								txtlen = sprintf_s(tmps, 300, "%g | X=%g Y=%g Z=%g",
									sqrt(pData[nPoints - 1].Speed.X * pData[nPoints - 1].Speed.X + pData[nPoints - 1].Speed.Y * pData[nPoints - 1].Speed.Y + pData[nPoints - 1].Speed.Z * pData[nPoints - 1].Speed.Z)
									, pData[nPoints - 1].Speed.X, pData[nPoints - 1].Speed.Y, pData[nPoints - 1].Speed.Z);
								TextOutA(hDC, 60, 50 + i * 50, tmps, txtlen);


								DCMem->SetTextColor(mRGB[(i + 8) % 16]);
								TextOutA(hDC, 1, 70 + i * 50, "Density", 7);
								DCMem->SetTextColor(RGB(0, 0, 0));

								txtlen = sprintf_s(tmps, 300, "%g | X=%g Y=%g Z=%g",
									sqrt(pData[nPoints - 1].Accel3D.X * pData[nPoints - 1].Accel3D.X + pData[nPoints - 1].Accel3D.Y * pData[nPoints - 1].Accel3D.Y + pData[nPoints - 1].Accel3D.Z * pData[nPoints - 1].Accel3D.Z)
									, pData[nPoints - 1].Accel3D.X, pData[nPoints - 1].Accel3D.Y, pData[nPoints - 1].Accel3D.Z);
								TextOutA(hDC, 60, iLastY = 70 + i * 50, tmps, txtlen);
							}
						}
						}
						catch (...)
						{
							;
						}

						SMinMaxRelation out_Min, out_Max;
						mData->Group->GetMinMaxRelation(&out_Min, &out_Max);

						txtlen = sprintf_s(tmps, 300, "MIN RELATIVE   Distance = %g | Speed = %g", out_Min.Distance.MMValue, sqrt(out_Min.Speed.MMValue));
						TextOutA(hDC, 1, iLastY + 50, tmps, txtlen);
						txtlen = sprintf_s(tmps, 300, "MAX RELATIVE   Distance = %g | Speed = %g", out_Max.Distance.MMValue, sqrt(out_Max.Speed.MMValue));
						TextOutA(hDC, 1, iLastY + 70, tmps, txtlen);
					}

					if (mData->DrawSet.ShowLinksTo && mData->DrawSet.nLinks > 2)
					for (int idParticle = 0; idParticle < nTrace; idParticle++)
					{
						int Pwr2Id = 1 << idParticle;

						if ((mData->DrawSet.ShowItems & Pwr2Id) && (Pwr2Id & mData->DrawSet.ShowLinksFrom))
						{
							HPEN PenLink = CreatePen(PS_SOLID, 1, mRGB[(idParticle + 8) % 16]);
							SelectObject(hDC, PenLink);

							SData* pData = mData->Group->m_Array[idParticle]->m_Data->m_Array;
							if (mData->DrawSet.nLinks == 0) mData->DrawSet.nLinks = 40;

							int StopIdx = mData->Group->m_Array[idParticle]->m_Data->CurPos + 1;
							if (StopIdx > mData->DrawSet.ViewPoint.EndIdx) StopIdx = mData->DrawSet.ViewPoint.EndIdx;

							int nPoints = mData->DrawSet.ViewPoint.GetCount(mData->Group->m_Array[idParticle]->m_Data->CurPos);
							int ViewStep = nPoints / mData->DrawSet.nLinks;
							if (ViewStep < 1) ViewStep = 1;

							double PrevTime = 0.0;

							for (int Itemidx = mData->DrawSet.ViewPoint.StartIdx; Itemidx < StopIdx; Itemidx += ViewStep)
							{
								int posC1 = (int)(mData->DrawSet.m_00_Pos.X + delta00.X + (double)(((double*)(&pData[Itemidx].position))[idxCrd1] * mData->DrawSet.ViewMltpl));
								int posC2 = (int)(mData->DrawSet.m_00_Pos.Y + delta00.Y + (double)(((double*)(&pData[Itemidx].position))[idxCrd2] * mData->DrawSet.ViewMltpl));


								for (int LinkToPtr = 0; LinkToPtr < nTrace; LinkToPtr++)
									if (idParticle != LinkToPtr && ((1 << LinkToPtr) & mData->DrawSet.ShowLinksTo))
									{
										SData VP_Data = { 0 };
										if (Itemidx == mData->DrawSet.ViewPoint.StartIdx)
											PrevTime = pData[Itemidx].TimePoint * 3.0 - pData[Itemidx + ViewStep].TimePoint * 2.0;

										if ((mData->Group->m_Array[LinkToPtr]->FindViewPoint(&VP_Data, pData + Itemidx, pData[Itemidx].TimePoint, PrevTime, pData[Itemidx].TimePoint) & SP_STATE_ERROR_MINOR) != SP_STATE_ERROR_MAIN)
										{
											PrevTime = VP_Data.TimePoint;
											MoveToEx(hDC, posC1, posC2, NULL);
											LineTo(hDC, 
												(int)(mData->DrawSet.m_00_Pos.X + delta00.X + (((double*)(&VP_Data.position))[idxCrd1] * mData->DrawSet.ViewMltpl)),
												(int)(mData->DrawSet.m_00_Pos.Y + delta00.Y + (((double*)(&VP_Data.position))[idxCrd2] * mData->DrawSet.ViewMltpl)));
										}
									}
							}
							DeleteObject(PenLink);
						}
					}

					for (int idParticle = 0; idParticle < nTrace; idParticle++)
					{
						int Pwr2Id = 1 << idParticle;

						if ((mData->DrawSet.ShowItems & Pwr2Id))
						{
							int nPoints = mData->DrawSet.ViewPoint.GetCount(mData->Group->m_Array[idParticle]->m_Data->CurPos);
							//int nPoints = mData->Group->m_Array[idParticle]->m_Data->CurPos + 1;
							HPEN PenPtr = CreatePen(PS_SOLID, 1, mRGB[idParticle % 16]);

							SData* pData = mData->Group->m_Array[idParticle]->m_Data->m_Array;

							int iStep = nPoints >> 14;
							if (iStep < 1) iStep = 1;
							SelectObject(hDC, PenPtr);

							int StartIdx = mData->DrawSet.ViewPoint.StartIdx;

							int posC1 = (int)(mData->DrawSet.m_00_Pos.X + delta00.X + (double)(((double*)(&pData[StartIdx].position))[idxCrd1] * mData->DrawSet.ViewMltpl));
							int posC2 = (int)(mData->DrawSet.m_00_Pos.Y + delta00.Y + (double)(((double*)(&pData[StartIdx].position))[idxCrd2] * mData->DrawSet.ViewMltpl));
							int Last1 = -posC1, Last2 = -posC2;


							MoveToEx(hDC, posC1 + 2, posC2 + 2, NULL);
							LineTo(hDC, posC1 - 2, posC2 + 2);
							//MoveToEx(hDC, posC1 - 2, posC2 + 2, NULL);
							LineTo(hDC, posC1 + 2, posC2 - 2);
							//MoveToEx(hDC, posC1 + 2, posC2 - 2, NULL);
							LineTo(hDC, posC1 - 2, posC2 - 2);
							//MoveToEx(hDC, posC1 - 2, posC2 - 2, NULL);
							LineTo(hDC, posC1 + 2, posC2 + 2);

							int StopIdx = mData->Group->m_Array[idParticle]->m_Data->CurPos + 1;
							if (StopIdx > mData->DrawSet.ViewPoint.EndIdx) StopIdx = mData->DrawSet.ViewPoint.EndIdx;

							for (int i = StartIdx + 1; i < StopIdx; i += iStep)
							{
								posC1 = (int)(mData->DrawSet.m_00_Pos.X + delta00.X + (double)(((double*)(&pData[i].position))[idxCrd1] * mData->DrawSet.ViewMltpl));
								posC2 = (int)(mData->DrawSet.m_00_Pos.Y + delta00.Y + (double)(((double*)(&pData[i].position))[idxCrd2] * mData->DrawSet.ViewMltpl));

								if (posC1 != Last1 || posC2 != Last2)
								{
									LineTo(hDC, posC1, posC2);
									//MoveToEx(hDC, posC1, posC2, NULL);
									Last1 = posC1;
									Last2 = posC2;
								}
							}
							DeleteObject(PenPtr);
						}
					}
				}
			}
			catch (...) { ; }

		}

		LeaveCriticalSection(&mData->Group->m_cs);
	}


}

void SaveBitmap(char* in_FileName, HDC in_hdc, HBITMAP in_hBMP)
{
	FILE* ptrFile;
	if (in_FileName && *in_FileName && in_hdc && in_hBMP && !fopen_s(&ptrFile, in_FileName, "wb"))
	{
		BITMAPINFO bmi = { 0 };
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

		if (GetDIBits(in_hdc, in_hBMP, 0, 0, NULL, &bmi, DIB_RGB_COLORS))
		{
			HANDLE hDIB = NULL;
			bmi.bmiHeader.biCompression = BI_RGB;

			if ((hDIB = GlobalAlloc(GHND, bmi.bmiHeader.biSizeImage)))
			{
				char* lpbitmap = (char*)GlobalLock(hDIB);

				if (GetDIBits(in_hdc, in_hBMP, 0, bmi.bmiHeader.biHeight, lpbitmap, &bmi, DIB_RGB_COLORS))
				{
					BITMAPFILEHEADER bmfh = { 0 };
					bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + bmi.bmiHeader.biSize;
					bmfh.bfSize = bmfh.bfOffBits + bmi.bmiHeader.biSizeImage;
					bmfh.bfType = 'B' + ('M' << 8);

					//Write the bitmap file header
					size_t nWrittenFileHeaderSize = fwrite(&bmfh, 1, sizeof(BITMAPFILEHEADER), ptrFile);
					//And then the bitmap info header
					size_t nWrittenInfoHeaderSize = fwrite(&bmi.bmiHeader, 1, sizeof(BITMAPINFOHEADER), ptrFile);
					//Finally, write the image data itself
					size_t nWrittenDIBDataSize = fwrite(lpbitmap, 1, bmi.bmiHeader.biSizeImage, ptrFile);
				}
				// Unlock and Free the DIB from the heap.
				GlobalUnlock(hDIB);
				GlobalFree(hDIB);
			}
		}
		fclose(ptrFile);
	}
	return;
}

