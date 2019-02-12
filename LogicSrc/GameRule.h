#pragma once
#include "Card.h"
#include <functional>

//����
typedef enum tagCardType
{
	CT_NONE			= 0x00,	// ������,��������
	CT_SINGLE,				// ����
	CT_DOUBLE,				// һ��
	CT_MULTIDOUBLETWO,		// ������ɵĽ��ö�
	CT_MULTITHREE,			// ������ɵĽ��ö�
	CT_THREE,				// ����
	CT_THREEWITHTWO,		// ������
	CT_MULTITHREEWITHTWO,	// ��ɻ�/��һ��
	CT_THREEWITHONE,		// ����һ
	CT_MULTITHREEWITHONE,	// С�ɻ�
	CT_MULTI,				// ˳��
	CT_FOURWITHTWO,			// �Ĵ���������������(����AAA25)
	CT_FOURWITHDTWO,		// �Ĵ���������������(����AAA2233)
	CT_BOOM,				// ը��
	CT_MAXBOOM,				// һ�Թ�
	CT_COUNT,
	
}TCARDTYPE;

//������Ϣ��
class CardTypeInfo
{
public:
	CardTypeInfo() : nType(CT_NONE){ }
	~CardTypeInfo(){ }
public:
	TCARDTYPE	nType;			// ����
	size_t					nCount;			// �������Ƶĸ���
	size_t					nLevel;			// ���𣬶���Ϊ�����������ֵ֮��
								// ������������һ���ɻ�Ϊ������֮��
public:
	void reset()
	{
		nType  =  CT_NONE;
		nCount = 0;
		nLevel = 0;
	}
	CardTypeInfo& operator=(CardTypeInfo cti)
	{
		this->nCount = cti.nCount;
		this->nLevel = cti.nLevel;
		this->nType  = cti.nType;
		return *this;
	}

	bool operator==(CardTypeInfo cti)
	{
		if(this->nCount == cti.nCount && 
			this->nLevel == cti.nLevel && 
			this->nType  == cti.nType
		 )
		{
			return true;
		}else
		{
			return false;
		}
	}
};

//�Ƿ��ܳ���
typedef enum tagOutCardResult
{
	OCR_OK = 0,				// �ܳ���
	OCR_OUTERROR,	// ���ܳ���
}OutCardResult;

//�Ƚ������ƵĴ�С
bool operator < (const TCards& card1, const TCards& card2);
bool operator > (const TCards& card1, const TCards& card2);
//ת��������Ϣ��BYTE������
int ConvertDataVector(const TCardVector& dvCards, BYTE* arrCardID, 
					  const int& nMaxCount);


//////////////////////////////////////////////////////////////////////////
//��Ϸ�����ࣺ
//		���Ĺ���--1.�ҳ�����
//				  2.�Ƚϴ�С
//				  3.�����Ƿ��������
//		
class GameRule
{
public:
	GameRule();
	~GameRule();

public:
	//////////////////////////////////////////////////////////////////////////
	//��ʼ������
	static void InitData(int nLevel = CV_2);
public:
	//////////////////////////////////////////////////////////////////////////
	//ְ�ܣ���ȡ���Ƶ�Ȩֵ
	//����������ָ����
	//���أ����ز������ݵĵ��Ƶ�Ȩֵ
	static int CardWeight(TCards card);
	//////////////////////////////////////////////////////////////////////////
	//ְ�ܣ���������
	//������dvCardsָ����Ҫ��������飬bLessָ�����ͷ�ʽ
	//		TURE��С����falseΪ�Ӵ�С����
	static void SortCards(TCardVector& dvCards, bool bLess = true);

	//////////////////////////////////////////////////////////////////////////
	//ְ�ܣ���ȡ��������
	//������dvCardsָ����Ҫ��ȡ���͵�����
	//���أ��ɹ���ȡ����󣬽�������Ϣ����������Ϣ�෵��		
	static CardTypeInfo GetCardType(const TCardVector& dvCards);

	//////////////////////////////////////////////////////////////////////////
	//ְ�ܣ��жϴ�������Ƿ���Գ���
	//������bFirstOutȷ���Ƿ��һ�ֳ��ƣ���һ�ֳ��ƿɳ���������
	//		dvFirstOutCardsָ��ǰһ����Ҵ������
	//		dvNowOutCardsָ����Ҫ�ж��Ƿ���ѹ���ϼҳ��Ƶ����顣
	//���أ����������ƣ������棬�����
	//��ע�� ȷ���Ƿ�������Ƶĺ���˼��
	//	����1.���ͣ�������������ͬ�����������ֵ֮��������������
	//	����2.ը�����κη�ը������,һ�Թ����
	//	����3.ը����ը����ѭ����1
	//	����4.������������һ���ɻ�����ѭ����1����ȽϹ���Ϊ��ͬ�������Ƶ���ֵ�Ƚϡ�
	//		  �����С��С
	//	����5.����һ�ֳ��ƻ������ʹ��ʱ������Գ��ƣ���һ�ֳ���ָ��
	//        ��Ϸ�ж��ַ������ƻ��߿��ֵ�һ�γ���
	//		  
	static bool CanOut(bool bFirstOut, const TCardVector& dvFirstOutCards,
					   const TCardVector& dvNowOutCards);

	//////////////////////////////////////////////////////////////////////////
	//ְ�ܣ�ȷ���Ƿ�ӵ�е�һ�ֳ����ʸ�
	//������pdvHandCardsָ��������ҵ�����
	//		nSizeָ������ָ���С
	//���أ����ӵ�е�һ�ֳ����ʸ񣬷�����λ������ָ���е�
	//		λ��
	//��ע���˴��ĵ�һ���ж��ʸ�Ϊ���Ὺ�ֵĵ�һ�γ����ʸ�
	static int FirstOutCard(const TCardVector  pdvHandCards[], int nSize);
	

	//////////////////////////////////////////////////////////////////////////
	//ְ�ܣ��ж������Ƿ�������
	//������dvCardsָ����Ҫ�жϵ����顢cardsInfo���ڷ���������Ϣ
	//���أ���������ƣ�����true�����򷵻�0
	//��ע���㷨����Ϊ�Ȳ�������͹�ʽ��ͨ�ʽ
	//		�Ȳ�����ͨ�ʽ��		An=a1+(n-1)*d
	//		�Ȳ�����ǰN��͹�˾��	Sn=n(n-1)*d/2����=������+ĩ���������2
	static bool IsMulti(const TCardVector& dvCards, CardTypeInfo& cardInfo);
	//////////////////////////////////////////////////////////////////////////
	//ְ�ܣ��ж������Ƿ��ǽ��ö�
	//������dvCardsָ����Ҫ�жϵ����顢cardsInfo���ڷ���������Ϣ
	//���أ���������ƣ�����true�����򷵻�0
	//��ע���㷨����Ϊ�Ȳ�������͹�ʽ��ͨ�ʽ
	//		�Ȳ�����ͨ�ʽ��		An=a1+(n-1)*d
	//		�Ȳ�����ǰN��͹�˾��	Sn=n(n-1)*d/2����=������+ĩ���������2
	static bool IsDoulbeMulti(const TCardVector& dvCards, CardTypeInfo& cardInfo);
	//////////////////////////////////////////////////////////////////////////
	//ְ�ܣ��ж������Ƿ������Ž��ö�
	//������dvCardsָ����Ҫ�жϵ����顢cardsInfo���ڷ���������Ϣ
	//���أ���������ƣ�����true�����򷵻�0
	//��ע���㷨����Ϊ�Ȳ�������͹�ʽ��ͨ�ʽ
	//		�Ȳ�����ͨ�ʽ��		An=a1+(n-1)*d
	//		�Ȳ�����ǰN��͹�˾��	Sn=n(n-1)*d/2����=������+ĩ���������2
	static bool IsThreeMulti(const TCardVector& dvCards, CardTypeInfo& cardInfo);

	//////////////////////////////////////////////////////////////////////////
	//ְ�ܣ��ж�������С�ɻ�
	//������dvCardsָ����Ҫ�жϵ����顢cardsInfo���ڷ���������Ϣ
	//���أ������С�ɻ�������true�����򷵻�0
	static bool IsMultiThreeWithOne(const TCardVector& dvCards, CardTypeInfo& cardInfo);
	//////////////////////////////////////////////////////////////////////////
	//ְ�ܣ��ж������Ƿ��Ǵ�ɻ�
	//������dvCardsָ����Ҫ�жϵ����顢cardsInfo���ڷ���������Ϣ
	//���أ�����Ǵ�ɻ��ɻ�������true�����򷵻�0
	static bool IsMultiThreeWithTwo(const TCardVector& dvCards, CardTypeInfo& cardInfo);
};