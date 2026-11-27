// DXVK native on MoltenVK, without P2CE (round 28, plan round F): an SDL
// window, a Direct3D 11 device and swap chain through libdxvk_d3d11.dylib,
// a clear to a colour, a present, then the back buffer read back through a
// staging texture to check the colour arrived. make p2ce-dxvk-clear builds
// and runs it (needs deps/dxvk-macos-ref's build and MoltenVK; not part of
// make check). Prints "clear OK" and exits 0 when the pixel matches.
#include <windows.h>
#include <d3d11.h>
#include <SDL.h>
#include <SDL_vulkan.h>
#include <cstdio>
#include <cstdlib>

static int fail(const char *what, HRESULT hr) {
    std::fprintf(stderr, "dxvk-clear: %s failed (0x%08x)\n", what, (unsigned)hr);
    return 1;
}

int main(int argc, char **argv) {
    int seconds = argc > 1 ? std::atoi(argv[1]) : 1;
    if (SDL_Init(SDL_INIT_VIDEO) != 0) { std::fprintf(stderr, "SDL_Init: %s\n", SDL_GetError()); return 1; }
    const char *vk = std::getenv("SDL_VULKAN_LIBRARY");
    if (SDL_Vulkan_LoadLibrary(vk) != 0) { std::fprintf(stderr, "SDL_Vulkan_LoadLibrary(%s): %s\n", vk ? vk : "default", SDL_GetError()); return 1; }
    SDL_Window *window = SDL_CreateWindow("DXVK clear", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360,
                                          SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);
    if (!window) { std::fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError()); return 1; }

    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferDesc.Width = 640;
    sd.BufferDesc.Height = 360;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.SampleDesc.Count = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 2;
    sd.OutputWindow = reinterpret_cast<HWND>(window);   // DXVK native: the SDL window is the HWND
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    IDXGISwapChain *swapchain = nullptr;
    ID3D11Device *device = nullptr;
    ID3D11DeviceContext *context = nullptr;
    D3D_FEATURE_LEVEL level = {};
    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
                                               D3D11_SDK_VERSION, &sd, &swapchain, &device, &level, &context);
    if (FAILED(hr)) return fail("D3D11CreateDeviceAndSwapChain", hr);
    std::printf("dxvk-clear: device at feature level %x\n", (unsigned)level);

    ID3D11Texture2D *back = nullptr;
    hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&back));
    if (FAILED(hr)) return fail("GetBuffer", hr);
    ID3D11RenderTargetView *rtv = nullptr;
    hr = device->CreateRenderTargetView(back, nullptr, &rtv);
    if (FAILED(hr)) return fail("CreateRenderTargetView", hr);

    const float colour[4] = {0.10f, 0.55f, 0.85f, 1.0f};   // a blue nobody draws by accident
    Uint32 end = SDL_GetTicks() + (Uint32)seconds * 1000u;
    bool checked = false, ok = false;
    while (SDL_GetTicks() < end) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) if (e.type == SDL_QUIT) end = 0;
        context->ClearRenderTargetView(rtv, colour);
        if (!checked) {
            // Read the cleared back buffer back before presenting it.
            D3D11_TEXTURE2D_DESC td = {};
            back->GetDesc(&td);
            td.Usage = D3D11_USAGE_STAGING;
            td.BindFlags = 0;
            td.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            td.MiscFlags = 0;
            ID3D11Texture2D *staging = nullptr;
            hr = device->CreateTexture2D(&td, nullptr, &staging);
            if (FAILED(hr)) return fail("CreateTexture2D (staging)", hr);
            context->CopyResource(staging, back);
            D3D11_MAPPED_SUBRESOURCE map = {};
            hr = context->Map(staging, 0, D3D11_MAP_READ, 0, &map);
            if (FAILED(hr)) return fail("Map", hr);
            const unsigned char *px = static_cast<const unsigned char *>(map.pData) + 100 * map.RowPitch + 100 * 4;
            std::printf("dxvk-clear: pixel (100,100) = %u %u %u %u\n", px[0], px[1], px[2], px[3]);
            ok = px[0] >= 24 && px[0] <= 28 && px[1] >= 138 && px[1] <= 142 && px[2] >= 215 && px[2] <= 219 && px[3] == 255;
            context->Unmap(staging, 0);
            staging->Release();
            checked = true;
        }
        hr = swapchain->Present(1, 0);
        if (FAILED(hr)) return fail("Present", hr);
    }
    rtv->Release();
    back->Release();
    context->Release();
    swapchain->Release();
    device->Release();
    SDL_DestroyWindow(window);
    SDL_Quit();
    std::printf(ok ? "dxvk-clear: clear OK\n" : "dxvk-clear: clear WRONG colour\n");
    return ok ? 0 : 1;
}
