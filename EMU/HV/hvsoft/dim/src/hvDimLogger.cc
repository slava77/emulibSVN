#include <iostream>
#include <map>

#include <dic.hxx>

using namespace std;

class LoggerMsg: public DimUpdatedInfo
{
public:
  // subscribe to service with handler
  LoggerMsg(char * name): DimUpdatedInfo(name,  "") {}
  void infoHandler( )
  {
    std::cout << getString();  // update handler
  }
};

int main(int argc, char** argv)
{
  char *ptr, *ptr1;
  int svctype;
  string dns_node = DimClient::getDnsNode();
  int dns_port = DimClient::getDnsPort();

  switch (argc)
    {
    case 3:
      dns_port = atoi(argv[2]);
    case 2:
      dns_node = argv[1];
    }
  cout << "Set DIM_DNS_NODE=" << dns_node << ":" << dns_port << endl;
  DimClient::setDnsNode(const_cast<char *>(dns_node.c_str()), dns_port);

  DimBrowser br;
  map<string,LoggerMsg*> loggers;

  loggers.clear();

  while (true)
    {

      br.getServices("HV_DIM_SERVER*/LOGGER");
      while ((svctype = br.getNextService(ptr, ptr1))!= 0)
        {

          map<string, LoggerMsg*>::iterator itr = loggers.find(string(ptr));
          if ( itr==loggers.end())
            {
              cout << "Adding logger for " << ptr << endl;
              loggers[string(ptr)] = new LoggerMsg(ptr);
              // std::cout << "type = " << svctype << " - " << ptr << " " << ptr1 << std::endl;
            }
        }

      sleep(10);
    }
  return 0;
}

