#include "Engine.h"
#include "Graphics/Light/PointLight.h"
#include "Graphics/Renderer/ForwardRenderer.h"
#include "Graphics/GUI/ProgressBar.h"
#include "SceneGraph/TraverseVisitor.h"
#include "Database/Collada.h"
#include <SDL.h>
#include <SDL_main.h>
#include "Network.h"

// platform specified
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>

#undef CreateFont

using namespace std;
using namespace net;
using namespace gmath;
using namespace slon;
using namespace graphics;
using namespace gui;
using boost::asio::ip::tcp;

struct Player
{
    int health;
};

typedef boost::intrusive_ptr<sg::MatrixTransform> matrix_transform_ptr;

// settings
unsigned	screenWidth;
unsigned	screenHeight;
float		fps;
bool		working;
bool        restart;
bool		keys[1024];

// game
int     ourId;
Player  players[2];

// scene
boost::intrusive_ptr<ForwardRenderer>      renderer;
boost::intrusive_ptr<sg::MatrixTransform>  sceneRoot;

// gui
sgl::ref_ptr<sgl::Font>         font;
boost::shared_ptr<ProgressBar>  powerGauge;

// states
bool        gameStarted;
bool		freeLook;
int         win;
bool        ourTurn;
float       chargeTime;
bool        fire;
bool        isCharging;
Vector3f	position;
Vector3f	direction;
Vector3f	up;

sgl::ref_ptr<sgl::ClearColorState>       clearColorState;
sgl::ref_ptr<sgl::VertexColorState>      vertexColorState;
sgl::ref_ptr<sgl::ProjectionMatrixState> projectionTransform;

// game data
std::vector<matrix_transform_ptr>   objects;
int                                 canonId;
int                                 kernelId;
int                                 playerHealth[2];
matrix_transform_ptr		        canonTransform;
Vector3f                            canonDirection;

// settings
bool fixedPipeline;

// network
boost::asio::io_service		ioService;
tcp::socket					castlesSocket(ioService);

void HandleEvents(float time)
{
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

        case SDL_MOUSEBUTTONDOWN:
            if (ourTurn)
            {
                isCharging = true;
                chargeTime = time;
            }
            break;

        case SDL_MOUSEBUTTONUP:
            if (isCharging)
            {
                fire = true;
                isCharging = false;
            }
            break;

        case SDL_MOUSEMOTION:
        {
			if (event.motion.x != screenWidth/2 || event.motion.y != screenHeight/2)
			{
				Vector2f rel = Vector2f( (float)-event.motion.xrel, (float)event.motion.yrel );
				direction = Mat3f::rotate_y(rel.x * 0.002f) * direction;
				SDL_WarpMouse(screenWidth/2, screenHeight/2);

				float value = sqrtf(canonDirection.x * canonDirection.x + canonDirection.z * canonDirection.z);
				canonDirection = Vector3f(direction.x * value, canonDirection.y, direction.z * value);
				canonDirection = Mat3f::rotate( -rel.y * 0.002f, cross(canonDirection, Vector3f(0.0, 1.0, 0.0)) ) * canonDirection;
				canonDirection = clamp( canonDirection, Vector3f(-1.0f, 0.2f, -1.0f), Vector3f(1.0f, 0.7f, 1.0f) );

				normalize(canonDirection);
			}
            break;
        }

        default:
            break;
        }
    }

    // fire if power is to much
    if (isCharging && time - chargeTime >= 5.0f) 
    {
        fire = true;
        isCharging = false;
    }
}

void HandleInput(float time)
{
    static float lastTime = time;
	float dt = time - lastTime;
    if ( keys[SDLK_w] ) {
        position += direction * 60.0f * dt;
    }
    if ( keys[SDLK_s] ) {
        position -= direction * 60.0f * dt;
    }
    if ( keys[SDLK_a] ) {
        position += cross(up, direction) * 60.0f * dt;
    }
    if ( keys[SDLK_d] ) {
        position -= cross(up, direction) * 60.0f * dt;
    }
    if ( keys[SDLK_r] ) {
        position += up * 60.0f * dt;
    }
    if ( keys[SDLK_f] ) {
        position -= up * 60.0f * dt;
    }
    if ( keys[SDLK_SPACE] )
    {
		freeLook = !freeLook;
        keys[SDLK_SPACE] = false;
	}
    if ( keys[SDLK_1] )
    {
		freeLook = false;
        keys[SDLK_1] = false;
	}
    if ( keys[SDLK_2] )
    {
		freeLook = true;
        keys[SDLK_2] = false;
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
    {
	    ostringstream ss;
	    ss << "FPS: " << fps << endl;

	    font->Bind(10, 12);
	    font->Print( screenWidth / 2.0f - 40.0f, 10.0f, ss.str() );
	    font->Unbind();
    }

    vertexColorState->SetColor(1.0, 0.5, 0.0, 1.0);
    vertexColorState->Setup();

    // draw our health
    {
        ostringstream ss;
	    ss << "Player HP: " << playerHealth[ourId] << endl;
	    font->Bind(15, 20);
	    font->Print( 30.0f, 20.0f, ss.str() );
	    font->Unbind();
    }

    // draw opponent health
    {
        ostringstream ss;
	    ss << "Opponent HP: " << playerHealth[1 - ourId] << endl;
	    font->Bind(15, 20);
	    font->Print( screenWidth - 200.0f, 20.0f, ss.str() );
	    font->Unbind();
    }

    // draw turn notice
    if (ourTurn)
    {
        float opacity = (sin(time * 0.2f) + 1.0f) / 2.0f;
        vertexColorState->SetColor(0.0, 1.0, 0.0, opacity);
        vertexColorState->Setup();

        font->Bind(20, 30);
        font->Print(screenWidth / 2.0f - 50.0f, 30.0f, "Fire Now!");
        font->Unbind();
    
        if (isCharging) {
            powerGauge->SetValue( (time - chargeTime) / 5.0f );
        }
        else {
            powerGauge->SetValue(0.0f);
        }
        powerGauge->Draw();
    }

    if (win > 0)
    {
        vertexColorState->SetColor(0.0f, 1.0f, 0.0f, 1.0f);
        vertexColorState->Setup();

        font->Bind(10, 12);
        font->Print(5.0f, 5.0f, "You are victorious!");
        font->Unbind();
    }
    else if (win < 0)
    {
        vertexColorState->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
        vertexColorState->Setup();

        font->Bind(10, 12);
        font->Print(5.0f, 5.0f, "You loose!");
        font->Unbind();
    }
}

void RenderScene(float time)
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

    //renderer->renderScene();
	RenderCommon(time);

	SDL_GL_SwapBuffers();
}

void UpdateScene(float /*time*/)
{
	if ( !freeLook )
	{
		position = ( canonTransform->getLocalToWorldTransform() * invert( sceneRoot->getTransformMatrix() ) ).translation();
		Matrix4f cameraTransform = Mat4f::look_at( position - direction * 30.0f + Vector3f(0.0, 7.0f, 0.0f),
												   position + Vector3f(0.0f, 6.0f, 0.0f),
												   up );
		sceneRoot->setTransformMatrix(cameraTransform);

		position = canonTransform->getTransformMatrix().translation();
		canonTransform->setTransformMatrix( Mat4f::frame(position, canonDirection, up) );
	}
	else
	{
		Matrix4f cameraTransform = Mat4f::look_at(position, position + direction, up);
		sceneRoot->setTransformMatrix(cameraTransform);
	}
}

void ConnectToServer(const std::string& serverName)
{
	// connect
    tcp::resolver resolver(ioService);
	tcp::resolver::query query(serverName, castles_service);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;

    // connect
    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoint_iterator != end)
    {
		castlesSocket.close();
		castlesSocket.connect(*endpoint_iterator++, error);
    }

	if (error) {
		throw boost::system::system_error(error);
	}

    // now we must get the greeting message
    if ( read_header(castlesSocket) == player_state_desc::header )
    {
        player_state_desc desc;
        desc.read(castlesSocket);
        if ( desc.state != player_state_desc::CREATE ) {
            throw std::runtime_error("Server doesn't send the greeting message");
        }

        // init players
        ourId = desc.id;
        players[0].health = players[1].health = 100;
    }
    else {
        throw std::runtime_error("Server doesn't send the greeting message");
    }
}

bool ReadServerMessages(float /*time*/)
{
    // get server messages
    if ( castlesSocket.available() > 0 )
    {
        if ( !gameStarted ) {
            gameStarted = true;
        }

        // receive sync stamp
        read_sync_stamp(castlesSocket);

        bool busy = true;
        while (busy)
        {
            switch( read_header(castlesSocket) )
            {
                case object_transform_desc::header:
                {
                    // find object & add it to the scene
                    object_transform_desc desc;
                    desc.read(castlesSocket);

                    assert( static_cast<size_t>(desc.id) < objects.size() && desc.id >= 0 );
                    if ( static_cast<size_t>(desc.id)  >= objects.size() || desc.id < 0 ) 
                    {
                        cerr << "invalid object transfom desc - ignored" << endl;
                        break;
                    }

                    objects[desc.id]->setTransformMatrix( desc.matrix );
                    break;
                }
                case player_state_desc::header:
                {
                    player_state_desc desc;
                    desc.read(castlesSocket);
                    if (desc.state == player_state_desc::TURN) {
                        ourTurn = true;
                    }
                    else if (desc.state == player_state_desc::WIN)
                    {
                        win = 1;
                    }
                    else if (desc.state == player_state_desc::LOOSE)
                    {
                        win = -1;
                    }
                    else if (desc.state == player_state_desc::EXIT)
                    {
                        // server restart
                        working = false;
                        restart = true;
                    }

                    break;
                }
                case player_damage_desc::header:
                {
                    player_damage_desc desc;
                    desc.read(castlesSocket);
                    playerHealth[desc.id] = desc.health;
                    break;
                }
                case object_desc::header:
                {
                    // find object & add it to the scene
                    object_desc desc;
                    desc.read(castlesSocket);
                    
                    assert( objects.size() == desc.id );
                    if ( objects.size() != desc.id ) 
                    {
                        cerr << "invalid object desc - ignored" << endl;
                        break;
                    }

                    sg::MatrixTransform* transform = dynamic_cast<sg::MatrixTransform*>( sg::findNamedNode(sceneRoot.get(), desc.nodeName) );
                    if ( desc.clone )
                    {
                        sg::MatrixTransform* newTransform = new sg::MatrixTransform();
                        newTransform->setTransformMatrix(desc.matrix);
                        for(size_t i = 0; i<transform->getNumChildren(); ++i)
                        {
                            graphics::Mesh* mesh = dynamic_cast<graphics::Mesh*>( transform->getChild(i) );
                            if (mesh)
                            {
                                graphics::Mesh* clone = new graphics::Mesh(*mesh);
                                newTransform->addChild(clone);
                            }
                        }
                        sceneRoot->addChild(newTransform);
                        objects.push_back( matrix_transform_ptr(newTransform) );
                    }
                    else
                    {
                        if ( desc.id != kernelId )
                        {
                            transform->getParent()->removeChild(transform);
                            sceneRoot->addChild(transform);
                        }
                        objects.push_back( matrix_transform_ptr(transform) );
                    }

                    break;
                }

                case sync_stamp:
                {
                    busy = false;
                    break;
                }

                default:
                    throw std::runtime_error("Unexpected server message");
            }
        }

        return true;
    }

    return false;
}

void handle_write()
{
}

void SynchroniseWithServer(float time)
{
    // get server messages
    static float lastSync = time;
    if (gameStarted)
    {
        lastSync = time;

        std::string message = make_message(sync_stamp);

        object_transform_desc desc;
        desc.id = canonId;
        desc.matrix = canonTransform->getTransformMatrix();
        message += desc.message();

        // if fired
        if (fire)
        {
            fire_canon_desc desc;
            desc.playerId = ourId;
            desc.impulse = (time - chargeTime + 1.0f) * 10000.0f;
            message += desc.message();
            ourTurn = fire = false;
        }

        message += make_message(sync_stamp);

        boost::asio::write(castlesSocket, boost::asio::buffer(message));
    }
}

void CreateScene()
{
	boost::shared_ptr<Engine> pEngine = Engine::Instance();

    // create renderer
    renderer.reset( new graphics::ForwardRenderer() );

    // create scene root
    sceneRoot.reset( new sg::MatrixTransform() );

	sgl::Light* light = pEngine->sglDevice()->CreateLight();
	light->SetAmbient( Vector4f(0.0f, 0.0f, 0.0f, 0.0f) );
	light->SetDiffuse( Vector4f(1.0f, 1.0f, 1.0f, 1.0f) );
	light->SetSpecular( Vector4f(1.0f, 1.0f, 1.0f, 1.0f) );
	light->SetPosition( Vector4f(-1000.0f, 1000.0f, 600.0f, 1.0f) ) ;
    sceneRoot->addChild( new PointLight(light) );

    // transform
    clearColorState.reset( pEngine->sglDevice()->CreateClearColorState() );
    vertexColorState.reset( pEngine->sglDevice()->CreateVertexColorState() );
    projectionTransform.reset( pEngine->sglDevice()->CreateProjectionMatrixState() );
	projectionTransform->SetMatrix( Mat4f::perspective(0.7853982f, 1.33f, 1.0f, 2000.0f) );
    projectionTransform->Setup();

	position = Vector3f(0.0f, 50.0f, 0.0f);
	direction = canonDirection = Vector3f(0.0f, 0.0, 1.0f);
	up = Vector3f(0.0f, 1.0f, 0.0f);

    // create font
    font.reset( pEngine->sglDevice()->CreateFont() );
	font->LoadFromFile("data/fonts/font.png");

    // create gauge
    powerGauge.reset( new ProgressBar( pEngine->sglDevice() ) );
    powerGauge->SetFont( font.get() );
    powerGauge->SetText( "Fire power" );
    powerGauge->SetBorderColor(1.0f, 1.0f, 0.0f, 1.0f);
    powerGauge->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
    powerGauge->SetPosition(screenWidth / 4 + 40, screenHeight - 30);
    powerGauge->SetSize(screenWidth / 2, 15);

    // default health
    playerHealth[0] = playerHealth[1] = 100;
    ourTurn = false;

	// fill keys
	fill(keys, keys + 1024, false);

	// hide cursor
	SDL_ShowCursor(false);

    // read scene
    while ( castlesSocket.available() > 0 )
    {
        switch( read_header(castlesSocket) )
        {
            case canon_desc::header:
            {
                // find canon
                canon_desc desc;
                desc.read(castlesSocket);
                canonId = desc.id;

                break;
            }
            case player_state_desc::header:
            {
                player_state_desc desc;
                desc.read(castlesSocket);
                if (desc.state == player_state_desc::TURN) {
                    ourTurn = true;
                }
                break;
            }
            case kernel_desc::header:
            {
                // find kernel
                kernel_desc desc;
                desc.read(castlesSocket);
                kernelId = desc.id;

                break;
            }
            case sky_box_desc::header:
            {
                // create sky box
                sky_box_desc desc;
                desc.read(castlesSocket);

                SkyBox* skyBox = new SkyBox();
	            skyBox->MakeFromSideTextures(desc.maps);
	            sceneRoot->addChild(skyBox);

                break;
            }
            case mesh_desc::header:
            {
                // load mesh
                mesh_desc desc;
                desc.read(castlesSocket);
                db::LoadColladaScene( desc.fileName, *sceneRoot );
                sg::TraverseVisitor visitor;
                sceneRoot->accept(visitor);

                break;
            }
            case object_desc::header:
            {
                // find object & add it to the scene
                object_desc desc;
                desc.read(castlesSocket);
                assert( objects.size() == desc.id );

                sg::MatrixTransform* transform = dynamic_cast<sg::MatrixTransform*>( sg::findNamedNode(sceneRoot.get(), desc.nodeName) );
                transform->setTransformMatrix(desc.matrix);

                if ( desc.id != kernelId )
                {
                    transform->getParent()->removeChild(transform);
                    sceneRoot->addChild(transform);
                }
                objects.push_back( matrix_transform_ptr(transform) );

                break;
            }
            default:
                throw std::runtime_error("Unexpected server message");
        }
    }

    // find canon transform
    canonTransform = objects[canonId];

    // we have read all
    write_sync_stamp(castlesSocket);

    // setup data
    Engine::Instance()->setSceneRoot( sceneRoot.get() );

	cerr << "scene created" << endl;
}

void UpdateAll(float time)
{
    HandleEvents(time);
    bool newMessages = ReadServerMessages(time);
    UpdateScene(time);
    SynchroniseWithServer(time);
    HandleInput(time);
}

void Run()
{
    working = true;
    freeLook = false;
    gameStarted = false;
    restart = false;
    win = 0;
    while (working)
    {
	    float time = SDL_GetTicks() / 1000.0f;
        UpdateAll(time);
	    RenderScene(time);
    }

    // close connection
    if ( castlesSocket.is_open() ) 
    {
        write_sync_stamp(castlesSocket);

        player_state_desc desc;
        desc.id = ourId;
        desc.state = player_state_desc::EXIT;
        desc.write(castlesSocket);
        castlesSocket.close();
    }
}

int main(int argc, char** argv)
{
    //freopen("err.txt", "w", stderr);
	cout << "Initialization started" << endl;

	// create engine first
	boost::shared_ptr<Engine> pEngine = Engine::Instance();

	int multisample = 3;
	bool fullscreen = false;
    fixedPipeline = false;
    screenWidth = 800;
    screenHeight = 600;

	// check settings
	std::string serverName;
	for(int i = 1; i<argc; ++i)
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
		else if ( argstr == "-l" )
		{
			fixedPipeline = true;
		}
		else {
			serverName = argstr;
		}
	}

	if ( serverName.empty() )
	{
		cerr << "Usage: castles.exe <server> [-nm] [-f]" << endl;
		return 1;
	}

	try
	{
		ConnectToServer(serverName);
	    cerr << "connection to server established" << endl;
    }
	catch(boost::system::system_error& err)
	{
		cerr << "Application error: " << err.what() << endl;
		return 1;
	}

	pEngine->setVideoMode(screenWidth, screenHeight, 32, fullscreen, false, multisample);
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
/*
    boost::asdb::socket_base::send_buffer_size option(512);
    castlesSocket.set_option(option);

    boost::asdb::socket_base::receive_buffer_size option;
    castlesSocket.get_option(option);
    int size = option.value();
    cerr << "size: " << size << endl;
*/

    // run main loop
	try
	{
        restart = true;
        while (restart) 
        {
            // run main loop
            Run();
            if (restart) {
                ConnectToServer(serverName);
            }
        }
    }
	catch(std::exception& err)
	{
		cerr << "Application error: " << err.what()  << ". Player " << ourId << endl;
		return 0;
	}

    return 0;
}
