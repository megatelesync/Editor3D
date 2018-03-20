#include "../../h/WindowUtil/WindowUtil_Master.h"
#include "boost/assert.hpp"

namespace Dv
{
	namespace Util
	{		
		/************************************************************************************
		* Main loop
		************************************************************************************/
		MainLoop::MainLoop(HWND hInWnd) :
			_hWnd(hInWnd)
		{
		}

		void MainLoop::Initialize(HWND hInWnd)
		{
			_hWnd = hInWnd;
			_bQuit = false;
			UserInitialize();
		}

		void MainLoop::Tick()
		{
			ZeroMemory(&_msg, sizeof(_msg));
			BOOL bMessagePeeked = PeekMessage(&_msg, nullptr, 0, 0, PM_REMOVE);
			if(_msg.message == WM_QUIT)
			{
				_bQuit = true;
				return;
			}
			if (bMessagePeeked)
			{
				TranslateMessage(&_msg);
				DispatchMessage(&_msg);
			}
			UserTick();
		}

		bool MainLoop::IsEndOfLoop() const
		{
			return _bQuit == true;
		}

		void InitWindow_AndMainLoop
		(
			MainLoop *pMainLoop,
			const char *windowClass,
			const char *title,
			UINT width, UINT height,
			WNDPROC wndProc,
			HINSTANCE hInstance
		)		
		{
			BOOST_ASSERT_MSG(pMainLoop, "Main loop pointer may NOT be nullptr");
			BOOST_ASSERT_MSG(windowClass, "Window class pointer may NO be nullptr");
			BOOST_ASSERT_MSG(title, "title pointer may NOT be nullptr");
			BOOST_ASSERT_MSG(wndProc, "window proc pointer may NOT be nullptr");

			auto hWnd = RegisterClasAndCreateWindow
			(
				hInstance, windowClass, wndProc, title, width, height
			);
			ShowWindow(hWnd, SW_SHOW);
			UpdateWindow(hWnd);
			pMainLoop->Initialize(hWnd);
		}

		void InitWindow_AndRunMainLoop
		(
			MainLoop *pMainLoop,
			const char *windowClass,
			const char *title,
			UINT width, UINT height,
			WNDPROC wndProc,
			HINSTANCE hInstance
		)
		{
			InitWindow_AndMainLoop(pMainLoop, windowClass, title, width, height, wndProc, hInstance);
			RunMainLoop(pMainLoop);
		}

		void RunMainLoop(MainLoop *pInLoop)
		{
			do
			{
				pInLoop->Tick();
			} while (false == pInLoop->IsEndOfLoop());
		}

		void TickMainLoop_NTimes_OrUntilEnd(MainLoop *pInLoop, int InNumTicks)
		{
			BOOST_ASSERT(InNumTicks >= 0);
			if(InNumTicks == 0)
			{
				RunMainLoop(pInLoop);
			}
			else
			{
				TickMainLoop_NTimes(pInLoop, InNumTicks);
			}
		}
		void TickMainLoop_NTimes(MainLoop *pInLoop, int InNumTicks)
		{
			BOOST_ASSERT(pInLoop);
			BOOST_ASSERT(InNumTicks > 0);
			for(int i = 0; i < InNumTicks; i++)
			{
				pInLoop->Tick();
			}
		}

		/************************************************************************************
		* Util functions
		************************************************************************************/
		HWND RegisterClasAndCreateWindow
		(
			HINSTANCE hInstance,
			const char *windowClass, 
			WNDPROC wndProc, 
			const char *title, 
			UINT width, UINT height
		)
		{
			WNDCLASSEX wc;
			ZeroMemory(&wc, sizeof(wc));
			wc.cbSize                                                       = sizeof(wc);
			wc.lpszClassName                                                = windowClass;
			wc.lpszMenuName                                                 = nullptr;
			wc.hbrBackground                                                = (HBRUSH) COLOR_WINDOW;
			wc.hCursor                                                      = LoadCursor(nullptr, IDC_ARROW);
			wc.hIcon                                                        = nullptr;
			wc.lpfnWndProc                                                  = wndProc;
			RegisterClassEx(&wc);

			HWND hWnd = CreateWindowEx
			(
				0,  // ex style
				windowClass, title, WS_OVERLAPPEDWINDOW, 
				0, 0, width, height, 
				0, 0, hInstance, 0
			);
			BOOST_ASSERT_MSG(hWnd, "Error: CreateWindowEx returned NULL window handle");
			return hWnd;
		}
	} // Util
} // Dv