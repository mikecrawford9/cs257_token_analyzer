#include "stdafx.h"
#include "DataStorage.h"
#include <algorithm>
#include <iostream>
#include <string>

using namespace std;

CDataStorage::CDataStorage(void)
{
}


CDataStorage::~CDataStorage(void)
{
}


bool CDataStorage::OpenConnection(string sDSN)
{
    connection = OCI_ConnectionCreate("xe", "kyoung", "dummypw", OCI_SESSION_DEFAULT);
    if (connection == NULL) {
        return false;
    } else {
        return true;
    }
}

#if DBSUPPORT
bool CDataStorage::OpenRecordset(ADODB::_RecordsetPtr& spRsNew, string sSQL,  long lMaxRecords, long eCursorType, long eLockType, bool bDisconnect)
{
	bool bRtn = false;

	try
	{
		ADODB::_RecordsetPtr loRsNew;
		HRESULT hr = loRsNew.CreateInstance(__uuidof(ADODB::Recordset));
		if (SUCCEEDED(hr))
		{
			//set rs parameters
			loRsNew->put_CacheSize( lMaxRecords > 0 ? lMaxRecords : 1 );
			loRsNew->put_MaxRecords( lMaxRecords > 0 ? lMaxRecords : 0 );
			loRsNew->put_CursorType( (ADODB::CursorTypeEnum)eCursorType );
			loRsNew->put_LockType( (ADODB::LockTypeEnum)eLockType );
			//open the rs
			if( m_AdoConnection != NULL )
			{
				// BSTR sSQL string to variant
				CComVariant vSQL( CComBSTR(sSQL.c_str()) );
				CComVariant voAdoConn( m_AdoConnection.GetInterfacePtr() );
				loRsNew->Open(vSQL, voAdoConn,
					( (ADODB::CursorTypeEnum)eCursorType ),
					( (ADODB::LockTypeEnum)eLockType ),
				ADODB::adCmdText);
			}
        
			//If disconnection was requested disconnect the rs
			if( bDisconnect )
				loRsNew->put_ActiveConnection( vtMissing );

			hr = loRsNew->QueryInterface(loRsNew.GetIID(),(void**)&spRsNew );

			if( hr == S_OK )
				bRtn = true;
		}
		
	}
	catch(_com_error &e)
	{
		//Handle errors
		char ErrorStr[1000];
		_bstr_t bstrSource(e.Source());
		_bstr_t bstrDescription(e.Description());

		sprintf_s(ErrorStr, 1000, "OpenRecordset Error\n\tCode = %08lx\n\tCode meaning = %s\n\tSource = %s\n\tDescription = %s\n",
			e.Error(), e.ErrorMessage(), (LPCSTR)bstrSource, (LPCSTR)bstrDescription );
		
		OutputDebugString(ErrorStr);
	}

	return bRtn;
}
#endif

#if 0
bool CDataStorage::ExecuteSQL(string & sSQL)
{
    OCI_Statement *st;

    st = OCI_StatementCreate(connection);
    if (!st) { return false; }

    if (!OCI_Prepare(st, sSQL.c_str()) || !OCI_Execute(st)) {
        return false;
    }

    if (!OCI_Commit(connection)) {
        return false;
    }

    return true;
}
#endif

bool CDataStorage::Upsert(CToken& t)
{
    OCI_Statement *st;
    char sSQL[1000];
    int rows;
    string token = t.getToken();
//    char *token = replace(orig_token.begin(), orig_token.end(), '.', ' ');
    replace(token.begin(), token.end(), '\'', ' ');

    sprintf(sSQL, "update Concepts set Tokens = '%s', Frequency = %ld, DocFrequency = %ld, Perm = %d where ID = %ld", token.c_str(), t.Freq(), t.DocFreq(), 0, t.DBID());

    st = OCI_StatementCreate(connection);
    if (!st) { return false; }

    if (!OCI_Prepare(st, sSQL) || !OCI_Execute(st)) {
printf("@@@ A\n");
        return false;
    }

    rows = OCI_GetAffectedRows(st);
    if (rows == 0) {
        OCI_StatementFree(st);
        st = OCI_StatementCreate(connection);
        if (!st) { return false; }
        sprintf(sSQL, "insert into Concepts (Tokens, Frequency, DocFrequency, Perm, TokenCount, ID) values ('%s', %ld, %ld, %d, %d, %ld)", token.c_str(), t.Freq(), t.DocFreq(), 0, 1, t.DBID());
        if (!OCI_Prepare(st, sSQL) || !OCI_Execute(st)) {
printf("@@@ B\n");
            return false;
        }
    }

    if (!OCI_Commit(connection)) {
printf("@@@ C\n");
        return false;
    }

    OCI_StatementFree(st);

    return true;
}
