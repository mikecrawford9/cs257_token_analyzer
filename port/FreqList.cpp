#include "stdafx.h"
#include "FreqList.h"
#include "DataStorage.h"


bool CFreqList::exists(const string& tokenKey)
{
	bool bExists=false;
	map<string, CToken>::iterator it = mList.find(tokenKey);
	bExists = (it != mList.end()); 
	return bExists;
}

//used during retrieve from DB, does not assign new DBID
void CFreqList::InitAdd(CToken& token)
{
	mList.insert(map<string, CToken>::value_type(token.Key(),token));
}

// Increment token count by 1, add if not exist.
// Will increment nextDBID if it is used.
void CFreqList::LogToken(CToken& token, long& nextDBID)
{
	map<string, CToken>::iterator it;
	it=mList.find(token.Key());
	if (it != mList.end())
	{	//already in list
		CToken& t = (it->second);
		if (t.LastDocFound()==token.LastDocFound())
			t.IncrFreq(token.LastDocFound());
		else
			t.IncrFreq();
	} 
	else
	{	//not already in list
		token.IncrFreq();
		//token.DBID(nextDBID++);

		mList.insert(map<string, CToken>::value_type(token.Key(),token));
	}
}

// Take input token count and add it into total count.
void CFreqList::mergeToken(CToken& token, long& nextDBID)
{
	map<string, CToken>::iterator it;
	it=mList.find(token.Key());
	if (it != mList.end())
	{	//already in list
		CToken& t = (it->second);
		if (t.LastDocFound() != token.LastDocFound())
			t.DocFreq(t.DocFreq() + token.DocFreq());
		t.Freq(t.Freq() + token.Freq()); 
	} 
	else
	{	//not already in list
		token.DBID(nextDBID++);
		mList.insert(map<string, CToken>::value_type(token.Key(),token));
	}
}


void CFreqList::serialize(P267_FILE *ofp, CDataStorage *pDB, long totalTokenCount, long totalDocCount)
{
	fprintf(ofp,"Total rows: %ld\n==========================\n", mList.size());
	//fprintf(ofp,"Token     \tTermFreq     \tTermFreq%%     \tDocFreq     \tDocFreq%%\n");
	fprintf(ofp,"%-50s   %-5s   %-10s   %-5s   %-10s\n", "Token", "TF", "TF%", "DF", "DF%");
	fprintf(ofp,"-------------------------------------------------------------------------------------------\n");
		
	map<string, CToken>::iterator it;
	double aDocFreqPercent;
	double aTermFreqPercent;
	char sSQL[1000];
	string csSQL;
	for(it=mList.begin(); it != mList.end(); it++)
	{
		CToken& t = (it->second);
		aDocFreqPercent = t.DocFreq()/(double)totalDocCount;
		aTermFreqPercent = t.Freq()/(double)totalTokenCount;
		fprintf(ofp,"%-50s   %-5ld   %-10lf   %-5ld   %-10lf\n", t.Key().c_str(), t.Freq(), aTermFreqPercent, t.DocFreq(), aDocFreqPercent);

#if 0   // Commented out till DB is supported.
		sprintf(sSQL, "exec sp_UpdateConcept %ld, \"%s\", %d, %ld, %ld, %d", t.DBID(), t.getToken(), 1, t.Freq(), t.DocFreq(),0); 
		csSQL = sSQL;
		if (!pDB->ExecuteSQL(csSQL))
		{
			printf("** Error executing: %s\n", sSQL);
		}
#endif
	}

}

// percent are 0.0 to 1.0
bool CFreqList::pruneDocFreq(double bottomPercent, double topPercent, long totalDocCount)
{
	//map<string, CToken>::iterator it;
	map<string, CToken>::iterator it;
	deque<string> lsToPrune;
	double aPercent;

	// find out which to prune
	for(it=mList.begin(); it != mList.end(); it++)
	{
		CToken& t = (it->second);
		aPercent = t.DocFreq()/(double)totalDocCount;
		if ( (aPercent < bottomPercent) || (aPercent > topPercent) )
		{
			lsToPrune.push_back(t.Token());
			if (SHOWPRUNE)
				printf("pruneDocFreq removing %s %lf\n", t.Token().c_str(), aPercent);
		}
	}

	printf("pruneDocFreq removing %ld of %ld = %ld\n", lsToPrune.size(), mList.size(), mList.size()-lsToPrune.size() );
		
	// Now do the actual pruning
	deque<string>::iterator pruneIt = lsToPrune.begin();
	while (pruneIt != lsToPrune.end())
	{
		mList.erase(*pruneIt);
		pruneIt++;
	}

	lsToPrune.clear();

	return true;
}

// prune term frequency within a file
bool CFreqList::pruneTermFreq(double bottomPercent, double topPercent)
{
	map<string, CToken>::iterator it;
	deque<string> lsToPrune;
	double aPercent;
		
	double totalTermCount = (double) mList.size();

	// find out which to prune
	for(it=mList.begin(); it != mList.end(); it++)
	{
		CToken& t = (it->second);
		aPercent = t.Freq()/(double)totalTermCount;
		if (isStopWord(t.Token()) || (aPercent < bottomPercent) || (aPercent > topPercent) )
		{
			lsToPrune.push_back(t.Token());
			if (SHOWPRUNE)
				printf("pruneTermFreq removing: %s %lf\n", t.Token().c_str(), aPercent);
		}
	}

	//printf("pruneTermFreq removing %d of %d = %d\n", lsToPrune.size(), mList.size(), mList.size()-lsToPrune.size() );

	// Now do the actual pruning
	deque<string>::iterator pruneIt = lsToPrune.begin();
	while (pruneIt != lsToPrune.end())
	{
		mList.erase(*pruneIt);
		pruneIt++;
	}

	lsToPrune.clear();

	return true;
}
