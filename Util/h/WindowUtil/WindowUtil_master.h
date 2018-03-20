#ifndef _WindowUtil_master_h_
#define _WindowUtil_master_h_

#include <Windows.h>

namespace Dv
{
	namespace Util
	{
		HWND RegisterClasAndCreateWindow
		(
			HINSTANCE hInstance,
			const char *windowClass, 
			WNDPROC wndProc, 
			const char *title, 
			UINT width, UINT height
		);

		/*****************************************************************************************
		* MainLoop class (typically - for test pruproses)
		*****************************************************************************************/
		class MainLoop
		{
		public:
			virtual ~MainLoop() {}

			// When used with default ctor, the window handle must be explicitly provided, using Initialize()
			MainLoop() = default;
			MainLoop(HWND hInWnd);

			// Initialize: Use only if default ctor used:
			void Initialize                                (HWND hInWnd);

			void Tick                                      ();
			bool                                           IsEndOfLoop() const;

			const MSG&                           GetMsg    () const                 { return _msg; }

			HWND                                 GetWND    () const                 { return _hWnd; }

		protected:
			virtual void                        UserTick() {};
			virtual void                        UserInitialize() {}

		private:
			MSG                                 _msg;
			HWND                                _hWnd;
			bool                                _bQuit = false;
		};

		/**********************************************************************************************
		* Inits window (creates, show) and runs the given main loop
		*
		* WARNING!!! Main loop must be created with default ctor
		* (i.e. using ctor that does NOT takes window handle as an argument)
		**********************************************************************************************/
		void InitWindow_AndRunMainLoop
		(
			MainLoop *pMainLoop,
			const char *windowClass,
			const char *title,
			UINT width = 1024, UINT height = 768,
			WNDPROC wndProc = &DefWindowProc,
			HINSTANCE hInstance = NULL
		);
		void InitWindow_AndMainLoop
		(
			MainLoop *pMainLoop,
			const char *windowClass,
			const char *title,
			UINT width = 1024, UINT height = 768,
			WNDPROC wndProc = &DefWindowProc,
			HINSTANCE hInstance = NULL
		);
		void RunMainLoop(MainLoop *pInLoop);

		// Just call the Tick() function of the main loop N times
		// (provided for testing purposes)
		void TickMainLoop_NTimes(MainLoop *pInLoop, int InNumTicks);

		// If num ticks is zero, call tick the normal way (until end of loop);
		void TickMainLoop_NTimes_OrUntilEnd(MainLoop *pInLoop, int InNumTicks);
	} // Util
} // Dv

#endif // _WindowUtil_master_h_