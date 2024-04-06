import TestClient;

int main()
{
	if (int awake_r = test::Awake(); 0 != awake_r) return awake_r;
	if (int start_r = test::Start(); 0 != start_r) return start_r;
	if (int update_r = test::Update(); 0 != update_r) return update_r;
	test::Cleanup();

	return 0;
}
