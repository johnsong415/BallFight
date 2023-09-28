#pragma once

#include "framework.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>

#include <dwrite.h>
#include <d2d1.h>
#include <string>


namespace wrl = Microsoft::WRL;

const int SQUARE_SIDE_LENGTH = 20;
const int TRIANGLES_IN_CIRCLE = 20;

struct Vertex
{
    float x = 0;
    float y = 0;
    float r = 0;
    float g = 0;
    float b = 0;
};

class Graphics
{
public:
	Graphics();
	~Graphics();

public:
    static void Initialize(HWND hWnd);
    static Graphics& Get();

public:
    void Clear();
    void DrawCircle(int radius, const Vector2D& coordinates, Color color);
    void EndFrame();

    void DrawText(const Vector2D& coordinates, const std::wstring& string, float fontSize);

private:
    void InitSwapChain(HWND hWnd);
    void InitRenderTargetView();
    void InitUnitCirlce();

    void Init2DFactories();
    void Init2DRenderTargetView(HWND hWnd);
    void SetTextResources(float fontSize);


    void ClearBuffer(float red, float green, float blue);

    void DrawCircleInternal(int radius, const Vector2D& center, float red, float green, float blue);

    Vertex ConvertPoint(const Vector2D& center);
    Vertex ConvertRadius(int radius);

private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_device = nullptr;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain = nullptr;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext = nullptr;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView = nullptr;

    Microsoft::WRL::ComPtr<ID2D1Factory> m_d2DFactory = nullptr;
    Microsoft::WRL::ComPtr<IDWriteFactory> m_dWriteFactory = nullptr;
    Microsoft::WRL::ComPtr<IDWriteTextFormat> m_textFormat = nullptr;
    Microsoft::WRL::ComPtr<IDWriteTextLayout> m_textLayout = nullptr;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_brush = nullptr;
    Microsoft::WRL::ComPtr<ID2D1RenderTarget> m_2DRenderTarget = nullptr;

    Vertex m_unitCircleVertices[TRIANGLES_IN_CIRCLE * 3];

private:
	HWND m_hWnd = NULL;
};
