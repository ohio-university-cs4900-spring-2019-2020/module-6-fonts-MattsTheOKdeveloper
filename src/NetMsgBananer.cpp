#include "NetMsgBananer.h"

#include <iostream>
#include <sstream>
#include <string>
#include "AftrManagers.h"
#include "Vector.h"
#include "GLView.h"
#include "GLViewNewModule.h"
#include "PhysX.h"
#include "WorldContainer.h"


#ifdef AFTR_CONFIG_USE_BOOST

using namespace Aftr;

NetMsgMacroDefinition(NetMsgBananer);

bool NetMsgBananer::toStream(NetMessengerStreamBuffer& os) const {
	os << pos.x << pos.y << pos.z;
	return true;
}

bool NetMsgBananer::fromStream(NetMessengerStreamBuffer& is) {
	float x, y, z;
	is >> x >> y >> z;
	this->pos = Vector(x, y, z);
	return true;
}

void NetMsgBananer::onMessageArrived() {
	((GLViewNewModule*)ManagerGLView::getGLView())->Bananer(this->pos);
}

std::string NetMsgBananer::toString() const {
	std::stringstream ss;
	ss << NetMsg::toString();
	ss << "   Message: " << this->pos << std::endl;

	return ss.str();
}

#endif