#define _DEBUG_NEW_REDEFINE_NEW 0
#include "Engine.h"
#include "Animation/Animation.h"
#include "Database/Collada/Collada.h"
#include "Graphics/Common.h"
#include "Graphics/PostProcess/FogFilter.h"
#include "Graphics/Renderable/SkyBox.h"
#include "Graphics/Renderer/ForwardRenderer.h"
#include "Input/KeyboardHandler.h"
#include "Input/MouseHandler.h"
#include "Realm/BVHLocation.h"
#include "Scene/LookAtCamera.h"
#include "Scene/ReflectCamera.h"
#include "Scene/DirectionalLight.h"
#include "Scene/Group.h"
#include "Thread/StartStopTimer.h"
#include <sgl/State.h>
#include <sgl/Font.h>
#include <sstream>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#ifdef WIN32
#include <windows.h>
#endif

using namespace math;
using namespace slon;
using namespace slon::scene;
using namespace slon::graphics;

bool        fullscreen;
bool        benchmark;
int         multisample;
int         fftSize;
Vector2ui   screenSize;

#undef CreateFont

namespace {

    LookAtCamera* createMainCamera(const sgl::rectangle& viewport)
    {
        // create camera
        LookAtCamera* camera = new LookAtCamera();
        camera->setViewport(viewport);
        camera->setProjectionMatrix( math::make_perspective( 0.7853982f,
                                                            static_cast<float>(viewport.width) / viewport.height,
                                                            1.0f,
                                                            4000.0f ) );

        return camera;
    }

} // anonymous namespace

class DemoScene
{
public:
    DemoScene( unsigned int windowWidth,
               unsigned int windowHeight,
               bool         fullscreen,
               unsigned int multisample ) :
        time(0.0f),
        fps(0.0f),
        dt(0.0f),
        drawHelp(true),
        needToTakeScreenshot(false)
    {
        Engine* engine = Engine::Instance();

        // setup graphics
        {
            using namespace slon::scene;
            using namespace slon::animation;

            // setup logging
            {
                log::currentLogManager().redirectOutput("database", "database_log.txt");
                log::currentLogManager().redirectOutput("graphics", "graphics_log.txt");
            }

            GraphicsManager& graphicsManager = engine->getGraphicsManager();
            graphicsManager.setVideoMode(windowWidth, windowHeight, 32, fullscreen, false, multisample);

            // create renderer
            {
                ForwardRendererDesc desc;
                desc.useDepthPass   = true;
                desc.makeDepthMap   = true;
                desc.useDebugRender = true;

                Renderer* renderer = graphicsManager.initRenderer(desc);
                renderer->connectPostRenderCallback( boost::bind(&DemoScene::OnPostRender, this, _1) );
            }
        #ifdef WIN32
            FreeConsole();
        #endif

            // create world
            realm::World*       world = realm::currentWorld();
            realm::location_ptr location(new realm::BVHLocation);
            world->addLocation(location);

            // Create skybox
            SkyBox* skyBox = new SkyBox();
            {
                const std::string SKY_BOX_MAPS[6] =
                {
                    "Data/SkyBox/sunset_west.jpg",
                    "Data/SkyBox/sunset_east.jpg",
                    "Data/SkyBox/sunset_up.jpg",
                    "Data/SkyBox/sunset_down.jpg",
                    "Data/SkyBox/sunset_south.jpg",
                    "Data/SkyBox/sunset_north.jpg"
                };
                skyBox->MakeFromSideTextures(SKY_BOX_MAPS);
            }
            world->addInfiniteNode(skyBox);

            // create scene
            {
                database::library_ptr library = database::loadLibrary("Data/Models/troll.dae");
                location->add(library->visualScenes.begin()->second.get());

				database::Library::key_animation_map animations = library->animations;
				if ( !animations.empty() )
				{
					StartStopTimer* timer = new StartStopTimer();
					timer->start();
					animations.begin()->second->play(timer);
				}

                // create light
                scene::DirectionalLight* light = new DirectionalLight();
                light->setDirection( normalize( Vector3f(-1.5f, -0.1f, 0.85f) ) );
                light->setColor( Vector4f(0.8f, 0.8f, 0.8f, 1.0f) );
                light->setAmbient(0.3f);
                light->setIntensity(1.5f);
                world->addInfiniteNode(light);
            }

            // Create camera
            {
                sgl::rectangle viewport(0, 0, windowWidth, windowHeight);
                camera.reset( createMainCamera(viewport) );
                graphicsManager.addCamera( camera.get() );
            }

            camera->setPosition( Vector3f(60.0f, 20.0f, 0.0f) );
            camera->setDirection( Vector3f(-1.0f, -1.0f, 1.0f) );
            camera->setUp( Vector3f(0.0f, 1.0f, 0.0f) );

            // create font
            font.reset( currentDevice()->CreateFont() );
            {
                sgl::Image* image = currentDevice()->CreateImage();
                image->LoadFromFile("Data/Fonts/font.png");
	            font->SetTexture( image->CreateTexture2D() );
            }
        }

        // setup input
        {
            using boost::bind;
            using namespace slon::input;
            using namespace slon::thread;

            InputManager& inputManager = engine->getInputManager();
            inputManager.showCursor(false);

            // fix cursor in the window center
            Vector2ui cursorPosition = screenSize / 2 ;
            inputManager.setCursorPosition(cursorPosition.x, cursorPosition.y);
            inputManager.fixCursorPosition(cursorPosition.x, cursorPosition.y);

            keyboardHandler.reset( new KeyboardHandler() );
            inputManager.addInputHandler( keyboardHandler.get() );

            keyboardHandler->connectKeyPressEventHandler( input::KEY_F1,     bind(&DemoScene::toggleHelpText,  this) );
            keyboardHandler->connectKeyPressEventHandler( input::KEY_F2,     bind(&DemoScene::toggleWireframe, this) );
            keyboardHandler->connectKeyPressEventHandler( input::KEY_F9,     bind(&DemoScene::takeScreenShot,  this) );
            keyboardHandler->connectKeyPressEventHandler( input::KEY_ESCAPE, bind(&DemoScene::stopDemo,        this) );

            keyboardHandler->connectKeyDownHandler( input::KEY_w,   bind(&DemoScene::flyCamera, this, math::Vector3f(  0.0f,   0.0f,  10.0f)) );
            keyboardHandler->connectKeyDownHandler( input::KEY_s,   bind(&DemoScene::flyCamera, this, math::Vector3f(  0.0f,   0.0f, -10.0f)) );
            keyboardHandler->connectKeyDownHandler( input::KEY_d,   bind(&DemoScene::flyCamera, this, math::Vector3f( 10.0f,   0.0f,   0.0f)) );
            keyboardHandler->connectKeyDownHandler( input::KEY_a,   bind(&DemoScene::flyCamera, this, math::Vector3f(-10.0f,   0.0f,   0.0f)) );
            keyboardHandler->connectKeyDownHandler( input::KEY_q,   bind(&DemoScene::flyCamera, this, math::Vector3f(  0.0f,  10.0f,   0.0f)) );
            keyboardHandler->connectKeyDownHandler( input::KEY_e,   bind(&DemoScene::flyCamera, this, math::Vector3f(  0.0f, -10.0f,   0.0f)) );

            // setup mouse input
            MouseHandler* mouseHandler = new MouseHandler();
            inputManager.addInputHandler(mouseHandler);

            mouseHandler->connectRelativeMouseMotionEventHandler( bind(&DemoScene::turnCamera, this, _1, _2) );
        }

        Engine::DESC desc;
        desc.multithreaded = false;
        desc.grabInput     = true;
        engine->run(desc);
    }

    ~DemoScene()
    {
    }

private:
    void toggleHelpText()   { drawHelp = !drawHelp; } // F1
    void toggleWireframe()  { currentRenderer()->toggleWireframe( !currentRenderer()->isWireframe() ); } // F2
    // Such a mess, because need to take screenshot after scene rendering
    void takeScreenShot()   { needToTakeScreenshot = true; } // F9
    void stopDemo()         { Engine::Instance()->stop(); } // ESC

    void turnCamera(int xrel, int yrel) // on mouse motion
    {
        const float bottom_threshold = -0.9f;
        const float top_threshold    =  0.5f;

        Vector2f rel = Vector2f( -static_cast<float>(xrel),
                                  static_cast<float>(yrel) );

        {
            thread::lock_ptr lock = camera->lockForWriting();
            camera->turnAroundAxis( rel.x * 0.002f, Vector3f(0.0f, 1.0f, 0.0f) );
            camera->turnPitch(-rel.y * 0.002f);

            Vector3f direction = camera->getDirection();
            if (direction.y < bottom_threshold)
            {
                direction.y = bottom_threshold;
                camera->setDirection( normalize(direction) );
            }
            else if (direction.y > top_threshold)
            {
                direction.y = top_threshold;
                camera->setDirection( normalize(direction) );
            }
        }
        //std::cout << "turn" << std::endl;
    }

    void flyCamera(const math::Vector3f& speed)    
    { 
        camera->moveForward(speed.z * dt); 
        camera->moveRight(speed.x * dt); 
        camera->moveUp(speed.y * dt); 
    }

    void renderCommon()
    {
        using namespace std;

        // calculate fps using frame render time
        const float fpsAdaptSpeed = 0.1f;
        fps = fps * (1.0f - fpsAdaptSpeed) + fpsAdaptSpeed / dt;

        // draw fps
        {
            ostringstream ss;
            ss << "FPS: " << fps << endl;

            font->Bind(10, 12);
            font->Print( (float)screenSize.x / 2.0f - 40.0f, 10.0f, ss.str().c_str() );
            font->Unbind();
        }

        // draw help text
        if (drawHelp)
        {
            ostringstream ss;
            ss << "w,s,a,d,q,e - camera movement" << endl
               //<< "F9 - take screenshot" << endl
               << "F1 - toggle help text" << endl
               << "F2 - wireframe";

            font->Bind(10, 12);
            font->Print( 10.0f, 10.0f, ss.str().c_str() );
            font->Unbind();
        }
    }

    void OnPostRender(const Camera& renderCamera)
    {
        using namespace std;

        if ( &renderCamera == camera )
        {
            // render gui
            dt   = static_cast<float>( Engine::Instance()->getSimulationTimer().getTime() - time );
            time = static_cast<float>( Engine::Instance()->getSimulationTimer().getTime() );
            //handleContiniousEvents();
            renderCommon();
/*
            // here we can take screenshot
            if ( needToTakeScreenshot )
            {
                // get time
                static int screenNum = 0;

                ostringstream numSS;
                numSS << screenNum++;
                string name = string("screen_") + numSS.str() + ".jpg";

                // screenshot
                sgl::ref_ptr<sgl::Image> image( currentDevice()->CreateImage() );
                //currentDevice()->TakeScreenshot( image.get() );
                image->SaveToFile( name.c_str() );

                needToTakeScreenshot = false;
            }
*/
        }
    }

private:
    // water
    boost::intrusive_ptr<LookAtCamera>              camera;

    // modes
    sgl::ref_ptr<sgl::RasterizerState>              backFaceCullState;
    sgl::ref_ptr<sgl::RasterizerState>              frontFaceCullState;

    // input
    boost::intrusive_ptr<input::KeyboardHandler>    keyboardHandler;

    // font
    sgl::ref_ptr<sgl::Font>                         font;

    // settings
    float   time;
    float   fps;
    float   dt;
    bool    drawHelp;
    bool    needToTakeScreenshot;
};

void ParseCommandLine(int argc, char** argv)
{
    using namespace std;

    cout << "Command line arguments:" << endl
         << "--window <width> <height> - window size(default 800x600)" << endl
         << "--fullscreen - enable fullscreen mode" << endl;
         //<< "--multisample <multisample> - multisample radius(default is 3)" << endl
         //<< "--benchmark <size> - catch performance of the fft. Performs two fft(size^2)" << endl << endl;

    for(int i = 0; i<argc; ++i)
    {
        string argStr = argv[i];
        if (argStr == "--window")
        {
            if ( i + 2 >= argc ) {
                throw std::runtime_error("Wrong number of arguments");
            }

            string input = string(argv[i+1]) + " " + argv[i+2];
            istringstream ss(input);
            ss >> screenSize.x >> screenSize.y;
        }
        else if (argStr == "--multisample")
        {
            if ( i + 1 >= argc ) {
                throw std::runtime_error("Wrong number of arguments");
            }

            istringstream ss(argv[i+1]);
            ss >> multisample;
        }
        else if (argStr == "--fullscreen") {
            fullscreen = true;
        }
        else if (argStr == "--benchmark")
        {
            if ( i + 1 >= argc ) {
                throw std::runtime_error("Wrong number of arguments");
            }

            istringstream ss(argv[i+1]);
            ss >> fftSize;
            benchmark = true;
        }
    }
}

int main(int argc, char** argv)
{
    screenSize = Vector2ui(800, 600);
    multisample = 0;
    fullscreen = false;
    benchmark = false;

    try
    {
        ParseCommandLine(argc, argv);
    }
    catch(std::exception& err)
    {
        std::cerr << err.what() << std::endl;
        return 1;
    }

    // init engine
	Engine* engine = Engine::Instance();
	engine->init();
/*
    if (benchmark)
    {
        GraphicsManager& graphicsManager = engine->getGraphicsManager();
        graphicsManager.setVideoMode(32, 32, 32, false, false, 3);

        ppu::FFTFilter fftFilter;
        fftFilter.setSize(fftSize);

        // check fft performance
        const int num_fft = 100;
        std::cout << "Performing " << num_fft << " FFT of size (" << fftSize << "," << fftSize << ") "
				  << "with 32 bit floating accuracy.\n This will take some time.\n";
        std::cout.flush(); // Flush, so user can see warning

        double startTime = engine->getTime();
        for (int i = 0; i < num_fft; ++i)
        {
            fftFilter.perform();
        }
        currentDevice()->Sync();
        double averageFFTTime = (engine->getTime() - startTime) / num_fft;

        std::cout << "Average fft perform time: " << averageFFTTime << std::endl;
    }
    else
*/
    {
        try
        {
            DemoScene demoScene(screenSize.x, screenSize.y, fullscreen, multisample);
        }
        catch(std::exception& err)
        {
            std::cerr << err.what() << std::endl;
            return 1;
        }
    }

	return 0;
}
