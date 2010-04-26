/*****************************************************************************\
* $Id: FIFO.h,v 1.5 2009/12/10 16:24:28 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_FIFO_H__
#define __EMU_FED_FIFO_H__

#include <string>

namespace emu {
	namespace fed {

		/** @class FIFO A class for easy access to DCC FIFOs. **/
		class FIFO
		{
		public:

			//friend class FIFOParser;
			//friend class DCC;

			/** Constructor used to set the RUI **/
			FIFO(const unsigned int &fifoNumber, const unsigned int &rui = 0, const bool &used = true);
			
			/** @Returns the FIFO input number. **/
			inline unsigned int getNumber() { return fifoNumber_; }
			inline unsigned int number() { return fifoNumber_; }

			/** @Returns the DDU RUI of the FIFO **/
			inline unsigned int getRUI() { return rui_; }
			
			/** @Returns whether or not the owning DCC should be configured to use this FIFO **/
			inline bool isUsed() { return used_; }
			
			/** Sets whether of not the DCC should be configured to use this FIFO **/
			inline void setUsed(const bool &used) { used_ = used; }

		private:
			
			/// The FIFO number
			unsigned int fifoNumber_;
		
			/// The RUI of the DDU that this FIFO governs
			unsigned int rui_;
			
			/// Whether or not this FIFO is in use
			bool used_;

		};

	}
}

#endif