using System.Net;
using System.Net.Sockets;
using System.Data.SqlTypes;
using System.Data.SqlClient;

using Iconer.Utiliy;
using Iconer;

namespace Iconer
{
	internal class Worker
	{
		internal const AddressFamily addressFamily = AddressFamily.InterNetwork;
		internal const SocketType listenerType = SocketType.Dgram;
		internal const ProtocolType listenerProtoocol = ProtocolType.Udp;

		internal const SocketType interLobbyType = SocketType.Dgram;
		internal const ProtocolType interLobbyProtoocol = ProtocolType.Udp;

		internal static readonly IPAddress listenerAddress;
		internal static readonly int listenerPort;
		internal static readonly IPEndPoint listenerEndpoint;

		internal static readonly Socket? listenSocket;
		internal static readonly Socket? interLobbySocket;

		static Worker()
		{
			listenerAddress = IPAddress.Any;
			listenerPort = 40000;
			listenerEndpoint = new(listenerAddress, listenerPort);

			listenSocket = new(addressFamily, listenerType, listenerProtoocol);
			CodeQuaility.Assert(listenSocket is not null);

			interLobbySocket = new(addressFamily, interLobbyType, interLobbyProtoocol);
			CodeQuaility.Assert(interLobbySocket is not null);
		}

		public Worker()
		{
		}

		internal static void Execute(object? obj)
		{
			if (obj is null)
			{
				Console.WriteLine($"{obj} is null!");

				return;
			}

			var worker = (Worker)obj;
			if (worker is null)
			{
				Console.WriteLine($"{ obj } is not the class Worker!");

				return;
			}

			Console.WriteLine("Hello, World!");
		}
	}

	internal class ServerProgram
	{
		internal const int threadCount = 2;

		internal Thread[]? serverWorkers;

		public ServerProgram()
		{
		}

		public void Start()
		{
			serverWorkers = new Thread[threadCount];
			CodeQuaility.Assert(serverWorkers is not null);

			for (var i = 0; i < threadCount; ++i)
			{
				var worker = new Worker();
				CodeQuaility.Assert(worker is not null);

				var thread = new Thread(Worker.Execute);
				CodeQuaility.Assert(thread is not null);

				serverWorkers[i] = thread;

				thread.Start(worker);
			}
		}

		public static void Main(string[] args)
		{
			var app = new ServerProgram();
			CodeQuaility.Assert(app is not null);

			app.Start();
		}

		bool Receive()
		{
			byte[]? recv_buffer = new byte[512];

			//var task = listenSocket.ReceiveFromAsync(recv_buffer, listenerEndpoint);

			//return task != null;

			return false;
		}

	}
}
