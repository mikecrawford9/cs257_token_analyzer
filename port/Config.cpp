#include "stdafx.h"
#include "Config.h"
#include "P267_Primitives.h"
#include "TokenizerUtils.h"

CConfig::CConfig(void) 
{
	mSQLDSN="";
	mOutfilePrefix="";
	mNTupleCount =MAX_TUPLE_COUNT;
	mThreadCount=2;
}


CConfig::~CConfig(void)
{
}


bool CConfig::parseCommandLine(int argc, char** argv)
{
	bool brc=false;
	char* exeName = argv[0];
	if ((argc > 1) && (argv[1][0]!='?')) 
	{
		while (argc>1)
		{
			++argv;
			--argc;

			if (strncmp(*argv,"-tl",3)==0)
			{
				//specify number of threads, no space is allowed between the -t and the number.
				TERMFREQ_FROM = atof((const char*)&(*argv)[3]);
			} 
			else if (strncmp(*argv,"-th",3)==0)
			{
				//specify number of threads, no space is allowed between the -t and the number.
				TERMFREQ_TO = atof((const char*)&(*argv)[3]);
			} 
			else if (strncmp(*argv,"-sp",3)==0)
			{
				//display message of what is pruned
				SHOWPRUNE = true;
			} 
			else if (strncmp(*argv,"-dl",3)==0)
			{
				//specify number of threads, no space is allowed between the -t and the number.
				DOCFREQ_FROM = atof((const char*)&(*argv)[3]);
			} 
			else if (strncmp(*argv,"-dh",3)==0)
			{
				//specify number of threads, no space is allowed between the -t and the number.
				DOCFREQ_TO = atof((const char*)&(*argv)[3]);
			} 
			else if (strncmp(*argv,"-ps",3)==0)
			{
				//specify number of threads, no space is allowed between the -t and the number.
				MAXPARAGRAPH_SIZE = atol((const char*)&(*argv)[3]);
			} 
			else if (strncmp(*argv,"-o",2)==0)
			{
				//specify out file, currently this -o argument must be the first one, if used.
				//no space is allowed between the -o and the filename.
				mOutfilePrefix=&(*argv)[2];
				brc = true;
			}
			else if (strncmp(*argv,"-n",2)==0)
			{
				//specify number of levels, currently this -d argument must be the second one, if used.
				//no space is allowed between the -d and the number.
				mNTupleCount = MIN(MAX_TUPLE_COUNT, atol((const char*)&(*argv)[2]) );
			} 
			else if (strncmp(*argv,"-t",2)==0)
			{
				//specify number of threads, no space is allowed between the -t and the number.
				mThreadCount = MIN(MAX_THREAD_COUNT, atol((const char*)&(*argv)[2]) );
			} 
			else
			{
				// a file or a filelist
				char sfilename[512];
				P267_FILE *fpFilelist=NULL;
				char *s=*argv;

				if (strncmp(s,"-l",2)==0)
				{
					// this is a filelist, use it to get filenames to parse.
					s=&s[2];
					if ((fpFilelist = P267_open(s, P267_IOREAD))!=NULL)
					{
						char c;
						int idx=0;
						memset(sfilename,0,512);

						while((c = P267_getcf(fpFilelist)) != EOF) 
						{
							if ((c!=P267_NEWLINE) && (c!=P267_CARRIAGERETURN) && (c!=P267_ENDFILE))
							{
								sfilename[idx++]=c;
							} 
							else 
							{
								//Now we have a complete filename from one
								string trimmed = sfilename;
								trimmed.erase(trimmed.find_last_not_of(" \n\r\t")+1); 
								if (!trimmed.empty())
								{
									mInputFilelist.push_back(trimmed);
								}
								idx=0;
								memset(sfilename,0,512);
							}
						}

						P267_close(fpFilelist);
					}
				}
				else
				{
					mInputFilelist.push_back(s);
				}
			}
		}
	}

	if (brc && mInputFilelist.size()>0 && mThreadCount>0)
	{
		//printf what user has provided
		printf("\n**** Super Tokenizer ****\n");
		printf("Prefix output files with '%s'\n", mOutfilePrefix.c_str());
		printf("Analyze up to %ld tuples\n", mNTupleCount);
		printf("Spawn %ld threads\n", mThreadCount);
		printf("Keep Term Frequency: %.2f%% to %.2f%%\n", TERMFREQ_FROM*100, TERMFREQ_TO*100);
		printf("Keep Doc Frequency: %.2f%% to %.2f%%\n", DOCFREQ_FROM*100, DOCFREQ_TO*100);
		printf("Window size: %ld\n", WINDOW_SIZE);
		printf("Paragraph size: %ld to %ld\n", MINPARAGRAPH_SIZE, MAXPARAGRAPH_SIZE);
		printf("Number of files: %ld\n\n", mInputFilelist.size());
	}
	else 
	{
		brc = false;
		printf("Usage: %s -oOutFile [-nTupleCount] [-tThreadCount] [-tlTermFreqFrom] [-thTermFreqTo] [-dlDocFreqFrom] [-dhDocFreqTo] [-lFileList] [InputFiles...]\n",exeName);
	}

	return brc;
}


bool CConfig::parseConfig(void)
{
	bool brc=true;
	char sTag[1000], sValue[1000], aLine[1000];
	FILE *fp=fopen("sutoken.config", "r");
	if (fp)
	{
		fgets(aLine, sizeof(aLine), fp);
		sscanf(aLine, "%[^|]|%[^|]",sTag, sValue);

		if (strcmp(sTag,"SQLDSN")==0)
		{
			SQLDSN(sValue);
		}

		fclose(fp);
	} 
	else
	{
		printf("Error: unable to open sutoken.config file.\n\n");
                brc = false;
	}


	return brc;
}
