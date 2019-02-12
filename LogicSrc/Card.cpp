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
	else /* �Ʊ� */
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

//����
void PlayerCard::MakeCards()
{
	int NCARDNUM = 1; // һ����
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
	// ��ʼ���������
}

//����ϴ�Ʒ�ʽϴ��
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
//ְ�ܣ��ַ�����
//˵�������Ʒַ���ʽΪÿ��17�ţ���54����,��������Ϊ����
//		�����߼�---
//					���Ȼ�ȡ���������е�������
//					Ȼ���ȡ��ǰ��Ϸ��������ƽ
//					������ǰ51���ƣ����������
//					������Ϊ����
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
// ְ�ܣ���ȡ���Ƶ�����
// ���أ����ص��Ƶ�����
// ��ע��������һ��ר�к�����ֻ�ڶ�����ʹ�ã�
//	     �����У�
//			ͬ����˳�ӡ����ӡ��׷֡�3
//			ͬ��˳�׷֡�5
//			���׷֡�2
// �����㷨--��С����
// bool sort_min_max(TCards card1, TCards card2)
// {
// 	if (card1.GetValue()<card2.GetValue()) return true;
// 	return false;
// }

int PlayerCard::GetBaseCardsType()
{
// 	sort(m_dvBaseCard.begin(),m_dvBaseCard.end(),sort_min_max);
// 	int m = m_dvBaseCard.back().GetID();
// 	//�д���
// 	if (CID_BJ == m_dvBaseCard.back().GetID())
// 	{
// 		return BC_HAVEBIGJOKEER;
// 	}
// 	//ͬ��
// 	if (m_dvBaseCard.front().GetColor() == m_dvBaseCard.at(1).GetColor() && 
// 		m_dvBaseCard.front().GetColor() == m_dvBaseCard.back().GetColor())
// 	{
// 		//ͬ��˳
// 		if (m_dvBaseCard.front().GetValue()+2 == m_dvBaseCard.back().GetValue()&&
// 			m_dvBaseCard.at(1).GetValue()+1 == m_dvBaseCard.back().GetValue())
// 		{
// 			return BC_STRAIGHTFLUSH;  
// 		}else 
// 		{
// 			return BC_FLUSH;
// 		}
// 	}
// 	//˳��
// 	if (m_dvBaseCard.front().GetValue()+2 == m_dvBaseCard.back().GetValue()&&
// 		m_dvBaseCard.at(1).GetValue()+1 == m_dvBaseCard.back().GetValue())
// 	{
// 		return BC_STRAIGHT;  
// 	}
// 	//����
// 	if (m_dvBaseCard.front().GetValue() == m_dvBaseCard.back().GetValue())
// 	{
// 		return BC_BAOZI;
// 	}
	return BC_NOCARDTYPE;
}
