#pragma once
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <deque>

namespace Client
{
	struct PacketHeder
	{
		short Id;
		short BodySize;
	};

	const int PACKET_HEADER_SIZE = sizeof(PacketHeder);

	struct PacketBody
	{
		PacketBody() = default;

		short PacketId = 0;
		short PacketBodySize = 0;
		char* PacketData = nullptr;
	};

	enum class State
	{
		Disconnect = 0,
		Connecting = 1,
		Connected = 2,
	};

	class Network
	{
	public:
		Network() = default;

		~Network() = default;

		//윈속 초기화.
		int Init();

		//
		bool Connect(const char*,const int);

		int Update();

		int Release();

	private:
		State m_State{ State::Disconnect };

		SOCKET m_Sockfd = INVALID_SOCKET;
		
		std::deque<PacketBody> que;

		std::thread m_th;

		
		int Send();
		int Recv(char*);
		int RecvProc(const char * ,const int);
		int AddPacketToQue(const short,const short,const char*);
	};

}


