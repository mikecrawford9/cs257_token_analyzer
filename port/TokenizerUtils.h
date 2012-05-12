#ifndef _TOKENIZERUTILS
#define _TOKENIZERUTILS

#include <string>
#include <time.h>
using namespace std;

void initLegals();
void initStopWords();
bool isStopWord(const string& theWord);
void toLowerCase(string& theWord);

bool isLegalChar(char c);
bool isDelimiter(char c);

bool isUSState(const char* s);
bool isUSState(const string& s);

bool isNumber(const char* s);
bool isNumber(const string& s);

bool isZipCode(const char* s);
bool isZipCode(const string& s);

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))


class CBenchmark
{
	clock_t mStart;
public:
	CBenchmark() { mStart = clock(); }
	clock_t Elapsed() { return (clock() - mStart); }
	double ElapsedSecs() { return (1.0 * Elapsed())/CLOCKS_PER_SEC; }
};

#endif

