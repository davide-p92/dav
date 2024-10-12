#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/init.h>

//define simple struct
struct my_object {
	int field1;
	int field2;
	int field3;
};

//declare slab cache ptr
static struct kmem_cache *my_cache;

//allocate an array of objects for test
static struct my_object *obj1, *obj2, *obj3;

//module init func
static int __init slab_example_init(void) {
	pr_info("Initializing custom slab cache example.\n");

	//create slab cache
	my_cache = kmem_cache_create("my_object_cache", sizeof(struct my_object), 0, SLAB_HWCACHE_ALIGN, NULL);
	if(!my_cache) {
		pr_err("Failed to create slab cache!\n");
		return -ENOMEM;
	}

	//allocate objects from the slab cache
	obj1 = kmem_cache_alloc(my_cache, GFP_KERNEL);
	obj2 = kmem_cache_alloc(my_cache, GFP_KERNEL);
	obj3 = kmem_cache_alloc(my_cache, GFP_KERNEL);

	if(!obj1 || !obj2 || !obj3) {
		pr_err("Failed to allocate memory from slab cache\n");
		return -ENOMEM;
	}

	//simulate usage
	obj1->field1 = 10; obj1->field2 = 20; obj1->field3 = 30;
	obj2->field1 = 100; obj2->field2 = 200; obj2->field3 = 300;
	obj3->field1 = 1000; obj3->field2 = 2000; obj3->field3 = 3000;

	pr_info("Allocated objects:\n");
	pr_info("obj1: field1 = %d, field2 = %d, field3 = %d\n", obj1->field1, obj1->field2, obj1->field3);
	pr_info("obj2: field1 = %d, field2 = %d, field3 = %d\n", obj2->field1, obj2->field2, obj2->field3);
	pr_info("obj3: field1 = %d, field2 = %d, field3 = %d\n", obj3->field1, obj3->field2, obj3->field3);

	return 0;
}


//module exit func
static void __exit slab_example_exit(void) {
	pr_info("Cleaning up slab example...\n");

	//free objects back to slab cache
	kmem_cache_free(my_cache, obj1);
	kmem_cache_free(my_cache, obj2);
	kmem_cache_free(my_cache, obj3);

	//destroy slab cache
	kmem_cache_destroy(my_cache);
}

module_init(slab_example_init);
module_exit(slab_example_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Davide Pitton");
MODULE_DESCRIPTION("A simple example of slab allocation in Linux kern");
