
#include "DebugOut.h"

DebugOut::DebugOut(void )
{
 #ifdef  __DEBUGOUT__
 		AllocConsole();
 #endif
}

DebugOut::~DebugOut(void)
{
#ifdef __DEBUGOUT__
	FreeConsole();
#endif
}


void DebugOut::DebugString(const string& st, string sHead)
{
#ifdef __DEBUGOUT__
		std::string str = sHead;
		str += ":";
		if (st.empty()) 
		{
			str +="ЮЊПе\n";
		}
		
		DWORD numOfCharWritten =0;
	 	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), str.c_str(), str.size(), &numOfCharWritten, NULL);
	 	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),"\n",1,&numOfCharWritten,NULL);
#endif
}

void DebugOut::DebugString(const TCardVector& dvSt, string sHead)
{
#ifdef __DEBUGOUT__
	std::string str = sHead;
	str += ":\n";
	if (dvSt.empty())
	{
		str += "ЮЊПе\n";
	}else 
	{
		for (int i=0; i<dvSt.size(); ++i)
		{
			str += dvSt.at(i).GetName();
		}
		str += "\n";
	}
	
	DWORD numOfCharWritten =0;
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), str.c_str(), str.size(), &numOfCharWritten, NULL);
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),"\n",1,&numOfCharWritten,NULL);
#endif
}

void DebugOut::DebugString(const vector<TCardVector>& cvSt, string sHead)
{
#ifdef __DEBUGOUT__
	std::string str = sHead;
	str += ":\n";
	if (cvSt.empty())
	{
		str += "ЮЊПе\n";
	}else 
	{
		for (int i=0; i<cvSt.size(); ++i)
		{
			for (int j=0; j<cvSt.at(i).size(); ++j)
			{
				str += cvSt.at(i).at(j).GetName();
			}
			str += "\n";
		}
	}

	DWORD numOfCharWritten =0;
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), str.c_str(), str.size(), &numOfCharWritten, NULL);
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),"\n",1,&numOfCharWritten,NULL);
#endif
}

void DebugOut::DebugString(const int nSt,string sHead)
{
#ifdef __DEBUGOUT__
	std::string str = sHead;
	str += ":\n";
	char buf[10] = {0};
	sprintf(buf,"%d",nSt);
	str += buf;
	DWORD numOfCharWritten =0;
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), str.c_str(), str.size(), &numOfCharWritten, NULL);
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),"\n",1,&numOfCharWritten,NULL);
#endif
}