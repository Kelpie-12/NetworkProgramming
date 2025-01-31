#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib,"Ws2_32.lib")
using namespace std;
#define DEFAULT_PORT "27015"
#define BUFFER_SIZE 1500
#define MAX_CONNECTIONS		 3
#define CLASS_WORK
//#define HOME_WORK
union ClientSocketData
{
	SOCKADDR client_socket;
	unsigned long long data;
	ClientSocketData(SOCKADDR client_socket)
	{
		this->client_socket = client_socket;
	}
	unsigned long long get_data()const
	{
		return data;
	}
	unsigned short get_port()const
	{
		return 	(unsigned char)client_socket.sa_data[0] << 8 | (unsigned char)client_socket.sa_data[1];

		//return (data>>16)& 0xFFFF;
	}
	char* get_socket(char* sz_client_name) const
	{
		sprintf(sz_client_name, "%i.%i.%i.%i.:%i",
			(unsigned char)client_socket.sa_data[2],
			(unsigned char)client_socket.sa_data[3],
			(unsigned char)client_socket.sa_data[4],
			(unsigned char)client_socket.sa_data[5],
			(unsigned char)client_socket.sa_data[0] << 8 | (unsigned char)client_socket.sa_data[1]);
		return sz_client_name;
	}

};
void PrintNumberOfClients();
void HandleClient(LPVOID i);
SOCKET ClientSocket;
SOCKET client_sockets[MAX_CONNECTIONS]{};
HANDLE client_handles[MAX_CONNECTIONS]{};
DWORD dw_thread_id[MAX_CONNECTIONS]{};
int* client_number2[MAX_CONNECTIONS]{};
int client_number = 0;
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

	iResult = GetAddrInfo(NULL, DEFAULT_PORT, &hInst, &result);
	if (iResult != 0)
	{
		cout << "GetAddrInfo failed with error #" << iResult << endl;
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

	//3
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

	//4
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		cout << "Liste failed with error #" << WSAGetLastError();;
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}
	cout << "Server started on TCP port " << DEFAULT_PORT << endl;

	//5 
	do
	{
		//client_number = 0;
		CHAR sz_client_name[32];
		int namelen = 32;
		SOCKADDR client_socket;
		ZeroMemory(&client_socket, sizeof(client_socket));
		
		PrintNumberOfClients();
		if (client_number < MAX_CONNECTIONS)
		{
			client_number2[client_number] = (int*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(int));
			*client_number2[client_number] = client_number;

			client_sockets[client_number] = accept(ListenSocket, &client_socket, &namelen);
			//ClientSocket = accept(ListenSocket, &client_socket, &namelen);
			if (ClientSocket == INVALID_SOCKET)
			{
				cout << "Accept failed with error #" << WSAGetLastError() << endl;
				closesocket(ListenSocket);
				//WSACleanup();
				//return;
			}

			//HandleClient(ClientSocket);

			client_handles[client_number] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)HandleClient, client_number2[client_number], 0, &dw_thread_id[client_number]);
			client_number++;
		}
		else
		{
			SOCKET deny = accept(ListenSocket, &client_socket, &namelen);
			char sz_buff_deny2[BUFFER_SIZE]{};
			recv(deny, sz_buff_deny2, BUFFER_SIZE, 0);
			char sz_buff_deny[]{ "No free connection" };
			send(deny, sz_buff_deny, (int)strlen(sz_buff_deny), 0);
			shutdown(deny, SD_BOTH);
			closesocket(deny);
			cout << ClientSocketData(client_socket).get_socket(sz_client_name) << " was disconnected" << endl;

		}		
		Sleep(10);

	} while (true);

	//7

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

void PrintNumberOfClients()
{
	HANDLE hConcole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO info;
	ZeroMemory(&info, sizeof(info));
	GetConsoleScreenBufferInfo(hConcole, &info);
	SetConsoleCursorPosition(hConcole, COORD{ 85, 0 });
	cout << "Count client: " << client_number<<endl;
	SetConsoleCursorPosition(hConcole, info.dwCursorPosition);

}

void HandleClient(LPVOID lParam)
{
	CHAR sz_client_name[32]{};
	int namelen = 32;
	SOCKADDR client_socket;
	int i = *((int*)lParam);
	getpeername(client_sockets[i], &client_socket, &namelen);

	cout << "Client: " << ClientSocketData(client_socket).get_socket(sz_client_name) << endl;

	//closesocket(ClientSocket);
	//closesocket(ListenSocket);

	//6. Receive & Send data:
	char recvbuffer[BUFFER_SIZE]{};
	int received = 0;
	do
	{
		ZeroMemory(recvbuffer, BUFFER_SIZE);
		received = recv(client_sockets[i], recvbuffer, BUFFER_SIZE, 0);
		if (received > 0)
		{
			cout << "Bytes received:  \t" << received << endl;
			cout << "Message from " << sz_client_name << ":\t" << recvbuffer << endl;
			//cout << "Received message:\t" << recvbuffer << endl;
			int iSendResult = send(client_sockets[i], recvbuffer, received, 0);
			//int iSendResult = send(ClientSocket, "Ïðèâåò Client", received, 0);
			if (iSendResult == SOCKET_ERROR)
			{
				cout << "Send failed with error #" << WSAGetLastError() << endl;
				closesocket(client_sockets[i]);
				//WSACleanup();
				return;
			}
			cout << "Bytes sent: " << iSendResult << endl;
		}
		else if (received == 0)
		{
			cout << "Connection closing..." << endl;
		}
		else
		{
			cout << "Receive failed with error #" << WSAGetLastError() << endl;
			HeapFree(GetProcessHeap(), 0, client_number2[client_number]);
			//WSACleanup();
			//return;
		}
	} while (received > 0);
	closesocket(client_sockets[i]);
	CloseHandle(client_handles[i]);
	//7. Disconnection:
	int iResult = shutdown(client_sockets[i], SD_BOTH);
	if (iResult == SOCKET_ERROR)
	{
		cout << "shutdown failed with error #" << WSAGetLastError() << endl;
	}
	closesocket(client_sockets[i]);
}
