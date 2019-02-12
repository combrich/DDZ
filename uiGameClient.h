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
	// 游戏数据包处理
	virtual int OnReceivePacket(WORD nXYID, LPCSTR lpszPacket, WORD nSize);

	// 复盘数据包处理
	virtual int OnReplayPacket(WORD nXYID, LPCSTR lpszPacket, WORD nSize, CONST ReplayStatus& _RS);

	// 游戏框架事件处理响应
	virtual int OnNotifyMessage(UINT iCmd, WPARAM wParam, LPARAM lParam,string strType,LONG nID,string strKeyword);

	// 游戏准备发送聊天消息
	virtual int OnSendChat(DWORD dwColor, LPCSTR lpszChat);

	// 游戏准备显示聊天消息
	virtual int OnShowChat(DWORD dwColor, LPCSTR lpszChat);

	enum{
		PFID_SEAT = 0,
		PFID_LOCAL
	};
	// 游戏玩家数据过滤器(由开发者自行判断是否符合条件,若符合条件则返回0,否则返回非0.)
	virtual int OnPlayerFilter(DWORD dwFlags, WPARAM wParam, LPARAM lParam, IPLAYER* lpPlayer);

	// 游戏玩家数据
	virtual int OnPlayerData(TUSERID nUserID, DWORD dwFlags);

	// 游戏玩家进入
	virtual int OnPlayerEnter(TUSERID nUserID, BOOL IsContinue);

	// 游戏玩家离开
	virtual int OnPlayerLeave(TUSERID nUserID, LPCSTR lpszCause);

	// 游戏玩家开始
	virtual int OnPlayerStart(TUSERID nUserID, DWORD dwReserved);

	// 游戏玩家超时
	virtual int OnPlayerTimer(TUSERID nUserID, DWORD dwPower, UINT nElapse);

	// 游戏定时器
	virtual int OnGameTimer(UINT_PTR nIDEvent, UINT nElapse);

	virtual int OnWindowProcEx(UINT iMsg, WPARAM wParam, LPARAM lParam);

	virtual BOOL GetVideoParam(TUSERID nUserID, TVideoParam& tVideoParam);

	virtual BOOL GetClientVersion(DWORD& dwVer);

	virtual int OnUpdateCfgFramework();

	// 视频框事件处理响应
	virtual int OnVideoPanelMessage(UINT iMsg, TUSERID nUserID);

	// 使用道具提示
	virtual int OnHintUseProps(DWORD nFromBrandID, DWORD nFromNumberID,DWORD nToBrandID, DWORD nToNumberID, int nPropsID, int nCount,char szParam[]);

	// 玩家属性更新
	virtual int OnRptAttUpdate(DWORD nBrandID, DWORD nNumberID, int nCharmAdd, int nContributionAdd, BYTE nAttSize,int nAtt[10]);

	// 获取对谁说话文本，返回0采用默认设置
	virtual int GetChatToText(TUSERID nToUserID, string& strText);

	virtual LRESULT OnWindowDestroyEx();

	// 发送推特回应
	//virtual int OnRespSendTWritterMsg( BYTE nFlag, const string& strCaption, const string& strText, const string& strWeb );

	// 好友操作回应
	//virtual int OnRespFriendOp( BYTE nFlag, int nOp, TUSERID nUserID, const string& strUserName, int nRel, const string& strCaption, const string& strText, const string& strWeb );

	//// 选中玩家。nFlag：0，左键选中；1，右键选中
	//virtual int OnSelectUser( BYTE nFlag, TUSERID nUserID );

	//// 窗口创建完毕通知
	//virtual int OnFrameworkCreated( );

	////微博操作:返回好友最新推特信息
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
	//void ArrangeLocalPosition(LONG nGuiW,LONG nGuiH,SHORT nLocal);				// 游戏区布置
	//void ArrangeGUIPosition();
	void LoadConfig();
	void SaveConfig();
	BOOL IsSetMaxWindow();
	void SetWndTop(BOOL bTop);
private:
	TSETUPPARAM		m_tSetUpParam;
	BOOL			m_IsScreen;
	SHORT			m_nLocalTimer;						// 是否座位倒计时
	int				m_ClockTime;						// 时钟
	BYTE m_nGameStep;
	BYTE m_nTrust;
private:
	void ClearTable();								// 清除桌子信息
	void LoadGameConfig();
public:
	void OnGameTickTimer(DWORD dwTickNow);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	// 游戏逻辑协议分配
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
