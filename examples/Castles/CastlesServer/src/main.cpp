#include "Game.h"

int main(int argc, char** argv)
{
	try
	{
        while(true) 
        {
            boost::shared_ptr<Game> game( new Game() );
            boost::asio::io_service ioService;
            tcp_game_server server(game, ioService);
            ioService.run();
        }
        /*
        Game game;
        game.CreateScene();
        while(true) {
            game.Run();
        }*/
	}
	catch(std::exception& err)
	{
        std::cerr << "Application error: " << err.what() << std::endl; 
        return 0;
	}

    return 0;
}
