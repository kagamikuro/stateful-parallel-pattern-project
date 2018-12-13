#include <vector>
#include <iostream>
#include <ff/farm.hpp>
#include <ff/pipeline.hpp>
#include <math.h>
#include <cstring>
#include <string>
#include <queue>


using namespace ff;
using namespace std;



class Account{
public:
	static int balance;

};

std::mutex mt;
int Account :: balance = 30;
static int numTasks = 0;

//config array , firstNumber is type, secondNumber is amount
int config[15][2] = {{1,5},{0,0},{0,0},{0,0},{0,0},{1,5},{0,0},{0,0},{0,0},{1,5},{0,0},{0,0},{0,0},{1,5},{0,0}} ;

//flag for type, 0 is no running work , >0 means workers are reading ,<0 means workers are writing
volatile static int current_running_type = 0;

struct transaction{
private:
	int type;
	int no;
	int amount;
	int Balance;
	bool executed;


public:
	transaction(int t,int a,int n){
		/*
		 *type 0: checkBalance;
		 *type 1: save money;
		 *type 2: withdraw money;
		 */
		type = t;
		amount = a;
		no = n;
		executed = false;
		numTasks++;
	}

	void Sleep(){
		for(int n=0;n<1000000000;n++)
			sin(n);

	}
	

	void save(Account acc){
		for(int n=0;n<amount;n++){
			acc.balance += 1;
			Sleep();
		}
		Balance = acc.balance;
	}

	void withdraw(Account acc){
		for(int n=0;n<amount;n++){
			acc.balance -= 1;
			Sleep();
		}
		Balance = acc.balance;
	}

	void checkBalance(Account acc){
		Balance = acc.balance;
	}
	int getType(){
		return type;
	}
	int getAccountNo(){
		return no;
	}
	void setExecute(){
		executed = true;
	}

	bool isExecute(){
		return executed;
	}

	string getResult(){
		char str[1000];
		if(type == 0)
			sprintf(str,"check balance: balance is %d",Balance);
		if(type == 1)
			sprintf(str,"save %d, after transaction balance is %d",amount,Balance);
		if(type == 2)
			sprintf(str,"withdraw %d, after transaction balance is  %d",amount,Balance);


		return str;
	}  	

	
};


struct Worker:ff_node_t<transaction>{

	transaction *svc (transaction *t){
		//if the transaction has been execured,just pass it
		if(t->isExecute())
			return t;		
		Account acc;
		if(t->getType() == 0)
			t->checkBalance(acc);
		if(t->getType() == 1){
			mt.lock();
			t->save(acc);
			mt.unlock();
		}
		if(t->getType() == 2){
			mt.lock();
			t->withdraw(acc);
			mt.unlock();
		}
		return t;
	}
};



int currentTransNo = 0;

struct Emitter : ff_minode_t<transaction>{
	
	void smallSleep(){
		int x = 0;
		for(int n=0;n<10000000;n++)
			x = sin(n);
	}


	transaction *svc(transaction *t){
		if (t==NULL){

			std :: cout <<"hello start!"<<"\n";
			for(int n=0;n<15;n++){
				transaction *t = new transaction(config[n][0],config[n][1],n);			
				//workers are writing
				int i=0;
				while(current_running_type <0 && config[n][0]==0) ;
					//smallSleep();
					//std::cout<<"current_type: "<<current_running_type<<"\n";
				//workers are reading
				while(current_running_type >0 && (config[n][0]==1||config[n][0]==2)) ;
					//smallSleep();	
					//std::cout<<"current_type: "<<current_running_type<<"\n";
					
				ff_send_out(t);
				if(config[n][0]==0)
					current_running_type++;
				if(config[n][0]==1||config[n][0]==2)
					current_running_type--;
			}			
			return EOS;
		}
		else{
			std :: cout <<"Emitter receive feedback transaction "<<t->getAccountNo()<<" from collector!"<<"\n";
			t->setExecute();
			//if(t->getType()==0)
			//	current_running_type--;
			//if(t->getType()==1||t->getType()==2)
                        //        current_running_type++;
			//numTasks--;
			//if(numTasks == 0)
			//	return EOS;
		}		
		return GO_ON;

	}

};


struct lastStage : ff_monode_t<transaction>{
	
	transaction *svc(transaction *t){
		Account acc;
		//std :: cout <<"Collector received transaction : "<<t->getAccountNo()<<" "<< t->getResult() <<"\n";

		if(t->getType()==0)
                     current_running_type--;
                if(t->getType()==1||t->getType()==2)
                     current_running_type++;


		//if(!t->isExecute())
		//	ff_send_out_to(t,0);
		//else
			ff_send_out_to(t,1);
			
		return GO_ON;
	}
};


struct finalStage : ff_node_t<transaction>{

        transaction *svc(transaction *t){
                Account acc;
                std :: cout <<"final node received : "<<t->getAccountNo()<<" "<< t->getResult() <<"\n"; 


                return GO_ON;
        }
};


int main (int argc,char *argv [] ){
	

	clock_t c0,c1;
	time_t t0,t1;
	
	c0 = clock();
	t0 = time(NULL);	


	std :: vector<std::unique_ptr<ff_node>> workers;
	for (int i =0;i<10;++i)
		workers.push_back(make_unique<Worker>());
	
	ff_Farm<> farm(std::move(workers));
	Emitter emitter;
	lastStage collector;
	finalStage finalNode;


	ff_Pipe<> pipe1(emitter,farm,collector);
	pipe1.wrap_around();	
	ff_Pipe<> pipe2(pipe1,finalNode);	


	if (pipe2. run_and_wait_end()<0) 
		error("running myFarm");

	c1 = clock();
	t1 = time(NULL);
	
	std::cout<<"elapsed wall clock time: "<<(long)(t1-t0)<<"\n";
        std::cout<<"elapsed CPU time: "<<(long)(c1-c0)/CLOCKS_PER_SEC<<"\n";


	

	return 0 ;
}
