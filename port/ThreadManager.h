using namespace std;

#include "FreqList.h"

class CFreqList;
class CToken;
//class CDataStorage;

//enum TaskStatus { Started, InProgress, Completed, Canceled, Error };
//static char PROGRESS[3] = { '/', '-', '\\' };

//Base class for tasks
class CThreadManager
{
	//database
	CDataStorage *mDB;
	long mNextConcept_DBID;

        pthread_mutex_t tuple_lists_lock;// = PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_t master_list_lock;// = PTHREAD_MUTEX_INITIALIZER;


//	int mThreadCount;

//	HANDLE *mThreadHandles;

//	CRITICAL_SECTION mCSGuardMasterList;  // guards the following resources
	//deque<CToken> mMasterTokenList; // has position and token can appear more than once.
	
	//CRITICAL_SECTION mCSGuardTupleLists;  // guards the following resources
	CFreqList mTupleLists[7]; // 0 index is not used.  1 is for 1-tuple, 2 is for 2-tuple, and so on.

#if 0
	HANDLE mTaskAvailEvent;
	HANDLE mEndEvent;
	HANDLE mTaskCompletedEvent;
#endif

//	CRITICAL_SECTION mCSFileListIndex; // guards the following resources
	std::deque<string>* mInputFileList;
	long mCurrFileListIndex;
	long mCurrTargetTupleCount;
	long mCurrFilesProcessed;

	P267_FILE *mfpMasterTokens;
	long mMasterTokenCount;


//	static DWORD WINAPI ThreadLaunch( LPVOID lpParam );
protected:
//	DWORD ThreadProc();
	bool mergeUSAddress(deque<CToken>& lsTokens, int& wc);
	bool permuteParagraph(long targetTupleCount, const string& docName, deque<CToken>& lsTokens, CFreqList& freqTuples);
	bool permuteParagraph_r(long nDepth, long targetTupleCount, long nToken1Pos, const char* sPrefix, long nStartIdx, long nEndIdx, const string& docName, deque<CToken>& lsTokens, CFreqList& freqTuples);
	bool mergeTupleList(long targetTupleCount, CFreqList& freqTuples);
	bool mergeTokenList(deque<CToken>& lsTokens, int mCurrTargetTupleCount);

public:

	CThreadManager();
	~CThreadManager();

	void InitThreads(int threadcount);
	void EndThreads();

#if 0
        void setCurrentTuple(long n);
        int  getCurrentTuple();
#endif
//	bool parseNtuple(long n, const string& sFilePrefix);
	bool pruneNtuple(long n);
	void parseFile(const string& docName, int mCurrTargetTupleCount);
	void SetInputFileList(std::deque<string>* fl);
	void toFiles(const string& outfilePrefix);
	void serialize(long nTupleIndex, const string& outfilePrefix);
	void outputToFile(P267_FILE *ofp, char* filename, deque<CToken>& lsTokens);

	void openMasterTokensFile(const string& outfilePrefix);
	void closeMasterTokensFile();

	long MasterTokenCount() { return mMasterTokenCount; }
//	void InitFromDB(CDataStorage *pdb);
};
