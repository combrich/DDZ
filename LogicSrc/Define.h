#ifndef __GAMELOGIC_DEFINE_H__
#define __GAMELOGIC_DEFINE_H__
#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <string>

using namespace std;

#include "BaseDefine.h"

//#define MYVERSION_GAMELOGIC_FR	1
//#define GAME_CLIENT			// ������Ϸ�ͻ���

#define GAME_VERSION_MAJOR	1
#define GAME_VERSION_MINOR	1

#define TIMERDELAY			2		//ʱ�Ӹ����������һ��
#define GAMINGDELAYTIME		31		//��Ϸ�г�ʱʱ�䣬��������,  -1 for Client
#define NULLDELAYTIME		61		//����Ϸ�в�����ť��ʱʱ�䣬��������  -1 for Client

//For Server
#define GAMINGDELAY			(GAMINGDELAYTIME/TIMERDELAY+(GAMINGDELAYTIME%TIMERDELAY?1:0))			
//For Server
#define NULLDELAY			(NULLDELAYTIME/TIMERDELAY+(NULLDELAYTIME%TIMERDELAY?1:0))				
#define DELAYTOAUTO			3		//���ٴγ�ʱ������Ϊ�й�

#define CY_SAFE_DELETE(p)	{if(p){delete p;p=0;}}

#define GAMENAME		"������"

// ����״̬
#ifndef _TESTRELEASE
#define GAMECLIENTDIR	"DDZ_d.exe"
#define ROOMGAMERULE	"BasePlayTypeScore='1';BasePlayTypeScoreProp='0';ServicePayProp='0';ADURL='http://www.baidu.com/';"
#define DYNAMICGAME		//���������ʾ��ϷΪ��̬��Ϸ
#else
#define GAMECLIENTDIR	"DDZ.exe"
#define ROOMGAMERULE	"BasePlayTypeScore='1';BasePlayTypeScoreProp='0';ServicePayProp='0';ADURL='http://www.baidu.com/';"
#define DYNAMICGAME		//���������ʾ��ϷΪ��̬��Ϸ
#endif

#define	GAMECLASSNAME		CGameLogic

// �ͻ���״̬
typedef enum tagClientState
{
	csNull = 0,
	csRun,			// ������Ϸ��
	csRelink,		// ����������
	csLooker		// ���ڹ�ս��
}ClientState;

#define MAX_PLAYER			3		// ��������
#define MIN_PLAYER				3		// ��С�����


#define WAIT_LAST_READY_TIME	30

const size_t	MAX_TOTALCARDS	=54;	//������
const size_t  MIN_HANDCARDS		=17;	//ũ��������
const size_t	MAX_HANDCARDS		=20;	//����������
const size_t	BASE_CARDS				= 3;		//��������

enum 
{
	GT_SCORE=1,			//������Ϸ
	GT_RICH						//�Ƹ���Ϸ
};

enum 
{
	RT_NORMAL =1,		//��ͨ����
	RT_MATCH					//��������
};		

typedef enum tagBaseCardType
{
	BC_NOCARDTYPE   =1,			//ɢ��
	BC_FLUSH,					//ͬ��
	BC_STRAIGHT,				//˳��
	BC_BAOZI,					//����
	BC_STRAIGHTFLUSH,			//ͬ��˳
	BC_HAVEBIGJOKEER,			//�д���
	BC_ERROR									//���ִ���
} TBCARDTYPE;

enum
{
	GLT_GRAB =1,	//��������ʽȷ������
	GLT_SCORE,		//�зַ�ʽȷ������
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef unsigned long long USERID;

__inline USERID GetBrandID(USERID nUserID)
{
	return (nUserID >> 32);
}

__inline USERID GetNumberID(USERID nUserID)
{
	return (nUserID & 0xFFFFFFFF);
}

typedef enum tagGameState				// ��Ϸ״̬
{
	GAME_STATE_NONE = 0,
	GAME_STATE_TAKE,					//����
	GAME_STATE_GRAB,					//������
	GAME_STATE_PLAY,					//����
	GAME_STATE_END,						//����
	GAME_STATE_COUNT
} TGAMESTATE;

typedef enum tagGameStep				// ��Ϸ����
{
// 	GAME_STEP_NONE = 0,	
// 	GAME_STEP_TAKE,
// 	GAME_STEP_GRAB,
// 	GAME_STEP_PLAY,
// 	GAME_STEP_END,
// 	GAME_STEP_COUNT
 } TGAMESTEP;

#endif//__GAMELOGIC_DEFINE_H__

