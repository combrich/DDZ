#ifndef __GAMELOGIC_H__
#define __GAMELOGIC_H__

#include <vector>
#include <list>
#include "BaseLogic.h"
#include "GLProtocol.h"
#include "LogicData.h"
#include "Robot.h"
///////////////////////////////////////////////////////////////////////////////////////
// 游戏逻辑协议映射
#define BEGIN_MSG_MAP() \
	int OnGameLogicMsg(IUserPtr player,WORD XYID,const char* pMsg,int nLen) \
	{ \
	switch(XYID) \
		{

#define ON_MSG_MAP(xyid,onfun) \
	case xyid: return onfun(player,pMsg,nLen); break;

#define END_MSG_MAP() \
			default: break; \
		} \
		return 0; \
	};

//////////////////////////////////////////////////////////////////////////////////////////
// 客户端玩家列表要显示的项及宽度
const BYTE g_nUserListShowItemCount = 6;
const BYTE g_nUserListShowItems[] = {
		msgPlayerItem::SI_STATE,
		//msgPlayerItem::SI_SID,
		msgPlayerItem::SI_NICKNAME,
		msgPlayerItem::SI_GAMESCORE,
		msgPlayerItem::SI_SCORETITLE,
		msgPlayerItem::SI_ROUNDCOUNT,	
		msgPlayerItem::SI_WIN_PRO,			
		//msgPlayerItem::SI_LOSE,			
		//msgPlayerItem::SI_DRAW,
		//msgPlayerItem::SI_ESCAPE
};

const BYTE g_nUserListShowItemsRich[] = {
		msgPlayerItem::SI_STATE,
		//msgPlayerItem::SI_SID,
		msgPlayerItem::SI_NICKNAME,
		msgPlayerItem::SI_GAMEMONEY,
		msgPlayerItem::SI_MONEYTITLE,
		msgPlayerItem::SI_ROUNDCOUNT,	
		msgPlayerItem::SI_WIN_PRO,			
		//msgPlayerItem::SI_LOSE,			
		//msgPlayerItem::SI_DRAW,
		//msgPlayerItem::SI_ESCAPE
};

const LONG g_nUserListShowItemWidth[] = {
		40,
		65,
		65,
		65,
		65,
		65,
		//60,
		//60
};
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

DECLARE_DYNAMICCREATE
class CGameLogic : public CBaseLogic
{
	//////////////////////////////////////////////////////////////////////////
	// 构造

public:
	CGameLogic(void);
	virtual ~CGameLogic(void);
	void Reset();		// 清空所有状态与数据,此函数请谨慎调用。

	//////////////////////////////////////////////////////////////////////////
	// 基类接口
public:
	// 游戏逻辑对象创建时调用
	virtual int onCreate(ITable* table, const std::string& szRoomGameRule);		
	int onFrameWorkLogicCreate(ITable* table, const std::string& szRoomGameRule);
	int onBaseLogicCreate(ITable* table, const std::string& szRoomGameRule);

	// 定时器
	virtual int OnTimer( );	

	//////////////////////////////////////////////////////////////////////////
	// 游戏接口
public:

	// 接收到MESSAGE数据报
	virtual void OnMessageBox(IUserPtr player, char* buffer, int length);	

	// 进入游戏, 处理重连,和初始化, IsSeen 是不是旁观 
	virtual void OnEnterGame(IUserPtr player, bool IsSeen);		

	// 某玩家开始
	virtual void OnPlayerStart(IUserPtr pUser);

	// 游戏开始
	virtual	void OnStartGame();												

	// 旁观者退出
	virtual void OnLookerExit(IUserPtr pUser);

	// 玩家普通状态下的离开
	virtual void OnExitNormal(IUserPtr player);	

	// 强制退出
	virtual void OnExitForc(IUserPtr player);	

	// 结算
	virtual bool OnWinLostResult();

	// 游戏结束
	virtual void OnEndGame();		

	// 获取最大玩家数量
	virtual SHORT  GetMaxPlayers();

	// 获取最小开始数量
	virtual SHORT  GetMinPlayers();

	// 获取指定聊天类型的聊天颜色
	virtual ulong4	GetTalkColor(BYTE nTalkType = ttNormal);	

	// 获取客户端玩家列表要显示的项
	virtual void GetPlayerShowItems(BYTE nItems[],BYTE nItemWidth[],BYTE& nItemCount);

	// 旁观者允许看牌类型改变
	virtual void OnLookerWatchAllowTypeChange(IUserPtr pLooker,BOOL bIsWatchLooker);

	// 玩家允许看牌类型改变
	virtual void OnPlayerWatchAllowTypeChange(IUserPtr pPlayer,BYTE nWatchAllowType);

	// 获取等待最后一个玩家开始的时间
	virtual int GetWaitLastReadyTime();

	// 玩家能否正常退出
	virtual bool CanPlayerLeave(ulong4 nNumberID);

	// 游戏逻辑协议分配
	virtual	int OnGamePacket(IUserPtr player, WORD XYID, char* buffer, int length);

	// 断线
	virtual int onUserWireBreak( IUserPtr player, int keepTime );
public:

	BEGIN_MSG_MAP()
		ON_MSG_MAP(GLXY_MAKECARD,OnMsgMakeCard)
		ON_MSG_MAP(GLXY_GRABLANDLORD,OnGrabLandlord)
		ON_MSG_MAP(GLXY_OPENCARD,OnMsgOpenCard)
		ON_MSG_MAP(GLXY_OUTCARD,OnMsgOutCard)
		ON_MSG_MAP(GLXY_QIAOPIHUA,OnMsgQiaoPiHua)
		ON_MSG_MAP(GLXY_TRUST,OnMsgTrush)
	END_MSG_MAP()

	// 游戏逻辑协议处理，请将以下成员函数的定义写在 GameMsg.cpp 内。
	int OnMsgMakeCard(IUserPtr player,const char* pMsg,int nLen);
	int OnMsgTrush(IUserPtr player,const char* pMsg,int nLen);
	int OnMsgQiaoPiHua(IUserPtr player,const char* pMsg,int nLen);
	int OnGrabLandlord(IUserPtr player,const char* pMsg,int nLen);
	int OnMsgOpenCard(IUserPtr player,const char* pMsg,int nLen);
	int OnMsgOutCard(IUserPtr player,const char* pMsg,int nLen);

	//////////////////////////////////////////////////////////////////////////////////////////////
	
private:
	void TakeCards();												//发牌
	void ReTakeCards();										//重新发牌
	void AppendBase();											//添加底牌
	void SendStartGame(IUserPtr player = NULL);
	void SendLogicConfig(IUserPtr player = NULL);
	void SendLogicData(IUserPtr player = NULL);
	void SendAction(SHORT nFromSeat,SHORT nToSeat,BYTE action = msgAction::ACT_ANIFINISH);	 //发送动作
	void SendMsgOnGamingEnter(IUserPtr player);
	void SendPowerOnGamingEnter(IUserPtr player);

	void SendGameState(IUserPtr player = NULL);
	void SendGameData(IUserPtr player = NULL);
	void SendIdentity(int nSeat); //玩家身份
	void SendCards(msgCards& msgC);			
	void SendOutCard(msgOutCard msgOC);						//出牌	
	void SendPower(int nPower, int nWho,bool bOpenCard=false);
	void SendClock(int nPower, int nPowerSeat, IUserPtr pUser = NULL);
	void GotoEnd();
	void SendAddMult();

private:
	void InitLogicData();
	void GoToGameStep(BYTE nGameStep);
	void GotoNextPlayer(int nNowPlayer);
	void GotoNextGrabPlayer(int nNowPlayer);
private:
	size_t GetBaseCardsType(TCardVector& dvCards);
	int	 GetBaseCardMultiple();								//获取底牌倍数
	void CalcTotalMultiple();								//计算总倍速
private:
	LogicData*  m_pLogicData;

public:
	// 转接模块接口
	virtual CBaseLogic* GetLpBaseLogic();
	virtual SHORT GetMaxPlayerCount();
	//////////////////////////////////////////////////////////////////////////////////////////////
protected:
	// 脚本配置
	size_t		m_bOpenCardEnable;						//是否开启眀牌			
	time_t		m_StepTime;											// 记时时刻
	size_t		m_nRoomType;										//	房间类型
	size_t		m_nGameType;										// 游戏类型，财富，积分
	size_t		m_nGetLandType;								// 确定地主的方式
	size_t		m_nBaseScore;										// 游戏底分
	size_t		m_nEnableProp;									// 是否禁用道具
	size_t		m_nWinExp;												// 赢经验
	size_t		m_nLosExp;												// 输经验
	enum {ON_MISSION=1,OFF_MISSION};
	size_t		m_nOn_OffMission;						//是否开启任务
	size_t		m_nRunAwayPunish;					//逃跑惩罚系数
	size_t		m_nTimeGrab;								//抢地主时间
	size_t		m_nRobotTimeGrabMax;			//机器人抢地主时间
	size_t		m_nTimePlay;									//打牌时间
	size_t		m_nTimeFirstPlay;						//一局第一个出牌时间
	size_t		m_nTruOutTime;							//托管出牌时间 
	size_t		m_nRobotBaseRand;					//机器人叫地主概率

private:
	size_t		m_nRobotGrabTime;							//机器人抢地主时间
	SHORT	m_nNowPlayer;										//当前有权限的玩家位置
	SHORT	m_nLastOutCardSeat;						//上一个出牌玩家的位置
	SHORT	m_nFirstPlayer;										//没局游戏开局第一个拥有权限的玩家位置
	size_t		m_nGameState;										//当前游戏状态
	bool			m_bOpenHand[MAX_PLAYER];	//玩家是否是眀牌
	bool			m_bIsFirstOut;										//是否首轮出牌
	bool			m_bIsChunTian;										//是否是春天
	bool			m_bIsFanChunTian;							//是否是反春天
	size_t		m_nBoomCount;									//炸弹个数
	size_t		m_nNowPlayTime;								//当前出牌时间
private:
	TCardVector m_dvHandCards[MAX_PLAYER];				// 所有玩家手牌
	TCardVector m_dvBaseCards;													// 底牌
	TCardVector m_dvFirstOut;													//一把中第一手牌
	bool					 m_bCanGiveUp;											// 可否放弃出牌
	bool					 m_bIsCallLandlord;										// 是否是叫地主
	bool					 m_bHavePowerGrab[MAX_PLAYER];	//是否有抢地主权限
	size_t				 m_nCountGiveUp	;			//放弃出牌的玩家个数		
	int						 m_nGrabCount;						//玩家抢了多少次地主
	int						m_nCallLandlord;					// 叫地主玩家位置
	int						m_nGrabSeat;						// 抢地主玩家位置
	int						m_nLandlord;						//地主位置
	int						m_nTotalMultiple;				//总倍数
	int						m_nCallScore;					//叫了多少分
	int						m_nCallNum;						//多少个人叫了地主
	int						m_nLastCallSeat;				//最后叫分的玩家位置
	size_t				m_nWaitlastOutTime;				//手中只剩下一张牌的时候，上家出单张，多少秒后自动跳过
	TCardVector m_dvOutCards;								// 当前玩家的出牌
	TCardVector m_dvLastOutCards;					// 上一手玩家的出牌
	int						m_nBaseMultiple;							//底牌倍数
//////////////////////////////////////////////////////////////////////////
//任务系统
	bool IsFinishMission();								//判断是否完成任务
	void SendMission();										//发送任务
	bool		m_bFinishMission;						//是否完成任务
	int			m_nMissionCardType;						//任务牌型
	int			m_nLaseOutCT;							//最后出牌牌型
	
	
private:
	//判断牌组中所有牌是否都是唯一,如果是，返回true，否则返回false
	bool JudgeCardsIsSole(const TCardVector& dvCards);
	int ErrorAtCandsNotSole(IUserPtr player,msgOutCard* msgOC);

private:
	//在OnTime中处理的信息
	int T_WatiLastOut(time_t nPassTime );
	int T_OutTimePlayer();
	int T_OutTImeGrab();
	void IntoTrust(int nSeat); 
private:
	//机器人
	bool RobotPlayCard();
	bool RobotGrabLandlord(int nWhoIsAI);
	int  GetCardNum(int nCardVal, TCardVector dvSearch);
	msgOutCard  m_msgRobotOC;
	size_t m_nRobotOutCardTimeMax;
	size_t m_nRobotOutCardsTime;
	CRobot*		m_pRobot;											//机器人
	bool			m_bIsRobot[MAX_PLAYER];				// 进入游戏的玩家是否是机器人
};	

#endif//__GAMELOGIC_H__

