#ifndef _DATASTORE_H
#define _DATASTORE_H

#pragma once
//#import "msado15.dll" rename("EOF", "EndOfFile") //raw_interfaces_only	
//#include <atlbase.h>
//#include <AtlConv.h>
//#include <atlcomcli.h>

class CDataStorage
{
	string m_DSN;
//	ADODB::_ConnectionPtr m_AdoConnection;

public:
	CDataStorage(void);
	~CDataStorage(void);

	bool OpenConnection(string sDSN, short iTimeOut=30, long eCursorLocation=3, long eConnectMode=3);
	bool CloseConnection();
//	bool OpenRecordset(ADODB::_RecordsetPtr& spRsNew, string sSQL,  long lMaxRecords=-1, long eCursorType=2, long eLockType=4, bool bDisconnect=true);
	bool ExecuteSQL(string & sSQL);
	bool TestConnection(void);
};

#endif
