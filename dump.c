#include<string.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<net/ethernet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/ioctl.h>
#include<linux/if_ether.h>
#include<linux/if_packet.h>
#include<linux/if_arp.h>

struct ip_packet
{
	unsigned 
		head_len:4,
		version:4,
		type:8,
		total_len:16;
	unsigned
		sympol:16,
		flag:4,
		offset:12;
	unsigned
		live:8,
		proto:8,
		check:16;
	unsigned
		src_addr;
	unsigned
		des_addr;
};

struct tcp_packet
{
	unsigned
		src_port:16,
		des_port:16;
	unsigned
		seq_num;
	unsigned
		ack_num;
	unsigned
		save1:4,
		head_len:4,
		FIN:1,
		SYN:1,
		RST:1,
		PSH:1,
		ACK:1,
		URG:1,
		save2:2,
		window_size:16;
	unsigned
		check:16,
		eme_ptr:16;
};

struct udp_packet
{
	unsigned
		src_port:16,
		des_port:16;
	unsigned
		len:16,
		check:16;
};

void handle(char* buf,int size);
void show_ipmessage(struct ip_packet* ipp);
void show_tcpmessage(struct tcp_packet* tpp,unsigned short len);
void show_udpmessage(struct udp_packet* upp);


int main()
{
	int sock = socket(AF_PACKET,SOCK_RAW,htons(ETH_P_IP));
	if(sock < 0)
	{
		perror("socket eroor\n");
		return 0;
	}

	char buf[1514]= {0};
	while(1)
	{
		memset(buf,0,1514);
		int ret = read(sock,buf,sizeof(buf));
		if(ret <= 0)
		{
			perror("read error\n");
			return 0;
		}
		handle(buf,ret);
	}
}

void handle(char* buf,int size)
{
	
	struct ip_packet* ipp = NULL;
	struct tcp_packet* tpp = NULL;
	struct udp_packet* upp = NULL;

	unsigned short type = ntohs(*(unsigned short*)(buf + 12));
	if(type != 0x0800)
	{
		printf("it's not ip message\n");
		return;
	}

	ipp = (struct ip_packet*)(buf + 14);
	unsigned offset = ipp->head_len * 4;
	if(ipp->proto == 6)
	  tpp = (struct tcp_packet*)(buf + 14 + offset);
	else if(ipp->proto == 17)
	  upp = (struct udp_packet*)(buf+ 14 + offset);

	if(tpp && (ntohs(tpp->des_port) == 6000 || ntohs(tpp->src_port) == 6000))
	{
		show_ipmessage(ipp);
		show_tcpmessage(tpp,ntohs(ipp->total_len) - ipp->head_len * 4);
		printf("*******************Over*********************\n\n\n\n");
	}
	else if(upp && (ntohs(upp->des_port == 6000) || ntohs(upp->src_port) == 6000))
	{
		show_ipmessage(ipp);
		show_udpmessage(upp);
		printf("*******************Over*********************\n\n\n\n");
	}
}


void show_ipmessage(struct ip_packet* ipp)
{
	if(NULL == ipp)
	  return ;

	struct in_addr as,ad;
	as.s_addr = ipp->src_addr;
	ad.s_addr = ipp->des_addr;
	printf("*******************IP Head**********************\n");
	printf("源地址:%s\n",inet_ntoa(as));
	printf("目标地址:%s\n",inet_ntoa(as));
	printf("Version:%d\n",ipp->version);
}
void show_tcpmessage(struct tcp_packet* tpp,unsigned short len)
{
	if(NULL == tpp)
	  return;
	
	printf("*******************TCP Head**********************\n");
	printf("源端口:%d\n",ntohs(tpp->src_port));
	printf("目的端口:%d\n",ntohs(tpp->des_port));
	printf("序列号:%u\n",ntohl(tpp->seq_num));
	printf("确认号:%u\n",ntohl(tpp->ack_num));
	printf("SYN:%d	ACK:%d	FIN:%d	PSH:%d\n",tpp->SYN,tpp->ACK,tpp->FIN,tpp->PSH);
	printf("数据:%s\n",(char*)tpp + tpp->head_len * 4);
}
void show_udpmessage(struct udp_packet* upp)
{
	if(NULL == upp)
	  return;

	printf("*******************UDP Head**********************\n");
	printf("源端口:%d\n",ntohs(upp->src_port));
	printf("目的端口:%d\n",ntohs(upp->des_port));
	printf("数据%s\n",(char*)upp + 8);
}
