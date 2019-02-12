#pragma once

#include "GameAreaDefine.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "GameRule.h"
#include "../GCFWInterface.h"
#include "BaseGameImpl.h"
#include "uiPlayerFace.h"
#include "uiParticle.h"
#include "uiHeadPortrait.h"
#include "uiHandCards.h"
#include "ClockUI.h"
#include "GameTrusteeship.h"
#include "uiMakeCardPanel.h"
#define CHECK_BIT(value,bit)	((value & bit) == bit)

using namespace GCFW;

class uiGameArea :
	public uiGameInterface, public BaseGameImpl 
{
	uiFrameworkInterface* m_lpFramework;
///////////////////////////////////////////////////////////////////////////////////////////////////////
// 发送流形式的协议
public:
	template <class XieYi>
		void SendStreamPacket(XieYi& xy)
	{
		unsigned short nSendSize = sizeof(xy);

		bostream bos;
		bos.attach(m_sSendBuffer,nSendSize);
		bos << xy;
		m_nSendSize = (unsigned short)bos.length();

		if ( m_lpFramework )
		{
			m_lpFramework->SendPacket( xy.XY_ID,m_sSendBuffer, m_nSendSize );
		}	
	}

	// 发送缓冲与发送长度
	CHAR	m_sSendBuffer[SOCKETBUFFER_LEN];
	WORD	m_nSendSize;

///////////////////////////////////////////////////////////////////////////////////////////////////////
// 游戏区接口
public:
	// 游戏数据包处理
	virtual int OnReceivePacket(WORD nXYID, LPCSTR lpszPacket, WORD nSize);

	// 复盘数据包处理
	virtual int OnReplayPacket(WORD nXYID, LPCSTR lpszPacket, WORD nSize, CONST ReplayStatus& _RS);

	// 游戏框架事件处理响应
	virtual int OnNotifyMessage(UINT iCmd, WPARAM wParam, LPARAM lParam,const string& strType, LONG nID, const string& strKeyword);

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

	virtual int OnUpdateCfgFramework();

	virtual BOOL GetVideoParam(TUSERID nUserID, TVideoParam& tVideoParam);

	virtual BOOL GetClientVersion(DWORD& dwVer);

	// 视频框事件处理响应
	virtual int OnVideoPanelMessage(UINT iMsg, TUSERID nUserID);

	// 使用道具提示
	virtual int OnHintUseProps(DWORD nFromBrandID, DWORD nFromNumberID,DWORD nToBrandID, DWORD nToNumberID, int nPropsID, int nCount,char szParam[]);

	// 玩家属性更新
	virtual int OnRptAttUpdate(DWORD nBrandID, DWORD nNumberID, int nCharmAdd, int nContributionAdd, BYTE nAttSize,int nAtt[10]);

	// 获取对谁说话文本，返回0采用默认设置
	virtual int GetChatToText(TUSERID nToUserID, string& strText);

	virtual LRESULT OnWindowDestroyEx();

	// 获取系统消息颜色
	virtual DWORD GetFrameworkChatColor();

	virtual int OnWillSendPlayerChat( DWORD& dwColor, string& strChatText );

	// 窗口创建完毕通知
	virtual int OnFrameworkCreated( );

	//微博操作:返回好友最新推特信息
	virtual int OnRespGetUserRecent( TUSERID nUserID, const string& strContent, const string& strImagePath );

	// 收到玩家头像更新（USERID, 头像尺寸类型, 头像图片路径 ）
	virtual int OnRespPlayerFaceFrame( TUSERID nUserID, int nType, const string& strPath );

	// 好友操作回应
	virtual int OnRespFriendOp( BYTE nFlag, int nOp, TUSERID nUserID, const string& strUserName, int nRel, const string& strCaption, const string& strText, const string& strWeb );
	//换桌
	virtual int OnRespChangeSeatInTable( DWORD nNumber1, BYTE nSeat1,DWORD nNumber2, BYTE nSeat2 );
	virtual int	OnRespRoomInfo(int room, const string& strTitle );
public:
	uiGameArea(void);
	uiGameArea(uiFrameworkInterface* lpFramework);
	~uiGameArea(void);

	void Reset();

public:
	SHORT GetSeatByLocal(SHORT aLocal);
	SHORT GetLocalBySeat(SHORT aSeat);
	SHORT GetMySeatNo();
	BYTE  GetWindowSizeType();

	IPLAYER* GetSelfPlayer();
	IPLAYER* GetPlayerByLocal(SHORT nLocal);
	IPLAYER* GetPlayerBySeat(SHORT nSeat);

public:
	virtual BOOL OnCreate(LONG nGuiW, LONG nGuiH);
	virtual BOOL OnUpdate(float dt);

	virtual void OnMouseMove(LONG nStartX, LONG nStartY);
	virtual void OnMouseLButton(LONG nStartX, LONG nStartY, UINT aState);
	virtual void OnMouseRButton(LONG nStartX, LONG nStartY, UINT aState);
	virtual void OnKeyMsg(UINT nChar, UINT nFlags, UINT aState);
	virtual	int  OnFrameworkSize();

	virtual void OnSize(LONG nGuiW, LONG nGuiH);
	virtual void OnChildDraging(HGUI hChild, LPPOINT lpPtWorld);
	virtual LRESULT OnChildCommand(HGUI hChild, UINT iCmd, WPARAM wParam, LPARAM lParam);

public:
	void LoadConfig();
	void SaveConfig();
	BOOL IsSetMaxWindow();
	void SetWndTop(BOOL bTop);

private:
	TSETUPPARAM		m_tSetUpParam;
	msgLogicConfig  m_LogicConfig;
	msgLogicData	m_LogicData;

	BOOL			m_IsScreen;
	BOOL			m_IsAreaMax;
	BOOL			m_IsTop;

	BYTE	        m_nBankerSeat;
	BYTE			m_nGameState;

	SHORT		m_nLocalPowerSeat;					//有权限的玩家位置
	size_t			m_nSelfPower;								//自己拥有的权限,每当获得权限的时候，为这个值复制。权限移交后应该自己移除自己的权限
	LONG			m_nBasePlayScore;					//底分
	string			m_strHelpURL;
private:
	void ClearTable();												// 清除桌面信息
	void LoadGameConfig();
public:
	void OnGameTickTimer(DWORD dwTickNow);
//////////////////////////////////////////////////////////////////////////
//权限相关
	void GetPowerGrab(msgPower& power);
	void GetPowerPlay(msgPower& power);

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//位置相关
	POINT m_ptHand[MAX_PLAYER];				// 手牌摆放的坐标
	void ArrangeLocalPosition(LONG nGuiW,LONG nGuiH,SHORT nLocal);   //调整图片显示位置
	void ArrangeButtonPosition(LONG nGuiW,LONG nGuiH);				 // 调整按钮位置，
	void ArrangeClockPosition(LONG nGuiW,LONG nGuiH, short nLocal);				 // 调整时钟位置
	void ArrangeOutCard(LONG nGuiW, LONG nGuiH, SHORT nLocal);
	void ArrangePoint(int nBase ,int nMult);		//调整底分和倍数的位置
	void ArrangePlayBtn();		//调整出牌相关按钮
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	// 游戏逻辑协议分配
	BEGIN_MSG_MAP_C()
		ON_MSG_MAP_C(GLXY_TABLERULE,OnMsgTableRule)
		ON_MSG_MAP_C(GLXY_ENTERGAME,OnMsgEnterGame)		 // 进入游戏
		ON_MSG_MAP_C(GLXY_STARTGAME,OnMsgStartGame)		 // 开始游戏
		ON_MSG_MAP_C(GLXY_GAMESTATE,OnMsgGameState)	
		ON_MSG_MAP_C(GLXY_GRABLANDLORD,OnMsgGrab)
		ON_MSG_MAP_C(GLXY_OPENCARD,OnMsgOpenCard)
		ON_MSG_MAP_C(GLXY_OUTCARD,OnMsgOutCards)
		ON_MSG_MAP_C(GLXY_RELINK,OnMsgReLink)			 // 重连
		ON_MSG_MAP_C(GLXY_LOGICCONFIG,OnMsgLogicConfig)  
		ON_MSG_MAP_C(GLXY_LOGICDATA,OnMsgLogicData)		
		ON_MSG_MAP_C(GLXY_ENDSENDCARD,OnMsgEndSendCard)
		ON_MSG_MAP_C(GLXY_GAMEDATA,OnMsgGameData)
		ON_MSG_MAP_C(GLXY_BASE,OnMsgBase)	
		ON_MSG_MAP_C(GLXY_CARDS,OnMsgCards)	
		ON_MSG_MAP_C(GLXY_POWER,OnMsgPower)
		ON_MSG_MAP_C(GLXY_CLOCK,OnMsgClock)
		ON_MSG_MAP_C(GLXY_ENDGAME,OnMsgEndGame)
		ON_MSG_MAP_C(GLXY_QIAOPIHUA,OnMsgQiaoPiHua)
		ON_MSG_MAP_C(GLXY_TAKE,OnMsgTake)
		ON_MSG_MAP_C(GLXY_EXITPROCRES,OnMsgExitProcRes)
		ON_MSG_MAP_C(GLXY_ACTION,OnMsgAction)
		ON_MSG_MAP_C(GLXY_PLAYERIDENTITY,OnMsgIdentity)
		ON_MSG_MAP_C(GLXY_ADDMULTIPLE,OnMsgAddMul)
		ON_MSG_MAP_C(GLXY_MISSION,OnMission)
		ON_MSG_MAP_C(GLXY_TRUST,OnMsgTrush)
	END_MSG_MAP_C()

	int OnMsgTrush(LPCSTR pMsg, int nLen);
	int OnMsgIdentity(LPCSTR pMsg, int nLen);
	int OnMsgExitProcRes(LPCSTR pMsg,int nLen);
	int OnMsgTake(LPCTSTR pMsg, int nLen );
	int OnMsgQiaoPiHua(LPCTSTR pMsg, int nLen );
	int OnMsgOutCards(LPCSTR pMsg, int nLen);
	int OnMsgOpenCard(LPCSTR pMsg, int nLen);
	int OnMsgGrab(LPCSTR pMsg, int nLen);
	int OnMsgTableRule(LPCSTR pMsg,int nLen);
	int OnMsgEnterGame(LPCSTR pMsg,int nLen);
	int OnMsgStartGame(LPCSTR pMsg,int nLen);
	int OnMsgBase(LPCSTR pMsg,int nLen);
	int OnMsgCards(LPCSTR pMsg,int nLen);
	int OnMsgAction(LPCSTR pMsg, int nLen);

	int OnMsgReLink(LPCSTR pMsg,int nLen);
	int OnMsgLogicConfig(LPCSTR pMsg,int nLen);
	int OnMsgLogicData(LPCSTR pMsg, int nLen);
	int OnMsgEndSendCard(LPCSTR pMsg,int nLen);
	int OnMsgGameState(LPCSTR pMsg,int nLen);
	int OnMsgGameData(LPCSTR pMsg, int nLen);
	int OnMsgPower(LPCSTR pMsg, int nLen);
	int OnMsgClock(LPCSTR pMsg, int nLen);
	int OnMsgEndGame(LPCSTR pMsg,int nLen);
	int OnMsgAddMul(LPCSTR pMsg, int nLen);
	int OnMission(LPCSTR pMsg, int nLen);
//////////////////////////////////////////////////////////////////////////
//消息事件
private:
	int  OnBtnGrabLandlord(const string& strKeyword);
	int  OnBtnPlayCard(const string& strKeyword);
	//动画结束
	int  OnNotifyPlayFinish(WPARAM wParam, LPARAM lParam,const string& strType, LONG nID, const string& strKeyword);
	//移动结束
	int  OnNotifyMoveFinish(WPARAM wParam, LPARAM lParam,const string& strType, LONG nID, const string& strKeyword);
	//鼠标左键按下
	int  OnNotifyMouseLButton(WPARAM wParam, LPARAM lParam,const string& strType, LONG nID, const string& strKeyword);
	//鼠标进入
	int  OnNotifyMouseEnter(WPARAM wParam, LPARAM lParam,const string& strType, LONG nID, const string& strKeyword);
	//按钮响应
	int  OnNotifyButton(WPARAM wParam, LPARAM lParam,const string& strType, LONG nID, const string& strKeyword);
//按钮响应
	int  OnBtnColse();					//关闭客户端
	int  OnBtnMaxArea();			//侧边栏关闭
	int  OnBtnResizeArea();		//侧边栏打开
	int  OnBtnTop();						//窗口置顶
	int  OnBtnUnTop();				//窗口取消置顶
	int  OnBtnHelp();						//帮助
	int OnBtnGiveUp();				//放弃按钮
	int OnBtnOutCard();				//出牌按钮
	int OnBtnInfo();						//提示按钮
	int OnBtnOpenCard();			//明牌按钮
	int OnBtnChangeTable();	//换桌
	int OnBtnMusicOn();			//打开音效
	int OnBtnMusicOff();			//关闭音效
	int OnBtnCloseOK();			//窗口关闭确认
	int OnBtnCloseCancle();	//窗口关闭取消
	int OnBtnSpeak();					//发言按钮
	int OnBtnSet();						//设置按钮
	int OnBtnSetOK();					//设置确定 
	int OnBtnSetCancel();		//设置取消
	int OnEndBtnClose();			//结算关闭
	int OnBtnRunawayOK();	//逃跑确认
	int OnBtnSignUpFailure();//报名失败
	int OnBtnTrush();					//托管
	int OnBtnUnTrush();			//取消托管
	
	//仅限测试
	int OnBtnMakeCard();			//做牌
	int OnBtnMakeBase();		//做底牌
	int OnBtnEndMakeCard();//结束做牌
	int nMakeCardType;
	uiMakeCardPanel* m_uipMakeCards;					//做牌面板
	clock_t		m_nLBtnNow;		//鼠标单击现在按下的时间
	clock_t		m_nLBtnOld;		//鼠标单机上一次按下的时间
private:
	int				m_nLandlord;		//地址位置
	size_t		m_nRoomType;	//房间类型
	size_t		m_nGameType;	//游戏类型
	size_t		m_nGrabLandType;	//抢地主方式
	bool			m_bIsCallLandlord;	//是否是叫地主
	bool			m_bIsFirstOut;			//是否首轮出牌

	bool			m_bNotOutIsShow[MAX_PLAYER];			// 不出标志是否显示
	TCARDS m_BaseCards;										//底牌
	TCARDS m_HandCards[MAX_PLAYER];	//手牌
	TCARDS m_PromptCard;									// 提示玩家的出牌
	SHORT	 m_nCountPrompt;						//提示玩家出牌的起始牌位置
	bool			m_bIsAlreadyOut;				//玩家是否已经出牌
	bool			m_bIAmSeeing;					//自己是否是旁观者
	bool			m_bSpeakPlay;
	size_t		m_nTotalMult;						//总倍数
	int				m_nCallScore;					//叫分倍数 

public:
	TCARDS ConvertCardVectorToCards(const TCardVector& dvCards);
	TCardVector ConvertCardsToCardVector(const TCARDS& vCards);
	void SortCards(TCARDS& vCards);
//////////////////////////////////////////////////////////////////////////
//通讯相关
	void SendGrabLandlord(int nType);
	void SendOutCard(int nType);
///////////////////////////////////////////////////////////////////////////////////////////////////////
// 转接模块接口
public:
	virtual SHORT GetMaxPlayerCount();
	uiFrameworkInterface* GetFrameworkInterface();
///////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	void uiGameArea::OnBtnFriend(UINT nSeat);
public:
	enum spiType
	{
		SPI_RIGHTTOLEFT = 0,
		SPI_LEFTTORIGHT,
		SPI_GAMEWIN,
		SPI_COUNT
	};

	// 图形显示
	std::list<POINT> _BombPoint;
	// 显示准备标志
	void ShowPlayReadySign(SHORT nSeat, BOOL bShow = TRUE);
	//底牌的显示
	void ShowBase(BOOL bShow = TRUE, BYTE nActionType = msgBase::NOACTION);
	void InitBomb();
	void PlayCardParticle(spiType _spiType, int x, int y);
	HGUI GetGUIHandle(string strName, SHORT nSeat = -1);
	void ShowGrabBtn(bool IsCall, BOOL bShow /* = TRUE */,int nCall=0);
	// 显示抢地主相关标志
	void ShowSignGrab(int nSeat, int nType, BOOL bShow = TRUE);
	void ClearAllCallSign();
	void ShowAllPlayBtn(BOOL bShow,BOOL bGiveUp, BOOL bOutCard, BOOL bInfo, BOOL bOpenHand=FALSE);
	// 显示打出的牌
	int ShowOutCard(int nSeat, BOOL bShow = TRUE);
	// 显示不出标志
	void ShowNotOutSign(int nSeat, BOOL bShow = TRUE,bool bShowEx = false);
	void ShowAni(LPCTSTR strKey,bool IsOut =true, SHORT nSeat = -1,SHORT loop = 1, bool bShow = true);
	// 显示底牌牌型
	void ShowBaseCardsType(int nType,BOOL bShow = TRUE);
	// 关闭所有与出牌相关的按钮
	void ClearAllPlayBtn(void);
	void ShowSignLandlord(BOOL bShow = TRUE);
	void ShowAllMult(int nMult, BOOL bShow = TRUE);
	void ShowBasePoint(LONG nScore, BOOL bShow = TRUE);
	void ShowHandCount(SHORT nCount,SHORT nSeat, BOOL bShow = TRUE);
	//托管按钮
	void ShowBtnTrush(bool bShow = true);
	//显示任务
	enum 
	{
		ST_NONE,
		ST_MAX,
		ST_MIN,
	};
	void ShowMission(int nCardType ,int nShowType = ST_MIN, bool bShow = true);
//////////////////////////////////////////////////////////////////////////
//UI
	uiHandCards*		m_pHandCards[MAX_PLAYER];		//手牌		
	uiHandCards*		m_pOutCard[MAX_PLAYER];			//玩家打出的牌
	uiHandCards*		m_pBaseCards;		//底牌
	ClockUI*					m_pClockUI;
	uiHeadPortrait*	m_pHeadPortrait[MAX_PLAYER];	// 玩家形象
	
	TCardVector		m_dvOutCards;				//本轮玩家的出牌
	TCardVector		m_dvFrontOutCards;			//上一玩家的出牌
	bool							m_nBaseType;				//底牌显示牌型
//////////////////////////////////////////////////////////////////////////
//动态UI创建
private:
	bool CreateBase();
	bool CreateClock();
	bool CreateHand();
	bool CreateHead();
	bool CreateOut();
///////////////////////////////////////////////////////////////////////////////////////////////////////
public:
// 声音
	void PlayGameEffct(LPCTSTR strKey, char crSex = '5');
	BOGY::Direct2D::HCHANNEL m_hGameSound;
	void PlayOutCardEffec(const CardTypeInfo &cti,const TCardVector& dvOut);
///////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
private:
	char* CharAddPoint(long8 nScore);
	//可否出牌
	bool CanOut(bool bIsFirst = FALSE);
	bool GetGreaterThanNextOut(CardTypeInfo ctiFrontOut);						//获取玩家手牌中大于指定牌型的牌组
	//获取玩家手牌中拥有的基础牌型，基础牌型为单张、对子、三张、炸弹
	//并将获取到的基础牌型放入指定的容器中
	void FillBaseCards();			
	TCARDS GetBaseCards(int nCardsLen, int nType, TCARDS handCards);
	TCARDS GetBaseCardsEx(int nCardsLen, int nType, TCARDS handCards,TCARDS needReject);
	//手牌中是否有可以打出的牌
	bool HaveOutCard(int nCardsLen, CardTypeInfo ctiFrontOut ,TCARDS handCards,int nStep = 1);		
	bool HaveOutCardBoom(int nCardsLen, CardTypeInfo ctiFrontOut ,TCARDS handCards,int nStep = 1);	
	bool HaveOutCardMaxBoom(int nCardsLen, CardTypeInfo ctiFrontOut ,TCARDS handCards,int nStep = 1);	
	bool HaveOutCardEx(int nCardsLen,CardTypeInfo ctiFrontOut,TCARDS handCards);
private:
	//////////////////////////////////////////////////////////////////////////
	//存放的方式为从小到大连续存放
	TCARDS			m_PromptSingle;						//玩家手牌中拥有的所有单牌
	TCARDS			m_PromptDouble;						//玩家手牌中拥有的所有对子
	TCARDS			m_PromptThree;						//玩家手牌中所有的三张
	TCARDS			m_PromptBoom;						//玩家手牌中所有的炸弹
	TCARDS			 m_PromptTWT;						//玩家手牌中所有可组成的三带二
	TCARDS			m_PromptTWO;						//玩家手牌中所有可组成的三带一
	TCARDS			m_PromptFWTO;						//玩家手牌中所有可组成的四带二(二为单张)
	TCARDS			m_PromptFWDT;						//玩家手牌中所有可组成的四带二(二为对子)
	TCARDS			m_PromptMTWO;						//玩家手牌中所有可组成的小飞机
	TCARDS			m_PromptMTWT;						//玩家手牌中所有可组成的大飞机
	//////////////////////////////////////////////////////////////////////////
	SHORT			m_nCountBoomPrompt;						
	SHORT			m_nCountMaxBoomPrompt;
	SHORT			m_TakeCount;
	//////////////////////////////////////////////////////////////////////////
	//Append
	//此处为了解决比赛房间中，每局游戏结束后，玩家会起立并在此坐下
	//此时坐下的位置可能与之前的位置不同，那么下发的手牌可能显示错误
	int m_nLocalSeat[MAX_PLAYER];
};

