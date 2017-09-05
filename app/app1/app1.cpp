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

//MPI
#include <mpi.h>

//Local
//#include <Cuda/lib.cu.h>

using T = int;
int nbRequest = 10;
const int SERVER_ID=0;
int REQUEST_TAG=5555;
int RESULT_TAG=5556;

template<class T>
class MPIWorkProvider
{
  DataHandler<T>& data;
  int world_size;
  std::vector<MPI_Request> resultReqs;
  std::vector<std::pair<int, T> > reqData;
 
  bool sendWork(int rank)
  {
    const auto work = data.getWork();
    MPI_Request req;
    if(work.first == -1)
    {
      MPI_Isend(&nbRequest, 0, MPI_INT, rank, REQUEST_TAG, MPI_COMM_WORLD, &req);
      // We do no need the handle to the above operation, because we will never MPI_Wait on it.
      MPI_Request_free(&req);
      resultReqs[rank-1] = MPI_REQUEST_NULL;
      return false;
    }
    else
    {
      MPI_Isend(work.second, 1, MPI_INT, rank, REQUEST_TAG, MPI_COMM_WORLD, &req);
      reqData[rank-1].first = work.first;
      MPI_Irecv(&reqData[rank-1].second, 1, MPI_INT, rank, RESULT_TAG,
        MPI_COMM_WORLD, &resultReqs[rank-1]);
      MPI_Request_free(&req);
      return true;
    }
  }

  public:

    MPIWorkProvider(DataHandler<T>& data) : data(data)
  {
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    resultReqs.resize(world_size-1);
    reqData.resize(world_size-1);
  }

  void provide()
  {

    for(int a = 1; a < world_size; ++a)
      std::cerr << sendWork(a) << '\n';

    size_t loopIdx=0;
    while (true) {

      int index = 0;
      std::cout << resultReqs.size() << std::endl;
      std::cerr << "before wait any\n";
      MPI_Waitany(resultReqs.size(), resultReqs.data(), &index, MPI_STATUS_IGNORE);
      if(index == MPI_UNDEFINED)
        break;

      data.setResult(reqData[index].first, reqData[index].second);
      std::cout << std::boolalpha << sendWork(index+1) << std::endl;

      loopIdx++;
    }
  }
};

int main(int argc, char* argv[]) {
  
  // Init mpi and monitor runtime
  MPI_Init(NULL, NULL);
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  if (world_rank == SERVER_ID) {
    //Input data
    DataHandler<T> data(nbRequest);
    MPIWorkProvider<T> provider(data);
    provider.provide();

    // Synchronization
    MPI_Barrier(MPI_COMM_WORLD);

    auto print = [](T in) {
      std::cout<<in<<std::endl; };
    std::cout<<"Content of request: "<<std::endl;
    std::for_each(data.vWork().cbegin(), data.vWork().cend(), print);  
    std::cout<<"Content of result: "<<std::endl;
    std::for_each(data.vResult().cbegin(), data.vResult().cend(), print);  
  }
  else
  {
    while (true) {
      //TODO: blocking wait for a request (of unknown size) from rank 0
      int request[2];
      std::cout<<"Node: mpi request: waiting to  receive"<<std::endl;
      MPI_Status status;
      MPI_Probe(SERVER_ID, REQUEST_TAG, MPI_COMM_WORLD, &status);
      int reqSize;
      MPI_Get_count(&status, MPI_INT, &reqSize);
      if(reqSize > 0)
      {
        MPI_Recv(&request, reqSize, MPI_INT, SERVER_ID, REQUEST_TAG,
          MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout<<"Node: mpi request has been received"<<std::endl;
    
        //TODO: compute result
        int result = 0;
        //TODO: sending result to rank 0
        std::cout<<"Node: mpi result waiting to be send"<<std::endl;
        MPI_Send(&result, 1, MPI_INT, SERVER_ID, RESULT_TAG,
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
