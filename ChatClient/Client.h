#ifndef CLIENT_H
#define CLIENT_H

#include <thread>
#include <string>
#include <sio_client.h>

namespace MDView {
	
	sio::message::ptr CreateMessage(
		const std::string& key, 
		const std::string& uuid, 
		const std::string& msg, 
		const std::string& type, 
		const std::string& status,
		bool kill
	);

	class Client {
	public:
		Client() = default;
		~Client();

	public:
		void connect(const std::string& url);
		void send(sio::message::ptr& msg);
		bool isConnected();

	private:
		void _run(sio::client* client, const std::string& url);

	private:
		sio::client m_client;
	};
}

#endif