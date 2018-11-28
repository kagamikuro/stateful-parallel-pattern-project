#include<iostream>
#include<ff/node.hpp>
using namespace ff;
using namespace std;

struct myNode:ff_node{
	long counter;
	int svc_init(){
		std :: cout <<"hello I am starting!\n";
		counter = 0;
		return 0;
	}
	void *svc(void *task){
		if(++counter >5)
			return EOS;//end of stream
		return GO_ON;//keep call me again
	}	
	void svc_end(){
		std :: cout <<"goodbye!\n";
	}

	int run_and_wait_end(bool=false){
		if(ff_node::run()<0) return -1;
		return ff_node::wait();
	}

	int run_then_freeze(){
		return ff_node::freeze_and_run();
	}
	int wait_freezing(){
		return ff_node::wait_freezing();
	}
	int wait(){
		return ff_node::wait();
	}
};


int main(){
	myNode mynode;
	if(mynode.run_and_wait_end()<0)
		error("running myNode\n");
	std::cout << "first run done \n\n";
	long i=0;
	do{
		if(mynode.run_then_freeze()<0)
			error("running myNode\n");
		if(mynode.wait_freezing())
			error("waiting myNode\n");
		std :: cout <<"run"<<i<<"done\n\n";
	}while(++i<3);
	if(mynode.wait())
		error("waiting myNode");
	return 0;

}
