#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib,"Ws2_32.lib")
using namespace std;
#define DEFAULT_PORT "27015"
#define BUFFER_SIZE 1500
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
	hInst.ai_family = AF_UNSPEC; //AF_INET;
	hInst.ai_socktype = SOCK_STREAM;
	hInst.ai_protocol = IPPROTO_TCP;
	iResult = getaddrinfo("192.168.56.1", DEFAULT_PORT, &hInst, &result);
	if (iResult != 0)
	{
		cout << "getaddrinfo failed with error #" << iResult;
		WSACleanup();
		return;
	}

	SOCKET ConnectSocket = socket(hInst.ai_family, hInst.ai_socktype, hInst.ai_protocol);
	if (ConnectSocket == INVALID_SOCKET)
	{
		cout << "Socked failed with error #" << WSAGetLastError();;
		freeaddrinfo(result);
		WSACleanup();
		return;
	}
	iResult = connect(ConnectSocket, result->ai_addr, result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}
	freeaddrinfo(result);
	if (ConnectSocket == INVALID_SOCKET)
	{
		cout << "Unavle to connect to Server" << endl;
		WSACleanup();
		return;
	}

	const char sendbuffer[] = "Привет сервер";
	char recvbuffer[BUFFER_SIZE]{};
	iResult = send(ConnectSocket, sendbuffer, strlen(sendbuffer), 0);
	if (iResult == SOCKET_ERROR)
	{
		cout << "Send failed wiht error #" << WSAGetLastError();
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}
	cout << "Bytes sent: " << iResult << endl;
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		cout << "Shutdown failed wiht error #" << WSAGetLastError();
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}

	int received = 0;
	do
	{
		received = recv(ConnectSocket, recvbuffer, BUFFER_SIZE, 0);
		if (received > 0)
		{
			cout << "Bytes received: " << received << endl;
			cout << "Received message -> " << recvbuffer << endl;
		}
		else if (received == 0)
			cout << "Connection close" << endl;
		else
			cout << "Receive failed wiht error #" << WSAGetLastError();
	} while (received > 0);

	iResult = shutdown(ConnectSocket, SD_RECEIVE);
	if (iResult == SOCKET_ERROR)
		cout << "Shutdown failed wiht error #" << WSAGetLastError();

	closesocket(ConnectSocket);
	WSACleanup();
	system("pause");
	return;
#endif // CLASS_WORK

#ifdef HOME_WORK
	// Код клиента:
	WSADATA wsaData;
	SOCKET _socket;
	sockaddr_in addr;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	addr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
	addr.sin_port = htons(20000);
	connect(_socket, (SOCKADDR*)&addr, sizeof(addr));
	char buff[BUFFER_SIZE];
	const char* text = "Первое отправленное сообщение";
	cout << "\nPress Enter to send 'Hello world!' "
		"to server\n";
	cin.get();
	send(_socket, text, strlen(text), 0);
	int i = recv(_socket, buff, BUFFER_SIZE, 0);
	buff[i] = '\0';
	cout << buff << endl;
	closesocket(_socket);
	WSACleanup();
	system("pause");

#endif // HOME_WORK


}