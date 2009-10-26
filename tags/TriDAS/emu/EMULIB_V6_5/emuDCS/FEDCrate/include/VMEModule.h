/*****************************************************************************\
* $Id: VMEModule.h,v 3.5 2008/08/15 08:35:51 paste Exp $
*
* $Log: VMEModule.h,v $
* Revision 3.5  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __VMEMODULE_H__
#define __VMEMODULE_H__

#include "JTAG_constants.h"

#include "EmuFEDLoggable.h"
#include "JTAGDevice.h"

/* VMEModule is a virtual class for DCC and DDU classes.
 * A VMEModule should be apathetic to the controller and crate,
 * and although this breaks down at the program execution level (where
 * any given read/write from/to the DCC and DDU requires access of the
 * controller), it shouldn't break down at the object level.
 * Therefore, I removed or modified all public methods that referenced
 * the crate or controller.  Use the FEDCrate and VMEController objects
 * for those methods, not the DDU and DCC objects.
 */

namespace emu {

	namespace fed {

		class VMEController;
		
		class VMEModule: public EmuFEDLoggable, public JTAGDevice
		{
		public:
			//enum {MAXLINE = 70000};
			
			/// probably should make the user pass a Crate
			/// rather than a number
			/// automatically registers itself with the Crate
			VMEModule(int myCrate, int mySlot);
			VMEModule(int mySlot);
			virtual ~VMEModule() {};
			int slot() const {return slot_;}
			
			/// these will only be called by the VMEController
			virtual void start();
			virtual void end();
			/// should automatically start().  Here's what you do if
			/// you want to end() by hand
			void endDevice();
			
			enum BOARDTYPE { DDU_ENUM=0, DCC_ENUM };
			virtual unsigned int boardType() const = 0;
			
			void setController(VMEController *controller);
		
		protected:
		
			/// JTAG stuff
			void devdo(DEVTYPE dev,int ncmd,const char *cmd,int nbuf,const char *inbuf,char *outbuf,int irdsnd);
			void scan(int reg,const char *snd,int cnt2,char *rcv,int ird);
			void RestoreIdle();
			void InitJTAG(int port);
			void CloseJTAG();
			void send_last();
			void RestoreIdle_reset();
			void scan_reset(int reg, const char *snd, int cnt2, char *rcv,int ird);
			void sleep_vme(const char *outbuf);   // in usecs (min 16 usec)
			void sleep_vme2(unsigned short int time); // time in usec
			void long_sleep_vme2(float time);   // time in usec
			void handshake_vme();
			void flush_vme();
			void vmeser(const char *cmd,const char *snd,char *rcv);
			void vmepara(const char *cmd,const char *snd,char *rcv);
			void dcc(const char *cmd,char *rcv);
			void vme_adc(int ichp,int ichn,char *rcv);
			void vme_controller(int irdwr,unsigned short int *ptr,unsigned short int *data,char *rcv);
			void CAEN_close(void);
			int CAEN_reset(void);
			int CAEN_read(unsigned long Address,unsigned short int *data);
			int CAEN_write(unsigned long Address,unsigned short int *data);
		
			void sdly();
			void initDevice(int a);
			/// used for calls to do_vme
			enum FCN { VME_READ=1, VME_WRITE=2 };
			enum WRT { LATER, NOW };
		
		protected:
			/// required for DDU/DCC communications
			char sndbuf[4096];
			char rcvbuf[4096];
			char rcvbuf2[4096];
			char cmd[4096];
		
		private:
			VMEController *vmeController_; // need to know, don't need to share
			int slot_;
		};

	}
}

#endif
