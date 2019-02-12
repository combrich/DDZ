#include "uiGameArea.h"
#include "resource.h"
#include "direct.h"

uiGameArea::uiGameArea(void)
: m_lpFramework(0), m_hGameSound(0)
{
	m_bIAmSeeing = false;
	m_IsScreen  = 1;
	m_IsAreaMax = FALSE;
	m_IsTop		= FALSE;
	m_nRoomType = RT_NORMAL;
	m_nGameType = GT_SCORE;
	m_nGrabLandType = GLT_GRAB;
	m_bSpeakPlay = false;
	memset(&m_LogicData, 0, sizeof(m_LogicData));
	Reset();
}

uiGameArea::uiGameArea(uiFrameworkInterface* lpFramework)
{
	m_lpFramework = lpFramework;
	m_hGameSound	 = 0;
	m_IsScreen		 = 1;
	Reset();
}

uiGameArea::~uiGameArea(void)
{
	SaveConfig();
	if(m_hGameSound != NULL)
	{
		ChannelStop(m_hGameSound);
		m_hGameSound = 1;
	}
}

void uiGameArea::Reset()
{
	m_nCountBoomPrompt = 0;
	m_nCountPrompt = 0;
	m_nCountMaxBoomPrompt = 0;
	m_nLandlord = MAX_PLAYER;
	m_nCallScore = 0;
	m_nTotalMult = 1;
	m_nBaseType		= false;
	m_bIsAlreadyOut = false;
	m_PromptCard.clear();
	m_nLocalPowerSeat = MAX_PLAYER; 
	m_nBankerSeat     = MAX_PLAYER;
	m_nGameState      = GAME_STATE_NONE;
	m_nSelfPower			= msgPower::PowerNone;
	m_bIsCallLandlord= true;
	m_bIsFirstOut			= false;
	m_dvOutCards.clear();
	m_nCountPrompt = 0;
	m_dvFrontOutCards.clear();
	m_nLBtnNow = 0;
	m_nLBtnOld  =0;
	for (size_t i=0; i<MAX_PLAYER; ++i)
	{
		m_nLocalSeat[i] = MAX_PLAYER;
		m_bNotOutIsShow[MAX_PLAYER]=false;			// ������־�Ƿ���ʾ
		m_BaseCards.clear();
		m_HandCards[i].clear();
		m_ptHand[i].x = 0;
		m_ptHand[i].y = 0;
	}

}


void uiGameArea::ShowGrabBtn(bool IsCall, BOOL bShow /* = TRUE */, int nCall/*=0*/)
{
	if (m_nGrabLandType == GLT_GRAB)  //��������ʽ
	{
		if (IsCall)  //�״�Ϊ�е���
		{
			uiButton* pCallLandlord = (uiButton*)m_lpFramework->GetCfgGui("","KW_BTN_CALL_BANKER");
			if (NULL == pCallLandlord)
			{
				ShowTalk(0xFF00FF00,"��Դ����CL,���ͼ��ϵ�ͷ�!");
				return;
			}
			pCallLandlord->SetEnabled(TRUE);
			pCallLandlord->SetRender(bShow);
			//����
			uiButton* pNotCall = (uiButton*)m_lpFramework->GetCfgGui("","KW_BTN_NOT_CALL");
			if (NULL == pNotCall)
			{
				ShowTalk(0xFF00FF00,"��Դ����NC,���ͼ��ϵ�ͷ�!");
				return;
			}
			pNotCall->SetEnabled(TRUE);
			pNotCall->SetRender(bShow);
		}else
		{
			//������
			uiButton* pGrabLandlord = (uiButton*)m_lpFramework->GetCfgGui("","KW_BTN_ROB_GRAB");
			if (NULL == pGrabLandlord)
			{
				ShowTalk(0xFF00FF00,"��Դ����GL,���ͼ��ϵ�ͷ�!");
				return;
			}
			pGrabLandlord->SetEnabled(TRUE);
			pGrabLandlord->SetRender(bShow);
			//����
			uiButton* pNotGrab = (uiButton*)m_lpFramework->GetCfgGui("","KW_BTN_NOT_ROB");
			if (NULL == pNotGrab)
			{
				ShowTalk(0xFF00FF00,"��Դ����NG,���ͼ��ϵ�ͷ�!");
				return;
			}
			pNotGrab->SetEnabled(TRUE);
			pNotGrab->SetRender(bShow);
			return;
		}
	}
	else if (m_nGrabLandType == GLT_SCORE) //�зַ�ʽ
	{
		//һ��
		uiButton* pBtnOneScore = (uiButton*)m_lpFramework->GetCfgGui("","KW_BTN_ONE_SCORE");
		if (NULL == pBtnOneScore)
		{
			ShowTalk(0xFF00FF00,"��Դ����OS,���ͼ��ϵ�ͷ�");
			return ;
		}
		pBtnOneScore->SetRender(bShow);
		//����
		uiButton* pBtnTwoScore = (uiButton*)m_lpFramework->GetCfgGui("","KW_BTN_TWO_SCORE");
		if (NULL == pBtnTwoScore)
		{
			ShowTalk(0xFF00FF00,"��Դ����TS,���ͼ��ϵ�ͷ�");
			return ;
		}
		pBtnTwoScore->SetRender(bShow);
		//����
		uiButton* pBtnThreeScore = (uiButton*)m_lpFramework->GetCfgGui("","KW_BTN_THREE_SCORE");
		if (NULL == pBtnThreeScore)
		{
			ShowTalk(0xFF00FF00,"��Դ����TS,���ͼ��ϵ�ͷ�");
			return ;
		}
		pBtnThreeScore->SetRender(bShow);
		//����
		uiButton* pNotCall = (uiButton*)m_lpFramework->GetCfgGui("","KW_BTN_NO_CALL_SCORE");
		if (NULL == pNotCall)
		{
			ShowTalk(0xFF00FF00,"��Դ����NC,���ͼ��ϵ�ͷ�!");
			return;
		}
		pNotCall->SetEnabled(TRUE);
		pNotCall->SetRender(bShow);
		if (bShow)
		{
			pBtnOneScore->SetEnabled(TRUE);
			pBtnTwoScore->SetEnabled(TRUE);
			pBtnThreeScore->SetEnabled(TRUE);
			if (nCall == 1)
			{
				pBtnOneScore->SetEnabled(FALSE);
			}else if (nCall==2)
			{
				pBtnOneScore->SetEnabled(FALSE);
				pBtnTwoScore->SetEnabled(FALSE);
			}
		}
	}
}

void uiGameArea::GetPowerGrab(msgPower& power)
{
	m_bIsCallLandlord = power.isCallLandLord;
	if (power.HaveGrabPower)
	{
		ShowGrabBtn(m_bIsCallLandlord,TRUE,power.nScore);
	}
}


void uiGameArea::GetPowerPlay(msgPower& power)
{
	m_bIsFirstOut		=  power.bIsFirstOut;
	ShowAllPlayBtn(TRUE,!m_bIsFirstOut,FALSE,!m_bIsFirstOut,power.bOpenCard);
	if (this->CanOut(m_bIsFirstOut))
	{
		uiButton* lpOutCard = (uiButton*)m_lpFramework->GetCfgGui("","KW_BTN_PLAY_CARD");
		lpOutCard->SetEnabled(TRUE);
	}
	//�� ����
	TCARDS tmpPopCards;
	m_pHandCards[SELFLOCALNO]->GetCheckedCards(tmpPopCards);
	CardTypeInfo cti = GameRule::GetCardType(m_dvFrontOutCards);
	if (!m_dvFrontOutCards.empty() &&!GetGreaterThanNextOut(cti))
	{
		GUIEx* hCanNotPlay = (GUIEx*)m_lpFramework->GetCfgGui("","KW_SIGN_NO_BIGGER");
		hCanNotPlay->SetRender(TRUE);
		hCanNotPlay->SetRenderEx(false,MT_NULL,AT_CHANGE,RT_NULL,500,300,100);
	}
	m_pHandCards[SELFLOCALNO]->ClearCheckedCards();
	m_pHandCards[SELFLOCALNO]->SetCheckedCards(tmpPopCards);
}



// ��ʾ�������
int uiGameArea::ShowOutCard(int nSeat, BOOL bShow /*=TRUE*/)
{
	TCARDS oCards = ConvertCardVectorToCards(m_dvFrontOutCards);
	SortCards(oCards);
	m_pOutCard[nSeat]->DeleteAllCards();
	m_pOutCard[nSeat]->SetCards(oCards);

	ArrangeOutCard(GetGuiW(),GetGuiH(),nSeat);
	m_pOutCard[nSeat]->SetRender(bShow);
	return 0;
}


void uiGameArea::ShowAni( LPCTSTR strKey, bool IsOut /*=ture*/,SHORT nSeat /*= -1*/,SHORT loop /* = 1*/,bool bShow /*=true*/)
{
	HGUI lpGui = GetGUIHandle(strKey, nSeat);
	if (lpGui /*&& !m_tSetUpParam.IsAni*/) 
	{
		if (IsOut)
		{
			lpGui->SetCenter(m_pOutCard[SEAT2LOCAL(nSeat)],0,10);
		}
		lpGui->SetRender(bShow);
		lpGui->ReStartAni(loop);
		if (strKey == "AnimationFeiJi_" && bShow)
		{
			POINT pt = {GetGuiW()-150,GetGuiH()/2-UI_H(lpGui)/2};
			lpGui->SetStartPos(pt);
			lpGui->AutoSpeedUpMoveTo(-UI_W(lpGui),lpGui->GetStartPosY(),40);
		}
	}
}


// ��ʾ������־
void uiGameArea::ShowNotOutSign(int nSeat, BOOL bShow/* = TRUE*/, bool bShowEx /*=false*/)
{
	GUIEx* hNotOut= (GUIEx*)m_lpFramework->GetCfgGui("", GetFormatString("KW_SIGN_NOT_OUT_%d",nSeat));
	if (!hNotOut)
	{
		ShowTalk(0xFF00FF00,"��Դ����0�����ͼ��ϵ�ͷ�!");
		return;
	}
	hNotOut->SetRender(bShow);
	//��������
	if (bShowEx && m_bNotOutIsShow[LOCAL2SEAT(nSeat)])
	{
		hNotOut->SetRender(TRUE);
		hNotOut->SetRenderEx(false,MT_NULL,AT_CHANGE,RT_NULL,500,300,50);
	}
	if (bShow)
	{
		m_bNotOutIsShow[LOCAL2SEAT(nSeat)] = TRUE;
	}else 
	{
		m_bNotOutIsShow[LOCAL2SEAT(nSeat)] = FALSE;
	}
}

//��ʾ������صİ�ť
// ��������ʾ������,����
void uiGameArea::ShowAllPlayBtn(BOOL bShow,BOOL bGiveUp, BOOL bOutCard, BOOL bInfo, BOOL bOpenHand)
{
	//�b��
	if (bOpenHand || !bShow)
	{
		uiButton* pOpenCard = (uiButton*)m_lpFramework->GetCfgGui("","KW_BTN_OPEN_CARD");
		if (pOpenCard)
		{
			pOpenCard->SetRender(bShow);
		}
	}
	//����
	uiButton* lpOutCard = (uiButton*)m_lpFramework->GetCfgGui("","KW_BTN_PLAY_CARD");
	lpOutCard->SetEnabled(bOutCard);
	lpOutCard->SetRender(bShow);

	//����
	uiButton* lpGiveUp = (uiButton*)m_lpFramework->GetCfgGui("","KW_BTN_NOT_PLAY");
	lpGiveUp->SetEnabled(bGiveUp);
	lpGiveUp->SetRender(bShow);
	//��ʾ
	uiButton* lpInfo = (uiButton*)m_lpFramework->GetCfgGui("","KW_BTN_HINT_CARD");
	lpInfo->SetEnabled(TRUE);
	lpInfo->SetRender(bShow);

	POINT pt = {GetGuiW()/2,GetGuiH() - 175 - UI_H(lpOutCard)};
	int   nBtnW = UI_W(lpOutCard);
	if (bOpenHand)
	{
		lpOutCard->SetStartPos(pt.x-nBtnW*2 -22.5, pt.y);
		lpGiveUp->SetStartPos(pt.x-nBtnW - 7.5,pt.y);
		lpInfo->SetStartPos(pt.x + 7.5,pt.y);
		uiButton* lpOpenCard = (uiButton*)m_lpFramework->GetCfgGui("","KW_BTN_OPEN_CARD");
		if (lpOpenCard)  lpOpenCard->SetStartPos(pt.x+nBtnW+ 22.5,pt.y);
	}else 
	{
		lpOutCard->SetStartPos(pt.x-nBtnW * 1.5 -15,pt.y);
		lpGiveUp->SetStartPos(pt.x - nBtnW/2,pt.y);
		lpInfo->SetStartPos(pt.x + nBtnW/2 + 15,pt.y);
	}
}

void uiGameArea::LoadConfig()
{
	m_IsScreen = 1;
	IniParser* ini = IniParser::Create( );
	if (ini)
	{
		if (ini->FromFile(GetFullPath("Config.ini")))
		{
			m_tSetUpParam.IsSnd = ini->GetNumber("GAME","SND",1);
			m_tSetUpParam.IsAni = ini->GetNumber("GAME","ANI",1);
			m_tSetUpParam.IsWndTop = ini->GetNumber("GAME","WNDTOP",0);
			m_tSetUpParam.IsWndFlash = ini->GetNumber("GAME","WNDFLASH",1);
			m_tSetUpParam.IsWndJump = ini->GetNumber("GAME","WNDJUMP",0);
			m_IsScreen = ini->GetNumber("GAME","SCREEN",1);
		}
		ini->Release();
	}
	IniParser* ini2 = IniParser::Create( );
	if (ini2)
	{
		if (ini2->FromFile(GetFullPath("layout.ini")))
		{
			m_strHelpURL = ini2->GetString("TITLEBAR","ButtonHelp.URL","");
		}
		ini2->Release();
	}
}

void uiGameArea::SaveConfig()
{
	IniParser* ini = IniParser::Create( );
	if (ini)
	{	
		ini->SetNumber("GAME","SND",m_tSetUpParam.IsSnd);
		ini->SetNumber("GAME","ANI",m_tSetUpParam.IsAni);
		ini->SetNumber("GAME","WNDTOP",m_tSetUpParam.IsWndTop);
		ini->SetNumber("GAME","WNDFLASH",m_tSetUpParam.IsWndFlash);
		ini->SetNumber("GAME","SCREEN",m_IsScreen);
		ini->SaveFile(GetFullPath("Config.ini"));
	
		ini->Release();
	}
}

BOOL uiGameArea::IsSetMaxWindow()
{
	return m_IsScreen;
}

void uiGameArea::SetWndTop( BOOL bTop )
{
	SetWindowPos(m_lpFramework->GetSafeWnd(),((bTop == TRUE) ? HWND_TOPMOST : HWND_NOTOPMOST),0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
}

//ʱ�Ӵ���
bool uiGameArea::CreateClock()
{
	//ʱ��
	m_pClockUI = new ClockUI;
	if(!m_pClockUI) return FALSE;
	if (!m_pClockUI->Create("ʱ��",1,1,this)) return FALSE;
	if(!AppendChild(m_pClockUI,0,0)) return FALSE;
	m_pClockUI->SetLocal(MAX_PLAYER);
	m_pClockUI->SetRender(FALSE);
	return true;
}

//���ƴ���
bool uiGameArea::CreateBase()
{
	m_pBaseCards = new uiHandCards;
	if (!m_pBaseCards) return FALSE;
	if (!m_pBaseCards->Create("����",1,1)) return FALSE;
	if (!AppendChild(m_pBaseCards,0,0))  return FALSE;
	m_pBaseCards->SetCheckedRect(FALSE);
	m_pBaseCards->SetCapture(FALSE);
	m_pBaseCards->SetLocalSeat(SELFLOCALNO);
	m_pBaseCards->SetRender(FALSE);
	m_pBaseCards->SetColsSpacing(56);
	m_pBaseCards->SetLineLimit(3);
	m_pBaseCards->SetGroupID(2,true);
	return true;
}

bool uiGameArea::CreateHead()
{
	for (size_t i =0; i<MAX_PLAYER; ++i)
	{
		m_pHeadPortrait[i] = new uiHeadPortrait;
		if (!m_pHeadPortrait[i] || !m_pHeadPortrait[i]->Create("ͷ��", 1,1,this)) return FALSE;
		if(!AppendChild(m_pHeadPortrait[i],0,0)) return FALSE;
		m_pHeadPortrait[i]->SetDragDrop(FALSE);
		m_pHeadPortrait[i]->SetRender(FALSE);
	}
	return true;
}
bool uiGameArea::CreateHand()
{
	for (int i=0;i<MAX_PLAYER;i++)
	{	//����
		m_pHandCards[i] = new uiHandCards;
		if(!m_pHandCards[i]) return FALSE;
		if(!m_pHandCards[i]->Create("����", 1, 1)) return FALSE;
		if(!AppendChild(m_pHandCards[i], 0, 0)) return FALSE;
		m_pHandCards[i]->SetCheckRect(TRUE);
		m_pHandCards[i]->SetLocalSeat(SELFLOCALNO);
		m_pHandCards[i]->SetCapture(TRUE);
		m_pHandCards[i]->SetRender(FALSE);		
		m_pHandCards[i]->SetGroupID(0,TRUE);
		if (SELFLOCALNO == i)
		{
				m_pHandCards[i]->SetCheckRect(TRUE);
		}else
		{
			m_pHandCards[i]->SetCheckRect(FALSE);
			//������ҵ����ǲ���ѡ�еģ��⼴�����������Ϣ
			m_pHandCards[i]->SetCapture(FALSE);
			m_pHandCards[i]->SetLineLimit(1);
			m_pHandCards[i]->SetGroupID(1,TRUE);
			m_pHandCards[i]->UpdateAllCards();
		}
		
	}
	return true;
}

bool uiGameArea::CreateOut()
{
	for (size_t i=0; i<MAX_PLAYER; ++i)
	{
		m_pOutCard[i] = new uiHandCards;
		if (!m_pOutCard[i]) return FALSE;
		if (!m_pOutCard[i]->Create("����",1,1)) return FALSE;
		if (!AppendChild(m_pOutCard[i],0,0)) return FALSE;
		m_pOutCard[i]->SetCheckRect(TRUE);
		m_pOutCard[i]->SetLocalSeat(SELFLOCALNO);
		m_pOutCard[i]->SetCapture(FALSE);
		m_pOutCard[i]->SetRender(FALSE);
		m_pOutCard[i]->SetGroupID(1,TRUE);
	}

	return true;
}

BOOL uiGameArea::OnCreate( LONG nGuiW, LONG nGuiH )
{
	if(0 == m_lpFramework)
	{
		if(0 != GCFW_GetFramework(m_lpFramework))
		{
			return FALSE;
		}	
	}
	//lin
	SetFocusStatus(GfsWindowHandler);
	//lin
	SetFocusStatus(GfsKeyboard);
	LoadGameConfig();
	//UI����
	CreateBase();
	CreateClock();
	CreateHand();
	CreateOut();
	CreateHead();

	//�����������
	m_uipMakeCards = new uiMakeCardPanel;
	if (NULL == m_uipMakeCards || !m_uipMakeCards->Create("�������",1,1,this))
	{
		return false;
	}
	AppendChild(m_uipMakeCards,200,200);
	m_uipMakeCards->SetCapture(true);	
	m_uipMakeCards->SetDragDrop(true);
	m_uipMakeCards->SetRender(false);
	
	return TRUE;
}


//������Ϸ����ؿؼ���λ��
//�˴�����ͷ������λ��
void uiGameArea::ArrangeLocalPosition(LONG nGuiW,LONG nGuiH,SHORT nLocal)
{
	if (nLocal >= MAX_PLAYER)
	{
		char buf[1024]={0};
		sprintf(buf,"����λ�õ�ʱ������һ�������λ��%d,���ͼ��ϵ�ͷ�",nLocal);
		ShowTalk(0xFF00FF00,"");
		return;
	}

	if(m_IsScreen&& nLocal != SELFLOCALNO) //���������󻯣�ɶҲ����
	{
		m_pHandCards[nLocal]->SetGroupID(0,TRUE);
		m_pHandCards[nLocal]->SetLineSpacing(30);
	}
	else if (nLocal !=SELFLOCALNO)
	{
		m_pHandCards[nLocal]->SetGroupID(1,TRUE);
		m_pHandCards[nLocal]->SetLineSpacing(20);
	}
	m_pHandCards[nLocal]->UpdateAllCards();

	LONG nGameW = nGuiW;
	LONG nGmaeH = nGuiH;

	POINT ptHead = {0,0};
	LONG nHeadW = UI_W(m_pHeadPortrait[nLocal]);  //��ȡ���ͷ��UI�Ŀ��
	LONG nHeadH = UI_H(m_pHeadPortrait[nLocal]);  //��ȡ���ͷ��UI�ĸ߶�

	LONG nHandH = UI_H(m_pHandCards[nLocal]);	  //��ȡ������Ƹ߶�
	LONG nHandW = UI_W(m_pHandCards[nLocal]);     //��ȡ������ƿ��
	m_IsScreen = MAXWINDOW();

	POINT ptBase = {0,0};
	ptBase.x = nGuiW/2 - 90;  //x��λ����Ϸ��������в�ƫ��90��λ
	POINT ptHand = {0,0};
	ptHand.x = nGuiW/2 - 70;  //x��λ����Ϸ��������в�ƫ��70��λ
	if(LP_LEFT == nLocal)  //������--0
	{
		ptHand.x = 130;
		ptHand.y = nGuiH-(nGuiH-nHandH)/2 -nHandH - 10;
		ptHead.x = 15;
		ptHead.y = nGuiH/2 - nHeadH+20;

	}
	else if (LP_DOWN == nLocal)  //�����·�---�����Զ���Լ���λ��--1
	{
		ptHand.x  = nGuiW/2-nHandW/2;
		ptHand.y  = nGuiH - nHandH - 45;
		ptHead.x =  nGuiW/2 - 380 ;
		ptHead.y = nGuiH - nHeadH - 50;
	}
	else if (LP_RIGHT == nLocal) //�����ҷ�---2
	{
		ptHand.x = nGuiW - 130 - nHandW;
		ptHand.y = nGuiH - (nGuiH-nHandH)/2 - nHandH - 10;
		ptHead.x = nGuiW - nHeadW - 15;
		ptHead.y = nGuiH/2 - nHeadH+20;
	}
	else if (LP_UP == nLocal)   // �����Ϸ�---3
	{

	}
	m_ptHand[nLocal] = ptHand;
	m_pHeadPortrait[nLocal]->SetStartPos(ptHead);
	m_pHandCards[nLocal]->SetStartPos(ptHand.x,ptHand.y);
}

void uiGameArea::OnMouseMove(LONG nStartX, LONG nStartY)
{
	uiGameInterface::OnMouseMove(nStartX,nStartY);
}

void uiGameArea::OnMouseLButton(LONG nStartX, LONG nStartY, UINT aState)
{
	if ( aState == GmsDown )
	{
		HGUI pGuiSpeak = m_lpFramework->GetCfgGui("","KW_SPEAK_BACK");
		if (pGuiSpeak && pGuiSpeak->IsRender())
		{
			pGuiSpeak->SetRender(FALSE); 
		}
	}
}

void uiGameArea::OnMouseRButton(LONG nStartX, LONG nStartY, UINT aState)
{
	m_pHandCards[SELFLOCALNO] ->ClearCheckedCards();
	m_dvOutCards.clear();
	uiButton* pBtn = (uiButton*)m_lpFramework ->GetCfgGui("","KW_BTN_PLAY_CARD");
	if (NULL != pBtn) pBtn->SetEnabled(FALSE);
}

void uiGameArea::OnKeyMsg(UINT nChar, UINT nFlags, UINT aState)
{
	if (GmsDown == aState)
	{
		if (nChar == VK_SPACE)
		{
			m_lpFramework->ShowChat(0xFFFFFFFF, "VK_SPACE");
		}
	}
}

//���ڳߴ�仯
int uiGameArea::OnFrameworkSize()
{
	if(m_IsAreaMax)
	{
		SetWindowPos(GetSafeWnd(),NULL,0,0,m_lpFramework->GetSize().cx-2,GetSize().cy,SWP_NOZORDER|SWP_NOMOVE);
		SetSize(m_lpFramework->GetSize().cx-2,GetSize().cy);
	}

	return 1;
}

void uiGameArea::OnSize(LONG nGuiW, LONG nGuiH)
{
	m_IsScreen = MAXWINDOW();	
	ArrangePlayBtn();
	ArrangePoint(m_nBasePlayScore,m_nTotalMult);
	if (m_nGameState == GAME_STATE_PLAY || m_nGameState == GAME_STATE_GRAB ||
		m_nGameState == GAME_STATE_END )
	{
		ShowBase();
	}
	for(BYTE i=0;i<MAX_PLAYER;i++)
	{
		ArrangeLocalPosition(nGuiW, nGuiH, i);
		ArrangeOutCard(nGuiW,nGuiH,i);
	}
	ArrangeButtonPosition(nGuiW,nGuiH);
	ArrangeClockPosition(nGuiW, nGuiH, m_nLocalPowerSeat);
	ArrangeButtonPosition(nGuiW, nGuiH);
	
	if (m_pBaseCards->IsRender()) ShowBase(TRUE,msgBase::NOACTION);

	if (m_nGameState == GAME_STATE_PLAY || 
		m_nGameState == GAME_STATE_GRAB ||
		m_nGameState == GAME_STATE_TAKE ||
		m_nGameState == GAME_STATE_END)
	{
		ShowBasePoint(m_nBasePlayScore,TRUE);
		ShowAllMult(m_nTotalMult,TRUE);
		ShowHandCount(m_HandCards[SELFLOCALNO].size(),SELFLOCALNO,TRUE);
	}  
}

LRESULT uiGameArea::OnChildCommand(HGUI hChild, UINT iCmd, WPARAM wParam, LPARAM lParam)
{
	if ( iCmd == CMD_MOUSELBUTTON  || iCmd == CMD_CARD_CHECK_RECT)
	{
		m_nLBtnNow = clock();
		int nTick = m_nLBtnOld-m_nLBtnNow;
		//�����������ϰ��£�������Ҵ�����Ϸ״̬����ȡ���ѡ�е������ƣ����ж��Ƿ�ɳ���
		if (hChild == m_pHandCards[SELFLOCALNO] && m_nGameState != GAME_STATE_END)
		{
			uiButton* lpOutCard = (uiButton*)m_lpFramework->GetCfgGui("","KW_BTN_PLAY_CARD");
			if ( GetSelfPlayer() &&  GetSelfPlayer()->getState() != US_SEEING && 
				SELFLOCALNO == m_nLocalPowerSeat  && m_nGameState == GAME_STATE_PLAY)
			{
				if (CanOut(m_bIsFirstOut) && !m_dvOutCards.empty()) //�ɳ��ƣ�ʹ���ư�ť��Ч
				{
					lpOutCard->SetEnabled(TRUE);
				}else				
				{
					lpOutCard->SetEnabled(FALSE);
				}
			}
		}
		//time(&m_nLBtnOld);
		m_nLBtnOld = clock();
	}else if (iCmd == CMD_MOUSERBUTTON && hChild == m_pHandCards[SELFLOCALNO])
	{
		if (GmsDown == wParam && CanOut(m_bIsFirstOut) && 
			m_nGameState == GAME_STATE_PLAY && m_nLocalPowerSeat == SELFLOCALNO
			&& !m_bIsAlreadyOut)
		{
			this->OnBtnOutCard();
			m_bIsAlreadyOut	= true;
		}
	}else if (CMD_MOVE_FINISH == iCmd )
	{
		CloseAni();
		if (hChild == m_pBaseCards)
		{
			ShowBase(m_pBaseCards->IsRender(),msgBase::NOACTION);
		}
	}
	return 0;
}


//��ʾ����
void uiGameArea::ShowAllMult(int nMult, BOOL bShow /*= TRUE*/ )
{
	uiNumber* lpGui = (uiNumber*)GetGUIHandle("MulNum");
	if (lpGui != NULL) {
		HGUI  hBCBack = m_lpFramework->GetCfgGui("GV","GameBaseCardBack");
		if (hBCBack) hBCBack->SetRender(TRUE);
		lpGui->SetCenter(hBCBack,-40,50);
		lpGui->SetSize(57,15);
		lpGui->SetNumber(nMult, true);
		lpGui->SetRender(bShow);
	}
	ArrangePoint(m_nBasePlayScore,m_nTotalMult);
}

//��ʾ�׷�
void uiGameArea::ShowBasePoint( LONG nScore, BOOL bShow /*= TRUE*/ )
{
	uiNumber* lpGui = (uiNumber*)GetGUIHandle("BasePointNum");
	if (lpGui != NULL) {
		HGUI  hBCBack = m_lpFramework->GetCfgGui("GV","GameBaseCardBack");
		if (hBCBack) hBCBack->SetRender(TRUE);
		lpGui->SetNumber(nScore, true);
		lpGui->SetRender(bShow);
		lpGui->SetCenter(hBCBack,96,50);
		lpGui->SetSize(95,15);
	}
	ArrangePoint(m_nBasePlayScore,m_nTotalMult);
}

void uiGameArea::ArrangePlayBtn()
{
	//�b��
	uiButton* pOpenCard = (uiButton*)m_lpFramework->GetCfgGui("","KW_BTN_OPEN_CARD");
	if (NULL == pOpenCard) return;
	//����
	uiButton* lpOutCard = (uiButton*)m_lpFramework->GetCfgGui("","KW_BTN_PLAY_CARD");
	if (NULL == lpOutCard) return;
	//����
	uiButton* lpGiveUp = (uiButton*)m_lpFramework->GetCfgGui("","KW_BTN_NOT_PLAY");
	if (NULL == lpGiveUp) return;
	//��ʾ
	uiButton* lpInfo = (uiButton*)m_lpFramework->GetCfgGui("","KW_BTN_HINT_CARD");
	if (NULL == lpInfo) return;

	POINT pt = {GetGuiW()/2,GetGuiH() - 175 - UI_H(lpOutCard)};
	int   nBtnW = UI_W(lpOutCard);
	//��������
	if (pOpenCard->IsRender())
	{
		lpOutCard->SetStartPos(pt.x-nBtnW*2 -22.5, pt.y);
		lpGiveUp->SetStartPos(pt.x-nBtnW - 7.5,pt.y);
		lpInfo->SetStartPos(pt.x + 7.5,pt.y);
		uiButton* lpOpenCard = (uiButton*)m_lpFramework->GetCfgGui("","KW_BTN_OPEN_CARD");
		if (lpOpenCard)  lpOpenCard->SetStartPos(pt.x+nBtnW+ 22.5,pt.y);
	}else 
	{
		lpOutCard->SetStartPos(pt.x-nBtnW * 1.5 -15,pt.y);
		lpGiveUp->SetStartPos(pt.x - nBtnW/2,pt.y);
		lpInfo->SetStartPos(pt.x + nBtnW/2 + 15,pt.y);
	}
}

void uiGameArea::ArrangePoint(int nBase ,int nMult)
{
	uiNumber* lpGui = (uiNumber*)GetGUIHandle("BasePointNum");
	HGUI  hBCBack = m_lpFramework->GetCfgGui("GV","GameBaseCardBack");
	if (lpGui != NULL) {
		lpGui->SetNumber(nBase,TRUE);
		lpGui->SetCenter(hBCBack,96,50);
		lpGui->SetSize(95,15);
	}

	uiNumber* lpGui1= (uiNumber*)GetGUIHandle("MulNum");
	if (lpGui1 != NULL) {
		lpGui1->SetNumber(nMult,TRUE);
		lpGui1->SetCenter(hBCBack,-40,50);
		lpGui1->SetSize(57,15);
	}
}

//��ʾ�������
void uiGameArea::ShowMission(int nCardType, int nShowType, bool bShow)
{
	GUIEx* hGui = (GUIEx*)m_lpFramework->GetCfgGui("","GameRenWuBack");
	if (hGui == NULL) return;
	hGui->SetRender(bShow);
	HGUI  hGuiKind = (GUI*) m_lpFramework->GetCfgGui("","GameRenWuKind");
	hGuiKind->GoAniFrame(nCardType);
	hGuiKind->SetRender(bShow);
}

//��ʾ��������
void uiGameArea::ShowHandCount( SHORT nCount, SHORT nSeat,BOOL bShow /*= TRUE*/ )
{
	if (nSeat < 0 || nSeat >= MAX_PLAYER)
	{
		return;
	}
	//��ʾ��������������
	uiNumber* lpGui = (uiNumber*)GetGUIHandle(GetFormatString("CardCountNum_%d",nSeat));
	uiTextCtrl* lpText = (uiTextCtrl*)m_lpFramework->GetCfgGui("",GetFormatString("CardCountNumText_%d",nSeat));
	if (lpGui != NULL && lpText != NULL)
	{
		lpGui->SetNumber(nCount,false);
		lpGui->ArrangeNumber();
		lpGui->SetRender(bShow);
		lpText->SetRender(bShow);
	}
}


//��ʾ������־
void uiGameArea::ShowSignLandlord(BOOL bShow /*=TRUE*/)
{
	HGUI hLandlord = GetGUIHandle(GetFormatString("KW_SIGN_BANKER_%d",SEAT2LOCAL(m_nLandlord)));
	if (hLandlord)
		hLandlord->SetRender(bShow);
}

void uiGameArea::OnGameTickTimer(DWORD dwTickNow)
{

}

void uiGameArea::LoadGameConfig()
{

}


//�йܰ�ť
void uiGameArea::ShowBtnTrush(bool bShow /*= true*/)
{
	if (m_nGameState == GAME_STATE_PLAY || GAME_STATE_END == m_nGameState)
	{
		uiButton* pButton = (uiButton*)m_lpFramework->GetCfgGui("","KW_BTN_TRUST");
		if (pButton)
		{
			pButton->SetRender(bShow);
		}

		HGUI hTrust = GetGUIHandle("KW_TRUST_PANEL");
		if (hTrust)
		{
			hTrust->SetRender(!bShow);
		}
	}	
}


//�������
void uiGameArea::ClearTable()
{
	ShowSignLandlord(FALSE);
	uiButton* pButton = (uiButton*)m_lpFramework->GetCfgGui("","KW_BTN_TRUST");
	if (pButton)
	{
		pButton->SetRender(FALSE);
	}
	//�رվ�������
	for (int i=0; i<MAX_PLAYER;i++)
	{
		ShowAni("AniWarning_",FALSE,i,0,FALSE);
	}
	ShowBase(FALSE);
	if (RT_NORMAL == m_nRoomType)
	{
		uiButton* lpBtnCT = (uiButton*) m_lpFramework->GetCfgGui( "", "KW_CHANGESEAT_TABLE" );	
		if (lpBtnCT)	
		{
			lpBtnCT->SetRender(FALSE);	
		}
	}
	//���ؽ����
	HGUI pEndBack = m_lpFramework->GetCfgGui("GV", "GameEndBack");
	if (pEndBack)  pEndBack->SetRender(FALSE);
	ShowAni("AniFarmerWin",FALSE,-1,-1,FALSE);
	ShowAni("AniBankerWin",FALSE,-1,-1,FALSE);
	ShowAni("AniBankerWinEx",FALSE,-1,-1,FALSE);
	ShowAni("AniFarmerWinEx",FALSE,-1,-1,FALSE);

	//����ǿ�˿�
	HGUI pEndExit = m_lpFramework->GetCfgGui("","MessageBoxHint2");
	if (pEndExit)  pEndExit->SetRender(FALSE);
	//����ʱ�Ӻ����ơ�����
	GUIEx* hNotOut = NULL;
	for (int i=0;i<MAX_PLAYER;i++)
	{
		m_pHandCards[i]->SetRender(FALSE);
		m_pHandCards[i]->DeleteAllCards();
		m_pOutCard[i]->SetRender(FALSE);
		m_pOutCard[i]->DeleteAllCards();
		//���س��Ʊ�־
		hNotOut = (GUIEx*)m_lpFramework->GetCfgGui("", GetFormatString("KW_SIGN_NOT_OUT_%d",i));
		if (!hNotOut)
		{
			return;
		}

		hNotOut->SetRender(FALSE);
	}
	m_pClockUI->SetRender(FALSE);

	//���ص���
	m_pBaseCards->SetRender(FALSE);
	m_pBaseCards->DeleteAllCards();
	ShowBase(FALSE);
	//���ص��Ʊ���
	ShowBaseCardsType(BC_NOCARDTYPE,FALSE);
	//����������
	for (int i=0; i<MAX_PLAYER; i++)
	{
		ShowHandCount(0,i,FALSE);
	}
	//���ص׷�
	ShowBasePoint(0,FALSE);
	//���ر���
	ShowAllMult(0,FALSE);
	//���ؽе�����ذ�ť
	for (int i=0; i<MAX_PLAYER; i++)
	{
		this->ShowSignGrab(i,msgGrabLandlord::CALL,FALSE);
		this->ShowSignGrab(i,msgGrabLandlord::NOTCALL,FALSE);
		this->ShowSignGrab(i,msgGrabLandlord::GRAB,FALSE);
		this->ShowSignGrab(i,msgGrabLandlord::NOTGRAB,FALSE);
	}
	ClearAllCallSign();
}
//����ʱ��λ��
void uiGameArea::ArrangeClockPosition(LONG nGuiW,LONG nGuiH,short nLocal)
{	
	if (nLocal >= MAX_PLAYER)
	{
		return;
	}
	POINT ptClock = {0,0};
	long nHandH = UI_H(m_pHandCards[nLocal]);
	long nHandW = UI_W(m_pHandCards[nLocal]);
	long nClockH= UI_H(m_pClockUI);
	long nClockW= UI_W(m_pClockUI);
	if(LP_LEFT == nLocal)  //������--0
	{
		ptClock.x = 131 + nHandW;
		ptClock.y = nGuiH/2 - nClockH/2 ;
	}
	else if (LP_DOWN == nLocal)  //�����·�---�����Զ���Լ���λ��--1
	{
		ptClock.x = nGuiW/2 - nClockW/2;
		ptClock.y = nGuiH - 290;
	}
	else if (LP_RIGHT == nLocal) //�����ҷ�---2
	{
		ptClock.x = nGuiW - nHandW - nClockW - 131;
		ptClock.y = nGuiH/2 - nClockH/2 ;
	}
	else if (LP_UP == nLocal)   // �����Ϸ�---3
	{

	}
	m_pClockUI->SetStartPos(ptClock);
}

void uiGameArea::ArrangeButtonPosition(LONG nGuiW,LONG nGuiH)
{
	m_IsScreen = MAXWINDOW();
	HGUI hBtnStart     = m_lpFramework->GetCfgGui("","KW_BTN_START_GAME");
	if(NULL == hBtnStart)
	{
			ShowTalk(0xFF00FF00,"11��ʼ��ť��Դ��ȡʧ�ܣ����ͼ��ϵ�ͷ���");
	}

	if (RT_NORMAL == m_nRoomType)
	{
		//������ť��λ��
		uiButton* lpBtnCT = (uiButton*) m_lpFramework->GetCfgGui( "", "KW_CHANGESEAT_TABLE" );	
		if (lpBtnCT )	
		{
			lpBtnCT->SetCenterVert(this,150);
			lpBtnCT->SetCenterHorz(this,-80);
		}
		//��ʼ��ť��λ��
		hBtnStart->SetCenterVert(this,150);
		hBtnStart->SetCenterHorz(this,80);
	}else if (RT_MATCH == m_nRoomType)
	{
		//��ʼ������λ��
		hBtnStart->SetCenter(this,-15 ,150);
	}

	//׼����ʶλ��
	for (size_t i=0; i<MAX_PLAYER; ++i)
	{
		string strKey = GetFormatString("KW_PLAYER_READY_%d", i);
		HGUI hSignReady     = m_lpFramework->GetCfgGui("",strKey);
		if (hSignReady == NULL)
		{
			char buf[130] = {0};
			sprintf(buf,"%d׼����ʶ��Դ��ȡʧ�ܣ����ͼ��ϵ�ͷ���",i);
			ShowTalk(0xFF00FF00,buf);
		}
		if (RT_NORMAL == m_nRoomType)
		{
		}else if (RT_MATCH == m_nRoomType)
		{
			hSignReady->SetCenter(this,-110,130);
		}
	}
}


//���ó�����ʾλ��
void uiGameArea::ArrangeOutCard(LONG nGuiW, LONG nGuiH, SHORT nLocal)
{
	if (nLocal >= MAX_PLAYER)
	{
		return;
	}
	POINT ptOut = {0,0};
	long nHandH = UI_H(m_pHandCards[nLocal]);
	long nHandW = UI_W(m_pHandCards[nLocal]);

	long nOutW  =  UI_W(m_pOutCard[nLocal]);
	long nOutH  =  UI_H(m_pOutCard[nLocal]);
	m_IsScreen = MAXWINDOW();
	if (m_IsScreen)
	{
		m_pOutCard[nLocal]->SetGroupID(0,true);
	}else 
	{
		m_pOutCard[nLocal]->SetGroupID(1,TRUE);
	}
	if(LP_LEFT == nLocal)  //������--0
	{
		ptOut.x = 150 + nHandW;
		ptOut.y = nGuiH/2 - nOutH/2 ;
	}
	else if (LP_DOWN == nLocal)  //�����·�---�����Զ���Լ���λ��--1
	{
		ptOut.y = nGuiH - 60 - nHandH - nOutH;
		ptOut.x = nGuiW/2-nOutW/2;
	}
	else if (LP_RIGHT == nLocal) //�����ҷ�---2
	{
		ptOut.x = nGuiW   - 151 - nHandW - nOutW;
		ptOut.y = nGuiH/2 - nOutH/2;
	}
	else if (LP_UP == nLocal)   // �����Ϸ�---3
	{

	}
	m_pOutCard[nLocal]->SetStartPos(ptOut);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uiFrameworkInterface* uiGameArea::GetFrameworkInterface()
{
	return m_lpFramework;
}

SHORT uiGameArea::GetMaxPlayerCount()
{
	return MAX_PLAYER;
}


void uiGameArea::OnBtnFriend(UINT nSeat)
{
	IPLAYER* iPlayer = NULL;
	if (m_lpFramework->GetPlayingPlayerBySeat(LOCAL2SEAT(nSeat), iPlayer))
	{
		m_lpFramework->SendFriendOp(MAKEUSERID(iPlayer->getNumberID()), iPlayer->getNickName(), FRIENDOP_ADD);
	}
}



void uiGameArea::InitBomb()
{
	for (UINT nIndex = 0; nIndex < 10; nIndex++)
	{
		POINT ptLast = GetStartPos();
		POINT ptNew = {RandomLong(-10, 10), RandomLong(-10, 10)};
		_BombPoint.push_back(ptNew);
		_BombPoint.push_back(ptLast);
	}
}


void uiGameArea::PlayCardParticle(spiType _spiType, int x, int y)
{
	TPARTICLEINFO	pinfo;
	memset(&pinfo, 0, sizeof(pinfo));
	LoadParticle	ldmodel;
	ldmodel.model=LoadParticle::PT_LOAD_FILE;
	sprintf(ldmodel.texname, "GameArea.ImageShunStar");


	switch(_spiType)
	{
	case SPI_LEFTTORIGHT:
		sprintf(ldmodel.filename, "Psi\\LeftToRight.psi");
		break;
	case SPI_RIGHTTOLEFT:
		sprintf(ldmodel.filename, "Psi\\RightToLeft.psi");
		break;
	case SPI_GAMEWIN:
		sprintf(ldmodel.filename, "Psi\\GameWin.psi");
		break;
	default:
		break;
	}

	for(SHORT i = 0; i < MAX_PLAYER; i++)
	{
		if (i==0)
		{
			ldmodel.Quad_tx=96;
			ldmodel.Quad_ty=0;
			ldmodel.Quad_tw=32;
			ldmodel.Quad_th=32;

			pinfo.aColorStart.Set(0xFF79F6FF);
			pinfo.aColorEnd.Set(0xFFD2FBF7);
		} 
		else	if(i==1)
		{
			ldmodel.Quad_tx=32;
			ldmodel.Quad_ty=64;
			ldmodel.Quad_tw=32;
			ldmodel.Quad_th=32;

			pinfo.aColorStart.Set(0xFFFD91FF);
			pinfo.aColorEnd.Set(0xFFEFC7F1);
		}
		else	if(i==2)
		{
			ldmodel.Quad_tx=32;
			ldmodel.Quad_ty=32;
			ldmodel.Quad_tw=32;
			ldmodel.Quad_th=32;

			pinfo.aColorStart.Set(0xFFFCDD6E);
			pinfo.aColorEnd.Set(0xFFFFF8C1);
		}
		else	if(i==3)
		{
			ldmodel.Quad_tx=32;
			ldmodel.Quad_ty=96;
			ldmodel.Quad_tw=32;
			ldmodel.Quad_th=32;

			pinfo.aColorStart.Set(0xFFC739A2);
			pinfo.aColorEnd.Set(0xFFF5F103);
		}
	}
}

BOGY::DirectUI::HGUI uiGameArea::GetGUIHandle( string strName, SHORT nSeat /*= -1*/ )
{
	if (nSeat == -1) {
		return m_lpFramework->GetCfgGui("GV", strName); 
	}
	else {
		string strSeat = GetFormatString("%d", SEAT2LOCAL(nSeat));
		return m_lpFramework->GetCfgGui("GV", strName + strSeat);
	}
}

void uiGameArea::PlayGameEffct( LPCTSTR strKey, char crSex )
{	
	IPLAYER* pPlayer = GetPlayerByLocal(m_nLocalPowerSeat);
	if (!pPlayer) 
	{
		return ;
	}
	if (pPlayer->IsMale())   //����
	{
		PlayEffect(GetFormatString("game.snd_m_%s", strKey));
	}else if (pPlayer->IsFemale())//Ů��
	{
		PlayEffect(GetFormatString("game.snd_w_%s", strKey));
	}
}

TCARDS uiGameArea::ConvertCardVectorToCards(const TCardVector& dvCards)
{
	TCARDS vCards;
	for(size_t i=0; i<dvCards.size(); i++)
	{
		vCards.push_back(dvCards[i].GetID());
	}

	return vCards;
}

TCardVector uiGameArea::ConvertCardsToCardVector(const TCARDS& vCards)
{
	TCardVector dvCards;
	for(size_t i=0; i<vCards.size(); i++)
	{
		dvCards.push_back(TCards(vCards[i]));
	}

	return dvCards;
}

void uiGameArea::SortCards(TCARDS& vCards)
{
	TCardVector dvCards = ConvertCardsToCardVector(vCards);
	GameRule::SortCards(dvCards, false);
	vCards = ConvertCardVectorToCards(dvCards);
}

//��ʾ׼����־
void uiGameArea::ShowPlayReadySign(SHORT nSeat, BOOL bShow/* = TRUE*/)
{
	if ( nSeat == MAX_PLAYER )
	{		
		for ( short i = 0; i < MAX_PLAYER; i++ )
		{
			ShowPlayReadySign( i, bShow );
		}
		return ;
	}

	string strKey = GetFormatString("KW_PLAYER_READY_%d", nSeat);
	HGUI hGui     = m_lpFramework->GetCfgGui("",strKey);
	if (hGui == NULL)
	{
		ShowTalk(0xFF00FF00,"׼����ʶ��Դ��ȡʧ�ܣ����ͼ��ϵ�ͷ���");
	}
	if (RT_MATCH == m_nRoomType)
	{
		if (!hGui->DetachAnimate())
		{
			ShowTalk(0xFF00FF00,"׼����ʶ��Դ�ͷ�ʧ�ܣ����ͼ��ϵ�ͷ���");
		}
		if(!hGui->AttachAnimate("GameArea.SignReadyMatch",0,1))
		{
			ShowTalk(0xFF00FF00,"׼����ʶ��Դ����ʧ�ܣ����ͼ��ϵ�ͷ���");
		}
	}
	hGui->SetRender(bShow );
	ArrangeButtonPosition(GetGuiW(),GetGuiH());
}

//��ʾ����
void uiGameArea::ShowBase(BOOL bShow /*=TRUE*/,BYTE nAction)
{
	HGUI  hBCBack = m_lpFramework->GetCfgGui("GV","GameBaseCardBack");
	if (hBCBack)  
	{
		m_pBaseCards->SetRender(bShow);
		m_pBaseCards->SetCenter(hBCBack,0,-12);
		if (nAction == msgBase::NOACTION)
		{
		}else if (nAction == msgBase::TAKECARD)
		{
		}else if (nAction == msgBase::ADDHAND)
		{

		}
	}else
	{
		ShowTalk(0xFF00FF00,"���ƿ���Դ��ȡʧ�ܣ����ͼ��ϵ�ͷ�!");
	}
	
}

//��ʾ��������
void uiGameArea::ShowBaseCardsType(int nType,BOOL bShow/* = TRUE*/)
{
	if (nType == BC_ERROR) return ;
	HGUI  hBack = m_lpFramework->GetCfgGui("","BackMulKind");
	HGUI  hType = m_lpFramework->GetCfgGui("","GameBaseMulKind");
	if (hBack==NULL || hType == NULL) return ;
	int index = 0;
	hType->SetRender(TRUE);
	switch(nType)
	{
	case BC_NOCARDTYPE:				//ɢ��
		for (index =0; index<5;index++)
		{
			hType->GoAniFrame(index);
			hType->SetRender(FALSE);
		}
		hBack->SetRender(FALSE);
		break;

	case BC_HAVEBIGJOKEER:			//�д���
		hBack->SetRender(bShow);
		hType->GoAniFrame(0);
		hType->SetRender(bShow);
		ShowAni("AniDaWang",FALSE);
		break;

	case BC_FLUSH:					//ͬ��
		hBack->SetRender(bShow);
		hType->GoAniFrame(1);
		hType->SetRender(bShow);
		ShowAni("AniTongHua",FALSE);
		break;

	case BC_STRAIGHT:				//˳��
		hBack->SetRender(bShow);
		hType->GoAniFrame(2);
		hType->SetRender(bShow);
		ShowAni("AniShunZi",FALSE);
		break;

	case BC_BAOZI:					//����
		hBack->SetRender(bShow);
		hType->GoAniFrame(3);
		hType->SetRender(bShow);
		ShowAni("AniBaoZi",FALSE);
		break;

	case BC_STRAIGHTFLUSH:			//ͬ��˳
		hBack->SetRender(bShow);
		hType->GoAniFrame(4);
		hType->SetRender(bShow);
		ShowAni("AniTongHuaShun",FALSE);
		break;

	default:
		for (index =0; index<5;index++)
		{
			hType->GoAniFrame(index);
			hType->SetRender(FALSE);
		}
		hBack->SetRender(FALSE);
		break;
	}
}

//������нзֱ�ʶ
void uiGameArea::ClearAllCallSign()
{
	HGUI hGui = NULL;
	for (size_t i=0; i<MAX_PLAYER; ++i)
	{
		hGui = m_lpFramework->GetCfgGui("", GetFormatString("KW_SIGN_NOT_CALL_%d",i));
		if (!hGui)return;
		hGui->SetRender(FALSE);

		hGui = m_lpFramework->GetCfgGui("", GetFormatString("KW_SIGN_ONE_SCORE_%d",i));
		if (!hGui)return;
		hGui->SetRender(FALSE);

		hGui = m_lpFramework->GetCfgGui("", GetFormatString("KW_SIGN_TWO_SCORE_%d",i));
		if (!hGui)return;
		hGui->SetRender(FALSE);

		hGui = m_lpFramework->GetCfgGui("", GetFormatString("KW_SIGN_THREE_SCORE_%d",i));
		if (!hGui)return;
		hGui->SetRender(FALSE);
		
	}
}
// ��ʾ��������ر�־
void uiGameArea::ShowSignGrab(int nSeat, int nType, BOOL bShow /*= TRUE*/)
{
	HGUI hGui  = NULL;
	switch(nType)
	{
	case msgGrabLandlord::CALL:     //�е���
		hGui = m_lpFramework->GetCfgGui("", GetFormatString("KW_SIGN_CALL_BANKER_%d",nSeat));
		if (!hGui)return;
		hGui->SetRender(bShow);
		break;

	case msgGrabLandlord::NOTCALL:  //����
		hGui = m_lpFramework->GetCfgGui("", GetFormatString("KW_SIGN_NOT_CALL_%d",nSeat));
		if (!hGui)return;
		hGui->SetRender(bShow);
		break;

	case msgGrabLandlord::GRAB:     //������
		hGui = m_lpFramework->GetCfgGui("", GetFormatString("KW_SIGN_ROB_BANKER_%d",nSeat));
		if (!hGui)return;
		hGui->SetRender(bShow);
		break;

	case msgGrabLandlord::NOTGRAB:  //����
		hGui = m_lpFramework->GetCfgGui("", GetFormatString("KW_SIGN_NOT_ROB_%d",nSeat));
		if (!hGui)return;
		hGui->SetRender(bShow);
		break;
	case msgGrabLandlord::CALLSCORE://�з�
		if (0 == m_nCallScore)
		{
			hGui = m_lpFramework->GetCfgGui("", GetFormatString("KW_SIGN_NOT_CALL_%d",nSeat));
			if (!hGui)return;
			hGui->SetRender(bShow);
		}
		else if (1==m_nCallScore )
		{
			hGui = m_lpFramework->GetCfgGui("", GetFormatString("KW_SIGN_ONE_SCORE_%d",nSeat));
			if (!hGui)return;
			hGui->SetRender(bShow);
		}
		else if (2==m_nCallScore)
		{
			hGui = m_lpFramework->GetCfgGui("", GetFormatString("KW_SIGN_TWO_SCORE_%d",nSeat));
			if (!hGui)return;
			hGui->SetRender(bShow);
		}
		else if (3 == m_nCallScore )
		{
			hGui = m_lpFramework->GetCfgGui("", GetFormatString("KW_SIGN_THREE_SCORE_%d",nSeat));
			if (!hGui)return;
			hGui->SetRender(bShow);
		}
		break;
	default:
		break;
	}
}

//������Ч
void uiGameArea::PlayOutCardEffec(const CardTypeInfo &ctiOut, const TCardVector& dvOut)
{
	char buf[10] = {};
	int nSize = 0;
	PlayEffect("game.snd_out");
	int nValue = dvOut.front().GetValue();
	switch(ctiOut.nType)
	{
	case CT_SINGLE:
		PlayGameEffct(itoa(nValue,buf,10));
		break;

	case CT_DOUBLE:
		sprintf(buf,"%d%d",nValue,nValue);
		PlayGameEffct(buf);
		break;

	case CT_THREE:
		sprintf(buf,"%d%d%d",nValue,nValue,nValue);
		PlayGameEffct(buf);
		break;

	case CT_BOOM:
		PlayGameEffct("bomb");
		break;

	case  CT_MAXBOOM:
		PlayGameEffct("huojian");
		break;

	case CT_THREEWITHTWO:			//������
		PlayGameEffct("three_two");
		break;

	case CT_THREEWITHONE:			//����һ
		PlayGameEffct("three_one");
		break;

	case CT_MULTI:				//��˳
		PlayGameEffct("shunzi");
		break;

	case CT_MULTIDOUBLETWO:		//˫˳
		PlayGameEffct("duiduishun");
		break;

	case CT_MULTITHREE:		//��˳
		PlayGameEffct("sanshun");
		PlayEffect("game.snd_feiji_s");
		break;

	case CT_MULTITHREEWITHONE:	//С�ɻ�
		PlayGameEffct("feiji");
		PlayEffect("game.snd_feiji_s");
		break;

	case CT_MULTITHREEWITHTWO:   //��ɻ�
		PlayGameEffct("feiji");
		PlayEffect("game.snd_feiji_s");
		break;

	case CT_FOURWITHTWO:		//�Ĵ���(AAAA24)
		PlayGameEffct("four_two");
		break;

	case CT_FOURWITHDTWO:		//�Ĵ���(AAAA4477)
		PlayGameEffct("four_two");
		break;

	default: 
		break;
	}
}


//////////////////////////////////////////////////////////////////////////
char* uiGameArea::CharAddPoint(long8 nScore)
{
	char sz[256];
	memset(sz,0,sizeof(sz));
	sprintf(sz,"%I64d",nScore);

	char* pEnd = sz + strlen(sz) - 1;
	static char Byffer[256] = {0};
	char* pStart = Byffer;
	while (pEnd - sz > 3 || pEnd - sz == 3 && sz[0] != '-')
	{
		short i = 0;
		while(i<3)
		{
			pStart[i] = (pEnd - i)[0];
			i++;
		}
		pStart[i] = ',';
		pStart += 4;
		pEnd -= 3;
	}

	while(pEnd >= sz)
	{
		pStart[0] = pEnd[0];
		pStart++;
		pEnd--;
	}

	pStart[0] = 0;
	pStart = Byffer;
	pEnd = pStart + strlen(Byffer) - 1;
	while(pStart < pEnd)
	{
		pStart[0] ^= pEnd[0];
		pEnd[0] ^= pStart[0];
		pStart[0] ^= pEnd[0];
		pStart++;
		pEnd--;
	}

	return Byffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////