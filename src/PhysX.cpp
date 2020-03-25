#include "PhysX.h"

#ifdef AFTR_CONFIG_USE_BOOST

using namespace Aftr;
using namespace physx;

PhysX* PhysX::New(physx::PxRigidActor* actor, const std::string& modelFileName, Vector scale, MESH_SHADING_TYPE shadingType) {
	PhysX* wo = new PhysX(actor);
	wo->onCreate(modelFileName, scale, shadingType);
	return wo;
}

PhysX::PhysX(physx::PxRigidActor* actor) : IFace(this), WO() {
	this->actor = actor;
}

void Physx::onCreate(const std::string& modelFileName, Vector scale, MESH_SHADING_TYPE shadingType) {
	WO::onCreate(modelFileName, scale, shadingType);
}

#endif