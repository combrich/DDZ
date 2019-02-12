#pragma once
#include "Card.h"
#include <functional>

//牌型
typedef enum tagCardType
{
	CT_NONE			= 0x00,	// 无牌型,不符规则
	CT_SINGLE,				// 单张
	CT_DOUBLE,				// 一对
	CT_MULTIDOUBLETWO,		// 对子组成的姐妹对
	CT_MULTITHREE,			// 三张组成的姐妹对
	CT_THREE,				// 三张
	CT_THREEWITHTWO,		// 三带二
	CT_MULTITHREEWITHTWO,	// 大飞机/带一队
	CT_THREEWITHONE,		// 三带一
	CT_MULTITHREEWITHONE,	// 小飞机
	CT_MULTI,				// 顺子
	CT_FOURWITHTWO,			// 四带二，带的是两张(形如AAA25)
	CT_FOURWITHDTWO,		// 四带二，带的是两对(形如AAA2233)
	CT_BOOM,				// 炸弹
	CT_MAXBOOM,				// 一对鬼
	CT_COUNT,
	
}TCARDTYPE;

//牌型信息类
class CardTypeInfo
{
public:
	CardTypeInfo() : nType(CT_NONE){ }
	~CardTypeInfo(){ }
public:
	TCARDTYPE	nType;			// 牌型
	size_t					nCount;			// 牌型中牌的个数
	size_t					nLevel;			// 级别，定义为牌组的所有牌值之和
								// 三带二、三带一、飞机为三张牌之和
public:
	void reset()
	{
		nType  =  CT_NONE;
		nCount = 0;
		nLevel = 0;
	}
	CardTypeInfo& operator=(CardTypeInfo cti)
	{
		this->nCount = cti.nCount;
		this->nLevel = cti.nLevel;
		this->nType  = cti.nType;
		return *this;
	}

	bool operator==(CardTypeInfo cti)
	{
		if(this->nCount == cti.nCount && 
			this->nLevel == cti.nLevel && 
			this->nType  == cti.nType
		 )
		{
			return true;
		}else
		{
			return false;
		}
	}
};

//是否能出牌
typedef enum tagOutCardResult
{
	OCR_OK = 0,				// 能出牌
	OCR_OUTERROR,	// 不能出牌
}OutCardResult;

//比较两张牌的大小
bool operator < (const TCards& card1, const TCards& card2);
bool operator > (const TCards& card1, const TCards& card2);
//转换牌类信息到BYTE数组中
int ConvertDataVector(const TCardVector& dvCards, BYTE* arrCardID, 
					  const int& nMaxCount);


//////////////////////////////////////////////////////////////////////////
//游戏规则类：
//		核心功能--1.找出牌型
//				  2.比较大小
//				  3.决定是否允许出牌
//		
class GameRule
{
public:
	GameRule();
	~GameRule();

public:
	//////////////////////////////////////////////////////////////////////////
	//初始化数据
	static void InitData(int nLevel = CV_2);
public:
	//////////////////////////////////////////////////////////////////////////
	//职能：获取单牌的权值
	//参数：参数指定牌
	//返回：返回参数传递的单牌的权值
	static int CardWeight(TCards card);
	//////////////////////////////////////////////////////////////////////////
	//职能：排序牌组
	//参数：dvCards指定需要排序的牌组，bLess指定牌型方式
	//		TURE由小到大，false为从大到小排序
	static void SortCards(TCardVector& dvCards, bool bLess = true);

	//////////////////////////////////////////////////////////////////////////
	//职能：获取牌组类型
	//参数：dvCards指定需要获取类型的牌组
	//返回：成功获取牌组后，将牌组信息放入牌组信息类返回		
	static CardTypeInfo GetCardType(const TCardVector& dvCards);

	//////////////////////////////////////////////////////////////////////////
	//职能：判断打出的牌是否可以出牌
	//参数：bFirstOut确定是否第一手出牌，第一手出牌可出随意牌组
	//		dvFirstOutCards指定前一个玩家打出的牌
	//		dvNowOutCards指定需要判断是否能压过上家出牌的牌组。
	//返回：如果允许出牌，返回真，否则假
	//附注： 确定是否允许出牌的核心思想
	//	规则1.牌型，牌张数两者相同，则牌组的牌值之和最大，则牌组最大
	//	规则2.炸弹比任何非炸弹都大,一对鬼最大
	//	规则3.炸弹和炸弹遵循规则1
	//	规则4.三带二、三带一、飞机不遵循规则1，其比较规则为相同的三张牌的牌值比较。
	//		  大则大，小则小
	//	规则5.当第一手出牌或者牌型大的时候则可以出牌，第一手出牌指代
	//        游戏中对手放弃出牌或者开局第一次出牌
	//		  
	static bool CanOut(bool bFirstOut, const TCardVector& dvFirstOutCards,
					   const TCardVector& dvNowOutCards);

	//////////////////////////////////////////////////////////////////////////
	//职能：确定是否拥有第一手出牌资格
	//参数：pdvHandCards指定所有玩家的手牌
	//		nSize指定牌组指针大小
	//返回：如果拥有第一手出牌资格，返回其位于牌组指针中的
	//		位置
	//附注：此处的第一手判断资格为争夺开局的第一次出牌资格
	static int FirstOutCard(const TCardVector  pdvHandCards[], int nSize);
	

	//////////////////////////////////////////////////////////////////////////
	//职能：判断牌型是否是连牌
	//参数：dvCards指定需要判断的牌组、cardsInfo用于返回牌组信息
	//返回：如果是连牌，返回true，否则返回0
	//附注：算法核心为等差数列求和公式及通项公式
	//		等差数列通项公式：		An=a1+(n-1)*d
	//		等差数列前N项和公司：	Sn=n(n-1)*d/2、和=（首项+末项）×项数÷2
	static bool IsMulti(const TCardVector& dvCards, CardTypeInfo& cardInfo);
	//////////////////////////////////////////////////////////////////////////
	//职能：判断牌型是否是姐妹对
	//参数：dvCards指定需要判断的牌组、cardsInfo用于返回牌组信息
	//返回：如果是连牌，返回true，否则返回0
	//附注：算法核心为等差数列求和公式及通项公式
	//		等差数列通项公式：		An=a1+(n-1)*d
	//		等差数列前N项和公司：	Sn=n(n-1)*d/2、和=（首项+末项）×项数÷2
	static bool IsDoulbeMulti(const TCardVector& dvCards, CardTypeInfo& cardInfo);
	//////////////////////////////////////////////////////////////////////////
	//职能：判断牌型是否是三张姐妹对
	//参数：dvCards指定需要判断的牌组、cardsInfo用于返回牌组信息
	//返回：如果是连牌，返回true，否则返回0
	//附注：算法核心为等差数列求和公式及通项公式
	//		等差数列通项公式：		An=a1+(n-1)*d
	//		等差数列前N项和公司：	Sn=n(n-1)*d/2、和=（首项+末项）×项数÷2
	static bool IsThreeMulti(const TCardVector& dvCards, CardTypeInfo& cardInfo);

	//////////////////////////////////////////////////////////////////////////
	//职能：判断牌型是小飞机
	//参数：dvCards指定需要判断的牌组、cardsInfo用于返回牌组信息
	//返回：如果是小飞机，返回true，否则返回0
	static bool IsMultiThreeWithOne(const TCardVector& dvCards, CardTypeInfo& cardInfo);
	//////////////////////////////////////////////////////////////////////////
	//职能：判断牌型是否是大飞机
	//参数：dvCards指定需要判断的牌组、cardsInfo用于返回牌组信息
	//返回：如果是大飞机飞机，返回true，否则返回0
	static bool IsMultiThreeWithTwo(const TCardVector& dvCards, CardTypeInfo& cardInfo);
};