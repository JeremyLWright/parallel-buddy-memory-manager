/**
 * @brief Basic Driver for the Binary Buddy Allocator
 *
 * @par
 * Copyright Jeremy Wright (c) 2011
 * Creative Commons Attribution-ShareAlike 3.0 Unported License.
 */

#include "BuddyAllocator.hpp"
#include "SieveOfAtkin.hpp"
#include <iostream>
#include <pthread.h>
#include <sstream>
#include <string>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <sstream>
using namespace std;
/**
 * Uses a C++ streams to convert a string representation to some other type.
 *
 * @param[out] t Target to save the converted value
 * @param[in] s Source string to convert the value from.
 * @param[in] f Format e.g.std::hex, or std::dec
 * @return returns fail is conversion is not successful.
 */
template <typename T>
bool from_string(T& t, const std::string& s, std::ios_base& (*f)(std::ios_base&))
{
	std::istringstream iss(s, std::istringstream::in);
	return !(iss >> f >> t).fail();
}

BuddyAllocator<int, 10> processesAllocator;

void Process()
{
    processesAllocator.allocate(10);
    SieveOfAtkin primes(100);
    stringstream s;
    int const number = 100 + rand() % 100; //Random number between 100 and 200
    for(int i = 0; i < number; ++i)
    {
#ifdef OUTPUT_PROGRESS
        s << "Thread: " << pthread_self() << " " << static_cast<float>(i)/static_cast<float>(number)*100 << "% complete." << endl;
        cout << s.str();
#endif
        primes.next();
    }
    pthread_exit(0);
}

int main(int argc, const char *argv[])
{
    size_t numProcesses;
    try
    {
        from_string<size_t>(numProcesses, argv[1], std::dec);
    }
    catch(std::logic_error&)
    {
        cerr << "Incorrect Number of Parameters" << endl;
        exit(1);
    }
    cout << "Allocating " << numProcesses << " processes." << endl;
    pthread_t processes[numProcesses];

    /* Spawn Processes */
    for(size_t i = 0; i < numProcesses; ++i)
    {
        pthread_create(&processes[i], NULL, &Process, NULL);
    }
    
    /* Wait for Processes */
    for(size_t i = 0; i < numProcesses; ++i)
    {
        pthread_join(processes[i], NULL);
        cout << "Process " << i << " completed." << endl;
    }


    return 0;
}
