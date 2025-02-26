#pragma once
#include "CSpParticle.h"

#define SPG_MAX_COUNT	0x1000
#define SPG_STATE_CNTR_STEP 0x10

class CSP_Group
{
private:
	CSpParticle** m_Array;
	int m_Count;
	int m_StateCounter;

public:
	int AddSpParticle(int in_flags, int in_Count = 1);
	int SetStateCntr(int in_NewCntr);
	int SetParticleData(int in_Idx, int in_flags, const SData& in_Ptr);

	int NextStep();
	CSP_Group();
	~CSP_Group();
	const CSpParticle* GetItem(int in_Idx);

};

