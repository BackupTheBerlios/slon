#include "Engine.h"
#include "Graphics/Common.h"
#include "Database/Collada/Collada.h"
#include "Graphics/Effect/PhillipsSpectrum.h"
#include "Graphics/Effect/WaterEffect.h"
#include "Graphics/PostProcess/FogFilter.h"
#include "Graphics/Renderable/ProjectedGrid.h"
#include "Graphics/Renderable/SkyBox.h"
#include "Scene/Camera/LookAtCamera.h"
#include "Scene/Camera/ReflectCamera.h"
#include "Scene/Light/DirectionalLight.h"
#include "Input/KeyboardHandler.h"
#include "Input/MouseHandler.h"
#include "Scene/Group.h"
#include <sgl/State.h>
#include <sgl/Font.h>
#include <sstream>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

using namespace math;
using namespace slon;
using namespace slon::scene;
using namespace slon::graphics;

bool        fullscreen;
bool        benchmark;
int         multisample;
int         fftSize;
Vector2ui   screenSize;
std::string demoModel;

// water spectrums
boost::shared_ptr<PhillipsSpectrum> phillipsSpectrum[4];

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

    ReflectCamera* createReflectCamera(const Camera& mainCamera, sgl::Device* device)
    {
        using sgl::Texture;

        // create camera
        ReflectCamera* reflectCamera = new ReflectCamera( mainCamera, math::Planef(0.0f, 1.0f, 0.0f, 0.0f) );
        reflectCamera->setViewport( sgl::rectangle(0, 0, 512, 512) );

        // create refract texture
        sgl::Texture2D* reflectTexture = 0;
        {
            sgl::Texture2D::DESC desc;
            desc.format     = Texture::RGBA8;
            desc.width      = 512;
            desc.height     = 512;
            desc.data       = 0;
            reflectTexture  = device->CreateTexture2D(desc);
        }

        sgl::SamplerState* samplerState = 0;
        {
            sgl::SamplerState::DESC desc;
            desc.filter[0] = sgl::SamplerState::LINEAR;
            desc.filter[1] = sgl::SamplerState::LINEAR;
            desc.filter[2] = sgl::SamplerState::LINEAR;

            desc.wrapping[0] = sgl::SamplerState::CLAMP;
            desc.wrapping[1] = sgl::SamplerState::CLAMP;

            samplerState = device->CreateSamplerState(desc);
        }
        reflectTexture->BindSamplerState(samplerState);
        reflectTexture->GenerateMipmap();

        // create render target
        sgl::RenderTarget* renderTarget = device->CreateRenderTarget();
        renderTarget->SetColorAttachment(0, reflectTexture, 0);
        renderTarget->SetDepthStencil(true);
        if ( sgl::SGL_OK != renderTarget->Dirty() ) {
            throw std::runtime_error("Can't create render target for reflections.");
        }
        reflectCamera->setRenderTarget(renderTarget);

        return reflectCamera;
    }

    SlaveCamera* createRefractCamera(const Camera& mainCamera, sgl::Device* device)
    {
        using sgl::Texture;

        // create camera
        SlaveCamera* refractCamera = new SlaveCamera(mainCamera);
        refractCamera->setViewport( sgl::rectangle(0, 0, 512, 512) );

        // create refract texture
        sgl::Texture2D* refractTexture = 0;
        {
            sgl::Texture2D::DESC desc;
            desc.format = Texture::RGBA8;
            desc.width  = 512;
            desc.height = 512;
            desc.data   = 0;
            refractTexture = device->CreateTexture2D(desc);
        }

        sgl::SamplerState* samplerState = 0;
        {
            sgl::SamplerState::DESC desc;
            desc.filter[0] = sgl::SamplerState::LINEAR;
            desc.filter[1] = sgl::SamplerState::LINEAR;
            desc.filter[2] = sgl::SamplerState::LINEAR;

            desc.wrapping[0] = sgl::SamplerState::CLAMP;
            desc.wrapping[1] = sgl::SamplerState::CLAMP;

            samplerState = device->CreateSamplerState(desc);
        }
        refractTexture->BindSamplerState(samplerState);
        refractTexture->GenerateMipmap();

        // create render target
        sgl::RenderTarget* renderTarget = device->CreateRenderTarget();
        renderTarget->SetColorAttachment(0, refractTexture, 0);
        //renderTarget->SetDepthStencilAttachment(depthTexture, 0);
        if ( sgl::SGL_OK != renderTarget->Dirty() ) {
            throw std::runtime_error("Can't create render target for refractions.");
        }
        refractCamera->setRenderTarget(renderTarget);

        return refractCamera;
    }

    sgl::Texture2D* createRefractTexture(sgl::Device* device)
    {
        // create refract texture
        sgl::Texture2D* refractTexture = 0;
        {
            sgl::Texture2D::DESC desc;
            desc.format = sgl::Texture::RGBA8;
            desc.width  = screenSize.x;
            desc.height = screenSize.y;
            desc.data   = 0;
            refractTexture = device->CreateTexture2D(desc);
        }

        sgl::SamplerState* samplerState = 0;
        {
            sgl::SamplerState::DESC desc;
            desc.filter[0]   = sgl::SamplerState::LINEAR;
            desc.filter[1]   = sgl::SamplerState::LINEAR;
            desc.filter[2]   = sgl::SamplerState::LINEAR;

            desc.wrapping[0] = sgl::SamplerState::CLAMP;
            desc.wrapping[1] = sgl::SamplerState::CLAMP;

            samplerState = device->CreateSamplerState(desc);
        }
        refractTexture->BindSamplerState(samplerState);
        refractTexture->GenerateMipmap();

        return refractTexture;
    }

    sgl::Texture2D* createDepthTexture(sgl::Device* device)
    {
        // create refract texture
        sgl::Texture2D* depthTexture = 0;
        {
            sgl::Texture2D::DESC desc;
            desc.format    = sgl::Texture::D24;
            desc.width     = screenSize.x;
            desc.height    = screenSize.y;
            desc.data      = 0;
            depthTexture   = device->CreateTexture2D(desc);
        }

        sgl::SamplerState* samplerState = 0;
        {
            sgl::SamplerState::DESC desc;
            desc.filter[0]   = sgl::SamplerState::NEAREST;
            desc.filter[1]   = sgl::SamplerState::NEAREST;
            desc.filter[2]   = sgl::SamplerState::NONE;

            desc.wrapping[0] = sgl::SamplerState::CLAMP;
            desc.wrapping[1] = sgl::SamplerState::CLAMP;

            samplerState = device->CreateSamplerState(desc);
        }
        depthTexture->BindSamplerState(samplerState);

        return depthTexture;
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
			using namespace slon::database;

			GraphicsManager& graphicsManager = currentGraphicsManager();
			DatabaseManager& databaseManager = currentDatabaseManager();

            // setup logging
            {
                log::currentLogManager().redirectOutput("database", "database_log.txt");
                log::currentLogManager().redirectOutput("graphics", "graphics_log.txt");
            }

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
            //FreeConsole();

			realm::World& world = realm::currentWorld();

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
            world.add(skyBox, false);

            // create scene
            {
                database::library_ptr library = database::loadLibrary("Data/Models/castle.DAE");
                world.add(library->getVisualScenes().front().second.get());

                // create light
                scene::DirectionalLight* light = new DirectionalLight();
                light->setDirection( normalize( Vector3f(-1.5f, -0.1f, 0.85f) ) );
                light->setColor( Vector4f(0.8f, 0.8f, 0.8f, 1.0f) );
                light->setAmbient(0.3f);
                light->setIntensity(1.5f);
                world.add(light, false);
            }

            // Create cameras & water surface
            {
                sgl::rectangle viewport(0, 0, windowWidth, windowHeight);
                camera.reset( createMainCamera(viewport) );

                // attach post effects
                fog_filter_ptr fogFilter( new FogFilter() );
                fogFilter->setFogColor( math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f) );
                fogFilter->setFogDensity(0.001f);
                fogFilter->setFogLevel(0.0f);
                fogFilter->setFogHeightFalloff(0.005f);
                camera->getPostEffectChain().push_back(fogFilter);

                sgl::Device* device = currentDevice();
                reflectCamera.reset( createReflectCamera(*camera, device) );

                graphicsManager.addCamera( reflectCamera.get() );
                graphicsManager.addCamera( camera.get() );

                // create water
                phillipsSpectrum[0].reset( new PhillipsSpectrum( 128,  Vector2f(3.0f, 0.0f), Vector2f(25.6f,  25.6f) ) );
                phillipsSpectrum[1].reset( new PhillipsSpectrum( 256,  Vector2f(4.0f, 0.0f), Vector2f(25.6f, 25.6f) ) );
                phillipsSpectrum[2].reset( new PhillipsSpectrum( 512,  Vector2f(5.0f, 5.0f), Vector2f(51.2f,  51.2f) ) );
                phillipsSpectrum[3].reset( new PhillipsSpectrum( 1024, Vector2f(5.0f, 5.0f), Vector2f(102.4f, 102.4f) ) );

                // get reflect/refract textures
                sgl::Texture2D* reflectTexture = static_cast<sgl::Texture2D*>( reflectCamera->getRenderTarget()->ColorAttachment(0) );
                sgl::Texture2D* refractTexture = createRefractTexture(device);
                sgl::Texture2D* depthTexture   = createDepthTexture(device);

                waterEffect.reset( new WaterEffect(phillipsSpectrum[1]) );
                waterEffect->setEnvironmentTexture( skyBox->getCubeMap() );
                waterEffect->setReflectCameraAndTexture(reflectCamera.get(), reflectTexture);
                waterEffect->setRefractTexture(refractTexture);
                waterEffect->setDepthTexture(depthTexture);
                waterEffect->setWaterTransparency(0.5f);
                waterEffect->setWaveSharpness(0.5f);
                waterEffect->setSqueezing(8.0f);
                waterEffect->setDistanceSmoothness(0.001f);

                // choose size according to the window
                int sizeX = static_cast<int>( 150 * (windowWidth / 800.0) );
                int sizeY = static_cast<int>( 600 * (windowHeight / 600.0) );

                ocean.reset( new ProjectedGrid(waterEffect) );
                ocean->setupGrid(sizeX, sizeY);
                world.add(ocean.get(), false);

                // setup special states
                sgl::RasterizerState::DESC desc;
                desc.cullMode = sgl::RasterizerState::BACK;
                desc.fillMode = sgl::RasterizerState::SOLID;
                backFaceCullState.reset( device->CreateRasterizerState(desc) );
                desc.cullMode = sgl::RasterizerState::FRONT;
                desc.fillMode = sgl::RasterizerState::SOLID;
                frontFaceCullState.reset( device->CreateRasterizerState(desc) );
            }

            camera->setPosition( Vector3f(60.0f, 20.0f, 0.0f) );
            camera->setDirection( Vector3f(-1.0f, -1.0f, 1.0f) );
            camera->setUp( Vector3f(0.0f, 1.0f, 0.0f) );

            // create font
            graphics::texture_ptr fontTexture = loadTexture("Data/Fonts/font.png");
            if ( graphics::Texture2D* texture = dynamic_cast<graphics::Texture2D*>(fontTexture.get()) )
			{
				font.reset( currentDevice()->CreateFont() );
				font->SetTexture(texture);
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

            keyboardHandler->connectKeyPressEventHandler( input::KEY_1, bind( &DemoScene::setupGrid, this, 0,  0.5f ) );
            keyboardHandler->connectKeyPressEventHandler( input::KEY_2, bind( &DemoScene::setupGrid, this, 1,  0.5f ) );
            keyboardHandler->connectKeyPressEventHandler( input::KEY_3, bind( &DemoScene::setupGrid, this, 2,  1.0f ) );
            keyboardHandler->connectKeyPressEventHandler( input::KEY_4, bind( &DemoScene::setupGrid, this, 3, 1.0f ) );

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

            keyboardHandler->connectKeyDownHandler( input::KEY_UP,      bind(&DemoScene::modifySharpness, this,  0.5f) );
            keyboardHandler->connectKeyDownHandler( input::KEY_DOWN,    bind(&DemoScene::modifySharpness, this, -0.5f) );

            // setup mouse input
            MouseHandler* mouseHandler = new MouseHandler();
            inputManager.addInputHandler(mouseHandler);

            mouseHandler->connectRelativeMouseMotionEventHandler( bind(&DemoScene::turnCamera, this, _1, _2) );
        }

        Engine::DESC desc;
        desc.multithreaded = false;
        desc.grabInput     = false;
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

    void setupGrid( unsigned int    spectrum,
                    float           sharpness )
    {
        waterEffect->setFrequencySpectrum(phillipsSpectrum[spectrum]);
        waterEffect->setWaveSharpness(sharpness);
    }

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

    void modifySharpness(float modifier)
    {
        waterEffect->setWaveSharpness( waterEffect->getWaveSharpness() + modifier* dt );
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
            ss << "1,2,3,4: - Different weather conditions" << endl
               << "up/down - increase/decrease wave sharpness: " << waterEffect->getWaveSharpness() << endl
               << "w,s,a,d,q,e - camera movement" << endl
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
    boost::intrusive_ptr<SlaveCamera>               reflectCamera;
    boost::intrusive_ptr<ProjectedGrid>             ocean;
    boost::intrusive_ptr<WaterEffect>               waterEffect;
    sgl::ref_ptr<sgl::Texture2D>                    refractTexture;

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
