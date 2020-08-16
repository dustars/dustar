/*
Class:Window
Author:Rich Davison
Description:Creates and handles the Window, including the initialisation of the mouse and keyboard.

Last Updated:
7/31/2020: Add the ImGui messange handler function for win32 into the class to be able to get ImGui work.
*/
#pragma once
#pragma warning( disable : 4099 )

#include <string>

#include <windows.h>
#include <io.h>
#include <stdio.h>
#include <fcntl.h>

#include "RenderBase.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "GameTimer.h"

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define WINDOWCLASS "WindowClass"

class RenderBase;

class Window	{
public:
	Window(std::string title = "OpenGL Framework", int sizeX = 800, int sizeY = 600, bool fullScreen = false);
	~Window(void);

	bool	UpdateWindow();	

	void	SetRenderer(RenderBase* r);

	HWND	GetHandle();

	bool	HasInitialised();

	void	LockMouseToWindow(bool lock);
	void	ShowOSPointer(bool show);

	Vector2	GetScreenSize() {return size;};

	static Keyboard*	GetKeyboard()	{return keyboard;}
	static Mouse*		GetMouse()		{return mouse;}

	GameTimer*   GetTimer()		{return timer;}

protected:
	void	CheckMessages(MSG &msg);
	static LRESULT CALLBACK WindowProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

	HWND			windowHandle;

	static Window*		window;
	static Keyboard*	keyboard;
	static Mouse*		mouse;

	GameTimer*	timer;

	RenderBase*		renderer;

	bool				forceQuit;
	bool				init;
	bool				fullScreen;
	bool				lockMouse;
	bool				showMouse;

	Vector2				position;
	Vector2				size;

	float				elapsedMS;

	bool				mouseLeftWindow;
};