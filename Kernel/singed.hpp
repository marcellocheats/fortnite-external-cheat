#pragma once
#include <Windows.h>
#include <fstream>
#include <filesystem>

#include "..\string.hpp"
#include "raw_driver.hpp"
#include "file_utils.hpp"
#include "service_utils.hpp"

#define SERVICE_NAME XorStr(L"SandyBridge").c_str()
#define DISPLAY_NAME XorStr(L"SandyKernel").c_str()

namespace Renderer::Renderer(ID3D11Device *direct3DDevice, const std::wstring &defaultFontFamily) {

    direct3DDevice(direct3DDevice),
    immediateContext(nullptr),
    inputLayout(nullptr),
    vertexShader(nullptr),
    pixelShader(nullptr),
    fontFactory(nullptr),
    fontWrapper(nullptr),
    defaultFontFamily(defaultFontFamily),
    maxVertices(1024 * 4 * 3)
{
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 16 ,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    ID3DBlob *vsBlob = nullptr;
    ID3DBlob *psBlob = nullptr;

    direct3DDevice->GetImmediateContext(&immediateContext);

    throwIfFailed(FW1CreateFactory(FW1_VERSION, &fontFactory));

    renderList = std::make_unique<RenderList>(fontFactory, maxVertices);

    throwIfFailed(fontFactory->CreateFontWrapper(direct3DDevice, defaultFontFamily.c_str(), &fontWrapper));


    throwIfFailed(D3DCompile(shader, std::size(shader), nullptr, nullptr, nullptr, "VS", "vs_4_0", 0, 0, &vsBlob, nullptr));
    throwIfFailed(D3DCompile(shader, std::size(shader), nullptr, nullptr, nullptr, "PS", "ps_4_0", 0, 0, &psBlob, nullptr));

    throwIfFailed(direct3DDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader));
    throwIfFailed(direct3DDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader));

    throwIfFailed(direct3DDevice->CreateInputLayout(layout, static_cast<UINT>(std::size(layout)), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout));

    safeRelease(vsBlob);
    safeRelease(psBlob);


    D3D11_BLEND_DESC blendDesc{};

    blendDesc.RenderTarget->BlendEnable = TRUE;
    blendDesc.RenderTarget->SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget->DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget->SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget->DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget->BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget->BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget->RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    throwIfFailed(direct3DDevice->CreateBlendState(&blendDesc, &blendState));

    D3D11_BUFFER_DESC bufferDesc{};

    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = sizeof(Vertex) * static_cast<UINT>(maxVertices);
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;

    throwIfFailed(direct3DDevice->CreateBuffer(&bufferDesc, nullptr, &vertexBuffer));

    bufferDesc = {};

    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = sizeof(XMMATRIX);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;

    throwIfFailed(direct3DDevice->CreateBuffer(&bufferDesc, nullptr, &screenProjectionBuffer));

    D3D11_VIEWPORT viewport{};
    UINT numViewports = 1;

    immediateContext->RSGetViewports(&numViewports, &viewport);

    projection = XMMatrixOrthographicOffCenterLH(viewport.TopLeftX, viewport.Width, viewport.Height, viewport.TopLeftY,
        viewport.MinDepth, viewport.MaxDepth);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    throwIfFailed(immediateContext->Map(screenProjectionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
    {
        std::memcpy(mappedResource.pData, &projection, sizeof(XMMATRIX));
    }
    immediateContext->Unmap(screenProjectionBuffer, 0);
}
