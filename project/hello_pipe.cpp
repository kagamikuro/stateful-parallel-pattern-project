#include<iostream>
#include<ff/pipeline.hpp>
#include<math.h>
using namespace ff;
using namespace std;


typedef double myTask;


struct firstStage : ff_node_t<myTask>{
	myTask *svc(myTask*){

		for(int count = 1;count<=8;count++){
			myTask *result = new myTask(0);
			for(long i=0;i<100000000;++i)
				*result += sin(i);			

			ff_send_out(result);
				//ff_send_out(new Task(i));//send tasks to next stage
		}
		return EOS;
	}	
};

struct secondStage:ff_node{
	void *svc(void *t){
		//std :: cout <<"Hello I'm stage 2,I've received:"<<*((myTask*)t)<<"\n";
		for(long i=100000000;i<200000000;++i)
			*((myTask*)t) += sin(i);
		//std :: cout <<"Hello I'm stage 2,I've received:"<<*((myTask*)t)<<"\n";
		return t;//pass the task to the next stage
	}

};

struct thirdStage:ff_node{
	void *svc(void *t){
		//std :: cout <<"Hello I'm stage 3,I've received:"<<*((myTask*)t)<<"\n";
		for(long i=200000000;i<300000000;++i)
                     *((myTask*)t) += sin(i);
		
		//std :: cout <<"Hello I'm stage 3,I've received:"<<*((myTask*)t)<<"\n";
		return t;

	}


};

struct fourthStage:ff_node{
        void *svc(void *t){
		//std :: cout <<"Hello I'm stage 4,I've received:"<<*((myTask*)t)<<"\n";

                for(long i=300000000;i<400000000;++i)
                        *((myTask*)t) += sin(i);

                //std :: cout <<"Hello I'm stage 4,I've received:"<<*((myTask*)t)<<"\n";
                return t;

        }


};

struct fifthStage:ff_node{
        void *svc(void *t){

                for(long i=400000000;i<500000000;++i)
                       *((myTask*)t) += sin(i);

                //std :: cout <<"Hello I'm stage 5,I've received:"<<*task<<"\n";
                return t;

        }


};

struct sixthStage:ff_node{
        void *svc(void *t){

                for(long i=500000000;i<600000000;++i)
                       *((myTask*)t) += sin(i);

                //std :: cout <<"Hello I'm stage 6,I've received:"<<*task<<"\n";
                return t;

        }


};

struct seventhStage:ff_node{
        void *svc(void *t){

                for(long i=600000000;i<700000000;++i)
                       *((myTask*)t) += sin(i);

                //std :: cout <<"Hello I'm stage 7,I've received:"<<*task<<"\n";
                return t;

        }


};

struct eighthStage:ff_node_t<myTask>{
        myTask *svc(myTask *task){

                for(long i=700000000;i<800000000;++i)
                       *task += sin(i);

                std :: cout <<"Hello I'm stage 8,I've received:"<<*task<<"\n";
                return GO_ON;

        }


};




int main(){
		
	std::cout<<"pipe: "<<"\n";
	clock_t c0,c1;
	time_t t0,t1;
	
	c0 = clock();
	t0 = time(NULL);
	
	firstStage _1;
	secondStage _2;
	thirdStage _3;
	fourthStage _4;
	fifthStage _5;
	sixthStage _6;
	seventhStage _7;
       	eighthStage _8;

	ff_Pipe<> pipe(_1,_2,_3);
	
	pipe.add_stage(_4);
	pipe.add_stage(_5);
	pipe.add_stage(_6);
	pipe.add_stage(_7);
	pipe.add_stage(_8);


	if(pipe.run_and_wait_end()<0)
		error("running pipe");
	c1 = clock();
	t1 = time(NULL);
	std::cout<<"elapsed wall clock time: "<<(long)(t1-t0)<<"\n";
	std::cout<<"elapsed CPU time: "<<(long)(c1-c0)/CLOCKS_PER_SEC<<"\n";


	std::cout<<"serial: "<<"\n";
	c0 = clock();
        t0 = time(NULL);
	
	for(int count = 1;count<=8;count++){
		double result = 0;	
		for(long i=0;i<800000000;++i)
			result += sin(i);	   
		std :: cout <<"result from serial is "<< result << "\n";
	}
	c1 = clock();
        t1 = time(NULL);
        std::cout<<"elapsed wall clock time: "<<(long)(t1-t0)<<"\n";
        std::cout<<"elapsed CPU time: "<<(long)(c1-c0)/CLOCKS_PER_SEC<<"\n";
        
		
}


