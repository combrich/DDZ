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
	m_PromptSingle.clear();						//玩家手牌中拥有的所有单牌
	m_PromptDouble.clear();						//玩家手牌中拥有的所有对子
	m_PromptThree.clear();						//玩家手牌中所有的三张
	m_PromptBoom.clear();						//玩家手牌中所有的炸弹
	m_PromptTWT.clear();						//玩家手牌中所有可组成的三带二
	m_PromptTWO.clear();						//玩家手牌中所有可组成的三带一
	m_PromptFWTO.clear();						//玩家手牌中所有可组成的四带二(二为单张)
	m_PromptFWDT.clear();						//玩家手牌中所有可组成的四带二(二为对子)
	m_PromptMTWO.clear();						//玩家手牌中所有可组成的小飞机
	m_PromptMTWT.clear();						//玩家手牌中所有可组成的大飞机
}

//对手牌进行重排序的排序依据
bool cmp_min_max(Trused::TCARD card_1, Trused::TCARD card_2)
{
	TCards card1(card_1);
	TCards card2(card_2);
	int value1 = GameRule::CardWeight(card_1);
	int value2 = GameRule::CardWeight(card_2);
	return value1 <  value2;
}


//////////////////////////////////////////////////////////////////////////
//职能：查找手牌中大于上家出牌的牌组，并将找到的牌组设置为选中状态
//参数：citFrontOut为上家出牌的牌组信息
//返回：如果成功找到满足条件的牌组，返回true否则返回false
bool GameTrusteeship::GetGreaterThanNextOut(CardTypeInfo ctiFrontOut)
{
	if (ctiFrontOut.nType ==  CT_MAXBOOM)
	{
		return false;
	}
	
	//临时牌组类型信息
	CardTypeInfo ctiTmp;
	ctiTmp.reset();
	//手牌的数量小于上家出牌数量
	bool bRet = false;
	//将玩家手牌中拥有的所有基本牌型分类装存
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

	case CT_THREEWITHTWO:			//三带二
		bRet = HaveOutCard(m_PromptTWT.size()-5,ctiFrontOut,m_PromptTWT,5);
		break;

	case CT_THREEWITHONE:			//三带一
		bRet = HaveOutCard(m_PromptTWO.size()-4,ctiFrontOut,m_PromptTWO,4);
		break;

	case CT_MULTI:				//单顺
		sort(m_PromptSingle.begin(),m_PromptSingle.end(),cmp_min_max);
		bRet = HaveOutCard(m_PromptSingle.size()-ctiFrontOut.nCount, ctiFrontOut,m_PromptSingle);
		break;

	case CT_MULTIDOUBLETWO:		//双顺
		sort(m_PromptDouble.begin(),m_PromptDouble.end(),cmp_min_max);
		bRet = HaveOutCard(m_PromptDouble.size()-ctiFrontOut.nCount, ctiFrontOut,m_PromptDouble);
		break;

	case CT_MULTITHREE:		//三顺
		sort(m_PromptThree.begin(),m_PromptThree.end(),cmp_min_max);
		bRet = HaveOutCard(m_PromptThree.size()-ctiFrontOut.nCount, ctiFrontOut,m_PromptThree);
		break;

	case CT_MULTITHREEWITHONE:	//小飞机
		bRet = HaveOutCardEx(m_PromptTWO.size()-ctiFrontOut.nCount,ctiFrontOut,m_PromptTWO);
		break;

	case CT_MULTITHREEWITHTWO:   //大飞机
		bRet = HaveOutCardEx(m_PromptTWT.size()-ctiFrontOut.nCount,ctiFrontOut,m_PromptTWT);
		break;

	case CT_FOURWITHTWO:		//四带二(AAAA24)
		bRet = HaveOutCard(m_PromptFWTO.size()-6,ctiFrontOut,m_PromptFWTO,6);
		break;

	case CT_FOURWITHDTWO:		//四带二(AAAA4477)
		bRet = HaveOutCard(m_PromptFWDT.size()-8,ctiFrontOut,m_PromptFWDT,8);
		break;
	default: 
		break;
	}


	//没有对应的牌型，最后试试有没有炸弹 
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
	//此处处理玩家手牌中有牌符合要求，
	//但是牌无法满足正常循环提示
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

//判断玩家手牌中是否有可以打出的牌
bool GameTrusteeship::HaveOutCard(int nCardsLen, CardTypeInfo ctiFrontOut ,Trused::TCARDS handCards, int nStep /*=1*/)
{
	TCardVector	 dvMaybeOut;		//可能可以打出的牌
	CardTypeInfo ctiMaybeOut;		//dvMaybeOut牌型信息
	m_PromptCard.clear();
	TCardVector  dvTmpHand = ConvertCardsToCardVector(handCards);


	//处理拥有相同牌型可压牌的情况
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

//判断玩家手牌中是否有可以打出的牌,及判断玩家手中是否有炸弹牌型
bool GameTrusteeship::HaveOutCardBoom(int nCardsLen, CardTypeInfo ctiFrontOut ,Trused::TCARDS handCards, int nStep /*=1*/)
{
	TCardVector	 dvMaybeOut;		//可能可以打出的牌
	CardTypeInfo ctiMaybeOut;		//dvMaybeOut牌型信息
	m_PromptCard.clear();
	TCardVector  dvTmpHand = ConvertCardsToCardVector(handCards);


	//处理拥有相同牌型可压牌的情况
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

//判断玩家手牌中是否有可以打出的牌,飞机牌型
bool GameTrusteeship::HaveOutCardMaxBoom(int nCardsLen, CardTypeInfo ctiFrontOut ,Trused::TCARDS handCards, int nStep /*=1*/)
{
	TCardVector	 dvMaybeOut;		//可能可以打出的牌
	CardTypeInfo ctiMaybeOut;		//dvMaybeOut牌型信息
	m_PromptCard.clear();
	TCardVector  dvTmpHand = ConvertCardsToCardVector(handCards);


	//处理拥有相同牌型可压牌的情况
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

//用于判定玩家手中是否有可以打出的飞机牌型
bool GameTrusteeship::HaveOutCardEx(int nCardsLen,CardTypeInfo ctiFrontOut,Trused::TCARDS handCards)
{
	//将飞机转换为三顺
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

	//将能够组成小飞机的三顺从手牌中剔除
	Trused::TCARDS tmpHandCards = m_HandCard;
	for (size_t i=0; i<m_PromptCard.size();i++)
	{
		tmpHandCards.erase(remove(tmpHandCards.begin(),tmpHandCards.end(),m_PromptCard.at(i)),
			tmpHandCards.end());
	}

	if (bRet && ctiFrontOut.nType == CT_MULTITHREEWITHONE)
	{
		if (bRet && tmpHandCards.size() < ctiTmp.nCount/3)  //没有足够的牌能够与三顺组成飞机
		{
			m_PromptCard.clear();
			return false;
		}

		//与单牌组成飞机
		Trused::TCARDS tmpSigCard = GetBaseCardsEx(1,CT_SINGLE,tmpHandCards,m_PromptCard);

		//将单牌与三顺组成小飞机
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

		if (bRet && tmpHandCards.size() < ctiTmp.nCount/3*2)  //没有足够的牌能够与三顺组成飞机
		{
			m_PromptCard.clear();
			return false;
		}

		//将对子与三顺组成大飞机
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

//获取玩家手牌中拥有的基础牌型，基础牌型为单张、对子、三张、炸弹
//并将获取到的基础牌型放入指定的容器中
void GameTrusteeship::FillBaseCards()
{
	//预备工作
	m_PromptBoom.clear();
	m_PromptSingle.clear();
	m_PromptDouble.clear();
	m_PromptThree.clear();
	m_PromptTWT.clear();
	m_PromptTWO.clear();
	m_PromptFWTO.clear();
	m_PromptFWDT.clear();
	//获取手牌中所有的基础牌型
	Trused::TCARDS tcHands = m_HandCard;
	sort(tcHands.begin(),tcHands.end(),cmp_min_max);
	//炸弹
	m_PromptBoom	= GetBaseCards(4,CT_BOOM, tcHands);
	//剔除手牌中的炸弹

	for (int i=0; i<m_PromptBoom.size(); i++)
	{
		tcHands.erase(remove(tcHands.begin(),tcHands.end(),m_PromptBoom.at(i)),tcHands.end());
	}


	//三张
	m_PromptThree	= GetBaseCards(3,CT_THREE,tcHands);
	//剔除手牌中的三张和炸弹
	for (int i=0; i<m_PromptThree.size(); i++)
	{
		tcHands.erase(remove(tcHands.begin(),tcHands.end(),m_PromptThree.at(i)),tcHands.end());
	}

	//两张
	m_PromptDouble  = GetBaseCards(2,CT_DOUBLE,tcHands);
	//剔除手牌中的炸弹，三张，一对
	for (int i=0; i<m_PromptDouble.size(); i++)
	{
		tcHands.erase(remove(tcHands.begin(),tcHands.end(),m_PromptDouble.at(i)),tcHands.end());
	}


	//除去炸弹，三张，一对后的就是单牌了
	m_PromptSingle = tcHands;


	sort(m_PromptSingle.begin(),m_PromptSingle.end(),cmp_min_max);
	sort(m_PromptDouble.begin(),m_PromptDouble.end(),cmp_min_max);
	sort(m_PromptThree.begin(),m_PromptThree.end(),cmp_min_max);

	//获取完所有基本牌型过后，单牌可由对子、三张、炸弹拆成
	//对子可由炸弹、三张拆解成，所以应当在结尾添加相应拆开的牌
	//拆牌的时候需要注意：炸弹拆三张可拆2个相同的
	Trused::TCARDS tmpThree;
	Trused::TCARDS tmpDouble;
	tmpDouble.clear();
	tmpThree.clear();
	//三张
	for (int i =0; i<m_PromptBoom.size(); i+=4)
	{
		tmpThree.push_back(m_PromptBoom.at(i));
		tmpThree.push_back(m_PromptBoom.at(i+1));
		tmpThree.push_back(m_PromptBoom.at(i+2));
	}

	//一对
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
	//单张
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

	//追加三张
	for (int i=0; i<tmpThree.size();i++)
	{
		m_PromptThree.push_back(tmpThree.at(i));
	}
	//追加一对
	for (int i=0; i<tmpDouble.size();i++)
	{
		m_PromptDouble.push_back(tmpDouble.at(i));
	}
	//三带二需需要基本牌型获取完毕后才可获取
	//将手牌中所有可能的三带二组合放入tmp向量中
	for (int i=0; i<m_PromptThree.size(); i+=3)
	{
		for (int j=0; j<m_PromptDouble.size(); j+=2)
		{	
			if (m_PromptDouble.at(j) == m_PromptThree.at(i))
			{
				continue;
			}
			//三张
			m_PromptTWT.push_back(m_PromptThree.at(i));
			m_PromptTWT.push_back(m_PromptThree.at(i+1));
			m_PromptTWT.push_back(m_PromptThree.at(i+2));
			//一对
			m_PromptTWT.push_back(m_PromptDouble.at(j));
			m_PromptTWT.push_back(m_PromptDouble.at(j+1));
		}
	}

	//三带一需需要基本牌型获取完毕后才可获取
	//将手牌中所有可能的三带一组合放入向量中
	for (int i=0; i<m_PromptThree.size(); i+=3)
	{
		for (int j=0; j<m_PromptSingle.size(); j++)
		{	
			if (m_PromptSingle.at(j) == m_PromptThree.at(i))
			{
				continue;
			}
			//三张
			m_PromptTWO.push_back(m_PromptThree.at(i));
			m_PromptTWO.push_back(m_PromptThree.at(i+1));
			m_PromptTWO.push_back(m_PromptThree.at(i+2));
			//一对
			m_PromptTWO.push_back(m_PromptSingle.at(j));
		}
	}

	//四带二需需要基本牌型获取完毕后才可获取
	//将手牌中所有可能的四带二组合放入向量向量中
	for (int i=0; i<m_PromptBoom.size(); i+=4)   //四带二带两张单牌
	{
		for (int j=0; j<m_PromptSingle.size()-1; j++)
		{	
			if (m_PromptSingle.at(j) == m_PromptBoom.at(i)
				||m_PromptSingle.at(j+1) == m_PromptBoom.at(i))
			{
				continue;
			}
			//四张
			m_PromptFWTO.push_back(m_PromptBoom.at(i));
			m_PromptFWTO.push_back(m_PromptBoom.at(i+1));
			m_PromptFWTO.push_back(m_PromptBoom.at(i+2));
			m_PromptFWTO.push_back(m_PromptBoom.at(i+3));
			//一对
			m_PromptFWTO.push_back(m_PromptSingle.at(j));
			m_PromptFWTO.push_back(m_PromptSingle.at(j+1));

		}
	}

	//四带二需需要基本牌型获取完毕后才可获取
	//将手牌中所有可能的四带二组合放入向量向量中
	for (int i=0; i<m_PromptBoom.size(); i+=4) //四带二带两对
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
			//四张
			m_PromptFWDT.push_back(m_PromptBoom.at(i));
			m_PromptFWDT.push_back(m_PromptBoom.at(i+1));
			m_PromptFWDT.push_back(m_PromptBoom.at(i+2));
			m_PromptFWDT.push_back(m_PromptBoom.at(i+3));
			//一对
			m_PromptFWDT.push_back(m_PromptDouble.at(j));
			m_PromptFWDT.push_back(m_PromptDouble.at(j+1));
			m_PromptFWDT.push_back(m_PromptDouble.at(j+2));
			m_PromptFWDT.push_back(m_PromptDouble.at(j+3));
		}
	}
}

//获取基础牌型
Trused::TCARDS GameTrusteeship::GetBaseCards(int nCardsLen, int nType, Trused::TCARDS handCards)
{
	Trused::TCARDS tmpCards;
	tmpCards.clear();

	if (nCardsLen > handCards.size() || handCards.size() <= 0 )
	{
		return tmpCards;
	}
	TCardVector dvMayBeOut;
	CardTypeInfo ctiMayBeOut;		//用于获取牌型信息
	CardTypeInfo ctiFrontMBO;		//前面一个牌型信息
	ctiFrontMBO.reset();
	ctiMayBeOut.reset();
	TCardVector  dvTmpHand = ConvertCardsToCardVector(handCards);

	//获取手牌中拥有的基础牌
	for (int i=0; i<=dvTmpHand.size()- nCardsLen; i++)  //for1   
	{
		dvMayBeOut.clear();
		dvMayBeOut.insert(dvMayBeOut.end(),dvTmpHand.begin()+i,dvTmpHand.begin()+i+nCardsLen);
		
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

//获取指定牌组中的某一种基本牌，并排除与needReject牌值相同的牌
Trused::TCARDS GameTrusteeship::GetBaseCardsEx(int nCardsLen, int nType, Trused::TCARDS handCards,Trused::TCARDS needReject)
{
	Trused::TCARDS tmpCards;
	tmpCards.clear();

	if (nCardsLen > handCards.size())
	{
		return tmpCards;
	}
	TCardVector dvMayBeOut;
	CardTypeInfo ctiMayBeOut;		//用于获取牌型信息
	CardTypeInfo ctiFrontMBO;		//前面一个牌型信息
	ctiFrontMBO.reset();
	ctiMayBeOut.reset();
	TCardVector  dvTmpHand = ConvertCardsToCardVector(handCards);
	TCardVector  dvTmpNR   = ConvertCardsToCardVector(needReject);

	//排除不需要获取的牌
	for(int i=0;i<dvTmpNR.size();i++)
	{
		dvTmpHand.erase(remove_if(dvTmpHand.begin(),dvTmpHand.end(),IsEqualValue(dvTmpNR[i])),dvTmpHand.end());
	}

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

