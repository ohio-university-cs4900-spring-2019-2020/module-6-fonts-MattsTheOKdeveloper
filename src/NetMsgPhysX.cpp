#include "NetMsgPhysX.h"
#include <iostream>
#include <sstream>
#include <string>
#include "GLViewNewModule.h"
#include "PhysX.h"
#include "WorldContainer.h"

#ifdef AFTR_CONFIG_USE_BOOST
using namespace Aftr;

NetMsgMacroDefinition(NetMsgPhysX);

bool NetMsgPhysX::toStream(NetMessengerStreamBuffer& os) const {
	os << pos.x << pos.y << pos.z;
	os << pose[0] << pose[1] << pose[2] << pose[3];
	os << pose[4] << pose[5] << pose[6] << pose[7];
	os << pose[8] << pose[9] << pose[10] << pose[11];
	os << pose[12] << pose[13] << pose[14] << pose[15];
	os << index;
	return true;
}

bool NetMsgPhysX::fromStream(NetMessengerStreamBuffer& is) {
	float x, y, z;
	is >> x >> y >> z;
	pos = Vector(x, y, z);
	is >> pose[0] >> pose[1] >> pose[2] >> pose[3];
	is >> pose[4] >> pose[5] >> pose[6] >> pose[7];
	is >> pose[8] >> pose[9] >> pose[10] >> pose[11];
	is >> pose[12] >> pose[13] >> pose[14] >> pose[15];
	is >> index;
	return true;
}

void NetMsgPhysX::onMessageArrived() {
	for (auto& i : ((GLViewNewModule*)ManagerGLView::getGLView())->bananas) {
		if (i.second == this->index) {
			PhysX* wo = i.first;
			wo->setDisplayMatrix(this->pose);
			wo->setPosition(this->pos);
			std::cout << this->toString() << std::endl;
		}
	}
}

std::string NetMsgPhysX::toString() const {
	std::stringstream ss;
	ss << NetMsg::toString();
	ss << "Message: " << std::endl;
	ss << "\tPosition:\t" << this->pos << std::endl;
	ss << "\tPose:\t\t" << this->pose << std::endl;
	ss << "\tIndex:\t\t" << this->index << std::endl;

	return ss.str();
}
#endif