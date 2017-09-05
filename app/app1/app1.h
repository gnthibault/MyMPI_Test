// vim: ts=2
// vim: sw=2
// vim: et
#pragma once

#include <vector>
#include <mutex>
#include <utility>

template<class T>
class DataHandler
{
  std::vector<T> m_vWork;
  uint64_t m_requestCounter=0;
  std::mutex m_requestMutex;
  //Output dat TODO TN: think about the fact it will be variable lenght
  std::vector<T> m_vResult;
  // int64_t resultCounter=0;
  std::mutex m_resultMutex;

  public:

    DataHandler(int nbWorks) : m_vWork(nbWorks, 0), m_vResult(nbWorks, 0)
  {}

  std::pair<int, T*> getWork() {
    std::lock_guard<std::mutex> lock(m_requestMutex);
    if(m_requestCounter >= m_vWork.size())
      return std::make_pair(-1, nullptr);
    auto ret = std::make_pair(m_requestCounter, &m_vWork[m_requestCounter]);
    ++m_requestCounter;
    return ret;
  }

  void setResult(int r, const T& v) {
    std::lock_guard<std::mutex> lock(m_resultMutex);
    m_vResult[r] = v;
  }

  const std::vector<T>& vWork() {return m_vWork;}
  const std::vector<T>& vResult() {return m_vResult;}
};
