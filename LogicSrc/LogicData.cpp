#include "LogicData.h"

LogicData::LogicData( int nMaxPlayer ) : m_DataPC(nMaxPlayer), m_nMaxPlayer(nMaxPlayer)
{
	m_DataPC.MakeCards();

	m_bTrust = new bool[m_nMaxPlayer];
	m_nTrustNum = new int[m_nMaxPlayer];
	m_bActionOut = new bool[m_nMaxPlayer];
	m_nCatchScore = new int[m_nMaxPlayer];

	ResetLogicData();
	ResetTrustData();

	m_bIsGameZhang = false;
	m_nShuffleTimes = 10;
}

LogicData::~LogicData()
{
	safe_delete_arr(m_bTrust);
	safe_delete_arr(m_nTrustNum);
	safe_delete_arr(m_bActionOut);
	safe_delete_arr(m_nCatchScore);
}

//重设玩家牌逻辑数据
void LogicData::ResetLogicData()
{

	m_DataPC.ShuffleCards();
	
	m_DataPC.ClearAllHandCards();
	m_DataPC.ClearAllPlayerOutCards();
	m_DataPC.ClearAllFrontOutCards();
	m_DataPC.ClearAllOutCards();
	m_DataPC.ClearBaseCard();

	for(int i=0; i<m_nMaxPlayer; i++)
	{
		m_bActionOut[i] = false;
		m_nCatchScore[i] = 0;
	}
}

//重设托管数据
void LogicData::ResetTrustData()
{
	for(int i=0; i<m_nMaxPlayer; i++)
	{
		m_bTrust[i] = false;
		m_nTrustNum[i] = 0;
	}
}
