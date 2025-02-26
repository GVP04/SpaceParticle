#include "pch.h"
#include "CSpParticle.h"
#include "CSDataArray.h"


CSpParticle::CSpParticle()
{
	m_Data = new CSDataArray;
	NewPoint_Clear();
}

CSpParticle::CSpParticle(int in_MaxItems)
{
	m_Data = new CSDataArray(in_MaxItems);
}

CSpParticle::~CSpParticle()
{
	if (m_Data) delete m_Data;
}

SData* CSpParticle::GetCurData()
{
	return m_Data == NULL ? &CSDataArray::EmptyData : m_Data->GetCurData();
}

void CSpParticle::NewPoint_Begin()
{
	NewPoint_Clear();
	m_NewPointState = SP_NEWPOINT_STATE_ON;
}

int CSpParticle::MoveToNewPoint(bool in_ClearData)
{
	//////
	NewPoint_Clear();

	return 0;
}

void CSpParticle::NewPoint_Clear()
{
	m_NewPointState = SP_NEWPOINT_STATE_OFF;
	m_NextData = { 0 };
}

void CSpParticle::NewPoint_AddExternSP(const SData& Ptr)
{

}

void CSpParticle::NewPoint_AddExternSP(const CSpParticle& Ptr)
{

}

int CSpParticle::SetParticleData(int in_flags, const SData& in_Ptr)
{
	int ret = 0;

	if (m_Data)
	{
		SData* curData = m_Data->GetCurData();
		if (curData)
		{
			if ((in_flags & SP_DATA_DENCITY))
			{
				curData->Dencity = in_Ptr.Dencity;
				ret |= SP_DATA_DENCITY; 
			}
			if ((in_flags & SP_DATA_SPEED)) 
			{
				curData->Speed = in_Ptr.Speed;
				ret |= SP_DATA_SPEED;
			}
			if ((in_flags & SP_DATA_POSITION))
			{
				curData->position = in_Ptr.position; 
				ret |= SP_DATA_POSITION;
			}
			if ((in_flags & SP_DATA_COUNTER))
			{ 
				curData->counter = in_Ptr.counter;
				ret |= SP_DATA_COUNTER;
			}
		}
	}

	return ret;
}




/////////////////////// STATIC ///////////////////////
double CSpParticle::GetDistance(CSpParticle *Ptr1, CSpParticle *Ptr2) {	return CSpParticle::GetDistance(*Ptr1->GetCurData(), *Ptr2->GetCurData());}
double CSpParticle::GetDistance(const SData& Ptr1, const SData& Ptr2) {	return CSpParticle::GetDistance(Ptr1.position, Ptr2.position);}
double CSpParticle::GetDistance(const SPos &Ptr1, const SPos& Ptr2)   {	return sqrt(CSpParticle::GetDistancePwr2(Ptr1, Ptr2));}

double CSpParticle::GetDistancePwr2(CSpParticle* Ptr1, CSpParticle* Ptr2) {	return CSpParticle::GetDistancePwr2(*Ptr1->GetCurData(), *Ptr2->GetCurData());}
double CSpParticle::GetDistancePwr2(const SData& Ptr1, const SData& Ptr2) {	return CSpParticle::GetDistancePwr2(Ptr1.position, Ptr2.position);}
double CSpParticle::GetDistancePwr2(const SPos& Ptr1, const SPos& Ptr2)   
{
	return (Ptr1.X - Ptr2.X) * (Ptr1.X - Ptr2.X) + (Ptr1.Y - Ptr2.Y) * (Ptr1.Y - Ptr2.Y) + (Ptr1.Z - Ptr2.Z) * (Ptr1.Z - Ptr2.Z);
}

double CSpParticle::GetRelativeAbsSpeed(CSpParticle *Ptr1, CSpParticle *Ptr2) {	return CSpParticle::GetDistance(*Ptr1->GetCurData(), *Ptr2->GetCurData());}
double CSpParticle::GetRelativeAbsSpeed(const SData& Ptr1, const SData& Ptr2) {	return CSpParticle::GetDistance(Ptr1.Speed, Ptr2.Speed);}
double CSpParticle::GetRelativeAbsSpeed(const SPos& Ptr1, const SPos& Ptr2)
{
	return sqrt((Ptr1.X - Ptr2.X) * (Ptr1.X - Ptr2.X) + (Ptr1.Y - Ptr2.Y) * (Ptr1.Y - Ptr2.Y) + (Ptr1.Z - Ptr2.Z) * (Ptr1.Z - Ptr2.Z));
}

double CSpParticle::GetRelativeSpeed(CSpParticle *Ptr1, CSpParticle *Ptr2) {	return CSpParticle::GetRelativeSpeed(*Ptr1->GetCurData(), *Ptr2->GetCurData());}
double CSpParticle::GetRelativeSpeed(const SData& Ptr1, const SData& Ptr2)
{
	/// Бредовое и дурацкое решение. Надо переделать
	SPosUnion deltaSpeed;
	deltaSpeed.posDbl.X = Ptr1.Speed.X - Ptr2.Speed.X;
	deltaSpeed.posDbl.Y = Ptr1.Speed.Y - Ptr2.Speed.Y;
	deltaSpeed.posDbl.Z = Ptr1.Speed.Z - Ptr2.Speed.Z;

	SPosUnion deltaPos;
	deltaPos.posDbl.X = Ptr1.position.X - Ptr2.position.X;
	deltaPos.posDbl.Y = Ptr1.position.Y - Ptr2.position.Y;
	deltaPos.posDbl.Z = Ptr1.position.Z - Ptr2.position.Z;

	double deltaT = 0.0;
	if (((deltaSpeed.posInt64.X ^ deltaPos.posInt64.X) & 0x8000000000000000) && deltaSpeed.posDbl.X != 0.0) //сближение
		deltaT = deltaPos.posDbl.X / deltaSpeed.posDbl.X;

	if (((deltaSpeed.posInt64.Y ^ deltaPos.posInt64.Y) & 0x8000000000000000) && deltaSpeed.posDbl.Y != 0.0) //сближение
	{
		double deltaT_ = deltaPos.posDbl.Y / deltaSpeed.posDbl.Y;
		if (deltaT_ < deltaT) deltaT = deltaT_;
	}

	if (((deltaSpeed.posInt64.Z ^ deltaPos.posInt64.Z) & 0x8000000000000000) && deltaSpeed.posDbl.Z != 0.0) //сближение
	{
		double deltaT_ = deltaPos.posDbl.Z / deltaSpeed.posDbl.Z;
		if (deltaT_ < deltaT) deltaT = deltaT_;
	}

	deltaT /= 2.0;
	SPos newPos;
	newPos.X = Ptr2.position.X + deltaT * deltaSpeed.posDbl.X;
	newPos.Y = Ptr2.position.Y + deltaT * deltaSpeed.posDbl.Y;
	newPos.Z = Ptr2.position.Z + deltaT * deltaSpeed.posDbl.Z;

	double CurDist = CSpParticle::GetDistance(Ptr1.position, Ptr2.position);
	double NewDist = CSpParticle::GetDistance(Ptr1.position, newPos);
	double AbsSpeed = CSpParticle::GetRelativeAbsSpeed(Ptr1, Ptr2);

	return NewDist > CurDist ? AbsSpeed : -AbsSpeed;
}
