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
#define SP_DENSITY_MIN_DELTA	0.000000000001

#define SP_CLEAR_SOFT			0x0000
#define SP_CLEAR_HARD			0x0001


#define SP_PREC_TIME_MLTPL		0.00000000000001
#define SP_PREC_SPEED_PWR2		0.0000000001
#define SP_PREC_LIMIT			0.0000000000001

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

#define SP_STATE_WARNING_MASK					0x000FFF00		

// аболютная скорость изменила статус предельной скорости
#define SP_STATE_ABS_CHANGE_LS					0x00000400
// аболютная скорость равна предельной
#define SP_STATE_ABS_SPEED_AL					0x00000800

// фантомное расстояние меньше CriticalDistance
#define SP_STATE_PHANTOM_DIST_LE_CRITICAL		0x00002000
#define SP_STATE_PHANTOM_BESTUCK				0x00004000
// фантомное расстояние менее или равно CriticalDistance
#define SP_STATE_PHANTOM_DIST_CRITICAL_CHG		0x00008000
#define SP_STATE_ANTIPHANTOM_DIST_CRITICAL_CHG	0x00010000

// фантомная плотность меньше 1
#define SP_STATE_PHANTOM_DENSITY_LE_0			0x00040000
// фантомная плотность равна 1
#define SP_STATE_PHANTOM_DENSITY_LE_CHG			0x00080000
#define SP_STATE_ANTIPHANTOM_DENSITY_LE_CHG		0x00100000

// ускорение меньше 0
#define SP_STATE_REL_ACCELERATION_INF			0x00020000

#define SP_STATE_PHANTOM_CRITICAL_CHANGED		(SP_STATE_PHANTOM_DIST_CRITICAL_CHG|SP_STATE_PHANTOM_DENSITY_LE_CHG)
#define SP_STATE_ANTIPHANTOM_CRITICAL_CHANGED	(SP_STATE_ANTIPHANTOM_DIST_CRITICAL_CHG|SP_STATE_ANTIPHANTOM_DENSITY_LE_CHG)





//////////////////WARNINGS

#define SP_STATE_RELATION_MASK			0x0F000000
#define SP_STATE_RELATION_MASK_CLR_CMD	0xF0FFFFFF
#define SP_STATE_RELATION_MASK_CLR_ALL	0x00000000
#define SP_STATE_RELATION_MASK_CLR_NDST	0x00002000
#define SP_STATE_NEED_RECALC			0x01000000
#define SP_STATE_NEED_UPDATE			0x03000000
#define SP_STATE_FAR_AWAY				0x00000100
#define SP_STATE_AFTER_FAR_AWAY			0x00000200



#define SDA_DEF_AMOUNT		0x100000
#define SD_PARAMSLEN		0x10000


//Флаги расчета CalcSingleRelation

#define SP_CALCRELATION_FIND_SIMPLE				0x00000000
#define SP_CALCRELATION_FIND_VIEWPOINT			0x00000001
#define SP_CALCRELATION_BREAK_ON_MINOR_ERROR	0x00000002
#define SP_CALCRELATION_NEED_UPDATE_ONLY		0x00000004
#define SP_CALCRELATION_NEED_UPDATE_ONLY		0x00000004


class CDrawDlg;
class CFindDlg;
class CSP_Group;

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


	// Расчёт третьей производной (рывка)
	void CALC_JERK_AS_RELATIVE()
	{
		if (DistancePwr2 != 1.0)
		{
			double DistPwr2minus1 = DistancePwr2 - 1.0;
			double a_magnitude = 1.0 / DistPwr2minus1;
			double DistDotVelocity = Distance_3D.DOT(RelativeVelocity_3D);
			double d_a_dt = -2.0 * DistancePwr1 * DistDotVelocity / (DistPwr2minus1 * DistPwr2minus1);

		
			// Производная единичного вектора
			SPos dn_dt = RelativeVelocity_3D; dn_dt.DIV(DistancePwr1);
			SPos dn_dt2 = Distance_3D;  dn_dt2.MULT( DistDotVelocity / DistancePwr3);
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
	void CALC_SNAP_AS_RELATIVE()
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
	SData ByBeStuckData;
	SData ByNormalData;
};

union RelationAndBeStuck
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
	SSingleRelation Velocity;

	void CLEAR()
	{
		//Distance.CLEAR();
		//Velocity.CLEAR();

		ZeroMemory(&Real_Distance, sizeof(SSingleRelation));
		ZeroMemory(&Phantom_Distance, sizeof(SSingleRelation));
		ZeroMemory(&Velocity, sizeof(SSingleRelation));
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

	PrecissionInfo PrecInfo;


	//установки
	double MinStepSet;
	double TraceMinStep;
	double HistoryMinStep;
	double TraceBaseStep;
	double BeStuckMaxStep;
	double MaxStep;

	//stat
	__int64 start_Calc;
	__int64 end_Calc;

	__int64 start_CalcCur;
	__int64 end_CalcCur;

	inline void SetMinStepDeltaMinus() { MinStepDeltaMinus = MinStep * 0.1; };

public:

	void CLEAR()
	{
		if (MinStepSet < SP_PREC_TIME_MLTPL) MinStepSet = SP_PREC_TIME_MLTPL;
		if (MinStep < MinStepSet) MinStep = MinStepSet;

		TraceMinStep = 0.0000001;
		HistoryMinStep = 0.1;
		TraceBaseStep = 0.001;
		BeStuckMaxStep = 0.00001;
		PrevPrevStep = PrevStep = NextStep = MinStep;
		NextStep4321 = NextStep * 0.4321;
		SetMinStepDeltaMinus();
		PrevTime = CurTime = 0.0;
		PrevUpLimit = UpLimit = SP_MAX_TIME_DISTANCE;
		NextTime = SP_PREC_TIME_MLTPL;
		FixStartTime();
		PrecInfo.CLEAR();
	}

	void RESET()
	{
		CLEAR();
		SetMinStep(SP_PREC_TIME_MLTPL);
		SetMaxStep(0.01);
	}

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

	double CheckByBeStuckStep()
	{
		return BeStuckMaxStep < NextStep ? SetStep(BeStuckMaxStep) : NextStep;
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

	double SetBeStuckMaxStep(double in_Step)
	{
		double ret = BeStuckMaxStep;
		
		if (BeStuckMaxStep < SP_PREC_TIME_MLTPL) BeStuckMaxStep = SP_PREC_TIME_MLTPL;
		else BeStuckMaxStep = in_Step;

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
		NextStep4321 = NextStep * 0.4321;
		return NextTime;
	}

	double SetNextTimeEx(double in_NextTime)
	{
		double delta = NextTime - in_NextTime;

		if (delta < 0)
			SetNextTimeForce(in_NextTime);
		else if (NextStep * 0.05 > delta)
			SetStep(NextStep * 0.90);
		else if (NextStep * 0.95 < delta)
			SetStep(NextStep * 0.10);
		else
			SetStep(in_NextTime - CurTime);

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
			if (CurTime > UpLimit) UpLimit = CurTime + NextStep;
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
		if (MaxStepTmp >= MinStep && MaxStepTmp < PrevStep)
		{
			SetStep(MaxStepTmp);
		}
		else 
			if (PrevPrevStep > PrevStep)
				SetStep(PrevStep);
			else
				SetStep(PrevStep * 1.2);
		if (IsMinimalStep())
			PrevUpLimit = UpLimit = NextTime;
	}

	double GetUpLimit() const { return UpLimit; }
	double SetUpLimit(double in_tUpLimit) 
	{
		UpLimit = in_tUpLimit - MinStep * 0.5;

		if (UpLimit < CurTime + MinStep) UpLimit = CurTime + MinStep;
		if (UpLimit > NextTime) UpLimit = NextTime;

		return UpLimit; 
	}

	void FixStartTimeCur()
	{
		end_Calc = end_CalcCur = start_CalcCur = GetTickCount64();
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

struct SP_Calc
{

	//Runtime
	SP_CalcStat Stat;
	SPTime Time;

	CSP_Group* Group;
	CDrawDlg* DlgTrace;

	SDrawSettings DrawSet;
	SP_Refresh refresh;

	LoopCounter CalcCounter;

	SP_CalcFindSet CalcFindSet;

	//settings&rules//////////////////////
	char* CalcParams;
	double maxAbsSpeed;
	double maxAbsSpeedPwr2;
	double ParentSpaceThickness;
	double CriticalDistance;
	double CriticalDistancePwr2;
	double CriticalDistancePwr3;

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

	int nIterations;
	int OutPutFlags;

	int AlighmentRes1;



	SP_Calc()
	{
		Stat.CLEAR();
		Time.RESET();
		nIterations = 1000000;

		Group = nullptr;;
		DlgTrace = nullptr;;

		refresh = { 0 };

		CalcParams = nullptr;

		maxAbsSpeed = 1.0;
		maxAbsSpeedPwr2 = 1.0;
		ParentSpaceThickness = 0.0;
		CriticalDistance = 1.0;
		CriticalDistancePwr2 = 1.0;
		CriticalDistancePwr3 = 1.0;
		AccCalcType = 0; 
		DensityCalcType = 0;
		sRandSeed_Calc = -1;

		AdditiveType = 0;
		ClassicType = 0;
		DensityDerivLevel = 0;
		WindType = 0;
		CollisionType = COLISSION_SLIPPERY | COLISSION_SPEED_OPPOSITE | COLISSION_POS_SET1 | COLISSION_ACCEL_SET1;
	
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
	int m_FIND_REFRESH;
	int sRandSeed;


	SP_CalcFind()
	{
		DlgFind = nullptr;
		stat = {0};
		m_FIND_REFRESH = 5;
		FindParams = nullptr;
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
	int m_CountBeStuck;

public:
	SData* GetDataById(int in_Id);
	SData* GetCurData(); 
	const SData* GetCurData_C();
	const SData* GetPrevData_C();
	int GetCurData(SData* out_Data);
	SData* GetNextDataPtr() { return &m_NextData; };
	SData GetNextData() { return m_NextData; };

	void GetLastDeltaAcc(SPos& in_DeltaAcc);

	static int ItgrlFull(SData& Result, const SData& DataSrc, double in_Time, const SP_Calc& in_pCalcParams, double * const out_KeyPoint);
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
	int CalcPrevData(SData& out_Data, double in_DataTime);
	void Clear(int in_Flags);

	SPos GetCurPosition() { return GetCurData_C()->Position; };
	void GetCurPosition(SPos& inout_Position) { inout_Position = GetCurData_C()->Position; };
	SPos GetCurSpeed() { return GetCurData_C()->Velocity; };
	void GetCurSpeed(SPos &inout_Speed) { inout_Speed = GetCurData_C()->Velocity; };
	double GetCurSpeedValue(){ return GetCurData_C()->AbsSpeed; };
	SPos GetCurAcc() { return GetCurData_C()->Accel_2_3D; };
	int GetCurPos();
	double GetCurTime();
	double GetPrevTime(int n_StepsBefore);
	int GetPointCount();
	int FindViewPointByTimeLinear(SData& out_Data, const SData& in_Pos, double in_ViewTime, double in_StartTime);
	int FindViewPointMinIdByTime(const SPos& in_Pos, double in_ViewTime, double in_StartTime, double in_EndTime);
	int FindViewPointByTime(SData& out_Data, const SData& in_Pos, double in_ViewTime, double in_StartTime, double in_EndTime, int * const out_LowIdx = nullptr);
	//int FindViewPointByDistance(FindInIntervalParams & Params);
	int GetPrevDataByTime(SData& out_Data, double in_ViewTime, double * const out_KeyPoint);
	int GetPrevDataByTimeFast_(SData& out_Data, int& out_Idx, int StartIdx, double in_ViewTime, double* const out_KeyPoint);
	int GetPrevDataByTimeLinear(SData& out_Data, int& out_Idx, int StartIdx, double in_ViewTime);
	int GetNextDataByTime(SData& out_Data, double in_ViewTime, double * const out_KeyPoint);
	int GetMovement(SData & out_Data, const SData & cur_Data, double in_NewTime, double* const out_KeyPoint);
	int GetDataAtLowPoint(SData& out_Data, double in_ViewTime);
	char* GetStateDescription(char *out_Str, int in_StrLen, double in_ReportTime);
	char* GetStateDescription(char* out_Str, int in_StrLen, int in_nPoints);
	double GetAbsSpeedPwr2();
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




double Calc_RelativeAcceleration(int in_RelPartType, int in_CalcType, double in_DensityAtBasePoint);
double Calc_RelativeDensity(int in_RelPartType, int in_CalcType, double in_DistancePwr1, double in_DistancePwr2);
