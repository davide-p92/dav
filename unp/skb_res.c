#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>

static int __init(void) {
	struct sk_buff *skb;
	int reserve_size = 64; //64 B

	//allocate a new socket buffer
	skb = alloc_skb(ETH_DATA_LEN, GFP_ATOMIC);
	if(!skb) {
		printk(KERN_ERR "Failed to allocate skb\n");
		return -ENOMEM;
	}

	//Reserve space in the socket buffer
	skb_reserve(skb, reserve_size);

	//access reserved area for custom header / metadata
	//e.g. custom data in reserved area
	memset(skb->data, 0xAA, reserve_size);

	print(KERN_INFO, "Reserved %d bytes in skb.\n", reserve_size);
	//free allocated skb
	kfree_skb(skb);
	return 0;
}

static void __exit skb_reserve_example_exit(void) {
	printk(KERN_INFO "skb_reserve example module unloaded.\n");
}

module_init(skb_reserve_example_init);
module_exit(skb_reserve_example_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Davide Pitton & CG");
MODULE_DESCRIPTION("skb_reserve Example Module");
MODULE_VERSION("1.0");
