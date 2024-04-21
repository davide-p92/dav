#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>

//1 - KERNEL MODULE SETUP
//define kernel module details:
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Davide Pitton and CG");
MODULE_DESCRIPTION("TCP Header Modification Module");
MODULE_VERSION("1.0");

//Netfilter hook
static unsigned int hook_func(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
	//3 - MODIFY TCP HEADER
	//check if it's an IPv4 packet and TCp
	if(skb->protocol == htons(ETH_P_IP) && ip_hdr(skb)->protocol == IPPROTO_TCP)
	{
		struct tcphdr *tcp_header;
		tcp_header = tcp_hdr(skb); //access TCP header
		tcp_header->source = htons(NEW_SOURCE_PORT);
		//4 - REINJECT MODIFIED PACKET
		return nf_reinject(skb, state->net, state->pf, state->hook);
	}

	//Packet interception and modification code will go here
	return NF_ACCEPT; //default action for the intercepted packet
}

//Netfilter hooks: they provide packet inspection to kernel modules, 
//(headers, payload, metadata, IP, ports, ...)
//packet modification, enforcing firewall, connection tracking (state info)
static struct nf_hook_ops nfho = {
	.hook = hook_func, //hook func to be ccalld for intercepted pkts
	.hooknum = NF_INET_POST_ROUTING, //hook into outgoing packets
	.pf = PF_INET, //IPv4 packets
	.priority = NF_IP_PRI_FIRST //highest
};

//2 - HOOKING INTO OUTGOING PACKETS
//hook into outgoing packets
static int __init init_module(void) {
	//register the hook with Netfilter
	nf_register_net_hook(&init_net, &nfho);
	printk(KERN_INFO "TCP Header Modification Module loaded.\n");
	return 0;
}

//5 - CLEAN UP / UNLOADING
static void __exit cleanup_module(void) {
	//unregister the hook when unloading the module
	nf_unregister_net_hook(&init_net, &nfho);
	printk(KERN_INFO "TCP Header Modification Module unloaded.\n");
}

//module initialization and cleanup declarts
module_init(init_module);
module_exit(cleanup_module);
