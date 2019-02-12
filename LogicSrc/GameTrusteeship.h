/************************************************************************/
/*  文件名：GameTrusteeship.h											*/
/*  职能：  斗地主托管类												*/
/*  版本：  1.0															*/
/*	创建者: tmfish														*/
/*  日期：  2013/12/10													*/
/************************************************************************/

#pragma once
#include "Card.h"
#include "Define.h"
#include "GameRule.h"

namespace Trused
{
	typedef int TCARD;
	typedef vector<TCARD> TCARDS;
}

class GameTrusteeship
{
public:
	GameTrusteeship(void);
	GameTrusteeship(TCardVector handCard){ m_HandCard = this->ConvertCardVectorToCards(handCard);}
	virtual ~GameTrusteeship(void);
public:
	//////////////////////////////////////////////////////////////////////////
	//职能：查找手牌中大于上家出牌的牌组，并将找到的牌组设置为选中状态
	//参数：citFrontOut为上家出牌的牌组信息
	//返回：如果成功找到满足条件的牌组，返回true否则返回false
	bool GetGreaterThanNextOut(CardTypeInfo ctiFrontOut);

protected:
	//判断玩家手牌中是否有可以打出的牌
	bool HaveOutCard(int nCardsLen, CardTypeInfo ctiFrontOut ,Trused::TCARDS handCards, int nStep =1);
	//判断玩家手牌中是否有可以打出的牌,及判断玩家手中是否有炸弹牌型
	bool HaveOutCardBoom(int nCardsLen, CardTypeInfo ctiFrontOut ,Trused::TCARDS handCards, int nStep =1);
	//判断玩家手牌中是否有可以打出的牌,飞机牌型
	bool HaveOutCardMaxBoom(int nCardsLen, CardTypeInfo ctiFrontOut ,Trused::TCARDS handCards, int nStep =1);
	//用于判定玩家手中是否有可以打出的飞机牌型
	bool HaveOutCardEx(int nCardsLen,CardTypeInfo ctiFrontOut,Trused::TCARDS handCards);
	//获取玩家手牌中拥有的基础牌型，基础牌型为单张、对子、三张、炸弹
	//并将获取到的基础牌型放入指定的容器中
	void FillBaseCards();
	//获取基础牌型
	Trused::TCARDS GetBaseCards(int nCardsLen, int nType, Trused::TCARDS handCards);
	//获取指定牌组中的某一种基本牌，并排除与needReject牌值相同的牌
	Trused::TCARDS GetBaseCardsEx(int nCardsLen, int nType, Trused::TCARDS handCards,Trused::TCARDS needReject);
	void Clear();
private:
	/*
	*  类职能：STL erase算法的算子，用于剔除牌组中牌值相同的牌
	*/
	class IsEqualValue{
	public :
		IsEqualValue(TCards &temp):tCard(temp){} 
		bool operator() (TCards &temp)
		{
			if(tCard.GetID()==temp.GetID())
				return true;
			return false;
		}
	private :
		TCards  tCard;
	};

private:
	//////////////////////////////////////////////////////////////////////////
	//存放的方式为从小到大连续存放
	Trused::TCARDS			m_PromptSingle;						//玩家手牌中拥有的所有单牌
	Trused::TCARDS			m_PromptDouble;						//玩家手牌中拥有的所有对子
	Trused::TCARDS			m_PromptThree;						//玩家手牌中所有的三张
	Trused::TCARDS			m_PromptBoom;						//玩家手牌中所有的炸弹
	Trused::TCARDS          m_PromptTWT;						//玩家手牌中所有可组成的三带二
	Trused::TCARDS			m_PromptTWO;						//玩家手牌中所有可组成的三带一
	Trused::TCARDS			m_PromptFWTO;						//玩家手牌中所有可组成的四带二(二为单张)
	Trused::TCARDS			m_PromptFWDT;						//玩家手牌中所有可组成的四带二(二为对子)
	Trused::TCARDS			m_PromptMTWO;						//玩家手牌中所有可组成的小飞机
	Trused::TCARDS			m_PromptMTWT;						//玩家手牌中所有可组成的大飞机
	//////////////////////////////////////////////////////////////////////////
	//手牌
	Trused::TCARDS m_HandCard;
	SHORT  m_nSeat;		

public:
		Trused::TCARDS m_PromptCard;
private:
	TCardVector ConvertCardsToCardVector(const Trused::TCARDS& vCards)
	{
		TCardVector dvCards;
		for(size_t i=0; i<vCards.size(); i++)
		{
			dvCards.push_back(TCards(vCards[i]));
		}

		return dvCards;
	}
	Trused::TCARDS ConvertCardVectorToCards(const TCardVector& dvCards)
	{
		Trused::TCARDS vCards;
		for(size_t i=0; i<dvCards.size(); i++)
		{
			vCards.push_back(dvCards[i].GetID());
		}

		return vCards;
	}
};
