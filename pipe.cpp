#include <iostream>
#include <cstdio>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
using namespace std;

struct argument {
	bool exit;
	int* filedes;
};

void* threadfunc1(void* args) {
	cout << endl << "Thread 1 has started!" << endl;
	int *j = new int;
	*j = 0;
	argument* x;
	x = (argument*)args;
	int* des = x->filedes;
	while (!x->exit) {
			++(*j);
			cout << "Trying to send message..." << endl;
			int a = 100;
			write(des[1],&a,sizeof(a));
			cout << "Message with code " << a <<" has been sent" << endl;
			usleep(100000);
	}
	cout << "Thread 1 ready to exit" << endl;
	pthread_exit(j);
}

void* threadfunc2(void* args)
{
	cout << endl << "Thread 2 has started!" << endl;
	int * j = new int;
	*j = 0;
	argument* x;
	x = (argument*)args;
	int* des = x->filedes;
	while(!x->exit) {
			++(*j);
			int b;
			cout << "Trying to accept message..." << endl;
			if(read(des[0],&b,sizeof(b))==-1) {
				perror("read");
			}
			else {
				cout << "Message with code " << b << " has been accepted" << endl;
			}

			usleep(100000);
			
	}	
	cout << "Thread 2 ready to exit" << endl;
	pthread_exit(j);
}

int main()
{
	pthread_t thread1,thread2;
	int filedes[2];	
	cout << "The program starts" << endl;
	if(!pipe(filedes))
		cout << "Pipe created" << endl;
	fcntl(filedes[0], F_SETFL, fcntl(filedes[0], F_GETFL)|O_NONBLOCK);
	perror("fcntl");
	argument* arg1 = new argument;
	argument* arg2 = new argument;
	arg1->filedes = filedes;
	arg2->filedes = filedes;
	arg1->exit = false;
	arg2->exit = false;
	if(!pthread_create(&thread1,NULL,threadfunc1,arg1))
		cout << "Thread 1 created" << endl;
	if(!pthread_create(&thread2,NULL,threadfunc2,arg2))
		cout << "Thread 2 created" << endl;
	cout << "Press any key to stop" << endl;
	getchar();
	arg1->exit = true;
	arg2->exit = true;
	cout << endl;
	void* val;
	if(!pthread_join(thread1,&val))
		cout << "Thread 1 finished successfully with code " << *((int*)val) << endl;
	if(!pthread_join(thread2,&val))
		cout << "Thread 2 finished successfully with code " << *((int*)val) << endl;
	delete arg1;
	delete arg2;
	cout << "The program has finished correctly" << endl;
	return 0;
}
