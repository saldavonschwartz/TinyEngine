//
//  Task.hpp
//  TinyEngine
//
//  Created by Federico Saldarini on 10/31/19.
//  Copyright © 2019 0xfede. All rights reserved.
//

#ifndef Task_hpp
#define Task_hpp

#include <thread>
#include <vector>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>

using std::future;
using std::function;
using std::deque;
using std::vector;
using std::thread;
using std::mutex;
using std::condition_variable;
using std::unique_lock;
using std::packaged_task;


class Task {
public:
  using TaskType = packaged_task<void()>;
  
  class Group : public vector<future<void>> {
  public:
    void wait() {
      for (auto& t : *this) {
        t.wait();
      }
    }
  };

  static size_t threadCount;
  
  static void init() {
    ticking = true;
    
    for(int i = 0; i < threadCount; i++) {
      pool.emplace_back(tick);
    }
  }
  
  template <class... Tn>
  static Group submit(Tn&&... tn) {
    Group g;
    _submit(g, tn...);
    return g;
  }
  
  template <class... Tn>
  static void submit(Group& g, Tn&&... tn) {
    _submit(g, tn...);
  }
  
  template <class T0, class... Tn>
  static void _submit(Group& g, T0&& t0, Tn&&... tn) {
    {
      unique_lock<mutex> lock(M);
      q.emplace_back(t0);
      g.emplace_back(q.back().get_future());
    }

    cv.notify_one();
    
    if constexpr (sizeof...(Tn)) {
      _submit(g, tn...);
    }
  }
  
  static void shutDown() {
      {
        unique_lock<mutex> lock(M);
        ticking = false;
      }
      
      cv.notify_all();
    
      for (auto& t : pool) {
        t.join();
        
      }
      
      pool.empty();
  }
  
protected:
  static condition_variable cv;
  static vector<thread> pool;
  static deque<TaskType> q;
  static bool ticking;
  static mutex M;
  
  static void tick() {
    while (true) {
      packaged_task<void()> task;
      
      {
        unique_lock<mutex> lock(M);
        
        cv.wait(lock, [] {
          return !q.empty() || !ticking;
        });
        
        if (!ticking) {
          return;
        }
        
        task = move(q.front());
        q.pop_front();
      }
      
      task();
    }
  };
};

inline size_t Task::threadCount = thread::hardware_concurrency();
inline deque<Task::TaskType> Task::q;
inline condition_variable Task::cv;
inline bool Task::ticking = false;
inline vector<thread> Task::pool;
inline mutex Task::M;

#endif /* Task_hpp */
