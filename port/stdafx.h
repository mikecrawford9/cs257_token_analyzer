// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#ifndef _STDAFX_H
#define _STDAFX_H

#pragma once

using namespace std;

//#include "targetver.h"

#include <stdio.h>
//#include <tchar.h>

//#include <windows.h>
#include <iostream>

//STL library
#include <iostream>
#include <vector>
#include <deque>
#include <string>
#include <map>
#include <queue>

#include "P267_Primitives.h"
#include "Token.h"
#include "ThreadManager.h"
#include "FreqList.h"


extern long WINDOW_SIZE; // max distance to be considered for forming primitive concept
extern long MAX_TUPLE_COUNT;
#define MAX_THREAD_COUNT    10

extern long SLIDER_SIZE; // number of tokens to increment in each processing iteration, should be less than WINDOW_SIZE. 
	//This is also the count written to file at a time.

extern long LOOKAHEAD_SIZE; // number of tokens we keep in memory for processing.  We need twice as much because
	//some tokens may be merged into one, such as US addresses.

extern long MAXPARAGRAPH_SIZE; //50; // max number of words in a paragraph.
extern long MINPARAGRAPH_SIZE; // min number of words in a paragraph before we process it.

extern double TERMFREQ_FROM;
extern double TERMFREQ_TO;

extern double DOCFREQ_FROM;
extern double DOCFREQ_TO;

extern bool SHOWPRUNE;

#endif
