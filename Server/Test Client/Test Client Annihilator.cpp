module TestClient;

void
test::Cleanup()
{
	app_socket.Close();

	iconer::net::Cleanup();
}
