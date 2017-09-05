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

void provideWorkToNode(
    std::vector<T>& vRequest,
    int64_t& requestCounter,
    std::mutex& requestMutex,
    std::vector<T>& vResult,
    int64_t& resultCounter,
    std::mutex& resultMutex,
    int nodeId) {

  size_t loopIdx=0;
  while (true) {

    // Now one can wait for incomming results
    if (loopIdx>0) {
      // Handling output safely
      std::lock_guard<std::mutex> guard(resultMutex);
      if (requestCounter<vRequest.size()) {
        //TODO: add blocking recv call from rank nodeId
        vResult.at(resultCounter)=resultCounter;
        resultCounter++;
      } else {
        //TODO: throw error properly
        std::cout<<"This error should not arise !"<<std::endl;
        return;  
      }
    }

    // Handling input safely
    {
      std::lock_guard<std::mutex> guard(requestMutex);
      if (requestCounter<vRequest.size()) {
        //TODO: add blocking send call to rank nodeId
        vRequest.at(requestCounter)=requestCounter;
        requestCounter++;
      } else {
        //TODO: add blocking void send call to rank nodeId
        return;
      }
    }
    loopIdx++;
  }
};


int main(int argc, char* argv[]) {
  
  // Init mpi and monitor runtime
  MPI_Init(NULL, NULL);
  int world_rank, world_size;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  //Threads: 1 per computing node
  std::list<std::thread> lThreads;
  //Input data
  std::vector<T> vRequest(nbRequest,0);
  int64_t requestCounter=0;
  std::mutex requestMutex;
  //Output dat TODO TN: think about the fact it will be variable lenght
  std::vector<T> vResult(nbRequest,0);
  int64_t resultCounter=0;
  std::mutex resultMutex;

  if (world_rank == 0) {
    //Launch a group of threads: 1 per node
    for (int i=0; i < world_size; ++i) {
      lThreads.emplace_back(
        provideWorkToNode,
        std::ref(vRequest),
        std::ref(requestCounter),
        std::ref(requestMutex),
        std::ref(vResult),
        std::ref(resultCounter),
        std::ref(resultMutex),
        i);
    }
  }

  bool isNext = true;
  while (isNext) {
    //TODO: blocking wait for a request (of unknown size) from rank 0
   
    //TODO: sending result to rank 0
  }

  /*const int MAX_NUMBERS = 100;
  int numbers[MAX_NUMBERS];
  // Pick a random amont of integers to send to process one
  srand(time(NULL));
  number_amount = (rand() / (float)RAND_MAX) * MAX_NUMBERS;
  // Send the amount of integers to process one
  MPI_Send(numbers, number_amount, MPI_INT, 1, 0, MPI_COMM_WORLD);
  printf("0 sent %d numbers to 1\n", number_amount);*/
  /*MPI_Status status;

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
  free(number_buf);*/

  if (world_rank==0) {
    //Join the threads with the main thread
    for(auto& thread : lThreads) {
      thread.join();
    }
  }

  // Synchronization
  MPI_Barrier(MPI_COMM_WORLD);
  if (world_rank==0) {
    auto print = [](T in) {
      std::cout<<in<<std::endl; };
    std::cout<<"Content of request: "<<std::endl;
    std::for_each(vRequest.cbegin(), vRequest.cend(), print);  
    std::cout<<"Content of result: "<<std::endl;
    std::for_each(vResult.cbegin(), vResult.cend(), print);  
  }

  MPI_Finalize();
  
  return EXIT_SUCCESS;
}
