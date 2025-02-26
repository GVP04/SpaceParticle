#include "pch.h"
#include "CSP_Group.h"


CSP_Group::CSP_Group()
{
	m_Array = new CSpParticle * [SPG_MAX_COUNT];
	ZeroMemory(m_Array,  sizeof(CSpParticle *) * SPG_MAX_COUNT);
	m_Count = 0;
	m_StateCounter = 0;
}
CSP_Group::~CSP_Group()
{
	if (m_Array)
	{
		for (int i = 0; i < m_Count; i++) 
			if (m_Array[i])
			{
				delete m_Array[i]; 
				m_Array[i] = NULL; 
			}
		delete[] m_Array;
		m_Array = NULL;
	}
}


int CSP_Group::AddSpParticle(int in_flags, int in_Count)
{
	int ret = m_Count;
	if (m_Array)
	{
		while (in_Count-- > 0 && m_Count < SPG_MAX_COUNT - 1)
			m_Array[m_Count++] = new CSpParticle;
	}

	return m_Count - ret;
}


int CSP_Group::NextStep()
{
	if (m_Array && m_Count > 0)
	{
		for (int i = 0; i < m_Count; i++) 
			if (m_Array[i])
			{
				m_Array[i]->NewPoint_Begin();
				for (int j = 0; j < m_Count; j++)
					if (i!=j && m_Array[j])
					{
						m_Array[i]->NewPoint_AddExternSP(*m_Array[j]);

					}
			}
	}
	return m_Count;
}

const CSpParticle* CSP_Group::GetItem(int in_Idx)
{
	CSpParticle* ret = NULL;

	if (m_Array && in_Idx >= 0 && in_Idx < m_Count)
		ret = m_Array[in_Idx];

	return ret;
}

int CSP_Group::SetStateCntr(int in_NewCntr)
{
	if (in_NewCntr > m_StateCounter) m_StateCounter = in_NewCntr;

	return m_StateCounter;
}

int CSP_Group::SetParticleData(int in_Idx, int in_flags, const SData& in_Ptr)
{
	int ret = 0;
	if (m_Array && in_Idx >= 0 && in_Idx < m_Count)
		ret = m_Array[in_Idx]->SetParticleData(in_flags,  in_Ptr);

	return ret;
}
