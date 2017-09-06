#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>


int nr_queues = 0;
module_param(nr_queues, int, 0);

static int test_cpu_to_queue_index(unsigned int nr_queues, const int cpu)
{
	/*
	 * Non present CPU will be mapped to queue index 0.
	 */
	if (!cpu_present(cpu))
		return 0;
	return cpu % nr_queues;
}

static int test_get_first_sibling(unsigned int cpu)
{
	unsigned int ret;

	ret = cpumask_first(topology_sibling_cpumask(cpu));
	if (ret < nr_cpu_ids)
		return ret;

	return cpu;
}

int map[2014];
/* code from block/blk-mq_cpumap.c */
int test_blk_mq_map_queues(void)
{
	unsigned int cpu, first_sibling;

	for_each_possible_cpu(cpu) {
		printk(KERN_INFO "---------------Processing CPU %d---------------\n", cpu);
		if (cpu < nr_queues) {
			map[cpu] = test_cpu_to_queue_index(nr_queues, cpu);
			printk(KERN_INFO " cpu < nr_queues [%d] = %d\n", cpu, map[cpu]);
		} else {
			first_sibling = test_get_first_sibling(cpu);
			if (first_sibling == cpu) {
				map[cpu] = test_cpu_to_queue_index(nr_queues, cpu);
				printk(KERN_INFO "first_sibling == cpu map[%d] = %d\n", cpu, map[cpu]);
			} else {
				map[cpu] = test_cpu_to_queue_index(nr_queues, cpu);
				map[cpu] = map[first_sibling];
				printk(KERN_INFO "first_sibling != cpu map[%d] = %d\n", cpu, map[cpu]);
			}
		}
	}

	return 0;
}

static int __init hello_init(void)
{
	if (nr_queues == 0)
		nr_queues = nr_cpu_ids;

	test_blk_mq_map_queues();
	return 0;
}

static void __exit hello_cleanup(void)
{
	printk(KERN_INFO "good bye..\n");
}

module_init(hello_init);
module_exit(hello_cleanup);
MODULE_LICENSE("GPL");
