// vim: ts=2
// vim: sw=2
// vim: et

#include "app1.h"

//STL
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <iostream>
#include <list>
#include <mutex>
#include <thread>
#include <vector>
#include <map>


//Local
//#include <Cuda/lib.cu.h>

using RequestType=std::vector<int>;
using ResultType=std::vector<int>;

int main(int argc, char* argv[]) {
  
  // Init mpi and monitor runtime
  MPI_Init(NULL, NULL);
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  if(world_rank==CLIENT_ID){
    //Export start and end links to Hashmap

    //input data
    DataHandler<RequestType> workQueue;
    std::vector<std::vector<int>> sortedRoutes(8,std::vector<int>(2,1));

    std::for_each(sortedRoutes.cbegin(),sortedRoutes.cend(),
                [&](const auto& request) {
                  workQueue.push_back(RequestType(
                     request.cbegin(),request.cend()));}
    );

    //Schedule Request to Servers
    MPIWorkProvider<RequestType> provider(workQueue);
    std::cout<<"Test 1"<<std::endl;
    provider.provide();

    //Synchronization
    MPI_Barrier(MPI_COMM_WORLD);


/*   auto print = [](T in) {
      std::cout<<in<<std::endl; };
    std::cout<<"Content of request: "<<std::endl;
    std::for_each(data.vWork().cbegin(), data.vWork().cend(), print);  
    std::cout<<"Content of result: "<<std::endl;
    std::for_each(data.vResult().cbegin(), data.vResult().cend(), print);  */
  }
  else
  {
    while (true) {
      //TODO: blocking wait for a request (of unknown size) from rank 0
      RequestType request;
      MPI_Status status;
      MPI_Probe(CLIENT_ID, REQUEST_TAG, MPI_COMM_WORLD, &status);
      int reqSize;
      MPI_Get_count(&status, MPI_INT, &reqSize);
      if(reqSize > 0)
      {
        request.resize(reqSize);
        MPI_Recv(request.data(), reqSize, MPI_INT, CLIENT_ID, REQUEST_TAG,
          MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout<<"Node: mpi request has been received"<<std::endl;
    
        //TODO: compute result
        ResultType result(10,1);

        std::cout<<"Node: mpi result waiting to be send"<<std::endl;
        MPI_Send(result.data(), result.size(), MPI_INT, CLIENT_ID, RESULT_TAG,
          MPI_COMM_WORLD);
        std::cout<<"Node: mpi result has been sent"<<std::endl;
      }
      else
      {
        std::cout<<"Node: terminating"<<std::endl;
        break;
      }
    }

    // Synchronization
    MPI_Barrier(MPI_COMM_WORLD);

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

  MPI_Finalize();
  
  return EXIT_SUCCESS;
}
