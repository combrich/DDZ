#include "GameLogic.h"


IMPLEMENT_DYNAMICCREATE(CGameLogic)
CGameLogic::CGameLogic(void)
{
	m_nGetLandType = GLT_SCORE;
	m_nRoomType = RT_NORMAL;
	m_nGameType = GT_SCORE;
	CRandom::InitRandSeed();
	m_pLogicData = new LogicData(MAX_PLAYER);
	for (size_t i=0; i<MAX_PLAYER; ++i)
	{
		m_bIsRobot[i] = FALSE;
	}
	Reset();
}

CGameLogic::~CGameLogic(void)
{
	safe_delete(m_pLogicData);
}

//重设数据
void CGameLogic::Reset()
{	
	for (int i=0; i<MAX_PLAYER; ++i)
	{
		m_dvHandCards[i].clear();
		m_bOpenHand[i] = FALSE;
		m_bHavePowerGrab[i] = TRUE;
	}
	m_nLastCallSeat = MAX_PLAYER;
	m_nCallNum = 0;
	m_nCallScore	= 0;
	m_nRobotOutCardTimeMax = 15;
	srand(time(0));
	m_nRobotGrabTime = rand()%(m_nRobotTimeGrabMax+2);
	m_nRobotOutCardsTime = rand()%(m_nRobotOutCardTimeMax+1);
	m_nWaitlastOutTime = 2;
	m_nNowPlayTime = m_nTimePlay;
	m_nLaseOutCT		= CT_NONE;
	m_nBaseMultiple = 1;
	m_bFinishMission = false;
	m_nGameState = GAME_STATE_NONE;
	m_nTotalMultiple =  1;
	m_dvFirstOut.clear();
	m_bIsChunTian = FALSE;
	m_bIsFanChunTian = FALSE;
	m_bCanGiveUp = false;
	m_dvLastOutCards.clear();
	m_dvOutCards.clear();
	m_nGrabCount			= -1;
	m_nCallLandlord		= -1;
	m_nGrabSeat				= -1;
	m_nLandlord				= -1;
	m_bIsFirstOut				= true;
	m_nCountGiveUp=0;
	m_nBoomCount = 0;
	m_dvBaseCards.clear();
	m_nNowPlayer = MAX_PLAYER;
	m_nFirstPlayer = MAX_PLAYER;
	m_pLogicData->ResetTrustData();
}

//发送任务信息
void CGameLogic::SendMission()
{
	srand(time(0));
	msgMission	msgM;
	InitMsg(msgM);
	m_nMissionCardType = rand()%(msgMission::MCT_MAXBOOM+1);
	//m_nMissionCardType = 8;
	if (m_nMissionCardType >=9 || m_nMissionCardType <0)
	{
		m_nMissionCardType = 0;
	}

	msgM.nMisssionID = m_nMissionCardType;
	SendGamePacket_All(msgM);
}


int CGameLogic::GetBaseCardMultiple()
{
	int baseCardType;
	baseCardType = GetBaseCardsType(m_dvBaseCards);
	switch(baseCardType)
	{  
	case BC_FLUSH:			m_nBaseMultiple = 3; break;	
	case BC_STRAIGHT:		m_nBaseMultiple = 3; break;		
	case BC_BAOZI:			m_nBaseMultiple = 3; break;			
	case BC_STRAIGHTFLUSH:	m_nBaseMultiple = 5; break;	
	case BC_HAVEBIGJOKEER:  m_nBaseMultiple = 2; break;	
	default: m_nBaseMultiple = 1; break;
	}
	return m_nBaseMultiple;
}

int CGameLogic::onFrameWorkLogicCreate(ITable* table, const std::string& szRoomGameRule)
{
	//获得桌子的椅子数
	m_pTable = table;
	m_theMaxChairs = m_pTable->getChairs();	

	// 修正一下，如果游戏配置中的最大人数超过游戏内设最大人数的话
	if (m_theMaxChairs > (UINT)GetMaxPlayers())
		m_theMaxChairs = (UINT)GetMaxPlayers();

	//打印一下文件版本
	//m_pTable->debugString("FRAMEWORK_LOGIC_VERSION:%s,MaxChairs:%d",FRAMEWORK_LOGIC_VERSION,m_theMaxChairs);

	std::string strTemp;

	// 是否视频游戏
	strTemp = table->getLuaStrValue(szRoomGameRule,"IsVideoGame");
	if(strTemp != "")
	{	
		if(strTemp != "0")
		{
			m_cfIsVideoGame = TRUE;	
		}
		else 
		{
			m_cfIsVideoGame = FALSE;
		}	
	}
	else 
	{
		m_cfIsVideoGame = FALSE;
	}

	// 视频服务器IP
	m_cfVideoSvrIP = table->getLuaStrValue(szRoomGameRule,"VideoSvrIP");
	if(m_cfVideoSvrIP == "")
	{
		m_cfVideoSvrIP = "122.225.102.99";
	}

	// 视频服务器端口
	strTemp = table->getLuaStrValue(szRoomGameRule,"VideoSvrPort");
	if(strTemp != "")
	{
		m_cfVideoSvrPort = atoi(strTemp.c_str());
		if(m_cfVideoSvrPort <= 0)
		{
			m_cfVideoSvrPort = 8906;
		}	
	}
	else 
	{
		m_cfVideoSvrPort = 8906;
	}

	// 用假名
	strTemp = table->getLuaStrValue(szRoomGameRule,"FalseName");
	if(strTemp != "")
	{
		m_cfFalseName = atoi(strTemp.c_str());
		if(m_cfFalseName < 0)
		{
			m_cfFalseName = 0;
		}	
	}
	else 
	{
		m_cfFalseName = 0;
	}

	// 用假名
	strTemp = table->getLuaStrValue(szRoomGameRule,"CanChat");
	if(strTemp != "")
	{
		m_cfCanChat = atoi(strTemp.c_str());
		if(m_cfCanChat < 0)
		{
			m_cfCanChat = 1;
		}	
	}
	else 
	{
		m_cfCanChat = 1;
	}

	// 右边广告
	m_cfAdUrl = table->getLuaStrValue(szRoomGameRule,"ADURL");

	// 上方广告
	m_cfAdTop = table->getLuaStrValue(szRoomGameRule,"ADTOP");

	// 游戏区广告
	m_cfAdGame = table->getLuaStrValue(szRoomGameRule,"ADGAME");

	// 在游戏客户端但需大厅处理的按钮
	//m_cfBtnToRoom = table->getLuaStrValue(szRoomGameRule,"BtnToRoom");

	return 0;
}

int CGameLogic::onBaseLogicCreate(ITable* table, const std::string& szRoomGameRule)
{
	onFrameWorkLogicCreate(table,szRoomGameRule);

	/////////////////////////////////////////////////////////////////////////////////////////////
// 	static bool sbLog = false;
// 	if ( !sbLog )
// 	{
		m_pTable->debugString("========基础逻辑创建============");
		m_pTable->debugString("BaseLogicVersion: %s",BASE_LOGIC_VERSION );
// 		m_pTable->debugString("FrameworkProtocolVersion" );
 		m_pTable->debugString("================================");
// 		sbLog = true;
// 	}
	/////////////////////////////////////////////////////////////////////////////////////////////

	return 0;
}

int CGameLogic::onCreate(ITable* table, const std::string& szRoomGameRule)
{
	//onBaseLogicCreate( table, szRoomGameRule );
	CBaseLogic::onCreate( table, szRoomGameRule );
	//房间类型
	m_nRoomType = table->getLuaIntValue(szRoomGameRule,"nRoomType" );
	if (m_nRoomType !=RT_NORMAL && m_nRoomType != RT_MATCH)	m_nRoomType = RT_NORMAL;
	//财富类型
	m_nGameType = table->getLuaIntValue(szRoomGameRule,"nGameType");
	if (m_nGameType != GT_SCORE && m_nGameType != GT_RICH) m_nGameType = GT_SCORE;
	//抢地主方式
	m_nGetLandType = table->getLuaIntValue(szRoomGameRule,"nGrabLandType");
	if (m_nGetLandType != GLT_GRAB && m_nGetLandType != GLT_SCORE) m_nGetLandType = GLT_GRAB;
	//游戏底分
	m_nBaseScore = table->getLuaIntValue(szRoomGameRule,"nBaseScore");
	if (m_nBaseScore<=0 ) m_nBaseScore = 100;
	//是否禁用道具
	m_nEnableProp = table->getLuaIntValue(szRoomGameRule,"nEnableProp");
	if (m_nEnableProp!=1 && m_nEnableProp!=2) m_nEnableProp = 1;
	//赢经验
	m_nWinExp = table->getLuaIntValue(szRoomGameRule,"nWinExp");
	if (m_nWinExp<0) m_nWinExp =  0;
	//输经验
	m_nLosExp = table->getLuaIntValue(szRoomGameRule,"nLosExp");
	if (m_nLosExp<0) m_nLosExp = 0;
	//逃跑惩罚系数
	m_nRunAwayPunish = table->getLuaIntValue(szRoomGameRule,"nRunAwayPunish");
	if (m_nLosExp<=0)  m_nRunAwayPunish = 4;
	//是否开启任务系统
	m_nOn_OffMission = table->getLuaIntValue(szRoomGameRule,"nOnOffMission");
	if (m_nOn_OffMission!= ON_MISSION && m_nOn_OffMission != OFF_MISSION) 
			m_nOn_OffMission = ON_MISSION;
	//叫地主时间
	m_nTimeGrab = table->getLuaIntValue(szRoomGameRule,"nTimeGrab");
	if (m_nTimeGrab <=0) m_nTimeGrab = 15;
	//普通出牌时间
	m_nTimePlay = table->getLuaIntValue(szRoomGameRule,"nTimePlay");
	if (m_nTimePlay <=0) m_nTimePlay  = 20;
	//每局第一个出牌时间
	m_nTimeFirstPlay = table->getLuaIntValue(szRoomGameRule,"nTimeFirst");
	if (m_nTimeFirstPlay<=0) m_nTimeFirstPlay = 30;
	//托管出牌时间
	m_nTruOutTime = table->getLuaIntValue(szRoomGameRule,"nTruOutTime");
	if (m_nTruOutTime <=0) m_nTruOutTime = 2;
	//机器人抢地主最大时间
	m_nRobotTimeGrabMax  = table->getLuaIntValue(szRoomGameRule,"nRobotGrab");
	if (m_nRobotTimeGrabMax <=0) m_nRobotTimeGrabMax = 8;
	//机器人乱叫的概率
	m_nRobotBaseRand=table->getLuaIntValue(szRoomGameRule,"nRobotBaseRand");
	if (m_nRobotBaseRand<=0) m_nRobotBaseRand = 80;
	//是否开启眀牌
	m_bOpenCardEnable = table->getLuaIntValue(szRoomGameRule,"nEnableOC");
	if (m_bOpenCardEnable!=0 || m_bOpenCardEnable!=1) m_bOpenCardEnable = 1;
	return 0;
}


//判读是否完成任务
bool CGameLogic::IsFinishMission()
{
	CardTypeInfo cti = GameRule::GetCardType(m_dvOutCards);
	int nLastCT = msgMission::MCT_NONE;
	switch(cti.nType)
	{
	case CT_THREE:
		m_nLaseOutCT = msgMission::MCT_THREEE;
		break;

	case CT_THREEWITHONE:
		m_nLaseOutCT = msgMission::MCT_THREEW_OT;
		break;

	case CT_THREEWITHTWO:
		m_nLaseOutCT = msgMission::MCT_THREEW_OT;
		break;

	case CT_FOURWITHDTWO:
		m_nLaseOutCT = msgMission::MCT_FOUR_T;
		break;

	case CT_FOURWITHTWO:
		m_nLaseOutCT = msgMission::MCT_FOUR_T;
		break;

	case CT_MULTI:
		m_nLaseOutCT = msgMission::MCT_SINGLEMULTI;
		break;

	case CT_MULTIDOUBLETWO:
		m_nLaseOutCT = msgMission::MCT_DOUBLEMULTI;
		break;

	case CT_MULTITHREE:
		m_nLaseOutCT = msgMission::MCT_FEIJI;
		break;

	case CT_MULTITHREEWITHTWO:
		m_nLaseOutCT = msgMission::MCT_FEIJIDCB;
		break;

	case CT_MULTITHREEWITHONE:
		m_nLaseOutCT = msgMission::MCT_FEIJIDCB;
		break;

	case CT_BOOM:
		m_nLaseOutCT = msgMission::MCT_BOOM;
		break;

	case CT_MAXBOOM:
		m_nLaseOutCT = msgMission::MCT_MAXBOOM;
		break;

	default:
		m_nLaseOutCT = msgMission::MCT_NONE;
	}

	if (m_nLaseOutCT == m_nMissionCardType && m_nOn_OffMission == ON_MISSION)
	{
		m_bFinishMission = true;

		return true;
	}
	return false;
}

//剩下最后一张牌的时候，最后一张牌不等了
int CGameLogic::T_WatiLastOut(time_t nPassTime )
{
	msgOutCard msgOC;
	InitMsg(msgOC);
	msgOC.nSeat = m_nNowPlayer;
	msgOC.nType = msgOutCard::GIVEUP;
	msgOC.nHandCount = 1;
	this->OnMsgOutCard(m_pTable->getPlayer(m_nNowPlayer),(char*)&msgOC,sizeof(msgOC));
	return 0;
}

//打牌超时
int CGameLogic::T_OutTimePlayer()
{
	msgTrust msgT;
	InitMsg(msgT);
	msgT.nSeat = m_nNowPlayer;
	msgT.nType = msgTrust::T_BEGIN;
	this->OnMsgTrush(m_pTable->getPlayer(m_nNowPlayer),(char*)&msgT,sizeof(msgT));
	IntoTrust(m_nNowPlayer);
	return 0;
}

//抢地主超时
int CGameLogic::T_OutTImeGrab()
{
	msgGrabLandlord msgGL;
	InitMsg(msgGL);
	msgGL.thePlayerSeat = m_nNowPlayer;
	msgGL.thePlayerOperator = msgGrabLandlord::NOTCALL;
	if (m_nGetLandType == GLT_GRAB)
	{
		if (m_bIsCallLandlord)
		{
			msgGL.thePlayerOperator = msgGrabLandlord::NOTCALL;
		}else 
		{
			msgGL.thePlayerOperator = msgGrabLandlord::NOTGRAB;
		}
	}
	this->OnGrabLandlord(m_pTable->getPlayer(m_nNowPlayer),(char*)&msgGL,sizeof(msgGL));
	return 0;
}

/*
*托管：托管策略，如果玩家必须出牌，或者手上的牌能够一手全部打出，则出牌，否则不出
*/
void CGameLogic::IntoTrust(int nSeat)
{
	if (m_nGameState == GAME_STATE_PLAY)//打牌托管
	{
		GameRule::SortCards(m_dvHandCards[nSeat],false);
		msgOutCard msgOC;
		InitMsg(msgOC);
		if (GameRule::CanOut(!m_bCanGiveUp,m_dvLastOutCards,m_dvHandCards[m_nNowPlayer]))
		{
			//拿出打出的牌数据
			TCardVector TmpOC;
			TmpOC = m_dvHandCards[nSeat];
			msgOC.nSeat = nSeat;
			msgOC.nType = msgOutCard::OUTCARD;
			msgOC.nCount= ConvertDataVector(TmpOC,msgOC.cards,TmpOC.size());
			msgOC.nHandCount = m_dvHandCards[nSeat].size();
		}
		else 
		{
			if (!m_bCanGiveUp)
			{
				TCardVector TmpOC;
				msgOC.nSeat = nSeat;
				TmpOC.push_back(m_dvHandCards[nSeat].back());
				msgOC.nType = msgOutCard::OUTCARD;
				msgOC.nCount= ConvertDataVector(TmpOC,msgOC.cards,TmpOC.size());
				msgOC.nHandCount = m_dvHandCards[m_nNowPlayer].size();
			}else 
			{
				msgOC.nSeat = nSeat;
				msgOC.nType = msgOutCard::GIVEUP;	
			}
		}
		this->OnMsgOutCard(m_pTable->getPlayer(m_nNowPlayer),(char*)&msgOC,sizeof(msgOC));
	}
}

int CGameLogic::OnTimer( )
 {
	CBaseLogic::OnTimer();
	//处理超时
	time_t now;
	time(&now);
	// 逝去时间
	time_t nPassTime = now - m_StepTime;
	if (m_nGameState == GAME_STATE_GRAB)
	{
		if (nPassTime > m_nTimeGrab)
		{
			T_OutTImeGrab();
		}else if (m_bIsRobot[m_nNowPlayer] && nPassTime > m_nRobotGrabTime)
		{
			RobotGrabLandlord(m_nNowPlayer); 
		}
	}else if (m_nGameState == GAME_STATE_PLAY)
	{
		if (m_nNowPlayer != m_nLastOutCardSeat && m_dvHandCards[m_nNowPlayer].size() == 1 && m_dvLastOutCards.size() >1
			&&nPassTime >m_nWaitlastOutTime && !m_bIsRobot[m_nNowPlayer] && !m_pLogicData->IsTrust(m_nNowPlayer)) 
		{
			//最后一张牌不等
			T_WatiLastOut(nPassTime);
		}else if (m_pLogicData->IsTrust(m_nNowPlayer) &&nPassTime > m_nTruOutTime)
		{
			//托管出牌
			IntoTrust(m_nNowPlayer);
		}
		else if (nPassTime >=m_nNowPlayTime)
		{
			//出牌超时
			T_OutTimePlayer();
		}else if (m_bIsRobot[m_nNowPlayer] && nPassTime>m_nRobotOutCardsTime)
		{
			//机器人出牌
			this->OnMsgOutCard(m_pTable->getPlayer(m_nNowPlayer),(char*)&m_msgRobotOC,sizeof(m_msgRobotOC));
		}
	}else if (GAME_STATE_TAKE == m_nGameState)
	{
		if (nPassTime >=1)
		{
			GoToGameStep(GAME_STATE_GRAB);
		}
	}
	return 0;
}


 int CGameLogic::onUserWireBreak( IUserPtr player, int keepTime )
 {
	 return 1;
 }


int CGameLogic::OnGamePacket(IUserPtr player, WORD XYID, char* buffer, int length)
{
	//发给消息处理
	return OnGameLogicMsg(player,XYID,buffer,length);
}

void CGameLogic::OnMessageBox(IUserPtr player, char* buffer, int length)
{
	msgMessageBox *lpMsg = ParseMsg(msgMessageBox,buffer,length);
	if(NULL == lpMsg) return;

	if ( m_cfCanChat != 1 ) return ;

	CFrameworkLogic::OnMessageBox( player, buffer, length );
}

void CGameLogic::OnLookerExit(IUserPtr pUser)
{
	if(pUser.get() == NULL) return;
}

void CGameLogic::OnPlayerStart(IUserPtr pUser)
{
	if(pUser.get() == NULL)
	{
		m_pTable->debugString("ERROR:OnPlayerStart  玩家不存在");
		return;
	}
	if (m_nRoomType == RT_NORMAL)
	{
		CHAR szSendInfo[256];
		sprintf(szSendInfo, "%s 已准备",pUser->getNickName());
		SendHintInfo(NULL, szSendInfo,0xFF00FF00);
	}
}

// 进入游戏
void CGameLogic::OnEnterGame(IUserPtr player, bool IsSeen)
{
	if(player.get() == NULL) 
	{
		m_pTable->debugString("ERROR:OnEnterGame player参数为空");
		return ;
	}

	//告知进入玩家房间规则
	msgTableRule msgTR;
	ParseMsg(msgTableRule,&msgTR,sizeof(msgTableRule));
	msgTR.nRoomType = m_nRoomType;
	msgTR.nGameType = m_nGameType;
	msgTR.nGrabLandType = m_nGetLandType;
	msgTR.nBase = m_nBaseScore;
	SendGamePacket_OnePlayer(player,msgTR);

	GoToGameStep(GAME_STATE_NONE);
	BYTE nSeat = (BYTE)player->getSeat();
	if (nSeat >= MAX_PLAYER)
	{
		m_pTable->debugString("ERROR:OnEnterGame 玩家位置不合法.其值为%d",nSeat);
		return ;
	}
	char szUserEnter[256]={0};
	if (IsSeen)
	{
		SendHintInfo(player,"欢迎进入皇后斗地主！您当前是旁观者",0xFFFFFF00);
		SendHintInfo(player,"为防止作弊，旁观玩家只可查看游戏玩家出牌，不可见任何一家手牌",0xFFFFFF00);
		SendHintInfo(player,"旁观玩家不可操作牌",0xFFFFFF00);
		sprintf(szUserEnter,"旁观者:%s 进来了！",player->getNickName());
		SendHintInfo(NULL,szUserEnter,0xFF00FF00);
	}else 
	{
		m_bIsRobot[player->getSeat()] = (1 ==player->getUserType()?true:false);
		char szGameHint[256];
		if (m_nRoomType == RT_NORMAL)
		{
			SendHintInfo(player,"欢迎进入皇后斗地主，祝您游戏愉快!",0xFFFFFF00);
			SendHintInfo(player,"本房间禁止聊天.",0xFFFFFF00);
			SendHintInfo(player,"游戏开始后禁止逃跑，强制关闭游戏将判断为自动认输.",0xFFFFFF00);
			sprintf(szUserEnter,"%s 进来了！",player->getNickName());
			SendHintInfo(NULL,szUserEnter,0xFF00FF00);
		}else  if (m_nRoomType == RT_MATCH)
		{
			SendHintInfo(player,"欢迎进入斗地主比赛,祝您游戏愉快!",0xFFFFFF00);
			SendHintInfo(player,"为防作弊，您将无法查看其它玩家的信息！",0xFFFFFF00);
		}
	}
}

//计算总倍数
void CGameLogic::CalcTotalMultiple()
{
	m_nTotalMultiple = 1;

	if (m_nGetLandType  == GLT_GRAB)
	{
		//抢地主的产生的倍数
		m_nGrabCount>0?m_nTotalMultiple = (m_nTotalMultiple<<m_nGrabCount):m_nTotalMultiple;
	}else if (m_nGetLandType == GLT_SCORE)
	{
		//m_nCallScore>0?m_nTotalMultiple = (m_nTotalMultiple<<m_nCallScore):m_nTotalMultiple;
		m_nTotalMultiple = m_nTotalMultiple*m_nCallScore;
	}
	//炸弹产生的倍数
	m_nBoomCount>0?m_nTotalMultiple =(m_nTotalMultiple<<m_nBoomCount):m_nTotalMultiple;
	//明牌产生的倍数
	for (size_t i=0; i<MAX_PLAYER; ++i)
	{
		if (m_bOpenHand[i])
		{
			m_nTotalMultiple *= 2;
		}
	}
	//春天产生的倍数
	if (m_bIsChunTian)
	{
		m_nTotalMultiple *= 2;
	}
	//反春天产生的倍数
	if (m_bIsFanChunTian)
	{
		m_nTotalMultiple *= 2;
	}
	//玩家完成任务产生的倍数
	if (m_bFinishMission)
	{
		m_nTotalMultiple *= 2;
	}
	//底牌产生的倍数
	m_nTotalMultiple *= m_nBaseMultiple;

}
void CGameLogic::OnStartGame()
{
	//////////////////////////////////////////////////////////////////////////
	InitLogicData();
	SendStartGame();
	// 初始化玩家手牌
	m_pLogicData->GetDataPC()->InitPlayerCard();
	Reset();
	//随机确定一个玩家来叫地主
	srand((unsigned int)time(NULL));
	m_nNowPlayer = rand()%MAX_PLAYER;
	m_nFirstPlayer = m_nNowPlayer;
	m_bIsCallLandlord	= true;
	//发牌
	GoToGameStep(GAME_STATE_TAKE);
	TakeCards();

	//发送任务
	if (m_nOn_OffMission == OFF_MISSION)	//任务系统不开启
	{
	}else 
	{
		SendMission();
	}
}

void CGameLogic::OnEndGame()
{
}

SHORT CGameLogic::GetMaxPlayers()
{
	return MAX_PLAYER;
}
SHORT  CGameLogic::GetMinPlayers()
{
	return MIN_PLAYER;
}

ulong4 CGameLogic::GetTalkColor(BYTE nTalkType /*= ttNormal*/)
{
	if(nTalkType == ttSys)
	{
		return 0xFFFF0000;
	}
	return 0xFFFF0000;
}

void CGameLogic::GetPlayerShowItems(BYTE nItems[],BYTE nItemWidth[],BYTE& nItemCount)
{
	if ( m_cfFalseName == 1 )
	{
		for(int i = 0; i< g_nUserListShowItemCount; i++)
		{
			nItems[i] = msgPlayerItem::SI_NICKNAME;
			nItemWidth[i] = 200;
		}
		nItemCount = 1;
	}
	else
	{
		if ( m_pTable->getPlayType() == PT_PLAY_SCORE )
		{
			for(int i = 0; i< g_nUserListShowItemCount; i++)
			{
				nItems[i] = g_nUserListShowItems[i];
				nItemWidth[i] = g_nUserListShowItemWidth[i];
			}
		}
		else
		{
			for(int i = 0; i< g_nUserListShowItemCount; i++)
			{
				nItems[i] = g_nUserListShowItemsRich[i];
				nItemWidth[i] = g_nUserListShowItemWidth[i];
			}
		}

		nItemCount = g_nUserListShowItemCount;
	}
}

void CGameLogic::OnLookerWatchAllowTypeChange(IUserPtr pLooker,BOOL bIsWatchLooker)
{

}

void CGameLogic::OnPlayerWatchAllowTypeChange(IUserPtr pPlayer,BYTE nWatchAllowType)
{

}

int CGameLogic::GetWaitLastReadyTime()
{
	return WAIT_LAST_READY_TIME;
}

bool CGameLogic::CanPlayerLeave(ulong4 nNumberID)
{
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// 转接模块接口
CBaseLogic* CGameLogic::GetLpBaseLogic()
{
	return this;
}

SHORT CGameLogic::GetMaxPlayerCount()
{
	return MAX_PLAYER;
}
//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
// 结算相关
void CGameLogic::OnExitNormal(IUserPtr pUser)
{
	m_pTable->debugString("正常退出");
}

void CGameLogic::OnExitForc(IUserPtr pUser)
{
	if(pUser.get() == NULL) 
	{
		m_pTable->debugString("ERROR:在我处理逃跑消息的时候，传送进来的是一个NULL");
		return;
	}
	for (int i=0; i<MAX_PLAYER; ++i)
	{
		msgCards msgC;
		InitMsg(msgC);
		msgC.nType = msgCards::UPDATE;
		msgC.nSeat = i;
		msgC.nCount = ConvertDataVector(m_dvHandCards[i],msgC.cards,m_dvHandCards[i].size());
		SendGamePacket_All(msgC);
	}
	
	// 此时玩家离开，表示逃跑。
	// 游戏正在继续中，若逃跑的玩家为正在操作的玩家，则将这个玩家做放弃处理。
	short nSeat = pUser->getSeat();
	if (nSeat >= MAX_PLAYER) return;
	CalcTotalMultiple();
	if(m_nTotalMultiple == 0) m_nTotalMultiple = 1;
	long8 nWinLostScore = m_nTotalMultiple * m_nBaseScore * m_nRunAwayPunish;
	//如果玩家的钱不够扣，那么就扣完好了
	if (m_nGameType == GT_RICH && pUser->getGameRich() < nWinLostScore)
	{
		nWinLostScore = pUser->getGameRich();
	}
	TALLY myTallys[MAX_PLAYER];
	int nTallyCount = 0;
	for(int i=0; i<MAX_PLAYER; i++)
	{
		CLogicPlayer* pPlayer = GetLogicPlayer(i);
		if(pPlayer == NULL) 
		{
			char buf[1024] = {0};
			sprintf(buf,"ERROR_NP:在逃跑的记分中,%d位置的玩家没有正确的获取",i);
			m_pTable->debugString(buf);
			continue;
		}

		myTallys[nTallyCount]._numid = pPlayer->getNumberID();
		if (i == nSeat)
		{
			myTallys[nTallyCount]._result = TALLY::endEscape;
			if (m_nGameType == GT_SCORE)
				myTallys[nTallyCount]._score = -nWinLostScore;
			else if (m_nGameType == GT_RICH)
			{
				myTallys[nTallyCount]._rich = -nWinLostScore;
			}
		}
		else
		{
			myTallys[nTallyCount]._result = TALLY::endWin;
			if (m_nGameType == GT_SCORE)
				myTallys[nTallyCount]._score = 0;
			else if (m_nGameType == GT_RICH)
				myTallys[nTallyCount]._rich = nWinLostScore/2;
		}

		myTallys[nTallyCount]._charge = 0;

		nTallyCount++;
	}
	
	msgExitProcRes msgEPR;
	msgEPR.nExitScore = -nWinLostScore;
	msgEPR.nSeat  = pUser->getSeat();
	SendGamePacket_All(msgEPR);

	char szEsport[250];
	std::string strName = pUser->getNickName();
	 if (RT_MATCH == m_nRoomType)
	{
		strName = "游戏玩家";
	}
	sprintf(szEsport, "%s逃跑，本局结束",strName.c_str());
	SendHintInfo(NULL,szEsport,0xFFFFFF00);
	if (m_nGameType==GT_SCORE)
		sprintf(szEsport,"%s扣除%d分",strName.c_str(),nWinLostScore);
	else if (m_nGameType == GT_RICH)
		sprintf(szEsport,"%s扣除%d皇后币",strName.c_str(),nWinLostScore);
	SendHintInfo(NULL,szEsport,0xFFFFFF00);
	for (size_t i=0; i<MAX_PLAYER; ++i)
	{
		m_pTable->debugString("逃跑玩家%s ,分数数值%d",strName.c_str(),myTallys[i]._rich);
	}
	EndGame(myTallys,nTallyCount);


	
	Reset();
	m_nGameState = GAME_STATE_END;
	SendGameState();
}

void CGameLogic::GotoNextPlayer(int nNowPlayer)
{
	int nNextSeat = (nNowPlayer + 1) % MAX_PLAYER;
	if(m_nGameState  != US_SEEING)
	{
		m_nNowPlayer = nNextSeat;
		m_bCanGiveUp = m_nCountGiveUp>=2?false:true;
		m_bIsFirstOut    = !m_bCanGiveUp;
		if (!m_bCanGiveUp)
		{
			m_dvLastOutCards.clear();
		}
		SendPower(msgPower::PowerPlay, m_nNowPlayer);
		SendClock(msgPower::PowerPlay,m_nNowPlayer);
		time(&m_StepTime);
		srand(time(0));
		m_nRobotOutCardsTime = rand()%(m_nRobotOutCardTimeMax+1);
		if (m_pTable->getPlayer(m_nNowPlayer)->getUserType() == 1)
		{
			RobotPlayCard();
			//SetRobotOutCardsTime();
		}
	}
}

bool CGameLogic::OnWinLostResult()
{
	//计算输赢数据
	int nWinLost[MAX_PLAYER] = {0};
	int bIsLandlordWin  = -1;
	if (m_nNowPlayer == m_nLandlord)
	{
		bIsLandlordWin = 1;
	}
	bool bPlayerRichLess = false; //玩家财富不够
	for (int i=0; i<MAX_PLAYER; i++)
	{
		IUserPtr lpUser = m_pTable->getPlayer(i);
		if(lpUser == NULL) 
		{
			char buf[1024] = {0};
			sprintf(buf,"ERROR_NP:在正常的记分中,%d位置的玩家没有正确的获取!!!",i);
			m_pTable->debugString(buf);
			continue;
		}
		//财富版游戏为了防止玩家财富不够扣，那么做如下异常处理
		//如果输赢数值已经大于玩家游戏财富，那么仅仅扣除玩家游戏财富
		//同时也只加这部分财富
		int nRich = lpUser->getGameRich();
		if (i == m_nLandlord)   //地主
		{
			nWinLost[i] = m_nBaseScore*m_nTotalMultiple * bIsLandlordWin * 2;
		}else				   //非地主
		{
			nWinLost[i] = -1 * m_nBaseScore*m_nTotalMultiple * bIsLandlordWin ;
		}
		if (GT_RICH == m_nGameType && nWinLost[i]<0 && nRich< abs(nWinLost[i]))
		{
			bPlayerRichLess = true;
			nWinLost[i] = -nRich;
		}
	}
	//对记分再次验证
	if (bPlayerRichLess)
	{
		if (1==bIsLandlordWin) //地主赢
		{
			SendHintInfo(m_pTable->getPlayer(m_nLandlord),"有玩家总财富不够，您所赢财富将相应减少!!", 0XFF00FF00);
			nWinLost[m_nLandlord] = 0;
			for (size_t i=0; i<MAX_PLAYER; ++i)
			{
				if (i != m_nLandlord) nWinLost[m_nLandlord] += abs(nWinLost[i]);
			}
		}else //地主输，分不够扣
		{
			for (size_t i=0; i<MAX_PLAYER; ++i)
			{	
				if (i != m_nLandlord) 
				{
					SendHintInfo(m_pTable->getPlayer(i)," 地主财富不够，您所赢财富将相应减少!!", 0XFF00FF00);
					nWinLost[i] = abs(nWinLost[m_nLandlord])/2;
				}
			}
		}
	}
	if (m_nEnableProp == 1 && m_nGameType ==GT_SCORE)
	{
		//道具效果计算
		for (int i=0; i<MAX_PLAYER; i++)
		{
			CLogicPlayer* player = GetLogicPlayer(i);
			if(player == NULL) continue;

			std::vector<PropShowTimeData> PSTD = player->getPropShowData();
			time_t residueTimer;
			for (int j=0; j<PSTD.size(); j++)
			{
				std::string strInfo = player->getNickName(); 
				residueTimer = PSTD.at(j).endtime - time(0);
				int nHour	= residueTimer/ 3600;
				int nMin	=(residueTimer % 3600)/60;
				int nSec	=((residueTimer % 3600)%60);
				char buf[100]={};
				if (PSTD.at(j).propid == 35 && nWinLost[i] > 0) //玩家拥有双倍记分卡
				{
					strInfo += "双倍积分卡生效，所得积分翻倍!";
					sprintf(buf,"双倍积分卡将在%d时%d分%d秒后失效",nHour,nMin,nSec);
					strInfo += buf;
					SendHintInfo(NULL,strInfo.c_str(),0xFFFF7800);
					nWinLost[i] = nWinLost[i]*2;
				}else if (PSTD.at(j).propid == 36 && nWinLost[i] < 0)  //玩家拥有护身符
				{
					strInfo += "护身符生效，将不扣分!";
					sprintf(buf,"护身符将在%d时%d分%d秒后失效",nHour,nMin,nSec);
					strInfo += buf;
					SendHintInfo(NULL,strInfo.c_str(),0xFFFF7800);
					nWinLost[i] = 0;
				}
			}
		}
	}
	msgEndGame msgEG;
	InitMsg(msgEG);
	msgEG.nGameType = m_nGameType;
	for (int i=0 ;i<MAX_PLAYER; i++)
	{
		msgEG.nLandLord = m_nLandlord;
		if (bIsLandlordWin == 1)
		{
			if (m_bIsChunTian)
			{
				SendAction(m_nNowPlayer,MAX_PLAYER,msgAction::ACT_LANDCHUNTIAN);  //地主春天动画
			}else
			{
				SendAction(m_nNowPlayer,MAX_PLAYER,msgAction::ACT_LANDWIN);
			}
			msgEG.bLandLordWin = true;
		}else 
		{
			if (m_bIsFanChunTian)
			{
				SendAction(m_nNowPlayer,MAX_PLAYER,msgAction::ACT_NOLANDCHUNTIAN);
			}else 
			{
				SendAction(m_nNowPlayer,MAX_PLAYER,msgAction::ACT_LANDLOS);
			}
			msgEG.bLandLordWin = false;
		}
		msgEG.nWinLost[i] = nWinLost[i];
		if (nWinLost[i] > 0)
		{
			msgEG.nExp[i] = m_nWinExp;
		}else if (nWinLost[i] <=0)
		{
			msgEG.nExp[i] = m_nLosExp;
		}
	}
	msgEG.nTotalMultiple			= m_nTotalMultiple;
	msgEG.bIsNoOutCard		= m_bIsChunTian;
	msgEG.bFanChunTian		= m_bIsFanChunTian;
	msgEG.bIsOpenHands		= m_bOpenHand[m_nNowPlayer];
	msgEG.nBaseMultiple			= m_nBaseMultiple;
	msgEG.nCountBoom			= m_nBoomCount;
	msgEG.nGrabCount				= m_nGrabCount;
	msgEG.nCallScore				= m_nCallScore;
	msgEG.bFinishMission		= m_bFinishMission;
	
	if (msgEG.bFinishMission && m_nOn_OffMission == ON_MISSION)
	{
		SendAction(m_nNowPlayer,MAX_PLAYER,msgAction::ACT_FINISHMISSION);
		CalcTotalMultiple();
		SendAddMult();
	}
	// 计分
	TALLY myTallys[MAX_PLAYER];
	int nTallyCount = 0;

	for(int i=0; i<MAX_PLAYER; i++)
	{
		CLogicPlayer* pPlayer = GetLogicPlayer(i);
		if(pPlayer == NULL) 
		{
			char buf[1024] = {0};
			sprintf(buf,"ERROR_NP:在正常的记分中,%d位置的玩家没有正确的获取",i);
			m_pTable->debugString(buf);
			continue;
		}
		myTallys[nTallyCount]._numid = pPlayer->getNumberID();
		if (m_nGameType ==GT_SCORE)
		{
			myTallys[nTallyCount]._score  =  nWinLost[i];
		}
		else if (m_nGameType == GT_RICH)
		{
			myTallys[nTallyCount]._rich  =  nWinLost[i];
		}

		if (nWinLost[i] >0)
		{
			myTallys[nTallyCount]._result = TALLY::endWin;
			myTallys[nTallyCount]._exp	  = m_nWinExp;
		}
		else
		{
			myTallys[nTallyCount]._result  = TALLY::endLose;
			myTallys[nTallyCount]._exp	  = m_nLosExp;
		}

		myTallys[nTallyCount]._charge = 0;
		nTallyCount++;
	}
	EndGame(myTallys,nTallyCount);
	SendGamePacket_All(msgEG);
	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// 职能：获取底牌的牌型
// 返回：返回底牌的牌型
// 附注：函数是一个专有函数，只在斗地主使用，
//	     牌型有：
//			同花、顺子、豹子、底分×3
//			同花顺底分×5
//			王底分×2
// 排序算法--由小到大
bool sort_min_max(TCards card1, TCards card2)
{
	return (card1.GetValue() < card1.GetValue());
}

size_t CGameLogic::GetBaseCardsType(TCardVector& dvCards)
{
	if (dvCards.empty())
	{
		m_pTable->debugString("ERROR:获取底牌牌型时出错!传入的参数为空");
		return BC_ERROR;
	}
	GameRule::SortCards(dvCards);
	int m = dvCards.back().GetID();
	//有大王
	if (CID_BJ == dvCards.back().GetID())
	{
		return BC_HAVEBIGJOKEER;
	}
	
	//同花
	if (dvCards.front().GetColor() == dvCards.at(1).GetColor() && 
		dvCards.front().GetColor() == dvCards.back().GetColor())
	{
		//同花顺
		if (dvCards.front().GetValue()+2 == dvCards.back().GetValue()&&
			dvCards.at(1).GetValue()+1 == dvCards.back().GetValue())
		{
			return BC_STRAIGHTFLUSH;  
		}else 
		{
			return BC_FLUSH;
		}
	}
	//顺子
	if (dvCards.front().GetValue()+2 == dvCards.back().GetValue()&&
		dvCards.at(1).GetValue()+1 == dvCards.back().GetValue())
	{
		return BC_STRAIGHT;  
	}
	//豹子
	if (dvCards.front().GetValue() == dvCards.back().GetValue())
	{
		return BC_BAOZI;
	}
	return BC_NOCARDTYPE;
}

bool CGameLogic::JudgeCardsIsSole(const TCardVector& dvCards)
{
	//bool bIsSole = true;
	for (int i=0; i<dvCards.size(); ++i)
	{
		for (int j=i+1; j<dvCards.size();++j)
		{
			if (dvCards.at(i).GetID() == dvCards.at(j).GetID())
			{
				return false;
			}
		}
	}
	return true;
}

//出牌牌组中含有相同牌的时候的异常处理
int CGameLogic::ErrorAtCandsNotSole(IUserPtr player,msgOutCard* msgOC)
{
	std::string strErr = "ERROR_NS_B:出牌不唯一";
	m_pTable->debugString(strErr.c_str());
	strErr = "出牌数据:";
	for (int i=0; i<m_dvOutCards.size(); ++i)
	{
		strErr += m_dvOutCards.at(i).GetName();
	}
	m_pTable->debugString(strErr.c_str());
	if (player->getUserType() == 1) m_pTable->debugString("出牌玩家是机器人");
	else m_pTable->debugString("出牌玩家是正常玩家");
	strErr.clear();
	for (int i=0; i<MAX_PLAYER;++i)
	{
		IUserPtr playerTest = m_pTable->getPlayer(i);
		if (playerTest == NULL) strErr += "未获取到玩家   ";
		strErr+= playerTest->getNickName();
		strErr+= "\n";
		strErr+="当前手牌：\n";
		GameRule::SortCards(m_dvHandCards[i]);
		for (int j=0; j<m_dvHandCards[i].size(); ++j)
		{
			strErr+=m_dvHandCards[i].at(j).GetName();
		}
		strErr+="\n";
		strErr+="初始手牌:\n";
		TCardVector dvInit = m_pLogicData->GetDataPC()->GetHandCards(i);
		GameRule::SortCards(dvInit);
		for (int j=0; j<dvInit.size(); ++j)
		{
			strErr+=dvInit.at(j).GetName();
		}
		strErr+="\n";
	}
	m_pTable->debugString(strErr.c_str());
	strErr = "地主是:  ";
	strErr+= m_pTable->getPlayer(m_nLandlord)->getNickName();
	m_pTable->debugString(strErr.c_str());
	strErr = "拥有出牌权的是:  ";
	strErr+= m_pTable->getPlayer(m_nNowPlayer)->getNickName();
	m_pTable->debugString(strErr.c_str());
	strErr ="上一手出牌：\n";
	if (m_dvLastOutCards.empty()) strErr += "为空";
	else 
	{
		GameRule::SortCards(m_dvLastOutCards);
		for (int n=0; n<m_dvLastOutCards.size(); ++n)
		{
			strErr+=m_dvLastOutCards.at(n).GetName();
		}
	}
	m_pTable->debugString(strErr.c_str());
	strErr = "ERROR_NS_E";
	m_pTable->debugString(strErr.c_str());
	if (!m_bCanGiveUp)
	{
		msgOC->nType = msgOutCard::OUTCARD;
		msgOC->nCount = ConvertDataVector(m_dvHandCards[m_nNowPlayer],msgOC->cards,1);
		msgOC->nHandCount =  m_dvHandCards[m_nNowPlayer].size();
	}else 
	{
		msgOC->nType = msgOutCard::GIVEUP;
	}
	if (msgOC->nCount ==0)    //没有取到数据
	{
		return -1;
	}
	this->OnMsgOutCard(player,(char*)msgOC,sizeof(msgOutCard));
	SendHintInfo(player,"系统反外挂程序检查到您使用非法软件，自动跳过您的权限，如果判断错误，请截图联系客服！为您带来不便致以诚挚的歉意！",0xFF00FF00);
	return 0;
}

void CGameLogic::GotoEnd()
{
	//把玩家手牌显示一遍
	for (int i=0; i<MAX_PLAYER; ++i)
	{
		msgCards msgC;
		InitMsg(msgC);
		msgC.nType = msgCards::UPDATE;
		msgC.nSeat = i;
		msgC.nCount = ConvertDataVector(m_dvHandCards[i],msgC.cards,m_dvHandCards[i].size());
		SendGamePacket_All(msgC);
	}
	// 结算
	// 是否打出春天或者反春天
	if (m_nNowPlayer == m_nLandlord)
	{
		m_bIsChunTian = true;
		for (int i=0; i<MAX_PLAYER; i++)
		{
			if (i != m_nLandlord && m_dvHandCards[i].size() != 17)
			{
				m_bIsChunTian = false;
				break;
			}
		}
		if (m_bIsChunTian)
		{
		}
	}else  if (m_dvHandCards[m_nLandlord].size() == 
		(MAX_HANDCARDS-m_dvFirstOut.size()))
	{
		m_bIsFanChunTian = true;
	}
	if (m_nOn_OffMission == ON_MISSION)
	{
		this->IsFinishMission();
	}
	CalcTotalMultiple();
	SendAddMult();
	OnWinLostResult();
}


bool CGameLogic::RobotGrabLandlord(int nWhoIsAI)
{
	AICallIn aciNowCall;
	AICallOut acoOutCall;
	if (GLT_GRAB == m_nGetLandType)
		aciNowCall.callscore = m_nGrabCount;
	else if (GLT_SCORE == m_nGetLandType)
		aciNowCall.callscore = m_nCallScore;
	int nCardValue = CV_3;
	for (int i= CRobot::CARD_3; i<=CRobot::CARD_BJ; i++)
	{
		if (i != CRobot::CARD_2)
		{
			aciNowCall.val[i] = GetCardNum(nCardValue,m_dvHandCards[m_nNowPlayer]);
		}else 
		{
			aciNowCall.val[i] = GetCardNum(CV_2,m_dvHandCards[m_nNowPlayer]);
		}
		nCardValue++;
	}

	m_pRobot = new CRobot(m_dvHandCards);
	if (NULL == m_pRobot) return false;

	msgGrabLandlord msgGL;
	if (GLT_GRAB == m_nGetLandType)
	{
		if (m_bHavePowerGrab[m_nNowPlayer])
		{
			m_pRobot->RobotCall(aciNowCall,acoOutCall,m_nRobotBaseRand);
			msgGL.thePlayerSeat = m_nNowPlayer;
			if (acoOutCall.returnscore == aciNowCall.callscore
				&& acoOutCall.returnscore == -1)  //不叫
			{
				msgGL.thePlayerOperator = msgGrabLandlord::NOTCALL;
			}else if (acoOutCall.returnscore > aciNowCall.callscore
				&&	acoOutCall.returnscore == 0) //叫地主
			{
				msgGL.thePlayerOperator = msgGrabLandlord::CALL;
			}else if (acoOutCall.returnscore > aciNowCall.callscore 
				&& acoOutCall.returnscore > 0)  //抢地主
			{
				msgGL.thePlayerOperator = msgGrabLandlord::GRAB;
			}else if (acoOutCall.returnscore == aciNowCall.callscore
				&& aciNowCall.callscore >= 0) //不抢
			{
				msgGL.thePlayerOperator = msgGrabLandlord::NOTGRAB;
			}
		}else 
		{
			msgGL.thePlayerOperator = msgGrabLandlord::NOTGRAB;
		}
	}
	else if (GLT_SCORE == m_nGetLandType)
	{
		msgGL.thePlayerSeat = m_nNowPlayer;
		msgGL.nScore = m_pRobot->RobotCallEx(aciNowCall,acoOutCall,m_nRobotBaseRand);
		if(msgGL.nScore == 0)
		{
			msgGL.thePlayerOperator = msgGrabLandlord::NOTCALL;
		}else
		{
			msgGL.thePlayerOperator = msgGrabLandlord::CALLSCORE;
		}
	}
	IUserPtr player = m_pTable->getPlayer(nWhoIsAI);
	if (!player)  return false;
	OnGrabLandlord(player,(char*)&msgGL,sizeof(msgGL));
	delete m_pRobot;
	return true;
}


int CGameLogic::GetCardNum(int nCardVal, TCardVector dvSearch)
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

bool CGameLogic::RobotPlayCard()
{
	CRobot Robot(m_nLandlord);
	//msgOutCard msgOC;
	InitMsg(m_msgRobotOC);
	m_msgRobotOC.nSeat = m_nNowPlayer;
	m_msgRobotOC.nHandCount = m_dvHandCards[m_nNowPlayer].size();

	TCardVector tmp;
	CardTypeInfo cti;
	if (!m_bCanGiveUp)
	{
		m_msgRobotOC.nType = msgOutCard::OUTCARD;
		tmp = Robot.RobotPlayCards(m_dvHandCards,m_nNowPlayer);
		m_nRobotOutCardTimeMax = Robot.GetSuggestTime();
		cti = GameRule::GetCardType(tmp);
		if (cti.nType > CT_NONE && cti.nType < CT_COUNT)
		{
			m_msgRobotOC.nCount= ConvertDataVector(tmp,m_msgRobotOC.cards,tmp.size());
		}else 
		{
			m_msgRobotOC.nCount = ConvertDataVector(m_dvHandCards[m_nNowPlayer],m_msgRobotOC.cards,1);
		}
	}else 
	{
		if (Robot.RobotFollowCards(m_dvHandCards,m_dvLastOutCards,m_nLastOutCardSeat,tmp,m_nNowPlayer))
		{
			cti = GameRule::GetCardType(tmp);
			if (cti.nType > CT_NONE && cti.nType < CT_COUNT)
			{
				m_msgRobotOC.nType = msgOutCard::OUTCARD;
				m_msgRobotOC.nCount= ConvertDataVector(tmp,m_msgRobotOC.cards,tmp.size());
			}else 
			{
				m_msgRobotOC.nType = msgOutCard::GIVEUP;
			}
		}else 
		{
			m_msgRobotOC.nType = msgOutCard::GIVEUP;
		}
		m_nRobotOutCardTimeMax = Robot.GetSuggestTime();
	}
	return true;
}
