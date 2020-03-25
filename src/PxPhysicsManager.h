#pragma once
#include "PxPhysicsAPI.h"
#include "NetMessengerClient.h"
#include <map>

namespace Aftr {
	class PhysX;
	class ModelDataSharedID;

	class PxPhysicsManager {
	public:
		PxPhysicsManager();
		static PxPhysicsManager* New() { return new PxPhysicsManager(); };
		~PxPhysicsManager();
		void simulate(NetMessengerClient* net, std::map<PhysX*, int> bananas);
		physx::PxPhysics* physics;
		physx::PxScene* scene;

	private:
		physx::PxFoundation* foundation;
		physx::PxU32 version = PX_PHYSICS_VERSION;
		physx::PxDefaultErrorCallback err;
		physx::PxDefaultAllocator alloc;
		physx::PxPvd* pvd;
		physx::PxCooking* cook;
		physx::PxDefaultCpuDispatcher* dispatch;
		physx::PxMaterial* mats;
		physx::PxPvdSceneClient* client;
	};
}