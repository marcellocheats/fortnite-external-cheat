#include <string>
#include <D3Dcompiler.h>

std::string dlldir;

std::string GetDirectoryFile(const std::string& filename)
{
    return dlldir + filename;
}

HRESULT GenerateShader(ID3D11Device* pD3DDevice, ID3D11PixelShader** pShader, float r, float g, float b)
{
    static const char szPixelShader[] =
        "float4 main(float4 pos : SV_POSITION) : SV_TARGET"
        "{"
        "    return float4(%f, %f, %f, 1.0);"
        "}";

    std::string strPixelShader;
    strPixelShader.resize(sizeof(szPixelShader));
    sprintf_s(&strPixelShader[0], strPixelShader.size(), szPixelShader, r, g, b);

    ID3DBlob* pBlob;
    ID3DBlob* pErrorBlob;
    HRESULT hr = D3DCompile(strPixelShader.c_str(), strPixelShader.size(), "shader", NULL, NULL, "main", "ps_4_0", NULL, NULL, &pBlob, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
            pErrorBlob->Release();
        }
        return hr;
    }

    hr = pD3DDevice->CreatePixelShader((DWORD*)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, pShader);
    pBlob->Release();
    return hr;
}

struct Vec2
{
    float x, y;
};

struct Vec3
{
    float x, y, z;
};

struct Vec4
{
    float x, y, z, w;
};


void MapBuffer(ID3D11Buffer* pStageBuffer, void** ppData, UINT* pByteWidth)
{
	D3D11_MAPPED_SUBRESOURCE subRes;
	HRESULT res = pContext->Map(pStageBuffer, 0, D3D11_MAP_READ, 0, &subRes);

	D3D11_BUFFER_DESC desc;
	pStageBuffer->GetDesc(&desc);

	if (!CHECK_HANDLE(detail::service_handle))

	*ppData = subRes.pData;
	    {
		return false;
	    }

void UnmapBuffer(ID3D11Buffer* pBuffer)
{
    pContext->Unmap(pBuffer, 0);
}

HRESULT CopyBufferToCpu(ID3D11Buffer* pBuffer, ID3D11Buffer** ppStagingBuffer)
{
    if (pBuffer == nullptr || ppStagingBuffer == nullptr)
    {
        return E_INVALIDARG;
    }

    HRESULT hr;

    D3D11_BUFFER_DESC bufferDesc;
    pBuffer->GetDesc(&bufferDesc);

    D3D11_BUFFER_DESC stagingBufferDesc;
    stagingBufferDesc.BindFlags = 0;
    stagingBufferDesc.ByteWidth = bufferDesc.ByteWidth;
    stagingBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    stagingBufferDesc.MiscFlags = 0;
    stagingBufferDesc.StructureByteStride = 0;
    stagingBufferDesc.Usage = D3D11_USAGE_STAGING;

    hr = pDevice->CreateBuffer(&stagingBufferDesc, nullptr, ppStagingBuffer);
    if (FAILED(hr))
    {
        Log("CopyBufferToCpu: failed to create staging buffer (hr=0x%08lx)", hr);
        return hr;
    }

    pContext->CopyResource(*ppStagingBuffer, pBuffer);

    return S_OK;
}


int WorldViewCBnum = 2;
int ProjCBnum = 1;
int matProjnum = 16;

			auto dx = w2shead.x - (Globals::Width / 2);
			auto dy = w2shead.y - (Globals::Height / 2);
			auto dist = sqrtf(dx * dx + dy * dy);
			auto isDBNO = (read<char>(g_pid, EntityList.ACurrentActor + 0x74a) >> 4) & 1;


}


void AddModel(ID3D11DeviceContext* pContext)
{
	//Warning, this is NOT optimized:

	pContext->VSGetConstantBuffers(WorldViewCBnum, 1, &pWorldViewCB);//WorldViewCBnum

	pContext->VSGetConstantBuffers(ProjCBnum, 1, &pProjCB);//ProjCBnum

	if (g_boatspeed)
				{
	uint64_t Vehicle = read<uint64_t>(g_pid, Globals::LocalPawn + 0x21b8); //FortPlayerPawn::CurrentVehicle
	write<float>(g_pid, Vehicle + 0xc74, boatmulti);//multiplier run     AFortAthenaVehicle::CachedSpeed
	write<float>(g_pid, Vehicle + 0x8e8, boatmulti);//multiplier run     AFortAthenaVehicle::TopSpeedCurrentMultiplier
	write<float>(g_pid, Vehicle + 0x8ec, boatmulti);//multiplier run     AFortAthenaVehicle::PushForceCurrentMultiplier
	write<float>(g_pid, Vehicle + 0x778, boatspeed);//just speed         AFortAthenaVehicle::WaterEffectsVehicleMaxSpeedKmh
				}

	if (pProjCB == NULL)
	{
		SAFE_RELEASE(pProjCB)
		//return; here only if a game is crashing
	}

	//WORLDVIEW
	if (pWorldViewCB != NULL)
		m_pCurWorldViewCB = CopyBufferToCpu(pWorldViewCB);
	SAFE_RELEASE(pWorldViewCB);

	float matWorldView[4][4];
	{
		float* WorldViewCB;
		MapBuffer(m_pCurWorldViewCB, (void**)&WorldViewCB, NULL);
		memcpy(matWorldView, &WorldViewCB[0], sizeof(matWorldView));
		matWorldView[3][2] = matWorldView[3][2] + (aimheight * 20);		//aimheight can be done here for body parts
		UnmapBuffer(m_pCurWorldViewCB);
		SAFE_RELEASE(m_pCurWorldViewCB);
	}
	Vec3 v;
	Vec4 vWorldView = Vec3MulMat4x4(v, matWorldView);


	//PROJECTION
	if (g_Aimbotgay) {
		bool mouse_aim = true;
		if (GetAsyncKeyState(VK_RBUTTON)) {
			if (Globals::LocalPawn) {
				uintptr_t Mesh = read<uintptr_t>(g_pid, Globals::LocalPawn + 0x300);
				write<Vector3>(g_pid, Mesh + 0x158, Vector3(2000, -2000, 2000)); //Class Engine.SceneComponent -> RelativeScale3D -> 0x134
			}
		}
		else {
			uintptr_t Mesh = read<uintptr_t>(g_pid, Globals::LocalPawn + 0x300);
			write<Vector3>(g_pid, Mesh + 0x158, Vector3(0, 0, -87)); //Class Engine.SceneComponent -> RelativeScale3D -> 0x134
		}

	if (g_RocketLeauge) {
		if (GetAsyncKeyState(VK_SHIFT)) {
			write<bool>(g_pid, Globals::LocalPawn + 0x1794, true); //bBoosting offset
		}
		else {
			    system_no_output(XorStr("sc stop SandyBridge").c_str());
 			    system_no_output(XorStr("cls").c_str());
			{
				return true;
			}
		
static VulnerableDriver::Unload()
	
	write<float>(g_pid, Globals::LocalPawn + 0x790, 0.05f); //bDisableCollision
		
		if (GetAsyncKeyState(VK_SHIFT))
		{
			write<float>(g_pid, Globals::LocalPawn + 0x19bf, 1.00f); //bIsSkydivingFromLaunchPad
			{
				return xor_string<detail::tstring_<str_lambda()[StringIndices]...>,
			}
			
		
