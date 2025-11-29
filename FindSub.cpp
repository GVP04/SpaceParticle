#include "pch.h"
#include "CommonFunc.h"
#include "SpaceOM.h"
#include "SpaceOMDlg.h"

#include "CSP_Group.h"
#include "CDrawDlg.h"
#include "CFindDlg.h"

#include "CSpParticle.h"
#include "CSDataArray.h"


int mRGB[16] = {
	RGB(200, 0, 0),RGB(0, 200, 0),RGB(0, 0, 200),RGB(200, 200, 0),RGB(200, 0, 200),RGB(0, 200, 200),
	RGB(200, 100, 0),RGB(200, 0, 100),RGB(200, 100, 100),RGB(100, 200, 0),RGB(0, 200, 100),RGB(100, 0, 200),
	RGB(0, 100, 200),RGB(0, 100, 100),RGB(100, 0, 100),RGB(100, 100, 0),
};

int mRGBDark[16] = {
	RGB(111, 0, 0),RGB(0, 111, 0),RGB(0, 0, 111),RGB(111, 111, 0),RGB(111, 0, 111),RGB(0, 111, 111),
	RGB(111, 51, 0),RGB(111, 0, 51),RGB(111, 51, 51),RGB(51, 111, 0),RGB(0, 111, 51),RGB(51, 0, 111),
	RGB(0, 51, 111),RGB(0, 51, 51),RGB(51, 0, 51),RGB(51, 51, 0),
};


UINT DoCalc_1(LPVOID pParam)
{
	if (pParam)
	{
		SP_Calc* m_Calc = (SP_Calc*)pParam;
		FILE* ptrFile = NULL;

		m_Calc->Group->Reset();
		m_Calc->Time.BaseStep = 0.002;
		m_Calc->Time.MinStep = SP_PREC_TIME_MINSTEP;
		m_Calc->Time.MaxStep = 1000;

		m_Calc->Group->SetCalcState(M_STATE_CALC | M_STATE_READY);

		m_Calc->Group->ReadSettingsGroup(m_Calc->CalcParams, SGROUP_ALL);


		if ((m_Calc->OutPutFlags & 0x0001) == 0 || !fopen_s(&ptrFile, "C:\\SParticle\\report.txt", "wt"))
		{
			for (int i = 0; i < m_Calc->InitialParticleCount; i++)
			{
				SData initData = { 0 };
				m_Calc->Group->AddSpParticle(SP_DATA_ALL, &initData, 1, m_Calc);
			}

			if (m_Calc->sRandSeed_Calc < 0)
			{
				for (int i = 0; i < m_Calc->Group->GetCount(); i++)
				{
					char tmps[500];
					SData initData = { 0 };
					sprintf_s(tmps, 500, "p%d_POS", i);
					ReadCalcParam(m_Calc->CalcParams, tmps, initData.position, 0.0);
					sprintf_s(tmps, 500, "p%d_SPEED", i);
					ReadCalcParam(m_Calc->CalcParams, tmps, initData.Speed, 0.0000001);

					m_Calc->Group->SetParticleData(i, SP_DATA_ALL, &initData);
				}
			}
			else
			{
				srand(m_Calc->sRandSeed_Calc);

				double dblHaf = RAND_MAX * 0.5;
				double dblDiv = 1.0 / (double)RAND_MAX;
				char* tmpOutput = new char[10000];

				m_Calc->Group->Clear();
				m_Calc->Time.CurTime = 0.0;

				for (int i = 0; i < m_Calc->Group->GetCount(); i++)
				{
					SData initData = { 0 };

					initData.position.X = (m_Calc->CalcFindSet.maxPos.X - m_Calc->CalcFindSet.minPos.X) * (dblHaf - (double)rand()) * dblDiv;
					initData.position.Y = (m_Calc->CalcFindSet.maxPos.Y - m_Calc->CalcFindSet.minPos.Y) * (dblHaf - (double)rand()) * dblDiv;
					initData.position.Z = (m_Calc->CalcFindSet.maxPos.Z - m_Calc->CalcFindSet.minPos.Z) * (dblHaf - (double)rand()) * dblDiv;

					if (m_Calc->ParentSpaceThickness < 1.0)
					{
						int bDo = 1;
						while (bDo)
						{
							bDo = 0;
							for (int j = 0; j < i && !bDo; j++)
							{
								SPos cmpPos = m_Calc->Group->GetItem(i)->GetCurPosition();
								cmpPos.X -= initData.position.X;
								cmpPos.Y -= initData.position.Y;
								cmpPos.Z -= initData.position.Z;
								if (cmpPos.LEN() + m_Calc->ParentSpaceThickness < 1.0)
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

					initData.Speed.X = (m_Calc->CalcFindSet.maxSpeed.X - m_Calc->CalcFindSet.minSpeed.X) * (dblHaf - (double)rand());
					initData.Speed.Y = (m_Calc->CalcFindSet.maxSpeed.Y - m_Calc->CalcFindSet.minSpeed.Y) * (dblHaf - (double)rand());
					initData.Speed.Z = (m_Calc->CalcFindSet.maxSpeed.Z - m_Calc->CalcFindSet.minSpeed.Z) * (dblHaf - (double)rand());

					initData.Speed.SET_LENGHT(SpeedPwr1);

					m_Calc->Group->SetParticleData(i, SP_DATA_ALL, &initData);
				}
			}

			if (ptrFile && (m_Calc->OutPutFlags & 0x0001)) m_Calc->Group->WriteState(ptrFile, m_Calc->Group->GetCurTime());

			m_Calc->Group->CalcRelations(0.0, NULL, NULL, NULL, 0);
			m_Calc->Group->CalcDataByCurRelations();

			if (ptrFile && (m_Calc->OutPutFlags & 0x0001)) m_Calc->Group->WriteState(ptrFile, m_Calc->Group->GetCurTime());

			int i = 0;

			double tmpTimeDelta = m_Calc->Time.MinStep;

			while ((i++ < m_Calc->nIterations || m_Calc->Group->m_maxCountLimit > 5000) && (m_Calc->Group->GetCalcState() & M_STATE_CALC) && !(m_Calc->Group->GetCalcState() & M_STATE_BREAK))
				if (!m_Calc->Group->DoPause())
				{
					m_Calc->Group->DoDelay();
					EnterCriticalSection(&m_Calc->Group->m_cs_Command);
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

						if (m_Calc->CalcFindSet.StopIF.minRelDist < r_Min.Distance.MMValue)
							m_Calc->Group->ClearCalcState(M_STATE_CALC);
					}
					LeaveCriticalSection(&m_Calc->Group->m_cs_Command);
				}

			EnterCriticalSection(&m_Calc->Group->m_cs);

			if (ptrFile) fclose(ptrFile);

			m_Calc->Group->ClearCalcState(M_STATE_CALC);
			m_Calc->Group->SetCalcState(M_STATE_CHANGED);

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
		m_Calc->stat = { 0 };
		m_Calc->curCalc.Group->SetCalcState(M_STATE_CALC);

		m_Calc->curCalc.Group->ReadSettingsGroup(m_Calc->FindParams, SGROUP_ALL);

		if (m_Calc->curCalc.Group->m_maxCountLimit < 10000)
		{
			m_Calc->curCalc.Group->m_maxCountLimit = m_Calc->curCalc.nIterations;
			if (m_Calc->curCalc.Group->m_maxCountLimit < 10000)
				m_Calc->curCalc.Group->m_maxCountLimit = 500000;
		}

		m_Calc->curCalc.Group->SetCalcState(M_STATE_READY);

		{
			double dblHaf = RAND_MAX * 0.5;
			double dblDiv = 1.0 / (double)RAND_MAX;
			char* tmpOutput = new char[10000];

			char fileMane[500];
			{
				char tmpCamtion[500];
				sprintf_s(tmpCamtion, "Find NP%d_C%d_ADD%d_Wind%d_Clls%X_DCT%d_ACT%d_DL%d_nc%0.3d_PT%g_%d",
					m_Calc->curCalc.Group->GetCount(), m_Calc->curCalc.ClassicType, m_Calc->curCalc.AdditiveType,
					m_Calc->curCalc.WindType, m_Calc->curCalc.CollisionType, m_Calc->curCalc.DensityCalcType, m_Calc->curCalc.AccCalcType, m_Calc->curCalc.DensityDerivLevel, m_Calc->stat.Count,
					m_Calc->curCalc.ParentSpaceThickness, GetTickCount());
				::SetWindowTextA(m_Calc->DlgFind->m_hWnd, tmpCamtion);
				sprintf_s(fileMane, "C:\\SParticle\\%s.txt", tmpCamtion);
			}

			for (m_Calc->stat.Count = 0; (m_Calc->curCalc.Group->GetCalcState() & M_STATE_CALC) && !(m_Calc->curCalc.Group->GetCalcState() & M_STATE_BREAK); m_Calc->stat.Count++)
			{
				if (m_Calc->sRandSeed == -1) srand(m_Calc->stat.Count);
				m_Calc->curCalc.Group->ClearCalcState(M_STATE_READY);
				EnterCriticalSection(&m_Calc->curCalc.Group->m_cs);
				EnterCriticalSection(&m_Calc->curCalc.Group->m_cs_Command);

				for (int i = 0; i < m_Calc->curCalc.InitialParticleCount; i++)
				{
					SData initData = { 0 };
					m_Calc->curCalc.Group->AddSpParticle(SP_DATA_ALL, &initData, 1, &m_Calc->curCalc);
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

							initData.Speed.SET_LENGHT(SpeedPwr1);

							m_Calc->curCalc.Group->SetParticleData(i, SP_DATA_ALL, &initData);
							tmppos = m_Calc->curCalc.Group->GetItem(i)->GetStateDescription(tmppos);
							tmppos += sprintf_s(tmppos, 1000, "\n");
						}
					}

					m_Calc->curCalc.Group->SetCalcState(M_STATE_READY);

					if (ptrFile && (m_Calc->curCalc.OutPutFlags & 0x0001)) m_Calc->curCalc.Group->WriteState(ptrFile, m_Calc->curCalc.Group->GetCurTime());
					m_Calc->curCalc.Group->CalcRelations(0.0, NULL, NULL, NULL, 0);
					m_Calc->curCalc.Group->CalcDataByCurRelations();

					if (ptrFile && (m_Calc->curCalc.OutPutFlags & 0x0001)) m_Calc->curCalc.Group->WriteState(ptrFile, m_Calc->curCalc.Group->GetCurTime());

					double tmpTimeDelta = m_Calc->curCalc.Time.MinStep;

					m_Calc->curCalc.Stat.CLEAR();

					m_Calc->curCalc.Time.CurStep = m_Calc->curCalc.Time.BaseStep;

					m_Calc->curCalc.Group->GetMinMaxData(&m_Calc->curCalc.Stat.v_Min, &m_Calc->curCalc.Stat.v_Max, &m_Calc->curCalc.Stat.v_MinSD, &m_Calc->curCalc.Stat.v_MaxSD);
					m_Calc->curCalc.Group->GetMinMaxRelation(&m_Calc->curCalc.Stat.Rel_Min, &m_Calc->curCalc.Stat.Rel_Max);
					m_Calc->curCalc.CurStep = 0;

					LeaveCriticalSection(&m_Calc->curCalc.Group->m_cs_Command);
					LeaveCriticalSection(&m_Calc->curCalc.Group->m_cs);



					while (
						(m_Calc->curCalc.CurStep++ < m_Calc->Settings.StopIF.Iteration/* || m_Calc->curCalc.Group->m_maxCountLimit > 5000*/)
						&& (m_Calc->curCalc.Group->GetCalcState() & M_STATE_CALC)
						&& !(m_Calc->curCalc.Group->GetCalcState() & M_STATE_BREAK)
						&& m_Calc->curCalc.Time.CurTime < m_Calc->Settings.StopIF.Time  //Ok
						&& !(m_Calc->curCalc.Group->GetCalcState() & M_STATE_DO_NEXT)

						&& m_Calc->curCalc.Time.CurStep < m_Calc->Settings.StopIF.TimeStep
						&& m_Calc->curCalc.Stat.Rel_Min.Distance.MMValue < m_Calc->Settings.StopIF.minRelDist
						)
						if (!m_Calc->curCalc.Group->DoPause())
						{
							EnterCriticalSection(&m_Calc->curCalc.Group->m_cs_Command);
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
							LeaveCriticalSection(&m_Calc->curCalc.Group->m_cs_Command);
							m_Calc->curCalc.Group->DoDelay();
						}
					if (ptrFile) fclose(ptrFile);

					//					if (m_Calc->curCalc.Stat.Rel_Min.Distance.MMValue < m_Calc->Settings.StopIF.minRelDist)
								//			MessageBox(m_Calc->DlgFind->m_hWnd,L"??????????",L"FFFFFFFFFFFF",MB_OK);

					if (
						(m_Calc->curCalc.Group->GetCalcState() & M_STATE_DO_REPORT) == M_STATE_DO_REPORT
						||
						(
							(
								m_Calc->curCalc.Time.CurTime >= m_Calc->Settings.StopIF.Time
								|| m_Calc->curCalc.Time.CurStep >= m_Calc->Settings.StopIF.TimeStep
								|| m_Calc->curCalc.CurStep >= m_Calc->Settings.StopIF.Iteration
							)
							&& (m_Calc->curCalc.Group->GetCalcState() & M_STATE_DO_NEXT) != M_STATE_DO_NEXT

							)
						)
					{
						m_Calc->stat.Reported++;
						if (!fopen_s(&ptrFile, fileMane, "at"))
						{
							fputs(tmpOutput, ptrFile);
							m_Calc->curCalc.Group->WriteState(ptrFile, 1, 3);
							fclose(ptrFile);
						}
					}
					m_Calc->curCalc.Group->ClearCalcState(M_STATE_DO_NEXT | M_STATE_DO_REPORT);
				}
				EnterCriticalSection(&m_Calc->curCalc.Group->m_cs);
				int tmpState = m_Calc->curCalc.Group->GetCalcState();
				m_Calc->curCalc.Group->Reset();
				m_Calc->curCalc.Group->SetCalcState(tmpState);
				m_Calc->curCalc.Group->ClearCalcState(M_STATE_READY);
				LeaveCriticalSection(&m_Calc->curCalc.Group->m_cs);
			}
			delete[] tmpOutput;
		}

		m_Calc->curCalc.Group->ClearCalcState(M_STATE_CALC);
		m_Calc->curCalc.Group->SetCalcState(M_STATE_CHANGED);
	}

	return 0;
}

void PaintTraceToCDC(CDC* DCMem, SP_Calc* mData, int in_DrawFlags, SPos& delta00)
{
	if (DCMem && DCMem->m_hDC && mData && mData->Group)
	{
		EnterCriticalSection(&mData->Group->m_cs);

		if ((in_DrawFlags & SP_DRAW_XYZ) == SP_DRAW_XYZ)
		{
			SDrawSettings oldSet = mData->DrawSet;
			HDC hDC = DCMem->m_hDC;

			if ((in_DrawFlags & SP_DRAW_CLEARBKG))
				DCMem->FillSolidRect(0, 0, (int)mData->DrawSet.BMP_Size.X, (int)mData->DrawSet.BMP_Size.Y, RGB(255, 255, 255));

			mData->DrawSet.ViewMltpl *= 0.5;
			mData->DrawSet.BMP_Size.X *= 0.5;
			mData->DrawSet.BMP_Size.Y *= 0.5;

			int LenLine = 5, LenStep = 9;
			HPEN PenPtr = CreatePen(PS_SOLID, 1, 0x808080);
			SelectObject(hDC, PenPtr);
			int coordConst = (int)(delta00.Y + mData->DrawSet.BMP_Size.Y);
			int coordStart = (int)delta00.X + LenLine;

			for (int i = 0; i < oldSet.BMP_Size.X; i += LenStep)
			{
				MoveToEx(hDC, (int)delta00.X + i, coordConst, NULL);
				LineTo(hDC, coordStart + i, coordConst);
			}

			coordConst = (int)(delta00.X + mData->DrawSet.BMP_Size.X);
			coordStart = (int)delta00.Y + LenLine;
			for (int i = 0; i < oldSet.BMP_Size.Y; i += LenStep)
			{
				MoveToEx(hDC, coordConst, (int)delta00.Y + i, NULL);
				LineTo(hDC, coordConst, coordStart + i);
			}
			DeleteObject(PenPtr);

			in_DrawFlags &= (0xFFFFFFFF ^ (SP_DRAW_XYZ | SP_DRAW_CLEARBKG));

			mData->DrawSet.m_00_Pos.X = (mData->DrawSet.BMP_Size.X - (mData->DrawSet.PosMax.X - mData->DrawSet.PosMin.X) * mData->DrawSet.ViewMltpl) * 0.5 - oldSet.PosMin.X * mData->DrawSet.ViewMltpl;
			mData->DrawSet.m_00_Pos.Y = (mData->DrawSet.BMP_Size.Y - (mData->DrawSet.PosMax.Y - mData->DrawSet.PosMin.Y) * mData->DrawSet.ViewMltpl) * 0.5 - oldSet.PosMin.Y * mData->DrawSet.ViewMltpl;
			PaintTraceToCDC(DCMem, mData, in_DrawFlags | SP_DRAW_XY, delta00);

			in_DrawFlags &= (0xFFFFFFFF ^ (SP_DRAW_TEXT));

			mData->DrawSet.m_00_Pos.X = mData->DrawSet.BMP_Size.X + (mData->DrawSet.BMP_Size.X - (mData->DrawSet.PosMax.Z - mData->DrawSet.PosMin.Z) * mData->DrawSet.ViewMltpl) * 0.5 - oldSet.PosMin.Z * mData->DrawSet.ViewMltpl;
			mData->DrawSet.m_00_Pos.Y = (mData->DrawSet.BMP_Size.Y - (mData->DrawSet.PosMax.Y - mData->DrawSet.PosMin.Y) * mData->DrawSet.ViewMltpl) * 0.5 - oldSet.PosMin.Y * mData->DrawSet.ViewMltpl;
			PaintTraceToCDC(DCMem, mData, in_DrawFlags | SP_DRAW_YZ, delta00);

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
				if ((in_DrawFlags & SP_DRAW_XYZ) == SP_DRAW_YZ) { idxCrd1 = 2; idxCrd2 = 1; }

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
					//ќтрисовка параметров группы
					if ((in_DrawFlags & SP_DRAW_GROUPINFO) && mData->DrawSet.ShowGroupArrows)
					{
						HPEN PenGrSpeed = CreatePen(PS_DOT, 1, 0x80A080);
						HPEN PenGrAcc = CreatePen(PS_DOT, 1, 0xA08080);

						if (mData->DrawSet.nLinks == 0) mData->DrawSet.nLinks = 10;

						int nPoints = mData->DrawSet.ViewPoint.GetCount(mData->Group->m_Array[0]->m_Data->CurPos);
						int ViewStep = nPoints / mData->DrawSet.nLinks;
						if (ViewStep < 1) ViewStep = 1;

						GroupSummaryInfo GrInfo;
						int StopIdx = mData->Group->m_Array[0]->m_Data->CurPos + 1;
						if (StopIdx > mData->DrawSet.ViewPoint.EndIdx) StopIdx = mData->DrawSet.ViewPoint.EndIdx;

						for (int Itemidx = mData->DrawSet.ViewPoint.StartIdx; Itemidx < StopIdx; Itemidx += ViewStep)
						{
							mData->Group->GetGroupSummaryInfo(&GrInfo, Itemidx);
							int posSX = (int)(mData->DrawSet.m_00_Pos.X + delta00.X + (double)(((double*)(&GrInfo.Position))[idxCrd1] * mData->DrawSet.ViewMltpl));
							int posSY = (int)(mData->DrawSet.m_00_Pos.Y + delta00.Y + (double)(((double*)(&GrInfo.Position))[idxCrd2] * mData->DrawSet.ViewMltpl));

							if ((mData->DrawSet.ShowGroupArrows & SP_DRFLG_GROUP_SPEED))
							{//GroupSpeed
								SPos endPoint;
								endPoint = GrInfo.Speed_3D;
								endPoint.MULT(mData->DrawSet.ViewMltpl);
								endPoint.ADD(GrInfo.Position);

								int posEX = (int)(mData->DrawSet.m_00_Pos.X + delta00.X + (((double*)(&endPoint))[idxCrd1] * mData->DrawSet.ViewMltpl));
								int posEY = (int)(mData->DrawSet.m_00_Pos.Y + delta00.Y + (((double*)(&endPoint))[idxCrd2] * mData->DrawSet.ViewMltpl));

								SelectObject(hDC, PenGrSpeed);
								DrawArrow(hDC, posSX, posSY, posEX, posEY, 5);
							}
							if ((mData->DrawSet.ShowGroupArrows & SP_DRFLG_GROUP_ACC))
							{//GroupAcc
								SPos endPoint;
								endPoint = GrInfo.Acceleration_3D;
								endPoint.MULT(mData->DrawSet.ViewMltpl*10.0);
								endPoint.ADD(GrInfo.Position);

								int posEX = (int)(mData->DrawSet.m_00_Pos.X + delta00.X + (((double*)(&endPoint))[idxCrd1] * mData->DrawSet.ViewMltpl));
								int posEY = (int)(mData->DrawSet.m_00_Pos.Y + delta00.Y + (((double*)(&endPoint))[idxCrd2] * mData->DrawSet.ViewMltpl));

								SelectObject(hDC, PenGrAcc);
								DrawArrow(hDC, posSX, posSY, posEX, posEY, 5);
							}
						}
						DeleteObject(PenGrAcc);
						DeleteObject(PenGrSpeed);
					}

					//отрисовка векторов скорости и ускорени€ 
					if (mData->DrawSet.ShowSpeedFor || mData->DrawSet.ShowAccFor)
					{
						GroupSummaryInfo GrInfo;

						SPos delta = mData->DrawSet.PosMax;
						delta.MINUS(mData->DrawSet.PosMin);
						double ArrowsMagn = ((double*)(&delta))[idxCrd1];
						if (ArrowsMagn > ((double*)(&delta))[idxCrd2]) ArrowsMagn = ((double*)(&delta))[idxCrd2];


						for (int idParticle = 0; idParticle < nTrace; idParticle++)
						{
							int Pwr2Id = 1 << idParticle;

							if ((mData->DrawSet.ShowItems & Pwr2Id) && ((Pwr2Id & mData->DrawSet.ShowSpeedFor) || (Pwr2Id & mData->DrawSet.ShowAccFor)))
							{
								HPEN PenLink = CreatePen(PS_DASH, 1, mRGB[(idParticle + 8) % 16]);
								SelectObject(hDC, PenLink);

								SData* pData = mData->Group->m_Array[idParticle]->m_Data->m_Array;
								if (mData->DrawSet.nLinks == 0) mData->DrawSet.nLinks = 10;

								int StopIdx = mData->Group->m_Array[idParticle]->m_Data->CurPos + 1;
								if (StopIdx > mData->DrawSet.ViewPoint.EndIdx) StopIdx = mData->DrawSet.ViewPoint.EndIdx;

								int nPoints = mData->DrawSet.ViewPoint.GetCount(mData->Group->m_Array[idParticle]->m_Data->CurPos);
								int ViewStep = nPoints / mData->DrawSet.nLinks;
								if (ViewStep < 1) ViewStep = 1;

								for (int Itemidx = mData->DrawSet.ViewPoint.StartIdx; Itemidx < StopIdx; Itemidx += ViewStep)
								{
									int posSX = (int)(mData->DrawSet.m_00_Pos.X + delta00.X + (double)(((double*)(&pData[Itemidx].position))[idxCrd1] * mData->DrawSet.ViewMltpl));
									int posSY = (int)(mData->DrawSet.m_00_Pos.Y + delta00.Y + (double)(((double*)(&pData[Itemidx].position))[idxCrd2] * mData->DrawSet.ViewMltpl));
									//mData->Group->GetGroupSummaryInfo(&GrInfo, Itemidx);

									for(int i = 0; i < 2; i++)
										if (((mData->DrawSet.ShowSpeedFor & Pwr2Id) && i == 0) || ((mData->DrawSet.ShowAccFor & Pwr2Id) && i == 1))
										{
											SPos endPoint;
											if (i == 0)
											{
												endPoint = pData[Itemidx].Speed;
												//endPoint.ADD(GrInfo.Speed_3D);
												endPoint.MULT(mData->DrawSet.ViewMltpl);
												endPoint.ADD(pData[Itemidx].position);
											}
											else
											{
												endPoint = pData[Itemidx].Accel3D;
												endPoint.MULT(mData->DrawSet.ViewMltpl * ArrowsMagn);
												endPoint.ADD(pData[Itemidx].position);
											}

											int posEX = (int)(mData->DrawSet.m_00_Pos.X + delta00.X + (((double*)(&endPoint))[idxCrd1] * mData->DrawSet.ViewMltpl));
											int posEY = (int)(mData->DrawSet.m_00_Pos.Y + delta00.Y + (((double*)(&endPoint))[idxCrd2] * mData->DrawSet.ViewMltpl));

											DrawArrow(hDC, posSX, posSY, posEX, posEY, 3);
										}
								}
								DeleteObject(PenLink);
							}
						}
					}

					//отрисовка св€зей 
					if (mData->DrawSet.ShowLinksTo && mData->DrawSet.nLinks > 2)
						for (int idParticle = 0; idParticle < nTrace; idParticle++)
						{
							int Pwr2Id = 1 << idParticle;

							if ((mData->DrawSet.ShowItems & Pwr2Id) && (Pwr2Id & mData->DrawSet.ShowLinksFrom))
							{
								HPEN PenLink = CreatePen(PS_DASHDOTDOT, 1, mRGB[(idParticle + 8) % 16]);
								SelectObject(hDC, PenLink);

								SData* pData = mData->Group->m_Array[idParticle]->m_Data->m_Array;
								if (mData->DrawSet.nLinks == 0) mData->DrawSet.nLinks = 10;

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
							LineTo(hDC, posC1 + 2, posC2 - 2);
							LineTo(hDC, posC1 - 2, posC2 - 2);
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

