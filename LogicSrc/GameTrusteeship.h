/************************************************************************/
/*  �ļ�����GameTrusteeship.h											*/
/*  ְ�ܣ�  �������й���												*/
/*  �汾��  1.0															*/
/*	������: tmfish														*/
/*  ���ڣ�  2013/12/10													*/
/************************************************************************/

#pragma once
#include "Card.h"
#include "Define.h"
#include "GameRule.h"

namespace Trused
{
	typedef int TCARD;
	typedef vector<TCARD> TCARDS;
}

class GameTrusteeship
{
public:
	GameTrusteeship(void);
	GameTrusteeship(TCardVector handCard){ m_HandCard = this->ConvertCardVectorToCards(handCard);}
	virtual ~GameTrusteeship(void);
public:
	//////////////////////////////////////////////////////////////////////////
	//ְ�ܣ����������д����ϼҳ��Ƶ����飬�����ҵ�����������Ϊѡ��״̬
	//������citFrontOutΪ�ϼҳ��Ƶ�������Ϣ
	//���أ�����ɹ��ҵ��������������飬����true���򷵻�false
	bool GetGreaterThanNextOut(CardTypeInfo ctiFrontOut);

protected:
	//�ж�����������Ƿ��п��Դ������
	bool HaveOutCard(int nCardsLen, CardTypeInfo ctiFrontOut ,Trused::TCARDS handCards, int nStep =1);
	//�ж�����������Ƿ��п��Դ������,���ж���������Ƿ���ը������
	bool HaveOutCardBoom(int nCardsLen, CardTypeInfo ctiFrontOut ,Trused::TCARDS handCards, int nStep =1);
	//�ж�����������Ƿ��п��Դ������,�ɻ�����
	bool HaveOutCardMaxBoom(int nCardsLen, CardTypeInfo ctiFrontOut ,Trused::TCARDS handCards, int nStep =1);
	//�����ж���������Ƿ��п��Դ���ķɻ�����
	bool HaveOutCardEx(int nCardsLen,CardTypeInfo ctiFrontOut,Trused::TCARDS handCards);
	//��ȡ���������ӵ�еĻ������ͣ���������Ϊ���š����ӡ����š�ը��
	//������ȡ���Ļ������ͷ���ָ����������
	void FillBaseCards();
	//��ȡ��������
	Trused::TCARDS GetBaseCards(int nCardsLen, int nType, Trused::TCARDS handCards);
	//��ȡָ�������е�ĳһ�ֻ����ƣ����ų���needReject��ֵ��ͬ����
	Trused::TCARDS GetBaseCardsEx(int nCardsLen, int nType, Trused::TCARDS handCards,Trused::TCARDS needReject);
	void Clear();
private:
	/*
	*  ��ְ�ܣ�STL erase�㷨�����ӣ������޳���������ֵ��ͬ����
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

private:
	//////////////////////////////////////////////////////////////////////////
	//��ŵķ�ʽΪ��С�����������
	Trused::TCARDS			m_PromptSingle;						//���������ӵ�е����е���
	Trused::TCARDS			m_PromptDouble;						//���������ӵ�е����ж���
	Trused::TCARDS			m_PromptThree;						//������������е�����
	Trused::TCARDS			m_PromptBoom;						//������������е�ը��
	Trused::TCARDS          m_PromptTWT;						//������������п���ɵ�������
	Trused::TCARDS			m_PromptTWO;						//������������п���ɵ�����һ
	Trused::TCARDS			m_PromptFWTO;						//������������п���ɵ��Ĵ���(��Ϊ����)
	Trused::TCARDS			m_PromptFWDT;						//������������п���ɵ��Ĵ���(��Ϊ����)
	Trused::TCARDS			m_PromptMTWO;						//������������п���ɵ�С�ɻ�
	Trused::TCARDS			m_PromptMTWT;						//������������п���ɵĴ�ɻ�
	//////////////////////////////////////////////////////////////////////////
	//����
	Trused::TCARDS m_HandCard;
	SHORT  m_nSeat;		

public:
		Trused::TCARDS m_PromptCard;
private:
	TCardVector ConvertCardsToCardVector(const Trused::TCARDS& vCards)
	{
		TCardVector dvCards;
		for(size_t i=0; i<vCards.size(); i++)
		{
			dvCards.push_back(TCards(vCards[i]));
		}

		return dvCards;
	}
	Trused::TCARDS ConvertCardVectorToCards(const TCardVector& dvCards)
	{
		Trused::TCARDS vCards;
		for(size_t i=0; i<dvCards.size(); i++)
		{
			vCards.push_back(dvCards[i].GetID());
		}

		return vCards;
	}
};
