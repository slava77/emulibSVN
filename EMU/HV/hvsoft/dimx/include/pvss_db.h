#ifndef PVSS_DB_H
#define PVSS_DB_H
#include <string>
#include <vector>
using namespace std;

string host_database;
vector<string> module_database;
vector<int> card_database;
vector<int> primary_database;
vector<int> primary_database_2;

int number_of_masters;

int pvss_databases();
int fill_pvss_databases(int hostid);
#endif

