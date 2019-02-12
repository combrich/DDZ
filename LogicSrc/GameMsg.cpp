#include "GameLogic.h"
// 游戏协议接收处理成员函数定义


// 发送开始游戏
void CGameLogic::SendStartGame(IUserPtr player /*= NULL*/)
{
	msgStartGame myStartGame;
	InitMsg(myStartGame);

	myStartGame.theIsGameStart = TRUE;

	if(player == NULL)
	{
		SendGamePacket_All(myStartGame);
	}
	else if (player.get()!= NULL)
	{
		SendGamePacket_OnePlayer(player,myStartGame);
	}
}


//发牌
void CGameLogic::TakeCards()
{
	msgTake msgT;
	InitMsg(msgT);
	msgT.nType =msgTake::TAKE_BEGIN;
	SendGamePacket_All(msgT);
	m_dvBaseCards		 = m_pLogicData->GetDataPC()->GetBaseCards();
	//发底牌
	msgBase msgB;
	InitMsg(msgB);
	msgB.nType		 = msgBase::UPDATE;
 	msgB.nCount				=  BASE_CARDS;
	msgB.nActionType   = msgBase::TAKECARD;
	for (size_t i=0; i<BASE_CARDS; ++i)
	{
		msgB.cards[i] = CID_BACK;
	}
 	msgB.nBaseType		= BC_ERROR;
	SendGamePacket_All(msgB);
	//发玩家手牌
	BYTE  tmpHands[MAX_PLAYER][MIN_HANDCARDS]={0,0,0};
	for (size_t i=0; i<MAX_PLAYER; i++)
	{
		m_dvHandCards[i] = m_pLogicData->GetDataPC()->GetHandCards(i);
		ConvertDataVector(m_dvHandCards[i],tmpHands[i],MIN_HANDCARDS);
	}
	for (size_t i=0; i<MIN_HANDCARDS; ++i)
	{
		for (size_t j=0; j<MAX_PLAYER; ++j)
		{
			msgCards msgC;
			InitMsg(msgC);
			msgC.nType = msgCards::ADD;
			msgC.nSeat = j;
			msgC.nCount = 1;
			msgC.cards[0]=tmpHands[j][i];
			SendCards(msgC);
		}
	}
	//发送发牌结束
	msgT.nType = msgTake::TAKE_END;
	SendGamePacket_All(msgT);
}
//发送逻辑配置
void CGameLogic::SendLogicConfig(IUserPtr player /*= NULL*/)
{
}

// 发送逻辑数据
void CGameLogic::SendLogicData(IUserPtr player/* = NULL*/)
{
	msgLogicData msgLD;
	InitMsg(msgLD);

	msgLD.BasePlayTypeScore = m_nBaseScore;
	if(player.get() == NULL) //get函数用于判断player是否存在
	{
		SendGamePacket_All(msgLD);
	}
	else
	{
		SendGamePacket_OnePlayer(player,msgLD);
	}
}

// 发送游戏数据
void CGameLogic::SendGameData(IUserPtr player/* = NULL*/)
{
}

// 发送游戏进入
void CGameLogic::SendMsgOnGamingEnter(IUserPtr player)
{
	
}


// 发送游戏权限
void CGameLogic::SendPowerOnGamingEnter(IUserPtr player)
{
}

//发送游戏状态
void CGameLogic::SendGameState(IUserPtr player /*= NULL*/)
{
	msgGameState msgGS;
	InitMsg(msgGS);
	msgGS.nGameState = m_nGameState;
	if(player.get() == NULL)
	{
		SendGamePacket_All(msgGS);
	}
	else
	{
		SendGamePacket_OnePlayer(player,msgGS);
	}
}


//发送倍数
void CGameLogic::SendAddMult()
{
	msgAddMult msgAM;
	InitMsg(msgAM);
	msgAM.nMultiple  = m_nTotalMultiple;
	SendGamePacket_All(msgAM);
}


//发送动作信息
void CGameLogic::SendAction(SHORT nFromSeat,SHORT nToSeat,BYTE action /*=msgAction::ACT_ANIFINISH*/)
{
	msgAction msgA;
	InitMsg(msgA);
	msgA.theFromSeat  = nFromSeat;
	msgA.theToSeat    = nToSeat;
	CardTypeInfo ctiOut = GameRule::GetCardType(m_dvOutCards);
	if (action == msgAction::ACT_ANIFINISH)
	{
		switch(ctiOut.nType)
		{
		case CT_BOOM:
			msgA.theAction = msgAction::ACT_ZHADAN;
			break;

		case CT_MULTITHREEWITHTWO:
			msgA.theAction = msgAction::ACT_FEIJI;
			break;

		case CT_MULTITHREEWITHONE:
			msgA.theAction = msgAction::ACT_FEIJI;
			break;

		case CT_MAXBOOM:
			msgA.theAction = msgAction::ACT_HUOJIAN;
			break;

		case CT_THREEWITHTWO:
			msgA.theAction = msgAction::ACT_SANDAIER;
			break;

		case CT_THREEWITHONE:
			msgA.theAction = msgAction::ACT_SANDAIYI;
			break;

		case CT_MULTITHREE:
			msgA.theAction = msgAction::ACT_SANSHUN;
			break;

		case CT_MULTIDOUBLETWO:
			msgA.theAction = msgAction::ACT_SHUANGSHUN;
			break;

		case CT_MULTI:
			msgA.theAction = msgAction::ACT_SHUNZI;
			break;

		case CT_FOURWITHTWO:
			msgA.theAction = msgAction::ACT_SIDAIER;
			break;

		case CT_FOURWITHDTWO:
			msgA.theAction = msgAction::ACT_SIDAIER;
			break;
		default:
			break;
		}
	}else 
	{
		msgA.theAction    = action;
	}
	SendGamePacket_All(msgA);
}

//发送出牌信息
void CGameLogic::SendOutCard(msgOutCard msgOC )
{	
	int nSeat = msgOC.nSeat;
	msgOC.nHandCount = m_dvHandCards[m_nNowPlayer].size();
	IUserPtr pUser = m_pTable->getPlayer(nSeat);
	if(pUser.get() != NULL)
	{
		SendGamePacket_All(msgOC);
	}
	//如果玩家是出牌，那么发送信息提示客户端删除牌组
	if (!m_dvOutCards.empty() && msgOC.nType == msgOutCard::OUTCARD)
	{
		msgCards msgDC;
		InitMsg(msgDC);
		msgDC.nType = msgCards::UPDATE;
		msgDC.nSeat = msgOC.nSeat;
		msgDC.nCount = ConvertDataVector(m_dvHandCards[msgOC.nSeat],msgDC.cards,m_dvHandCards[msgOC.nSeat].size());
		SendCards(msgDC);
	}
}


//向客户端发送玩家身份信息
void CGameLogic::SendIdentity(int  nSeat)
{
	IUserPtr pUser = m_pTable->getPlayer(nSeat);
	if(pUser.get() != NULL)
	{
		msgPlayerIdentity msgPI;
		InitMsg(msgPI);
		msgPI.nSeat = nSeat;
		if (m_nLandlord >=0 && m_nLandlord < MAX_PLAYER) 
		{
			msgPI.bIsLandlord = true;
		}else 
		{
			msgPI.bIsLandlord = false;
		}
		SendGamePacket_All(msgPI);
	}
}

// 发牌
void CGameLogic::SendCards(msgCards& msgC)
{
	int nSeat = msgC.nSeat;
	if (nSeat<0 || nSeat >=MAX_PLAYER)
	{
		m_pTable->debugString("SendCards中nSeat出错nSeat=%d",nSeat);
		return;
	}
	IUserPtr pUser = m_pTable->getPlayer(nSeat);
	if (pUser==NULL)
	{
		m_pTable->debugString("SendCards中没有获取到%d位置上的玩家数据",nSeat);
		return;
	}
	//如果玩家是眀牌,那么就想所有玩家发送真实的牌数据，否则发送全是牌背的数据
	if (m_bOpenHand[msgC.nSeat])
	{
		SendGamePacket_All(msgC);
	}else
	{
		IUserPtr pUser = m_pTable->getPlayer(msgC.nSeat);
		if (pUser == NULL)
		{
			m_pTable->debugString("ERROR:在SendCards中,获取%d号位置上的玩家失败",msgC.nSeat);
		}
		//想自己发送正确的牌
		SendGamePacket_OnePlayer(pUser,msgC);
		//向其他玩家发送牌背
		for (size_t i=0; i<msgC.nCount; ++i)
		{
			msgC.cards[i] = CID_BACK;
		}
		SendGamePacket_NoSelf(pUser,msgC);
	}
	
}

// 发送权限
void CGameLogic::SendPower(int nPower, int nWho, bool  bOpenHand/*=fase*/)
{
	msgPower msgP;
	InitMsg(msgP);
	msgP.nPower						= nPower;												//关于什么的权限
	msgP.nWhoPower			= nWho;													//谁的权限
	msgP.bCanGiveUp			= m_bCanGiveUp;								//是否放弃出牌
	msgP.isCallLandLord		= m_bIsCallLandlord;						//是否是第一次叫地主
	msgP.HaveGrabPower  = m_bHavePowerGrab[nWho];  // 是否拥有叫地主的权限
	msgP.nCountGiveUp		= m_nCountGiveUp;							// 是否是首家出牌
	msgP.bOpenCard				= bOpenHand;										// 是否可明牌
	msgP.bIsFirstOut				= m_bIsFirstOut;								//是否首轮出牌
	msgP.nScore						= m_nCallScore;									//叫了多少分
	SendGamePacket_All(msgP);
}

// 发送时钟
void CGameLogic::SendClock(int nPower, int nPowerSeat, IUserPtr pUser /*= NULL*/)
{
	msgClock myClock;
	InitMsg(myClock);
	myClock.Who = nPowerSeat;
	size_t nTime = 0;
	//打牌定时器 
	if(nPower == msgPower::PowerPlay)
	{
		 nTime= m_nNowPlayTime;
	}else if (nPower == msgPower::PowerGrab)  //抢地主定时器
	{
		nTime = m_nTimeGrab;
	}
	
	myClock.Time = nTime;
	if(pUser.get() == NULL)
	{
		if(m_pLogicData->IsTrust(nPowerSeat))
		{
			IUserPtr pClockUser = m_pTable->getPlayer(nPowerSeat);
			SendGamePacket_NoSelf(pClockUser, myClock);
			myClock.Time = nTime ;
			SendGamePacket_OnePlayer(pClockUser, myClock);
		}
		else
		{
			SendGamePacket_All(myClock);
		}
	}
	else
	{
		SendGamePacket_OnePlayer(pUser, myClock);
	}
}
//////////////////////////////////////////////////////////////////////////

// 每盘初始化逻辑数据
void CGameLogic::InitLogicData()
{
	m_pLogicData->ResetLogicData();
}


// 开始下一步骤
void CGameLogic::GoToGameStep(BYTE nGameStep)
{
	m_nGameState=nGameStep;
	SendGameState();
	switch(nGameStep)
	{
	case GAME_STATE_TAKE:
		//给客户端预留两秒钟时间处理动画
		time(&m_StepTime);
		break;
	case GAME_STATE_GRAB:
		m_nCallScore = 0;
		m_nCallNum = 0;
		SendPower(msgPower::PowerGrab,m_nNowPlayer);
		SendClock(msgPower::PowerGrab,m_nNowPlayer);
		time(&m_StepTime);
		break;
	case GAME_STATE_PLAY:
		SendIdentity(m_nLandlord);
		m_nNowPlayTime = m_nTimeFirstPlay;
		SendPower(msgPower::PowerPlay,m_nNowPlayer,m_bOpenCardEnable);
		SendClock(msgPower::PowerPlay,m_nNowPlayer);
		time(&m_StepTime);
		//对机器人出牌时间进行设置
		if (m_pTable->getPlayer(m_nNowPlayer)->getUserType() == 1)
		{
			RobotPlayCard();
			srand(time(0));
			m_nRobotOutCardsTime = rand()%(15);
		}
		break;
	case GAME_STATE_END:
		GotoEnd();
		break;
	default:
		break;
	}
	
}


//接收客户端发送过来的抢地主有关的消息，
//决定谁是地主
int CGameLogic::OnGrabLandlord(IUserPtr player,const char* pMsg,int nLen)
{
	msgGrabLandlord* msgGL = ParseMsg(msgGrabLandlord,pMsg,nLen);
	if (GAME_STATE_GRAB != m_nGameState)
	{
		m_pTable->debugString("ERROR:玩家发来抢地主消息，但是这个时候游戏状态不是叫地主状态");
		return -1;
	}
	if (NULL == msgGL)
	{
		m_pTable->debugString("ERROR:解析抢地主包错误");
		return 1;
	}
	else if (msgGL->thePlayerSeat != m_nNowPlayer)
	{
		m_pTable->debugString("ERROR:没有权限的玩家发来抢地主请求%d",msgGL->thePlayerSeat);
		return 2;
	}
	if (m_nGetLandType == GLT_GRAB)
	{
		m_bIsCallLandlord = false;
		if (msgGL->thePlayerOperator == msgGrabLandlord::CALL)					 //叫地主
		{
			m_nGrabCount++;
			m_nCallLandlord  =  msgGL->thePlayerSeat;
		}
		else if (msgGL->thePlayerOperator == msgGrabLandlord::GRAB)			 //抢地主
		{
			SendAction(msgGL->thePlayerSeat,MAX_PLAYER,msgAction::ACT_ANIGRAB);
			m_nGrabCount ++;
			m_nGrabSeat  = msgGL->thePlayerSeat;
			m_bHavePowerGrab[msgGL->thePlayerSeat] = false;
			CalcTotalMultiple();
			SendAddMult();
		}
		else if (msgGL->thePlayerOperator == msgGrabLandlord::NOTGRAB)			//不抢
		{
			m_bHavePowerGrab[msgGL->thePlayerSeat] = false;
		}
		else if (msgGL->thePlayerOperator == msgGrabLandlord::NOTCALL)			//不叫
		{
			m_bHavePowerGrab[msgGL->thePlayerSeat] = false;
		}

		SendGamePacket_All(*msgGL);
		int nHaveGrabPower = 0;
		for(int i=0; i<MAX_PLAYER; i++)
		{
			if (m_bHavePowerGrab[i])
			{
				nHaveGrabPower++;
			}
		}

		if (nHaveGrabPower==0)          
		{
			if (m_nCallLandlord == -1)	 // 没有一个玩家叫地主，重新发牌
			{
				ReTakeCards();
				return 2;
			}else     //  首叫地主的玩家抢回地主
			{
				m_nLandlord  = m_nGrabSeat;
				m_nNowPlayer = m_nLandlord;
				AppendBase();
				GoToGameStep(GAME_STATE_PLAY);
			}
			return 0;
		} 
		else if (nHaveGrabPower == 1 && m_nGrabSeat == -1 && m_nCallLandlord != -1)  //首家叫了地主后其他人都没抢
		{
			m_nLandlord  = m_nCallLandlord;
			m_nNowPlayer = m_nLandlord;
			AppendBase();
			GoToGameStep(GAME_STATE_PLAY);
			return 0;
		}

		//将权限移交给下一个玩家
		if (msgGL->thePlayerOperator == msgGrabLandlord::NOTCALL)
		{
			m_bIsCallLandlord = true; //叫地主
		}
		GotoNextGrabPlayer(m_nNowPlayer);
		return 0;
	}
	else if(GLT_SCORE == m_nGetLandType)
	{
		if (msgGL->thePlayerOperator == msgGrabLandlord::CALLSCORE)
		{
			m_nCallLandlord = msgGL->thePlayerSeat;
			m_nCallScore = msgGL->nScore;
			m_nCallNum++;
			m_nLastCallSeat = msgGL->thePlayerSeat;
		}else if (msgGL->thePlayerOperator == msgGrabLandlord::NOTCALL)
		{
			m_nCallNum++;
		}
		SendGamePacket_All(*msgGL);
		//确定了地主
		if (3 == m_nCallScore)
		{
			m_nLandlord  = m_nCallLandlord;
			m_nNowPlayer = m_nLandlord;
			AppendBase();
			GoToGameStep(GAME_STATE_PLAY);
			return 0;
		}
		
		if (MAX_PLAYER == m_nCallNum)
		{
			//没有玩家叫地主
			if (0 == m_nCallScore)
			{
				ReTakeCards();
				return 2;
			}else if (m_nCallScore >0)
			{
				m_nLandlord  = m_nCallLandlord;
				if (m_nLastCallSeat >=0 && m_nLastCallSeat <MAX_PLAYER)
					m_nNowPlayer = m_nLastCallSeat;
				else 
				{
					ReTakeCards();
					return 2;
				}
				AppendBase();
				GoToGameStep(GAME_STATE_PLAY);
				return 0;
			}
		}
		GotoNextGrabPlayer(m_nNowPlayer);
	}
	
	return 0;
}

//将拥有权限的玩家移交到下一家
void CGameLogic::GotoNextGrabPlayer(int nNowPlayer)
{
	srand(time(0));
	int nNextSeat = (nNowPlayer + 1) % MAX_PLAYER;
	if(m_nGameState  != US_SEEING)
	{
		m_nNowPlayer = nNextSeat;
		if (m_nNowPlayer == m_nFirstPlayer && !m_bHavePowerGrab[m_nNowPlayer])
		{
			GotoNextGrabPlayer(m_nNowPlayer);
			return;
		}
		SendPower(msgPower::PowerGrab, m_nNowPlayer);
		SendClock(msgPower::PowerGrab, m_nNowPlayer);
		time(&m_StepTime);
		srand(time(0));
		m_nRobotGrabTime = rand()%(m_nRobotTimeGrabMax+1);
	}
}

//将底牌添加到地主手牌中
void CGameLogic::AppendBase()
{
	//将底牌显示
	msgBase msgB;
	InitMsg(msgB);
	msgB.nType		 = msgBase::UPDATE;
	msgB.nCount				=  ConvertDataVector(m_dvBaseCards,msgB.cards,m_dvBaseCards.size());
	msgB.nBaseType		= GetBaseCardsType(m_dvBaseCards);
	msgB.nActionType   = msgBase::ADDHAND;
	SendGamePacket_All(msgB);

	m_dvHandCards[m_nLandlord] += m_dvBaseCards;
	msgCards msgC;
	msgC.nType = msgCards::ADDS;
	msgC.nSeat = m_nLandlord;
	msgC.nCount = ConvertDataVector(m_dvBaseCards,msgC.cards,m_dvBaseCards.size());
	IUserPtr puser = m_pTable->getPlayer(m_nLandlord);
	if (puser==NULL)
	{
		m_pTable->debugString("ERROR:玩家获取失败---AppendBase");
		return;
	}
	SendCards(msgC);

	this->GetBaseCardMultiple();
	this->CalcTotalMultiple();
	SendAddMult();
	//SendGamePacket_All(msgC);
}

//重新发牌
void CGameLogic::ReTakeCards()
{
	msgCards msgHC;
	msgBase	msgB;
	InitMsg(msgHC);
	InitMsg(msgB);
	msgHC.nType=msgCards::DEL;
	msgB.nType =msgBase::DEL;
	SendGamePacket_All(msgHC);
	SendGamePacket_All(msgB);

	CHAR szSendInfo[256];
	sprintf(szSendInfo, "没有玩家叫地主，重新发牌！");
	SendHintInfo(NULL, szSendInfo,0xFF00C0FF);

	InitLogicData();
	Reset();
	// 初始化玩家手牌
	m_pLogicData->GetDataPC()->InitPlayerCard();
	GoToGameStep(GAME_STATE_TAKE);
	TakeCards();

	srand((unsigned int)time(NULL));
	m_nNowPlayer	 = rand()%MAX_PLAYER;
	m_nFirstPlayer	= m_nNowPlayer;
	m_bIsCallLandlord = true;
}

//对客户端出牌信息的响应
int CGameLogic::OnMsgOutCard(IUserPtr player,const char* pMsg,int nLen)
{
	msgOutCard* msgOC = ParseMsg(msgOutCard,pMsg,nLen);
	if (GAME_STATE_PLAY != m_nGameState)
	{
		m_pTable->debugString("ERROR:不在玩牌状态，玩家发来出牌消息");
		return -2;
	}
	if (msgOC == NULL)
	{
		m_pTable->debugString("ERROR:解析出牌消息出错");
		return -1;
	}
	if (m_nNowPlayer != msgOC->nSeat)
	{
		m_pTable->debugString("ERROR:发送来的出牌信息和有权限的玩家不符合");
		return -1;
	}
	if (player==NULL)
	{
		m_pTable->debugString("ERROR:一个未知的玩家发来出牌消息");
		return -1;
	}
	if (msgOC->nType == msgOutCard::GIVEUP)	//如果玩家放弃出牌
	{
		//如果上一次出牌玩家的上一家放弃出牌，那么上一次出牌玩家就可
		//成为第一手牌出牌玩家
		m_dvOutCards.clear();
		++m_nCountGiveUp;
		SendOutCard(*msgOC);	
		GotoNextPlayer(m_nNowPlayer);
		return 1;
	}
	else if (msgOC->nType == msgOutCard::OUTCARD)
	{
		//拿出打出的牌数据
		m_dvOutCards.clear();
		for (int i=0;i<msgOC->nCount;i++)
		{
			TCards tmp(msgOC->cards[i]);
			m_dvOutCards.push_back(tmp);
		}
		TCardVector& hCards = m_dvHandCards[m_nNowPlayer];

		//bug调试，该bug导致出牌的时候会出现玩家打出两张完全相同的牌出来
		//该调试临时加在此处，当再次出现该错误的时候，应该检查代码并修正
		if (!JudgeCardsIsSole(m_dvOutCards))
		{
			return ErrorAtCandsNotSole(player,msgOC);
		}

		if (GameRule::CanOut(!m_bCanGiveUp,m_dvLastOutCards,m_dvOutCards))
		{
			//保存地主打出的第一手牌
			if (m_dvHandCards[m_nNowPlayer].size() == 20 && m_nNowPlayer == m_nLandlord)
			{
				m_nNowPlayTime = m_nTimePlay;
				m_dvFirstOut = m_dvOutCards;
			}
			CardTypeInfo cti = GameRule::GetCardType(m_dvOutCards);
			if (cti.nType == CT_BOOM || cti.nType == CT_MAXBOOM)   //打出炸弹
			{
				m_nBoomCount ++;
				CalcTotalMultiple();
				SendAddMult();
			}
	
			m_nCountGiveUp=0;
			hCards -= m_dvOutCards;
			//发送出牌
			SendOutCard(*msgOC);
			//发送牌型动画
			SendAction(m_nNowPlayer,MAX_PLAYER);
			if (hCards.empty())
			{
				GoToGameStep(GAME_STATE_END);
			}else
			{
				m_nLastOutCardSeat = m_nNowPlayer;
				m_dvLastOutCards	   = m_dvOutCards;
				GotoNextPlayer(m_nNowPlayer);
			}
		}else 
		{	
			//异常处理
			if (!m_bCanGiveUp)
			{
				msgOC->nType = msgOutCard::OUTCARD;
				GameRule::SortCards(m_dvHandCards[m_nNowPlayer]);
				msgOC->nCount = ConvertDataVector(m_dvHandCards[m_nNowPlayer],msgOC->cards,1);
				msgOC->nHandCount = m_dvHandCards[m_nNowPlayer].size();
			}else 
			{
				msgOC->nType = msgOutCard::GIVEUP;
			}
			this->OnMsgOutCard(player,(char*)msgOC,sizeof(msgOutCard));
		}
	}
	return 1;
}


//发言
int CGameLogic::OnMsgQiaoPiHua(IUserPtr player,const char* pMsg,int nLen)
{	
	msgQiaoPiHua* lpMsg = ParseMsg(msgQiaoPiHua,pMsg,nLen);
	if (0 == lpMsg) return 0;

	msgQiaoPiHua msgQ;
	msgQ.nSeat = lpMsg->nSeat;
	msgQ.nNum = lpMsg->nNum;
	SendGamePacket_All(msgQ);

	return 1;
}

//托管
int CGameLogic::OnMsgTrush(IUserPtr player,const char* pMsg,int nLen)
{
	if (player == NULL)
	{
		m_pTable->debugString("ERROR:一个未知的玩家发来了托管消息");
		return -1;
	}
	msgTrust* msgT = ParseMsg(msgTrust,pMsg,nLen);
	if (NULL == msgT)
	{
		m_pTable->debugString("ERROR:托管数据解析错误");
		return -1;
	}
	std::string strInfo = player->getNickName();
	if (RT_MATCH == m_nRoomType)
	{
		strInfo = "游戏玩家";	
	}
	
	if (msgT->nType == msgTrust::T_BEGIN)
	{
		strInfo+="进入托管";
		SendHintInfo(NULL,strInfo.c_str(),0xFF00FF00);
		m_pLogicData->SetTrust(msgT->nSeat,true);
	}else if (msgT->nType == msgTrust::T_END)
	{
		strInfo+="取消托管";
		SendHintInfo(NULL,strInfo.c_str(),0xFF00FF00);
		m_pLogicData->SetTrust(msgT->nSeat,false);
	}
	SendGamePacket_OnePlayer(player,*msgT);
	return 1;
}

//明牌
int CGameLogic::OnMsgOpenCard(IUserPtr player,const char* pMsg,int nLen)
{
	msgOpenCard* lpMsg = ParseMsg(msgOpenCard,pMsg,nLen);
	if (lpMsg == NULL)
	{
		m_pTable->debugString("ERROR:眀牌解析出错");
		return -1;
	}
	if (lpMsg->nSeat != m_nLandlord)
	{
		m_pTable->debugString("ERROR:提出眀牌的玩家不是地主.地主位置%d,请求眀牌玩家位置%d",m_nLandlord,lpMsg->nSeat);
		return -2;
	}
	
	m_bOpenHand[lpMsg->nSeat] = true;
	msgCards msgCHC;
	InitMsg(msgCHC);
	msgCHC.nType = msgCards::UPDATE;
	msgCHC.nSeat = m_nLandlord;
	msgCHC.nCount = ConvertDataVector(m_dvHandCards[m_nLandlord],msgCHC.cards,m_dvHandCards[m_nLandlord].size());
	SendGamePacket_All(msgCHC);
	SendGamePacket_OnePlayer(player,*lpMsg);
	SendAction(m_nNowPlayer,MAX_PLAYER,msgAction::ACT_OPENCARD);

	this->CalcTotalMultiple();
	SendAddMult();
	return 1;
}



//做牌，该消息处理只局限与内部测试使用，发行版本绝对不可响应
int CGameLogic::OnMsgMakeCard(IUserPtr player,const char* pMsg,int nLen)
{
#ifdef NEIWANG
	msgMakeCard* lpMsg = ParseMsg(msgMakeCard,pMsg,nLen);
	if (lpMsg == NULL)  return false;
	if (!lpMsg->bIsMake) return true;

	if (lpMsg->nType == msgMakeCard::MAKEBASE)  //做底牌
	{
		m_pLogicData->GetDataPC()->SetBaseCards(lpMsg->cards,lpMsg->nCount);
		for (int i=0; i<lpMsg->nCount;i++)
		{
			m_dvBaseCards.at(i).SetID(lpMsg->cards[i]);
		}
	
	}else if (lpMsg->nType == msgMakeCard::MAKEHANDCARD)							//做手牌
	{
		for (int i=0; i<lpMsg->nCount; i++)
		{
			m_dvHandCards[lpMsg->nSeat].at(i).SetID(lpMsg->cards[i]);
			msgCards msgCHC;
			InitMsg(msgCHC);
			msgCHC.nType = msgCards::UPDATE;
			msgCHC.nSeat = lpMsg->nSeat;
			msgCHC.nCount = ConvertDataVector(m_dvHandCards[lpMsg->nSeat],msgCHC.cards,m_dvHandCards[lpMsg->nSeat].size());
			//SendGamePacket_All(msgCHC);
			SendCards(msgCHC);
		}
	}
#endif
	return 0;
}

//////////////////////////////////////////////////////////////////////////