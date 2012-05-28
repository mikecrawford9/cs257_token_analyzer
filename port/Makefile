prog = sutoken

CC = g++

filelist = Config.cpp  P267_Primitives.cpp  FreqList.cpp stdafx.cpp  DataStorage.cpp TokenizerUtils.cpp ThreadManager.cpp sutoken.c

$(prog):    $(filelist)
	$(CC) -g -o $(prog) $(filelist) -lpthread -locilib

clean:
	rm -f $(prog) *.o *\.tokens.txt *\.[1-7]tuple.txt core

run:    $(prog)
	rm -f *\.tokens.txt *\.[1-7]tuple.txt; $(prog) -lharrylist.txt -oout -t6

runwin: $(prog)
	rm -f *\.tokens.txt *\.[1-7]tuple.txt; ./$(prog).exe -lharrylist.txt -oout -t6
