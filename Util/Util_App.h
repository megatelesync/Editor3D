#ifndef _Util_App_h_
#define _Util_App_h_

#include <string>
#include <windows.h>

namespace Dv
{
	namespace Util
	{
		struct App
		{
			// apllication instance handle
			HINSTANCE					hInstance;
			// main window
			HWND						hWndMain;
			// Cmd line where the apllication was launched with
			std::string					launchCmdLine;			

			App() {}
			App
			(
				HINSTANCE hInstanceIn, 
				HWND hWndMainIn, 
				const std::string& launchCmdLineIn
			) : 
				hInstance(hInstanceIn),
				hWndMain(hWndMainIn),
				launchCmdLine(launchCmdLineIn)
			{
			}
		};
	} // Util
} // Dv

#endif // _Util_App_h_