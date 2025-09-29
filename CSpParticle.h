#pragma once


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

// аболютная скорость изменила статус предельной скорости
#define SP_STATE_ABS_CHANGE_LS		0x00000400
// аболютная скорость равна предельной
#define SP_STATE_ABS_SPEED_AL		0x00000800
// относительная скорость перешла границу предельной
#define SP_STATE_PHANTOM_DIST_LE1	0x00002000
// фантомное расстояние меньше 1.0 - толщина родительского пространства
#define SP_STATE_REL_CHANGE_LS		0x00004000
// относительная скорость выше предельной
#define SP_STATE_REL_SPEED_AL		0x00008000



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
struct SPos
{
	double X;
	double Y;
	double Z;
};

struct SPosI64
{
	__int64 X;
	__int64 Y;
	__int64 Z;
};

union SPosUnion
{
	SPos posDbl;
	SPosI64 posInt64;
};


struct SData_SD
{
	double position;
	double Speed;
	double Accel3D;
	double AccelDeriv3D;
	double Density; 
	double TimePoint; // типа, это время
};


struct SData
{
	SPos position;
	SPos Speed;
	SPos Accel3D;   
	SPos AccelDeriv3D;
	double AbsSpeed;
	double Density;  
	double TimePoint; // типа, это время
};

struct SPRelation
{
	SData ViewPointData;

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
};

struct SSingleRelation
{
	SPRelation relation;
	int idxParticle1;
	int idxParticle2;
	SData Part1;
	SData Part2;
	double MMValue;
};


struct SMinMaxRelation
{
	/// <summary>
	/// SSingleRelation содержит идентификатор частицы, что опасно в случае реорганизации массива частиц
	/// </summary>
	SSingleRelation Distance;
	SSingleRelation Speed;
};


#define SP_SHOWFLAG_TEXT		0x0000000FF 
#define SP_SHOWFLAG_TRACE		0x00000FF00 
#define SP_SHOWFLAG_CLEARBKG	0x000010000 

#define SP_DRAW_XYZ			0x000000003 
#define SP_DRAW_XY			0x000000000 
#define SP_DRAW_XZ			0x000000001 
#define SP_DRAW_YZ			0x000000002 

#define SP_DRAW_3D			0x000000010 


struct SDrawSettings
{
	SPos m_00_Pos; //позиция нулевой координаты на картинке
	double ViewMltpl;   //множитель (увеличение)
	SPos BMP_Size;  //размер картинки
	SPos PosMin; //Окно просмотра в позициях траектории
	SPos PosMax; //Окно просмотра в позициях траектории

	int ShowItems;
	int ShowLinksFrom;
	int ShowLinksTo;
	int nLinks;
	int iShowFlags;
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
};


struct SP_Calc
{
	SP_CalcStat Stat;
	CSP_Group* Group;
	int State;
	CDrawDlg* DlgTrace;
	SDrawSettings DrawSet;
	SP_Refresh refresh;
	SPTime Time;
	////////////////////////
	char* CalcParams;
	double maxRelativeSpeed;
	double maxAbsSpeed;
	double ParentSpaceThickness;

	int AccCalcType; //0 нет влияния плотности на время/скорость. 1 есть влияние умножене. 2 есть влияние умножене. 
	int DensityCalcType; //
	//0  -  dens = 1 - 1 / S^2
	//1  -  dens = 1 - 1 / (1 + S^2)
	//2  -  dens = 1 - 1 / (1 + S)^2

	int AdditiveType; //0 аддитивность отсутствует, т.е. каждая пара частиц взаимодействует независимо. 1 - аддитивность есть.
	int ClassicType; // 1 - ViewPoint в точке нахождения частицы на момент запроса. 2 - ViewPoint в точке следующей расчетной позиции если таковая есть
	int OutPutFlags;
	int DensityDerivLevel;
	int WindType;
	int CurStep;
};


struct SP_CalcFindStat
{
	int Count;
	int Reported;
};



struct SP_CalcFindSet
{
	SPos minPos;
	SPos maxPos;
	SPos minSpeed;
	SPos maxSpeed;

	double Stop_Time;
	double Stop_minRelDist;
	double Stop_minMaxRelSpeed;
	double Stop_CurStep;

	double ExcludeDistance;
};




struct SP_CalcFind
{
	SP_Calc curCalc;
	SP_CalcFindSet Settings;
	int FindState;
	CFindDlg* DlgFind;
	SP_CalcFindStat stat;
	int m_FIND_REFRESH;
	char* FindParams;
};


class CSDataArray;

class CSpParticle
{
	friend class CSP_Group;
	friend class CDrawDlg;
	friend void PaintTraceToCDC(CDC* DCMem, SP_Calc* mData, int in_DrawFlags, SPos& delta);

public:
	SData *GetCurData();

	// STATIC <<<<<<<<<<
	static double GetDistance(CSpParticle *Ptr1, CSpParticle *Ptr2);
	static double GetDistance(const SData &Ptr1, const SData &Ptr2);
	static double GetDistance(const SPos &Ptr1, const SPos &Ptr2);

	static double GetDistancePwr2(CSpParticle *Ptr1, CSpParticle *Ptr2);
	static double GetDistancePwr2(const SData &Ptr1, const SData &Ptr2);
	static double GetDistancePwr2(const SPos &Ptr1, const SPos &Ptr2);

	static double Get3D_Pwr2(const SPos& Ptr);
	static double Get3D_Pwr1(const SPos& Ptr) { return sqrt(Get3D_Pwr2(Ptr)); }

	static int ItgrlFull(SData* Result, const SData& DataSrc, double in_Time, SP_Calc* in_pCalcParams);
	static double GetRelativeSpeed(CSpParticle* Ptr1, CSpParticle* Ptr2, SPos *RSpeed3D, SP_Calc* in_pCalcParams);
	static double GetRelativeSpeed(const SData& Ptr1, const SData& Ptr2, SPos* RSpeed3D, SP_Calc* in_pCalcParams);

	//recurcive
	static int Itgrl_s(SPos* D_Res, SPos* D1, double in_Mltpl, int iLev, int iMaxLev, double Tx);
	static int Itgrl_s(SPos* D_Res, SPos* D1, int iMaxLev, double Tx);
	static char* GetFlagsDescription(char* out_Str, int in_Flags);
	// STATIC >>>>>>>>>>

	int CalcMidData( SData *Result, const SData& Ptr1, const SData& Ptr2, double absMidPoint);
	int Itgrl(SPos* D_Res, SPos* D1, double in_Mltpl, int iLev, int iMaxLev, double Tx, double DensMltpl);
	int Itgrl(SPos* D_Res, SPos* D1, int iMaxLev, double Tx, double DensMltpl);

public:
	///NewPoin
	int MoveToNewPoint(int in_maxCountLimit, bool in_ClearData = true);
	int NewPoint_Begin(double in_NewTimePoint);
	void NewPoint_Clear();
	int NewPoint_CheckLevel();
	///
	int CalcPrevData(SData* out_Data, double in_DataTime);
	void Clear();

	SPos GetCurPosition();
	double GetCurTime();
	double GetPrevTime(int n_StepsBefore);
	int GetPointCount();
	int FindViewPoint(SData* out_Data, SData* in_Pos, double in_ViewTime, SPRelation *in_LastRelation);
	int GetPrevDataByTime(SData *out_Data, double in_ViewTime, double *out_KeyPoint);
	int GetNextDataByTime(SData *out_Data, double in_ViewTime, double *out_KeyPoint);
	int GetDataAtLowPoint(SData* out_Data, double in_ViewTime);
	int DeletePoints(double TimeFromGE, double TimeToLE);
	char* GetStateDescription(char *out_Str, double in_ReportTime);
	char* GetStateDescription(char* out_Str, int in_nPoints = 1);
	double GetAbsSpeedPwr2();
	void GetMinMaxTracePos(SPos& min_Pos, SPos& max_Pos);

private:
	SData* GetDataAtLowPoint(double in_ViewTime);
	CSDataArray *m_Data;
	int m_NewPointState;
	SData m_NextData;
	SP_Calc* m_pCalc;

public:
	CSpParticle(SP_Calc *in_pCalc);
	CSpParticle(int in_MaxItems);
	int SetParticleData(int in_flags, const SData *in_Ptr);

	~CSpParticle();
};

