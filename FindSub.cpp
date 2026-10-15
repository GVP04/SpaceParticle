#include "pch.h"

#include "CommonFunc.h"
#include "SpaceOM.h"
#include "SpaceOMDlg.h"

#include "CSP_Group.h"
#include "CDrawDlg.h"
#include "CFindDlg.h"

#include "CSDataArray.h"


UINT DoCalc_1(LPVOID pParam)
{
	if (pParam)
	{
		SP_Calc* m_Calc = (SP_Calc*)pParam;
		{		FILE* ptrFile = nullptr;

		m_Calc->Group->Reset();

		REPORTER_THREAD_CALC(m_Calc->Group->m_reporter.ReadParams("FILENAME \"GROUP_REPORT.TXT\" FILEMODE = \"w\"");)
		REPORTER_THREAD_CALC(m_Calc->Group->m_reporter.OPEN();)

		m_Calc->Group->SetCalcState(M_STATE_CALC | M_STATE_READY);
		m_Calc->Group->ReadSettingsGroup(m_Calc->CalcParamsDecoded, SGROUP_ALL);

		if ((m_Calc->OutPutFlags & 0x0001) == 0 || !fopen_s(&ptrFile, "report.txt", "wt"))

			m_Calc->Group->PrepareForCalc(0);

			if (ptrFile && (m_Calc->OutPutFlags & 0x0001))
				m_Calc->Group->WriteState(ptrFile, m_Calc->Group->GetCurTime());

			m_Calc->Time.FixStartTimeCur();
			
			while (
				m_Calc->CalcCounter.NextAndCheck()
				&& m_Calc->Group->GetCurTime() < m_Calc->CalcFindSet.StopIF.Time && (m_Calc->Group->GetCalcState() & M_STATE_CALC) && !(m_Calc->Group->GetCalcState() & M_STATE_BREAK))
				if (!m_Calc->Group->DoPause())
				{
					//ASSERT(m_Calc->CalcCounter.GetCurStep()!= 409);
					m_Calc->Group->DoDelay();
					EnterCriticalSection(&m_Calc->Group->m_cs_Command);
					{
						if (ptrFile && (m_Calc->OutPutFlags & 0x0001)) fprintf(ptrFile, "%d\t%d\t", m_Calc->CalcCounter.GetCurLoop(), m_Calc->CalcCounter.GetCurStep());
						REPORTER_THREAD_CALC(m_Calc->Group->m_reporter.OUTPUT("MAIN Step ", tttt++);)

						m_Calc->CalcCounter.AssertCurStep();

						m_Calc->Group->NextStep();

						if (ptrFile && (m_Calc->OutPutFlags & 0x0001)) m_Calc->Group->WriteState(ptrFile, 1, 1);

						if (m_Calc->CalcFindSet.StopIF.minRelDist < m_Calc->Stat.Rel_Min.Real_Distance.MMValue)
							m_Calc->Group->ClearCalcState(M_STATE_CALC);
					}
					m_Calc->Time.FixEndTimeCur();
					LeaveCriticalSection(&m_Calc->Group->m_cs_Command);
				}

			if (ptrFile) fclose(ptrFile);

		}

		REPORTER_THREAD_CALC(m_Calc->Group->m_reporter.CLOSE();)
		m_Calc->Group->ClearCalcState(M_STATE_CALC);
		m_Calc->Group->SetCalcState(M_STATE_CHANGED);
		m_Calc->Group->ResumeThreadsAfter();
	}
	return 0;
}

UINT DoCalc_Find(LPVOID pParam)
{
	if (pParam)
	{
		SP_CalcFind* m_Calc = (SP_CalcFind*)pParam;
		FILE* ptrFile = nullptr;

		m_Calc->curCalc.Group->Reset();
		m_Calc->stat = { 0 };
		m_Calc->curCalc.Group->SetCalcState(M_STATE_CALC);

		m_Calc->curCalc.Group->ReadSettingsGroup(m_Calc->FindParamsDecoded, SGROUP_ALL);

		m_Calc->curCalc.Group->SetCalcState(M_STATE_READY);
		m_Calc->curCalc.Time.FixStartTime();

		{
			char fileMane[500];
			{
				char tmpCaption[500];
				sprintf_s(tmpCaption, "Find NP%d_C%d_ADD%d_Wind%d_DCT%d_ACT%d_DL%d_nc%0.3d_PT%g_%d",
					m_Calc->curCalc.Group->GetCount(), m_Calc->curCalc.ClassicType, m_Calc->curCalc.AdditiveType,
					m_Calc->curCalc.WindType, m_Calc->curCalc.DensityCalcType, m_Calc->curCalc.AccCalcType, m_Calc->curCalc.DensityDerivLevel, m_Calc->stat.Count,
					m_Calc->curCalc.ParentSpaceThickness, GetTickCount());
				::SetWindowTextA(m_Calc->DlgFind->m_hWnd, tmpCaption);
				sprintf_s(fileMane, "%s.txt", tmpCaption);
			}

			char* tmpOutput = new char[SPG_TEMPSTRING_LEN + 0x10];
			for (m_Calc->stat.Count = 0; (m_Calc->curCalc.Group->GetCalcState() & M_STATE_CALC) && !(m_Calc->curCalc.Group->GetCalcState() & M_STATE_BREAK); m_Calc->stat.Count++)
			{

				if ((m_Calc->curCalc.OutPutFlags & 0x0001) == 0 || !fopen_s(&ptrFile, fileMane, "wt")) // странный вход в блок расчета :)
				{
					m_Calc->curCalc.Group->PrepareForCalc(1);

					sprintf_s(tmpOutput, SPG_TEMPSTRING_LEN, "Iteration\t%d\tReported\t%d\n", m_Calc->stat.Count, m_Calc->stat.Reported);

					if (ptrFile && (m_Calc->curCalc.OutPutFlags & 0x0001)) m_Calc->curCalc.Group->WriteState(ptrFile, m_Calc->curCalc.Group->GetCurTime());


					while (
							m_Calc->FindCounter.NextAndCheck()
						&& (m_Calc->curCalc.Group->GetCalcState() & M_STATE_CALC)
						&& !(m_Calc->curCalc.Group->GetCalcState() & M_STATE_BREAK)
						&& m_Calc->curCalc.Time.GetCurTime() < m_Calc->Settings.StopIF.Time  //Ok
						&& !(m_Calc->curCalc.Group->GetCalcState() & M_STATE_DO_NEXT)

						&& m_Calc->curCalc.Time.GetStep() < m_Calc->Settings.StopIF.TimeStep
						&& m_Calc->curCalc.Stat.Rel_Min.Real_Distance.MMValue < m_Calc->Settings.StopIF.minRelDist
						)
						if (!m_Calc->curCalc.Group->DoPause())
						{
							EnterCriticalSection(&m_Calc->curCalc.Group->m_cs_Command);
							{
								if (ptrFile && (m_Calc->curCalc.OutPutFlags & 0x0001)) fprintf(ptrFile, "%d%d\t", m_Calc->FindCounter.GetCurLoop(), m_Calc->FindCounter.GetCurStep());

								m_Calc->curCalc.Group->NextStep();

								if (ptrFile && (m_Calc->curCalc.OutPutFlags & 0x0001)) m_Calc->curCalc.Group->WriteState(ptrFile, 1, 1);

								m_Calc->curCalc.Stat.MoveNext();

								if (m_Calc->curCalc.Group->GetCount() > 2 && m_Calc->curCalc.Stat.Rel_Max.Real_Distance.MMValue > m_Calc->Settings.ExcludeDistance)
								{
									int idToExclude = m_Calc->curCalc.Group->GetIdByMaxDistance(m_Calc->Settings.ExcludeDistance);
									if (idToExclude >= 0)
										m_Calc->curCalc.Group->RemoveSpParticle(idToExclude);
								}

								m_Calc->curCalc.Time.FixEndTimeCur();
							}
							LeaveCriticalSection(&m_Calc->curCalc.Group->m_cs_Command);

							m_Calc->curCalc.Group->DoDelay();
						}
					if (ptrFile) fclose(ptrFile);


					if (
						(m_Calc->curCalc.Group->GetCalcState() & M_STATE_DO_REPORT) == M_STATE_DO_REPORT
						||
						(
							(
								m_Calc->curCalc.Time.GetCurTime() >= m_Calc->Settings.StopIF.Time
								|| m_Calc->curCalc.Time.GetStep() >= m_Calc->Settings.StopIF.TimeStep
								//////|| m_Calc->curCalc. >= m_Calc->Settings.StopIF.Iteration
								|| !m_Calc->FindCounter.IsOn()
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
				m_Calc->curCalc.Group->ClearCalcState(M_STATE_READY);
				LeaveCriticalSection(&m_Calc->curCalc.Group->m_cs);
			}
			delete[] tmpOutput;
		}

		m_Calc->curCalc.Group->Reset();
		m_Calc->curCalc.Group->ClearCalcState(M_STATE_CALC);
		m_Calc->curCalc.Group->SetCalcState(M_STATE_CHANGED);
	}

	return 0;
}

void PaintTraceToCDC(CDC* DCMem, SP_Calc* mData, int in_DrawFlags, SPos& delta00)
{
	if (DCMem && DCMem->m_hDC && mData && mData->Group)
	{
		if ((in_DrawFlags & SP_DRAW_XYZ) == SP_DRAW_XYZ)
		{
			SDrawSettings oldSet = mData->DrawSet;
			HDC hDC = DCMem->m_hDC;

			if ((in_DrawFlags & SP_DRAW_CLEARBKG))
				DCMem->FillSolidRect(0, 0, (int)mData->DrawSet.BMP_Size.X, (int)mData->DrawSet.BMP_Size.Y, RGB(255, 255, 255));

			mData->DrawSet.ViewMltpl *= 0.5;
			mData->DrawSet.BMP_Size.X *= 0.5;
			mData->DrawSet.BMP_Size.Y *= 0.5;
			mData->DrawSet.BMP_Size.Z *= 0.5;

			int LenLine = 5, LenStep = 9;
			HPEN PenPtr = CreatePen(PS_SOLID, 1, 0x808080);
			SelectObject(hDC, PenPtr);
			int coordConst = (int)(delta00.Y + mData->DrawSet.BMP_Size.Y);
			int coordStart = (int)delta00.X + LenLine;

			for (int i = 0; i < oldSet.BMP_Size.X; i += LenStep)
			{
				MoveToEx(hDC, (int)delta00.X + i, coordConst, nullptr);
				LineTo(hDC, coordStart + i, coordConst);
			}

			coordConst = (int)(delta00.X + mData->DrawSet.BMP_Size.X);
			coordStart = (int)delta00.Y + LenLine;
			for (int i = 0; i < oldSet.BMP_Size.Y; i += LenStep)
			{
				MoveToEx(hDC, coordConst, (int)delta00.Y + i, nullptr);
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
			HPEN PenBlack1 = CreatePen(PS_SOLID, 1, 0);


			int nTrace = mData->Group->GetCount();
			if (nTrace > 1 && mData->Group->m_Array[0] && mData->Group->m_ParticleInfoArray)
			try
			{
				int StopIdx_0 = mData->Group->m_ParticleInfoArray->GetCurPos();
				if (StopIdx_0 > mData->DrawSet.ViewPoint.EndIdx) StopIdx_0 = mData->DrawSet.ViewPoint.EndIdx;
				SData* StopData_0 = mData->Group->m_Array[0]->GetDataById(StopIdx_0);
				if (!StopData_0) StopData_0 = mData->Group->m_Array[0]->GetCurData();

				int nPoints_0 = mData->DrawSet.ViewPoint.GetCount(StopIdx_0);

				GroupSummaryInfo GrInfo_Cur;
				mData->Group->GetGroupSummaryInfo(&GrInfo_Cur, StopData_0->TimePoint);

				if (StopData_0)
				{
					HDC hDC = DCMem->m_hDC;

					if ((in_DrawFlags & SP_DRAW_TEXT))
					{
						int yStart = 10;
						char tmps[300];

						int txtlen = sprintf_s(tmps, 300, "nPoints = %d inView = %d Elapsed = ", mData->Group->m_ParticleInfoArray->GetCurPos(), nPoints_0);
						txtlen += mData->Time.GetElapsedCur(tmps + txtlen);

						TextOutA(hDC, 1, yStart, tmps, txtlen);
						txtlen = sprintf_s(tmps, 300, "Time = %15.15g  | TimeStep = %15.15g ", StopData_0->TimePoint, mData->Group->m_pCalc->Time.GetStep());
						TextOutA(hDC, 1, yStart += 20, tmps, txtlen);
						txtlen = sprintf_s(tmps, 300, "Group Velocity %g | X=%g Y=%g Z=%g", GrInfo_Cur.GroupVelocity, GrInfo_Cur.Speed_3D.X, GrInfo_Cur.Speed_3D.Y, GrInfo_Cur.Speed_3D.Z);
						TextOutA(hDC, 1, yStart += 20, tmps, txtlen);
						txtlen = sprintf_s(tmps, 300, "Group Accel %g | X=%g Y=%g Z=%g", GrInfo_Cur.Acceleration, GrInfo_Cur.Acceleration_3D.X, GrInfo_Cur.Acceleration_3D.Y, GrInfo_Cur.Acceleration_3D.Z);
						TextOutA(hDC, 1, yStart += 20, tmps, txtlen);
						txtlen = sprintf_s(tmps, 300, "Group Pos X=%g Y=%g Z=%g", GrInfo_Cur.Position.X, GrInfo_Cur.Position.Y, GrInfo_Cur.Position.Z);
						TextOutA(hDC, 1, yStart += 20, tmps, txtlen);

						try
						{
							SData nullData;nullData.CLEAR();

							for (int i = 0; i < nTrace; i++, yStart+= 2)
							{
								CSpParticle* pParticle = mData->Group->m_Array[i];
								int StopIdx = pParticle->GetCurPos();
								if (StopIdx > mData->DrawSet.ViewPoint.EndIdx) StopIdx = mData->DrawSet.ViewPoint.EndIdx;

								SData* pData = pParticle->GetDataById(StopIdx);
								if (pData == nullptr) pData = &nullData;

								yStart += 20;

								DCMem->SetTextColor(pParticle->m_TraceSettings.Normal.Color);


								txtlen = sprintf_s(tmps, 300, "%cP_%0.2d Velocity", pParticle->GetParticleType() != SP_TYPE_NEGATIVE ? 'N' : 'A', i);
								TextOutA(hDC, 1, yStart, tmps, txtlen);


								DCMem->SetTextColor(RGB(0, 0, 0));

								txtlen = sprintf_s(tmps, 300, "%g | X=%g Y=%g Z=%g", pData->Velocity.LEN(), pData->Velocity.X, pData->Velocity.Y, pData->Velocity.Z);
								TextOutA(hDC, 105, yStart, tmps, txtlen);


								DCMem->SetTextColor(pParticle->m_TraceSettings.Normal.Color);
								TextOutA(hDC, 47, yStart += 20, "Density", 7);
								DCMem->SetTextColor(RGB(0, 0, 0));

								txtlen = sprintf_s(tmps, 300, "%g | Acc %g | X=%g Y=%g Z=%g", pData->Density, pData->Accel_2_3D.LEN(), pData->Accel_2_3D.X, pData->Accel_2_3D.Y, pData->Accel_2_3D.Z);
								TextOutA(hDC, 105, yStart, tmps, txtlen);
							}
						}
						catch (...)
						{
							;
						}

						txtlen = sprintf_s(tmps, 300, "MIN RELATIVE   Dist_R/P= %g/%g | Velocity = %g", 
							mData->Group->m_pCalc->Stat.Rel_Min.Real_Distance.MMValue, mData->Group->m_pCalc->Stat.Rel_Min.Phantom_Distance.MMValue, mData->Group->m_pCalc->Stat.Rel_Min.RelVelocity.MMValue);
						yStart += 20;  TextOutA(hDC, 1, yStart, tmps, txtlen);
						txtlen = sprintf_s(tmps, 300, "MAX RELATIVE   Dist_R/P= %g/%g | Velocity = %g", 
							mData->Group->m_pCalc->Stat.Rel_Max.Real_Distance.MMValue, mData->Group->m_pCalc->Stat.Rel_Max.Phantom_Distance.MMValue, mData->Group->m_pCalc->Stat.Rel_Max.RelVelocity.MMValue);
						yStart += 20; TextOutA(hDC, 1, yStart, tmps, txtlen);
					}
					//Отрисовка параметров группы
					if ((in_DrawFlags & SP_DRAW_GROUPINFO) && mData->DrawSet.ShowGroupArrows)
					{
						if (mData->DrawSet.nLinks == 0) mData->DrawSet.nLinks = 10;

						int ViewStep = nPoints_0 / mData->DrawSet.nLinks;
						if (ViewStep < 1) ViewStep = 1;

						GroupSummaryInfo GrInfo;
						int prevPosX = 0, prevPosY = 0;

						for (int Itemidx = mData->DrawSet.ViewPoint.StartIdx; Itemidx <= StopIdx_0; Itemidx += ViewStep)
						{
							if (Itemidx + ViewStep > StopIdx_0)
								Itemidx = StopIdx_0;

							if ((mData->DrawSet.ShowGroupArrows & SP_DRFLG_GROUP_HISTORY_ALL) || (Itemidx == StopIdx_0 && (mData->DrawSet.ShowGroupArrows & SP_DRFLG_GROUP_CUR_ALL)))
							{
								if (Itemidx == StopIdx_0)
									GrInfo = GrInfo_Cur;
								else
									mData->Group->GetGroupSummaryInfo(&GrInfo, Itemidx);

								int posSX = (int)(mData->DrawSet.m_00_Pos.X + delta00.X + (double)(((double*)(&GrInfo.Position))[idxCrd1] * mData->DrawSet.ViewMltpl));
								int posSY = (int)(mData->DrawSet.m_00_Pos.Y + delta00.Y + (double)(((double*)(&GrInfo.Position))[idxCrd2] * mData->DrawSet.ViewMltpl));

								if (Itemidx == mData->DrawSet.ViewPoint.StartIdx)	{	prevPosX = posSX; prevPosY = posSY;	}

								if ((mData->DrawSet.ShowGroupArrows & SP_DRFLG_GROUP_HISTORY_TRACE) || (Itemidx == StopIdx_0 && (mData->DrawSet.ShowGroupArrows& SP_DRFLG_GROUP_CUR_TRACE)))
								{//GroupSpeed
									SPos endPoint;
									endPoint = GrInfo.Speed_3D;
									endPoint.MULT(mData->DrawSet.ViewMltpl);
									endPoint.ADD(GrInfo.Position);

									int posEX = (int)(mData->DrawSet.m_00_Pos.X + delta00.X + (((double*)(&endPoint))[idxCrd1] * mData->DrawSet.ViewMltpl));
									int posEY = (int)(mData->DrawSet.m_00_Pos.Y + delta00.Y + (((double*)(&endPoint))[idxCrd2] * mData->DrawSet.ViewMltpl));

									SelectObject(hDC, Itemidx == StopIdx_0 ? PenDGrTrace : PenGrTrace);

									MoveToEx(hDC, prevPosX, prevPosY, nullptr);
									LineTo(hDC, posEX, posEY);
								}

								if ((mData->DrawSet.ShowGroupArrows & SP_DRFLG_GROUP_HISTORY_SPEDD) || (Itemidx == StopIdx_0 && (mData->DrawSet.ShowGroupArrows& SP_DRFLG_GROUP_CUR_SPEED)))
								{//GroupSpeed
									SPos endPoint;
									endPoint = GrInfo.Speed_3D;
									endPoint.MULT(mData->DrawSet.ViewMltpl);
									endPoint.ADD(GrInfo.Position);

									int posEX = (int)(mData->DrawSet.m_00_Pos.X + delta00.X + (((double*)(&endPoint))[idxCrd1] * mData->DrawSet.ViewMltpl));
									int posEY = (int)(mData->DrawSet.m_00_Pos.Y + delta00.Y + (((double*)(&endPoint))[idxCrd2] * mData->DrawSet.ViewMltpl));

									SelectObject(hDC, Itemidx == StopIdx_0 ? PenDGrSpeed : PenGrSpeed);
									DrawArrow(hDC, posSX, posSY, posEX, posEY, 5);
								}
								if ((mData->DrawSet.ShowGroupArrows & SP_DRFLG_GROUP_HISTORY_ACC) || (Itemidx == StopIdx_0 && (mData->DrawSet.ShowGroupArrows & SP_DRFLG_GROUP_CUR_ACC)))
								{//GroupAcc
									SPos endPoint;
									endPoint = GrInfo.Acceleration_3D;
									endPoint.MULT(mData->DrawSet.ViewMltpl * 10.0);
									endPoint.ADD(GrInfo.Position);

									int posEX = (int)(mData->DrawSet.m_00_Pos.X + delta00.X + (((double*)(&endPoint))[idxCrd1] * mData->DrawSet.ViewMltpl));
									int posEY = (int)(mData->DrawSet.m_00_Pos.Y + delta00.Y + (((double*)(&endPoint))[idxCrd2] * mData->DrawSet.ViewMltpl));

									SelectObject(hDC, Itemidx == StopIdx_0 ? PenDGrAcc : PenGrAcc);
									DrawArrow(hDC, posSX, posSY, posEX, posEY, 5);
								}
								prevPosX = posSX;
								prevPosY = posSY;
							}
						}
					}

					//отрисовка векторов скорости и ускорения 
					if (mData->DrawSet.ShowSpeedFor || mData->DrawSet.ShowAccFor)
					{
						GroupSummaryInfo GrInfo;

						SPos delta = mData->DrawSet.PosMax;
						delta.MINUS(mData->DrawSet.PosMin);
						double ArrowsMagn = ((double*)(&delta))[idxCrd1];
						if (ArrowsMagn > ((double*)(&delta))[idxCrd2]) ArrowsMagn = ((double*)(&delta))[idxCrd2];

						int EndIdx = mData->DrawSet.ViewPoint.EndIdx;
						if (EndIdx > mData->Group->m_ParticleInfoArray->GetCurPos())
							EndIdx = mData->Group->m_ParticleInfoArray->GetCurPos();
						int* arrIdx = new int[mData->DrawSet.nLinks + 100];
						double TimeStart = mData->Group->m_ParticleInfoArray->GetDataById(mData->DrawSet.ViewPoint.StartIdx)->TimePoint;
						arrIdx[0] = mData->DrawSet.ViewPoint.StartIdx;
						double TimeEnd = mData->Group->m_ParticleInfoArray->GetDataById(EndIdx)->TimePoint;
						arrIdx[mData->DrawSet.nLinks - 1] = arrIdx[mData->DrawSet.nLinks] = EndIdx;
						double deltaT = (TimeEnd - TimeStart) / (mData->DrawSet.nLinks - 1);

						double tmpTime = TimeStart + deltaT;
						for (int i = 1; i < mData->DrawSet.nLinks; i++, tmpTime += deltaT)
							arrIdx[i] = mData->Group->m_ParticleInfoArray->GetLowIdxByTime(tmpTime);


						for (int idParticle = 0; idParticle < nTrace; idParticle++)
						{
							int Pwr2Id = 1 << idParticle;
							CSpParticle* pParticle = mData->Group->m_Array[idParticle];

							if ((mData->DrawSet.ShowItems & Pwr2Id) && ((Pwr2Id & mData->DrawSet.ShowSpeedFor) || (Pwr2Id & mData->DrawSet.ShowAccFor)))
							{
								HPEN PenLinkVelocity = CreatePen(PS_DASH, 1, pParticle->m_TraceSettings.Velocity.Color);
								HPEN PenLinkAcc = CreatePen(PS_DASH, 1, pParticle->m_TraceSettings.Acceleration.Color);

								if (mData->DrawSet.nLinks == 0) mData->DrawSet.nLinks = 10;

								int nPoints = mData->DrawSet.ViewPoint.GetCount(pParticle->GetCurPos());
								SData nullSData;
								nullSData.CLEAR();

								for (int LinkNum = 0; LinkNum < mData->DrawSet.nLinks; LinkNum++)
								{
									SData* pData = pParticle->GetDataById(arrIdx[LinkNum]);

									int posSX = (int)(mData->DrawSet.m_00_Pos.X + delta00.X + (double)(((double*)(&pData->Position))[idxCrd1] * mData->DrawSet.ViewMltpl));
									int posSY = (int)(mData->DrawSet.m_00_Pos.Y + delta00.Y + (double)(((double*)(&pData->Position))[idxCrd2] * mData->DrawSet.ViewMltpl));

									for(int i = 0; i < 2; i++)
										if (((mData->DrawSet.ShowSpeedFor & Pwr2Id) && i == 0) || ((mData->DrawSet.ShowAccFor & Pwr2Id) && i == 1))
										{
											SPos endPoint;
											if (i == 0)
											{
												endPoint = pData->Velocity;
												endPoint.MULT(mData->DrawSet.ViewMltpl);
												endPoint.ADD(pData->Position);
												SelectObject(hDC, PenLinkVelocity);
											}
											else
											{
												endPoint = pData->Accel_2_3D;
												endPoint.MULT(mData->DrawSet.ViewMltpl * ArrowsMagn);
												endPoint.ADD(pData->Position);
												SelectObject(hDC, PenLinkAcc);
											}

											int posEX = (int)(mData->DrawSet.m_00_Pos.X + delta00.X + (((double*)(&endPoint))[idxCrd1] * mData->DrawSet.ViewMltpl));
											int posEY = (int)(mData->DrawSet.m_00_Pos.Y + delta00.Y + (((double*)(&endPoint))[idxCrd2] * mData->DrawSet.ViewMltpl));

											DrawArrow(hDC, posSX, posSY, posEX, posEY, 3);
										}
								}
								DeleteObject(PenLinkAcc);
								DeleteObject(PenLinkVelocity);
							}
						}
						delete[] arrIdx;
					}

					//отрисовка связей 
					if (mData->DrawSet.ShowLinksFrom && mData->DrawSet.ShowLinksTo && mData->DrawSet.nLinks > 2)
					{
						int EndIdx = mData->DrawSet.ViewPoint.EndIdx;
						if (EndIdx > mData->Group->m_ParticleInfoArray->GetCurPos())
							EndIdx = mData->Group->m_ParticleInfoArray->GetCurPos();
						int* arrIdx = new int[mData->DrawSet.nLinks + 100];
						double TimeStart = mData->Group->m_ParticleInfoArray->GetDataById(mData->DrawSet.ViewPoint.StartIdx)->TimePoint;
						arrIdx[0] = mData->DrawSet.ViewPoint.StartIdx;
						double TimeEnd = mData->Group->m_ParticleInfoArray->GetDataById(EndIdx)->TimePoint;
						arrIdx[mData->DrawSet.nLinks - 1] = arrIdx[mData->DrawSet.nLinks] = EndIdx;
						double deltaT = (TimeEnd - TimeStart) / (mData->DrawSet.nLinks - 1);

						double tmpTime = TimeStart + deltaT;
						for (int i = 1; i < mData->DrawSet.nLinks; i++, tmpTime += deltaT)
							arrIdx[i] = mData->Group->m_ParticleInfoArray->GetLowIdxByTime(tmpTime);

						for (int idParticle = 0; idParticle < nTrace; idParticle++)
						{
							int Pwr2Id = 1 << idParticle;

							if ((mData->DrawSet.ShowItems & Pwr2Id) && (Pwr2Id & mData->DrawSet.ShowLinksFrom))
							{
								HPEN PenLink = CreatePen(PS_DASHDOTDOT, 1, mData->Group->m_Array[idParticle]->m_TraceSettings.PhantomLink.Color);
								SelectObject(hDC, PenLink);

								for (int LinkToPtr = 0; LinkToPtr < nTrace; LinkToPtr++)
									if (idParticle != LinkToPtr && ((1 << LinkToPtr) & mData->DrawSet.ShowLinksTo))
									{
										double PrevTime = -999999.0;

										for (int LinkNum = 0; LinkNum < mData->DrawSet.nLinks; LinkNum++)
										{
											SData* pData = mData->Group->m_Array[idParticle]->GetDataById(arrIdx[LinkNum]);
											if (pData)
											{
												int posC1 = (int)(mData->DrawSet.m_00_Pos.X + delta00.X + (double)(((double*)(&pData->Position))[idxCrd1] * mData->DrawSet.ViewMltpl));
												int posC2 = (int)(mData->DrawSet.m_00_Pos.Y + delta00.Y + (double)(((double*)(&pData->Position))[idxCrd2] * mData->DrawSet.ViewMltpl));
												SData VP_Data;VP_Data.CLEAR();

												int idMin = mData->Group->m_Array[LinkToPtr]->FindViewPointMinIdByTime(pData->Position, pData->TimePoint, PrevTime, pData->TimePoint);
												if (idMin < 0)
													mData->Group->m_Array[LinkToPtr]->FindViewPointByTimeLinear_LE0(VP_Data, *pData, pData->TimePoint, PrevTime);
												else
													VP_Data = *mData->Group->m_Array[LinkToPtr]->GetDataById(idMin);

												if (fabs(VP_Data.Position.X) > 1.0e50)
												{
													VP_Data.Position.X = pData->Position.X + 1.5;
													VP_Data.Position.Y = pData->Position.Y + 1.5;
													VP_Data.Position.Z = pData->Position.Z + 1.5;
												}

												PrevTime = VP_Data.TimePoint;
												MoveToEx(hDC, posC1, posC2, nullptr);
												LineTo(hDC,
													(int)(mData->DrawSet.m_00_Pos.X + delta00.X + (((double*)(&VP_Data.Position))[idxCrd1] * mData->DrawSet.ViewMltpl)),
													(int)(mData->DrawSet.m_00_Pos.Y + delta00.Y + (((double*)(&VP_Data.Position))[idxCrd2] * mData->DrawSet.ViewMltpl)));

												//отрисовка барьера стрелкой в направлении от фантома
												SPos PB_Point; PB_Point.SET_AS_DELTA(pData->Position, VP_Data.Position);
												PB_Point.SET_LENGHT(mData->Group->m_pCalc->Critical.Distance);

												SPos PB_start = PB_Point;
												PB_start.SET_LENGHT(0.1);

												PB_Point += VP_Data.Position;

												int posEX = (int)(mData->DrawSet.m_00_Pos.X + delta00.X + (((double*)(&PB_Point))[idxCrd1] * mData->DrawSet.ViewMltpl));
												int posEY = (int)(mData->DrawSet.m_00_Pos.Y + delta00.Y + (((double*)(&PB_Point))[idxCrd2] * mData->DrawSet.ViewMltpl));
												PB_Point -= PB_start;

												int posSX = (int)(mData->DrawSet.m_00_Pos.X + delta00.X + (((double*)(&PB_Point))[idxCrd1] * mData->DrawSet.ViewMltpl));
												int posSY = (int)(mData->DrawSet.m_00_Pos.Y + delta00.Y + (((double*)(&PB_Point))[idxCrd2] * mData->DrawSet.ViewMltpl));

												SelectObject(hDC, PenBlack1);
												DrawArrow(hDC, posSX, posSY, posEX, posEY, 5);
												SelectObject(hDC, PenLink);
											}
										}
									}
								DeleteObject(PenLink);
							}
						}
						delete[] arrIdx;
					}

					for (int idParticle = 0; idParticle < nTrace; idParticle++)
					{
						int Pwr2Id = 1 << idParticle;
						CSpParticle* pParticle = mData->Group->m_Array[idParticle];

						if (nTrace >= 32 || (mData->DrawSet.ShowItems & Pwr2Id))
						{
							SDrawTraceSettings& TraceSet = mData->Group->m_Array[idParticle]->m_TraceSettings;

							HPEN PenPtr = CreatePen(TraceSet.Normal.Type, TraceSet.Normal.Width, TraceSet.Normal.Color);
							HPEN PenPtrUltimate = CreatePen(TraceSet.Slave.Type, TraceSet.Slave.Width, TraceSet.Slave.Color);
							HPEN PenPtrInside = CreatePen(TraceSet.Inside.Type, TraceSet.Inside.Width, TraceSet.Inside.Color);

							SelectObject(hDC, PenPtr);

							STracePosInfo* PInfoStart = mData->Group->m_ParticleInfoArray->GetDataById(mData->DrawSet.ViewPoint.StartIdx);
							STracePosInfo* PInfoEnd = mData->Group->m_ParticleInfoArray->GetDataById(mData->DrawSet.ViewPoint.EndIdx);
							if (!PInfoEnd) PInfoEnd = mData->Group->m_ParticleInfoArray->GetCurData();
							double deltaT = (PInfoEnd->TimePoint - PInfoStart->TimePoint) * 0.0001220703125; // 0.0001220703125 = 1/ 0x2000 
							if (deltaT < mData->Time.GetMinStep()) 
								deltaT = mData->Time.GetMinStep();
							double TimeCur = PInfoStart->TimePoint;

							int CurIdx = pParticle->GetLowIdxByTime(TimeCur);

							SData* pDataCur = pParticle->GetDataById(CurIdx);

							int posC1 = (int)(mData->DrawSet.m_00_Pos.X + delta00.X + (double)(((double*)(&pDataCur->Position))[idxCrd1] * mData->DrawSet.ViewMltpl));
							int posC2 = (int)(mData->DrawSet.m_00_Pos.Y + delta00.Y + (double)(((double*)(&pDataCur->Position))[idxCrd2] * mData->DrawSet.ViewMltpl));
							int Last1 = -posC1, Last2 = -posC2;


							MoveToEx(hDC, posC1 + 4, posC2 + 4, nullptr);
							LineTo(hDC, posC1 - 4, posC2 + 4);
							LineTo(hDC, posC1 - 4, posC2 - 4);
							LineTo(hDC, posC1 + 4, posC2 - 4);
							LineTo(hDC, posC1 + 4, posC2 + 4);
							MoveToEx(hDC, posC1, posC2, nullptr);

							int LastID = CurIdx;
							while (TimeCur < PInfoEnd->TimePoint)
							{
								TimeCur += deltaT;
								if (TimeCur > PInfoEnd->TimePoint) TimeCur = PInfoEnd->TimePoint;

								CurIdx = pParticle->GetLowIdxByTime(TimeCur);
								if (CurIdx > LastID)
								{
									LastID = CurIdx;
									pDataCur = pParticle->GetDataById(CurIdx);
									SelectObject(hDC, PenPtr);

									//ASSERT(pDataCur->Velocity.Z == 0.0 && pDataCur->Position.Z == 0.0 && pDataCur->Accel_2_3D.Z == 0.0 && pDataCur->Jerk_3_3D.Z == 0.0 && pDataCur->Velocity_Linear.Z == 0.0);
										if ((pDataCur->State & GR_PS_COLLISION_IS_ULTIMATE))
											SelectObject(hDC, PenPtrUltimate);

										if ((pDataCur->State & GR_PS_COLLISION_IS_INSIDE))
												SelectObject(hDC, PenPtrInside);

									posC1 = (int)(mData->DrawSet.m_00_Pos.X + delta00.X + (double)(((double*)(&pDataCur->Position))[idxCrd1] * mData->DrawSet.ViewMltpl));
									posC2 = (int)(mData->DrawSet.m_00_Pos.Y + delta00.Y + (double)(((double*)(&pDataCur->Position))[idxCrd2] * mData->DrawSet.ViewMltpl));

									if (posC1 != Last1 || posC2 != Last2)
									{
										LineTo(hDC, posC1, posC2);
										Last1 = posC1;
										Last2 = posC2;
									}
								}
							}
							DeleteObject(PenPtrInside);
							DeleteObject(PenPtrUltimate);
							DeleteObject(PenPtr);
						}
					}
				}
			}
			catch (...) { ; }

			DeleteObject(PenBlack1);
		}
	}
}

int SaveBitmap(const char* in_FileName, HDC in_hdc, HBITMAP in_hBMP)
{
	int ret = -1;
	FILE* ptrFile;
	if (in_FileName && *in_FileName && in_hdc && in_hBMP && !fopen_s(&ptrFile, in_FileName, "wb"))
	{
		BITMAPINFO bmi = { 0 };
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		ret = -2;
		if (GetDIBits(in_hdc, in_hBMP, 0, 0, nullptr, &bmi, DIB_RGB_COLORS))
		{
			ret = -3;
			HANDLE hDIB = nullptr;
			bmi.bmiHeader.biCompression = BI_RGB;

			if ((hDIB = GlobalAlloc(GHND, bmi.bmiHeader.biSizeImage)))
			{
				ret = -4;
				char* lpbitmap = (char*)GlobalLock(hDIB);

				if (GetDIBits(in_hdc, in_hBMP, 0, bmi.bmiHeader.biHeight, lpbitmap, &bmi, DIB_RGB_COLORS))
				{
					ret = 0;
					BITMAPFILEHEADER bmfh = { 0 };
					bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + bmi.bmiHeader.biSize;
					bmfh.bfSize = bmfh.bfOffBits + bmi.bmiHeader.biSizeImage;
					bmfh.bfType = 'B' + ('M' << 8);

					//Write the bitmap file header
					fwrite(&bmfh, 1, sizeof(BITMAPFILEHEADER), ptrFile);
					//And then the bitmap info header
					fwrite(&bmi.bmiHeader, 1, sizeof(BITMAPINFOHEADER), ptrFile);
					//Finally, write the image data itself
					fwrite(lpbitmap, 1, bmi.bmiHeader.biSizeImage, ptrFile);
				}
				// Unlock and Free the DIB from the heap.
				GlobalUnlock(hDIB);
				GlobalFree(hDIB);
			}
		}
		fclose(ptrFile);
	}
	return ret;
}


int ReadCalcParam_(const char* in_Src, const char* in_Mask, SPos& data, double defValue, int inFlags)
{
	int ret = 0;
	const char* pos = in_Src;
	int lenMask = (int)strlen(in_Mask);
	if (!(inFlags & READPARAMS_NOT_USE_DEFS))
	{
		data.X = defValue;
		data.Y = defValue;
		data.Z = defValue;
	}

	while ((pos = strstr(pos, in_Mask)))
	{
		pos++;
		const char* posEQ = strstr(pos, "=");
		if (posEQ++)
		{
			if (pos[lenMask] == 'X')
			{
				ret = 1;
				data.X = atof(posEQ);
			}
			else	if (pos[lenMask] == 'Y')
			{
				ret = 1;
				data.Y = atof(posEQ);
			}
			else	if (pos[lenMask] == 'Z')
			{
				ret = 1;
				data.Z = atof(posEQ);
			}
		}
	}
	return ret;
}

int ReadCalcParam(const char* in_Src, const char* in_Mask, SPos& data, double defValue, int inFlags)
{
	int ret = 0;
	const char* pos = in_Src;

	if (!(inFlags & READPARAMS_NOT_USE_DEFS))
		data.X = data.Y = data.Z = defValue;

	size_t len = strlen(in_Mask);

	while (pos && (pos = strstr(pos, in_Mask)))
	{
		pos += len;
		while (*pos == ' ') pos++;

		if (*pos == '=')
		{
			data.X = atof(++pos);
			while (*pos && *pos != ',' && *pos != '\r' && *pos != '\n') pos++;

			if (*pos == ',')
			{
				data.Y = atof(++pos);

				while (*pos && *pos != ',' && *pos != '\r' && *pos != '\n') pos++;

				if (*pos == ',')
					data.Z = atof(++pos);
			}

			pos = nullptr;
		}
	
	}
	return ret;
}

int ReadCalcParam(const char* in_Src, const char* in_Mask, int& data, int defValue, int inFlags)
{
	int ret = 0;
	const char* pos = in_Src;
	if (!(inFlags & READPARAMS_NOT_USE_DEFS)) data = defValue;

	if ((pos = strstr(pos, in_Mask)))
	{
		pos++;
		const char* posEQ = strstr(pos, "=");
		if (posEQ++)
		{
			while (*posEQ == ' ' || *posEQ == '\t') posEQ++;
			ret = 1;
			if (sscanf_s(posEQ, "%i", &data) == 0)
			{
				if (!(inFlags & READPARAMS_NOT_USE_DEFS)) data = defValue;
				else data = 0;
			}
		}
	}
	return ret;
}

int ReadCalcParam(const char* in_Src, const char* in_Mask, double& data, double defValue, int inFlags)
{
	int ret = 0;
	const char* pos = in_Src;
	if (!(inFlags & READPARAMS_NOT_USE_DEFS)) data = defValue;

	if ((pos = strstr(pos, in_Mask)))
	{
		pos++;
		const char* posEQ = strstr(pos, "=");
		if (posEQ++)
		{
			data = atof(posEQ);
			ret = 1;
		}
	}
	return ret;
}
