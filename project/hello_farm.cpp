#include <vector>
#include <iostream>
#include <ff/farm.hpp>
#include <ff/pipeline.hpp>
#include <math.h>

using namespace ff;
using namespace std;

typedef double task;

struct Worker:ff_node_t<task>{

	task *svc (task *t){
		//std::cout << "Hello I'm the worker " <<get_my_id()<< ", from cpu: "<<ff_getMyCpu()<<"\n";
		for(long i=0;i<1000000000;i++)
			*t += sin(i);
		return t;
	}
};

struct firstStage : ff_node_t<task>{
	task *svc(task *){
		task *t = new task(0);
		for(int i=0;i<8;i++)
			ff_send_out(new double(0));
		return EOS;
	}

}Emitter;

struct lastStage : ff_node_t<task>{
	task *svc(task *t){
		std :: cout <<"Collector received " << *t <<"\n";
		delete t;
		return GO_ON;
	}

}Collector;

int main (int argc,char *argv [] ){
	//assert(argc>1);
	//int nworkers = atoi(argv [1]) ;
	//std::vector<ff_node*> Workers ;
	//for ( int i =0;i<nworkers;++i) 
	//	Workers.push_back(new Worker) ;
	
	clock_t c0,c1;
	time_t t0,t1;
	
	c0 = clock();
	t0 = time(NULL);	
	std :: cout << "farm:\n";
	
	int n =10;
	ff_Farm<task> myFarm([n](){
			std :: vector<std::unique_ptr<ff_node>> workers;
			for ( int i =0;i<8;++i)
				 workers.push_back(std :: unique_ptr<ff_node_t<task>>(new Worker));
			return workers;	 			

			}(),Emitter,Collector);

	if (myFarm. run_and_wait_end()<0) 
		error("running myFarm");

	c1 = clock();
	t1 = time(NULL);
	
	std::cout<<"elapsed wall clock time: "<<(long)(t1-t0)<<"\n";
        std::cout<<"elapsed CPU time: "<<(long)(c1-c0)/CLOCKS_PER_SEC<<"\n";


	std:: cout <<"serial:\n";

	c0 = clock();
        t0 = time(NULL);
	
	for(int size=1;size<=8;size++){
		double result = 0;
		for(long i=0;i<1000000000;i++)
			result += sin(i);
		std :: cout <<"result from serial: "<<result<<"\n";
	}
	
	c1 = clock();
        t1 = time(NULL);

	std::cout<<"elapsed wall clock time: "<<(long)(t1-t0)<<"\n";
        std::cout<<"elapsed CPU time: "<<(long)(c1-c0)/CLOCKS_PER_SEC<<"\n";


	return 0 ;
}
