#include "GameRunnable.h"



GameRunnable::GameRunnable() 
	: OgreBites::ApplicationContext("SeaWolves"),
	mTrayMgr(0),
	mScnMgr(0),
	mCam(0),
	mWindow(0),
	mRayScnQuery(0),
	robotAnimState(NULL),
	mUnitNode(0),
	mWalkList(0),
	mWalkSpeed(0),
	mDirection(0, 0, 0), //direction the object is moving
	mDistance(0), // distance object has left to travel
	mDestination(0, 0, 0), // destination the object is moving towards
	robotEntity(NULL)
{
}


GameRunnable::~GameRunnable()
{
}


void GameRunnable::createSquare(int width, int height, int edgeLength, std::string meshName, bool oddOrEven, Ogre::ColourValue color)
{
	Ogre::ManualObject* manual = NULL;
	{
		manual = mScnMgr->createManualObject(meshName);
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
	Ogre::Entity* planeEntity = mScnMgr->createEntity(meshName);
	mScnMgr->getRootSceneNode()->createChildSceneNode()->attachObject(planeEntity);
	planeEntity->setCastShadows(false);
	if (oddOrEven) {
		planeEntity->setMaterialName("Examples/Rockwall");
	}
	else {
		planeEntity->setMaterialName("Examples/Water02");
	}
}


void GameRunnable::createTileMap(void)
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
		
		std::stringstream meshName;
		meshName << i;
		std::string strToPass = meshName.str();

		if (temp % 2 == 0) {
			createSquare(xscale, yscale, edgeLength, strToPass, true, (Ogre::ColourValue::Green));
		}
		else {
			createSquare(xscale, yscale, edgeLength, strToPass, false, (Ogre::ColourValue::Red));
		}
	}
}


void GameRunnable::gridSquareCordFinder(std::string squareName)
{
	int squareNumber = stoi(squareName);
	int edgeLength = 50;
	int dimension = 11;
	float fedge = edgeLength * 0.7f;
	int x = 0;
	int z = 0;
	int xcount = 0;
	int ycount = 0;
	int total = 0;
	bool end = false;
	for (xcount; xcount < dimension; xcount++) {
		total = xcount;
		while (total <= squareNumber) {
			if (total == squareNumber) {
				end = true;
				break;
			}
			else {
				total += dimension;
				ycount++;
			}
		}
		if (end) {
			break;
		}
		else {
			ycount = 0;
		}
	}

	x = (xcount * fedge) + (fedge * 0.5);
	z = (ycount * fedge) + (fedge * 1.5);
	mWalkList.push_back(Ogre::Vector3(x, 0, z));

	std::ostringstream oss;
	oss << "Object is: " << squareNumber << " X: " << xcount << " Y: " << ycount;
	//mCordPanel->setText(oss.str());
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
	//mTrayMgr->refreshCursor();
	return true;
}


bool GameRunnable::mousePressed(const OgreBites::MouseButtonEvent &evt)
{
	bool mbRight = false;
	
	if (evt.button == SDL_BUTTON_RIGHT) {
		mbRight = true;
	}
	
	Ogre::String objectName = "";
	Ogre::Ray mouseRay = mTrayMgr->getCursorRay(mCam);

	mRayScnQuery->setRay(mouseRay);
	Ogre::RaySceneQueryResult& result = mRayScnQuery->execute();
	Ogre::RaySceneQueryResult::iterator it = result.begin();

	for (; it != result.end(); it++)
	{
		objectName = it->movable->getName();
	}

	if (mbRight) {
		gridSquareCordFinder(objectName);
		//mWalkList.push_back(Ogre::Vector3(x, 0, y));
	}

	return true;
}


void GameRunnable::setup(void)
{	
	// do not forget to call the base first
	OgreBites::ApplicationContext::setup();
	addInputListener(this);

	Ogre::Root* root = getRoot();
	Ogre::ConfigDialog* dialog = OgreBites::getNativeConfigDialog();
	OgreBites::TrayListener* mTrayListener = new OgreBites::TrayListener();

	mScnMgr = root->createSceneManager();
	mScnMgr->addRenderQueueListener(getOverlaySystem());
	mTrayMgr = new OgreBites::TrayManager("InterfaceName", getRenderWindow(), mTrayListener);
	mTrayMgr->showFrameStats(OgreBites::TrayLocation::TL_BOTTOMLEFT);

	addInputListener(mTrayMgr);

	Ogre::RTShader::ShaderGenerator* shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
	shadergen->addSceneManager(mScnMgr);
	camNode = mScnMgr->getRootSceneNode()->createChildSceneNode("camNode");
	mCam = mScnMgr->createCamera("myCam");
	
	camNode->setPosition(500, 750, 1000);
	camNode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TransformSpace::TS_WORLD);
	mCam->setNearClipDistance(5);
	camNode->attachObject(mCam);

	Ogre::Viewport* vp = getRenderWindow()->addViewport(mCam);
	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
	mCam->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
	root->addFrameListener(this);

	robotEntity = mScnMgr->createEntity("robot.mesh");
	robotEntity->setCastShadows(true);
	mUnitNode = mScnMgr->getRootSceneNode()->createChildSceneNode("RobotNode", Ogre::Vector3(15.0f, 0.0f, 50.0f));
	mUnitNode->attachObject(robotEntity);
	robotAnimState = robotEntity->getAnimationState("Idle");
	robotAnimState->setLoop(true);
	robotAnimState->setEnabled(true);

	createTileMap();

	mScnMgr->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f));
	//scnMgr->setShadowTechnique(Ogre::ShadowTechnique::SHADOWTYPE_STENCIL_MODULATIVE);

	Ogre::Light* spotLight = mScnMgr->createLight("SpotLight");
	spotLight->setDiffuseColour(0, 0, 1.0);
	spotLight->setSpecularColour(0, 0, 1.0);
	spotLight->setType(Ogre::Light::LT_SPOTLIGHT);
	Ogre::SceneNode* spotLightNode = mScnMgr->getRootSceneNode()->createChildSceneNode();
	spotLightNode->attachObject(spotLight);
	spotLightNode->setDirection(-1, -1, 0);
	spotLightNode->setPosition(Ogre::Vector3(200, 200, 0));
	spotLight->setSpotlightRange(Ogre::Degree(35), Ogre::Degree(50));	
	
	mRayScnQuery = mScnMgr->createRayQuery(Ogre::Ray());
	mRayScnQuery->setSortByDistance(true);
}
//----------------------------------------------------------------
void GameRunnable::frameRendered(const Ogre::FrameEvent& evt)
{
	/* Camera movement */
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

	/* Robot animation and movement */
	if (robotAnimState != NULL) {
		if (mDirection == Ogre::Vector3::ZERO) {

			if (nextLocation()) {
				Ogre::Vector3 src = mUnitNode->getOrientation() * Ogre::Vector3::UNIT_X;

				if ((1.0 + src.dotProduct(mDirection)) < 0.0001)
				{
					mUnitNode->yaw(Ogre::Degree(180));
				}
				else
				{
					Ogre::Quaternion quat = src.getRotationTo(mDirection);
					mUnitNode->rotate(quat);
				}
				robotAnimState = robotEntity->getAnimationState("Walk");
				robotAnimState->setLoop(true);
				robotAnimState->setEnabled(true);
			}

		}
		else {
			Ogre::Real move = mWalkSpeed * evt.timeSinceLastFrame;
			mDistance -= move;
			if (mDistance <= 0.0f) {
				mUnitNode->setPosition(mDestination);
				mDirection = Ogre::Vector3::ZERO;

				// Consider adding rotation code (intermediate tutorial1)
				robotAnimState = robotEntity->getAnimationState("Idle");
				robotAnimState->setLoop(true);
				robotAnimState->setEnabled(true);

			}
			else {
				mUnitNode->translate(move * mDirection);
			}
		}


		robotAnimState->addTime(evt.timeSinceLastFrame);
	}
	
	mScnMgr->getSceneNode("camNode")->translate(transVector * evt.timeSinceLastFrame, Ogre::Node::TS_LOCAL);
}


bool GameRunnable::nextLocation()
{
	if (mWalkList.empty()) { return false; }
	mDestination = mWalkList.front();  // this gets the front of the deque
	mWalkList.pop_front();             // this removes the front of the deque
	mDirection = mDestination - mUnitNode->getPosition();
	mDistance = mDirection.normalise();
	return true;
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