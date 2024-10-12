//#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/module.h>
//#include <linux/delay.h>
#include <linux/kernel.h>
//#include <linux/init.h>

static struct task_struct *my_thread;

int my_thread_fn(void *data) {
	while(!kthread_should_stop()) {
		//your periodic task here
		pr_info("Kernel thread running!\n");
		ssleep(5);
	}
	return 0;
}

static int __init my_module_init(void) {
	//create and run a kernel thread
	my_thread = kthread_run(my_thread_fn, NULL, "my kthread");
	if(IS_ERR(my_thread)) {
		pr_err("Faild to create thread\n");
		return PTR_ERR(my_thread);
	}
	return 0;
}

static void __exit my_module_exit(void) {
	if(my_thread)
		kthread_stop(my_thread); //gracefully stop
}

module_init(my_module_init);
module_exit(my_module_exit);
