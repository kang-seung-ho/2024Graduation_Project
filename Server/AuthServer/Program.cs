using System.Net;
using System.Net.Sockets;

using Iconer.Utiliy;

Console.WriteLine("Hello, World!");



const AddressFamily addressFamily = AddressFamily.InterNetwork;
const SocketType listenerType = SocketType.Dgram;
const ProtocolType listenerProtoocol = ProtocolType.Udp;


Socket listenSocket = new(addressFamily, listenerType, listenerProtoocol);
CodeQuaility.Assert(listenSocket is not null);

const SocketType interLobbyType = SocketType.Dgram;
const ProtocolType interLobbyProtoocol = ProtocolType.Udp;
Socket interLobbySocket = new(addressFamily, interLobbyType, interLobbyProtoocol);
CodeQuaility.Assert(interLobbySocket is not null);

bool Receive()
{
	static IPAddress listenerAddress = IPAddress.Any;
	static int listenerPort = 40000;

	static IPEndPoint endpoint = new(listenerAddress, listenerPort);

	byte[]? recv_buffer = new byte[512];

	var task = listenSocket.ReceiveFromAsync(recv_buffer, endpoint);

	return task != null;
}

while (true)
{


	
}
