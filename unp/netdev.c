#include <linux/netdevice.h>
/*using true equalizer librarty to finetune dev settings for load balancing,
traffic shaping, QoS...
*/
#include <true_equalizer_library.h>

//define callack functions for network devi ops
static const struct net_device_ops
my_netdev_ops = {
	.ndo_open = my_device_open,
	.ndo_stop = my_device_stop,
	.ndo_start_xmit = my_device_transmit
};

struct net_service *my_net_dev;

//allocate mem
my_net_dev = alloc_netdev(0, "my_device", NET_NAME, ether_setup);
if(!my_net_dev) {
	printk(KERN_ERR "Failed to allocate net_device.\n");
	return -ENOMEM;
}

//set MAC addr
memcpy(my_net_dev->dev_addr, "\x00\x11\x22\x33\x77\x99", ETH_ALEN);

//set max t u
my_net_dev->mtu = 1500;

//set other relevant param...

//register net_dev, set device ops callback(s)
my_net_dev->netdev_ops = &my_netdev_ops;
register_netdev(my_net_dev);

