/*
　　　　　　　　┏┓　　　┏┓ 
　　　　　　　┏┛┻━━━┛┻┓ 
　　　　　　　┃　　　　　　　┃ 　 
　　　　　　　┃　　　━　　　┃ 
　　　　　　　┃　┳┛　┗┳　┃ 
　　　　　　　┃　　　　　　　┃ 
　　　　　　　┃　　　┻　　　┃ 
　　　　　　　┃　　　　　　　┃ 
　　　　　　　┗━┓　　　┏━┛ 
　　　　　　　　　┃　　　┃神兽庇佑，代码无bug　　　　　　　　　 
　　　　　　　　　┃　　　┃code is far away from bug with the animal protecting 
　　　　　　　　　┃　　　┗━━━┓ 
　　　　　　　　　┃　　　　　　　┣┓ 
　　　　　　　　　┃　　　　　　　┏┛ 
　　　　　　　　　┗┓┓┏━┳┓┏┛ 
　　　　　　　　　　┃┫┫　┃┫┫ 
　　　　　　　　　　┗┻┛　┗┻┛ 
		如果代码显示效果不理想，可尝试将字体设置为方正兰亭中黑13号字体
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 职能：windows调试输出类
// 应用：在window窗口程序中，调试时需要输出一些数据，或者需要实时跟踪数据变化，
//		 VS调试模式但不执行比较麻烦或者不方便直接在程序中输出，便可调用该类.
// 表现：调用该类将会创建打开一个win32控制台窗口，可在此窗口输出相关信息。
// 
//提供接口：DebugString,函数默认只有量个参数，参数1指定需要打印到控制台窗口的程序
//				   支持TCardVector、TCards、String、vector<TCardVector>,参数2指定
//					标题，
//				   如有需要， 请自行重载扩展
//
// 注意事项：1.调用该类需要自定义一个名为__DEBUGOUT__的宏，若没有定义该宏，所有的调用
//           都只是执行一个空函数。默认在本头文件中定义，如果需要停用本类，请将
//			 __DEBUGOUT__去掉
//			 2.本类没有使用多线程或多进程技术，关掉程序本身或者win32控制台窗口都将
//			 结束进程
//																		小炮哥 
//																		2014年6月28日
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef  DEBUG_OUT_H
#define DEBUG_OUT_H
#include "Define.h"
#include "Card.h"

class DebugOut
{
public:
	DebugOut(void);
	~DebugOut(void);
public:
	virtual void DebugString(const string& st, string sHead="默认标题");
	virtual void DebugString(const TCardVector& dvSt, string sHead);
	virtual void DebugString(const vector<TCardVector>& cvSt, string sHead);
	virtual void DebugString(const int nSt,string sHead);
};

#endif

