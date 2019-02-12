#pragma once
#include "Card.h"
#include "GameRule.h"

class LogicData
{
public:
	LogicData(int nMaxPlayer);
	virtual ~LogicData();
protected:
	const int m_nMaxPlayer;			// 最大玩家数
private:
	bool IsIllegal(int nSeat) const { return (nSeat < 0 || nSeat >= m_nMaxPlayer); }

public:
	void ResetLogicData();

//玩家手牌
public:
	PlayerCard*	GetDataPC() { return &m_DataPC; }
private:
	PlayerCard  m_DataPC;			// 玩家牌数据

	//玩家托管标志
public:
	void SetTrust(int nSeat, bool bTrust) { if(IsIllegal(nSeat)) return; m_bTrust[nSeat] = bTrust; }
	void SetTrustNum(int nSeat, int nTrustNum) { if(IsIllegal(nSeat)) return; m_nTrustNum[nSeat] = nTrustNum; }
	bool IsTrust(int nSeat) const { if(IsIllegal(nSeat)) return false; return m_bTrust[nSeat]; }
	int  GetTrustNum(int nSeat) const { if(IsIllegal(nSeat)) return 0; return m_nTrustNum[nSeat]; }
	void ResetTrustData();
private:
	bool* m_bTrust;
	int*  m_nTrustNum;

	// 行牌标志
public:
	void SetActionOut(int nSeat, bool bActionOut) { if(IsIllegal(nSeat)) return; m_bActionOut[nSeat] = bActionOut; } 
	bool IsActionOut(int nSeat) const { if(IsIllegal(nSeat)) return 0; return m_bActionOut[nSeat]; }
private:
	bool* m_bActionOut;

	// 抓分数据
public:
	void AddCatchScore(int nSeat, int nCatchScore) { if(IsIllegal(nSeat)) return; m_nCatchScore[nSeat] += nCatchScore; }
	int  GetCatchScore(int nSeat) const { if(IsIllegal(nSeat)) return 0; return m_nCatchScore[nSeat]; }
private:
	int*  m_nCatchScore;

	//待定
public:
	void SetIsGameZhang(bool bGameZhang) { m_bIsGameZhang = bGameZhang; }
	void SetShuffleTimes(int nShuffleTimes) { m_nShuffleTimes = nShuffleTimes; }
private:
	bool m_bIsGameZhang;
	int  m_nShuffleTimes;
};

