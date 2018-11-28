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
int Account :: balance = 27;

struct transaction{
private:
	int type;
	int no;
	int amount;
	int Balance;

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



queue<transaction*> q;
int currentTransNo = 0;

struct Emitter : ff_node_t<transaction>{
	Emitter(ff_loadbalancer *const lb):lb(lb){}
	transaction *svc(transaction *t){
		int channelID = lb->get_channel_id();

                //std :: cout <<"get feedback from channel: "<<channelID<<"\n";

		if(channelID < 0){
			//std :: cout << "Transaction number " << t->getAccountNo() << " coming from Stage0 \n" ;
			if(t->getType() != 0 && t->getAccountNo()> currentTransNo +1)
	{
			q.push(t);	
			return GO_ON;
		}
			return t;	
		}
		else{
			std :: cout<< "Results: [" <<  t->getResult() << "] coming from Worker" << channelID <<"\n";
			currentTransNo = t->getAccountNo();
			if(q.empty()){
				delete t;
				return GO_ON;
			}
			else{
				transaction *t2 =q.front();
				
				if(t2->getAccountNo() < currentTransNo){
					q.pop();
					return t2;
				}
				else{
					return GO_ON;
				}
			}
		}
	}
	void eosnotify(ssize_t){
		lb->broadcast_task(EOS);
	}

	ff_loadbalancer *lb;
};


struct firstStage : ff_node_t<transaction>{
	
	transaction *svc(transaction *task){

		transaction *t1 = new transaction(1,4,0);
		transaction *t2 = new transaction(0,0,1);
		transaction *t3 = new transaction(0,0,2);
		transaction *t4 = new transaction(1,4,3);
		transaction *t5 = new transaction(0,0,2);
		transaction *t6 = new transaction(0,0,2);
		transaction *t7 = new transaction(1,4,4);
		transaction *t8 = new transaction(0,0,5);
		transaction *t9 = new transaction(1,4,6);
		transaction *t10 = new transaction(0,0,7);
		transaction *t11 = new transaction(0,0,8);
		transaction *t12 = new transaction(1,4,9);
		transaction *t13 = new transaction(0,0,10);
		transaction *t14 = new transaction(0,0,2);
		transaction *t15 = new transaction(0,0,2);
		ff_send_out(t1);
		ff_send_out(t2);
		ff_send_out(t3);
		ff_send_out(t4);
		ff_send_out(t5);
		ff_send_out(t6);
		ff_send_out(t7);
		ff_send_out(t8);
		ff_send_out(t9);
		ff_send_out(t10);
		ff_send_out(t11);
		ff_send_out(t12);
		ff_send_out(t13);
		ff_send_out(t14);
		ff_send_out(t15);
		getchar();
		return EOS;
	}

};

struct lastStage : ff_node_t<transaction>{
	
	transaction *svc(transaction *t){
		Account acc;
		std :: cout <<"Collector received: " << t->getResult() <<"\n";
		delete t;
		return GO_ON;
	}
};

int main (int argc,char *argv [] ){
	

	clock_t c0,c1;
	time_t t0,t1;
	
	c0 = clock();
	t0 = time(NULL);	
	std :: cout << "farm:\n";
	
	int n =10;


	std :: vector<std::unique_ptr<ff_node>> workers;
	for (int i =0;i<10;++i)
		workers.push_back(make_unique<Worker>());
	
	ff_Farm<> farm(std::move(workers));
	
	farm.remove_collector();	

	firstStage stage0;
	Emitter emitter(farm.getlb());
        lastStage collector;
	
	farm.add_emitter(emitter);
		
	farm.wrap_around();

	ff_Pipe<> pipe(stage0,farm);	

	if (pipe. run_and_wait_end()<0) 
		error("running myFarm");

	c1 = clock();
	t1 = time(NULL);
	
	std::cout<<"elapsed wall clock time: "<<(long)(t1-t0)<<"\n";
        std::cout<<"elapsed CPU time: "<<(long)(c1-c0)/CLOCKS_PER_SEC<<"\n";


	

	return 0 ;
}
