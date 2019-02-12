#include "GameLogic.h"
// ��ϷЭ����մ����Ա��������


// ���Ϳ�ʼ��Ϸ
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


//����
void CGameLogic::TakeCards()
{
	msgTake msgT;
	InitMsg(msgT);
	msgT.nType =msgTake::TAKE_BEGIN;
	SendGamePacket_All(msgT);
	m_dvBaseCards		 = m_pLogicData->GetDataPC()->GetBaseCards();
	//������
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
	//���������
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
	//���ͷ��ƽ���
	msgT.nType = msgTake::TAKE_END;
	SendGamePacket_All(msgT);
}
//�����߼�����
void CGameLogic::SendLogicConfig(IUserPtr player /*= NULL*/)
{
}

// �����߼�����
void CGameLogic::SendLogicData(IUserPtr player/* = NULL*/)
{
	msgLogicData msgLD;
	InitMsg(msgLD);

	msgLD.BasePlayTypeScore = m_nBaseScore;
	if(player.get() == NULL) //get���������ж�player�Ƿ����
	{
		SendGamePacket_All(msgLD);
	}
	else
	{
		SendGamePacket_OnePlayer(player,msgLD);
	}
}

// ������Ϸ����
void CGameLogic::SendGameData(IUserPtr player/* = NULL*/)
{
}

// ������Ϸ����
void CGameLogic::SendMsgOnGamingEnter(IUserPtr player)
{
	
}


// ������ϷȨ��
void CGameLogic::SendPowerOnGamingEnter(IUserPtr player)
{
}

//������Ϸ״̬
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


//���ͱ���
void CGameLogic::SendAddMult()
{
	msgAddMult msgAM;
	InitMsg(msgAM);
	msgAM.nMultiple  = m_nTotalMultiple;
	SendGamePacket_All(msgAM);
}


//���Ͷ�����Ϣ
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

//���ͳ�����Ϣ
void CGameLogic::SendOutCard(msgOutCard msgOC )
{	
	int nSeat = msgOC.nSeat;
	msgOC.nHandCount = m_dvHandCards[m_nNowPlayer].size();
	IUserPtr pUser = m_pTable->getPlayer(nSeat);
	if(pUser.get() != NULL)
	{
		SendGamePacket_All(msgOC);
	}
	//�������ǳ��ƣ���ô������Ϣ��ʾ�ͻ���ɾ������
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


//��ͻ��˷�����������Ϣ
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

// ����
void CGameLogic::SendCards(msgCards& msgC)
{
	int nSeat = msgC.nSeat;
	if (nSeat<0 || nSeat >=MAX_PLAYER)
	{
		m_pTable->debugString("SendCards��nSeat����nSeat=%d",nSeat);
		return;
	}
	IUserPtr pUser = m_pTable->getPlayer(nSeat);
	if (pUser==NULL)
	{
		m_pTable->debugString("SendCards��û�л�ȡ��%dλ���ϵ��������",nSeat);
		return;
	}
	//�������Ǳb��,��ô����������ҷ�����ʵ�������ݣ�������ȫ���Ʊ�������
	if (m_bOpenHand[msgC.nSeat])
	{
		SendGamePacket_All(msgC);
	}else
	{
		IUserPtr pUser = m_pTable->getPlayer(msgC.nSeat);
		if (pUser == NULL)
		{
			m_pTable->debugString("ERROR:��SendCards��,��ȡ%d��λ���ϵ����ʧ��",msgC.nSeat);
		}
		//���Լ�������ȷ����
		SendGamePacket_OnePlayer(pUser,msgC);
		//��������ҷ����Ʊ�
		for (size_t i=0; i<msgC.nCount; ++i)
		{
			msgC.cards[i] = CID_BACK;
		}
		SendGamePacket_NoSelf(pUser,msgC);
	}
	
}

// ����Ȩ��
void CGameLogic::SendPower(int nPower, int nWho, bool  bOpenHand/*=fase*/)
{
	msgPower msgP;
	InitMsg(msgP);
	msgP.nPower						= nPower;												//����ʲô��Ȩ��
	msgP.nWhoPower			= nWho;													//˭��Ȩ��
	msgP.bCanGiveUp			= m_bCanGiveUp;								//�Ƿ��������
	msgP.isCallLandLord		= m_bIsCallLandlord;						//�Ƿ��ǵ�һ�νе���
	msgP.HaveGrabPower  = m_bHavePowerGrab[nWho];  // �Ƿ�ӵ�не�����Ȩ��
	msgP.nCountGiveUp		= m_nCountGiveUp;							// �Ƿ����׼ҳ���
	msgP.bOpenCard				= bOpenHand;										// �Ƿ������
	msgP.bIsFirstOut				= m_bIsFirstOut;								//�Ƿ����ֳ���
	msgP.nScore						= m_nCallScore;									//���˶��ٷ�
	SendGamePacket_All(msgP);
}

// ����ʱ��
void CGameLogic::SendClock(int nPower, int nPowerSeat, IUserPtr pUser /*= NULL*/)
{
	msgClock myClock;
	InitMsg(myClock);
	myClock.Who = nPowerSeat;
	size_t nTime = 0;
	//���ƶ�ʱ�� 
	if(nPower == msgPower::PowerPlay)
	{
		 nTime= m_nNowPlayTime;
	}else if (nPower == msgPower::PowerGrab)  //��������ʱ��
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

// ÿ�̳�ʼ���߼�����
void CGameLogic::InitLogicData()
{
	m_pLogicData->ResetLogicData();
}


// ��ʼ��һ����
void CGameLogic::GoToGameStep(BYTE nGameStep)
{
	m_nGameState=nGameStep;
	SendGameState();
	switch(nGameStep)
	{
	case GAME_STATE_TAKE:
		//���ͻ���Ԥ��������ʱ�䴦����
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
		//�Ի����˳���ʱ���������
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


//���տͻ��˷��͹������������йص���Ϣ��
//����˭�ǵ���
int CGameLogic::OnGrabLandlord(IUserPtr player,const char* pMsg,int nLen)
{
	msgGrabLandlord* msgGL = ParseMsg(msgGrabLandlord,pMsg,nLen);
	if (GAME_STATE_GRAB != m_nGameState)
	{
		m_pTable->debugString("ERROR:��ҷ�����������Ϣ���������ʱ����Ϸ״̬���ǽе���״̬");
		return -1;
	}
	if (NULL == msgGL)
	{
		m_pTable->debugString("ERROR:����������������");
		return 1;
	}
	else if (msgGL->thePlayerSeat != m_nNowPlayer)
	{
		m_pTable->debugString("ERROR:û��Ȩ�޵���ҷ�������������%d",msgGL->thePlayerSeat);
		return 2;
	}
	if (m_nGetLandType == GLT_GRAB)
	{
		m_bIsCallLandlord = false;
		if (msgGL->thePlayerOperator == msgGrabLandlord::CALL)					 //�е���
		{
			m_nGrabCount++;
			m_nCallLandlord  =  msgGL->thePlayerSeat;
		}
		else if (msgGL->thePlayerOperator == msgGrabLandlord::GRAB)			 //������
		{
			SendAction(msgGL->thePlayerSeat,MAX_PLAYER,msgAction::ACT_ANIGRAB);
			m_nGrabCount ++;
			m_nGrabSeat  = msgGL->thePlayerSeat;
			m_bHavePowerGrab[msgGL->thePlayerSeat] = false;
			CalcTotalMultiple();
			SendAddMult();
		}
		else if (msgGL->thePlayerOperator == msgGrabLandlord::NOTGRAB)			//����
		{
			m_bHavePowerGrab[msgGL->thePlayerSeat] = false;
		}
		else if (msgGL->thePlayerOperator == msgGrabLandlord::NOTCALL)			//����
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
			if (m_nCallLandlord == -1)	 // û��һ����ҽе��������·���
			{
				ReTakeCards();
				return 2;
			}else     //  �׽е�����������ص���
			{
				m_nLandlord  = m_nGrabSeat;
				m_nNowPlayer = m_nLandlord;
				AppendBase();
				GoToGameStep(GAME_STATE_PLAY);
			}
			return 0;
		} 
		else if (nHaveGrabPower == 1 && m_nGrabSeat == -1 && m_nCallLandlord != -1)  //�׼ҽ��˵����������˶�û��
		{
			m_nLandlord  = m_nCallLandlord;
			m_nNowPlayer = m_nLandlord;
			AppendBase();
			GoToGameStep(GAME_STATE_PLAY);
			return 0;
		}

		//��Ȩ���ƽ�����һ�����
		if (msgGL->thePlayerOperator == msgGrabLandlord::NOTCALL)
		{
			m_bIsCallLandlord = true; //�е���
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
		//ȷ���˵���
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
			//û����ҽе���
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

//��ӵ��Ȩ�޵�����ƽ�����һ��
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

//��������ӵ�����������
void CGameLogic::AppendBase()
{
	//��������ʾ
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
		m_pTable->debugString("ERROR:��һ�ȡʧ��---AppendBase");
		return;
	}
	SendCards(msgC);

	this->GetBaseCardMultiple();
	this->CalcTotalMultiple();
	SendAddMult();
	//SendGamePacket_All(msgC);
}

//���·���
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
	sprintf(szSendInfo, "û����ҽе��������·��ƣ�");
	SendHintInfo(NULL, szSendInfo,0xFF00C0FF);

	InitLogicData();
	Reset();
	// ��ʼ���������
	m_pLogicData->GetDataPC()->InitPlayerCard();
	GoToGameStep(GAME_STATE_TAKE);
	TakeCards();

	srand((unsigned int)time(NULL));
	m_nNowPlayer	 = rand()%MAX_PLAYER;
	m_nFirstPlayer	= m_nNowPlayer;
	m_bIsCallLandlord = true;
}

//�Կͻ��˳�����Ϣ����Ӧ
int CGameLogic::OnMsgOutCard(IUserPtr player,const char* pMsg,int nLen)
{
	msgOutCard* msgOC = ParseMsg(msgOutCard,pMsg,nLen);
	if (GAME_STATE_PLAY != m_nGameState)
	{
		m_pTable->debugString("ERROR:��������״̬����ҷ���������Ϣ");
		return -2;
	}
	if (msgOC == NULL)
	{
		m_pTable->debugString("ERROR:����������Ϣ����");
		return -1;
	}
	if (m_nNowPlayer != msgOC->nSeat)
	{
		m_pTable->debugString("ERROR:�������ĳ�����Ϣ����Ȩ�޵���Ҳ�����");
		return -1;
	}
	if (player==NULL)
	{
		m_pTable->debugString("ERROR:һ��δ֪����ҷ���������Ϣ");
		return -1;
	}
	if (msgOC->nType == msgOutCard::GIVEUP)	//�����ҷ�������
	{
		//�����һ�γ�����ҵ���һ�ҷ������ƣ���ô��һ�γ�����ҾͿ�
		//��Ϊ��һ���Ƴ������
		m_dvOutCards.clear();
		++m_nCountGiveUp;
		SendOutCard(*msgOC);	
		GotoNextPlayer(m_nNowPlayer);
		return 1;
	}
	else if (msgOC->nType == msgOutCard::OUTCARD)
	{
		//�ó������������
		m_dvOutCards.clear();
		for (int i=0;i<msgOC->nCount;i++)
		{
			TCards tmp(msgOC->cards[i]);
			m_dvOutCards.push_back(tmp);
		}
		TCardVector& hCards = m_dvHandCards[m_nNowPlayer];

		//bug���ԣ���bug���³��Ƶ�ʱ��������Ҵ��������ȫ��ͬ���Ƴ���
		//�õ�����ʱ���ڴ˴������ٴγ��ָô����ʱ��Ӧ�ü����벢����
		if (!JudgeCardsIsSole(m_dvOutCards))
		{
			return ErrorAtCandsNotSole(player,msgOC);
		}

		if (GameRule::CanOut(!m_bCanGiveUp,m_dvLastOutCards,m_dvOutCards))
		{
			//�����������ĵ�һ����
			if (m_dvHandCards[m_nNowPlayer].size() == 20 && m_nNowPlayer == m_nLandlord)
			{
				m_nNowPlayTime = m_nTimePlay;
				m_dvFirstOut = m_dvOutCards;
			}
			CardTypeInfo cti = GameRule::GetCardType(m_dvOutCards);
			if (cti.nType == CT_BOOM || cti.nType == CT_MAXBOOM)   //���ը��
			{
				m_nBoomCount ++;
				CalcTotalMultiple();
				SendAddMult();
			}
	
			m_nCountGiveUp=0;
			hCards -= m_dvOutCards;
			//���ͳ���
			SendOutCard(*msgOC);
			//�������Ͷ���
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
			//�쳣����
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


//����
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

//�й�
int CGameLogic::OnMsgTrush(IUserPtr player,const char* pMsg,int nLen)
{
	if (player == NULL)
	{
		m_pTable->debugString("ERROR:һ��δ֪����ҷ������й���Ϣ");
		return -1;
	}
	msgTrust* msgT = ParseMsg(msgTrust,pMsg,nLen);
	if (NULL == msgT)
	{
		m_pTable->debugString("ERROR:�й����ݽ�������");
		return -1;
	}
	std::string strInfo = player->getNickName();
	if (RT_MATCH == m_nRoomType)
	{
		strInfo = "��Ϸ���";	
	}
	
	if (msgT->nType == msgTrust::T_BEGIN)
	{
		strInfo+="�����й�";
		SendHintInfo(NULL,strInfo.c_str(),0xFF00FF00);
		m_pLogicData->SetTrust(msgT->nSeat,true);
	}else if (msgT->nType == msgTrust::T_END)
	{
		strInfo+="ȡ���й�";
		SendHintInfo(NULL,strInfo.c_str(),0xFF00FF00);
		m_pLogicData->SetTrust(msgT->nSeat,false);
	}
	SendGamePacket_OnePlayer(player,*msgT);
	return 1;
}

//����
int CGameLogic::OnMsgOpenCard(IUserPtr player,const char* pMsg,int nLen)
{
	msgOpenCard* lpMsg = ParseMsg(msgOpenCard,pMsg,nLen);
	if (lpMsg == NULL)
	{
		m_pTable->debugString("ERROR:�b�ƽ�������");
		return -1;
	}
	if (lpMsg->nSeat != m_nLandlord)
	{
		m_pTable->debugString("ERROR:����b�Ƶ���Ҳ��ǵ���.����λ��%d,����b�����λ��%d",m_nLandlord,lpMsg->nSeat);
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



//���ƣ�����Ϣ����ֻ�������ڲ�����ʹ�ã����а汾���Բ�����Ӧ
int CGameLogic::OnMsgMakeCard(IUserPtr player,const char* pMsg,int nLen)
{
#ifdef NEIWANG
	msgMakeCard* lpMsg = ParseMsg(msgMakeCard,pMsg,nLen);
	if (lpMsg == NULL)  return false;
	if (!lpMsg->bIsMake) return true;

	if (lpMsg->nType == msgMakeCard::MAKEBASE)  //������
	{
		m_pLogicData->GetDataPC()->SetBaseCards(lpMsg->cards,lpMsg->nCount);
		for (int i=0; i<lpMsg->nCount;i++)
		{
			m_dvBaseCards.at(i).SetID(lpMsg->cards[i]);
		}
	
	}else if (lpMsg->nType == msgMakeCard::MAKEHANDCARD)							//������
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