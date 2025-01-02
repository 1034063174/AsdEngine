#pragma once
#include <Windows.h>

//enum Globals
//{
//
//    PlayerName = 0x00C79D18,                    // 3.3.5a 12340
//    CurrentAccount = 0x00B6AA40,                // 3.3.5a 12340
//    CurrentRealm = 0x00C79B9E,                  // 3.3.5a 12340
//    CurrentTargetGUID = 0x00BD07B0,             // 3.3.5a 12340
//    LastTargetGUID = 0x00BD07B8,                // 3.3.5a 12340
//    MouseOverGUID = 0x00BD07A0,                 // 3.3.5a 12340
//    FollowGUID = 0x00CA11F8,                    // 3.3.5a 12340
//    ComboPoint = 0x00BD084D,                    // 3.3.5a 12340
//    LootWindow = 0x00BFA8D8,                    // 3.3.5a 12340
//    KnownSpell = 0x00BE5D88,                    // 3.3.5a 12340
//    IsLoadingOrConnecting = 0x00B6AA38,         // 3.3.5a 12340
//    Movement_Field = 0xD8,                      // 3.3.5a 12340
//    SpellCooldownPtr = 0x00D3F5AC,              // 3.3.5a 12340
//    Timestamp = 0x00B1D618,                     // 3.3.5a 12340
//    LastHardwareAction = 0x00B499A4,            // 3.3.5a 12340
//    ClntObjMgrObjectPtr = 0x004D4DB0,           // 3.3.5a 12340
//    ClntObjMgrGetActivePlayer = 0x004D3790,     // 3.3.5a 12340
//    ClntObjMgrGetActivePlayerObj = 0x4038F0,    // 3.3.5a 12340
//    HandleTerrainClick = 0x00527830,            // 3.3.5a 12340
//    CGGameUI_Target = 0x00524BF0,               // 3.3.5a 12340
//    Spell_C_CastSpell = 0x0080DA40,             // 3.3.5a 12340
//    CGUnit_C__GetCreatureType = 0x0071F300,     // 3.3.5a 12340
//    UnitName1 = 0x964,                          // 3.3.5a 12340
//    UnitName2 = 0x5C,                           // 3.3.5a 12340
//    nbItemsSellByMerchant = 0x00BFA3F0,         // 3.3.5a 12340
//    CInputControl = 0x00C24954,                 // 3.3.5a 12340
//    BuildNumber = 0x00B3203C,                   // 3.3.5a 12340
//    GetMinimapZoneText = 0x00BD077C,            // 3.3.5a 12340
//    GetZoneText = 0x00BD0788,                   // 3.3.5a 12340
//    GetSubZoneText = 0x00BD0784,                // 3.3.5a 12340
//    GetInternalMapName = 0x00CE06D0,            // 3.3.5a 12340
//    LocalGUID = 0x00CA1238,                     // 3.3.5a 12340
//    GetZoneID = 0x00BD080C,                     // 3.3.5a 12340
//    IsBobbingOffset = 0xBC,                     // 3.3.5a 12340
//    ChatboxIsOpen = 0x00D41660,                 // 3.3.5a 12340
//    M2Model__IsOutdoors = 0x0077FBF0,           // 3.3.5a 12340
//    CVar_MaxFPS = 0x00C5DF7C,                   // 3.3.5a 12340
//    CGWorldFrame__RenderWorld = 0x004FAF90,     // 3.3.5a 12340
//    CGWorldFrame__Intersect = 0x0077F310,       // 3.3.5a 12340
//
//};
//
// enum Battleground
//{
//
//    IsBattlegroundFinished = 0x00BEA588,        // 3.5.5a 12340
//
//};
//
// enum CGUnit_C__GetCreatureRank
// {
//
//     CGUnit_C__GetCreatureRank = 0x00718DE0,     // 3.3.5a 12340
//     Offset1 = 0x964,                            // 3.3.5a 12340
//     Offset4 = 0x18,                             // 3.3.5a 12340
//
// };
//
// enum CGWorldFrame__GetActiveCamera
// {
//
//
//     CGWorldFrame__GetActiveCamera = 0x4F5960,   // 3.3.5a 12340
//     Camera_Pointer = 0x00B7436C,                // 3.3.5a 12340
//     Camera_Offset = 0x7E20,                     // 3.3.5a 12340
//     Camera_X = 0x8,                             // 3.3.5a 12340
//     Camera_Y = 0xC,                             // 3.3.5a 12340
//     Camera_Z = 0x10,                            // 3.3.5a 12340
//     Camera_Follow_GUID = 0x88,                  // 3.3.5a 12340
//
// };
//
// enum ShapeshiftForm
// {
//
//     CGUnit_C__GetShapeshiftFormId = 0x0071AF70, // 3.3.5a 12340
//
//     BaseAddress_Offset1 = 0xD0,                 // 3.3.5a 12340 
//     BaseAddress_Offset2 = 0x1D3,                // 3.3.5a 12340 
//
// };
//
// enum Lua
// {
//
//     Lua_State = 0x00D3F78C,                     // 3.3.5a 12340
//     Lua_DoString = 0x00819210,                  // 3.3.5a 12340
//     Lua_GetLocalizedText = 0x007225E0,          // 3.3.5a 12340
//     Lua_SetTop = 0x000084DBF0,                  // 3.3.5a 12340
//
// };
//
// enum Movements
// {
//
//     MoveForwardStart = 0x005FC200,              // 3.3.5a 12340
//     MoveForwardStop = 0x005FC250,               // 3.3.5a 12340
//     MoveBackwardStart = 0x005FC290,             // 3.3.5a 12340
//     MoveBackwardStop = 0x005FC2E0,              // 3.3.5a 12340
//     TurnLeftStart = 0x005FC320,                 // 3.3.5a 12340
//     TurnLeftStop = 0x005FC360,                  // 3.3.5a 12340
//     TurnRightStart = 0x005FC3B0,                // 3.3.5a 12340
//     TurnRightStop = 0x005FC3F0,                 // 3.3.5a 12340
//     JumpOrAscendStart = 0x005FBF80,             // 3.3.5a 12340
//     AscendStop = 0x005FC0A0,                    // 3.3.5a 12340
//
// };
//
// enum ObjectManager
// {
//
//     CurMgrPointer = 0x00C79CE0,                 // 3.3.5a 12340
//     CurMgrOffset = 0x2ED0,                      // 3.3.5a 12340
//     NextObject = 0x3C,                          // 3.3.5a 12340
//     FirstObject = 0xAC,                         // 3.3.5a 12340
//     LocalGUID = 0xC0                            // 3.3.5a 12340
//
// };
//
// enum Corpse
// {
//
//     X = 0x00BD0A58,                              // 3.3.5a 12340
//     Y = X + 0x4,                                 // 3.3.5a 12340
//     Z = X + 0x8,                                 // 3.3.5a 12340
//
// };
//
// enum Party
// {
//
//     s_LeaderGUID = 0x00BD1968,                  // 3.3.5a 12340
//     s_Member1GUID = 0x00BD1948,                 // 3.3.5a 12340
//     s_Member2GUID = s_Member1GUID + 0x8,        // 3.3.5a 12340
//     s_Member3GUID = s_Member2GUID + 0x8,        // 3.3.5a 12340
//     s_Member4GUID = s_Member3GUID + 0x8,        // 3.3.5a 12340
//
// };
//
// enum Direct3D9
// {
//
//     pDevicePtr_1 = 0x00C5DF88,                  // 3.3.5a 12340
//     pDevicePtr_2 = 0x397C,                      // 3.3.5a 12340
//     oBeginScene = 0xA4,                         // 3.3.5a 12340
//     oEndScene = 0xA8,                           // 3.3.5a 12340
//     oClear = 0xAC,                              // 3.3.5a 12340
//
// };
//
// enum VFTableIndex
// {
//
//     Interact = 44,                              // 3.3.5a 12340
//     GetName = 54,                               // 3.3.5a 12340
//
// };
//
// enum UnitBaseGetUnitAura
// {
//
//     CGUnit_Aura = 0x00556E10,                   // 3.3.5a 12340
//     AURA_COUNT_1 = 0xDD0,                       // 3.3.5a 12340
//     AURA_COUNT_2 = 0xC54,                       // 3.3.5a 12340
//     AURA_TABLE_1 = 0xC50,                       // 3.3.5a 12340
//     AURA_TABLE_2 = 0xC58,                       // 3.3.5a 12340
//     AURA_SIZE = 0x18,                           // 3.3.5a 12340
//     AURA_SPELL_ID = 0x8                         // 3.3.5a 12340
//
// };
//
// enum ClickToMove
// {
//
//     CGPlayer_C__ClickToMove = 0x00727400,       // 3.3.5a 12340
//
//     CTM_Activate_Pointer = 0xBD08F4,            // 3.3.5a 12340
//     CTM_Activate_Offset = 0x30,                 // 3.3.5a 12340
//
//     CTM_Base = 0x00CA11D8,                      // 3.3.5a 12340
//     CTM_X = 0x8C,                               // 3.3.5a 12340
//     CTM_Y = 0x90,                               // 3.3.5a 12340
//     CTM_Z = 0x94,                               // 3.3.5a 12340
//     CTM_TurnSpeed = 0x4,                        // 3.3.5a 12340
//     CTM_Distance = 0xC,                         // 3.3.5a 12340
//     CTM_Action = 0x1C,                          // 3.3.5a 12340
//     CTM_GUID = 0x20,                            // 3.3.5a 12340
//
// };
//
// enum IsFlying
// {
//
//     // Reversed from Lua_IsFlying
//
//     IsFlyingOffset = 0x44,                      // 3.3.5a 12340
//     IsFlying_Mask = 0x2000000,                  // 3.3.5a 12340
//
// };
//
// enum IsSwimming
// {
//
//     // Reversed from Lua_IsSwimming
//
//     IsSwimmingOffset = 0xA30,                   // 3.3.5a 12340
//     IsSwimming_Mask = 0x200000,                 // 3.3.5a 12340
//
// };
//
// enum AutoLoot
// {
//
//     AutoLoot_Activate_Pointer = 0x00BD0914,     // 3.3.5a 12340
//     AutoLoot_Activate_Offset = 0x30,            // 3.3.5a 12340
//
// };
//
// enum AutoSelfCast
// {
//
//     AutoSelfCast_Activate_Pointer = 0xBD0920,   // 3.3.5a 12340
//     AutoSelfCast_Activate_Offset = 0x30,        // 3.3.5a 12340
//
// };
//
// enum WoWChat
// {
//
//     ChatBufferStart = 0x00B75A60,               // 3.3.5a 12340
//     NextMessage = 0x17C0,                       // 3.3.5a 12340
//
// };
struct _ClientOffsets
{
	DWORD   StaticClientConnection = 0x00C79CE0;
	DWORD   ObjectManagerOffset = 0x2ED0;
	DWORD   FirstObjectOffset = 0xAC;
	DWORD   LocalGuidOffset = 0xC0;
	DWORD   NextObjectOffset = 0x3C;
	DWORD   LocalPlayerGUID = 0xBD07A8;
	DWORD   LocalTargetGUID = 0x00BD07B0;
}ClientOffsets;

struct _NameOffsets
{
	DWORD  nameStore = 0x00C5D938 + 0x8;
	DWORD  nameMask = 0x24;
	DWORD  nameBase = 0x1C;
	DWORD   nameString = 0x20;
}NameOffsets;

struct _ObjectOffsets
{
	DWORD   Type = 0x14;
	DWORD   Pos_X = 0x79C;
	DWORD  Pos_Y = 0x798;
	DWORD  Pos_Z = 0x7A0;
	DWORD   Rot = 0x7A8;
	DWORD   Guid = 0x30;
	DWORD   UnitFields = 0x8;
}ObjectOffsets;

struct  _UnitOffsets
{
	DWORD  Level = 0x36 * 4;
	DWORD    Health = 0x18 * 4;
	DWORD  Energy = 0x19 * 4;
	DWORD  MaxHealth = 0x20 * 4;
	DWORD  SummonedBy = 0xE * 4;
	DWORD  MaxEnergy = 0x21 * 4;
	DWORD UNIT_FIELD_CHARM = 0x18;
	DWORD UNIT_FIELD_SUMMON = 0x20;
	DWORD UNIT_FIELD_CRITTER = 0x28;
	DWORD UNIT_FIELD_CHARMEDBY = 0x30;
	DWORD UNIT_FIELD_SUMMONEDBY = 0x38;
	DWORD UNIT_FIELD_CREATEDBY = 0x40;
	DWORD UNIT_FIELD_TARGET = 0x48;
	DWORD UNIT_FIELD_CHANNEL_OBJECT = 0x50;
	DWORD UNIT_CHANNEL_SPELL = 0x58;
	DWORD UNIT_FIELD_BYTES_0 = 0x5C;
	DWORD UNIT_FIELD_HEALTH = 0x60;
	DWORD UNIT_FIELD_POWER1 = 0x64;
	DWORD UNIT_FIELD_POWER2 = 0x68;
	DWORD UNIT_FIELD_POWER3 = 0x6C;
	DWORD UNIT_FIELD_POWER4 = 0x70;
	DWORD UNIT_FIELD_POWER5 = 0x74;
	DWORD UNIT_FIELD_POWER6 = 0x78;
	DWORD UNIT_FIELD_POWER7 = 0x7C;
	DWORD UNIT_FIELD_MAXHEALTH = 0x80;
	DWORD UNIT_FIELD_MAXPOWER1 = 0x84;
	DWORD UNIT_FIELD_MAXPOWER2 = 0x88;
	DWORD UNIT_FIELD_MAXPOWER3 = 0x8C;
	DWORD UNIT_FIELD_MAXPOWER4 = 0x90;
	DWORD UNIT_FIELD_MAXPOWER5 = 0x94;
	DWORD UNIT_FIELD_MAXPOWER6 = 0x98;
	DWORD UNIT_FIELD_MAXPOWER7 = 0x9C;
	DWORD UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER = 0xA0;
	DWORD UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER = 0xBC;
	DWORD UNIT_FIELD_LEVEL = 0xD8;
	DWORD UNIT_FIELD_FACTIONTEMPLATE = 0xDC;
	DWORD UNIT_VIRTUAL_ITEM_SLOT_ID = 0xE0;
	DWORD UNIT_FIELD_FLAGS = 0xEC;
	DWORD UNIT_FIELD_FLAGS_2 = 0xF0;
	DWORD UNIT_FIELD_AURASTATE = 0xF4;
	DWORD UNIT_FIELD_BASEATTACKTIME = 0xF8;
	DWORD UNIT_FIELD_RANGEDATTACKTIME = 0x100;
	DWORD UNIT_FIELD_BOUNDINGRADIUS = 0x104;
	DWORD UNIT_FIELD_COMBATREACH = 0x108;
	DWORD UNIT_FIELD_DISPLAYID = 0x10C;
	DWORD UNIT_FIELD_NATIVEDISPLAYID = 0x110;
	DWORD UNIT_FIELD_MOUNTDISPLAYID = 0x114;
	DWORD UNIT_FIELD_MINDAMAGE = 0x118;
	DWORD UNIT_FIELD_MAXDAMAGE = 0x11C;
	DWORD UNIT_FIELD_MINOFFHANDDAMAGE = 0x120;
	DWORD UNIT_FIELD_MAXOFFHANDDAMAGE = 0x124;
	DWORD UNIT_FIELD_BYTES_1 = 0x128;
	DWORD UNIT_FIELD_PETNUMBER = 0x12C;
	DWORD UNIT_FIELD_PET_NAME_TIMESTAMP = 0x130;
	DWORD UNIT_FIELD_PETEXPERIENCE = 0x134;
	DWORD UNIT_FIELD_PETNEXTLEVELEXP = 0x138;
	DWORD UNIT_DYNAMIC_FLAGS = 0x13C;
	DWORD UNIT_MOD_CAST_SPEED = 0x140;
	DWORD UNIT_CREATED_BY_SPELL = 0x144;
	DWORD UNIT_NPC_FLAGS = 0x148;
	DWORD UNIT_NPC_EMOTESTATE = 0x14C;
	DWORD UNIT_FIELD_STAT0 = 0x150;
	DWORD UNIT_FIELD_STAT1 = 0x154;
	DWORD UNIT_FIELD_STAT2 = 0x158;
	DWORD UNIT_FIELD_STAT3 = 0x15C;
	DWORD UNIT_FIELD_STAT4 = 0x160;
	DWORD UNIT_FIELD_POSSTAT0 = 0x164;
	DWORD UNIT_FIELD_POSSTAT1 = 0x168;
	DWORD UNIT_FIELD_POSSTAT2 = 0x16C;
	DWORD UNIT_FIELD_POSSTAT3 = 0x170;
	DWORD UNIT_FIELD_POSSTAT4 = 0x174;
	DWORD UNIT_FIELD_NEGSTAT0 = 0x178;
	DWORD UNIT_FIELD_NEGSTAT1 = 0x17C;
	DWORD UNIT_FIELD_NEGSTAT2 = 0x180;
	DWORD UNIT_FIELD_NEGSTAT3 = 0x184;
	DWORD UNIT_FIELD_NEGSTAT4 = 0x188;
	DWORD UNIT_FIELD_RESISTANCES = 0x18C;
	DWORD UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE = 0x1A8;
	DWORD UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE = 0x1C4;
	DWORD UNIT_FIELD_BASE_MANA = 0x1E0;
	DWORD UNIT_FIELD_BASE_HEALTH = 0x1E4;
	DWORD UNIT_FIELD_BYTES_2 = 0x1E8;
	DWORD UNIT_FIELD_ATTACK_POWER = 0x1EC;
	DWORD UNIT_FIELD_ATTACK_POWER_MODS = 0x1F0;
	DWORD UNIT_FIELD_ATTACK_POWER_MULTIPLIER = 0x1F4;
	DWORD UNIT_FIELD_RANGED_ATTACK_POWER = 0x1F8;
	DWORD UNIT_FIELD_RANGED_ATTACK_POWER_MODS = 0x1FC;
	DWORD UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER = 0x200;
	DWORD UNIT_FIELD_MINRANGEDDAMAGE = 0x204;
	DWORD UNIT_FIELD_MAXRANGEDDAMAGE = 0x208;
	DWORD UNIT_FIELD_POWER_COST_MODIFIER = 0x20C;
	DWORD UNIT_FIELD_POWER_COST_MULTIPLIER = 0x228;
	DWORD UNIT_FIELD_MAXHEALTHMODIFIER = 0x244;
	DWORD UNIT_FIELD_HOVERHEIGHT = 0x248;
	DWORD UNIT_FIELD_PADDING = 0x24C;
	DWORD  UNIT_FIELDS = 0x59;
} UnitOffsets;
DWORD game_path = 0xB32360;
//public enum WoWObjectType : uint
//{
//	Object = 0,
//	Item = 1,
//	Container = 2,
//	Unit = 3,
//	Player = 4,
//	GameObject = 5,
//	DynamicObject = 6,
//	Corpse = 7,
//	AiGroup = 8,
//	AreaTrigger = 9
//}