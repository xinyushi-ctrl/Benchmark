#include <x86_energy.h>

#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define MAX 1000000


void print_time()
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer [128];
 
    time (&rawtime);
 
    timeinfo = localtime (&rawtime);
    strftime (buffer,sizeof(buffer),"%Y/%m/%d %H:%M:%S",timeinfo);
    printf("%s      ", buffer);
}

void addition(){
    int i,sum = 0;
    for(i = 0;i < MAX; i++)
        sum++;
//    printf("%d  ",sum);
    
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
    for (size_t i = 0; i < a->nr_avail_sources; i++)
    {
        printf("\nTesting source %s\n", a->avail_sources[i].name);
        int ret = a->avail_sources[i].init();
        if (ret != 0)
        {
            printf("Init failed\n");
            continue;
        }
        for (int j = 0; j < X86_ENERGY_COUNTER_SIZE; j++)
        {
            if (a->source_granularities[j] >= X86_ENERGY_GRANULARITY_SIZE)
                continue;
            printf("Try counter %d\n", j);
            for (int package = 0;
                 package < x86_energy_arch_count(hw_root, a->source_granularities[j]); package++)
            {
                printf("avail for granularity %d. There are %d devices avail for this counter, "
                       "testing %d\n",
                       a->source_granularities[j],
                       x86_energy_arch_count(hw_root, a->source_granularities[j]), package);
                x86_energy_single_counter_t t = a->avail_sources[i].setup(j, package);
                if (t == NULL)
                {
                    printf("Could not add counter %d for package\n", j);
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
                    printf("Read value %e\n", value2 - value);
                    sleep(1);

                    addition();
                    value = a->avail_sources[i].read(t);               
                    print_time();
                    printf("Read value %e\n", value - value2);
                    sleep(1);
                }

                a->avail_sources[i].close(t);
            }
        }
        a->avail_sources[i].fini();
    }
    x86_energy_free_architecture_nodes(hw_root);
}
