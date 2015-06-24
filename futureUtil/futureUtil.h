#pragma once

#include <future>
#include <functional>
#include <list>
#include <chrono>
#include <thread>

/// calls a function on each element of the list when it becomes ready, consumes the elements and empties the list
template<typename T, typename ReturnType>
void CallWhenReady(std::list<std::future<T>>& list, std::function<ReturnType(T)>& function)
{
    std::chrono::milliseconds delay(1);

    while (!list.empty())
    {
        std::this_thread::sleep_for(delay);
        std::list<std::future<T>>::iterator it = list.begin();
        while (it != list.end())
        {
            if ((*it)._Is_ready())
            {
                function((*it).get());
                it = list.erase(it);
            }
            else
                ++it;

        }
    }
}

/// calls a function on each element of the list waiting for it to be ready, don't attempt to use the futures after calling this
template<typename T, typename ReturnType>
void WaitThenCall(std::list<std::future<T>>& list, std::function<ReturnType(T)>& function)
{
    std::list<std::future<T>>::iterator it = list.begin();
    while (it != list.end())
    {
        (*it).wait();
 
        function((*it).get());
        ++it;
    }
}

template<typename T, typename ReturnType>
void WaitThenCall(std::list<std::pair<std::future<T>, std::function<ReturnType(T)>>>& list )
{
    auto it = list.begin();
    while (it != list.end())
    {
        auto future = std::move((*it).first);
        auto function = (*it).second;

        future.wait();

        function(future.get());
        ++it;
    }
}