#include "AFEB/teststand/utils/String.h"

#include <iostream>
using namespace std;

vector<string> AFEB::teststand::utils::splitSting( const string& str, const string& delimiters ){
  vector<string> pieces;
  size_t start = 0;
  size_t found = str.find_first_of( delimiters );
  while( found!=string::npos ){
    pieces.push_back( str.substr( start, found-start ) );
    //cout << "split: " << pieces.back() << endl;
    start = found + 1;
    found = str.find_first_of( delimiters, start );
  }
  return pieces;
}

string AFEB::teststand::utils::shaveOffBlanks( const string& str ){
  const string blanks(" \t");
  size_t start = str.find_first_not_of( blanks );
  size_t stop  = str.find_last_not_of ( blanks );
  if ( start == string::npos ) start = 0;
  if ( stop  == string::npos ) stop  = str.size() - 1;
  return str.substr( start, stop + 1 - start );
}
