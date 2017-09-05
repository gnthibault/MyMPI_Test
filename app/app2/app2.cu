//STL
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <iostream>
#include <list>
#include <mutex>
#include <thread>
#include <vector>

//MPI
#include <mpi.h>

//Local
//#include <Cuda/lib.cu.h>

using T = int;
int nbRequest = 10;
int nbNodes = 4;

void provideWorkToNode(
    std::vector<T>& vRequest,
    int64_t& requestCounter,
    std::mutex& requestMutex,
    std::vector<T>& vResult,
    int64_t& resultCounter,
    std::mutex& resultMutex) {
  // Handling input safely
  {
    std::lock_guard<std::mutex> guard(requestMutex);
    vRequest.at(requestCounter)=requestCounter;
    requestCounter++;
  }

  // Handling output safely
  {
    std::lock_guard<std::mutex> guard(resultMutex);
    vResult.at(resultCounter)=resultCounter;
    resultCounter++;
  }
};


int main(int argc, char* argv[]) {
  
  //Threads
  std::list<std::thread> lThreads;

  //Input data
  std::vector<T> vRequest(nbRequest,0);
  int64_t requestCounter=0;
  std::mutex requestMutex;

  //Output dat TODO TN: think about the fact it will be variable lenght
  std::vector<T> vResult(nbRequest,0);
  int64_t resultCounter=0;
  std::mutex resultMutex;

  //Launch a group of threads
  for (int i = 0; i < nbNodes; ++i) {
    lThreads.emplace_back(
      provideWorkToNode,
      std::ref(vRequest),
      std::ref(requestCounter),
      std::ref(requestMutex),
      std::ref(vResult),
      std::ref(resultCounter),
      std::ref(resultMutex));
  }

  //Join the threads with the main thread
  for(auto& thread : lThreads) {
    thread.join();
  }

  auto print = [](T in) {
    std::cout<<in<<std::endl; };
  std::cout<<"Content of request: "<<std::endl;
  std::for_each(vRequest.cbegin(), vRequest.cend(), print);  
  std::cout<<"Content of result: "<<std::endl;
  std::for_each(vResult.cbegin(), vResult.cend(), print);  

  // Init mpi and monitor runtime
  /*MPI_Init(NULL, NULL);

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  int number_amount;
  if (world_rank == 0) {
    const int MAX_NUMBERS = 100;
    int numbers[MAX_NUMBERS];
    // Pick a random amont of integers to send to process one
    srand(time(NULL));
    number_amount = (rand() / (float)RAND_MAX) * MAX_NUMBERS;
    // Send the amount of integers to process one
    MPI_Send(numbers, number_amount, MPI_INT, 1, 0, MPI_COMM_WORLD);
    printf("0 sent %d numbers to 1\n", number_amount);
  } else if (world_rank == 1) {
    MPI_Status status;
    // Probe for an incoming message from process zero
    MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
    // When probe returns, the status object has the size and other
    // attributes of the incoming message. Get the size of the message.
    MPI_Get_count(&status, MPI_INT, &number_amount);
    // Allocate a buffer just big enough to hold the incoming numbers
    int* number_buf = (int*)malloc(sizeof(int) * number_amount);
    // Now receive the message with the allocated buffer
    MPI_Recv(number_buf, number_amount, MPI_INT, 0, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    printf("1 dynamically received %d numbers from 0.\n",
           number_amount);
    free(number_buf);
  }
  MPI_Finalize();*/
  
  return EXIT_SUCCESS;
}
