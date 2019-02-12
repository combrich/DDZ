#include ".\uigameend.h"

IMPLEMENT_RUNTIMECLASSINFO(uiGameEnd, GUI)
uiGameEnd::uiGameEnd(void)
{
}

uiGameEnd::~uiGameEnd(void)
{
	XFontRelease(m_hxFont);
}
BOOL uiGameEnd::OnCreate(LONG nGuiW, LONG nGuiH)
{
	m_hxFont = XFontCreate("宋体",12,XFS_NORMAL);
	for(int i=0; i<MAX_PLAYER; i++)
	{
		m_pUserName[i] = new uiTextCtrl;
		if (NULL == m_pUserName[i] ||
			TRUE != m_pUserName[i]->Create("文本", 0, 0, this)) return FALSE;
		AppendChild(m_pUserName[i],0,0);
		m_pUserName[i]->SetFont(m_hxFont);
		m_pUserName[i]->SetTextColor(ARGB(255,147,73,0));
		m_pUserName[i]->SetRender(TRUE);
		m_pUserName[i]->SetLayout(TL_LEFT, TL_TOP);
		m_pUserName[i]->SetAutoSize(FALSE);
		m_pUserName[i]->SetEndEllipsis(TRUE);
		m_pUserName[i]->SetLineLimit(100);
		m_pUserName[i]->SetSize(100, 35);
		m_pUserName[i]->SetText("");
		m_pUserName[i]->SetStartPos(0, 0);
		m_pUserName[i]->SetCapture(FALSE);

		m_pGameScore[i] = new uiTextCtrl;
		if (NULL == m_pGameScore[i] ||
			TRUE != m_pGameScore[i]->Create("文本", 0, 0, this)) return FALSE;
		AppendChild(m_pGameScore[i],0,0);
		m_pGameScore[i]->SetFont(m_hxFont);
		m_pGameScore[i]->SetTextColor(ARGB(255,54,54,54));
		m_pGameScore[i]->SetRender(TRUE);
		m_pGameScore[i]->SetLayout(TL_LEFT, TL_TOP);
		m_pGameScore[i]->SetAutoSize(FALSE);
		m_pGameScore[i]->SetEndEllipsis(TRUE);
		m_pGameScore[i]->SetLineLimit(100);
		m_pGameScore[i]->SetSize(100, 35);
		m_pGameScore[i]->SetText("");
		m_pGameScore[i]->SetStartPos(0, 0);
		m_pGameScore[i]->SetCapture(FALSE);

		m_pGameMul[i] = new uiTextCtrl;
		if (NULL == m_pGameMul[i] ||
			TRUE != m_pGameMul[i]->Create("文本", 0, 0, this)) return FALSE;
		AppendChild(m_pGameMul[i],0,0);
		m_pGameMul[i]->SetFont(m_hxFont);
		m_pGameMul[i]->SetTextColor(ARGB(255,54,54,54));
		m_pGameMul[i]->SetRender(TRUE);
		m_pGameMul[i]->SetLayout(TL_LEFT, TL_TOP);
		m_pGameMul[i]->SetAutoSize(FALSE);
		m_pGameMul[i]->SetEndEllipsis(TRUE);
		m_pGameMul[i]->SetLineLimit(100);
		m_pGameMul[i]->SetSize(100, 35);
		m_pGameMul[i]->SetText("");
		m_pGameMul[i]->SetStartPos(0, 0);
		m_pGameMul[i]->SetCapture(FALSE);

		m_pWinMask[i] = new GUI;
		if (NULL == m_pWinMask[i] ||
			TRUE != m_pWinMask[i]->FromAnimate("输赢标记", "GC.WinMask", 0, 0, this))
		{
			DELETE_OBJECT(m_pWinMask[i]);
			return false;
		}
		if (!AppendChild(m_pWinMask[i],0,0))
			return false;
		m_pWinMask[i]->SetRender(FALSE);
		m_pWinMask[i]->SetCapture(FALSE);

		m_pXiaoFuShen[i] = new GUI;
		if (NULL == m_pXiaoFuShen[i] ||
			TRUE != m_pXiaoFuShen[i]->FromAnimate("小福神标记", "GC.XiaoFuShenMask_S", 0, 0, this))
		{
			DELETE_OBJECT(m_pXiaoFuShen[i]);
			return false;
		}
		if (!AppendChild(m_pXiaoFuShen[i],0,0))
			return false;
		m_pXiaoFuShen[i]->SetRender(FALSE);
		m_pXiaoFuShen[i]->SetCapture(FALSE);

		m_pRenWuWanCheng[i] = new GUI;
		if (NULL == m_pRenWuWanCheng[i] ||
			TRUE != m_pRenWuWanCheng[i]->FromAnimate("任务完成标记", "GC.RenWuWanChengMask_S", 0, 0, this))
		{
			DELETE_OBJECT(m_pRenWuWanCheng[i]);
			return false;
		}
		if (!AppendChild(m_pRenWuWanCheng[i],0,0))
			return false;
		m_pRenWuWanCheng[i]->SetRender(FALSE);
		m_pRenWuWanCheng[i]->SetCapture(FALSE);
	}

	m_pDaFuShenEx = new GUI;
	if (NULL == m_pDaFuShenEx ||
		TRUE != m_pDaFuShenEx->FromAnimate("大福神标记", "GC.DaFuShenMask_B", 0, 0, this))
	{
		DELETE_OBJECT(m_pDaFuShenEx);
		return false;
	}
	if (!AppendChild(m_pDaFuShenEx,0,0))
		return false;
	m_pDaFuShenEx->SetRender(FALSE);
	m_pDaFuShenEx->SetCapture(FALSE);

	m_pRenWuWanChengEx = new GUI;
	if (NULL == m_pRenWuWanChengEx ||
		TRUE != m_pRenWuWanChengEx->FromAnimate("大福神标记", "GC.RenWuWanChengMask_B", 0, 0, this))
	{
		DELETE_OBJECT(m_pRenWuWanChengEx);
		return false;
	}
	if (!AppendChild(m_pRenWuWanChengEx,0,0))
		return false;
	m_pRenWuWanChengEx->SetRender(FALSE);
	m_pRenWuWanChengEx->SetCapture(FALSE);

	m_pXiaoFuShenEx = new GUI;
	if (NULL == m_pXiaoFuShenEx ||
		TRUE != m_pXiaoFuShenEx->FromAnimate("小福神标记", "GC.XiaoFuShenMask_B", 0, 0, this))
	{
		DELETE_OBJECT(m_pXiaoFuShenEx);
		return false;
	}
	if (!AppendChild(m_pXiaoFuShenEx,0,0))
		return false;
	m_pXiaoFuShenEx->SetRender(FALSE);
	m_pXiaoFuShenEx->SetCapture(FALSE);

	for(int i=0; i<msgMultData::COUNT; i++)
	{
		m_pTextMul[i] = new uiTextCtrl;
		if (NULL == m_pTextMul[i] ||
			TRUE != m_pTextMul[i]->Create("文本", 0, 0, this)) return FALSE;
		AppendChild(m_pTextMul[i],0,0);
		m_pTextMul[i]->SetFont(m_hxFont);
		m_pTextMul[i]->SetTextColor(ARGB(255,4,4,4));
		m_pTextMul[i]->SetRender(TRUE);
		m_pTextMul[i]->SetLayout(TL_LEFT, TL_TOP);
		m_pTextMul[i]->SetAutoSize(FALSE);
		m_pTextMul[i]->SetEndEllipsis(TRUE);
		m_pTextMul[i]->SetLineLimit(450);
		m_pTextMul[i]->SetSize(450, 35);
		m_pTextMul[i]->SetText("");
		m_pTextMul[i]->SetStartPos(0, 0);
		m_pTextMul[i]->SetCapture(FALSE);

		m_pMulName[i] = new uiTextCtrl;
		if (NULL == m_pMulName[i] ||
			TRUE != m_pMulName[i]->Create("文本", 0, 0, this)) return FALSE;
		AppendChild(m_pMulName[i],0,0);
		m_pMulName[i]->SetFont(m_hxFont);
		m_pMulName[i]->SetTextColor(ARGB(255,147,73,0));
		m_pMulName[i]->SetRender(TRUE);
		m_pMulName[i]->SetLayout(TL_LEFT, TL_TOP);
		m_pMulName[i]->SetAutoSize(FALSE);
		m_pMulName[i]->SetEndEllipsis(TRUE);
		m_pMulName[i]->SetLineLimit(450);
		m_pMulName[i]->SetSize(450, 35);
		m_pMulName[i]->SetText("");
		m_pMulName[i]->SetStartPos(0, 0);
		m_pMulName[i]->SetCapture(FALSE);
	}

	m_pTextTotalMul = new uiTextCtrl;
	if (NULL == m_pTextTotalMul ||
		TRUE != m_pTextTotalMul->Create("文本", 0, 0, this)) return FALSE;
	AppendChild(m_pTextTotalMul,0,0);
	m_pTextTotalMul->SetFont(m_hxFont);
	m_pTextTotalMul->SetTextColor(ARGB(255,4,4,4));
	m_pTextTotalMul->SetRender(TRUE);
	m_pTextTotalMul->SetLayout(TL_LEFT, TL_TOP);
	m_pTextTotalMul->SetAutoSize(FALSE);
	m_pTextTotalMul->SetEndEllipsis(TRUE);
	m_pTextTotalMul->SetLineLimit(80);
	m_pTextTotalMul->SetSize(80, 35);
	m_pTextTotalMul->SetText("");
	m_pTextTotalMul->SetStartPos(0, 0);
	m_pTextTotalMul->SetCapture(FALSE);


	m_pTotalMulName = new uiTextCtrl;
	if (NULL == m_pTotalMulName ||
		TRUE != m_pTotalMulName->Create("文本", 0, 0, this)) return FALSE;
	AppendChild(m_pTotalMulName,0,0);
	m_pTotalMulName->SetFont(m_hxFont);
	m_pTotalMulName->SetTextColor(ARGB(255,147,73,0));
	m_pTotalMulName->SetRender(TRUE);
	m_pTotalMulName->SetLayout(TL_LEFT, TL_TOP);
	m_pTotalMulName->SetAutoSize(FALSE);
	m_pTotalMulName->SetEndEllipsis(TRUE);
	m_pTotalMulName->SetLineLimit(80);
	m_pTotalMulName->SetSize(80, 35);
	m_pTotalMulName->SetText("");
	m_pTotalMulName->SetStartPos(0, 0);
	m_pTotalMulName->SetCapture(FALSE);


	m_pAniEndFarmer = new GUI;
	if (NULL == m_pAniEndFarmer ||
		TRUE != m_pAniEndFarmer->FromAnimate("农民赢动画", "GC.AniFarmer", 0, 0, this))
	{
		DELETE_OBJECT(m_pAniEndFarmer);
		return false;
	}
	if (!AppendChild(m_pAniEndFarmer,0,0))
		return false;
	m_pAniEndFarmer->SetRender(FALSE);
	m_pAniEndFarmer->SetCapture(FALSE);

	m_pAniEndBanker = new GUI;
	if (NULL == m_pAniEndBanker ||
		TRUE != m_pAniEndBanker->FromAnimate("地主赢动画", "GC.AniBanker", 0, 0, this))
	{
		DELETE_OBJECT(m_pAniEndBanker);
		return false;
	}
	if (!AppendChild(m_pAniEndBanker,0,0))
		return false;
	m_pAniEndBanker->SetRender(FALSE);
	m_pAniEndBanker->SetCapture(FALSE);

	m_pAniEndFarmerEx = new GUI;
	if (NULL == m_pAniEndFarmerEx ||
		TRUE != m_pAniEndFarmerEx->FromAnimate("反春天动画", "GC.AniFanChunTian", 0, 0, this))
	{
		DELETE_OBJECT(m_pAniEndFarmerEx);
		return false;
	}
	if (!AppendChild(m_pAniEndFarmerEx,0,0))
		return false;
	m_pAniEndFarmerEx->SetRender(FALSE);
	m_pAniEndFarmerEx->SetCapture(FALSE);

	m_pAniEndBankerEx = new GUI;
	if (NULL == m_pAniEndBankerEx ||
		TRUE != m_pAniEndBankerEx->FromAnimate("春天动画", "GC.AniChunTian", 0, 0, this))
	{
		DELETE_OBJECT(m_pAniEndBankerEx);
		return false;
	}
	if (!AppendChild(m_pAniEndBankerEx,0,0))
		return false;
	m_pAniEndBankerEx->SetRender(FALSE);
	m_pAniEndBankerEx->SetCapture(FALSE);

	m_pButtonClose = new uiButton;
	if (!m_pButtonClose ||
		!m_pButtonClose->FromAnimate("关闭按钮", "GC.ButtonClose", this))
	{
		DELETE_OBJECT(m_pButtonClose);
		return false;
	}
	if (!AppendChild(m_pButtonClose,0,0))
	{
		return false;
	}
	m_pButtonClose->SetRender(TRUE);
	m_pButtonClose->SetEnabled(TRUE);
	m_pButtonClose->SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(32649)));

	m_pButtonSendBlog = new uiButton;
	if (!m_pButtonSendBlog ||
		!m_pButtonSendBlog->FromAnimate("发送按钮", "GC.ButtonEndSendBlog", this))
	{
		DELETE_OBJECT(m_pButtonSendBlog);
		return false;
	}
	if (!AppendChild(m_pButtonSendBlog,0,0))
	{
		return false;
	}
	m_pButtonSendBlog->SetRender(FALSE);
	m_pButtonSendBlog->SetEnabled(TRUE);
	m_pButtonSendBlog->SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(32649)));


	//GameScore nGameScore[3];
	//sprintf(nGameScore[0].cName, "玩家0号");
	//nGameScore[0].iScore = -6;
	//nGameScore[0].iMul = 8;

	//sprintf(nGameScore[1].cName, "玩家1号");
	//nGameScore[1].iScore = -6;
	//nGameScore[1].iMul = 8;

	//sprintf(nGameScore[2].cName, "玩家2号");
	//nGameScore[2].iScore = 12;
	//nGameScore[2].iMul = 8;

	//int Mul[10] = {0,0,0,0,0,0,0,0,0,0};
	//BOOL bXiaoFuXian[3] = {FALSE, TRUE, FALSE};
	//BOOL bRenWuWanCheng[3] = {FALSE, TRUE, FALSE};
	//BOOL bDaFuXian = TRUE;
	//SetGameEndInfo(2, 2, nGameScore, Mul, bXiaoFuXian, bRenWuWanCheng, bDaFuXian, FALSE);
	//SetDragDrop(TRUE);
	return TRUE;
}

void uiGameEnd::OnSize(LONG nGuiW, LONG nGuiH)
{
	for(int i=0; i<MAX_PLAYER; i++)
	{
		m_pUserName[i]->SetStartPos(107, 87+36*i);
		m_pWinMask[i]->SetStartPos(197, 87+36*i-5);
		m_pGameMul[i]->SetStartPos(268, 87+36*i);
		m_pGameScore[i]->SetStartPos(285, 87+36*i);
		m_pXiaoFuShen[i]->SetStartPos(300, 77+36*i);
		m_pRenWuWanCheng[i]->SetStartPos(325, 77+36*i);
	}

	m_pAniEndFarmer->SetStartPos(160, -75);
	m_pAniEndBanker->SetStartPos(160, -90);

	m_pAniEndFarmerEx->SetStartPos(160, -110);
	m_pAniEndBankerEx->SetStartPos(160, -110);

	m_pTotalMulName->SetStartPos(20, 58);
	m_pTextTotalMul->SetStartPos(65, 58);


	m_pButtonClose->SetStartPos(nGuiW-80, nGuiH-40);
	m_pButtonSendBlog->SetStartPos(nGuiW-180, nGuiH-40);

	m_pDaFuShenEx->SetStartPos(nGuiW-355, nGuiH-71);
	m_pXiaoFuShenEx->SetStartPos(nGuiW-355, nGuiH-71);
	m_pRenWuWanChengEx->SetStartPos(nGuiW-280, nGuiH-71);

	return;
}

LRESULT uiGameEnd::OnChildCommand(HGUI hChild, UINT iCmd, WPARAM wParam, LPARAM lParam)
{
	if(iCmd == CMD_BUTTON)
	{
		if(hChild == m_pButtonClose)
			ClearGameEnd();
		else if(hChild == m_pButtonSendBlog)
			SendChildCommand(CMD_ENDPANELBLOG, 0, 0);
		else{}
	}
	return 1;
}

BOOL uiGameEnd::SetGameEndInfo(UINT nBankerSeat, UINT nSelfSeat, string name[], ulong8 nGameScore[], int iMul[],  int bRenWuWanCheng[], BOOL bXiaoFuXing[], BOOL bDaFuXing, BOOL bCanSendBlog)
{
	m_nSelfSeat = nSelfSeat;
	m_nBankerSeat = nBankerSeat;

	m_pButtonSendBlog->SetRender(bCanSendBlog);
	for(int i=0; i<MAX_PLAYER; i++)
	{
		m_pUserName[i]->SetText(name[i].c_str());

		char lpGameInfo[100];
		memset(lpGameInfo, 0, sizeof(lpGameInfo));
		sprintf(lpGameInfo, "%d", nGameScore[i]);
		m_pGameScore[i]->SetText(lpGameInfo);

		//memset(lpGameInfo, 0, sizeof(lpGameInfo));
		//sprintf(lpGameInfo, "%d", nGameScore[i].iMul);
		m_pGameMul[i]->SetText("");
		if(nGameScore[i] > 0)
		{
			m_pWinMask[i]->GoAniFrame(0);
			m_pWinMask[i]->SetRender(TRUE);
		}
		else
		{
			m_pWinMask[i]->GoAniFrame(1);
			m_pWinMask[i]->SetRender(TRUE);
		}
		m_pXiaoFuShen[i]->SetRender(bXiaoFuXing[i]);
		m_pRenWuWanCheng[i]->SetRender(bRenWuWanCheng[i]);
	}


	m_pRenWuWanChengEx->SetRender(bRenWuWanCheng[m_nSelfSeat]);
	m_pXiaoFuShenEx->SetRender(bXiaoFuXing[m_nSelfSeat]);
	m_pDaFuShenEx->SetRender(bDaFuXing);
	
	if(nGameScore[nBankerSeat] > 0)
	{
		if(iMul[msgMultData::CHUNTIANMULT] > 0)
		{
			m_pAniEndBankerEx->SetRender(TRUE);
			m_pAniEndBankerEx->PlayAni(-1);
		}
		else
		{
			m_pAniEndBanker->SetRender(TRUE);
			m_pAniEndBanker->PlayAni(-1);
		}
	}
	else if(nGameScore[nBankerSeat] < 0)
	{		
		if(iMul[msgMultData::FANCHUNMULT] > 0)
		{
			m_pAniEndFarmerEx->SetRender(TRUE);
			m_pAniEndFarmerEx->PlayAni(-1);
		}
		else
		{
			m_pAniEndFarmer->SetRender(TRUE);
			m_pAniEndFarmer->PlayAni(-1);
		}
	}
	else{}

	LPSTR lpMulName[msgMultData::COUNT] = {"抢地主", "炸弹", "春天", "反春天", "加倍", "明牌"};
	char lpMulStr[200];
	char lpTotalMulStr[200];
	int iTotalMul = 1;
	int index = 0;
	memset(lpMulStr, 0, sizeof(lpMulStr));
	for(int i=0; i<msgMultData::COUNT; i++)
	{
		if(iMul[i] > 0)
		{
			m_pMulName[index]->SetText(lpMulName[i]);
			sprintf(lpMulStr, "×%d", iMul[i]);
			m_pTextMul[index]->SetText(lpMulStr);
			m_pMulName[index]->SetStartPos(20, 80+index*23);
			m_pTextMul[index]->SetStartPos(65, 80+index*23);
			index++;
			iTotalMul *= iMul[i];
		}
	}
	, "特殊底"
	sprintf(lpTotalMulStr, "×%d", iTotalMul);
	if(iTotalMul != 0)
	{
		m_pTotalMulName->SetText("总倍数");
		m_pTextTotalMul->SetText(lpTotalMulStr);
	}
	//BringToTop();
	SetRender(TRUE);
	return TRUE;
}

BOOL uiGameEnd::ClearGameEnd()
{
	for(int i=0; i<MAX_PLAYER; i++)
	{
		m_pUserName[i]->SetText("");
		m_pGameScore[i]->SetText("");
		m_pGameMul[i]->SetText("");
		m_pWinMask[i]->SetRender(FALSE);
		m_pXiaoFuShen[i]->SetRender(FALSE);
		m_pRenWuWanCheng[i]->SetRender(FALSE);
	}
	for(int i=0; i<msgMultData::COUNT; i++)
	{
		m_pTextMul[i]->SetText("");
		m_pMulName[i]->SetText("");
	}

	m_pDaFuShenEx->SetRender(FALSE);
	m_pRenWuWanChengEx->SetRender(FALSE);
	m_pXiaoFuShenEx->SetRender(FALSE);

	m_pTextTotalMul->SetText("");
	m_pAniEndBanker->SetRender(FALSE);
	m_pAniEndBanker->PauseAni(TRUE);
	m_pAniEndFarmer->SetRender(FALSE);
	m_pAniEndFarmer->PauseAni(TRUE);

	m_pAniEndBankerEx->SetRender(FALSE);
	m_pAniEndBankerEx->PauseAni(TRUE);
	m_pAniEndFarmerEx->SetRender(FALSE);
	m_pAniEndFarmerEx->PauseAni(TRUE);
	SetRender(FALSE);
	return TRUE;
}