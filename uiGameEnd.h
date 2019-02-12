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
	uiTextCtrl*	m_pUserName[MAX_PLAYER];		// �ǳ�
	uiTextCtrl*	m_pGameScore[MAX_PLAYER];		// ����
	uiTextCtrl*	m_pGameMul[MAX_PLAYER];			// ����
	uiTextCtrl* m_pTextMul[msgMultData::COUNT];	// ����
	uiTextCtrl* m_pMulName[msgMultData::COUNT];	// ��������
	uiTextCtrl* m_pTextTotalMul;				// �ܷ���
	uiTextCtrl* m_pTotalMulName;				// �ܷ�������
	HGUI		m_pWinMask[MAX_PLAYER];			// ��Ӯ���
	HGUI		m_pXiaoFuShen[MAX_PLAYER];		// С������
	HGUI		m_pRenWuWanCheng[MAX_PLAYER];	// ������ɱ��

	HGUI		m_pDaFuShenEx;					// ������
	HGUI		m_pRenWuWanChengEx;				// ������ɱ��
	HGUI		m_pXiaoFuShenEx;				// С������
	HGUI		m_pAniEndFarmer;				// ũ��Ӯ����
	HGUI		m_pAniEndBanker;				// ����Ӯ����

	HGUI		m_pAniEndFarmerEx;				// ũ��Ӯ���� ����
	HGUI		m_pAniEndBankerEx;				// ����Ӯ���� ����

	uiButton*	m_pButtonClose;					// �رհ�ť
	uiButton*	m_pButtonSendBlog;				// ս������
	HXFONT		m_hxFont;						// ����

	UINT		m_nSelfSeat;					// ����λ��
	UINT		m_nBankerSeat;					// ����λ��
};
