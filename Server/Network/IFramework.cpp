module;

module Iconer.Net.IFramework;

void
iconer::net::IFramework::Cleanup()
{
	listenSocket.Close();
}
