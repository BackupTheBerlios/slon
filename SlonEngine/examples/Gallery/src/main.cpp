#include "Engine.h"
#include "Graphics/Renderer/ForwardRenderer.h"
#include "Graphics/Light/PointLight.h"
#include "Graphics/Camera/LookAtCamera.h"
#include "Database/Collada.h"
#include "SDL.h"
#include "SDL_main.h"

// platform specified
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>

#undef CreateFont

using namespace std;
using namespace sgl::math;
using namespace slon;
using namespace graphics;

// settings
unsigned	screenWidth;
unsigned	screenHeight;
float		fps;
bool		working;
bool		keys[1024];

// scene
boost::shared_ptr<ForwardRenderer>  renderer;
boost::shared_ptr<scene::Group>        sceneRoot;
boost::shared_ptr<LookAtCamera>     camera;
sgl::ref_ptr<sgl::Light>            globalLight;

// gui
sgl::ref_ptr<sgl::Font>     font;

// states
sgl::ref_ptr<sgl::ClearColorState>       clearColorState;
sgl::ref_ptr<sgl::VertexColorState>      vertexColorState;
sgl::ref_ptr<sgl::ModelViewMatrixState>  modelViewTransform;
sgl::ref_ptr<sgl::ProjectionMatrixState> projectionTransform;

void HandleEvents()
{
	int warped = 0;

    // events
    SDL_Event event;
    while( SDL_PollEvent( &event ) )
    {
        switch( event.type )
        {
        /* SDL_QUIT event (window close) */
        case SDL_QUIT:
            working = false;
            break;

        case SDL_KEYDOWN:
            keys[event.key.keysym.sym] = true;
            break;

        case SDL_KEYUP:
            keys[event.key.keysym.sym] = false;
            break;

        case SDL_MOUSEMOTION:
        {
			if (!warped)
			{
				Vector2f rel = Vector2f( (float)event.motion.xrel, (float)event.motion.yrel );
                camera->turnYaw(rel.x * 0.002f);
				SDL_WarpMouse(screenWidth/2, screenHeight/2);
				warped = 2;
			}
			else {
				--warped;
			}
            break;
        }

        default:
            break;
        }
    }
}

void HandleInput(float time)
{
    static float lastTime = time;
	float dt = time - lastTime;
    if ( keys[SDLK_w] ) {
        camera->moveForward(20.0f * dt);
    }
    if ( keys[SDLK_s] ) {
        camera->moveForward(-20.0f * dt);
    }
    if ( keys[SDLK_d] ) {
        camera->moveRight(-20.0f * dt);
    }
    if ( keys[SDLK_a] ) {
        camera->moveRight(20.0f * dt);
    }
    if ( keys[SDLK_r] ) {
        camera->moveUp(20.0f * dt);
    }
    if ( keys[SDLK_f] ) {
        camera->moveUp(-20.0f * dt);
    }
    if ( keys[SDLK_ESCAPE] )
    {
        working = false;
        keys[SDLK_ESCAPE] = false;
    }
	lastTime = time;
}

void RenderCommon(float time)
{
	glDisable(GL_CULL_FACE);

	// font color
    vertexColorState->SetColor(1.0, 1.0, 0.0, 1.0);
    vertexColorState->Setup();

    static float lastTime = time;
    static int frames = 0;
    ++frames;
    if ( time - lastTime > 0.5f )
    {
        fps = 2.0f * frames / (1.0f + (time - lastTime - 0.5f) / 0.5f);
		frames = 0;
		lastTime = time;
    }

    // draw fps
	ostringstream ss;
	ss << "FPS: " << fps << endl;

	font->Bind(10, 12);
	font->Print( screenWidth / 2.0f - 40.0f, 10.0f, ss.str() );
	font->Unbind();
}

void RenderScene(float time)
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	// Update global light position
	float height = fabs( sin(time * 0.04f) );
	float dist = cos(time * 0.04f);
	globalLight->SetPosition( Vector4f(80.0f, height * 30.0f + 30.0f, dist * 30.0f, 1.0f) );

	Vector4f color = Vector4f( 1.0f - height * 0.5f, 0.7f, 0.7f, 2.0f) / 2.0f;
	globalLight->SetAmbient(color);
	globalLight->SetDiffuse(color);
	globalLight->SetSpecular(color);

	//renderer->renderScene();
	RenderCommon(time);

    SDL_GL_SwapBuffers();
}

void CreateScene()
{
	boost::shared_ptr<Engine> pEngine = Engine::Instance();

	//galleryMesh.LoadFromColladaFile("data/Models/torus.DAE", "Torus_Knot01");
    sceneRoot.reset( new scene::Group() );

	cerr << "starting scene loading" << endl;

	// create renderer
	renderer.reset( new ForwardRenderer() );

	db::LoadColladaScene("Data/Models/gallery.DAE", *sceneRoot);

    // create camera
    camera.reset( new LookAtCamera() );
    camera->setPosition( Vector3f(0.0f, 20.0f, 0.0f) );
    camera->setDirection( Vector3f(0.0f, 0.0f, 1.0f) );
    camera->setUp( Vector3f(0.0f, 1.0f, 0.0f) );

	cerr << "scene loaded" << endl;

    // States
    projectionTransform.reset( pEngine->sglDevice()->CreateProjectionMatrixState() );
    clearColorState.reset( pEngine->sglDevice()->CreateClearColorState() );
    vertexColorState.reset( pEngine->sglDevice()->CreateVertexColorState() );

	// light
	Vector4f ambient(0.5f, 0.5f, 0.5f, 1.0f);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, &ambient.x);

	globalLight.reset( pEngine->sglDevice()->CreateLight() );
	globalLight->SetAmbient( Vector4f(0.0f, 0.0f, 0.0f, 0.0f) );
	globalLight->SetPosition( Vector4f(80.0f, 30.0f, 30.0f, 1.0f) ) ;
    sceneRoot->addChild( new PointLight( globalLight.get() ) );

	sgl::Light* light = pEngine->sglDevice()->CreateLight();
	light->SetAmbient( Vector4f(0.0f, 0.0f, 0.0f, 0.0f) );
	light->SetDiffuse( Vector4f(0.4f, 0.4f, 0.4f, 1.0f) );
	light->SetPosition( Vector4f(-30.0f, 27.0f, 0.0f, 1.0f) ) ;
	sceneRoot->addChild( new PointLight(light) );

	light = pEngine->sglDevice()->CreateLight();
	light->SetAmbient( Vector4f(0.0f, 0.0f, 0.0f, 0.0f) );
	light->SetDiffuse( Vector4f(0.4f, 0.4f, 0.4f, 1.0f) );
	light->SetPosition( Vector4f(30.0f, 27.0f, 0.0f, 1.0f) ) ;
	sceneRoot->addChild( new PointLight(light) );

    // transform
    projectionTransform.reset( pEngine->sglDevice()->CreateProjectionMatrixState() );
	projectionTransform->SetMatrix( Mat4f::perspective(0.7853982f, 1.33f, 0.1f, 1000.0f) );
    projectionTransform->Setup();

	cerr << "scene created" << endl;

    // create font
    font.reset( pEngine->sglDevice()->CreateFont() );
	try
	{
		font->LoadFromFile("Data/Fonts/font.png");
	}
	catch(std::runtime_error& err)
	{
		cerr << "Couldnt' load font: " << err.what() << endl;
	}

	cerr << "font loaded" << endl;

	// fill keys
	fill(keys, keys + 1024, false);

	// hide cursor
	SDL_ShowCursor(false);
}

int main(int argc, char** argv)
{
	cout << "Initialization started" << endl;

	// create engine first
	boost::shared_ptr<Engine> pEngine = Engine::Instance();

	int multisample = 3;
	bool fullscreen = false;
    screenWidth = 800;
    screenHeight = 600;

	// check settings
	for(int i = 0; i<argc; ++i)
	{
		std::string argstr = std::string(argv[i]);
		if ( argstr == "-nm" ) {
			multisample = 0;
		}
		else if ( argstr == "-f" )
		{
			const SDL_VideoInfo* vInfo = SDL_GetVideoInfo();
			screenWidth = vInfo->current_w;
			screenHeight = vInfo->current_h;
			fullscreen = true;
		}
	}
	pEngine->getGraphicsManager().setVideoMode(screenWidth, screenHeight, 32, fullscreen, false, multisample);

	cerr << "video mode set" << endl;

	try
	{
		CreateScene();
	}
	catch(std::runtime_error& err)
	{
		cerr << "Application error: " << err.what() << endl;
		return 1;
	}

	cerr << "scene created" << endl;

	working = true;
	while (working)
	{
		float time = SDL_GetTicks() / 1000.0f;
		HandleEvents();
		RenderScene(time);
		HandleInput(time);
	}

    return 0;
}
