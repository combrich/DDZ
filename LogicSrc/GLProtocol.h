#ifndef __GAMELOGIC_PROTOCOL_H__
#define __GAMELOGIC_PROTOCOL_H__

#include "Define.h"
#include "FrameworkProtocol.h"

#include "biostream.h"
#include "typedef.h"

#pragma pack(1)

#define GLXY_FIRST_ID 0x0400

enum GL_XYID
{
	GLXY_FIRST	= GLXY_FIRST_ID,

	/* ����Э����Ϸ����ʵ��	*/
	
	GLXY_ACTION,						// ��Ҷ�����һ��Ȩֵ�ͻ���������ĳЩ������Ҫ�������ͻ�����ʾ����ʱʹ�ã�
	GLXY_TABLERULE,				// ���ӹ���
	GLXY_ENTERGAME,				// ������Ϸ(ֻ����ҿ����յ�)
	GLXY_STARTGAME,				// ��ʼ��Ϸ
	GLXY_ENDGAME,				// ������Ϸ
	GLXY_LEAVEGAME,				// ������Ϸ(ֻ����ҿ����յ�)
	GLXY_PLAYINFO,				// ��Ϸ��Ϣ
	GLXY_TIMEOUT,				// ��ҳ�ʱ
	GLXY_POWER,					// ���Ȩ��
	GLXY_ADURL,					// ����ַ
	GLXY_OPENCARD,		//	�b����Ϣ
	GLXY_TAKE,						// ����
	/* ��������Ϸ����Ա��չ	*/
	GLXY_QIAOPIHUA,			//����
	GLXY_GRABLANDLORD,			//������
	GLXY_RELINK,				// ����
	GLXY_TALKINFO,				// ϵͳ��Ϣ
	GLXY_AUTOOUT,				// �Զ�������Ϣ
	GLXY_MOVEINFO,				// �ƶ������Ϣ

	GLXY_MAKECARD,				//���� ---������Աר�ã���Ӧ�����������հ汾��
	GLXY_CARDS,					// ����
	GLXY_BASE,					// ����
	GLXY_RECVCARD,              // �յ�������

	GLXY_OUTCARD,				// ���� S <-> C
	GLXY_FIRSTOUTCARD,			// ���ȳ����� S -> C
	GLXY_STARTTURN,				// ��ʼһ��
	GLXY_ENDTURN,				// ����һ��
	GLXY_LOGICCONFIG,			// �߼�����
	GLXY_LOGICDATA,				// �߼�����
	GLXY_GAMESTATE,				// ��Ϸ״̬
	GLXY_BANKERSEAT,			// ׯ��λ��
	GLXY_CLOCK,					// ʱ��
	GLXY_GAMEDATA,
	GLXY_KOUDI,					// �۵�(S <-> C)
	GLXY_LIANGZHU,				// ����(S <-> C)
	GLXY_ENDREVERSEHOST,		// ��������
	GLXY_ENDSENDCARD,           // ���ƽ���
	GLXY_WRONGOUT,				// ˦�ƴ��� S -> C 
	GLXY_PLAYERIDENTITY,	//������
	GLXY_CALLANTE,				// �з�

	GLXY_EXITPROCRES,		//����
	GLXY_ADDMULTIPLE,		//�ӱ�
	GLXY_MISSION,//����ϵͳ
	GLXY_TRUST,	//�й�

	/* ��������Ϸ����Ա��չ	*/
	GLXY_COUNT,
	GLXY_LAST
};

//////////////////////////////////////////////////////////////////////////
// ��Ϸ�߼�Э��


//�й�Ȩ��C<->S
struct msgTrust
{
	enum {XY_ID = GLXY_TRUST};
	enum {T_BEGIN,T_END};	//��ʼ�йܣ������й�
	SHORT nSeat;		//���λ�� (C->S)
	size_t	 nType;		//����

};

//����
struct msgMakeCard
{
	enum {XY_ID =GLXY_MAKECARD};
	enum {NOTMAKE =0, //������
		MAKEBASE,  //������
		MAKEHANDCARD};  //������

	SHORT	nSeat;		//���λ��
	bool	bIsMake;	//�Ƿ�����
	BYTE 	cards[20];	//������
	int     nCount;
	int    nType;		//��ʲô��
};

//����
struct msgMission
{
	enum {XY_ID = GLXY_MISSION};
	enum {
		MCT_THREEE		= 0,	//����
		MCT_THREEW_OT,			//����һ(��)
		MCT_FOUR_T,				//�Ĵ���
		MCT_SINGLEMULTI,		//��˳
		MCT_DOUBLEMULTI,		//˫˳
		MCT_FEIJI,				//�ɻ�
		MCT_FEIJIDCB,			//�ɻ������
		MCT_BOOM,				//ը��
		MCT_MAXBOOM,			//���
		MCT_NONE,				//����

	};
	int		nMisssionID;	//����ID
};
//���ڷ�����Ϸ������Ϣ
struct msgAddMult
{
	enum {XY_ID = GLXY_ADDMULTIPLE};
	int	 nMultiple;		//����
};

// ����(S --> C)
struct msgAction
{
	enum { XY_ID = GLXY_ACTION };
	enum Action
	{
		ACT_ANIFINISH = 0,		// �������
		ACT_ANIGRAB,			// ������
		ACT_FEIJI,				// �ɻ�
		ACT_HUOJIAN,			// ���
		ACT_SANDAIER,			// ������
		ACT_SANDAIYI,			// ����һ
		ACT_SANSHUN,			// ��˳
		ACT_SHUANGSHUN,			// ˫˳
		ACT_SHUNZI,				// ˳��
		ACT_SIDAIER,			// �Ĵ���
		ACT_ZHADAN	,			// ը��
		ACT_OPENCARD,			// ����
		ACT_LANDWIN,			// ����Ӯ (ũ����)
		ACT_LANDLOS,			// ������ (ũ��Ӯ)
		ACT_LANDCHUNTIAN,		// ��������
		ACT_NOLANDCHUNTIAN,		// ũ����
		ACT_FINISHMISSION,		// �������
	};

	BYTE		theFromSeat;					// ˭���Ͷ���
	BYTE		theToSeat;						// ˭���ܶ��������>=MAX_PLAYERS�Ļ���������Ҳ���
	BYTE		theAction;						// ����	
};

//����������
struct msgPlayerIdentity
{
	enum {XY_ID = GLXY_PLAYERIDENTITY};
	SHORT  nSeat;		//λ��
	bool   bIsLandlord;	//�Ƿ��ǵ���
};

// ��Ϸ����(S --> C)
struct msgTableRule
{
	enum { XY_ID = GLXY_TABLERULE };

	size_t		nRoomType;							//�������ͣ�����?��ͨ
	size_t		nGameType;							//��Ϸ���ͣ��Ƹ�?����		
	size_t		nGrabLandType;				//��������ʽ,���������з�
	int				nBase;										//����׷�
};

struct msgTake
{
	enum { XY_ID = GLXY_TAKE };
	enum {TAKE_BEGIN, TAKE_END};
	BYTE nType;
};

struct msgExitProcRes
{
	enum { XY_ID = GLXY_EXITPROCRES };
	BYTE	nFlag;
	SHORT   nSeat;
	long8	nExitScore;
};

// ������Ϸ(S --> C)
struct msgEnterGame
{
	enum { XY_ID = GLXY_ENTERGAME };
};

// ��ʼ��Ϸ(S --> C)
struct msgStartGame
{
	enum { XY_ID = GLXY_STARTGAME };

	BYTE		theIsGameStart;
};

//////////////////////////////////////////////////////////////////////////
//����
struct msgQiaoPiHua
{
	enum { XY_ID = GLXY_QIAOPIHUA };

	BYTE nFlag;
	BYTE nNum;	// ��Ƥ�����
	BYTE nSeat;
};

// ������Ϸ(S --> C)
struct msgEndGame
{

	enum { XY_ID = GLXY_ENDGAME };
	enum { 
		// ��Ϸ��������
		COMMON=0,	// ��������
		ESCAPE,			// �������ܽ���
	};	

	long8 nWinLost[MAX_PLAYER];
	long8 nHistory[MAX_PLAYER];
	int	  nExp[MAX_PLAYER];	// ����ֵ
	int   nLandLord;		// ����λ��
	int   nCountBoom;		// ը�����ִ���
	int   nGrabCount;		// ����������
	int	nCallScore;			//�зַ���
	int   nBaseMultiple;	// ���Ʊ���
	int   nTotalMultiple;	// �ܱ���
	bool  bLandLordWin;		// �Ƿ��ǵ���Ӯ��
	bool  bIsNoOutCard;		// �Ƿ��Ǵ���
	bool  bFanChunTian;		// �Ƿ��Ƿ�����
	bool  bIsOpenHands;		// �Ƿ�����
	bool  bFinishMission;	// �������
	int	  nGameType;		//��Ϸ����
};

// �뿪��Ϸ(S --> C)
struct msgLeaveGame
{
	enum { XY_ID = GLXY_LEAVEGAME };
};

// ��Ϸ��Ϣ(S --> C)
struct msgPlayInfo
{
	enum { XY_ID = GLXY_PLAYINFO };
};

// ��ҳ�ʱ
struct msgTimeOut
{
	enum {XY_ID = GLXY_TIMEOUT};
};


// ������(C<-->S)
struct msgGrabLandlord
{
	enum {XY_ID = GLXY_GRABLANDLORD};
	enum {
		CALL		= 1,		//�е���
		NOTCALL,			//����
		GRAB,					//������
		NOTGRAB,		//����
		CALLSCORE,	//�з�
	};
	//����������ģʽ
	SHORT thePlayerSeat;		//���λ��
	int   thePlayerOperator;		//����������Ĳ���
	//���ڽз�ģʽ
	int	nScore;								//�зֶ���
};

// ���Ȩ��
struct msgPower
{
	enum { XY_ID = GLXY_POWER };

	enum 
	{
		PowerNone = 0x00,
		PowerGrab,
		PowerPlay,
	};

	int  nWhoPower;						// ˭��Ȩ��
	int  nPower;								// ����ʲô��Ȩ��
	bool bCanGiveUp;					// �ɷ��������
	bool isCallLandLord;			// �Ƿ��ǽе���
	bool HaveGrabPower;			// �Ƿ�ӵ����������Ȩ��
	bool bOpenCard;					// ���Խ�����(����ֻ������������������Ȩ�޽�����)
	bool bIsFirstOut;					// �����ֳ���
	int  nCountGiveUp;				// �����������Ƶ����
	int  nScore;								// �е�����ʱ���Ѿ����˶��ٷ�
};

//�����������
struct msgOutCard
{
	enum { XY_ID = GLXY_OUTCARD };
	enum { NONE =0,
		OUTCARD,						//��ҳ���
		GIVEUP,						//��������
	};
	BYTE		nType;					// ����
	BYTE		nSeat;					// ˭����
	BYTE		cards[MAX_HANDCARDS];	// ������
	int 		 nCount;					// ����
	int         nHandCount;				// ���������
};

//����
struct msgOpenCard
{
	enum {XY_ID = GLXY_OPENCARD};
	int		nSeat;	
};

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!��ʱ����������ƿ���!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ��������ս
struct msgReLink 
{
	enum { XY_ID = GLXY_RELINK };

	BYTE		nGameState;									// ״̬
	BYTE		theBanker;									// ׯ��
	int			theBaseAnte[MAX_PLAYER];					// ����
	int			theAnte[MAX_PLAYER];						// 4���˽еĻ�����
	BYTE		theReady[MAX_PLAYER];						// �Ƿ��Ѿ�׼������ = 0û��׼�� = 1��ʾ�Ѿ�׼����
	BYTE		theCall[MAX_PLAYER];						// �Ƿ��ׯ���ˣ����˾������һ��������ׯ��
	BYTE		theNowSeat;									// ��ǰλ��
	BYTE		theLastPlayer;								// ���һ�����Ƶ����	
	BYTE		theHandCount[MAX_PLAYER];					// ��������

};

struct msgTalkInfo
{
	enum { XY_ID = GLXY_TALKINFO};

	char strInfo[50];
};

//�Զ�������Ϣ
struct msgAutoOut
{
	enum{ XY_ID = GLXY_AUTOOUT};

	int theSeat;             //�Զ����Ƶ����λ��
};

// �ƶ����ͷ��
struct msgMoveInfo
{
	enum { XY_ID = GLXY_MOVEINFO };

};

// ����(S --> C)
struct msgBase
{
	enum { XY_ID = GLXY_BASE };
	enum { ADD=0,
		ADDS,
		DEL,
		UPDATE                           // �����ƣ�ȥ��ԭ����������
	};
	enum
	{
		NOACTION,				//�޶���
		TAKECARD ,			//����
		ADDHAND					//���ӵ�����������
	};
// 
 	BYTE		nBaseType;							// ���Ƶ�����
 	BYTE		nType;										// ����
	BYTE		cards[BASE_CARDS];	// ������
 	int 				nCount;									// ����
	BYTE		nActionType;						// ��������
};

// ����(S --> C)
struct msgCards
{
	enum { XY_ID = GLXY_CARDS };
	enum { ADD=0,
		   ADDS,
		   DEL,
		   UPDATE,                          // �����ƣ�ȥ��ԭ����������
		   TAKE,								  // ����	
	};

	BYTE		nType;					// ����
	BYTE		nSeat;					// ˭����
	BYTE		cards[MAX_HANDCARDS];	// ������
	int 		nCount;					// ����
};

struct msgEndSendCard
{
	enum { XY_ID = GLXY_ENDSENDCARD };

	DWORD theDownTime;

	BYTE		nFlag;
	int			nMaxseat;
	int         nMinseat;
	int			nTakeCount;
};

// ��ʼһ��(S --> C)
struct msgStartTurn
{
	enum { XY_ID = GLXY_STARTTURN };

	BYTE		theBanker;							// ׯ��
	BYTE        theKouDiPlayer;                     // ���۵׵����
};

// ����һ��(S --> C)
struct msgEndTurn
{
	enum { XY_ID = GLXY_ENDTURN };

	BYTE        theOutGreatestSeat;                     // �þֳ����������

	int         theScoreCardsCount;                     // ץ�ķ��Ƶĸ���
	int         theCatchScore;                          // ץ�ַ�ץ��

	bool        isLastTurn;                             // ���һ��
};

struct msgLogicConfig
{
	enum{XY_ID = GLXY_LOGICCONFIG};

	BYTE		IsSysTrust;						// �Ƿ��Զ��й�
	BYTE		WaitLastReadyType;				// �ȴ����һ����ҵķ�ʽ��1���Զ���ʼ��2���Զ��˳����������ȴ���
};

struct msgLogicData
{
	enum{XY_ID = GLXY_LOGICDATA};

	LONG		BasePlayTypeScore;				// ��

	msgLogicData()	{ reset();						}
	void reset()	{ memset(this,0,sizeof(*this)); }

	friend bostream& operator<<(bostream& ar, const msgLogicData& obj )
	{
		ar << obj.BasePlayTypeScore;
		return ar;
	}

	friend bistream& operator>>(bistream& ar, msgLogicData& obj )
	{
		obj.reset();
		ar >> obj.BasePlayTypeScore;
		return ar;
	}
};

struct msgGameData
{
	enum{XY_ID = GLXY_GAMEDATA};

	LONG		nBaseScore;				// ��
	LONG		nTotalScore;
	msgGameData()	{ reset();						}
	void reset()	{ memset(this,0,sizeof(*this)); }

	friend bostream& operator<<(bostream& ar, const msgGameData& obj )
	{
		ar << obj.nBaseScore;
		ar << obj.nTotalScore;

		return ar;
	}

	friend bistream& operator>>(bistream& ar, msgGameData& obj )
	{
		obj.reset();
		ar >> obj.nBaseScore;
		ar >> obj.nTotalScore;
		return ar;
	}
};

struct msgGameState
{
	enum {XY_ID = GLXY_GAMESTATE};

	BYTE  nGameState;
};


struct msgBankerSeat
{
	enum { XY_ID = GLXY_BANKERSEAT };

	int nSeat;
};

// ʱ��
struct msgClock
{
	enum {XY_ID = GLXY_CLOCK};

	SHORT Who;		// ָ��
	BYTE Time;		// ʱ��
};

struct msgCallAnte 
{
	enum { XY_ID = GLXY_CALLANTE };

	BYTE		theSeat;								//˭�з�
	int			theBaseAnte;							// ��С��ע���ӵĳ���
	int			theSBaseDouble7;						// ��С��ע���ӵ�8��
	int			theSBaseDouble4;						// ��С��ע���ӵ�4��
	int			theSBaseDouble2;						// ��С��ע���ӵ�2��
	int			theSBaseDouble1;						// ��С��ע���ӵ�1��
	bool		theCall;								//�Ƿ���Խз�
};

struct msgWrongOut
{
	enum {XY_ID = GLXY_WRONGOUT};

	int		Seat;
	int		OutCount;
	int     theErrorCount;             // �������
};

struct msgADUrl
{
	enum { XY_ID = GLXY_ADURL};

	char theADUrl1[256];
};


#pragma pack()
#endif//__GAMELOGIC_PROTOCOL_H__

