#include <x86_energy.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#define MAX 1000000


void print_time() // precision millisecond
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer [128];
    struct timeval tv;  
    gettimeofday(&tv,NULL);  

    time (&rawtime);
 
    timeinfo = localtime (&rawtime);
    strftime (buffer,sizeof(buffer),"%Y/%m/%d %H:%M:%S",timeinfo);
    printf("%s  ", buffer);
    printf("timestamp(milli):%ld ",tv.tv_sec*1000 + tv.tv_usec/1000); 

}

void addition(){
    int i,sum = 0;
    for(i = 0;i < MAX; i++)
        sum++;    
}


int main()
{
    x86_energy_architecture_node_t* hw_root = x86_energy_init_architecture_nodes();
    x86_energy_mechanisms_t* a = x86_energy_get_avail_mechanism();
    if(a && a->name)
    {
        printf("Architecture: %s\n", a->name);
    } else {
    	printf(x86_energy_error_string());
    	return 1;
    }

    size_t i;
    printf("Please enter the number of the selecting testing source (sysfs=0; perf=1; msr=2):");
    scanf("%zu",&i);

    if (i < a->nr_avail_sources)
    {

        printf("\nTesting source %s\n", a->avail_sources[i].name);
        int ret = a->avail_sources[i].init();
        if (ret != 0)
        {
            printf("Init failed\n");
        }

    printf("Please enter the number of the selecting counter(PCKG=0; CORES=1; DRAM=2; GPU=3; PLATFORM=4; SINGLE_CORE=5):");
    
    int in;

    scanf("%d",&in);
    if(in>=X86_ENERGY_COUNTER_SIZE)
        printf("Wrong input,  please input again\n");
    else{
        for (int package = 0;package < x86_energy_arch_count(hw_root, a->source_granularities[in]); package++)
            {
                printf("avail for granularity %d. There are %d devices avail for this counter, "
                       "testing %d\n",
                       a->source_granularities[in],
                       x86_energy_arch_count(hw_root, a->source_granularities[in]), package);
                x86_energy_single_counter_t t = a->avail_sources[i].setup(in, package);
                if (t == NULL)
                {
                    printf("Could not add counter %d for package\n", in);
                    printf("%s", x86_energy_error_string());
                    continue;
                }
            
                double value = a->avail_sources[i].read(t);
                //printf("Read value %e\n", value);
                sleep(1);
                double value2 = 0;

                for(int x = 0; x < 5; x++){
                      
                    addition();
                    value2 = a->avail_sources[i].read(t);
                    print_time();                
                    printf("Read value %e Joules\n", value2 - value);
                    sleep(1);

                    addition();
                    value = a->avail_sources[i].read(t);               
                    print_time();
                    printf("Read value %e Joules\n", value - value2);
                    sleep(1);
                }

                a->avail_sources[i].close(t);
            }
        }
        a->avail_sources[i].fini();
    }
    x86_energy_free_architecture_nodes(hw_root);
}