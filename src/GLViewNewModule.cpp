#include "GLViewNewModule.h"

#include "WorldList.h" //This is where we place all of our WOs
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "Axes.h" //We can set Axes to on/off with this
#include "PhysicsEngineODE.h"
#include "AftrGLRendererBase.h"

//Different WO used by this module
#include "WO.h"
#include "WOStatic.h"
#include "WOStaticPlane.h"
#include "WOStaticTrimesh.h"
#include "WOTrimesh.h"
#include "WOHumanCyborg.h"
#include "WOHumanCal3DPaladin.h"
#include "WOWayPointSpherical.h"
#include "WOLight.h"
#include "WOSkyBox.h"
#include "WOCar1970sBeater.h"
#include "Camera.h"
#include "CameraStandard.h"
#include "CameraChaseActorSmooth.h"
#include "CameraChaseActorAbsNormal.h"
#include "CameraChaseActorRelNormal.h"
#include "Model.h"
#include "ModelDataShared.h"
#include "ModelMesh.h"
#include "ModelMeshDataShared.h"
#include "ModelMeshSkin.h"
#include "WONVStaticPlane.h"
#include "WONVPhysX.h"
#include "WONVDynSphere.h"
#include "AftrGLRendererBase.h"
#include "../cwin64/WObanana.h"
#include "../cwin64/NetMsgBanana.h"

//If we want to use way points, we need to include this.
#include "NewModuleWayPoints.h"
#include <irrKlang.h>
#include <Windows.h>
#include <conio.h>
#include "../cwin64/NetMsgTxt.h"
#include "../cwin64/NetMsgBananer.h"

using namespace irrklang;
using namespace Aftr;
using namespace physx;

const std::string sharedMM = ManagerEnvironmentConfiguration::getSMM();

GLViewNewModule* GLViewNewModule::New( const std::vector< std::string >& args )
{
   GLViewNewModule* glv = new GLViewNewModule( args );
   glv->init( Aftr::GRAVITY, Vector( 0, 0, -1.0f ), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE );
   glv->onCreate();
   return glv;
}


GLViewNewModule::GLViewNewModule( const std::vector< std::string >& args ) : GLView( args )
{
   //ISoundEngine* Engine = createIrrKlangDevice();
   //Initialize any member variables that need to be used inside of LoadMap() here.
   //Note: At this point, the Managers are not yet initialized. The Engine initialization
   //occurs immediately after this method returns (see GLViewNewModule::New() for
   //reference). Then the engine invoke's GLView::loadMap() for this module.
   //After loadMap() returns, GLView::onCreate is finally invoked.

   //The order of execution of a module startup:
   //GLView::New() is invoked:
   //    calls GLView::init()
   //       calls GLView::loadMap() (as well as initializing the engine's Managers)
   //    calls GLView::onCreate()

   //GLViewNewModule::onCreate() is invoked after this module's LoadMap() is completed.
}


void GLViewNewModule::onCreate()
{
   //GLViewNewModule::onCreate() is invoked after this module's LoadMap() is completed.
   //At this point, all the managers are initialized. That is, the engine is fully initialized.
   
    this->Engine = createIrrKlangDevice();
    if (this->pe != NULL)
    {
        //optionally, change gravity direction and magnitude here
        //The user could load these values from the module's aftr.conf
        this->pe->setGravityNormalizedVector(Vector(0, 0, -1.0f));
        this->pe->setGravityScalar(Aftr::GRAVITY);
    }
    this->setActorChaseType(STANDARDEZNAV);
    this->physics = PxPhysicsManager::New();
    this->banana = WObanana::New();
    worldLst->push_back(this->banana->getbananaWO());


    /*
    //Play the 2D music (background) - Banana Phone song - from youtube - link in assignment video
                    and 3D music (on location) - Given Soundtrack
    ISoundSource* noise1 = Engine->addSoundSourceFromFile("../irrKlang/irrKlang-64bit-1.6.0/media/getout.ogg");
    noise1->setDefaultVolume(0.9f);
    //call the pointer to play the 3D music
    ISound* music2 = Engine->play3D(noise1, vec3df(50, 50, 50), true);
    //music->setIsPaused(false);
    //music->setMinDistance(5.0f);
    */
    ISoundSource* banana1 = Engine->addSoundSourceFromFile("../irrKlang/irrKlang-64bit-1.6.0/media/Riffi_Bananaphone.mp3");
    banana1->setDefaultVolume(2.0f);
    ISound* music2 = Engine->play2D(banana1, true);
    this->initTxt = "Hit the Enter key to type a message";
    this->worldTxt = this->getInitialWorldTxt();
    worldLst->push_back(this->worldTxt);


    //This is where I initialize the multiple ports that is able to run the multiple instances
    if (ManagerEnvironmentConfiguration::getVariableValue("NetServerLisenPort") == "12683") {
        this->net = NetMessengerClient::New("127.0.0.1", "12682");
    }
    else {
        this->net = NetMessengerClient::New("127.0.0.1", "12683");
    }

    //Default is STANDARDEZNAV mode
   //this->setNumPhysicsStepsPerRender( 0 ); //pause physics engine on start up; will remain paused till set to 1
}


GLViewNewModule::~GLViewNewModule()
{
   //Implicitly calls GLView::~GLView()
}


void GLViewNewModule::updateWorld()
{
    //set listener position
    Engine->setListenerPosition(
        /*Position*/vec3df(cam->getPosition().x, cam->getPosition().y, cam->getPosition().z),
        /*Look Direction*/vec3df(cam->getLookDirection().x, cam->getLookDirection().y, cam->getLookDirection().z),
        /*Velocity*/vec3df(0, 0, 0),
        /*Normal Direction\upVector*/ vec3df(cam->getNormalDirection().x, cam->getNormalDirection().y, cam->getNormalDirection().z)
    );
    Engine->setRolloffFactor(0.1f);
    if (this->isMoving()) {
        this->banana->update();
    }
    if (this->net->isTCPSocketOpen()) {
        if (this->worldTxt->getText() != this->initTxt) {
            this->net->sendNetMsgSynchronousTCP(NetMsgTxt(this->worldTxt->getText()));
        }
        if (this->isMoving()) {
            this->net->sendNetMsgSynchronousTCP(NetMsgBanana(this->banana->getPosition(), this->banana->getLookDirection()));
        }
    }
    this->physics->simulate(this->net, this->bananas);
    GLView::updateWorld(); //Just call the parent's update world first.
                          //If you want to add additional functionality, do it after
                          //this call.   
}


void GLViewNewModule::onResizeWindow( GLsizei width, GLsizei height )
{
   GLView::onResizeWindow( width, height ); //call parent's resize method.
}


void GLViewNewModule::onMouseDown( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseDown( e );
}


void GLViewNewModule::onMouseUp( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseUp( e );
}


void GLViewNewModule::onMouseMove( const SDL_MouseMotionEvent& e )
{
   GLView::onMouseMove( e );
}


void GLViewNewModule::onKeyDown( const SDL_KeyboardEvent& key )
{
    if (!this->typing) {
        GLView::onKeyDown(key);
            if (key.keysym.sym == SDLK_0)
        this->setNumPhysicsStepsPerRender(1);

        //slide whistle controls
        if (key.keysym.sym == SDLK_COMMA)
        {
            Engine->play2D("../irrKlang/irrKlang-64bit-1.6.0/media/whistle_up.mp3");
        }
        else if (key.keysym.sym == SDLK_PERIOD)
        {
            Engine->play2D("../irrKlang/irrKlang-64bit-1.6.0/media/whistle_down.mp3");
        }
        //recognizes moving banana
        if (this->isMoving()) { this->banana->onKeyDown(key); }




        //camera controls
        SDL_Keycode pressed = key.keysym.sym;
        if (pressed == SDLK_w && this->cam != nullptr) {
            this->cam->moveInLookDirection(10);
        }
        if (pressed == SDLK_s && this->cam != nullptr) {
            this->cam->moveOppositeLookDirection(10);
        }
        if (pressed == SDLK_a && this->cam != nullptr) {
            this->cam->moveLeft();
            this->cam->moveLeft();
            this->cam->moveLeft();
            this->cam->moveLeft();
            this->cam->moveLeft();
            this->cam->moveLeft();
        }
        if (pressed == SDLK_d && this->cam != nullptr) {
            this->cam->moveRight();
            this->cam->moveRight();
            this->cam->moveRight();
            this->cam->moveRight();
            this->cam->moveRight();
            this->cam->moveRight();
        }
    }

    //
    this->KeyPress(key.keysym.sym);
}


void GLViewNewModule::onKeyUp( const SDL_KeyboardEvent& key )
{
   GLView::onKeyUp( key );
   if (this->isMoving()) { this->banana->onKeyUp(key); }
}


void Aftr::GLViewNewModule::loadMap()
{
   this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
   this->actorLst = new WorldList();
   this->netLst = new WorldList();

   std::vector<std::string> skyBoxImageNames;

   skyBoxImageNames.push_back(sharedMM + "/images/skyboxes/space_Hubble_Nebula+6.jpg");
   {
       ManagerLight::setGlobalAmbientLight(aftrColor4f(4.0f, 3.0f, 2.0f, 1.0f));
       WOLight* light = WOLight::New();
       light->isDirectionalLight(true);
       light->setPosition(Vector(0, 0, 100));
       light->getModel()->setDisplayMatrix(Mat4::rotateIdentityMat({ 0,1,0 }, 90.0f * Aftr::DEGtoRAD));
       light->setLabel("Light");
       worldLst->push_back(light);
   }

   ManagerOpenGLState::GL_CLIPPING_PLANE = 1000.0;
   ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
   ManagerOpenGLState::enableFrustumCulling = false;
   Axes::isVisible = true;
   this->glRenderer->isUsingShadowMapping( false ); //set to TRUE to enable shadow mapping, must be using GL 3.2+

   this->cam->setPosition( 0,0,10 );

   std::string shinyRedPlasticCube( ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl" );
   std::string wheeledCar( ManagerEnvironmentConfiguration::getSMM() + "/models/rcx_treads.wrl" );
   std::string grass( ManagerEnvironmentConfiguration::getSMM() + "/models/grassFloor400x400_pp.wrl" );
   std::string human( ManagerEnvironmentConfiguration::getSMM() + "/models/human_chest.wrl" );
      
   //SkyBox Textures readily available
   //skyBoxImageNames.push_back(ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_warp+6.jpg");
   
   float ga = 0.1f; //Global Ambient Light level for this module
   ManagerLight::setGlobalAmbientLight( aftrColor4f( ga, ga, ga, 1.0f ) );
   WOLight* light = WOLight::New();
   light->isDirectionalLight( true );
   light->setPosition( Vector( 100, 100, 100 ) );
   //Set the light's display matrix such that it casts light in a direction parallel to the -z axis (ie, downwards as though it was "high noon")
   //for shadow mapping to work, this->glRenderer->isUsingShadowMapping( true ), must be invoked.
   light->getModel()->setDisplayMatrix( Mat4::rotateIdentityMat( { 0, 1, 0 }, 90.0f * Aftr::DEGtoRAD ) );
   light->setLabel( "Light" );
   worldLst->push_back( light );

   //Create the SkyBox
   WO* wo = WOSkyBox::New( skyBoxImageNames.at( 0 ), this->getCameraPtrPtr() );
   wo->setPosition( Vector( 0,0,0 ) );
   wo->setLabel( "Sky Box" );
   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   worldLst->push_back( wo );

   ////Create the infinite grass plane (the floor)
   wo = WO::New( grass, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
   wo->setPosition( Vector( 0, 0, 0 ) );
   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   ModelMeshSkin& grassSkin = wo->getModel()->getModelDataShared()->getModelMeshes().at( 0 )->getSkins().at( 0 );
   grassSkin.getMultiTextureSet().at( 0 )->setTextureRepeats( 5.0f );
   grassSkin.setAmbient( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Color of object when it is not in any light
   grassSkin.setDiffuse( aftrColor4f( 1.0f, 1.0f, 1.0f, 1.0f ) ); //Diffuse color components (ie, matte shading color of this object)
   grassSkin.setSpecular( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Specular color component (ie, how "shiney" it is)
   grassSkin.setSpecularCoefficient( 10 ); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
   wo->setLabel( "Grass" );
   worldLst->push_back( wo );


   //This is the 1st object that I added to my world
   
   //ModelMeshSkin& bananaSkin = banana->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0);
   //bananaSkin.setColor(aftrColor4ub(Vector(235.0f, 235.0f, 52.0f)));
   
   //this is the position and rendering of my object
  
   /*
   //this is another object I added
   WO* box = WO::New("../mm/models/woodencrate.dae", Vector(2, 2, 2));
   //and this is the rendering and position of that second object
   box->setPosition(Vector(-50, -50, 0));
   box->renderOrderType = RENDER_ORDER_TYPE::roBLEND_FUNC_GL_ONE;
   worldLst->push_back(box);
   */
   createNewModuleWayPoints();
}

bool GLViewNewModule::isMoving() {
    return this->banana != nullptr;
}


void GLViewNewModule::createNewModuleWayPoints()
{
   
   // Create a waypoint with a radius of 3, a frequency of 5 seconds, activated by GLView's camera, and is visible.
   WayPointParametersBase params(this);
   params.frequency = 5000;
   params.useCamera = true;
   params.visible = true;
   WOWayPointSpherical* wayPt = WOWP1::New( params, 3 );
   //x is red, y is green, z is blue
   wayPt->setPosition( Vector( 50, 50, 50 ) );
   worldLst->push_back( wayPt );
   
}

WOFTGLString* GLViewNewModule::getInitialWorldTxt() {
    WOFTGLString* worldString = WOFTGLString::New(sharedMM + "/fonts/primemin.ttf", 72);
    worldString->setPosition(500, 0, 500);
    worldString->setLabel("World Text");
    worldString->setText(this->initTxt);
    worldString->getModel()->setLookDirection(Vector(0, -1, 0));
    worldString->getModel()->setNormalDirection(Vector(-1, 0, 0));
    worldString->getModel()->setScale(Vector(1000, 1000, 1000));
    return worldString;
}

void GLViewNewModule::KeyPress(const SDL_Keycode& key) {
    if (this->typing) {
        switch (key) {
            case SDLK_RETURN:
                this->typing = false;
                break;
            case SDLK_LSHIFT:
            case SDLK_RSHIFT:
                if (!this->shift) {
                    this->shift = true;
                }
                break;
            case SDLK_TAB:
                net->sendNetMsgSynchronousTCP(NetMsgTxt(this->worldTxt->getText()));
                break;
            case SDLK_ESCAPE:
                this->typing = false;
                break;
            default:
                this->type(key);
        }
        return;
    }
    switch (key) {
        case SDLK_RETURN:
            this->typing = true;
            if (this->worldTxt->getText() == this->initTxt) {
                this->worldTxt->setText("");
            }
            return;
        case SDLK_b:
            this->Bananer();
            break;
        default: break;
    }
}

void GLViewNewModule::type(const SDL_Keycode& key) {
    std::string text = this->worldTxt->getText();
    if (key == SDLK_BACKSPACE) {
        if (text.size() < 2) {
            text = "";
        }
        else {
            text.pop_back();
        }
    }
    else if (this->shift) {
        text += this->getShift(key);
    }
    else {
        text += key;
    }
    this->worldTxt->setText(text);
}

char GLViewNewModule::getShift(char c) {
    if (c < 123 && c > 96) {
        return (c - 32);
    }
    switch (c) {
    case 39: return 34;// ' = "
    case 44: return 60;// , = <
    case 45: return 95;// - = _
    case 46: return 62;// . = >
    case 47: return 63;// / = ?
    case 48: return 41;// 0 = )
    case 49: return 33;// 1 = !
    case 50: return 64;// 2 = @
    case 51: return 35;// 3 = #
    case 52: return 36;// 4 = $
    case 53: return 37;// 5 = %
    case 54: return 94;// 6 = ^
    case 55: return 38;// 7 = &
    case 56: return 42;// 8 = *
    case 57: return 40;// 9 = (
    case 59: return 58;// ; = :
    case 61: return 43;// = = +
    case 91: return 123;// [ = {
    case 92: return 124;// \ = |
    case 93: return 125;// ] = }
    case 96: return 126;// ` = ~
    default: return c;

    }
}

void GLViewNewModule::Bananer(Vector pos) {
    if (pos == Vector(0, 0, 0)) {
        pos = this->cam != nullptr ? this->cam->getPosition() : Vector(0, 0, 0);
    }
    PxTransform t = PxTransform(PxVec3(pos.x, pos.y, pos.z));
    PxShape* shape = this->physics->physics->createShape(PxBoxGeometry(2.0f, 2.0f, 2.0f), *this->physics->physics->createMaterial(0.5f, 0.3f, 0.2f));
    PxRigidDynamic* actor = PxCreateDynamic(*this->physics->physics, t, *shape, 10.0f);
    
    PhysX* wo = PhysX::New(actor, "../mm/models/banana.dae", Vector(15, 15, 15));
    wo->actor->userData = wo;
    wo->setPosition(pos);
    worldLst->push_back(wo);
    this->physics->scene->addActor(*wo->actor);

    if (this->net != nullptr && this->net->isTCPSocketOpen()) {
        NetMsgBananer msg;
        msg.pos = pos;
        this->net->sendNetMsgSynchronousTCP(msg);
    }
    this->bananas.insert(std::pair(wo, numBananas));
    numBananas++;
    
}


/*
//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_water+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_dust+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_mountains+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_winter+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/early_morning+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_afternoon+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy3+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day2+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_deepsun+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_evening+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning2+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_noon+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_warp+6.jpg" ); - current using this one
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_Hubble_Nebula+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_gray_matter+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_easter+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_hot_nebula+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_ice_field+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_lemon_lime+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_milk_chocolate+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_solar_bloom+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_thick_rb+6.jpg" );
*/
