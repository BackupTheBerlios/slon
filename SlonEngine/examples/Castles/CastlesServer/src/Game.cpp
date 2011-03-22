#include "Game.h"
#include "Database/Collada.h"
#include <SDL.h>
#include <boost/thread.hpp>

using namespace net;
using namespace std;

Game::Game()
{
    playerTurn = 0;
    allGetNecessary[0] = allGetNecessary[1] = true;
    restartGame = false;
    sendTurnMessage = true;

    CreateScene();
}

void Game::CreateScene()
{
    //sgl::ref_ptr<sgl::Light>	globalLight;
    Scene::scene_desc desc;
    desc.canonName[0] = "Canon_01-node";
    desc.canonName[1] = "Canon_02-node";
    desc.castleName[0] = "Castle_01-node";
    desc.castleName[1] = "Castle_02-node";
    desc.kernelName[0] = "Kernel_01-node_PIVOT";
    desc.kernelName[1] = "Kernel_02-node_PIVOT";
    desc.sceneFileName = "data/Models/castles.DAE";
    desc.skyBoxMaps[0] = "data/SkyBox/thunder_east.jpg";
    desc.skyBoxMaps[1] = "data/SkyBox/thunder_west.jpg";
    desc.skyBoxMaps[2] = "data/SkyBox/thunder_up.jpg";
    desc.skyBoxMaps[3] = "data/SkyBox/thunder_down.jpg";
    desc.skyBoxMaps[4] = "data/SkyBox/thunder_north.jpg";
    desc.skyBoxMaps[5] = "data/SkyBox/thunder_south.jpg";
    scene.Reset(desc);
}

void Game::Run()
{
    while(!restartGame)
    {
        float time = SDL_GetTicks() / 1000.0f;
        scene.AdvanceSimulation(time);
    }
}

/*
void Game::Run()
{
    // network
    boost::asio::io_service	ioService;

    // wait for players to start the game
    playerTurn = 0;
    WaitForPlayers(ioService);

    // run emulation
    bool exit = false;
    int fps = 0;
    allGetNecessary[0] = allGetNecessary[1] = true;
    gameEnded = false;
    sendTurnMessage = true;

    
    while(!exit) 
    {
        float time = SDL_GetTicks() / 1000.0f;

        scene.AdvanceSimulation(time);

        ++fps;
        static float lastTime = time;
        if ( time - lastTime > 1.0f ) 
        {
            lastTime = time;
            cerr << "fps: " << fps << std::endl;
            fps = 0;
        }

        if ( !players[0]->socket.is_open()
             || !players[1]->socket.is_open() ) 
        {
            break;
        }

        SynchroniseWithPlayer(players[0], time);
        SynchroniseWithPlayer(players[1], time);
    }
}
*/

void tcp_game_server::start_accept()
{
    tcp_connection::pointer newConnection = tcp_connection::create( game, acceptor.io_service() );
    acceptor.async_accept( newConnection->get_socket(),
                           boost::bind( &tcp_game_server::handle_accept, this, newConnection,
                                        boost::asio::placeholders::error ) );
}

void tcp_game_server::handle_accept( tcp_connection::pointer          connection,
                                     const boost::system::error_code& error )
{
    if (!error)
    {
        // try to add player
        Player player;
        player.id = game->players.size();
		
	    // check client
        try
        {
            cerr << "Player is trying to connect" << endl;

            // send greeting message
            player_state_desc desc;
            desc.state = player_state_desc::CREATE;
            desc.id = player.id; 
            desc.write( connection->get_socket() );

            // send scene description to the player
            game->scene.TranslateToClient( connection->get_socket(), player.id );

            // get response from the player that he is ready
            read_sync_stamp( connection->get_socket() );

            cerr << "Player connected" << endl;
        }
        catch (std::runtime_error& err)
        {
            cerr << "Player doesn't connected: " << err.what() << endl;
            return;
        }

        // add player & connection for it
        game->players.push_back(player);
        connections.push_back(connection);

        if ( game->players.size() < Game::num_players ) 
        {  
            // wait other players
            start_accept();
        }
        else
        {
            typedef boost::shared_ptr<boost::thread> thread_ptr;

            // start game
            std::vector<thread_ptr> connectionThreads( connections.size() );
            for(size_t i = 0; i<connections.size(); ++i)
            {
                connectionThreads[i].reset( new boost::thread( boost::bind(&tcp_connection::start, connections[i], i) ) );
            }

            // run main game loop
            game->Run();

            // join threads to the main thread
            for(size_t i = 0; i<connections.size(); ++i) {
                connectionThreads[i]->join();
            }
        }
    }
}

void tcp_connection::start(int playerId)
{
    // unlock players
    write_sync_stamp(socket);
    write_sync_stamp(socket);

    cerr << "Game started for player " << playerId << endl;
    
    // get client messages
    try
    {
        bool gameEnded = false;
        while (!game->restartGame)
        {
             // receive sync stamp
            read_sync_stamp(socket);

            bool busy = true;
            while (busy) 
            {
                switch( read_header(socket) ) 
                {
                    case object_transform_desc::header:
                    {
                        object_transform_desc desc;
                        desc.read(socket);
                        game->scene.UpdateObjects(desc);
                        break;
                    }

                    case player_state_desc::header:
                    {
                        player_state_desc desc;
                        desc.read(socket);
                        if ( desc.state == player_state_desc::EXIT ) 
                        {
                            socket.close();
                            game->restartGame = true;
                            cerr << "exit received" << endl;
                            return;
                        }

                        break;
                    }

                    case fire_canon_desc::header:
                    {
                        fire_canon_desc desc;
                        desc.read(socket);
                        game->scene.FireCanon(desc.impulse, playerId);
                        game->allGetNecessary[0] = game->allGetNecessary[1] = false;
                        game->playerTurn = 1 - game->playerTurn;
                        game->sendTurnMessage = true;
                        break;
                    }

                    case sync_stamp:
                    {
                        busy = false;
                        break;
                    }

                    default:
                        throw std::runtime_error("Client sent unexpected message");
                }
            }

            // begin transmission
            //message.clear();
            message = make_message(sync_stamp);

            // send client messages
            if ( game->scene.HaveNecessaryData() && !game->allGetNecessary[playerId] )
            {
                message += game->scene.NeccessaryMessage();
                game->allGetNecessary[playerId] = true; 
                if ( game->allGetNecessary[0] && game->allGetNecessary[1] ) {
                    game->scene.ClearNecessaryMessage();
                }
            }

            // send health
            if ( !gameEnded )
            {
                player_damage_desc damageDesc;
                damageDesc.id = playerId;
                damageDesc.health = game->scene.PlayerHealth(playerId);
                message += damageDesc.message();
                if ( damageDesc.health <= 0 ) 
                {
                    player_state_desc desc;
                    desc.id = playerId;
                    desc.state = player_state_desc::LOOSE;
                    message += desc.message();
                    gameEnded = true;
                }

                // send health
                damageDesc.id = 1 - playerId;
                damageDesc.health = game->scene.PlayerHealth(1 - playerId);
                message += damageDesc.message();
                if ( damageDesc.health <= 0 ) 
                {
                    player_state_desc desc;
                    desc.id = playerId;
                    desc.state = player_state_desc::WIN;
                    message += desc.message();
                    gameEnded = true;
                }
            }

            // send player turn message
            if (game->sendTurnMessage && playerId == game->playerTurn) 
            { 
                player_state_desc desc;
                desc.id = game->playerTurn; 
                desc.state = player_state_desc::TURN;
                message += desc.message();
                game->sendTurnMessage = false;
            }

            float time = SDL_GetTicks() / 1000.0f;
            message += game->scene.MakeMessage(time);

            if (game->restartGame) 
            {
                // close connection
                player_state_desc desc;
                desc.id = playerId;
                desc.state = player_state_desc::EXIT;
                message += desc.message();
                gameEnded = true;
            }

            message += make_message(sync_stamp);
            size_t transmitted = boost::asio::write( socket, boost::asio::buffer(message) );
            if ( transmitted < message.size() ) {
                throw std::runtime_error("Can't write message to client");
            }
        }

        // close connection
        socket.close();
    }
    catch(std::exception& err)
    {
        cerr << "Error: " << err.what() << ". Player " << playerId << endl;
        game->restartGame = true;
    }
}

/*
void Game::SynchroniseWithPlayer(player_ptr player, float time)
{
    // get client messages
    if ( player->socket.available() > 0 )
    {
        // receive sync stamp
        read_sync_stamp(player->socket);

        bool busy = true;
        while (busy) 
        {
            switch( read_header(player->socket) ) 
            {
                case object_transform_desc::header:
                {
                    object_transform_desc desc;
                    desc.read(player->socket);
                    scene.UpdateObjects(desc);
                    break;
                }

                case player_state_desc::header:
                {
                    player_state_desc desc;
                    desc.read(player->socket);
                    if ( desc.state == player_state_desc::EXIT ) 
                    {
                        player->socket.close();
                        return;
                    }

                    break;
                }

                case fire_canon_desc::header:
                {
                    fire_canon_desc desc;
                    desc.read(player->socket);
                    scene.FireCanon(desc.impulse, player->id);
                    allGetNecessary[0] = allGetNecessary[1] = false;
                    playerTurn = 1 - playerTurn;
                    sendTurnMessage = true;
                    break;
                }

                case sync_stamp:
                {
                    busy = false;
                    break;
                }

                default:
                    throw std::runtime_error("Client sent unexpected message");
            }
        }

        // begin transmission
        write_sync_stamp(player->socket);

        // send client messages
        if ( scene.HaveNecessaryData() && !allGetNecessary[player->id] )
        {
            allGetNecessary[player->id] = true; 
            scene.TranslateNeccessary(player->socket, allGetNecessary[0] && allGetNecessary[1]);
        }

        // send health
        if ( !gameEnded )
        {
            player_damage_desc damageDesc;
            damageDesc.id = player->id;
            damageDesc.health = scene.PlayerHealth(player->id);
            damageDesc.write(player->socket);
            if ( damageDesc.health <= 0 ) 
            {
                player_state_desc desc;
                desc.id = player->id;
                desc.state = player_state_desc::LOOSE;
                desc.write(player->socket);

                desc.id = 1 - player->id;
                desc.state = player_state_desc::WIN;
                desc.write(player->socket);
            }

            // send health
            damageDesc.id = 1 - player->id;
            damageDesc.health = scene.PlayerHealth(1 - player->id);
            damageDesc.write(player->socket);
            if ( damageDesc.health <= 0 ) 
            {
                player_state_desc desc;
                desc.id = player->id;
                desc.state = player_state_desc::WIN;
                desc.write(player->socket);

                desc.id = 1 - player->id;
                desc.state = player_state_desc::LOOSE;
                desc.write(player->socket);
            }
        }

        // send player turn message
        if (sendTurnMessage && player->id == playerTurn) 
        { 
            player_state_desc desc;
            desc.id = playerTurn; 
            desc.state = player_state_desc::TURN;
            desc.write(player->socket);
            sendTurnMessage = false;
        }

        scene.SynchroniseWithPlayer(player->socket, time);
        write_sync_stamp(player->socket);
    }
}
*/