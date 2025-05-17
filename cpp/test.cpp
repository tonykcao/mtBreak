// C++ program for demonstrating 
// similaritites
#include <ctime>
#include <iostream>
#include <random>
using namespace std;

int main()
{
  // Initializing the sequence 
  // with a seed value
  // similar to srand()
  mt19937 mt(time(nullptr)); 

  // Printing a random number
  // similar to rand()
  cout << mt() << '\n'; 
  return 0;
}