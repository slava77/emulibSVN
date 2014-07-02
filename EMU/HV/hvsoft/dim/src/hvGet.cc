#include <dic.hxx>
#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <ncurses.h>

#include "hvdim.h"
#include "hvprimary.h"

using namespace std;

int read_module(char *, DimRpcInfo &rpc, int cardaddr,int addr, int part);

int main(int argc, char **argv)
{

  string servername=HV_DIM_NAME;
  int hostid = 0;
  bool fHostID = false;
  stringstream sname;

  if (argc >= 2)
    {
      for (int i=1; i< argc; i++)
        {
          if (argv[i][0] == '-')
            {
              switch  (argv[i][1])
                {
                case 'h' :
                  hostid =  atoi(argv[i]+2);
                  hostid = (hostid>0)?hostid:0;
                  sname << servername << "_HID" << hostid;
                  sname >> servername;
                  fHostID = true;
                  break;
                }
            }
        }
    }

  int cardaddr, addr;

  DimRpcInfo rpc(string(servername+string("/HV_REAL_DATA")).c_str(),-1, 0);

  read_module("ME+2/1/14", rpc, 1,2,2);
  read_module("ME+2/1/15", rpc, 1,1,1);
  read_module("ME+2/1/16", rpc, 1,1,2);

  read_module("ME+2/2/27", rpc, 1,7,0);
  read_module("ME+2/2/28", rpc, 0,5,0);
  read_module("ME+2/2/29", rpc, 1,6,0);
  read_module("ME+2/2/30", rpc, 0,7,0);
  read_module("ME+2/2/31", rpc, 1,5,0);
  read_module("ME+2/2/32", rpc, 0,6,0);


  read_module("ME+3/1/14", rpc, 1,2,1);
  read_module("ME+3/1/15", rpc, 1,0,2);
  read_module("ME+3/1/16", rpc, 1,0,1);

  read_module("ME+3/2/27", rpc, 0,13,0);
  read_module("ME+3/2/28", rpc, 0,8,0);
  read_module("ME+3/2/29", rpc, 0,10,0);
  read_module("ME+3/2/30", rpc, 0,11,0);
  read_module("ME+3/2/31", rpc, 0,12,0);
  read_module("ME+3/2/32", rpc, 1,8,0);


  return 0;
}

int read_module(char * chamber, DimRpcInfo &rpc, int cardaddr,int addr, int part)
{

  HVcmdDIM cmd;
  HVboardDIM realcardDIM;
  HVboard realcard;
  cmd.ID = rpc.getId();
  cmd.data.card = cardaddr;
  cmd.data.module = addr;
  /////cout << "size:" << sizeof(cmd) << endl;
  rpc.setData((void *) &cmd, sizeof(cmd));


  if (rpc.getSize() != (sizeof(struct HVboardDIM)))
    {
      cerr << "Wrong card or module number" << endl;
      ///endwin();
      return -1;
    }
  memcpy(&realcardDIM, rpc.getData(), rpc.getSize());
  memcpy(&realcard, &realcardDIM.data, sizeof(realcard));

  int i = addr;

  if (part <= 1)
    printf( "%s voltage=%4ld V  current=%6.1f uA\n", chamber, realcard.module.chan[0].vmon,
            ((float)realcard.module.chan[0].imon)/ADC_CONV_COEF);
  else
    printf( "%s voltage=%4ld V  current=%6.1f uA\n", chamber, realcard.module.chan[0+18].vmon,
            ((float)realcard.module.chan[0+18].imon)/ADC_CONV_COEF);

}
