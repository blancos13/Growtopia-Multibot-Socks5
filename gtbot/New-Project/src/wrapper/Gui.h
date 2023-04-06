#pragma once

#include "Font.h"
#include <d3d9.h>

#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_impl_dx9.h"
#include "../vendor/imgui/imgui_impl_win32.h"

#include <chrono>
#include <thread>
#include <Windows.h>

namespace Gui {
	
	bool Instance = true;
	
	HWND window = nullptr;
	WNDCLASSEX windowClass{0};
	
	PDIRECT3D9 d3d;
	LPDIRECT3DDEVICE9 device;
	D3DPRESENT_PARAMETERS presentParameters{0};
	
	MSG message{0};
	
	void CreateHWindow(LPCWSTR windowName);
	void DestroyHWindow();
	
	bool CreateDevice();
	void ResetDevice();
	void DestroyDevice();
	
	void CreateImGui();
	void DestroyImGui();

	bool BeginRender();
	void EndRender();
};



extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;
	
	switch (msg) {
		case WM_SIZE:
			if (Gui::device != 0 && wParam != SIZE_MINIMIZED) {
				Gui::presentParameters.BackBufferWidth = LOWORD(lParam);
				Gui::presentParameters.BackBufferHeight = HIWORD(lParam);
				Gui::ResetDevice();
			}
			return 0;
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU)
				return 0;
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
    }
	
    return DefWindowProc(hWnd, msg, wParam, lParam);
}



void Gui::CreateHWindow(LPCWSTR windowName) {
	
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hIcon = 0;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = 0;
	windowClass.lpszMenuName = 0;
	windowClass.lpszClassName = windowName;
	windowClass.hIconSm = 0;
	
	/*
	windowClass = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0L,
		0L,
		GetModuleHandle(0),
		0,
		0,
		0,
		0,
		windowName,
		0
	};
	*/
	
	RegisterClassEx(&windowClass);
	window = CreateWindow(windowClass.lpszClassName, windowClass.lpszClassName, WS_POPUP, 0, 0, 5, 5, 0, 0, windowClass.hInstance, 0);
}

void Gui::DestroyHWindow() {
	DestroyWindow(window);
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}




bool Gui::CreateDevice() {
	if (!(d3d = Direct3DCreate9(D3D_SDK_VERSION)))
		return false;
	
	presentParameters.Windowed = TRUE;
	presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
	presentParameters.EnableAutoDepthStencil = TRUE;
	presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &presentParameters, &device) < 0)
		return false;
	
	ShowWindow(window, SW_HIDE);
	UpdateWindow(window);
	return true;
}

void Gui::ResetDevice() {
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT result = device->Reset(&presentParameters);
	if (result == D3DERR_INVALIDCALL)
		IM_ASSERT(0);
	ImGui_ImplDX9_CreateDeviceObjects();
}

void Gui::DestroyDevice() {
	if (device) {
		device->Release();
		device = nullptr;
	}

	if (d3d) {
		d3d->Release();
		d3d = nullptr;
	}
}



void Gui::CreateImGui() {
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	
	
	ImFontConfig font_cfg;
	font_cfg.FontDataOwnedByAtlas = false;
	
	ImGuiIO& io = ::ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.IniFilename = nullptr;
	io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Custom), sizeof(Custom), 22.4f, &font_cfg);
	io.Fonts->AddFontDefault();
	
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
}

void Gui::DestroyImGui() {
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}




bool Gui::BeginRender() {
	if (PeekMessage(&message, 0, 0U, 0U, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
		return true;
	}
	
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	return false;
}

void Gui::EndRender() {
	ImGui::EndFrame();
	
	//device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);
    device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	
	if (device->BeginScene() >= 0) {
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}
	
	// Using multi viewports
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
	
	if (device->Present(0, 0, 0, 0) == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		ResetDevice();
}
