/*
　　　　　　　　┏┓　　　┏┓ 
　　　　　　　┏┛┻━━━┛┻┓ 
　　　　　　　┃　　　　　　　┃ 　 
　　　　　　　┃　　　━　　　┃ 
　　　　　　　┃　┳┛　┗┳　┃ 
　　　　　　　┃　　　　　　　┃ 
　　　　　　　┃　　　┻　　　┃ 
　　　　　　　┃　　　　　　　┃ 
　　　　　　　┗━┓　　　┏━┛ 
　　　　　　　　　┃　　　┃神兽庇佑，代码无bug　　　　　　　　　 
　　　　　　　　　┃　　　┃code is far away from bug with the animal protecting 
　　　　　　　　　┃　　　┗━━━┓ 
　　　　　　　　　┃　　　　　　　┣┓ 
　　　　　　　　　┃　　　　　　　┏┛ 
　　　　　　　　　┗┓┓┏━┳┓┏┛ 
　　　　　　　　　　┃┫┫　┃┫┫ 
　　　　　　　　　　┗┻┛　┗┻┛ 
*/
#include "Robot.h"

DebugOut cDebugOut;

////////////////////////////////////////////////////////////////////////
//对CardsVector排序的排序算子
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
//对TCardVector排序的排序算子
bool cmpTV(TCards T1,TCards T2)
{
	int nT1 = GameRule::CardWeight(T1);
	int nT2 = GameRule::CardWeight(T2);
	return nT1<nT2?true:false;
}

//对TCardVector排序的排序算子由打到小
bool cmpMaxMin(TCards T1,TCards T2)
{
	int nT1 = GameRule::CardWeight(T1);
	int nT2 = GameRule::CardWeight(T2);
	return nT1>nT2?true:false;
}
//结构体函数
void CARDSTYLE::Fill(TCardVector dv,int nType)
{
	sort(dv.begin(),dv.end(),cmpTV);
	max = dv.back().GetValue();
	min = dv.front().GetValue();
	cards = dv;
	switch(nType)
	{
	case CT_BOOM: //炸弹
		m_value = nLevelBoom;
		break;

	case CT_THREE: //三张
		m_value = nLevelThree;
		break;

	case CT_DOUBLE: //对子
		m_value = nLevelDouble;
		break;

	case CT_MULTITHREE: //三顺
		m_value = dv.size();
		break;

	case CT_MULTIDOUBLETWO: //双顺
		m_value = dv.size();
		break;

	case CT_MULTI:	//顺子
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


//单张排序算子
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
//职能：机器人抢地主
//说明：机器人抢地主实际上只是计算下自己手中
//          大牌的权值，是否强地主交给游戏逻辑决定
//打牌权值定义：炸弹为5 大王为5，小王为3, 2为2,A为1
//参数：参数指定需要计算权值的牌
//返回：返回指定牌组的权值
int  CRobot::RobotGrabLand(TCardVector dvHand)
{
	if (dvHand.empty())
	{
		return -1;
	}
	int nLevel =0;
	sort(dvHand.begin(),dvHand.end(),cmpTV);
	//火箭
	TCardVector tmp;
	tmp.push_back(dvHand.back());
	tmp.push_back(dvHand.at(dvHand.size()-2));
	CardTypeInfo cti = GameRule::GetCardType(tmp);
	if (cti.nType == CT_MAXBOOM) nLevel += 5;
	//炸弹
	CardsVector boom;
	this->ChoseBoom(boom,dvHand);
	nLevel += 5 * boom.size();
	//鬼牌
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
	//牌型权值定义：
	//	大王=5，小王=3，2=2，A=1，炸弹=5
	BYTE topvalue = 5 * theCallIn.val[CARD_BJ] + 3 * theCallIn.val[CARD_SJ] + 
		            2 * theCallIn.val[CARD_2]  + theCallIn.val[CARD_A];
	for (int i=CARD_3;i<CARD_K;i++)
	{
		if (theCallIn.val[i] == 4)
		{
			topvalue += 5;
		}
	}
	//抢地主策略
	//1.权值大于
	/*
		牌型Lev权值定义：
		大王=5，小王=3，2=2，A=1，炸弹=5
		８　８　４　５×ｎ
		if 牌值小于等于4  不叫，叫一分
		if 牌值大于4小于等于8  叫一分，两分
		if 牌值大于8小于等于11 将地主，叫一分，两分，三分
		if 牌值大于11          叫三分
	*/
	srand(time(0));
	int nCharacter = rand()%100;
	if ( nCharacter <= nBaseRand)  //默认%80的情况电脑将会根据牌型抢地主
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
	}else if (nCharacter > nBaseRand && nCharacter <= (nBaseRand+(100-nBaseRand)/2))  //默认%10无脑抢地主
	{
		return 3;
	}else   //永远不抢地主 
	{
		return 0;
	}
	return 0;
}

bool CRobot::RobotCall( AICallIn theCallIn, AICallOut& theCallOut, int nBaseRand /*=80*/)
{
	theCallOut.returnscore = 0;

	//牌型权值定义：
	//	大王=5，小王=3，2=2，A=1，炸弹=5
	BYTE topvalue = 5 * theCallIn.val[CARD_BJ] + 3 * theCallIn.val[CARD_SJ] + 
		            2 * theCallIn.val[CARD_2]  + theCallIn.val[CARD_A];
	for (int i=CARD_3;i<CARD_K;i++)
	{
		if (theCallIn.val[i] == 4)
		{
			topvalue += 5;
		}
	}
	//抢地主策略
	//1.权值大于
	/*
		牌型Lev权值定义：
		大王=5，小王=3，2=2，A=1，炸弹=5

		８　８　４　５×ｎ

		if 牌值小于等于4  不叫、不抢
		if 牌值大于4小于等于8  叫地主，1，
		if 牌值大于8小于等于11 将地主，1，2
		if 牌值大于11          无条件抢地主
	*/
	srand(time(0));
	int nCharacter = rand()%100;
	if ( nCharacter <= nBaseRand)  //默认%80的情况电脑将会根据牌型抢地主
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
	}else if (nCharacter > nBaseRand && nCharacter <= (nBaseRand+(100-nBaseRand)/2))  //默认%10无脑抢地主
	{
		theCallOut.returnscore = theCallIn.callscore;
	}else   //永远不抢地主 
	{
		theCallOut.returnscore = theCallIn.callscore;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
//函数职能：机器人出牌
//参数：	handCards玩家手中的牌,nSeat指定出牌玩家位置
//返回:		返回需要打出的牌，该函数不存在也不允许失败。
//by：      tmfish  at 2014-03-29
TCardVector CRobot::RobotPlayCards( TCardVector handCards[MAX_PLAYER], int nSeat)
{
	//也许有的时候，最后可以一把打完所有的牌，这种情况下不进行分析
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
		//确认警报级别
		m_Warring[i].nSeat=i;
		m_Warring[i].nWL  = m_dvPlayerCards[i].size();
	}

	m_nOutSeat = nSeat;
	//分析手牌
	for (int i=0; i<MAX_PLAYER; i++)
	{
		ShowDebug(m_dvPlayerCards[i],"玩家手牌");
		cvTmpJoker.clear();
		sort(m_dvPlayerCards[i].begin(),m_dvPlayerCards[i].end(),cmpTV);
		//大小王给去掉
		while (!m_dvPlayerCards[i].empty() && m_dvPlayerCards[i].back().GetValue() > CV_A)
		{
			cvTmpJoker.push_back(m_dvPlayerCards[i].back());
			m_dvPlayerCards[i].pop_back();
		}
		int nLevel = this->AnalyzeCards(m_dvPlayerCards[i],i);

		m_dvPlayerCards[i] += cvTmpJoker;
	}
	//debug
	this->ShowDebug(m_PlayerSplit[m_nOutSeat],"拆牌为");
	//debug
	return this->FindNowOut();
}

//计算指定牌组中各个牌值的数量
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
//函数职能：机器人跟牌
//参数说明：参数1指定所有玩家的手牌，参数2指定需要压的牌，参数3指定需要压的牌是谁打出的
//                 参数4用于返回跟牌，参数5指定是谁在跟牌
//返回：       如果机器人成功找到压牌，返回true，如果返回false即不压牌
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
		//确认警报级别
		m_Warring[i].nSeat=i;
		m_Warring[i].nWL  = m_dvPlayerCards[i].size();
	}
	//寻找当前玩家手牌中最大的三张牌
	this->FindMaxThreeCards();

	//分析手牌
	for (int i=0; i<MAX_PLAYER; i++)
	{
		cvTmpJoker.clear();
		sort(m_dvPlayerCards[i].begin(),m_dvPlayerCards[i].end(),cmpTV);
		//大小王给去掉
		while (!m_dvPlayerCards[i].empty() && m_dvPlayerCards[i].back().GetValue() > CV_A)
		{
			cvTmpJoker.push_back(m_dvPlayerCards[i].back());
			m_dvPlayerCards[i].pop_back();
		}
		int nLevel = this->AnalyzeCards(m_dvPlayerCards[i],i);
		//把大小王给加回来
		m_dvPlayerCards[i] += cvTmpJoker;
	}
	sort(dvOut.begin(),dvOut.end(),cmpTV);
	this->ShowDebug(m_dvBooms,"炸弹");

	bool bFlag = FindNowFollow(dvOut,dvRes);
	return bFlag;
}

//参数指定待转换的牌组，参数2返回转换后的结果
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
//参数指定待转换的牌组，参数2返回转换后的结果
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
//逻辑函数
//分析牌
	/*
	a) 先确定炸弹（轻重炸弹）
	b）再确定三条
	c）再确定三顺
	d）再确定单顺
	e) 再确定双顺
	f）再确定对子
	e) 再确定单牌
	*/
int	 CRobot::AnalyzeCards(TCardVector dvHandCards, int nSeat)
{
	this->ResetTmp();
	TCardVector tmpHandCards;
	sort(dvHandCards.begin(),dvHandCards.end(),cmpTV);	
	TCardVector dvAnalyCards = dvHandCards;
	//炸弹
	this->ChoseBoom(cvTmpBoom,tmpHandCards);
	tmpHandCards = dvAnalyCards;
	//三张
	this->ChoseThree(cvTmpThree,tmpHandCards);
	tmpHandCards = dvAnalyCards;
	//三顺
	this->ChoseThreeMultis(cvTmpThreeMulti,cvTmpThree);
	tmpHandCards = dvAnalyCards;
	//对子
	this->ChoseDouble(cvTmpDouble,tmpHandCards);
	tmpHandCards = dvAnalyCards;
	//双顺
	this->ChoseDoubleMultis(cvTmpDoubleMulti,cvTmpDouble);
	//单顺
	tmpHandCards = dvAnalyCards;
	this->ChoseSingleMultis(cvTmpSingleMulti,tmpHandCards,cvTmpDoubleMulti);
	//遍历拆牌
	int tempbosb,tempplane,tempLinkdouble,tempthree,tempdouble,tmpSingleMulti;

	tmpSingleMulti = cvTmpSingleMulti.empty()?0:1;
	tempbosb	= cvTmpBoom.empty()?0:1;
	tempplane	= cvTmpThreeMulti.empty()?0:1;
	tempLinkdouble = cvTmpDoubleMulti.empty()?0:1;
	tempthree	= cvTmpThree.empty()?0:1;
	tempdouble  = cvTmpDoubleMulti.empty()?0:1;
	for (int i=0;i<=tempbosb;i++)//炸弹
	{
		for (int j=0;j<=tempplane;j++)//三顺
		{
			for (int k=0;k<=tempLinkdouble;k++)//连队
			{
				for (int f=0;f<=tempthree;f++)//3条
				{
					for (int h=0;h<=tempdouble;h++)//对子
					{
						for (int n=0;n<=tmpSingleMulti;n++)//单顺
						{
							ErgodicCards(i,j,k,f,h,n,nSeat);
						}
					}
				}
			}
		}
	}

	//根据手数和权值挑选出最优的拆牌方式
	FindOptimalSplit(nSeat);


	//分析到出牌玩家的手数
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
		//计算手数
	}
	return m_PlayerSplit[nSeat].GetHandCount();
}


//////////////////////////////////////////////////////////////////////////
//选取单顺
//选取规则
/* 
	先取出最小的一个五连单顺，再在剩余的牌中选取最小的一个五连。依次类推，直到没有五连为止。
	完成后对五连进行扩展，将剩余的牌与已经取出的牌进行比对，如果某张剩余的牌与已知的连牌能组
	成更大的连牌，则将其合并。一直到无法合并为止。 最后，如果某两组连牌能够无缝连接成为更大
	的连牌，则将其合并为一组。
*/
void CRobot::ChoseSingleMultis(CardsVector& cvNeedFill, TCardVector& dvFrom,CardsVector cvDoubles)
{
	if (dvFrom.empty())
	{
		return;
	}
	//选取五连
	sort(dvFrom.begin(),dvFrom.end(),cmpSingle);
	
	while (dvFrom.size()>=5)
	{
		TCardVector TmpCard =FindFiveMultis(dvFrom);
		if(TmpCard.size() == 5)
		{	
			cvNeedFill.push_back(TmpCard);
			//删除当前牌组中选取出来构成顺子的牌
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
	//扩展五连
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
		//删除当前牌组中构成顺子的牌
		//删除当前牌组中选取出来构成顺子的牌
		for (int i=0; i<cvNeedFill.at(j).size();i++)
		{
			dvFrom.erase(remove(dvFrom.begin(),dvFrom.end(),cvNeedFill.at(j).at(i)),
				dvFrom.end());
		}
	}
	//合并五连
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
					//将两组完全相同的顺子组成双顺后，把他们移出单顺牌组
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
//选取指定牌组中最小的五连
//参数：dvCards指定在哪儿选取五连
//返回：如果在指定牌组中找到五连，那么返回找到的五连，否则返回一个空数组

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
	//查找最小的顺子
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
//对子牌组排序算子
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
//选取双顺
//选取规则
	/*
		将一副牌中所有的对子检测出来，然后将对子排序，如果有相连的对子，将其合并成(尽可能大的)双顺.
	*/
void CRobot::ChoseDoubleMultis(CardsVector& cvNeedFill, CardsVector& dvFrom)
{
	if (dvFrom.size() < 3 )
	{
		return ;
	}
	//把2去掉
	if (dvFrom.back().front().GetValue() == CV_2)
	{
		dvFrom.pop_back();
	}
	sort(dvFrom.begin(),dvFrom.end(),cmp_double);
	//在所有对子中比较找出双顺
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
			//提取出对子
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
//		MessageBox(NULL,"走goto语句","info",MB_OK);
	//	goto AllDoubleMulti;
	}

	//将能够构成双顺的对子从对子牌组数组中剔除
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
//三张牌组排序算子
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
//选取三顺
	/*
		一副牌中的所有三条都能比较方便地提取出来。 
		在三条的基础上进行比较，如果有相连的三条，则将其合并成（尽可能大的）三顺。
	*/
void CRobot::ChoseThreeMultis(CardsVector& cvNeedFill, CardsVector& dvFrom)
{
	if (dvFrom.size() < 2 )
	{
		return ;
	}

	//把2去掉
	if (dvFrom.back().front().GetValue() == CV_2)
	{
		dvFrom.pop_back();
	}

	sort(dvFrom.begin(),dvFrom.end(),cmp_three);
	//在所有对子中比较找出双顺
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
			//提取出三张
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

	//将能够构成双顺的对子从对子牌组数组中剔除
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
//选取三张
void CRobot::ChoseThree(CardsVector& cvNeedFill, TCardVector& dvFrom)
{
	FillCards(3,CT_THREE,cvNeedFill,dvFrom);
}

//////////////////////////////////////////////////////////////////////////
//选取对子
void CRobot::ChoseDouble(CardsVector& cvNeedFill, TCardVector& dvFrom)
{
	FillCards(2,CT_DOUBLE,cvNeedFill,dvFrom);
}

//////////////////////////////////////////////////////////////////////////
//选取炸弹,炸弹可带一张单牌，所以选出的炸弹的牌张数可能是3、4两种种长度
//cvNeedFill指定选取出的炸弹存放位置，dvFrom指定从哪儿选取炸弹
void CRobot::ChoseBoom(CardsVector& cvNeedFill, TCardVector& dvFrom)
{
	cvNeedFill.clear();
	//四张的炸弹
	FillCards(4,CT_BOOM,cvNeedFill,dvFrom);
}


//当自己快要打完的时候，寻找出牌
TCardVector CRobot::MeWillFinish()
{
	TCardVector tmp;
	tmp.clear();

	return tmp;
}

//当对手快要打完的时候，寻找出牌
TCardVector CRobot::RivalWillFinish()
{
  	TCardVector tmp;
  	tmp.clear();
  	return tmp;
}

//一般情况下的出牌流程
TCardVector CRobot::FindOutGeneral(bool bNotOutS,bool bNotOutD,bool bNotOutT)
{
	TCardVector tmp;
	tmp.clear();
	int nNext = GetNextPlayerSeat(m_nOutSeat);
	int nLast =  GetLastPlayerSeat(m_nOutSeat);
	//选择三张带的是单张还是对子，
	bool bChoseSingle = true; 
	//如果单张能有控牌权，但是对子没有，那么带对子，否则带单张
	if (m_dvSingles.empty())
	{
		bChoseSingle = false;
	}else if (!m_dvSingles.empty() && !m_dvDoubles.empty() )
	{
		int nValueS = GameRule::CardWeight(m_dvSingles.front());
		int nValueD = GameRule::CardWeight(m_dvDoubles.front().front());
		bChoseSingle = nValueD>nValueS?true:false;
	}
		//三张
	/*一般情况下，都是应当先出三带一（二）再出三张
		带出的牌的优先原则
		 1.首先得是小牌，
		 2.孤立的牌优先级最高，再是不能接回的牌，最后是单张优先于对子
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
		//如果到这里还是没有选取出搭子，那么考虑把对子当做单张带出
		return tmp;
	}

	//三顺
	if (1==FindOutFromThreeMulti(tmp))
	{
		// 为三顺找到搭子搭成飞机带翅膀牌型
		int nThreeLen = tmp.size()/3; 
		if (bChoseSingle)
		{
			if (m_dvSingles.size() >=nThreeLen)  //给三顺搭上单张
			{
				if (m_dvSingles.at(nThreeLen-1).GetValue() <= CV_K && 
					m_dvSingles.at(nThreeLen-1).GetValue()  != CV_2)
				{
					tmp.insert(tmp.end(),m_dvSingles.begin(),m_dvSingles.begin()+nThreeLen);
					return tmp;
				}
			}
			if (m_dvDoubles.size() >= nThreeLen) //带对子
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
			if (m_dvDoubles.size() >= nThreeLen) //带对子
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

			if (m_dvSingles.size() >=nThreeLen)  //给三顺搭上单张
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

	//若果走到这里，马上面临选择单张，但是有可能三张或者三顺牌值比较大，所以没有选出，
	//此处应当为这些个大哥留个单张或者对子
	//给三张留着搭子
	TCardVector tmpSingle = m_dvSingles;
	CardsVector tmpDoulse = m_dvDoubles;
	this->ReservedSmallCards(bChoseSingle);
	//单张
	if (1 == FindOutFromSingle(tmp))
	{
		return tmp;
	}
	//对子
	if (1 == FindOutFromDoubles(tmp))
	{
		return tmp;
	}
	//走到这里，证明单张和对子也没有符合条件的出牌，
	//还原分析好的牌组
	m_dvSingles  =  tmpSingle;
	m_dvDoubles = tmpDoulse;

	//单顺
	if (1==FindOutFromSingleMuti(tmp))
	{
		return tmp;
	}
	//双顺
	if (1==FindOutFromDoubleMuti(tmp))
	{
		return tmp;
	}

	//可能全是大牌
	return FastOut();

	if (m_dvBooms.size()>0)   //炸弹--正确分析
	{
		sort(m_dvBooms.begin(),m_dvBooms.end(),cmpCV);	
		tmp = m_dvBooms.front();
		return tmp;
	}
	//最后是火箭
	if (m_dvMaxBoom.size() >0)
	{
		tmp = m_dvMaxBoom;
		return tmp;
	}
	//分析出错、、意外处理
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
		if (m_nOutSeat == m_nLandlord)  //地主的对手有两个
		{
			if (GetGreaterThanNextOut(m_dvPlayerCards[nNext],myCards.back(),Res) ||
				GetGreaterThanNextOut(m_dvPlayerCards[nLast],myCards.back(),Res))
			{
				return true;
			}else 
			{
				return false;
			}
		}else   //农民的对手只有一个地主
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
		if (m_nOutSeat == m_nLandlord)  //地主的对手有两个
		{
			if (GetGreaterThanNextOut(m_dvPlayerCards[nNext],tmp,Res) &&
				GetGreaterThanNextOut(m_dvPlayerCards[nLast],tmp,Res))
			{
				return true;
			}else 
			{
				return false;
			}
		}else   //农民的对手只有一个地主
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
			if (m_nOutSeat == m_nLandlord) //地主有两个对手
			{
				if (!GetGreaterThanNextOut(m_dvPlayerCards[nLast],TmpO,tmp)&&
					!GetGreaterThanNextOut(m_dvPlayerCards[nNext],TmpO,tmp))
				{
					nCount++;
				}
			}else //农民只有地主一个对手
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
			if (m_nOutSeat == m_nLandlord) //地主有两个对手
			{
				if (!GetGreaterThanNextOut(m_dvPlayerCards[nLast],myCards.at(i),tmp)&&
					!GetGreaterThanNextOut(m_dvPlayerCards[nNext],myCards.at(i),tmp))
				{
					nCount++;
				}
			}else //农民只有地主一个对手
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

//判断指定牌组是否拥有绝对控制权，
//绝对控制权：对手没有牌能压过自己
bool CRobot::HaveAbsoluteControl(CardsVector myCards)
{
	if (myCards.empty()) return false;
	int nNext = GetNextPlayerSeat(m_nLandlord);
	int nLast  = GetLastPlayerSeat(m_nLandlord);
	sort(myCards.begin(),myCards.end(),cmpCV);
	TCardVector Res;
	if (m_nOutSeat == m_nLandlord)  //地主的对手有两个
	{
		if (!GetGreaterThanNextOut(m_dvPlayerCards[nNext],myCards.front(),Res) &&
			!GetGreaterThanNextOut(m_dvPlayerCards[nLast],myCards.front(),Res))
		{
			return true;
		}else 
		{
			return false;
		}
	}else   //农民的对手只有一个地主
	{
		return !GetGreaterThanNextOut(m_dvBackupPlayerCards[m_nLandlord],myCards.front(),Res);
	}
	return false;
}
//单张处理
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
		if (m_nOutSeat == m_nLandlord)  //地主的对手有两个
		{
			if (!GetGreaterThanNextOut(m_dvPlayerCards[nNext],tmp,Res) &&
				!GetGreaterThanNextOut(m_dvPlayerCards[nLast],tmp,Res))
			{
				return true;
			}else 
			{
				return false;
			}
		}else   //农民的对手只有一个地主
		{
			return !GetGreaterThanNextOut(m_dvBackupPlayerCards[m_nLandlord],tmp,Res);
		}
	}
	return false;
}
//农民剩下1张牌的时候地主的出牌 
//地主剩下一张牌，出牌的情况
/*
	1.自己能完全控制出牌权，那么控场关牌
	2.自己不能完全控制出牌权
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


//为飞机找翅膀
void CRobot::FindWing(TCardVector dvPlant,TCardVector dvSingle, CardsVector dvDoule)
{
	if (!dvPlant.empty())
	{
		if (!dvSingle.empty())
		{

		}
	}
}
//农民剩下2张牌的时候地主的出牌
//农民剩下两张牌的时候可能出现的情况
/*
 1.是一个对子
 2.是两个单张
 3.是一对鬼---这个不用考虑任何事情了，完全没辙
*/
TCardVector CRobot::FindOutWhenFarmer2()
{	
	int nNext = GetNextPlayerSeat(m_nLandlord);
	int nLast = GetLastPlayerSeat(m_nLandlord);
	TCardVector tmp,Res;
	CardTypeInfo cti1 = GameRule::GetCardType(m_dvPlayerCards[nNext]);
	CardTypeInfo cti2 = GameRule::GetCardType(m_dvPlayerCards[nLast]);
	if (cti1.nType == CT_DOUBLE || cti2.nType == CT_DOUBLE)  //报的双是对子
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
	}else   //是一对单牌
	{
		//return FastOut(true);
		return FindOutGeneral();
	}
	return Res;
}

//地主手数为3的时候的出牌
//当手数为3的时候，可能出现的情况
/*
1.三手牌都拥有绝对控制权，随意打都赢
2.三手牌中有两手拥有绝对控制权，最后打最小的牌
3.三手牌中有一手牌拥有绝对控制权，不大好赢的样子
   a)如果三手牌都相同，那么打出中间牌
   b)如果有两手牌相同，那么打出相同两手牌中牌值最小的一个
   c)如果三手牌都不相同，除掉拥有控制权的牌后常规出牌
4.没有一手牌能拥有控制权.一般都会输，可能会赢
*/
TCardVector CRobot::FindOutAtLand3()
{
//  	this->ShowDebug(m_dvThreeMultis,"三顺");
//  	this->ShowDebug(m_dvSingles,"单张");
//  	this->ShowDebug(m_dvDoubles,"对子");
	TCardVector tmp;
	tmp.clear();
	//计算自己手牌中对手要不起的牌数
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
	//寻找出牌
	switch(nCount)
	{
	case 1:
		//三手牌牌型相同且牌数相同、打出第二大的牌
		if (nSameCount == 3)
		{
			if (!((tmp = FindMidCard(m_dvBooms)).empty()));
			else if (!((tmp = FindMidCard(m_dvThreeMultis)).empty()));
			else if (!((tmp = FindMidCard(m_dvDoubleMultis)).empty()));
			else if (!((tmp = FindMidCard(m_dvSingleMultis)).empty()));
			else if (!((tmp = FindMidCard(m_dvThrees)).empty()));
			else if (!((tmp = FindMidCard(m_dvDoubles)).empty()));
			//单张需要特殊处理
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
//地主手数为2的时候的出牌
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
		tmp = m_dvThrees.back();//三带二
		if (!m_dvDoubles.empty())  //有对子也给打掉好了
		{
			tmp += m_dvDoubles.front();
			return tmp; 
		}else if (!m_dvSingles.empty())   //三带一
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
// 	if (!m_dvSingles.empty() && !RivalHaveGreaterThanMe(m_dvSingles)) //单张需要单独处理
// 	{
// 		tmp.push_back(m_dvSingles.back());
// 		return tmp;
// 	}
	//如果说自己手中的牌没有能够压过对手的，那么选择打出牌张数最多的牌
	return FastOut();
}

//////////////////////////////////////////////////////////////////////////
//农民手数为2的时候寻找出牌
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
		tmp = m_dvThrees.back();//三带二
		if (!m_dvDoubles.empty() && m_dvSingles.empty())  //有对子也给打掉好了
		{
			tmp += m_dvDoubles.front();
			return tmp; 
		}else if (!m_dvSingles.empty() && m_dvDoubles.empty())   //三带一
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
	if (!m_dvSingles.empty() && !RivalHaveGreaterThanMe(m_dvSingles)) //单张需要单独处理
	{
		tmp.push_back(m_dvSingles.back());
		return tmp;
	}
	//如果说自己手中的牌没有能够压过对手的，那么选择打出牌张数最多的牌
	return FastOut();
}

//////////////////////////////////////////////////////////////////////////
//农民手数为3的时候寻找出牌
TCardVector CRobot::FindOutAtFarmer3()
{
	return FindOutAtLand3();
}
//////////////////////////////////////////////////////////////////////////
//农民手中手数较多的时候的出牌
TCardVector CRobot::FindOutAtFarmerG()
{
	TCardVector tmp;
	tmp.clear();
	int nSnipeSeat = GetLastPlayerSeat(m_nLandlord); //狙击位，阻挠地主出牌
	int nFireSeat    = GetNextPlayerSeat(m_nLandlord); //火力位，一般负责关牌
	//如果农民是狙击手的出牌
	//狙击位的玩家应当尽量杜绝地主出牌
	if (m_nOutSeat = nSnipeSeat )
	{
		tmp =  FindOutGeneral();
		TCardVector dvLandSingle; //地主单张
		CardsVector  dvLandDoule;//地主对子
		CardTypeInfo cti = GameRule::GetCardType(tmp);
		int nLevelOut = cti.nLevel;
		switch(cti.nType)
		{
		case CT_SINGLE:  //打出不能让地主过的最小单张
			break;

		case CT_DOUBLE:
			break;
			
		default:
			break;
		}

	}else  //火力位的出牌
	{
		return FindOutGeneral();
	}
	return tmp;
}
//地主剩下一张牌的时候的出牌
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
//地主剩下两张牌的时候的出牌
TCardVector CRobot::FindOutWhenLand2()
{
	TCardVector tmp,Res;
	CardTypeInfo cti = GameRule::GetCardType(m_dvPlayerCards[m_nLandlord]);
	if (cti.nType == CT_DOUBLE)  //报的双是对子
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
	}else   //是一对单牌
	{
		return FastOut(false);
	}
	return Res;
}

//当自家还有三手的时候的出牌
TCardVector CRobot::FindOutAtLevel3()
{
	TCardVector tmp;
	tmp.clear();
	//计算自己手中拥有完全控制权的牌数
	int nCount  = 0;
	if (m_nOutSeat  == m_nLandlord) //地主
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
		if (myCards.front().size() == 3) //三张需要看看把对子或者单张带出去
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
		if (!m_dvDoubles.empty() )  //有对子也给打掉好了
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
	if (!m_dvSingles.empty()) //单张需要单独处理
	{
		TCardVector mysingleCards;
		sort(m_dvSingles.begin(),m_dvSingles.end(),cmpTV);
		mysingleCards.push_back(m_dvSingles.back());
		if (m_nLandlord == m_nOutSeat) //地主
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
		}else  //农民
		{
			if (!GetGreaterThanNextOut(m_dvBackupPlayerCards[m_nLandlord],mysingleCards,tmp))
			{
				tmp.clear();
				tmp = mysingleCards;
				return tmp;
			}
		}
	
	}
	//如果说自己手中的牌没有绝对控制权，那么正常出牌好了
	return FindOutGeneral();
}

//赢面比较小的时候跑牌
TCardVector CRobot::FastOut(bool bMaxToMin)
{
	TCardVector tmp;
	tmp.clear();
	//选择三张带的是单张还是对子，
	bool bChoseSingle = true; 
	//如果单张能有控牌权，但是对子没有，那么带对子，否则带单张
	if (RivalHaveGreaterThanMe(m_dvDoubles)&&
		!RivalHaveGreaterThanMe(m_dvSingles))
	{
		bChoseSingle = false;
	}
	//三顺
	if (1==FindOutUnthink(tmp,m_dvThreeMultis))
	{
		// 为三顺找到搭子搭成飞机带翅膀牌型
		int nThreeLen = tmp.size()/3; 
		if (bChoseSingle)
		{
			if (m_dvSingles.size() >=nThreeLen)  //给三顺搭上单张
			{
				tmp.insert(tmp.end(),m_dvSingles.begin(),m_dvSingles.begin()+nThreeLen);
				return tmp;
			}
			if (m_dvDoubles.size() >= nThreeLen) //带对子
			{
				for (int i=0; i<nThreeLen; i++)
				{
					tmp += m_dvDoubles.at(i);
				}
				return tmp;
			}
		}else 
		{
			if (m_dvDoubles.size() >= nThreeLen) //带对子
			{
				for (int i=0; i<nThreeLen; i++)
				{
					tmp += m_dvDoubles.at(i);
				}
				return tmp;
			}
			if (m_dvSingles.size() >=nThreeLen)  //给三顺搭上单张
			{
				tmp.insert(tmp.end(),m_dvSingles.begin(),m_dvSingles.begin()+nThreeLen);
				return tmp;
			}
		}
		return tmp;
	}
	//单顺
	if (1==FindOutUnthink(tmp,m_dvSingleMultis))
	{
		return tmp;
	}
	//双顺
	if (1==FindOutUnthink(tmp,m_dvDoubleMultis))
	{
		return tmp;
	}

	//三带二
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
	//三张 
	if (1 == FindOutUnthink(tmp,m_dvThrees))
	{
		return tmp;
	}

	//对子
	if (1 == FindOutUnthink(tmp,m_dvDoubles))
	{
		return tmp;
	}
	
	//单张
	if (!m_dvSingles.empty()&&!bMaxToMin)
	{
		tmp.push_back(m_dvSingles.front());
	}else if (!m_dvSingles.empty()  && m_dvSingles.size() >=2 && bMaxToMin)
	{
		tmp.push_back(m_dvSingles.at(1));
	}

	//炸弹、不可随意炸，需查看炸完后自己是否能赢，赢则炸，否则以四带二打出
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
//寻找当前应当打出的牌
//返回： 找到需要打出的牌后进行返回，该函数不允许失败
/*
	打牌顺序：1.三顺
	          2.双顺
			  3.顺子
			  4.三代二
			  5.三张
			  6.对子
			  7.单张
*/
//机器人偷看牌作弊出牌版本
TCardVector CRobot::FindNowOut()
{
	TCardVector tmp;
	TCardVector UnUser;
	int nNext =GetNextPlayerSeat(m_nOutSeat); //下家
	int nLast = GetLastPlayerSeat(m_nOutSeat); //上家
	if (m_nOutSeat == m_nLandlord)  //出牌的玩家是地主
	{
		//对手快打完了的时候优先级应当最高
		if (m_Warring[nNext].nWL== 1 || m_Warring[nLast].nWL == 1) //农民报单
		{
			m_nSuggestOutTime = 0;
			return FindOutWhenFarmer1();
	/*		return FastOut(true);*/
		}else if (m_Warring[nNext ].nWL == 2 || m_Warring[nLast].nWL ==2 ) //农民报双
		{
			m_nSuggestOutTime = 0;
			return FindOutWhenFarmer2();
		} 

		if (m_nLevel[m_nOutSeat]==2 )   //自己家手中还有两手牌
		{
			m_nSuggestOutTime = 0;
			return FindOutAtLand2();
		}else  if (m_nLevel[m_nOutSeat] == 3 ) //自己手中还有三手牌
		{
			m_nSuggestOutTime = 0;
			return FindOutAtLand3();
		}else  
		{	
			m_nSuggestOutTime = 6;
			return FindOutGeneral();  //正常出牌
		}
	}
	else   //出牌的玩家是农民
	{
		//如果下家是自己的队友，并且只剩下一手牌了，找到一手能够被下家要上的牌打出，如果自己手中有炸弹,先出炸弹
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
					//最后打出一个最小的单张
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
		
		//首先，优先考虑别让对手关牌
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
		//自己的手数小于等于3
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
	//一般出牌
	m_nSuggestOutTime = 5;
	return  FindOutGeneral();	
}

//地主下家的出牌
TCardVector CRobot::FindOutFarmerFire()
{
	TCardVector tmp;
	//对手牌快出完的时候应当优先考虑
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
	//自己的手数小于等于3
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

//地主上家的出牌
TCardVector CRobot::FindOutFarmerSnipe()
{
	//如果自己能够直接出完，那么就出
	//计算自己手牌中对手要不起的牌数
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
	//自己的手数小于等于3
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
//遍历拆牌
void CRobot::ErgodicCards(bool bBoom,bool bThree, bool bDouble, bool bThreeMulti,bool bDouleMulti,bool bMulti, int nSeat)
{
	this->ResetTmp();
	TCardVector tmphandCards = m_dvPlayerCards[nSeat];
	 
	if (bBoom) //炸弹
	{
		this->ChoseBoom(cvTmpBoom,tmphandCards);
	}
	if (bThree) //三张
	{
		this->ChoseThree(cvTmpThree,tmphandCards);
	}
	if (bDouble) //对子
	{
		this->ChoseDouble(cvTmpDouble,tmphandCards);
	}
	if (bThreeMulti)//三顺
	{
		this->ChoseThreeMultis(cvTmpThreeMulti,cvTmpThree);
	}
	if (bDouleMulti) //双顺
	{
		this->ChoseDoubleMultis(cvTmpDoubleMulti,cvTmpDouble);
	}
	if (bMulti) //单顺
	{
		this->ChoseSingleMultis(cvTmpSingleMulti,tmphandCards,cvTmpDoubleMulti);
	}
	//最后再挣扎下
	this->ChoseBoom(cvTmpBoom,tmphandCards);
	this->ChoseThree(cvTmpThree,tmphandCards);
	this->ChoseDouble(cvTmpDouble,tmphandCards);
	this->ChoseDoubleMultis(cvTmpDoubleMulti,cvTmpDouble);
	this->ChoseThreeMultis(cvTmpThreeMulti,cvTmpThree);
	this->ChoseSingleMultis(cvTmpSingleMulti,tmphandCards,cvTmpDoubleMulti);
	cvTmpSingle += tmphandCards;
	
	//最后处理下王牌
	if (cvTmpJoker.size() == 1)  //单王
	{
		cvTmpSingle += cvTmpJoker;
	}else if (cvTmpJoker.size() == 2) //王炸
	{
		cvTmpMaxBoom = cvTmpJoker;
	}
	
	//将这一次拆牌的的手数和所有牌的权值存放
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
	//火箭
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
	//单张的权值
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
	//将这一次的拆牌信息填充进入拆牌信息向量中
	m_allSplitInfo[nSeat].push_back(tmpSI);
}

//为三张和三顺预留比较小的单张或者对子
void CRobot::ReservedSmallCards(bool bChoseSingle)
{
	int nLen = m_dvThrees.size();
	for (int i=0; i<m_dvThreeMultis.size();i++)
	{
		nLen += m_dvThreeMultis.at(i).size()/3;
	}
	if (nLen > 0)
	{
		bool bFlagReserved  = false; //是否已经预留牌张
		if (bChoseSingle)  //优先选择留单张
		{
			if (m_dvSingles.size() >= nLen) //留单张
			{
				if (m_dvSingles.at(nLen-1).GetValue() < CV_A && 
					m_dvSingles.at(nLen-1).GetValue() != CV_2)
				{
					m_dvSingles.erase(m_dvSingles.begin(),m_dvSingles.begin()+nLen);
					bFlagReserved = true;
				}
			}
			if (!bFlagReserved && m_dvDoubles.size() >= nLen) //留对子
			{
				if (m_dvDoubles.at(nLen-1).front().GetValue() < CV_A && 
					m_dvDoubles.at(nLen-1).front().GetValue()  != CV_2)
				{
					m_dvDoubles.erase(m_dvDoubles.begin(),m_dvDoubles.begin()+nLen);
				}
			}
		}else  //优先选择留对子 
		{
			if (m_dvDoubles.size() >= nLen) //留对子
			{
				if (m_dvDoubles.at(nLen-1).front().GetValue() < CV_A && 
					m_dvDoubles.at(nLen-1).front().GetValue()  != CV_2)
				{
					m_dvDoubles.erase(m_dvDoubles.begin(),m_dvDoubles.begin()+nLen);
					bFlagReserved = true;
				}
			}

			if (!bFlagReserved && m_dvSingles.size() >= nLen) //留单张
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
//找寻能够压过的牌
//OutCard 指定需要压的牌，参数fromCards指定需要从哪儿找压牌，参数三返回
//找到的压牌
//如果成功从fromCards中找到能压过OutCard的牌，那么返回true并且以FollowCards返回
//找到的压牌，如果fromCards中没有牌组能够压过OutCard，那么返回false并且将FollowCards
//清空
bool CRobot::FindFollowCards(TCardVector OutCard,CardsVector fromCards,TCardVector& FollowCards, int nLevel)
{
	if (OutCard.empty() ) return false;
	if (!fromCards.empty())    //
	{
		for (int nIndex =0; nIndex < fromCards.size(); nIndex++) //如果能够从最优分析里拿出就拿出牌跟
		{
			if (GameRule::CanOut(false,OutCard,fromCards.at(nIndex))) 
			{
				FollowCards = fromCards.at(nIndex);
				return true;
			}   
		}
		//从最优分析里面的与出牌相同的牌组中查找是否有打得过的牌
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
		//从全部牌中查找能否压过的牌
		//从全部手牌中查找出牌的时候，考虑尽量不破坏拆牌
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
	}else  if (nLevel == 2)//从全部牌中查找能否压过的牌
	{
		if (GetGreaterThanNextOut(m_dvOldCards,OutCard,FollowCards))
		{
			return true;
		}
	}

	FollowCards.clear();
	return false;
}

//常规出牌,只从最优分析中提取出牌
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

	//常规出牌
	switch (ctiOT.nType)
	{
	case CT_MAXBOOM:
		m_nSuggestOutTime = 1;
		return false;
		break;

	case CT_BOOM:  //炸弹
		FindFollowCards(dvOut,m_dvBooms,dvRes,nLevel);
		if (!dvRes.empty()) m_nSuggestOutTime = 5;
		else m_nSuggestOutTime = 1;
		break;

	case CT_SINGLE: //单张
		for (int i=0; i<m_dvSingles.size(); i++)
		{
			if (CmpSingle(m_dvSingles.at(i),dvOut.front()))
			{
				dvRes.push_back(m_dvSingles.at(i));
				return true;
			}
		}	
		if (nLevel < 2 && nLevel >=0 )  //一般拆第二大的对子,或者三张
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
	case CT_DOUBLE: //对子
		FindFollowCards(dvOut,m_dvDoubles,dvRes,nLevel);
		if (dvRes.empty()) m_nSuggestOutTime = 1;
		else m_nSuggestOutTime = 3;
		break;

	case CT_THREE:  //三张
		FindFollowCards(dvOut,m_dvThrees,dvRes,nLevel);
		m_nSuggestOutTime = 3;
		break;

	case CT_MULTI: //顺子
		FindFollowCards(dvOut,m_dvSingleMultis,dvRes,nLevel);
		if (dvRes.empty()) m_nSuggestOutTime = 3;
		else m_nSuggestOutTime = 8;
		break;

	case CT_MULTIDOUBLETWO:  //双顺
		FindFollowCards(dvOut,m_dvDoubleMultis,dvRes,nLevel);
		if (dvRes.empty()) m_nSuggestOutTime = 3;
		else m_nSuggestOutTime = 8;
		break;

	case CT_MULTITHREE:  //三顺
		FindFollowCards(dvOut,m_dvThreeMultis,dvRes,nLevel);
		if (dvRes.empty()) m_nSuggestOutTime = 3;
		else m_nSuggestOutTime = 8;
		break;

	case CT_THREEWITHONE://三带一
		//首先分离三带一中的三张和一张
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

	case  CT_THREEWITHTWO: //三代二
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

	case CT_MULTITHREEWITHONE:  //飞机带翅膀， 翅膀为单
		SeparationCards(dvOut,tmpCV);
		tmpt.clear();
		for (int i=0; i<tmpCV.size(); i++)
		{
			tmpt += tmpCV.at(i);
		}
		FindFollowCards(tmpt,m_dvThreeMultis,dvRes,nLevel);
		//飞机找搭子
		if (!dvRes.empty() && m_dvSingles.size() >=tmpCV.size()&& nLevel < 2 && 
			m_dvSingles.at(tmpCV.size()-1).GetValue() < CV_A && m_dvSingles.at(tmpCV.size()-1).GetValue() !=CV_2) //找搭子
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

	case CT_MULTITHREEWITHTWO: //飞机带翅膀，翅膀为双
		SeparationCards(dvOut,tmpCV);
		tmpt.clear();
		for (int i=0; i<tmpCV.size(); i++)
		{
			tmpt += tmpCV.at(i);
		}
		FindFollowCards(tmpt,m_dvThreeMultis,dvRes,nLevel);
		//飞机找搭子
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

//拼命接牌，用于不接牌就会输掉了的情况。。比较捉急
bool CRobot::BendOverBackwardsFollow(TCardVector& dvRes,TCardVector dvOut)
{
	return FindFollowGeneral(dvOut,dvRes);
}

//地主接牌
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
		if (!FollowCards.empty())  //如果牌很大，出牌也是大牌就压，不是过
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
				if (CmpSingle(tmpFollow.front(),m_dvMaxThreeCards.back()) || tmpFollow.front().GetValue() == CV_2) //如果找到的出牌属于三个最大的牌值之一，
				{					
					int nOut = GameRule::CardWeight(tmpOut.front().GetValue());
					int nFollow = GameRule::CardWeight(tmpFollow.front().GetValue());
					if(nFollow-nOut ==1)  //代价足够，可以压牌
					{
					}else
					{
						//如果出牌后自己的能够确定关牌，出、否则过
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
		if (nHandNum ==1&& GameRule::CanOut(false,OutCard,m_dvPlayerCards[nWarringSeat]))  //农民只剩下一手牌了,并且出牌和剩余牌相同
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
				//如果最大的单张能够压过，那么就出最大的单张,如果没有，那么拆最大的对子，
				//如果还没有，拆最大的三张，如果还没有，看炸弹。还没有，看最大的牌点。
				//最后过
				sort(m_dvSingles.begin(),m_dvSingles.end(),cmpMaxMin);
				if (!m_dvSingles.empty() &&CmpSingle(m_dvSingles.front(),tmpLandCards.front()) &&
					CmpSingle(m_dvSingles.front(),OutCard.front()))
				{
					FollowCards.push_back(m_dvSingles.front());
					m_nSuggestOutTime = 2;
					return  true;
				}
				//如果除去炸弹，自己能被对手压过的手数小于2，
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
				//寻炸弹
				//如果除去炸弹，自己能被对手压过的手数小于2，
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
				//拆三条
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
				//寻找最大的对子打出
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
			//如果除去炸弹，自己能被对手压过的手数小于2，
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

//农民接牌
bool CRobot::FindFollowFarmer(const TCardVector& OutCard, TCardVector& FollowCards)
{
	TCardVector tmp;
	TCardVector tmpOut = OutCard;

	//对于队友的牌，可以考虑不跟
	if (m_nLastOut != m_nLandlord)
	{
		//如果地主是上家，出牌的的是队友，直接过
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
		//如果队友只剩下一手牌了，并且剩下的手牌和他出的牌是相同的，那么不要接这个牌,除非他的这一手牌能够被地主一把接完
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
				//如果除去炸弹，自己能被对手压过的手数小于2，
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
				//拆对子
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
				//拆三张
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
				//最后看看最大的牌点是否能压过
				sort(m_dvPlayerCards[m_nOutSeat].begin(), m_dvPlayerCards[m_nOutSeat].end(),cmpMaxMin);
				if (CmpSingle(m_dvPlayerCards[m_nOutSeat].front(),OutCard.front()))
				{
					FollowCards.push_back(m_dvPlayerCards[m_nOutSeat].front());
					m_nSuggestOutTime = 6;
					return true;
				}
				//因为到这里基本所有的可能都没法接牌，所以直接返回了。
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
				//拆三张
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
				//寻找最大的对子
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

				//因为到这里基本所有的可能都没法接牌，所以直接返回了。
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
	//不拆牌能跟
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
		if (!FollowCards.empty())  //如果牌很大，出牌也是大牌就压，不是过
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
				if (CmpSingle(tmpFollow.front(),m_dvMaxThreeCards.back())|| tmpFollow.front().GetValue() == CV_2) //如果找到的出牌属于三个最大的牌值之一，
				{
					int nOut =GameRule::CardWeight( tmpOut.front().GetValue()) ;
					int nFollow = GameRule::CardWeight(tmpFollow.front().GetValue());
					if(nFollow-nOut ==1)  //代价足够，可以压牌
					{
					}else
					{
						//如果出牌后自己的能够确定关牌，出、否则过
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
		//狙击位的玩家应当尝试拆牌出一下
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
		if (nLast == m_nLandlord && !FollowCards.empty()) //地主下家，如果出牌是单张并且没有选出出牌，拆牌打一下
		{
			if (OutCard.size() == 1)
			{
				m_nSuggestOutTime = 10;
				FindFollowGeneral(OutCard,FollowCards,1);	
			}
		}
		//地主上家挡牌
		if (m_nLandlord ==  nNext && !FollowCards.empty())
		{
			int nCardValue = GameRule::CardWeight(FollowCards.front().GetValue());

			if (OutCard.size() == 1 && nCardValue < CV_10)
			{
				//当选出来的跟牌单张小于10的时候，考虑下选择一张更大一点的牌打出，如果实在没有，
				//就算了
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
				//当选出来跟牌的对子小于8的时候，考虑下更大一点的牌打出，如果实在么有，就算了
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
		//地主下家压牌
		//如果确定自己的队友没有能压过地主的出牌，那么自己应该尝试把牌拆一下
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
		if (nHandNum ==1 && GameRule::CanOut(false,OutCard,m_dvPlayerCards[m_nLandlord]))  //地主剩下一手牌
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
				//如果除去炸弹，自己能被对手压过的手数小于2，
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
				//拆对子
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
				//拆三张
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
				//最后看看最大的牌点是否能压过
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
				//拆三张
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
				//寻找最大的对子
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
		//如果队友出了一个很大的牌，那就不要接了
		if (m_nOutSeat != m_nLandlord)
		{
			CardTypeInfo ctiTeam = GameRule::GetCardType(OutCard);
			if (ctiTeam.nType == CT_BOOM ) FollowCards.clear();
			if (ctiTeam.nType == CT_SINGLE || ctiTeam.nType == CT_DOUBLE)
			{
				//队友打出的牌蛮大，就不要跟了
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
//寻找当前应该跟的牌,
//找到跟上的牌后，返回true，并填充FollowCards为跟牌，
//如果没有找到能跟上的牌，返回false,并将FollowCards置空
bool CRobot::FindNowFollow(TCardVector OutCard, TCardVector& FollwCards)
{
	 FollwCards.clear();
	 if (OutCard.empty()) return false;
	 //如果自己手中的所有牌能够接牌，直接接了
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
	//如果自己手中只剩下两手牌，且两手牌相同，其中较大的一手牌最大，那么打出这手牌
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

	 //地主跟牌
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

 //计算指定牌组中的大牌张数
 void CRobot::CalcBigNum(int& nBig, TCardVector dvWho)
{
	int nLast  = GetLastPlayerSeat(m_nLandlord);
	int nNext = GetNextPlayerSeat(m_nLandlord);
	//大牌的决定方式：刨掉大小王后，剩下的最大2组即为大牌，大小王也为大牌
	//Ex:当前所有玩家手中的牌还剩下:大王，三个2，三个K，三个J，4个10，那
	//么大王，2，K J便是大牌
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
	//计算所有炸弹的张数
	for (int i=0; i<MAX_PLAYER;i++)
	{
		nNowMaxCount += m_PlayerSplit[i].s_maxBoom.size();
		nNowMaxCount += m_PlayerSplit[i].s_boom.size();
	}
	//大小王一定是大牌
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
 //计算本方(自己和队友)与当前所有牌的大牌数量
 void CRobot::CalcBigNum(int& my, int& all)
 {
	 int nLast  = GetLastPlayerSeat(m_nLandlord);
	 int nNext = GetNextPlayerSeat(m_nLandlord);
	 //大牌的决定方式：刨掉大小王后，剩下的最大2组即为大牌，大小王也为大牌
	 //Ex:当前所有玩家手中的牌还剩下:大王，三个2，三个K，三个J，4个10，那
	 //么大王，2，K J便是大牌
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
	 //自己或者自己与队友
	 TCardVector dvMyTeamCards;
 	 if (m_nOutSeat == m_nLandlord)
	 {
		 //炸弹也算为一个大牌
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
	 //计算所有炸弹的张数
	 for (int i=0; i<MAX_PLAYER;i++)
	 {
		 nNowMaxCount += m_PlayerSplit[i].s_maxBoom.size();
		 nNowMaxCount += m_PlayerSplit[i].s_boom.size();
	 }
	 //大小王一定是大牌
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

 //普通接牌
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
			//如果接牌后自己肯定能关牌，直接接牌
			//计算自己手牌中对手要不起的牌数
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
			//如果选取出来的是大牌，减去打出的大牌，除非自己手中还有剩下所有大牌的一半以上，
			//否则不打出
			int nOutBingNum = 0;   //出牌中的大牌张数
			this->CalcBigNum(nOutBingNum,dvRes);
			if (cti.nType == CT_BOOM) nOutBingNum ++;
			else if (cti.nType == CT_MAXBOOM) nOutBingNum ++;

			if (cti.nType == CT_BOOM || cti.nType == CT_MAXBOOM ||
				dvRes.front().GetValue() >= CV_A || dvRes.front().GetValue() == CV_2)  
			{
			//	if((nMyBigCardCount- nOutBingNum)*2 < nAllBigCardCount-nOutBingNum) 
				if (m_nLandlord == m_nOutSeat && nMyBigRatio < 40) //地主的大牌占%40以下时，不出
				{
					dvRes.clear();
					return false;
				}else  if (nMyBigRatio < 50)
				{
					dvRes.clear();
					return false;
				}
			}
			//如果出牌后自己手数增量
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
//重置所有数据
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
//重置临时数据
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
//根据权值和手数选择最优拆牌方式
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
		}else if (nMin == (m_allSplitInfo[nSeat].at(i).GetHandCount()- m_allSplitInfo[nSeat].front().s_boom.size())) //手数相同则选择权值大的
		{
			int nLevel_1 = m_allSplitInfo[nSeat].at(nIndex).GetAllLevel();
			int nLevel_2 = m_allSplitInfo[nSeat].at(i).GetAllLevel();
			nIndex =  (nLevel_1 > nLevel_2?nIndex:i);  
			nMin   = m_allSplitInfo[nSeat].at(nIndex).GetHandCount();
			//权值、手数都相同，那么就随便选一个好了
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
		//以最优拆牌方式填充牌组数组
		ConvertCTToCV(m_allSplitInfo[nSeat].at(nIndex).s_boom,m_dvBooms,CT_BOOM);
		ConvertCTToCV(m_allSplitInfo[nSeat].at(nIndex).s_three,m_dvThrees,CT_THREE);
		ConvertCTToCV(m_allSplitInfo[nSeat].at(nIndex).s_doule,m_dvDoubles,CT_DOUBLE);
		ConvertCTToCV(m_allSplitInfo[nSeat].at(nIndex).s_threeMulti,m_dvThreeMultis,CT_MULTITHREE);
		ConvertCTToCV(m_allSplitInfo[nSeat].at(nIndex).s_doubleMulti,m_dvDoubleMultis,CT_MULTIDOUBLETWO);
		ConvertCTToCV(m_allSplitInfo[nSeat].at(nIndex).s_singleMulti,m_dvSingleMultis,CT_MULTI);
// 		//火箭
		//m_dvMaxBoom  = m_allSplitInfo[nSeat].at(nIndex).s_maxBoom.
		if (!cvTmpJoker.empty()) m_dvMaxBoom = cvTmpJoker;
// 		if (cvTmpJoker.size() == 2)
// 		{
// 			m_dvMaxBoom+= cvTmpJoker;
// 		}else if (cvTmpJoker.size() == 1) //单张王牌
// 		{
// 			m_dvSingles.push_back(cvTmpJoker.front());
// 		}
		//单张
		for (int i=0; i<m_allSplitInfo[nSeat].at(nIndex).s_single.size(); i++)
		{
			m_dvSingles += m_allSplitInfo[nSeat].at(nIndex).s_single.at(i).cards;
		}
	} 
	
}
//////////////////////////////////////////////////////////////////////////
//职能：查找是否拥有指定牌型的牌
//参数：nCardsLen牌型长度，nType牌目标型，targetCards目标牌组
//返回：返回目标牌组中所有的nType牌型，如果目标牌组中没有指定
//      的牌型，返回size为0的向量
TCardVector CRobot::GetBaseCards(int nCardsLen, int nType,  TCardVector targetCards)
{
	TCardVector tmpCards;
	tmpCards.clear();

	if (nCardsLen > targetCards.size() || targetCards.size() <= 0 )
	{
		return tmpCards;
	}
	TCardVector dvMayBeOut;
	CardTypeInfo ctiMayBeOut;		//用于获取牌型信息
	CardTypeInfo ctiFrontMBO;		//前面一个牌型信息
	ctiFrontMBO.reset();
	ctiMayBeOut.reset();
	TCardVector  dvTmpHand = targetCards;

	//获取手牌中拥有的基础牌
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
				//剔除因为牌组存在炸弹，获取一对的时候会出现获取到3对的类似情况
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
//填充牌组数组数据
//参数： nLen指定填充的牌组原子牌型长度，nType，指定填充的牌组内的牌型，
//       dvNeedFill，指定需要填充的牌组数组
void CRobot::FillCards(int nLen,int nType,CardsVector& dvNeedFill,TCardVector& dvWhere)
{
	if (dvWhere.size() < nLen)
	{
		return;
	}
	GameRule::SortCards(dvWhere);
	//提取出当前牌中拥有的ntype牌型
	TCardVector dvTmp = GetBaseCards(nLen,nType,dvWhere);
	TCardVector dvTmpSingleType;
	for (TCardVector::iterator it = dvTmp.begin();it<dvTmp.end();it+=nLen)
	{
		dvTmpSingleType.clear();
		dvTmpSingleType.insert(dvTmpSingleType.begin(),it,it+nLen);
		dvNeedFill.push_back(dvTmpSingleType);
	}
	//将填充到dvNeedFill中的牌从dvWhere中剔除
	for (int i=0;i<dvTmp.size();i++)
	{
		dvWhere.erase(remove(dvWhere.begin(),dvWhere.end(),dvTmp.at(i)),dvWhere.end());
	}
}

//计算指定玩家的要不起对手的手牌数量
int CRobot::CalcAllNotGreateThanCards()
{
	//计算自己手牌中对手要不起的牌数
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
		//拿出打出的牌数据
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
//以下为出牌查找函数组，函数功能皆为从指定的牌组数组中找到可以打出的牌
//如果找到可以打出的牌，则返回1，并使用参数Res带回需要打出的牌
//如果没有找到可以打出的牌或者打出的牌不合理，那么返回0 
int  CRobot::FindOutFromThreeMulti(TCardVector& Res)//三顺
{
	/*
		三顺出牌原则：
		1.三顺在理论上被压住的可能性较小，一般情况应当先打出。
		2.如果三顺的的最小牌的值过大(大于J)，此时分情况处理。
		  a)如果对手的牌还有很多(多于6张)，打出三顺后自己的牌很小（牌值很小、单张很多）
		  那么应当留一下.
		  b)打出三顺后自己剩下的牌很大,(牌值很大、能一把打出)
		  那么打出牌    
	*/
	Res.clear();
	if (m_dvThreeMultis.size() >0) 
	{
		return FindGeneralOut(Res,m_dvThreeMultis);
	}
	return 0;
}

int  CRobot::FindOutFromDoubleMuti(TCardVector& Res)//双顺
{
	Res.clear();
	if (m_dvDoubleMultis.size() >0) 
	{	
		return FindGeneralOut(Res,m_dvDoubleMultis);
	}
	return 0;
}

int  CRobot::FindOutFromSingleMuti(TCardVector& Res)//单顺
{
	Res.clear();
	if (m_dvSingleMultis.size() >0) 
	{
		return FindGeneralOut(Res,m_dvSingleMultis);
	}
	return 0;
}

int  CRobot::FindOutFromThreeWTwo(TCardVector& Res) //三带二
{
	if (m_dvThrees.size() > 0 && m_dvDoubles.size() > 0 ) //三代二 -- 正确分析
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

int  CRobot::FindOutFromThrees(TCardVector& Res)//三张
{
	Res.clear();
	if (m_dvThrees.size() >0)//三张--正确分析
	{
		if (1 == FindGeneralOut(Res,m_dvThrees))
		{
			return 1;
		}
	}
	return 0;
}

int  CRobot::FindOutFromDoubles(TCardVector& Res)//对子
{
	Res.clear();
	if (m_dvDoubles.size()>0 ) //对子--正确分析
	{
		return FindGeneralOut(Res,m_dvDoubles);

	}
	return 0;
}

int  CRobot::FindOutFromSingle(TCardVector& Res)//单张
{
	if (m_dvSingles.size() > 0 &&
		m_dvSingles.front().GetValue() <CV_A &&
		m_dvSingles.front() != CV_2 )   //单张--正确分析
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
		}else  //牌值过大的时候，确定选出的牌是打是留
		{
			//确定是否打出这个牌型
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

//寻找当前所有玩家手牌中最大的三张牌
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
//显示分析后的手牌
void CRobot::ShowDebug(CardsVector ct,char* name)
{
	char buf[10];
	std::string str = name;
	if (ct.empty())
	{
		str += "为空";
		cDebugOut.DebugString(str);
		return;
	}
	str += "有：";
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
		test+= "为空";
		cDebugOut.DebugString(test);
	}else 
	{
		test += "有\n";
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

//显示分析后的手牌
void CRobot::ShowDebug(SplitInfo sp,char* strWho)
{
	std::string str = strWho;
	str+= "分析后的牌为:";
	cDebugOut.DebugString(str);
	this->ShowDebug(sp.s_boom,"炸弹");
	this->ShowDebug(sp.s_three,"三张");
	this->ShowDebug(sp.s_threeMulti,"三顺");
	this->ShowDebug(sp.s_doule,"对子");
	this->ShowDebug(sp.s_doubleMulti,"双顺");
	this->ShowDebug(sp.s_singleMulti,"单顺");
	this->ShowDebug(sp.s_single,"单张");
	this->ShowDebug(sp.s_maxBoom,"火箭");
	cDebugOut.DebugString("结束\n");
}
//debug
//显示分析后的手牌
void CRobot::ShowDebug(TCardVector ct,char* name)
{
	char buf[10];
	std::string str = name;
	if (ct.empty())
	{
		str += "为空";
		cDebugOut.DebugString(str);
		return;
	}
	str += "有：";
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
