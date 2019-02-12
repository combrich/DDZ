#include "./Card.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
TCards::TCards(BYTE nID/* = CID_BACK*/)
{
	SetID(nID);
}

TCards::TCards( const TCards& objCard )
{
	if(this != &objCard)
	{
		SetID(objCard.m_nID);
	}
}

TCards::~TCards(void)
{
}

TCards& TCards::operator=( const TCards& objCard )
{
	if(&objCard != this)
	{
		SetID(objCard.m_nID);
	}
	return *this;
}

void TCards::SetID( BYTE nID )
{
	m_nID = nID;

	if(CID_SJ == nID || CID_BJ == nID)
	{
		if(CID_SJ == nID)
		{
			m_nValue = CV_SJ;
		}
		else 
		{
			m_nValue = CV_BJ;
		}
		m_nColor = PC_NULL;
		m_strName = strCardValue[m_nValue];
	}
	else if(CID_DIAMOND_A <= nID && CID_SPADE_K >= nID)
	{
		int nMod = (nID - 1) % 13;
		m_nValue = TCARDVALUE(nMod == 0 ? CV_A : nMod + 1);
		m_nColor = TCARDCOLOR((nID - 1) / 13 + PC_DIAMOND);
		m_strName = strCardColor[m_nColor] + strCardValue[m_nValue];
	}
	else /* 牌背 */
	{
		m_nValue = CV_NULL;
		m_nColor = PC_NULL;
		m_strName = strCardValue[m_nValue];
	}
	//if (m_nValue == CV_2) m_nValue = CV_M;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
PlayerCard::PlayerCard(int nPlayer) : m_nPlayer(nPlayer)
{
	m_pHandCard = new TCardVector[m_nPlayer];
	m_pOutCard = new TCardVector[m_nPlayer];
	m_pFrontOutCard = new TCardVector[m_nPlayer];
}

PlayerCard::~PlayerCard(void)
{
	safe_delete_arr(m_pHandCard);
	safe_delete_arr(m_pOutCard);
	safe_delete_arr(m_pFrontOutCard);
}

//做牌
void PlayerCard::MakeCards()
{
	int NCARDNUM = 1; // 一副牌
	TCards card;
	m_dvAllCard.clear();
	int len=0;
	for(int i=0; i<NCARDNUM; i++)
	{
		for(int nCID=CID_DIAMOND_A; nCID<=CID_BJ; nCID++)
		{
			card.SetID(nCID);
			m_dvAllCard += card;
			len ++;
		}
	}
	len=(int)m_dvAllCard.size();
	// 初始化玩家手牌
}

//根据洗牌方式洗牌
void PlayerCard::ShuffleCards(ShuffleMode sm/* = SM_STL*/, int nShuffle/* = 1000*/)
{
	/*if(SM_STL == sm)
	{
		nShuffle = CRandom::Random_Int(2, 5);
		for(int i=0; i<nShuffle; i++)
		{
			std::random_shuffle(m_dvAllCard.begin(), m_dvAllCard.end());
		}
	}
	else /*if(SM_COMMON == sm)
	{
		int nIndex1, nIndex2;
		TCards tmpCard;
		int nTotal = (int)m_dvAllCard.size();
		for(int i=0; i<nShuffle; i++)
		{
			nIndex1 = CRandom::Random_Int(0, nTotal-1);
			nIndex2 = CRandom::Random_Int(0, nTotal-1);
			tmpCard = m_dvAllCard[nIndex1];
			m_dvAllCard[nIndex1] = m_dvAllCard[nIndex2];
			m_dvAllCard[nIndex2] = tmpCard;
		}
	}*/

	int nIndex1;
	TCards tmpCard;
	int nTotal = (int)m_dvAllCard.size();

	while(--nTotal)
	{
		nIndex1 = rand()%nTotal;
		tmpCard = m_dvAllCard[nTotal];
		m_dvAllCard[nTotal] = m_dvAllCard[nIndex1];
		m_dvAllCard[nIndex1] = tmpCard;
	}
}

//////////////////////////////////////////////
//职能：分发手牌
//说明：手牌分发方式为每人17张，总54张牌,留三张作为底牌
//		发牌逻辑---
//					首先获取牌组中所有的牌数量
//					然后获取当前游戏的人数，平
//					均分配前51张牌，将最后三张
//					牌设置为底牌
void PlayerCard::InitPlayerCard()
{
	int nTotal =(int)m_dvAllCard.size();
	int ndivide = (nTotal-BASE_CARDS) / m_nPlayer;
	int nIndex = 0;
	for(int i=0; i<m_nPlayer; i++)
	{
		for(int j=0; j<ndivide; j++)
		{
			AppendOneHandCard(i, m_dvAllCard[nIndex++]);
		}
	}
	TCardVector::iterator it = m_dvAllCard.begin();
	m_dvBaseCard.insert(m_dvBaseCard.end(),it+nIndex,m_dvAllCard.end());
}

//////////////////////////////////////////////////////////////////////////
// 职能：获取底牌的牌型
// 返回：返回底牌的牌型
// 附注：函数是一个专有函数，只在斗地主使用，
//	     牌型有：
//			同花、顺子、豹子、底分×3
//			同花顺底分×5
//			王底分×2
// 排序算法--由小到大
// bool sort_min_max(TCards card1, TCards card2)
// {
// 	if (card1.GetValue()<card2.GetValue()) return true;
// 	return false;
// }

int PlayerCard::GetBaseCardsType()
{
// 	sort(m_dvBaseCard.begin(),m_dvBaseCard.end(),sort_min_max);
// 	int m = m_dvBaseCard.back().GetID();
// 	//有大王
// 	if (CID_BJ == m_dvBaseCard.back().GetID())
// 	{
// 		return BC_HAVEBIGJOKEER;
// 	}
// 	//同花
// 	if (m_dvBaseCard.front().GetColor() == m_dvBaseCard.at(1).GetColor() && 
// 		m_dvBaseCard.front().GetColor() == m_dvBaseCard.back().GetColor())
// 	{
// 		//同花顺
// 		if (m_dvBaseCard.front().GetValue()+2 == m_dvBaseCard.back().GetValue()&&
// 			m_dvBaseCard.at(1).GetValue()+1 == m_dvBaseCard.back().GetValue())
// 		{
// 			return BC_STRAIGHTFLUSH;  
// 		}else 
// 		{
// 			return BC_FLUSH;
// 		}
// 	}
// 	//顺子
// 	if (m_dvBaseCard.front().GetValue()+2 == m_dvBaseCard.back().GetValue()&&
// 		m_dvBaseCard.at(1).GetValue()+1 == m_dvBaseCard.back().GetValue())
// 	{
// 		return BC_STRAIGHT;  
// 	}
// 	//豹子
// 	if (m_dvBaseCard.front().GetValue() == m_dvBaseCard.back().GetValue())
// 	{
// 		return BC_BAOZI;
// 	}
	return BC_NOCARDTYPE;
}
