#include "uiGameArea.h"
#include "resource.h"
#include "direct.h"

//void  uiGameArea::LoadGEConfig()
//{
//	LPCSTR lpStr = GetFullPath("GEConfig.ini");
//	MAX_PLAYER = GetPrivateProfileInt("GEDEMO","MAX_PLAYER",4,lpStr);
//	MIN_PLAYER = GetPrivateProfileInt("GEDEMO","MIN_PLAYER",4,lpStr);
//}


uiGameArea::uiGameArea(void)
: m_lpFramework(0)
{
	//LoadGEConfig();

	m_nTrust = 0;
	m_IsScreen = 1;

	m_nLocalTimer = MAX_PLAYER;
	Reset();
}

uiGameArea::uiGameArea(uiFrameworkInterface* lpFramework)
{
	//LoadGEConfig();
	m_lpFramework = lpFramework;

	m_nTrust = 0;
	m_IsScreen = 1;

	m_nLocalTimer = MAX_PLAYER;

	Reset();
}

uiGameArea::~uiGameArea(void)
{
	SaveConfig();

	if (m_lpGameView)
	{
		m_lpGameView->Release();
		m_lpGameView = NULL;
	}
}

void uiGameArea::Reset()
{
	m_lpGameView = NULL;
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
	//m_AniMode = m_tSetUpParam.IsAni;
	SwitchEffect(m_tSetUpParam.IsSnd);
	SetWndTop(m_tSetUpParam.IsWndTop);

}
void uiGameArea::SaveConfig()
{
	IniParser* ini = IniParser::Create( );
	if (ini)
	{	
		m_IsScreen = MAXWINDOW();
		ini->SetNumber("GAME","SND",m_tSetUpParam.IsSnd);
		ini->SetNumber("GAME","ANI",m_tSetUpParam.IsAni);
		ini->SetNumber("GAME","WNDTOP",m_tSetUpParam.IsWndTop);
		ini->SetNumber("GAME","WNDFLASH",m_tSetUpParam.IsWndFlash);
		/*ini->SetNumber("GAME","WNDJUMP",m_tSetUpParam.IsWndJump);*/
		ini->SetNumber("GAME","SCREEN",m_IsScreen);

		ini->SaveFile(GetFullPath("Config.ini"));
	
		ini->Release();
	}
}

BOOL uiGameArea::IsSetMaxWindow()
{
	return m_IsScreen;
}

void uiGameArea::SetWndTop(BOOL bTop)
{
	SetWindowPos(GetSafeWnd(),((bTop == TRUE) ? HWND_TOPMOST : HWND_NOTOPMOST),0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
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

	SetFocusStatus(GfsKeyboard);

	LoadGameConfig();


	if (!m_Module.Load("GameClient.dll"))
	{
		WriteLog(0, "加载模块失败");
	}
	else
	{
		Client::TCreateClientInterface CreateClientInterface;
		NPF_LOADFUNCTION((HMODULE)m_Module, CreateClientInterface);

		m_fnClientInterface = CreateClientInterface;
		m_lpGameView = 0;
		m_hWndClient = 0;

		if (0 != m_fnClientInterface(NPF_GET_INTERFACE_IID(ClientInterface::IGameView), (void**)&m_lpGameView) ||
			0 == m_lpGameView)
		{
			WriteLog(0, "创建接口失败");
			return FALSE;
		}

		CHAR szWorkDir[MAX_PATH];
		strcpy(szWorkDir,GetStartDir());
		if (m_lpGameView->Create("游戏区", nGuiW, nGuiH, GetSafeWnd(), szWorkDir))
		{

		}
		//// 设置回调接口
		//EVENT::BASIC::SetupSkin _SetupSkin;
		//_SetupSkin.nSize = 0;
		//m_lpGameView->Invoke(this, EVENT::BASIC::EVENT_ID_SETUPSKIN, (LPSTR)&_SetupSkin, sizeof(_SetupSkin));
		if ( LoadRes( GetFullPath( "Framework.Dat" ), "Framework.xml", GetFullPath( "Resource\\Framework.xml" ), "" ) == FALSE )
		{
			return FALSE;
		}
		if ( CreateCfgGui( "Shell", GetFullPath( "FrameworkConfig.xml" ), "Framework_0" ) == FALSE )
		{
			return FALSE;
		}
		if ( SetShell() == FALSE )
		{
			return FALSE;
		}
	}

	//////////////////////////////////////////////////////
	return TRUE;
}

void uiGameArea::OnMouseMove(LONG nStartX, LONG nStartY)
{
	uiGameInterface::OnMouseMove(nStartX,nStartY);
}

void uiGameArea::OnMouseLButton(LONG nStartX, LONG nStartY, UINT aState)
{
	int a = 1;
	a = a;
	if (GmsDown == aState)
	{

	}
	else if(GmsUp == aState)
	{
	}
}

void uiGameArea::OnMouseRButton(LONG nStartX, LONG nStartY, UINT aState)
{
	//try
	//{
	//	int ret = Call("OnGameAreaMouseRButton", CMD_MOUSERBUTTON, nStartX, nStartY, aState);
	//}
	//catch(...)
	//{

	//}
}

void uiGameArea::OnKeyMsg(UINT nChar, UINT nFlags, UINT aState)
{
	if (GksDown == aState)
	{
		if (nChar == VK_SPACE)
		{
			m_lpFramework->ShowChat(0xFFFFFFFF, "VK_SPACE");
		}
	}
}

void uiGameArea::OnSize(LONG nGuiW, LONG nGuiH)
{

	m_IsScreen = MAXWINDOW();

	int a = 0;

	m_lpGameView->Resize(0, 0, nGuiW, nGuiH, TRUE);
}



LRESULT uiGameArea::OnChildCommand(HGUI hChild, UINT iCmd, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//void uiGameArea::ArrangeGUIPosition()
//{
//
//}
//
//void uiGameArea::SetClockPos( BYTE aLocal )
//{
//
//}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

//void uiGameArea::OnButtonStart()
//{
//	m_nLocalTimer = MAX_PLAYER;
//}

void uiGameArea::OnGameTickTimer(DWORD dwTickNow)
{

}



void uiGameArea::LoadGameConfig()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int uiGameArea::OnMsgAction( LPCSTR pMsg,int nLen )
{

	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL uiGameArea::LoadRes( const string& strDataPath, const string& strDataXML, const string& strResPath, const string& strPassword )
{
	EVENT::BASIC::EventLoadRes eve;
	strcpy( eve.szDataPath, strDataPath.c_str() );
	strcpy( eve.szDataXML, strDataXML.c_str() );
	strcpy( eve.szResPath, strResPath.c_str() );
	strcpy( eve.szPassword, strPassword.c_str() );
	return Invoke( eve ) == 0;
}

BOOL uiGameArea::CreateCfgGui( const string& strKey, const string& strCfgPath, const string& strNodeKey )
{
	EVENT::BASIC::EventCreateCfgGui eve;
	strcpy( eve.szKey, strKey.c_str() );
	strcpy( eve.szXMLPath, strCfgPath.c_str() );
	strcpy( eve.szNodeKey, strNodeKey.c_str() );
	return Invoke( eve ) == 0;
}

BOOL uiGameArea::SetShell()
{
	EVENT::BASIC::EventSetShell eve;
	return Invoke( eve ) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL uiGameArea::SetZoom( BOOL bZoom )
{
	LONG nClientW, nClientH, nClientX, nClientY;

	RECT rcWork;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);

	if (bZoom)
	{
		nClientW = rcWork.right - rcWork.left;
		nClientH = rcWork.bottom - rcWork.top;
		nClientX = rcWork.left;
		nClientY = rcWork.top;
	}
	else
	{
		nClientW = 800;//g_BuilderCS.nClientW;
		nClientH = 600;//g_BuilderCS.nClientH;
		nClientX = rcWork.left + ( rcWork.right - rcWork.left - nClientW ) / 2;
		nClientY = rcWork.top + ( rcWork.bottom - rcWork.top - nClientH ) / 2;
	}
	FrameworkResize(nClientX, nClientY, nClientW, nClientH);

	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 边锋游戏基础框架
LRESULT uiGameArea::Invoke(IGameUnknown* lpAsker, UINT nID, LPSTR lpszData, UINT nSize)
{
	if ( lpszData == NULL )
	{
		return 0;
	}
	// 鼠标事件
	using namespace EVENT::RETURNELEMENT;
	using namespace EVENT::UIELEMENT;
	if (EVENT_ID_MOUSEBUTTON == nID)
	{
		MouseButton *lpMouseButton = (MouseButton*)lpszData;
		if (lpMouseButton && nSize == sizeof(MouseButton))
		{
			if(lpMouseButton->nIndex == MouseButton::MiLeft && lpMouseButton->nStatus == MouseButton::MsDown)
			{	
				//WriteLog(WRITELOG_OUTPUT, "右键按下");
				//lin
				FrameworkDrag();
			}
		}
	}
	// 按钮时间事件
	else if (EVENT_ID_CARDCLICKBUTTON == nID)
	{
	}
	else if(EVENT_ID_CLOCKOUTTIME == nID)
	{
		ClockOutTime *lpClockOutTime = (ClockOutTime*)lpszData;

		int a= 11;
	}
	else if (EVENT_ID_BOXCHECK == nID)
	{

	}
	else if (EVENT_ID_CLICKBUTTON == nID)
	{
	}

	else if (EVENT_ID_BLOG == nID)
	{
		Blog *lpBlog = (Blog*)lpszData;
	}
	//lin
	else if ( EVENT_ID_DROG == nID )
	{
		if ( lpAsker == m_lpGameView )
		{
			FrameworkDrag();
		}	
	}
	else if ( EVENT_ID_ZOOM == nID )
	{
		if ( lpAsker == m_lpGameView )
		{
			TZoom *lpEvent = (TZoom*)lpszData;
			SetZoom( lpEvent->bZoom );
		}
	}
	return 0;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////