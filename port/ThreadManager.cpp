#include "stdafx.h"
#include "TokenizerUtils.h"
#include "DataStorage.h"

//#define WIN32_LEAN_AND_MEAN 


//Each thread instance will run its own instance of this function.
#if 0
DWORD CThreadManager::ThreadProc() 
{
	static int nextThreadID=0;
	int threadID=++nextThreadID;
	printf("ThreadProc(ThreadID=%d) started\n",threadID);
	
	HANDLE events[]={mEndEvent, mTaskAvailEvent};

	long fileIndex;
	while (TRUE)
	{
		DWORD waitNum=WaitForMultipleObjects(2,events,FALSE, INFINITE);
		//printf("ThreadProc(ID=%d) WaitForMultiple returned %d\n",threadID, waitNum);
		if (waitNum==WAIT_OBJECT_0)
		{
			//printf("ThreadProc(ID=%d) received mEndEvent\n",threadID);
			break; // app ending
		}

		while (TRUE)
		{
			{				
				CSLock cslock(mCSFileListIndex); // Get access to the file index

				if (mCurrFileListIndex >= (long) mInputFileList->size())  
				{	// The last thread to finish the list should indicate no more files left.
					//printf("ThreadProc(ID=%d) ResetEvent(mTaskAvailEvent);\n",threadID);
					ResetEvent(mTaskAvailEvent);
					break; // Done, no more. Note this 'If' check is needed in case of race condition
				}

				fileIndex = mCurrFileListIndex++;
			}

			//printf("ThreadProc(ID=%d) parseFile(%s)\n",threadID, mInputFileList->at(fileIndex).c_str());
			parseFile(mInputFileList->at(fileIndex).c_str());

			{
				CSLock cslock(mCSFileListIndex);
				mCurrFilesProcessed++;  // whether succeeded or not, we have consumed this file
			}
		}
	
		//printf("ThreadProc(ID=%d) task done, SetEvent(mTaskCompletedEvent)\n",threadID);
		SetEvent(mTaskCompletedEvent);
	}

	printf("ThreadProc(ThreadID=%d) completed\n",threadID);
	return 0;
}
#endif


bool CThreadManager::mergeTupleList(long targetTupleCount, CFreqList& freqTuples)
{
//	CSLock cslock(mCSGuardTupleLists);

	map<string, CToken>& mapList = freqTuples.getList();
	map<string, CToken>::iterator it;
	for(it=mapList.begin(); it != mapList.end(); it++)
	{
		CToken& t = (it->second);
		mTupleLists[targetTupleCount].mergeToken(t, mNextConcept_DBID);
	}

	return true;
}

void CThreadManager::parseFile(const string& docName)
{
	P267_FILE *fp = P267_open(docName.c_str(), P267_IOREAD);
	if (fp != NULL)
	{
		CFreqList tempTupleList;

		deque<CToken> lsTokens;
		CToken aToken;
		string sTemp;

		int c,d=10;
		int wc = 0;

		//find file size
		fseek(fp, 0, SEEK_END); // seek to end of file 
		long filesize = ftell(fp); // get current file pointer 
		fseek(fp, 0, SEEK_SET); // seek back to beginning of file 
		//printf("\nparseFile(%s) with %d bytes....       \n",docName.c_str(), filesize);

		while (isDelimiter(c=P267_getcf(fp)));
	
		bool usAddressDetected=false;
		P267_ungetc(c,fp);
		while((c = P267_getcf(fp)) != P267_ENDFILE) 
		{
			if (!isLegalChar(c) && (c!=P267_NEWLINE) && (c!=P267_CARRIAGERETURN))
			{
				wc++;
				//fprintf(ofp," [0x%x]\t%d\t unprintable\n", c, wc);
			}
		
			if (isLegalChar(c))
			{ 
				aToken.clear();
				aToken.LastDocFound(docName);
				sTemp="";

				//Form a generic token with all legal chars
				do 
				{
					sTemp += c;
					//fprintf(ofp,"%c", c);
				} 
				while (isLegalChar(c=P267_getcf(fp)));
				P267_ungetc(c,fp);

				wc++;
				//fprintf(ofp,"\t%d\t%s\n",wc,srcfilename);
			
				aToken.set(sTemp.c_str(),wc);

				////// Determine whether the period character signifies end of sentence.
				//if (aToken.Token()[aToken.Token().length()-1] == '.' || aToken.Token()[aToken.Token().length()-1] == ',')
				//{
				//	//period is last char, remove it.
				//	string s = aToken.Token().substr(0,aToken.Token().length()-1);
				//	aToken.set(s.c_str(), aToken.Pos());

				//	wc++; // this accounts for the extra period just removed.
				//}
			
				if (aToken.Token().empty())
					continue; // it is blank after trimming so skip it

				/// Insert into our token list
				lsTokens.push_back(aToken);

				/////// Determine if possible US address and put it into a single token
				if (isUSState(aToken.Token()))
				{	
					usAddressDetected=true;  // allow one more iteration to consume the zip code.
				} 
				else if (usAddressDetected && isZipCode(aToken.Token()))
				{
					// Assume we have a zip code. This is a US address, so merge all relevant tokens into one.
					mergeUSAddress(lsTokens, wc);
					usAddressDetected=false;
				} 
				else 
				{
					usAddressDetected=false; //false alarm, not US address
				}
			} 
			else
			{
				//we try to analyze when there is newline, but it has to be greater than minsize but less than maxsize
	
				if ((lsTokens.size() >= MAXPARAGRAPH_SIZE) || 
					( ((c==P267_NEWLINE) || (c==P267_CARRIAGERETURN)) && (lsTokens.size() > MINPARAGRAPH_SIZE)) )
				{
					// permute within the paragraph
					long currPos=ftell(fp);
					//printf("\b\b\b\b\b\b\b\b[%05.2lf%%]",currPos*100.0/filesize);
					//printf("parseFile(%s) [%05.2lf%%]\n",docName.c_str(), currPos*100.0/filesize);
					permuteParagraph(mCurrTargetTupleCount, docName, lsTokens, tempTupleList);

					//mergeTupleList(mCurrTargetTupleCount, tempTupleList);
					mergeTokenList(lsTokens);
					lsTokens.clear();

					////// Output to file 
					//outputToFile(ofp, srcfilename, lsTokens);
				}		
			}

		}

		//permute and output remaining
		permuteParagraph(mCurrTargetTupleCount, docName, lsTokens, tempTupleList);

		//tempTupleList.pruneTermFreq(0.01, 0.3);
		tempTupleList.pruneTermFreq(TERMFREQ_FROM, TERMFREQ_TO);

		mergeTupleList(mCurrTargetTupleCount, tempTupleList);
		tempTupleList.clear();

		mergeTokenList(lsTokens);
		lsTokens.clear();

		lsTokens.clear();
		tempTupleList.clear();

		P267_close(fp);
	}
	else
	{
		printf("Can't open %s\n", docName.c_str());
	}

}

// targetTupleCount: the n-tuple we're trying to find.
bool CThreadManager::permuteParagraph(long targetTupleCount, const string& docName, deque<CToken>& lsTokens, CFreqList& freqTuples)
{
	static char PROGRESS[3] = { '/', '-', '\\' };

	CToken aToken;

	for (long i=0; i < (long) lsTokens.size(); i++)
	{
//		printf("\b%c", PROGRESS[i%3]); // visual displays progress
		aToken = lsTokens[i];

		if (targetTupleCount==1) {
			freqTuples.LogToken(aToken, mNextConcept_DBID);
		} else {
			// mTupleList[minus one] is stable so no need to acquire lock first in order to check it.
			if (mTupleLists[1].exists(aToken.Key()))
			{   // only permute if (n-1)-tuple exists
				permuteParagraph_r(2, targetTupleCount, aToken.Pos(), aToken.Token().c_str(), i+1, MIN(i+WINDOW_SIZE, lsTokens.size()), docName, lsTokens, freqTuples);
			}
		}
	}


	return true;
}

// This function is recursively called starting with 2-tuple and on.
// targetTupleCount: the target n-tuple. 
// nDepth: this starts at 2 and goes up til targetTupleCount.  When nDepth == TargetTupleCount, that is when recursion terminates.
bool CThreadManager::permuteParagraph_r(long nDepth, long targetTupleCount, long nToken1Pos, const char* sPrefix, long nStartIdx, long nEndIdx, const string& docName, deque<CToken>& lsTokens, CFreqList& freqTuples)
{
	CToken aToken, aTokenTuple;
	char sTemp[256];
	long i, toIdx;

	/* // Debug print
	printf("permuteParagraph(");
	for (i=nStartIdx; i < toIdx; i++)
	{
		aToken = lsTokens[i];
		printf("%s ",aToken.getToken());
	}
	printf(")\n%d\n\n", toIdx-nStartIdx);
	*/
	
	for (i=nStartIdx; i < nEndIdx; i++)
	{
		aToken = lsTokens[i];
		if (!mTupleLists[1].exists(aToken.Token()))
		{   
			continue; // only permute if second token has not been pruned AND it is not doubled
		}

		sprintf(sTemp, "%s %s", sPrefix, aToken.getToken());

		aTokenTuple.clear();
		aTokenTuple.LastDocFound(docName);
		aTokenTuple.set(sTemp, nToken1Pos);

		//if (nDepth == targetTupleCount)
		//	freqTuples.LogToken(aTokenTuple);
		//else
		//	permuteParagraph_r(nDepth+1,targetTupleCount, nToken1Pos, sTemp, i+1, nEndIdx, sSrcfilename, lsTokens, freqTuples);

		if (nDepth == targetTupleCount) {
			freqTuples.LogToken(aTokenTuple, mNextConcept_DBID);
		} else {
			// mTupleList[minus one] is stable so no need to acquire lock first in order to check it.
			if (mTupleLists[nDepth].exists(aTokenTuple.Key()))
			{   // only permute if (n-1)-tuple exists
				permuteParagraph_r(nDepth+1, targetTupleCount, aToken.Pos(), aTokenTuple.Token().c_str(), i+1, MIN(i+WINDOW_SIZE, lsTokens.size()), docName, lsTokens, freqTuples);
			}
		}
	}


	return true;

}

//output the first count of items to file.
//if there is any token to be output has an ending comma, the comma will be removed.  This was 
//an artifact in helping parse US address.
void CThreadManager::outputToFile(P267_FILE *ofp, char* filename, deque<CToken>& lsTokens)
{
	CToken aToken;
	deque<CToken>::const_iterator ii;
	char sSQL[1000];

	for(ii=lsTokens.begin(); ii<lsTokens.end(); ii++)
	{
		aToken = lsTokens.front();

		//throw away the token if it only contains the comma
		if (aToken.Token()!=",")
		{
			if (aToken.Token()[aToken.Token().length()-1] == ',') {
				string s = aToken.Token().substr(0,aToken.Token().length()-1);
				fprintf(ofp,"\"%s\",%s,%d\n",s.c_str(), filename, aToken.getPos());
				//fprintf(ofp,"%-20s\t%-30s\t%-10d\n",s.c_str(), srcfilename, aToken.getPos());
			} 
			else {
				fprintf(ofp,"\"%s\", %s,%d\n",aToken.getToken(),filename,aToken.getPos());
				//fprintf(ofp,"%-20s\t%-30s\t%-10d\n",aToken.getToken(),srcfilename,aToken.getPos());
				sprintf(sSQL, "exec sp_UpdateConcept %d, %s, %d, %d, %d, %d", aToken.DBID(), aToken.getToken(), 1, aToken.Freq(), aToken.DocFreq(),0); 
				string csSQL = sSQL;
#if Commenting_out_db_stuff_for_now
				if (!mDB->ExecuteSQL(csSQL))
				{
					printf("** Error executing: %s\n", sSQL);
				}
#endif
			}
		}

		lsTokens.pop_front();
	}
}

#if 0
bool CThreadManager::parseNtuple(long n, const string& sFilePrefix)
{
	printf("\n***** parseNtuple(analyzing %d-tuples) started *****\n",n);
	bool brc=true;
	
	bool writeToFileDone=false;

	{
		CSLock cslock(mCSFileListIndex);
		mCurrFileListIndex = 0;
		mCurrFilesProcessed = 0;
		mCurrTargetTupleCount = n;
	}

	SetEvent(mTaskAvailEvent);
	ResetEvent(mTaskCompletedEvent);

	//long completionCountdown =mThreadCount;

	long iProgress=0;
	while(true)
	{
		iProgress++;
		//{
		//	printf("parseNtuple waited mTaskCompletedEvent a Thread has completed\n");
		//	completionCountdown--;
		//}
		//printf("parseNtuple waited mTaskCompletedEvent completionCountdown=%d\n",completionCountdown);

		WaitForSingleObject(mTaskCompletedEvent, 1000);
		{
			CSLock cslock(mCSFileListIndex);
			if (mCurrFilesProcessed >= (long) mInputFileList->size())  
			{
				printf("\b\b\b\b\b\b\b\b\b\b\b[100%%][-]");
				break; // mCurrTargetTupleCount task is done!
			} 
			//printf("parseNtuple: mCurrFileListIndex=%d\n",mCurrFileListIndex);
			//printf("[%05.2lf%%]", mCurrFilesProcessed/(double) mInputFileList->size());
			printf("\b\b\b\b\b\b\b\b\b\b\b[%05.2lf%%][%c]", mCurrFilesProcessed*100.0/mInputFileList->size(), PROGRESS[iProgress%3]);
		}

		if (!writeToFileDone)
		{
			//use this cpu cycle to write to file of previous tuple count
			writeToFileDone=true;
			if (mCurrTargetTupleCount>=2)
			{
				closeMasterTokensFile();
				serialize(mCurrTargetTupleCount-1, sFilePrefix);
			}
		}
	}

	ResetEvent(mTaskAvailEvent);

	pruneNtuple(n);
	printf("\n===== parseNtuple(analyzing %d-tuples) completed =====\n",n);

	return brc;
}
#endif

bool CThreadManager::pruneNtuple(long n)
{
//	CSLock cslock(mCSGuardTupleLists);
	mTupleLists[n].pruneDocFreq(DOCFREQ_FROM, DOCFREQ_TO, (long) mInputFileList->size());

	//if (n==1)
	//	mTupleLists[n].pruneDocFreq(0.1, 0.5, (long) mInputFileList->size());
	//else 
		//mTupleLists[n].pruneDocFreq(0.003, 0.5, (long) mInputFileList->size());	

	return true;
}

CThreadManager::CThreadManager()
{ 
#if 0
	InitializeCriticalSection(&mCSFileListIndex); 
	InitializeCriticalSection(&mCSGuardTupleLists);
	InitializeCriticalSection(&mCSGuardMasterList);

	mTaskAvailEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	mEndEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	mTaskCompletedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	mMasterTokenCount = 0;
#endif
}

#if 0
void CThreadManager::SetInputFileList(std::deque<string>* fl)
{
	mInputFileList=fl;
}
#endif

CThreadManager::~CThreadManager()
{
#if 0
	DeleteCriticalSection(&mCSFileListIndex);
	DeleteCriticalSection(&mCSGuardTupleLists);
	DeleteCriticalSection(&mCSGuardMasterList);
	CloseHandle(mTaskAvailEvent);
	CloseHandle(mEndEvent);
	CloseHandle(mTaskCompletedEvent);
#endif

}


#if 0
void CThreadManager::InitThreads(int threadcount)
{
	mThreadCount = threadcount;
	mThreadHandles = new HANDLE[mThreadCount];

	for (int i=0; i<mThreadCount; i++)
	{
		mThreadHandles[i] = CreateThread(NULL, 0, ThreadLaunch, this, 0, NULL);
	}
}


void CThreadManager::EndThreads()
{
	SetEvent(mEndEvent);
}
#endif

// form the address into a single token
bool CThreadManager::mergeUSAddress(deque<CToken>& lsTokens, int& wc)
{
	bool bMerged=false;
	//step back until number or max of 20 tokens

	int max=20;
	CToken aToken;
	deque<CToken>::iterator ii;
	ii=lsTokens.end();
	ii--; //this is the zip code
	while (ii!=lsTokens.begin() && max-- > 0)
	{
		ii--;
		if (isNumber(ii->Token()))
		{
			//start concatenate from here
			int tokenCount=0;
			deque<CToken>::iterator starterase=ii;
			string aUSAddress;
			while (ii!=lsTokens.end())
			{
				aUSAddress += ii->Token();
				aUSAddress += " ";
				tokenCount++;
				ii++;
			}

			wc -= tokenCount;

			while (tokenCount-- > 0)
			{
				lsTokens.pop_back();
			}

			aToken.set(aUSAddress.c_str(),wc);
			lsTokens.push_back(aToken);
			
			break;
		}

	}

	bMerged=true;
	return bMerged;
}

// Output 1 tuple file
void CThreadManager::serialize(long nTupleIndex, const string& outfilePrefix)
{
	P267_FILE *fpOut;
	char sfilename[512];

//	CSLock cslock2(mCSGuardTupleLists);
	sprintf(sfilename,"%s.%dtuple.txt",outfilePrefix.c_str(),nTupleIndex);
	printf("\nWriting %d-tuples to file '%s' ...\n", nTupleIndex, sfilename);
	if ((fpOut = P267_open(sfilename, P267_IOWRITE))!=NULL)
	{
		mTupleLists[nTupleIndex].serialize(fpOut, mDB, mMasterTokenCount, (long) mInputFileList->size());
		P267_close(fpOut);
	}
	printf("Writing %d-tuples completed.\n", nTupleIndex);
}


#if 0
void CThreadManager::toFiles(const string& outfilePrefix)
{
	printf("Writing to files...\n");
	P267_FILE *fpOut;
	char sfilename[512];

	//sprintf(sfilename,"%s.tokens.txt",outfilePrefix.c_str());
	//if ((fpOut = P267_open(sfilename, P267_IOWRITE))!=NULL)
	//{
	//	CSLock cslock(mCSGuardMasterList);
	//	deque<CToken>::iterator ii;

	//	fprintf(fpOut,"Total rows: %d\n==========================\n", mMasterTokenList.size());
	//	fprintf(fpOut,"Token     \tDocName     \tPosition\n");
	//	fprintf(fpOut,"--------------------------------------\n");
	//	for(ii= mMasterTokenList.begin(); ii<mMasterTokenList.end(); ii++)
	//	{ 
	//		fprintf(fpOut,"\"%s\", %s, %d\n", ii->Token().c_str(), "null", ii->getPos());
	//	}
	//	P267_close(fpOut);
	//}

	CSLock cslock2(mCSGuardTupleLists);
	for (int i=1; i <= 6; i++)
	{
		sprintf(sfilename,"%s.%dtuple.txt",outfilePrefix.c_str(),i);
		if ((fpOut = P267_open(sfilename, P267_IOWRITE))!=NULL)
		{
			mTupleLists[i].serialize(fpOut, mDB, mMasterTokenCount, (long) mInputFileList->size());
			P267_close(fpOut);
		}
	}
}
#endif

// Only do merge when mCurrTargetTupleCount==1
bool CThreadManager::mergeTokenList(deque<CToken>& lsTokens)
{
	if (mCurrTargetTupleCount!=1 || mfpMasterTokens==NULL)
		return false;

//	CSLock cslock(mCSGuardMasterList);
	deque<CToken>::iterator ii;

	for(ii=lsTokens.begin(); ii<lsTokens.end(); ii++)
	{
		fprintf(mfpMasterTokens,"%-30s   %-50s   %-10d\n", ii->Token().c_str(), ii->LastDocFound().c_str(), ii->getPos());
		mMasterTokenCount++;
		//mMasterTokenList.push_back(*ii);
	}
	fprintf(mfpMasterTokens,"<p>\n");

	return true;
}


#if 0
void CThreadManager::openMasterTokensFile(const string& outfilePrefix)
{
	char sfilename[512];

	sprintf(sfilename,"%s.tokens.txt",outfilePrefix.c_str());
	if ((mfpMasterTokens = P267_open(sfilename, P267_IOWRITE))!=NULL)
	{
		//fprintf(mfpMasterTokens,"Token     \tDocName     \tPosition\n");
		fprintf(mfpMasterTokens,"%-30s   %-50s   %-10s\n", "Token", "DocName", "Position");
		fprintf(mfpMasterTokens,"----------------------------------------------------------------------------------------------\n");
	}
}

void CThreadManager::closeMasterTokensFile()
{
	if (mfpMasterTokens)
	{	fprintf(mfpMasterTokens,"Total token count: %d\n", mMasterTokenCount);
		P267_close(mfpMasterTokens);
	}
	mfpMasterTokens=NULL;
}

void CThreadManager::InitFromDB(CDataStorage *pdb) 
{ 
	CSLock cslock(mCSGuardTupleLists);

	mDB=pdb; 
	ADODB::_RecordsetPtr pRS=NULL;
	char sSQL[1000];
	CToken aToken;
	CComVariant cvTokens, freq, docfreq, neverdel, dbid;

	sprintf(sSQL, "exec sp_GetMaxConceptID"); 
	if (mDB->OpenRecordset(pRS, sSQL))
	{
		mNextConcept_DBID = (long) pRS->Fields->Item["maxval"]->Value + 1;
	}

	for (int i=1; i<7; i++)
	{
		sprintf(sSQL, "exec sp_GetConcepts %d", i); 
		if (mDB->OpenRecordset(pRS, sSQL))
		{

			while (!pRS->EndOfFile)
			{
				aToken.clear();

				aToken.DBID((long)pRS->Fields->Item["ID"]->Value);
				
				cvTokens = pRS->Fields->Item["Tokens"]->Value;
				aToken.set((char*) bstr_t(cvTokens.bstrVal),0);
				
				aToken.Freq((long)pRS->Fields->Item["Frequency"]->Value);
				aToken.DocFreq((long)pRS->Fields->Item["DocFrequency"]->Value);
				aToken.Perm((int)pRS->Fields->Item["Perm"]->Value);

				mTupleLists[i].InitAdd(aToken);

				pRS->MoveNext();
			}

			pRS->Close();
		}
	}

	//sprintf(sSQL, "SELECT count(*) as total  FROM  [Concepts]"); 
	//if (mDB->OpenRecordset(pRS, sSQL))
	//{
	//	if (!pRS->EndOfFile)
	//	{
	//		CComVariant total = pRS->Fields->Item["total"]->Value;
	//		mNextConcept_DBID = total.lVal + 1;
	//	}

	//	//pRS->MoveNext();
	//	pRS->Close();
	//}

	//Build the mTupleLists from DB
}
#endif
