//#define PAGE_SIZE 4096
#define BIGPHYS_PAGES_3 5000 
#define RING_PAGES_3 1000
#define RING_ENTRY_LENGTH 8
#define MAXPACKET_3 9100
//#define MAXEVENT_3 30100
#define MAXEVENT_3 180000
#define TAILMEM 100
#define TAILPOS  80
//#define SKB_EXTRA 14
#define SKB_EXTRA 12 // skb->len is not the actual lenght. skb->len+SKB_EXTRA is.
#define SKB_OFFSET -14 // Data doesn't start at skb->data, but at skb->data+SKB_OFFSET.
