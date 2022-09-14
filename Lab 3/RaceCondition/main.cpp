#include <thread>
#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>

// Include file and line numbers for memory leak detection for visual studio in debug mode
#if defined _MSC_VER && defined _DEBUG
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define ENABLE_LEAK_DETECTION() _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)
#else
#define ENABLE_LEAK_DETECTION()
#endif

std::mutex mutex1;
int* status;
std::condition_variable con;
int* stringsStatus;
int count;

struct ThreadStruct
{
    int id; // // ID of the thread
    int sharedStringLength; // Length of the shared string
    int numberOfStringsToGenerate;  // Number of strings a single thread will generate
    int runType; // run type

    char* sharedString;
    // Shared string that will be generate in each thread. This memory is shared among all threads
};

// Prompts the user to press enter and waits for user input
void Pause()
{
    printf("Press enter to continue\n");
    getchar();
}

// Entry point for worker threads. 
// Arguments:
// threadData - Pointer to per-thread data for this thread.

void ThreadEntryPoint(ThreadStruct* threadData)
{
    bool waiting;
    /*if (threadData->runType == 1) {
       mutex1.lock();
    }
    if (threadData->runType == 2) {
        mutex1.lock();
        
    }
    if (threadData->runType == 3) {
        waiting = true;
        while (waiting)
        {
            waiting = false;
            for (int i = 0; i < threadData->id; i++)
                if (status[i] == 0)
                    waiting = true;
        }
        mutex1.lock();

    }
    */
    for (int i = 0; i < threadData->numberOfStringsToGenerate; i++) //std::this_thread::sleep_for(std::chrono::milliseconds(10)))
    {
        if (threadData->runType == 1) {
            mutex1.lock();
        }
        if (threadData->runType == 2) {
            mutex1.lock();
            std::this_thread::sleep_for(std::chrono::milliseconds(0));
        }
        if (threadData->runType == 3) {
            waiting = true;
            while (waiting)
            {
                waiting = false;
                for (int i = 0; i < threadData->id; i++)
                    if (status[i] == 0)
                        waiting = true;
            }
            mutex1.lock();
        }
    
        for (int j = 0; j < threadData->sharedStringLength; j++)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(1));
            threadData->sharedString[j] = 'A' + threadData->id;
        }
       
        printf("Thread %d: %s\n", threadData->id, threadData->sharedString);
        if (threadData->runType == 1) {
            mutex1.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            
        }
        if (threadData->runType == 2) {
            mutex1.unlock();
            

        }
        if (threadData->runType == 3) {
            mutex1.unlock();
            
            status[threadData->id] = 1;
        }
       
    }
    
    //if (threadData->runType == 1) {
    //    mutex1.unlock();
    //}
    /*if (threadData->runType == 2) {
        mutex1.unlock();

    }
    if (threadData->runType == 3) {
        mutex1.unlock();
        status[threadData->id] = 1;
    }*/

}

int main(int argc, char** argv)
{
    ENABLE_LEAK_DETECTION();

    int threadCount = 0;
    int sharedStringLength = 0;
    int numberOfStringsToGenerate = 0;
    int runType = 0;
    char* sharedString = NULL;
    ThreadStruct* perThreadData = NULL;

    if (argc != 5)
    {
        printf("argc is %d", argc);
        fprintf(stderr, "Error: missing or incorrect command line arguments\n\n");
        fprintf(stderr, "Usage: RaceCondition threadCount sharedStringLength numberOfStringsToGenerate runType\n\n");
        fprintf(stderr, "Arguments:\n");
        fprintf(stderr, "    threadCount                  Number of threads to create.\n");
        fprintf(stderr, "    sharedStringLength           Length of string to generate.\n");
        fprintf(stderr, "    numberOfStringsToGenerate    Number of strings to generate per thread.\n");
        fprintf(stderr, "    runType                      The run type.\n\n");
        Pause();
        return 1;
    }

    threadCount = atoi(argv[1]);
    sharedStringLength = atoi(argv[2]);
    numberOfStringsToGenerate = atoi(argv[3]);
    runType = atoi(argv[4]);

    if (threadCount < 0 || sharedStringLength < 0 || numberOfStringsToGenerate < 0 || runType < 0)
    {
        fprintf(stderr, "Error: All arguments must be positive integer values.\n");
        Pause();
        return 1;
    }

    printf("%d thread(s), string sharedStringLength %d, %d iterations, %d runType\n",
        threadCount, sharedStringLength, numberOfStringsToGenerate, runType);

    sharedString = new char[sharedStringLength + 1];
    memset(sharedString, 0, sharedStringLength + 1);
    perThreadData = new ThreadStruct[threadCount];

    // Create a container to store the thread class objects
    std::vector<std::thread> threads;

    // Create a container to store the thread execution status
    status = new int[threadCount];

    // Mark that no thread has been finished yet.
    for (int i = 0; i < threadCount; ++i)
        status[i] = 0;

    count = threadCount;
    stringsStatus = new int[numberOfStringsToGenerate];

    for (int i = 0; i < numberOfStringsToGenerate; i++)
        stringsStatus[i] = 0;

    // NOTE: Do NOT change this for loop header
    for (int i = threadCount - 1; i >= 0; i--)
    {
        perThreadData[i].id = i;
        perThreadData[i].sharedStringLength = sharedStringLength;
        perThreadData[i].numberOfStringsToGenerate = numberOfStringsToGenerate;
        perThreadData[i].sharedString = sharedString;
        perThreadData[i].runType = runType;
        threads.push_back(std::thread(ThreadEntryPoint, &perThreadData[i]));
    }

    // Join the threads
    for (auto& thread : threads) {
        thread.join();
    }

    // Clean up the memory 
    delete[] status;
    delete[] sharedString;
    delete[] perThreadData;

    Pause();

    return 0;
}