#include "GameRunnable.h"



GameRunnable::GameRunnable() : OgreBites::ApplicationContext("SeaWolves")
{
}


GameRunnable::~GameRunnable()
{
}


bool GameRunnable::keyPressed(const OgreBites::KeyboardEvent& evt)
{
	if (evt.keysym.sym == OgreBites::SDLK_ESCAPE)
	{
		getRoot()->queueEndRendering();
	}
	return true;
}

bool GameRunnable::mouseMoved(const OgreBites::MouseMotionEvent &evt)
{
	return true;
}

void createSquare(Ogre::SceneManager* mSceneMgr, int width, int height, int edgeLength, std::string meshName, bool oddOrEven, Ogre::ColourValue color)
{
	Ogre::ManualObject* manual = NULL;
	{
		manual = mSceneMgr->createManualObject(meshName);
		bool doIwantToUpdateItLater = false;
		manual->setDynamic(doIwantToUpdateItLater);
		float triangleScale = 0.7f;

		manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);
		{
			int cm = width * triangleScale;
			int cp = height * triangleScale;
			edgeLength = edgeLength * triangleScale;

			manual->position(cm, 0, cp);// a vertex
			manual->colour(color);
			manual->position((cm - edgeLength), 0, cp);// a vertex
			manual->colour(color);
			manual->position((cm - edgeLength), 0, (cp + edgeLength));// a vertex
			manual->colour(color);
			manual->position(cm, 0, (cp + edgeLength));// a vertex
			manual->colour(color);

			manual->position(cm, 0, cp);// a vertex
			manual->colour(color);
			manual->position((cm - edgeLength), 0, cp);// a vertex
			manual->colour(color);
			manual->position((cm - edgeLength), 0, (cp + edgeLength));// a vertex
			manual->colour(color);
			manual->position(cm, 0, (cp + edgeLength));// a vertex
			manual->colour(color);

			//face behind / front
			manual->triangle(0, 1, 2);
			manual->triangle(2, 3, 0);
			manual->triangle(4, 6, 5);
			manual->triangle(6, 4, 7);

			//face top / down
			manual->triangle(0, 4, 5);
			manual->triangle(5, 1, 0);
			manual->triangle(2, 6, 7);
			manual->triangle(7, 3, 2);

			//face left / right
			manual->triangle(0, 7, 4);
			manual->triangle(7, 0, 3);
			manual->triangle(1, 5, 6);
			manual->triangle(6, 2, 1);

			manual->colour(Ogre::ColourValue::Red);
		}
		manual->end();

		Ogre::String resourceGroup = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;
		manual->convertToMesh(meshName);
	}
	Ogre::Entity* planeEntity = mSceneMgr->createEntity(meshName);
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(planeEntity);
	planeEntity->setCastShadows(false);
	if (oddOrEven) {
		planeEntity->setMaterialName("Examples/Rockwall");
	}
	else {
		planeEntity->setMaterialName("Examples/Water02");
	}
}

void createTileMap(Ogre::SceneManager* scnMgr)
{
	int gridWidth = 11;
	int gridHeight = 11;
	int edgeLength = 50;
	int xscale, yscale = 0;
	for (int i = 0; i < (gridHeight * gridWidth); ++i) {
		int temp = i;
		if ((temp % gridWidth) == 0) {
			xscale = edgeLength;
			yscale += edgeLength;
		}
		else {
			xscale += edgeLength;
		}
		//OutputDebugStringW(L"Into the Maw");
		std::stringstream meshName;
		meshName << i;
		std::string strToPass = meshName.str();

		if (temp % 2 == 0) {
			createSquare(scnMgr, xscale, yscale, edgeLength, strToPass, true, (Ogre::ColourValue::Green));
		}
		else {
			createSquare(scnMgr, xscale, yscale, edgeLength, strToPass, false, (Ogre::ColourValue::Red));
		}
	}
}

void GameRunnable::setup(void)
{	
	// do not forget to call the base first
	OgreBites::ApplicationContext::setup();

	addInputListener(this);

	Ogre::Root* root = getRoot();
	Ogre::ConfigDialog* dialog = OgreBites::getNativeConfigDialog();

	scnMgr = root->createSceneManager();
	Ogre::RTShader::ShaderGenerator* shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
	shadergen->addSceneManager(scnMgr);
	Ogre::SceneNode* camNode = scnMgr->getRootSceneNode()->createChildSceneNode("camNode");
	Ogre::Camera* cam = scnMgr->createCamera("myCam");
	
	camNode->setPosition(400, 600, 800);
	camNode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TransformSpace::TS_WORLD);
	cam->setNearClipDistance(5);
	camNode->attachObject(cam);

	Ogre::Viewport* vp = getRenderWindow()->addViewport(cam);
	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
	cam->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

	Ogre::Entity* robotEntity = scnMgr->createEntity("robot.mesh");
	robotEntity->setCastShadows(true);
	scnMgr->getRootSceneNode()->createChildSceneNode()->attachObject(robotEntity);

	createTileMap(scnMgr);

	scnMgr->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f));
	//scnMgr->setShadowTechnique(Ogre::ShadowTechnique::SHADOWTYPE_STENCIL_MODULATIVE);

	Ogre::Light* spotLight = scnMgr->createLight("SpotLight");
	spotLight->setDiffuseColour(0, 0, 1.0);
	spotLight->setSpecularColour(0, 0, 1.0);
	spotLight->setType(Ogre::Light::LT_SPOTLIGHT);
	Ogre::SceneNode* spotLightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
	spotLightNode->attachObject(spotLight);
	spotLightNode->setDirection(-1, -1, 0);
	spotLightNode->setPosition(Ogre::Vector3(200, 200, 0));
	spotLight->setSpotlightRange(Ogre::Degree(35), Ogre::Degree(50));
	
}
//----------------------------------------------------------------
void GameRunnable::frameRendered(const Ogre::FrameEvent& evt)
{
	Ogre::Vector3 transVector = Ogre::Vector3::ZERO;
	int mMoveScale = 250;
	
	int x, y;
	SDL_GetMouseState(&x, &y);
	
	if (x > (getRenderWindow()->getWidth() - 20)) {
		transVector.x += mMoveScale;
	} 
	else if (x < 20) {
		transVector.x -= mMoveScale;
	}
	else if (y > (getRenderWindow()->getHeight() - 20)) {
		transVector.y -= mMoveScale;
	}
	else if (y < 20) {
		transVector.y += mMoveScale;
	}
	
	scnMgr->getSceneNode("camNode")->translate(transVector * evt.timeSinceLastFrame, Ogre::Node::TS_LOCAL);
}



#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
	int main(int argc, char *argv[])
#endif
	{
		// Create application object
		GameRunnable app;

		try {
			app.initApp();
			app.getRoot()->startRendering();
			app.closeApp();
			return 0;
		}
		catch (Ogre::Exception& e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
			std::cerr << "An exception has occurred: " <<
				e.getFullDescription().c_str() << std::endl;
#endif
		}

		return 0;
	}

#ifdef __cplusplus
}
#endif