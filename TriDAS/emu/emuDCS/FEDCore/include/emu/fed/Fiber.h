/*****************************************************************************\
* $Id: Fiber.h,v 1.7 2010/11/30 10:10:15 cvuosalo Exp $
\*****************************************************************************/
#ifndef __EMU_FED_FIBER_H__
#define __EMU_FED_FIBER_H__

#include <string>

namespace emu {
	namespace fed {

		/** @class Fiber A class for easy access to chamber names. **/
		class Fiber
		{
		public:

			//friend class FiberParser;
			//friend class DDU;

			/** Default constructor **/
			Fiber();

			/** Constructor to build using the name of the fiber **/
			Fiber(const unsigned int &fifoNumber, const std::string &name = "", const bool &killed = false, const bool &ignoreErr = false);

			/** Constructor used to set all the variables **/
			Fiber(const unsigned int &fifoNumber, const unsigned int &plusMinus, const unsigned int &station, const unsigned int &ring, const unsigned int &number, const bool &killed = false, const bool &ignoreErr = false);

			/** Constructor with a string for the endcap **/
			Fiber(const unsigned int &fifoNumber, const std::string &endcap, const unsigned int &station, const unsigned int &ring, const unsigned int &number, const bool &killed, const bool &ignoreErr = false);

			/** @returns the fiber input number. **/
			inline unsigned int getFiberNumber() { return fiberNumber_; }
			inline unsigned int number() { return fiberNumber_; }

			/** Set the fiber number **/
			inline void setFiberNumber(const unsigned int &number) { fiberNumber_ = number; }

			/** @returns a human-readable string naming the chamber, like "+1/2/33". **/
			inline std::string getName() { return name_; }

			/** Set the name via string. **/
			inline void setName(const std::string &name) { parseName_(name); }

			/** @returns the endcap where the chamber is located.  Is either "+" or "-". **/
			inline std::string getEndcap() { return endcap_; }

			/** @returns the endcap where the chamber is located.  Is either 1 or 2. **/
			inline unsigned int getPlusMinus() { return plusMinus_; }

			/** @returns the station where the chamber is located.  Is a number 1, 2, 3, or 4. **/
			inline unsigned int getStation() { return station_; }

			/** @returns the "ring" of the chamber, or on which annulus it is located.  Is a number 1, 2, or 3. **/
			inline unsigned int getRing() { return ring_; }

			/** @returns The chamber number.  The combination of endcap, station, type, and number uniquely defines each chamber. **/
			inline unsigned int getNumber() { return number_; }

			/** @returns whether or not the owning DDU should be configured to kill this fiber **/
			inline bool isKilled() { return killed_; }

			/** Sets whether or not the DDU should be configured to kill this fiber **/
			inline void setKilled(const bool &killed) { killed_ = killed; }

			/** @returns whether or not random DDU errors should be ignored **/
			inline bool ignoreErr() { return ignoreErr_; }

			/** Sets whether or not random DDU errors should be ignored **/
			inline void setIgnoreErr(const bool &ignoreErr) { ignoreErr_ = ignoreErr; }

		private:

			/** Parse out the name string and set the corresponding values. **/
			void parseName_(const std::string &name);

			/** Generate the name from the stored values **/
			void generateName_();

			/// The fiber input number
			unsigned int fiberNumber_;

			/// The name of the chamber (so I don't have to regenerate it every time)
			std::string name_;

			/// The endcap (+ or -)
			std::string endcap_;

			/// Whether this is the plus or minus side (1 or 2, matching CMSSW's notation)
			unsigned int plusMinus_;

			/// The station
			unsigned int station_;

			/// The ring
			unsigned int ring_;

			/// The chamber/SP number
			unsigned int number_;

			/// Whether or not this fiber is killed
			bool killed_;

			/// Ignore rare random errors from DDU boards to decrease FMM error rate
			bool ignoreErr_;

		};

	}
}

#endif
