#ifndef SLON_ENGINE_CASTLES_NETWORK_H
#define SLON_ENGINE_CASTLES_NETWORK_H

#include <sgl/math/Matrix.hpp>
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

namespace net {

using boost::asio::ip::tcp;
namespace asio = boost::asio;

/// port for the game
static const int    castles_port	= 6882;
static const char*	castles_service	= "6882";

/// synchronization primitive
static const int    sync_stamp = 0xBB00;

inline void read_string(tcp::socket& socket, std::string& str)
{
    size_t size;
	asio::read( socket, asio::buffer( &size, sizeof(size_t) ) );
    str.resize(size);
    asio::read( socket, asio::buffer( &str[0], str.size() ) );
}

inline void write_string(tcp::socket& socket, const std::string& str)
{
    size_t size = str.size();
	asio::write( socket, asio::buffer( &size, sizeof(size_t) ) );
    asio::write( socket, asio::buffer( str.data(), str.size() ) );
}

template<class POD_T>
std::string make_message(POD_T value)
{
    std::string result;
    result.resize( sizeof(POD_T) );
    memcpy( &result[0], &value, sizeof(POD_T) );
    return result;
}

inline std::string make_message(const std::string& str)
{
    std::string result;
    result.resize( str.size() + sizeof(size_t) );
    size_t size = str.size();
    memcpy( &result[0], &size, sizeof(size_t) );
    memcpy( &result[sizeof(size_t)], &str[0], str.size() );
    return result;
}

inline int read_header(tcp::socket& socket)
{
	int value;
    asio::read( socket, asio::buffer(&value, sizeof(int)) );
    return value;
}

inline void read_sync_stamp(tcp::socket& socket)
{
	int value;
    asio::read( socket, asio::buffer(&value, sizeof(int)) );
    if ( value != sync_stamp ) {
        throw std::runtime_error("Received invalid synchronization stamp");
    }
}

inline void write_sync_stamp(tcp::socket& socket)
{
    asio::write( socket, asio::buffer(&sync_stamp, sizeof(int)) );
}

/** struct to transfer mesh desc */
struct object_desc
{
	static const int header = 0xAA01;

	int			        id;
    bool                clone;
	std::string         nodeName;
    sgl::math::Matrix4f matrix;

    std::string message() const
    {
        std::string buffer;
        buffer += make_message(header);
        buffer += make_message(id);
        buffer += make_message(clone);
        buffer += make_message(nodeName);
        buffer += make_message(matrix);
        return buffer;
    }

	size_t write(tcp::socket& socket) const
	{
		asio::write( socket, asio::buffer( &header, sizeof(int) ) );
		asio::write( socket, asio::buffer( &id, sizeof(int) ) );
		asio::write( socket, asio::buffer( &clone, sizeof(bool) ) );
		write_string(socket, nodeName);
        asio::write( socket, asio::buffer( &matrix, sizeof(gmath::Matrix4f) ) );
        return sizeof(object_desc) + sizeof(header);
	}

	size_t read(tcp::socket& socket)
	{
        asio::read( socket, asio::buffer( &id, sizeof(int) ) );
        asio::read( socket, asio::buffer( &clone, sizeof(bool) ) );
		read_string(socket, nodeName);
        asio::read( socket, asio::buffer( &matrix, sizeof(gmath::Matrix4f) ) );
        return sizeof(object_desc);
	}
};

/** struct to transfer mesh desc */
struct mesh_desc
{
	static const int header = 0xAA02;

	std::string fileName;

    std::string message() const
    {
        std::string buffer;
        buffer += make_message(header);
        buffer += make_message(fileName);
        return buffer;
    }

	size_t write(tcp::socket& socket) const
	{
		asio::write( socket, asio::buffer( &header, sizeof(int) ) );
		write_string(socket, fileName);
        return sizeof(mesh_desc) + sizeof(header);
	}

	size_t read(tcp::socket& socket)
	{
		read_string(socket, fileName);
        return sizeof(mesh_desc);
	}
};

/** struct to transfer skyBox desc */
struct sky_box_desc
{
	static const int header = 0xAA03;

	std::string maps[6];

    std::string message() const
    {
        std::string buffer;
        buffer += make_message(header);
        for(int i = 0; i<6; ++i)
		{
            buffer += make_message(maps[i]);
        }
        return buffer;
    }

	size_t write(tcp::socket& socket)
	{
		asio::write( socket, asio::buffer( &header, sizeof(int) ) );
        size_t size = sizeof(header);
        for(int i = 0; i<6; ++i)
		{
			write_string(socket, maps[i]);
            size += maps[i].size();
		}
        return size;
	}

	size_t read(tcp::socket& socket)
	{
        size_t size = 0;
		for(int i = 0; i<6; ++i)
		{
			read_string(socket, maps[i]);
            size += maps[i].size();
		}
        return size;
	}
};

/** struct to transfer object transform */
struct object_transform_desc
{
	static const int header = 0xAA04;

	int				id;
	gmath::Matrix4f	matrix;

    std::string message() const
    {
        std::string buffer;
        buffer += make_message(header);
        buffer += make_message(id);
        buffer += make_message(matrix);
        return buffer;
    }

	size_t write(tcp::socket& socket) const
	{
		asio::write( socket, asio::buffer( &header, sizeof(int) ) );
		asio::write( socket, asio::buffer( &id, sizeof(int) ) );
        asio::write( socket, asio::buffer( &matrix, sizeof(gmath::Matrix4f) ) );
        return sizeof(object_transform_desc) + sizeof(header);
	}

	size_t read(tcp::socket& socket)
	{
		asio::read( socket, asio::buffer( &id, sizeof(int) ) );
        asio::read( socket, asio::buffer( &matrix, sizeof(gmath::Matrix4f) ) );
        return sizeof(object_transform_desc);
	}
};

/** struct to transfer player state */
struct player_state_desc
{
	static const int header = 0xAA06;

	enum action
	{
		CREATE,
        READY,
		LOOSE,
        TURN,
		WIN,
        EXIT
	};

	int		id;
	action	state;

    std::string message() const
    {
        std::string buffer;
        buffer += make_message(header);
        buffer += make_message(id);
        buffer += make_message(state);
        return buffer;
    }

	size_t write(tcp::socket& socket)
	{
		asio::write( socket, asio::buffer( &header, sizeof(int) ) );
		asio::write( socket, asio::buffer( &id, sizeof(int) ) );
		asio::write( socket, asio::buffer( &state, sizeof(action) ) );
        return sizeof(player_state_desc) + sizeof(header);
	}

	size_t read(tcp::socket& socket)
	{
		asio::read( socket, asio::buffer( &id, sizeof(int) ) );
		asio::read( socket, asio::buffer( &state, sizeof(action) ) );
        return sizeof(player_state_desc);
	}
};

/** struct to transfer player health */
struct player_damage_desc
{
	static const int header = 0xAA0A;

	int id;
	int health;

    std::string message() const
    {
        std::string buffer;
        buffer += make_message(header);
        buffer += make_message(id);
        buffer += make_message(health);
        return buffer;
    }

	size_t write(tcp::socket& socket) const
	{
		asio::write( socket, asio::buffer( &header, sizeof(int) ) );
		asio::write( socket, asio::buffer( &id, sizeof(int) ) );
		asio::write( socket, asio::buffer( &health, sizeof(int) ) );
        return sizeof(player_damage_desc) + sizeof(header);
	}

	size_t read(tcp::socket& socket)
	{
		asio::read( socket, asio::buffer(&id, sizeof(int)) );
		asio::read( socket, asio::buffer(&health, sizeof(int)) );
        return sizeof(player_damage_desc);
	}
};

/** struct to transfer canon desc */
struct canon_desc
{
	static const int header = 0xAA07;

	int id;

    std::string message() const
    {
        std::string buffer;
        buffer += make_message(header);
        buffer += make_message(id);
        return buffer;
    }

	size_t write(tcp::socket& socket) const
	{
		asio::write( socket, asio::buffer( &header, sizeof(int) ) );
		asio::write( socket, asio::buffer( &id, sizeof(int) ) );
        return sizeof(canon_desc) + sizeof(header);
	}

	size_t read(tcp::socket& socket)
	{
        asio::read( socket, asio::buffer( &id, sizeof(int) ) );
        return sizeof(canon_desc);
	}
};

/** struct to transfer kernel desc */
struct kernel_desc
{
	static const int header = 0xAA08;

	int id;

    std::string message() const
    {
        std::string buffer;
        buffer += make_message(header);
        buffer += make_message(id);
        return buffer;
    }

	size_t write(tcp::socket& socket) const
	{
		asio::write( socket, asio::buffer( &header, sizeof(int) ) );
		asio::write( socket, asio::buffer( &id, sizeof(int) ) );
        return sizeof(kernel_desc) + sizeof(header);
	}

	size_t read(tcp::socket& socket)
	{
        asio::read( socket, asio::buffer( &id, sizeof(int) ) );
        return sizeof(kernel_desc);
	}
};

/** struct to transfer canon firing */
struct fire_canon_desc
{
	static const int header = 0xAA0B;

	int		playerId;
    float   impulse;

    std::string message() const
    {
        std::string buffer;
        buffer += make_message(header);
        buffer += make_message(playerId);
        buffer += make_message(impulse);
        return buffer;
    }

	size_t write(tcp::socket& socket)
	{
		asio::write( socket, asio::buffer( &header, sizeof(int) ) );
		asio::write( socket, asio::buffer( &playerId, sizeof(int) ) );
		asio::write( socket, asio::buffer( &impulse, sizeof(float) ) );
        return sizeof(fire_canon_desc) + sizeof(header);
	}

	size_t read(tcp::socket& socket)
	{
		asio::read( socket, asio::buffer( &playerId, sizeof(int) ) );
		asio::read( socket, asio::buffer( &impulse, sizeof(float) ) );
        return sizeof(fire_canon_desc);
	}
};

} // namespace net

#endif // SLON_ENGINE_CASTLES_NETWORK_H
