// erasing from vector
#include <iostream>
#include <vector>
#include <stdio.h>
#include <algorithm>

using namespace std;

int main ()
{
  //vector<int> myvector {1,1,0,1,1,0,1,0,0,1};

  // set some values (from 1 to 10)
  //for (int i=1; i<=10; i++) myvector.push_back(i);

  static const int arr[] = {1,1,0,0,1,0,1,0,0,1};
  vector<int> myvector (arr, arr + sizeof(arr) / sizeof(arr[0]) );

  vector< int >::iterator it = myvector.begin();

  while(it != myvector.end()) {

           if(*it == 0) {
        	   myvector.erase(it);
          }
          else ++it;
  }

//  // erase the 6th element
//  myvector.erase (myvector.begin()+5);
//
//  // erase the first 3 elements:
//  myvector.erase (myvector.begin(),myvector.begin()+3);

  std::cout << "myvector contains:\n";

  for(int j=0;j<myvector.size();j++){
	  cout << myvector[j]<< " ";
  }
  cout << "\n";

  return 0;
}
