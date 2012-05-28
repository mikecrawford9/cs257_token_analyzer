#ifndef _DATASTORE_H
#define _DATASTORE_H

#pragma once
//#import "msado15.dll" rename("EOF", "EndOfFile") //raw_interfaces_only	
//#include <atlbase.h>
//#include <AtlConv.h>
//#include <atlcomcli.h>
#include "ocilib.h"

class CDataStorage
{
	string m_DSN;
//	ADODB::_ConnectionPtr m_AdoConnection;
        OCI_Connection *connection;

public:
	CDataStorage(void);
	~CDataStorage(void);

	bool OpenConnection(string sDSN);
//	bool CloseConnection();
//	bool OpenRecordset(ADODB::_RecordsetPtr& spRsNew, string sSQL,  long lMaxRecords=-1, long eCursorType=2, long eLockType=4, bool bDisconnect=true);
	bool ExecuteSQL(string & sSQL);
        bool Upsert(CToken & token);
//	bool TestConnection(void);
};

#endif
