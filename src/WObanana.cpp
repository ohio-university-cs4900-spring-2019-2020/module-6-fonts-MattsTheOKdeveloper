#include "WObanana.h"
#ifdef AFTR_CONFIG_USE_BOOST

#include "Camera.h"
#include "Model.h"
#include <complex>
#include "Mat4Fwd.h"
#include <irrKlang.h>

using namespace Aftr;
using namespace irrklang;
WObanana* WObanana::New(const std::string modelFileName, Vector scale, MESH_SHADING_TYPE shadingType, std::string label, Vector pos) {
	//WObanana* wow = new WObanana(modelFileName, scale, shadingType, label, pos);
	return new WObanana(modelFileName, scale, shadingType, label, pos);
}

WObanana::WObanana(const std::string modelFileName, Vector scale, MESH_SHADING_TYPE shadingType, std::string label, Vector pos) {
	this->speed = 1;
	this->roll = 0;
	this->banana = WO::New(modelFileName, scale, shadingType);
	this->banana->setLabel(label);
	this->banana->setPosition(pos);
}

WObanana::~WObanana() {
}

void WObanana::onKeyDown(const SDL_KeyboardEvent& key) {
	SDL_Keycode keyDown = key.keysym.sym;
	if (keyDown == SDLK_PLUS || keyDown == SDLK_EQUALS) {
		if (this->speed < 10) {
			++this->speed;
		}
		else if (this->speed < 0 || this->speed > 10) {
			//reset speed in case it becomes too large or negative
			this->speed = 0;
		}
		//else do nothing for now
	}
	if (keyDown == SDLK_MINUS) {
		if (this->speed > 0) {
			--this->speed;
		}
		else if (this->speed < 0 || this->speed > 10) {
			//reset speed in case it becomes too large or negative
			this->speed = 0;
		}
		//else do nothing for now
	}
	if (isMovementKey(keyDown)) {
		std::set<SDL_Keycode>::iterator found = keysPressed.find(keyDown);
		if (found == keysPressed.end()) {
			keysPressed.insert(keyDown);
		}
	}
}

void WObanana::onKeyUp(const SDL_KeyboardEvent& key) {
	SDL_Keycode keyUp = key.keysym.sym;
	if (isMovementKey(keyUp)) {
		std::set<SDL_Keycode>::iterator found = keysPressed.find(keyUp);
		if (found != keysPressed.end()) {
			keysPressed.erase(found);
		}
	}
}

void WObanana::onMouseDown(const SDL_MouseButtonEvent& e) {
	//if (e.button == 0) { this->pitch -= (e.y * 0.1f); } 
	//else if (e.button == 1) { this->angleAround = (e.x * 0.3f); }
}

void WObanana::onMouseUp(const SDL_MouseButtonEvent& e) {}
void WObanana::onMouseMove(const SDL_MouseMotionEvent& e) {}


void WObanana::update() {
	for (std::set<SDL_Keycode>::iterator it = this->keysPressed.begin(); it != this->keysPressed.end(); ++it) {
		if (*it == SDLK_UP) {
			this->move(this->speed);
		}
		if (*it == SDLK_LEFT) {
			this->banana->rotateAboutRelZ(0.1f);
		}
		if (*it == SDLK_DOWN) {
			this->back(this->speed);
		}
		if (*it == SDLK_RIGHT) {
			this->banana->rotateAboutRelZ(-0.1f);
		}
	}
	//this->calculateCameraPos(this->calculateHorizontal(), this->calculateVertical());
	//this->calculateLookAngle();
}

void WObanana::move(float distance) {
	for (int i = 0; i < distance; ++i) {
		this->moveRelative(this->getLookDirection());
	}
}

void WObanana::back(float distance) {
	for (int i = 0; i < distance; ++i) {
		this->moveRelative(this->getLookDirection() * -1);
	}
}

bool WObanana::isMovementKey(SDL_Keycode key) {
	return (key == SDLK_UP || key == SDLK_LEFT || key == SDLK_DOWN || key == SDLK_RIGHT ||
		key == SDLK_w || key == SDLK_a || key == SDLK_s || key == SDLK_d);
}

bool WObanana::isMoving() {
	return this->keysPressed.find(SDLK_UP) != this->keysPressed.end()
		|| this->keysPressed.find(SDLK_w) != this->keysPressed.end()
		|| this->keysPressed.find(SDLK_DOWN) != this->keysPressed.end()
		|| this->keysPressed.find(SDLK_s) != this->keysPressed.end();
}

#endif