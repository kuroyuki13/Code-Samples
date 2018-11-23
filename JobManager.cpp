#include "JobManager.h"



JobManager::JobManager(int n_Threads)
{
	NumberOfThreads = n_Threads;
	//initializing critical section
	InitializeCriticalSection(&Crittical_Section);
	//initializing event and semaphore
	hEvent = CreateEvent( 
		NULL, // security attribute
		1, // manual reset event
		0, // initially nonsignaled
		NULL // name (optional)
		);
	hSemaphore = CreateSemaphore(
		NULL,
		0,
		1000,
		NULL);

	myThreads = new HANDLE[n_Threads];
	waitingThreads = 0;
	//creating threads
	for (int i = 0; i < NumberOfThreads; i++)
	{
		HANDLE temporaryThread = CreateThread(NULL, 0, &ThreadFunction, this, 0, NULL);
		myThreads[i] = temporaryThread;
	}

}


JobManager::~JobManager()
{
	//destroy all handles
	CloseHandle(hEvent);
	CloseHandle(hSemaphore);
	for (int i = 0; i < NumberOfThreads; i++) CloseHandle(myThreads[i]);
	DeleteCriticalSection(&Crittical_Section);
}

DWORD WINAPI ThreadFunction(LPVOID lParam)
{
	JobManager* manager = (JobManager*)lParam;

	while (1)
	{
		EnterCriticalSection(&manager->Crittical_Section); // enter critical section before writing data
		manager->waitingThreads++;	//write data
		LeaveCriticalSection(&manager->Crittical_Section); //leave section
		//if all threads are waiting set event to active.
		if (manager->waitingThreads == manager->NumberOfThreads) SetEvent(manager->hEvent); 

		WaitForSingleObject(manager->hSemaphore, INFINITE); //threads wait for jobs to become available
		ResetEvent(manager->hEvent);	//there are jobs, reset the event.

		EnterCriticalSection(&manager->Crittical_Section); //enter critical section before writing data
		manager->waitingThreads--; //thread is working, remove from waiting threads
		Job* job = manager->GetJobFromList(); //get a job from the joblist
		LeaveCriticalSection(&manager->Crittical_Section); // leave section

		void(*task)(void*); //creating function name
		task = job->function; //giving function its adress
		task(job->param); //execute function with job parameters
	}
	return 0;
}


Job* JobManager::GetJobFromList()
{
	int size = JobList.size(); //size of the job list
	Job* t = JobList[size - 1]; //get last job on the list
	JobList.erase(JobList.begin() + (size - 1)); // got the job, erase it from the list
	return t; //return the job
}

void JobManager::AddJobToList(Job* newJob)
{
	JobList.push_back(newJob); //add job to the list
	ReleaseSemaphore(hSemaphore, 1, NULL); //increase semaphore count, there are more jobs available.

}

void JobManager::WaitForJobs(DWORD ms)
{
	WaitForSingleObject(hEvent, ms); //wait for event object to be set.
}