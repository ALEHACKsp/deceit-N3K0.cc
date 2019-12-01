#include "class.h"


 IRender* sdk::global_var::pRender = 0;
 IPhysicalWorld* sdk::global_var::pPhysicalWorld = 0;
 IGameFramework* sdk::global_var::pGameFramework = 0;
 IEntitySystem* sdk::global_var::pEntitySystem = 0;
 IConsole* sdk::global_var::pConsole = 0;
 void* sdk::global_var::pLog = 0;

#define pGameFramework_offset 0x1B458D0
#define pEntitySystem_offset  0x1B458E0
#define pRender_offset		  0x1B45930
#define pPhysicalWorld_offset 0x1B45870
#define pConsole_offset		  0x1B458E8
#define pLog_offset			  0x1B45910

 bool sdk::Initialize(bool dump) {
	 static DWORD64 game_base = reinterpret_cast<DWORD64>(GetModuleHandleA(0));
	 sdk::global_var::pRender        = reinterpret_cast<IRender*>        (*(DWORD64*)(game_base + pRender_offset));
	 sdk::global_var::pPhysicalWorld = reinterpret_cast<IPhysicalWorld*> (*(DWORD64*)(game_base + pPhysicalWorld_offset));
	 sdk::global_var::pGameFramework = reinterpret_cast<IGameFramework*> (*(DWORD64*)(game_base + pGameFramework_offset));
	 sdk::global_var::pEntitySystem  = reinterpret_cast<IEntitySystem*>  (*(DWORD64*)(game_base + pEntitySystem_offset));
	 sdk::global_var::pConsole		 = reinterpret_cast<IConsole*>		 (*(DWORD64*)(game_base + pConsole_offset));
	 sdk::global_var::pLog			 = reinterpret_cast<void*>			 (*(DWORD64*)(game_base + pLog_offset));

	 if (dump) {
		 printf("game_base -> %p \n", GetModuleHandleA(0));
		 printf("sdk::global_var::pRender -> %p \n",		sdk::global_var::pRender);
		 printf("sdk::global_var::pPhysicalWorld -> %p \n", sdk::global_var::pPhysicalWorld);
		 printf("sdk::global_var::pGameFramework -> %p \n", sdk::global_var::pGameFramework);
		 printf("sdk::global_var::pEntitySystem -> %p \n", sdk::global_var::pEntitySystem);
		 printf("sdk::global_var::pLog -> %p \n", sdk::global_var::pLog);

		 printf("Width : %d Height : %d \n", *(__int32*)(game_base + width_offset), *(__int32*)(game_base + height_offset));
	 }

	 return sdk::global_var::pRender		&& 
			sdk::global_var::pPhysicalWorld && 
			sdk::global_var::pGameFramework && 
			sdk::global_var::pEntitySystem;
 }



 Vector IEntity::get_bone_position(__int64 id) {
	 ICharacterInstance* pCharacterInstance = this->get_character(0);
	 if (pCharacterInstance != 0) {
		 ISkeletonPose* pSkeletonPose = pCharacterInstance->GetISkeletonPose();
		 if (pSkeletonPose != 0) {
			 QuatT bitch = pSkeletonPose->GetAbsJointByID(id);
			 Matrix34 World = this->m_worldTM;
			 Matrix34 SkeletonAbs = Matrix34(bitch);
			 return (World * SkeletonAbs).GetTranslation();
		 }
	 }
	 return Vector();
 }

 Vector IEntity::get_bone_position(const char* name) {
	 __int64 id = this->get_character(0)->GetIDefaultSkeleton()->GetJointIDByName(name);
	 return get_bone_position(id);
 }

 void IEntity::set_position(Vector pos) {
	 ((void(__fastcall*)(IEntity*, Vector*, int, unsigned __int8))((DWORD64)(GetModuleHandleA(0)) + 0xC51BF0))(this, &pos, 0, 0);
	 // I'm sorry for not use actual vtable I just found it from reverse ScriptBind_Entity::SetPos function
 }

 