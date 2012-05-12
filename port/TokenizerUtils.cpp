#include "stdafx.h"
#include "P267_Primitives.h"
#include "TokenizerUtils.h"
#include <map>

bool gLegalChar[256];
char gDelimiters[] = {P267_SPACE, P267_TAB, P267_CARRIAGERETURN, P267_NEWLINE};
string gUSStates = " AL AK AZ AR CA CO CT DE FL GA HI ID IL IN IA KS KY LA ME MD MA MI MN MS MO MT NE NV NH NJ NM NY NC ND OH OK OR PA RI SC SD TN TX UT VT VA WA WV WI WY ";
map<std::string, int>  mStopWords;

/////////////////////
// Define legal characters for forming tokens
// ASCII code is used as index to array, and the value indicates legal if true.
void initLegals()
{	
	memset(gLegalChar,false,256); //set all to false

	int i;

	// Numbers 0-9
	for (i=48; i<=57; i++)
		gLegalChar[i]=true;

	// A-Z
	for (i=65; i<=90; i++)
		gLegalChar[i]=true;

	// a-z
	for (i=97; i<=122; i++)
		gLegalChar[i]=true;

	gLegalChar['@']=true;
	gLegalChar['.']=true;
	gLegalChar[',']=true;
	gLegalChar['-']=true;
	gLegalChar['\'']=true;

}

void initStopWords()
{
	mStopWords.insert(map<string, int>::value_type("a",0));
	mStopWords.insert(map<string, int>::value_type("about",0));
	mStopWords.insert(map<string, int>::value_type("an",0));
	mStopWords.insert(map<string, int>::value_type("as",0));
	mStopWords.insert(map<string, int>::value_type("at",0));
	mStopWords.insert(map<string, int>::value_type("are",0));
	mStopWords.insert(map<string, int>::value_type("and",0));
	mStopWords.insert(map<string, int>::value_type("any",0));
	mStopWords.insert(map<string, int>::value_type("also",0));
	mStopWords.insert(map<string, int>::value_type("because",0));
	mStopWords.insert(map<string, int>::value_type("better",0));
	mStopWords.insert(map<string, int>::value_type("be",0));
	mStopWords.insert(map<string, int>::value_type("but",0));
	mStopWords.insert(map<string, int>::value_type("by",0));
	mStopWords.insert(map<string, int>::value_type("every",0));
	mStopWords.insert(map<string, int>::value_type("for",0));
	mStopWords.insert(map<string, int>::value_type("from",0));
	mStopWords.insert(map<string, int>::value_type("here",0));
	mStopWords.insert(map<string, int>::value_type("he",0));
	mStopWords.insert(map<string, int>::value_type("him",0));
	mStopWords.insert(map<string, int>::value_type("his",0));
	mStopWords.insert(map<string, int>::value_type("her",0));
	mStopWords.insert(map<string, int>::value_type("i",0));
	mStopWords.insert(map<string, int>::value_type("in",0));
	mStopWords.insert(map<string, int>::value_type("into",0));
	mStopWords.insert(map<string, int>::value_type("is",0));
	mStopWords.insert(map<string, int>::value_type("if",0));
	mStopWords.insert(map<string, int>::value_type("it",0));
	mStopWords.insert(map<string, int>::value_type("i'm",0));
	mStopWords.insert(map<string, int>::value_type("i've",0));
	mStopWords.insert(map<string, int>::value_type("i'll",0));
	mStopWords.insert(map<string, int>::value_type("it'll",0));
	mStopWords.insert(map<string, int>::value_type("it's",0));
	mStopWords.insert(map<string, int>::value_type("of",0));
	mStopWords.insert(map<string, int>::value_type("on",0));
	mStopWords.insert(map<string, int>::value_type("only",0));
	mStopWords.insert(map<string, int>::value_type("or",0));
	mStopWords.insert(map<string, int>::value_type("she",0));
	mStopWords.insert(map<string, int>::value_type("so",0));
	mStopWords.insert(map<string, int>::value_type("the",0));
	mStopWords.insert(map<string, int>::value_type("to",0));
	mStopWords.insert(map<string, int>::value_type("than",0));
	mStopWords.insert(map<string, int>::value_type("that",0));
	mStopWords.insert(map<string, int>::value_type("there",0));
	mStopWords.insert(map<string, int>::value_type("they",0));
	mStopWords.insert(map<string, int>::value_type("these",0));
	mStopWords.insert(map<string, int>::value_type("those",0));
	mStopWords.insert(map<string, int>::value_type("then",0));
	mStopWords.insert(map<string, int>::value_type("too",0));
	mStopWords.insert(map<string, int>::value_type("this",0));
	mStopWords.insert(map<string, int>::value_type("was",0));
	mStopWords.insert(map<string, int>::value_type("we",0));
	mStopWords.insert(map<string, int>::value_type("were",0));
	mStopWords.insert(map<string, int>::value_type("with",0));
	mStopWords.insert(map<string, int>::value_type("which",0));
	mStopWords.insert(map<string, int>::value_type("you",0));
	mStopWords.insert(map<string, int>::value_type("your",0));
	mStopWords.insert(map<string, int>::value_type("-",0));
}

bool isStopWord(const string& theWord)
{
	char sLower[52];
	int i;
	for (i=0; i < theWord.size() && i<51; i++)
	{
		sLower[i]=tolower(theWord.at(i));
	}
	sLower[i]=0;

	map<string, int>::iterator it = mStopWords.find(sLower);
	return (it != mStopWords.end());
}

void toLowerCase(string& theWord)
{
	for (int i=0; i < theWord.size() && i<51; i++)
	{
		theWord[i] = tolower(theWord.at(i));
	}
}

bool isLegalChar(char c) 
{ 
	return gLegalChar[c]; 
}

bool isDelimiter(char c) 
{
	for (int i=0; i < sizeof(gDelimiters); i++)
		if (c==gDelimiters[i]) 
			return true;

	return false;
}

bool isUSState(const char* s)
{
	string ss = " ";
	ss += s;
	ss += " ";
	long thePos =  gUSStates.find(ss);

	return (thePos >= 0);
}

bool isUSState(const string& s) { return isUSState(s.c_str()); }


bool isNumber(const char* s)
{
	int len = strlen(s);
	bool bResult=(len>0);

	while (len>0)
	{
		if (!isdigit(s[len-1]))
		{
			bResult=false;
			break;
		}
		len--;
	}

	return bResult;
}

bool isNumber(const string& s) { return isNumber(s.c_str()); }

bool isZipCode(const char* s)
{
	int len = strlen(s);
	bool bResult=(len>0);

	while (len>0)
	{
		if (!isdigit(s[len-1]) && s[len-1]!='-')
		{
			bResult=false;
			break;
		}
		len--;
	}

	return bResult;
}

bool isZipCode(const string& s) { return isZipCode(s.c_str()); }

// detect whether we have reached end of paragraph
// this will consume the newline and carriage return if it returns true
			// detect if a new paragraph is started
			// 1. return, return
			// 2. return, tab
			// 3. return, space, space
bool isEndParagraph(char lastc, P267_FILE *pfd)
{
	bool bResult=false;


	if (lastc!=P267_ENDFILE) 
	{
		char nextFewChars[3];
		memset(nextFewChars,0,3);

		for (int i=0; i<3; i++) {
			if ((nextFewChars[i]=P267_getcf(pfd))==P267_ENDFILE)
				break;
		}
		
		if (lastc==P267_CARRIAGERETURN) {
			// probably in windows system so check for 
			if (nextFewChars[0]==P267_NEWLINE) {
				if (nextFewChars[1]==P267_CARRIAGERETURN && nextFewChars[2]==P267_NEWLINE) {
					bResult=true;//detected 0xD, 0xA, 0xD, 0xA  sequence
				} else if (nextFewChars[1]=='\t') {
					bResult=true;//detected 0xD, 0xA, tab sequence
				} else if (nextFewChars[1]==' ' && nextFewChars[2]==' ') {
					bResult=true;//detected 0xD, 0xA, space, space
				}
			}
		} else if (lastc==P267_NEWLINE) {
			// probably unix system
			if (nextFewChars[0]==P267_NEWLINE) {
				bResult=true;//detected 0xA, 0xA  sequence
			} else if (nextFewChars[0]=='\t') {
				bResult=true;//detected 0xA, tab sequence
			} else if (nextFewChars[0]==' ' && nextFewChars[1]==' ') {
				bResult=true;//detected 0xA, space, space
			}
		}
	} 
	else 
	{
		bResult=true;
	}


	return bResult;
}

