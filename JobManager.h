#pragma once
#include <vector>
#include <Windows.h>


struct Job
{
	void* param;
	void(*function)(void*);
};


class JobManager
{
public:
	JobManager(int n_Threads);
	~JobManager(void);
	
	void AddJobToList(Job* newjob); //add job to list
	void WaitForJobs(DWORD ms); //wait for joblist to be empty

	HANDLE hSemaphore; //to synchronise threads
	HANDLE hEvent; 
	CRITICAL_SECTION Crittical_Section;

	int waitingThreads;
	//vector threads
protected:
	friend DWORD WINAPI ThreadFunction(LPVOID lParam); //thread base function
	int NumberOfThreads;
	std::vector<Job*> JobList; //vector to hold a list of jobs to do
	HANDLE* myThreads; //array of threads
	Job* GetJobFromList(); //returns and removes last job on the list.
	
	
	

};