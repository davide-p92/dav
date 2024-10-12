#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/net.h>

static int __init skb_read_init(void) {
	struct net_device *dev;
	struct sk_buff *skb;

	for_each_netdev(&init_net, dev) {
		struct sk_buff_head *skb_head = &dev->qdisc->q.qlen;
		skb_queue_walk(skb_head, skb) {
			printk(KERN_INFO "SKB: len=%u, data_len=%u, head=%p, data=%p\n",
					skb->len, skb->data_len, skb->head, skb->data);
		}
	}

	return 0;
}

static void __exit skb_read_exit(void) {
	printk(KERN_INFO "SKB Reader Module Exiting\n");
}

module_init(skb_read_init);
module_exit(skb_read_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Davide Pitton");
MODULE_DESCRIPTION("A module to read skbuff structs");
