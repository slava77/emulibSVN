
#include <string>
#include <vector>
using namespace std;

extern string host_database;
extern vector<string> module_database;
extern vector<int> card_database;
extern vector<int> primary_database;
extern vector<int> primary_database_2;

extern int number_of_masters;

int pvss_databases();
int fill_pvss_databases(int hostid);
