// vim: ts=2
// vim: sw=2
// vim: et
#pragma once

#include <vector>
#include <memory>
#include <mutex>
#include <utility>

//MPI
#include <mpi.h>

#define SERVER_ID 0
#define REQUEST_TAG 5555
#define RESULT_TAG 5556


template<class T>
class DataHandler
{
public:

  DataHandler()=default;

  std::pair<int, T*> getWork() {
    std::lock_guard<std::mutex> lock(m_requestMutex);
    if(m_requestCounter >= m_vWork.size())
      return std::make_pair(-1, nullptr);
    auto ret = std::make_pair(m_requestCounter, m_vWork.at(m_requestCounter).get());
    ++m_requestCounter;
    return ret;
  }
  void push_back(const T& workItem){
    {
      std::lock_guard<std::mutex> lock(m_requestMutex);
      m_vWork.emplace_back(std::unique_ptr<T> (new T(workItem)));
    }
    {
      std::lock_guard<std::mutex> lock(m_resultMutex);
      m_vResult.emplace_back(std::unique_ptr<T> (new T()));
    }
  }

  void setResult(int r, const T& v) {
    std::lock_guard<std::mutex> lock(m_resultMutex);
    m_vResult.at(r) = std::unique_ptr<T> (new T(v));
  }

  const std::vector<T>& vWork() {return m_vWork;}
  const std::vector<T>& vResult() {return m_vResult;}

private:
  std::vector<std::unique_ptr<T>> m_vWork;       //T becames vector of int (list of request)
  uint64_t m_requestCounter=0;  //index of current request
  std::mutex m_requestMutex;    //protect simultaneous access to m_vWork
  std::vector<std::unique_ptr<T>> m_vResult;     //T becames vector of int (list of routes)
  std::mutex m_resultMutex;     //protect simultaneous access to m_vResult
};

template<class T>
class MPIWorkProvider
{
  bool sendWork(int rank)
  {
    const auto work = m_data.getWork();
    MPI_Request req;
    if(work.first == -1)
    {
      MPI_Isend(nullptr, 0, MPI_INT, rank, REQUEST_TAG, MPI_COMM_WORLD, &req);
      // We do no need the handle to the above operation, because we will never MPI_Wait on it.
      MPI_Request_free(&req);
      //m_resultReqs.at(rank-1) = MPI_REQUEST_NULL;
      m_currentWorldSize--;
      return false;
    }
    else
    {
      size_t size=work.second->size();
      if (size<1) {
        std::cout<<"ERROR is HERE"<<std::endl;
        return false;
      }
      MPI_Isend(work.second->data(), work.second->size(), MPI_INT, rank,
        REQUEST_TAG, MPI_COMM_WORLD, &req);
      m_resultData.at(rank-1).first = work.first;

      //Now handling variable size answer
      /*MPI_Status status;
      MPI_Iprobe(rank, RESULT_TAG, MPI_COMM_WORLD, &req, &status);
      int reqSize;
      MPI_Get_count(&status, MPI_INT, &reqSize);
      MPI::Status::Get_source()
      m_resultData.at(rank-1).first = work.first;
      m_resultData.at(rank-1).second->resize();*/
      //MPI_recv(m_resultData.at(rank-1).second->data(), 1, MPI_INT, rank, RESULT_TAG,
      //  MPI_COMM_WORLD);
      //MPI_Request_free(&req);
      return true;
    }
  }

public:

    MPIWorkProvider(DataHandler<T>& data) : m_data(data)
  {
    MPI_Comm_size(MPI_COMM_WORLD, &m_worldSize);
    m_currentWorldSize=m_worldSize;
    m_resultReqs.resize(m_worldSize-1,MPI_REQUEST_NULL);
    for (int i=0;i<m_worldSize-1;++i){
      m_resultData.push_back(std::make_pair(0,std::unique_ptr<T>(new T(1))));
    }

  }

  void provide()
  {

    for(int a = 1; a < m_worldSize; ++a) {
      std::cout<<"Sending work to server "<<a<<std::endl;
      sendWork(a);
    }

    while (m_currentWorldSize>1) {
      MPI_Status status;
      int flag;
      MPI_Iprobe(MPI_ANY_SOURCE, RESULT_TAG, MPI_COMM_WORLD, &flag, &status);

      if (flag) {
        // Get informations about the incoming message
        int rank=status.MPI_SOURCE;
        int sizeBuff;
        MPI_Get_count(&status, MPI_INT, &sizeBuff);
        
        //Now allocate enough space to get result  
        m_resultData.at(rank-1).second->resize(sizeBuff);
        // Actually blocking receive
        MPI_Recv(m_resultData.at(rank-1).second->data(), sizeBuff,
          MPI_INT, rank, RESULT_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //copy to the result buffer
        m_data.setResult(m_resultData.at(rank-1).first,
          *m_resultData.at(rank-1).second);
        sendWork(rank);
      }
      
      /*std::cerr << "before wait any resultReqs size is "<<m_resultReqs.size()
        <<std::endl;
      MPI_Waitany(m_resultReqs.size(), m_resultReqs.data(), &index, MPI_STATUS_IGNORE);
      std::cerr<<"After wait any"<<std::endl;
      if(index == MPI_UNDEFINED)
        break;
      std::cerr<<"Before setResults"<<std::endl;
      std::cerr<<"After setResults"<<std::endl;
      std::cout << std::boolalpha << sendWork(index+1) << std::endl*/;
    }
  }

private:
  DataHandler<T>& m_data;       //Thread-safe work queue
  int m_worldSize;             //number of ranks
  int m_currentWorldSize; 
  std::vector<MPI_Request> m_resultReqs; //List of asyncronous mpi events
  std::vector<std::pair<int, std::unique_ptr<T>> > m_resultData; //result key, buffer
};


