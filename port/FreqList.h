#ifndef _FREQLIST_H
#define _FREQLIST_H

class CDataStorage;

class CFreqList
{
	map<string, CToken> mList;

public:
	CFreqList() {}
	~CFreqList() {}

	void clear() { mList.clear(); }

	bool exists(const string& tokenKey);

	//used during retrieve from DB, does not assign new DBID
	void InitAdd(CToken& token);  

	// Increment token count by 1, add if not exist.
	// Will increment nextDBID if it is used.
	void LogToken(CToken& token, long& nextDBID);

	// Take input token count and add it into total count.
	void mergeToken(CToken& token, long& nextDBID);

	long size() { return (long) mList.size(); }
	map<string, CToken>& getList() { return mList; }

	void serialize(P267_FILE *ofp, CDataStorage *pDB, long totalTokenCount, long totalDocCount);

	// percent are 0.0 to 1.0
	bool pruneDocFreq(double bottomPercent, double topPercent, long totalDocCount);

	// prune term frequency within a file
	bool pruneTermFreq(double bottomPercent, double topPercent);
};

#endif
