#pragma once
#include "CommonLibrary.h"
#include "Define.h"

//牌花色
typedef enum tagCardColor
{
	PC_NULL = 0,
	PC_DIAMOND,			// 方块
	PC_CLUB,			// 梅花
	PC_HEART,			// 红桃
	PC_SPADE,			// 黑桃

	PC_COUNT,

}TCARDCOLOR;

// 牌数值
typedef enum tagCardValue
{
	CV_NULL = 0,	//牌面
	CV_2 = 2,
	CV_3,
	CV_4,
	CV_5,
	CV_6,
	CV_7,
	CV_8,
	CV_9,
	CV_10,
	CV_J,
	CV_Q,
	CV_K,
	CV_A,
	CV_SJ,	// 小王
	CV_BJ,	// 大王
	CV_COUNT,
}TCARDVALUE;

// 牌的ID，用来标示每张牌的索引
typedef enum tagCardID
{
	CID_NONE = 0,
	// 方块
	CID_DIAMOND_A,
	CID_DIAMOND_2,
	CID_DIAMOND_3,
	CID_DIAMOND_4,
	CID_DIAMOND_5,
	CID_DIAMOND_6,
	CID_DIAMOND_7,
	CID_DIAMOND_8,
	CID_DIAMOND_9,
	CID_DIAMOND_10,
	CID_DIAMOND_J,
	CID_DIAMOND_Q,
	CID_DIAMOND_K,
	// 梅花
	CID_CLUB_A,
	CID_CLUB_2,
	CID_CLUB_3,
	CID_CLUB_4,
	CID_CLUB_5,
	CID_CLUB_6,
	CID_CLUB_7,
	CID_CLUB_8,
	CID_CLUB_9,
	CID_CLUB_10,
	CID_CLUB_J,
	CID_CLUB_Q,
	CID_CLUB_K,
	// 红心
	CID_HEART_A,
	CID_HEART_2,
	CID_HEART_3,
	CID_HEART_4,
	CID_HEART_5,
	CID_HEART_6,
	CID_HEART_7,
	CID_HEART_8,
	CID_HEART_9,
	CID_HEART_10,
	CID_HEART_J,
	CID_HEART_Q,
	CID_HEART_K,
	// 黑桃
	CID_SPADE_A,
	CID_SPADE_2,
	CID_SPADE_3,
	CID_SPADE_4,
	CID_SPADE_5,
	CID_SPADE_6,
	CID_SPADE_7,
	CID_SPADE_8,
	CID_SPADE_9,
	CID_SPADE_10,
	CID_SPADE_J,
	CID_SPADE_Q,
	CID_SPADE_K,
	// 大小鬼
	CID_SJ,
	CID_BJ,
	// 牌背
	CID_BACK,

	CID_COUNT,

}TCARDID;

static const string strCardValue[] = {"牌背", "", "2","3","4","5","6","7","8","9","10","J","Q","K","A","小王","大王"};
static const string strCardColor[] = { "" , "方块", "梅花", "红心", "黑桃" };

//////////////////////////////////////////////////////////////////////////
//Class Name: 扑克牌
//
//////////////////////////////////////////////////////////////////////////
class TCards
{
public:
	TCards(BYTE id,string name,TCARDVALUE value,TCARDCOLOR color):
	  m_nID(id),m_strName(name),m_nValue(value),m_nColor(color){}
	TCards(BYTE nID = CID_BACK);
	TCards(const TCards& objCard);
	virtual ~TCards(void);
public:
	TCards& operator = (const TCards& objCard);
	bool operator == (const TCards& objCard) const { return objCard.m_nID == this->m_nID; }
	bool operator != (const TCards& objCard) const { return !(*this == objCard); }

	void SetID(BYTE nID);
	
	BYTE		GetID() const { return m_nID; }
	string		GetName() const { return m_strName; }
	TCARDVALUE  GetValue() const { return m_nValue; }
	TCARDCOLOR  GetColor() const { return m_nColor; }

private:
	BYTE		m_nID;			// 牌ID
	string		m_strName;      // 牌名
	TCARDVALUE  m_nValue;		// 牌值
	TCARDCOLOR  m_nColor;		// 花色
};

//////////////////////////////////////////////////////////////////////////
// 玩家手牌
//
//////////////////////////////////////////////////////////////////////////
typedef DataVector<TCards> TCardVector;
class PlayerCard
{
public:
	PlayerCard(int nPlayer);
	virtual ~PlayerCard(void);
public:
	// 洗牌方式
	typedef enum tagShuffleMode
	{
		SM_COMMON,  // 普通
		SM_STL,		// 调用STL算法
	}ShuffleMode;

	// 初始化整副牌
	virtual void MakeCards();

	// 根据洗牌方式洗牌
	void ShuffleCards(ShuffleMode sm = SM_STL, int nShuffle = 1000);
	
	// 初始化手牌
	virtual void InitPlayerCard();

public:
	void SetHandCards(int nSeat, const TCardVector& handCards) { m_pHandCard[nSeat] = handCards; }
	void AppendOneHandCard(int nSeat, const TCards& handCard) { m_pHandCard[nSeat] += handCard; }
	void AppendOneBaseCard(const TCards& baseCard) {m_dvBaseCard+=baseCard;}
	void AppendHandCards(int nSeat, const TCardVector& handCards) { m_pHandCard[nSeat] += handCards; }
	void DeleteHandCards(int nSeat, const TCardVector& handCards) { m_pHandCard[nSeat] -= handCards; }
	void ClearHandCards(int nSeat) { m_pHandCard[nSeat].clear(); }
	void ClearAllHandCards() { for(int i=0; i<m_nPlayer; i++) m_pHandCard[i].clear(); }
	
	void SetOutCards(int nSeat, const TCardVector& outCards) { m_pOutCard[nSeat] = outCards; }
	void ClearOutCards(int nSeat) { m_pOutCard[nSeat].clear(); }
	void ClearAllPlayerOutCards() {for(int i=0; i<m_nPlayer; i++) m_pOutCard[i].clear(); }
	
	void DelBaseCard(const TCardVector& baseCard) { m_dvBaseCard -= baseCard; }
	void ClearBaseCard() { m_dvBaseCard.clear(); }
	const TCardVector& GetBaseCards() const { return m_dvBaseCard; }
	void SetBaseCards( BYTE baseCards[],int nLen)
	{
		for (int i=0; i<nLen ;i++)
		{
			m_dvBaseCard.at(i).SetID(baseCards[i]);
		}
	}

	const TCardVector& GetHandCards(int nSeat) const { return m_pHandCard[nSeat]; }
	const TCardVector& GetOutCards(int nSeat)  const { return m_pOutCard[nSeat]; }
	//获取底牌的牌型-
	//这是一个专有函数，在斗地主中使用
	int GetBaseCardsType();
private:
	const int		m_nPlayer;		// 玩家个数
	TCardVector		m_dvAllCard;	// 所有牌
	TCardVector		m_dvBaseCard;	// 底牌

	TCardVector*	m_pHandCard;	// 玩家手牌
	TCardVector*	m_pOutCard;	    // 玩家出牌
	

// 上一轮出牌
public:
	void SetFrontOutCards(int nSeat, const TCardVector& frontOutCards) { m_pFrontOutCard[nSeat] = frontOutCards; }
	void ClearFrontOutCards(int nSeat) { m_pFrontOutCard[nSeat].clear(); }
	void ClearAllFrontOutCards() { for(int i=0; i<m_nPlayer; i++) m_pFrontOutCard[i].clear(); }
	const TCardVector& GetFrontOutCards(int nSeat)  const { return m_pFrontOutCard[nSeat]; }
private:
	TCardVector*	m_pFrontOutCard;

// 所有玩家出牌
public:
	void AppendOutCards(const TCardVector& dvOutCards) { m_dvAllOutCards += dvOutCards; }
	const TCardVector& GetAllOutCards() { return m_dvAllOutCards; }	
	void ClearAllOutCards() { m_dvAllOutCards.clear(); }
private:
	TCardVector		m_dvAllOutCards;

};
