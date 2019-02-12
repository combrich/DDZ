#ifndef __GAMELOGIC_PROTOCOL_H__
#define __GAMELOGIC_PROTOCOL_H__

#include "Define.h"
#include "FrameworkProtocol.h"

#include "biostream.h"
#include "typedef.h"

#pragma pack(1)

#define GLXY_FIRST_ID 0x0400

enum GL_XYID
{
	GLXY_FIRST	= GLXY_FIRST_ID,

	/* 以下协议游戏必须实现	*/
	
	GLXY_ACTION,						// 玩家动作（一般权值客户端做出来某些操作需要让其他客户端显示动画时使用）
	GLXY_TABLERULE,				// 桌子规则
	GLXY_ENTERGAME,				// 进入游戏(只有玩家可以收到)
	GLXY_STARTGAME,				// 开始游戏
	GLXY_ENDGAME,				// 结束游戏
	GLXY_LEAVEGAME,				// 进入游戏(只有玩家可以收到)
	GLXY_PLAYINFO,				// 游戏信息
	GLXY_TIMEOUT,				// 玩家超时
	GLXY_POWER,					// 玩家权限
	GLXY_ADURL,					// 广告地址
	GLXY_OPENCARD,		//	眀牌消息
	GLXY_TAKE,						// 发牌
	/* 以下由游戏程序员扩展	*/
	GLXY_QIAOPIHUA,			//发言
	GLXY_GRABLANDLORD,			//抢地主
	GLXY_RELINK,				// 重连
	GLXY_TALKINFO,				// 系统消息
	GLXY_AUTOOUT,				// 自动出牌消息
	GLXY_MOVEINFO,				// 移动玩家信息

	GLXY_MAKECARD,				//做牌 ---测试人员专用，不应当出现在最终版本中
	GLXY_CARDS,					// 手牌
	GLXY_BASE,					// 底牌
	GLXY_RECVCARD,              // 收到发的牌

	GLXY_OUTCARD,				// 出牌 S <-> C
	GLXY_FIRSTOUTCARD,			// 首先出的牌 S -> C
	GLXY_STARTTURN,				// 开始一轮
	GLXY_ENDTURN,				// 结束一轮
	GLXY_LOGICCONFIG,			// 逻辑配置
	GLXY_LOGICDATA,				// 逻辑数据
	GLXY_GAMESTATE,				// 游戏状态
	GLXY_BANKERSEAT,			// 庄家位置
	GLXY_CLOCK,					// 时钟
	GLXY_GAMEDATA,
	GLXY_KOUDI,					// 扣底(S <-> C)
	GLXY_LIANGZHU,				// 亮主(S <-> C)
	GLXY_ENDREVERSEHOST,		// 反主结束
	GLXY_ENDSENDCARD,           // 发牌结束
	GLXY_WRONGOUT,				// 甩牌错误 S -> C 
	GLXY_PLAYERIDENTITY,	//玩家身份
	GLXY_CALLANTE,				// 叫分

	GLXY_EXITPROCRES,		//逃跑
	GLXY_ADDMULTIPLE,		//加倍
	GLXY_MISSION,//任务系统
	GLXY_TRUST,	//托管

	/* 以上由游戏程序员扩展	*/
	GLXY_COUNT,
	GLXY_LAST
};

//////////////////////////////////////////////////////////////////////////
// 游戏逻辑协议


//托管权限C<->S
struct msgTrust
{
	enum {XY_ID = GLXY_TRUST};
	enum {T_BEGIN,T_END};	//开始托管，结束托管
	SHORT nSeat;		//玩家位置 (C->S)
	size_t	 nType;		//类型

};

//做牌
struct msgMakeCard
{
	enum {XY_ID =GLXY_MAKECARD};
	enum {NOTMAKE =0, //不做牌
		MAKEBASE,  //做底牌
		MAKEHANDCARD};  //做手牌

	SHORT	nSeat;		//玩家位置
	bool	bIsMake;	//是否做牌
	BYTE 	cards[20];	//牌数据
	int     nCount;
	int    nType;		//做什么牌
};

//任务
struct msgMission
{
	enum {XY_ID = GLXY_MISSION};
	enum {
		MCT_THREEE		= 0,	//三张
		MCT_THREEW_OT,			//三带一(二)
		MCT_FOUR_T,				//四带二
		MCT_SINGLEMULTI,		//单顺
		MCT_DOUBLEMULTI,		//双顺
		MCT_FEIJI,				//飞机
		MCT_FEIJIDCB,			//飞机带翅膀
		MCT_BOOM,				//炸弹
		MCT_MAXBOOM,			//火箭
		MCT_NONE,				//错误

	};
	int		nMisssionID;	//任务ID
};
//用于发送游戏倍数信息
struct msgAddMult
{
	enum {XY_ID = GLXY_ADDMULTIPLE};
	int	 nMultiple;		//倍数
};

// 动作(S --> C)
struct msgAction
{
	enum { XY_ID = GLXY_ACTION };
	enum Action
	{
		ACT_ANIFINISH = 0,		// 动画完成
		ACT_ANIGRAB,			// 抢地主
		ACT_FEIJI,				// 飞机
		ACT_HUOJIAN,			// 火箭
		ACT_SANDAIER,			// 三带二
		ACT_SANDAIYI,			// 三带一
		ACT_SANSHUN,			// 三顺
		ACT_SHUANGSHUN,			// 双顺
		ACT_SHUNZI,				// 顺子
		ACT_SIDAIER,			// 四带二
		ACT_ZHADAN	,			// 炸弹
		ACT_OPENCARD,			// 明牌
		ACT_LANDWIN,			// 地主赢 (农民输)
		ACT_LANDLOS,			// 地主输 (农民赢)
		ACT_LANDCHUNTIAN,		// 地主春天
		ACT_NOLANDCHUNTIAN,		// 农民春天
		ACT_FINISHMISSION,		// 任务完成
	};

	BYTE		theFromSeat;					// 谁发送动作
	BYTE		theToSeat;						// 谁接受动作，如果>=MAX_PLAYERS的话对所有玩家操作
	BYTE		theAction;						// 动作	
};

//发送玩家身份
struct msgPlayerIdentity
{
	enum {XY_ID = GLXY_PLAYERIDENTITY};
	SHORT  nSeat;		//位置
	bool   bIsLandlord;	//是否是地主
};

// 游戏配置(S --> C)
struct msgTableRule
{
	enum { XY_ID = GLXY_TABLERULE };

	size_t		nRoomType;							//房间类型，比赛?普通
	size_t		nGameType;							//游戏类型，财富?积分		
	size_t		nGrabLandType;				//抢地主方式,抢地主？叫分
	int				nBase;										//房间底分
};

struct msgTake
{
	enum { XY_ID = GLXY_TAKE };
	enum {TAKE_BEGIN, TAKE_END};
	BYTE nType;
};

struct msgExitProcRes
{
	enum { XY_ID = GLXY_EXITPROCRES };
	BYTE	nFlag;
	SHORT   nSeat;
	long8	nExitScore;
};

// 进入游戏(S --> C)
struct msgEnterGame
{
	enum { XY_ID = GLXY_ENTERGAME };
};

// 开始游戏(S --> C)
struct msgStartGame
{
	enum { XY_ID = GLXY_STARTGAME };

	BYTE		theIsGameStart;
};

//////////////////////////////////////////////////////////////////////////
//发言
struct msgQiaoPiHua
{
	enum { XY_ID = GLXY_QIAOPIHUA };

	BYTE nFlag;
	BYTE nNum;	// 俏皮话编号
	BYTE nSeat;
};

// 结束游戏(S --> C)
struct msgEndGame
{

	enum { XY_ID = GLXY_ENDGAME };
	enum { 
		// 游戏结束类型
		COMMON=0,	// 正常结束
		ESCAPE,			// 有人逃跑结束
	};	

	long8 nWinLost[MAX_PLAYER];
	long8 nHistory[MAX_PLAYER];
	int	  nExp[MAX_PLAYER];	// 经验值
	int   nLandLord;		// 地主位置
	int   nCountBoom;		// 炸弹出现次数
	int   nGrabCount;		// 抢地主次数
	int	nCallScore;			//叫分分数
	int   nBaseMultiple;	// 底牌倍数
	int   nTotalMultiple;	// 总倍数
	bool  bLandLordWin;		// 是否是地主赢了
	bool  bIsNoOutCard;		// 是否是春天
	bool  bFanChunTian;		// 是否是反春天
	bool  bIsOpenHands;		// 是否明牌
	bool  bFinishMission;	// 完成任务
	int	  nGameType;		//游戏类型
};

// 离开游戏(S --> C)
struct msgLeaveGame
{
	enum { XY_ID = GLXY_LEAVEGAME };
};

// 游戏信息(S --> C)
struct msgPlayInfo
{
	enum { XY_ID = GLXY_PLAYINFO };
};

// 玩家超时
struct msgTimeOut
{
	enum {XY_ID = GLXY_TIMEOUT};
};


// 抢地主(C<-->S)
struct msgGrabLandlord
{
	enum {XY_ID = GLXY_GRABLANDLORD};
	enum {
		CALL		= 1,		//叫地主
		NOTCALL,			//不叫
		GRAB,					//抢地主
		NOTGRAB,		//不抢
		CALLSCORE,	//叫分
	};
	//用于抢地主模式
	SHORT thePlayerSeat;		//玩家位置
	int   thePlayerOperator;		//玩家抢地主的操作
	//用于叫分模式
	int	nScore;								//叫分多少
};

// 玩家权限
struct msgPower
{
	enum { XY_ID = GLXY_POWER };

	enum 
	{
		PowerNone = 0x00,
		PowerGrab,
		PowerPlay,
	};

	int  nWhoPower;						// 谁的权限
	int  nPower;								// 关于什么的权限
	bool bCanGiveUp;					// 可否放弃出牌
	bool isCallLandLord;			// 是否是叫地主
	bool HaveGrabPower;			// 是否拥有抢地主的权限
	bool bOpenCard;					// 可以叫明牌(明牌只有是抢完地主后地主有权限叫明牌)
	bool bIsFirstOut;					// 是首轮出牌
	int  nCountGiveUp;				// 连续放弃出牌的玩家
	int  nScore;								// 叫地主的时候，已经叫了多少分
};

//出牌玩家数据
struct msgOutCard
{
	enum { XY_ID = GLXY_OUTCARD };
	enum { NONE =0,
		OUTCARD,						//玩家出牌
		GIVEUP,						//放弃出牌
	};
	BYTE		nType;					// 类型
	BYTE		nSeat;					// 谁的牌
	BYTE		cards[MAX_HANDCARDS];	// 牌数据
	int 		 nCount;					// 牌数
	int         nHandCount;				// 玩家手牌数
};

//明牌
struct msgOpenCard
{
	enum {XY_ID = GLXY_OPENCARD};
	int		nSeat;	
};

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!到时候再重新设计考虑!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// 重连，观战
struct msgReLink 
{
	enum { XY_ID = GLXY_RELINK };

	BYTE		nGameState;									// 状态
	BYTE		theBanker;									// 庄家
	int			theBaseAnte[MAX_PLAYER];					// 基础
	int			theAnte[MAX_PLAYER];						// 4个人叫的基础分
	BYTE		theReady[MAX_PLAYER];						// 是否已经准备好了 = 0没有准备 = 1表示已经准备了
	BYTE		theCall[MAX_PLAYER];						// 是否叫庄过了，叫了就让最后一个人来当庄家
	BYTE		theNowSeat;									// 当前位置
	BYTE		theLastPlayer;								// 最后一个出牌的玩家	
	BYTE		theHandCount[MAX_PLAYER];					// 手牌张数

};

struct msgTalkInfo
{
	enum { XY_ID = GLXY_TALKINFO};

	char strInfo[50];
};

//自动出牌消息
struct msgAutoOut
{
	enum{ XY_ID = GLXY_AUTOOUT};

	int theSeat;             //自动出牌的玩家位置
};

// 移动玩家头像
struct msgMoveInfo
{
	enum { XY_ID = GLXY_MOVEINFO };

};

// 底牌(S --> C)
struct msgBase
{
	enum { XY_ID = GLXY_BASE };
	enum { ADD=0,
		ADDS,
		DEL,
		UPDATE                           // 更新牌，去掉原来的所有牌
	};
	enum
	{
		NOACTION,				//无动作
		TAKECARD ,			//发牌
		ADDHAND					//附加到地主手牌中
	};
// 
 	BYTE		nBaseType;							// 底牌的牌型
 	BYTE		nType;										// 类型
	BYTE		cards[BASE_CARDS];	// 牌数据
 	int 				nCount;									// 牌数
	BYTE		nActionType;						// 动画类型
};

// 发牌(S --> C)
struct msgCards
{
	enum { XY_ID = GLXY_CARDS };
	enum { ADD=0,
		   ADDS,
		   DEL,
		   UPDATE,                          // 更新牌，去掉原来的所有牌
		   TAKE,								  // 发牌	
	};

	BYTE		nType;					// 类型
	BYTE		nSeat;					// 谁的牌
	BYTE		cards[MAX_HANDCARDS];	// 牌数据
	int 		nCount;					// 牌数
};

struct msgEndSendCard
{
	enum { XY_ID = GLXY_ENDSENDCARD };

	DWORD theDownTime;

	BYTE		nFlag;
	int			nMaxseat;
	int         nMinseat;
	int			nTakeCount;
};

// 开始一轮(S --> C)
struct msgStartTurn
{
	enum { XY_ID = GLXY_STARTTURN };

	BYTE		theBanker;							// 庄家
	BYTE        theKouDiPlayer;                     // 最后扣底的玩家
};

// 结束一轮(S --> C)
struct msgEndTurn
{
	enum { XY_ID = GLXY_ENDTURN };

	BYTE        theOutGreatestSeat;                     // 该局出牌最大的玩家

	int         theScoreCardsCount;                     // 抓的分牌的个数
	int         theCatchScore;                          // 抓分方抓分

	bool        isLastTurn;                             // 最后一轮
};

struct msgLogicConfig
{
	enum{XY_ID = GLXY_LOGICCONFIG};

	BYTE		IsSysTrust;						// 是否自动托管
	BYTE		WaitLastReadyType;				// 等待最后一个玩家的方式。1，自动开始；2，自动退出；其它，等待。
};

struct msgLogicData
{
	enum{XY_ID = GLXY_LOGICDATA};

	LONG		BasePlayTypeScore;				// 底

	msgLogicData()	{ reset();						}
	void reset()	{ memset(this,0,sizeof(*this)); }

	friend bostream& operator<<(bostream& ar, const msgLogicData& obj )
	{
		ar << obj.BasePlayTypeScore;
		return ar;
	}

	friend bistream& operator>>(bistream& ar, msgLogicData& obj )
	{
		obj.reset();
		ar >> obj.BasePlayTypeScore;
		return ar;
	}
};

struct msgGameData
{
	enum{XY_ID = GLXY_GAMEDATA};

	LONG		nBaseScore;				// 底
	LONG		nTotalScore;
	msgGameData()	{ reset();						}
	void reset()	{ memset(this,0,sizeof(*this)); }

	friend bostream& operator<<(bostream& ar, const msgGameData& obj )
	{
		ar << obj.nBaseScore;
		ar << obj.nTotalScore;

		return ar;
	}

	friend bistream& operator>>(bistream& ar, msgGameData& obj )
	{
		obj.reset();
		ar >> obj.nBaseScore;
		ar >> obj.nTotalScore;
		return ar;
	}
};

struct msgGameState
{
	enum {XY_ID = GLXY_GAMESTATE};

	BYTE  nGameState;
};


struct msgBankerSeat
{
	enum { XY_ID = GLXY_BANKERSEAT };

	int nSeat;
};

// 时钟
struct msgClock
{
	enum {XY_ID = GLXY_CLOCK};

	SHORT Who;		// 指向
	BYTE Time;		// 时间
};

struct msgCallAnte 
{
	enum { XY_ID = GLXY_CALLANTE };

	BYTE		theSeat;								//谁叫分
	int			theBaseAnte;							// 最小下注银子的除数
	int			theSBaseDouble7;						// 最小下注银子的8倍
	int			theSBaseDouble4;						// 最小下注银子的4倍
	int			theSBaseDouble2;						// 最小下注银子的2倍
	int			theSBaseDouble1;						// 最小下注银子的1倍
	bool		theCall;								//是否可以叫分
};

struct msgWrongOut
{
	enum {XY_ID = GLXY_WRONGOUT};

	int		Seat;
	int		OutCount;
	int     theErrorCount;             // 错误次数
};

struct msgADUrl
{
	enum { XY_ID = GLXY_ADURL};

	char theADUrl1[256];
};


#pragma pack()
#endif//__GAMELOGIC_PROTOCOL_H__

