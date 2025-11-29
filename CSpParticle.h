#pragma once

#define SP_DENCITY_MIN			0.0
#define SP_DENCITY_MAX			1.0
#define SP_DENCITY_MIN_DELTA	0.000000000001


#define SP_PREC_TIME_MINSTEP	0.0000000001
#define SP_PREC_SPEED_PWR2		0.0000000001
#define SP_PREC_LIMIT			0.0000000000001

#define SP_NEWPOINT_STATE_OFF		0x0000
#define SP_NEWPOINT_STATE_ON		0x0001
#define SP_NEWPOINT_STATE_READY		0x0003

#define SP_DATA_DENCITY			0x0001
#define SP_DATA_SPEED			0x0002
#define SP_DATA_POSITION		0x0004
#define SP_DATA_TIMEPOINT		0x0008
#define SP_DATA_ALL				0x000F
#define SP_DATA_ALL_WO_COUNTER	0x0007

//ABS - абсолютная величина (связанность первого уровня)
//REL - относительная величина (связанность второго уровня)

// Ok
#define SP_STATE_ОК					0x00000000
// ошибка
#define SP_STATE_ERROR_MAIN			0x80000000
#define SP_STATE_ERROR_MINOR		0xC0000000
// в процессе
#define SP_STATE_INPROC				0x00000001
// позиция на нижней границе диапазона
#define SP_STATE_DOWN				0x00000002
// позиция на верхней границе диапазона
#define SP_STATE_UP					0x00000004
// позиция внутри границ диапазона
#define SP_STATE_MID				0x00000006

// точка видимости ранее начальной точки сохраненной траектории (<0)
#define SP_STATE_LOW_VIEW			0x00000010
// точка видимости позже последней точки сохраненной траектории (<0)
#define SP_STATE_HIGH_VIEW			0x00000020
// нулевая абсолютная скорость
#define SP_STATE_ABS_0_SPEED		0x00000040
// нулевая относительная скорость
#define SP_STATE_REL_0_SPEED		0x00000080


//////////////////WARNINGS    

#define SP_STATE_WARNING_MASK				0x000FFF00		

// аболютная скорость изменила статус предельной скорости
#define SP_STATE_ABS_CHANGE_LS				0x00000400
// аболютная скорость равна предельной
#define SP_STATE_ABS_SPEED_AL				0x00000800
// относительная скорость перешла границу предельной
#define SP_STATE_PHANTOM_DIST_LECRITICAL	0x00002000
// фантомное расстояние меньше 1.0 - толщина родительского пространства
#define SP_STATE_REL_CHANGE_LS				0x00004000
// относительная скорость выше предельной
#define SP_STATE_REL_SPEED_AL				0x00008000
// ускорение меньше 0
#define SP_STATE_REL_ACCELERATION_L0		0x00020000

//////////////////WARNINGS



#define SP_STATE_NEED_RECALC			0x010000000
#define SP_STATE_NEED_UPDATE			0x030000000
#define SP_STATE_FAR_AWAY				0x000000001



#define SDA_DEF_AMOUNT		0x10000
#define SD_PARAMSLEN		0x10000



class CDrawDlg;
class CFindDlg;
class CSP_Group;

struct SP_Refresh
{
	int Delta;
	int Cntr;
};

struct SPTime
{
	double BaseStep;
	double CurStep;
	double CurTime;
	double MinStep;
	double MaxStep;
};


// максимальная плотность на максимальном удалении равна 1 плотности родительского пространства
// все параметры нормированы по единице.


#include "SPos.h"

struct SPRelation
{
	SData ViewPointData;
	int State;

	double DensityAtBasePoint;

	SPos Distance_3D;
	double DistancePwr1;
	double DistancePwr2;
	double DistancePwr3;

	SPos RelativeSpeed_3D;
	double RelativeSpeedValue;
	double RelativeSpeedValueAbs;

	SPos Acceleration_3D;
	double AccelerationValue;

	SPos AccelerationDeriv_3D;
	double AccelerationDerivValue;

	SPos Wind_3D;
	double WindValue;

	double LastViewPointTime;

	SPRelation()
	{
		CLEAR();
	};

	void CLEAR()
	{
		ViewPointData.CLEAR();
		State = 0;

		DensityAtBasePoint = 0.0;

		Distance_3D = { 0 };
		DistancePwr1 = 0.0;
		DistancePwr2 = 0.0;
		DistancePwr3 = 0.0;

		RelativeSpeed_3D = { 0 };
		RelativeSpeedValue = 0.0;
		RelativeSpeedValueAbs = 0.0;

		Acceleration_3D = { 0 };
		AccelerationValue = 0.0;

		AccelerationDeriv_3D = { 0 };
		AccelerationDerivValue = 0.0;

		Wind_3D = { 0 };
		WindValue = 0.0;

		LastViewPointTime = 0.0;
	};

	void SET_AS_FARAWAY()
	{
		ViewPointData = { 0 };
		State = SP_STATE_FAR_AWAY;
		DensityAtBasePoint = 0.9999999999999;

		Distance_3D.X = Distance_3D.Y = Distance_3D.Z = 9.9e99;
		DistancePwr1 = DistancePwr2 = DistancePwr3 = 9.9e99;
			
		RelativeSpeed_3D = { 0 };
		RelativeSpeedValue = RelativeSpeedValueAbs = 0.0;

		Acceleration_3D = { 0 };
		AccelerationValue = 0.0;

		AccelerationDeriv_3D = { 0 };
		AccelerationDerivValue = 0.0;

		Wind_3D = { 0 };
		WindValue = 0.0;
	}
};

struct SSingleRelation
{
	SPRelation relation;
	int idxParticle1;
	int idxParticle2;
	SData Part1;
	SData Part2;
	double MMValue;

	void CLEAR()
	{
		relation.CLEAR();
		relation.LastViewPointTime = -1000000.0;
		idxParticle1 = 0;
		idxParticle2 = 0;
		Part1 = { 0 };
		Part2 = { 0 };
		MMValue = 0.0;
	}

	SSingleRelation()
	{
		CLEAR();
	}
};


struct SMinMaxRelation
{
	/// <summary>
	/// SSingleRelation содержит идентификатор частицы, что опасно в случае реорганизации массива частиц
	/// </summary>
	SSingleRelation Distance;
	SSingleRelation Speed;

	void CLEAR()
	{
		Distance.CLEAR();
		Speed.CLEAR();
	}
};

#define SP_DRFLG_GROUP_SPEED	0x00000001 
#define SP_DRFLG_GROUP_ACC		0x00000002 

#define SP_DRAW_TEXT		0x00000100 
#define SP_DRAW_CLEARBKG	0x00000200 
#define SP_DRAW_GROUPINFO	0x00000400 

#define SP_DRAW_XYZ			0x00000003 
#define SP_DRAW_XY			0x00000000 
#define SP_DRAW_XZ			0x00000001 
#define SP_DRAW_YZ			0x00000002 

#define SP_DRAW_3D			0x00000010 

#define SP_DRAW_MAX_ID		0x10000000

struct ViewPointData
{

	double TimeInPc;
	double Spread;
	int StartIdx;
	int EndIdx;

	ViewPointData()
	{
		TimeInPc = -1.0;
		Spread = 100.0;
		StartIdx = 0;
		EndIdx = SP_DRAW_MAX_ID;
	}

	int GetCount(int in_MaxId)
	{
		if (in_MaxId > EndIdx) in_MaxId = EndIdx;
		if (in_MaxId < StartIdx) in_MaxId = StartIdx;
		return in_MaxId - StartIdx + 1;
	}

};


struct SDrawSettings
{
	SPos m_00_Pos; //позиция нулевой координаты на картинке
	double ViewMltpl;   //множитель (увеличение)
	double ViewMltpl_Ext;   //множитель (увеличение)
	SPos BMP_Size;  //размер картинки
	SPos PosMin; //Окно просмотра в позициях траектории
	SPos PosMax; //Окно просмотра в позициях траектории
	SPos WindowPos; //сдвиг окна просмотра от вершины картинки

	ViewPointData ViewPoint;

	int ShowItems;
	int ShowLinksFrom;
	int ShowSpeedFor;
	int ShowAccFor;
	int ShowGroupArrows;
	int ShowLinksTo;
	int nLinks;
	int iShowFlags;

	SDrawSettings()
	{
		m_00_Pos = {0};
		ViewMltpl = 1.0;
		ViewMltpl_Ext = 0.1;
		BMP_Size.X = 1400;
		BMP_Size.Y = 1400;
		BMP_Size.Z = 1400;
		PosMin = { 0 }; 
		PosMax = { 0 }; 
		WindowPos = { 0 };

		ShowSpeedFor = 0x0;
		ShowAccFor = 0x0;
		ShowItems = 0xFFFFFFFF;
		ShowGroupArrows = 0xFFFFFFFF;
		ShowLinksFrom = 0x1;
		ShowLinksTo = 0;
		nLinks = 10;
		iShowFlags = SP_DRAW_TEXT | SP_DRAW_CLEARBKG | SP_DRAW_GROUPINFO;
	}

};

struct SP_CalcStat
{
	SData v_Min;
	SData v_Max;
	SData_SD v_MinSD;
	SData_SD v_MaxSD;
	SMinMaxRelation Rel_Min;
	SMinMaxRelation Rel_Max;
	SMinMaxRelation Prev_Rel_Min;
	SMinMaxRelation Prev_Rel_Max;

	void CLEAR()
	{
		v_Min = { 0 };
		v_Max = { 0 };
		v_MinSD = { 0 };
		v_MaxSD = { 0 };
		Rel_Min.CLEAR();
		Rel_Max.CLEAR();
		Prev_Rel_Min.CLEAR();
		Prev_Rel_Max.CLEAR();
	}

	SP_CalcStat()
	{
		CLEAR();
	}

};


#define COLISSION_TYPEMASK		0x0000000F
#define COLISSION_NONE			0x00000000
//упругое столкновение
#define COLISSION_ELASTIC		0x00000001
//неупругое столкновение
#define COLISSION_INELASTIC		0x00000002
//абсолютно скользкое slippery
#define COLISSION_SLIPPERY		0x00000003

//изменяет скорость
#define COLISSION_SPEED_MASK	0x000000F0
#define COLISSION_SPEED_ADD		0x00000010
#define COLISSION_SPEED_OPPOSITE	0x00000020
#define COLISSION_SPEED_NOCHANGE	0x00000020
#define COLISSION_SPEED_VIEWPOINT	0x00000030
#define COLISSION_SPEED_SET4	0x00000040
//изменяет позицию
#define COLISSION_POS_MASK		0x00000F00
#define COLISSION_POS_SET1		0x00000100
#define COLISSION_POS_SET2		0x00000200
#define COLISSION_POS_SET3		0x00000300
#define COLISSION_POS_SET4		0x00000400
//изменяет ускорение
#define COLISSION_ACCEL_MASK	0x0000F000
#define COLISSION_ACCEL_SET1	0x00001000
#define COLISSION_ACCEL_SET2	0x00002000
#define COLISSION_ACCEL_SET3	0x00003000
#define COLISSION_ACCEL_SET4	0x00004000

struct SP_StopParam
{
	double Time;
	double minRelDist;
	double minMaxRelSpeed;
	double TimeStep;
	int Iteration;

	SP_StopParam()
	{
		CLEAR();
	};

	void CLEAR()
	{
		Time = 1.0e7;
		minRelDist = 1000.0;
		minMaxRelSpeed = 0.001;
		TimeStep = 1000.0;
		Iteration = 0x7FFF0000;
	};
};

struct SP_CalcFindSet
{
	SPos minPos;
	SPos maxPos;
	SPos minSpeed;
	SPos maxSpeed;

	SP_StopParam StopIF;

	double ExcludeDistance;

	SP_CalcFindSet()
	{
		CLEAR();
	};

	void CLEAR()
	{
		minPos = { 0 };
		maxPos = { 0 };
		minSpeed = { 0 };
		maxSpeed = { 0 };

		StopIF.CLEAR();

		ExcludeDistance = 500.0;
	};
};


struct SP_Calc
{

	//Runtime
	int CurStep;
	SP_CalcStat Stat;
	SPTime Time;
	int nIterations;

	CSP_Group* Group;
	CDrawDlg* DlgTrace;

	SDrawSettings DrawSet;
	SP_Refresh refresh;


	//settings&rules//////////////////////
	char* CalcParams;
	double maxRelativeSpeed;
	double maxAbsSpeed;
	double maxAbsSpeedPwr2;
	double ParentSpaceThickness;
	double CriticalDistance;
	int InitialParticleCount;
	int AccCalcType; //0 нет влияния плотности на время/скорость. 1 есть влияние умножене. 2 есть влияние умножене. 
	int DensityCalcType; //
	//0  -  dens = 1 - 1 / S^2
	//1  -  dens = 1 - 1 / (1 + S^2)
	//2  -  dens = 1 - 1 / (1 + S)^2

	int AdditiveType; //0 аддитивность отсутствует, т.е. каждая пара частиц взаимодействует независимо. 1 - аддитивность есть.
	int ClassicType; // 1 - ViewPoint в точке нахождения частицы на момент запроса. 2 - ViewPoint в точке следующей расчетной позиции если таковая есть
	int DensityDerivLevel;
	int WindType;
	int CollisionType;
	int sRandSeed_Calc;

	int OutPutFlags;

	SP_CalcFindSet CalcFindSet;


	SP_Calc()
	{
		CurStep = 0;
		Stat.CLEAR();
		Time = { 0 };
		nIterations = 1000000;

		Group = NULL;;
		DlgTrace = NULL;;

		refresh = { 0 };

		CalcParams = NULL;

		maxRelativeSpeed = 2.0;
		maxAbsSpeed = 1.0;
		maxAbsSpeedPwr2 = 1.0;
		ParentSpaceThickness = 0.0;
		CriticalDistance = 1.0;
		InitialParticleCount = 2;
		AccCalcType = 0; 
		DensityCalcType = 0;
		sRandSeed_Calc = -1;

		AdditiveType = 0;
		ClassicType = 0;
		DensityDerivLevel = 0;
		WindType = 0;
		CollisionType = COLISSION_SLIPPERY | COLISSION_SPEED_OPPOSITE | COLISSION_POS_SET1 | COLISSION_ACCEL_SET1;
	
		OutPutFlags = 0;
	}
};

struct SP_CalcFindStat
{
	int Count;
	int Reported;
};


struct SP_CalcFind
{
	SP_Calc curCalc;
	SP_CalcFindSet Settings;
	CFindDlg* DlgFind;
	SP_CalcFindStat stat;
	int m_FIND_REFRESH;
	char* FindParams;
	int sRandSeed;

	SP_CalcFind()
	{
		DlgFind = NULL;
		stat = {0};
		m_FIND_REFRESH = 5;
		FindParams = NULL;
		sRandSeed = 0;
		stat = { 0 };
	};
};

class CSDataArray;

class CSpParticle
{
	friend class CSP_Group;
	friend class CDrawDlg;
	friend void PaintTraceToCDC(CDC* DCMem, SP_Calc* mData, int in_DrawFlags, SPos& delta);

public:
	SData *GetCurData();
	SData* GetDataById(int in_Id);

	static double GetDistance(CSpParticle* Ptr1, CSpParticle* Ptr2) { return SData::GetDistance(*Ptr1->GetCurData(), *Ptr2->GetCurData()); }
	static double GetDistancePwr2(CSpParticle* Ptr1, CSpParticle* Ptr2) { return SData::GetDistancePwr2(*Ptr1->GetCurData(), *Ptr2->GetCurData()); }
	static double GetRelativeSpeed(CSpParticle* Ptr1, CSpParticle* Ptr2, SPos* RSpeed3D) { return SData::GetRelativeSpeed(*Ptr1->GetCurData(), *Ptr2->GetCurData(), RSpeed3D); }

	//recurcive

	int ItgrlFull(SData* Result, const SData& DataSrc, double in_Time, SP_Calc* in_pCalcParams);

	int CalcMidData( SData *Result, const SData& Ptr1, const SData& Ptr2, double absMidPoint);
	int Itgrl(SPos* D_Res, SPos* D1, double in_Mltpl, int iLev, int iMaxLev, double Tx, double DensMltpl);
	int Itgrl(SPos* D_Res, SPos* D1, int iMaxLev, double Tx, double DensMltpl);

	static int Itgrl_s(SPos* D_Res, SPos* D1, double in_Mltpl, int iLev, int iMaxLev, double Tx);
	static int Itgrl_s(SPos* D_Res, SPos* D1, int iMaxLev, double Tx);


public:
	///NewPoin
	int MoveToNewPoint(int in_maxCountLimit, bool in_ClearData = true);
	int NewPoint_Begin(double in_NewTimePoint);
	void NewPoint_Clear();
	///
	int CalcPrevData(SData* out_Data, double in_DataTime);
	void Clear();

	SPos GetCurPosition();
	double GetCurTime();
	double GetPrevTime(int n_StepsBefore);
	int GetPointCount();
	int FindViewPoint(SData* out_Data, SData* in_Pos, double in_ViewTime, double in_StartTime, double in_EndTime);
	int FindViewPointEx(SData* out_Data, SData* in_Pos, double in_ViewTime);
	int GetPrevDataByTime(SData *out_Data, double in_ViewTime, double *out_KeyPoint);
	int GetNextDataByTime(SData *out_Data, double in_ViewTime, double *out_KeyPoint);
	int GetDataAtLowPoint(SData* out_Data, double in_ViewTime);
	int DeletePoints(double TimeFromGE, double TimeToLE);
	char* GetStateDescription(char *out_Str, double in_ReportTime);
	char* GetStateDescription(char* out_Str, int in_nPoints = 1);
	double GetAbsSpeedPwr2();
	void GetMinMaxTracePos(SPos& min_Pos, SPos& max_Pos, int idxStart, int idx_End);
	int GetLowIdxByTime(double in_ViewTime);
	int GetMaxIdx();

	static char* GetFlagsDescription(char* out_Str, int in_Flags);
	int GetDataById(int in_Idx, SData *inout_Reseult);

private:
	SData* GetDataAtLowPoint(double in_ViewTime);
	CSDataArray *m_Data;
	int m_NewPointState;
	SData m_NextData;
	SP_Calc* m_pCalc;

public:
	CSpParticle(SP_Calc *in_pCalc);
	CSpParticle(int in_MaxItems);
	int SetParticleData(int in_flags, SData *in_Ptr);

	~CSpParticle();
};

