#include "stdafx.h"
#include "DataStorage.h"


CDataStorage::CDataStorage(void)
{
}


CDataStorage::~CDataStorage(void)
{
}


bool CDataStorage::OpenConnection(string sDSN, short iTimeOut, long eCursorLocation, long eConnectMode)
{
	bool bRtn=false;
#if 0
	USES_CONVERSION;

	try {
		HRESULT hr = m_AdoConnection.CreateInstance(__uuidof(ADODB::Connection));
		if (SUCCEEDED(hr))
		{
			m_DSN = sDSN;
			_bstr_t aBSTR = T2OLE ((LPTSTR)sDSN.c_str() );

			m_AdoConnection->put_ConnectionString(aBSTR);		
			m_AdoConnection->put_CommandTimeout( iTimeOut );
			m_AdoConnection->put_CursorLocation( (ADODB::CursorLocationEnum )eCursorLocation );
			m_AdoConnection->put_Mode( (ADODB::ConnectModeEnum )eConnectMode );
		
			OutputDebugString(aBSTR);
			hr = m_AdoConnection->Open(aBSTR,_bstr_t(""),_bstr_t(""),ADODB::adOptionUnspecified);
		
			if( hr == S_OK )
				bRtn = true;
		}
	}
	catch ( _com_error ex)
	{
		OutputDebugString( ex.Description());
	}

#endif
	return bRtn;
}

bool CDataStorage::CloseConnection()
{
#if 0
	if( m_AdoConnection != NULL )
	{
		long connState;
		m_AdoConnection->get_State( &connState );
		if( connState == ADODB::adStateOpen )
			m_AdoConnection->Close();
	}

	m_AdoConnection=NULL;

#endif
	return true;
}

#if 0
bool CDataStorage::OpenRecordset(ADODB::_RecordsetPtr& spRsNew, string sSQL,  long lMaxRecords, long eCursorType, long eLockType, bool bDisconnect)
{
	bool bRtn = false;
	USES_CONVERSION;

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

bool CDataStorage::ExecuteSQL(string & sSQL)
{
	bool bRtn = false;
#if 0
	HRESULT hr = E_FAIL;

	try
	{
		ADODB::_RecordsetPtr spRs = NULL;
		spRs = m_AdoConnection->Execute(_bstr_t(sSQL.c_str()),&vtMissing, -1);

		if( spRs != NULL )
			bRtn = true;
	}
	catch(_com_error &e)
	{
		//Handle errors
		char ErrorStr[1000];
		_bstr_t bstrSource(e.Source());
		_bstr_t bstrDescription(e.Description());

		sprintf_s(ErrorStr, 1000, "ExecuteSQL Error\n\tCode = %08lx\n\tCode meaning = %s\n\tSource = %s\n\tDescription = %s\n",
			e.Error(), e.ErrorMessage(), (LPCSTR)bstrSource, (LPCSTR)bstrDescription );
		OutputDebugString(ErrorStr);
	}

#endif
   	return bRtn;
}



bool CDataStorage::TestConnection(void)
{
	bool bRtn = false;
#if 0
        
        bRtn = OpenConnection("Provider=SQLOLEDB;Data Source=127.0.0.1;Initial Catalog=SemanticDB;User Id=cs267;Password=harmonic");

	if (bRtn)
	{
		ADODB::_RecordsetPtr rs = NULL;
		OpenRecordset(rs,"SELECT * FROM URLPages");

		while(!(rs->EndOfFile))
		{
			CComVariant cv = rs->Fields->GetItem("Location")->GetValue();

			_bstr_t bs = cv.bstrVal;
			OutputDebugString(bs);

			rs->MoveNext();
		}

		string sql="exec sp_DBInsert URLPages, 'ID, URL, Location','''13'', ''URL13'', ''LOC13'' '";
		ExecuteSQL(sql);
	}

#endif
	return bRtn;
}
