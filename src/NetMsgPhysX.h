#pragma once
#include "BigInclude.h"
#include "NetMsgMacroMethods.h"
#include "../../../include/aftr/NetMsg.h"

#ifdef AFTR_CONFIG_USE_BOOST
namespace Aftr {
	class NetMsgPhysX : public NetMsg {
	public:
		NetMsgMacroDeclaration(NetMsgPhysX);

		virtual ~NetMsgPhysX() {};
		virtual bool toStream(NetMessengerStreamBuffer& os) const;
		virtual bool fromStream(NetMessengerStreamBuffer& is);
		virtual void onMessageArrived();
		virtual std::string toString() const;

		Mat4 pose;
		Vector pos;
		int index;
	};
}
#endif