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
int SERVER_ID=0;
int REQUEST_TAG=5555;
int RESULT_TAG=5556;

void provideWorkToNode(
    std::vector<T>& vRequest,
    int64_t& requestCounter,
    std::mutex& requestMutex,
    std::vector<T>& vResult,
    int64_t& resultCounter,
    std::mutex& resultMutex,
    int world_rank,
    int nodeId) {

  size_t loopIdx=0;
  while (true) {
    if (world_rank == SERVER_ID) {
      int lRequestCounter;
      {
        // Handling input safely
        std::lock_guard<std::mutex> guard(requestMutex);
        if (requestCounter<vRequest.size()) {
          lRequestCounter=requestCounter;
          requestCounter++;
        } else {
          return;
        }
      }
      //TODO request counter is not very useful
      vRequest.at(lRequestCounter)=lRequestCounter;
      //TODO : do the actual work
     
      int lResultCounter;
      {
        // Handling output safely
        std::lock_guard<std::mutex> guard(resultMutex);
        if (resultCounter<vResult.size()) {
          lResultCounter=resultCounter;
          resultCounter++;
        } else {
          //TODO: throw error properly
          //std::cout<<"This error should not arise !"<<std::endl;
          return;  
        }
      }
      //TODO : write the actual result
      vResult.at(lResultCounter)=lResultCounter;
    } else {
      // Now one can wait for incomming results
      if (loopIdx>0) {
        int lResultCounter;
        {
          // Handling output safely
          std::lock_guard<std::mutex> guard(resultMutex);
          if (resultCounter<vResult.size()) {
            lResultCounter=resultCounter;
            resultCounter++;
          } else {
            //TODO: throw error properly
            //std::cout<<"This error should not arise !"<<std::endl;
            return;  
          }
        }
        int result;
        MPI_Recv(&result, 1, MPI_INT, nodeId, RESULT_TAG,
          MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout<<"Thread: mpi result has been received"<<std::endl;
        //TODO : write the actual result
        vResult.at(lResultCounter)=lResultCounter;
      }

      int status;
      int lRequestCounter;
      {
        // Handling input safely
        std::lock_guard<std::mutex> guard(requestMutex);
        if (requestCounter<vRequest.size()) {
          status=1;
          //std::cout<<"Thread: mpi request has been sent"<<std::endl;
          lRequestCounter=requestCounter;
          requestCounter++;
        } else {
          status=0;
          std::cout<<"Thread: void mpi request will be sent"<<std::endl;
        }
      }

      //TODO cRequest is not very useful, can be deleted
      vRequest.at(lRequestCounter)=lRequestCounter;
      int request[2]={0,status};
      MPI_Send(&request, 2, MPI_INT, nodeId, REQUEST_TAG,
        MPI_COMM_WORLD);
      if (status==0) {
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

  if (world_rank == SERVER_ID) {
    //Launch a group of threads: 1 per node, except the server node
    for (int i=0; i < world_size; ++i) {
      lThreads.emplace_back(
        provideWorkToNode,
        std::ref(vRequest),
        std::ref(requestCounter),
        std::ref(requestMutex),
        std::ref(vResult),
        std::ref(resultCounter),
        std::ref(resultMutex),
        world_rank,
        i);
    }
  } else {
    bool isNext = true;
    while (isNext) {
      //TODO: blocking wait for a request (of unknown size) from rank 0
      int request[2];
      std::cout<<"Node: mpi request: waiting to  receive"<<std::endl;
      MPI_Recv(&request, 2, MPI_INT, SERVER_ID, REQUEST_TAG,
        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      std::cout<<"Node: mpi request has been received"<<std::endl;
    

      T status = request[1];
      if (status!=0) { 
        //TODO: compute result
        int result = 0;
        //TODO: sending result to rank 0
        std::cout<<"Node: mpi result waiting to be send"<<std::endl;
        MPI_Send(&result, 1, MPI_INT, SERVER_ID, RESULT_TAG,
          MPI_COMM_WORLD);
        std::cout<<"Node: mpi result has been sent"<<std::endl;
      } else {
        isNext=false;
      }
    }
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
