#include <dxgi.h>
#include <d3d11.h>
#include "kiero.hpp"
#include "kiero_d3d11.hpp"
#include "kiero_intern.hpp"


kiero::Error kiero::locate2()
{
  IDXGIFactory* factory;
  auto hresult = CreateDXGIFactory(
    __uuidof(IDXGIFactory),
    (void**)&factory
  );
  if (hresult != S_OK) {
    KIERO_DBG_MSG("CreateDXGIFactory failed (%d)", hresult);
    return Error_D3D11_CreateDXGIFactoryFailed;
  }
  KIERO_DEFER([&]() { factory->Release(); });

  kiero::_::DummyWin32Window window;
  kiero::_::create_dummy_win32_window(&window);
  KIERO_DEFER([&]() { kiero::_::destroy_dummy_win32_window(&window); });

  DXGI_SWAP_CHAIN_DESC sc_desc = {};
  sc_desc.BufferDesc.Width = 100;
  sc_desc.BufferDesc.Height = 100;
  sc_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sc_desc.BufferDesc.RefreshRate.Numerator = 60;
  sc_desc.BufferDesc.RefreshRate.Denominator = 1;
  sc_desc.SampleDesc.Count = 1;
  sc_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sc_desc.BufferCount = 1;
  sc_desc.OutputWindow = window.hwnd;
  sc_desc.Windowed = TRUE;
  sc_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
  sc_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

  const D3D_FEATURE_LEVEL feature_levels[] = {
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
  };

  UINT count = 0;
  IDXGIAdapter* adapter = nullptr;
  while (factory->EnumAdapters(count, &adapter) != DXGI_ERROR_NOT_FOUND)
  {
    IDXGISwapChain* swapchain;
    ID3D11Device* device;
    ID3D11DeviceContext* context;
    D3D_FEATURE_LEVEL feature_level;
  
    hresult = D3D11CreateDeviceAndSwapChain(
      adapter,
      D3D_DRIVER_TYPE_UNKNOWN,
      nullptr,
      0,
      feature_levels,
      ARRAYSIZE(feature_levels),
      D3D11_SDK_VERSION,
      &sc_desc,
      &swapchain,
      &device,
      &feature_level,
      &context
    );
    if (hresult != S_OK) {
      KIERO_DBG_MSG("D3D11CreateDeviceAndSwapChain failed (%d)", hresult);
      return Error_D3D11_CreateDeviceAndSwapChainFailed;
    }

    printf("\nadapter: %u\n", count);
    
    for (auto vtable = *(void***)swapchain; vtable; vtable++) {
      auto ptr = *vtable;
      if (!ptr) break;
      //output->swapchain_methods.push_back(ptr);
      printf("swapchain: %p\n", ptr);
    }
  
    for (auto vtable = *(void***)device; vtable; vtable++) {
      auto ptr = *vtable;
      if (!ptr) break;
      //output->device_methods.push_back(ptr);
      printf("device: %p\n", ptr);
    }
  
    for (auto vtable = *(void***)context; vtable; vtable++) {
      auto ptr = *vtable;
      if (!ptr) break;
      //output->context_methods.push_back(ptr);
      printf("context: %p\n", ptr);
    }

    swapchain->Release();
    device->Release();
    context->Release();
    adapter->Release();
    count++;
  }
  return count;
}
