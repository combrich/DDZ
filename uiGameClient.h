#pragma once

#include "GameAreaDefine.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BaseGameImpl.h"
#include "../GCFWInterface.h"

using namespace GCFW;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "IUI/IClientView.h"
using namespace NewPlatform;
using namespace ClientInterface;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "SmartModule.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#include "ILogicEngine.h"
//#include "ClientSwitch.h"
//#include "UISwitch.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#include "LuaScript.h"
//#include "MahData.h"
class uiGameArea :
	public uiGameInterface, public BaseGameImpl, public IGameShell/*, public CClientSwitch, public CUISwitch, public CLuaScript, public CMahBaseFunc*/
{
	uiFrameworkInterface* m_lpFramework;
public:
	// ��Ϸ���ݰ�����
	virtual int OnReceivePacket(WORD nXYID, LPCSTR lpszPacket, WORD nSize);

	// �������ݰ�����
	virtual int OnReplayPacket(WORD nXYID, LPCSTR lpszPacket, WORD nSize, CONST ReplayStatus& _RS);

	// ��Ϸ����¼�������Ӧ
	virtual int OnNotifyMessage(UINT iCmd, WPARAM wParam, LPARAM lParam,string strType,LONG nID,string strKeyword);

	// ��Ϸ׼������������Ϣ
	virtual int OnSendChat(DWORD dwColor, LPCSTR lpszChat);

	// ��Ϸ׼����ʾ������Ϣ
	virtual int OnShowChat(DWORD dwColor, LPCSTR lpszChat);

	enum{
		PFID_SEAT = 0,
		PFID_LOCAL
	};
	// ��Ϸ������ݹ�����(�ɿ����������ж��Ƿ��������,�����������򷵻�0,���򷵻ط�0.)
	virtual int OnPlayerFilter(DWORD dwFlags, WPARAM wParam, LPARAM lParam, IPLAYER* lpPlayer);

	// ��Ϸ�������
	virtual int OnPlayerData(TUSERID nUserID, DWORD dwFlags);

	// ��Ϸ��ҽ���
	virtual int OnPlayerEnter(TUSERID nUserID, BOOL IsContinue);

	// ��Ϸ����뿪
	virtual int OnPlayerLeave(TUSERID nUserID, LPCSTR lpszCause);

	// ��Ϸ��ҿ�ʼ
	virtual int OnPlayerStart(TUSERID nUserID, DWORD dwReserved);

	// ��Ϸ��ҳ�ʱ
	virtual int OnPlayerTimer(TUSERID nUserID, DWORD dwPower, UINT nElapse);

	// ��Ϸ��ʱ��
	virtual int OnGameTimer(UINT_PTR nIDEvent, UINT nElapse);

	virtual int OnWindowProcEx(UINT iMsg, WPARAM wParam, LPARAM lParam);

	virtual BOOL GetVideoParam(TUSERID nUserID, TVideoParam& tVideoParam);

	virtual BOOL GetClientVersion(DWORD& dwVer);

	virtual int OnUpdateCfgFramework();

	// ��Ƶ���¼�������Ӧ
	virtual int OnVideoPanelMessage(UINT iMsg, TUSERID nUserID);

	// ʹ�õ�����ʾ
	virtual int OnHintUseProps(DWORD nFromBrandID, DWORD nFromNumberID,DWORD nToBrandID, DWORD nToNumberID, int nPropsID, int nCount,char szParam[]);

	// ������Ը���
	virtual int OnRptAttUpdate(DWORD nBrandID, DWORD nNumberID, int nCharmAdd, int nContributionAdd, BYTE nAttSize,int nAtt[10]);

	// ��ȡ��˭˵���ı�������0����Ĭ������
	virtual int GetChatToText(TUSERID nToUserID, string& strText);

	virtual LRESULT OnWindowDestroyEx();

	// �������ػ�Ӧ
	//virtual int OnRespSendTWritterMsg( BYTE nFlag, const string& strCaption, const string& strText, const string& strWeb );

	// ���Ѳ�����Ӧ
	//virtual int OnRespFriendOp( BYTE nFlag, int nOp, TUSERID nUserID, const string& strUserName, int nRel, const string& strCaption, const string& strText, const string& strWeb );

	//// ѡ����ҡ�nFlag��0�����ѡ�У�1���Ҽ�ѡ��
	//virtual int OnSelectUser( BYTE nFlag, TUSERID nUserID );

	//// ���ڴ������֪ͨ
	//virtual int OnFrameworkCreated( );

	////΢������:���غ�������������Ϣ
	//virtual int OnRespGetUserRecent( TUSERID nUserID, const string& strContent, const string& strImagePath );

public:
	uiGameArea(void);
	uiGameArea(uiFrameworkInterface* lpFramework);
	~uiGameArea(void);

	void Reset();

public:
	SHORT GetSeatByLocal(SHORT aLocal);
	SHORT GetLocalBySeat(SHORT aSeat);
	SHORT GetMySeatNo();
	IPLAYER* GetSelfPlayer();
	IPLAYER* GetPlayerByLocal(SHORT nLocal);
	IPLAYER* GetPlayerBySeat(SHORT nSeat);
	BYTE	GetWindowSizeType();

public:
	virtual BOOL OnCreate(LONG nGuiW, LONG nGuiH);
	//virtual BOOL OnRender(LONG xPos, LONG yPos, LONG wSize, LONG hSize);
	virtual BOOL OnUpdate(float dt);

	virtual void OnMouseMove(LONG nStartX, LONG nStartY);
	virtual void OnMouseLButton(LONG nStartX, LONG nStartY, UINT aState);
	virtual void OnMouseRButton(LONG nStartX, LONG nStartY, UINT aState);
	virtual void OnKeyMsg(UINT nChar, UINT nFlags, UINT aState);

	virtual void OnSize(LONG nGuiW, LONG nGuiH);
	virtual LRESULT OnChildCommand(HGUI hChild, UINT iCmd, WPARAM wParam, LPARAM lParam);
	virtual void OnChildDraging(HGUI hChild, LPPOINT lpPtWorld);



public:
	//void ArrangeLocalPosition(LONG nGuiW,LONG nGuiH,SHORT nLocal);				// ��Ϸ������
	//void ArrangeGUIPosition();
	void LoadConfig();
	void SaveConfig();
	BOOL IsSetMaxWindow();
	void SetWndTop(BOOL bTop);
private:
	TSETUPPARAM		m_tSetUpParam;
	BOOL			m_IsScreen;
	SHORT			m_nLocalTimer;						// �Ƿ���λ����ʱ
	int				m_ClockTime;						// ʱ��
	BYTE m_nGameStep;
	BYTE m_nTrust;
private:
	void ClearTable();								// ���������Ϣ
	void LoadGameConfig();
public:
	void OnGameTickTimer(DWORD dwTickNow);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	// ��Ϸ�߼�Э�����
	BEGIN_MSG_MAP_C()
		ON_MSG_MAP_C(GLXY_ACTION,OnMsgAction)	
	END_MSG_MAP_C()
			
	int OnMsgAction(LPCSTR pMsg,int nLen);


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	virtual int QueryInterface(/*[IN]*/const IID& riid, /*[OUT]*/void** lppInterface)
	{
		if (0 == lppInterface)
			return NPF_RESULT_INVALID_REF;

		Client::IClientUnknown* lpClientUnknown = 0;

		if (IsEqualGUID(riid, NPF_GET_INTERFACE_IID(Client::IClientUnknown)))
		{
			lpClientUnknown = (Client::IClientUnknown*)this;
		}
		else if (IsEqualGUID(riid, NPF_GET_INTERFACE_IID(ClientInterface::IGameShell)))
		{
			lpClientUnknown = (Client::IClientUnknown*)(ClientInterface::IGameShell*)this;
		}
		else
		{
			return NPF_RESULT_INVALID_IID;
		}

		if (lpClientUnknown == 0)
			return NPF_RESULT_NOTIMPL;
		lpClientUnknown->AddRef();

		*lppInterface = (void*)lpClientUnknown;

		return NPF_RESULT_SUCCEED;
	}

	virtual long Release(void)
	{
		return 1;
	}

	virtual long AddRef(void)
	{
		return 1;
	}

	virtual HWND GetSafeHwnd() const
	{
		return GetSafeWnd();
	}

	virtual LRESULT	Invoke(IGameUnknown* lpAsker, UINT nID, LPSTR lpszData, UINT nSize);
	SmartModule m_Module;

	Client::TCreateClientInterface m_fnClientInterface;
	ClientInterface::IGameView* m_lpGameView;
	HWND m_hWndClient;

public:
	template<class TEVENT>
		LRESULT Invoke(const TEVENT& eve)
	{
		if ( m_lpGameView == NULL )
		{
			return -1;
		}
		return m_lpGameView->Invoke(this, eve.EVENT_ID, (LPSTR)&eve, sizeof(eve));
	}

public:	
	BOOL LoadRes( const string& strDataPath, const string& strDataXML, const string& strResPath, const string& strPassword );
	BOOL CreateCfgGui( const string& strKey, const string& strCfgPath, const string& strNodeKey );
	BOOL SetShell();

public:
	BOOL SetZoom( BOOL bZoom );
		
};
