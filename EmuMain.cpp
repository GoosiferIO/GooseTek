#include "EmuMain.h"
#include "detours.h"
#include "EmuControls.h"
#include "ZTCheats.h"
#include "ZooState.h"

#define instance EmuMain::shared_instance()

// #define instance 

//------ Function pointers 
DWORD updateAddress = 0x41a16b; // 0x00401644; // address of update function in game
typedef void(__thiscall* _origUpdate)(void* thisptr); // define original update function


EmuMain::EmuMain()
{
	this->IsConsoleRunning = false;
	this->hasEmuRunOnce = false;
	this->CommandIsProcessing = false;
	this->HasConsoleOpenedOnce = false;
	this->ctrlMPressed = false;
	this->hasHooked = false;
	this->zoo_models = new ZooModels();
	this->console = new EmuConsole(this->tokens);
}

void EmuMain::init()
{
	instance.emu_run.InitEmuAPI();

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach((PVOID*)&updateAddress, (PVOID)&RunEmu);
	DetourTransactionCommit();
}

DWORD WINAPI EmuMain::ZooConsole()
{
	EmuMain::shared_instance().HasConsoleOpenedOnce = true;

	HWND consoleWindow = EmuConsole::createConsole();

	while (instance.IsConsoleRunning)
	{
		// ------ Tokenize the input
		if (!instance.CommandIsProcessing)
		{
			instance.console->WriteToConsole(">> ");
			instance.CommandIsProcessing = true; // set flag to true to avoid multiple commands being processed at once
			instance.console->tokenize(instance.CommandIsProcessing);
		}
		Sleep(10);
	}

	// ------ Close the console window
	instance.HasConsoleOpenedOnce = false;
	instance.IsConsoleRunning = false;
	FreeConsole();
	Sleep(100);
	PostMessage(consoleWindow, WM_CLOSE, 0, 0);
	return 1;
}

void __fastcall EmuMain::RunEmu(void* thisptr) {

	// main loop

	EmuControls::procControls(); // process controls

	// if (!EmuMain::shared_instance().hasHooked) {
	// 	EmuControls::InitializeHook(); // initialize mouse hook

	// 	EmuMain::shared_instance().hasHooked = true;
	// }

	ZTCheats::InvisibleInvincibleCheat(); // run cheats

	// only run scripts while zoo is loaded and not in main menu
	if ((int)ZooState::object_ptr(0x0) > 0) {
		// f << "[" << timestamp << "] " << "Is no longer in main menu!" << std::endl;
		if (ZooState::IsZooLoaded() == true) {
			// f << "[" << timestamp << "] " << "Zoo is loaded!" << std::endl;
			if (!instance.hasEmuRunOnce) {
				// f << "[" << timestamp << "] " << "Running emu_run scripts..." << std::endl;
				instance.emu_gawk.ExecuteScripts("emu_gawk");
				instance.hasEmuRunOnce = true;
				// f << "[" << timestamp << "] " << "Scripts executed!" << std::endl;
			}
			instance.emu_run.ExecuteScripts("emu_run");
			// f << "[" << timestamp << "] " << "Scripts executed!" << std::endl;
		}
	}

	if (ZooState::IsZooLoaded() == false) {
		EmuMain::shared_instance().hasEmuRunOnce = false;

	}

	//---- Process the input tokens while console is running
	if (!EmuMain::shared_instance().tokens.empty() && EmuMain::shared_instance().IsConsoleRunning == true)
	{
		EmuMain::shared_instance().console->processInput(EmuMain::shared_instance().IsConsoleRunning);
		EmuMain::shared_instance().tokens.clear();
		EmuMain::shared_instance().CommandIsProcessing = false; // reset flag to allow another command to be tokenized
	}

	//---- return to original update function
	_origUpdate ogUpdate = (_origUpdate)updateAddress;
	// f << "[" << timestamp << "] " << "EMU loop finished!" << std::endl;

	ogUpdate(thisptr);

	// f << "[" << timestamp << "] " << "ogUpdate(thisptr) called!" << std::endl;
}