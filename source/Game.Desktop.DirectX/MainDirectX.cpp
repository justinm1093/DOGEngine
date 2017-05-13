
#include "pch.h"

#include <d3d11.h>			// SimpleMath.h needs this before its own #include
#include "SimpleMath.h"


LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	// based on the value of message, handle things differently
	switch(message)
	{
		// read when the window is closed
		case WM_DESTROY:
			PostQuitMessage(0);
			result = 0;
			break;

		// default message handling behaviors
		default:
			result = DefWindowProc(windowHandle, message, wParam, lParam);
			break;
	}

	return result;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// avoids unreferenced param warning which would cause compilation error
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// create handle to window and window class
	HWND windowHandle;
	WNDCLASSEX windowClass;

	// clear window class memory and set its data, then register the window class
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	windowClass.lpszClassName = L"WindowClass";
	RegisterClassEx(&windowClass);

	windowHandle = CreateWindowEx(
		NULL,					
		L"WindowClass",			// window class name
		L"DirectX Window",		// window name
		WS_OVERLAPPEDWINDOW,	// window style
		300,					// x position
		300,					// y position
		500,					// width
		400,					// height
		NULL,					// parent window
		NULL,					// menu
		hInstance,				// application handle
		NULL);					// use with multiple windows

	// display window on screen
	ShowWindow(windowHandle, nCmdShow);

	// enter program event loop
	MSG message;
	while(GetMessage(&message, NULL, 0, 0))
	{
		// translates keystroke messages to proper format
		TranslateMessage(&message);

		// send message to WindowProc function
		DispatchMessage(&message);
	}

	return (int)message.wParam;
}
