#pragma once
#include "stdafx.h"
#include "PokerDefine.h"
#include "uiNumber.h"

const UINT CMD_GAMEEND		= CMD_USER + 5000;
const UINT CMD_ENDPANELBLOG	= CMD_USER + 5001;
class uiGameEnd : public GUI
{
	DECLARE_RUNTIMECLASSINFO(uiGameEnd);
public:
	uiGameEnd(void);
	~uiGameEnd(void);
public:
	virtual BOOL OnCreate(LONG nGuiW, LONG nGuiH);
	virtual void OnSize(LONG nGuiW, LONG nGuiH);
	virtual LRESULT OnChildCommand(HGUI hChild, UINT iCmd, WPARAM wParam, LPARAM lParam);

	BOOL SetGameEndInfo(UINT nBankerSeat, UINT nSelfSeat, string name[], ulong8 nGameScore[], int iMul[],  int bRenWuWanCheng[], BOOL bXiaoFuXing[], BOOL bDaFuXing, BOOL bCanSendBlog);
	BOOL ClearGameEnd();
private:
	uiTextCtrl*	m_pUserName[MAX_PLAYER];		// 昵称
	uiTextCtrl*	m_pGameScore[MAX_PLAYER];		// 分数
	uiTextCtrl*	m_pGameMul[MAX_PLAYER];			// 倍数
	uiTextCtrl* m_pTextMul[msgMultData::COUNT];	// 翻倍
	uiTextCtrl* m_pMulName[msgMultData::COUNT];	// 翻倍名称
	uiTextCtrl* m_pTextTotalMul;				// 总翻倍
	uiTextCtrl* m_pTotalMulName;				// 总翻倍名称
	HGUI		m_pWinMask[MAX_PLAYER];			// 输赢标记
	HGUI		m_pXiaoFuShen[MAX_PLAYER];		// 小福神标记
	HGUI		m_pRenWuWanCheng[MAX_PLAYER];	// 任务完成标记

	HGUI		m_pDaFuShenEx;					// 大福神标记
	HGUI		m_pRenWuWanChengEx;				// 任务完成标记
	HGUI		m_pXiaoFuShenEx;				// 小福神标记
	HGUI		m_pAniEndFarmer;				// 农民赢动画
	HGUI		m_pAniEndBanker;				// 地主赢动画

	HGUI		m_pAniEndFarmerEx;				// 农民赢动画 春天
	HGUI		m_pAniEndBankerEx;				// 地主赢动画 春天

	uiButton*	m_pButtonClose;					// 关闭按钮
	uiButton*	m_pButtonSendBlog;				// 战绩分享
	HXFONT		m_hxFont;						// 字体

	UINT		m_nSelfSeat;					// 地主位置
	UINT		m_nBankerSeat;					// 地主位置
};
