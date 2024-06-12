#include "ThreadPool.hpp"
#include "Framework.hpp"

bool
ServerThreadPool::OnProcessTask(ServerFramework& framework, iconer::net::IoEvent task)
{
	if (task.isSucceed)
	{
	}
	else
	{

	}

	return task.isSucceed;
}
