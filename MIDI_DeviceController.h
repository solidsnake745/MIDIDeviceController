#ifndef MIDI_DeviceController_h
	#define MIDI_DeviceController_h

	#include "Settings.h"
	#include <Arduino.h>
	#include <TimerOne.h>
	#include "MIDI_DeviceController/Device.h"
	#include "SerialDebug.h"

	class MIDI_DeviceController
	{
		// Give Device access to all private members
		friend class Device;
		
		static SerialDebug _debug;
		
		//Constructors and instance management
		//_______________________________________________________________________________________________________
		private:
			MIDI_DeviceController();
			static MIDI_DeviceController *_instance;
			
		public:
			//Used to populate our single instance MDC for consumption
			static MIDI_DeviceController &getInstance();
			
		//Device management
		//_______________________________________________________________________________________________________
		private:
			static Device *_devices[MAX_DEVICES];
			static Device *_enabledDevices[MAX_DEVICES];

			uint8_t reloadEnabledDevices();

		public:
			void printStatus(); //Print each device slot's status		
			
			void addDevice(uint8_t index, Device *d); //Adds a device to the correlating index given the device's ID
			void addDevices(Device devices[], uint8_t numDevices); //Adds the given set of devices
			Device *getDevice(uint8_t index); //Retrieves a device at specified index
			void deleteDevice(uint8_t index); //Deletes device at specified index if populated

			void resetPositions();
			void calibratePositions();
		
			void assignNote(int8_t index, uint8_t note);
			void pitchBend(int8_t index, uint16_t bend);
			void clearNote(int8_t index, uint8_t note);
			
		//Note Processing
		//_______________________________________________________________________________________________________
		private:
			bool _isProcessing = false;
			bool _autoProcessing = true;
			
			//Operates devices during interrupt process
			void processNotes();
			
			//Static method for interrupt to attach to
			static void lawl();
		
			void noteAssigned();
		
		public:
			//Starts the interrupt process to play notes
			bool startProcessing();
			
			//Stops the interrupt process to play notes
			void stopProcessing();
			
			//Processes reset for auto processing (needs to be called in your loop!)
			bool resetProcessing();
			
			//Indicates whether note processing is on/off
			bool isProcessing();

		//Settings
		//_______________________________________________________________________________________________________
		private:
			uint32_t _lastAssign = 0;
			volatile uint32_t _maxDuration = MAX_DURATION_DEFAULT;
			uint16_t _idleTimeout = IDLE_TIMEOUT_DEFAULT;

		public:	
			uint8_t getMaxDevices();
			uint32_t getMaxDuration();
			void setResolution(uint16_t resolution = DEFAULT_RESOLUTION);
			void setMaxDuration(uint32_t value = MAX_DURATION_DEFAULT);
			void setIdleTimeout(int16_t value = IDLE_TIMEOUT_DEFAULT);		
			void setAutoProcess(bool value);

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
			void testDeviceInterrupt(uint8_t index);
			
			//Pitch bend test
			void testPitchBend(uint8_t index);
			
			//Basic load test to ensure we're able to play 
			void loadTest(uint8_t numDevices = MAX_DEVICES);
			
			//Plays a sequence across all devices to test set configuration
			void playStartupSequence(uint8_t version);
	};

	//Defines a global instance of our class for users to consume
	extern MIDI_DeviceController MDC;
#endif
