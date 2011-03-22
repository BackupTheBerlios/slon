#ifndef SLON_ENGINE_CASTLES_GAME_H
#define SLON_ENGINE_CASTLES_GAME_H

#include "Network.h"
#include "Scene.h"
#include <boost/enable_shared_from_this.hpp>

using boost::asio::ip::tcp;

struct Player
{
    Player() : 
        health(100)
    {}

    int id;
    int health;
};

class Game
{
public:
    Game();
    void CreateScene();
    //void SynchroniseWithPlayer(player_ptr player, float time);

    static const int num_players = 2;

    Scene   scene;
    bool    allGetNecessary[2];
    int     playerTurn;
    bool    sendTurnMessage;
    bool    restartGame;

    std::vector<Player> players;

    void Run();
};

class tcp_connection : 
    public boost::enable_shared_from_this<tcp_connection>
{
public:
    typedef boost::shared_ptr<tcp_connection> pointer;

    static pointer create(boost::shared_ptr<Game> game, boost::asio::io_service& ioService)
    {
        return pointer( new tcp_connection(game, ioService) );
    }

    tcp::socket& get_socket() { return socket; }

    void start(int playerId);

private:
    tcp_connection(boost::shared_ptr<Game> _game, boost::asio::io_service& ioService) : 
        socket(ioService),
        game(_game)
    {}

    void handle_write(const boost::system::error_code& error, size_t bytes_transferred)
    {
        if (!error) 
        {
            std::cerr << "Player disconnected: " << boost::system::system_error(error).what() << std::endl;   
            //game->Restart();
        }
        else if ( bytes_transferred < message.size() ) 
        {
            std::cerr << "Player disconnected: can't send message" << std::endl;
            //game->Restart();
        }
    }

    tcp::socket             socket;
    std::string             message;
    boost::shared_ptr<Game> game;
};

class tcp_game_server
{
public:
    tcp_game_server(boost::shared_ptr<Game> _game, boost::asio::io_service& ioService) :
        game(_game),
        acceptor( ioService, tcp::endpoint(tcp::v4(), net::castles_port) )
    {
        start_accept();
    }

    void start_accept();
    void handle_accept(tcp_connection::pointer connection, const boost::system::error_code& error);

private:
    // network
    boost::shared_ptr<Game>                 game;
    net::tcp::acceptor                      acceptor;
    std::vector<tcp_connection::pointer>    connections;
};

#endif // SLON_ENGINE_CASTLES_GAME_H
