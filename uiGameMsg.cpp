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
		如果代码显示效果不理想，可尝试将字体设置为方正兰亭中黑13号字体
*/
//////////////////////////////////////////////////////////////////////////
//消息处理
//////////////////////////////////////////////////////////////////////////
#include "uiGameArea.h"


bool cmp_min_max_byte(BYTE nID1,BYTE nID2)
{
	TCards card1(nID1);
	TCards card2(nID2);
	return (GameRule::CardWeight(card1) < GameRule::CardWeight(card2));
}
bool cmp_max_min_byte(BYTE nID1,BYTE nID2)
{
	TCards card1(nID1);
	TCards card2(nID2);
	return (GameRule::CardWeight(card1) > GameRule::CardWeight(card2));
}


int uiGameArea::OnMsgExitProcRes(LPCSTR pMsg,int nLen)
{
	msgExitProcRes* lpMsg = ParseMsg(msgExitProcRes,pMsg,nLen);
	if (NULL == lpMsg) return 0;
	
	//隐藏玩牌按钮
	ShowAllPlayBtn(FALSE,FALSE,FALSE,FALSE,FALSE);
	ShowGrabBtn(TRUE,FALSE);
	ShowGrabBtn(FALSE,FALSE);
	ShowBtnTrush();
	for (int i=0; i<MAX_PLAYER;i++)
	{
		ShowSignGrab(i, msgGrabLandlord::CALL,FALSE);
		ShowSignGrab(i, msgGrabLandlord::NOTCALL,FALSE);
		ShowSignGrab(i, msgGrabLandlord::GRAB,FALSE);
		ShowSignGrab(i, msgGrabLandlord::NOTGRAB,FALSE);
		ShowNotOutSign(i,FALSE);
	}
	ClearAllCallSign();
	m_pClockUI ->SetRender(FALSE);
	return 1;
}

//发牌结束
int uiGameArea::OnMsgTake(LPCTSTR pMsg, int nLen )
{
	msgTake* lpMsg = ParseMsg(msgTake,pMsg,nLen);
	if (NULL == lpMsg)
	{
		ShowTalk(0xFF00FF00,"发牌消息错误，请截图联系客服!");
	}
	if (lpMsg->nType == msgTake::TAKE_BEGIN)
	{
		PlayEffect("game.snd_start");
	}else if (lpMsg->nType == msgTake::TAKE_END)
	{
		for (size_t i=0; i<MAX_PLAYER; ++i)
		{
			sort(m_HandCards[i].begin(),m_HandCards[i].end(),cmp_max_min_byte);
			m_pHandCards[i]->SetCards(m_HandCards[i]);
			m_pHandCards[i]->SetRender(TRUE);
			ArrangeLocalPosition(GetGuiW(), GetGuiH(), i);
		}
	}
	return 0;
}
//发牌
int uiGameArea::OnMsgCards(LPCSTR pMsg,int nLen)
{
	msgCards* lpMsg = ParseMsg(msgCards,pMsg,nLen);
	if (NULL == lpMsg) 
	{
		ShowTalk(0xFF00FF00,"解析手牌包的时候出现错误，请截图联系客服!");
		return 0;
	}
	int nLocal = SEAT2LOCAL(lpMsg->nSeat);
	int nSeat = lpMsg->nSeat;

	if (lpMsg->nType == msgCards::DEL)
	{
		for (int i=0 ;i<MAX_PLAYER;i++)
		{
			m_HandCards[i].clear();
			m_pHandCards[i]->UpdateAllCards();
			m_pHandCards[i]->SetRender(FALSE);
		}
	}else if (lpMsg->nType == msgCards::UPDATE)
	{
		m_HandCards[nLocal].clear();
		m_HandCards[nLocal].insert(m_HandCards[nLocal].end(),lpMsg->cards,lpMsg->cards+lpMsg->nCount);
		sort(m_HandCards[nLocal].begin(),m_HandCards[nLocal].end(),cmp_max_min_byte);
		m_pHandCards[nLocal]->SetCards(m_HandCards[nLocal]);
		m_pHandCards[nLocal]->SetRender(TRUE);
		//显示玩家手牌张数
		if (m_nGameState == GAME_STATE_PLAY)
		{
			ShowHandCount(m_HandCards[nLocal].size(),nLocal);
		}
		ArrangeLocalPosition(GetGuiW(), GetGuiH(), nLocal);
	}else if (lpMsg->nType == msgCards::ADDS)
	{
		TCARDS tmpBase;
		tmpBase.insert(tmpBase.end(),lpMsg->cards,lpMsg->cards+lpMsg->nCount);
		m_HandCards[nLocal].insert(m_HandCards[nLocal].end(),tmpBase.begin(),tmpBase.begin()+tmpBase.size());
		sort(m_HandCards[nLocal].begin(),m_HandCards[nLocal].end(),cmp_max_min_byte);
		m_pHandCards[nLocal]->SetCards(m_HandCards[nLocal]);
		if (nLocal == SELFLOCALNO)
		{
			m_pHandCards[nLocal]->SetCheckedCards(tmpBase);
		}
		ShowHandCount(m_HandCards[nLocal].size(),nLocal);
		ArrangeLocalPosition(GetGuiW(), GetGuiH(), nLocal);
	}else if (lpMsg->nType == msgCards::ADD)
	{
		m_HandCards[nLocal].insert(m_HandCards[nLocal].end(),lpMsg->cards,lpMsg->cards+lpMsg->nCount);
		m_pHandCards[nLocal]->SetCards(m_HandCards[nLocal]);
		m_pHandCards[nLocal]->SetRender(TRUE);
		ShowHandCount(m_HandCards[nLocal].size(),nLocal);
		ArrangeLocalPosition(GetGuiW(), GetGuiH(), nLocal);
	}
	int nHandSize = m_HandCards[SEAT2LOCAL(lpMsg->nSeat)].size();
	if (nHandSize==2 && m_nGameState == GAME_STATE_PLAY)
	{
		IPLAYER* player = GetPlayerBySeat(nSeat);
		if (!player) return -1;
		if (player->IsMale())   //男人
		{
			PlayEffect("game.snd_m_Remain2");
		}else if (player->IsFemale())//女人
		{
			PlayEffect("game.snd_w_Remain2");
		}
		ShowAni("AniWarning_",false,lpMsg->nSeat,-1,true);

	}else if (nHandSize==1 && m_nGameState == GAME_STATE_PLAY)
	{
		IPLAYER* player = GetPlayerBySeat(nSeat);
		if (!player) return -1;
		if (player->IsMale())   //男人
		{
			PlayEffect("game.snd_m_Remain1");
		}else if (player->IsFemale())//女人
		{
			PlayEffect("game.snd_w_Remain1");
		}
		ShowAni("AniWarning_",false,lpMsg->nSeat,-1,true);
	}
	return 1;
}


//结束发牌
int uiGameArea::OnMsgEndSendCard(LPCSTR pMsg,int nLen)
{
	msgEndSendCard* lpMsg = ParseMsg(msgEndSendCard,pMsg,nLen);
	if (NULL == lpMsg) return 0;


	return 1;
}


//权限
int uiGameArea::OnMsgPower(LPCSTR pMsg,int nLen)
{
	msgPower* lpMsg = ParseMsg(msgPower,pMsg,nLen);
	if (NULL == lpMsg) 
	{
		char buf[1024] ={0};
		sprintf(buf,"权限解析错误，请截图联系客服msgPower=%d,nLen=%d",sizeof(msgPower),nLen);
		ShowTalk(0xFF00FF00,buf);
		return 0;
	}
	//隐藏不出按钮
	if (2 == lpMsg->nCountGiveUp)
	{
		for (int i=0; i<MAX_PLAYER; i++)
		{
			this->ShowNotOutSign(i,FALSE,true);
		}
		m_dvFrontOutCards.clear();
	}
	m_nLocalPowerSeat = SEAT2LOCAL(lpMsg->nWhoPower);
	if (msgPower::PowerPlay == lpMsg->nPower)
	{
		this->ShowOutCard(m_nLocalPowerSeat,FALSE);
		this->ShowNotOutSign(m_nLocalPowerSeat,FALSE);
	}else if (msgPower::PowerGrab == lpMsg->nPower)
	{
		this->ShowSignGrab(m_nLocalPowerSeat,msgGrabLandlord::CALL,FALSE);
		this->ShowSignGrab(m_nLocalPowerSeat,msgGrabLandlord::NOTCALL,FALSE);
		this->ShowSignGrab(m_nLocalPowerSeat,msgGrabLandlord::GRAB,FALSE);
		this->ShowSignGrab(m_nLocalPowerSeat,msgGrabLandlord::NOTGRAB,FALSE);
	}
	
	if(GetSelfPlayer() && GetSelfPlayer()->getState() != US_SEEING && SELFLOCALNO == m_nLocalPowerSeat)
	{
		if(TRUE == m_tSetUpParam.IsWndFlash)
		{
			FlashWindow(m_lpFramework->GetSafeWnd(),TRUE);
		}
		//叫地主权限
		if (msgPower::PowerGrab == lpMsg->nPower && m_nGameState == GAME_STATE_GRAB)
		{
			GetPowerGrab(*lpMsg);
		}
		//出牌权限
		if(msgPower::PowerPlay == lpMsg->nPower && m_nGameState == GAME_STATE_PLAY)
		{
			GetPowerPlay(*lpMsg);
			m_nCountBoomPrompt = 0;
			m_nCountPrompt = 0;
			m_nCountMaxBoomPrompt = 0;
			m_bIsAlreadyOut = false;
		}
	}
	return 1;
}


//进入游戏
int uiGameArea::OnMsgEnterGame(LPCSTR pMsg,int nLen)
{
	msgEnterGame* lpMsg = ParseMsg(msgEnterGame,pMsg,nLen);
	if(NULL == lpMsg) return 0;
	return 1;
}

int uiGameArea::OnMsgTableRule(LPCSTR pMsg,int nLen)
{
	msgTableRule* lpMsg = ParseMsg(msgTableRule,pMsg,nLen);
	if (NULL == lpMsg) 
	{
		ShowTalk(0xFF00FF00,"接收桌子规则时包解析出错，请截图联系客服!");
		return 0;
	}
	m_nRoomType = lpMsg->nRoomType;
	m_nGameType = lpMsg->nGameType;
	m_nBasePlayScore =lpMsg->nBase;
	m_nGrabLandType = lpMsg->nGrabLandType;
	
	HGUI hGui     = m_lpFramework->GetCfgGui("","KW_BTN_START_GAME");
	if (hGui==NULL)
	{
		ShowTalk(0xFF00FF00,"开始按钮资源获取失败，请截图联系客服");
		return 0;
	}
	if (RT_MATCH == m_nRoomType)
	{
		if (!hGui->DetachAnimate())
		{
			ShowTalk(0xFF00FF00,"开始按钮资源释放失败，请截图联系客服");
		}
		if(!hGui->AttachAnimate("GameArea.BtnStartMatch",0,4))
		{
			ShowTalk(0xFF00FF00,"开始按钮资源关联失败，请截图联系客服！");
		}
	}
	hGui->SetRender(TRUE);
	
	//显示换桌按钮
	if (RT_NORMAL ==m_nRoomType)
	{
		uiButton* lpBtnCT = (uiButton*) m_lpFramework->GetCfgGui( "", "KW_CHANGESEAT_TABLE" );	
		if (lpBtnCT)
		{
			lpBtnCT->SetRender(TRUE);	
		}
	}
	ArrangeButtonPosition(GetGuiW(),GetGuiH());
	return 1;
}
// 开始游戏
int uiGameArea::OnMsgStartGame(LPCSTR pMsg,int nLen)
{
	msgStartGame* lpMsg = ParseMsg(msgStartGame,pMsg,nLen);
	if (NULL == lpMsg) 
	{
		ShowTalk(0xFF00FF00,"开始游戏的时候包解析错误，请截图联系客服!");
		return 0;
	}
	ShowPlayReadySign(MAX_PLAYER,FALSE);
	ShowBase();
	return 1;
}

//重连
int uiGameArea::OnMsgReLink(LPCSTR pMsg,int nLen)
{
	msgReLink* lpMsg = ParseMsg(msgReLink,pMsg,nLen);
	if(NULL == lpMsg) return 0;

	return 1;
}

//逻辑配置
int uiGameArea::OnMsgLogicConfig(LPCSTR pMsg,int nLen)
{
	msgLogicConfig* lpMsg = ParseMsg(msgLogicConfig,pMsg,nLen);
	if(NULL == lpMsg ) return 0;

	m_LogicConfig = *lpMsg;

	return 1;;
}

//逻辑数据
int uiGameArea::OnMsgLogicData(LPCSTR pMsg,int nLen)
{
	msgLogicData* lpMsg = ParseMsg(msgLogicData,pMsg,nLen);
	if(NULL == lpMsg) return 0;

	m_LogicData = *lpMsg;
	

	IPLAYER* iplayer = GetSelfPlayer();
	if(iplayer == NULL)  return 1;

	return 1;
}

//游戏状态
int uiGameArea::OnMsgGameState(LPCSTR pMsg,int nLen)
{
	msgGameState* lpMsg = ParseMsg(msgGameState,pMsg,nLen);
	if (NULL == lpMsg) 
	{
		ShowTalk(0xFF00FF00,"游戏状态解析错误,请截图联系客服");
		return 0;
	}
	m_nGameState = lpMsg->nGameState;
	switch(m_nGameState)
	{
	case GAME_STATE_TAKE:
	case GAME_STATE_GRAB:
	case GAME_STATE_PLAY:
		m_pClockUI->SetRender(FALSE);
		ShowBasePoint(m_nBasePlayScore,TRUE);
		ShowAllMult(m_nTotalMult,TRUE);
		ShowBtnTrush();
		for (size_t i=0; i<MAX_PLAYER; ++i)
		{
			this->ShowSignGrab(i,msgGrabLandlord::CALL,FALSE);
			this->ShowSignGrab(i,msgGrabLandlord::NOTCALL,FALSE);
			this->ShowSignGrab(i,msgGrabLandlord::GRAB,FALSE);
			this->ShowSignGrab(i,msgGrabLandlord::NOTGRAB,FALSE);
		}
		ClearAllCallSign();
		break;
	case GAME_STATE_END:
		ShowBtnTrush();
		break;
	default:
		{
			break;
		}
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////
//任务
int uiGameArea::OnMission(LPCSTR pMsg, int nLen)
{
	msgMission* msgM = ParseMsg(msgMission,pMsg,nLen);
	int _nMissionCT = msgM->nMisssionID;
	if (_nMissionCT>=0 && _nMissionCT<9)
	{
		ShowMission(msgM->nMisssionID,ST_MAX,TRUE);
	}else 
	{
		ShowMission(-1,ST_MAX,FALSE);
		ShowMission(-1,ST_MIN,FALSE);
	}
	return 1;
}

//倍数增加
int uiGameArea::OnMsgAddMul(LPCSTR pMsg, int nLen)
{
	msgAddMult* lpMsg = ParseMsg(msgAddMult,pMsg,nLen);
	if (NULL == lpMsg) return 0;
	m_nTotalMult = lpMsg->nMultiple;
	ShowAllMult(m_nTotalMult,TRUE);
	return 1;
}

//游戏数据
int uiGameArea::OnMsgGameData(LPCSTR pMsg,int nLen)
{
	msgGameData* lpMsg = ParseMsg(msgGameData,pMsg,nLen);
	if(NULL == lpMsg) return 0;


	return 1;
}



int uiGameArea::OnMsgAction(LPCSTR pMsg, int nLen)
{
	msgAction* lpMsg = ParseMsg(msgAction,pMsg,nLen);
	if (NULL == lpMsg) return -1;

	int nFromSeat = lpMsg->theFromSeat;
	int nToSeat   = lpMsg->theToSeat;
	int nAction   = lpMsg->theAction;
//	OpenAni();
	switch(nAction)
	{
	case msgAction::ACT_ZHADAN:
		PlayEffect("game.snd_tomb");
		ShowAni("AniBoom_",TRUE,nFromSeat);
		break;

	case  msgAction::ACT_FEIJI:
		ShowAni("AnimationFeiJi_",TRUE,nFromSeat);
		break;

	case msgAction::ACT_HUOJIAN:
		ShowAni("AnimationHuoJian_",TRUE,nFromSeat);
		PlayEffect("game.snd_tomb");
		break;

	case msgAction::ACT_SANDAIER:
		ShowAni("AnimationSanDaiEr_",TRUE, nFromSeat); 
		break;

	case msgAction::ACT_SANDAIYI:
		ShowAni("AnimationSanDaiYi_",TRUE, nFromSeat); 
		break;

	case msgAction::ACT_SANSHUN:
		ShowAni("AnimationFeiJi_",TRUE, nFromSeat); ;
		break;

	case msgAction::ACT_SHUANGSHUN:
		ShowAni("AnimationShuangShun_",TRUE,nFromSeat);
		break;

	case  msgAction::ACT_SHUNZI:
		ShowAni("AnimationDanShun_",TRUE, nFromSeat);
		break;

	case msgAction::ACT_ANIGRAB:
		ShowAni("AnimationQiangDiZhu",FALSE);
		break;

	case msgAction::ACT_SIDAIER:
		ShowAni("AnimationSiDaiEr_",TRUE,nFromSeat);
		break;

	case msgAction::ACT_OPENCARD:
		ShowAni("AniMingPai",FALSE);
		PlayGameEffct("mingpai");
		break;

	case msgAction::ACT_LANDLOS:
		ShowAni("AniFarmerWin",FALSE,-1,-1);
		break;

	case msgAction::ACT_LANDWIN:
		ShowAni("AniBankerWin",FALSE,-1,-1);
		break;

	case msgAction::ACT_LANDCHUNTIAN:
		ShowAni("AniBankerWinEx",FALSE,-1,-1);
		break;

	case  msgAction::ACT_NOLANDCHUNTIAN:
		ShowAni("AniFarmerWinEx",FALSE,-1,-1);
		break;

	case  msgAction::ACT_FINISHMISSION:
		PlayEffect("game.snd_finishmission");
		ShowAni("AniRenWuWanCheng",FALSE);
		break;
	default:
		break;
	}
	return 1;
}

//底牌
int uiGameArea::OnMsgBase(LPCSTR pMsg,int nLen)
{
	msgBase* lpMsg = ParseMsg(msgBase,pMsg,nLen);
	if (NULL == lpMsg) 
	{
		ShowTalk(0xFF00FF00,"解析底牌包的时候出现错误，请截图联系客服!");
		return 0;
	}
	
	if(lpMsg->nType == msgBase::DEL)
	{
		m_BaseCards.clear();
		m_pBaseCards->DeleteAllCards();
		m_pBaseCards->UpdateAllCards();
		m_pBaseCards->SetRender(FALSE);
	}else if (lpMsg->nType == msgBase::UPDATE)
	{
		m_BaseCards.clear();
		m_BaseCards.insert(m_BaseCards.end(),lpMsg->cards,lpMsg->cards+lpMsg->nCount);
		if (m_BaseCards.empty())
		{
			ShowTalk(0xFF00FF00,"底牌数据出错，请截图联系客服");
			return 0;
		}
		m_pBaseCards->DeleteAllCards();
		m_pBaseCards->SetCards(m_BaseCards);
		m_nBaseType = lpMsg->nActionType;
		ShowBase(TRUE,lpMsg->nActionType);
		ShowBaseCardsType(lpMsg->nBaseType);
	}
	return 1;
}



//时钟
int uiGameArea::OnMsgClock(LPCSTR pMsg, int nLen)
{
	msgClock* lpMsg = ParseMsg(msgClock,pMsg,nLen);
	if(NULL == lpMsg) return 0;

	if(lpMsg->Who >=0 && lpMsg->Who < MAX_PLAYER)
	{
		m_pClockUI->SetLocal(SEAT2LOCAL(lpMsg->Who));
	}
	else 
	{
		return -1;
	}

	m_pOutCard[SEAT2LOCAL(lpMsg->Who)] ->SetRender(FALSE);
	m_lpFramework->SetGameTimer(TIMER_POWER,lpMsg->Time);
	m_pClockUI->SetClock(lpMsg->Time);
	m_pClockUI->SetRender(TRUE);
	//m_pClockUI->SetCenter(this,0,0);
	ArrangeClockPosition(GetGuiW(), GetGuiH(),SEAT2LOCAL(lpMsg->Who));
	return 1;
}



//////////////////////////////////////////////////////////////////////////
//发言
int uiGameArea::OnMsgQiaoPiHua(LPCTSTR pMsg, int nLen )
{
	msgQiaoPiHua* lpMsg = ParseMsg(msgQiaoPiHua,pMsg,nLen);
	if (NULL == lpMsg) return 0;

	SHORT nLocal = SEAT2LOCAL(lpMsg->nSeat);

	GUI* pGuiWord = (GUI*)m_lpFramework->GetCfgGui("",GetFormatString("KW_SPEAK_WORD_%d",nLocal));
	GUI* pGuiDialog = (GUI*)m_lpFramework->GetCfgGui("",GetFormatString("KW_DIALOG_%d",nLocal));
	if (pGuiWord && pGuiDialog)
	{
		pGuiWord->GoAniFrame(lpMsg->nNum);
		pGuiWord->SetRender(TRUE);
		pGuiDialog->SetRender(TRUE);
	}

	m_lpFramework->SetGameTimer(TIMER_SPEAK_DIALOG_L + nLocal,2);
	return 1;
}

//结束游戏
int uiGameArea::OnMsgEndGame(LPCSTR pMsg,int nLen)
{
	//关闭警报动画
	for (int i=0; i<MAX_PLAYER;i++)
	{
		ShowAni("AniWarning_",FALSE,i,0,FALSE);
	}
	ShowAllPlayBtn(FALSE,FALSE,FALSE,FALSE);
	IPLAYER* iSelf = GetSelfPlayer();

	for (int i=0; i<MAX_PLAYER; i++)
	{
		this->ShowNotOutSign(i,FALSE);
	}
	//结算
	msgEndGame* lpMsg = ParseMsg(msgEndGame,pMsg,nLen);
	if (NULL == lpMsg) return 0;


	m_nLandlord = lpMsg->nLandLord;
	//清空赢家的手牌
	IPLAYER* iplayer = GetSelfPlayer();
	if(iplayer == NULL)
	{
		return 1;
	}


	m_lpFramework->SetGameTimer(TIMER_POWER,0);
	m_pClockUI->SetRender(FALSE);

	short nseat = iplayer->getSeat();

	//显示结算面板
	HGUI pEndBack = m_lpFramework->GetCfgGui("GV", "GameEndBack");
	pEndBack->SetStartPos(GetGuiW()/2-UI_W(pEndBack)/2,GetGuiH()/2-UI_H(pEndBack)/2);
	if (pEndBack)  pEndBack->SetRender(TRUE);


	std::string strNickName;
	std::string strKey;
	m_lpFramework->ShowChat(0xFF00C0FF,"【系统提示】游戏结束，本局输赢如下：");
	m_lpFramework->ShowChat(0xFF00C0FF,"------------结算开始----------");
	//播放输赢音效
	if (lpMsg->nWinLost[1] > 0)
	{
		PlayEffect("game.snd_failed");
	}else
	{
		PlayEffect("game.sndWin");
	}
	for (int i =0;i<MAX_PLAYER;i++)
	{
		IPLAYER* pUser = GetPlayerBySeat(i);
		if(pUser)
		{
			strNickName = pUser->getNickName();
		}

		string szScore = CharAddPoint(lpMsg->nWinLost[i]);

		//玩家姓名
		strKey = GetFormatString("EndPlayerName_%d",i);
		uiTextCtrl* lpEndPlayName = (uiTextCtrl*)m_lpFramework->GetCfgGui("",strKey);

		if (lpEndPlayName)
		{
			lpEndPlayName->SetText(strNickName.c_str());
			if (SEAT2LOCAL(i) != SELFLOCALNO && 
				RT_MATCH == m_nRoomType)
			{
				lpEndPlayName->SetText("游戏玩家");
			}
			lpEndPlayName->SetRender(TRUE);
			HGUI lpNameBackEnd = (HGUI)m_lpFramework->GetCfgGui("","BACK_OF_NAME_END");
			if (lpNameBackEnd)  lpEndPlayName->SetCenterHorz(lpNameBackEnd);
		}

		//输赢分数
		strKey = GetFormatString("EndPlayerScore_%d",i);
		uiTextCtrl* lpEndPlayScore = (uiTextCtrl*)m_lpFramework->GetCfgGui("",strKey);
		if (lpEndPlayScore)
		{
			lpEndPlayScore->SetText(szScore.c_str());
			lpEndPlayScore->SetRender(TRUE);
		}

		//输赢标志
		strKey = GetFormatString("EndSignWinLost_%d",i);
		HGUI pSignWinLost = m_lpFramework->GetCfgGui("GV", strKey);
		if (pSignWinLost)
		{
			if (lpMsg->bLandLordWin)  //地主赢了
			{
				if (i == m_nLandlord )
				{
					pSignWinLost->GoAniFrame(0);
					pSignWinLost->SetRender(TRUE);
				}else 
				{
					pSignWinLost->GoAniFrame(1);
					pSignWinLost->SetRender(TRUE);
				}	
			}else   //地主输了
			{
				if (i == m_nLandlord )
				{
					pSignWinLost->GoAniFrame(1);
					pSignWinLost->SetRender(TRUE);
				}else 
				{
					pSignWinLost->GoAniFrame(0);
					pSignWinLost->SetRender(TRUE);
				}	
			}
		}

		int xPosName  = -130;
		int xPosValue = -90;
		int yPos	  = -40;
		//总倍数名
		uiTextCtrl* lpGameTotalMultName = (uiTextCtrl*)m_lpFramework->GetCfgGui("","GameTotalMultName");
		lpGameTotalMultName->ClearText();
		lpGameTotalMultName->SetTextColor(0xFF514027);
		//总倍数
		uiTextCtrl* lpGameTotalMult = (uiTextCtrl*)m_lpFramework->GetCfgGui("","GameTotalMult");
		lpGameTotalMult->ClearText();
		if (lpGameTotalMultName)
		{
			lpGameTotalMultName->SetText("总倍数×");
			lpGameTotalMultName->SetCenter(pEndBack,xPosName,yPos);
			lpGameTotalMultName->SetRender(TRUE);
		}
		if (lpGameTotalMult)
		{
			lpGameTotalMult->Format("%d",lpMsg->nTotalMultiple);
			lpGameTotalMult->SetCenter(pEndBack,xPosValue,yPos);
			yPos+=20;
			lpGameTotalMult->SetRender(TRUE);
		}

		//抢地主倍数名
		uiTextCtrl* lpGameRobMultName = (uiTextCtrl*)m_lpFramework->GetCfgGui("","GameRobMultName");
		lpGameRobMultName->ClearText();
		//抢地主倍数
		uiTextCtrl* lpGameRobMultt = (uiTextCtrl*)m_lpFramework->GetCfgGui("","GameRobMult");
		lpGameRobMultt->ClearText();
		lpGameRobMultt->SetTextColor(0xFF514027);
		if (lpMsg->nGrabCount>0 || lpMsg->nCallScore>0)
		{
			if (lpGameRobMultName)
			{
				if (GLT_GRAB == m_nGrabLandType)
					lpGameRobMultName->SetText("抢地主×");
				else if (GLT_SCORE == m_nGrabLandType)
					lpGameRobMultName->SetText("叫分×");
				lpGameRobMultName->SetCenter(pEndBack,xPosName,yPos);
				lpGameRobMultName->SetRender(TRUE);
			}
			if (lpGameRobMultt)
			{
				int GrabMult = 1;
				if (GLT_GRAB == m_nGrabLandType)
				{
					for (int i=0; i<lpMsg->nGrabCount;i++)
					{
						GrabMult *= 2;
					}
				}else if (GLT_SCORE == m_nGrabLandType)
				{
					GrabMult= lpMsg->nCallScore;
				}
				lpGameRobMultt->Format("%d",GrabMult);
				lpGameRobMultt->SetCenter(pEndBack,xPosValue,yPos);
				yPos+=20;
				lpGameRobMultt->SetRender(TRUE);
			}
		}

		//炸弹倍数名
		uiTextCtrl* lpGameBombMultName = (uiTextCtrl*)m_lpFramework->GetCfgGui("","GameBombMultName");
		lpGameBombMultName->ClearText();
		lpGameBombMultName->SetTextColor(0xFF514027);
		//炸弹倍数
		uiTextCtrl* lpGameBombMult = (uiTextCtrl*)m_lpFramework->GetCfgGui("","GameBombMult");
		lpGameBombMult->ClearText();
		lpGameBombMult->SetTextColor(0xFF514027);
		if (lpMsg->nCountBoom>0)
		{
			if (lpGameBombMultName)
			{
				lpGameBombMultName->SetText("炸弹×");
				lpGameBombMultName->SetCenter(pEndBack,xPosName,yPos);
				lpGameBombMultName->SetRender(TRUE);
			}
			if (lpGameBombMult)
			{
				int BoomMult = 1;
				for (int i=0; i<lpMsg->nCountBoom;i++)
				{
					BoomMult *= 2;
				}
				lpGameBombMult->Format("%d",BoomMult);
				lpGameBombMult->SetCenter(pEndBack,xPosValue,yPos);
				//yPos>0?yPos-=20:yPos+=20;
				yPos+=20;
				lpGameBombMult->SetRender(TRUE);
			}
		}

		//春天名
		uiTextCtrl* lpGameChunTianMultName = (uiTextCtrl*)m_lpFramework->GetCfgGui("","GameChunTianMultName");
		lpGameChunTianMultName->ClearText();
		lpGameChunTianMultName->SetTextColor(0xFF514027);
		//春天
		uiTextCtrl* lpGameChunTianMult = (uiTextCtrl*)m_lpFramework->GetCfgGui("","GameChunTianMult");
		lpGameChunTianMult->ClearText();
		lpGameChunTianMult->SetTextColor(0xFF514027);
		if (lpMsg->bIsNoOutCard)
		{
			if (lpGameChunTianMultName)
			{

				lpGameChunTianMultName->SetText("春天×");
				lpGameChunTianMultName->SetCenter(pEndBack,xPosName,yPos);
				lpGameChunTianMultName->SetRender(TRUE);
			}
			if (lpGameChunTianMult)
			{
				lpGameChunTianMult->Format("%d",2);
				lpGameChunTianMult->SetCenter(pEndBack,xPosValue,yPos);
				//yPos>0?yPos-=20:yPos+=20;
				yPos+=20;
				lpGameChunTianMult->SetRender(TRUE);
			}
		}

		//反春天名
		uiTextCtrl* lpGameFanChunTianMultName = (uiTextCtrl*)m_lpFramework->GetCfgGui("","GameFanChunMultName");
		lpGameFanChunTianMultName->ClearText();
		lpGameFanChunTianMultName->SetTextColor(0xFF514027);
		//反春天
		uiTextCtrl* lpGameFanChunMult = (uiTextCtrl*)m_lpFramework->GetCfgGui("","GameFanChunMult");
		lpGameFanChunMult->ClearText();
		lpGameFanChunMult->SetTextColor(0xFF514027);
		if (lpMsg->bFanChunTian)
		{
			if (lpGameFanChunTianMultName)
			{

				lpGameFanChunTianMultName->SetText("反春天×");
				lpGameFanChunTianMultName->SetCenter(pEndBack,xPosName,yPos);
				lpGameFanChunTianMultName->SetRender(TRUE);
			}
			if (lpGameFanChunMult)
			{
				lpGameFanChunMult->Format("%d",2);
				lpGameFanChunMult->SetCenter(pEndBack,xPosValue,yPos);
				//yPos>0?yPos-=20:yPos+=20;
				yPos+=20;
				lpGameFanChunMult->SetRender(TRUE);
			}
		}
		//底牌名
		uiTextCtrl* lpGameSpecBaseMultName = (uiTextCtrl*)m_lpFramework->GetCfgGui("","GameSpecBaseMultName");
		lpGameSpecBaseMultName->ClearText();
		lpGameSpecBaseMultName->SetTextColor(0xFF514027);
		//底牌
		uiTextCtrl* lpGameSpecBaseMult = (uiTextCtrl*)m_lpFramework->GetCfgGui("","GameSpecBaseMult");
		lpGameSpecBaseMult->ClearText();
		lpGameSpecBaseMult->SetTextColor(0xFF514027);
		if (lpMsg->nBaseMultiple > 1)
		{
			if (lpGameSpecBaseMultName)
			{
				lpGameSpecBaseMultName->SetText("底牌×");
				lpGameSpecBaseMultName->SetCenter(pEndBack,xPosName,yPos);
				lpGameSpecBaseMultName->SetRender(TRUE);
			}

			if (lpGameSpecBaseMult)
			{
				lpGameSpecBaseMult->Format("%d",lpMsg->nBaseMultiple);
				lpGameSpecBaseMult->SetCenter(pEndBack,xPosValue,yPos);
				//yPos>0?yPos-=20:yPos+=20;
				yPos+=20;
				lpGameSpecBaseMult->SetRender(TRUE);
			}
		}

		//明牌名
		uiTextCtrl* lpGameOpenMultName = (uiTextCtrl*)m_lpFramework->GetCfgGui("","GameOpenMultName");
		lpGameOpenMultName->ClearText();
		lpGameOpenMultName->SetTextColor(0xFF514027);
		//明牌
		uiTextCtrl* lpGameOpenMult = (uiTextCtrl*)m_lpFramework->GetCfgGui("","GameOpenMult");
		lpGameOpenMult->ClearText();
		lpGameOpenMult->SetTextColor(0xFF514027);
		if (lpMsg->bIsOpenHands)
		{
			if (lpGameOpenMultName)
			{
				lpGameOpenMultName->SetText("明牌×");
				lpGameOpenMultName->SetCenter(pEndBack,xPosName,yPos);
				lpGameOpenMultName->SetRender(TRUE);
			}

			if (lpGameOpenMult)
			{
				lpGameOpenMult->Format("%d",2);
				lpGameOpenMult->SetCenter(pEndBack,xPosValue,yPos);
				//yPos>0?yPos-=20:yPos+=20;
				yPos+=20;
				lpGameOpenMult->SetRender(TRUE);
			}
		}

		//任务完成名
		uiTextCtrl* lpFinishMissionName = (uiTextCtrl*)m_lpFramework->GetCfgGui("","GamsMissionFinishName");
		lpFinishMissionName->ClearText();
		lpFinishMissionName->SetTextColor(0xFF514027);
		//任务完成倍数
		uiTextCtrl* lpFinishMissionMult = (uiTextCtrl*)m_lpFramework->GetCfgGui("","GameFinishMissionMult");
		lpFinishMissionMult->ClearText();
		lpFinishMissionMult->SetTextColor(0xFF514027);
		if (lpMsg->bFinishMission)
		{
			if (lpFinishMissionName)
			{
				lpFinishMissionName->SetText("任务完成×");
				lpFinishMissionName->SetCenter(pEndBack,xPosName,yPos);
				lpFinishMissionName->SetRender(TRUE);
			}

			if (lpFinishMissionMult)
			{
				lpFinishMissionMult->Format("%d",2);
				lpFinishMissionMult->SetCenter(pEndBack,xPosValue,yPos);
				//yPos>0?yPos-=20:yPos+=20;
				yPos+=20;
				lpFinishMissionMult->SetRender(TRUE);
			}
		}
		if (SEAT2LOCAL(i) != SELFLOCALNO && 
			RT_MATCH == m_nRoomType)
		{
			m_lpFramework->ShowChat(0xFF00C0FF,"游戏玩家");
		}else
		{
			m_lpFramework->ShowChat(0xFF00C0FF,GetFormatString("%s", strNickName.c_str()));
		}
		if (lpMsg->nGameType == GT_SCORE)
		{
			if (lpMsg->nWinLost[i] >= 0)
			{
				m_lpFramework->ShowChat(0xFF00C0FF,GetFormatString("&nbsp&nbsp积分：+%I64d  经验：+%ld", lpMsg->nWinLost[i],lpMsg->nExp[i]));
			}
			else
			{
				m_lpFramework->ShowChat(0xFF00C0FF,GetFormatString("&nbsp&nbsp积分：%I64d  经验：+%ld", lpMsg->nWinLost[i],lpMsg->nExp[i]));
			}
		}else if (lpMsg->nGameType == GT_RICH)
		{
			if (lpMsg->nWinLost[i] >= 0)
			{
				m_lpFramework->ShowChat(0xFF00C0FF,GetFormatString("&nbsp&nbsp皇后币：+%I64d  经验：+%ld", lpMsg->nWinLost[i],lpMsg->nExp[i]));
			}
			else
			{
				m_lpFramework->ShowChat(0xFF00C0FF,GetFormatString("&nbsp&nbsp皇后币：%I64d  经验：+%ld", lpMsg->nWinLost[i],lpMsg->nExp[i]));
			}
		}
	}
	m_lpFramework->ShowChat(0xFF00C0FF,"------------结算结束----------");	

	if (RT_NORMAL == m_nRoomType)
	{
		uiButton* lpBtnCT = (uiButton*) m_lpFramework->GetCfgGui( "", "KW_CHANGESEAT_TABLE" );	
		if (lpBtnCT)	
		{
			lpBtnCT->SetRender(TRUE);	
		}
	}
	ArrangeButtonPosition(GetGuiW(),GetGuiH());
	return 1;
}

void uiGameArea::SendGrabLandlord(int nType)
{
	msgGrabLandlord msgGL;
	InitMsg(msgGL);
	msgGL.thePlayerOperator = nType;
	msgGL.thePlayerSeat = LOCAL2SEAT(m_nLocalPowerSeat); 
	msgGL.nScore = m_nCallScore;
	SendGamePacket(msgGL);
}

int uiGameArea::OnMsgGrab(LPCSTR pMsg, int nLen)
{
	msgGrabLandlord* lpMsg = ParseMsg(msgGrabLandlord,pMsg,nLen);
	if (NULL == lpMsg) return 0;
	int nSeat = SEAT2LOCAL(lpMsg->thePlayerSeat);
	int nOper = lpMsg->thePlayerOperator;
	m_nCallScore = lpMsg->nScore;
	ShowSignGrab(nSeat,nOper,TRUE);
	//音效
	switch(lpMsg->thePlayerOperator)
	{
	case msgGrabLandlord::NOTCALL:
		PlayGameEffct("bujiao");
		PlayGameEffct("nonescore");
		break;	

	case msgGrabLandlord::NOTGRAB:
		PlayGameEffct("notgrab");
		break;

	case msgGrabLandlord::CALL:
		PlayGameEffct("jiaodizhu");
		break;

	case msgGrabLandlord::GRAB:
		PlayGameEffct("grab");
		break;
	case msgGrabLandlord::CALLSCORE:
		//叫分
		break;
	default:
		break;
	}
	if (nSeat == SELFLOCALNO)
	{
		ShowGrabBtn(true,false);
		ShowGrabBtn(false,false);
	}
	return 1;
}

int uiGameArea::OnMsgOpenCard(LPCSTR pMsg, int nLen)
{
	msgOpenCard* lpMsg = ParseMsg(msgOpenCard,pMsg,nLen);
	if (lpMsg == NULL)
	{
		ShowTalk(0xFF00FF00,"包解析错误，请截图联系客服--OnMsgOpenCard");
		return -1;
	}
	m_pHandCards[1]->ClearCheckedCards();
	ShowAllPlayBtn(TRUE,FALSE,FALSE,FALSE);
	uiButton* pOpenCard = (uiButton*)m_lpFramework->GetCfgGui("","KW_BTN_OPEN_CARD");
	if (pOpenCard)
	{
		pOpenCard->SetRender(FALSE);
	}
	return 0;
}

//向服务端发送打出的牌的数据
void uiGameArea::SendOutCard(int nType)
{
	msgOutCard msgOC;
	InitMsg(msgOC);
	msgOC.nCount= ConvertDataVector(m_dvOutCards,msgOC.cards,m_dvOutCards.size());
	msgOC.nType = nType;
	msgOC.nSeat = GetMySeatNo();
	SendGamePacket(msgOC);
}

//玩家身份
int uiGameArea::OnMsgIdentity(LPCSTR pMsg, int nLen)
{
	msgPlayerIdentity* lpMsg = ParseMsg(msgPlayerIdentity,pMsg,nLen);
	if (NULL == lpMsg ) return 0;
	if (lpMsg->bIsLandlord)
	{
		m_nLandlord = lpMsg->nSeat;
	}
	ShowSignLandlord(TRUE);
	return 1;
}

int uiGameArea::OnMsgTrush(LPCSTR pMsg, int nLen)
{
	msgTrust* msgT = ParseMsg(msgTrust,pMsg,nLen);
	if (NULL == msgT)
	{
		ShowTalk(0xFF00FF00,"Trush消息解析错误，请截图联系客服!");
		return 0;
	}
	if (msgT->nType == msgTrust::T_BEGIN)
	{
		ShowBtnTrush(false);
	}else if (msgT->nType == msgTrust::T_END)
	{
		ShowBtnTrush();
	}
	return 1;
}
//出牌
int uiGameArea::OnMsgOutCards(LPCSTR pMsg, int nLen)
{
	msgOutCard*  msgOC = ParseMsg(msgOutCard,pMsg,nLen);
	if(msgOC==NULL)
	{
		ShowTalk(0xFF00FF00,"出牌数据错误，请截图联系客服");
	}
	int nSeat = SEAT2LOCAL(msgOC->nSeat);

	ShowAllPlayBtn(FALSE,FALSE,FALSE,FALSE);
	//隐藏不出标识和已经打出的手牌
	if (msgOC->nType == msgOutCard::GIVEUP)				//玩家放弃出牌
	{
		//播放不出音效
		srand(time(0));
		int n = rand();
		int nIndex  = n%2?0:1;
		char buf[20] ;
		sprintf(buf,"pass%d",nIndex);
		PlayGameEffct(buf);
		this->ShowNotOutSign(nSeat,true);
	}else if (msgOC->nType == msgOutCard::OUTCARD)		//玩家出牌
	{
		m_dvFrontOutCards.clear();
		for (int i=0;i<msgOC->nCount;i++)
		{
			TCards tmp(msgOC->cards[i]);
			m_dvFrontOutCards.push_back(tmp);
		}
		ShowOutCard(nSeat);
		CardTypeInfo cti = GameRule::GetCardType(m_dvFrontOutCards);
		if (cti.nType == CT_NONE) return -1;
		PlayOutCardEffec(cti,m_dvFrontOutCards);
	}else
	{
		return -1;
	}
	return 1;
}
