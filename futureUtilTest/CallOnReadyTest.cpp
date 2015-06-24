
#include "futureUtil/futureUtil.h"
#include <gtest/gtest.h>
#include <functional>
#include <future>
#include <iostream>


typedef double type;

const int upperLimit = 1000;

/// todo try thread pool of 7 threads
/// todo compare to omp (with critical section)
/// blog post

// computationally intensive function that returns the passed in value
type ExpensiveCalculateSelf(type original)
{
    double doubOriginal = original;
    double intermediateValue = std::sqrt(std::sqrt(doubOriginal*doubOriginal*doubOriginal*doubOriginal));
    intermediateValue = intermediateValue / upperLimit;
    
    // as the amount of work decreases, the overhead of spawning threads overwhelms the cost of work
    for (int i = 0; i < 10000000; ++i)
    {
        intermediateValue = std::acos(std::cos(intermediateValue));
        intermediateValue = std::asin(std::sin(intermediateValue));
    }

    return static_cast<type>(intermediateValue*upperLimit);
}

bool ExpensiveBoolGenerator(type original)
{
    return (original == ExpensiveCalculateSelf(original));
}

class Graph;
class Node;
/// this prototype function shows how a real edge connecting function would be implemented
void NodeConnectorExample(bool los, Graph*, Node*, Node*)
{
}


void PrintInt(type value)
{
   //std::cout << value << std::endl;
}

TEST(CallWhenReady, CallWhenReady)
{
    std::function<type(type)> expensiveFunction = ExpensiveCalculateSelf;
    std::function<void(type)> functionToCallWhenReady = PrintInt;

    std::list<std::future<type>> futures;

    for (type i = 0; i < upperLimit; ++i)
    {
        futures.push_back(std::async(std::launch::async, expensiveFunction, i));
    }


    CallWhenReady(futures, functionToCallWhenReady);
}

TEST(CallWhenReady, WaitThenCall)
{
    std::function<type(type)> expensiveFunction = ExpensiveCalculateSelf;
    std::function<void(type)> functionToCallWhenReady = PrintInt;

    std::list<std::future<type>> futures;

    for (type i = 0; i < upperLimit; ++i)
    {
        futures.push_back(std::async(std::launch::async, expensiveFunction, i));
    }


    WaitThenCall(futures, functionToCallWhenReady);
}

TEST(NodeConnectorExample, WaitThenCall)
{
    std::function<bool(type)> losCheck = ExpensiveBoolGenerator;


    std::list<std::pair<std::future<bool>, std::function<void(bool)>>> futures;

    for (type i = 0; i < upperLimit; ++i)
    {
        Graph* graph;
        Node* node1; //these are the nodes that we would normally test for LOS, instead we're using the ExpensiveBoolGenerator
        Node* node2;

        ///Bind sets the parameters known now for the NodeConnectorExample function so that only one currently unknown will need to be passed into it when its called in the future
        std::function<void(bool)> connectNodes = std::bind(NodeConnectorExample, std::placeholders::_1, graph, node1, node2);

        futures.push_back(std::make_pair(std::async(std::launch::async, losCheck, i), connectNodes));
    }

    WaitThenCall(futures);
}

TEST(CallWhenReady, SynchronousBaseline)
{
    std::list<type> results;
    for (type i = 0; i < upperLimit; ++i)
    {
        EXPECT_NEAR(i, ExpensiveCalculateSelf(i), 1);
        //std::cout << i  << " " << ExpensiveCalculateSelf(i) << std::endl;
    }
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    std::getchar(); // keep console window open until Return keystroke
}