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
// ��������ʽ��Э��
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

	// ���ͻ����뷢�ͳ���
	CHAR	m_sSendBuffer[SOCKETBUFFER_LEN];
	WORD	m_nSendSize;

///////////////////////////////////////////////////////////////////////////////////////////////////////
// ��Ϸ���ӿ�
public:
	// ��Ϸ���ݰ�����
	virtual int OnReceivePacket(WORD nXYID, LPCSTR lpszPacket, WORD nSize);

	// �������ݰ�����
	virtual int OnReplayPacket(WORD nXYID, LPCSTR lpszPacket, WORD nSize, CONST ReplayStatus& _RS);

	// ��Ϸ����¼�������Ӧ
	virtual int OnNotifyMessage(UINT iCmd, WPARAM wParam, LPARAM lParam,const string& strType, LONG nID, const string& strKeyword);

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

	virtual int OnUpdateCfgFramework();

	virtual BOOL GetVideoParam(TUSERID nUserID, TVideoParam& tVideoParam);

	virtual BOOL GetClientVersion(DWORD& dwVer);

	// ��Ƶ���¼�������Ӧ
	virtual int OnVideoPanelMessage(UINT iMsg, TUSERID nUserID);

	// ʹ�õ�����ʾ
	virtual int OnHintUseProps(DWORD nFromBrandID, DWORD nFromNumberID,DWORD nToBrandID, DWORD nToNumberID, int nPropsID, int nCount,char szParam[]);

	// ������Ը���
	virtual int OnRptAttUpdate(DWORD nBrandID, DWORD nNumberID, int nCharmAdd, int nContributionAdd, BYTE nAttSize,int nAtt[10]);

	// ��ȡ��˭˵���ı�������0����Ĭ������
	virtual int GetChatToText(TUSERID nToUserID, string& strText);

	virtual LRESULT OnWindowDestroyEx();

	// ��ȡϵͳ��Ϣ��ɫ
	virtual DWORD GetFrameworkChatColor();

	virtual int OnWillSendPlayerChat( DWORD& dwColor, string& strChatText );

	// ���ڴ������֪ͨ
	virtual int OnFrameworkCreated( );

	//΢������:���غ�������������Ϣ
	virtual int OnRespGetUserRecent( TUSERID nUserID, const string& strContent, const string& strImagePath );

	// �յ����ͷ����£�USERID, ͷ��ߴ�����, ͷ��ͼƬ·�� ��
	virtual int OnRespPlayerFaceFrame( TUSERID nUserID, int nType, const string& strPath );

	// ���Ѳ�����Ӧ
	virtual int OnRespFriendOp( BYTE nFlag, int nOp, TUSERID nUserID, const string& strUserName, int nRel, const string& strCaption, const string& strText, const string& strWeb );
	//����
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

	SHORT		m_nLocalPowerSeat;					//��Ȩ�޵����λ��
	size_t			m_nSelfPower;								//�Լ�ӵ�е�Ȩ��,ÿ�����Ȩ�޵�ʱ��Ϊ���ֵ���ơ�Ȩ���ƽ���Ӧ���Լ��Ƴ��Լ���Ȩ��
	LONG			m_nBasePlayScore;					//�׷�
	string			m_strHelpURL;
private:
	void ClearTable();												// ���������Ϣ
	void LoadGameConfig();
public:
	void OnGameTickTimer(DWORD dwTickNow);
//////////////////////////////////////////////////////////////////////////
//Ȩ�����
	void GetPowerGrab(msgPower& power);
	void GetPowerPlay(msgPower& power);

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//λ�����
	POINT m_ptHand[MAX_PLAYER];				// ���ưڷŵ�����
	void ArrangeLocalPosition(LONG nGuiW,LONG nGuiH,SHORT nLocal);   //����ͼƬ��ʾλ��
	void ArrangeButtonPosition(LONG nGuiW,LONG nGuiH);				 // ������ťλ�ã�
	void ArrangeClockPosition(LONG nGuiW,LONG nGuiH, short nLocal);				 // ����ʱ��λ��
	void ArrangeOutCard(LONG nGuiW, LONG nGuiH, SHORT nLocal);
	void ArrangePoint(int nBase ,int nMult);		//�����׷ֺͱ�����λ��
	void ArrangePlayBtn();		//����������ذ�ť
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	// ��Ϸ�߼�Э�����
	BEGIN_MSG_MAP_C()
		ON_MSG_MAP_C(GLXY_TABLERULE,OnMsgTableRule)
		ON_MSG_MAP_C(GLXY_ENTERGAME,OnMsgEnterGame)		 // ������Ϸ
		ON_MSG_MAP_C(GLXY_STARTGAME,OnMsgStartGame)		 // ��ʼ��Ϸ
		ON_MSG_MAP_C(GLXY_GAMESTATE,OnMsgGameState)	
		ON_MSG_MAP_C(GLXY_GRABLANDLORD,OnMsgGrab)
		ON_MSG_MAP_C(GLXY_OPENCARD,OnMsgOpenCard)
		ON_MSG_MAP_C(GLXY_OUTCARD,OnMsgOutCards)
		ON_MSG_MAP_C(GLXY_RELINK,OnMsgReLink)			 // ����
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
//��Ϣ�¼�
private:
	int  OnBtnGrabLandlord(const string& strKeyword);
	int  OnBtnPlayCard(const string& strKeyword);
	//��������
	int  OnNotifyPlayFinish(WPARAM wParam, LPARAM lParam,const string& strType, LONG nID, const string& strKeyword);
	//�ƶ�����
	int  OnNotifyMoveFinish(WPARAM wParam, LPARAM lParam,const string& strType, LONG nID, const string& strKeyword);
	//����������
	int  OnNotifyMouseLButton(WPARAM wParam, LPARAM lParam,const string& strType, LONG nID, const string& strKeyword);
	//������
	int  OnNotifyMouseEnter(WPARAM wParam, LPARAM lParam,const string& strType, LONG nID, const string& strKeyword);
	//��ť��Ӧ
	int  OnNotifyButton(WPARAM wParam, LPARAM lParam,const string& strType, LONG nID, const string& strKeyword);
//��ť��Ӧ
	int  OnBtnColse();					//�رտͻ���
	int  OnBtnMaxArea();			//������ر�
	int  OnBtnResizeArea();		//�������
	int  OnBtnTop();						//�����ö�
	int  OnBtnUnTop();				//����ȡ���ö�
	int  OnBtnHelp();						//����
	int OnBtnGiveUp();				//������ť
	int OnBtnOutCard();				//���ư�ť
	int OnBtnInfo();						//��ʾ��ť
	int OnBtnOpenCard();			//���ư�ť
	int OnBtnChangeTable();	//����
	int OnBtnMusicOn();			//����Ч
	int OnBtnMusicOff();			//�ر���Ч
	int OnBtnCloseOK();			//���ڹر�ȷ��
	int OnBtnCloseCancle();	//���ڹر�ȡ��
	int OnBtnSpeak();					//���԰�ť
	int OnBtnSet();						//���ð�ť
	int OnBtnSetOK();					//����ȷ�� 
	int OnBtnSetCancel();		//����ȡ��
	int OnEndBtnClose();			//����ر�
	int OnBtnRunawayOK();	//����ȷ��
	int OnBtnSignUpFailure();//����ʧ��
	int OnBtnTrush();					//�й�
	int OnBtnUnTrush();			//ȡ���й�
	
	//���޲���
	int OnBtnMakeCard();			//����
	int OnBtnMakeBase();		//������
	int OnBtnEndMakeCard();//��������
	int nMakeCardType;
	uiMakeCardPanel* m_uipMakeCards;					//�������
	clock_t		m_nLBtnNow;		//��굥�����ڰ��µ�ʱ��
	clock_t		m_nLBtnOld;		//��굥����һ�ΰ��µ�ʱ��
private:
	int				m_nLandlord;		//��ַλ��
	size_t		m_nRoomType;	//��������
	size_t		m_nGameType;	//��Ϸ����
	size_t		m_nGrabLandType;	//��������ʽ
	bool			m_bIsCallLandlord;	//�Ƿ��ǽе���
	bool			m_bIsFirstOut;			//�Ƿ����ֳ���

	bool			m_bNotOutIsShow[MAX_PLAYER];			// ������־�Ƿ���ʾ
	TCARDS m_BaseCards;										//����
	TCARDS m_HandCards[MAX_PLAYER];	//����
	TCARDS m_PromptCard;									// ��ʾ��ҵĳ���
	SHORT	 m_nCountPrompt;						//��ʾ��ҳ��Ƶ���ʼ��λ��
	bool			m_bIsAlreadyOut;				//����Ƿ��Ѿ�����
	bool			m_bIAmSeeing;					//�Լ��Ƿ����Թ���
	bool			m_bSpeakPlay;
	size_t		m_nTotalMult;						//�ܱ���
	int				m_nCallScore;					//�зֱ��� 

public:
	TCARDS ConvertCardVectorToCards(const TCardVector& dvCards);
	TCardVector ConvertCardsToCardVector(const TCARDS& vCards);
	void SortCards(TCARDS& vCards);
//////////////////////////////////////////////////////////////////////////
//ͨѶ���
	void SendGrabLandlord(int nType);
	void SendOutCard(int nType);
///////////////////////////////////////////////////////////////////////////////////////////////////////
// ת��ģ��ӿ�
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

	// ͼ����ʾ
	std::list<POINT> _BombPoint;
	// ��ʾ׼����־
	void ShowPlayReadySign(SHORT nSeat, BOOL bShow = TRUE);
	//���Ƶ���ʾ
	void ShowBase(BOOL bShow = TRUE, BYTE nActionType = msgBase::NOACTION);
	void InitBomb();
	void PlayCardParticle(spiType _spiType, int x, int y);
	HGUI GetGUIHandle(string strName, SHORT nSeat = -1);
	void ShowGrabBtn(bool IsCall, BOOL bShow /* = TRUE */,int nCall=0);
	// ��ʾ��������ر�־
	void ShowSignGrab(int nSeat, int nType, BOOL bShow = TRUE);
	void ClearAllCallSign();
	void ShowAllPlayBtn(BOOL bShow,BOOL bGiveUp, BOOL bOutCard, BOOL bInfo, BOOL bOpenHand=FALSE);
	// ��ʾ�������
	int ShowOutCard(int nSeat, BOOL bShow = TRUE);
	// ��ʾ������־
	void ShowNotOutSign(int nSeat, BOOL bShow = TRUE,bool bShowEx = false);
	void ShowAni(LPCTSTR strKey,bool IsOut =true, SHORT nSeat = -1,SHORT loop = 1, bool bShow = true);
	// ��ʾ��������
	void ShowBaseCardsType(int nType,BOOL bShow = TRUE);
	// �ر������������صİ�ť
	void ClearAllPlayBtn(void);
	void ShowSignLandlord(BOOL bShow = TRUE);
	void ShowAllMult(int nMult, BOOL bShow = TRUE);
	void ShowBasePoint(LONG nScore, BOOL bShow = TRUE);
	void ShowHandCount(SHORT nCount,SHORT nSeat, BOOL bShow = TRUE);
	//�йܰ�ť
	void ShowBtnTrush(bool bShow = true);
	//��ʾ����
	enum 
	{
		ST_NONE,
		ST_MAX,
		ST_MIN,
	};
	void ShowMission(int nCardType ,int nShowType = ST_MIN, bool bShow = true);
//////////////////////////////////////////////////////////////////////////
//UI
	uiHandCards*		m_pHandCards[MAX_PLAYER];		//����		
	uiHandCards*		m_pOutCard[MAX_PLAYER];			//��Ҵ������
	uiHandCards*		m_pBaseCards;		//����
	ClockUI*					m_pClockUI;
	uiHeadPortrait*	m_pHeadPortrait[MAX_PLAYER];	// �������
	
	TCardVector		m_dvOutCards;				//������ҵĳ���
	TCardVector		m_dvFrontOutCards;			//��һ��ҵĳ���
	bool							m_nBaseType;				//������ʾ����
//////////////////////////////////////////////////////////////////////////
//��̬UI����
private:
	bool CreateBase();
	bool CreateClock();
	bool CreateHand();
	bool CreateHead();
	bool CreateOut();
///////////////////////////////////////////////////////////////////////////////////////////////////////
public:
// ����
	void PlayGameEffct(LPCTSTR strKey, char crSex = '5');
	BOGY::Direct2D::HCHANNEL m_hGameSound;
	void PlayOutCardEffec(const CardTypeInfo &cti,const TCardVector& dvOut);
///////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
private:
	char* CharAddPoint(long8 nScore);
	//�ɷ����
	bool CanOut(bool bIsFirst = FALSE);
	bool GetGreaterThanNextOut(CardTypeInfo ctiFrontOut);						//��ȡ��������д���ָ�����͵�����
	//��ȡ���������ӵ�еĻ������ͣ���������Ϊ���š����ӡ����š�ը��
	//������ȡ���Ļ������ͷ���ָ����������
	void FillBaseCards();			
	TCARDS GetBaseCards(int nCardsLen, int nType, TCARDS handCards);
	TCARDS GetBaseCardsEx(int nCardsLen, int nType, TCARDS handCards,TCARDS needReject);
	//�������Ƿ��п��Դ������
	bool HaveOutCard(int nCardsLen, CardTypeInfo ctiFrontOut ,TCARDS handCards,int nStep = 1);		
	bool HaveOutCardBoom(int nCardsLen, CardTypeInfo ctiFrontOut ,TCARDS handCards,int nStep = 1);	
	bool HaveOutCardMaxBoom(int nCardsLen, CardTypeInfo ctiFrontOut ,TCARDS handCards,int nStep = 1);	
	bool HaveOutCardEx(int nCardsLen,CardTypeInfo ctiFrontOut,TCARDS handCards);
private:
	//////////////////////////////////////////////////////////////////////////
	//��ŵķ�ʽΪ��С�����������
	TCARDS			m_PromptSingle;						//���������ӵ�е����е���
	TCARDS			m_PromptDouble;						//���������ӵ�е����ж���
	TCARDS			m_PromptThree;						//������������е�����
	TCARDS			m_PromptBoom;						//������������е�ը��
	TCARDS			 m_PromptTWT;						//������������п���ɵ�������
	TCARDS			m_PromptTWO;						//������������п���ɵ�����һ
	TCARDS			m_PromptFWTO;						//������������п���ɵ��Ĵ���(��Ϊ����)
	TCARDS			m_PromptFWDT;						//������������п���ɵ��Ĵ���(��Ϊ����)
	TCARDS			m_PromptMTWO;						//������������п���ɵ�С�ɻ�
	TCARDS			m_PromptMTWT;						//������������п���ɵĴ�ɻ�
	//////////////////////////////////////////////////////////////////////////
	SHORT			m_nCountBoomPrompt;						
	SHORT			m_nCountMaxBoomPrompt;
	SHORT			m_TakeCount;
	//////////////////////////////////////////////////////////////////////////
	//Append
	//�˴�Ϊ�˽�����������У�ÿ����Ϸ��������һ��������ڴ�����
	//��ʱ���µ�λ�ÿ�����֮ǰ��λ�ò�ͬ����ô�·������ƿ�����ʾ����
	int m_nLocalSeat[MAX_PLAYER];
};

