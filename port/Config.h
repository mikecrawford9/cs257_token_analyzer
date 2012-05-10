#include <string>
#include <deque>
using namespace std;
#include <cstdlib>

class CConfig
{
	string mSQLDSN;
	string mOutfilePrefix;
	long mNTupleCount;
	deque<string> mInputFilelist;
	long mThreadCount;
public:
	CConfig(void);
	~CConfig(void);
	bool parseCommandLine(int argc, char** argv);

	long ThreadCount() { return mThreadCount; }
	long NTupleCount() { return mNTupleCount; }

	deque<string>& InputFileList() { return mInputFilelist; }

	const string& OutfilePrefix() { return mOutfilePrefix; }
	bool parseConfig(void);

	const string& SQLDSN() { return mSQLDSN; }
	void SQLDSN(const string& dsn) { mSQLDSN=dsn; }
};

