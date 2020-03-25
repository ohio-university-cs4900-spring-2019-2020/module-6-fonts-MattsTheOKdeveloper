#pragma once

#include "WO.h"
#include "Model.h"
#include <irrKlang.h>

#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr {

	class WObanana {
	public:
		static WObanana* New(
			const std::string modelFileName = "../mm/models/banana.dae",
			Vector scale = Vector(1, 1, 1),
			MESH_SHADING_TYPE shadingType = MESH_SHADING_TYPE::mstSMOOTH,
			std::string label = "banana",
			Vector pos = Vector(-50, -50, 7)
		);
		~WObanana();

		//User Keyboard Input Specific
		//-----------------------------------
		void onKeyDown(const SDL_KeyboardEvent& key);
		void onKeyUp(const SDL_KeyboardEvent& key);
		void onMouseWheelScroll(const SDL_MouseWheelEvent& e);
		void onMouseDown(const SDL_MouseButtonEvent& e);
		void onMouseUp(const SDL_MouseButtonEvent& e);
		void onMouseMove(const SDL_MouseMotionEvent& e);

		void setSpeed(float newSpeed) { this->speed = newSpeed; }
		float getSpeed() { return this->speed; }

		Vector getPosition() { return banana->getPosition(); }
		Vector getLookDirection() { return banana->getLookDirection(); };
		Vector getNormalDirection() { return banana->getNormalDirection(); };
		void setPosition(const Vector& newXYZ) { banana->setPosition(newXYZ); };
		void setPosition(float x, float y, float z) { banana->setPosition(Vector(x, y, z)); };
		void setLookDirection(const Vector& lookDirection) { banana->getModel()->setLookDirection(lookDirection); }
		void setNormalDirection(const Vector& normalDirection) { banana->getModel()->setNormalDirection(normalDirection); }
		void moveRelative(const Vector& dxdydz) { banana->moveRelative(dxdydz); };
		// Moves the banana forward or backwards by the distance
		void move(float distance = 0.1f);
		void back(float distance = 0.1f);
		// Proccess the keys pressed
		void update();
		WO* getbananaWO() { return banana; };

		bool isMoving();
		float lookAngle = 0;
		float yaw = 0;


	protected:
		WO* banana;
		std::set<SDL_Keycode> keysPressed;
		float speed, roll;
		float distanceFrombanana = 32.28f;
		float angleAround = 0;
		float pitch = 16.1892f;

		WObanana(
			const std::string modelFileName = "../mm/models/banana.dae",
			Vector scale = Vector(1, 1, 1),
			MESH_SHADING_TYPE shadingType = MESH_SHADING_TYPE::mstSMOOTH,
			std::string label = "banana",
			Vector pos = Vector(-50, -50, 7)
		);

	private:
		bool isMovementKey(SDL_Keycode key);
	};

}

#endif