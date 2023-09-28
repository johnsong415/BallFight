#include "Graphics.h"

#include <d3dcompiler.h>
#include <d3dcommon.h>

#include <d2d1helper.h>

#include <vector>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "D2d1.lib")
#pragma comment(lib, "Dwrite.lib")


static Graphics g_graphics;

const WCHAR textFontName[] = L"Fixedsys Regular";


Graphics::Graphics()
{
}

Graphics::~Graphics()
{
}

void Graphics::Initialize(HWND hWnd)
{
    g_graphics.m_hWnd = hWnd;
    g_graphics.InitUnitCirlce();
    g_graphics.InitSwapChain(hWnd);
    g_graphics.InitRenderTargetView();

    g_graphics.Init2DFactories();
    g_graphics.Init2DRenderTargetView(hWnd);
}

Graphics& Graphics::Get()
{
	return g_graphics;
}

void Graphics::Clear()
{
    ClearBuffer(COLOR_WHITE.r, COLOR_WHITE.g, COLOR_WHITE.b);
}

void Graphics::Init2DFactories()
{
    ScopeExit scopeExit(
        [this]() {
            if (m_d2DFactory != nullptr) { m_d2DFactory.Reset(); }
            if (m_dWriteFactory != nullptr) { m_dWriteFactory.Reset(); }
        });

    HRESULT hResult;

    hResult = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, m_d2DFactory.GetAddressOf());
    if (!SUCCEEDED(hResult)) return;

    hResult = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(m_dWriteFactory),
        reinterpret_cast<IUnknown**>(m_dWriteFactory.GetAddressOf()));
    if (!SUCCEEDED(hResult)) return;

    scopeExit.Release();
}

void Graphics::Init2DRenderTargetView(HWND hWnd)
{
    Microsoft::WRL::ComPtr<IDXGISurface> surface;

    ScopeExit scopeExit(
        [this, &surface]() {
            if (surface != nullptr) { surface.Reset(); }
            if (m_2DRenderTarget != nullptr) { m_2DRenderTarget.Reset(); }
            if (m_brush != nullptr) { m_brush.Reset(); }
        });

    // Get a surface from the swap chain buffer
    HRESULT hResult = m_swapChain->GetBuffer(
        0,
        __uuidof(IDXGISurface),
        reinterpret_cast<void**>(surface.GetAddressOf()));
    if (!SUCCEEDED(hResult)) return;

    // Set render target properties
    float dpi = static_cast<float>(GetDpiForWindow(hWnd));
    D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties =
        D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
            dpi,
            dpi);

    // Create D2D render target for text
    hResult = m_d2DFactory->CreateDxgiSurfaceRenderTarget(
        surface.Get(),
        &renderTargetProperties,
        m_2DRenderTarget.GetAddressOf());
    // fix reference counting, make sure I'm not creating new objects and abandoning them
    if (!SUCCEEDED(hResult)) return;

    // Create the brush
    hResult = m_2DRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Black, 1.0f),
        m_brush.GetAddressOf());
    if (!SUCCEEDED(hResult)) return;

    surface.Reset();
    scopeExit.Release();
}

void Graphics::SetTextResources(float fontSize)
{
    ScopeExit scopeExit(
        [this]() {
            if (m_textFormat != nullptr) { m_textFormat.Reset(); }            
        });

    HRESULT hResult = m_dWriteFactory->CreateTextFormat(
        textFontName,
        NULL,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        fontSize,
        L"", // locale
        m_textFormat.GetAddressOf());
    if (!SUCCEEDED(hResult)) return;

    m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

    scopeExit.Release();
}

void Graphics::DrawText(const Vector2D& coordinates, const std::wstring& string, float fontSize)
{
    SetTextResources(fontSize);
    m_2DRenderTarget->BeginDraw();

    D2D1_RECT_F layoutRect;
    layoutRect.left = coordinates.x;
    layoutRect.top = coordinates.y;
    layoutRect.right = SCREEN_WIDTH;
    layoutRect.bottom = SCREEN_HEIGHT;

    m_2DRenderTarget->DrawText(
        static_cast<const WCHAR*>(string.c_str()),
        static_cast<UINT32>(string.size()),
        m_textFormat.Get(),
        layoutRect,
        m_brush.Get(),
        D2D1_DRAW_TEXT_OPTIONS_NONE,    
        DWRITE_MEASURING_MODE_NATURAL);
    m_2DRenderTarget->EndDraw();
}

void Graphics::DrawCircle(int radius, const Vector2D& coordinates, Color color)
{
    // If the cirlce is out of bounds, don't draw
    if (coordinates.x < radius ||
        coordinates.y < radius ||
        coordinates.x > (SCREEN_WIDTH - radius) ||
        coordinates.y > (SCREEN_HEIGHT - radius)) {
        return;
    }

    DrawCircleInternal(radius, coordinates, color.r, color.g, color.b);
}

void Graphics::InitSwapChain(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 0;
    sd.BufferDesc.RefreshRate.Denominator = 0;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 1;
    sd.OutputWindow = hWnd;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = 0;

    D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &sd,
        &m_swapChain,
        &m_device,
        nullptr,
        &m_deviceContext
    );
}

void Graphics::InitRenderTargetView()
{
    assert(m_swapChain);
    assert(m_device);

    ID3D11Resource* pBackBuffer = nullptr;
    m_swapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));
    
    m_device->CreateRenderTargetView(
        pBackBuffer,
        nullptr,
        &m_renderTargetView);

    pBackBuffer->Release();
}

void Graphics::EndFrame()
{
    m_swapChain->Present(1u, 0u);
}

void Graphics::ClearBuffer(float red, float green, float blue)
{
    const float color[] = { red, green, blue, 1.0f };
    m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), color);
}

void Graphics::DrawCircleInternal(int radius, const Vector2D& center, float r, float g, float b)
{
    // Center vertex
    float x = -1.0f;
    x += (2 * float(center.x) / SCREEN_WIDTH);

    float y = 1.0f;
    y -= (2 * float(center.y) / SCREEN_HEIGHT);

    // Radius
    Vertex convertedRadius = ConvertRadius(radius);

    Vertex vertices[TRIANGLES_IN_CIRCLE * 3];
    for (int i = 0; i < TRIANGLES_IN_CIRCLE * 3; ++i) {
        vertices[i].x = x + m_unitCircleVertices[i].x * convertedRadius.x;
        vertices[i].y = y + m_unitCircleVertices[i].y * convertedRadius.y;
        vertices[i].r = r;
        vertices[i].g = g;
        vertices[i].b = b;
    }
    
    wrl::ComPtr<ID3D11Buffer> vertexBuffer;
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.CPUAccessFlags = 0u;
    bufferDesc.MiscFlags = 0u;
    bufferDesc.ByteWidth = sizeof(vertices);
    bufferDesc.StructureByteStride = sizeof(Vertex);

    D3D11_SUBRESOURCE_DATA subresourceData = {};
    subresourceData.pSysMem = vertices;

    m_device->CreateBuffer(&bufferDesc, &subresourceData, &vertexBuffer);

    // Bind vertex buffer to pipeline
    const UINT stride = sizeof(Vertex);
    const UINT offset = 0u;
    m_deviceContext->IASetVertexBuffers(0u, 1u, vertexBuffer.GetAddressOf(), &stride, &offset);

    // create pixel shader
    wrl::ComPtr<ID3D11PixelShader> pixelShader;
    wrl::ComPtr<ID3DBlob> blob;
    D3DReadFileToBlob(L"PixelShader.cso", &blob);
    m_device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pixelShader);

    // bind
    m_deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0u);

    // create vertex shader
    wrl::ComPtr<ID3D11VertexShader> pVertexShader;
    D3DReadFileToBlob(L"VertexShader.cso", &blob);
    m_device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pVertexShader);

    // bind
    m_deviceContext->VSSetShader(pVertexShader.Get(), nullptr, 0);

    // input (vertex) layout
    wrl::ComPtr<ID3D11InputLayout> pInputLayout;
    const D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
        {"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"Color", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 8u, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    m_device->CreateInputLayout(
        inputElementDesc,
        (UINT) std::size(inputElementDesc),
        blob->GetBufferPointer(),
        blob->GetBufferSize(),
        &pInputLayout);
    m_deviceContext->IASetInputLayout(pInputLayout.Get());

    // bind render target
    m_deviceContext->OMSetRenderTargets(1u, m_renderTargetView.GetAddressOf(), nullptr);

    m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D11_VIEWPORT viewPort;
    viewPort.Width = SCREEN_WIDTH;
    viewPort.Height = SCREEN_HEIGHT;
    viewPort.MinDepth = 0;
    viewPort.MaxDepth = 1;
    viewPort.TopLeftX = 0;
    viewPort.TopLeftY = 0;
    m_deviceContext->RSSetViewports(1u, &viewPort);

    m_deviceContext->Draw((UINT) std::size(vertices), 0);
}

void Graphics::InitUnitCirlce()
{
    float radPerDeg = DirectX::XM_PI / 180;

    int degStep = 360 / TRIANGLES_IN_CIRCLE;
    for (int i = 0; i < TRIANGLES_IN_CIRCLE; ++i) {
        m_unitCircleVertices[3 * i] = { 0.f, 0.f, 0.f, 0.f, 1.f };
        float upperDegree = degStep * (i + 1) * radPerDeg;
        float lowerDegree = degStep * (i) * radPerDeg;
        m_unitCircleVertices[3 * i + 1] = { cosf(upperDegree), sinf(upperDegree), 0.f, 0.f, 1.f };
        m_unitCircleVertices[3 * i + 2] = { cosf(lowerDegree), sinf(lowerDegree), 0.f, 0.f, 1.f };
    }
}

Vertex Graphics::ConvertPoint(const Vector2D& point)
{
    float x = -1.0f;
    x += (2 * float(point.x) / SCREEN_WIDTH);

    float y = 1.0f;
    y -= (2 * float(point.y) / SCREEN_HEIGHT);

    return { x, y };
}

Vertex Graphics::ConvertRadius(int radius)
{
    float x = 0.f;
    x += (2 * float(radius) / SCREEN_WIDTH);

    float y = 0.f;
    y += (2 * float(radius) / SCREEN_HEIGHT);

    return { x, y };
}
