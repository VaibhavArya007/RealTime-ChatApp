#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <thread>
#include <vector>

using namespace std;

#pragma comment(lib, "ws2_32.lib")


bool initialize() {
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;

}

void interact_with_client(SOCKET clientSocket, vector<SOCKET>& clients) {
	//send or receive
	cout << "client has been connected" << endl;
	char buffer[4096];

	while (1) {
		int byterecvd = recv(clientSocket, buffer, sizeof(buffer), 0);
		
		if (byterecvd <= 0) {

			cout << "client has been disconnected" << endl;
			break;
		}
		
		
		string message(buffer, byterecvd);
		cout << "message from client : " << message << endl;

		for (auto client : clients) {
			if (client != clientSocket) {
				send(client, message.c_str(), message.length(), 0);
			}
		}
	}

	auto it = find(clients.begin(), clients.end(), clientSocket);
	if (it != clients.end()) {
		clients.erase(it);
	}

	closesocket(clientSocket);
	
}


int main() {
	if (!initialize()) {

		cout << "winsock initialization failed" << endl;
		return 1;
	}
	cout << "server program" << endl;

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0 );

	if (listenSocket == INVALID_SOCKET) {
		cout << "socket cannot be created" << endl;
		return 1;
	}
	int port = 2003;
	sockaddr_in serveraddr;
	//here we create the structure of address and port
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port); //host to network => hton


	//convert ip (0.0.0.0) ie the local host put it inside the sin_family in binary form
	if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1) {

		cout << "setting address structure failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}


	if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
		cout << "bind failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//listening
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		//max amount
		cout << "listen failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	cout << "server has started listening on the port : " << port << endl;
	vector<SOCKET> clients;

	while (1) {

		//accept from client
		SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);

		if (clientSocket == INVALID_SOCKET) {
			cout << "the client socket is invalid" << endl;
		}
		clients.push_back(clientSocket);
		thread t1(interact_with_client, clientSocket, std::ref(clients));
		t1.detach();

	}



	


	closesocket(listenSocket);


	WSACleanup();
	return 0;
}