/*
������������������������������ 
�������������������ߩ��������ߩ� 
�������������������������������� �� 
�������������������������������� 
�������������������ש������ס��� 
�������������������������������� 
�����������������������ߡ������� 
�������������������������������� 
�������������������������������� 
�������������������������������ޱ��ӣ�������bug������������������ 
����������������������������code is far away from bug with the animal protecting 
������������������������������������ 
�����������������������������������ǩ� 
�������������������������������������� 
�����������������������������ש����� 
�����������������������ϩϡ����ϩ� 
�����������������������ߩ������ߩ� 
*/
// �����ذ����
//////////////////////////////////////////////////////////////////////////
//�ļ���:	Robot.h
//ְ�ܣ�	���˶�����������
//by��      tmfish at 2014-03-29
//////////////////////////////////////////////////////////////////////////
#ifndef __ROBOT_H__
#define __ROBOT_H__
#include "Card.h"
#include <vector>
#include "GameRule.h"
#include "GameTrusteeship.h"
#include <stdlib.h>
#include <map>
#include "DebugOut.h"

const int nRivalCount = 2;	//���ָ���
//#include <Windows.h>

//////////////////////////////////////////////////////////////////////////
//Ĭ������¸������Ȩֵ
const UINT nLevelSingle = 1;		//����
const UINT nLevelDouble = 2;		//����
const UINT nLevelThree	= 3;		//����
const UINT nLevelThreeM	= 6;		//��˳
const UINT nLevelDoubleM= 4;		//˫˳
const UINT nLevelSingleM= 5;		//��˳
const UINT nLevelBoom	= 7;		//ը��
//////////////////////////////////////////////////////////////////////////
//������Ϣ
struct CARDSTYLE{
	int max;//���ֵ
	int min;//��Сֵ
	int m_value;//Ȩֵ
	TCardVector cards;
	bool m_ISprimary;//����֮��,trueΪ����,falseΪ���� 
	CARDSTYLE()
	{
		m_ISprimary =false;
	}
	void Reset()
	{
		max = 0;
		min =0;
		m_value =0;
		cards.clear();
	}
	void Fill(TCardVector dv,int nType = CT_NONE);
};

typedef vector<CARDSTYLE> CT;
//������Ϣ
struct  SplitInfo //������Ϣ
{
	int s_nSeat;		//������ҵ�λ��
	CT s_boom;			//ը��
	CT s_three;			//����
	CT s_doule;			//����
	CT s_single;		//����
	CT s_threeMulti;	//��˳
	CT s_doubleMulti;	//˫˳
	CT s_singleMulti;   //��˳
	CT s_maxBoom;	//���
	SplitInfo operator=(const SplitInfo& des)
	{
		this->s_boom = des.s_boom;
		this->s_three = des.s_three;
		this->s_doule = des.s_doule;
		this->s_single = des.s_single;
		this->s_threeMulti = des.s_threeMulti;
		this->s_doubleMulti = des.s_doubleMulti;
		this->s_singleMulti = des.s_singleMulti;
		this->s_nSeat   = des.s_nSeat;
		this->s_maxBoom = des.s_maxBoom;
		return *this;
	}
	void Append(CT& des, CT from)
	{
		for ( int i=0; i<from.size(); i++)
		{
			des.push_back(from.at(i));
		}
	}
	SplitInfo operator+ (const SplitInfo& from)
	{
		SplitInfo tmp; 
		tmp = *this;
		Append(tmp.s_boom,from.s_boom);
		Append(tmp.s_three,from.s_three);
		Append(tmp.s_doule,from.s_doule);
		Append(tmp.s_single,from.s_single);
		Append(tmp.s_singleMulti,from.s_singleMulti);
		Append(tmp.s_doubleMulti,from.s_doubleMulti);
		Append(tmp.s_threeMulti,from.s_threeMulti);
		Append(tmp.s_maxBoom,from.s_maxBoom);
		tmp.s_nSeat =MAX_PLAYER;
		return tmp;
	}
	int GetHandCount() //��ȡ����
	{
		int nLevel = s_boom.size() +s_three.size() + s_doule.size() + s_single.size() + s_threeMulti.size()
			+s_doubleMulti.size()+s_singleMulti.size() + s_maxBoom.size();
		//����һ�����������ɻ���������Ҫ���⴦��
		int nThreeCount = 0;
		for (int i=0; i<s_threeMulti.size(); i++)
		{
			nThreeCount += (s_threeMulti.at(i).cards.size())/3;
		}
		nThreeCount += s_three.size();
		 int nMin = nThreeCount<(s_doule.size()+s_single.size())?nThreeCount:s_doule.size()+s_single.size();
		 nLevel-=nMin;
		// ը�����ü���������1.
	//	nLevel -= s_boom.size();
		return nLevel;
	}
	int GetOneTypeLevel(CT& who) //��ȡĳһ���͵�Ȩֵ
	{
		int nLevel = 0;
		for (int i=0; i<who.size();i++)
		{
			nLevel += who.at(i).m_value;
		}
		return nLevel;
	}
	int GetAllLevel() //��ȡ�ܵ�Ȩֵ
	{
		int nLevel = 0;
		nLevel = GetOneTypeLevel(s_boom) + GetOneTypeLevel(s_three)
			+ GetOneTypeLevel(s_doule) + GetOneTypeLevel(s_single)
			+ GetOneTypeLevel(s_threeMulti) + GetOneTypeLevel(s_doubleMulti)
			+ GetOneTypeLevel(s_singleMulti);			//��Ȩֵ
		return nLevel;
	}
};

//��Ҿ�������
struct PlayerWarring
{
	int nWL;
	int nSeat;
};

struct AICallOut {
	SHORT			returnscore;	// �зֽ׶�ʱAI�еķ��� 
};

//�зֽ׶�������Ϣ
struct AICallIn{
	BYTE	val[15];				//����ͳ�Ʊ� val[0]ΪCARD_3, val[1]ΪCARD_4 ��������
	SHORT	callscore;				//�з�Ȩֵ��Ȩֵ���㷽ʽ--��һ�ε���+1
};



typedef vector<TCardVector> CardsVector;

class CRobot 
{
public:
	CRobot(int nLandLord );	//Ĭ�Ϲ��캯���������Ҫ���ƻ��߸��ƣ�����ֻ��ͨ���ù��캯�����й���
	CRobot(TCardVector handCards[MAX_PLAYER]);
	CRobot(TCardVector dvHandCards);
	~CRobot();
//���ýӿ�
public:
	//�����˽е���
	bool RobotCall(AICallIn theCallIn, AICallOut& theCallOut, int nBaseRand = 80);
	enum {CALL_0_1,CALL_1_2,CALL_1_2_3,CALL_3};
	int RobotCallEx(AICallIn theCallIn, AICallOut& theCallOut,int nBaseRand = 80);
	//////////////////////////////////////////////////////////////////////////
	//ְ�ܣ������������� --����
	//˵����������������ʵ����ֻ�Ǽ������Լ�����
	//          ���Ƶ�Ȩֵ���Ƿ�ǿ����������Ϸ�߼�����
	//����Ȩֵ���壺ը��Ϊ5 ����Ϊ5��С��Ϊ3, 2Ϊ2,AΪ1
	//����������ָ����Ҫ����Ȩֵ����
	//���أ�����ָ�������Ȩֵ
	int  RobotGrabLand(TCardVector dvHand);
	
	//������ָ����ֵ��ͬ���Ƶ�����
	int GetCardNum(int nCardVal, TCardVector dvSearch);
	//////////////////////////////////////////////////////////////////////////
	//����ְ�ܣ������˳���
	//������	handCards����������е��ƣ�nSeatָ����ǰ�������λ��
	//
	//����:		������Ҫ������ƣ��ú���������Ҳ������ʧ�ܡ�
	//by��      tmfish  
	virtual TCardVector RobotPlayCards(TCardVector handCards[MAX_PLAYER],int nSeat);
	//////////////////////////////////////////////////////////////////////////
	//����ְ�ܣ������˸���
	//����˵��������1ָ��������ҵ����ƣ�����2ָ����Ҫѹ���ƣ�����3ָ����Ҫѹ������˭�����
	//                 ����4���ڷ��ظ��ƣ�����5ָ����˭�ڸ���
	//���أ�       ��������˳ɹ��ҵ�ѹ�ƣ�����true���������false����ѹ��
	virtual bool  RobotFollowCards(TCardVector handCards[MAX_PLAYER],TCardVector dvOut,
		int nOutSeat,TCardVector& dvRes,int nWhoOut);
	
	//ƴ�����ƣ����ڲ����ƾͻ�����˵���������Ƚ�׽��
	bool BendOverBackwardsFollow(TCardVector& dvRes,TCardVector dvOut);
	//�������ֻʣ�������Ƶ�ʱ��Ľ���
	bool PlayerFollowAtHand2(const TCardVector& dvOut ,TCardVector& dvRes);
	//��ȡ�����ʱ��
	int    GetSuggestTime();
	//�߼�����
	//���ý���ʱ��
	void SetSuggestTime(int nTime);
protected:
	
	//������������
	void Reset();
	//������ʱ����
	void ResetTmp();
	//����Ȩֵ������ѡ�����Ų��Ʒ�ʽ,����ָ���Ƿ�������Ƕ��ֵ��ƣ�
	//����������Ƕ��ֵ��ƣ���ô�����m_SplitRival�ṹ��
	void FindOptimalSplit(int bRival = false);
	//��CardVectorת��ΪCARDSTYPE
	//����ָ����ת�������飬����2����ת����Ľ��
	void ConvertCVToCT(CardsVector& who, CT& ct,int nType = CT_NONE);
	//��CARDSTYPEת��ΪCardVector
	//����ָ����ת�������飬����2����ת����Ľ��
	void ConvertCTToCV(CT& who,CardsVector& cv,int nType = CT_NONE);
	//������
	//����1ָ����Ҫ�������ƣ�����2ָ���������Ƶ����λ��
	//�����������ǰ������ң��򷵻����������򷵻�0
	int  AnalyzeCards(TCardVector dvHandCards, int nSeat);
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//ѡȡ��˳
	//ѡȡ����
	/* 
		��ȡ����С��һ��������˳������ʣ�������ѡȡ��С��һ���������������ƣ�ֱ��û������Ϊֹ��
		��ɺ������������չ����ʣ��������Ѿ�ȡ�����ƽ��бȶԣ����ĳ��ʣ���������֪����������
		�ɸ�������ƣ�����ϲ���һֱ���޷��ϲ�Ϊֹ�� ������ĳ���������ܹ��޷����ӳ�Ϊ�����
		���ƣ�����ϲ�Ϊһ�顣
	*/
	void ChoseSingleMultis(CardsVector& cvNeedFill, TCardVector& dvFrom,CardsVector cvDoubles);
	//////////////////////////////////////////////////////////////////////////
	//ѡȡ˫˳
	//ѡȡ����
	/*
		��һ���������еĶ��Ӽ�������Ȼ�󽫶�����������������Ķ��ӣ�����ϲ���(�����ܴ��)˫˳.
	*/
	void ChoseDoubleMultis(CardsVector& cvNeedFill, CardsVector& dvFrom);
	//////////////////////////////////////////////////////////////////////////
	//ѡȡ��˳
	/*
		һ�����е������������ܱȽϷ������ȡ������ 
		�������Ļ����Ͻ��бȽϣ����������������������ϲ��ɣ������ܴ�ģ���˳��
	*/
	void ChoseThreeMultis(CardsVector& cvNeedFill, CardsVector& dvFrom);
	//////////////////////////////////////////////////////////////////////////
	//ѡȡ����
	void ChoseThree(CardsVector& cvNeedFill, TCardVector& dvFrom);		
	//////////////////////////////////////////////////////////////////////////
	//ѡȡ����
	void ChoseDouble(CardsVector& cvNeedFill, TCardVector& dvFrom);
	//////////////////////////////////////////////////////////////////////////
	//ѡȡը��,
	void ChoseBoom(CardsVector& cvNeedFill, TCardVector& dvFrom);
	//////////////////////////////////////////////////////////////////////////
	//Ѱ�ҵ�ǰӦ���������
	//���أ� �ҵ���Ҫ������ƺ���з��أ��ú���������ʧ��
	TCardVector FindNowOut();

	//////////////////////////////////////////////////////////////////////////
	//����Ϊ���Ʋ��Һ����飬�������ܽ�Ϊ��ָ���������������ҵ����Դ������
	//����ҵ����Դ�����ƣ��򷵻�1����ʹ�ò���Res������Ҫ�������
	//���û���ҵ����Դ�����ƻ��ߴ�����Ʋ�������ô����0 
	int  FindOutFromThreeMulti(TCardVector& Res);//��˳
	int  FindOutFromDoubleMuti(TCardVector& Res);//˫˳
	int  FindOutFromSingleMuti(TCardVector& Res);//��˳
	int  FindOutFromThreeWTwo(TCardVector&Res); //������
	int  FindOutFromThrees(TCardVector& Res);//����
	int  FindOutFromDoubles(TCardVector& Res);//����
	int  FindOutFromSingle(TCardVector& Res);//����
	//�����ϼҵĳ���
	TCardVector FindOutFarmerSnipe();
	//�����¼ҵĳ���
	TCardVector FindOutFarmerFire();
	//////////////////////////////////////////////////////////////////////////
	//һ�������²��ҳ���
	//������ Res���ڷ��ز��ҵĽ��,Fromָ���Ӻδ�����
	//���أ� �ɹ��ҵ��򷵻�1�����򷵻�0
	int	 FindGeneralOut(TCardVector& Res,CardsVector From);
	//���Գ���
	int  FindOutUnthink(TCardVector& Res,CardsVector From);
	//���Լ���������Ϊ2��ʱ����ҳ���
	TCardVector FindOutAtLevel2(); 
	//���Լ���������Ϊ3��ʱ����ҳ���
	TCardVector FindOutAtLevel3();
	//ũ������Ϊ2��ʱ��Ѱ�ҳ���
	TCardVector FindOutAtFarmer2();
	//ũ������Ϊ3��ʱ��Ѱ�ҳ���
	TCardVector FindOutAtFarmer3();
	//ũ�������϶��ʱ��ĳ���
	TCardVector FindOutAtFarmerG();
	//����ʣ��һ���Ƶ�ʱ���ʱ��ũ�����
	TCardVector	FindOutWhenLand1();
	//����ʣ�������Ƶ�ʱ��ũ����ҳ���
	TCardVector FindOutWhenLand2();
	//�ж�ָ�������Ƿ���ѹ���Լ���ָ������
	bool RivalHaveGreaterThanMe(CardsVector myCards); 
	bool RivalHaveGreaterThanMe(TCardVector myCards);
	
	
	//�ж�ָ�������Ƿ�ӵ�о��Կ���Ȩ��
	//���Կ���Ȩ���ڶ�С���������Ҫ����
	bool HaveAbsoluteControl(CardsVector myCards);
	bool HaveAbsoluteControl(TCardVector myCards);
	//Ѱ�Ҷ��ѿ���ѹ������
	TCardVector FindTeammateCanOut();
	//һ������µĳ���
	//�ɹ����س��ƣ�����������ʧ��
	//����ָ���������ƣ��ݶ����ܲ�Ӧ�ô�������У����š����ӡ���˳
	TCardVector FindOutGeneral(bool bNotOutS=true,bool bNotOutD=true,bool bNotOutT=true);
	//�����Լ�ĳһ�����ж���Ҫ�����������
	int CalcGreaterThanRivalCount(CardsVector myCards, int& nSameCount);
	int CalcGreaterThanRivalCount(TCardVector myCards, int& nSameCount);
	//����ָ����ҵĶ����Ƿ�ӵ����
	//�ҳ�ĳһ��������ֵλ���м����
	TCardVector FindMidCard(CardsVector myCards);
	//Ӯ��Ƚ�С��ʱ������
	TCardVector FastOut(bool bMaxToMin=false);
	//�������ʣ�������Ƶ�ʱ��ĳ���
	TCardVector FindOutAtLand2();
	TCardVector FindOutAtLand3();
	//�������ʣ�¶��ֵ�ʱ��ĳ���
	TCardVector FindOutAtLandG();
	//ũ��ʣ��1���Ƶ�ʱ������ĳ��� 
	TCardVector FindOutWhenFarmer1();
	//ũ��ʣ��2���Ƶ�ʱ������ĳ���
	TCardVector FindOutWhenFarmer2();
	//Ϊ�ɻ��ҳ��
	void FindWing(TCardVector dvPlant,TCardVector dvSingle, CardsVector dvDoule);
	//�������
	bool FindFollowGeneral(TCardVector OutCard, TCardVector& FollwCards, int nLevel=0);
	//////////////////////////////////////////////////////////////////////////
	//Ѱ�ҵ�ǰӦ�ø�����,
	//�ҵ����ϵ��ƺ󣬷���true�������FollowCardsΪ���ƣ�
	//���û���ҵ��ܸ��ϵ��ƣ�����false,����FollowCards�ÿ�
	bool FindNowFollow(TCardVector OutCard, TCardVector& FollwCards);
	
	//��������
	bool FindFollowLand(const TCardVector& OutCard, TCardVector& FollowCards);
	//ũ�����
	bool FindFollowFarmer(const TCardVector& OutCard, TCardVector& FollowCards);
	//��ͨ����
	bool GeneralFollw(TCardVector& dvRes,TCardVector dvOut);
	bool CmpSingle(TCards T1,TCards T2);
	//////////////////////////////////////////////////////////////////////////
	//�����ܸ��Ƶ�����
	//������fromCardsָ����������ѹ�ƣ�dvOutCardsָ����Ҫѹ���ƣ�ResCards����
	//�����ҵ����ܹ����ϵ���
	//���أ�����ɹ��ҵ���ѹ�����ƣ�����true�����򷵻�false
	bool GetGreaterThanNextOut(TCardVector fromCards,TCardVector& dvOutCard,TCardVector& ResCards);
	//����ĳ��Ҷ����������Լ�Ҫ������������
	int CalcAllNotGreateThanCards();
	/////////////////////////////////////////////////////////////////////
	//ְ�ܣ������Ƿ�ӵ��ָ�����͵���
	//������nCardsLen���ͳ��ȣ�nType��Ŀ���ͣ�targetCardsĿ������
	//���أ�����Ŀ�����������е�nType���ͣ����Ŀ��������û��ָ��
	//      �����ͣ�����sizeΪ0������
	TCardVector GetBaseCards(int nCardsLen, int nType,  TCardVector targetCards);
	//////////////////////////////////////////////////////////////////////////
	//���������������
	//������ nLenָ����������ԭ�����ͳ��ȣ�nType��ָ�����������ڵ����ͣ�
	//       dvNeedFill��ָ����Ҫ������������,dvWhereָ����Ҫ���Ķ�ѡȡ��ֵ�������
	void FillCards(int nLen,int nType,CardsVector& dvNeedFill,TCardVector& dvWhere);
	//////////////////////////////////////////////////////////////////////////
	//Ѱ��dvCards�е���С������
	TCardVector FindFiveMultis(TCardVector& dvCards);
	//Ѱ�ҵ�ǰ�����������������������
	void FindMaxThreeCards();
	//����ָ�������и�����ֵ������
	void CalcCardConut(int nNum[],TCardVector dvCards);
	//���㱾��(�Լ��Ͷ���)�뵱ǰ�����ƵĴ�������
	void CalcBigNum(int& my, int& all);
	void CalcBigNum(int& nBig, TCardVector dvWho);
	//////////////////////////////////////////////////////////////////////////
	//��ȡ����ϼң�����¼�λ��
	int GetNextPlayerSeat(int nSeat);
	int GetLastPlayerSeat(int nSeat);
	//////////////////////////////////////////////////////////////////////////
	//��Ѱ�ܹ�ѹ������
	//OutCard ָ����Ҫѹ���ƣ�����fromCardsָ����Ҫ���Ķ���ѹ�ƣ�����������
	//�ҵ���ѹ��
	//����ɹ���fromCards���ҵ���ѹ��OutCard���ƣ���ô����true������FollowCards����
	//�ҵ���ѹ�ƣ����fromCards��û�������ܹ�ѹ��OutCard����ô����false���ҽ�FollowCards
	//���
	bool FindFollowCards(TCardVector OutCard,CardsVector fromCards,TCardVector& FollowCards, int nLevel);
	//Ϊ���ź���˳Ԥ���Ƚ�С�ĵ��Ż��߶���
	void ReservedSmallCards(bool bChoseSingle = false);
	//Ϊһ������������е�ÿһ����Ѱ����֮ƥ������ŵص�
	void FindFamily(TCardVector dvOrphan);
	//��������
	void ErgodicCards(bool bBoom,bool bThree, bool bDouble, bool bThreeMulti,bool bDouleMulti,bool bMulti,int nSeat );
	//��������һ�����������ɻ������
	//����1ָ�����飬���������������е�����
	void SeparationCards(const TCardVector& dvCards,CardsVector& cvMain);
	//�����޳�����������ֵ��dvDel��ͬ��Ԫ��
	void DelCV(CardsVector& cvWho,const TCardVector& dvDel);
protected:
	TCardVector     m_dvAllCards;											//�������֮��
	TCardVector		m_dvPlayerCards[MAX_PLAYER];		//�������
	TCardVector		m_dvBackupPlayerCards[MAX_PLAYER];	//������Ʊ���
	TCardVector		m_dvNowCards_1;				//������ҵ�����
	TCardVector		m_dvOldCards;				//������ҵ����Ƶı���
	int				m_nLandlord;				//�������λ��
	int				m_nOutSeat;					//�������λ��
	int				m_nLastOut;					//��һ���Ƴ������λ��
// �Լ������� 
	CardsVector		m_dvSingleMultis;	//��˳�������飬��ŷ���������е�˳
	CardsVector		m_dvDoubleMultis;	//˫˳�������飬��ŷ����������˫˳
	CardsVector		m_dvThreeMultis;	//��˳�������飬��ŷ������������˳
	CardsVector		m_dvDoubles;		//�������飬��ŷ���������ж���
	CardsVector     m_dvThrees;		    //�������飬��ŷ��������������
	CardsVector		m_dvBooms;			//ը�����飬��ŷ����������ը��
	TCardVector		m_dvSingles;		//�������飬��ŷ���������е���
	TCardVector     m_dvMaxBoom;	//��ը
	int				m_nRivalCount;								//��������ʣ��������
	int				m_nThreatenPlayCards;					//�����в��������������ڵ������ԣ������в���
	int				m_nLevel[MAX_PLAYER];			//�����������Ҫ�����ܰ�����ȫ������
	PlayerWarring	m_Warring[MAX_PLAYER];								//��������

	TCardVector m_dvMaxThreeCards;	//������������
	//��ʱ����
	TCardVector		cvTmpJoker;				//��ʱ�������
	TCardVector     cvTmpMaxBoom;		//��ʱ��Ż������
	CardsVector		cvTmpDoubleMulti;	//˫˳�������飬
	CardsVector		cvTmpThreeMulti;	//��˳�������飬
	CardsVector		cvTmpDouble;		//�������飬
	CardsVector     cvTmpThree;		    //�������飬
	CardsVector		cvTmpBoom;			//ը�����飬
	TCardVector		cvTmpSingle;		//��������
	CardsVector     cvTmpSingleMulti;	//��˳����

	int				m_nLevelRival;											//���ֵ�ǰ����
	vector<SplitInfo> m_allSplitInfo[3];							//���п��ܲ�����Ϣ
	SplitInfo		  m_PlayerSplit[MAX_PLAYER];			//����Ʒ�����Ľ��
	SplitInfo		 m_RivalSplit;											//���ֵ����Ʒ���
	int				m_nCardValue[15];									//����������и�����ֵ���������±�0ΪCV_3�����ε���
	int				m_nSuggestOutTime;								//����ĳ���ʱ��
private:
	TCardVector MeWillFinish();
	//�����ֿ�Ҫ�����ʱ��Ѱ�ҳ���
	TCardVector RivalWillFinish();
private:
	/*
	*  ��ְ�ܣ�STL remove_if�㷨�����ӣ������޳���������ֵ��ͬ����
	*/
	class IsEqualValue{
	public :
		IsEqualValue(TCards &temp):tCard(temp){} 
		bool operator() (TCards &temp)
		{
			if(tCard.GetID()==temp.GetID())
				return true;
			return false;
		}
	private :
		TCards  tCard;
	};	
public:
		enum
		{
			CARD_3	 = 0,
			CARD_4,
			CARD_5,
			CARD_6,
			CARD_7,
			CARD_8,
			CARD_9,
			CARD_10,
			CARD_J,
			CARD_Q,
			CARD_K,
			CARD_A,
			CARD_2,
			CARD_SJ,    // С��
			CARD_BJ, 	// ����
		};
private:
	void ShowDebug(CardsVector ct,char* name);
	void ShowDebug(TCardVector ct,char* name);
	void ShowDebug(SplitInfo sp,char* who);
	void ShowDebug(CT ct,char* name);
	
};


#endif  //__ROBOT_H__