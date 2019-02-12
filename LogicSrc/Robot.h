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
// 唵嘛呢叭咪吽
//////////////////////////////////////////////////////////////////////////
//文件名:	Robot.h
//职能：	三人斗地主机器人
//by：      tmfish at 2014-03-29
//////////////////////////////////////////////////////////////////////////
#ifndef __ROBOT_H__
#define __ROBOT_H__
#include "Card.h"
#include <vector>
#include "GameRule.h"
#include "GameTrusteeship.h"
#include <stdlib.h>
#include <map>
#include "DebugOut.h"

const int nRivalCount = 2;	//对手个数
//#include <Windows.h>

//////////////////////////////////////////////////////////////////////////
//默认情况下各牌组的权值
const UINT nLevelSingle = 1;		//单张
const UINT nLevelDouble = 2;		//对子
const UINT nLevelThree	= 3;		//三张
const UINT nLevelThreeM	= 6;		//三顺
const UINT nLevelDoubleM= 4;		//双顺
const UINT nLevelSingleM= 5;		//单顺
const UINT nLevelBoom	= 7;		//炸弹
//////////////////////////////////////////////////////////////////////////
//牌型信息
struct CARDSTYLE{
	int max;//最大值
	int min;//最小值
	int m_value;//权值
	TCardVector cards;
	bool m_ISprimary;//主次之分,true为主牌,false为次牌 
	CARDSTYLE()
	{
		m_ISprimary =false;
	}
	void Reset()
	{
		max = 0;
		min =0;
		m_value =0;
		cards.clear();
	}
	void Fill(TCardVector dv,int nType = CT_NONE);
};

typedef vector<CARDSTYLE> CT;
//拆牌信息
struct  SplitInfo //拆牌信息
{
	int s_nSeat;		//拆牌玩家的位置
	CT s_boom;			//炸弹
	CT s_three;			//三张
	CT s_doule;			//对子
	CT s_single;		//单张
	CT s_threeMulti;	//三顺
	CT s_doubleMulti;	//双顺
	CT s_singleMulti;   //单顺
	CT s_maxBoom;	//火箭
	SplitInfo operator=(const SplitInfo& des)
	{
		this->s_boom = des.s_boom;
		this->s_three = des.s_three;
		this->s_doule = des.s_doule;
		this->s_single = des.s_single;
		this->s_threeMulti = des.s_threeMulti;
		this->s_doubleMulti = des.s_doubleMulti;
		this->s_singleMulti = des.s_singleMulti;
		this->s_nSeat   = des.s_nSeat;
		this->s_maxBoom = des.s_maxBoom;
		return *this;
	}
	void Append(CT& des, CT from)
	{
		for ( int i=0; i<from.size(); i++)
		{
			des.push_back(from.at(i));
		}
	}
	SplitInfo operator+ (const SplitInfo& from)
	{
		SplitInfo tmp; 
		tmp = *this;
		Append(tmp.s_boom,from.s_boom);
		Append(tmp.s_three,from.s_three);
		Append(tmp.s_doule,from.s_doule);
		Append(tmp.s_single,from.s_single);
		Append(tmp.s_singleMulti,from.s_singleMulti);
		Append(tmp.s_doubleMulti,from.s_doubleMulti);
		Append(tmp.s_threeMulti,from.s_threeMulti);
		Append(tmp.s_maxBoom,from.s_maxBoom);
		tmp.s_nSeat =MAX_PLAYER;
		return tmp;
	}
	int GetHandCount() //获取手数
	{
		int nLevel = s_boom.size() +s_three.size() + s_doule.size() + s_single.size() + s_threeMulti.size()
			+s_doubleMulti.size()+s_singleMulti.size() + s_maxBoom.size();
		//三带一、三代二、飞机的手数需要特殊处理
		int nThreeCount = 0;
		for (int i=0; i<s_threeMulti.size(); i++)
		{
			nThreeCount += (s_threeMulti.at(i).cards.size())/3;
		}
		nThreeCount += s_three.size();
		 int nMin = nThreeCount<(s_doule.size()+s_single.size())?nThreeCount:s_doule.size()+s_single.size();
		 nLevel-=nMin;
		// 炸弹作用极大，手数减1.
	//	nLevel -= s_boom.size();
		return nLevel;
	}
	int GetOneTypeLevel(CT& who) //获取某一牌型的权值
	{
		int nLevel = 0;
		for (int i=0; i<who.size();i++)
		{
			nLevel += who.at(i).m_value;
		}
		return nLevel;
	}
	int GetAllLevel() //获取总的权值
	{
		int nLevel = 0;
		nLevel = GetOneTypeLevel(s_boom) + GetOneTypeLevel(s_three)
			+ GetOneTypeLevel(s_doule) + GetOneTypeLevel(s_single)
			+ GetOneTypeLevel(s_threeMulti) + GetOneTypeLevel(s_doubleMulti)
			+ GetOneTypeLevel(s_singleMulti);			//总权值
		return nLevel;
	}
};

//玩家警报级别
struct PlayerWarring
{
	int nWL;
	int nSeat;
};

struct AICallOut {
	SHORT			returnscore;	// 叫分阶段时AI叫的分数 
};

//叫分阶段手牌信息
struct AICallIn{
	BYTE	val[15];				//手牌统计表 val[0]为CARD_3, val[1]为CARD_4 依次类推
	SHORT	callscore;				//叫分权值，权值计算方式--叫一次地主+1
};



typedef vector<TCardVector> CardsVector;

class CRobot 
{
public:
	CRobot(int nLandLord );	//默认构造函数，如果需要出牌或者跟牌，有且只有通过该构造函数进行构造
	CRobot(TCardVector handCards[MAX_PLAYER]);
	CRobot(TCardVector dvHandCards);
	~CRobot();
//调用接口
public:
	//机器人叫地主
	bool RobotCall(AICallIn theCallIn, AICallOut& theCallOut, int nBaseRand = 80);
	enum {CALL_0_1,CALL_1_2,CALL_1_2_3,CALL_3};
	int RobotCallEx(AICallIn theCallIn, AICallOut& theCallOut,int nBaseRand = 80);
	//////////////////////////////////////////////////////////////////////////
	//职能：机器人抢地主 --作废
	//说明：机器人抢地主实际上只是计算下自己手中
	//          大牌的权值，是否强地主交给游戏逻辑决定
	//打牌权值定义：炸弹为5 大王为5，小王为3, 2为2,A为1
	//参数：参数指定需要计算权值的牌
	//返回：返回指定牌组的权值
	int  RobotGrabLand(TCardVector dvHand);
	
	//计算与指定牌值相同的牌的数量
	int GetCardNum(int nCardVal, TCardVector dvSearch);
	//////////////////////////////////////////////////////////////////////////
	//函数职能：机器人出牌
	//参数：	handCards代表玩家手中的牌，nSeat指定当前出牌玩家位置
	//
	//返回:		返回需要打出的牌，该函数不存在也不允许失败。
	//by：      tmfish  
	virtual TCardVector RobotPlayCards(TCardVector handCards[MAX_PLAYER],int nSeat);
	//////////////////////////////////////////////////////////////////////////
	//函数职能：机器人跟牌
	//参数说明：参数1指定所有玩家的手牌，参数2指定需要压的牌，参数3指定需要压的牌是谁打出的
	//                 参数4用于返回跟牌，参数5指定是谁在跟牌
	//返回：       如果机器人成功找到压牌，返回true，如果返回false即不压牌
	virtual bool  RobotFollowCards(TCardVector handCards[MAX_PLAYER],TCardVector dvOut,
		int nOutSeat,TCardVector& dvRes,int nWhoOut);
	
	//拼命接牌，用于不接牌就会输掉了的情况。。比较捉急
	bool BendOverBackwardsFollow(TCardVector& dvRes,TCardVector dvOut);
	//玩家手中只剩下两手牌的时候的接牌
	bool PlayerFollowAtHand2(const TCardVector& dvOut ,TCardVector& dvRes);
	//获取建议的时间
	int    GetSuggestTime();
	//逻辑函数
	//设置建议时间
	void SetSuggestTime(int nTime);
protected:
	
	//重置所有数据
	void Reset();
	//重置临时数据
	void ResetTmp();
	//根据权值及手数选择最优拆牌方式,参数指定是否分析的是对手的牌，
	//如果分析的是对手的牌，那么将填充m_SplitRival结构体
	void FindOptimalSplit(int bRival = false);
	//将CardVector转换为CARDSTYPE
	//参数指定待转换的牌组，参数2返回转换后的结果
	void ConvertCVToCT(CardsVector& who, CT& ct,int nType = CT_NONE);
	//将CARDSTYPE转换为CardVector
	//参数指定待转换的牌组，参数2返回转换后的结果
	void ConvertCTToCV(CT& who,CardsVector& cv,int nType = CT_NONE);
	//分析牌
	//参数1指定需要分析的牌，参数2指定分析的牌的玩家位置
	//如果分析道当前出牌玩家，则返回手数，否则返回0
	int  AnalyzeCards(TCardVector dvHandCards, int nSeat);
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//选取单顺
	//选取规则
	/* 
		先取出最小的一个五连单顺，再在剩余的牌中选取最小的一个五连。依次类推，直到没有五连为止。
		完成后对五连进行扩展，将剩余的牌与已经取出的牌进行比对，如果某张剩余的牌与已知的连牌能组
		成更大的连牌，则将其合并。一直到无法合并为止。 最后，如果某两组连牌能够无缝连接成为更大的
		连牌，则将其合并为一组。
	*/
	void ChoseSingleMultis(CardsVector& cvNeedFill, TCardVector& dvFrom,CardsVector cvDoubles);
	//////////////////////////////////////////////////////////////////////////
	//选取双顺
	//选取规则
	/*
		将一副牌中所有的对子检测出来，然后将对子排序，如果有相连的对子，将其合并成(尽可能大的)双顺.
	*/
	void ChoseDoubleMultis(CardsVector& cvNeedFill, CardsVector& dvFrom);
	//////////////////////////////////////////////////////////////////////////
	//选取三顺
	/*
		一副牌中的所有三条都能比较方便地提取出来。 
		在三条的基础上进行比较，如果有相连的三条，则将其合并成（尽可能大的）三顺。
	*/
	void ChoseThreeMultis(CardsVector& cvNeedFill, CardsVector& dvFrom);
	//////////////////////////////////////////////////////////////////////////
	//选取三张
	void ChoseThree(CardsVector& cvNeedFill, TCardVector& dvFrom);		
	//////////////////////////////////////////////////////////////////////////
	//选取对子
	void ChoseDouble(CardsVector& cvNeedFill, TCardVector& dvFrom);
	//////////////////////////////////////////////////////////////////////////
	//选取炸弹,
	void ChoseBoom(CardsVector& cvNeedFill, TCardVector& dvFrom);
	//////////////////////////////////////////////////////////////////////////
	//寻找当前应当打出的牌
	//返回： 找到需要打出的牌后进行返回，该函数不允许失败
	TCardVector FindNowOut();

	//////////////////////////////////////////////////////////////////////////
	//以下为出牌查找函数组，函数功能皆为从指定的牌组数组中找到可以打出的牌
	//如果找到可以打出的牌，则返回1，并使用参数Res带回需要打出的牌
	//如果没有找到可以打出的牌或者打出的牌不合理，那么返回0 
	int  FindOutFromThreeMulti(TCardVector& Res);//三顺
	int  FindOutFromDoubleMuti(TCardVector& Res);//双顺
	int  FindOutFromSingleMuti(TCardVector& Res);//单顺
	int  FindOutFromThreeWTwo(TCardVector&Res); //三带二
	int  FindOutFromThrees(TCardVector& Res);//三张
	int  FindOutFromDoubles(TCardVector& Res);//对子
	int  FindOutFromSingle(TCardVector& Res);//单张
	//地主上家的出牌
	TCardVector FindOutFarmerSnipe();
	//地主下家的出牌
	TCardVector FindOutFarmerFire();
	//////////////////////////////////////////////////////////////////////////
	//一般条件下查找出牌
	//参数： Res用于返回查找的结果,From指定从何处查找
	//返回： 成功找到则返回1，否则返回0
	int	 FindGeneralOut(TCardVector& Res,CardsVector From);
	//无脑出牌
	int  FindOutUnthink(TCardVector& Res,CardsVector From);
	//在自己手牌手数为2的时候查找出牌
	TCardVector FindOutAtLevel2(); 
	//在自己手牌手数为3的时候查找出牌
	TCardVector FindOutAtLevel3();
	//农民手数为2的时候寻找出牌
	TCardVector FindOutAtFarmer2();
	//农民手数为3的时候寻找出牌
	TCardVector FindOutAtFarmer3();
	//农民手数较多的时候的出牌
	TCardVector FindOutAtFarmerG();
	//地主剩下一张牌的时候的时候农民出牌
	TCardVector	FindOutWhenLand1();
	//地主剩下两张牌的时候农民玩家出牌
	TCardVector FindOutWhenLand2();
	//判断指定对手是否有压过自己的指定牌型
	bool RivalHaveGreaterThanMe(CardsVector myCards); 
	bool RivalHaveGreaterThanMe(TCardVector myCards);
	
	
	//判断指定牌组是否拥有绝对控制权，
	//绝对控制权：第二小的牌组对手要不起
	bool HaveAbsoluteControl(CardsVector myCards);
	bool HaveAbsoluteControl(TCardVector myCards);
	//寻找队友可以压过的牌
	TCardVector FindTeammateCanOut();
	//一般情况下的出牌
	//成功返回出牌，函数不允许失败
	//参数指定不出的牌，暂定可能不应该打出的牌有，单张、对子、三顺
	TCardVector FindOutGeneral(bool bNotOutS=true,bool bNotOutD=true,bool bNotOutT=true);
	//计算自己某一牌组中对手要不起的牌组数
	int CalcGreaterThanRivalCount(CardsVector myCards, int& nSameCount);
	int CalcGreaterThanRivalCount(TCardVector myCards, int& nSameCount);
	//计算指定玩家的对手是否拥有能
	//找出某一牌组中牌值位于中间的牌
	TCardVector FindMidCard(CardsVector myCards);
	//赢面比较小的时候跑牌
	TCardVector FastOut(bool bMaxToMin=false);
	//地主玩家剩下两手牌的时候的出牌
	TCardVector FindOutAtLand2();
	TCardVector FindOutAtLand3();
	//地主玩家剩下多手的时候的出牌
	TCardVector FindOutAtLandG();
	//农民剩下1张牌的时候地主的出牌 
	TCardVector FindOutWhenFarmer1();
	//农民剩下2张牌的时候地主的出牌
	TCardVector FindOutWhenFarmer2();
	//为飞机找翅膀
	void FindWing(TCardVector dvPlant,TCardVector dvSingle, CardsVector dvDoule);
	//常规出牌
	bool FindFollowGeneral(TCardVector OutCard, TCardVector& FollwCards, int nLevel=0);
	//////////////////////////////////////////////////////////////////////////
	//寻找当前应该跟的牌,
	//找到跟上的牌后，返回true，并填充FollowCards为跟牌，
	//如果没有找到能跟上的牌，返回false,并将FollowCards置空
	bool FindNowFollow(TCardVector OutCard, TCardVector& FollwCards);
	
	//地主接牌
	bool FindFollowLand(const TCardVector& OutCard, TCardVector& FollowCards);
	//农民接牌
	bool FindFollowFarmer(const TCardVector& OutCard, TCardVector& FollowCards);
	//普通接牌
	bool GeneralFollw(TCardVector& dvRes,TCardVector dvOut);
	bool CmpSingle(TCards T1,TCards T2);
	//////////////////////////////////////////////////////////////////////////
	//查找能跟牌的牌组
	//参数：fromCards指定从哪里找压牌，dvOutCards指定需要压的牌，ResCards用于
	//返回找到的能够跟上的牌
	//返回：如果成功找到能压过的牌，返回true，否则返回false
	bool GetGreaterThanNextOut(TCardVector fromCards,TCardVector& dvOutCard,TCardVector& ResCards);
	//计算某玩家对手手牌中自己要不起的牌组个数
	int CalcAllNotGreateThanCards();
	/////////////////////////////////////////////////////////////////////
	//职能：查找是否拥有指定牌型的牌
	//参数：nCardsLen牌型长度，nType牌目标型，targetCards目标牌组
	//返回：返回目标牌组中所有的nType牌型，如果目标牌组中没有指定
	//      的牌型，返回size为0的向量
	TCardVector GetBaseCards(int nCardsLen, int nType,  TCardVector targetCards);
	//////////////////////////////////////////////////////////////////////////
	//填充牌组数组数据
	//参数： nLen指定填充的牌组原子牌型长度，nType，指定填充的牌组内的牌型，
	//       dvNeedFill，指定需要填充的牌组数组,dvWhere指定需要重哪儿选取牌值进行填充
	void FillCards(int nLen,int nType,CardsVector& dvNeedFill,TCardVector& dvWhere);
	//////////////////////////////////////////////////////////////////////////
	//寻找dvCards中的最小的五连
	TCardVector FindFiveMultis(TCardVector& dvCards);
	//寻找当前所有玩家手牌中最大的三张牌
	void FindMaxThreeCards();
	//计算指定牌组中各个牌值的数量
	void CalcCardConut(int nNum[],TCardVector dvCards);
	//计算本方(自己和队友)与当前所有牌的大牌数量
	void CalcBigNum(int& my, int& all);
	void CalcBigNum(int& nBig, TCardVector dvWho);
	//////////////////////////////////////////////////////////////////////////
	//获取玩家上家，玩家下家位置
	int GetNextPlayerSeat(int nSeat);
	int GetLastPlayerSeat(int nSeat);
	//////////////////////////////////////////////////////////////////////////
	//找寻能够压过的牌
	//OutCard 指定需要压的牌，参数fromCards指定需要从哪儿找压牌，参数三返回
	//找到的压牌
	//如果成功从fromCards中找到能压过OutCard的牌，那么返回true并且以FollowCards返回
	//找到的压牌，如果fromCards中没有牌组能够压过OutCard，那么返回false并且将FollowCards
	//清空
	bool FindFollowCards(TCardVector OutCard,CardsVector fromCards,TCardVector& FollowCards, int nLevel);
	//为三张和三顺预留比较小的单张或者对子
	void ReservedSmallCards(bool bChoseSingle = false);
	//为一组孤立的牌组中的每一张牌寻找与之匹配的最优地点
	void FindFamily(TCardVector dvOrphan);
	//遍历拆牌
	void ErgodicCards(bool bBoom,bool bThree, bool bDouble, bool bThreeMulti,bool bDouleMulti,bool bMulti,int nSeat );
	//分离三带一，三带二，飞机带翅膀
	//参数1指定牌组，参数二返回三带中的三张
	void SeparationCards(const TCardVector& dvCards,CardsVector& cvMain);
	//用于剔除对子数组中值与dvDel相同的元素
	void DelCV(CardsVector& cvWho,const TCardVector& dvDel);
protected:
	TCardVector     m_dvAllCards;											//玩家手牌之和
	TCardVector		m_dvPlayerCards[MAX_PLAYER];		//玩家手牌
	TCardVector		m_dvBackupPlayerCards[MAX_PLAYER];	//玩家手牌备份
	TCardVector		m_dvNowCards_1;				//出牌玩家的手牌
	TCardVector		m_dvOldCards;				//出牌玩家的手牌的备份
	int				m_nLandlord;				//地主玩家位置
	int				m_nOutSeat;					//出牌玩家位置
	int				m_nLastOut;					//上一手牌出牌玩家位置
// 自己的牌组 
	CardsVector		m_dvSingleMultis;	//单顺牌组数组，存放分析后的所有单顺
	CardsVector		m_dvDoubleMultis;	//双顺牌组数组，存放分析后的所有双顺
	CardsVector		m_dvThreeMultis;	//三顺牌组数组，存放分析后的所有三顺
	CardsVector		m_dvDoubles;		//对子数组，存放分析后的所有对子
	CardsVector     m_dvThrees;		    //三条数组，存放分析后的所有三条
	CardsVector		m_dvBooms;			//炸弹数组，存放分析后的所有炸弹
	TCardVector		m_dvSingles;		//单张数组，存放分析后的所有单张
	TCardVector     m_dvMaxBoom;	//王炸
	int				m_nRivalCount;								//对手手中剩余牌张数
	int				m_nThreatenPlayCards;					//最具威胁玩家手牌数，对于地主而言，最具威胁玩家
	int				m_nLevel[MAX_PLAYER];			//手数，最快需要几把能把手牌全部打完
	PlayerWarring	m_Warring[MAX_PLAYER];								//警报级别

	TCardVector m_dvMaxThreeCards;	//最大的三张牌组
	//临时数组
	TCardVector		cvTmpJoker;				//临时存放王牌
	TCardVector     cvTmpMaxBoom;		//临时存放火箭数组
	CardsVector		cvTmpDoubleMulti;	//双顺牌组数组，
	CardsVector		cvTmpThreeMulti;	//三顺牌组数组，
	CardsVector		cvTmpDouble;		//对子数组，
	CardsVector     cvTmpThree;		    //三条数组，
	CardsVector		cvTmpBoom;			//炸弹数组，
	TCardVector		cvTmpSingle;		//单张数组
	CardsVector     cvTmpSingleMulti;	//单顺数组

	int				m_nLevelRival;											//对手当前手数
	vector<SplitInfo> m_allSplitInfo[3];							//所有可能拆牌信息
	SplitInfo		  m_PlayerSplit[MAX_PLAYER];			//玩家牌分析后的结果
	SplitInfo		 m_RivalSplit;											//对手的手牌分析
	int				m_nCardValue[15];									//所有玩家手中各个牌值的张数，下标0为CV_3，依次递增
	int				m_nSuggestOutTime;								//建议的出牌时间
private:
	TCardVector MeWillFinish();
	//当对手快要打完的时候，寻找出牌
	TCardVector RivalWillFinish();
private:
	/*
	*  类职能：STL remove_if算法的算子，用于剔除牌组中牌值相同的牌
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
public:
		enum
		{
			CARD_3	 = 0,
			CARD_4,
			CARD_5,
			CARD_6,
			CARD_7,
			CARD_8,
			CARD_9,
			CARD_10,
			CARD_J,
			CARD_Q,
			CARD_K,
			CARD_A,
			CARD_2,
			CARD_SJ,    // 小王
			CARD_BJ, 	// 大王
		};
private:
	void ShowDebug(CardsVector ct,char* name);
	void ShowDebug(TCardVector ct,char* name);
	void ShowDebug(SplitInfo sp,char* who);
	void ShowDebug(CT ct,char* name);
	
};


#endif  //__ROBOT_H__