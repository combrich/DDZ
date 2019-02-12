#include "GameRule.h"

bool operator < (const TCards& card1, const TCards& card2)
{
	return (GameRule::CardWeight(card1) < GameRule::CardWeight(card2));
}
bool operator > (const TCards& card1, const TCards& card2)
{
	return (GameRule::CardWeight(card1) > GameRule::CardWeight(card2));
}

bool cmp_card_min_max(TCards card1, TCards card2)
{
	return (GameRule::CardWeight(card1) < GameRule::CardWeight(card2));
}
bool cmp_card_max_mix(TCards card1, TCards card2)
{
	return (GameRule::CardWeight(card1) > GameRule::CardWeight(card2));
}
/*
* ����ְ�ܣ���dvCards��ǰnMaxCount����arrCardID������
*/
int ConvertDataVector(const TCardVector& dvCards, BYTE* arrCardID, const int& nMaxCount)
{
	int nCount = 0;
	for(int i=0; i<(int)dvCards.size(); i++)
	{
		if(nCount >= nMaxCount)
		{
			break;
		}
		nCount++;
		arrCardID[i] = dvCards[i].GetID();
	}
	return nCount;
}


//////////////////////////////////////////////////////////////////////////
//int	GameRule::snLevel = CV_2;

GameRule::GameRule()
{
}

GameRule::~GameRule()
{
}

void GameRule::InitData( int nLevel /*= CV_2*/ )
{
//	snLevel     = nLevel;
}

int GameRule::CardWeight(TCards card)
{
	int nWeight =0 ;
	const int nCardID			= card.GetID();
	const int nCardValue	= card.GetValue();
	if ( nCardID == CID_BJ || nCardID == CID_SJ)
	{
		nWeight =nCardValue+1;
	}else if (nCardValue == CV_2)
	{
		nWeight = CV_A+1;
	}
	else
	{
		nWeight = nCardValue;
	}
	return nWeight;
}

//////////////////////////////////////////////////////////////////////////
//ְ�ܣ���������
//������dvCardsָ����Ҫ��������飬bLessָ�����ͷ�ʽ
//		TUREΪ��С�������ͣ�falseΪ�Ӵ�С����
void GameRule::SortCards(TCardVector& dvCards, bool bLess /* true*/)
{
	if(bLess)
	{
		std::sort(dvCards.begin(), dvCards.end(), less<TCards>());
	}
	else
	{
		std::sort(dvCards.begin(), dvCards.end(), greater<TCards>());
	}
 }

//////////////////////////////////////////////////////////////////////////
//��switch�����д -- ����ͨ������������ȡ��������
#define  BEGIN_JUDGE_CARDSTYPE(judge) \
	switch(judge)\
{
#define  DECLARER_JUDGE_CARDSTYPE(judge,judgefun,dvCards,dvCardsInfo) \
case judge:\
{\
	judgefun(dvCards,dvCardsInfo);\
	break;\
}
#define END_JUDGE_CARDSTYPE(judgefun,dvCardInfo)\
default:\
	judgefun(dvCardInfo);\
	break;\
}
//��switch�����д
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//ְ�ܣ��ж������Ƿ�������
//������dvCardsָ����Ҫ�жϵ����顢cardsInfo���ڷ���������Ϣ
//���أ���������ƣ�����true�����򷵻�0
//��ע���㷨����Ϊ�Ȳ�������͹�ʽ��ͨ�ʽ
//		�Ȳ�����ͨ�ʽ��		An=a1+(n-1)*d
//		�Ȳ�����ǰN��͹�˾��	Sn=n(n-1)*d/2����=������+ĩ���������2
bool GameRule::IsMulti(const TCardVector& dvCards, CardTypeInfo& cardsInfo)
{
	int d = 1;
	size_t n = dvCards.size();
	vector<int> An;
	An.clear();
	for (size_t i=0; i<n; i++)
	{
		An.push_back(dvCards.at(i).GetValue());
	}
	
	if (CV_2 == An.back() || CV_BJ == An.front()|| CV_SJ == An.front()) //2�����ܷ�������
	{
		cardsInfo.reset();
		return false;
	}
	bool bfMulti = true;
	for(size_t i=0; i<n-1; i++)
	{
		if (An.at(i)-An.at(i+1)!= 1)
		{
			bfMulti = false;
		}
	}
	if (bfMulti)   //�Ȳ�������͹�ʽ
	{
		cardsInfo.nLevel = (An.front()+An.back())*n/2 ;    //�Ȳ�����ǰn��͹�ʽ
		cardsInfo.nCount = dvCards.size();
		cardsInfo.nType  = CT_MULTI;
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
//ְ�ܣ��ж������Ƿ��ǽ��ö�
//������dvCardsָ����Ҫ�жϵ����顢cardsInfo���ڷ���������Ϣ
//���أ���������ƣ�����true�����򷵻�0
//��ע���㷨����Ϊ�Ȳ�������͹�ʽ��ͨ�ʽ
//		�Ȳ�����ͨ�ʽ��		An=a1+(n-1)*d
//		�Ȳ�����ǰN��͹�ʽ��	Sn=n(n-1)*d/2����=������+ĩ���������2
bool GameRule::IsDoulbeMulti(const TCardVector& dvCards, CardTypeInfo& cardsInfo)
{
	if (dvCards.back().GetValue() == CV_2 || dvCards.size() <6) //2���ܷ�������,�������ϲſɳ�Ϊ����
	{
		cardsInfo.reset();
		return false;
	}
	size_t n = dvCards.size();
	vector<int> An;
	An.clear();
	TCardVector dvSplit1;
	TCardVector dvSplit2;
	dvSplit2.clear();
	dvSplit1.clear();
	for (size_t i=0; i<n; i++)
	{
		if (i%2 == 0)
		{
			dvSplit1.push_back(dvCards.at(i));
		}else
		{
			dvSplit2.push_back(dvCards.at(i));
		}
	}

	bool bfDM = true;
	for(size_t i=0; i<n/2-1; i++)
	{
		if ((dvSplit1.at(i).GetValue()) - (dvSplit1.at(i+1).GetValue()) !=  1 ||
			(dvSplit2.at(i).GetValue()) - (dvSplit2.at(i+1).GetValue()) !=  1 ||
			(dvSplit2.at(i).GetValue()) !=(dvSplit1.at(i).GetValue())
			)
		{
			bfDM = false;
		}
	}
	if (bfDM)   //�Ȳ�������͹�ʽ
	{
		cardsInfo.nLevel = (dvCards.front().GetValue()+dvCards.back().GetValue())*n ;    //�Ȳ�����ǰn��͹�ʽ
		cardsInfo.nCount = dvCards.size();
		cardsInfo.nType  = CT_MULTIDOUBLETWO;
		return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
//ְ�ܣ��ж������Ƿ������Ž��ö�
//������dvCardsָ����Ҫ�жϵ����顢cardsInfo���ڷ���������Ϣ
//���أ���������ƣ�����true�����򷵻�0
//��ע���㷨����Ϊ�Ȳ�������͹�ʽ��ͨ�ʽ
//		�Ȳ�����ͨ�ʽ��		An=a1+(n-1)*d
//		�Ȳ�����ǰN��͹�ʽ��	Sn=n(n-1)*d/2����=������+ĩ���������2
bool GameRule::IsThreeMulti(const TCardVector& dvCards, CardTypeInfo& cardsInfo)
{
	if (dvCards.back().GetValue() == CV_2 ) //2���ܷ�������
	{
		cardsInfo.reset();
		return false;
	}
	size_t n = dvCards.size();
	vector<int> An;
	An.clear();
	TCardVector dvSplit1;
	TCardVector dvSplit2;
	TCardVector dvSplit3;
	dvSplit2.clear();
	dvSplit1.clear();
	dvSplit3.clear();
	for (size_t i=0; i<n; i++)
	{
		if (i%3 == 0)
		{
			dvSplit1.push_back(dvCards.at(i));
		}else if (i%3 == 1)
		{
			dvSplit2.push_back(dvCards.at(i));
		}else
		{
			dvSplit3.push_back(dvCards.at(i));
		}
	}

	bool bfDM = true;
	for(size_t i=0; i<n/3-1; i++)
	{
		if ((dvSplit1.at(i).GetValue()) - (dvSplit1.at(i+1).GetValue()) !=  1 ||
			(dvSplit2.at(i).GetValue()) - (dvSplit2.at(i+1).GetValue()) !=  1 ||
			(dvSplit3.at(i).GetValue()) - (dvSplit3.at(i+1).GetValue()) !=  1 ||
			(dvSplit1.at(i).GetValue()) !=(dvSplit2.at(i).GetValue()) ||
			(dvSplit2.at(i).GetValue()) !=(dvSplit3.at(i).GetValue()) ||
			(dvSplit1.at(i).GetValue()) !=(dvSplit3.at(i).GetValue()))
		{
			bfDM = false;
		}
	}
	if (bfDM)   //�Ȳ�������͹�ʽ
	{
		cardsInfo.nLevel = (dvCards.front().GetValue()+dvCards.back().GetValue())*n/2*3 ;    //�Ȳ�����ǰn��͹�ʽ
		cardsInfo.nCount = dvCards.size();
		cardsInfo.nType  = CT_MULTITHREE;
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
//ְ�ܣ��ж�������С�ɻ�
//������dvCardsָ����Ҫ�жϵ����顢cardsInfo���ڷ���������Ϣ
//���أ������С�ɻ�������true�����򷵻�0
bool GameRule::IsMultiThreeWithOne(const TCardVector& dvCards, CardTypeInfo& cardInfo)
{
	TCardVector dvThree;			//���ڻ�ȡdvCards�����е�������
	TCardVector	dvMayBe;
	CardTypeInfo ctiMayBe;
	dvMayBe.clear();
	dvThree.clear();

	for (size_t i=0; i<=dvCards.size()- 3; i++)  //for1   
	{
		dvMayBe.clear();
		for (size_t j=i; j<3+i; j++)  //for2
		{
			if (!dvThree.empty() && dvCards.at(j) == dvThree.back())
			{
				break;	
			}
			dvMayBe.push_back(dvCards.at(j));
		} //end for2

		if (!dvMayBe.empty())
		{
			ctiMayBe = GameRule::GetCardType(dvMayBe);
			if (ctiMayBe.nType == CT_THREE  )
			{
				for (size_t n =i; n<i+3; n++)
				{
					dvThree.push_back(dvCards.at(n).GetID());
				}
			}
		}
	}  //end for 1
	
	size_t remainCards = dvCards.size()-dvThree.size();	//��ȥ���ź�ʣ�µ�����
	if (remainCards == dvCards.size()/4 && IsThreeMulti(dvThree,cardInfo))
	{
		cardInfo.nType  = CT_MULTITHREEWITHONE;
		cardInfo.nCount = dvCards.size(); 
		return true;
	}else if (remainCards < dvCards.size()/4)  //�п��ܴ���������
	{
		if (dvThree.front().GetValue()-dvThree.at(3).GetValue() == 1) 
		{
			for (int i = 0; i<3; i++)
			{
				dvThree.pop_back();
			}
			if (3+remainCards == dvCards.size()/4 && IsThreeMulti(dvThree,cardInfo))
			{
				cardInfo.nType  = CT_MULTITHREEWITHONE;
				cardInfo.nCount = dvCards.size(); 
				return true;
			}
		}else 
		{
			for (int i = 0; i<3; i++)
			{
				dvThree.erase(dvThree.begin());
			}
			if (3+remainCards == dvCards.size()/4 && IsThreeMulti(dvThree,cardInfo))
			{
				cardInfo.nType  = CT_MULTITHREEWITHONE;
				cardInfo.nCount = dvCards.size(); 
				return true;
			}
		}
	}
	cardInfo.reset();
	return false;
}

//////////////////////////////////////////////////////////////////////////
//ְ�ܣ��ж������Ƿ��Ǵ�ɻ�
//������dvCardsָ����Ҫ�жϵ����顢cardsInfo���ڷ���������Ϣ
//���أ�����Ǵ�ɻ�������true�����򷵻�0
bool GameRule::IsMultiThreeWithTwo(const TCardVector& dvCards, CardTypeInfo& cardInfo)
{
	if (dvCards.size()%5 != 0 || dvCards.back().GetValue() == CV_2
		&& dvCards.at(dvCards.size()-3) == CV_2)
	{
		cardInfo.reset();
		return false;
	}
	TCardVector dvThree;			//���ڻ�ȡdvCards�����е�������
	TCardVector	dvMayBe;
	TCardVector dvDouble;	
	CardTypeInfo ctiMayBe;
	dvDouble.clear();
	dvMayBe.clear();
	dvThree.clear();

	for (size_t i=0; i<=dvCards.size()- 3; i++)  //for1   
	{
		dvMayBe.clear();
		for (size_t j=i; j<3+i; j++)  //for2
		{
			dvMayBe.push_back(dvCards.at(j));  
		} //end for2

		if (!dvMayBe.empty())
		{
			ctiMayBe = GameRule::GetCardType(dvMayBe);
			if (ctiMayBe.nType == CT_THREE  )
			{
				for (size_t n =i; n<i+3; n++)
				{
					dvThree.push_back(dvCards.at(n).GetID());
				}
			}
		}
	}  //end for 1
	
	if (dvCards.size()-dvThree.size() != dvCards.size()/5*2)
	{
		cardInfo.reset();
		return false;
	}
	//��ɻ��У���ȥ���ž�Ӧ��ֻʣ��һ��
	dvDouble = dvCards;
	for (size_t i=0; i<dvThree.size(); i++)
	{
		dvDouble.erase(remove(dvDouble.begin(),dvDouble.end(),dvThree.at(i)),dvDouble.end());
	}

	for (size_t i=0; i<dvDouble.size(); i+=2)
	{
		if (dvDouble.at(i).GetValue() !=dvDouble.at(i+1).GetValue())
		{
			cardInfo.reset();
			return false;
		}
	}

	if (IsThreeMulti(dvThree,cardInfo))
	{
		cardInfo.nCount = dvCards.size();
		cardInfo.nType  = CT_MULTITHREEWITHTWO;
		return true;
	}
	cardInfo.reset();
	return false;
}

//////////////////////////////////////////////////////////////////////////
//ְ�ܣ��ж������Ƿ�������
//������dvCardsָ����Ҫ�жϵ����顢cardsInfo���ڷ���������Ϣ
//���أ���������ƣ�����true�����򷵻�0
//��ע���㷨����Ϊ�Ȳ�������͹�ʽ��ͨ�ʽ
//		�Ȳ�����ͨ�ʽ��		An=a1+(n-1)*d
//		�Ȳ�����ǰN��͹�˾��	Sn=n(n-1)*d/2����=������+ĩ���������2

//////////////////////////////////////////////////////////////////////////
//���������������жϺ�����
//�����������͵Ĺ�ϵ
// 1 -- ����
// 2 -- һ�ԡ���ը
// 3 -- ����
// 4 -- ը��������һ
// 5 -- ˳�ӡ�������
// 6 -- ˳�ӡ�˫˳����˳���Ĵ���
// 7 -- ˳��
// 8 -- ˳�ӡ�˫˳��С�ɻ�
// 9 -- ˳�ӡ���˳��
//10 -- ˳�ӡ�˫˳����ɻ�
//11 -- ˳��
//12 -- ˳�ӡ�˫˳����˳��С�ɻ�
//13 -- ����������
//14 -- ˫˳
//15 -- ��˳����ɻ�
//16 -- ˫˳��С�ɻ�
//17 -- ����������
//18 -- ˳�ӡ�˫˳����˳��
//19 -- ����������
//20 -- ˫˳����ɻ�
//2��������˳
// ������ְ�ܣ�ͨ�������������ȡ������
// �����������dvCardsָ����Ҫ�ж����͵����飬
//			   cardsInfo���ڽ��ջ�ȡ������Ϣ

//������һ����
//����
void OneCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	if (dvCards.empty())
	{
		cardsInfo.reset();
		return;
	}
	cardsInfo.nCount = (int)dvCards.size();
	cardsInfo.nLevel = GameRule::CardWeight(dvCards.front());
	cardsInfo.nType	 = CT_SINGLE;
}

//������������
// 2 -- һ�ԡ���ը
void TwoCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	if (dvCards.front().GetValue() == dvCards.back().GetValue()) //��������Ȳſ�Ϊ����
	{
		cardsInfo.nLevel   = GameRule::CardWeight(dvCards.front())*2;
		cardsInfo.nCount	= (int)dvCards.size();
		cardsInfo.nType		= CT_DOUBLE;
	}else if (dvCards.front().GetValue() == CV_BJ && 
					 dvCards.back().GetValue()== CV_SJ)  //һ�Թ���󣬶�Ȩֵ��������
	{
		cardsInfo.nCount = (int)dvCards.size();
		cardsInfo.nLevel = CV_BJ*4;
		cardsInfo.nType  = CT_MAXBOOM;
	}
	else
	{
		cardsInfo.reset();
	}
}
//������������
//����
void ThreeCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	if (dvCards.front().GetValue() != dvCards.back().GetValue())
	{
		cardsInfo.reset();
		return;
	}
	cardsInfo.nLevel = GameRule::CardWeight(dvCards.front())*3;
	cardsInfo.nType = CT_THREE;
	cardsInfo.nCount = (int)dvCards.size();
}

//������������
//ը��������һ��
void FourCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	if (dvCards.front().GetValue() == dvCards.back().GetValue()) //ը��
	{
		cardsInfo.nLevel = GameRule::CardWeight(dvCards.front())*4;
		cardsInfo.nType = CT_BOOM;
		cardsInfo.nCount = (int)dvCards.size();
	}else if (dvCards.front().GetValue() == dvCards.at(2).GetValue()||
		dvCards.back().GetValue() == dvCards.at(1).GetValue())		//����һ
	{
		cardsInfo.nLevel = GameRule::CardWeight(dvCards.at(1))*3; //0,1,2,3����������һ��ô����1��Ԫ����������֮һ
		cardsInfo.nCount = (int)dvCards.size();
		cardsInfo.nType  = CT_THREEWITHONE;
	}
	else 
	{
		cardsInfo.reset();
	}
}

//������������
//˳�ӡ�������
void FiveCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	//0,1,2,3,4 ������ô���У�2�������������ŵ�
	if ((dvCards.at(0).GetValue() == dvCards.at(2).GetValue() && dvCards.at(3).GetValue()==dvCards.at(4).GetValue()) ||
		 (dvCards.at(0).GetValue() == dvCards.at(1).GetValue() && dvCards.at(2).GetValue()==dvCards.at(4).GetValue()) )
	{
		cardsInfo.nCount = (int)dvCards.size();
		cardsInfo.nType = CT_THREEWITHTWO;
		cardsInfo.nLevel = GameRule::CardWeight(dvCards.at(2));
	}
	else if (GameRule::IsMulti(dvCards,cardsInfo))
	{
	}else
	{
		cardsInfo.reset();	
	}
}
//������������
//˳�ӡ�˫˳����˳���Ĵ���
void SixCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	DataVector<TCards>::iterator dit = dvCards.begin();
	//����
	if (GameRule::IsMulti(dvCards,cardsInfo))
	{
		return;
	}else if(GameRule::IsDoulbeMulti(dvCards,cardsInfo)) //���ö�
	{
		return;
	}else if (GameRule::IsThreeMulti(dvCards,cardsInfo)) //���Ž��ö�
	{
		return;
	}else 
	{
		for (int i=0; i<3; i++)   //�����Ĵ��� ��Ϊ���ŵ���
		{
			if (dvCards.at(i).GetValue() == dvCards.at(i+3).GetValue())  
			{
				cardsInfo.nLevel = GameRule::CardWeight(dvCards.at(i));
				cardsInfo.nType  = CT_FOURWITHTWO;
				cardsInfo.nCount = dvCards.size();
				return;
			}
		}
		cardsInfo.reset();
	}
}
//������������
//˳��
void SevenCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	GameRule::IsMulti(dvCards,cardsInfo);
}
//�����а�����
//˳�ӡ�˫˳���Ĵ�������Ϊһ�ԣ���С�ɻ�
void EightCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	if (GameRule::IsMulti(dvCards,cardsInfo))
	{
		return;
	}
	else if(GameRule::IsDoulbeMulti(dvCards,cardsInfo))
	{
		return;
	}
	else if (GameRule::IsMultiThreeWithOne(dvCards,cardsInfo))  //С�ɻ�
	{
		return;
	}
	/*�Ĵ������������*/
	else if (dvCards.front().GetValue() == dvCards.at(3).GetValue() &&
			 dvCards.at(4).GetValue() == dvCards.at(5).GetValue()   &&
			 dvCards.at(6).GetValue() == dvCards.at(7).GetValue())
	{
		cardsInfo.nType = CT_FOURWITHDTWO;
		cardsInfo.nLevel = GameRule::CardWeight(dvCards.front())*4;
		cardsInfo.nCount = (int)dvCards.size();

	}else if (dvCards.at(2).GetValue() == dvCards.at(5).GetValue() &&
		dvCards.at(0).GetValue() == dvCards.at(1).GetValue()   &&
		dvCards.at(6).GetValue() == dvCards.at(7).GetValue())
	{
		cardsInfo.nType = CT_FOURWITHDTWO;
		cardsInfo.nLevel = GameRule::CardWeight(dvCards.at(2))*4;
		cardsInfo.nCount = (int)dvCards.size();

	}else if (dvCards.at(4).GetValue() == dvCards.at(7).GetValue() &&
		dvCards.at(0).GetValue() == dvCards.at(1).GetValue()   &&
		dvCards.at(3).GetValue() == dvCards.at(2).GetValue())
	{
		cardsInfo.nLevel = GameRule::CardWeight(dvCards.at(4))*4;
		cardsInfo.nType = CT_FOURWITHDTWO;
		cardsInfo.nCount = (int)dvCards.size();
	}
	else
	{
		cardsInfo.reset();
	}
}
//�����о�����
//˳�ӡ���˳
void NineCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	if (GameRule::IsMulti(dvCards,cardsInfo))
	{
		return;
 	}else if(GameRule::IsThreeMulti(dvCards,cardsInfo))
 	{
 		return;
 	}else
	{
		cardsInfo.reset();
	}
}
//������ʮ����
//˳�ӡ�˫˳����ɻ�
void TenCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	if (GameRule::IsMulti(dvCards,cardsInfo))		//˳��
	{
		return;
	}else if(GameRule::IsDoulbeMulti(dvCards,cardsInfo))  //˫˳
	{
		return;
	}else if (GameRule::IsMultiThreeWithTwo(dvCards,cardsInfo))  //��ɻ�
	{
		return;
	}
}
//������ʮһ����
//˳��
void ElevenCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	GameRule::IsMulti(dvCards,cardsInfo);
}
//������ʮ������
//��˳��˫˳����˳��С�ɻ�
void TwelveCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	if (GameRule::IsMulti(dvCards,cardsInfo))
	{
		return ;
	}else if (GameRule::IsDoulbeMulti(dvCards,cardsInfo))
	{
		return;
	}else if (GameRule::IsThreeMulti(dvCards,cardsInfo))
	{
		return;
	}else if (GameRule::IsMultiThreeWithOne(dvCards,cardsInfo))
	{
		return;
	}
}
//������ʮ������
void ThirteenCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
}
//������ʮ������
//˫˳
void FourteenCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	if (GameRule::IsDoulbeMulti(dvCards,cardsInfo)) return;
}

//������ʮ������
//��˳����ɻ�
void FifteenCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	if (GameRule::IsThreeMulti(dvCards,cardsInfo)) return;
	if (GameRule::IsMultiThreeWithTwo(dvCards,cardsInfo)) return;
}

//������ʮ������
//˳�ӡ�˫˳��С�ɻ�
void SixteenCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	if (GameRule::IsDoulbeMulti(dvCards,cardsInfo)) return;
	if (GameRule::IsMultiThreeWithOne(dvCards,cardsInfo)) return;
}

//������ʮ������
void SeventeenCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
}

//������ʮ������
//˫˳����˳
void EighteenCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	if (GameRule::IsDoulbeMulti(dvCards,cardsInfo)) return;
	if (GameRule::IsThreeMulti(dvCards,cardsInfo)) return;
}

//������ʮ������
void NineTeenCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
}

//������20����
//˫˳����ɻ�
void TwentyCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	if (GameRule::IsDoulbeMulti(dvCards,cardsInfo)) return;
	if (GameRule::IsThreeMulti(dvCards,cardsInfo)) return;
}
//��������������
void ErrorCard(CardTypeInfo& cardsInfo)
{
	cardsInfo.reset();
}



//���������������жϺ�����
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//ְ�ܣ���ȡ��������
//������dvCardsָ����Ҫ��ȡ���͵�����
//���أ��ɹ���ȡ����󣬽�������Ϣ����������Ϣ�෵��
//		������nLevel��Ȩֵ
CardTypeInfo  GameRule::GetCardType(const TCardVector& dvCards)
{
	CardTypeInfo cInfo;
	cInfo.reset();
	if (dvCards.empty())
	{
		return cInfo;
	}

	TCardVector dvCardTmp;
	dvCardTmp = dvCards;

	SortCards(dvCardTmp,false);
	
	//������������ȡ����Ϣ
	BEGIN_JUDGE_CARDSTYPE(dvCardTmp.size())
	DECLARER_JUDGE_CARDSTYPE(1,OneCard,dvCardTmp,cInfo)
	DECLARER_JUDGE_CARDSTYPE(2,TwoCard,dvCardTmp,cInfo)
	DECLARER_JUDGE_CARDSTYPE(3,ThreeCard,dvCardTmp,cInfo)
	DECLARER_JUDGE_CARDSTYPE(4,FourCard,dvCardTmp,cInfo)
	DECLARER_JUDGE_CARDSTYPE(5,FiveCard,dvCardTmp,cInfo)
	DECLARER_JUDGE_CARDSTYPE(6,SixCard,dvCardTmp,cInfo)
	DECLARER_JUDGE_CARDSTYPE(7,SevenCard,dvCardTmp,cInfo)
	DECLARER_JUDGE_CARDSTYPE(8,EightCard,dvCardTmp,cInfo)
	DECLARER_JUDGE_CARDSTYPE(9,NineCard,dvCardTmp,cInfo)
	DECLARER_JUDGE_CARDSTYPE(10,TenCard,dvCardTmp,cInfo)
	DECLARER_JUDGE_CARDSTYPE(11,ElevenCard,dvCardTmp,cInfo)
	DECLARER_JUDGE_CARDSTYPE(12,TwelveCard,dvCardTmp,cInfo)
	DECLARER_JUDGE_CARDSTYPE(14,FourteenCard,dvCardTmp,cInfo)
	DECLARER_JUDGE_CARDSTYPE(15,FifteenCard,dvCardTmp,cInfo)
	DECLARER_JUDGE_CARDSTYPE(16,SixteenCard,dvCardTmp,cInfo)
	DECLARER_JUDGE_CARDSTYPE(18,EighteenCard,dvCardTmp,cInfo)
	DECLARER_JUDGE_CARDSTYPE(20,TwentyCard,dvCardTmp,cInfo)
	END_JUDGE_CARDSTYPE(ErrorCard,cInfo)
	return cInfo;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//ְ�ܣ��жϴ�������Ƿ���Գ���
//������bFirstOutȷ���Ƿ��һ�ֳ��ƣ���һ�ֳ��ƿɳ���������
//		dvFirstOutCardsָ�����ƴ������
//		dvNowOutCardsָ����Ҫ�жϵĽ�����ҵ����顣
//���أ����������ƣ������棬�����
//��ע�� ȷ���Ƿ�������Ƶĺ���˼��
//	����1.ը�����κη�ը������
//	����2.���ͣ�������������ͬ�����������ֵ֮��������������
//	����3.ը����ը����ѭ����1
//	����4.����������ѭ����1����ȽϹ���Ϊ��ͬ�������Ƶ���ֵ�Ƚϡ�
//		  �����С��С
//	����5.����һ�ֳ��ƻ������ʹ��ʱ������Գ��ƣ���һ�ֳ���ָ��
//        ��Ϸ�ж��ַ������ƻ��߿��ֵ�һ�γ���
// 
bool  GameRule::CanOut(bool bFirstOut, const TCardVector& dvFirstOutCards,
				   const TCardVector& dvNowOutCards)
{
	CardTypeInfo infoFirstCard = GameRule::GetCardType(dvFirstOutCards); //��ȡ�������������Ϣ
	CardTypeInfo infoSecondCard= GameRule::GetCardType(dvNowOutCards); //��ȡѹ�����������Ϣ

	if (bFirstOut) 
	{
		if (infoSecondCard.nType != CT_NONE) return true;
		return false;
	}else if (dvNowOutCards.empty())
	{
		return false;
	}else if (dvFirstOutCards.empty())//��ҳ���һ����
	{
		if (infoSecondCard.nType == CT_NONE)
		{
			return false;
		}
		return true;
	}

	if (infoSecondCard.nType == CT_MAXBOOM)
	{
		return true;
	}
	if (infoFirstCard.nType == CT_MAXBOOM)
	{
		return false;
	}
	if (infoSecondCard.nType == CT_BOOM)
	{
		if (infoFirstCard.nType != CT_BOOM )
		{
			return true;
		}else if (infoSecondCard.nLevel > infoFirstCard.nLevel)
		{
			return true;
		}
	}

	if (infoFirstCard.nType != infoSecondCard.nType)   //���Ͳ�ͬ����ը���ⲻ�ɳ���
	{
		if (infoSecondCard.nType == CT_BOOM)
		{
			return true;
		}
		return false;
	}else if (infoSecondCard.nCount == infoFirstCard.nCount)		//������ͬ����������ͬ
	{
		if (infoFirstCard.nLevel < infoSecondCard.nLevel)
		{
			return true;
		}
		return false;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
//ְ�ܣ�ȷ���Ƿ�ӵ�е�һ�ֳ����ʸ�
//������pdvHandCardsָ��������ҵ�����
//		nSizeָ������ָ���С
//���أ����ӵ�е�һ�ֳ����ʸ񣬷�����λ������ָ���е�
//		λ��
//��ע���˴��ĵ�һ���ж��ʸ�Ϊ���Ὺ�ֵĵ�һ�γ����ʸ�
//		�����������Ժ���-���ҵ�һ���У���3Ϊ��׼���Ƶ�
//		Ϊ�ڶ����У��ǰ�ߵõ�һ�����ʸ�
#include <map>
int  GameRule::FirstOutCard(const TCardVector  pdvHandCards[], int nSize)
{
	if (NULL == pdvHandCards || nSize < 3)
	{
		return -1;  
	}
	int nIndex = 0;
	for (nIndex = 0;nIndex<nSize-1; nIndex++)
	{
		for(int i=0; i <(int)pdvHandCards[nIndex].size(); i++)
 		{
			int cdColor = (pdvHandCards[nIndex]).at(i).GetColor();
			int cdValue = (pdvHandCards[nIndex]).at(i).GetValue();
			if (3 == nSize) //���������ʱ��ض���һ��ӵ�к���3
			{
				if (PC_SPADE == cdColor && CV_3 ==cdValue )
				{
					return nIndex;
				}
			}else			//��������,��ʵ��,�ݶ�ʵ��3�˹���
			{
				//if (PC_SPADE == cdColor && CV_3 ==cdValue )
				//{
				//	dvTmp.insert(pair<int,int>(nIndex, i));
				//}
				srand((unsigned int)time(NULL));
				return rand()%2;
			}
		}
	}

	return nIndex;
}
