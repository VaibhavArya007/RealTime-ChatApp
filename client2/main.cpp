//client 2 code

#include <iostream>
#include<WinSock2.h>
#include<string>
#include <WS2tcpip.h>
#include <thread>
using namespace std;

#pragma comment (lib, "ws2_32.lib")

bool initialise() {
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void SendMessageToServer (SOCKET s) {
	cout << "enter your chat name : " << endl;
	string name;
	getline(cin, name);
	cout << "Enter your message" << endl;
	string message;
	while (1) {
		getline(cin, message);
		string msg = name + " : " + message;
		int bytesent = send(s, msg.c_str(), msg.length(), 0);
		if (bytesent == SOCKET_ERROR) {
			cout << "error sending message " << endl;
			break;
		}

		if (message == "quit") {
			cout << "stopping the program" << endl;
			break;
		}

	}

	closesocket(s);
	WSACleanup();

}
void ReceiveMessage(SOCKET s) {
	char buffer[4096];
	int recvlen;
	string msg = "";
	while (1) {

		recvlen= recv(s, buffer, sizeof(buffer), 0);
		if (recvlen <= 0) {
			cout << "disconnected from the server " << endl;
			break;
		}
		else {
			msg = string(buffer, recvlen);
			cout << msg << endl;
		}
	}
	closesocket(s);
	WSACleanup();
}

int main() {
	if (!initialise()) {
		cout << "initialize winsock failed" << endl;
		return 1;
	}
	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET) {
		cout << "invalid socket" << endl;
		return 1;
	}
	int port = 2003;
	string serveraddress = "127.0.0.1";
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);

	inet_pton(AF_INET, serveraddress.c_str(), &(serveraddr.sin_addr));

	if (connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {

		cout << "unable to connect to server" << endl;

		closesocket(s);
		WSACleanup();
		return 1;
	}

	cout << "successfully connected to server" << endl;

	thread senderthread(SendMessageToServer, s);
	thread receiver(ReceiveMessage, s);
	
	senderthread.join();

	receiver.join();


	return 0;
}