#ifndef MIDI_Device_Controller_h
	#define MIDI_Device_Controller_h

	#include "Settings.h"
	#include "MDC_Extras.h"
	#include <Arduino.h>	
	#include "MIDI_Device_Controller/MIDI_Periods.h"
	#include "MIDI_Devices/MIDI_Pitch.h"
	#include "MIDI_Devices/MIDI_SN74HC595N.h"
	#include "MIDI_Devices/MIDI_Digital_IO.h"
	#include "SerialDebug/SerialDebug.h"

	#include "MIDI_Device_Controller/ITimer/ITimer.h"
	//Resolve timer interrupt implementation
	#if defined(ARDUINO_ARCH_AVR)
		#warning "ARDUINO_ARCH_AVR"
		#include "MIDI_Device_Controller/ITimer/TimerOne_Timer.h"
	#elif defined(CORE_TEENSY)
		#warning "CORE_TEENSY"
		#include "MIDI_Device_Controller/ITimer/TimerOne_Timer.h"
	#elif defined(ESP32)
		#warning "TODO: Implement ESP32 timer"
		//TODO: Implement ESP32 timer
	#endif

	///[MDC] Controls and manages various MIDI device objects
	class MIDI_Device_Controller
	{
		// Give Device access to all private members
		friend class MIDI_Pitch;
		friend class MIDI_SN74HC595N;
		friend class MIDI_Digital_IO;
		
		static SerialDebug _debug;
		
		//Constructors and instance management
		//_______________________________________________________________________________________________________
		private:
			MIDI_Device_Controller();
			static MIDI_Device_Controller *_instance;
			
		public:
			//Used to populate our single instance MDC for consumption
			/// @private
			static MIDI_Device_Controller &getInstance();
		
		//Device management/operation
		//_______________________________________________________________________________________________________
		private:
			static MIDI_Pitch *_pitchDevices[MAX_PITCH_DEVICES];
			static MIDI_Pitch *_enabledPitchDevices[MAX_PITCH_DEVICES];
			static uint8_t _numEnabled;
			
			uint8_t reloadEnabledDevices();
			
			static MIDI_SN74HC595N *_SN74HC595N;
			
			static MIDI_Digital_IO *_digitalIO;
			
		public:
			///Prints status information about this controller to Serial
			void printStatus();
			
			///Adds a device to the controller
			/*!
				\param index Index to assign the device to
				\param d Device to add
			*/
			void addDevice(uint8_t index, MIDI_Pitch *d);
			
			///Retrieves a device from the controller
			/*!
				\param index Index to retrieve the device from
			*/
			MIDI_Pitch *getDevice(uint8_t index);
			
			///Deletes a device from the controller
			/*!
				\param index Index to try deleting the device from
			*/
			void deleteDevice(uint8_t index);

			void setSN74HC595N(MIDI_SN74HC595N *device);
			MIDI_SN74HC595N *getSN74HC595N();
			
			void setDigitalIO(MIDI_Digital_IO *device);
			MIDI_Digital_IO *getDigitalIO();
			
			void resetDevicePositions();
			void calibrateDevicePositions();
		
			void playDeviceNote(int8_t index, uint8_t note);
			void bendDeviceNote(int8_t index, uint16_t bend);
			void stopDeviceNote(int8_t index, uint8_t note);
			
		//Note Processing
		//_______________________________________________________________________________________________________
		private:
			bool _isPlayingNotes = false;
			bool _autoPlayNotes = true;

			//Resolve timer interrupt implementation
			#if defined(ARDUINO_ARCH_AVR)
				ITimer *_timer = new TimerOne_Timer();
			#elif defined(CORE_TEENSY)
				ITimer *_timer = new TimerOne_Timer();
			#elif defined(ESP32)
				//TODO: Implement ESP32 timer
				ITimer *_timer;
			#endif
			
			//Operates devices during interrupt process
			void processNotes();
			
			//Static method for interrupt to attach to
			static void lawl();
		
			void noteAssigned();
		
		public:
			//Starts the interrupt process to play notes
			bool startPlaying();
			
			//Stops the interrupt process to play notes
			void stopPlaying();
			
			//Processes reset for auto processing (needs to be called in your loop!)
			bool process();
			
			//Indicates whether note processing is on/off
			bool isPlayingNotes();

		//Settings
		//_______________________________________________________________________________________________________
		private:
			uint32_t _lastAssign = 0;
			uint32_t _maxDuration = MAX_DURATION_DEFAULT;
			uint16_t _idleTimeout = IDLE_TIMEOUT_DEFAULT;

		public:
			//Indicates whether note processing is automatically started on note assignment
			bool isAutoPlayEnabled();
			
			uint8_t getMaxPitchDevices();
			uint32_t getMaxDuration();
			void setResolution(uint16_t resolution = DEFAULT_RESOLUTION);
			
			///@private
			void setDebugResolution();
			
			///@private
			void setMaxDuration(uint32_t value = MAX_DURATION_DEFAULT);
			
			///Sets the timeout period in milliseconds
			/*!
				When idle, all processing and outputs are turned off.
				\param value Number of milliseconds
			*/
			void setIdleTimeout(int16_t value = IDLE_TIMEOUT_DEFAULT);		
			void setAutoPlay(bool value);

		//LED pin functionality
		//_______________________________________________________________________________________________________
		private: 
			int8_t _ledPin = -1;
			void LEDOn();
			void LEDOff();
		
		public: 
			void setLEDPin(int8_t pin);
		
		//Tests/Debug 
		//_______________________________________________________________________________________________________
		//If you make any changes to this library, a good way to ensure
		//it still works is by running all of these test functions
		public:
			//Plays a test tone on a given device via our interrupt process a few times
			void testPitchDeviceInterrupt(uint8_t index);
			
			//Pitch bend test
			void testPitchBend(uint8_t index);
			
			//Basic load test to ensure we're able to play 
			void loadTest(uint8_t numDevices = MAX_PITCH_DEVICES);
			
			//Plays a sequence across all devices to test set configuration
			void playStartupSequence(uint8_t version);
	};

	//Defines a global instance of our class for users to consume
	extern MIDI_Device_Controller MDC;
#endif
