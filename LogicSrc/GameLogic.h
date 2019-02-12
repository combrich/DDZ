#ifndef __GAMELOGIC_H__
#define __GAMELOGIC_H__

#include <vector>
#include <list>
#include "BaseLogic.h"
#include "GLProtocol.h"
#include "LogicData.h"
#include "Robot.h"
///////////////////////////////////////////////////////////////////////////////////////
// ��Ϸ�߼�Э��ӳ��
#define BEGIN_MSG_MAP() \
	int OnGameLogicMsg(IUserPtr player,WORD XYID,const char* pMsg,int nLen) \
	{ \
	switch(XYID) \
		{

#define ON_MSG_MAP(xyid,onfun) \
	case xyid: return onfun(player,pMsg,nLen); break;

#define END_MSG_MAP() \
			default: break; \
		} \
		return 0; \
	};

//////////////////////////////////////////////////////////////////////////////////////////
// �ͻ�������б�Ҫ��ʾ������
const BYTE g_nUserListShowItemCount = 6;
const BYTE g_nUserListShowItems[] = {
		msgPlayerItem::SI_STATE,
		//msgPlayerItem::SI_SID,
		msgPlayerItem::SI_NICKNAME,
		msgPlayerItem::SI_GAMESCORE,
		msgPlayerItem::SI_SCORETITLE,
		msgPlayerItem::SI_ROUNDCOUNT,	
		msgPlayerItem::SI_WIN_PRO,			
		//msgPlayerItem::SI_LOSE,			
		//msgPlayerItem::SI_DRAW,
		//msgPlayerItem::SI_ESCAPE
};

const BYTE g_nUserListShowItemsRich[] = {
		msgPlayerItem::SI_STATE,
		//msgPlayerItem::SI_SID,
		msgPlayerItem::SI_NICKNAME,
		msgPlayerItem::SI_GAMEMONEY,
		msgPlayerItem::SI_MONEYTITLE,
		msgPlayerItem::SI_ROUNDCOUNT,	
		msgPlayerItem::SI_WIN_PRO,			
		//msgPlayerItem::SI_LOSE,			
		//msgPlayerItem::SI_DRAW,
		//msgPlayerItem::SI_ESCAPE
};

const LONG g_nUserListShowItemWidth[] = {
		40,
		65,
		65,
		65,
		65,
		65,
		//60,
		//60
};
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

DECLARE_DYNAMICCREATE
class CGameLogic : public CBaseLogic
{
	//////////////////////////////////////////////////////////////////////////
	// ����

public:
	CGameLogic(void);
	virtual ~CGameLogic(void);
	void Reset();		// �������״̬������,�˺�����������á�

	//////////////////////////////////////////////////////////////////////////
	// ����ӿ�
public:
	// ��Ϸ�߼����󴴽�ʱ����
	virtual int onCreate(ITable* table, const std::string& szRoomGameRule);		
	int onFrameWorkLogicCreate(ITable* table, const std::string& szRoomGameRule);
	int onBaseLogicCreate(ITable* table, const std::string& szRoomGameRule);

	// ��ʱ��
	virtual int OnTimer( );	

	//////////////////////////////////////////////////////////////////////////
	// ��Ϸ�ӿ�
public:

	// ���յ�MESSAGE���ݱ�
	virtual void OnMessageBox(IUserPtr player, char* buffer, int length);	

	// ������Ϸ, ��������,�ͳ�ʼ��, IsSeen �ǲ����Թ� 
	virtual void OnEnterGame(IUserPtr player, bool IsSeen);		

	// ĳ��ҿ�ʼ
	virtual void OnPlayerStart(IUserPtr pUser);

	// ��Ϸ��ʼ
	virtual	void OnStartGame();												

	// �Թ����˳�
	virtual void OnLookerExit(IUserPtr pUser);

	// �����ͨ״̬�µ��뿪
	virtual void OnExitNormal(IUserPtr player);	

	// ǿ���˳�
	virtual void OnExitForc(IUserPtr player);	

	// ����
	virtual bool OnWinLostResult();

	// ��Ϸ����
	virtual void OnEndGame();		

	// ��ȡ����������
	virtual SHORT  GetMaxPlayers();

	// ��ȡ��С��ʼ����
	virtual SHORT  GetMinPlayers();

	// ��ȡָ���������͵�������ɫ
	virtual ulong4	GetTalkColor(BYTE nTalkType = ttNormal);	

	// ��ȡ�ͻ�������б�Ҫ��ʾ����
	virtual void GetPlayerShowItems(BYTE nItems[],BYTE nItemWidth[],BYTE& nItemCount);

	// �Թ������������͸ı�
	virtual void OnLookerWatchAllowTypeChange(IUserPtr pLooker,BOOL bIsWatchLooker);

	// ������������͸ı�
	virtual void OnPlayerWatchAllowTypeChange(IUserPtr pPlayer,BYTE nWatchAllowType);

	// ��ȡ�ȴ����һ����ҿ�ʼ��ʱ��
	virtual int GetWaitLastReadyTime();

	// ����ܷ������˳�
	virtual bool CanPlayerLeave(ulong4 nNumberID);

	// ��Ϸ�߼�Э�����
	virtual	int OnGamePacket(IUserPtr player, WORD XYID, char* buffer, int length);

	// ����
	virtual int onUserWireBreak( IUserPtr player, int keepTime );
public:

	BEGIN_MSG_MAP()
		ON_MSG_MAP(GLXY_MAKECARD,OnMsgMakeCard)
		ON_MSG_MAP(GLXY_GRABLANDLORD,OnGrabLandlord)
		ON_MSG_MAP(GLXY_OPENCARD,OnMsgOpenCard)
		ON_MSG_MAP(GLXY_OUTCARD,OnMsgOutCard)
		ON_MSG_MAP(GLXY_QIAOPIHUA,OnMsgQiaoPiHua)
		ON_MSG_MAP(GLXY_TRUST,OnMsgTrush)
	END_MSG_MAP()

	// ��Ϸ�߼�Э�鴦���뽫���³�Ա�����Ķ���д�� GameMsg.cpp �ڡ�
	int OnMsgMakeCard(IUserPtr player,const char* pMsg,int nLen);
	int OnMsgTrush(IUserPtr player,const char* pMsg,int nLen);
	int OnMsgQiaoPiHua(IUserPtr player,const char* pMsg,int nLen);
	int OnGrabLandlord(IUserPtr player,const char* pMsg,int nLen);
	int OnMsgOpenCard(IUserPtr player,const char* pMsg,int nLen);
	int OnMsgOutCard(IUserPtr player,const char* pMsg,int nLen);

	//////////////////////////////////////////////////////////////////////////////////////////////
	
private:
	void TakeCards();												//����
	void ReTakeCards();										//���·���
	void AppendBase();											//��ӵ���
	void SendStartGame(IUserPtr player = NULL);
	void SendLogicConfig(IUserPtr player = NULL);
	void SendLogicData(IUserPtr player = NULL);
	void SendAction(SHORT nFromSeat,SHORT nToSeat,BYTE action = msgAction::ACT_ANIFINISH);	 //���Ͷ���
	void SendMsgOnGamingEnter(IUserPtr player);
	void SendPowerOnGamingEnter(IUserPtr player);

	void SendGameState(IUserPtr player = NULL);
	void SendGameData(IUserPtr player = NULL);
	void SendIdentity(int nSeat); //������
	void SendCards(msgCards& msgC);			
	void SendOutCard(msgOutCard msgOC);						//����	
	void SendPower(int nPower, int nWho,bool bOpenCard=false);
	void SendClock(int nPower, int nPowerSeat, IUserPtr pUser = NULL);
	void GotoEnd();
	void SendAddMult();

private:
	void InitLogicData();
	void GoToGameStep(BYTE nGameStep);
	void GotoNextPlayer(int nNowPlayer);
	void GotoNextGrabPlayer(int nNowPlayer);
private:
	size_t GetBaseCardsType(TCardVector& dvCards);
	int	 GetBaseCardMultiple();								//��ȡ���Ʊ���
	void CalcTotalMultiple();								//�����ܱ���
private:
	LogicData*  m_pLogicData;

public:
	// ת��ģ��ӿ�
	virtual CBaseLogic* GetLpBaseLogic();
	virtual SHORT GetMaxPlayerCount();
	//////////////////////////////////////////////////////////////////////////////////////////////
protected:
	// �ű�����
	size_t		m_bOpenCardEnable;						//�Ƿ����b��			
	time_t		m_StepTime;											// ��ʱʱ��
	size_t		m_nRoomType;										//	��������
	size_t		m_nGameType;										// ��Ϸ���ͣ��Ƹ�������
	size_t		m_nGetLandType;								// ȷ�������ķ�ʽ
	size_t		m_nBaseScore;										// ��Ϸ�׷�
	size_t		m_nEnableProp;									// �Ƿ���õ���
	size_t		m_nWinExp;												// Ӯ����
	size_t		m_nLosExp;												// �侭��
	enum {ON_MISSION=1,OFF_MISSION};
	size_t		m_nOn_OffMission;						//�Ƿ�������
	size_t		m_nRunAwayPunish;					//���ܳͷ�ϵ��
	size_t		m_nTimeGrab;								//������ʱ��
	size_t		m_nRobotTimeGrabMax;			//������������ʱ��
	size_t		m_nTimePlay;									//����ʱ��
	size_t		m_nTimeFirstPlay;						//һ�ֵ�һ������ʱ��
	size_t		m_nTruOutTime;							//�йܳ���ʱ�� 
	size_t		m_nRobotBaseRand;					//�����˽е�������

private:
	size_t		m_nRobotGrabTime;							//������������ʱ��
	SHORT	m_nNowPlayer;										//��ǰ��Ȩ�޵����λ��
	SHORT	m_nLastOutCardSeat;						//��һ��������ҵ�λ��
	SHORT	m_nFirstPlayer;										//û����Ϸ���ֵ�һ��ӵ��Ȩ�޵����λ��
	size_t		m_nGameState;										//��ǰ��Ϸ״̬
	bool			m_bOpenHand[MAX_PLAYER];	//����Ƿ��Ǳb��
	bool			m_bIsFirstOut;										//�Ƿ����ֳ���
	bool			m_bIsChunTian;										//�Ƿ��Ǵ���
	bool			m_bIsFanChunTian;							//�Ƿ��Ƿ�����
	size_t		m_nBoomCount;									//ը������
	size_t		m_nNowPlayTime;								//��ǰ����ʱ��
private:
	TCardVector m_dvHandCards[MAX_PLAYER];				// �����������
	TCardVector m_dvBaseCards;													// ����
	TCardVector m_dvFirstOut;													//һ���е�һ����
	bool					 m_bCanGiveUp;											// �ɷ��������
	bool					 m_bIsCallLandlord;										// �Ƿ��ǽе���
	bool					 m_bHavePowerGrab[MAX_PLAYER];	//�Ƿ���������Ȩ��
	size_t				 m_nCountGiveUp	;			//�������Ƶ���Ҹ���		
	int						 m_nGrabCount;						//������˶��ٴε���
	int						m_nCallLandlord;					// �е������λ��
	int						m_nGrabSeat;						// ���������λ��
	int						m_nLandlord;						//����λ��
	int						m_nTotalMultiple;				//�ܱ���
	int						m_nCallScore;					//���˶��ٷ�
	int						m_nCallNum;						//���ٸ��˽��˵���
	int						m_nLastCallSeat;				//���зֵ����λ��
	size_t				m_nWaitlastOutTime;				//����ֻʣ��һ���Ƶ�ʱ���ϼҳ����ţ���������Զ�����
	TCardVector m_dvOutCards;								// ��ǰ��ҵĳ���
	TCardVector m_dvLastOutCards;					// ��һ����ҵĳ���
	int						m_nBaseMultiple;							//���Ʊ���
//////////////////////////////////////////////////////////////////////////
//����ϵͳ
	bool IsFinishMission();								//�ж��Ƿ��������
	void SendMission();										//��������
	bool		m_bFinishMission;						//�Ƿ��������
	int			m_nMissionCardType;						//��������
	int			m_nLaseOutCT;							//����������
	
	
private:
	//�ж��������������Ƿ���Ψһ,����ǣ�����true�����򷵻�false
	bool JudgeCardsIsSole(const TCardVector& dvCards);
	int ErrorAtCandsNotSole(IUserPtr player,msgOutCard* msgOC);

private:
	//��OnTime�д������Ϣ
	int T_WatiLastOut(time_t nPassTime );
	int T_OutTimePlayer();
	int T_OutTImeGrab();
	void IntoTrust(int nSeat); 
private:
	//������
	bool RobotPlayCard();
	bool RobotGrabLandlord(int nWhoIsAI);
	int  GetCardNum(int nCardVal, TCardVector dvSearch);
	msgOutCard  m_msgRobotOC;
	size_t m_nRobotOutCardTimeMax;
	size_t m_nRobotOutCardsTime;
	CRobot*		m_pRobot;											//������
	bool			m_bIsRobot[MAX_PLAYER];				// ������Ϸ������Ƿ��ǻ�����
};	

#endif//__GAMELOGIC_H__

