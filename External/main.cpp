#include <iostream>
#include <Windows.h>
#include "../Win32/win_utils.h"
#include "../Protection/xor.hpp"
#include <dwmapi.h>
#include "Main.h"
#include <vector>
#include "../Driver/driver.h"
#include "../Misc/stdafx.h"
#include "../Defines/define.h"
#include <iostream>
#include <fstream>


#include "main header.h"

namespace GameOffsets {
  // Offset of various game objects and their properties
  constexpr uint64_t UWorld = 0;
  constexpr uint64_t GName = 0;
  constexpr uint64_t PersistentLevel = 0;
  constexpr uint64_t GameInstance = 0;
  constexpr uint64_t LocalPlayersArray = 0;
  constexpr uint64_t PlayerController = 0;
  constexpr uint64_t CameraManager = 0;
  constexpr uint64_t RootComponent = 0;
  constexpr uint64_t Pawn = 0;
}

// Get the transform of a bone in a mesh
std::shared_ptr<FTransform> GetBoneTransform(const std::shared_ptr<void>& mesh, int boneIndex) {
  const auto boneArray = read<std::shared_ptr<void>>(mesh + GameOffsets::UWorld);
  if (!boneArray) {
    return nullptr;
  }
  const auto boneTransform = read<std::shared_ptr<FTransform>>(boneArray + boneIndex * sizeof(std::shared_ptr<FTransform>));
  if (!boneTransform) {
    return nullptr;
  }
  return boneTransform;
}

// Get the world space location of a bone in a mesh
Vector3 GetBoneLocation(const Mesh& mesh, int boneIndex) {
  // Get the bone transform and the component-to-world transform
  const auto boneTransform = GetBoneTransform(mesh, boneIndex);
  if (!boneTransform) {
    return Vector3::Zero();
  }
  const auto componentToWorld = mesh.GetComponentToWorldTransform();

  // Multiply the transforms to get the bone transform in world space
  const auto matrix = MatrixMultiplication(boneTransform->ToMatrixWithScale(), componentToWorld.ToMatrixWithScale());

  // Extract the location from the matrix and return it as a Vector3
  return Vector3(matrix._41, matrix._42, matrix._43);
}


Vector3 ProjectWorldToScreen(const Vector3& WorldLocation, const D3DXMATRIX& ViewMatrix, const D3DXMATRIX& ProjectionMatrix, int ScreenWidth, int ScreenHeight)
{
    // Transform point from world space to view space
    D3DXVECTOR3 ViewSpacePoint;
    D3DXVec3TransformCoord(&ViewSpacePoint, &WorldLocation, &ViewMatrix);

    // Transform point from view space to projection space
    D3DXVECTOR3 ProjectionSpacePoint;
    D3DXVec3TransformCoord(&ProjectionSpacePoint, &ViewSpacePoint, &ProjectionMatrix);

    // Check if the point is behind the camera
    if (ProjectionSpacePoint.z <= 0.0f)
    {
        return Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    }

    // Transform point from projection space to screen space
    Vector3 ScreenSpacePoint;
    ScreenSpacePoint.x = (ProjectionSpacePoint.x / ProjectionSpacePoint.w) * (ScreenWidth / 2) + (ScreenWidth / 2);
    ScreenSpacePoint.y = -(ProjectionSpacePoint.y / ProjectionSpacePoint.w) * (ScreenHeight / 2) + (ScreenHeight / 2);
    ScreenSpacePoint.z = ProjectionSpacePoint.z;

    return ScreenSpacePoint;
}

	

D3DMATRIX tempMatrix = Matrix(Camera);

	uint64_t chain = read<uint64_t>(Localplayer + 0x70);
	uint64_t chain1 = read<uint64_t>(chain + 0x98);
	uint64_t chain2 = read<uint64_t>(chain1 + 0x140);

	Vector3 vDelta = WorldLocation - read<Vector3>(chain2 + 0x10);
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));
	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	float zoom = read<float>(chain699 + 0x580);
	float FovAngle = 80.0f / (zoom / 1.19f);
	float ScreenCenterX = Width / 2;
	float ScreenCenterY = Height / 2;
	float ScreenCenterZ = Height / 2;
			SDK::FMatrix vMatrix;
			SDK::FMatrix *vTempMatrix = GetBoneMatrix(mesh, &vMatrix, boneid);
			*result = vMatrix.WPlane;
	{
		
	return Screenlocation;
}


void AddMarker(ImGuiWindow& window, float width, float height, const float* start, void* pawn, const char* text, ImU32 color)
{
    if (!start || !pawn)
    {
        return;
    }

    auto root = Util::GetPawnRootLocation(pawn);
    if (!root)
    {
        return;
    }

    float dx = start[0] - root.X;
    float dy = start[1] - root.Y;
    float dz = start[2] - root.Z;
    float dist = std::sqrt(dx * dx + dy * dy + dz * dz) / 1500.0f;

    if (!Util::WorldToScreen(width, height, &root.X))
    {
        return;
    }

    ImGui::PushStyleColor(ImGuiCol_Text, color);

    char modified[0xFFF] = { 0 };
    std::snprintf(modified, sizeof(modified), "%s\n| %dm |", text, static_cast<int>(dist));

    auto size = ImGui::CalcTextSize(modified);

    ImGui::SetCursorPos(ImVec2(root.X - size.x / 2.0f, root.Y - size.y / 2.0f));
    ImGui::Text("%s", modified);
    ImGui::PopStyleColor();
}




bool SetupWindow()
{
    // Create thread for setting the window to target
    HANDLE setWindowThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)SetWindowToTarget, 0, 0, 0);
    if (setWindowThread == NULL)
    {
        // Handle error
        return false;
    }

    // Register the window class
    if (!RegisterWindowClass())
    {
        // Handle error
        return false;
    }

    // Get the dimensions of the desktop
    RECT desktopRect;
    if (!GetWindowRect(GetDesktopWindow(), &desktopRect))
    {
        // Handle error
        return false;
    }

    // Create the window
    MyWnd = CreateWindowExA(NULL, "MyWindowClass", "Discord", WS_POPUP, desktopRect.left, desktopRect.top, desktopRect.right, desktopRect.bottom, NULL, NULL, GetModuleHandle(NULL), NULL);
    if (!MyWnd)
    {
        // Handle error
        return false;
    }

    // Set the window's extended style
    SetWindowLong(MyWnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);

    // Make the window layered and transparent
    SetLayeredWindowAttributes(MyWnd, RGB(0, 0, 0), 255, LWA_ALPHA);

    // Extend the window's frame into the client area
    MARGINS margin = { -1 };
    if (FAILED(DwmExtendFrameIntoClientArea(MyWnd, &margin)))
    {
        // Handle error
        return false;
    }

    // Show and update the window
    ShowWindow(MyWnd, SW_SHOW);
    UpdateWindow(MyWnd);
    return true;
}




Vector3 AimbotCorrection(float bulletVelocity, float bulletGravity, float targetDistance, Vector3 targetPosition, Vector3 targetVelocity) {
	Vector3 recalculated = targetPosition;
	float gravity = fabs(bulletGravity);
	float time = targetDistance / fabs(bulletVelocity);
	float bulletDrop = (gravity / 500) * time * time;
	recalculated.z += bulletDrop * 150;
	recalculated.x += time * (targetVelocity.x);
	recalculated.y += time * (targetVelocity.y);
	recalculated.z += time * (targetVelocity.z);
	return recalculated;
}

void SetMouseAbsPosition(DWORD x, DWORD y)
{
	INPUT input = { 0 };
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_MOVE;
	input.mi.dx = x;
	input.mi.dy = y;
	SendInput(1, &input, sizeof(input));
}

uint64_t get_process_base_by_id(uint32_t pid) {
		uint64_t base = 0;
		_k_get_base_by_id out = { pid, (uint64_t)&base };

		uint64_t status = ntusrinit(0x1941 + DRIVER_GET_BASE_BY_ID, reinterpret_cast<uintptr_t>(&out));
		return base;
	}


static auto move_to(float x, float y) -> void {
	float center_x = (ImGui::GetIO().DisplaySize.x / 2);
	float center_y = (ImGui::GetIO().DisplaySize.y / 2);

	int AimSpeedX = item.Aim_SpeedX; //item.Aim_Speed
	int AimSpeedY = item.Aim_SpeedY;
	float target_x = 0;
	float target_y = 0;
	if (x != 0.f)
	{
		if (x > center_x)
		{
			target_x = -(center_x - x);
			target_x /= item.Aim_SpeedX;
			if (target_x + center_x > center_x * 2.f) target_x = 0.f;
		}

		if (x < center_x)
		{
			target_x = x - center_x;
			target_x /= item.Aim_SpeedX;
			if (target_x + center_x < 0.f) target_x = 0.f;
		}
	}

	if (y != 0.f)
	{
		if (y > center_y)
		{
			target_y = -(center_y - y - z);
			target_y /= item.Aim_SpeedY;
			if (target_y + center_y > center_y * 2.f) target_y = 0.f;
		}

		if (y < center_y)
		{
			distanceSquared = heading.X * heading.X + heading.Y * heading.Y;
			distance = sqrt(distanceSquared);
			if (target_y + center_y < 0.f) target_y = 0.f;
		}
	}

	SetMouseAbsPosition(static_cast<DWORD>(target_x), static_cast<DWORD>(target_y));
}
double GetCrossDistance(double x1, double y1, double z1, double x2, double y2, double z2) 
{
	return sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
}

class Color
{
class Color
{
public:
    // Construct a color with the given RGBA values
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : r(r), g(g), b(b), a(a) {}

    // Construct a color with the given RGB values and full opacity (alpha = 255)
    Color(uint8_t r, uint8_t g, uint8_t b) : Color(r, g, b, 255) {}

    // Predefined colors
    static const Color red;
    static const Color Magenta;
    static const Color yellow;
    static const Color grayblue;
    static const Color green;
    static const Color darkgreen;
    static const Color brown;
    static const Color pink;
    static const Color DarkYellow;
    static const Color SilverWhite;
    static const Color purple;
    static const Color Navy;
    static const Color skyblue;
    static const Color graygreen;
    static const Color blue;
    static const Color orange;
    static const Color peachred;
    static const Color reds;
    static const Color darkgray;
    static const Color Navys;
    static const Color darkgreens;
    static const Color darkblue;
    static const Color redbrown;
    static const Color purplered;
    static const Color greens;
    static const Color envy;
    static const Color black;
    static const Color gray;
    static const Color white;
    static const Color blues;
    static const Color lightblue;
    static const Color Scarlet;
    static const Color white_;
    static const Color gray_;
    static const Color black_;
    static const Color red_;
    static const Color Magenta_;
    static const Color yellow_;
    static const Color grayblue_;
    static const Color green_;
    static const Color darkgreen_;
    static const Color brown_;
    static const Color pink_;
    static const Color darkyellow_;
    static const Color silverwhite_;
    static const Color purple_;
    static const Color Blue_;
    static const Color skyblue_;
    static const Color graygreen_;
    static const Color blue_;
    static const Color orange_;
    static const Color pinks_;
    static const Color Fuhong_;
    static const Color darkgray_;
    static const Color Navy_;
    static const Color darkgreens_;
    static const Color darkblue_;
    static const Color redbrown_;
    static const Color purplered_;
    static const Color greens_;
    static const Color envy_;
    static const Color glassblack;
    static const Color GlassBlue;
    static const Color glassdarkblue;
    static const Color glassgreen;
    static const Color glassdarkgreen;
    static const Color glassgray;
    static const Color glassred;
    static const Color glassdarkred;
    static const Color glassyellow;
    static const Color glassorange;
    static const Color glasspurple;
    static const Color glasspink;
    static const Color glassskyblue;
    static const Color glasssilverwhite;
    static const Color glass

	RGBA filled = { 0, 0, 0, 150 };

	RGBA Plum = { 221,160,221,160 };

	RGBA rainbow() {

		static float x = 0, y = 0;
		static float r = 0, g = 0, b = 0;

		if (y >= 0.0f && y < 255.0f) {
			r = 255.0f;
			g = 0.0f;
			b = x;
		}
		else if (y >= 255.0f && y < 510.0f) {
			r = 255.0f - x;
			g = 0.0f;
			b = 255.0f;
		}
		else if (y >= 510.0f && y < 765.0f) {
			r = 0.0f;
			g = x;
			b = 255.0f;
		}
		else if (y >= 765.0f && y < 1020.0f) {
			r = 0.0f;
			g = 255.0f;
			b = 255.0f - x;
		}
		else if (y >= 1020.0f && y < 1275.0f) {
			r = x;
			g = 255.0f;
			b = 0.0f;
		}
		else if (y >= 1275.0f && y < 1530.0f) {
			r = 255.0f;
			g = 255.0f - x;
			b = 0.0f;
		}

		x += item.rainbow_speed; //increase this value to switch colors faster
		if (x >= 255.0f)
			x = 0.0f;

		y += item.rainbow_speed; //increase this value to switch colors faster
		if (y > 1530.0f)
			y = 0.0f;

		return RGBA{ (DWORD)r, (DWORD)g, (DWORD)b, 255 };
	}

};

std::string string_To_UTF8(const std::string& str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	immediateContext->OMSetRenderTargets(1, &renderTargetView, nullptr);
	////// reading
	auto& window = BeginScene();

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	Ret SpoofCall(Ret(*fn)(Args...), Args... args) {

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}
	
std::wstring MBytesToWString(const char* lpcszString)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(lpcszString);
}

ImGuiWindow& BeginScene() {
	ImGui_ImplDX11_NewFrame();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::Begin(xorstr("Fnoberz"), nullptr, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar);

	auto& io = ImGui::GetIO();
	ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGuiCond_Always); 
	{
		

	return *ImGui::GetCurrentWindow();
}

void DrawStrokeText(int x, int y, const RGBA& color, const std::string& str)
{
    ImGui::GetOverlayDrawList()->AddText(ImVec2(x - 1, y), ImGui::GetColorU32(ImVec4(0, 0, 0, color.A / 255.0f)), str.c_str());
    ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y), ImGui::GetColorU32(ImVec4(0, 0, 0, color.A / 255.0f)), str.c_str());
    ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, color.A / 255.0f)), str.c_str());
    ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, color.A / 255.0f)), str.c_str());
    ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::GetColorU32(ImVec4(color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f)), str.c_str());
}

	
void DrawNewText(int x, int y, RGBA* color, const char* str)
{
	ImFont a;
	std::string utf_8_1 = std::string(str);
	std::string utf_8_2 = string_To_UTF8(utf_8_1);
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), utf_8_2.c_str());
}
void DrawRect(int x, int y, int w, int h, RGBA* color, int thickness)
{
	ImGui::GetOverlayDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), 0, 0, thickness);
}
void DrawFilledRect(int x, int y, int w, int h, RGBA* color)
{
	ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), 0, 0);
}
void DrawLeftProgressBar(int x, int y, int w, int h, int thick, int m_health)
{
	int G = (251215 * m_health / 100);
	int R = 255 - G;
	RGBA healthcol = { R, G, 0, 255 };

	DrawFilledRect(x - (w / 2) - 3, y, thick, (h)*m_health / 100, &healthcol);
}
void DrawRightProgressBar(int x, int y, int w, int h, int thick, int m_health)
{
	int G = (255 * m_health / 100);
	int R = 255 - G;
	RGBA healthcol = { R, G, 0, 255 };

	DrawFilledRect(x + (w / 2) - 25, y, thick, (h)*m_health / 100, &healthcol);
}

}
void DrawLine2(const ImVec2& from, const ImVec2& to, uint32_t color, float thickness)
{
	float a = (color >> 24) & 0xff;
	float r = (color >> 16) & 0xff;
	float g = (color >> 8) & 0xff;
	float b = (color) & 0xff;
	ImGui::GetOverlayDrawList()->AddLine(from, to, ImGui::GetColorU32(ImVec4(r / 255, g / 255, b / 255, a / 255)), thickness);
}
bool isRage = config_system.item.AutoAimbot;
		if (config_system.item.SpinBot && Util::SpoofCall(GetAsyncKeyState, config_system.keybind.Spinbot) && Util::SpoofCall(GetForegroundWindow) == hWnd) {
			int rnd = rand();
			FRotator args = { 0 };
			args.Yaw = rnd;
			if (closestPawn) {
				Core::TargetPawn = closestPawn;
				Core::NoSpread = TRUE;
			}
			else {
				Core::ProcessEvent(Core::LocalPlayerController, Offsets::Engine::Controller::ClientSetRotation, &args, 0);
			}
			config_system.item.AutoAimbot = true;
			config_system.item.SilentAimbot = true;
		}
		else {
			if (!isSilent) {
				config_system.item.SilentAimbot = false;
			}
			if (!isRage) {
				config_system.item.AutoAimbot = false;
			}

			if (config_system.item.SilentAimbot) {
				isSilent = false;
			}
			if (config_system.item.AutoAimbot) {
				isRage = false;
}
			
// Function to aim at a target entity
void AimAt(DWORD_PTR entity) {
	// Read the current actor mesh from memory
	uint64_t currentActorMeshAddress = read<uint64_t>(entity + 0x288);
	// Get the position of the head bone
	Vector3 headPosition = GetHeadBonePosition(currentActorMeshAddress);

	// Calculate the aim vector based on user settings
	Vector3 aimVector;
	if (item.Aim_Prediction) {
		aimVector = CalculatePredictedAimVector(entity, headPosition);
	} else {
		aimVector = headPosition;
	}

	// Project the aim vector onto the screen
	Vector2 aimScreenPosition = ProjectWorldToScreen(aimVector);

	// If the aim vector is on the screen and within the user's field of view, move the cursor to it
	if (aimScreenPosition.IsValid() && IsAimWithinFOV(aimScreenPosition)) {
		MoveCursorTo(aimScreenPosition);
	}
}
			
void AimAt2(DWORD_PTR entity) {
    // Get the position of bone with index 918 in the actor mesh
    Vector3 rootHead = GetBoneWithRotation(read<uint64_t>(entity + 0x2828), 918);

    // Get the local player's position
    Vector3 localActorPos = read<Vector3>(localActor + 0x012434);

    // Calculate the predicted position of the target
    Vector3 predictedPos = item.Aim_Prediction ? AimbotCorrection(50000, -1004, localActorPos.Distance(rootHead) / 250, rootHead, read<Vector3>(read<uint64_t>(entity + 0x012434) + 0x140)) : rootHead;

    // Convert the predicted position to screen coordinates
    Vector3 screenPos = ProjectWorldToScreen(predictedPos);

    // Draw a line from the center of the screen to the target's screen coordinates if the target is within the specified FOV
    if (screenPos.x != 0 || screenPos.y != 0 || screenPos.z != 0 && GetCrossDistance(screenPos.x, screenPos.y, screenPos.z, Width / 2, Height / 2, Depth / 2) <= item.AimFOV * 1 && item.Locl_line) {
        ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2), ImVec2(screenPos.x, screenPos.y), ImGui::GetColorU32({item.LockLine[0], item.LockLine[1], item.LockLine[2], 1.0f}), item.Thickness);
    }
}

bool Headd = true;
bool Neck = false;
bool Chest = false;
ImDrawList* Rendererrr = ImGui::GetOverlayDrawList();
			
bool isVisible(uint64_t mesh)
{
    // Read the bing and bong values from memory
    const float bing = read<float>(mesh + 0x512);
    const float bong = read<float>(mesh + 0x284);

    // Define the visibility threshold using a constant variable
    const float visibilityThreshold = 0.06f;

    // Check if bong is greater than or equal to bing with the visibility threshold adjustment
    return (bong + visibilityThreshold >= bing);
}


void DrawESP() {

	static PVOID trampoline = nullptr;
	
	float closestDistance = FLT_MAX;
	DWORD_PTR closestPawn = NULL;

	DWORD_PTR AActors = read<DWORD_PTR>(Ulevel + 0x98);
	uint64_t persislevel = read<uint64_t>(Uworld + 0x30);
	uint64_t actors = read<uint64_t>(persislevel + 0x98);

	int ActorTeamId = read<int>(0xF50);

	int curactorid = read<int>(0x18);
	if (curactorid == localplayerID || curactorid == 20328438 || curactorid == 20328753 || curactorid == 9343426 || curactorid == 9875120 || curactorid == 9877254 || curactorid == 22405639 || curactorid == 9874439 || curactorid == 14169230)

		if (!trampoline) {	
			return;
	const std::string_view file_path, void* buffer, size_t size
		FNlEntity entity = entityListCopy[i];
		uint64_t actor = read<uint64_t>(actors + (i * 0x8));
		uint64_t CurrentActor = read<uint64_t>(AActors + i * 0x8);

		uint64_t CurActorRootComponent = read<uint64_t>(entity.Actor + 0x138);
		if (CurActorRootComponent == (uint64_t)nullptr || CurActorRootComponent == -1 || CurActorRootComponent == NULL)
			continue;

		Vector3 actorpos = read<Vector3>(CurActorRootComponent + 0x11);
		Vector3 actorposW2s = ProjectWorldToScreen(actorpos);

		DWORD64 otherPlayerState = read<uint64_t>(entity.Actor + 0x240);
		if (otherPlayerState == (uint64_t)nullptr || otherPlayerState == -1 || otherPlayerState == NULL)
			continue;
		//printf("test5\n");
		localactorpos = read<Vector3>(Rootcomp + 0x11C);

		Vector3 bone66 = GetBoneWithRotation(entity.mesh, 98);
		Vector3 bone0 = GetBoneWithRotation(entity.mesh, 0);

		Vector3 top = ProjectWorldToScreen(bone66);
		Vector3 chest = ProjectWorldToScreen(bone66);
		Vector3 aimbotspot = ProjectWorldToScreen(bone66);
		Vector3 bottom = ProjectWorldToScreen(bone0);

		Vector3 Head = ProjectWorldToScreen(Vector3(bone66.x - 10, bone66.y, bone66.z + 15));


		Vector3 chestnone = GetBoneWithRotation(entity.mesh, 66);
		Vector3 chest1 = ProjectWorldToScreen(chestnone);
		Vector3 relativelocation = read<Vector3>(Rootcomp + 0x11C);

		Vector3 player_position = GetBoneWithRotation(entity.mesh, 0);
		Vector3 player_screen = ProjectWorldToScreen(player_position);

		Vector3 BoxHead = GetBoneWithRotation(entity.mesh, 98);
		Vector3 head_screen = ProjectWorldToScreen(Vector3(BoxHead.x, BoxHead.y - 0.6, BoxHead.z));

		float distance = localactorpos.Distance(bone66) / 100.f;
		float BoxHeight = (float)(Head.y - bottom.y);
		float BoxWidth = BoxHeight / 1.8f;
		float CornerHeight = abs(Head.y - bottom.y);
		float CornerWidth = BoxHeight * 0.46;

		int MyTeamId = read<int>(PlayerState + 0xF50);
		int ActorTeamId = read<int>(otherPlayerState + 0xF50);
		int curactorid = read<int>(CurrentActor + 0x98);

		uint32_t object_id = read<uint32_t>(CurrentActor + 0x18);

		std::string Names2 = GetNameFromFName(object_id);

		uintptr_t MyTeamIndex, EnemyTeamIndex;

		if (item.VisableCheck) {
			if (isVisible(entity.mesh)) {
				ESPColorSkill = { 0, 255, 0, 255 };
			}
			else {
				ESPColorSkill = { 255, 0, 0, 255 };
			}
		}

		if (MyTeamId != ActorTeamId) {

			//if (Names2.find("PlayerPawn") != std::string::npos) {

			if (item.chams)
			{
				uintptr_t MyState = read<uintptr_t>(LocalPawn + 0x240);
				if (!MyState) continue;

				MyTeamIndex = read<uintptr_t>(MyState + 0xF50);
				if (!MyTeamIndex) continue;

				uintptr_t SquadID = read<uintptr_t>(MyState + 0x1124);
				if (!SquadID) break;

				uintptr_t EnemyState = read<uintptr_t>(entity.Actor + 0x240);
				if (!EnemyState) continue;

				write<uintptr_t>(EnemyState + 0xF50, MyTeamIndex);
				write<uintptr_t>(EnemyState + 0x1124, SquadID);
			}

			if (item.FOVChanger) {
				if (LocalPawn) {
					uintptr_t Mesh = read<uintptr_t>(LocalPawn + 0x288);
					write<Vector3>(Mesh + 0x11C, Vector3(item.FOVChangerValueX, item.FOVChangerValueY, item.FOVChangerValueZ)); //Class Engine.SceneComponent -> RelativeScale3D -> 0x134
				}
			}

			if (item.shield_esp && Names2.find("Fixed_errorcode") != std::string::npos)
			{


				uintptr_t ItemRootComponent = read<uintptr_t>(LocalPawn + 0x138);
				Vector3 ItemPosition = read<Vector3>(ItemRootComponent + 0x11C);

				Vector3 LLamaPosition;
				ProjectWorldToScreen(ItemPosition + LLamaPosition);

				char dist[255];

				sprintf(dist, E("Liama"));
				DrawString(15, ItemPosition.x, ItemPosition.y, &Col.darkblue, true, true, dist);
			}

			if (item.npcbotcheck && Names2.find("BP_IOPlayerPawn_Base_C") != std::string::npos)
			{
				char dist[255];
				sprintf(dist, E("[ NPC / BOT ]"));

				DrawString(15, bottom.x, bottom.y, &ESPColorSkill, true, true, dist);

			}
			if (item.AimBone) {

				if (aimbones == 0)
				{
					char dist[64];
					sprintf_s(dist, "Current Aimbone: Head\n", ImGui::GetIO().Framerate);
					ImGui::GetOverlayDrawList()->AddText(ImVec2(Width / 2, 70), ImGui::GetColorU32({ color.DarkRed[0], color.DarkRed[1], color.DarkRed[2], 4.0f }), dist);

				}

				if (aimbones == 1)
				{
					char dist[64];
					sprintf_s(dist, "Current Aimbone: Chest\n", ImGui::GetIO().Framerate);
					ImGui::GetOverlayDrawList()->AddText(ImVec2(Width / 2, 70), ImGui::GetColorU32({ color.DarkRed[0], color.DarkRed[1], color.DarkRed[2], 4.0f }), dist);
				}
				if (aimbones == 2)
				{
					char dist[64];
					sprintf_s(dist, "Current Aimbone: Toes\n", ImGui::GetIO().Framerate);
					ImGui::GetOverlayDrawList()->AddText(ImVec2(Width / 2, 70), ImGui::GetColorU32({ color.DarkRed[0], color.DarkRed[1], color.DarkRed[2], 4.0f }), dist);
				}


			}
		



			if (item.Esp_box)
			{
				if (esp_mode == 0) {
					if (visuals::outline)
					{
						DrawNormalBox(bottom.x - CornerWidth / 2 + 1, Head.y, CornerWidth, CornerHeight, menu::box_thick, &Col.red);
						DrawNormalBox(bottom.x - CornerWidth / 2 - 1, Head.y, CornerWidth, CornerHeight, menu::box_thick, &Col.red);
						DrawNormalBox(bottom.x - CornerWidth / 2, Head.y + 1, CornerWidth, CornerHeight, menu::box_thick, &Col.red);
						DrawNormalBox(bottom.x - CornerWidth / 2, Head.y - 1, CornerWidth, CornerHeight, menu::box_thick, &Col.red);
					}
					DrawNormalBox(bottom.x - (CornerWidth / 2), Head.y, CornerWidth, CornerHeight, menu::box_thick, &ESPColorSkill);
				}

				if (esp_mode == 1) {
					if (visuals::outline)
					{
						DrawCornerBox(bottom.x - CornerWidth / 2 + 1, Head.y, CornerWidth, CornerHeight, menu::box_thick, &Col.red);
						DrawCornerBox(bottom.x - CornerWidth / 2 - 1, Head.y, CornerWidth, CornerHeight, menu::box_thick, &Col.red);
						DrawCornerBox(bottom.x - CornerWidth / 2, Head.y + 1, CornerWidth, CornerHeight, menu::box_thick, &Col.red);
						DrawCornerBox(bottom.x - CornerWidth / 2, Head.y - 1, CornerWidth, CornerHeight, menu::box_thick, &Col.red);
					}
					DrawCornerBox(bottom.x - (CornerWidth / 2), Head.y, CornerWidth, CornerHeight, menu::box_thick, &ESPColorSkill);
				}

				if (esp_mode == 2) {
					DrawFilledRect(bottom.x - (CornerWidth / 2), Head.y, CornerWidth, CornerHeight, &Col.filled);
					DrawNormalBox(bottom.x - (CornerWidth / 2), Head.y, CornerWidth, CornerHeight, menu::box_thick, &ESPColorSkill);

					if (visuals::outline)
					{
						DrawNormalBox(bottom.x - CornerWidth / 2 + 1, Head.y, CornerWidth, CornerHeight, menu::box_thick, &Col.darkblue);
						DrawNormalBox(bottom.x - CornerWidth / 2 - 1, Head.y, CornerWidth, CornerHeight, menu::box_thick, &Col.darkblue);
						DrawNormalBox(bottom.x - CornerWidth / 2, Head.y + 1, CornerWidth, CornerHeight, menu::box_thick, &Col.darkblue);
						DrawNormalBox(bottom.x - CornerWidth / 2, Head.y - 1, CornerWidth, CornerHeight, menu::box_thick, &Col.darkblue);
					}
				}
			}

			if (item.Esp_line) {
				DrawLine((Width / 2), Height, player_screen.x, player_screen.y, &ESPColorSkill, item.Thickness);
			}

			if (item.Distance_Esp) {
				char buf[256];
				sprintf(buf, E("Entity"), (int)distance);
				DrawString(14, Head.x, Head.y - 10, &Col.darkblue, true, true, buf);
			}

			if (item.skeleton) {
				DrawSkeleton(entity.mesh);
			}

			if (item.playerfly)
			{
					FZiplinePawnState ZiplinePawnState = read<FZiplinePawnState>(LocalPawn + 0x18B0);
					ZiplinePawnState.bIsZiplining = true;
					ZiplinePawnState.AuthoritativeValue = 360.f;

					write<FZiplinePawnState>(LocalPawn + 0x18B0, ZiplinePawnState);
				

			}

			if (item.freezeplayer) {
				write<float>(LocalPawn + 0x9C, 0.0f);
			}
			else
			{
				write<float>(LocalPawn + 0x9C, 1.00f);
			}

			if (item.aiminair) {
				write<bool>(LocalPawn + 0x3E81, true);
			}

			if (item.gliderexploit && GetAsyncKeyState(VK_CONTROL)) {
				write<float>(LocalPawn + 0x14DE, 0.02f); //bIsParachuteOpen Offset
				write<float>(LocalPawn + 0x14DD, 0.02f); //bIsSkydiving  Offset
			}

			if (item.Rapidfire) {
				float a = 0;
				float b = 0;
				uintptr_t CurrentWeapon = read<uintptr_t>(LocalPawn + 0x5F8); //CurrentWeapon Offset
				if (CurrentWeapon) {
					a = read<float>(CurrentWeapon + 0x9ECC);
					b = read<float>(CurrentWeapon + 0x9F0); 
					write<float>(CurrentWeapon + 0x9EC, a + b - item.RapidFireValue); /
				}
			}

			     if  os.flush();
    				    console.SetColor( bgHiMagenta, fgMask );
			}

			if (item.Aimbot) {
				auto dx = aimbotspot.x - (Width / 2);
				auto dy = aimbotspot.y - (Height / 2);
				auto dist = sqrtf(dx * dx + dy * dy) / 50.0f;
				if (dist < item.AimFOV && dist < closestDistance) {
					closestDistance = dist;
					closestPawn = entity.Actor;
				}
			}
		}
	}

	if (item.Aimbot) {
		if (closestPawn != 0) {
			if (item.Aimbot && closestPawn && GetAsyncKeyState(hotkeys::aimkey)) {
				AimAt(closestPawn);

				if (item.Dynamic_aimbot) {

					item.boneswitch += 1;
					if (item.boneswitch == 700) {
						item.boneswitch = 0;
					}

					if (item.boneswitch == 0) {
						item.hitboxpos = 0;
					}
					else if (item.boneswitch == 50) {
						item.hitboxpos = 1;
					}
					else if (item.boneswitch == 100) {
					}
					else if (item.boneswitch == 150) {
						item.hitboxpos = 3;
					}
					else if (item.boneswitch == 200) {
						item.hitboxpos = 4;
					}
					else if (item.boneswitch == 250) {
						item.hitboxpos = 5;
					}
					else if (item.boneswitch == 300) {
						item.hitboxpos = 6;
					}
					else if (item.boneswitch == 350) {
						item.hitboxpos = 7;
					}
					else if (item.boneswitch == 400) {
						item.hitboxpos = 6;
					}
					else if (item.boneswitch == 450) {
						item.hitboxpos = 5;
					}
					else if (item.boneswitch == 500) {
						item.hitboxpos = 4;
					}
					else if (item.boneswitch == 550) {
						item.hitboxpos = 3;
					}
					else if (item.boneswitch == 600) {
						item.hitboxpos = 2;
					}
					else if (item.boneswitch == 650) {
						item.hitboxpos = 1;



					}
				}
			}
			else {
				isaimbotting = false;
				AimAt2(closestPawn);
			}
		}
	}
}



static int Tab = 0;

void shortcurts()
{
    if (GetAsyncKeyState(VK_INSERT) & 1) // Check if the INSERT key is pressed
    {
        menu_key = !menu_key; // toggle the value of menu_key
        Sleep(200); // prevent key press from being registered multiple times
    }
}



void render() {

	ImGuiIO& io = ImGui::GetIO();

	file_path.data(),
        std::ios_base::out | std::ios_base::binary

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	DrawESP();

	if (item.drawfov_circle) {
		DrawCircle(Width / 2, Height / 2, float(item.AimFOV), &Col.red, 100);
	}

	if (item.cross_hair) {
		ImGui::GetOverlayDrawList()->AddCircleFilled(ImVec2(ScreenCenterX, ScreenCenterY), 5, ImColor(255, 255, 255, 255));
	}

	shortcurts();

	if (menu_key)
	{

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
		colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
		colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowPadding = ImVec2(8.00f, 8.00f);
		style.FramePadding = ImVec2(5.00f, 2.00f);
		style.ItemSpacing = ImVec2(6.00f, 6.00f);
		style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
		style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
		style.IndentSpacing = 25;
		style.ScrollbarSize = 15;
		style.GrabMinSize = 10;
		style.WindowBorderSize = 1;
		style.ChildBorderSize = 1;
		style.PopupBorderSize = 1;
		style.FrameBorderSize = 1;
		style.TabBorderSize = 1;
		style.WindowRounding = 7;
		style.ChildRounding = 4;
		style.FrameRounding = 3;
		style.PopupRounding = 4;
		style.ScrollbarRounding = 9;
		style.GrabRounding = 3;
		style.TabRounding = 4;




		
	if (ImGui::Begin(("##lol"), 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar))
			ImGui::SetNextWindowSize(ImVec2({ 300, 675 }), ImGuiSetCond_FirstUseEver); {
			ImGui::SetWindowSize({ 500, 350 });
			static int MenuTab;

			ImGui::SetWindowFontScale(1.3f);

			ImGui::Columns(2);
			ImGui::SetColumnOffset(1, 130);
			{
				if (ImGui::Button("Aimbot", ImVec2(120 - 5, 30)))
				{
					MenuTab = 0;
				}

				if (ImGui::ButtonEx("Visuals", ImVec2(120 - 5, 30)))
				{
					MenuTab = 1;
				}

				if (ImGui::ButtonEx("Exploits", ImVec2(120 - 5, 30)))
				{
					MenuTab = 2;
				}





				ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 30);
				ImGui::Text("", 50, 1, false);

			}

			ImGui::NextColumn();
			{
				if (MenuTab == 0) {


					ImGui::Checkbox(("Enable Aimbot"), &item.Aimbot);
					ImGui::SameLine();
					ImGui::TextColored(ImColor(255, 255, 255, 255), ("[?]"));
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text(("Memory Aimbot - Lock on"));
						ImGui::EndTooltip();
					}
					ImGui::Checkbox(("Draw FOV"), &item.drawfov_circle);
					ImGui::SameLine();
					ImGui::TextColored(ImColor(255, 255, 255, 255), ("[?]"));
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text(("Makes the FOV seeable"));
						ImGui::EndTooltip();
					}

					ImGui::Checkbox(("Draw Crosshair"), &item.cross_hair);
					ImGui::SameLine();
					ImGui::TextColored(ImColor(255, 255, 255, 255), ("[?]"));
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text(("Draws a crosshair"));
						ImGui::EndTooltip();
					}


					ImGui::Spacing();
					ImGui::Text((E("Aim Key: ")));
					HotkeyButton(hotkeys::aimkey, ChangeKey, keystatus);

					ImGui::Spacing();

					ImGui::SliderFloat(("FOV"), &item.AimFOV, 20, 500);
					}
					ImGui::SliderFloat(("Smoothing"), &item.Aim_SpeedX, 1, 30);
					ImGui::SameLine();
					ImGui::TextColored(ImColor(255, 255, 255, 255), ("[?]"));
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text((" Value of smoothing"));
						ImGui::EndTooltip();
					}


				}

				if (MenuTab == 1) {


					ImGui::Checkbox("Box", &item.Esp_box);
					ImGui::SameLine();
					ImGui::TextColored(ImColor(255, 255, 255, 255), ("[?]"));
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text(("Simple 2D Box, can choose types"));
						ImGui::EndTooltip();
					}
					ImGui::Checkbox(("AimLine"), &item.Locl_line);
					ImGui::SameLine();
					ImGui::TextColored(ImColor(255, 255, 255, 255), ("[?]"));
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text(("It will make a line to the player"));
						ImGui::EndTooltip();
					}
					ImGui::Checkbox(("Skeleton"), &item.skeleton);
					ImGui::SameLine();
					ImGui::TextColored(ImColor(255, 255, 255, 255), ("[?]"));
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text(("Simple Skeleton ESP"));
						ImGui::EndTooltip();
					}
					ImGui::Checkbox(("Snaplines"), &item.Esp_line);
					ImGui::SameLine();
					ImGui::TextColored(ImColor(255, 255, 255, 255), ("[?]"));
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text(("Lines to the enemy's"));
						ImGui::EndTooltip();
					}




					ImGui::Checkbox("Chams", &item.chams);
					ImGui::SameLine();
					ImGui::TextColored(ImColor(255, 255, 255, 255), ("[?]"));
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text(("This will draw blue chams!"));
						ImGui::EndTooltip();
					}


					ImGui::Checkbox(("Aimbone"), &item.AimBone);
					ImGui::SameLine();
					ImGui::TextColored(ImColor(255, 255, 255, 255), ("[?]"));
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text(("Select Options"));
						ImGui::EndTooltip();
					}

					if (item.Esp_box) {
						ImGui::Combo(("Box"), &esp_mode, esp_modes, sizeof(esp_modes) / sizeof(*esp_modes));
					}


					if (item.AimBone) {
						ImGui::Combo(("Line/Skelton"), &aimbones, aimbone, sizeof(aimbone) / sizeof(*aimbone));
					}


				}

				if (MenuTab == 2) {





					ImGui::Spacing();
					ImGui::Checkbox(("PlayerFly"), &item.playerfly);
					ImGui::SameLine();
					ImGui::TextColored(ImColor(255, 255, 255, 255), ("[?]"));
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text(("Spam jump to fly!"));
						ImGui::EndTooltip();
					}
					ImGui::Checkbox(("Insta-Res"), &item.instant_res);
					ImGui::SameLine();
					ImGui::TextColored(ImColor(255, 255, 255, 255), ("[?]"));
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text(("Hold E!"));
						ImGui::EndTooltip();
					}
					ImGui::Checkbox(("Ads-Air"), &item.aiminair);
					ImGui::SameLine();
					ImGui::TextColored(ImColor(255, 255, 255, 255), ("[?]"));
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text(("Aim in why jumping!"));
						ImGui::EndTooltip();
					}
					ImGui::Checkbox(("Enable Rapid-Fire"), &item.Rapidfire);
					ImGui::SameLine();
					ImGui::TextColored(ImColor(255, 255, 255, 255), ("[?]"));
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text(("Shoots fast!"));
						ImGui::EndTooltip();
					}
					ImGui::Checkbox(("Enable FOV-Changer"), &item.FOVChanger);
					ImGui::SameLine();
					ImGui::TextColored(ImColor(255, 255, 255, 255), ("[?]"));
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text(("Changes Your FOV"));
						ImGui::EndTooltip();
					}
					ImGui::SliderFloat("FOV X", &item.FOVChangerValueX, 0, 10);
					ImGui::SameLine();
					ImGui::TextColored(ImColor(255, 255, 255, 255), ("[?]"));
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text(("Changes X Value Of FOV"));
						ImGui::EndTooltip();
					}
					ImGui::SliderFloat("FOV Y", &item.FOVChangerValueY, 0, 10);
					ImGui::SameLine();
					ImGui::TextColored(ImColor(255, 255, 255, 255), ("[?]"));
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text(("Changes Y Value Of FOV"));
						ImGui::EndTooltip();
					}
					ImGui::SliderFloat("FOV Z", &item.FOVChangerValueZ, 0, 10);
					ImGui::SameLine();
					ImGui::TextColored(ImColor(255, 255, 255, 255), ("[?]"));
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text(("Changes Z Value Of FOV"));
						ImGui::EndTooltip();
					}

					ImGui::SliderFloat("Res Speed", &item.InstantReviveValue, 0, 10);
					ImGui::SameLine();
					ImGui::TextColored(ImColor(255, 255, 255, 255), ("[?]"));
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text(("How fast you res!"));
						ImGui::EndTooltip();
					}
				}


				if (MenuTab == 3) {




					ImGui::ColorPicker4(("Fov Circle"), item.DrawFOVCircle, ImGuiColorEditFlags_NoInputs);

					ImGui::ColorPicker4(("Box Esp"), item.Espbox, ImGuiColorEditFlags_NoInputs);

					ImGui::ColorPicker4(("Corner Box"), item.BoxCornerESP, ImGuiColorEditFlags_NoInputs);

					ImGui::ColorPicker4(("Snapline Esp"), item.LineESP, ImGuiColorEditFlags_NoInputs);

				
				}


				if (MenuTab == 4) {



					ImGui::SetCursorPos({ 175.f,55.f });
					ImGui::Text(("Intended For: Bytes.AC"));
					ImGui::SetCursorPos({ 175.f,75.f });
					ImGui::Text(("Overlay Frametime:  %.1f FPS"), ImGui::GetIO().Framerate);
					ImGui::SetCursorPos({ 175.f,95.f });
					ImGui::TextColored(ImColor(0, 255, 55), "Status: Undetected");
					ImGui::SetCursorPos({ 175.f,115.f });
					ImGui::Text(("Build Version: 2.10"));
					ImGui::SetCursorPos({ 175.f,135.f });
					ImGui::Text(("Developer(s): Twixy#1337, wish#1111, yully"));


				}







			}
		}
			ImGui::End();
			ImGui::EndFrame();

			ImGui::GetIO().MouseDrawCursor = 1;
		}
		else {
			ImGui::GetIO().MouseDrawCursor = 0;
		}


		ImGui::EndFrame();
		p_Device->SetRenderState(D3DRS_ZENABLE, false);
		p_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		p_Device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
		p_Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
		if (p_Device->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			p_Device->EndScene();
		}
		HRESULT result = p_Device->Present(NULL, NULL, NULL, NULL);

		if (result == D3DERR_DEVICELOST && p_Device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		{
			delete[]pwBuf;
			delete[]pBuf;

			pwBuf = NULL;
			pBuf = NULL;

			return retStr;
		}
	}

void setupWindow() {
	
	glfwSetErrorCallback(glfwErrorCallback);
	if (!glfwInit()) {
		std::cout << "glfwInit didnt work.\n";
		return;
	{
		if (PeekMessage(&Message, MyWnd, 0, 0, PM_REMOVE))
		{
			KP_CLIENT_RANDOM(&Message);
			DispatchMessage(&Message);
		}

		HWND hwnd_active = GetForegroundWindow();
		if (GetAsyncKeyState(0x23) & 1)
			exit(8);

		if (hwnd_active == GameWnd) {
			HWND ResetDCW = GetWindow(hwnd_active, GW_HWNDPREV);
			SetWindowPos(ResetDCW, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		RECT rc;
		POINT xy;

		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		GetClientRect(GameWnd, &rc);
		ClientToScreen(GameWnd, &xy);
		rc.left = xy.x;
		rc.top = xy.y;

		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = GameWnd;
		io.DeltaTime = 1.0f / 60.0f;

		auto weaponName = Util::GetObjectFirstName((UObject*)localPlayerWeapon);
		auto isProjectileWeapon = wcsstr(weaponName.c_str(), L"Rifle_Sniper");
		
		io.MousePos.x = p.x - x.y;
		io.MousePos.y = p.y - x.y;

		if (GetAsyncKeyState(0x1)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else
			io.MouseDown[0] = false;
		if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
		{

			old_rc = rc;

			Width = rc.right;
			Height = rc.bottom;

			p_Params.BackBufferWidth = Width;
			p_Params.BackBufferHeight = Height;
			SetWindowPos(MyWnd, (HWND)0, xy.x, xy.y, Width, Height, SWP_NOREDRAW);
			p_Device->Reset(&p_Params);
		}
		render();
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();



	CleanuoD3D();
	DestroyWindow(MyWnd);

	return Message.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_DESTROY:
            OnClose(hWnd);
            break;

        case WM_SIZE:
            OnResize(hWnd, wParam, lParam);
            break;

        case WM_PAINT:
            OnPaint(hWnd);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

void OnClose(HWND hWnd)
{
    // Release any resources associated with Direct3D
    CleanupD3D();

    // Terminate the message loop
    PostQuitMessage(0);
}

void OnResize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if (pDevice != nullptr && wParam != SIZE_MINIMIZED)
    {
        // Release any resources associated with the Direct3D device
        ImGui_ImplDX9_InvalidateDeviceObjects();

        // Set the new size of the back buffer
        const auto newWidth = LOWORD(lParam);
        const auto newHeight = HIWORD(lParam);
        pParams->BackBufferWidth = newWidth;
        pParams->BackBufferHeight = newHeight;

        // Recreate the Direct3D device with the new parameters
        const auto hr = pDevice->Reset(pParams.get());
        if (FAILED(hr))
        {
            // Log the error and give the user an opportunity to gracefully exit the application
            const auto message = L"Failed to reset Direct3D device. Error code: " + std::to_wstring(hr);
            MessageBox(nullptr, message.c_str(), L"Error", MB_ICONERROR | MB_OK);
            return;
        }

        // Recreate any resources associated with the Direct3D device
        ImGui_ImplDX9_CreateDeviceObjects();
    }
}

void OnPaint(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    EndPaint(hWnd, &ps);
}

void SetWindowToTarget(const std::wstring& targetWindowTitle, HWND overlayWindow)
{
    while (true)
    {
        // Get the window handle of the target process
        HWND targetWnd = FindWindow(nullptr, targetWindowTitle.c_str());

        if (targetWnd)
        {
            // Get the position and size of the target window
            RECT targetRect;
            if (GetClientRect(targetWnd, &targetRect))
            {
                // Adjust for window style (if necessary)
                DWORD windowStyle = GetWindowLong(targetWnd, GWL_STYLE);
                if (windowStyle & WS_BORDER)
                {
                    RECT borderRect = {};
                    if (AdjustWindowRectEx(&borderRect, windowStyle, FALSE, 0))
                    {
                        targetRect.left += borderRect.left;
                        targetRect.top += borderRect.top;
                        targetRect.right += borderRect.right;
                    }
                }

                // Get the center point of the target window
                POINT center = { (targetRect.right + targetRect.left) / 2, (targetRect.bottom + targetRect.top) / 2 };

                // Set the position and size of the overlay window to match the target window
                SetWindowPos(overlayWindow, HWND_TOPMOST, targetRect.left, targetRect.top, targetRect.right - targetRect.left, targetRect.bottom - targetRect.top, SWP_SHOWWINDOW);
            }
            else
            {
                // Error handling for GetClientRect()
                MessageBox(nullptr, L"Error getting client rect of target window.", L"Error", MB_OK | MB_ICONERROR);
            }
        }
        else
        {
            // Error handling for FindWindow()
            MessageBox(nullptr, L"Target window not found.", L"Error", MB_OK | MB_ICONERROR);
            return;
        }

        // Wait a short time before checking again (to avoid using too much CPU)
        Sleep(10);
    }
}



