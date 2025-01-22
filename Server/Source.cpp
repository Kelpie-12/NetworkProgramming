#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib,"Ws2_32.lib")
using namespace std;
#define DEFAULT_PORT "27015"
#define BUTTER_SIZE 1500
#define CLASS_WORK
//#define HOME_WORK
void main()
{
	setlocale(LC_ALL, "");
#ifdef CLASS_WORK
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		cout << "Winsock initialisation failed with error #" << iResult;
		WSACleanup();
		return;
	}
	addrinfo* result = nullptr;
	addrinfo* ptr = nullptr;
	addrinfo hInst;

	ZeroMemory(&hInst, sizeof(hInst));
	hInst.ai_family = AF_INET;
	hInst.ai_socktype = SOCK_STREAM;
	hInst.ai_protocol = IPPROTO_TCP;
	hInst.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(nullptr, DEFAULT_PORT, &hInst, &result);
	if (iResult != 0)
	{
		cout << "getaddrinfo failed with error #" << iResult;
		WSACleanup();
		return;
	}
	SOCKET ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET)
	{
		cout << "Socked failed with error #" << WSAGetLastError();;
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	iResult = bind(ListenSocket, result->ai_addr, result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		cout << "Bind failed with error #" << WSAGetLastError();;
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}
	freeaddrinfo(result);
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		cout << "Liste failed with error #" << WSAGetLastError();;
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}
	cout << "Server started on TCP port " << DEFAULT_PORT << endl;

	SOCKET ClientSocket = accept(ListenSocket, nullptr, nullptr);
	if (ClientSocket == INVALID_SOCKET)
	{
		cout << "Accept failed with error #" << WSAGetLastError();;
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}

	//closesocket(ClientSocket);
	closesocket(ListenSocket);
	char recvbuffer[BUTTER_SIZE]{};
	int received = 0;
	do
	{
		received = recv(ClientSocket, recvbuffer, BUTTER_SIZE, 0);
		if (received > 0)
		{
			cout << "Bytes received: " << received << endl;
			cout << recvbuffer << endl;
			int iSendResult = send(ClientSocket, "Привет Client", received, 0);
			if (iSendResult == SOCKET_ERROR)
			{
				cout << "Send failed wiht error #" << WSAGetLastError();
				closesocket(ClientSocket);
				WSACleanup();
				return;
			}
			cout << "Bytes sent : " << iSendResult << endl;
		}
		else if (received == 0)
			cout << "Connection close..." << endl;
		else
		{
			cout << "Receive failed wiht error #" << WSAGetLastError();
			closesocket(ClientSocket);
			WSACleanup();
			return;
		}
	} while (received > 0);

	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
		cout << "Shutdown failed wiht error #" << WSAGetLastError();

	closesocket(ClientSocket);
	WSACleanup();
	system("pause");
#endif // CLASS_WORK

#ifdef HOME_WORK
	// Код сервера :
	WSADATA wsaData;
	SOCKET _socket;
	SOCKET acceptSocket;
	sockaddr_in addr;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	addr.sin_family = AF_INET;
	inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);
	addr.sin_port = htons(20000);
	bind(_socket, (SOCKADDR*)&addr, sizeof(addr));
	listen(_socket, 1);
	cout << "Server is started\n";
	acceptSocket = accept(_socket, NULL, NULL);

	char buf[1500];
	int i = recv(acceptSocket, buf, 1500, 0);
	buf[i] = '\0';
	cout << buf << endl;
	const char* text = "Hello from server!";
	send(acceptSocket, text, strlen(text), 0);
	closesocket(acceptSocket);
	closesocket(_socket);
	
	WSACleanup();
	system("pause");
#endif // HOME_WORK

}