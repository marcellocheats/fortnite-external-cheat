onstexpr uintptr_t DefuseProgress = 0x500;
constexpr uintptr_t CurrentDefuseSection = 0x514;
constexpr uintptr_t TimeRemainingToExplode = 0x4d4;
constexpr uintptr_t BombHasBeenDefused = 0x4DD;
constexpr uintptr_t BombHasExploded = 0x4b0;

constexpr uintptr_t uworld_state = 0x912B300;
constexpr uintptr_t uworld_key = uworld_state + 0x38;

constexpr uintptr_t game_instance = 0x1A0;
constexpr uintptr_t persistent_level = 0x38;

constexpr uintptr_t local_player_array = 0x40;
constexpr uintptr_t local_player_controller = 0x38;
constexpr uintptr_t local_player_pawn = 0x460;
constexpr uintptr_t control_rotation = 0x440;


constexpr uintptr_t camera_manager = 0x478;
constexpr uintptr_t camera_position = 0x1260;
constexpr uintptr_t camera_rotation = 0x126C;
constexpr uintptr_t camera_fov = 0x1278;

constexpr uintptr_t actor_array = 0xA0;
constexpr uintptr_t actor_count = actor_array + 0x18;

constexpr uintptr_t unique_id = 0x38;
constexpr uintptr_t mesh_component = 0x430;
constexpr uintptr_t last_submit_time = 0x378;
constexpr uintptr_t last_render_time = last_submit_time + 0x4;
constexpr uintptr_t bone_array = 0x5c0;
constexpr uintptr_t bone_array_Cached = bone_array + 0x10;
constexpr uintptr_t bone_count = bone_array + 0x8;
constexpr uintptr_t component_to_world = 0x250;
constexpr uintptr_t root_component = 0x230;
constexpr uintptr_t RelativeLocation = 0x164;
constexpr uintptr_t root_position = 0x164;
constexpr uintptr_t damage_handler = 0x9e0;
constexpr uintptr_t health = 0x1B0;
constexpr uintptr_t dormant = 0x100;
constexpr uintptr_t player_state = 0x3F0;
constexpr uintptr_t team_component = 0x628;
constexpr uintptr_t team_id = 0xF8;


constexpr uintptr_t inventory = 0x948;
constexpr uintptr_t current_equipable = 0x238;
constexpr uintptr_t ammo_info = 0xfb0;
constexpr uintptr_t max_ammo = 0x120;
constexpr uintptr_t current_ammo = 0x11C;
constexpr uintptr_t ObjID = 0x18;

constexpr uintptr_t fresnel_intensity = 0x6b8;

constexpr uintptr_t viewangle = 0x170;


constexpr uintptr_t GameState = 0x140;
constexpr uintptr_t MapLoadModel = 0x140;
constexpr uintptr_t ClientGameInstance = 0x678;
