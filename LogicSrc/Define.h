#ifndef __GAMELOGIC_DEFINE_H__
#define __GAMELOGIC_DEFINE_H__
#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <string>

using namespace std;

#include "BaseDefine.h"

//#define MYVERSION_GAMELOGIC_FR	1
//#define GAME_CLIENT			// 定义游戏客户端

#define GAME_VERSION_MAJOR	1
#define GAME_VERSION_MINOR	1

#define TIMERDELAY			2		//时钟隔多少秒调用一次
#define GAMINGDELAYTIME		31		//游戏中超时时间，即多少秒,  -1 for Client
#define NULLDELAYTIME		61		//非游戏中不按按钮超时时间，即多少秒  -1 for Client

//For Server
#define GAMINGDELAY			(GAMINGDELAYTIME/TIMERDELAY+(GAMINGDELAYTIME%TIMERDELAY?1:0))			
//For Server
#define NULLDELAY			(NULLDELAYTIME/TIMERDELAY+(NULLDELAYTIME%TIMERDELAY?1:0))				
#define DELAYTOAUTO			3		//多少次超时将被设为托管

#define CY_SAFE_DELETE(p)	{if(p){delete p;p=0;}}

#define GAMENAME		"斗地主"

// 调试状态
#ifndef _TESTRELEASE
#define GAMECLIENTDIR	"DDZ_d.exe"
#define ROOMGAMERULE	"BasePlayTypeScore='1';BasePlayTypeScoreProp='0';ServicePayProp='0';ADURL='http://www.baidu.com/';"
#define DYNAMICGAME		//定义这个表示游戏为动态游戏
#else
#define GAMECLIENTDIR	"DDZ.exe"
#define ROOMGAMERULE	"BasePlayTypeScore='1';BasePlayTypeScoreProp='0';ServicePayProp='0';ADURL='http://www.baidu.com/';"
#define DYNAMICGAME		//定义这个表示游戏为动态游戏
#endif

#define	GAMECLASSNAME		CGameLogic

// 客户端状态
typedef enum tagClientState
{
	csNull = 0,
	csRun,			// 正在游戏中
	csRelink,		// 正在重连中
	csLooker		// 正在观战中
}ClientState;

#define MAX_PLAYER			3		// 最大玩家数
#define MIN_PLAYER				3		// 最小玩家数


#define WAIT_LAST_READY_TIME	30

const size_t	MAX_TOTALCARDS	=54;	//总牌数
const size_t  MIN_HANDCARDS		=17;	//农民手牌数
const size_t	MAX_HANDCARDS		=20;	//地主手牌数
const size_t	BASE_CARDS				= 3;		//底牌牌数

enum 
{
	GT_SCORE=1,			//积分游戏
	GT_RICH						//财富游戏
};

enum 
{
	RT_NORMAL =1,		//普通房间
	RT_MATCH					//比赛房间
};		

typedef enum tagBaseCardType
{
	BC_NOCARDTYPE   =1,			//散牌
	BC_FLUSH,					//同花
	BC_STRAIGHT,				//顺子
	BC_BAOZI,					//豹子
	BC_STRAIGHTFLUSH,			//同花顺
	BC_HAVEBIGJOKEER,			//有大王
	BC_ERROR									//出现错误
} TBCARDTYPE;

enum
{
	GLT_GRAB =1,	//抢地主方式确定地主
	GLT_SCORE,		//叫分方式确定地主
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef unsigned long long USERID;

__inline USERID GetBrandID(USERID nUserID)
{
	return (nUserID >> 32);
}

__inline USERID GetNumberID(USERID nUserID)
{
	return (nUserID & 0xFFFFFFFF);
}

typedef enum tagGameState				// 游戏状态
{
	GAME_STATE_NONE = 0,
	GAME_STATE_TAKE,					//发牌
	GAME_STATE_GRAB,					//抢地主
	GAME_STATE_PLAY,					//打牌
	GAME_STATE_END,						//结算
	GAME_STATE_COUNT
} TGAMESTATE;

typedef enum tagGameStep				// 游戏步骤
{
// 	GAME_STEP_NONE = 0,	
// 	GAME_STEP_TAKE,
// 	GAME_STEP_GRAB,
// 	GAME_STEP_PLAY,
// 	GAME_STEP_END,
// 	GAME_STEP_COUNT
 } TGAMESTEP;

#endif//__GAMELOGIC_DEFINE_H__

