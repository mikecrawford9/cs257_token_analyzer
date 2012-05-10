#include <stdio.h>
#include <pthread.h>
#include "stdafx.h"
#include "P267_Primitives.h"
#include "Config.h"

static pthread_t threads[MAX_THREAD_COUNT];

static pthread_mutex_t findex_lock = PTHREAD_MUTEX_INITIALIZER;
static int findex = 0;

static pthread_mutex_t flist_lock = PTHREAD_MUTEX_INITIALIZER;

CConfig theConfig;

static void *analyze(void *arg) {
    int findex_l, i;

//    printf("%d", theConfig.InputFileList().size());
    pthread_mutex_lock(&findex_lock);
    if (findex >= theConfig.InputFileList().size()) {
        pthread_mutex_unlock(&findex_lock);
        return NULL;
    }
    findex_l = findex++;
    pthread_mutex_unlock(&findex_lock);

    pthread_mutex_lock(&flist_lock);
    printf("analyzing... thread_id = %lu, file = %s\n",
            pthread_self(), theConfig.InputFileList().at(findex_l).c_str());
    pthread_mutex_unlock(&flist_lock);

    for (i = 1; i <= theConfig.NTupleCount(); i++) {
    }

    
    return NULL;
}

int main(int argc, char *argv[])
{
    int i;

    /*
     * Create threads each of which parses certain text files, analyzes
     * each n-tuple, and outputs data to each n-tuple analysis data file.
     */
    if(!theConfig.parseConfig()) {
        perror("config parse failed");
        exit(EXIT_FAILURE);
    }

    theConfig.parseCommandLine(argc, argv);

    for (i = 0; i < MAX_THREAD_COUNT; i++) {
        pthread_create(&threads[i], NULL, &analyze, NULL);
    }

    /* wait for all threads to be done with their analyses */
    for (i = 0; i < MAX_THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    /* serialize all n-pair files */
    /* serialize(); */

    return 0;
}
