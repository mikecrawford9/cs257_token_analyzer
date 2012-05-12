#ifndef _TOKEN_H
#define _TOKEN_H

#include "stdafx.h"
#include "TokenizerUtils.h"

extern bool SHOWPRUNE;

class CToken
{
	long mDBID;

	string mToken;
	long mPos;
	long mFreq; // count of occurrences
	long mDocFreq; //count of documents it exists in
	int mPerm; // 1 indicates permanent, 0 temporary/discovered

	string mLastDocFound; //doc name where it was found last
public:
	void set(const char* t, long pos) 
	{ 
		mToken=t; 
		toLowerCase(mToken); 
		mToken.erase(mToken.find_last_not_of(" \n\r\t'?:;.,")+1); 
		mPos=pos; 
	}

	const char* getToken() { return mToken.c_str(); }
	long getPos() { return mPos; }
	const string& Token() { return mToken; }
	string Key() { return mToken; }
	const long Pos() { return mPos; }
	const long Freq() { return mFreq; }
	const long DocFreq() { return mDocFreq; }

	void Freq(long freq) { mFreq = freq; }
	void DocFreq(long freq) { mDocFreq = freq; }
	
	long DBID() 
	{ 
		return mDBID; 
	}
	void DBID(long dbid) 
	{ 
		mDBID=dbid; 
	}

	int Perm() { return mPerm; }
	void Perm(int p) { mPerm=p; }

	void LastDocFound(const string &filename) { mLastDocFound=filename; }
	const string& LastDocFound() { return mLastDocFound; }

	// copy constructor
	CToken() 
	{
		mFreq = mDocFreq = mPos = mDBID = 0;
	}


	CToken(const CToken& c) { 
		mToken = c.mToken; 
		mPos=c.mPos;
		mFreq = c.mFreq;
		mDocFreq = c.mDocFreq;
		mLastDocFound = c.mLastDocFound;
		mDBID = c.mDBID;
	}

	~CToken() 
	{
		mFreq = mDocFreq = mPos = mDBID = 0;
	}

	void clear() 
	{ 
		mFreq = mDocFreq = 0; 
		mToken=""; 
		mPos=-1; 
		mDBID = 0;
	}

	void IncrFreq(const string& docName) 
	{ 
		mFreq++; 
		if (mLastDocFound!=docName) {
			//increment doc frequency only once per document for each token
			mLastDocFound=docName;
			mDocFreq++;
		}
	}

	void IncrFreq()
	{
		mFreq++;
		mDocFreq++;
	}
};

#endif
