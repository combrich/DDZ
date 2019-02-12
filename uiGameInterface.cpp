#include "uiGameArea.h"
#include "shellapi.h"
#include "direct.h"

#include <sstream>


//对手牌进行重排序的排序依据
bool cmp_min_max_(TCARD card_1, TCARD card_2)
{
	TCards card1(card_1);
	TCards card2(card_2);
	int value1 = GameRule::CardWeight(card1);
	int value2 = GameRule::CardWeight(card2);
	return value1 < value2;
}

int uiGameArea::OnReceivePacket( WORD nXYID, LPCSTR lpszPacket, WORD nSize )
{
	//PushGamePacket(nXYID,lpszPacket,nSize);
	//CCCardLogicSwitch::OnReceivePacket( nXYID, lpszPacket, nSize );
	//如果播放动画
	if(IsPlayAni() || nXYID == GLXY_CARDS || nXYID == GLXY_TAKE)
	{
		PushGamePacket(nXYID,lpszPacket,nSize);
	}
	else
	{
		OnGameLogicMsg(nXYID,lpszPacket,nSize);
	}
//	OnGameLogicMsg(nXYID,lpszPacket,nSize);
	return 0;
}

int uiGameArea::OnReplayPacket(WORD nXYID, LPCSTR lpszPacket, WORD nSize, CONST ReplayStatus& _RS)
{
	return 0;
}

int uiGameArea::OnSendChat( DWORD dwColor, LPCSTR lpszChat )
{
	return 0;
}

int uiGameArea::OnShowChat( DWORD dwColor, LPCSTR lpszChat )
{
	return 0;
}

int uiGameArea::OnPlayerFilter(DWORD dwFlags, WPARAM wParam, LPARAM lParam, IPLAYER* lpPlayer)
{
	if(dwFlags == PFID_SEAT)
	{
		if(lpPlayer->getSeat() == wParam && lpPlayer->getState() != US_SEEING)
		{
			return 0;
		}
	}

	return -1;
}

int uiGameArea::OnPlayerEnter( TUSERID nUserID, BOOL IsContinue )
{
	IPLAYER* iplayer;
	if (!m_lpFramework->GetPlayerData(nUserID, iplayer))
		return 0;

	if (iplayer->getState() == US_SEEING)
	{
		HGUI pGuiSpeak = m_lpFramework->GetCfgGui("","KW_BTN_SPEAK");
		if (pGuiSpeak)
		{
			pGuiSpeak->SetRender(FALSE); 
		}
		IPLAYER* pl = GetSelfPlayer();
		if (pl->getSID() == iplayer->getSID())
		{
			m_bIAmSeeing = true;
		}else
		{
			m_bIAmSeeing = false;
		}
		this->OnSize(GetGuiW(),GetGuiH());
	}

	int nSeat = iplayer->getSeat();
	SHORT nLocal = SEAT2LOCAL(nSeat);

	uiPlayerFace* lpFace = (uiPlayerFace*)GetGUIHandle("Face_", iplayer->getSeat());
	if (lpFace && iplayer->getState() != US_SEEING)
	{
		m_pHeadPortrait[nLocal]->SetHeadSex('1' == iplayer->getSex());
		m_pHeadPortrait[nLocal]->SetRender(TRUE);

		m_lpFramework->ReqPlayerFaceFrame(nUserID, FT_NORMAL );
		lpFace->SetRender(FALSE);
	}

	HGUI hGui     = m_lpFramework->GetCfgGui("","KW_BTN_START_GAME");
	if (hGui && nLocal == SELFLOCALNO) hGui->SetRender(FALSE);

	
	uiTextCtrl* lpTextName =  (uiTextCtrl*)GetGUIHandle("KW_PLAYER_NAME_", nSeat);
	if (lpTextName)
	{
		lpTextName->SetText(iplayer->getNickName());
		if (RT_MATCH == m_nRoomType && nLocal != SELFLOCALNO)
			lpTextName->SetText("游戏玩家");
	
		HGUI lpNameBack = (GUI*)GetGUIHandle("BACK_OF_NAME_",nSeat);
		if (lpNameBack && lpTextName)
		{
			lpTextName->SetCenter(lpNameBack);
		}
		lpTextName->SetRender(TRUE);
	}

	if (iplayer->getState() != US_SEEING)
	{
		if (iplayer->getSex() == '0')
		{
			m_pHeadPortrait[nLocal]->SetHeadPortrait("..\\..\\skin\\goodsImg\\10.png",true);
		}
		else
		{
			m_pHeadPortrait[nLocal]->SetHeadPortrait("..\\..\\skin\\goodsImg\\9.png",true);
		}
		m_pHeadPortrait[nLocal]->SetRender(TRUE);
	}
	ArrangeLocalPosition(GetGuiW(),GetGuiH(),nLocal);
	return 0;
}

int uiGameArea::OnPlayerData( TUSERID nUserID, DWORD dwFlags )
{
	IPLAYER* iplayer;
	if (!m_lpFramework->GetPlayerData(nUserID, iplayer))
	{
		return 0;
	}

	//显示哪些玩家已经准备
	if(US_SEEING != iplayer->getState())
	{
		SHORT nLocal = SEAT2LOCAL(iplayer->getSeat());
		if(iplayer->GetPlayState() == psReady)
		{
			ShowPlayReadySign(nLocal,TRUE);
		}
	}

	
	return 0;
}

int uiGameArea::OnPlayerStart( TUSERID nUserID, DWORD dwReserved )
{
	IPLAYER* iplayer;
	if (!m_lpFramework->GetPlayerData(nUserID, iplayer))
		return 0;
	if (iplayer->getSeat() == GetMySeatNo() )
	{
		ClearTable();
		Reset();
	}
	m_nLocalSeat[iplayer->getSeat()] = SEAT2LOCAL(iplayer->getSeat());
	return 0;
}

int uiGameArea::OnPlayerLeave( TUSERID nUserID, LPCSTR lpszCause )
{

	IPLAYER* iplayer;
	if (!m_lpFramework->GetPlayerData(nUserID, iplayer))
		return 0;

	std::string szInfo;
	szInfo.clear();
	szInfo += "【系统提示】";
	if (RT_NORMAL == m_nRoomType)
	{
		szInfo = iplayer->getNickName();
		szInfo +=  "离开了";
		m_lpFramework->ShowChat(0xFF00C0FF, szInfo.c_str());
	}else if (RT_MATCH == m_nRoomType)
	{
		//比赛不要提示这个了
	}
	
	
	

	ShowSignLandlord(FALSE);
	//显示换桌按钮
	IPLAYER* lpSelf = GetSelfPlayer();
	if (iplayer->getState() != US_SEEING && RT_NORMAL==m_nRoomType &&
		(m_nGameState == GAME_STATE_PLAY || m_nGameState==GAME_STATE_GRAB))
	{
		uiButton* lpBtnCT = (uiButton*) m_lpFramework->GetCfgGui( "", "KW_CHANGESEAT_TABLE" );	
		if (lpBtnCT)
		{
			lpBtnCT->SetRender(TRUE);	
		}
	}

	//关闭警报动画
	for (int i=0; i<MAX_PLAYER;i++)
	{
		ShowAni("AniWarning_",FALSE,i,0,FALSE);
	}

	if(iplayer->getState() != US_SEEING)
	{
		//隐藏玩家头像及昵称
		SHORT nLocal = SEAT2LOCAL(iplayer->getSeat());
		m_pHeadPortrait[nLocal]->SetRender(FALSE);
		m_pHeadPortrait[nLocal]->SetHeadPortrait("");
		
		uiTextCtrl* lpTextName =  (uiTextCtrl*)GetGUIHandle("KW_PLAYER_NAME_", iplayer->getSeat());
		if (lpTextName)
		{
			lpTextName->ClearText();
			lpTextName->SetRender(FALSE);
		}
		for (int i=0; i<MAX_PLAYER;i++)
		{
			ShowAni("AniWarning_",FALSE,i,0,FALSE);
		}
		ShowPlayReadySign(nLocal,FALSE);
		uiPlayerFace* lpGui = (uiPlayerFace*)GetGUIHandle("Face_", iplayer->getSeat());
		if (lpGui)
		{
			lpGui->SetRender(FALSE);
		}

		ShowHandCount(0,nLocal, FALSE);
		IPLAYER* iSelf = GetSelfPlayer();
		if(iSelf)
		{
			if(iSelf->getState() != US_SEEING && (m_nGameState != GAME_STATE_NONE && m_nGameState != GAME_STATE_END))
			{
				HGUI pGuiHint = m_lpFramework->GetCfgGui("GV", "MessageBoxHint2");
				if (pGuiHint != NULL)
				{
					pGuiHint->SetRender(TRUE);
					pGuiHint->BringToTop();
				}
			}
			else if(iSelf->getState() == US_SEEING)
			{
				if(iSelf->getSeat() == iplayer->getSeat())
				{
					ShowTalkEx("【系统提示】您旁观的玩家已经离开,游戏窗口即将关闭.");
					m_lpFramework->GameClientClose(10);
				}
			}
		}	
	}
	return 0;
}

int uiGameArea::OnPlayerTimer( TUSERID nUserID, DWORD dwPower, UINT nElapse )
{
	IPLAYER* iplayer;
	if (!m_lpFramework->GetPlayerData(nUserID, iplayer))
	{
		return 0;
	}
	return 0;
}

int uiGameArea::OnGameTimer( UINT_PTR nIDEvent, UINT nElapse )
{
	UINT nRemainTime = nElapse;
	if (TIMER_POWER == nIDEvent)
	{
		m_pClockUI->SetClock(nRemainTime);
		if (nRemainTime == 0)
		{
			m_pClockUI->SetRender(FALSE);
		}
	}
	 if ( nRemainTime == 0 )
	{
		if (TIMER_SPEAK_STOP == nIDEvent)
		{
			m_bSpeakPlay = false;
		}
		else if (TIMER_SPEAK_DIALOG_L == nIDEvent)
		{
			GUI* pGuiDialog = (GUI*)m_lpFramework->GetCfgGui("",GetFormatString("KW_DIALOG_%d",0));
			if (pGuiDialog) pGuiDialog->SetRender(FALSE);
		}
		else if (TIMER_SPEAK_DIALOG_D == nIDEvent)
		{
			GUI* pGuiDialog = (GUI*)m_lpFramework->GetCfgGui("",GetFormatString("KW_DIALOG_%d",1));
			if (pGuiDialog) pGuiDialog->SetRender(FALSE);
		}
		else if (TIMER_SPEAK_DIALOG_R == nIDEvent)
		{
			GUI* pGuiDialog = (GUI*)m_lpFramework->GetCfgGui("",GetFormatString("KW_DIALOG_%d",2));
			if (pGuiDialog) pGuiDialog->SetRender(FALSE);
		}
	}
	return 0;
}

int uiGameArea::OnWindowProcEx(UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT  uiGameArea::OnWindowDestroyEx()
{
	
	return 0;
}


BOOL uiGameArea::GetVideoParam(TUSERID nUserID, TVideoParam& tVideoParam)
{
	return FALSE;
	
	IPLAYER* pPlayer;
	if(m_lpFramework->GetPlayerData(nUserID,pPlayer))
	{
		LONG nGuiW = UI_W(this);
		LONG nGuiH = UI_H(this);
		SHORT nLocal = SEAT2LOCAL(pPlayer->getSeat());

		/*if(GetWindowSizeType() == WNDSZ_BIG)
		{*/
			if(LP_LEFT == nLocal)
			{
				tVideoParam.ptWorldPos.x = GetWorldPosX() + 6; 
				tVideoParam.ptWorldPos.y = GetWorldPosY() + 6;
				tVideoParam.bRender = TRUE;
				tVideoParam.nSizeType = VIDEOSZ_BIG;
			}
			else if( LP_DOWN == nLocal)
			{
				tVideoParam.ptWorldPos.x = GetWorldPosX() + nGuiW + 20; 
				tVideoParam.ptWorldPos.y = GetWorldPosY() + 6;
				tVideoParam.bRender = TRUE;
				tVideoParam.nSizeType = VIDEOSZ_BIG;
			}
			else if(LP_RIGHT == nLocal)
			{	
				tVideoParam.ptWorldPos.x = GetWorldPosX() + nGuiW - 6 - 180; 
				tVideoParam.ptWorldPos.y = GetWorldPosY() + 6;
				tVideoParam.bRender = TRUE;
				tVideoParam.nSizeType = VIDEOSZ_BIG;
			}
			else if(LP_UP == nLocal)
			{
				tVideoParam.ptWorldPos.x = GetWorldPosX() + (nGuiW - 180)/2; 
				tVideoParam.ptWorldPos.y = GetWorldPosY() + 6;
				tVideoParam.bRender = TRUE;
				tVideoParam.nSizeType = VIDEOSZ_BIG;
			}
		/*}
		else
		{
			if(LP_LEFT == nLocal)
			{
				tVideoParam.ptWorldPos.x = GetWorldPosX() + 6; 
				tVideoParam.ptWorldPos.y = GetWorldPosY() + 6;
				tVideoParam.bRender = TRUE;
				tVideoParam.nSizeType = VIDEOSZ_MIDDLE;
			}
			else if( LP_DOWN == nLocal)
			{
				tVideoParam.ptWorldPos.x = GetWorldPosX() + nGuiW + 40; 
				tVideoParam.ptWorldPos.y = GetWorldPosY() + 6;
				tVideoParam.bRender = TRUE;
				tVideoParam.nSizeType = VIDEOSZ_MIDDLE;
			}
			else if(LP_RIGHT == nLocal)
			{	
				tVideoParam.ptWorldPos.x = GetWorldPosX() + nGuiW - 6 - 140; 
				tVideoParam.ptWorldPos.y = GetWorldPosY() + 6;
				tVideoParam.bRender = TRUE;
				tVideoParam.nSizeType = VIDEOSZ_MIDDLE;
			}
			else if(LP_UP == nLocal)
			{
				tVideoParam.ptWorldPos.x = GetWorldPosX() + (nGuiW - 140)/2; 
				tVideoParam.ptWorldPos.y = GetWorldPosY() + 6;
				tVideoParam.bRender = TRUE;
				tVideoParam.nSizeType = VIDEOSZ_MIDDLE;
			}
		}*/

		return TRUE;
	}	

	return FALSE;
}

int uiGameArea::OnVideoPanelMessage(UINT iMsg, TUSERID nUserID)
{
	return 0;
}

// 使用道具提示
int uiGameArea::OnHintUseProps(DWORD nFromBrandID, DWORD nFromNumberID,DWORD nToBrandID, DWORD nToNumberID, int nPropsID, int nCount,char szParam[])
{
	TUSERID nToUserID = MAKEUSERID(nToNumberID);

	IPLAYER* pToPlayer;
	if(m_lpFramework->GetPlayerData(nToUserID,pToPlayer))
	{
		if(nPropsID == 50)
		{
			
		}
		else if(nPropsID == 51)
		{
			
		}

		TUSERID nFromUserID = MAKEUSERID(nFromNumberID);

		IPLAYER* pFromPlayer;
		if(m_lpFramework->GetPlayerData(nFromUserID,pFromPlayer))
		{
			if(nPropsID == 50)
			{
				ShowTalk(0xFFFFFFFF,GetFormatString("%s(%d)送%s(%d)玫瑰%d朵",pFromPlayer->getNickName(), nFromNumberID, pToPlayer->getNickName(), nToNumberID, nCount));
			}
			else if(nPropsID == 51)
			{
				ShowTalk(0xFFFFFFFF,GetFormatString("%s(%d)送%s(%d)钻石%d颗",pFromPlayer->getNickName(), nFromNumberID, pToPlayer->getNickName(), nToNumberID, nCount));
			}
		}
		else
		{
			if(nPropsID == 50)
			{
				ShowTalk(0xFFFFFFFF,GetFormatString("玩家(%d)送%s(%d)玫瑰%d朵", nFromNumberID, pToPlayer->getNickName(), nToNumberID, nCount));
			}
			else if(nPropsID == 51)
			{
				ShowTalk(0xFFFFFFFF,GetFormatString("玩家(%d)送%s(%d)钻石%d颗", nFromNumberID, pToPlayer->getNickName(), nToNumberID, nCount));
			}
		}	
	}
	
	return 0;
}

int uiGameArea::OnRptAttUpdate(DWORD nBrandID, DWORD nNumberID, int nCharmAdd, int nContributionAdd, BYTE nAttSize,int nAtt[10])
{
	if(nCharmAdd <= 0 && nContributionAdd <= 0)
	{
		return 0;
	}

	TUSERID nUserID = MAKEUSERID(nNumberID);

	IPLAYER* pPlayer;
	if(m_lpFramework->GetPlayerData(nUserID, pPlayer))
	{
		std::string strChat = pPlayer->getNickName();

		if(nCharmAdd > 0)
		{
			strChat += GetFormatString("魅力增加%d",nCharmAdd);

			if(nContributionAdd > 0)
			{
				strChat += ",";
			}
		}

		if(nContributionAdd > 0)
		{
			strChat += GetFormatString("贡献增加%d",nContributionAdd);
		}

		ShowTalk(0xFFFFFFFF,strChat.c_str());	
	}

	return 0;
}

BOOL uiGameArea::GetClientVersion(DWORD& dwVer)
{
	dwVer = CLIENT_VERSION;

	return TRUE;
}

int uiGameArea::OnUpdateCfgFramework()
{
	return 0;
}

int uiGameArea::GetChatToText(TUSERID nToUserID, string& strText)
{
	return 0;
}

// 改变聊天窗口颜色
DWORD uiGameArea::GetFrameworkChatColor()
{
	//return 0xFFFF00FF;
	return 0;
}

int uiGameArea::OnWillSendPlayerChat( DWORD& dwColor, string& strChatText )
{
	dwColor = 0xFFFFEFB4;
	return 0;
}

// 窗口创建完毕通知
int uiGameArea::OnFrameworkCreated( )
{
	// 加载配置文件
	LoadConfig();
	SwitchEffect(m_tSetUpParam.IsSnd);
	m_tSetUpParam.IsWndTop = m_IsTop;
	SetWndTop(m_tSetUpParam.IsWndTop);
	uiPushButton* pGameAni = (uiPushButton*)m_lpFramework->GetCfgGui("GV", "PushBtnAni"); 
	uiPushButton* pSound = (uiPushButton*)m_lpFramework->GetCfgGui("GV", "PushBtnSound"); 
	if (pGameAni && pSound)
	{
		if (m_tSetUpParam.IsSnd)
		{
			pSound->SetCheck(1);
		}
		if (m_tSetUpParam.IsAni)
		{
			pGameAni->SetCheck(1);

			if (m_hGameSound == 0)
			{
				m_tSetUpParam.IsSnd = 1;
				m_lpFramework->SwitchEffect(m_tSetUpParam.IsSnd);
				SwitchEffect(m_tSetUpParam.IsSnd);
				m_hGameSound = PlayEffect("game.gamesound", 100, 0, 1.0, TRUE); 
			}
			if (pSound->GetCheck() == 0)
			{
				m_tSetUpParam.IsSnd = 0;
				m_lpFramework->SwitchEffect(m_tSetUpParam.IsSnd);
				SwitchEffect(m_tSetUpParam.IsSnd);
			}
		}
	}

	if (m_IsScreen)	m_lpFramework->SendNotifyMessage(CMD_BUTTON,0,0,"Button", 504,"KW_TITLE_BAR_BTN_MAXIMIZE");
	return 0;
}

//微博操作:返回好友最新推特信息
int uiGameArea::OnRespGetUserRecent( TUSERID nUserID, const string& strContent, const string& strImagePath )
{
	IPLAYER* pPlayer;
	m_lpFramework->GetPlayerData(nUserID, pPlayer);
	uiPlayerFace* lpGui = (uiPlayerFace*)GetGUIHandle("Face_", pPlayer->getSeat());
	if (lpGui)
	{
		lpGui->SetBlogText(strContent.c_str());
	}
	return 0;
}

// 收到玩家头像更新（USERID, 头像尺寸类型, 头像图片路径 ）
int uiGameArea::OnRespPlayerFaceFrame( TUSERID nUserID, int nType, const string& strPath )
{
	if ( FT_NORMAL == nType )
	{
		IPLAYER* pPlayer;
		if( m_lpFramework->GetPlayerData(nUserID,pPlayer) )
		{
			uiPlayerFace* lpGui = (uiPlayerFace*)GetGUIHandle("Face_", pPlayer->getSeat());
			if (lpGui)
			{
				lpGui->SetFace(strPath.c_str());
			}
		}
	}
	return 0;
}

// 好友操作回应
int uiGameArea::OnRespFriendOp( BYTE nFlag, int nOp, TUSERID nUserID, const string& strUserName, int nRel, const string& strCaption, const string& strText, const string& strWeb )
{
	if (nFlag == 0)		// 好友操作成功
	{
		IPLAYER* iPlayer = NULL;
		if (!m_lpFramework->GetPlayerData(nUserID, iPlayer))
		{
			return 0;
		}
		HGUI lpGui = GetGUIHandle("SignFriend_", iPlayer->getSeat());
		HGUI lpGuiBtn = GetGUIHandle("BtnFriend_", iPlayer->getSeat());
		if (!lpGui || !lpGuiBtn) return 0;
		if (iPlayer->GetRel() == FRIENDREL_NORMAL)
		{
			lpGui->SetRender(TRUE);
			lpGui->GoAniFrame(0);
			lpGuiBtn->SetRender(FALSE);
		}
		else if (iPlayer->GetRel() == FRIENDREL_STRANGE)
		{
			lpGui->SetRender(FALSE);
			lpGuiBtn->SetRender(TRUE);		
		}
		else if (iPlayer->GetRel() == FRIENDREL_BLACK)
		{
			lpGui->SetRender(TRUE);
			lpGui->GoAniFrame(1);
			lpGuiBtn->SetRender(FALSE);		
		}
		else if (iPlayer->GetRel() == FRIENDREL_INVITE)
		{
			lpGuiBtn->SetRender(FALSE);	
		}
	}
	else				// 好友操作失败
	{
		
	}
	return 0;
}


//可否出牌
bool uiGameArea::CanOut(bool bIsFirst /*=false*/)
{
	TCARDS oCards;
	oCards.clear();
	int nCount = m_pHandCards[SELFLOCALNO]->GetCheckedCards(oCards);
	if (oCards.empty())
	{
		return false;
	}
	m_dvOutCards.clear();
	m_dvOutCards = ConvertCardsToCardVector(oCards);
	return GameRule::CanOut(bIsFirst,m_dvFrontOutCards,m_dvOutCards);
}


//////////////////////////////////////////////////////////////////////////
//职能：查找手牌中大于上家出牌的牌组，并将找到的牌组设置为选中状态
//参数：citFrontOut为上家出牌的牌组信息
//返回：如果成功找到满足条件的牌组，返回true否则返回false
bool uiGameArea::GetGreaterThanNextOut(CardTypeInfo ctiFrontOut)
{
	if (ctiFrontOut.nType ==  CT_MAXBOOM)
	{
		return false;
	}
start:
	//临时牌组类型信息
	CardTypeInfo ctiTmp;
	ctiTmp.reset();
	//手牌的数量小于上家出牌数量
	bool bRet = false;
	//将玩家手牌中拥有的所有基本牌型分类装存
	FillBaseCards();
	switch(ctiFrontOut.nType)
	{
	case CT_SINGLE:
		bRet = HaveOutCard(m_PromptSingle.size()-1,ctiFrontOut,m_PromptSingle);
		break;

	case CT_DOUBLE:
		bRet = HaveOutCard(m_PromptDouble.size()-2,ctiFrontOut,m_PromptDouble);
		break;

	case CT_THREE:
		bRet = HaveOutCard(m_PromptThree.size()-3,ctiFrontOut,m_PromptThree);
		break;

	case CT_BOOM:
		bRet = HaveOutCard(m_PromptBoom.size()-4,ctiFrontOut,m_PromptBoom);
		break;

	case CT_THREEWITHTWO:			//三带二
		bRet = HaveOutCard(m_PromptTWT.size()-5,ctiFrontOut,m_PromptTWT,5);
		break;

	case CT_THREEWITHONE:			//三带一
		bRet = HaveOutCard(m_PromptTWO.size()-4,ctiFrontOut,m_PromptTWO,4);
		break;

	case CT_MULTI:				//单顺
		sort(m_PromptSingle.begin(),m_PromptSingle.end(),cmp_min_max_);
		bRet = HaveOutCard(m_PromptSingle.size()-ctiFrontOut.nCount, ctiFrontOut,m_PromptSingle);
		break;

	case CT_MULTIDOUBLETWO:		//双顺
		sort(m_PromptDouble.begin(),m_PromptDouble.end(),cmp_min_max_);
		bRet = HaveOutCard(m_PromptDouble.size()-ctiFrontOut.nCount, ctiFrontOut,m_PromptDouble);
		break;

	case CT_MULTITHREE:		//三顺
		sort(m_PromptThree.begin(),m_PromptThree.end(),cmp_min_max_);
		bRet = HaveOutCard(m_PromptThree.size()-ctiFrontOut.nCount, ctiFrontOut,m_PromptThree);
		break;

	case CT_MULTITHREEWITHONE:	//小飞机
		bRet = HaveOutCardEx(m_PromptTWO.size()-ctiFrontOut.nCount,ctiFrontOut,m_PromptTWO);
		break;

	case CT_MULTITHREEWITHTWO:   //大飞机
		bRet = HaveOutCardEx(m_PromptTWT.size()-ctiFrontOut.nCount,ctiFrontOut,m_PromptTWT);
		break;

	case CT_FOURWITHTWO:		//四带二(AAAA24)
		bRet = HaveOutCard(m_PromptFWTO.size()-6,ctiFrontOut,m_PromptFWTO,6);
		break;

	case CT_FOURWITHDTWO:		//四带二(AAAA4477)
		bRet = HaveOutCard(m_PromptFWDT.size()-8,ctiFrontOut,m_PromptFWDT,8);
		break;
	default: 
		break;
	}

	//没有对应的牌型，最后试试有没有炸弹 
	if (!bRet && ctiFrontOut.nType != CT_BOOM)
	{
		ctiTmp.nType	= CT_BOOM;
		ctiTmp.nCount	= 4;
		ctiTmp.nLevel   = 4;
		bRet = HaveOutCardBoom(m_PromptBoom.size()-4, ctiTmp,m_PromptBoom);
	}
	if (!bRet && ctiFrontOut.nType != CT_MAXBOOM)
	{
		ctiTmp.nType	= CT_MAXBOOM;
		ctiTmp.nCount	= 2;
		ctiTmp.nLevel   = 1;
		bRet = HaveOutCardMaxBoom(m_PromptSingle.size()-2, ctiTmp,m_PromptSingle);
	}
	//此处处理玩家手牌中有牌符合要求，
	//但是牌无法满足正常循环提示
	if (!bRet && (m_nCountPrompt > 0 || m_nCountBoomPrompt > 0 || m_nCountMaxBoomPrompt > 0))
	{
		m_nCountBoomPrompt = 0;
		m_nCountPrompt = 0;
		m_nCountMaxBoomPrompt = 0;
		goto start;
	}
	return bRet;
}


//获取玩家手牌中拥有的基础牌型，基础牌型为单张、对子、三张、炸弹
//并将获取到的基础牌型放入指定的容器中
void uiGameArea::FillBaseCards()
{	
	//预备工作
	m_PromptBoom.clear();
	m_PromptSingle.clear();
	m_PromptDouble.clear();
	m_PromptThree.clear();
	m_PromptTWT.clear();
	m_PromptTWO.clear();
	m_PromptFWTO.clear();
	m_PromptFWDT.clear();
	//获取手牌中所有的基础牌型
	TCARDS tcHands = m_HandCards[SELFLOCALNO];
	sort(tcHands.begin(),tcHands.end(),cmp_min_max_);
	//炸弹
	m_PromptBoom	= GetBaseCards(4,CT_BOOM, tcHands);
	//剔除手牌中的炸弹

	for (int i=0; i<m_PromptBoom.size(); i++)
	{
		tcHands.erase(remove(tcHands.begin(),tcHands.end(),m_PromptBoom.at(i)),tcHands.end());
	}


	//三张
	m_PromptThree	= GetBaseCards(3,CT_THREE,tcHands);
	//剔除手牌中的三张和炸弹
	for (int i=0; i<m_PromptThree.size(); i++)
	{
		tcHands.erase(remove(tcHands.begin(),tcHands.end(),m_PromptThree.at(i)),tcHands.end());
	}

	//两张
	m_PromptDouble  = GetBaseCards(2,CT_DOUBLE,tcHands);
	//剔除手牌中的炸弹，三张，一对
	for (int i=0; i<m_PromptDouble.size(); i++)
	{
		tcHands.erase(remove(tcHands.begin(),tcHands.end(),m_PromptDouble.at(i)),tcHands.end());
	}


	//除去炸弹，三张，一对后的就是单牌了
	m_PromptSingle = tcHands;


	sort(m_PromptSingle.begin(),m_PromptSingle.end(),cmp_min_max_);
	sort(m_PromptDouble.begin(),m_PromptDouble.end(),cmp_min_max_);
	sort(m_PromptThree.begin(),m_PromptThree.end(),cmp_min_max_);

	//获取完所有基本牌型过后，单牌可由对子、三张、炸弹拆成
	//对子可由炸弹、三张拆解成，所以应当在结尾添加相应拆开的牌
	//拆牌的时候需要注意：炸弹拆三张可拆2个相同的
	TCARDS tmpThree;
	TCARDS tmpDouble;
	tmpDouble.clear();
	tmpThree.clear();
	//三张
	for (int i =0; i<m_PromptBoom.size(); i+=4)
	{
		tmpThree.push_back(m_PromptBoom.at(i));
		tmpThree.push_back(m_PromptBoom.at(i+1));
		tmpThree.push_back(m_PromptBoom.at(i+2));
	}

	//一对
	for (int i=0; i<m_PromptThree.size(); i+=3)
	{
		tmpDouble.push_back(m_PromptThree.at(i));
		tmpDouble.push_back(m_PromptThree.at(i+1));
	}
	for (int i=0 ;i<m_PromptBoom.size();i+=4)
	{
		tmpDouble.push_back(m_PromptBoom.at(i));
		tmpDouble.push_back(m_PromptBoom.at(i+1));
	}
	//单张
	for (int i=0; i<m_PromptDouble.size(); i+=2)
	{
		m_PromptSingle.push_back(m_PromptDouble.at(i));
	}
	for (int i=0; i<m_PromptThree.size();i+=3)
	{
		m_PromptSingle.push_back(m_PromptThree.at(i));
	}
	for (int i=0; i<m_PromptBoom.size();i+=4)
	{
		m_PromptSingle.push_back(m_PromptBoom.at(i));
	}

	//追加三张
	for (int i=0; i<tmpThree.size();i++)
	{
		m_PromptThree.push_back(tmpThree.at(i));
	}
	//追加一对
	for (int i=0; i<tmpDouble.size();i++)
	{
		m_PromptDouble.push_back(tmpDouble.at(i));
	}
	//三带二需需要基本牌型获取完毕后才可获取
	//将手牌中所有可能的三带二组合放入tmp向量中
	for (int i=0; i<m_PromptThree.size(); i+=3)
	{
		for (int j=0; j<m_PromptDouble.size(); j+=2)
		{	
			if (m_PromptDouble.at(j) == m_PromptThree.at(i))
			{
				continue;
			}
			//三张
			m_PromptTWT.push_back(m_PromptThree.at(i));
			m_PromptTWT.push_back(m_PromptThree.at(i+1));
			m_PromptTWT.push_back(m_PromptThree.at(i+2));
			//一对
			m_PromptTWT.push_back(m_PromptDouble.at(j));
			m_PromptTWT.push_back(m_PromptDouble.at(j+1));
		}
	}

	//三带一需需要基本牌型获取完毕后才可获取
	//将手牌中所有可能的三带一组合放入向量中
	for (int i=0; i<m_PromptThree.size(); i+=3)
	{
		for (int j=0; j<m_PromptSingle.size(); j++)
		{	
			if (m_PromptSingle.at(j) == m_PromptThree.at(i))
			{
				continue;
			}
			//三张
			m_PromptTWO.push_back(m_PromptThree.at(i));
			m_PromptTWO.push_back(m_PromptThree.at(i+1));
			m_PromptTWO.push_back(m_PromptThree.at(i+2));
			//一对
			m_PromptTWO.push_back(m_PromptSingle.at(j));
		}
	}

	//四带二需需要基本牌型获取完毕后才可获取
	//将手牌中所有可能的四带二组合放入向量向量中
	for (int i=0; i<m_PromptBoom.size(); i+=4)   //四带二带两张单牌
	{
		for (int j=0; j<m_PromptSingle.size()-1; j++)
		{	
			if (m_PromptSingle.at(j) == m_PromptBoom.at(i)
				||m_PromptSingle.at(j+1) == m_PromptBoom.at(i))
			{
				continue;
			}
			//四张
			m_PromptFWTO.push_back(m_PromptBoom.at(i));
			m_PromptFWTO.push_back(m_PromptBoom.at(i+1));
			m_PromptFWTO.push_back(m_PromptBoom.at(i+2));
			m_PromptFWTO.push_back(m_PromptBoom.at(i+3));
			//一对
			m_PromptFWTO.push_back(m_PromptSingle.at(j));
			m_PromptFWTO.push_back(m_PromptSingle.at(j+1));

		}
	}

	//四带二需需要基本牌型获取完毕后才可获取
	//将手牌中所有可能的四带二组合放入向量向量中
	for (int i=0; i<m_PromptBoom.size(); i+=4) //四带二带两对
	{
		for (int j=0; j<m_PromptDouble.size()-4; j+=2)
		{	
			if (m_PromptDouble.size() < 4)
			{
				break;
			}
			if (m_PromptDouble.at(j) == m_PromptBoom.at(i) 
				|| m_PromptDouble.at(j+2)==m_PromptBoom.at(i))
			{
				continue;
			}
			//四张
			m_PromptFWDT.push_back(m_PromptBoom.at(i));
			m_PromptFWDT.push_back(m_PromptBoom.at(i+1));
			m_PromptFWDT.push_back(m_PromptBoom.at(i+2));
			m_PromptFWDT.push_back(m_PromptBoom.at(i+3));
			//一对
			m_PromptFWDT.push_back(m_PromptDouble.at(j));
			m_PromptFWDT.push_back(m_PromptDouble.at(j+1));
			m_PromptFWDT.push_back(m_PromptDouble.at(j+2));
			m_PromptFWDT.push_back(m_PromptDouble.at(j+3));
		}
	}
}


//判断玩家手牌中是否有可以打出的牌
bool uiGameArea::HaveOutCard(int nCardsLen, CardTypeInfo ctiFrontOut ,TCARDS handCards, int nStep /*=1*/)
{
	TCardVector	 dvMaybeOut;		//可能可以打出的牌
	CardTypeInfo ctiMaybeOut;		//dvMaybeOut牌型信息
	m_PromptCard.clear();
	TCardVector  dvTmpHand = ConvertCardsToCardVector(handCards);


	//处理拥有相同牌型可压牌的情况
	for (int i=m_nCountPrompt; i<=nCardsLen; i+=nStep)  //for1   
	{
		dvMaybeOut.clear();
		for (int j=i; j<ctiFrontOut.nCount+i; j++)  //for2
		{
 			dvMaybeOut.push_back(dvTmpHand.at(j));  
		} //end for2

		
		if (!dvMaybeOut.empty())
		{
			ctiMaybeOut = GameRule::GetCardType(dvMaybeOut);
			if (ctiMaybeOut.nLevel > ctiFrontOut.nLevel 
				&& ctiFrontOut.nType == ctiMaybeOut.nType )
			{
				m_pHandCards[SELFLOCALNO]->SetCheckedCards(ConvertCardVectorToCards(dvMaybeOut));
				m_pHandCards[SELFLOCALNO]->GetCheckedCards(m_PromptCard);
				//循环提示
				m_nCountPrompt = i+nStep;
				return true;
			}
		}
	}  //end for 1

	return false;
}

//判断玩家手牌中是否有可以打出的牌
bool uiGameArea::HaveOutCardBoom(int nCardsLen, CardTypeInfo ctiFrontOut ,TCARDS handCards, int nStep /*=1*/)
{
	TCardVector	 dvMaybeOut;		//可能可以打出的牌
	CardTypeInfo ctiMaybeOut;		//dvMaybeOut牌型信息
	m_PromptCard.clear();
	TCardVector  dvTmpHand = ConvertCardsToCardVector(handCards);

	//处理拥有相同牌型可压牌的情况
	for (int i=m_nCountBoomPrompt; i<=nCardsLen; i+=nStep)  //for1   
	{
		dvMaybeOut.clear();
		for (int j=i; j<ctiFrontOut.nCount+i; j++)  //for2
		{
 			dvMaybeOut.push_back(dvTmpHand.at(j));  
		} //end for2

		
		if (!dvMaybeOut.empty())
		{
			ctiMaybeOut = GameRule::GetCardType(dvMaybeOut);
			if (ctiMaybeOut.nLevel > ctiFrontOut.nLevel 
				&& ctiFrontOut.nType == ctiMaybeOut.nType)
			{
				m_pHandCards[SELFLOCALNO]->SetCheckedCards(ConvertCardVectorToCards(dvMaybeOut));
				m_pHandCards[SELFLOCALNO]->GetCheckedCards(m_PromptCard);
				//循环提示
				m_nCountBoomPrompt = i+nStep;
				return true;
			}

		}
	}  //end for 1

	return false;
}

//判断玩家手牌中是否有可以打出的牌
bool uiGameArea::HaveOutCardMaxBoom(int nCardsLen, CardTypeInfo ctiFrontOut ,TCARDS handCards, int nStep /*=1*/)
{
	TCardVector	 dvMaybeOut;		//可能可以打出的牌
	CardTypeInfo ctiMaybeOut;		//dvMaybeOut牌型信息
	m_PromptCard.clear();
	TCardVector  dvTmpHand = ConvertCardsToCardVector(handCards);


	//处理拥有相同牌型可压牌的情况
	for (int i=m_nCountMaxBoomPrompt; i<=nCardsLen; i+=nStep)  //for1   
	{
		dvMaybeOut.clear();
		for (int j=i; j<ctiFrontOut.nCount+i; j++)  //for2
		{
			dvMaybeOut.push_back(dvTmpHand.at(j));  
		} //end for2


		if (!dvMaybeOut.empty())
		{
			ctiMaybeOut = GameRule::GetCardType(dvMaybeOut);
			if (ctiMaybeOut.nLevel > ctiFrontOut.nLevel 
				&& ctiFrontOut.nType == ctiMaybeOut.nType )
			{
				m_pHandCards[SELFLOCALNO]->SetCheckedCards(ConvertCardVectorToCards(dvMaybeOut));
				m_pHandCards[SELFLOCALNO]->GetCheckedCards(m_PromptCard);
				//循环提示
				m_nCountMaxBoomPrompt = i+nStep;
				return true;
			}

		}
	}  //end for 1

	return false;
}

//用于判定玩家手中是否有可以打出的飞机牌型
bool uiGameArea::HaveOutCardEx(int nCardsLen,CardTypeInfo ctiFrontOut,TCARDS handCards)
{
	//将飞机转换为三顺
	CardTypeInfo ctiTmp = ctiFrontOut;
	ctiTmp.nType  = CT_MULTITHREE;
	if (ctiFrontOut.nType == CT_MULTITHREEWITHONE)
	{
		ctiTmp.nCount = ctiFrontOut.nCount - ctiFrontOut.nCount/4; 
		ctiTmp.nType  = CT_MULTITHREE;
	}else 
	{
		ctiTmp.nCount = ctiFrontOut.nCount - ctiFrontOut.nCount/5*2;
		ctiTmp.nType  = CT_MULTITHREE;
	}
	
	sort(m_PromptThree.begin(),m_PromptThree.end(),cmp_min_max_);
	bool bRet = HaveOutCard(m_PromptThree.size()-ctiTmp.nCount, ctiTmp,m_PromptThree,3);

	//将能够组成小飞机的三顺从手牌中剔除
	TCARDS tmpHandCards = m_HandCards[SELFLOCALNO];
	for (int i=0; i<m_PromptCard.size();i++)
	{
		tmpHandCards.erase(remove(tmpHandCards.begin(),tmpHandCards.end(),m_PromptCard.at(i)),
					       tmpHandCards.end());
	}

	if (bRet && ctiFrontOut.nType == CT_MULTITHREEWITHONE)
	{
		if (bRet && tmpHandCards.size() < ctiTmp.nCount/3)  //没有足够的牌能够与三顺组成飞机
		{
			m_PromptCard.clear();
			m_pHandCards[SELFLOCALNO]->SetCheckedCards(m_PromptCard);
			return false;
		}

		//与单牌组成飞机
		TCARDS tmpSigCard = GetBaseCardsEx(1,CT_SINGLE,tmpHandCards,m_PromptCard);

		//将单牌与三顺组成小飞机
 		static int g_iSig = 0;  
 		if (g_iSig >= tmpSigCard.size())
 		{
 			g_iSig =0;
 		}
 		for (int i=0; i<ctiTmp.nCount/3; i++)
 		{
 			m_PromptCard.push_back(tmpSigCard.at(i));
 		}
 		g_iSig++;
		m_pHandCards[SELFLOCALNO]->SetCheckedCards(m_PromptCard);
		return true;

	}else if (bRet && ctiFrontOut.nType == CT_MULTITHREEWITHTWO)
	{
	
		if (bRet && tmpHandCards.size() < ctiTmp.nCount/3*2)  //没有足够的牌能够与三顺组成飞机
		{
			m_PromptCard.clear();
			m_pHandCards[SELFLOCALNO]->SetCheckedCards(m_PromptCard);
			return false;
		}

		//将对子与三顺组成大飞机
		TCARDS tmpDouCards = GetBaseCards(2,CT_DOUBLE,tmpHandCards);
		static int g_iDouble = 0;  
		if (g_iDouble >= tmpDouCards.size())
		{
			g_iDouble =0;
		}
		
		for (int i=0; i<ctiTmp.nCount/3*2; i+=2)
		{
			m_PromptCard.push_back(tmpDouCards.at(i));
			m_PromptCard.push_back(tmpDouCards.at(i+1));
		}
		g_iDouble+=2;
		m_pHandCards[SELFLOCALNO]->SetCheckedCards(m_PromptCard);
		return true;
	}
	return false;
}

//获取基础牌型
/*
CT_SINGLE,				// 单张
CT_DOUBLE,				// 一对
CT_THREE,				// 三张
CT_THREEWITHTWO,		// 三带二
CT_MULTITHREEWITHTWO,	// 大飞机/带一队
CT_THREEWITHONE,		// 三带一
CT_MULTITHREEWITHONE,	// 小飞机
CT_FOURWITHTWO,			// 四带二，带的是两张(形如AAA25)
CT_FOURWITHTDTWO,		// 四带二，带的是两对(形如AAA2233)
CT_BOOM,				// 炸弹
CT_MAXBOOM,				// 一对鬼
*/
TCARDS uiGameArea::GetBaseCards(int nCardsLen, int nType, TCARDS handCards)
{
	TCARDS tmpCards;
	tmpCards.clear();

	if (nCardsLen > handCards.size() || handCards.size() <= 0 )
	{
		return tmpCards;
	}
	TCardVector dvMayBeOut;
	CardTypeInfo ctiMayBeOut;		//用于获取牌型信息
	CardTypeInfo ctiFrontMBO;		//前面一个牌型信息
	ctiFrontMBO.reset();
	ctiMayBeOut.reset();
	TCardVector  dvTmpHand = ConvertCardsToCardVector(handCards);

	//获取手牌中拥有的基础牌
	for (int i=0; i<=dvTmpHand.size()- nCardsLen; i++)  //for1   
	{
		dvMayBeOut.clear();
		for (int j=i; j<nCardsLen+i; j++)  //for2
		{
			dvMayBeOut.push_back(dvTmpHand.at(j));  
		} //end for2
		
		if (!dvMayBeOut.empty())
		{
			ctiMayBeOut = GameRule::GetCardType(dvMayBeOut);
			if (ctiMayBeOut.nType == nType  )
			{
				//剔除因为牌组存在炸弹，获取一对的时候会出现获取到3对的类似情况
				if (ctiMayBeOut.nLevel == ctiFrontMBO.nLevel)
				{
					ctiFrontMBO = ctiMayBeOut;
					continue;
				}
				for (int n =i; n<i+nCardsLen; n++)
				{
					tmpCards.push_back(dvTmpHand.at(n).GetID());
				}
			}
			ctiFrontMBO = ctiMayBeOut;
		}
	}  //end for 1

	return tmpCards;
}


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



//获取指定牌组中的某一种基本牌，并排除与needReject牌值相同的牌
TCARDS uiGameArea::GetBaseCardsEx(int nCardsLen, int nType, TCARDS handCards,TCARDS needReject)
{
	TCARDS tmpCards;
	tmpCards.clear();

	if (nCardsLen > handCards.size())
	{
		return tmpCards;
	}
	TCardVector dvMayBeOut;
	CardTypeInfo ctiMayBeOut;		//用于获取牌型信息
	CardTypeInfo ctiFrontMBO;		//前面一个牌型信息
	ctiFrontMBO.reset();
	ctiMayBeOut.reset();
	TCardVector  dvTmpHand = ConvertCardsToCardVector(handCards);
	TCardVector  dvTmpNR   = ConvertCardsToCardVector(needReject);
	
	//排除不需要获取的牌
	for(int i=0;i<dvTmpNR.size();i++)
	{
		dvTmpHand.erase(remove_if(dvTmpHand.begin(),dvTmpHand.end(),IsEqualValue(dvTmpNR[i])),dvTmpHand.end());
	}

	//获取手牌中拥有的基础牌
	for (int i=0; i<=dvTmpHand.size()- nCardsLen; i++)  //for1   
	{
		dvMayBeOut.clear();
		for (int j=i; j<nCardsLen+i; j++)  //for2
		{
			dvMayBeOut.push_back(dvTmpHand.at(j));  
		} //end for2

		if (!dvMayBeOut.empty())
		{
			ctiMayBeOut = GameRule::GetCardType(dvMayBeOut);
			if (ctiMayBeOut.nType == nType  )
			{
				//剔除因为牌组存在炸弹，获取一对的时候会出现获取到3对的类似情况
				if (ctiMayBeOut.nLevel == ctiFrontMBO.nLevel)
				{
					ctiFrontMBO = ctiMayBeOut;
					continue;
				}
				for (int n =i; n<i+nCardsLen; n++)
				{
					tmpCards.push_back(dvTmpHand.at(n).GetID());
				}
			}
			ctiFrontMBO = ctiMayBeOut;
		}
	}  //end for 1

	return tmpCards;
}

int	uiGameArea::OnRespRoomInfo(int room, const string& strTitle )
{
	string strRoomInfo = "皇后游戏中心 - 斗地主 ";
	strRoomInfo += strTitle;
	uiTextCtrl* lpRoomInfo =  (uiTextCtrl*)m_lpFramework->GetCfgGui("","TitleGameName");
	if (lpRoomInfo)
	{
		lpRoomInfo->SetText(strRoomInfo.c_str());
	}

	return 0;
}


int uiGameArea::OnRespChangeSeatInTable( DWORD nNumber1, BYTE nSeat1,DWORD nNumber2, BYTE nSeat2 )
{
	ClearTable();
	return 0;
}
//////////////////////////////////////////////////////////////////////////