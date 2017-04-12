#include "Network.h"

const char* SERVERRIP = "127.0.0.1";
const int SERVERPORT = 23452;
const int BUFFERSIZE = 4096;

//�����ʱ�ȭ�� ���� �Ҵ�.
int Client::Network::Init()
{
	//������ ������ üũ �����Ѵ�.MAKEWORD�� �ΰ��� ���� ����� ����� �ִ°�.
	WORD wVersionRequested = MAKEWORD(2, 2);
	//�ʱ�ȭ �� ������ ������ ���¸� �����ϴ� ����.
	WSADATA wsaData;

	//���ڷ� ���޵� ������ ���� �������� ������ ���� DLL�� �ʱ�ȭ �ϴ°�.
	auto returnVal = WSAStartup(wVersionRequested, &wsaData);

	//������ ���� �ʱ�ȭ�� �����ϸ� 0�� ��ȯ�Ѵ�.
	if (returnVal != 0)
	{
		return -1;
	}

	//����� �����ϵ��� ���ҽ��� �Ҵ��ϰ� �� ���ҽ��� ����Ҽ� �ִ� ���ϵ�ũ���� SOCKET �� ��ȯ �Ѵ�.
	//���� �ּ�ü�� ���� Ÿ�� ���������� �����Ѵ�, TCP�� ���� ��Ʈ�� udp�� ���� ��׷�(������ �׷�)�� �����Ѵ�.
	m_Sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_Sockfd == INVALID_SOCKET)
	{
		return -2;
	}


	return 0;
}

bool Client::Network::Connect(const char* pAddr,const int pPort)
{
	//SOCKADDR ���� �ּ� ����ü, SOCKADDR_IN�� TCP/IP���� ���Ǵ� ���� �ּ� ����ü ���� UDP������ ���
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVERPORT);//TODO: pPort
	serverAddr.sin_addr.s_addr = inet_addr(SERVERRIP);//TODO: pAddr //���� 32��Ʈ ����ü�ε� IPv6�� ��� ����?

	auto returnVal = connect(m_Sockfd, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	
	if (returnVal == SOCKET_ERROR)
	{
		return false;
	}

	m_State = State::Connected;

	m_th = std::thread([this]() { Update(); });

	return true;
}

int Client::Network::Update()
{
	auto returnVal = 0;

	while (true)
	{
		char buffer[BUFFERSIZE];
		auto length = Recv(buffer);
		
		if (length == 0)
		{
			returnVal = length;
			break;
		}

		if (length < 0)
		{
			returnVal = length;
			break;
		}

		RecvProc(buffer, length);
	}
	

	return returnVal;
}

int Client::Network::Release()
{
	//������ �ݴ´�
	closesocket(m_Sockfd);
	//������ ���� ���ҽ��� ��ȯ�Ѵ�.
	WSACleanup();
	return 0;
}

int Client::Network::Send()
{
	return 0;
}

int Client::Network::Recv(char* pBuffer)
{
	int receivedLen;

	receivedLen = recv(m_Sockfd, pBuffer, BUFFERSIZE, 0);
	
	if (receivedLen == 0)
	{
		return 0;//���� ����
	}

	if (receivedLen < 0)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			return -1;//WSAEWOULDBLOCK Error!
		}
	}
	
	return receivedLen;
}

int Client::Network::RecvProc(const char * pBuffer,const int pReceivedLen)
{
	auto readPos = 0;
	const auto dataSize = pReceivedLen;
	PacketHeder* pPacketHeader;

	while (dataSize-readPos > PACKET_HEADER_SIZE)//�����÷ο�� ���ٰ� ���������Ƿ� �Ƹ� 0�ΰ�� ���´�
	{
		pPacketHeader = (PacketHeder*)&pBuffer[readPos];
		auto id = pPacketHeader->Id;
		auto size = pPacketHeader->BodySize;
		readPos += PACKET_HEADER_SIZE;
		
		AddPacketToQue(id,size,&pBuffer[readPos]);


	}


	return 0;
}

int Client::Network::AddPacketToQue(const short pId, const short pSize,const char * pData)
{
	return 0;
}
