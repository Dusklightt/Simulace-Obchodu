#include <simlib.h>
#include <getopt.h>
#include <stdio.h>

const static int default_customer = 100;
const static double default_deviation = 20;
const static int default_cashbox_count = 3;
const static int default_minutes = 210;
Facility *cashboxes_global;

Stat *averageArrivalTime = new Stat("Average time between customer arrival");
Stat *averageWaitTime = new Stat("Average queue waiting time");
Stat *averageServiceTime = new Stat("Average service time");

class Customer : public Process
{
	long cashbox_count;
	double Arrival;
	double Service;
	void Behavior(){
		Arrival = Time;
		int i = 0;
		for(int a=1; a < cashbox_count; a++){
			if((cashboxes_global)[i].QueueLen() > (cashboxes_global)[a].QueueLen()){
				i=a;
			}
		}
		Seize(*cashboxes_global[i]);
		(*averageWaitTime)(Time-Arrival);
		Service = Time;
		Wait(Uniform(60,180));
		Release(*cashboxes_global[i]);
		(*averageServiceTime)(Time-Service);
	}
public:
	Customer(long cashbox_count) : cashbox_count(cashbox_count) {
		Activate();
	}

};

class Generator : public Event
{
	double interval;
	double deviation;
	long cashbox_count;
	void Behavior(){
		
		(new Customer(cashbox_count));
		double devValue = (deviation / 100)*interval;
		double randomInterval = Uniform(interval - devValue, interval + devValue);
		Activate(Time+(60/randomInterval));
		(*averageArrivalTime)(60/randomInterval);
	}

	void PrintEnd(){
		for(int j=0; j < cashbox_count; j++){
			(*cashboxes_global)[j].Output();
		}
	}

public:
	Generator(double interval, double deviation, long cashbox_count) : interval(interval), deviation(deviation), cashbox_count(cashbox_count){
		Activate();	
	}
};

void print_usage(){
	fprintf(stdout, "usage\n");
}

int main(int argc, char *argv[]) {
	double customer_interval;
	long int customer_per_hour = default_customer;
	long int cashbox_count = default_cashbox_count;
	long int minutes = default_minutes;
	int opt;
	static const struct option long_opts[] = {
		{ "interval", required_argument, 0, 'i'},
		{ "count", required_argument, 0, 'c'},
		{ "minutes", required_argument, 0, 'm'},
		{0,0,0,0}	
	};

	while((opt = getopt_long(argc, argv, "i:c:m:", long_opts, 0)) != -1){
		switch(opt){
			case 'i':
				
				customer_per_hour = strtol(optarg, nullptr, 10);	

				if(customer_per_hour < 0){
					fprintf(stderr, "Interval of customer arrival must be positive.\n");
					return 1;
				}
				break;
			case 'c':

				cashbox_count = strtol(optarg, nullptr, 10);

				if(cashbox_count < 0){
					fprintf(stderr, "Maximum number of cashboxes must be positive.\n");
					return 1;
				}
				break;
			case 'm':

				minutes = strtol(optarg, nullptr, 10);

				if(cashbox_count < 0){
					fprintf(stderr, "Time in minutes must be positive.\n");
					return 1;
				}
				break;
			default:
				print_usage();
				return 1;
		}
	}
	
	customer_interval = customer_per_hour/60.00;
	
	
	cashboxes_global = new Facility[cashbox_count];
	fprintf(stdout, "Running simulation with %.2f customer interval and %ld maximum cashboxes\n", customer_interval, cashbox_count);
	
	for(int i=1; i<=10; i++)  {
		Init(0, minutes*60);          		// init time
		for(int j=0; j<cashbox_count; j++){
			cashboxes_global[j].Clear();
		}
		averageArrivalTime->Clear();
		averageWaitTime->Clear();
		averageServiceTime->Clear();
		fprintf(stdout, "############################################################\n"
				"%d. simulace\n", i);
		(new Generator(customer_interval, default_deviation, cashbox_count));
		
		Run();                		// single simulation experiment
		averageArrivalTime->Output();
		averageWaitTime->Output(); 	// print run statistics
		averageServiceTime->Output();
	}

	return 0;
}

