#include "../DeviceChain_Controller.h"

SerialDebug DeviceChain_Controller::_debug;

//Constructors and instance management
//_______________________________________________________________________________________________________

//Global singleton instance
DeviceChain_Controller DCC = DeviceChain_Controller::getInstance();

DeviceChain_Controller *DeviceChain_Controller::_instance = NULL;

DeviceChain_Controller::DeviceChain_Controller()
{
}

DeviceChain_Controller &DeviceChain_Controller::getInstance()
{
	//Single instance check, instantiation, and return
	if (_instance == NULL) _instance = new DeviceChain_Controller();
	return *_instance;
}

//Device management
//_______________________________________________________________________________________________________
DeviceChain_Base *DeviceChain_Controller::_chains[MAX_CHAINS];

void DeviceChain_Controller::printStatus()
{
	int i = 0;
	while(i != MAX_CHAINS)
	{
		_debug.println(F("Chain %d"), i);
		if(_chains[i])
		{
			_debug.debugln(5, F("Populated"));
			//TODO: print info about chain
		}
		else
		{
			_debug.println(F("Empty"));
		}
		
		_debug.println("");
		delay(10);
		i++;
	}
}

void DeviceChain_Controller::addChain(uint8_t index, DeviceChain_Base *c)
{
	if(index > MAX_CHAINS - 1)
	{
		_debug.println(F("Can't add chain at index %d"), index);
		_debug.debugln(5, F("Max chain index is %d"), MAX_CHAINS - 1);
		return;
	}

	if(_chains[index] != NULL)
	{		
		_debug.println(F("Chain already exists at index %d"), index);
		return;
	}
	
	_chains[index] = c;
}

DeviceChain_Base *DeviceChain_Controller::getChain(uint8_t index)
{
	if(index > MAX_CHAINS - 1)
	{
		// _debug.debugln(3, F("Max index is %d"), MAX_CHAINS - 1);
		return NULL;
	}
	
	return _chains[index];
}

void DeviceChain_Controller::createChain(uint8_t index, ChainType_t type, uint8_t deviceIndexes[])
{
	if(index > MAX_CHAINS - 1)
	{
		_debug.println(F("Can't add chain at index %d"), index);
		_debug.debugln(5, F("Max chain index is %d"), MAX_CHAINS - 1);
		return;
	}

	if(_chains[index] != NULL)
	{		
		_debug.println(F("Chain already exists at index %d"), index);
		return;
	}
	
	DeviceChain_Base *newChain;
	
	switch(type)
	{
		case Direct:
			newChain = new DeviceChain_Direct(); break;			
		case FirstAvailable:
			newChain = new DeviceChain_FA(); break;			
		case RoundRobin:
			newChain = new DeviceChain_RR(); break;			
		default: 
			newChain = new DeviceChain_Base(); break;
	}
	
	uint8_t numDevices = sizeof(deviceIndexes) / sizeof(uint8_t);
	_debug.debugln(5, F("Attempting to add %d device(s)"), numDevices);
	
	int i = 0;
	while(i != MAX_DEVICES)
		newChain->addDevice(MDC.getDevice(deviceIndexes[i++]));
	
	addChain(index, newChain);
}

void DeviceChain_Controller::deleteChain(uint8_t index)
{
	if(index > MAX_CHAINS - 1)
	{
		_debug.debugln(3, F("Max index is %d"), MAX_CHAINS - 1);
		return;
	}
	
	if(_chains[index])
	{
		_debug.debugln(2, F("Deleting chain at %d"), index);
		delete _chains[index];
		_chains[index] = NULL;
		return;
	}
	else
	{
		_debug.debugln(2, F("No chain at %d"), index);
	}
}

void DeviceChain_Controller::assignNote(int8_t index, uint8_t note)
{	
	DeviceChain_Base *c = getChain(index);
	if(!c) return;
	c->assignNote(note);
}

void DeviceChain_Controller::clearNote(int8_t index, uint8_t note)
{
	DeviceChain_Base *c = getChain(index);
	if(!c) return;
	c->clearNote(note);
}

void DeviceChain_Controller::pitchBend(int8_t index, uint16_t bend)
{
	DeviceChain_Base *c = getChain(index);
	if(!c) return;
	c->pitchBend(bend);
}

//Settings
//_______________________________________________________________________________________________________

//Tests/Debug
//_______________________________________________________________________________________________________
