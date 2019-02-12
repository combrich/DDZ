#pragma once

#include <ctime>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

typedef unsigned char	BYTE;

#define safe_delete(p) { if(p) { delete p; p = NULL; } }
#define safe_delete_arr(p) { if(p) { delete[] p; p = NULL; } }

//////////////////////////////////////////////////////////////////////////
//	随机数发生器
//
//////////////////////////////////////////////////////////////////////////
class CRandom
{
public:
	CRandom(){}
	~CRandom(){}
public:
	// 更新随机种子
	static void InitRandSeed()
	{
		int seed = (int)time(NULL);
		seed = seed / 2;
		srand((unsigned int)seed);
	}
	static int Random_Int(int nMin, int nMax)
	{
		int nRand = rand() % (nMax - nMin + 1);
		return (nRand + nMin);
	}
};


//////////////////////////////////////////////////////////////////////////
//	数据向量
//
//////////////////////////////////////////////////////////////////////////
template<typename _Ty>
class DataVector : public vector<_Ty>
{
public:
	explicit DataVector(){}
	DataVector(const DataVector<_Ty>& cell);
	virtual ~DataVector(){}
public:
	DataVector<_Ty>& operator=(const DataVector<_Ty>& cell);
	DataVector<_Ty>& operator-=(const _Ty& _Val);
	DataVector<_Ty>& operator-=(const DataVector<_Ty>& cell);
	DataVector<_Ty>& operator+=(const _Ty& _Val);
	DataVector<_Ty>& operator+=(const DataVector<_Ty>& cell);
	bool operator==(const DataVector<_Ty>& cell) const;
	bool operator!=(const DataVector<_Ty>& cell) const;
	//_Ty& operator[](int index) { return vector<_Ty>::operator[](index); }
	//const _Ty& operator[](int index) const { return vector<_Ty>::operator[](index); }

public:
	// 是否含有子集 subCell
	bool haveSubclass(const DataVector<_Ty>& subCell) const;
	typedef bool (*LessComPare)(const _Ty&, const _Ty&);
	bool haveSubclass(const DataVector<_Ty>& subCell, LessComPare _LC) const;
	// 获取指定长度的子集
	bool getSubclass(int nPosStart, int nCount, DataVector<_Ty>& dvSub) const;
private:
	
};


template<typename _Ty>
DataVector<_Ty>::DataVector( const DataVector<_Ty>& cell )
	: vector<_Ty>(cell)
{
	if(this != &cell)
	{
	}
}

template<typename _Ty>
DataVector<_Ty>& DataVector<_Ty>::operator=( const DataVector<_Ty>& cell )
{
	if(this != &cell)
	{
		this->clear();
		vector<_Ty>::operator = (cell);
	}

	return *this;
}

template<typename _Ty>
DataVector<_Ty>& DataVector<_Ty>::operator-=( const _Ty& _Val )
{
	typename vector< _Ty>::iterator iter = std::find(this->begin(), this->end(), _Val);
	if(iter != this->end())
	{
		this->erase(iter);
	}

	return *this;
}

template<typename _Ty>
DataVector<_Ty>& DataVector<_Ty>::operator-=( const DataVector<_Ty>& cell )
{
	for(size_t i=0; i<cell.size(); i++)
	{
		this->operator -= (cell[i]);
	}

	return *this;
}

template<typename _Ty>
DataVector<_Ty>& DataVector<_Ty>::operator+=( const _Ty& _Val )
{
	this->push_back(_Val);

	return *this;
}

template<typename _Ty>
DataVector<_Ty>& DataVector<_Ty>::operator+=( const DataVector<_Ty>& cell )
{
	this->insert(this->end(), cell.begin(), cell.end());

	return *this;
}

template<typename _Ty>
bool DataVector<_Ty>::operator==( const DataVector<_Ty>& cell ) const
{
	return ( this->size() == cell.size() && haveSubclass(cell) );
}

template<typename _Ty>
bool DataVector<_Ty>::operator!=( const DataVector<_Ty>& cell ) const
{
	return !(*this == cell);
}

template<typename _Ty>
bool DataVector<_Ty>::haveSubclass(const DataVector<_Ty>& subCell) const
{
	if(subCell.size() > this->size())
	{
		return false;
	}

	DataVector<_Ty> tmpDV(*this);
	tmpDV -= subCell;

	return ( this->size() - tmpDV.size() == subCell.size() );
}

template<typename _Ty>
bool DataVector<_Ty>::haveSubclass( const DataVector<_Ty>& subCell, LessComPare _LC ) const
{
	DataVector<_Ty> tmpDV(*this);
	DataVector<_Ty> tmpDVSub(subCell);
	std::sort(tmpDV.begin(), tmpDV.end(), _LC);
	std::sort(tmpDVSub.begin(), tmpDVSub.end(), _LC);

	return std::includes(tmpDV.begin(), tmpDV.end(), tmpDVSub.begin(), tmpDVSub.end(), _LC);
}

template<typename _Ty>
bool DataVector<_Ty>::getSubclass( int nPosStart, int nCount, DataVector<_Ty>& dvSub ) const
{
	dvSub.clear();

	int nEnd = (int)nPosStart + (nCount-1);
	if( nCount > 0 && (size_t)nEnd < this->size() )
	{
		typename DataVector<_Ty>::const_iterator iterStart = this->begin() + nPosStart;
		dvSub.insert(dvSub.end(), iterStart, iterStart+nCount);

		return true;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////
//  单件
//
//////////////////////////////////////////////////////////////////////////
template <typename T> class Singleton
{
protected:

	static T* ms_Singleton;

public:
	Singleton( void )
	{
		assert( !ms_Singleton );
#if defined( _MSC_VER ) && _MSC_VER < 1200	 
		int offset = (int)(T*)1 - (int)(Singleton <T>*)(T*)1;
		ms_Singleton = (T*)((int)this + offset);
#else
		ms_Singleton = static_cast< T* >( this );
#endif
	}
	~Singleton( void )
	{  assert( ms_Singleton );  ms_Singleton = 0;  }
	static T& getSingleton( void )
	{	assert( ms_Singleton );  return ( *ms_Singleton ); }
	static T* getSingletonPtr( void )
	{ return ms_Singleton; }
};

//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////