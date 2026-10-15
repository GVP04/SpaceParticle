#pragma once
#include "CommonFunc.h"
#include <chrono>

#define UseTemplates_in_H 


//пожалуй, икосаэдр теоретичски максимальная по числу вершин фигура при внешенй равноудаленной плотности вершин и при удалении вершин от центра на длину ребра
#define SP_MAX_STUCK_MASTERS		12


//Типы и базовые свойства частиц
#define SP_TYPE_FORM_MASK	0x000000FF
//Частица с плотностью 0-1
#define SP_TYPE_POSITIVE	0x00000000
//Частица с плотностью 1-0
#define SP_TYPE_NEGATIVE	0x00000001
 

#define SP_MAX_TIME_DISTANCE	1.0e99
#define SP_MAX_TIME_ACC			1.0e99


#define SP_DENSITY_MIN			0.0
#define SP_DENSITY_MAX			1.0

#define SP_CLEAR_SOFT			0x0000
#define SP_CLEAR_HARD			0x0001


#define SP_PREC_TIME_MLTPL		0.00000000000001

#define SP_NEWPOINT_STATE_OFF		0x0000
#define SP_NEWPOINT_STATE_ON		0x0001
#define SP_NEWPOINT_STATE_READY		0x0003

#define SP_DATA_DENSITY			0x0001
#define SP_DATA_SPEED			0x0002
#define SP_DATA_POSITION		0x0004
#define SP_DATA_TIMEPOINT		0x0008
#define SP_DATA_ALL				0x000F
#define SP_DATA_ALL_WO_COUNTER	0x0007

//ABS - абсолютная величина (связанность первого уровня)
//REL - относительная величина (связанность второго уровня)

// Ok
#define SP_STATE_ОК								0x00000000
// ошибка
#define SP_STATE_ERROR_MAIN						0x80000000
#define SP_STATE_ERROR_MINOR					0xC0000000
// в процессе
#define SP_STATE_INPROC							0x00000001
// позиция на нижней границе диапазона
#define SP_STATE_DOWN							0x00000002
// позиция на верхней границе диапазона
#define SP_STATE_UP								0x00000004
// позиция внутри границ диапазона
#define SP_STATE_MID							0x00000006
//следующий шаг должен быть минимальным
#define SP_STATE_NEXT_STEP_MIN					0x00000008


// точка видимости ранее начальной точки сохраненной траектории (<0)
#define SP_STATE_LOW_VIEW						0x00000010
// точка видимости позже последней точки сохраненной траектории (<0)
#define SP_STATE_HIGH_VIEW						0x00000020
// нулевая абсолютная скорость
#define SP_STATE_ABS_0_SPEED					0x00000040
// нулевая относительная скорость
#define SP_STATE_REL_0_SPEED					0x00000080


//////////////////WARNINGS    
#define SP_STATE_LOW_PRECISSION					0x00400000

#define SP_STATE_WARNING_MASK					0x000FFF00		

// аболютная скорость изменила статус предельной скорости
#define SP_STATE_ABS_CHANGE_LS					0x00000400
// аболютная скорость равна предельной
#define SP_STATE_ABS_SPEED_AL					0x00000800

// фантомное расстояние меньше CriticalDistance
#define SP_STATE_PHANTOM_DIST_LE_CRITICAL		0x00002000
#define SP_STATE_PHANTOM_BESTUCK				0x00004000
#define SP_STATE_PHANTOM_EXPELLED				0x00001000
// фантомное расстояние менее или равно CriticalDistance
#define SP_STATE_PHANTOM_DIST_CRITICAL_CHG		0x00008000

// фантомная плотность меньше 1
#define SP_STATE_PHANTOM_DENSITY_LE_0			0x00040000
// фантомная плотность равна 1
#define SP_STATE_PHANTOM_DENSITY_LE_CHG			0x00080000

#define SP_STATE_PHANTOM_CRITICAL_CHANGED		(SP_STATE_PHANTOM_DIST_CRITICAL_CHG|SP_STATE_PHANTOM_DENSITY_LE_CHG)





//////////////////WARNINGS

#define SP_STATE_RELATION_MASK					0x0F000000
#define SP_STATE_RELATION_MASK_CLR_CMD			0xF0FFFFFF
#define SP_STATE_RELATION_MASK_CLR_ALL			0x00000000
#define SP_STATE_NEED_RECALC					0x01000000
#define SP_STATE_NEED_UPDATE					0x03000000
#define SP_STATE_FAR_AWAY						0x00000100
#define SP_STATE_AFTER_FAR_AWAY					0x00000200

#define SP_STATE_DATA_LINEAR					0x00100000

////////////////////////////					0xC35CFFFF


#define SDA_DEF_AMOUNT		0x100000
#define SD_PARAMSLEN		0x10000
#define SD_PARAMSLENDECODED	0x40000


//Флаги расчета CalcSingleRelation

#define SP_CALCRELATION_FIND_SIMPLE				0x00000000
#define SP_CALCRELATION_FIND_VIEWPOINT			0x00000001
#define SP_CALCRELATION_BREAK_ON_MINOR_ERROR	0x00000002
#define SP_CALCRELATION_NEED_UPDATE_ONLY		0x00000004
#define SP_CALCRELATION_NEED_UPDATE_ONLY		0x00000004

////////////////////////GR_PS///////////////////////////////////////////////////////////

#define GR_PS_NORM					0x00000000
#define GR_PS_MAXABSSPEED			0x00000001
#define GR_PS_MAXRELATIVESPEED		0x00000002
#define GR_PS_FAR_AWAY_PRESENT		0x00000004

#define GR_PS_FAR_AWAY_PRESENT_ON	0x00000010
#define GR_PS_FAR_AWAY_PRESENT_OFF	0x00000020
#define GR_PS_POSITION_CHANGED		0x00000040
#define GR_PS_POSITION_LOW_PREC		0x00000080

#define GR_PS_MAX_IS_PRESENT		0x00000F00
#define GR_PS_MAXABSSPEED_ON		0x00000100
#define GR_PS_MAXABSSPEED_OFF		0x00000200
#define GR_PS_MAXRELATIVESPEED_ON	0x00000400
#define GR_PS_MAXRELATIVESPEED_OFF	0x00000800

#define GR_PS_COLLISION_PRESENT		0x000FF000
#define GR_PS_COLLISION_DENSITY		0x00002000
#define GR_PS_COLLISION_DISTANCE	0x00004000
#define GR_PS_COLLISION_BESTUCK		0x00010000
#define GR_PS_COLLISION_EXPELLED	0x00020000
#define GR_PS_COLLISION_IS_INSIDE	0x00040000
#define GR_PS_COLLISION_IS_ULTIMATE	0x00080000

#define GR_PS_DATA_LINEAR			0x00100000

#define GR_PS_RELATION_CURRENT		0x01000000
#define GR_PS_RELATION_NEXT			0x02000000
#define GR_PS_RELATION_PREV			0x04000000
#define GR_PS_RELATION_TEMP			0x08000000
#define GR_PS_RELATION_TYPEMASK		0x0F000000

#define GR_PS_RELATION_NEEDRECALC	0x10000000
#define GR_PS_RELATION_NONEEDRECALC	0x20000000

///////////////////////////////     0x3F1F6FF7
//////////////////////GR_PS////////////////////////////////////////////////////////////





class CDrawDlg;
class CFindDlg;
class CSP_Group;
class CSpParticle;

struct SP_Refresh
{
	int Delta;
	int Cntr;
};


// максимальная плотность на максимальном удалении равна 1 плотности родительского пространства
// все параметры нормированы по единице.


#include "SPos.h"

struct SPRelation
{
	SData ViewPointData;
	CSpParticle* ViewParticlePtr;
	SPos Distance_3D;
	//простая векторная разница скоростей без проекции на вектор дистанции
	SPos RelativeVelocity_3D;
	SPos Acceleration_3D;
	SPos Jerk_3_3D;
	SPos Snap_4_3D;
	SPos Wind_3D;

	double DistancePwr1;
	double DistancePwr2;
	double DistancePwr3;

	double DensityAtBasePoint;

	double RelativeSpeed;
	double RelativeSpeedAbs;

	double AccelerationValue;

	double LastViewPointTime;
	double RealDistancePwr1;
	double RealRelativeSpeed;

	int State;
	int LowIdxOfViewPont_Near;

	void CLEAR()
	{
		ViewPointData.CLEAR();
		ViewParticlePtr = nullptr;
		State = 0;
		LowIdxOfViewPont_Near = -1;
		DensityAtBasePoint = 0.0;

		Distance_3D = { 0 };
		DistancePwr1 = 0.0;
		DistancePwr2 = 0.0;
		DistancePwr3 = 0.0;

		RealDistancePwr1 = 0.0;
		RealRelativeSpeed = 0.0;

		RelativeVelocity_3D = { 0 };
		RelativeSpeed = 0.0;
		RelativeSpeedAbs = 0.0;

		Acceleration_3D = { 0 };
		AccelerationValue = 0.0;

		Jerk_3_3D = { 0 };
		Snap_4_3D = { 0 };
		Wind_3D = { 0 };

		LastViewPointTime = 0.0;
	};

	void SET_AS_FARAWAY(int in_RelationType)
	{
		////ViewPointData.SET_AS_FARAWAY();
		State = SP_STATE_FAR_AWAY;

		if ((in_RelationType & SP_TYPE_NEGATIVE))
		{
			DensityAtBasePoint = 0.0;
		}
		else
		{
			DensityAtBasePoint = 1.0;
		}

		Distance_3D.X = Distance_3D.Y = Distance_3D.Z = 9.9e99;
		DistancePwr1 = DistancePwr2 = DistancePwr3 = 9.9e99;

		RelativeVelocity_3D = { 0 };
		RelativeSpeed = RelativeSpeedAbs = 0.0;

		Acceleration_3D = { 0 };
		AccelerationValue = 0.0;

		Jerk_3_3D = { 0 };
		Snap_4_3D = { 0 };
		Wind_3D = { 0 };
	}

	double Calc_RelativeAcceleration(int in_CalcType);
	double Calc_RelativeDensity(int in_CalcType);
	// Расчёт третьей производной (рывка)
	void CALC_JERK_AS_RELATIVE();
	// Расчёт четвёртой производной (snap) численно
	void CALC_SNAP_AS_RELATIVE();
};

struct SSingleRelation
{
	SPRelation relation;
	SData Part1;
	SData Part2;
	double MMValue;
	int idxParticle1;
	int idxParticle2;

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

struct SelfCalcData
{
	SData ByRelatedData;
	SData ByNormalData;
};

union RelationAndRelated
{
	SPRelation AsRelation;
	SelfCalcData AsSelf;
};




struct SMinMaxRelation
{
	/// <summary>
	/// SSingleRelation содержит идентификатор частицы, что опасно в случае реорганизации массива частиц
	/// </summary>
	SSingleRelation Real_Distance;
	SSingleRelation Phantom_Distance;
	SSingleRelation RelVelocity;

	void CLEAR()
	{
		//Distance.CLEAR();
		//RelVelocity.CLEAR();

		ZeroMemory(&Real_Distance, sizeof(SSingleRelation));
		ZeroMemory(&Phantom_Distance, sizeof(SSingleRelation));
		ZeroMemory(&RelVelocity, sizeof(SSingleRelation));
	}
};

#define SP_DRFLG_GROUP_CUR_ALL			0x0000000F 
#define SP_DRFLG_GROUP_CUR_SPEED		0x00000001 
#define SP_DRFLG_GROUP_CUR_ACC			0x00000002 
#define SP_DRFLG_GROUP_CUR_TRACE		0x00000004 
#define SP_DRFLG_GROUP_HISTORY_ALL		0x00000F00 
#define SP_DRFLG_GROUP_HISTORY_SPEDD	0x00000100 
#define SP_DRFLG_GROUP_HISTORY_ACC		0x00000200 
#define SP_DRFLG_GROUP_HISTORY_TRACE	0x00000400 

#define SP_DRAW_TEXT		0x00000100 
#define SP_DRAW_CLEARBKG	0x00000200 
#define SP_DRAW_GROUPINFO	0x00000400 

#define SP_DRAW_XYZ			0x00000003 
#define SP_DRAW_XY			0x00000000 
#define SP_DRAW_XZ			0x00000001 
#define SP_DRAW_YZ			0x00000002 

#define SP_DRAW_MAX_ID		0x10000000

#define SP_DRAW_PART_ALL		0x07000000
#define SP_DRAW_PART_MS			0x03000000
#define SP_DRAW_PART_MAIN		0x01000000
#define SP_DRAW_PART_SECONDARY	0x02000000
#define SP_DRAW_PART_ETERNAL	0x04000000



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



struct SDrawElementSettings
{
	int Width;
	int Color;
	int Type;
	int Rerserve;

	void INIT()
	{
		Width = 0;
		Color = 0;
		Type = 0;
		Rerserve = 0;
	}
};


#define DTS_ETYPE_MASK			0x000000FF
#define DTS_ETYPE_NORM			0x00000001
#define DTS_ETYPE_SLAVE			0x00000002
#define DTS_ETYPE_INSIDE		0x00000004
#define DTS_ETYPE_VELOSITY		0x00000008
#define DTS_ETYPE_ACCELERATION	0x00000010
#define DTS_ETYPE_PHLINK		0x00000020

struct SDrawTraceSettings
{
	SDrawElementSettings Normal;
	SDrawElementSettings Slave;
	SDrawElementSettings Inside;
	SDrawElementSettings Velocity;
	SDrawElementSettings Acceleration;
	SDrawElementSettings PhantomLink;

	SDrawTraceSettings()
	{
		INIT();
	}

	void INIT()
	{
		Normal.INIT();
		Slave.INIT();
		Inside.INIT();
		Velocity.INIT();
		Acceleration.INIT();
		PhantomLink.INIT();
	}

	void SET(int in_Variant)
	{
		const int mRGB[16] = {
			RGB(200, 0, 0),RGB(0, 200, 0),RGB(0, 0, 200),RGB(200, 200, 0),RGB(200, 0, 200),RGB(0, 200, 200),
			RGB(200, 100, 0),RGB(200, 0, 100),RGB(200, 100, 100),RGB(100, 200, 0),RGB(0, 200, 100),RGB(100, 0, 200),
			RGB(0, 100, 200),RGB(0, 100, 100),RGB(100, 0, 100),RGB(100, 100, 0),
		};

		const int mRGBDark[16] = {
			RGB(111, 0, 0),RGB(0, 111, 0),RGB(0, 0, 111),RGB(111, 111, 0),RGB(111, 0, 111),RGB(0, 111, 111),
			RGB(111, 51, 0),RGB(111, 0, 51),RGB(111, 51, 51),RGB(51, 111, 0),RGB(0, 111, 51),RGB(51, 0, 111),
			RGB(0, 51, 111),RGB(0, 51, 51),RGB(51, 0, 51),RGB(51, 51, 0),
		};


		Normal.Color = mRGB[in_Variant % 16];
		Slave.Color = mRGB[in_Variant % 16];
		Inside.Color = mRGBDark[in_Variant % 16];
		Velocity.Color = mRGB[in_Variant % 16];
		Acceleration.Color = mRGB[in_Variant % 16];
		PhantomLink.Color = mRGB[in_Variant % 16];

		Normal.Width = 2;
		Slave.Width = 1;
		Inside.Width = 1;
		Velocity.Width = 1;
		Acceleration.Width = 1;
		PhantomLink.Width = 1;

		Normal.Type = PS_SOLID;
		Slave.Type = PS_SOLID;
		Inside.Type = PS_SOLID;
		Velocity.Type = PS_DASH;
		Acceleration.Type = PS_DASH;
		PhantomLink.Type = PS_DASHDOTDOT;
	}

	int SET(const char* SettingsString)
	{
		int ret = 0;
		if (SettingsString)
		{
			const char* subname[] = { "NORMAL.","SLAVE.", "INSIDE.", "VELOCITY.", "ACCELERATION.", "PHANTOMLINK.", "", };
			SDrawElementSettings* pTypes = &Normal;

			int TypeNum = 0;

			while (*subname[TypeNum])
			{
				char subnameFull[100];
				strcpy_s(subnameFull, 100, subname[TypeNum]);
				strcat_s(subnameFull, 100, "TYPE");
				ret += ReadCalcParam(SettingsString, subnameFull, pTypes[TypeNum].Type, 0, READPARAMS_NOT_USE_DEFS);
				strcpy_s(subnameFull, 100, subname[TypeNum]);
				strcat_s(subnameFull, 100, "WIDTH");
				ret += ReadCalcParam(SettingsString, subnameFull, pTypes[TypeNum].Width, 0, READPARAMS_NOT_USE_DEFS);
				strcpy_s(subnameFull, 100, subname[TypeNum]);
				strcat_s(subnameFull, 100, "COLOR");
				ret += ReadCalcParam(SettingsString, subnameFull, pTypes[TypeNum].Color, 0, READPARAMS_NOT_USE_DEFS);
				TypeNum++;
			}
		}
		return ret;
	}
};

struct SDrawSettings
{
	SPos m_00_Pos; //позиция нулевой координаты на картинке
	SPos BMP_Size;  //размер картинки
	SPos PosMin; //Окно просмотра в позициях траектории
	SPos PosMax; //Окно просмотра в позициях траектории
	SPos WindowPos; //сдвиг окна просмотра от вершины картинки

	ViewPointData ViewPoint;

	double ViewMltpl;   //множитель (увеличение)
	double ViewMltpl_Ext;   //множитель (увеличение)

	int ShowItems;
	int ShowLinksFrom;
	int ShowSpeedFor;
	int ShowAccFor;
	int ShowGroupArrows;
	int ShowLinksTo;
	int ShowFlags;
	int nLinks;
	int iShowFlags;

	SDrawSettings()
	{
		m_00_Pos = {0};
		ViewMltpl = 1.0;
		ViewMltpl_Ext = 0.1;
		BMP_Size.X = 2000;
		BMP_Size.Y = 1200;
		BMP_Size.Z = 1200;
		PosMin = { 0 }; 
		PosMax = { 0 }; 
		WindowPos = { 0 };

		ShowSpeedFor = 0x0;
		ShowAccFor = 0x0;
		ShowItems = 0xFFFFFFFF;
		ShowGroupArrows = 0xFFFFFFFF;
		ShowLinksFrom = 0x1;
		ShowLinksTo = 0;
		ShowFlags = 0xFFFFFFFF;
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
		//Rel_Min.CLEAR();
		//Rel_Max.CLEAR();
		//Prev_Rel_Min.CLEAR();
		//Prev_Rel_Max.CLEAR();

		ZeroMemory(&Rel_Min, sizeof(SMinMaxRelation));
		ZeroMemory(&Rel_Max, sizeof(SMinMaxRelation));
		ZeroMemory(&Prev_Rel_Min, sizeof(SMinMaxRelation));
		ZeroMemory(&Prev_Rel_Max, sizeof(SMinMaxRelation));
	}

	SP_CalcStat()
	{
		CLEAR();
	}

	void MoveNext()
	{
		Prev_Rel_Min = Rel_Min;
		Prev_Rel_Max = Rel_Max;
	};
};

struct SP_StopParam
{
	double Time;
	double minRelDist;
	double minMaxRelSpeed;
	double TimeStep;
	int Iteration;
	int AlighmentRes1;

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


struct PrecissionInfo
{
private:
	double MltplDistance;
	double MltplSpeed;
	double MltplAccel;
	double PrecDistance;
	double PrecSpeed;
	double PrecAccel;

	double BasePrecission;

public:
	inline double GetDistanceMltpl() const { return MltplDistance; };
	inline double GetSpeedMltpl() const { return MltplSpeed; };
	inline double GetAccelMltpl() const { return MltplAccel; };
	inline double GetDistancePrec() const { return PrecDistance; };
	inline double GetSpeedPrec() const { return PrecSpeed; };
	inline double GetAccelPrec() const { return PrecAccel; };

	void CLEAR()
	{
		MltplDistance = 1.0;
		MltplSpeed = 1.0;
		MltplAccel = 1.0;
		SetBasePrecission(SP_PREC_TIME_MLTPL);
	};

	double SetBasePrecission(double in_BasePrecission)
	{
		if (in_BasePrecission < 0.0) in_BasePrecission = SP_PREC_TIME_MLTPL;
		BasePrecission = in_BasePrecission;

		PrecDistance = BasePrecission * MltplDistance;
		PrecSpeed = BasePrecission * MltplSpeed;
		PrecAccel = BasePrecission * MltplAccel;

		return BasePrecission;
	}

	double SetDistanceMltpl(double in_Value) { return PrecDistance =  (MltplDistance = in_Value) * BasePrecission; };
	double SetSpeedMltpl(double in_Value) { return PrecSpeed = (MltplSpeed = in_Value) * BasePrecission; };
	double SetAccelMltpl(double in_Value) { return PrecAccel = (MltplAccel = in_Value) * BasePrecission; };

	const PrecissionInfo Set(PrecissionInfo& in_Prec)
	{
		SetDistanceMltpl(in_Prec.MltplDistance);
		SetSpeedMltpl(in_Prec.MltplSpeed);
		SetAccelMltpl(in_Prec.MltplAccel);
		return *this;
	}
};

struct CalcDelay
{
private:
	int m_DelayMS;
	double m_DelayEveryTimeDelta;
	double TimeOfLastDelay;

public:

	CalcDelay()
	{
		CLEAR();
	}

	void CLEAR()
	{
		m_DelayMS = 0;
		m_DelayEveryTimeDelta = 1.0e100;
		TimeOfLastDelay = 0.0;
	}

	void RESET()
	{
		TimeOfLastDelay = 0.0;
	}


	bool CheckDelay(double in_CurTime) {return (in_CurTime >= TimeOfLastDelay + m_DelayEveryTimeDelta);	}
	void SetTimeOfLastDelay(double in_Time) { TimeOfLastDelay = in_Time; }
	void SetDelay(int in_Delay) { m_DelayMS = in_Delay;}
	void SetDelayEveryTimeDelta(double in_Delay) { m_DelayEveryTimeDelta = in_Delay;}
	void DoDelay() { if (m_DelayMS > 0 && m_DelayMS < 10000) Sleep(m_DelayMS); };
};


#define SPTIME_NEXT_STEP_MIN		0x00001000

struct SPTime
{
private:
	double MinStep;
	double MinStepDeltaMinus;
	double NextStep;
	double NextStep4321;
	double PrevStep;
	double PrevPrevStep;
	double CurTime;
	double NextTime;
	double PrevTime;
	double UpLimit;
	double PrevUpLimit;

	int State;
	int Reserve;

	PrecissionInfo PrecInfo;

	//установки
	double MinStepSet;
	double TraceMinStep;
	double HistoryMinStep;
	double TraceBaseStep;
	double RelatedMaxStep;
	double MaxStep;

	//stat
	__int64 start_Calc;
	__int64 end_Calc;

	__int64 start_CalcCur;
	__int64 end_CalcCur;

	inline void SetMinStepDeltaMinus() { MinStepDeltaMinus = MinStep * 0.1; };

public:
	CalcDelay Delay;
	bool CheckDelay() { return Delay.CheckDelay(CurTime); }
	void SetTimeOfLastDelay() { Delay.SetTimeOfLastDelay(CurTime); }
	void DoDelay() { Delay.DoDelay(); }



	void CLEAR()
	{
		if (MinStepSet < SP_PREC_TIME_MLTPL) MinStepSet = SP_PREC_TIME_MLTPL;
		if (MinStep < MinStepSet) MinStep = MinStepSet;

		PrevPrevStep = PrevStep = NextStep = MinStep;
		NextStep4321 = NextStep * 0.4321;
		SetMinStepDeltaMinus();
		PrevTime = CurTime = 0.0;
		PrevUpLimit = UpLimit = SP_MAX_TIME_DISTANCE;
		NextTime = SP_PREC_TIME_MLTPL;
		FixStartTime();
		PrecInfo.CLEAR();
		State = 0;
	}

	void RESET()
	{
		RelatedMaxStep = 0.00001;
		TraceBaseStep = 0.001;
		TraceMinStep = 0.0000001;
		HistoryMinStep = 0.1;
		CLEAR();
		SetMinStep(SP_PREC_TIME_MLTPL);
		SetMaxStep(0.01);
		Delay.RESET();
	}

	int AddFlag(int in_Flag) {	return (State |= in_Flag);	}
	int DelFlag(int in_Flag) { return (State &= ~in_Flag); }
	int GetFlag(int in_Flag) { return State; }

	inline bool IsMinimalStep() const { return NextStep <= MinStep; }
	inline bool IsMinimalStep(double in_CmpDelta) const { return NextStep <= MinStep; }
	inline double GetStep() const { return NextStep; }
	inline double GetPrevStep() const { return PrevStep; }
	inline double GetPrevPrevStep() const { return PrevPrevStep; }
	inline double GetStep4321() const { return NextStep4321; }

	inline double GetPrecDistance() const { return PrecInfo.GetDistancePrec(); }
	inline double GetPrecSpeed() const { return PrecInfo.GetSpeedPrec(); }
	inline double GetPrecAccel() const { return PrecInfo.GetAccelPrec(); }

	inline double GetPrecDistanceMltpl() const { return PrecInfo.GetDistanceMltpl(); }
	inline double GetPrecSpeedMltpl() const { return PrecInfo.GetSpeedMltpl(); }
	inline double GetPrecAccelMltpl() const { return PrecInfo.GetAccelMltpl(); }

	const inline PrecissionInfo GetPrecInfo() const { return PrecInfo; }

	double SetPrecDistanceMltpl(double in_Value) { PrecInfo.SetDistanceMltpl(in_Value);	return PrecInfo.GetDistanceMltpl(); }
	double SetPrecSpeedMltpl(double in_Value) { PrecInfo.SetSpeedMltpl(in_Value);	return PrecInfo.GetSpeedMltpl(); }
	double SetPrecAccelMltpl(double in_Value) { PrecInfo.SetAccelMltpl(in_Value);	return PrecInfo.GetAccelMltpl(); }
	const PrecissionInfo SetPrecMltpl(PrecissionInfo& in_Prec) {	return PrecInfo.Set(in_Prec);	}

	//inline double GetMinStepDeltaMinus() { return MinStepDeltaMinus; }

	double CheckByRelatedStep()
	{
		return RelatedMaxStep < NextStep ? SetStep(RelatedMaxStep) : NextStep;
	}

	double SetMinStep(double in_Step)
	{
		double ret = MinStepSet;
		
		if (in_Step < SP_PREC_TIME_MLTPL) MinStepSet = SP_PREC_TIME_MLTPL;
		else MinStepSet = in_Step;

		if (MaxStep < MinStepSet) MaxStep = MinStepSet;
		if (MinStep < MinStepSet) MinStep = MinStepSet;
		SetMinStepDeltaMinus();

		PrecInfo.SetBasePrecission(MinStep);

		return ret;
	}

	double SetRelatedMaxStep(double in_Step)
	{
		double ret = RelatedMaxStep;
		
		if (RelatedMaxStep < SP_PREC_TIME_MLTPL) RelatedMaxStep = SP_PREC_TIME_MLTPL;
		else RelatedMaxStep = in_Step;

		return ret;
	}
	double CorrectDelta_ByMinStep(double in_Delta) const
	{
		in_Delta -= MinStepDeltaMinus;

		if (in_Delta < MinStep)
			in_Delta = MinStep;

		return in_Delta;
	}

	double GetNextTime_ByCorrectedDelta(double in_Delta) const
	{
		in_Delta -= MinStepDeltaMinus;

		if (in_Delta < MinStep)
			in_Delta = MinStep;

		double ret = CurTime + in_Delta;
		if (ret > UpLimit) ret = UpLimit;
		return ret;
	}

	double CheckDelta_ByMinStep(double in_Delta) const
	{
		if (in_Delta < MinStep)
			in_Delta = MinStep;

		return in_Delta;
	}
	double SetMaxStep(double in_Step)
	{
		double ret = MaxStep;
		
		if (in_Step < MinStep) MaxStep = MinStep;
		else MaxStep = in_Step;

		return ret;
	}
	double SetStep(double in_Step)
	{
		NextStep = in_Step;
		if (NextStep < MinStep) NextStep = MinStep;
		if (NextStep > MaxStep)	NextStep = MaxStep;

		NextTime = CurTime + NextStep;
		if (NextTime > UpLimit)
		{
			NextTime = UpLimit;
			NextStep = NextTime - CurTime;
		}
		NextStep4321 = NextStep * 0.4321;
		ASSERT(NextStep > 1.0e-14);
		return NextStep;
	}
	double SetNextTimeSimple(double in_NextTime)
	{
		SetStep(in_NextTime - CurTime);

		return NextTime;
	}

	double SetNextTimeForce(double in_NextTime)
	{
		PrevUpLimit = UpLimit = SP_MAX_TIME_DISTANCE;
		NextTime = in_NextTime;
		NextStep = NextTime - CurTime;

		ASSERT(NextStep > 1.0e-14);

		NextStep4321 = NextStep * 0.4321;
		return NextTime;
	}

	double SetNextTimeEx(double in_NextTime)
	{
		double delta = NextTime - in_NextTime;
		if (delta != 0.0)
		{
			if (delta < 0)
				SetNextTimeForce(in_NextTime);
			//else if (NextStep * 0.0625 > delta)
			//	SetStep(NextStep * 0.9375);
			//else if (NextStep * 0.9375 < delta)
			//	SetStep(NextStep * 0.125);
			else
			{
				NextStep = in_NextTime - CurTime;
				ASSERT(NextStep > 0.0);
				NextTime = CurTime + NextStep;
				if (NextTime > UpLimit)
				{
					NextTime = UpLimit;
					NextStep = NextTime - CurTime;
				}
				NextStep4321 = NextStep * 0.4321;
				if (NextStep4321 <= 0) NextStep4321 = NextStep;
			}
		}

		return NextTime;
	}

	double SetTraceMinStep(double in_TraceMinStep)
	{
		TraceMinStep = in_TraceMinStep;
		if (TraceMinStep < MinStep)
			TraceMinStep = MinStep;

		return TraceMinStep;
	}

	double SetHistoryMinStep(double in_HistoryMinStep)
	{
		HistoryMinStep = in_HistoryMinStep;
		if (HistoryMinStep < MinStep)
			HistoryMinStep = MinStep;

		return HistoryMinStep;
	}

	double SetTraceBaseStep(double in_TraceBaseStep)
	{ 
		TraceBaseStep = in_TraceBaseStep;
		if (TraceBaseStep < MinStep)
			TraceBaseStep = MinStep;

		return TraceBaseStep;
	}

	inline double GetCurTime() const { return CurTime; }
	inline double GetNextTime() const { return NextTime; }
	inline double GetMinStep() const { return MinStep; }
	inline double GetTraceMinStep() const { return TraceMinStep; }
	inline double GetHistoryMinStep() const { return HistoryMinStep; }
	inline double GetTraceBaseStep() const { return TraceBaseStep; }

	double SetCurTime(double in_CurTime)
	{
		if (in_CurTime != CurTime)
		{
			PrevTime = CurTime;
			CurTime = in_CurTime;
			PrevPrevStep = PrevStep;
			SetStep(PrevStep = CurTime - PrevTime);
//			if (CurTime > UpLimit) UpLimit = CurTime + NextStep;
		}
		else
		{
			ASSERT(0);
		}

		return PrevTime;
	}
	double GetPrevTime() const { return PrevTime; }

	void PrepareNextStep(SP_CalcStat& in_Stat)
	{
		double tmpMinStepSet = CurTime * SP_PREC_TIME_MLTPL + SP_PREC_TIME_MLTPL;

		if (tmpMinStepSet > MinStepSet)
			SetMinStep(tmpMinStepSet);


		double MaxStepTmp = 0.0;
		if (UpLimit < PrevUpLimit)
		{
			MaxStepTmp = (UpLimit - CurTime) * 0.5;
			if (MaxStepTmp < MinStep) MaxStepTmp = MinStep;
		}


		PrevUpLimit = UpLimit = SP_MAX_TIME_DISTANCE;

		//место для изменения динамики рпасчета
		if ((State & SPTIME_NEXT_STEP_MIN))
		{
			SetStep(MinStep);
		}
		else if (MaxStepTmp >= MinStep && MaxStepTmp < PrevStep)
		{
			SetStep(MaxStepTmp);
		}
		else if (PrevPrevStep > PrevStep)
				SetStep(PrevStep);
			else
				SetStep(PrevStep * 1.2);

//		if (IsMinimalStep())
//			PrevUpLimit = UpLimit = NextTime;
	}

	double SetUpLimit(double in_tUpLimit) 
	{
//		UpLimit = in_tUpLimit - MinStep * 0.5;

//		if (UpLimit < CurTime + MinStep) UpLimit = CurTime + MinStep;
//		if (UpLimit > NextTime) UpLimit = NextTime;

		return UpLimit; 
	}

	void FixStartTimeCur()
	{
		end_Calc = end_CalcCur = start_CalcCur = GetTickCount64();
	}

	int GetStartTime()
	{
		return (int)start_CalcCur;
	}

	void FixEndTimeCur()
	{
		end_Calc = end_CalcCur = GetTickCount64();
	}

	void FixStartTime()
	{
		start_CalcCur = end_CalcCur = end_Calc = start_Calc = GetTickCount64();
	}

	void FixEndTime()
	{
		end_Calc = GetTickCount64();
	}
	__int64 TimeFromStart() const
	{
		return end_Calc - start_Calc;
	}

	__int64 TimeFromStartCur() const
	{
		return end_CalcCur - start_CalcCur;
	}

	int GetElapsed(char* in_Str) const
	{
		__int64 delta = end_Calc - start_Calc;

		int ms = (delta % 1000);
		delta /= 1000;
		int sec = (delta % 60);
		delta /= 60;
		int min = (delta % 60);
		delta /= 60;
		int hour = (int)delta;

		return sprintf_s(in_Str, 20, "%d:%0.2d:%0.2d:%0.2d", hour, min, sec, ms);
	}

	int GetElapsedCur(char* in_Str) const
	{
		__int64 delta = end_CalcCur - start_CalcCur;

		int ms = (delta % 1000);
		delta /= 1000;
		int sec = (delta % 60);
		delta /= 60;
		int min = (delta % 60);
		delta /= 60;
		int hour = (int)delta;

		return sprintf_s(in_Str, 20, "%d:%0.2d:%0.2d:%0.2d", hour, min, sec, ms);
	}
};

struct CriticalValues
{
private:
	double Eps_Delta;
public:
	double Speed;
	double SpeedPlusEps;
	double SpeedMinusEps;
	double SpeedPwr2;
	double Distance;
	double DistancePlusEps;
	double DistanceMinusEps;
	double DistancePwr2;
	double DistancePwr3;

	CriticalValues()
	{
		INIT();
	}

	void INIT()
	{
		Speed = 1.0;
		Distance = 1.0;
		Eps_Delta = 1.0e-14;
		Prepare();
	}

	void SetSpeed(double in_Value)
	{
		Speed = in_Value;
		Prepare();
	}

	void SetDistance(double in_Value)
	{
		Distance = in_Value;
		Prepare();
	}
	void SetEps_Delta(double in_Value)
	{
		Eps_Delta = in_Value;
		Prepare();
	}

	void Prepare()
	{
		SpeedPwr2 = Speed * Speed;

		DistancePwr2 = Distance * Distance;
		DistancePwr3 = DistancePwr2 * Distance;

		SpeedPlusEps = Speed + Eps_Delta;
		SpeedMinusEps = Speed - Eps_Delta;
		DistancePlusEps = Distance + Eps_Delta;
		DistanceMinusEps = Distance - Eps_Delta;
	}

	inline bool IsDistanceNear(double in_Value) const {	return (in_Value <= DistancePlusEps && in_Value >= DistanceMinusEps);}
	inline bool IsSpeedNear(double in_Value) const { return (in_Value <= SpeedPlusEps && in_Value >= SpeedMinusEps); }
};


struct SP_Calc
{

	//Runtime
	SP_CalcStat Stat;
	SPTime Time;

	CSP_Group* Group;
	CDrawDlg* DlgTrace;

	SDrawSettings DrawSet;
	SP_Refresh refresh;
	SP_Refresh ShotRefresh;

	LoopCounter CalcCounter;

	CriticalValues Critical;
	double ParentSpaceThickness;

	SP_CalcFindSet CalcFindSet;

	//settings&rules//////////////////////
	char* CalcParams;
	char* CalcParamsDecoded;

	int AccCalcType; //0 нет влияния плотности на время/скорость. 1 есть влияние умножене. 2 есть влияние умножене. 
	int DensityCalcType; //
	//0  -  dens = 1 - 1 / S^2
	//1  -  dens = 1 - 1 / (1 + S^2)
	//2  -  dens = 1 - 1 / (1 + S)^2

	int AdditiveType; //0 аддитивность отсутствует, т.е. каждая пара частиц взаимодействует независимо. 1 - аддитивность есть.
	int ClassicType; // 1 - ViewPoint в точке нахождения частицы на момент запроса. 2 - ViewPoint в точке следующей расчетной позиции если таковая есть
	int DensityDerivLevel;
	int WindType;
	int sRandSeed_Calc;

	int nIterations;
	int OutPutFlags;
	int InitialCheckFlags;

	int AlighmentRes1;

	SP_Calc()
	{
		Stat.CLEAR();
		Time.RESET();
		nIterations = 1000000;

		Group = nullptr;;
		DlgTrace = nullptr;;

		refresh = { 0 };
		ShotRefresh = { 0 };

		CalcParams = nullptr;
		CalcParamsDecoded = nullptr;

		ParentSpaceThickness = 0.0;
		AccCalcType = 0; 
		DensityCalcType = 0;
		sRandSeed_Calc = -1;

		AdditiveType = 0;
		ClassicType = 0;
		DensityDerivLevel = 0;
		WindType = 0;
	
		InitialCheckFlags = 1;
		OutPutFlags = 0;
		AlighmentRes1 = 0;
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
	SP_CalcFindStat stat;

	LoopCounter FindCounter;

	CFindDlg* DlgFind;
	char* FindParams;
	char* FindParamsDecoded;
	int m_FIND_REFRESH;
	int sRandSeed;


	SP_CalcFind()
	{
		DlgFind = nullptr;
		stat = {0};
		m_FIND_REFRESH = 5;
		FindParams = nullptr;
		FindParamsDecoded = nullptr;
		sRandSeed = -1;
		stat = { 0 };
	};
};

struct FindVPParam
{
	SPos Position;
	SPos Velocity;
	double TimePoint;

	void LinearMove(double in_Time)
	{
		SPos delta = Velocity;
		delta.MULT(in_Time);
		Position.ADD(delta);
		TimePoint += in_Time;
	}
};

struct FindVPPtrData
{
	FindVPParam StartData;
	double StopTime;
};


//==============================================
//PartT.Pos.X = Part0.Pos.X + Part0.Vel.X * Time
//ViPoT.Pos.X = ViPo0.Pos.X + ViPo0.Vel.X * Time * K
//
//S ^ 2 = (ViPoT.Pos.X - PartT.Pos.X) ^ 2     ++Y ++Z
//S ^ 2 = (Part0.Pos.X + Part0.Vel.X * Time - ViPo0.Pos.X - ViPo0.Vel.X * Time * K) ^ 2     ++Y ++Z
//S ^ 2 = ((Part0.Pos.X - ViPo0.Pos.X) + Time * (Part0.Vel.X - ViPo0.Vel.X * K)) ^ 2     ++Y ++Z
//DeltaPos0.X = Part0.Pos.X - ViPo0.Pos.X
//S ^ 2 = (DeltaPos0 + Time * (Part0.Vel.X - ViPo0.Vel.X * K)) ^ 2     ++Y ++Z
//DeltaVel.X = Part0.Vel.X - ViPo0.Vel.X * K
//S ^ 2 = (DeltaPos0.X + Time * DeltaVel.X) ^ 2     ++Y ++Z
//S ^ 2 = DeltaPos0.X ^ 2 + 2 * DeltaPos0.X * Time * DeltaVel.X + Time ^ 2 * DeltaVel.X ^ 2     ++Y ++Z
//0 = DeltaPos0.X ^ 2 + 2 * DeltaPos0.X * Time * DeltaVel.X + Time ^ 2 * DeltaVel.X ^ 2     ++Y ++Z - S ^ 2
//
//0 = DeltaPos0.X ^ 2 + 2 * DeltaPos0.X * Time * DeltaVel.X + Time ^ 2 * DeltaVel.X ^ 2 + DeltaPos0.Y ^ 2 + 2 * DeltaPos0.Y * Time * DeltaVel.Y + Time ^ 2 * DeltaVel.Y ^ 2 + DeltaPos0.Z ^ 2 + 2 * DeltaPos0.Z * Time * DeltaVel.Z + Time ^ 2 * DeltaVel.Z ^ 2 - S ^ 2
//S ^ 2 = ((DeltaT = (TimeP-TimeVp + DistanceToFind)) + Time) ^ 2 = DeltaT ^ 2 + 2 * DeltaT * Time + Time ^ 2
//
//0 = DeltaPos0.X ^ 2 + 2 * DeltaPos0.X * Time * DeltaVel.X + Time ^ 2 * DeltaVel.X ^ 2 + DeltaPos0.Y ^ 2 + 2 * DeltaPos0.Y * Time * DeltaVel.Y + Time ^ 2 * DeltaVel.Y ^ 2 + DeltaPos0.Z ^ 2 + 2 * DeltaPos0.Z * Time * DeltaVel.Z + Time ^ 2 * DeltaVel.Z ^ 2 - DeltaT ^ 2 - 2 * DeltaT * Time - Time ^ 2
//
//0 =
//Time ^ 2 * (+DeltaVel.X ^ 2 + DeltaVel.Y ^ 2 + DeltaVel.Z ^ 2 - 1)
//+ Time * (2 * {+DeltaPos0.X * DeltaVel.X + DeltaPos0.Y * DeltaVel.Y + DeltaPos0.Z * DeltaVel.Z - DeltaT))
//+ DeltaPos0.X ^ 2 + DeltaPos0.Y ^ 2 + DeltaPos0.Z ^ 2 - DeltaT ^ 2
//============ =
//0 =
//Time ^ 2 * (DeltaVel.LENPWR2 - 1)
//+ Time * (2 * { DeltaPos0.DOT(DeltaVel) + DeltaT))
//+ DeltaPos0.LENPWR2 - DeltaT ^ 2
//============ =
//A = DeltaVel.LENPWR2 - 1
//B = 2 * {DeltaPos0.DOT(DeltaVel) + DeltaT)
//C = DeltaPos0.LENPWR2 - DeltaT ^ 2
//==============================================

struct FindViewPointInInterval
{
	FindVPPtrData ViewPoint;
	FindVPPtrData Particle;

	FindVPParam ViewPointResult;
	FindVPParam ParticleResult;

	TwoDoubleRoots result;

	double ParticleTimeDelta;
	double DistanceToFind;

	int FindViewPointByDistance()
	{
		int ret = SP_STATE_ОК;
		double dT_Particle = Particle.StopTime - Particle.StartData.TimePoint;
		if (dT_Particle > 0.0)
		{
			//	A = DeltaVel.LENPWR2 - 1
			//	B = 2 * {DeltaPos0.DOT(DeltaVel) + DeltaT)
			//	C = DeltaPos0.LENPWR2 - DeltaT ^ 2

			double TimeK = (ViewPoint.StopTime - ViewPoint.StartData.TimePoint) / dT_Particle;
			double DeltaT = Particle.StartData.TimePoint - ViewPoint.StartData.TimePoint - DistanceToFind;

			SPos DeltaVel = ViewPoint.StartData.Velocity;
			DeltaVel.MULT(TimeK);
			DeltaVel.MINUS(Particle.StartData.Velocity);

			SPos DeltaPos0 = ViewPoint.StartData.Position;
			DeltaPos0.MINUS(Particle.StartData.Position);

			//double A = DeltaVel.LENPWR2() - 1;
			double B = DeltaVel.DOT(DeltaPos0) + DeltaT; B += B;
			//double C = DeltaPos0.LENPWR2() - DeltaT * DeltaT;

			result = QuadraticEequationSolution(/*A*/DeltaVel.LENPWR2() - 1.0, B, /*C*/DeltaPos0.LENPWR2() - DeltaT * DeltaT);

			ParticleTimeDelta = result.root1.Dbl;

			if (isnan(ParticleTimeDelta) || ParticleTimeDelta <= 0.0 || ParticleTimeDelta > dT_Particle)
				ParticleTimeDelta = result.root2.Dbl;

			if (isnan(ParticleTimeDelta) || ParticleTimeDelta <= 0.0 || ParticleTimeDelta > dT_Particle)
			{
				ret = SP_STATE_ERROR_MINOR;
				//ViewPointResult = { 0 };
				//Particle = { 0 };
			}
			else
			{
				ParticleResult = Particle.StartData;
				ParticleResult.LinearMove(ParticleTimeDelta);
				ViewPointResult = ViewPoint.StartData;
				ViewPointResult.LinearMove(ParticleTimeDelta * TimeK);
			}
		}
		else
			ret = SP_STATE_ERROR_MINOR;

		return ret;
	}
};

#ifdef UseTemplates_in_H
template <typename T_Data, typename T_TimeCompare>
#endif // UseTemplates_in_H
class SPTraceArrayDynEx;

#define ITGL_CALC_NONE			0x00000
#define ITGL_CALC_POSITION		0x00001
#define ITGL_CALC_SPEED			0x00002
#define ITGL_CALC_ACCELERATION	0x00004
#define ITGL_CALC_ACCDERIV		0x00008
#define ITGL_CALC_TIME			0x00010
#define ITGL_CALC_USELINEAR		0x00010



class CSpParticle
{
	friend class CSP_Group;
	friend class CDrawDlg;

private:
	SData* GetDataAtLowPoint(double in_ViewTime);

#ifdef UseTemplates_in_H
	SPTraceArrayDynEx<SData, double>* m_Data;
#else
	SPTraceArrayDynEx* m_Data;
#endif // UseTemplates_in_H


	SData m_NextData;
	SP_Calc* m_pCalc;
	int m_NewPointState;
	int m_StateNext;
	int m_StateCurr;
	int m_StatePrev;
	int m_TypeFlags;

public:
	int m_CountRelated;
	SDrawTraceSettings m_TraceSettings;

public:
	//SData* GetDataByIdLinear(int in_Id);
	SData* GetDataById(int in_Id);
	SData* GetCurData(); 
	const SData* GetCurData_C();
	const SData* GetPrevData_C();
	int GetCurData(SData* out_Data);
	SData* GetNextDataPtr() { return &m_NextData; };
	SData GetNextData() { return m_NextData; };

	void GetLastDeltaAcc(SPos& in_DeltaAcc);
	int GetTypeFlags() { return m_TypeFlags; }
	int GetParticleType() { return (m_TypeFlags & SP_TYPE_FORM_MASK); }

	static int ItgrlFull(SData& Result, const SData& DataSrc, double in_Time, const SP_Calc& in_pCalcParams, double * const out_KeyPoint, int in_Flags);
	static int ItgrlFullMoveOnMaxSpeedOnly(SData& Result, const SData& DataSrc, double in_Time, const SP_Calc& in_pCalcParams, int in_CalcFlag);

	int CalcMidData(SData& Result, const SData&  Ptr1, const SData& Ptr2, double absMidPoint);
	int Itgrl(SPos* const D_Res, SPos* const D1, double in_Mltpl, int iLev, int iMaxLev, double Tx, double DensMltpl);
	int Itgrl(SPos* const D_Res, SPos* const D1, int iMaxLev, double Tx, double DensMltpl);

	static int Itgrl_s(SPos* const D_Res, SPos* const D1, double in_Mltpl, int iLev, int iMaxLev, double Tx);
	static int Itgrl_s(SPos* const D_Res, SPos* const D1, int iMaxLev, double Tx);


public:
	///NewPoin
	int MoveToNewPoint(int AddType);
	int NewPoint_Begin(double in_NewTimePoint);
	void NewPoint_Clear();
	///
	void Clear(int in_Flags);

	SPos GetCurPosition() { return GetCurData_C()->Position; };
	void GetCurPosition(SPos& inout_Position) { inout_Position = GetCurData_C()->Position; };
	SPos GetCurVelocity() { return GetCurData_C()->Velocity; };
	SPos GetCurVelocity_Linear() { return GetCurData_C()->Velocity_Linear; };
	void GetCurVelocity(SPos &inout_V) { inout_V = GetCurData_C()->Velocity; };
	void GetCurVelocity_Linear(SPos &inout_V) { inout_V = GetCurData_C()->Velocity_Linear; };
	double GetCurSpeedValue(){ return GetCurData_C()->AbsSpeed; };
	SPos GetCurAcc() { return GetCurData_C()->Accel_2_3D; };
	int GetCurPos();
	double GetCurTime();
	double GetPrevTime(int n_StepsBefore);
	//для получения точки видимости на прямолмнейной равномерной траектории
	int FindViewPointByTimeLinear_LE0(SData& out_Data, const SData& in_Pos, double in_ViewTime, double in_StartTime);
	int FindViewPointMinIdByTime(const SPos& in_Pos, double in_ViewTime, double in_StartTime, double in_EndTime);
	int FindViewPointByTime(SData& out_Data, const SData& in_Pos, double in_ViewTime, double in_StartTime, double in_EndTime, int * const out_LowIdx = nullptr);
	//int FindViewPointByDistance(FindInIntervalParams & Params);
	int GetPrevDataByTime(SData& out_Data, double in_ViewTime, double * const out_KeyPoint);
	int GetPrevDataByTimeLinear(SData& out_Data, int& out_Idx, int StartIdx, double in_ViewTime);
	int GetNextDataByTime(SData& out_Data, double in_ViewTime, double * const out_KeyPoint);
	int GetDataAtLowPoint(SData& out_Data, double in_ViewTime);
	char* GetStateDescription(char *out_Str, int in_StrLen, double in_ReportTime);
	char* GetStateDescription(char* out_Str, int in_StrLen, int in_nPoints);
	void GetMinMaxTracePos(SPos& min_Pos, SPos& max_Pos, int idxStart, int idx_End);
	int GetLowIdxByTime(double in_ViewTime);
	int GetLowIdxByTime(double in_ViewTime, int in_IdxStart);
	
	static char* GetFlagsDescription(char* out_Str, int in_Flags);
	int GetDataById(int in_Idx, SData *inout_Reseult);

public:
	CSpParticle(SP_Calc& in_pCalc, int in_TypeFlags);
	CSpParticle(int in_MaxItems, int in_TypeFlags);
	int SetParticleData(int in_flags, const SData& in_Ptr);

	~CSpParticle();
};




