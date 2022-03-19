// main.cpp : Defines the entry point for the DLL application.
#include "..\common\pch.h"

#include "AddObjFactory.h"
#include "AddObject_h.h"
#include "AddObject_i.c"
#include "utilities.h"

#define DEBUG_LOGGER_ENABLED
#define FILE_LOGGER_ENABLED
#define LOG_PREFIX "[ADDOBJ-WINMAIN]"
#include "logger.h"

// global counter for locks & active objects.
long g_nComObjectsInUse = 0;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	LOG("Entering, hInstance: 0x%p, lpCmdLine: %s, nCmdShow: %d", hInstance, lpCmdLine, nCmdShow);

	WCHAR wsMessageBuffer[MAX_PATH] = { 0 };

	HRESULT hr = CoInitialize(nullptr);
	ErrorDescription(hr, wsMessageBuffer, _countof(wsMessageBuffer));
	LOG("CoInitialize() returned 0x%08x: %ws", hr, wsMessageBuffer);
	if (FAILED(hr))
		exit(-1);

	// TODO: elaborate
	// Let's register the type lib to get the 'free' type library marsahler.
	//ITypeLib* pTypeLib = nullptr;
	//hr = LoadTypeLibEx(L"IAdd.tlb", REGKIND_REGISTER, &pTypeLib);
	//pTypeLib->Release();
	//ErrorDescription(hr, wsMessageBuffer, _countof(wsMessageBuffer));
	//LOG("LoadTypeLibEx() returned 0x%08x: %ws", hr, wsMessageBuffer);
	//if (FAILED(hr)) {
	//	CoUninitialize();
	//	exit(-2);
	//}

	// Let's see if we were started by SCM.
	if (strstr(lpCmdLine, "/Embedding") || strstr(lpCmdLine, "-Embedding"))
	{
		// for debug
		MessageBox(nullptr, L"AddObj Server[LOCAL] is registering the classes", L"[LOCAL]EXE Message!", MB_OK | MB_SETFOREGROUND);

		// Create the AddObj class object.
		CAddObjFactory addObjFactory;

		// Register the AddObj Factory.
		DWORD regId = 0;
		hr = CoRegisterClassObject(CLSID_AddObject, (IClassFactory*)&addObjFactory, CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE, &regId);
		if (FAILED(hr))
		{
			ErrorDescription(hr, wsMessageBuffer, _countof(wsMessageBuffer));
			LOG("CoRegisterClassObject() returned 0x%08x: %ws", hr, wsMessageBuffer);
			CoUninitialize();
			exit(-3);
		}

		// Now just run until a quit message is sent,
		// in responce to the final release.
		MSG message = { 0 };
		while (GetMessage(&message, 0, 0, 0))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		// All done, so remove class object.
		hr = CoRevokeClassObject(regId);
		ErrorDescription(hr, wsMessageBuffer, _countof(wsMessageBuffer));
		LOG("CoRevokeClassObject() returned 0x%08x: %ws", hr, wsMessageBuffer);
	}

	// Terminate COM.
	CoUninitialize();
	MessageBox(nullptr, L"[LOCAL] Server is dying", L"[LOCAL]EXE Message!", MB_OK | MB_SETFOREGROUND);
	return 0;
}
