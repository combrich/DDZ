/*
������������������������������ 
�������������������ߩ��������ߩ� 
�������������������������������� �� 
�������������������������������� 
�������������������ש������ס��� 
�������������������������������� 
�����������������������ߡ������� 
�������������������������������� 
�������������������������������� 
�������������������������������ޱ��ӣ�������bug������������������ 
����������������������������code is far away from bug with the animal protecting 
������������������������������������ 
�����������������������������������ǩ� 
�������������������������������������� 
�����������������������������ש����� 
�����������������������ϩϡ����ϩ� 
�����������������������ߩ������ߩ� 
*/
#include "Robot.h"

DebugOut cDebugOut;

////////////////////////////////////////////////////////////////////////
//��CardsVector�������������
bool cmpCV(TCardVector T1,TCardVector T2)
{
	int nT1=0,nT2=0;
	for (int i=0; i<T1.size();i++)
	{
		nT1 = GameRule::CardWeight(T1.at(i));
	}
	for (int i=0; i<T2.size();i++)
	{
		nT2 = GameRule::CardWeight(T2.at(i));
	}
	return nT1<nT2?true:false;
}
//��TCardVector�������������
bool cmpTV(TCards T1,TCards T2)
{
	int nT1 = GameRule::CardWeight(T1);
	int nT2 = GameRule::CardWeight(T2);
	return nT1<nT2?true:false;
}

//��TCardVector��������������ɴ�С
bool cmpMaxMin(TCards T1,TCards T2)
{
	int nT1 = GameRule::CardWeight(T1);
	int nT2 = GameRule::CardWeight(T2);
	return nT1>nT2?true:false;
}
//�ṹ�庯��
void CARDSTYLE::Fill(TCardVector dv,int nType)
{
	sort(dv.begin(),dv.end(),cmpTV);
	max = dv.back().GetValue();
	min = dv.front().GetValue();
	cards = dv;
	switch(nType)
	{
	case CT_BOOM: //ը��
		m_value = nLevelBoom;
		break;

	case CT_THREE: //����
		m_value = nLevelThree;
		break;

	case CT_DOUBLE: //����
		m_value = nLevelDouble;
		break;

	case CT_MULTITHREE: //��˳
		m_value = dv.size();
		break;

	case CT_MULTIDOUBLETWO: //˫˳
		m_value = dv.size();
		break;

	case CT_MULTI:	//˳��
		m_value = dv.size();
		break;

	case CT_MAXBOOM:
		m_value = nLevelBoom;
		break;
	
	case CT_SINGLE:
		m_value = nLevelSingle;
		break;
	default:
		break;
	}
}

CRobot::CRobot(int nLandLord )
{
	this->Reset();
	m_nRivalCount = 0;
	m_nSuggestOutTime = 0;
	m_nLandlord = nLandLord;
}

CRobot::CRobot(TCardVector handCards[MAX_PLAYER])
{
	for (int i=0; i<MAX_PLAYER;i++)
	{
		m_dvPlayerCards[i] = handCards[i];
		m_dvBackupPlayerCards[i] = m_dvPlayerCards[i];
	}
	m_nRivalCount = 0;
	m_nSuggestOutTime = 0;
}

CRobot::CRobot(TCardVector dvHandCards)
{
	m_dvNowCards_1 = dvHandCards;
	m_nRivalCount = 0;
	m_nSuggestOutTime = 0;
}
CRobot::~CRobot(){}


//������������
bool cmpSingle(TCards T1,TCards T2)
{
	int nT1 = GameRule::CardWeight(T1);
	int nT2 = GameRule::CardWeight(T2);
	return nT1<nT1;
}

int CRobot::GetCardNum(int nCardVal, TCardVector dvSearch)
{
	int nCount = 0;
	for (int i=0; i<dvSearch.size();i++)
	{
		if (nCardVal == dvSearch.at(i).GetValue())
		{
			nCount++;
		}
	}
	return nCount;
}
//////////////////////////////////////////////////////////////////////////
//ְ�ܣ�������������
//˵����������������ʵ����ֻ�Ǽ������Լ�����
//          ���Ƶ�Ȩֵ���Ƿ�ǿ����������Ϸ�߼�����
//����Ȩֵ���壺ը��Ϊ5 ����Ϊ5��С��Ϊ3, 2Ϊ2,AΪ1
//����������ָ����Ҫ����Ȩֵ����
//���أ�����ָ�������Ȩֵ
int  CRobot::RobotGrabLand(TCardVector dvHand)
{
	if (dvHand.empty())
	{
		return -1;
	}
	int nLevel =0;
	sort(dvHand.begin(),dvHand.end(),cmpTV);
	//���
	TCardVector tmp;
	tmp.push_back(dvHand.back());
	tmp.push_back(dvHand.at(dvHand.size()-2));
	CardTypeInfo cti = GameRule::GetCardType(tmp);
	if (cti.nType == CT_MAXBOOM) nLevel += 5;
	//ը��
	CardsVector boom;
	this->ChoseBoom(boom,dvHand);
	nLevel += 5 * boom.size();
	//����
	switch (dvHand.back().GetValue()) 
	{
	case CV_BJ:
		nLevel += 5;
		break;
	case CV_SJ:
		nLevel += 3;
		break;
	default:
		break;
	}
	if (dvHand.at(dvHand.size()-2).GetValue() == CV_SJ) nLevel += 3;

	//2
	nLevel += 2*(GetCardNum(CV_2,dvHand));
	//A
	nLevel += GetCardNum(CV_A,dvHand);
	return nLevel;
}

int CRobot::RobotCallEx(AICallIn theCallIn, AICallOut& theCallOut,int nBaseRand)
{
	theCallOut.returnscore = 0;
	//����Ȩֵ���壺
	//	����=5��С��=3��2=2��A=1��ը��=5
	BYTE topvalue = 5 * theCallIn.val[CARD_BJ] + 3 * theCallIn.val[CARD_SJ] + 
		            2 * theCallIn.val[CARD_2]  + theCallIn.val[CARD_A];
	for (int i=CARD_3;i<CARD_K;i++)
	{
		if (theCallIn.val[i] == 4)
		{
			topvalue += 5;
		}
	}
	//����������
	//1.Ȩֵ����
	/*
		����LevȨֵ���壺
		����=5��С��=3��2=2��A=1��ը��=5
		������������������
		if ��ֵС�ڵ���4  ���У���һ��
		if ��ֵ����4С�ڵ���8  ��һ�֣�����
		if ��ֵ����8С�ڵ���11 ����������һ�֣����֣�����
		if ��ֵ����11          ������
	*/
	srand(time(0));
	int nCharacter = rand()%100;
	if ( nCharacter <= nBaseRand)  //Ĭ��%80��������Խ����������������
	{
		if (topvalue <= 4 && theCallIn.callscore<=0)
		{
			return abs((rand())%2);
		}else if (topvalue >4 && topvalue <= 8 && theCallIn.callscore <2)
		{
			int n= abs((rand()%3 +theCallIn.callscore)+theCallIn.callscore);
			if (n>2) n=2;
			return n;
		}else if (topvalue > 8 && topvalue <=11&& theCallIn.callscore <3)
		{
			int n= abs((rand()%3 +theCallIn.callscore)+theCallIn.callscore);
			if (n>3) n=3;
			return n;
		}else if (topvalue > 11)
		{
			return 3;
		}
	}else if (nCharacter > nBaseRand && nCharacter <= (nBaseRand+(100-nBaseRand)/2))  //Ĭ��%10����������
	{
		return 3;
	}else   //��Զ�������� 
	{
		return 0;
	}
	return 0;
}

bool CRobot::RobotCall( AICallIn theCallIn, AICallOut& theCallOut, int nBaseRand /*=80*/)
{
	theCallOut.returnscore = 0;

	//����Ȩֵ���壺
	//	����=5��С��=3��2=2��A=1��ը��=5
	BYTE topvalue = 5 * theCallIn.val[CARD_BJ] + 3 * theCallIn.val[CARD_SJ] + 
		            2 * theCallIn.val[CARD_2]  + theCallIn.val[CARD_A];
	for (int i=CARD_3;i<CARD_K;i++)
	{
		if (theCallIn.val[i] == 4)
		{
			topvalue += 5;
		}
	}
	//����������
	//1.Ȩֵ����
	/*
		����LevȨֵ���壺
		����=5��С��=3��2=2��A=1��ը��=5

		������������������

		if ��ֵС�ڵ���4  ���С�����
		if ��ֵ����4С�ڵ���8  �е�����1��
		if ��ֵ����8С�ڵ���11 ��������1��2
		if ��ֵ����11          ������������
	*/
	srand(time(0));
	int nCharacter = rand()%100;
	if ( nCharacter <= nBaseRand)  //Ĭ��%80��������Խ����������������
	{
		if (topvalue <= 4)
		{
			theCallOut.returnscore = theCallIn.callscore;
		}else if (topvalue >4 && topvalue <= 8  )
		{
			if (theCallIn.callscore < 2)
			{
				theCallOut.returnscore = theCallIn.callscore + 1;
			}else 
			{
				theCallOut.returnscore = theCallIn.callscore;
			}
		}else if (topvalue > 8 && topvalue <=11 )
		{
			if (theCallIn.callscore < 3)
			{
				theCallOut.returnscore = theCallIn.callscore + 1;
			}else 
			{
				theCallOut.returnscore = theCallIn.callscore;
			}
		}else if (topvalue > 11)
		{
			theCallOut.returnscore = theCallIn.callscore + 1;
		}else 
		{
			theCallOut.returnscore = theCallIn.callscore;
		}
	}else if (nCharacter > nBaseRand && nCharacter <= (nBaseRand+(100-nBaseRand)/2))  //Ĭ��%10����������
	{
		theCallOut.returnscore = theCallIn.callscore;
	}else   //��Զ�������� 
	{
		theCallOut.returnscore = theCallIn.callscore;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
//����ְ�ܣ������˳���
//������	handCards������е���,nSeatָ���������λ��
//����:		������Ҫ������ƣ��ú���������Ҳ������ʧ�ܡ�
//by��      tmfish  at 2014-03-29
TCardVector CRobot::RobotPlayCards( TCardVector handCards[MAX_PLAYER], int nSeat)
{
	//Ҳ���е�ʱ��������һ�Ѵ������е��ƣ���������²����з���
	CardTypeInfo cti = GameRule::GetCardType(handCards[nSeat]);
	if (cti.nType > CT_NONE && cti.nType < CT_COUNT && 
		cti.nType != CT_FOURWITHTWO && cti.nType != CT_FOURWITHDTWO)
	{
		return handCards[nSeat];
	}
	this->Reset();
	m_dvAllCards.clear();
	m_dvNowCards_1 = handCards[nSeat];
	m_dvOldCards = m_dvNowCards_1;
	for (int i=0; i<MAX_PLAYER; i++)
	{
		m_dvAllCards+=handCards[i];
		m_dvPlayerCards[i] = handCards[i];
		m_dvBackupPlayerCards[i] = handCards[i];
		//ȷ�Ͼ�������
		m_Warring[i].nSeat=i;
		m_Warring[i].nWL  = m_dvPlayerCards[i].size();
	}

	m_nOutSeat = nSeat;
	//��������
	for (int i=0; i<MAX_PLAYER; i++)
	{
		ShowDebug(m_dvPlayerCards[i],"�������");
		cvTmpJoker.clear();
		sort(m_dvPlayerCards[i].begin(),m_dvPlayerCards[i].end(),cmpTV);
		//��С����ȥ��
		while (!m_dvPlayerCards[i].empty() && m_dvPlayerCards[i].back().GetValue() > CV_A)
		{
			cvTmpJoker.push_back(m_dvPlayerCards[i].back());
			m_dvPlayerCards[i].pop_back();
		}
		int nLevel = this->AnalyzeCards(m_dvPlayerCards[i],i);

		m_dvPlayerCards[i] += cvTmpJoker;
	}
	//debug
	this->ShowDebug(m_PlayerSplit[m_nOutSeat],"����Ϊ");
	//debug
	return this->FindNowOut();
}

//����ָ�������и�����ֵ������
void CRobot::CalcCardConut(int nValue[],TCardVector dvCards)
{
	if (dvCards.empty()) return;
	for (int i=CV_2;i<=CV_BJ;i++)
	{
		if (i == CV_BJ) nValue[14] = GetCardNum(CV_BJ,dvCards);
		else if (i == CV_SJ) nValue [13] = GetCardNum(CV_SJ,dvCards);
		else if (i == CV_2)  nValue [12] = GetCardNum(CV_2,dvCards);
		else nValue[i-CV_3] = GetCardNum(i,dvCards);
	}
}
//////////////////////////////////////////////////////////////////////////
//����ְ�ܣ������˸���
//����˵��������1ָ��������ҵ����ƣ�����2ָ����Ҫѹ���ƣ�����3ָ����Ҫѹ������˭�����
//                 ����4���ڷ��ظ��ƣ�����5ָ����˭�ڸ���
//���أ�       ��������˳ɹ��ҵ�ѹ�ƣ�����true���������false����ѹ��
//by :           tmfish at 2014-04-16
bool  CRobot::RobotFollowCards(TCardVector handCards[MAX_PLAYER],TCardVector dvOut,
							   int nOutSeat,TCardVector& dvRes,int nWhoOut)
{
	this->Reset();
	m_nOutSeat = nWhoOut;
	m_nLastOut = nOutSeat;
	m_dvAllCards.clear();
	for (int i=0; i<MAX_PLAYER;i++)
	{
		m_dvPlayerCards[i] = handCards[i];
		m_dvAllCards+=handCards[i];
	}
	m_dvNowCards_1 = handCards[nWhoOut];
	m_dvOldCards = m_dvNowCards_1;
	for (int i=0; i<MAX_PLAYER; i++)
	{
		m_dvPlayerCards[i] = handCards[i];
		m_dvBackupPlayerCards[i] = handCards[i];
		//ȷ�Ͼ�������
		m_Warring[i].nSeat=i;
		m_Warring[i].nWL  = m_dvPlayerCards[i].size();
	}
	//Ѱ�ҵ�ǰ�������������������
	this->FindMaxThreeCards();

	//��������
	for (int i=0; i<MAX_PLAYER; i++)
	{
		cvTmpJoker.clear();
		sort(m_dvPlayerCards[i].begin(),m_dvPlayerCards[i].end(),cmpTV);
		//��С����ȥ��
		while (!m_dvPlayerCards[i].empty() && m_dvPlayerCards[i].back().GetValue() > CV_A)
		{
			cvTmpJoker.push_back(m_dvPlayerCards[i].back());
			m_dvPlayerCards[i].pop_back();
		}
		int nLevel = this->AnalyzeCards(m_dvPlayerCards[i],i);
		//�Ѵ�С�����ӻ���
		m_dvPlayerCards[i] += cvTmpJoker;
	}
	sort(dvOut.begin(),dvOut.end(),cmpTV);
	this->ShowDebug(m_dvBooms,"ը��");

	bool bFlag = FindNowFollow(dvOut,dvRes);
	return bFlag;
}

//����ָ����ת�������飬����2����ת����Ľ��
void CRobot::ConvertCTToCV(CT& who, CardsVector& cv, int nType)
{
	if (who.empty())
	{
		cv.clear();
	}else 
	{
		for (int i=0; i<who.size();i++)
		{
			cv.push_back(who.at(i).cards);
		}
	}
}
//����ָ����ת�������飬����2����ת����Ľ��
void CRobot::ConvertCVToCT(CardsVector& who, CT& ct, int nType)
{
	if (who.empty())
	{
		ct.clear();
	}else 
	{
		CARDSTYLE tmpct;
		for (int i=0; i<who.size();i++)
		{
			tmpct.Fill(who.at(i),nType);
			ct.push_back(tmpct);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
//�߼�����
//������
	/*
	a) ��ȷ��ը��������ը����
	b����ȷ������
	c����ȷ����˳
	d����ȷ����˳
	e) ��ȷ��˫˳
	f����ȷ������
	e) ��ȷ������
	*/
int	 CRobot::AnalyzeCards(TCardVector dvHandCards, int nSeat)
{
	this->ResetTmp();
	TCardVector tmpHandCards;
	sort(dvHandCards.begin(),dvHandCards.end(),cmpTV);	
	TCardVector dvAnalyCards = dvHandCards;
	//ը��
	this->ChoseBoom(cvTmpBoom,tmpHandCards);
	tmpHandCards = dvAnalyCards;
	//����
	this->ChoseThree(cvTmpThree,tmpHandCards);
	tmpHandCards = dvAnalyCards;
	//��˳
	this->ChoseThreeMultis(cvTmpThreeMulti,cvTmpThree);
	tmpHandCards = dvAnalyCards;
	//����
	this->ChoseDouble(cvTmpDouble,tmpHandCards);
	tmpHandCards = dvAnalyCards;
	//˫˳
	this->ChoseDoubleMultis(cvTmpDoubleMulti,cvTmpDouble);
	//��˳
	tmpHandCards = dvAnalyCards;
	this->ChoseSingleMultis(cvTmpSingleMulti,tmpHandCards,cvTmpDoubleMulti);
	//��������
	int tempbosb,tempplane,tempLinkdouble,tempthree,tempdouble,tmpSingleMulti;

	tmpSingleMulti = cvTmpSingleMulti.empty()?0:1;
	tempbosb	= cvTmpBoom.empty()?0:1;
	tempplane	= cvTmpThreeMulti.empty()?0:1;
	tempLinkdouble = cvTmpDoubleMulti.empty()?0:1;
	tempthree	= cvTmpThree.empty()?0:1;
	tempdouble  = cvTmpDoubleMulti.empty()?0:1;
	for (int i=0;i<=tempbosb;i++)//ը��
	{
		for (int j=0;j<=tempplane;j++)//��˳
		{
			for (int k=0;k<=tempLinkdouble;k++)//����
			{
				for (int f=0;f<=tempthree;f++)//3��
				{
					for (int h=0;h<=tempdouble;h++)//����
					{
						for (int n=0;n<=tmpSingleMulti;n++)//��˳
						{
							ErgodicCards(i,j,k,f,h,n,nSeat);
						}
					}
				}
			}
		}
	}

	//����������Ȩֵ��ѡ�����ŵĲ��Ʒ�ʽ
	FindOptimalSplit(nSeat);


	//������������ҵ�����
	if (nSeat == m_nOutSeat)
	{
		sort(m_dvSingles.begin(),m_dvSingles.end(),cmpTV);
		sort(m_dvDoubles.begin(),m_dvDoubles.end(),cmpCV);
		sort(m_dvThrees.begin(),m_dvThrees.end(),cmpCV);
		sort(m_dvSingleMultis.begin(),m_dvSingleMultis.end(),cmpCV);
		sort(m_dvDoubleMultis.begin(),m_dvDoubleMultis.end(),cmpCV);
		sort(m_dvThreeMultis.begin(),m_dvThreeMultis.end(),cmpCV);
		sort(m_dvBooms.begin(),m_dvBooms.end(),cmpCV);
		sort(m_dvOldCards.begin(),m_dvOldCards.end(),cmpTV);
		//��������
	}
	return m_PlayerSplit[nSeat].GetHandCount();
}


//////////////////////////////////////////////////////////////////////////
//ѡȡ��˳
//ѡȡ����
/* 
	��ȡ����С��һ��������˳������ʣ�������ѡȡ��С��һ���������������ƣ�ֱ��û������Ϊֹ��
	��ɺ������������չ����ʣ��������Ѿ�ȡ�����ƽ��бȶԣ����ĳ��ʣ���������֪����������
	�ɸ�������ƣ�����ϲ���һֱ���޷��ϲ�Ϊֹ�� ������ĳ���������ܹ��޷����ӳ�Ϊ����
	�����ƣ�����ϲ�Ϊһ�顣
*/
void CRobot::ChoseSingleMultis(CardsVector& cvNeedFill, TCardVector& dvFrom,CardsVector cvDoubles)
{
	if (dvFrom.empty())
	{
		return;
	}
	//ѡȡ����
	sort(dvFrom.begin(),dvFrom.end(),cmpSingle);
	
	while (dvFrom.size()>=5)
	{
		TCardVector TmpCard =FindFiveMultis(dvFrom);
		if(TmpCard.size() == 5)
		{	
			cvNeedFill.push_back(TmpCard);
			//ɾ����ǰ������ѡȡ��������˳�ӵ���
			for (int i=0; i<TmpCard.size();i++)
			{
				dvFrom.erase(remove(dvFrom.begin(),dvFrom.end(),TmpCard.at(i)),
					dvFrom.end());
			}
		
			TmpCard.clear();
		}else
		{
			break;
		}
	}
	//��չ����
	TCardVector tmp;
	CardTypeInfo cti;
	for (int j=0;j<cvNeedFill.size();j++)
	{
		for (int i=0;i<dvFrom.size();i++)
		{
			tmp.clear();
			cti.reset();
			tmp= cvNeedFill.at(j);
			tmp.push_back(dvFrom.at(i));

			GameRule::SortCards(tmp);
			GameRule::IsMulti(tmp,cti);
			if (cti.nType == CT_MULTI)
			{
				cvNeedFill.at(j) = tmp;	
			}
		}
		//ɾ����ǰ�����й���˳�ӵ���
		//ɾ����ǰ������ѡȡ��������˳�ӵ���
		for (int i=0; i<cvNeedFill.at(j).size();i++)
		{
			dvFrom.erase(remove(dvFrom.begin(),dvFrom.end(),cvNeedFill.at(j).at(i)),
				dvFrom.end());
		}
	}
	//�ϲ�����
	if (cvNeedFill.size()>=2)
	{
		CardTypeInfo ctiFirst,ctiSecend;
		ctiFirst.reset();
		ctiSecend.reset();
		TCardVector tmpCards;
		CardsVector tmpSingleMultis = cvNeedFill;
		for (int i=0; i<tmpSingleMultis.size()-1;i++)
		{
			tmpCards = tmpSingleMultis.at(i);
			ctiFirst = GameRule::GetCardType(tmpCards);
			for (int j=i+1 ;j<tmpSingleMultis.size();j++)
			{
				ctiSecend = GameRule::GetCardType(tmpSingleMultis.at(j));
				if (ctiFirst == ctiSecend)
				{
					tmpCards += tmpSingleMultis.at(j);
					cvTmpDoubleMulti.push_back(tmpCards);
					//��������ȫ��ͬ��˳�����˫˳�󣬰������Ƴ���˳����
					cvNeedFill.erase(remove(cvNeedFill.begin(),cvNeedFill.end(),tmpSingleMultis.at(i)),
						cvNeedFill.end());
					cvNeedFill.erase(remove(cvNeedFill.begin(),cvNeedFill.end(),tmpSingleMultis.at(j)),
						cvNeedFill.end());
					tmpCards.clear();
					break;
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//ѡȡָ����������С������
//������dvCardsָ�����Ķ�ѡȡ����
//���أ������ָ���������ҵ���������ô�����ҵ������������򷵻�һ��������

TCardVector CRobot::FindFiveMultis(TCardVector& dvCards)
{
	TCardVector dvFiveMultis;
	TCardVector dvTmpCards = dvCards;
	dvFiveMultis.clear();
	if (dvCards.size() < 5)
	{
		return dvFiveMultis;
	}
	
	sort(dvTmpCards.begin(),dvTmpCards.end(),cmpTV);
	//������С��˳��
	for (int i=0; i<dvTmpCards.size()-1;i++)
	{
		if (dvTmpCards.at(i).GetValue()+1 == dvTmpCards.at(i+1).GetValue())
		{
			if (dvFiveMultis.empty())
			{
				dvFiveMultis.push_back(dvTmpCards.at(i));
				dvFiveMultis.push_back(dvTmpCards.at(i+1));
			}else 
			{
				dvFiveMultis.pop_back();
				dvFiveMultis.push_back(dvTmpCards.at(i));
				dvFiveMultis.push_back(dvTmpCards.at(i+1));
			}
			
			if (dvFiveMultis.size()==5)
			{
				return dvFiveMultis;
			}
		}else if (dvTmpCards.at(i).GetValue() == dvTmpCards.at(i+1).GetValue())
		{
			continue;
		}else
		{
			dvFiveMultis.clear();
		}
	}
	if (dvFiveMultis.size() < 5)
	{
		dvFiveMultis.clear();
	}

	return dvFiveMultis;
}

//////////////////////////////////////////////////////////////////////////
//����������������
bool cmp_double(TCardVector T1,TCardVector T2)
{
	int nValueT1=0,nValueT2=0;
	for (int i=0; i<T1.size();i++)
	{
		nValueT1 += T1.at(i).GetValue();
		nValueT2 += T2.at(i).GetValue();
	}
	return (nValueT1 < nValueT2)?true:false;
}
//ѡȡ˫˳
//ѡȡ����
	/*
		��һ���������еĶ��Ӽ�������Ȼ�󽫶�����������������Ķ��ӣ�����ϲ���(�����ܴ��)˫˳.
	*/
void CRobot::ChoseDoubleMultis(CardsVector& cvNeedFill, CardsVector& dvFrom)
{
	if (dvFrom.size() < 3 )
	{
		return ;
	}
	//��2ȥ��
	if (dvFrom.back().front().GetValue() == CV_2)
	{
		dvFrom.pop_back();
	}
	sort(dvFrom.begin(),dvFrom.end(),cmp_double);
	//�����ж����бȽ��ҳ�˫˳
	vector<int>  nvDoubleValue; 
	int nValue = 0;
	for (int i=0;i <dvFrom.size();i++)
	{
		//for (int j=0;j<m_dvDoubles.at(i).size();j++)
		{
			nValue = dvFrom.at(i).front().GetValue();
		}
		nvDoubleValue.push_back(nValue);
	}

	int nBegin = 0;
	int nLen =1;
	{
	int i=0;
	for ( i=0; i<nvDoubleValue.size()-1;i++)
	{
		if (nvDoubleValue.at(i)+1 == nvDoubleValue.at(i+1))
		{
			nLen++;
		}
		if (nvDoubleValue.at(i)+1 != nvDoubleValue.at(i+1)
			||i == nvDoubleValue.size()-2)
		{
			//��ȡ������
//AllDoubleMulti:	
			if (nLen >= 3)
			{				
				TCardVector tmpDoubleMultis;
				//for (int j=0;j<nLen;j++)
				{
					for(int n=nBegin; n<nBegin+nLen; n++)
					{
						//if (nvDoubleValue.at(j) == m_dvDoubles.at(n).begin()->GetValue())
						{
							tmpDoubleMultis += dvFrom.at(n);
						}
					}
				}
				cvNeedFill.push_back(tmpDoubleMultis);
			}
			nBegin = i+1;
			nLen = 1;
		}
	}
}
//	if (nLen >=2) 
	{
//		MessageBox(NULL,"��goto���","info",MB_OK);
	//	goto AllDoubleMulti;
	}

	//���ܹ�����˫˳�Ķ��ӴӶ��������������޳�
	CardsVector tmpDoubles = dvFrom;
	for (int i=0; i<cvNeedFill.size();i++)
	{
		for (int j=0; j<cvNeedFill.at(i).size();j+=2)
		{
			for (int n=0;n<tmpDoubles.size();n++)
			{
				if (tmpDoubles.at(n).begin()->GetValue() == cvNeedFill.at(i).at(j).GetValue())
				{
					TCardVector tmpErase = tmpDoubles.at(n);
					dvFrom.erase(remove(dvFrom.begin(),dvFrom.end(),tmpErase),dvFrom.end());
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//����������������
bool cmp_three(TCardVector T1,TCardVector T2)
{
	int nValueT1=0,nValueT2=0;
	for (int i=0; i<T1.size();i++)
	{
		nValueT1 += T1.at(i).GetValue();
		nValueT2 += T2.at(i).GetValue();
	}
	return (nValueT1 < nValueT2)?true:false;
}
//ѡȡ��˳
	/*
		һ�����е������������ܱȽϷ������ȡ������ 
		�������Ļ����Ͻ��бȽϣ����������������������ϲ��ɣ������ܴ�ģ���˳��
	*/
void CRobot::ChoseThreeMultis(CardsVector& cvNeedFill, CardsVector& dvFrom)
{
	if (dvFrom.size() < 2 )
	{
		return ;
	}

	//��2ȥ��
	if (dvFrom.back().front().GetValue() == CV_2)
	{
		dvFrom.pop_back();
	}

	sort(dvFrom.begin(),dvFrom.end(),cmp_three);
	//�����ж����бȽ��ҳ�˫˳
	vector<int>  nvValueThree; 
	int nValue = 0;
	for (int i=0;i <dvFrom.size();i++)
	{
		//for (int j=0;j<m_dvDoubles.at(i).size();j++)
		{
			nValue = dvFrom.at(i).at(0).GetValue();
		}
		nvValueThree.push_back(nValue);
	}
	int nBegin = 0;
	int nLen =1;
{
	int i=0;
	for ( i=0; i<nvValueThree.size()-1;i++)
	{
		if (nvValueThree.at(i)+1 == nvValueThree.at(i+1))
		{
			nLen++;
		}
		if (nvValueThree.at(i)+1 != nvValueThree.at(i+1) ||
			nvValueThree.size()-2 == i)
		{
			//��ȡ������
			if (nLen >= 2)
			{
//AllMulti:
				TCardVector tmpThreeMultis;
			//	for (int j=0;j<nLen;j++)
				{
					for(int n=nBegin; n<nBegin+nLen; n++)
					{
					//	if (nvValueThree.at(j) == m_dvThrees.at(n).begin()->GetValue())
						{
							tmpThreeMultis += dvFrom.at(n);
						}
					}
				}
				cvNeedFill.push_back(tmpThreeMultis);
			}
			nBegin = i+1;
			nLen = 1;
		}
	}
}
//	if (nLen>=2) goto AllMulti;

	//���ܹ�����˫˳�Ķ��ӴӶ��������������޳�
	CardsVector tmpThrees = dvFrom;
	for (int i=0; i<cvNeedFill.size();i++)
	{
		for (int j=0; j<cvNeedFill.at(i).size();j+=3)
		{
			for (int n=0;n<tmpThrees.size();n++)
			{
				if (tmpThrees.at(n).begin()->GetValue() == cvNeedFill.at(i).at(j).GetValue())
				{
					TCardVector tmpErase = tmpThrees.at(n);
					dvFrom.erase(remove(dvFrom.begin(),dvFrom.end(),tmpErase),dvFrom.end());
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//ѡȡ����
void CRobot::ChoseThree(CardsVector& cvNeedFill, TCardVector& dvFrom)
{
	FillCards(3,CT_THREE,cvNeedFill,dvFrom);
}

//////////////////////////////////////////////////////////////////////////
//ѡȡ����
void CRobot::ChoseDouble(CardsVector& cvNeedFill, TCardVector& dvFrom)
{
	FillCards(2,CT_DOUBLE,cvNeedFill,dvFrom);
}

//////////////////////////////////////////////////////////////////////////
//ѡȡը��,ը���ɴ�һ�ŵ��ƣ�����ѡ����ը����������������3��4�����ֳ���
//cvNeedFillָ��ѡȡ����ը�����λ�ã�dvFromָ�����Ķ�ѡȡը��
void CRobot::ChoseBoom(CardsVector& cvNeedFill, TCardVector& dvFrom)
{
	cvNeedFill.clear();
	//���ŵ�ը��
	FillCards(4,CT_BOOM,cvNeedFill,dvFrom);
}


//���Լ���Ҫ�����ʱ��Ѱ�ҳ���
TCardVector CRobot::MeWillFinish()
{
	TCardVector tmp;
	tmp.clear();

	return tmp;
}

//�����ֿ�Ҫ�����ʱ��Ѱ�ҳ���
TCardVector CRobot::RivalWillFinish()
{
  	TCardVector tmp;
  	tmp.clear();
  	return tmp;
}

//һ������µĳ�������
TCardVector CRobot::FindOutGeneral(bool bNotOutS,bool bNotOutD,bool bNotOutT)
{
	TCardVector tmp;
	tmp.clear();
	int nNext = GetNextPlayerSeat(m_nOutSeat);
	int nLast =  GetLastPlayerSeat(m_nOutSeat);
	//ѡ�����Ŵ����ǵ��Ż��Ƕ��ӣ�
	bool bChoseSingle = true; 
	//����������п���Ȩ�����Ƕ���û�У���ô�����ӣ����������
	if (m_dvSingles.empty())
	{
		bChoseSingle = false;
	}else if (!m_dvSingles.empty() && !m_dvDoubles.empty() )
	{
		int nValueS = GameRule::CardWeight(m_dvSingles.front());
		int nValueD = GameRule::CardWeight(m_dvDoubles.front().front());
		bChoseSingle = nValueD>nValueS?true:false;
	}
		//����
	/*һ������£�����Ӧ���ȳ�����һ�������ٳ�����
		�������Ƶ�����ԭ��
		 1.���ȵ���С�ƣ�
		 2.�����������ȼ���ߣ����ǲ��ܽӻص��ƣ�����ǵ��������ڶ���
	*/
	if (1 == FindOutFromThrees(tmp))
	{
		if (bChoseSingle)
		{
			if (!m_dvSingles.empty() && m_dvSingles.front() > CV_2 && m_dvSingles.front() < CV_K )  
			{
// 				if (m_dvSingles.front().GetValue()  != 2 && m_dvSingles.front().GetValue() < CV_K)
// 				{
					tmp.push_back(m_dvSingles.front());
/*				}else if (!m_dvDoubles.empty() && )*/
				return tmp;
			}
			if 	(!m_dvDoubles.empty() && m_dvDoubles.front().front().GetValue() > CV_2 && m_dvDoubles.front().front().GetValue() < CV_K)
			{
				tmp += m_dvDoubles.front();
				return tmp;
			}
		}else 
		{
			if 	(!m_dvDoubles.empty() && m_dvDoubles.front().front().GetValue() > CV_2 && m_dvDoubles.front().front().GetValue() < CV_K)
			{
				tmp += m_dvDoubles.front();
				return tmp;
			}
			if (!m_dvSingles.empty() && m_dvSingles.front() > CV_2 && m_dvSingles.front() < CV_K )  
			{
				tmp.push_back(m_dvSingles.front());
				return tmp;
			}
		}
		//��������ﻹ��û��ѡȡ�����ӣ���ô���ǰѶ��ӵ������Ŵ���
		return tmp;
	}

	//��˳
	if (1==FindOutFromThreeMulti(tmp))
	{
		// Ϊ��˳�ҵ����Ӵ�ɷɻ����������
		int nThreeLen = tmp.size()/3; 
		if (bChoseSingle)
		{
			if (m_dvSingles.size() >=nThreeLen)  //����˳���ϵ���
			{
				if (m_dvSingles.at(nThreeLen-1).GetValue() <= CV_K && 
					m_dvSingles.at(nThreeLen-1).GetValue()  != CV_2)
				{
					tmp.insert(tmp.end(),m_dvSingles.begin(),m_dvSingles.begin()+nThreeLen);
					return tmp;
				}
			}
			if (m_dvDoubles.size() >= nThreeLen) //������
			{
				if (m_dvDoubles.at(nThreeLen-1).front().GetValue() <= CV_Q&&
					m_dvDoubles.at(nThreeLen-1).front().GetValue() != CV_2)
				{
					for (int i=0; i<nThreeLen; i++)
					{
						tmp += m_dvDoubles.at(i);
					}
					return tmp;
				}
			}
		}else 
		{
			if (m_dvDoubles.size() >= nThreeLen) //������
			{
				if (m_dvDoubles.at(nThreeLen-1).front().GetValue() <= CV_Q&&
					m_dvDoubles.at(nThreeLen-1).front().GetValue() != CV_2)
				{
					for (int i=0; i<nThreeLen; i++)
					{
						tmp += m_dvDoubles.at(i);
					}
					return tmp;
				}
			}

			if (m_dvSingles.size() >=nThreeLen)  //����˳���ϵ���
			{
				if (m_dvSingles.at(nThreeLen-1).GetValue() <= CV_K && 
					m_dvSingles.at(nThreeLen-1).GetValue()  != CV_2)
				{
					tmp.insert(tmp.end(),m_dvSingles.begin(),m_dvSingles.begin()+nThreeLen);
					return tmp;
				}
			}
		}
		return tmp;
	}

	//�����ߵ������������ѡ���ţ������п������Ż�����˳��ֵ�Ƚϴ�����û��ѡ����
	//�˴�Ӧ��Ϊ��Щ������������Ż��߶���
	//���������Ŵ���
	TCardVector tmpSingle = m_dvSingles;
	CardsVector tmpDoulse = m_dvDoubles;
	this->ReservedSmallCards(bChoseSingle);
	//����
	if (1 == FindOutFromSingle(tmp))
	{
		return tmp;
	}
	//����
	if (1 == FindOutFromDoubles(tmp))
	{
		return tmp;
	}
	//�ߵ����֤�����źͶ���Ҳû�з��������ĳ��ƣ�
	//��ԭ�����õ�����
	m_dvSingles  =  tmpSingle;
	m_dvDoubles = tmpDoulse;

	//��˳
	if (1==FindOutFromSingleMuti(tmp))
	{
		return tmp;
	}
	//˫˳
	if (1==FindOutFromDoubleMuti(tmp))
	{
		return tmp;
	}

	//����ȫ�Ǵ���
	return FastOut();

	if (m_dvBooms.size()>0)   //ը��--��ȷ����
	{
		sort(m_dvBooms.begin(),m_dvBooms.end(),cmpCV);	
		tmp = m_dvBooms.front();
		return tmp;
	}
	//����ǻ��
	if (m_dvMaxBoom.size() >0)
	{
		tmp = m_dvMaxBoom;
		return tmp;
	}
	//�������������⴦��
	sort(m_dvOldCards.begin(),m_dvOldCards.end(),cmpTV);
	tmp.push_back(m_dvOldCards.at(0));
	return tmp;
}

bool CRobot::RivalHaveGreaterThanMe(CardsVector myCards)
{
	TCardVector Res;
	if (!myCards.empty())
	{
		int nNext = GetNextPlayerSeat(m_nLandlord);
		int nLast  = GetLastPlayerSeat(m_nLandlord);
		sort(myCards.begin(),myCards.end(),cmpCV);
		if (m_nOutSeat == m_nLandlord)  //�����Ķ���������
		{
			if (GetGreaterThanNextOut(m_dvPlayerCards[nNext],myCards.back(),Res) ||
				GetGreaterThanNextOut(m_dvPlayerCards[nLast],myCards.back(),Res))
			{
				return true;
			}else 
			{
				return false;
			}
		}else   //ũ��Ķ���ֻ��һ������
		{
			return GetGreaterThanNextOut(m_dvBackupPlayerCards[m_nLandlord],myCards.back(),Res);
		}
	}
	return false;
}

bool CRobot::RivalHaveGreaterThanMe(TCardVector myCards)
{
	TCardVector Res;
	if (!myCards.empty())
	{
		int nNext = GetNextPlayerSeat(m_nLandlord);
		int nLast  = GetLastPlayerSeat(m_nLandlord);
		TCardVector tmp;
		sort(myCards.begin(),myCards.end(),cmpTV);
		tmp.push_back(myCards.back());
		if (m_nOutSeat == m_nLandlord)  //�����Ķ���������
		{
			if (GetGreaterThanNextOut(m_dvPlayerCards[nNext],tmp,Res) &&
				GetGreaterThanNextOut(m_dvPlayerCards[nLast],tmp,Res))
			{
				return true;
			}else 
			{
				return false;
			}
		}else   //ũ��Ķ���ֻ��һ������
		{
			return GetGreaterThanNextOut(m_dvBackupPlayerCards[m_nLandlord],tmp,Res);
		}
	}
	return false;
}

int CRobot::CalcGreaterThanRivalCount(TCardVector myCards,int& nSameCount)
{
	int nCount = 0;
	int nNext = GetNextPlayerSeat(m_nLandlord);
	int nLast  = GetLastPlayerSeat(m_nLandlord);
	if (!myCards.empty())
	{
		TCardVector tmp;
		TCardVector TmpO;
		for (int i=0;i<myCards.size();i++)
		{
			TmpO.clear();
			TmpO.push_back(myCards.at(i));
			if (m_nOutSeat == m_nLandlord) //��������������
			{
				if (!GetGreaterThanNextOut(m_dvPlayerCards[nLast],TmpO,tmp)&&
					!GetGreaterThanNextOut(m_dvPlayerCards[nNext],TmpO,tmp))
				{
					nCount++;
				}
			}else //ũ��ֻ�е���һ������
			{
				if (!GetGreaterThanNextOut(m_dvPlayerCards[m_nLandlord],TmpO,tmp))
				{
					nCount ++ ;
				}
			}
		}
	}
	nSameCount = max(nSameCount,nCount);
	return nCount;
}


int CRobot::CalcGreaterThanRivalCount(CardsVector myCards,int& nSameCount)
{
	int nCount = 0;
	int nNext = GetNextPlayerSeat(m_nLandlord);
	int nLast  = GetLastPlayerSeat(m_nLandlord);
	if (!myCards.empty())
	{
		for (int i=0;i<myCards.size();i++)
		{
			TCardVector tmp;
			if (m_nOutSeat == m_nLandlord) //��������������
			{
				if (!GetGreaterThanNextOut(m_dvPlayerCards[nLast],myCards.at(i),tmp)&&
					!GetGreaterThanNextOut(m_dvPlayerCards[nNext],myCards.at(i),tmp))
				{
					nCount++;
				}
			}else //ũ��ֻ�е���һ������
			{
				if (!GetGreaterThanNextOut(m_dvPlayerCards[m_nLandlord],myCards.at(i),tmp))
				{
					nCount ++ ;
				}
			}
		}
	}
	nSameCount = max(nSameCount,nCount);
	return nCount;
}


TCardVector CRobot::FindOutAtLandG()
{
	return FindOutGeneral();
}

//�ж�ָ�������Ƿ�ӵ�о��Կ���Ȩ��
//���Կ���Ȩ������û������ѹ���Լ�
bool CRobot::HaveAbsoluteControl(CardsVector myCards)
{
	if (myCards.empty()) return false;
	int nNext = GetNextPlayerSeat(m_nLandlord);
	int nLast  = GetLastPlayerSeat(m_nLandlord);
	sort(myCards.begin(),myCards.end(),cmpCV);
	TCardVector Res;
	if (m_nOutSeat == m_nLandlord)  //�����Ķ���������
	{
		if (!GetGreaterThanNextOut(m_dvPlayerCards[nNext],myCards.front(),Res) &&
			!GetGreaterThanNextOut(m_dvPlayerCards[nLast],myCards.front(),Res))
		{
			return true;
		}else 
		{
			return false;
		}
	}else   //ũ��Ķ���ֻ��һ������
	{
		return !GetGreaterThanNextOut(m_dvBackupPlayerCards[m_nLandlord],myCards.front(),Res);
	}
	return false;
}
//���Ŵ���
bool CRobot::HaveAbsoluteControl(TCardVector myCards)
{
	TCardVector Res;
	if (!myCards.empty())
	{
		int nNext = GetNextPlayerSeat(m_nLandlord);
		int nLast  = GetLastPlayerSeat(m_nLandlord);
		TCardVector tmp;
		sort(myCards.begin(),myCards.end(),cmpTV);
		tmp.push_back(myCards.front());
		if (m_nOutSeat == m_nLandlord)  //�����Ķ���������
		{
			if (!GetGreaterThanNextOut(m_dvPlayerCards[nNext],tmp,Res) &&
				!GetGreaterThanNextOut(m_dvPlayerCards[nLast],tmp,Res))
			{
				return true;
			}else 
			{
				return false;
			}
		}else   //ũ��Ķ���ֻ��һ������
		{
			return !GetGreaterThanNextOut(m_dvBackupPlayerCards[m_nLandlord],tmp,Res);
		}
	}
	return false;
}
//ũ��ʣ��1���Ƶ�ʱ������ĳ��� 
//����ʣ��һ���ƣ����Ƶ����
/*
	1.�Լ�����ȫ���Ƴ���Ȩ����ô�س�����
	2.�Լ�������ȫ���Ƴ���Ȩ
*/
TCardVector CRobot::FindOutWhenFarmer1()
{
	TCardVector tmpOut = m_dvPlayerCards[m_nLandlord];
	TCardVector tmpRes;
	sort(tmpOut.begin(),tmpOut.end(),cmpTV);
	if (tmpOut.size() == 2)
	{
		if (m_dvSingles.empty())
		{
			tmpRes.push_back(tmpOut.front());
		}
	}
	return FastOut(true);
}


//Ϊ�ɻ��ҳ��
void CRobot::FindWing(TCardVector dvPlant,TCardVector dvSingle, CardsVector dvDoule)
{
	if (!dvPlant.empty())
	{
		if (!dvSingle.empty())
		{

		}
	}
}
//ũ��ʣ��2���Ƶ�ʱ������ĳ���
//ũ��ʣ�������Ƶ�ʱ����ܳ��ֵ����
/*
 1.��һ������
 2.����������
 3.��һ�Թ�---������ÿ����κ������ˣ���ȫû��
*/
TCardVector CRobot::FindOutWhenFarmer2()
{	
	int nNext = GetNextPlayerSeat(m_nLandlord);
	int nLast = GetLastPlayerSeat(m_nLandlord);
	TCardVector tmp,Res;
	CardTypeInfo cti1 = GameRule::GetCardType(m_dvPlayerCards[nNext]);
	CardTypeInfo cti2 = GameRule::GetCardType(m_dvPlayerCards[nLast]);
	if (cti1.nType == CT_DOUBLE || cti2.nType == CT_DOUBLE)  //����˫�Ƕ���
	{
		Res = FindOutGeneral();
		if (!Res.empty())
		{
			CardTypeInfo ctiRes = GameRule::GetCardType(Res);
			if (ctiRes.nType == CT_DOUBLE)
			{
				Res.pop_back();
				return Res;
			}else 
			{
				return Res;
			}
		}
	}else   //��һ�Ե���
	{
		//return FastOut(true);
		return FindOutGeneral();
	}
	return Res;
}

//��������Ϊ3��ʱ��ĳ���
//������Ϊ3��ʱ�򣬿��ܳ��ֵ����
/*
1.�����ƶ�ӵ�о��Կ���Ȩ�������Ӯ
2.��������������ӵ�о��Կ���Ȩ��������С����
3.����������һ����ӵ�о��Կ���Ȩ�������Ӯ������
   a)��������ƶ���ͬ����ô����м���
   b)�������������ͬ����ô�����ͬ����������ֵ��С��һ��
   c)��������ƶ�����ͬ������ӵ�п���Ȩ���ƺ󳣹����
4.û��һ������ӵ�п���Ȩ.һ�㶼���䣬���ܻ�Ӯ
*/
TCardVector CRobot::FindOutAtLand3()
{
//  	this->ShowDebug(m_dvThreeMultis,"��˳");
//  	this->ShowDebug(m_dvSingles,"����");
//  	this->ShowDebug(m_dvDoubles,"����");
	TCardVector tmp;
	tmp.clear();
	//�����Լ������ж���Ҫ���������
	int nCount =0;
	int nSameCount =0 ;
	nCount += CalcGreaterThanRivalCount(m_dvBooms, nSameCount);
	nCount += CalcGreaterThanRivalCount(m_dvThreeMultis,nSameCount);
	nCount += CalcGreaterThanRivalCount(m_dvDoubleMultis,nSameCount);
	nCount += CalcGreaterThanRivalCount(m_dvSingleMultis,nSameCount);
	nCount += CalcGreaterThanRivalCount(m_dvThrees,nSameCount);
	nCount += CalcGreaterThanRivalCount(m_dvDoubles,nSameCount);
	nCount += CalcGreaterThanRivalCount(m_dvSingles,nSameCount);
	nCount += m_dvMaxBoom.empty()?0:1;
	//Ѱ�ҳ���
	switch(nCount)
	{
	case 1:
		//������������ͬ��������ͬ������ڶ������
		if (nSameCount == 3)
		{
			if (!((tmp = FindMidCard(m_dvBooms)).empty()));
			else if (!((tmp = FindMidCard(m_dvThreeMultis)).empty()));
			else if (!((tmp = FindMidCard(m_dvDoubleMultis)).empty()));
			else if (!((tmp = FindMidCard(m_dvSingleMultis)).empty()));
			else if (!((tmp = FindMidCard(m_dvThrees)).empty()));
			else if (!((tmp = FindMidCard(m_dvDoubles)).empty()));
			//������Ҫ���⴦��
			else if (!m_dvSingles.empty())
			{
				tmp.push_back(m_dvSingles.at(1));
			}
		}else 
		{
			if (!m_dvBooms.empty() && !RivalHaveGreaterThanMe(m_dvBooms))
			{
				m_dvBooms.pop_back();
			}	
			if (!m_dvThreeMultis.empty() && !RivalHaveGreaterThanMe(m_dvThreeMultis))
			{
				m_dvThreeMultis.pop_back();
			}
			if (!m_dvDoubleMultis.empty() && !RivalHaveGreaterThanMe(m_dvDoubleMultis))
			{
				m_dvDoubleMultis.pop_back();
			}
			if (!m_dvSingleMultis.empty() && !RivalHaveGreaterThanMe(m_dvSingleMultis))
			{
				m_dvSingleMultis.pop_back();
			}
			if (!m_dvThrees.empty() && !RivalHaveGreaterThanMe(m_dvThrees))
			{
				m_dvThrees.pop_back();
			}
			if (!m_dvDoubles.empty() && !RivalHaveGreaterThanMe(m_dvDoubles))
			{
				m_dvDoubles.pop_back();
			}
			if (!m_dvSingles.empty() && !RivalHaveGreaterThanMe(m_dvSingles))
			{
				m_dvSingles.pop_back();
			}
			tmp = FindOutGeneral();
		}
		break;

	case 2:
		tmp = FindOutAtLevel2(); 
		break;

	default: 
		tmp= FastOut(true);
		break;
	}
	return tmp;
}
//��������Ϊ2��ʱ��ĳ���
TCardVector CRobot::FindOutAtLand2()
{
	TCardVector tmp;
	tmp.clear();
	int nNext = GetNextPlayerSeat(m_nOutSeat);
	int nLast  = GetLastPlayerSeat(m_nOutSeat);
	m_RivalSplit = m_PlayerSplit[nNext] + m_PlayerSplit[nLast];

	tmp.clear();
	if (!m_dvMaxBoom.empty())
	{
		TCardVector tmphand=m_dvPlayerCards[m_nOutSeat];
		if (GameRule::CanOut(false,tmphand,m_dvPlayerCards[GetNextPlayerSeat(m_nOutSeat)])||
			GameRule::CanOut(false,tmphand,m_dvPlayerCards[GetLastPlayerSeat(m_nOutSeat)])
			)
		{
			tmp = m_dvMaxBoom;
		}
		m_dvMaxBoom.clear();
		tmp = FastOut();
		return tmp;
	}
	
	if (!m_dvBooms.empty() && !RivalHaveGreaterThanMe(m_dvBooms))
	{
		if (m_dvBooms.size() == 2)
		{
			tmp = m_dvBooms.front();
		}else 
		{
			m_dvBooms.clear();
			tmp =  FastOut();
		}
		return tmp;
	}
	if (!m_dvThreeMultis.empty() && !RivalHaveGreaterThanMe(m_dvThreeMultis))
	{
		tmp = m_dvThreeMultis.back();
		int nNum = tmp.size()/3;
		if (m_dvSingles.size() >=nNum)
		{
			for (int i=0; i<nNum; ++i)
			{
				tmp.push_back(m_dvSingles.at(i));
			}
		}else if (m_dvDoubles.size() >= nNum)
		{
			for (int i=0; i<nNum; ++i)
			{
				tmp += m_dvDoubles.at(i);
			}
		}
		return tmp;
	}
	if (!m_dvDoubleMultis.empty() && !RivalHaveGreaterThanMe(m_dvDoubleMultis))
	{
		tmp = m_dvDoubleMultis.back();
		return tmp;
	}
	if (!m_dvSingleMultis.empty() && !RivalHaveGreaterThanMe(m_dvSingleMultis))
	{
		tmp = m_dvSingleMultis.back();
		return tmp;
	}
	if (!m_dvThrees.empty() && !RivalHaveGreaterThanMe(m_dvThrees))
	{
		tmp = m_dvThrees.back();//������
		if (!m_dvDoubles.empty())  //�ж���Ҳ���������
		{
			tmp += m_dvDoubles.front();
			return tmp; 
		}else if (!m_dvSingles.empty())   //����һ
		{
			tmp.push_back(m_dvSingles.front());
			return tmp;
		}
		return tmp;
	}
// 	int nNext  = GetNextPlayerSeat(m_nLandlord);
// 	int nLast   = 
// 	if (!m_dvDoubles.empty() && !RivalHaveGreaterThanMe(m_dvDoubles))
// 	{
// 		if (m_nLandlord == m_nOutSeat)
// 		{
// 		}
// 		tmp = m_dvDoubles.back();
// 		return tmp;
// 	}
// 	if (!m_dvSingles.empty() && !RivalHaveGreaterThanMe(m_dvSingles)) //������Ҫ��������
// 	{
// 		tmp.push_back(m_dvSingles.back());
// 		return tmp;
// 	}
	//���˵�Լ����е���û���ܹ�ѹ�����ֵģ���ôѡ����������������
	return FastOut();
}

//////////////////////////////////////////////////////////////////////////
//ũ������Ϊ2��ʱ��Ѱ�ҳ���
TCardVector CRobot::FindOutAtFarmer2()
{
	TCardVector tmp;
	tmp.clear();
	if (!m_dvMaxBoom.empty())
	{
		m_dvMaxBoom.clear();
		tmp = FastOut();
		return tmp;
	}

	if (!m_dvBooms.empty() && !RivalHaveGreaterThanMe(m_dvBooms))
	{
		tmp = m_dvBooms.front();
		return tmp;
	}
	if (!m_dvThreeMultis.empty() && !RivalHaveGreaterThanMe(m_dvThreeMultis))
	{
		tmp = m_dvThreeMultis.back();
		if (!m_dvSingles.empty())
		{
			tmp.push_back(m_dvSingles.front());
		}else if (!m_dvDoubles.empty())
		{
			tmp += m_dvDoubles.front();
		}
		return tmp;
	}
	if (!m_dvDoubleMultis.empty() && !RivalHaveGreaterThanMe(m_dvDoubleMultis))
	{
		tmp = m_dvDoubleMultis.back();
		return tmp;
	}
	if (!m_dvSingleMultis.empty() && !RivalHaveGreaterThanMe(m_dvSingleMultis))
	{
		tmp = m_dvSingleMultis.back();
		return tmp;
	}
	if (!m_dvThrees.empty() && !RivalHaveGreaterThanMe(m_dvThrees))
	{
		tmp = m_dvThrees.back();//������
		if (!m_dvDoubles.empty() && m_dvSingles.empty())  //�ж���Ҳ���������
		{
			tmp += m_dvDoubles.front();
			return tmp; 
		}else if (!m_dvSingles.empty() && m_dvDoubles.empty())   //����һ
		{
			tmp.push_back(m_dvSingles.front());
			return tmp;
		}else if (!m_dvSingles.empty() && !m_dvDoubles.empty())
		{
			int nPowerSingle = GameRule::CardWeight(m_dvSingles.front());
			int nPowerDoule = GameRule::CardWeight(m_dvDoubles.front().front());
			if (nPowerDoule > nPowerSingle) tmp.push_back(m_dvSingles.front());
			else  tmp += m_dvDoubles.front();
		}
		return tmp;
	}
	if (!m_dvDoubles.empty() && !RivalHaveGreaterThanMe(m_dvDoubles))
	{
		tmp = m_dvDoubles.back();
		return tmp;
	}
	if (!m_dvSingles.empty() && !RivalHaveGreaterThanMe(m_dvSingles)) //������Ҫ��������
	{
		tmp.push_back(m_dvSingles.back());
		return tmp;
	}
	//���˵�Լ����е���û���ܹ�ѹ�����ֵģ���ôѡ����������������
	return FastOut();
}

//////////////////////////////////////////////////////////////////////////
//ũ������Ϊ3��ʱ��Ѱ�ҳ���
TCardVector CRobot::FindOutAtFarmer3()
{
	return FindOutAtLand3();
}
//////////////////////////////////////////////////////////////////////////
//ũ�����������϶��ʱ��ĳ���
TCardVector CRobot::FindOutAtFarmerG()
{
	TCardVector tmp;
	tmp.clear();
	int nSnipeSeat = GetLastPlayerSeat(m_nLandlord); //�ѻ�λ�����ӵ�������
	int nFireSeat    = GetNextPlayerSeat(m_nLandlord); //����λ��һ�㸺�����
	//���ũ���Ǿѻ��ֵĳ���
	//�ѻ�λ�����Ӧ�������ž���������
	if (m_nOutSeat = nSnipeSeat )
	{
		tmp =  FindOutGeneral();
		TCardVector dvLandSingle; //��������
		CardsVector  dvLandDoule;//��������
		CardTypeInfo cti = GameRule::GetCardType(tmp);
		int nLevelOut = cti.nLevel;
		switch(cti.nType)
		{
		case CT_SINGLE:  //��������õ���������С����
			break;

		case CT_DOUBLE:
			break;
			
		default:
			break;
		}

	}else  //����λ�ĳ���
	{
		return FindOutGeneral();
	}
	return tmp;
}
//����ʣ��һ���Ƶ�ʱ��ĳ���
TCardVector	CRobot::FindOutWhenLand1()
{
	TCardVector tmpOut = m_dvPlayerCards[m_nLandlord];
	TCardVector tmpRes;
	sort(tmpOut.begin(),tmpOut.end(),cmpTV);
	if (tmpOut.size() == 2)
	{
		if (m_dvSingles.empty())
		{
			tmpRes.push_back(tmpOut.front());
		}
	}
	return FastOut(true);
}
//����ʣ�������Ƶ�ʱ��ĳ���
TCardVector CRobot::FindOutWhenLand2()
{
	TCardVector tmp,Res;
	CardTypeInfo cti = GameRule::GetCardType(m_dvPlayerCards[m_nLandlord]);
	if (cti.nType == CT_DOUBLE)  //����˫�Ƕ���
	{
		Res = FindOutGeneral();
		if (!Res.empty())
		{
			CardTypeInfo ctiRes = GameRule::GetCardType(Res);
			if (ctiRes.nType == CT_DOUBLE)
			{
				Res.pop_back();
				return Res;
			}
			else 
			{
				return Res;
			}
		}
	}else   //��һ�Ե���
	{
		return FastOut(false);
	}
	return Res;
}

//���Լһ������ֵ�ʱ��ĳ���
TCardVector CRobot::FindOutAtLevel3()
{
	TCardVector tmp;
	tmp.clear();
	//�����Լ�����ӵ����ȫ����Ȩ������
	int nCount  = 0;
	if (m_nOutSeat  == m_nLandlord) //����
	{
		for (int i=0; i<MAX_PLAYER; i++)
		{
			if ( i != m_nOutSeat)
			{
//				nCount =  CalcGreaterThanRivalCount();
			}
		}
	}

	return tmp;
}

TCardVector CRobot::FindMidCard(CardsVector myCards)
{
	TCardVector tmp;
	tmp.clear();
	if (!myCards.empty())
	{
		tmp = myCards.at(1);
		if (myCards.front().size() == 3) //������Ҫ�����Ѷ��ӻ��ߵ��Ŵ���ȥ
		{
			if (!m_dvSingles.empty())
			{
				tmp.push_back(m_dvSingles.front());
			}else if (!m_dvDoubles.empty())
			{
				tmp += m_dvDoubles.front();
			}
		}
	}
	return tmp;
}

TCardVector CRobot::FindOutAtLevel2 ()
{
	m_nSuggestOutTime = 3;
	TCardVector tmp;
	tmp.clear();
	if (!m_dvBooms.empty() && !RivalHaveGreaterThanMe(m_dvBooms))
	{
		tmp = m_dvBooms.back();
		return tmp;
	}
	if (!m_dvThreeMultis.empty() && !RivalHaveGreaterThanMe(m_dvThreeMultis))
	{
		tmp = m_dvThreeMultis.back();
		int nSize = tmp.size()/3;
		if (!m_dvSingles.empty() || !m_dvDoubles.empty())
		{
			if (m_dvSingles.size() >= nSize)
			{
				sort(m_dvSingles.begin(),m_dvSingles.end(),cmpTV);
				tmp.insert(tmp.end(),m_dvSingles.begin(),m_dvSingles.begin()+nSize);
			}else if (m_dvDoubles.size() >=nSize)
			{
				sort(m_dvDoubles.begin(),m_dvDoubles.end(),cmpCV);
				for (int i=0; i<nSize; ++i)
				{
					tmp += m_dvDoubles.at(i);
				}
			}else if ((m_dvSingles.size() + m_dvDoubles.size()*2) >=nSize)
			{
				TCardVector tmpS = m_dvSingles;
				for (int i=0; i<m_dvDoubles.size(); ++i)
				{
					tmpS += m_dvDoubles.at(i);
				}
				sort(tmpS.begin(),tmpS.end(),cmpTV);
				tmp.insert(tmp.end(),tmpS.begin(),tmpS.begin()+nSize);
			}
		}
		return tmp;
	}
	if (!m_dvDoubleMultis.empty() && !RivalHaveGreaterThanMe(m_dvDoubleMultis))
	{
		tmp = m_dvDoubleMultis.back();
		return tmp;
	}
	if (!m_dvSingleMultis.empty() && !RivalHaveGreaterThanMe(m_dvSingleMultis))
	{
		tmp = m_dvSingleMultis.back();
		return tmp;
	}
	if (!m_dvThrees.empty() && !RivalHaveGreaterThanMe(m_dvThrees))
	{
		tmp = m_dvThrees.back();
		if (!m_dvDoubles.empty() )  //�ж���Ҳ���������
		{
			tmp += m_dvDoubles.front();
		}else if (!m_dvSingles.empty())
		{
			tmp += m_dvSingles.front();
		}
		return tmp;
	}
	if (!m_dvDoubles.empty() && !RivalHaveGreaterThanMe(m_dvDoubles))
	{
		tmp = m_dvDoubles.back();
		return tmp;
	}
	if (!m_dvSingles.empty()) //������Ҫ��������
	{
		TCardVector mysingleCards;
		sort(m_dvSingles.begin(),m_dvSingles.end(),cmpTV);
		mysingleCards.push_back(m_dvSingles.back());
		if (m_nLandlord == m_nOutSeat) //����
		{
			for (int i=0; i<MAX_PLAYER;i++)
			{
				if (i != m_nOutSeat && GetGreaterThanNextOut(m_dvBackupPlayerCards[i],mysingleCards,tmp))
				{
					tmp.clear();
					tmp = mysingleCards;
				}else if (i!=m_nOutSeat)
				{
					tmp.clear();
				}
			}
			if (!tmp.empty())
			{
				return tmp;
			}
		}else  //ũ��
		{
			if (!GetGreaterThanNextOut(m_dvBackupPlayerCards[m_nLandlord],mysingleCards,tmp))
			{
				tmp.clear();
				tmp = mysingleCards;
				return tmp;
			}
		}
	
	}
	//���˵�Լ����е���û�о��Կ���Ȩ����ô�������ƺ���
	return FindOutGeneral();
}

//Ӯ��Ƚ�С��ʱ������
TCardVector CRobot::FastOut(bool bMaxToMin)
{
	TCardVector tmp;
	tmp.clear();
	//ѡ�����Ŵ����ǵ��Ż��Ƕ��ӣ�
	bool bChoseSingle = true; 
	//����������п���Ȩ�����Ƕ���û�У���ô�����ӣ����������
	if (RivalHaveGreaterThanMe(m_dvDoubles)&&
		!RivalHaveGreaterThanMe(m_dvSingles))
	{
		bChoseSingle = false;
	}
	//��˳
	if (1==FindOutUnthink(tmp,m_dvThreeMultis))
	{
		// Ϊ��˳�ҵ����Ӵ�ɷɻ����������
		int nThreeLen = tmp.size()/3; 
		if (bChoseSingle)
		{
			if (m_dvSingles.size() >=nThreeLen)  //����˳���ϵ���
			{
				tmp.insert(tmp.end(),m_dvSingles.begin(),m_dvSingles.begin()+nThreeLen);
				return tmp;
			}
			if (m_dvDoubles.size() >= nThreeLen) //������
			{
				for (int i=0; i<nThreeLen; i++)
				{
					tmp += m_dvDoubles.at(i);
				}
				return tmp;
			}
		}else 
		{
			if (m_dvDoubles.size() >= nThreeLen) //������
			{
				for (int i=0; i<nThreeLen; i++)
				{
					tmp += m_dvDoubles.at(i);
				}
				return tmp;
			}
			if (m_dvSingles.size() >=nThreeLen)  //����˳���ϵ���
			{
				tmp.insert(tmp.end(),m_dvSingles.begin(),m_dvSingles.begin()+nThreeLen);
				return tmp;
			}
		}
		return tmp;
	}
	//��˳
	if (1==FindOutUnthink(tmp,m_dvSingleMultis))
	{
		return tmp;
	}
	//˫˳
	if (1==FindOutUnthink(tmp,m_dvDoubleMultis))
	{
		return tmp;
	}

	//������
	if (1 == FindOutUnthink(tmp,m_dvThrees))
	{
		tmp = m_dvThrees.front();
		if (bChoseSingle)
		{
			if (!m_dvSingles.empty() /*&& m_dvSingles.front() > CV_2 && m_dvSingles.front() < CV_K*/ )  
			{
				tmp.push_back(m_dvSingles.front());
				return tmp;
			}
			if 	(!m_dvDoubles.empty()/* && m_dvDoubles.front().front().GetValue() > CV_2 && m_dvDoubles.front().front().GetValue() < CV_K*/)
			{
				tmp += m_dvDoubles.front();
				return tmp;
			}
		}else 
		{
			if 	(!m_dvDoubles.empty() /*&& m_dvDoubles.front().front().GetValue() > CV_2 && m_dvDoubles.front().front().GetValue() < CV_K*/)
			{
				tmp += m_dvDoubles.front();
				return tmp;
			}
			if (!m_dvSingles.empty() /*&& m_dvSingles.front() > CV_2 && m_dvSingles.front() < CV_K */)  
			{
				tmp.push_back(m_dvSingles.front());
				return tmp;
			}
		}
		return tmp;
	}
	//���� 
	if (1 == FindOutUnthink(tmp,m_dvThrees))
	{
		return tmp;
	}

	//����
	if (1 == FindOutUnthink(tmp,m_dvDoubles))
	{
		return tmp;
	}
	
	//����
	if (!m_dvSingles.empty()&&!bMaxToMin)
	{
		tmp.push_back(m_dvSingles.front());
	}else if (!m_dvSingles.empty()  && m_dvSingles.size() >=2 && bMaxToMin)
	{
		tmp.push_back(m_dvSingles.at(1));
	}

	//ը������������ը����鿴ը����Լ��Ƿ���Ӯ��Ӯ��ը���������Ĵ������
	if (m_nLevel[m_nOutSeat]-m_dvBooms.size()-m_dvMaxBoom.size()/2 <= 1)
	{
		if (!m_dvBooms.empty())
		{
			tmp = m_dvBooms.front();
		}else if (!m_dvMaxBoom.empty())
		{
			tmp = m_dvMaxBoom;
		}
		return tmp;
	}else if (m_dvSingles.size() >=2 && !m_dvBooms.empty())
	{
		tmp = m_dvBooms.front();
		tmp += m_dvSingles.at(0);
		tmp += m_dvSingles.at(1);
	}
	return tmp;
}

int CRobot::GetNextPlayerSeat(int nSeat)
{
	return (nSeat+1)%MAX_PLAYER;
}

int CRobot::GetLastPlayerSeat(int nSeat)
{
	return (MAX_PLAYER+nSeat-1)%MAX_PLAYER;
}

TCardVector CRobot::FindTeammateCanOut()
{
	CardsVector tmp ;
	tmp.clear();
	return FastOut(true);
}
//Ѱ�ҵ�ǰӦ���������
//���أ� �ҵ���Ҫ������ƺ���з��أ��ú���������ʧ��
/*
	����˳��1.��˳
	          2.˫˳
			  3.˳��
			  4.������
			  5.����
			  6.����
			  7.����
*/
//������͵�������׳��ư汾
TCardVector CRobot::FindNowOut()
{
	TCardVector tmp;
	TCardVector UnUser;
	int nNext =GetNextPlayerSeat(m_nOutSeat); //�¼�
	int nLast = GetLastPlayerSeat(m_nOutSeat); //�ϼ�
	if (m_nOutSeat == m_nLandlord)  //���Ƶ�����ǵ���
	{
		//���ֿ�����˵�ʱ�����ȼ�Ӧ�����
		if (m_Warring[nNext].nWL== 1 || m_Warring[nLast].nWL == 1) //ũ�񱨵�
		{
			m_nSuggestOutTime = 0;
			return FindOutWhenFarmer1();
	/*		return FastOut(true);*/
		}else if (m_Warring[nNext ].nWL == 2 || m_Warring[nLast].nWL ==2 ) //ũ��˫
		{
			m_nSuggestOutTime = 0;
			return FindOutWhenFarmer2();
		} 

		if (m_nLevel[m_nOutSeat]==2 )   //�Լ������л���������
		{
			m_nSuggestOutTime = 0;
			return FindOutAtLand2();
		}else  if (m_nLevel[m_nOutSeat] == 3 ) //�Լ����л���������
		{
			m_nSuggestOutTime = 0;
			return FindOutAtLand3();
		}else  
		{	
			m_nSuggestOutTime = 6;
			return FindOutGeneral();  //��������
		}
	}
	else   //���Ƶ������ũ��
	{
		//����¼����Լ��Ķ��ѣ�����ֻʣ��һ�����ˣ��ҵ�һ���ܹ����¼�Ҫ�ϵ��ƴ��������Լ�������ը��,�ȳ�ը��
		if (GetNextPlayerSeat(m_nOutSeat) != m_nLandlord)
		{
			CardTypeInfo cti = GameRule::GetCardType(m_dvPlayerCards[GetNextPlayerSeat(m_nOutSeat)]);
			if (cti.nType > CT_NONE && cti.nType < CT_COUNT)
			{
				m_nSuggestOutTime = 0;
				TCardVector tmpDisbale;
				if (cti.nType == CT_SINGLE)
				{
					if (!m_dvBooms.empty())
					{
						if (!m_dvMaxBoom.empty())
						{
							return m_dvBooms.front();
						}else if (!GetGreaterThanNextOut(m_dvPlayerCards[m_nLandlord],m_dvBooms.back(),tmpDisbale))
						{
							return m_dvBooms.front();
						}
					}
					if (!m_dvMaxBoom.empty())
					{
						return m_dvMaxBoom;
					}
					//�����һ����С�ĵ���
					sort(m_dvPlayerCards[m_nOutSeat].begin(),m_dvPlayerCards[m_nOutSeat].end(),cmpTV);
					tmpDisbale.push_back(m_dvPlayerCards[m_nOutSeat].front());
					if (CmpSingle(m_dvPlayerCards[GetNextPlayerSeat(m_nOutSeat)].front(),tmpDisbale.front()))
					{
						return tmpDisbale;
					}
					//return  tmpDisbale;
				}else if (cti.nType == CT_DOUBLE)
				{
					CardsVector tmpDoubles;
					FillCards(2,CT_DOUBLE,tmpDoubles,m_dvPlayerCards[m_nOutSeat]);
					sort(tmpDoubles.begin(),tmpDoubles.end(),cmpCV);
					if (!tmpDoubles.empty())
					{
						return tmpDoubles.front();
					}
				}
			}
		}
		
		//���ȣ����ȿ��Ǳ��ö��ֹ���
		switch (m_Warring[m_nLandlord].nWL)  
		{
		case 1: 
			m_nSuggestOutTime = 0;
			return FindOutWhenLand1();   
			break;

		case 2:
			m_nSuggestOutTime = 0;
			return FindOutWhenLand2();
			break;

		default: 
			break;
		}
		//�Լ�������С�ڵ���3
		if (m_nLevel[m_nOutSeat]  ==2) 
		{
			m_nSuggestOutTime = 0;
			return FindOutAtFarmer2();
		}else  if (m_nLevel[m_nOutSeat] == 3)
		{
			m_nSuggestOutTime = 0;
			return FindOutAtFarmer3();
		}else  
		{	
			m_nSuggestOutTime = 5;
			return FindOutGeneral();
		}
	}
	//һ�����
	m_nSuggestOutTime = 5;
	return  FindOutGeneral();	
}

//�����¼ҵĳ���
TCardVector CRobot::FindOutFarmerFire()
{
	TCardVector tmp;
	//�����ƿ�����ʱ��Ӧ�����ȿ���
	switch (m_Warring[m_nLandlord].nWL )  
	{
	case 1: 
		return FindOutWhenLand1();   
		break;

	case 2:
		return FindOutWhenLand2();
		break;

	default: 
		break;
	}
	//�Լ�������С�ڵ���3
	if (m_nLevel[m_nOutSeat]  ==2) 
	{
		return FindOutAtFarmer2();
	}else  if (m_nLevel[m_nOutSeat] == 3)
	{
		return FindOutAtFarmer3();
	}else  
	{	
		return FindOutAtFarmerG();
	}
	return tmp;
}

//�����ϼҵĳ���
TCardVector CRobot::FindOutFarmerSnipe()
{
	//����Լ��ܹ�ֱ�ӳ��꣬��ô�ͳ�
	//�����Լ������ж���Ҫ���������
	int nCount =0;
	int nSameCount =0 ;
	nCount += CalcGreaterThanRivalCount(m_dvBooms, nSameCount);
	nCount += CalcGreaterThanRivalCount(m_dvThreeMultis,nSameCount);
	nCount += CalcGreaterThanRivalCount(m_dvDoubleMultis,nSameCount);
	nCount += CalcGreaterThanRivalCount(m_dvSingleMultis,nSameCount);
	nCount += CalcGreaterThanRivalCount(m_dvThrees,nSameCount);
	nCount += CalcGreaterThanRivalCount(m_dvDoubles,nSameCount);
	nCount += CalcGreaterThanRivalCount(m_dvSingles,nSameCount);

	switch (m_Warring[m_nLandlord].nWL)  
	{
	case 1: 
		return FindOutWhenLand1();   
		break;

	case 2:
		return FindOutWhenLand2();
		break;

	default: 
		break;
	}
	//�Լ�������С�ڵ���3
	if (m_nLevel[m_nOutSeat]  ==2) 
	{
		return FindOutAtFarmer2();
	}else  if (m_nLevel[m_nOutSeat] == 3)
	{
		return FindOutAtFarmer3();
	}else  
	{	
		return FindOutAtFarmerG();
	}
}

//////////////////////////////////////////////////////////////////////////
void CRobot::FindFamily(TCardVector dvOrphan)
{
//	m_dvSingles += dvOrphan;
}

//////////////////////////////////////////////////////////////////////////
//��������
void CRobot::ErgodicCards(bool bBoom,bool bThree, bool bDouble, bool bThreeMulti,bool bDouleMulti,bool bMulti, int nSeat)
{
	this->ResetTmp();
	TCardVector tmphandCards = m_dvPlayerCards[nSeat];
	 
	if (bBoom) //ը��
	{
		this->ChoseBoom(cvTmpBoom,tmphandCards);
	}
	if (bThree) //����
	{
		this->ChoseThree(cvTmpThree,tmphandCards);
	}
	if (bDouble) //����
	{
		this->ChoseDouble(cvTmpDouble,tmphandCards);
	}
	if (bThreeMulti)//��˳
	{
		this->ChoseThreeMultis(cvTmpThreeMulti,cvTmpThree);
	}
	if (bDouleMulti) //˫˳
	{
		this->ChoseDoubleMultis(cvTmpDoubleMulti,cvTmpDouble);
	}
	if (bMulti) //��˳
	{
		this->ChoseSingleMultis(cvTmpSingleMulti,tmphandCards,cvTmpDoubleMulti);
	}
	//�����������
	this->ChoseBoom(cvTmpBoom,tmphandCards);
	this->ChoseThree(cvTmpThree,tmphandCards);
	this->ChoseDouble(cvTmpDouble,tmphandCards);
	this->ChoseDoubleMultis(cvTmpDoubleMulti,cvTmpDouble);
	this->ChoseThreeMultis(cvTmpThreeMulti,cvTmpThree);
	this->ChoseSingleMultis(cvTmpSingleMulti,tmphandCards,cvTmpDoubleMulti);
	cvTmpSingle += tmphandCards;
	
	//�����������
	if (cvTmpJoker.size() == 1)  //����
	{
		cvTmpSingle += cvTmpJoker;
	}else if (cvTmpJoker.size() == 2) //��ը
	{
		cvTmpMaxBoom = cvTmpJoker;
	}
	
	//����һ�β��Ƶĵ������������Ƶ�Ȩֵ���
	SplitInfo tmpSI;
	CT tmpCT;
	ConvertCVToCT(cvTmpBoom,tmpCT,CT_BOOM);
	tmpSI.s_boom = tmpCT;
	tmpCT.clear();
	ConvertCVToCT(cvTmpThree,tmpCT,CT_THREE);
	tmpSI.s_three = tmpCT;
	tmpCT.clear();
	ConvertCVToCT(cvTmpDouble,tmpCT,CT_DOUBLE);
	tmpSI.s_doule = tmpCT;
	tmpCT.clear();
	ConvertCVToCT(cvTmpDoubleMulti,tmpCT,CT_MULTIDOUBLETWO);
	tmpSI.s_doubleMulti = tmpCT;
	tmpCT.clear();
	ConvertCVToCT(cvTmpThreeMulti,tmpCT,CT_MULTITHREE);
	tmpSI.s_threeMulti = tmpCT;
	tmpCT.clear();
	ConvertCVToCT(cvTmpSingleMulti,tmpCT,CT_MULTI);
	tmpSI.s_singleMulti = tmpCT;
	tmpCT.clear();
	//���
	CARDSTYLE tmp;
	tmp.Reset();
	if (!cvTmpMaxBoom.empty())
	{
		tmp.max = CV_BJ;
		tmp.min  = CV_SJ;
		tmp.cards = cvTmpMaxBoom;
		tmp.m_value = nLevelBoom;
		tmpSI.s_maxBoom.push_back(tmp);
	}
	//���ŵ�Ȩֵ
	for (int i=0; i<cvTmpSingle.size();i++)
	{
		tmp.Reset();
		tmp.max = cvTmpSingle.front().GetValue();
		tmp.min = tmp.max;
		tmp.cards.push_back(cvTmpSingle.at(i));
		tmp.m_value = nLevelSingle;
		tmpSI.s_single.push_back(tmp);
	}
	tmpSI.s_nSeat = nSeat;
	//����һ�εĲ�����Ϣ�����������Ϣ������
	m_allSplitInfo[nSeat].push_back(tmpSI);
}

//Ϊ���ź���˳Ԥ���Ƚ�С�ĵ��Ż��߶���
void CRobot::ReservedSmallCards(bool bChoseSingle)
{
	int nLen = m_dvThrees.size();
	for (int i=0; i<m_dvThreeMultis.size();i++)
	{
		nLen += m_dvThreeMultis.at(i).size()/3;
	}
	if (nLen > 0)
	{
		bool bFlagReserved  = false; //�Ƿ��Ѿ�Ԥ������
		if (bChoseSingle)  //����ѡ��������
		{
			if (m_dvSingles.size() >= nLen) //������
			{
				if (m_dvSingles.at(nLen-1).GetValue() < CV_A && 
					m_dvSingles.at(nLen-1).GetValue() != CV_2)
				{
					m_dvSingles.erase(m_dvSingles.begin(),m_dvSingles.begin()+nLen);
					bFlagReserved = true;
				}
			}
			if (!bFlagReserved && m_dvDoubles.size() >= nLen) //������
			{
				if (m_dvDoubles.at(nLen-1).front().GetValue() < CV_A && 
					m_dvDoubles.at(nLen-1).front().GetValue()  != CV_2)
				{
					m_dvDoubles.erase(m_dvDoubles.begin(),m_dvDoubles.begin()+nLen);
				}
			}
		}else  //����ѡ�������� 
		{
			if (m_dvDoubles.size() >= nLen) //������
			{
				if (m_dvDoubles.at(nLen-1).front().GetValue() < CV_A && 
					m_dvDoubles.at(nLen-1).front().GetValue()  != CV_2)
				{
					m_dvDoubles.erase(m_dvDoubles.begin(),m_dvDoubles.begin()+nLen);
					bFlagReserved = true;
				}
			}

			if (!bFlagReserved && m_dvSingles.size() >= nLen) //������
			{
				if (m_dvSingles.at(nLen-1).GetValue() < CV_A && 
					m_dvSingles.at(nLen-1).GetValue() != CV_2)
				{
					m_dvSingles.erase(m_dvSingles.begin(),m_dvSingles.begin()+nLen);
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//��Ѱ�ܹ�ѹ������
//OutCard ָ����Ҫѹ���ƣ�����fromCardsָ����Ҫ���Ķ���ѹ�ƣ�����������
//�ҵ���ѹ��
//����ɹ���fromCards���ҵ���ѹ��OutCard���ƣ���ô����true������FollowCards����
//�ҵ���ѹ�ƣ����fromCards��û�������ܹ�ѹ��OutCard����ô����false���ҽ�FollowCards
//���
bool CRobot::FindFollowCards(TCardVector OutCard,CardsVector fromCards,TCardVector& FollowCards, int nLevel)
{
	if (OutCard.empty() ) return false;
	if (!fromCards.empty())    //
	{
		for (int nIndex =0; nIndex < fromCards.size(); nIndex++) //����ܹ������ŷ������ó����ó��Ƹ�
		{
			if (GameRule::CanOut(false,OutCard,fromCards.at(nIndex))) 
			{
				FollowCards = fromCards.at(nIndex);
				return true;
			}   
		}
		//�����ŷ���������������ͬ�������в����Ƿ��д�ù�����
		if (nLevel < 2 && nLevel >0)
		{
			for (int nIndex =0; nIndex < fromCards.size(); nIndex++)
			{
				if (GetGreaterThanNextOut(fromCards.at(nIndex),OutCard,FollowCards))
				{
					return true;
				}
			}
		}
		//��ȫ�����в����ܷ�ѹ������
		//��ȫ�������в��ҳ��Ƶ�ʱ�򣬿��Ǿ������ƻ�����
		if (nLevel == 2)
		{
			switch(OutCard.size())
			{
			case 2:  
				for (int i=0; i<m_dvThrees.size(); i++)
				{
					if (GetGreaterThanNextOut(m_dvThrees.at(i),OutCard,FollowCards))
					{
						return true;
					}
				}
				for (int i=0; i<m_dvThreeMultis.size(); i++)
				{
					if (GetGreaterThanNextOut(m_dvThreeMultis.at(i),OutCard,FollowCards))
					{
						return true;
					}
				}
				if (GetGreaterThanNextOut(m_dvOldCards,OutCard,FollowCards))
				{
					return true;
				}
				break;

			default:
				if (GetGreaterThanNextOut(m_dvOldCards,OutCard,FollowCards))
				{
					return true;
				}
				break;
			}

		}
	}else  if (nLevel == 2)//��ȫ�����в����ܷ�ѹ������
	{
		if (GetGreaterThanNextOut(m_dvOldCards,OutCard,FollowCards))
		{
			return true;
		}
	}

	FollowCards.clear();
	return false;
}

//�������,ֻ�����ŷ�������ȡ����
bool CRobot::FindFollowGeneral(TCardVector dvOut, TCardVector& dvRes, int nLevel)
{
	if (dvOut.empty())
	{
		return false;
	}
	dvRes.clear();
	int nIndex =0;
	CardTypeInfo ctiOT  = GameRule::GetCardType(dvOut);
	TCardVector tmpCards;
	int nOutValue;
	tmpCards.clear();
	CardsVector tmpCV;
	TCardVector tmpt;

	//�������
	switch (ctiOT.nType)
	{
	case CT_MAXBOOM:
		m_nSuggestOutTime = 1;
		return false;
		break;

	case CT_BOOM:  //ը��
		FindFollowCards(dvOut,m_dvBooms,dvRes,nLevel);
		if (!dvRes.empty()) m_nSuggestOutTime = 5;
		else m_nSuggestOutTime = 1;
		break;

	case CT_SINGLE: //����
		for (int i=0; i<m_dvSingles.size(); i++)
		{
			if (CmpSingle(m_dvSingles.at(i),dvOut.front()))
			{
				dvRes.push_back(m_dvSingles.at(i));
				return true;
			}
		}	
		if (nLevel < 2 && nLevel >=0 )  //һ���ڶ���Ķ���,��������
		{
			sort(m_dvDoubles.begin(),m_dvDoubles.end(),cmpCV);
			if (m_dvDoubles.size()>=2 &&CmpSingle((m_dvDoubles.end()-2)->front(),dvOut.front()))
			{
				dvRes.push_back((m_dvDoubles.end()-2)->front());
				return true;
			}else if (!m_dvDoubles.empty() && CmpSingle(m_dvDoubles.back().front(),dvOut.front()))
			{
				dvRes.push_back(m_dvDoubles.back().front());
				return true;
			}
			sort(m_dvThrees.begin(),m_dvThrees.end(),cmpCV);
			if (m_dvThrees.size()>=2 && CmpSingle((m_dvThrees.end()-2)->front(),dvOut.front()))
			{
				dvRes.push_back((m_dvThrees.end()-2)->front());
				return true;
			}
			else if (!m_dvThrees.empty() && CmpSingle(m_dvThrees.back().front(),dvOut.front()))
			{
				dvRes.push_back(m_dvThrees.back().front());
				return true;
			}
		}
		if (nLevel ==2)
		{
			if (GetGreaterThanNextOut(m_dvPlayerCards[m_nOutSeat],dvOut,dvRes))
			{
				return true;
			}
		}
		if (!dvRes.empty()) m_nSuggestOutTime = 3;
		else m_nSuggestOutTime = 1;
		break;
	case CT_DOUBLE: //����
		FindFollowCards(dvOut,m_dvDoubles,dvRes,nLevel);
		if (dvRes.empty()) m_nSuggestOutTime = 1;
		else m_nSuggestOutTime = 3;
		break;

	case CT_THREE:  //����
		FindFollowCards(dvOut,m_dvThrees,dvRes,nLevel);
		m_nSuggestOutTime = 3;
		break;

	case CT_MULTI: //˳��
		FindFollowCards(dvOut,m_dvSingleMultis,dvRes,nLevel);
		if (dvRes.empty()) m_nSuggestOutTime = 3;
		else m_nSuggestOutTime = 8;
		break;

	case CT_MULTIDOUBLETWO:  //˫˳
		FindFollowCards(dvOut,m_dvDoubleMultis,dvRes,nLevel);
		if (dvRes.empty()) m_nSuggestOutTime = 3;
		else m_nSuggestOutTime = 8;
		break;

	case CT_MULTITHREE:  //��˳
		FindFollowCards(dvOut,m_dvThreeMultis,dvRes,nLevel);
		if (dvRes.empty()) m_nSuggestOutTime = 3;
		else m_nSuggestOutTime = 8;
		break;

	case CT_THREEWITHONE://����һ
		//���ȷ�������һ�е����ź�һ��
		SeparationCards(dvOut,tmpCV);
		FindFollowCards(tmpCV.front(),m_dvThrees,dvRes,nLevel);
		
		if (!dvRes.empty() && !m_dvSingles.empty())
		{
			if ( nLevel <2 &&m_dvSingles.front().GetValue() <=CV_A && m_dvSingles.front().GetValue() !=CV_2)
			{
				dvRes.push_back(m_dvSingles.front());
			}
			else if ( nLevel == 2 )
			{
				m_dvSingles-= dvRes;
				if (!m_dvSingles.empty())	dvRes.push_back(m_dvSingles.front());
			}
		}
		if (dvRes.empty()) m_nSuggestOutTime = 2;
		else m_nSuggestOutTime = 7;
		break;

	case  CT_THREEWITHTWO: //������
		SeparationCards(dvOut,tmpCV);
		FindFollowCards(tmpCV.front(),m_dvThrees,dvRes,nLevel);
		if (!dvRes.empty() && !m_dvDoubles.empty())
		{
			if (!m_dvDoubles.empty() && nLevel<2 &&m_dvDoubles.front().front().GetValue() <=CV_A &&
				  m_dvDoubles.front().front().GetValue() !=CV_2)
			{
				dvRes += m_dvDoubles.front();
			}else if (nLevel == 2 )
			{
				DelCV(m_dvDoubles,dvRes);
				if (!m_dvDoubles.empty())
				{
					dvRes+= m_dvDoubles.front();
				}
			}
		}
		if (dvRes.empty()) m_nSuggestOutTime = 3;
		else m_nSuggestOutTime = 8;
		break;

	case CT_MULTITHREEWITHONE:  //�ɻ������ ���Ϊ��
		SeparationCards(dvOut,tmpCV);
		tmpt.clear();
		for (int i=0; i<tmpCV.size(); i++)
		{
			tmpt += tmpCV.at(i);
		}
		FindFollowCards(tmpt,m_dvThreeMultis,dvRes,nLevel);
		//�ɻ��Ҵ���
		if (!dvRes.empty() && m_dvSingles.size() >=tmpCV.size()&& nLevel < 2 && 
			m_dvSingles.at(tmpCV.size()-1).GetValue() < CV_A && m_dvSingles.at(tmpCV.size()-1).GetValue() !=CV_2) //�Ҵ���
		{
			for (int i=0; i<tmpCV.size() ;i++)
			{
				dvRes.push_back(m_dvSingles.at(i));
			}
		}else if (nLevel == 2 && !dvRes.empty() && m_dvSingles.size() >= tmpCV.size())
		{
				m_dvSingles-=dvRes;
				if (m_dvSingles.size() >=tmpCV.size())
				{
					dvRes.insert(dvRes.end(),m_dvSingles.begin(),m_dvSingles.begin()+tmpCV.size());
				}else
				{
					m_dvOldCards -= dvRes;
					if (m_dvOldCards.size() >= tmpCV.size())
					{
						dvRes.insert(dvRes.end(),m_dvOldCards.begin(),m_dvOldCards.begin()+tmpCV.size());
					}
				}
		}
		if (dvRes.empty()) m_nSuggestOutTime = 3;
		else m_nSuggestOutTime = 8;
		break;

	case CT_MULTITHREEWITHTWO: //�ɻ�����򣬳��Ϊ˫
		SeparationCards(dvOut,tmpCV);
		tmpt.clear();
		for (int i=0; i<tmpCV.size(); i++)
		{
			tmpt += tmpCV.at(i);
		}
		FindFollowCards(tmpt,m_dvThreeMultis,dvRes,nLevel);
		//�ɻ��Ҵ���
		if (!dvRes.empty() && m_dvDoubles.size() >=tmpCV.size() && nLevel <2 && 
			m_dvDoubles.at(tmpCV.size()-1).front().GetValue() < CV_A && m_dvDoubles.at(tmpCV.size()-1).front().GetValue() !=CV_2) 
		{
			for (int i=0; i<tmpCV.size() ;i++)
			{
				dvRes += m_dvDoubles.at(i);
			}
		}else if (dvRes.empty() && m_dvDoubles.size()>=tmpCV.size() && nLevel == 2)
		{
			DelCV(m_dvDoubles,dvRes);
			if (m_dvDoubles.size() >=tmpCV.size())
			{
				for (int i=0; i<tmpCV.size() ;i++)
				{
					dvRes += m_dvDoubles.at(i);
				}
			}
		}
		if (dvRes.empty()) m_nSuggestOutTime = 3;
		else m_nSuggestOutTime = 8;
		break;

	default:
		m_nSuggestOutTime = 3;
		break;
	}
	if (!GameRule::CanOut(false,dvOut,dvRes))
	{
		dvRes.clear();
	}
	return !dvRes.empty();
}

bool CRobot::CmpSingle(TCards T1,TCards T2)
{
	int nValue1 = GameRule::CardWeight(T1.GetValue());
	int nValue2 = GameRule::CardWeight(T2.GetValue());
	return nValue1>nValue2?true:false;
}

//ƴ�����ƣ����ڲ����ƾͻ�����˵���������Ƚ�׽��
bool CRobot::BendOverBackwardsFollow(TCardVector& dvRes,TCardVector dvOut)
{
	return FindFollowGeneral(dvOut,dvRes);
}

//��������
bool CRobot::FindFollowLand(const TCardVector& OutCard, TCardVector& FollowCards)
{
	int nLastHandNum = m_PlayerSplit[GetLastPlayerSeat(m_nLandlord)].GetHandCount();
	int nNextHandNum = m_PlayerSplit[GetNextPlayerSeat(m_nLandlord)].GetHandCount();
	int nHandNum = nLastHandNum<nNextHandNum?nLastHandNum:nNextHandNum;
	int nWarringSeat = nHandNum == nLastHandNum?nLastHandNum:nNextHandNum;
	TCardVector tmpHands = m_dvPlayerCards[m_nOutSeat];

	if (nHandNum >3)
	{
		FindFollowGeneral(OutCard,FollowCards,1);
		if (!FollowCards.empty())  //����ƺܴ󣬳���Ҳ�Ǵ��ƾ�ѹ�����ǹ�
		{
			CardTypeInfo cti = GameRule::GetCardType(OutCard);
			if (cti.nType == CT_SINGLE ) 
			{
				m_nSuggestOutTime = 2;
				return true;
			}
			if (cti.nType ==  CT_MULTI || cti.nType == CT_MULTIDOUBLETWO || cti.nType == CT_MULTITHREE)
			{
				m_nSuggestOutTime = 3;
				return true;
			}
			sort(FollowCards.begin(),FollowCards.end(),cmpTV);
			TCardVector tmpOut = OutCard;
			TCardVector tmpFollow = FollowCards;
			sort(tmpOut.begin(),tmpOut.end(),cmpTV);
			if (cti.nType == CT_THREEWITHONE)
			{
				m_nSuggestOutTime = 5;
				if (tmpOut.front().GetValue() == tmpOut.at(2).GetValue())
				{
					tmpOut.pop_back();
				}
				if (tmpOut.at(1).GetValue() == tmpOut.back().GetValue())
				{
					tmpOut.erase(tmpOut.begin());
				}
				if (tmpFollow.front().GetValue() == tmpFollow.at(2).GetValue())
				{
					tmpFollow.pop_back();
				}
				if (tmpFollow.at(1).GetValue() == tmpFollow.back().GetValue())
				{
					tmpFollow.erase(tmpFollow.begin());
				}
			}
			if (cti.nType == CT_THREEWITHTWO)
			{
				m_nSuggestOutTime = 5;
				if (tmpOut.front().GetValue() == tmpOut.at(2).GetValue())
				{
					tmpOut.pop_back();
					tmpOut.pop_back();
				}
				if (tmpOut.at(1).GetValue() == tmpOut.back().GetValue())
				{
					tmpOut.erase(tmpOut.begin());
					tmpOut.erase(tmpOut.begin());
				}
				if (tmpFollow.front().GetValue() == tmpFollow.at(2).GetValue())
				{
					tmpFollow.pop_back();
					tmpFollow.pop_back();
				}
				if (tmpFollow.at(1).GetValue() == tmpFollow.back().GetValue())
				{
					tmpFollow.erase(tmpFollow.begin());
					tmpFollow.erase(tmpFollow.begin());
				}
			}
			
			if (cti.nType == CT_SINGLE || cti.nType == CT_DOUBLE || cti.nType == CT_THREE||
				cti.nType == CT_THREEWITHONE || CT_THREEWITHTWO)
			{
				if (CmpSingle(tmpFollow.front(),m_dvMaxThreeCards.back()) || tmpFollow.front().GetValue() == CV_2) //����ҵ��ĳ�����������������ֵ֮һ��
				{					
					int nOut = GameRule::CardWeight(tmpOut.front().GetValue());
					int nFollow = GameRule::CardWeight(tmpFollow.front().GetValue());
					if(nFollow-nOut ==1)  //�����㹻������ѹ��
					{
					}else
					{
						//������ƺ��Լ����ܹ�ȷ�����ƣ����������
						int nMyHandNum = m_PlayerSplit[m_nLandlord].GetHandCount();
						int nResidueCount = CalcAllNotGreateThanCards();
						if (nMyHandNum - nResidueCount <2);
						else
						{
							FollowCards.clear();
						}
					}
				}
			}
		}
	}
	else 
	{
		m_nSuggestOutTime = 6;
		if (nHandNum ==1&& GameRule::CanOut(false,OutCard,m_dvPlayerCards[nWarringSeat]))  //ũ��ֻʣ��һ������,���ҳ��ƺ�ʣ������ͬ
		{
			TCardVector tmpMyCards = m_dvPlayerCards[m_nOutSeat];
			int nNext =GetNextPlayerSeat(m_nLandlord);
			int nLast  = GetLastPlayerSeat(m_nLandlord);
			//int nSeat  = m_dvPlayerCards[nLast] < m_dvPlayerCards[nNext]?nLast:nNext;
			int nSeat;
			if (m_dvPlayerCards[nLast] < m_dvPlayerCards[nNext])
				nSeat = nLast;
			else 
				nSeat = nNext;

			TCardVector tmpLandCards = m_dvPlayerCards[nSeat];
			if (OutCard.size() == 1)
			{
				//������ĵ����ܹ�ѹ������ô�ͳ����ĵ���,���û�У���ô�����Ķ��ӣ�
				//�����û�У����������ţ������û�У���ը������û�У��������Ƶ㡣
				//����
				sort(m_dvSingles.begin(),m_dvSingles.end(),cmpMaxMin);
				if (!m_dvSingles.empty() &&CmpSingle(m_dvSingles.front(),tmpLandCards.front()) &&
					CmpSingle(m_dvSingles.front(),OutCard.front()))
				{
					FollowCards.push_back(m_dvSingles.front());
					m_nSuggestOutTime = 2;
					return  true;
				}
				//�����ȥը�����Լ��ܱ�����ѹ��������С��2��
				if (!m_dvBooms.empty())
				{
					int nMaxNum = CalcAllNotGreateThanCards();
					int nMyCount =  m_PlayerSplit[m_nOutSeat].GetHandCount();
					if (nMyCount - nMaxNum  <=1)
					{
						sort(m_dvBooms.begin(),m_dvBooms.end(),cmpCV);
						for (int b=0; b<m_dvBooms.size(); ++b)
						{
							if (GameRule::CanOut(false,OutCard,m_dvBooms.at(b)) &&
								GameRule::CanOut(false,tmpLandCards,m_dvBooms.at(b)))
							{
								FollowCards = m_dvBooms.at(b);
								m_nSuggestOutTime = 5;
								return true;
							}
						}
					}
				}
				if (!m_dvMaxBoom.empty())
				{
					int nMaxNum = CalcAllNotGreateThanCards();
					int nMyCount =  m_PlayerSplit[m_nOutSeat].GetHandCount();
					if (nMyCount - nMaxNum  <=1 )
					{
						FollowCards = m_dvMaxBoom;
						m_nSuggestOutTime = 5;
						return true;
					}
				}
				if (!m_dvDoubles.empty())
				{
					sort(m_dvDoubles.begin(),m_dvDoubles.end(),cmpCV);
					if (CmpSingle(m_dvDoubles.back().front(),tmpLandCards.front()) &&
						CmpSingle(m_dvDoubles.back().front(),OutCard.front()))
					{
						FollowCards.push_back(m_dvDoubles.back().front());
						m_nSuggestOutTime = 4;
						return  true;
					}
				}
				if (!m_dvThrees.empty())
				{
					sort(m_dvThrees.begin(),m_dvThrees.end(),cmpCV);
					if (CmpSingle(m_dvThrees.back().front(),tmpLandCards.front()) && 
						CmpSingle(m_dvThrees.back().front(),OutCard.front()))
					{
						FollowCards.push_back(m_dvThrees.back().front());
						m_nSuggestOutTime = 4;
						return  true;
					}
				}
				
				sort(tmpMyCards.begin(), tmpMyCards.end(),cmpMaxMin);
				FollowCards.push_back(tmpMyCards.front());
				if (CmpSingle(FollowCards.front(),OutCard.front()))
					return true;
				else 
				{
					FollowCards.clear();
				}
			}else if (OutCard.size() ==2)
			{
				sort(m_dvDoubles.begin(),m_dvDoubles.end(),cmpCV);
				if (!m_dvDoubles.empty() && GameRule::CanOut(false,tmpLandCards,m_dvDoubles.back()) &&
					GameRule::CanOut(false,OutCard,m_dvDoubles.back()))
				{
					FollowCards = m_dvDoubles.back();
					return true;
				}
				//Ѱը��
				//�����ȥը�����Լ��ܱ�����ѹ��������С��2��
				if (!m_dvBooms.empty())
				{
					int nMaxNum = CalcAllNotGreateThanCards();
					int nMyCount =  m_PlayerSplit[m_nOutSeat].GetHandCount();
					if (nMyCount - nMaxNum  <=1)
					{
						sort(m_dvBooms.begin(),m_dvBooms.end(),cmpCV);
						for (int b=0; b<m_dvBooms.size(); ++b)
						{
							if (GameRule::CanOut(false,OutCard,m_dvBooms.at(b)) &&
								GameRule::CanOut(false,tmpLandCards,m_dvBooms.at(b)))
							{
								FollowCards = m_dvBooms.at(b);
								m_nSuggestOutTime = 5;
								return true;
							}
						}
					}
				}
				if (!m_dvMaxBoom.empty())
				{
					int nMaxNum = CalcAllNotGreateThanCards();
					int nMyCount =  m_PlayerSplit[m_nOutSeat].GetHandCount();
					if (nMyCount - nMaxNum  <=1 )
					{
						FollowCards = m_dvMaxBoom;
						m_nSuggestOutTime = 5;
						return true;
					}
				}
				//������
				if (!m_dvThrees.empty())
				{
					sort(m_dvThrees.begin(),m_dvThrees.end(),cmpCV);
					TCardVector  tmpdoubel = m_dvThrees.back();
					tmpdoubel.pop_back();
					if (GameRule::CanOut(false,OutCard,tmpdoubel) && 
						GameRule::CanOut(false,tmpLandCards,tmpdoubel))
					{
						FollowCards = tmpdoubel;
						m_nSuggestOutTime = 5;
						return  true;
					}else
					{
						FollowCards.clear();
					}
				}
				//Ѱ�����Ķ��Ӵ��
				sort(tmpMyCards.begin(),tmpMyCards.end(),cmpMaxMin);
				for (int index=0; index<tmpMyCards.size()-1;++index)
				{
					FollowCards.clear();
					FollowCards.push_back(tmpMyCards.at(index));
					FollowCards.push_back(tmpMyCards.at(index+1));
					CardTypeInfo cti = GameRule::GetCardType(FollowCards);
					if (cti.nType == CT_DOUBLE &&GameRule::CanOut(false,OutCard,FollowCards))
					{
						return true;
					}else
					{
						FollowCards.clear();
					}
				}
			}
			//�����ȥը�����Լ��ܱ�����ѹ��������С��2��
			if (!m_dvBooms.empty())
			{
				int nMaxNum = CalcAllNotGreateThanCards();
				int nMyCount =  m_PlayerSplit[m_nOutSeat].GetHandCount();
				if (nMyCount - nMaxNum  <=1)
				{
					sort(m_dvBooms.begin(),m_dvBooms.end(),cmpCV);
					for (int b=0; b<m_dvBooms.size(); ++b)
					{
						if (GameRule::CanOut(false,OutCard,m_dvBooms.at(b)) &&
							GameRule::CanOut(false,tmpLandCards,m_dvBooms.at(b)))
						{
							FollowCards = m_dvBooms.at(b);
							m_nSuggestOutTime = 5;
							return true;
						}
					}
				}
			}
			if (!m_dvMaxBoom.empty())
			{
				int nMaxNum = CalcAllNotGreateThanCards();
				int nMyCount =  m_PlayerSplit[m_nOutSeat].GetHandCount();
				if (nMyCount - nMaxNum  <=1 )
				{
					FollowCards = m_dvMaxBoom;
					m_nSuggestOutTime = 5;
					return true;
				}
			}
		}
		FindFollowGeneral(OutCard,FollowCards,2);
	}
	return !FollowCards.empty();
}

//ũ�����
bool CRobot::FindFollowFarmer(const TCardVector& OutCard, TCardVector& FollowCards)
{
	TCardVector tmp;
	TCardVector tmpOut = OutCard;

	//���ڶ��ѵ��ƣ����Կ��ǲ���
	if (m_nLastOut != m_nLandlord)
	{
		//����������ϼң����Ƶĵ��Ƕ��ѣ�ֱ�ӹ�
		if (m_nLastOut == GetNextPlayerSeat(m_nOutSeat))
		{
			int nNumBig = CalcAllNotGreateThanCards();
			if (nNumBig  == m_PlayerSplit[m_nOutSeat].GetHandCount())
			{
				FindFollowGeneral(OutCard,FollowCards);
				m_nSuggestOutTime  = 6;
				return !FollowCards.empty();
			}else 
			{
				m_nSuggestOutTime = 2;
				return false;
			}
		}
		//�������ֻʣ��һ�����ˣ�����ʣ�µ����ƺ�������������ͬ�ģ���ô��Ҫ�������,����������һ�����ܹ�������һ�ѽ���
		if (m_nLandlord == GetNextPlayerSeat(m_nOutSeat) && 
			GameRule::CanOut(false,OutCard,m_dvPlayerCards[m_nLandlord]))
		{
			if (OutCard.size() ==1 )
			{
				sort(m_dvSingles.begin(),m_dvSingles.end(),cmpTV);
				if (!m_dvSingles.empty()&& CmpSingle(m_dvSingles.back(),OutCard.front()) &&
					CmpSingle(m_dvSingles.back(),m_dvPlayerCards[m_nLandlord].front()))
				{
					FollowCards.push_back(m_dvSingles.back());
					m_nSuggestOutTime = 6;
					return true;
				}
				//�����ȥը�����Լ��ܱ�����ѹ��������С��2��
				if (!m_dvBooms.empty())
				{
					int nMaxNum = CalcAllNotGreateThanCards();
					int nMyCount =  m_PlayerSplit[m_nOutSeat].GetHandCount();
					if (nMyCount - nMaxNum  <=1)
					{
						sort(m_dvBooms.begin(),m_dvBooms.end(),cmpCV);
						for (int b=0; b<m_dvBooms.size(); ++b)
						{
							if (GameRule::CanOut(false,OutCard,m_dvBooms.at(b)) &&
								GameRule::CanOut(false,m_dvPlayerCards[m_nLandlord],m_dvBooms.at(b)))
							{
									FollowCards = m_dvBooms.at(b);
									m_nSuggestOutTime = 8;
									return true;
							}
						}
					}
				}
				if (!m_dvMaxBoom.empty())
				{
					int nMaxNum = CalcAllNotGreateThanCards();
					int nMyCount =  m_PlayerSplit[m_nOutSeat].GetHandCount();
					if (nMyCount - nMaxNum  <=1 )
					{
						FollowCards = m_dvMaxBoom;
						m_nSuggestOutTime = 8;
						return true;
					}
				}
				//�����
				if (!m_dvDoubles.empty())
				{
					sort(m_dvDoubles.begin(),m_dvDoubles.end(),cmpCV);
					if ( CmpSingle(m_dvDoubles.back().front(),OutCard.front())&&
						CmpSingle(m_dvDoubles.back().front(),m_dvPlayerCards[m_nLandlord].front()))
					{
						FollowCards.push_back(m_dvDoubles.back().front());
						m_nSuggestOutTime = 6;
						return  true;
					}
				}
				//������
				if (!m_dvThrees.empty())
				{
					sort(m_dvThrees.begin(),m_dvThrees.end(),cmpCV);
					if (CmpSingle(m_dvThrees.back().front(),OutCard.front())&&
						CmpSingle(m_dvThrees.back().front(),m_dvPlayerCards[m_nLandlord].front()))
					{
						FollowCards.push_back(m_dvThrees.back().front());
						m_nSuggestOutTime = 6;
						return  true;
					}
				}
				//��󿴿������Ƶ��Ƿ���ѹ��
				sort(m_dvPlayerCards[m_nOutSeat].begin(), m_dvPlayerCards[m_nOutSeat].end(),cmpMaxMin);
				if (CmpSingle(m_dvPlayerCards[m_nOutSeat].front(),OutCard.front()))
				{
					FollowCards.push_back(m_dvPlayerCards[m_nOutSeat].front());
					m_nSuggestOutTime = 6;
					return true;
				}
				//��Ϊ������������еĿ��ܶ�û�����ƣ�����ֱ�ӷ����ˡ�
				FollowCards.clear();
				return false;
			}else if (OutCard.size() == 2)
			{
				sort(m_dvDoubles.begin(),m_dvDoubles.end(),cmpCV);
				if (!m_dvDoubles.empty() && 
					GameRule::CanOut(false,m_dvPlayerCards[m_nLandlord],m_dvDoubles.back())&&
					GameRule::CanOut(false,OutCard,m_dvDoubles.back()))
				{
					FollowCards = m_dvDoubles.back();
					m_nSuggestOutTime = 7;
					return true;
				}
				if (!m_dvBooms.empty())
				{
					int nMaxNum = CalcAllNotGreateThanCards();
					int nMyCount =  m_PlayerSplit[m_nOutSeat].GetHandCount();
					if (nMyCount - nMaxNum  <=1)
					{
						sort(m_dvBooms.begin(),m_dvBooms.end(),cmpCV);
						for (int b=0; b<m_dvBooms.size(); ++b)
						{
							if (GameRule::CanOut(false,OutCard,m_dvBooms.at(b)) &&
								GameRule::CanOut(false,m_dvPlayerCards[m_nLandlord],m_dvBooms.at(b)))
							{
								FollowCards = m_dvBooms.at(b);
								m_nSuggestOutTime = 8;
								return true;
							}
						}
					}
				}
				if (!m_dvMaxBoom.empty())
				{
					int nMaxNum = CalcAllNotGreateThanCards();
					int nMyCount =  m_PlayerSplit[m_nOutSeat].GetHandCount();
					if (nMyCount - nMaxNum  <=1 )
					{
						FollowCards = m_dvMaxBoom;
						m_nSuggestOutTime = 8;
						return true;
					}
				}
				//������
				if (!m_dvThrees.empty())
				{
					sort(m_dvThrees.begin(),m_dvThrees.end(),cmpCV);
					FollowCards = m_dvThrees.back();
					FollowCards.pop_back();
					if (GameRule::CanOut(false,OutCard,FollowCards) && 
						GameRule::CanOut(false,m_dvPlayerCards[m_nLandlord],FollowCards))
					{
						m_nSuggestOutTime = 4;
						return  true;
					}else
					{
						FollowCards.clear();
					}
				}
				//Ѱ�����Ķ���
				sort(m_dvPlayerCards[m_nOutSeat].begin(),m_dvPlayerCards[m_nOutSeat].end(),cmpMaxMin);
				for (int index=0; index<m_dvPlayerCards[m_nOutSeat].size()-1;++index)
				{
					FollowCards.clear();
					FollowCards.push_back(m_dvPlayerCards[m_nOutSeat].at(index));
					FollowCards.push_back(m_dvPlayerCards[m_nOutSeat].at(index+1));
					CardTypeInfo cti = GameRule::GetCardType(FollowCards);
					if (cti.nType == CT_DOUBLE && GameRule::CanOut(false,OutCard,FollowCards))
					{
						m_nSuggestOutTime = 4;
						return true;
					}else
					{
						FollowCards.clear();
					}
				}

				//��Ϊ������������еĿ��ܶ�û�����ƣ�����ֱ�ӷ����ˡ�
				FollowCards.clear();
				return false;
			}
		}

		sort(tmpOut.begin(),tmpOut.end(),cmpTV);
		if (CmpSingle(tmpOut.back(),m_dvMaxThreeCards.back())||
			tmpOut.back().GetValue() >= CV_K)
		{
			m_nSuggestOutTime = 2;
			return false;
		}
		if (GameRule::CanOut(false,OutCard,m_dvPlayerCards[m_nLastOut]))
		{
			m_nSuggestOutTime = 1;
			return false;
		}
		if (!GetGreaterThanNextOut(m_dvPlayerCards[m_nLandlord],tmpOut,tmp))
		{
			m_nSuggestOutTime = 1;
			return false;
		}
	}
	//�������ܸ�
	int nHandNum = m_PlayerSplit[m_nLandlord].GetHandCount();
	if (nHandNum >3)
	{
		if (m_PlayerSplit[m_nOutSeat].GetHandCount() ==1 )
		{
			if (GameRule::CanOut(false,OutCard,m_dvPlayerCards[m_nOutSeat]))
			{
				FollowCards = m_dvPlayerCards[m_nOutSeat];
				m_nSuggestOutTime = 1;
				return true;
			}else 
			{
				m_nSuggestOutTime =  1;
				return false;
			}
		}
		FindFollowGeneral(OutCard,FollowCards);
		if (!FollowCards.empty())  //����ƺܴ󣬳���Ҳ�Ǵ��ƾ�ѹ�����ǹ�
		{
			CardTypeInfo cti = GameRule::GetCardType(OutCard);
			if (cti.nType ==  CT_MULTI || cti.nType == CT_MULTIDOUBLETWO || cti.nType == CT_MULTITHREE)
			{
				m_nSuggestOutTime = 8;
				return true;
			}
			sort(FollowCards.begin(),FollowCards.end(),cmpTV);
			TCardVector tmpOut = OutCard;
			TCardVector tmpFollow = FollowCards;
			if (cti.nType == CT_THREEWITHONE)
			{
				m_nSuggestOutTime = 5;
				if (tmpOut.front().GetValue() == tmpOut.at(2).GetValue())
				{
					tmpOut.pop_back();
				}
				if (tmpOut.at(1).GetValue() == tmpOut.back().GetValue())
				{
					tmpOut.erase(tmpOut.begin());
				}
			}
			if (cti.nType == CT_THREEWITHTWO)
			{
				m_nSuggestOutTime = 5;
				if (tmpOut.front().GetValue() == tmpOut.at(2).GetValue())
				{
					tmpOut.pop_back();
					tmpOut.pop_back();
				}
				if (tmpOut.at(1).GetValue() == tmpOut.back().GetValue())
				{
					tmpOut.erase(tmpOut.begin());
					tmpOut.erase(tmpOut.begin());
				}
			}

			if (cti.nType == CT_SINGLE || cti.nType == CT_DOUBLE || cti.nType == CT_THREE||
				cti.nType == CT_THREEWITHONE || CT_THREEWITHTWO)
			{
				m_nSuggestOutTime = 5;
				if (CmpSingle(tmpFollow.front(),m_dvMaxThreeCards.back())|| tmpFollow.front().GetValue() == CV_2) //����ҵ��ĳ�����������������ֵ֮һ��
				{
					int nOut =GameRule::CardWeight( tmpOut.front().GetValue()) ;
					int nFollow = GameRule::CardWeight(tmpFollow.front().GetValue());
					if(nFollow-nOut ==1)  //�����㹻������ѹ��
					{
					}else
					{
						//������ƺ��Լ����ܹ�ȷ�����ƣ����������
						int nMyHandNum = m_PlayerSplit[m_nLandlord].GetHandCount();
						int nResidueCount = CalcAllNotGreateThanCards();
						if (nMyHandNum - nResidueCount <2);
						else
						{
							FollowCards.clear();
						}
					}
				}
			}
		}
		
		int nNext = GetNextPlayerSeat(m_nOutSeat);
		int nLast = GetLastPlayerSeat(m_nOutSeat);
		//�ѻ�λ�����Ӧ�����Բ��Ƴ�һ��
		if (FollowCards.empty())
		{
			TCardVector tmpOut = OutCard;
			TCardVector tmp;
			if (m_nLandlord == nNext || !GetGreaterThanNextOut(m_dvPlayerCards[nNext],tmpOut,tmp))
			{
				FollowCards.clear();
				FindFollowGeneral(OutCard,FollowCards,1);	
			}
		}
		if (nLast == m_nLandlord && !FollowCards.empty()) //�����¼ң���������ǵ��Ų���û��ѡ�����ƣ����ƴ�һ��
		{
			if (OutCard.size() == 1)
			{
				m_nSuggestOutTime = 10;
				FindFollowGeneral(OutCard,FollowCards,1);	
			}
		}
		//�����ϼҵ���
		if (m_nLandlord ==  nNext && !FollowCards.empty())
		{
			int nCardValue = GameRule::CardWeight(FollowCards.front().GetValue());

			if (OutCard.size() == 1 && nCardValue < CV_10)
			{
				//��ѡ�����ĸ��Ƶ���С��10��ʱ�򣬿�����ѡ��һ�Ÿ���һ����ƴ�������ʵ��û�У�
				//������
				for (int i =0; i<m_dvSingles.size(); i++)
				{
					int nTmpValue = GameRule::CardWeight(m_dvSingles.at(i).GetValue());
					if (nTmpValue > CV_10 && nTmpValue < CV_K)
					{
						FollowCards.clear();
						FollowCards.push_back(m_dvSingles.at(i));
						m_nSuggestOutTime = 8;
						return true;
					}
				}
			}else if (OutCard.size() == 2 && nCardValue < CV_8)
			{
				//��ѡ�������ƵĶ���С��8��ʱ�򣬿����¸���һ����ƴ�������ʵ��ô�У�������
				for (int i =0; i<m_dvDoubles.size(); i++)
				{
					int nTmpValue = GameRule::CardWeight(m_dvDoubles.at(i).front().GetValue());
					if (nTmpValue > CV_8 && nTmpValue < CV_K)
					{
						FollowCards.clear();
						FollowCards = m_dvDoubles.at(i);
						m_nSuggestOutTime = 8;
						return true;
					}
				}
			}
		}
		//�����¼�ѹ��
		//���ȷ���Լ��Ķ���û����ѹ�������ĳ��ƣ���ô�Լ�Ӧ�ó��԰��Ʋ�һ��
		if (FollowCards.empty() && m_nLastOut==m_nLandlord && nNext != m_nLandlord)
		{
			TCardVector dvUseless;
			if (OutCard.size() == 1 && !GetGreaterThanNextOut(m_dvPlayerCards[m_nOutSeat],tmpOut,dvUseless))
			{
				  FindFollowGeneral(OutCard,FollowCards,1);
			}
		}
	}
	else 
	{
		TCardVector tmpMyCards = m_dvPlayerCards[m_nOutSeat];
		TCardVector tmpLandCards = m_dvPlayerCards[m_nLandlord];
		if (nHandNum ==1 && GameRule::CanOut(false,OutCard,m_dvPlayerCards[m_nLandlord]))  //����ʣ��һ����
		{
			if (OutCard.size() ==1 )
			{
				sort(m_dvSingles.begin(),m_dvSingles.end(),cmpTV);
				if (!m_dvSingles.empty()&& CmpSingle(m_dvSingles.back(),OutCard.front()) &&
					CmpSingle(m_dvSingles.back(),m_dvPlayerCards[m_nLandlord].front()))
				{
					FollowCards.push_back(m_dvSingles.back());
					m_nSuggestOutTime = 4;
					return true;
				}
				//�����ȥը�����Լ��ܱ�����ѹ��������С��2��
				if (!m_dvBooms.empty())
				{
					int nMaxNum = CalcAllNotGreateThanCards();
					int nMyCount =  m_PlayerSplit[m_nOutSeat].GetHandCount();
					if (nMyCount - nMaxNum  <=1)
					{
						sort(m_dvBooms.begin(),m_dvBooms.end(),cmpCV);
						for (int b=0; b<m_dvBooms.size(); ++b)
						{
							if (GameRule::CanOut(false,OutCard,m_dvBooms.at(b)) &&
								GameRule::CanOut(false,m_dvPlayerCards[m_nLandlord],m_dvBooms.at(b)))
							{
								FollowCards = m_dvBooms.at(b);
								m_nSuggestOutTime = 7;
								return true;
							}
						}
					}
				}
				if (!m_dvMaxBoom.empty())
				{
					int nMaxNum = CalcAllNotGreateThanCards();
					int nMyCount =  m_PlayerSplit[m_nOutSeat].GetHandCount();
					if (nMyCount - nMaxNum  <=1 )
					{
						FollowCards = m_dvMaxBoom;
						m_nSuggestOutTime = 7;
						return true;
					}
				}
				//�����
				if (!m_dvDoubles.empty())
				{
					sort(m_dvDoubles.begin(),m_dvDoubles.end(),cmpCV);
					if ( CmpSingle(m_dvDoubles.back().front(),OutCard.front())&&
						CmpSingle(m_dvDoubles.back().front(),m_dvPlayerCards[m_nLandlord].front()))
					{
						FollowCards.push_back(m_dvDoubles.back().front());
						m_nSuggestOutTime = 4;
						return  true;
					}
				}
				//������
				if (!m_dvThrees.empty())
				{
					sort(m_dvThrees.begin(),m_dvThrees.end(),cmpCV);
					if (CmpSingle(m_dvThrees.back().front(),OutCard.front())&&
						CmpSingle(m_dvThrees.back().front(),m_dvPlayerCards[m_nLandlord].front()))
					{
						FollowCards.push_back(m_dvThrees.back().front());
						m_nSuggestOutTime = 4;
						return  true;
					}
				}
				//��󿴿������Ƶ��Ƿ���ѹ��
				sort(m_dvPlayerCards[m_nOutSeat].begin(), m_dvPlayerCards[m_nOutSeat].end(),cmpMaxMin);
				if (CmpSingle(m_dvPlayerCards[m_nOutSeat].front(),OutCard.front()))
				{
					FollowCards.push_back(m_dvPlayerCards[m_nOutSeat].front());
					return true;
				}
				FollowCards.clear();
				return false;
			}else if (OutCard.size() == 2)
			{
				sort(m_dvDoubles.begin(),m_dvDoubles.end(),cmpCV);
				if (!m_dvDoubles.empty() && 
					GameRule::CanOut(false,m_dvPlayerCards[m_nLandlord],m_dvDoubles.back())&&
					GameRule::CanOut(false,OutCard,m_dvDoubles.back()))
				{
					FollowCards = m_dvDoubles.back();
					m_nSuggestOutTime = 4;
					return true;
				}
				if (!m_dvBooms.empty())
				{
					int nMaxNum = CalcAllNotGreateThanCards();
					int nMyCount =  m_PlayerSplit[m_nOutSeat].GetHandCount();
					if (nMyCount - nMaxNum  <=1)
					{
						sort(m_dvBooms.begin(),m_dvBooms.end(),cmpCV);
						for (int b=0; b<m_dvBooms.size(); ++b)
						{
							if (GameRule::CanOut(false,OutCard,m_dvBooms.at(b)) &&
								GameRule::CanOut(false,m_dvPlayerCards[m_nLandlord],m_dvBooms.at(b)))
							{
								FollowCards = m_dvBooms.at(b);
								m_nSuggestOutTime = 8;
								return true;
							}
						}
					}
				}
				if (!m_dvMaxBoom.empty())
				{
					int nMaxNum = CalcAllNotGreateThanCards();
					int nMyCount =  m_PlayerSplit[m_nOutSeat].GetHandCount();
					if (nMyCount - nMaxNum  <=1 )
					{
						FollowCards = m_dvMaxBoom;
						m_nSuggestOutTime = 8;
						return true;
					}
				}
				//������
				if (!m_dvThrees.empty())
				{
					sort(m_dvThrees.begin(),m_dvThrees.end(),cmpCV);
					FollowCards = m_dvThrees.back();
					FollowCards.pop_back();
					if (GameRule::CanOut(false,OutCard,FollowCards) && 
						GameRule::CanOut(false,m_dvPlayerCards[m_nLandlord],FollowCards))
					{
						m_nSuggestOutTime = 4;
						return  true;
					}else
					{
						FollowCards.clear();
					}
				}
				//Ѱ�����Ķ���
				sort(m_dvPlayerCards[m_nOutSeat].begin(),m_dvPlayerCards[m_nOutSeat].end(),cmpMaxMin);
				for (int index=0; index<m_dvPlayerCards[m_nOutSeat].size()-1;++index)
				{
					FollowCards.clear();
					FollowCards.push_back(m_dvPlayerCards[m_nOutSeat].at(index));
					FollowCards.push_back(m_dvPlayerCards[m_nOutSeat].at(index+1));
					CardTypeInfo cti = GameRule::GetCardType(FollowCards);
					if (cti.nType == CT_DOUBLE && GameRule::CanOut(false,OutCard,FollowCards))
					{
						m_nSuggestOutTime = 4;
						return true;
					}else
					{
						FollowCards.clear();
					}
				}
				FollowCards.clear();
				return false;
			}
			FindFollowGeneral(OutCard,FollowCards,2);
		}
		FindFollowGeneral(OutCard,FollowCards,2);
		//������ѳ���һ���ܴ���ƣ��ǾͲ�Ҫ����
		if (m_nOutSeat != m_nLandlord)
		{
			CardTypeInfo ctiTeam = GameRule::GetCardType(OutCard);
			if (ctiTeam.nType == CT_BOOM ) FollowCards.clear();
			if (ctiTeam.nType == CT_SINGLE || ctiTeam.nType == CT_DOUBLE)
			{
				//���Ѵ���������󣬾Ͳ�Ҫ����
				if (m_dvMaxThreeCards.size() ==1 && OutCard.front().GetValue() == m_dvMaxThreeCards.front().GetValue()) 
				{
					FollowCards.clear();
				}else if (m_dvMaxThreeCards.size() >=2 && OutCard.front().GetValue() >= m_dvMaxThreeCards.at(1).GetValue())
				{
					FollowCards.clear();
				}
			}
			//
		}
	}
	return !FollowCards.empty();
}

void CRobot::DelCV(CardsVector& cvWho,const TCardVector& dvDel)
{
	if (cvWho.empty() || dvDel.empty()) return ;
	for (int i=0; i<cvWho.size(); ++i)
	{
		cvWho.at(i)-=dvDel;
		if (cvWho.at(i).empty())
		{
			cvWho.erase(cvWho.begin()+i);
			i =0; 
		}
	}
}
//////////////////////////////////////////////////////////////////////////
//Ѱ�ҵ�ǰӦ�ø�����,
//�ҵ����ϵ��ƺ󣬷���true�������FollowCardsΪ���ƣ�
//���û���ҵ��ܸ��ϵ��ƣ�����false,����FollowCards�ÿ�
bool CRobot::FindNowFollow(TCardVector OutCard, TCardVector& FollwCards)
{
	 FollwCards.clear();
	 if (OutCard.empty()) return false;
	 //����Լ����е��������ܹ����ƣ�ֱ�ӽ���
	 CardTypeInfo cti = GameRule::GetCardType(m_dvPlayerCards[m_nOutSeat]);
	 if (GameRule::CanOut(false,OutCard,m_dvPlayerCards[m_nOutSeat]))
	 {
		 FollwCards = m_dvPlayerCards[m_nOutSeat];
		 m_nSuggestOutTime = 1;
		 return true;
	 }
	
	int nMax = CalcAllNotGreateThanCards();
	int nHand = m_PlayerSplit[m_nOutSeat].GetHandCount();
	if (nMax+1 >= nHand)
	{
		if (!m_dvBooms.empty())
		{
			FollwCards = m_dvBooms.front();
			 m_nSuggestOutTime =1;
			return true;
		}else if (!m_dvMaxBoom.empty())
		{
			FollwCards = m_dvMaxBoom;
			m_nSuggestOutTime =1;
			return true;
		}
		
	}
	//����Լ�����ֻʣ�������ƣ�����������ͬ�����нϴ��һ���������ô���������
	if (m_PlayerSplit[m_nOutSeat].GetHandCount() == 2)
	{
		 if (PlayerFollowAtHand2(OutCard,FollwCards))
		 {
			 m_nSuggestOutTime = 1;
			 return true;
		 }else
		 {
			 FollwCards.clear();
		 }
	}

	 //��������
	bool bFlag = false;
	 if (m_nOutSeat == m_nLandlord)
	 {
		 bFlag =  FindFollowLand(OutCard,FollwCards);
		 if (!bFlag) m_nSuggestOutTime = 2;
		 return bFlag;
	 }
	 bFlag=FindFollowFarmer(OutCard,FollwCards);
	 if (!bFlag ) m_nSuggestOutTime = 2;
	 return bFlag;
}

 //����ָ�������еĴ�������
 void CRobot::CalcBigNum(int& nBig, TCardVector dvWho)
{
	int nLast  = GetLastPlayerSeat(m_nLandlord);
	int nNext = GetNextPlayerSeat(m_nLandlord);
	//���Ƶľ�����ʽ���ٵ���С����ʣ�µ����2�鼴Ϊ���ƣ���С��ҲΪ����
	//Ex:��ǰ����������е��ƻ�ʣ��:����������2������K������J��4��10����
	//ô������2��K J���Ǵ���
	TCardVector dvNowAllCards ;
	for (int i=0; i<MAX_PLAYER;i++)
	{
		dvNowAllCards += m_dvPlayerCards[i];
	}
	int nNumMy[15];
	int nNumAll[15];
	int nNowMaxCount = 0;
	int nMyBigCount = 0;
	int nIndex = 12;
	int nLoop = 2;

	this->CalcCardConut(nNumAll,dvNowAllCards);
	this->CalcCardConut(nNumMy,dvWho);
	//��������ը��������
	for (int i=0; i<MAX_PLAYER;i++)
	{
		nNowMaxCount += m_PlayerSplit[i].s_maxBoom.size();
		nNowMaxCount += m_PlayerSplit[i].s_boom.size();
	}
	//��С��һ���Ǵ���
	nNowMaxCount += nNumAll[13]; 
	nNowMaxCount += nNumAll[14];
	nMyBigCount += nNumMy[13];
	nMyBigCount += nNumMy[14];
	while (nLoop > 0  )
	{
		if (nNumAll[nIndex] == 0)  break;
		else nNowMaxCount += nNumAll[nIndex];
		nMyBigCount += nNumMy[nIndex];
		nIndex --;
		nLoop --;
	}
	
	nBig = nMyBigCount;
}
 //���㱾��(�Լ��Ͷ���)�뵱ǰ�����ƵĴ�������
 void CRobot::CalcBigNum(int& my, int& all)
 {
	 int nLast  = GetLastPlayerSeat(m_nLandlord);
	 int nNext = GetNextPlayerSeat(m_nLandlord);
	 //���Ƶľ�����ʽ���ٵ���С����ʣ�µ����2�鼴Ϊ���ƣ���С��ҲΪ����
	 //Ex:��ǰ����������е��ƻ�ʣ��:����������2������K������J��4��10����
	 //ô������2��K J���Ǵ���
	 TCardVector dvNowAllCards ;
	 for (int i=0; i<MAX_PLAYER;i++)
	 {
		 dvNowAllCards += m_dvPlayerCards[i];
	 }
	 int nNumAll[15]={};
	 int nNumMy[15];
	 int nNowMaxCount = 0;
	 int nMyBigCount = 0;
	 int nIndex = 12;
	 int nLoop = 2;
	 //�Լ������Լ������
	 TCardVector dvMyTeamCards;
 	 if (m_nOutSeat == m_nLandlord)
	 {
		 //ը��Ҳ��Ϊһ������
		 nMyBigCount += m_PlayerSplit[m_nLandlord].s_boom.size();
		 nMyBigCount += m_PlayerSplit[m_nLandlord].s_maxBoom.size();
		 dvMyTeamCards = m_dvPlayerCards[m_nLandlord];
	 }else
	 {
		 nMyBigCount += m_PlayerSplit[nNext].s_boom.size();
		 nMyBigCount += m_PlayerSplit[nLast].s_boom.size();
		 nMyBigCount += m_PlayerSplit[nNext].s_maxBoom.size();
		 nMyBigCount += m_PlayerSplit[nLast].s_maxBoom.size();

		 dvMyTeamCards += m_dvPlayerCards[nNext];
		 dvMyTeamCards += m_dvPlayerCards[nLast];
	 }
	 this->CalcCardConut(nNumAll,dvNowAllCards);
	 this->CalcCardConut(nNumMy,dvMyTeamCards);
	 //��������ը��������
	 for (int i=0; i<MAX_PLAYER;i++)
	 {
		 nNowMaxCount += m_PlayerSplit[i].s_maxBoom.size();
		 nNowMaxCount += m_PlayerSplit[i].s_boom.size();
	 }
	 //��С��һ���Ǵ���
	 nNowMaxCount += nNumAll[13]; 
	 nNowMaxCount += nNumAll[14];
	 nMyBigCount += nNumMy[13];
	 nMyBigCount += nNumMy[14];
	 while (nLoop > 0  )
	 {
		 if (nNumAll[nIndex] == 0)  break;
		 else nNowMaxCount += nNumAll[nIndex];
		 nMyBigCount += nNumMy[nIndex];
		 nIndex --;
		 nLoop --;
	 }
	 my = nMyBigCount;
	 all  = nNowMaxCount;
 }

 //��ͨ����
 bool CRobot::GeneralFollw(TCardVector& dvRes,TCardVector dvOut)
 {
	FindFollowGeneral(dvOut,dvRes);
	sort(dvRes.begin(),dvRes.end(),cmpTV);
	int nMyBigCardCount = 0,nAllBigCardCount =0;

	this->CalcBigNum(nMyBigCardCount,nAllBigCardCount);
	
	double   fnMyBigRatio = (double) nMyBigCardCount/nAllBigCardCount;
	int nMyBigRatio = int (fnMyBigRatio * 100);
	if (!dvRes.empty())
	{
		CardTypeInfo cti = GameRule::GetCardType(dvRes);
		if (cti.nType > CT_NONE && cti.nType < CT_COUNT)
		{
			//������ƺ��Լ��϶��ܹ��ƣ�ֱ�ӽ���
			//�����Լ������ж���Ҫ���������
			int nCount =0;
			int nSameCount =0 ;
			nCount += CalcGreaterThanRivalCount(m_dvBooms, nSameCount);
			nCount += CalcGreaterThanRivalCount(m_dvThreeMultis,nSameCount);
			nCount += CalcGreaterThanRivalCount(m_dvDoubleMultis,nSameCount);
			nCount += CalcGreaterThanRivalCount(m_dvSingleMultis,nSameCount);
			nCount += CalcGreaterThanRivalCount(m_dvThrees,nSameCount);
			nCount += CalcGreaterThanRivalCount(m_dvDoubles,nSameCount);
			nCount += CalcGreaterThanRivalCount(m_dvSingles,nSameCount);
			nCount += CalcGreaterThanRivalCount(m_dvMaxBoom,nSameCount);
			if (m_nLevel[m_nOutSeat]-nCount <= 1)
			{
				return true;
			}
			//���ѡȡ�������Ǵ��ƣ���ȥ����Ĵ��ƣ������Լ����л���ʣ�����д��Ƶ�һ�����ϣ�
			//���򲻴��
			int nOutBingNum = 0;   //�����еĴ�������
			this->CalcBigNum(nOutBingNum,dvRes);
			if (cti.nType == CT_BOOM) nOutBingNum ++;
			else if (cti.nType == CT_MAXBOOM) nOutBingNum ++;

			if (cti.nType == CT_BOOM || cti.nType == CT_MAXBOOM ||
				dvRes.front().GetValue() >= CV_A || dvRes.front().GetValue() == CV_2)  
			{
			//	if((nMyBigCardCount- nOutBingNum)*2 < nAllBigCardCount-nOutBingNum) 
				if (m_nLandlord == m_nOutSeat && nMyBigRatio < 40) //�����Ĵ���ռ%40����ʱ������
				{
					dvRes.clear();
					return false;
				}else  if (nMyBigRatio < 50)
				{
					dvRes.clear();
					return false;
				}
			}
			//������ƺ��Լ���������
		}
	}
	 return !dvRes.empty();
 }

 bool CRobot::PlayerFollowAtHand2(const TCardVector& dvOut, TCardVector& dvRes)
 {
	 TCardVector tmpMyCards;
	 TCardVector tmpOut = dvOut;
	 FindFollowGeneral(dvOut,dvRes);
	 if (!dvRes.empty() && !RivalHaveGreaterThanMe(dvRes))
	 {
		 return true;
	 }else
	 {
		 CardTypeInfo cti = GameRule::GetCardType(dvOut);
		 switch(cti.nType)
		 {
		 case CT_SINGLE:
			 if (m_dvSingles.size() ==2 && RivalHaveGreaterThanMe(m_dvSingles))
			 {
				 sort(m_dvSingles.begin(),m_dvSingles.end(),cmpMaxMin);
				 dvRes.clear();
				 dvRes.push_back(m_dvSingles.front());
				 return true;
			 }
			 break;

		 case CT_DOUBLE:
			 if (m_dvDoubles.size() == 2&& RivalHaveGreaterThanMe(m_dvDoubles))
			 {
				sort(m_dvDoubles.begin(),m_dvDoubles.end(),cmpCV);
				dvRes.clear();
				dvRes = m_dvDoubles.back();
				return true;
			 }
				break;
		 default:
			 break;
		 }
	 }
	return false;
 }
//////////////////////////////////////////////////////////////////////////
//������������
void CRobot::Reset()
{
	m_dvSingleMultis.clear();
	m_dvDoubles.clear();
	m_dvDoubleMultis.clear();
	m_dvThrees.clear();
	m_dvThreeMultis.clear();
	m_dvSingles.clear();
	m_dvBooms.clear();
	m_dvMaxBoom.clear();
	for (int i=0; i<MAX_PLAYER; i++)
	{
		m_allSplitInfo[i].clear();
	}
	for (int i=0; i<MAX_PLAYER;i++)
	{
		m_Warring[i].nWL= false;
		m_Warring[i].nSeat=MAX_PLAYER;
	}
}
//////////////////////////////////////////////////////////////////////////
//������ʱ����
void CRobot::ResetTmp()
{
	cvTmpBoom.clear();
	cvTmpDouble.clear();
	cvTmpSingle.clear();
	cvTmpSingleMulti.clear();
	cvTmpThree.clear();
	cvTmpThreeMulti.clear();
	cvTmpDoubleMulti.clear();
	cvTmpMaxBoom.clear();
}

//////////////////////////////////////////////////////////////////////////
//����Ȩֵ������ѡ�����Ų��Ʒ�ʽ
void CRobot::FindOptimalSplit(int nSeat)
{
	int nMin = m_allSplitInfo[nSeat].front().GetHandCount() - m_allSplitInfo[nSeat].front().s_boom.size();
	int nIndex = 0;
	for (int i=1; i<m_allSplitInfo[nSeat].size();i++)
	{
		if (nMin >( m_allSplitInfo[nSeat].at(i).GetHandCount()- m_allSplitInfo[nSeat].front().s_boom.size()))
		{
			nMin = m_allSplitInfo[nSeat].at(i).GetHandCount() - m_allSplitInfo[nSeat].front().s_boom.size();
			nIndex = i;
		}else if (nMin == (m_allSplitInfo[nSeat].at(i).GetHandCount()- m_allSplitInfo[nSeat].front().s_boom.size())) //������ͬ��ѡ��Ȩֵ���
		{
			int nLevel_1 = m_allSplitInfo[nSeat].at(nIndex).GetAllLevel();
			int nLevel_2 = m_allSplitInfo[nSeat].at(i).GetAllLevel();
			nIndex =  (nLevel_1 > nLevel_2?nIndex:i);  
			nMin   = m_allSplitInfo[nSeat].at(nIndex).GetHandCount();
			//Ȩֵ����������ͬ����ô�����ѡһ������
			if (nLevel_2 == nLevel_1)
			{
				nIndex = i;
			}
		}
	}

	m_PlayerSplit[nSeat] = m_allSplitInfo[nSeat].at(nIndex);
	m_nLevel[nSeat] = m_allSplitInfo[nSeat].at(nIndex).GetHandCount();
	if (nSeat == m_nOutSeat)
	{
		//�����Ų��Ʒ�ʽ�����������
		ConvertCTToCV(m_allSplitInfo[nSeat].at(nIndex).s_boom,m_dvBooms,CT_BOOM);
		ConvertCTToCV(m_allSplitInfo[nSeat].at(nIndex).s_three,m_dvThrees,CT_THREE);
		ConvertCTToCV(m_allSplitInfo[nSeat].at(nIndex).s_doule,m_dvDoubles,CT_DOUBLE);
		ConvertCTToCV(m_allSplitInfo[nSeat].at(nIndex).s_threeMulti,m_dvThreeMultis,CT_MULTITHREE);
		ConvertCTToCV(m_allSplitInfo[nSeat].at(nIndex).s_doubleMulti,m_dvDoubleMultis,CT_MULTIDOUBLETWO);
		ConvertCTToCV(m_allSplitInfo[nSeat].at(nIndex).s_singleMulti,m_dvSingleMultis,CT_MULTI);
// 		//���
		//m_dvMaxBoom  = m_allSplitInfo[nSeat].at(nIndex).s_maxBoom.
		if (!cvTmpJoker.empty()) m_dvMaxBoom = cvTmpJoker;
// 		if (cvTmpJoker.size() == 2)
// 		{
// 			m_dvMaxBoom+= cvTmpJoker;
// 		}else if (cvTmpJoker.size() == 1) //��������
// 		{
// 			m_dvSingles.push_back(cvTmpJoker.front());
// 		}
		//����
		for (int i=0; i<m_allSplitInfo[nSeat].at(nIndex).s_single.size(); i++)
		{
			m_dvSingles += m_allSplitInfo[nSeat].at(nIndex).s_single.at(i).cards;
		}
	} 
	
}
//////////////////////////////////////////////////////////////////////////
//ְ�ܣ������Ƿ�ӵ��ָ�����͵���
//������nCardsLen���ͳ��ȣ�nType��Ŀ���ͣ�targetCardsĿ������
//���أ�����Ŀ�����������е�nType���ͣ����Ŀ��������û��ָ��
//      �����ͣ�����sizeΪ0������
TCardVector CRobot::GetBaseCards(int nCardsLen, int nType,  TCardVector targetCards)
{
	TCardVector tmpCards;
	tmpCards.clear();

	if (nCardsLen > targetCards.size() || targetCards.size() <= 0 )
	{
		return tmpCards;
	}
	TCardVector dvMayBeOut;
	CardTypeInfo ctiMayBeOut;		//���ڻ�ȡ������Ϣ
	CardTypeInfo ctiFrontMBO;		//ǰ��һ��������Ϣ
	ctiFrontMBO.reset();
	ctiMayBeOut.reset();
	TCardVector  dvTmpHand = targetCards;

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
				tmpCards += dvMayBeOut;
			}
			ctiFrontMBO = ctiMayBeOut;
		}
	}  //end for 1

	return tmpCards;
}

//////////////////////////////////////////////////////////////////////////
//���������������
//������ nLenָ����������ԭ�����ͳ��ȣ�nType��ָ�����������ڵ����ͣ�
//       dvNeedFill��ָ����Ҫ������������
void CRobot::FillCards(int nLen,int nType,CardsVector& dvNeedFill,TCardVector& dvWhere)
{
	if (dvWhere.size() < nLen)
	{
		return;
	}
	GameRule::SortCards(dvWhere);
	//��ȡ����ǰ����ӵ�е�ntype����
	TCardVector dvTmp = GetBaseCards(nLen,nType,dvWhere);
	TCardVector dvTmpSingleType;
	for (TCardVector::iterator it = dvTmp.begin();it<dvTmp.end();it+=nLen)
	{
		dvTmpSingleType.clear();
		dvTmpSingleType.insert(dvTmpSingleType.begin(),it,it+nLen);
		dvNeedFill.push_back(dvTmpSingleType);
	}
	//����䵽dvNeedFill�е��ƴ�dvWhere���޳�
	for (int i=0;i<dvTmp.size();i++)
	{
		dvWhere.erase(remove(dvWhere.begin(),dvWhere.end(),dvTmp.at(i)),dvWhere.end());
	}
}

//����ָ����ҵ�Ҫ������ֵ���������
int CRobot::CalcAllNotGreateThanCards()
{
	//�����Լ������ж���Ҫ���������
	int nCount =0;
	int nSameCount =0 ;
	nCount += CalcGreaterThanRivalCount(m_dvBooms, nSameCount);
	nCount += CalcGreaterThanRivalCount(m_dvThreeMultis,nSameCount);
	nCount += CalcGreaterThanRivalCount(m_dvDoubleMultis,nSameCount);
	nCount += CalcGreaterThanRivalCount(m_dvSingleMultis,nSameCount);
	nCount += CalcGreaterThanRivalCount(m_dvThrees,nSameCount);
	nCount += CalcGreaterThanRivalCount(m_dvDoubles,nSameCount);
	nCount += CalcGreaterThanRivalCount(m_dvSingles,nSameCount);
	nCount += m_dvMaxBoom.empty()?0:1;
	return nCount;
}
bool CRobot::GetGreaterThanNextOut(TCardVector fromCards,TCardVector& dvOutCard,TCardVector& ResCards)
{
	GameTrusteeship CTrust(fromCards);
	CardTypeInfo ctiFrontInfo;
	ctiFrontInfo.reset();
	ctiFrontInfo = GameRule::GetCardType(dvOutCard);

	if (CTrust.GetGreaterThanNextOut(ctiFrontInfo)) 
	{
		Trused::TCARDS OutCards = CTrust.m_PromptCard;
		//�ó������������
		for (int i=0;i<OutCards.size();i++)
		{
			TCards tmp(OutCards.at(i));
			ResCards.push_back(tmp);
		}
		if (!ResCards.empty())
		{
			return true;
		}
	}
	return false;
}


//////////////////////////////////////////////////////////////////////////
//����Ϊ���Ʋ��Һ����飬�������ܽ�Ϊ��ָ���������������ҵ����Դ������
//����ҵ����Դ�����ƣ��򷵻�1����ʹ�ò���Res������Ҫ�������
//���û���ҵ����Դ�����ƻ��ߴ�����Ʋ�������ô����0 
int  CRobot::FindOutFromThreeMulti(TCardVector& Res)//��˳
{
	/*
		��˳����ԭ��
		1.��˳�������ϱ�ѹס�Ŀ����Խ�С��һ�����Ӧ���ȴ����
		2.�����˳�ĵ���С�Ƶ�ֵ����(����J)����ʱ���������
		  a)������ֵ��ƻ��кܶ�(����6��)�������˳���Լ����ƺ�С����ֵ��С�����źࣩܶ
		  ��ôӦ����һ��.
		  b)�����˳���Լ�ʣ�µ��ƺܴ�,(��ֵ�ܴ���һ�Ѵ��)
		  ��ô�����    
	*/
	Res.clear();
	if (m_dvThreeMultis.size() >0) 
	{
		return FindGeneralOut(Res,m_dvThreeMultis);
	}
	return 0;
}

int  CRobot::FindOutFromDoubleMuti(TCardVector& Res)//˫˳
{
	Res.clear();
	if (m_dvDoubleMultis.size() >0) 
	{	
		return FindGeneralOut(Res,m_dvDoubleMultis);
	}
	return 0;
}

int  CRobot::FindOutFromSingleMuti(TCardVector& Res)//��˳
{
	Res.clear();
	if (m_dvSingleMultis.size() >0) 
	{
		return FindGeneralOut(Res,m_dvSingleMultis);
	}
	return 0;
}

int  CRobot::FindOutFromThreeWTwo(TCardVector& Res) //������
{
	if (m_dvThrees.size() > 0 && m_dvDoubles.size() > 0 ) //������ -- ��ȷ����
	{
		sort(m_dvThrees.begin(),m_dvThrees.end(),cmpCV);
		sort(m_dvDoubles.begin(),m_dvDoubles.end(),cmpCV);
		if (m_nRivalCount > 3)
		{
			if (m_dvThrees.front().front().GetValue() < CV_J
				&&m_dvDoubles.front().front().GetValue()<CV_J)
			{
				Res =  m_dvThrees.at(0);
				Res +=  m_dvDoubles.at(0);
				return 1;
			}
		}
	}
	return 0;
}

int  CRobot::FindOutFromThrees(TCardVector& Res)//����
{
	Res.clear();
	if (m_dvThrees.size() >0)//����--��ȷ����
	{
		if (1 == FindGeneralOut(Res,m_dvThrees))
		{
			return 1;
		}
	}
	return 0;
}

int  CRobot::FindOutFromDoubles(TCardVector& Res)//����
{
	Res.clear();
	if (m_dvDoubles.size()>0 ) //����--��ȷ����
	{
		return FindGeneralOut(Res,m_dvDoubles);

	}
	return 0;
}

int  CRobot::FindOutFromSingle(TCardVector& Res)//����
{
	if (m_dvSingles.size() > 0 &&
		m_dvSingles.front().GetValue() <CV_A &&
		m_dvSingles.front() != CV_2 )   //����--��ȷ����
	{
		sort(m_dvSingles.begin(),m_dvSingles.end(),cmpTV);
		Res.push_back(m_dvSingles.at(0));
		return 1;
	}
	return 0;
}

int	CRobot::FindGeneralOut(TCardVector& Res,CardsVector From)
{
	if (!From.empty())
	{
		sort(From.begin(),From.end(),cmpCV);
		Res = From.front();
		if (Res.back().GetValue() < CV_Q && Res.back().GetValue() != CV_2)  
		{
			return 1;
		}else  //��ֵ�����ʱ��ȷ��ѡ�������Ǵ�����
		{
			//ȷ���Ƿ����������
			TCardVector tmp = m_dvOldCards;
			tmp-=Res;
			CRobot TmpRobot(m_dvPlayerCards[m_nLandlord]);
			int nCount = TmpRobot.AnalyzeCards(m_dvPlayerCards[m_nLandlord],m_nOutSeat);

			if (nCount == 2)
			{
				sort(tmp.begin(),tmp.end(),cmpTV);
				if (tmp.back().GetValue() > CV_K)
				{
					return 1;
				}
			}else if (nCount == 1) 
			{
				return 1;
			}
		}
	}
	Res.clear();
	return 0;
}

int  CRobot::FindOutUnthink(TCardVector& Res,CardsVector From)
{
	Res.clear();
	if (!From.empty())
	{
		sort(From.begin(),From.end(),cmpCV);
		Res = From.front();
		return 1;
	}
	return 0;
}

//Ѱ�ҵ�ǰ�����������������������
void  CRobot::FindMaxThreeCards()
{
	m_dvMaxThreeCards.clear();
	if (m_dvAllCards.empty()) return;
	sort(m_dvAllCards.begin(),m_dvAllCards.end(),cmpMaxMin);
	m_dvMaxThreeCards.push_back(m_dvAllCards.front());
	for (int i=1; i<m_dvAllCards.size(); ++i)
	{
		if (m_dvMaxThreeCards.back().GetValue() != m_dvAllCards.at(i).GetValue())
		{
			m_dvMaxThreeCards.push_back(m_dvAllCards.at(i));
		}
		if (m_dvMaxThreeCards.size() ==3 ) break;
	}
// 	if (m_dvMaxThreeCards.size() <3) m_dvMaxThreeCards.clear();
// 	else 
		sort(m_dvMaxThreeCards.begin(),m_dvMaxThreeCards.end(),cmpMaxMin);
}

int CRobot::GetSuggestTime()
{
	if (m_nSuggestOutTime>20)
	{
		m_nSuggestOutTime = 15;
	}
	if (m_nSuggestOutTime <0)
	{
		m_nSuggestOutTime =0;
	}
	return m_nSuggestOutTime;
}

void CRobot::SetSuggestTime(int nTime)
{
	m_nSuggestOutTime = nTime;
}

void CRobot::SeparationCards(const TCardVector& dvCards,CardsVector& cvMain)
{
	if (dvCards.size() < 4) return;
	cvMain.clear();
	TCardVector tmpCards = dvCards;
	ChoseThree(cvMain,tmpCards);
}
//////////////////////////////////////////////////////////////////////////
//debug

//debug
//��ʾ�����������
void CRobot::ShowDebug(CardsVector ct,char* name)
{
	char buf[10];
	std::string str = name;
	if (ct.empty())
	{
		str += "Ϊ��";
		cDebugOut.DebugString(str);
		return;
	}
	str += "�У�";
	cDebugOut.DebugString(str);
	for (int i=0; i<ct.size();i++)
	{
		str.clear();
		for (int j=0;j<ct.at(i).size();j++)
		{
			//itoa(ct.at(i).at(j).GetValue(),buf,10);
			str+=buf;
			str+=" ";
			str += ct.at(i).at(j).GetName();
			str += "  ";
		}
		cDebugOut.DebugString(str);
		cDebugOut.DebugString("\n");
	}
}

//debug
void CRobot::ShowDebug(CT ct,char* name)
{
	std::string test = name;
	if (ct.empty())
	{
		test+= "Ϊ��";
		cDebugOut.DebugString(test);
	}else 
	{
		test += "��\n";
		for (int i=0; i<ct.size();i++)
		{
			for (int j=0;j<ct.at(i).cards.size();j++)
			{
				test += ct.at(i).cards.at(j).GetName();
			}
		}
		cDebugOut.DebugString(test);
	}
}

//��ʾ�����������
void CRobot::ShowDebug(SplitInfo sp,char* strWho)
{
	std::string str = strWho;
	str+= "���������Ϊ:";
	cDebugOut.DebugString(str);
	this->ShowDebug(sp.s_boom,"ը��");
	this->ShowDebug(sp.s_three,"����");
	this->ShowDebug(sp.s_threeMulti,"��˳");
	this->ShowDebug(sp.s_doule,"����");
	this->ShowDebug(sp.s_doubleMulti,"˫˳");
	this->ShowDebug(sp.s_singleMulti,"��˳");
	this->ShowDebug(sp.s_single,"����");
	this->ShowDebug(sp.s_maxBoom,"���");
	cDebugOut.DebugString("����\n");
}
//debug
//��ʾ�����������
void CRobot::ShowDebug(TCardVector ct,char* name)
{
	char buf[10];
	std::string str = name;
	if (ct.empty())
	{
		str += "Ϊ��";
		cDebugOut.DebugString(str);
		return;
	}
	str += "�У�";
	cDebugOut.DebugString(str);
	str.clear();
	for (int j=0;j<ct.size();j++)
	{
// 		itoa(ct.at(i).at(j).GetValue(),buf,10);
// 		str+=buf;
// 		str+=" ";
		str += ct.at(j).GetName();
		str += "  ";
	}
	cDebugOut.DebugString(str);
}
//debug
