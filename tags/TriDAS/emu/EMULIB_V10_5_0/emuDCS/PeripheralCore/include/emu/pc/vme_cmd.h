int ife;

// unsigned int vmeadd_tmp;
unsigned int add_i,add_d,add_dh,add_ds,add_dt,add_sw,add_sr,add_rst,add_r;
unsigned int add_loww,add_lowr,add_lowwpr,add_lowrpr,add_lowrs,add_lowws;
unsigned int add_reset,add_cdac, add_vmefpga;
unsigned int add_adcw,add_adcr,add_adcrbb,add_adcws,add_adcrs;
unsigned int add_fifo_w00,add_fifo_w01,add_fifo_w10,add_fifo_w11,add_fifo_rln,add_fifo_rli,add_fifo_rhn,add_fifo_rhi,add_fifo_ws,add_fifo_rs,add_fifo_i;
unsigned int add_bucip,add_bucl,add_bucf,add_bucr,add_buci,add_buce;
unsigned int add_control_r,add_control_w;
unsigned int add_ucla;

unsigned long msk00={0x00000000};
unsigned long msk01={0x00001000};
unsigned long msk02={0x00002000};
unsigned long msk03={0x00003000};
unsigned long msk04={0x00004000};
unsigned long msk05={0x00005000};
unsigned long msk06={0x00006000};
unsigned long msk07={0x00007000};
unsigned long msk08={0x00008000};
unsigned long msk09={0x00009000};
unsigned long msk7f={0x0007fffe};
unsigned long msk0f={0x0000fffe};

unsigned long msk_clr={0xfffff0ff};
unsigned long msk_rst={0x00000018};
unsigned long msk_i={0x0000001c};
unsigned long msk_d={0x0000000c};
unsigned long msk_dh={0x00000004};
unsigned long msk_ds={0x00000000};
unsigned long msk_dt={0x00000008};
unsigned long msk_sw={0x00000020};
unsigned long msk_sr={0x00000024};
unsigned long msk_r={0x00000014};
unsigned long msk_adcw={0x00000000};
unsigned long msk_adcr={0x00000004};
unsigned long msk_adcrbb={0x0000000c};
unsigned long msk_adcws={0x00000020};
unsigned long msk_adcrs={0x00000024};
unsigned long msk_fifo_w00={0x00000000};
unsigned long msk_fifo_w01={0x00000004};
unsigned long msk_fifo_w10={0x00000008};
unsigned long msk_fifo_w11={0x0000000c};
unsigned long msk_fifo_rln={0x00000010};
unsigned long msk_fifo_rli={0x00000014};
unsigned long msk_fifo_rhn={0x00000018};
unsigned long msk_fifo_rhi={0x0000001c};
unsigned long msk_fifo_ws={0x00000020};
unsigned long msk_fifo_rs={0x00000024};
unsigned long msk_fifo_i={0x0000002c};
unsigned long msk_loww={0x00000000};
unsigned long msk_lowr={0x00000004};
unsigned long msk_lowwpr={0x00000010};
unsigned long msk_lowrpr={0x00000014};
unsigned long msk_lowws={0x00000020};
unsigned long msk_lowrs={0x00000024};
unsigned long msk_bucip={0x00000000};
unsigned long msk_bucl={0x00000004};
unsigned long msk_bucf={0x00000008};
unsigned long msk_bucr={0x0000000c};
unsigned long msk_buci={0x00000010};
unsigned long msk_buce={0x00000014};
unsigned long msk_control_r={0x00082800};
