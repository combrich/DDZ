#include "GameTrusteeship.h"

GameTrusteeship::GameTrusteeship(void)
{
}

GameTrusteeship::~GameTrusteeship(void)
{
	Clear();
}

void GameTrusteeship::Clear()
{
	m_HandCard.clear();
	m_PromptCard.clear();
	m_PromptSingle.clear();						//���������ӵ�е����е���
	m_PromptDouble.clear();						//���������ӵ�е����ж���
	m_PromptThree.clear();						//������������е�����
	m_PromptBoom.clear();						//������������е�ը��
	m_PromptTWT.clear();						//������������п���ɵ�������
	m_PromptTWO.clear();						//������������п���ɵ�����һ
	m_PromptFWTO.clear();						//������������п���ɵ��Ĵ���(��Ϊ����)
	m_PromptFWDT.clear();						//������������п���ɵ��Ĵ���(��Ϊ����)
	m_PromptMTWO.clear();						//������������п���ɵ�С�ɻ�
	m_PromptMTWT.clear();						//������������п���ɵĴ�ɻ�
}

//�����ƽ������������������
bool cmp_min_max(Trused::TCARD card_1, Trused::TCARD card_2)
{
	TCards card1(card_1);
	TCards card2(card_2);
	int value1 = GameRule::CardWeight(card_1);
	int value2 = GameRule::CardWeight(card_2);
	return value1 <  value2;
}


//////////////////////////////////////////////////////////////////////////
//ְ�ܣ����������д����ϼҳ��Ƶ����飬�����ҵ�����������Ϊѡ��״̬
//������citFrontOutΪ�ϼҳ��Ƶ�������Ϣ
//���أ�����ɹ��ҵ��������������飬����true���򷵻�false
bool GameTrusteeship::GetGreaterThanNextOut(CardTypeInfo ctiFrontOut)
{
	if (ctiFrontOut.nType ==  CT_MAXBOOM)
	{
		return false;
	}
	
	//��ʱ����������Ϣ
	CardTypeInfo ctiTmp;
	ctiTmp.reset();
	//���Ƶ�����С���ϼҳ�������
	bool bRet = false;
	//�����������ӵ�е����л������ͷ���װ��
	FillBaseCards();
	if (ctiFrontOut.nType == CT_NONE)
	{
		m_PromptCard.clear();
		m_PromptCard.push_back(m_PromptSingle.front());
		return true;
	}
	switch(ctiFrontOut.nType)
	{
	case CT_SINGLE:
		bRet = HaveOutCard(m_PromptSingle.size()-1,ctiFrontOut,m_PromptSingle);
		break;

	case CT_DOUBLE:
		bRet = HaveOutCard(m_PromptDouble.size()-2,ctiFrontOut,m_PromptDouble);
		break;

	case CT_THREE:
		bRet = HaveOutCard(m_PromptThree.size()-3,ctiFrontOut,m_PromptThree);
		break;

	case CT_BOOM:
		bRet = HaveOutCard(m_PromptBoom.size()-4,ctiFrontOut,m_PromptBoom);
		break;

	case CT_THREEWITHTWO:			//������
		bRet = HaveOutCard(m_PromptTWT.size()-5,ctiFrontOut,m_PromptTWT,5);
		break;

	case CT_THREEWITHONE:			//����һ
		bRet = HaveOutCard(m_PromptTWO.size()-4,ctiFrontOut,m_PromptTWO,4);
		break;

	case CT_MULTI:				//��˳
		sort(m_PromptSingle.begin(),m_PromptSingle.end(),cmp_min_max);
		bRet = HaveOutCard(m_PromptSingle.size()-ctiFrontOut.nCount, ctiFrontOut,m_PromptSingle);
		break;

	case CT_MULTIDOUBLETWO:		//˫˳
		sort(m_PromptDouble.begin(),m_PromptDouble.end(),cmp_min_max);
		bRet = HaveOutCard(m_PromptDouble.size()-ctiFrontOut.nCount, ctiFrontOut,m_PromptDouble);
		break;

	case CT_MULTITHREE:		//��˳
		sort(m_PromptThree.begin(),m_PromptThree.end(),cmp_min_max);
		bRet = HaveOutCard(m_PromptThree.size()-ctiFrontOut.nCount, ctiFrontOut,m_PromptThree);
		break;

	case CT_MULTITHREEWITHONE:	//С�ɻ�
		bRet = HaveOutCardEx(m_PromptTWO.size()-ctiFrontOut.nCount,ctiFrontOut,m_PromptTWO);
		break;

	case CT_MULTITHREEWITHTWO:   //��ɻ�
		bRet = HaveOutCardEx(m_PromptTWT.size()-ctiFrontOut.nCount,ctiFrontOut,m_PromptTWT);
		break;

	case CT_FOURWITHTWO:		//�Ĵ���(AAAA24)
		bRet = HaveOutCard(m_PromptFWTO.size()-6,ctiFrontOut,m_PromptFWTO,6);
		break;

	case CT_FOURWITHDTWO:		//�Ĵ���(AAAA4477)
		bRet = HaveOutCard(m_PromptFWDT.size()-8,ctiFrontOut,m_PromptFWDT,8);
		break;
	default: 
		break;
	}


	//û�ж�Ӧ�����ͣ����������û��ը�� 
	if (!bRet && ctiFrontOut.nType != CT_BOOM)
	{
		ctiTmp.nType	= CT_BOOM;
		ctiTmp.nCount	= 4;
		ctiTmp.nLevel   = 4;
		bRet = HaveOutCardBoom(m_PromptBoom.size()-4, ctiTmp,m_PromptBoom);
	}
	if (!bRet && ctiFrontOut.nType != CT_MAXBOOM)
	{
		ctiTmp.nType	= CT_MAXBOOM;
		ctiTmp.nCount	= 2;
		ctiTmp.nLevel   = 1;
		bRet = HaveOutCardMaxBoom(m_PromptSingle.size()-2, ctiTmp,m_PromptSingle);
	}
	//�˴�����������������Ʒ���Ҫ��
	//�������޷���������ѭ����ʾ
	if (!bRet )
	{
		CardTypeInfo ctiTmp;
		ctiTmp.reset();
		if (!m_PromptBoom.empty())
		{
			ctiTmp.nType	= CT_BOOM;
			ctiTmp.nCount	= 4;
			ctiTmp.nLevel   = 4;
			bRet = HaveOutCardBoom(m_PromptBoom.size()-4, ctiTmp,m_PromptBoom,4);
		}
		if (!bRet)
		{
			ctiTmp.nType  = CT_MAXBOOM;
			ctiTmp.nCount = 2;
			ctiTmp.nLevel = 2;
			bRet = HaveOutCardMaxBoom(m_PromptSingle.size()-2,ctiTmp,m_PromptSingle,2);
		}
	}
	return bRet;
}

//�ж�����������Ƿ��п��Դ������
bool GameTrusteeship::HaveOutCard(int nCardsLen, CardTypeInfo ctiFrontOut ,Trused::TCARDS handCards, int nStep /*=1*/)
{
	TCardVector	 dvMaybeOut;		//���ܿ��Դ������
	CardTypeInfo ctiMaybeOut;		//dvMaybeOut������Ϣ
	m_PromptCard.clear();
	TCardVector  dvTmpHand = ConvertCardsToCardVector(handCards);


	//����ӵ����ͬ���Ϳ�ѹ�Ƶ����
	for (int i=0; i<=nCardsLen; i+=nStep)  //for1   
	{
		dvMaybeOut.clear();
		for (int j=i; j<ctiFrontOut.nCount+i; j++)  //for2
		{
 			dvMaybeOut.push_back(dvTmpHand.at(j));  
		} //end for2

		
		if (!dvMaybeOut.empty())
		{
			ctiMaybeOut = GameRule::GetCardType(dvMaybeOut);
			if (ctiMaybeOut.nLevel > ctiFrontOut.nLevel 
				&& ctiFrontOut.nType == ctiMaybeOut.nType)
			{
				m_PromptCard = ConvertCardVectorToCards(dvMaybeOut);
				return true;
			}
		}
	}  //end for 1

	return false;
}

//�ж�����������Ƿ��п��Դ������,���ж���������Ƿ���ը������
bool GameTrusteeship::HaveOutCardBoom(int nCardsLen, CardTypeInfo ctiFrontOut ,Trused::TCARDS handCards, int nStep /*=1*/)
{
	TCardVector	 dvMaybeOut;		//���ܿ��Դ������
	CardTypeInfo ctiMaybeOut;		//dvMaybeOut������Ϣ
	m_PromptCard.clear();
	TCardVector  dvTmpHand = ConvertCardsToCardVector(handCards);


	//����ӵ����ͬ���Ϳ�ѹ�Ƶ����
	for (int i=0; i<=nCardsLen; i+=nStep)  //for1   
	{
		dvMaybeOut.clear();
		for (int j=i; j<ctiFrontOut.nCount+i; j++)  //for2
		{
			dvMaybeOut.push_back(dvTmpHand.at(j));  
		} //end for2


		if (!dvMaybeOut.empty())
		{
			ctiMaybeOut = GameRule::GetCardType(dvMaybeOut);
			if (ctiMaybeOut.nLevel > ctiFrontOut.nLevel 
				&& ctiFrontOut.nType == ctiMaybeOut.nType)
			{
 				m_PromptCard = ConvertCardVectorToCards(dvMaybeOut);
				return true;
			}

		}
	}  //end for 1

	return false;
}

//�ж�����������Ƿ��п��Դ������,�ɻ�����
bool GameTrusteeship::HaveOutCardMaxBoom(int nCardsLen, CardTypeInfo ctiFrontOut ,Trused::TCARDS handCards, int nStep /*=1*/)
{
	TCardVector	 dvMaybeOut;		//���ܿ��Դ������
	CardTypeInfo ctiMaybeOut;		//dvMaybeOut������Ϣ
	m_PromptCard.clear();
	TCardVector  dvTmpHand = ConvertCardsToCardVector(handCards);


	//����ӵ����ͬ���Ϳ�ѹ�Ƶ����
	for (int i=0; i<=nCardsLen; i+=nStep)  //for1   
	{
		dvMaybeOut.clear();
		for (int j=i; j<ctiFrontOut.nCount+i; j++)  //for2
		{
			dvMaybeOut.push_back(dvTmpHand.at(j));  
		} //end for2


		if (!dvMaybeOut.empty())
		{
			ctiMaybeOut = GameRule::GetCardType(dvMaybeOut);
			if (ctiMaybeOut.nLevel > ctiFrontOut.nLevel 
				&& ctiFrontOut.nType == ctiMaybeOut.nType)
			{
 				m_PromptCard = ConvertCardVectorToCards(dvMaybeOut);
				return true;
			}
		}
	}  //end for 1

	return false;
}

//�����ж���������Ƿ��п��Դ���ķɻ�����
bool GameTrusteeship::HaveOutCardEx(int nCardsLen,CardTypeInfo ctiFrontOut,Trused::TCARDS handCards)
{
	//���ɻ�ת��Ϊ��˳
	CardTypeInfo ctiTmp = ctiFrontOut;
	ctiTmp.nType  = CT_MULTITHREE;
	if (ctiFrontOut.nType == CT_MULTITHREEWITHONE)
	{
		ctiTmp.nCount = ctiFrontOut.nCount - ctiFrontOut.nCount/4; 
		ctiTmp.nType  = CT_MULTITHREE;
	}else 
	{
		ctiTmp.nCount = ctiFrontOut.nCount - ctiFrontOut.nCount/5*2;
		ctiTmp.nType  = CT_MULTITHREE;
	}

	sort(m_PromptThree.begin(),m_PromptThree.end(),cmp_min_max);
	bool bRet = HaveOutCard(m_PromptThree.size()-ctiTmp.nCount, ctiTmp,m_PromptThree,3);

	//���ܹ����С�ɻ�����˳���������޳�
	Trused::TCARDS tmpHandCards = m_HandCard;
	for (size_t i=0; i<m_PromptCard.size();i++)
	{
		tmpHandCards.erase(remove(tmpHandCards.begin(),tmpHandCards.end(),m_PromptCard.at(i)),
			tmpHandCards.end());
	}

	if (bRet && ctiFrontOut.nType == CT_MULTITHREEWITHONE)
	{
		if (bRet && tmpHandCards.size() < ctiTmp.nCount/3)  //û���㹻�����ܹ�����˳��ɷɻ�
		{
			m_PromptCard.clear();
			return false;
		}

		//�뵥����ɷɻ�
		Trused::TCARDS tmpSigCard = GetBaseCardsEx(1,CT_SINGLE,tmpHandCards,m_PromptCard);

		//����������˳���С�ɻ�
		static int g_iSig = 0;  
		if (g_iSig >= tmpSigCard.size())
		{
			g_iSig =0;
		}
		for (int i=0; i<ctiTmp.nCount/3; i++)
		{
			m_PromptCard.push_back(tmpSigCard.at(i));
		}
		g_iSig++;
		return true;

	}else if (bRet && ctiFrontOut.nType == CT_MULTITHREEWITHTWO)
	{

		if (bRet && tmpHandCards.size() < ctiTmp.nCount/3*2)  //û���㹻�����ܹ�����˳��ɷɻ�
		{
			m_PromptCard.clear();
			return false;
		}

		//����������˳��ɴ�ɻ�
		Trused::TCARDS tmpDouCards = GetBaseCards(2,CT_DOUBLE,tmpHandCards);
		static int g_iDouble = 0;  
		if (g_iDouble >= tmpDouCards.size())
		{
			g_iDouble =0;
		}

		for (int i=0; i<ctiTmp.nCount/3*2; i+=2)
		{
			m_PromptCard.push_back(tmpDouCards.at(i));
			m_PromptCard.push_back(tmpDouCards.at(i+1));
		}
		g_iDouble+=2;
		return true;
	}
	return false;
}

//��ȡ���������ӵ�еĻ������ͣ���������Ϊ���š����ӡ����š�ը��
//������ȡ���Ļ������ͷ���ָ����������
void GameTrusteeship::FillBaseCards()
{
	//Ԥ������
	m_PromptBoom.clear();
	m_PromptSingle.clear();
	m_PromptDouble.clear();
	m_PromptThree.clear();
	m_PromptTWT.clear();
	m_PromptTWO.clear();
	m_PromptFWTO.clear();
	m_PromptFWDT.clear();
	//��ȡ���������еĻ�������
	Trused::TCARDS tcHands = m_HandCard;
	sort(tcHands.begin(),tcHands.end(),cmp_min_max);
	//ը��
	m_PromptBoom	= GetBaseCards(4,CT_BOOM, tcHands);
	//�޳������е�ը��

	for (int i=0; i<m_PromptBoom.size(); i++)
	{
		tcHands.erase(remove(tcHands.begin(),tcHands.end(),m_PromptBoom.at(i)),tcHands.end());
	}


	//����
	m_PromptThree	= GetBaseCards(3,CT_THREE,tcHands);
	//�޳������е����ź�ը��
	for (int i=0; i<m_PromptThree.size(); i++)
	{
		tcHands.erase(remove(tcHands.begin(),tcHands.end(),m_PromptThree.at(i)),tcHands.end());
	}

	//����
	m_PromptDouble  = GetBaseCards(2,CT_DOUBLE,tcHands);
	//�޳������е�ը�������ţ�һ��
	for (int i=0; i<m_PromptDouble.size(); i++)
	{
		tcHands.erase(remove(tcHands.begin(),tcHands.end(),m_PromptDouble.at(i)),tcHands.end());
	}


	//��ȥը�������ţ�һ�Ժ�ľ��ǵ�����
	m_PromptSingle = tcHands;


	sort(m_PromptSingle.begin(),m_PromptSingle.end(),cmp_min_max);
	sort(m_PromptDouble.begin(),m_PromptDouble.end(),cmp_min_max);
	sort(m_PromptThree.begin(),m_PromptThree.end(),cmp_min_max);

	//��ȡ�����л������͹��󣬵��ƿ��ɶ��ӡ����š�ը�����
	//���ӿ���ը�������Ų��ɣ�����Ӧ���ڽ�β�����Ӧ�𿪵���
	//���Ƶ�ʱ����Ҫע�⣺ը�������ſɲ�2����ͬ��
	Trused::TCARDS tmpThree;
	Trused::TCARDS tmpDouble;
	tmpDouble.clear();
	tmpThree.clear();
	//����
	for (int i =0; i<m_PromptBoom.size(); i+=4)
	{
		tmpThree.push_back(m_PromptBoom.at(i));
		tmpThree.push_back(m_PromptBoom.at(i+1));
		tmpThree.push_back(m_PromptBoom.at(i+2));
	}

	//һ��
	for (int i=0; i<m_PromptThree.size(); i+=3)
	{
		tmpDouble.push_back(m_PromptThree.at(i));
		tmpDouble.push_back(m_PromptThree.at(i+1));
	}
	for (int i=0 ;i<m_PromptBoom.size();i+=4)
	{
		tmpDouble.push_back(m_PromptBoom.at(i));
		tmpDouble.push_back(m_PromptBoom.at(i+1));
	}
	//����
	for (int i=0; i<m_PromptDouble.size(); i+=2)
	{
		m_PromptSingle.push_back(m_PromptDouble.at(i));
	}
	for (int i=0; i<m_PromptThree.size();i+=3)
	{
		m_PromptSingle.push_back(m_PromptThree.at(i));
	}
	for (int i=0; i<m_PromptBoom.size();i+=4)
	{
		m_PromptSingle.push_back(m_PromptBoom.at(i));
	}

	//׷������
	for (int i=0; i<tmpThree.size();i++)
	{
		m_PromptThree.push_back(tmpThree.at(i));
	}
	//׷��һ��
	for (int i=0; i<tmpDouble.size();i++)
	{
		m_PromptDouble.push_back(tmpDouble.at(i));
	}
	//����������Ҫ�������ͻ�ȡ��Ϻ�ſɻ�ȡ
	//�����������п��ܵ���������Ϸ���tmp������
	for (int i=0; i<m_PromptThree.size(); i+=3)
	{
		for (int j=0; j<m_PromptDouble.size(); j+=2)
		{	
			if (m_PromptDouble.at(j) == m_PromptThree.at(i))
			{
				continue;
			}
			//����
			m_PromptTWT.push_back(m_PromptThree.at(i));
			m_PromptTWT.push_back(m_PromptThree.at(i+1));
			m_PromptTWT.push_back(m_PromptThree.at(i+2));
			//һ��
			m_PromptTWT.push_back(m_PromptDouble.at(j));
			m_PromptTWT.push_back(m_PromptDouble.at(j+1));
		}
	}

	//����һ����Ҫ�������ͻ�ȡ��Ϻ�ſɻ�ȡ
	//�����������п��ܵ�����һ��Ϸ���������
	for (int i=0; i<m_PromptThree.size(); i+=3)
	{
		for (int j=0; j<m_PromptSingle.size(); j++)
		{	
			if (m_PromptSingle.at(j) == m_PromptThree.at(i))
			{
				continue;
			}
			//����
			m_PromptTWO.push_back(m_PromptThree.at(i));
			m_PromptTWO.push_back(m_PromptThree.at(i+1));
			m_PromptTWO.push_back(m_PromptThree.at(i+2));
			//һ��
			m_PromptTWO.push_back(m_PromptSingle.at(j));
		}
	}

	//�Ĵ�������Ҫ�������ͻ�ȡ��Ϻ�ſɻ�ȡ
	//�����������п��ܵ��Ĵ�����Ϸ�������������
	for (int i=0; i<m_PromptBoom.size(); i+=4)   //�Ĵ��������ŵ���
	{
		for (int j=0; j<m_PromptSingle.size()-1; j++)
		{	
			if (m_PromptSingle.at(j) == m_PromptBoom.at(i)
				||m_PromptSingle.at(j+1) == m_PromptBoom.at(i))
			{
				continue;
			}
			//����
			m_PromptFWTO.push_back(m_PromptBoom.at(i));
			m_PromptFWTO.push_back(m_PromptBoom.at(i+1));
			m_PromptFWTO.push_back(m_PromptBoom.at(i+2));
			m_PromptFWTO.push_back(m_PromptBoom.at(i+3));
			//һ��
			m_PromptFWTO.push_back(m_PromptSingle.at(j));
			m_PromptFWTO.push_back(m_PromptSingle.at(j+1));

		}
	}

	//�Ĵ�������Ҫ�������ͻ�ȡ��Ϻ�ſɻ�ȡ
	//�����������п��ܵ��Ĵ�����Ϸ�������������
	for (int i=0; i<m_PromptBoom.size(); i+=4) //�Ĵ���������
	{
		for (int j=0; j<m_PromptDouble.size()-4; j+=2)
		{	
			if (m_PromptDouble.size() < 4)
			{
				break;
			}
			if (m_PromptDouble.at(j) == m_PromptBoom.at(i) 
				|| m_PromptDouble.at(j+2)==m_PromptBoom.at(i))
			{
				continue;
			}
			//����
			m_PromptFWDT.push_back(m_PromptBoom.at(i));
			m_PromptFWDT.push_back(m_PromptBoom.at(i+1));
			m_PromptFWDT.push_back(m_PromptBoom.at(i+2));
			m_PromptFWDT.push_back(m_PromptBoom.at(i+3));
			//һ��
			m_PromptFWDT.push_back(m_PromptDouble.at(j));
			m_PromptFWDT.push_back(m_PromptDouble.at(j+1));
			m_PromptFWDT.push_back(m_PromptDouble.at(j+2));
			m_PromptFWDT.push_back(m_PromptDouble.at(j+3));
		}
	}
}

//��ȡ��������
Trused::TCARDS GameTrusteeship::GetBaseCards(int nCardsLen, int nType, Trused::TCARDS handCards)
{
	Trused::TCARDS tmpCards;
	tmpCards.clear();

	if (nCardsLen > handCards.size() || handCards.size() <= 0 )
	{
		return tmpCards;
	}
	TCardVector dvMayBeOut;
	CardTypeInfo ctiMayBeOut;		//���ڻ�ȡ������Ϣ
	CardTypeInfo ctiFrontMBO;		//ǰ��һ��������Ϣ
	ctiFrontMBO.reset();
	ctiMayBeOut.reset();
	TCardVector  dvTmpHand = ConvertCardsToCardVector(handCards);

	//��ȡ������ӵ�еĻ�����
	for (int i=0; i<=dvTmpHand.size()- nCardsLen; i++)  //for1   
	{
		dvMayBeOut.clear();
		dvMayBeOut.insert(dvMayBeOut.end(),dvTmpHand.begin()+i,dvTmpHand.begin()+i+nCardsLen);
		
		if (!dvMayBeOut.empty())
		{
			ctiMayBeOut = GameRule::GetCardType(dvMayBeOut);
			if (ctiMayBeOut.nType == nType  )
			{
				//�޳���Ϊ�������ը������ȡһ�Ե�ʱ�����ֻ�ȡ��3�Ե��������
				if (ctiMayBeOut.nLevel == ctiFrontMBO.nLevel)
				{
					ctiFrontMBO = ctiMayBeOut;
					continue;
				}
				for (int n =i; n<i+nCardsLen; n++)
				{
					tmpCards.push_back(dvTmpHand.at(n).GetID());
				}
			}
			ctiFrontMBO = ctiMayBeOut;
		}
	}  //end for 1

	return tmpCards;
}

//��ȡָ�������е�ĳһ�ֻ����ƣ����ų���needReject��ֵ��ͬ����
Trused::TCARDS GameTrusteeship::GetBaseCardsEx(int nCardsLen, int nType, Trused::TCARDS handCards,Trused::TCARDS needReject)
{
	Trused::TCARDS tmpCards;
	tmpCards.clear();

	if (nCardsLen > handCards.size())
	{
		return tmpCards;
	}
	TCardVector dvMayBeOut;
	CardTypeInfo ctiMayBeOut;		//���ڻ�ȡ������Ϣ
	CardTypeInfo ctiFrontMBO;		//ǰ��һ��������Ϣ
	ctiFrontMBO.reset();
	ctiMayBeOut.reset();
	TCardVector  dvTmpHand = ConvertCardsToCardVector(handCards);
	TCardVector  dvTmpNR   = ConvertCardsToCardVector(needReject);

	//�ų�����Ҫ��ȡ����
	for(int i=0;i<dvTmpNR.size();i++)
	{
		dvTmpHand.erase(remove_if(dvTmpHand.begin(),dvTmpHand.end(),IsEqualValue(dvTmpNR[i])),dvTmpHand.end());
	}

	//��ȡ������ӵ�еĻ�����
	for (int i=0; i<=dvTmpHand.size()- nCardsLen; i++)  //for1   
	{
		dvMayBeOut.clear();
		for (int j=i; j<nCardsLen+i; j++)  //for2
		{
			dvMayBeOut.push_back(dvTmpHand.at(j));  
		} //end for2

		if (!dvMayBeOut.empty())
		{
			ctiMayBeOut = GameRule::GetCardType(dvMayBeOut);
			if (ctiMayBeOut.nType == nType  )
			{
				//�޳���Ϊ�������ը������ȡһ�Ե�ʱ�����ֻ�ȡ��3�Ե��������
				if (ctiMayBeOut.nLevel == ctiFrontMBO.nLevel)
				{
					ctiFrontMBO = ctiMayBeOut;
					continue;
				}
				for (int n =i; n<i+nCardsLen; n++)
				{
					tmpCards.push_back(dvTmpHand.at(n).GetID());
				}
			}
			ctiFrontMBO = ctiMayBeOut;
		}
	}  //end for 1
	return tmpCards;
}

