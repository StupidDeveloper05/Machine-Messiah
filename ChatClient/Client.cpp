#include "Client.h"
#include <iostream>

namespace MDView {
	
	Client::~Client()
	{
		m_client.sync_close();
	}
	void Client::connect(const std::string& url)
	{
		std::thread t(&MDView::Client::_run, this, &m_client, url);
		t.detach();
	}

	void Client::send(sio::message::ptr& msg)
	{
		m_client.socket()->emit("message", msg);
	}

	bool Client::isConnected()
	{
		return m_client.opened();
	}
	
	void Client::_run(sio::client* client, const std::string& url)
	{
		client->connect(url);
	}
	
	sio::message::ptr CreateMessage(const std::string& key, const std::string& uuid, const std::string& msg, const std::string& type, const std::string& status, bool kill)
	{
		auto message = sio::object_message::create();
		message->get_map()["key"] = sio::string_message::create(key);
		message->get_map()["uuid"] = sio::string_message::create(uuid);
		message->get_map()["kill"] = sio::bool_message::create(kill);
		message->get_map()["msg"] = sio::string_message::create(msg);
		message->get_map()["type"] = sio::string_message::create(type);
		message->get_map()["status"] = sio::string_message::create(status);
		return message;
	}
}