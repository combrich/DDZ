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
* 函数职能：将dvCards中前nMaxCount放入arrCardID数组中
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
//职能：排序牌组
//参数：dvCards指定需要排序的牌组，bLess指定牌型方式
//		TURE为从小到大牌型，false为从大到小排序
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
//大switch语句缩写 -- 用于通过牌组张数获取牌组类型
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
//大switch语句缩写
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//职能：判断牌型是否是连牌
//参数：dvCards指定需要判断的牌组、cardsInfo用于返回牌组信息
//返回：如果是连牌，返回true，否则返回0
//附注：算法核心为等差数列求和公式及通项公式
//		等差数列通项公式：		An=a1+(n-1)*d
//		等差数列前N项和公司：	Sn=n(n-1)*d/2、和=（首项+末项）×项数÷2
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
	
	if (CV_2 == An.back() || CV_BJ == An.front()|| CV_SJ == An.front()) //2、鬼不能放入连牌
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
	if (bfMulti)   //等差数列求和公式
	{
		cardsInfo.nLevel = (An.front()+An.back())*n/2 ;    //等差数列前n项和公式
		cardsInfo.nCount = dvCards.size();
		cardsInfo.nType  = CT_MULTI;
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
//职能：判断牌型是否是姐妹对
//参数：dvCards指定需要判断的牌组、cardsInfo用于返回牌组信息
//返回：如果是连牌，返回true，否则返回0
//附注：算法核心为等差数列求和公式及通项公式
//		等差数列通项公式：		An=a1+(n-1)*d
//		等差数列前N项和公式：	Sn=n(n-1)*d/2、和=（首项+末项）×项数÷2
bool GameRule::IsDoulbeMulti(const TCardVector& dvCards, CardTypeInfo& cardsInfo)
{
	if (dvCards.back().GetValue() == CV_2 || dvCards.size() <6) //2不能放入连牌,三队以上才可称为连牌
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
	if (bfDM)   //等差数列求和公式
	{
		cardsInfo.nLevel = (dvCards.front().GetValue()+dvCards.back().GetValue())*n ;    //等差数列前n项和公式
		cardsInfo.nCount = dvCards.size();
		cardsInfo.nType  = CT_MULTIDOUBLETWO;
		return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
//职能：判断牌型是否是三张姐妹对
//参数：dvCards指定需要判断的牌组、cardsInfo用于返回牌组信息
//返回：如果是连牌，返回true，否则返回0
//附注：算法核心为等差数列求和公式及通项公式
//		等差数列通项公式：		An=a1+(n-1)*d
//		等差数列前N项和公式：	Sn=n(n-1)*d/2、和=（首项+末项）×项数÷2
bool GameRule::IsThreeMulti(const TCardVector& dvCards, CardTypeInfo& cardsInfo)
{
	if (dvCards.back().GetValue() == CV_2 ) //2不能放入连牌
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
	if (bfDM)   //等差数列求和公式
	{
		cardsInfo.nLevel = (dvCards.front().GetValue()+dvCards.back().GetValue())*n/2*3 ;    //等差数列前n项和公式
		cardsInfo.nCount = dvCards.size();
		cardsInfo.nType  = CT_MULTITHREE;
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
//职能：判断牌型是小飞机
//参数：dvCards指定需要判断的牌组、cardsInfo用于返回牌组信息
//返回：如果是小飞机，返回true，否则返回0
bool GameRule::IsMultiThreeWithOne(const TCardVector& dvCards, CardTypeInfo& cardInfo)
{
	TCardVector dvThree;			//用于获取dvCards中所有的三张牌
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
	
	size_t remainCards = dvCards.size()-dvThree.size();	//除去三张后还剩下的牌型
	if (remainCards == dvCards.size()/4 && IsThreeMulti(dvThree,cardInfo))
	{
		cardInfo.nType  = CT_MULTITHREEWITHONE;
		cardInfo.nCount = dvCards.size(); 
		return true;
	}else if (remainCards < dvCards.size()/4)  //有可能带出的牌型
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
//职能：判断牌型是否是大飞机
//参数：dvCards指定需要判断的牌组、cardsInfo用于返回牌组信息
//返回：如果是大飞机，返回true，否则返回0
bool GameRule::IsMultiThreeWithTwo(const TCardVector& dvCards, CardTypeInfo& cardInfo)
{
	if (dvCards.size()%5 != 0 || dvCards.back().GetValue() == CV_2
		&& dvCards.at(dvCards.size()-3) == CV_2)
	{
		cardInfo.reset();
		return false;
	}
	TCardVector dvThree;			//用于获取dvCards中所有的三张牌
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
	//大飞机中，除去三张就应当只剩下一对
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
//职能：判断牌型是否是连牌
//参数：dvCards指定需要判断的牌组、cardsInfo用于返回牌组信息
//返回：如果是连牌，返回true，否则返回0
//附注：算法核心为等差数列求和公式及通项公式
//		等差数列通项公式：		An=a1+(n-1)*d
//		等差数列前N项和公司：	Sn=n(n-1)*d/2、和=（首项+末项）×项数÷2

//////////////////////////////////////////////////////////////////////////
//以下是牌组类型判断函数组
//牌张数和牌型的关系
// 1 -- 单牌
// 2 -- 一对、鬼炸
// 3 -- 三张
// 4 -- 炸弹、三带一
// 5 -- 顺子、三带二
// 6 -- 顺子、双顺、三顺、四带二
// 7 -- 顺子
// 8 -- 顺子、双顺、小飞机
// 9 -- 顺子、三顺、
//10 -- 顺子、双顺、大飞机
//11 -- 顺子
//12 -- 顺子、双顺、三顺、小飞机
//13 -- 不可能组牌
//14 -- 双顺
//15 -- 三顺、大飞机
//16 -- 双顺、小飞机
//17 -- 不可能组牌
//18 -- 顺子、双顺、三顺、
//19 -- 不可能组牌
//20 -- 双顺、大飞机
//2、鬼不可组顺
// 函数组职能：通过牌组的张数获取牌类型
// 函数组参数：dvCards指定需要判断牌型的牌组，
//			   cardsInfo用于接收获取牌型信息

//牌组有一张牌
//单牌
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

//牌组有两张牌
// 2 -- 一对、鬼炸
void TwoCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	if (dvCards.front().GetValue() == dvCards.back().GetValue()) //两张牌相等才可为对子
	{
		cardsInfo.nLevel   = GameRule::CardWeight(dvCards.front())*2;
		cardsInfo.nCount	= (int)dvCards.size();
		cardsInfo.nType		= CT_DOUBLE;
	}else if (dvCards.front().GetValue() == CV_BJ && 
					 dvCards.back().GetValue()== CV_SJ)  //一对鬼最大，对权值进行提升
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
//牌组有三张牌
//三张
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

//牌组有四张牌
//炸弹、三带一、
void FourCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	if (dvCards.front().GetValue() == dvCards.back().GetValue()) //炸弹
	{
		cardsInfo.nLevel = GameRule::CardWeight(dvCards.front())*4;
		cardsInfo.nType = CT_BOOM;
		cardsInfo.nCount = (int)dvCards.size();
	}else if (dvCards.front().GetValue() == dvCards.at(2).GetValue()||
		dvCards.back().GetValue() == dvCards.at(1).GetValue())		//三带一
	{
		cardsInfo.nLevel = GameRule::CardWeight(dvCards.at(1))*3; //0,1,2,3，不管三带一怎么带，1号元素总是三张之一
		cardsInfo.nCount = (int)dvCards.size();
		cardsInfo.nType  = CT_THREEWITHONE;
	}
	else 
	{
		cardsInfo.reset();
	}
}

//牌组有五张牌
//顺子、三带二
void FiveCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	//0,1,2,3,4 不论怎么排列，2号总是属于三张的
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
//牌组有六张牌
//顺子、双顺、三顺、四带二
void SixCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	DataVector<TCards>::iterator dit = dvCards.begin();
	//连牌
	if (GameRule::IsMulti(dvCards,cardsInfo))
	{
		return;
	}else if(GameRule::IsDoulbeMulti(dvCards,cardsInfo)) //姐妹对
	{
		return;
	}else if (GameRule::IsThreeMulti(dvCards,cardsInfo)) //三张姐妹对
	{
		return;
	}else 
	{
		for (int i=0; i<3; i++)   //处理四带二 二为两张单牌
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
//牌组有七张牌
//顺子
void SevenCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	GameRule::IsMulti(dvCards,cardsInfo);
}
//牌组有八张牌
//顺子、双顺、四带二（二为一对）、小飞机
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
	else if (GameRule::IsMultiThreeWithOne(dvCards,cardsInfo))  //小飞机
	{
		return;
	}
	/*四带二的三种情况*/
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
//牌组有九张牌
//顺子、三顺
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
//牌组有十张牌
//顺子、双顺、大飞机
void TenCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	if (GameRule::IsMulti(dvCards,cardsInfo))		//顺子
	{
		return;
	}else if(GameRule::IsDoulbeMulti(dvCards,cardsInfo))  //双顺
	{
		return;
	}else if (GameRule::IsMultiThreeWithTwo(dvCards,cardsInfo))  //大飞机
	{
		return;
	}
}
//牌组有十一张牌
//顺子
void ElevenCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	GameRule::IsMulti(dvCards,cardsInfo);
}
//牌组有十二张牌
//单顺、双顺、三顺、小飞机
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
//牌组有十三张牌
void ThirteenCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
}
//牌组有十四张牌
//双顺
void FourteenCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	if (GameRule::IsDoulbeMulti(dvCards,cardsInfo)) return;
}

//牌组有十五张牌
//三顺、大飞机
void FifteenCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	if (GameRule::IsThreeMulti(dvCards,cardsInfo)) return;
	if (GameRule::IsMultiThreeWithTwo(dvCards,cardsInfo)) return;
}

//牌组有十六张牌
//顺子、双顺、小飞机
void SixteenCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	if (GameRule::IsDoulbeMulti(dvCards,cardsInfo)) return;
	if (GameRule::IsMultiThreeWithOne(dvCards,cardsInfo)) return;
}

//牌组有十七张牌
void SeventeenCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
}

//牌组有十八张牌
//双顺、三顺
void EighteenCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	if (GameRule::IsDoulbeMulti(dvCards,cardsInfo)) return;
	if (GameRule::IsThreeMulti(dvCards,cardsInfo)) return;
}

//牌组有十九张牌
void NineTeenCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
}

//牌组有20张牌
//双顺、大飞机
void TwentyCard(TCardVector dvCards, CardTypeInfo& cardsInfo)
{
	if (GameRule::IsDoulbeMulti(dvCards,cardsInfo)) return;
	if (GameRule::IsThreeMulti(dvCards,cardsInfo)) return;
}
//牌组牌张数错误
void ErrorCard(CardTypeInfo& cardsInfo)
{
	cardsInfo.reset();
}



//以上是牌组类型判断函数组
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//职能：获取牌组类型
//参数：dvCards指定需要获取类型的牌组
//返回：成功获取牌组后，将牌组信息放入牌组信息类返回
//		并设置nLevel的权值
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
	
	//按照牌张数获取牌信息
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
//职能：判断打出的牌是否可以出牌
//参数：bFirstOut确定是否第一手出牌，第一手出牌可出随意牌组
//		dvFirstOutCards指定出牌打出的牌
//		dvNowOutCards指定需要判断的接牌玩家的牌组。
//返回：如果允许出牌，返回真，否则假
//附注： 确定是否允许出牌的核心思想
//	规则1.炸弹比任何非炸弹都大
//	规则2.牌型，牌张数两者相同，则牌组的牌值之和最大，则牌组最大
//	规则3.炸弹和炸弹遵循规则1
//	规则4.三带二不遵循规则1，其比较规则为相同的三张牌的牌值比较。
//		  大则大，小则小
//	规则5.当第一手出牌或者牌型大的时候则可以出牌，第一手出牌指代
//        游戏中对手放弃出牌或者开局第一次出牌
// 
bool  GameRule::CanOut(bool bFirstOut, const TCardVector& dvFirstOutCards,
				   const TCardVector& dvNowOutCards)
{
	CardTypeInfo infoFirstCard = GameRule::GetCardType(dvFirstOutCards); //获取出牌玩家牌型信息
	CardTypeInfo infoSecondCard= GameRule::GetCardType(dvNowOutCards); //获取压牌玩家牌型信息

	if (bFirstOut) 
	{
		if (infoSecondCard.nType != CT_NONE) return true;
		return false;
	}else if (dvNowOutCards.empty())
	{
		return false;
	}else if (dvFirstOutCards.empty())//玩家出第一手牌
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

	if (infoFirstCard.nType != infoSecondCard.nType)   //牌型不同，除炸弹外不可出牌
	{
		if (infoSecondCard.nType == CT_BOOM)
		{
			return true;
		}
		return false;
	}else if (infoSecondCard.nCount == infoFirstCard.nCount)		//牌型相同，且张数相同
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
//职能：确定是否拥有第一手出牌资格
//参数：pdvHandCards指定所有玩家的手牌
//		nSize指定牌组指针大小
//返回：如果拥有第一手出牌资格，返回其位于牌组指针中的
//		位置
//附注：此处的第一手判断资格为争夺开局的第一次出牌资格
//		争抢条件，以黑桃-红桃第一序列，以3为基准的牌点
//		为第二序列，最靠前者得第一出牌资格
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
			if (3 == nSize) //三个人玩的时候必定有一人拥有黑桃3
			{
				if (PC_SPADE == cdColor && CV_3 ==cdValue )
				{
					return nIndex;
				}
			}else			//两个人玩,待实现,暂定实现3人关牌
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
