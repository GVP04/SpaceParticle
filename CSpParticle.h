#pragma once

#define SP_NEWPOINT_STATE_OFF		0x0000
#define SP_NEWPOINT_STATE_ON		0x0001
#define SP_NEWPOINT_STATE_READY		0x0003

#define SP_DATA_DENCITY			0x0001
#define SP_DATA_SPEED			0x0002
#define SP_DATA_POSITION		0x0004
#define SP_DATA_COUNTER			0x0008
#define SP_DATA_ALL				0x000F
#define SP_DATA_ALL_WO_COUNTER	0x0007


#define SDA_DEF_AMOUNT		0x10000

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

// пока не нужно
struct SVector : SPos
{
	double Value;
};


struct SData
{
	SPos position;
	SPos Speed;
	SPos Dencity;
	__int64 counter; //счетчик состояния (типа время), скважность относительна
};

class CSDataArray;

class CSpParticle
{
public:
	SData *GetCurData();

	// STATIC <<<<<<<<<<
	static double GetDistance(CSpParticle *Ptr1, CSpParticle *Ptr2);
	static double GetDistance(const SData &Ptr1, const SData &Ptr2);
	static double GetDistance(const SPos &Ptr1, const SPos &Ptr2);
	static double GetDistancePwr2(CSpParticle *Ptr1, CSpParticle *Ptr2);
	static double GetDistancePwr2(const SData &Ptr1, const SData &Ptr2);
	static double GetDistancePwr2(const SPos &Ptr1, const SPos &Ptr2);

	static double GetRelativeAbsSpeed(CSpParticle* Ptr1, CSpParticle* Ptr2);
	static double GetRelativeAbsSpeed(const SData& Ptr1, const SData& Ptr2);
	static double GetRelativeAbsSpeed(const SPos& Ptr1, const SPos& Ptr2);

	static double GetRelativeSpeed(CSpParticle* Ptr1, CSpParticle* Ptr2);
	static double GetRelativeSpeed(const SData& Ptr1, const SData& Ptr2);
	// STATIC >>>>>>>>>>

	int MoveToNewPoint(bool in_ClearData = true);
public:
	void NewPoint_Begin();
	void NewPoint_Clear();
	void NewPoint_AddExternSP(const SData& Ptr);
	void NewPoint_AddExternSP(const CSpParticle& Ptr);


private:
	CSDataArray *m_Data;
	SData m_NextData;
	int m_NewPointState;

public:
	CSpParticle();
	CSpParticle(int in_MaxItems);
	int SetParticleData(int in_flags, const SData& in_Ptr);

	~CSpParticle();
};

