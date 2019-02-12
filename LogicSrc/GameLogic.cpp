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

//��������
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

//����������Ϣ
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
	//������ӵ�������
	m_pTable = table;
	m_theMaxChairs = m_pTable->getChairs();	

	// ����һ�£������Ϸ�����е��������������Ϸ������������Ļ�
	if (m_theMaxChairs > (UINT)GetMaxPlayers())
		m_theMaxChairs = (UINT)GetMaxPlayers();

	//��ӡһ���ļ��汾
	//m_pTable->debugString("FRAMEWORK_LOGIC_VERSION:%s,MaxChairs:%d",FRAMEWORK_LOGIC_VERSION,m_theMaxChairs);

	std::string strTemp;

	// �Ƿ���Ƶ��Ϸ
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

	// ��Ƶ������IP
	m_cfVideoSvrIP = table->getLuaStrValue(szRoomGameRule,"VideoSvrIP");
	if(m_cfVideoSvrIP == "")
	{
		m_cfVideoSvrIP = "122.225.102.99";
	}

	// ��Ƶ�������˿�
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

	// �ü���
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

	// �ü���
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

	// �ұ߹��
	m_cfAdUrl = table->getLuaStrValue(szRoomGameRule,"ADURL");

	// �Ϸ����
	m_cfAdTop = table->getLuaStrValue(szRoomGameRule,"ADTOP");

	// ��Ϸ�����
	m_cfAdGame = table->getLuaStrValue(szRoomGameRule,"ADGAME");

	// ����Ϸ�ͻ��˵����������İ�ť
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
		m_pTable->debugString("========�����߼�����============");
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
	//��������
	m_nRoomType = table->getLuaIntValue(szRoomGameRule,"nRoomType" );
	if (m_nRoomType !=RT_NORMAL && m_nRoomType != RT_MATCH)	m_nRoomType = RT_NORMAL;
	//�Ƹ�����
	m_nGameType = table->getLuaIntValue(szRoomGameRule,"nGameType");
	if (m_nGameType != GT_SCORE && m_nGameType != GT_RICH) m_nGameType = GT_SCORE;
	//��������ʽ
	m_nGetLandType = table->getLuaIntValue(szRoomGameRule,"nGrabLandType");
	if (m_nGetLandType != GLT_GRAB && m_nGetLandType != GLT_SCORE) m_nGetLandType = GLT_GRAB;
	//��Ϸ�׷�
	m_nBaseScore = table->getLuaIntValue(szRoomGameRule,"nBaseScore");
	if (m_nBaseScore<=0 ) m_nBaseScore = 100;
	//�Ƿ���õ���
	m_nEnableProp = table->getLuaIntValue(szRoomGameRule,"nEnableProp");
	if (m_nEnableProp!=1 && m_nEnableProp!=2) m_nEnableProp = 1;
	//Ӯ����
	m_nWinExp = table->getLuaIntValue(szRoomGameRule,"nWinExp");
	if (m_nWinExp<0) m_nWinExp =  0;
	//�侭��
	m_nLosExp = table->getLuaIntValue(szRoomGameRule,"nLosExp");
	if (m_nLosExp<0) m_nLosExp = 0;
	//���ܳͷ�ϵ��
	m_nRunAwayPunish = table->getLuaIntValue(szRoomGameRule,"nRunAwayPunish");
	if (m_nLosExp<=0)  m_nRunAwayPunish = 4;
	//�Ƿ�������ϵͳ
	m_nOn_OffMission = table->getLuaIntValue(szRoomGameRule,"nOnOffMission");
	if (m_nOn_OffMission!= ON_MISSION && m_nOn_OffMission != OFF_MISSION) 
			m_nOn_OffMission = ON_MISSION;
	//�е���ʱ��
	m_nTimeGrab = table->getLuaIntValue(szRoomGameRule,"nTimeGrab");
	if (m_nTimeGrab <=0) m_nTimeGrab = 15;
	//��ͨ����ʱ��
	m_nTimePlay = table->getLuaIntValue(szRoomGameRule,"nTimePlay");
	if (m_nTimePlay <=0) m_nTimePlay  = 20;
	//ÿ�ֵ�һ������ʱ��
	m_nTimeFirstPlay = table->getLuaIntValue(szRoomGameRule,"nTimeFirst");
	if (m_nTimeFirstPlay<=0) m_nTimeFirstPlay = 30;
	//�йܳ���ʱ��
	m_nTruOutTime = table->getLuaIntValue(szRoomGameRule,"nTruOutTime");
	if (m_nTruOutTime <=0) m_nTruOutTime = 2;
	//���������������ʱ��
	m_nRobotTimeGrabMax  = table->getLuaIntValue(szRoomGameRule,"nRobotGrab");
	if (m_nRobotTimeGrabMax <=0) m_nRobotTimeGrabMax = 8;
	//�������ҽеĸ���
	m_nRobotBaseRand=table->getLuaIntValue(szRoomGameRule,"nRobotBaseRand");
	if (m_nRobotBaseRand<=0) m_nRobotBaseRand = 80;
	//�Ƿ����b��
	m_bOpenCardEnable = table->getLuaIntValue(szRoomGameRule,"nEnableOC");
	if (m_bOpenCardEnable!=0 || m_bOpenCardEnable!=1) m_bOpenCardEnable = 1;
	return 0;
}


//�ж��Ƿ��������
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

//ʣ�����һ���Ƶ�ʱ�����һ���Ʋ�����
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

//���Ƴ�ʱ
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

//��������ʱ
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
*�йܣ��йܲ��ԣ������ұ�����ƣ��������ϵ����ܹ�һ��ȫ�����������ƣ����򲻳�
*/
void CGameLogic::IntoTrust(int nSeat)
{
	if (m_nGameState == GAME_STATE_PLAY)//�����й�
	{
		GameRule::SortCards(m_dvHandCards[nSeat],false);
		msgOutCard msgOC;
		InitMsg(msgOC);
		if (GameRule::CanOut(!m_bCanGiveUp,m_dvLastOutCards,m_dvHandCards[m_nNowPlayer]))
		{
			//�ó������������
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
	//����ʱ
	time_t now;
	time(&now);
	// ��ȥʱ��
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
			//���һ���Ʋ���
			T_WatiLastOut(nPassTime);
		}else if (m_pLogicData->IsTrust(m_nNowPlayer) &&nPassTime > m_nTruOutTime)
		{
			//�йܳ���
			IntoTrust(m_nNowPlayer);
		}
		else if (nPassTime >=m_nNowPlayTime)
		{
			//���Ƴ�ʱ
			T_OutTimePlayer();
		}else if (m_bIsRobot[m_nNowPlayer] && nPassTime>m_nRobotOutCardsTime)
		{
			//�����˳���
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
	//������Ϣ����
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
		m_pTable->debugString("ERROR:OnPlayerStart  ��Ҳ�����");
		return;
	}
	if (m_nRoomType == RT_NORMAL)
	{
		CHAR szSendInfo[256];
		sprintf(szSendInfo, "%s ��׼��",pUser->getNickName());
		SendHintInfo(NULL, szSendInfo,0xFF00FF00);
	}
}

// ������Ϸ
void CGameLogic::OnEnterGame(IUserPtr player, bool IsSeen)
{
	if(player.get() == NULL) 
	{
		m_pTable->debugString("ERROR:OnEnterGame player����Ϊ��");
		return ;
	}

	//��֪������ҷ������
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
		m_pTable->debugString("ERROR:OnEnterGame ���λ�ò��Ϸ�.��ֵΪ%d",nSeat);
		return ;
	}
	char szUserEnter[256]={0};
	if (IsSeen)
	{
		SendHintInfo(player,"��ӭ����ʺ󶷵���������ǰ���Թ���",0xFFFFFF00);
		SendHintInfo(player,"Ϊ��ֹ���ף��Թ����ֻ�ɲ鿴��Ϸ��ҳ��ƣ����ɼ��κ�һ������",0xFFFFFF00);
		SendHintInfo(player,"�Թ���Ҳ��ɲ�����",0xFFFFFF00);
		sprintf(szUserEnter,"�Թ���:%s �����ˣ�",player->getNickName());
		SendHintInfo(NULL,szUserEnter,0xFF00FF00);
	}else 
	{
		m_bIsRobot[player->getSeat()] = (1 ==player->getUserType()?true:false);
		char szGameHint[256];
		if (m_nRoomType == RT_NORMAL)
		{
			SendHintInfo(player,"��ӭ����ʺ󶷵�����ף����Ϸ���!",0xFFFFFF00);
			SendHintInfo(player,"�������ֹ����.",0xFFFFFF00);
			SendHintInfo(player,"��Ϸ��ʼ���ֹ���ܣ�ǿ�ƹر���Ϸ���ж�Ϊ�Զ�����.",0xFFFFFF00);
			sprintf(szUserEnter,"%s �����ˣ�",player->getNickName());
			SendHintInfo(NULL,szUserEnter,0xFF00FF00);
		}else  if (m_nRoomType == RT_MATCH)
		{
			SendHintInfo(player,"��ӭ���붷��������,ף����Ϸ���!",0xFFFFFF00);
			SendHintInfo(player,"Ϊ�����ף������޷��鿴������ҵ���Ϣ��",0xFFFFFF00);
		}
	}
}

//�����ܱ���
void CGameLogic::CalcTotalMultiple()
{
	m_nTotalMultiple = 1;

	if (m_nGetLandType  == GLT_GRAB)
	{
		//�������Ĳ����ı���
		m_nGrabCount>0?m_nTotalMultiple = (m_nTotalMultiple<<m_nGrabCount):m_nTotalMultiple;
	}else if (m_nGetLandType == GLT_SCORE)
	{
		//m_nCallScore>0?m_nTotalMultiple = (m_nTotalMultiple<<m_nCallScore):m_nTotalMultiple;
		m_nTotalMultiple = m_nTotalMultiple*m_nCallScore;
	}
	//ը�������ı���
	m_nBoomCount>0?m_nTotalMultiple =(m_nTotalMultiple<<m_nBoomCount):m_nTotalMultiple;
	//���Ʋ����ı���
	for (size_t i=0; i<MAX_PLAYER; ++i)
	{
		if (m_bOpenHand[i])
		{
			m_nTotalMultiple *= 2;
		}
	}
	//��������ı���
	if (m_bIsChunTian)
	{
		m_nTotalMultiple *= 2;
	}
	//����������ı���
	if (m_bIsFanChunTian)
	{
		m_nTotalMultiple *= 2;
	}
	//��������������ı���
	if (m_bFinishMission)
	{
		m_nTotalMultiple *= 2;
	}
	//���Ʋ����ı���
	m_nTotalMultiple *= m_nBaseMultiple;

}
void CGameLogic::OnStartGame()
{
	//////////////////////////////////////////////////////////////////////////
	InitLogicData();
	SendStartGame();
	// ��ʼ���������
	m_pLogicData->GetDataPC()->InitPlayerCard();
	Reset();
	//���ȷ��һ��������е���
	srand((unsigned int)time(NULL));
	m_nNowPlayer = rand()%MAX_PLAYER;
	m_nFirstPlayer = m_nNowPlayer;
	m_bIsCallLandlord	= true;
	//����
	GoToGameStep(GAME_STATE_TAKE);
	TakeCards();

	//��������
	if (m_nOn_OffMission == OFF_MISSION)	//����ϵͳ������
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
// ת��ģ��ӿ�
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
// �������
void CGameLogic::OnExitNormal(IUserPtr pUser)
{
	m_pTable->debugString("�����˳�");
}

void CGameLogic::OnExitForc(IUserPtr pUser)
{
	if(pUser.get() == NULL) 
	{
		m_pTable->debugString("ERROR:���Ҵ���������Ϣ��ʱ�򣬴��ͽ�������һ��NULL");
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
	
	// ��ʱ����뿪����ʾ���ܡ�
	// ��Ϸ���ڼ����У������ܵ����Ϊ���ڲ�������ң�������������������
	short nSeat = pUser->getSeat();
	if (nSeat >= MAX_PLAYER) return;
	CalcTotalMultiple();
	if(m_nTotalMultiple == 0) m_nTotalMultiple = 1;
	long8 nWinLostScore = m_nTotalMultiple * m_nBaseScore * m_nRunAwayPunish;
	//�����ҵ�Ǯ�����ۣ���ô�Ϳ������
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
			sprintf(buf,"ERROR_NP:�����ܵļǷ���,%dλ�õ����û����ȷ�Ļ�ȡ",i);
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
		strName = "��Ϸ���";
	}
	sprintf(szEsport, "%s���ܣ����ֽ���",strName.c_str());
	SendHintInfo(NULL,szEsport,0xFFFFFF00);
	if (m_nGameType==GT_SCORE)
		sprintf(szEsport,"%s�۳�%d��",strName.c_str(),nWinLostScore);
	else if (m_nGameType == GT_RICH)
		sprintf(szEsport,"%s�۳�%d�ʺ��",strName.c_str(),nWinLostScore);
	SendHintInfo(NULL,szEsport,0xFFFFFF00);
	for (size_t i=0; i<MAX_PLAYER; ++i)
	{
		m_pTable->debugString("�������%s ,������ֵ%d",strName.c_str(),myTallys[i]._rich);
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
	//������Ӯ����
	int nWinLost[MAX_PLAYER] = {0};
	int bIsLandlordWin  = -1;
	if (m_nNowPlayer == m_nLandlord)
	{
		bIsLandlordWin = 1;
	}
	bool bPlayerRichLess = false; //��ҲƸ�����
	for (int i=0; i<MAX_PLAYER; i++)
	{
		IUserPtr lpUser = m_pTable->getPlayer(i);
		if(lpUser == NULL) 
		{
			char buf[1024] = {0};
			sprintf(buf,"ERROR_NP:�������ļǷ���,%dλ�õ����û����ȷ�Ļ�ȡ!!!",i);
			m_pTable->debugString(buf);
			continue;
		}
		//�Ƹ�����ϷΪ�˷�ֹ��ҲƸ������ۣ���ô�������쳣����
		//�����Ӯ��ֵ�Ѿ����������Ϸ�Ƹ�����ô�����۳������Ϸ�Ƹ�
		//ͬʱҲֻ���ⲿ�ֲƸ�
		int nRich = lpUser->getGameRich();
		if (i == m_nLandlord)   //����
		{
			nWinLost[i] = m_nBaseScore*m_nTotalMultiple * bIsLandlordWin * 2;
		}else				   //�ǵ���
		{
			nWinLost[i] = -1 * m_nBaseScore*m_nTotalMultiple * bIsLandlordWin ;
		}
		if (GT_RICH == m_nGameType && nWinLost[i]<0 && nRich< abs(nWinLost[i]))
		{
			bPlayerRichLess = true;
			nWinLost[i] = -nRich;
		}
	}
	//�ԼǷ��ٴ���֤
	if (bPlayerRichLess)
	{
		if (1==bIsLandlordWin) //����Ӯ
		{
			SendHintInfo(m_pTable->getPlayer(m_nLandlord),"������ܲƸ�����������Ӯ�Ƹ�����Ӧ����!!", 0XFF00FF00);
			nWinLost[m_nLandlord] = 0;
			for (size_t i=0; i<MAX_PLAYER; ++i)
			{
				if (i != m_nLandlord) nWinLost[m_nLandlord] += abs(nWinLost[i]);
			}
		}else //�����䣬�ֲ�����
		{
			for (size_t i=0; i<MAX_PLAYER; ++i)
			{	
				if (i != m_nLandlord) 
				{
					SendHintInfo(m_pTable->getPlayer(i)," �����Ƹ�����������Ӯ�Ƹ�����Ӧ����!!", 0XFF00FF00);
					nWinLost[i] = abs(nWinLost[m_nLandlord])/2;
				}
			}
		}
	}
	if (m_nEnableProp == 1 && m_nGameType ==GT_SCORE)
	{
		//����Ч������
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
				if (PSTD.at(j).propid == 35 && nWinLost[i] > 0) //���ӵ��˫���Ƿֿ�
				{
					strInfo += "˫�����ֿ���Ч�����û��ַ���!";
					sprintf(buf,"˫�����ֿ�����%dʱ%d��%d���ʧЧ",nHour,nMin,nSec);
					strInfo += buf;
					SendHintInfo(NULL,strInfo.c_str(),0xFFFF7800);
					nWinLost[i] = nWinLost[i]*2;
				}else if (PSTD.at(j).propid == 36 && nWinLost[i] < 0)  //���ӵ�л����
				{
					strInfo += "�������Ч�������۷�!";
					sprintf(buf,"���������%dʱ%d��%d���ʧЧ",nHour,nMin,nSec);
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
				SendAction(m_nNowPlayer,MAX_PLAYER,msgAction::ACT_LANDCHUNTIAN);  //�������춯��
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
	// �Ʒ�
	TALLY myTallys[MAX_PLAYER];
	int nTallyCount = 0;

	for(int i=0; i<MAX_PLAYER; i++)
	{
		CLogicPlayer* pPlayer = GetLogicPlayer(i);
		if(pPlayer == NULL) 
		{
			char buf[1024] = {0};
			sprintf(buf,"ERROR_NP:�������ļǷ���,%dλ�õ����û����ȷ�Ļ�ȡ",i);
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
// ְ�ܣ���ȡ���Ƶ�����
// ���أ����ص��Ƶ�����
// ��ע��������һ��ר�к�����ֻ�ڶ�����ʹ�ã�
//	     �����У�
//			ͬ����˳�ӡ����ӡ��׷֡�3
//			ͬ��˳�׷֡�5
//			���׷֡�2
// �����㷨--��С����
bool sort_min_max(TCards card1, TCards card2)
{
	return (card1.GetValue() < card1.GetValue());
}

size_t CGameLogic::GetBaseCardsType(TCardVector& dvCards)
{
	if (dvCards.empty())
	{
		m_pTable->debugString("ERROR:��ȡ��������ʱ����!����Ĳ���Ϊ��");
		return BC_ERROR;
	}
	GameRule::SortCards(dvCards);
	int m = dvCards.back().GetID();
	//�д���
	if (CID_BJ == dvCards.back().GetID())
	{
		return BC_HAVEBIGJOKEER;
	}
	
	//ͬ��
	if (dvCards.front().GetColor() == dvCards.at(1).GetColor() && 
		dvCards.front().GetColor() == dvCards.back().GetColor())
	{
		//ͬ��˳
		if (dvCards.front().GetValue()+2 == dvCards.back().GetValue()&&
			dvCards.at(1).GetValue()+1 == dvCards.back().GetValue())
		{
			return BC_STRAIGHTFLUSH;  
		}else 
		{
			return BC_FLUSH;
		}
	}
	//˳��
	if (dvCards.front().GetValue()+2 == dvCards.back().GetValue()&&
		dvCards.at(1).GetValue()+1 == dvCards.back().GetValue())
	{
		return BC_STRAIGHT;  
	}
	//����
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

//���������к�����ͬ�Ƶ�ʱ����쳣����
int CGameLogic::ErrorAtCandsNotSole(IUserPtr player,msgOutCard* msgOC)
{
	std::string strErr = "ERROR_NS_B:���Ʋ�Ψһ";
	m_pTable->debugString(strErr.c_str());
	strErr = "��������:";
	for (int i=0; i<m_dvOutCards.size(); ++i)
	{
		strErr += m_dvOutCards.at(i).GetName();
	}
	m_pTable->debugString(strErr.c_str());
	if (player->getUserType() == 1) m_pTable->debugString("��������ǻ�����");
	else m_pTable->debugString("����������������");
	strErr.clear();
	for (int i=0; i<MAX_PLAYER;++i)
	{
		IUserPtr playerTest = m_pTable->getPlayer(i);
		if (playerTest == NULL) strErr += "δ��ȡ�����   ";
		strErr+= playerTest->getNickName();
		strErr+= "\n";
		strErr+="��ǰ���ƣ�\n";
		GameRule::SortCards(m_dvHandCards[i]);
		for (int j=0; j<m_dvHandCards[i].size(); ++j)
		{
			strErr+=m_dvHandCards[i].at(j).GetName();
		}
		strErr+="\n";
		strErr+="��ʼ����:\n";
		TCardVector dvInit = m_pLogicData->GetDataPC()->GetHandCards(i);
		GameRule::SortCards(dvInit);
		for (int j=0; j<dvInit.size(); ++j)
		{
			strErr+=dvInit.at(j).GetName();
		}
		strErr+="\n";
	}
	m_pTable->debugString(strErr.c_str());
	strErr = "������:  ";
	strErr+= m_pTable->getPlayer(m_nLandlord)->getNickName();
	m_pTable->debugString(strErr.c_str());
	strErr = "ӵ�г���Ȩ����:  ";
	strErr+= m_pTable->getPlayer(m_nNowPlayer)->getNickName();
	m_pTable->debugString(strErr.c_str());
	strErr ="��һ�ֳ��ƣ�\n";
	if (m_dvLastOutCards.empty()) strErr += "Ϊ��";
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
	if (msgOC->nCount ==0)    //û��ȡ������
	{
		return -1;
	}
	this->OnMsgOutCard(player,(char*)msgOC,sizeof(msgOutCard));
	SendHintInfo(player,"ϵͳ����ҳ����鵽��ʹ�÷Ƿ�������Զ���������Ȩ�ޣ�����жϴ������ͼ��ϵ�ͷ���Ϊ�������������Գ�ֿ��Ǹ�⣡",0xFF00FF00);
	return 0;
}

void CGameLogic::GotoEnd()
{
	//�����������ʾһ��
	for (int i=0; i<MAX_PLAYER; ++i)
	{
		msgCards msgC;
		InitMsg(msgC);
		msgC.nType = msgCards::UPDATE;
		msgC.nSeat = i;
		msgC.nCount = ConvertDataVector(m_dvHandCards[i],msgC.cards,m_dvHandCards[i].size());
		SendGamePacket_All(msgC);
	}
	// ����
	// �Ƿ���������߷�����
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
				&& acoOutCall.returnscore == -1)  //����
			{
				msgGL.thePlayerOperator = msgGrabLandlord::NOTCALL;
			}else if (acoOutCall.returnscore > aciNowCall.callscore
				&&	acoOutCall.returnscore == 0) //�е���
			{
				msgGL.thePlayerOperator = msgGrabLandlord::CALL;
			}else if (acoOutCall.returnscore > aciNowCall.callscore 
				&& acoOutCall.returnscore > 0)  //������
			{
				msgGL.thePlayerOperator = msgGrabLandlord::GRAB;
			}else if (acoOutCall.returnscore == aciNowCall.callscore
				&& aciNowCall.callscore >= 0) //����
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
