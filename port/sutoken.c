#include <stdio.h>
#include <pthread.h>
#include "stdafx.h"
#include "P267_Primitives.h"
#include "Config.h"
#include "DataStorage.h"

static pthread_t threads[MAX_THREAD_COUNT];

static pthread_mutex_t findex_lock = PTHREAD_MUTEX_INITIALIZER;
static int findex = 0;

static pthread_mutex_t flist_lock = PTHREAD_MUTEX_INITIALIZER;

CConfig theConfig;

CThreadManager threadManager;

CDataStorage theDB;

static void oci_error_handler(OCI_Error *err)
{
    printf(
            "code   :   ORA-%05i\n"
            "msg    :   %s\n"
            "sql    :   %s\n",
            OCI_ErrorGetOCICode(err),
            OCI_ErrorGetString(err),
            OCI_GetSql(OCI_ErrorGetStatement(err))
          );
}

static void *analyze(void *arg) {
    int findex_l, i;
    const char *inputFile;

    while (1) {
        pthread_mutex_lock(&findex_lock);
        if (findex >= theConfig.InputFileList().size()) {
            pthread_mutex_unlock(&findex_lock);
            break;
        }
        findex_l = findex++;
        pthread_mutex_unlock(&findex_lock);

        pthread_mutex_lock(&flist_lock);
        inputFile = theConfig.InputFileList().at(findex_l).c_str();
        pthread_mutex_unlock(&flist_lock);

        for (i = 1; i <= theConfig.NTupleCount(); i++) {
            threadManager.parseFile(inputFile, i);
        }
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    CBenchmark stopWatch;
    int i;

    /*
     * Create threads each of which parses certain text files, analyzes
     * each n-tuple, and outputs data to each n-tuple analysis data file.
     */
    if(!theConfig.parseConfig()) {
        perror("config parse failed");
        exit(EXIT_FAILURE);
    }

    OCI_Initialize(oci_error_handler, NULL, OCI_ENV_DEFAULT);

    if (!theDB.OpenConnection(theConfig.SQLDSN())) {
        printf("DB open connectionf failed\n");
    }

    theConfig.parseCommandLine(argc, argv);

    initLegals();
    initStopWords();

    threadManager.SetInputFileList(&theConfig.InputFileList());
    threadManager.openMasterTokensFile(theConfig.OutfilePrefix());

    for (i = 0; i < theConfig.ThreadCount(); i++) {
        pthread_create(&threads[i], NULL, &analyze, NULL);
    }

    /* wait for all threads to be done with their analyses */
    for (i = 0; i < theConfig.ThreadCount(); i++) {
        pthread_join(threads[i], NULL);
    }

    threadManager.closeMasterTokensFile();

    threadManager.setDataStorage(&theDB);

    /* serialize all n-pair files */
    for (i = 1; i <= theConfig.NTupleCount(); i++) {
        threadManager.serialize(i, theConfig.OutfilePrefix());
    }

    long totalCount = threadManager.MasterTokenCount();
    printf("\nProgram executed in %f seconds. Found %ld tokens. "
            "Speed = %lf tokens/sec\n",
            stopWatch.ElapsedSecs(), totalCount,
            totalCount / (double)stopWatch.ElapsedSecs());

    OCI_Cleanup();

    return 0;
}
