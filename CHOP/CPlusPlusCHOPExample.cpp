/* Shared Use License: This file is owned by Derivative Inc. (Derivative) and
 * can only be used, and/or modified for use, in conjunction with 
 * Derivative's TouchDesigner software, and only if you are a licensee who has
 * accepted Derivative's TouchDesigner license or assignment agreement (which
 * also govern the use of this file).  You may share a modified version of this
 * file with another authorized licensee of Derivative's TouchDesigner software.
 * Otherwise, no redistribution or sharing of this file, with or without
 * modification, is permitted.
 */

#include "CPlusPlusCHOPExample.h"
#include "../Include/LogitechLEDLib.h"

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <assert.h>
#include <string>

// These functions are basic C function, which the DLL loader can find
// much easier than finding a C++ Class.
// The DLLEXPORT prefix is needed so the compile exports these functions from the .dll
// you are creating
extern "C"
{

DLLEXPORT
void
FillCHOPPluginInfo(CHOP_PluginInfo *info)
{
	// Always set this to CHOPCPlusPlusAPIVersion.
	info->apiVersion = CHOPCPlusPlusAPIVersion;

	// The opType is the unique name for this CHOP. It must start with a 
	// capital A-Z character, and all the following characters must lower case
	// or numbers (a-z, 0-9)
	info->customOPInfo.opType->setString("Customsignal");

	// The opLabel is the text that will show up in the OP Create Dialog
	info->customOPInfo.opLabel->setString("Custom Signal");

	// Information about the author of this OP
	info->customOPInfo.authorName->setString("Author Name");
	info->customOPInfo.authorEmail->setString("email@email.com");

	// This CHOP can work with 0 inputs
	info->customOPInfo.minInputs = 0;

	// It can accept up to 1 input though, which changes it's behavior
	info->customOPInfo.maxInputs = 1;
}

DLLEXPORT
CHOP_CPlusPlusBase*
CreateCHOPInstance(const OP_NodeInfo* info)
{
	LogiLedInit();
	// Return a new instance of your class every time this is called.
	// It will be called once per CHOP that is using the .dll
	return new CPlusPlusCHOPExample(info);
}

DLLEXPORT
void
DestroyCHOPInstance(CHOP_CPlusPlusBase* instance)
{
	LogiLedShutdown();
	// Delete the instance here, this will be called when
	// Touch is shutting down, when the CHOP using that instance is deleted, or
	// if the CHOP loads a different DLL
	delete (CPlusPlusCHOPExample*)instance;
}

};


CPlusPlusCHOPExample::CPlusPlusCHOPExample(const OP_NodeInfo* info) : myNodeInfo(info)
{

}

CPlusPlusCHOPExample::~CPlusPlusCHOPExample()
{

}

void
CPlusPlusCHOPExample::getGeneralInfo(CHOP_GeneralInfo* ginfo, const OP_Inputs* inputs, void* reserved1)
{
	ginfo->cookEveryFrameIfAsked = false;

	ginfo->timeslice = false;

	ginfo->inputMatchIndex = 0;
}

bool
CPlusPlusCHOPExample::getOutputInfo(CHOP_OutputInfo* info, const OP_Inputs* inputs, void* reserved1)
{

	info->numChannels = 3;
	info->sampleRate = 120;
	return true;

}

void
CPlusPlusCHOPExample::getChannelName(int32_t index, OP_String *name, const OP_Inputs* inputs, void* reserved1)
{
	if (index == 0) {
		name->setString("LED_Red");
	}
	else if (index == 1) {
		name->setString("LED_Green");
	}
	else if (index == 2) {
		name->setString("LED_Blue");
	}
}

void
CPlusPlusCHOPExample::execute(CHOP_Output* output,
							  const OP_Inputs* inputs,
							  void* reserved)
{
	//TODO- possibly enable/disable these when not using them.
	inputs->enablePar("Allzonesrgb", 1);						// enable / disable parameters 
	inputs->enablePar("Zone1", 1);
	inputs->enablePar("Zone2", 1);
	inputs->enablePar("Zone3", 1);
	inputs->enablePar("Zone4", 1);

	int LED_RGB[3];													// This will store the RGB data in 0-100 range
	double RGBParam[3];												// This holds the RGB data in 0-1. range

	if (inputs->getParInt("Allzones") == 1.) {
		


		for (int i = 0; i < 3; i++) {
			RGBParam[i] = inputs->getParDouble("Allzonesrgb", i);		//Retrieve the RGB parameter values
			LED_RGB[i] = int(RGBParam[i] * 100);					//The API requests percentages in the range 0-100
			output->channels[i][0] = RGBParam[i];					// We will just output 3 rgb channels for now. TODO- fix this
		}

		LogiLedSetLighting(LED_RGB[0], LED_RGB[1], LED_RGB[2]);		 // call this if you don't want to set zone lighting

	}

	else if (inputs->getParInt("Allzones") == 0.) {


		// -----  Zone Lighting ------

		for (int zone=0; zone < inputs->getParInt("Zonecount"); zone++) {	
			// loop through the number of zones we have on our device
			// default count is 4.

			std::string s = "Zone" + std::to_string(zone+1);	//parameters we are working with are Zone1, Zone2, ect...
			char const *zoneStr = s.c_str();					//Concatenate Zone and int

			for (int i = 0; i < 3; i++) {
				
				RGBParam[i] = inputs->getParDouble(zoneStr, i);				//Retrieve the RGB parameter values
					LED_RGB[i] = int(RGBParam[i] * 100);					//The API requests percentages in the range 0-100
					output->channels[i][0] = RGBParam[i];					//We will output this even though we won't really use it
			
			};
			
			//todo add if else statements for other types of devices
			LogiLedSetLightingForTargetZone(LogiLed::DeviceType::Speaker, zone, LED_RGB[0], LED_RGB[1], LED_RGB[2]);
		}

	}

}

int32_t
CPlusPlusCHOPExample::getNumInfoCHOPChans(void * reserved1)
{
	return 0;
}

void
CPlusPlusCHOPExample::getInfoCHOPChan(int32_t index,
										OP_InfoCHOPChan* chan,
										void* reserved1)
{
}

bool		
CPlusPlusCHOPExample::getInfoDATSize(OP_InfoDATSize* infoSize, void* reserved1)
{

	return true;
}

void
CPlusPlusCHOPExample::getInfoDATEntries(int32_t index,
										int32_t nEntries,
										OP_InfoDATEntries* entries, 
										void* reserved1)
{
}

void
CPlusPlusCHOPExample::setupParameters(OP_ParameterManager* manager, void *reserved1)
{


	// Toggle All Zone Control
	{
		OP_NumericParameter	np;

		np.name = "Allzones";
		np.label = "Control All Zones";
		np.defaultValues[0] = 1;
		np.page = "All_Zones";

		OP_ParAppendResult res = manager->appendToggle(np);
		assert(res == OP_ParAppendResult::Success);
	}

	// All Zones RGB
	{
		OP_NumericParameter	np;

		np.name = "Allzonesrgb";
		np.label = "All Zones RGB";
		np.defaultValues[0] = .30;
		np.defaultValues[1] = .60;
		np.defaultValues[2] = .90;
		np.page = "All_Zones";

		for (int i = 0; i < 3; i++)
		{
			np.minValues[i] = 0.;
			np.maxValues[i] = 1.;
			np.minSliders[i] = 0.;
			np.maxSliders[i] = 1.;
			np.clampMins[i] = true;
			np.clampMaxes[i] = true;
		}
		OP_ParAppendResult res = manager->appendRGB(np);
		assert(res == OP_ParAppendResult::Success);
	}

	//Number of zones
	{
		OP_NumericParameter	np;

		np.name = "Zonecount";
		np.label = "Zone Count";
		np.defaultValues[0] = 4; //Default for the G560
		np.page = "Zones";

		OP_ParAppendResult res = manager->appendInt(np);
		assert(res == OP_ParAppendResult::Success);
	}

	// Device Type Menu -- Doesn't really do anything right now.
	{
		OP_StringParameter	sp;

		sp.name = "Devicetype";
		sp.label = "Device Type";
		sp.defaultValue = "Speaker"; //Default for the G560
		sp.page = "Zones";
		
		const char *names[] = { "Keyboard", "Mouse", "Mousemat", "Headset", "Speaker" };
		const char *labels[] = { "Keyboard", "Mouse", "Mousemat", "Headset", "Speaker" };

		OP_ParAppendResult res = manager->appendMenu(sp, 5, names, labels);


		assert(res == OP_ParAppendResult::Success);
	}


	// Zone 1 RGB Color
	{
		OP_NumericParameter	np;

		np.name = "Zone1";
		np.label = "Zone 1 RGB";
		np.defaultValues[0] = .30;
		np.defaultValues[1] = .60;
		np.defaultValues[2] = .90;
		np.page = "Zones";

		for (int i = 0; i < 3; i++)
		{
			np.minValues[i] = 0.;
			np.maxValues[i] = 1.;
			np.minSliders[i] = 0.;
			np.maxSliders[i] = 1.;
			np.clampMins[i] = true;
			np.clampMaxes[i] = true;
		}
		OP_ParAppendResult res = manager->appendRGB(np);
		assert(res == OP_ParAppendResult::Success);
	}

	// Zone 2 RGB Color
	{
		OP_NumericParameter	np;

		np.name = "Zone2";
		np.label = "Zone 2 RGB";
		np.defaultValues[0] = .30;
		np.defaultValues[1] = .60;
		np.defaultValues[2] = .90;
		np.page = "Zones";

		for (int i = 0; i < 3; i++)
		{
			np.minValues[i] = 0.;
			np.maxValues[i] = 1.;
			np.minSliders[i] = 0.;
			np.maxSliders[i] = 1.;
			np.clampMins[i] = true;
			np.clampMaxes[i] = true;
		}
		OP_ParAppendResult res = manager->appendRGB(np);
		assert(res == OP_ParAppendResult::Success);
	}

	// Zone 3 RGB Color
	{
		OP_NumericParameter	np;

		np.name = "Zone3";
		np.label = "Zone 3 RGB";
		np.defaultValues[0] = .30;
		np.defaultValues[1] = .60;
		np.defaultValues[2] = .90;
		np.page = "Zones";

		for (int i = 0; i < 3; i++)
		{
			np.minValues[i] = 0.;
			np.maxValues[i] = 1.;
			np.minSliders[i] = 0.;
			np.maxSliders[i] = 1.;
			np.clampMins[i] = true;
			np.clampMaxes[i] = true;
		}
		OP_ParAppendResult res = manager->appendRGB(np);
		assert(res == OP_ParAppendResult::Success);
	}

	// Zone 4 RGB Color
	{
		OP_NumericParameter	np;

		np.name = "Zone4";
		np.label = "Zone 4 RGB";
		np.defaultValues[0] = .30;
		np.defaultValues[1] = .60;
		np.defaultValues[2] = .90;
		np.page = "Zones";

		for (int i = 0; i < 3; i++)
		{
			np.minValues[i] = 0.;
			np.maxValues[i] = 1.;
			np.minSliders[i] = 0.;
			np.maxSliders[i] = 1.;
			np.clampMins[i] = true;
			np.clampMaxes[i] = true;
		}
		OP_ParAppendResult res = manager->appendRGB(np);
		assert(res == OP_ParAppendResult::Success);
	}






}

void 
CPlusPlusCHOPExample::pulsePressed(const char* name, void* reserved1)
{
	//if (!strcmp(name, "Reset"))
	//{
	//	myOffset = 0.0;
	//}
}

