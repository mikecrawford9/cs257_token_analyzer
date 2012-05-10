// stdafx.cpp : source file that includes just the standard includes
// sutoken.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

long WINDOW_SIZE = 30; // max distance to be considered for forming primitive concept
long MAX_TUPLE_COUNT = 6;

long SLIDER_SIZE = WINDOW_SIZE/2; // number of tokens to increment in each processing iteration, should be less than WINDOW_SIZE. 
	//This is also the count written to file at a time.

long LOOKAHEAD_SIZE = WINDOW_SIZE * 2; // number of tokens we keep in memory for processing.  We need twice as much because
	//some tokens may be merged into one, such as US addresses.

long MAXPARAGRAPH_SIZE = 250; //50; // max number of words in a paragraph.
long MINPARAGRAPH_SIZE = 30; // min number of words in a paragraph before we process it.

double TERMFREQ_FROM = 0.001;
double TERMFREQ_TO = 0.3;

double DOCFREQ_FROM = 0.1;
double DOCFREQ_TO = 1.0;

bool SHOWPRUNE = false;
