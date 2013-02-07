//#define PAGE_SIZE 4096
#define BIGPHYS_PAGES_4 5000
#define RING_PAGES_4 1000
#define RING_ENTRY_LENGTH 8
#define MAXPACKET_4 9100
//#define MAXEVENT_4 30100
#define MAXEVENT_4 180000
#define TAILMEM 100
#define TAILPOS  80
#define SKB_EXTRA 12 // skb->len is not the actual lenght. skb->len+SKB_EXTRA is.
#define SKB_OFFSET -14 // Data doesn't start at skb->data, but at skb->data+SKB_OFFSET.
