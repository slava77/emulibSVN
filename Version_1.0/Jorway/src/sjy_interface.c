/* ****************************************************************************
** FILE sjy_interface
**
** DESCRIPTION
**  User level code Interface between the CAMAC IEEE layer and Linux generic
**  scsi driver sg.c.
**
**
** DEVELOPERS
**  Dave Slimmer
**  Fermilab Online Systems Group
**  Batavia, Il 60510, U.S.A.
**
**
** MODIFCATIONS
**  Date    Initials Description
**  22sep97 das      derived from example to demonstrate the generic SCSI
**                   interface
**  19feb98 das      zero output buffer if command fails
**  14mar00 das      Readability improvements. Use cmd instead of memcopy cmdblk
**                   to cmd.
**  31mar00 das      Decode SCSI bus number from branch argument
**  16jun00 das      Modified handle_scsi_cmd so read data is copied to user buffer
**                   if a SHORT TRANSFER occurs indicated by sense key==9 with a 
**                   cfubc/csubc type read.
**  21jul00 das      bzero the cmd buffer in sjy_nondata, read, and write for case
**                   of short transfers (really needed for read)
**  17nov00 cjh      handle_scsi_cmd calculates transfer defecit from sense
**                   data, returns actual bytes transferred if Q-stopped
**  16jan02 das      use sg_hd->target_status instead of deprecated sg_hd->result
**                   in handle_scsi_cmd. Change sjy_notify_camac_nondata to make
**                   explicit check for SJY_CONDITION_MET.
**  23may02 das      removed unused variables i,cptr,lcmdblk,scmdblk
**  31jan03 das      handle_scsi_cmd: subtract word defecit from number of bytes 
**                   copied to user buffer, sjy_notify_camac_data: changed default
**                   return status to "no Q" (ENODEV), check explicitly for 
**                   SJY_CONDITION_MET before returning Q. This change requires
**                   the Fermi modified Jor73A firmware.
**  11apr03 das      added cast (unsigned char *)dataBuffer to satisfy g++
**  24jul03 das      added protection for corrupted defecit size
**  08nov05 das      set SJY411S non-data command size to 6 from 12 because
**                   of error messages sent to /var/log/messages like this:
**                    mtbf kernel: (scsi3:A:2:0): Abort Message Sent
**                    mtbf kernel: (scsi3:A:2:0): SCB 5 - Abort Completed.
**                    mtbf kernel: (scsi3:A:2:0): No or incomplete CDB sent to device.
**                    mtbf kernel: (scsi3:A:2:0): Protocol violation in Message-in pha
**                    se.  Attempting to abort.
** 06dec05 das       set SJY411S non-data command size back to 6 and added extra
**                   ioctl call to handle_scsi_cmd
**                   
**  ===========================================================================
*/
/*#define DEBUG*/

  #include <stdio.h>
  #include <unistd.h>
  #include <string.h>
  #include <fcntl.h>
  #include <errno.h>
  #include <signal.h>
  #include <sys/ioctl.h>
  #include <scsi/sg.h>
/*#include "/usr/src/linux/include/scsi/sg.h"*/
  #include "/usr/include/scsi/scsi.h"       /* defines STATUS_MASK for target_status */
  #include "ieee_fun_types.h"


  #define SCSI_OFF sizeof(struct sg_header)
  #define USER_BUF_SIZE (SG_BIG_BUFF - (SCSI_OFF + 18 + 511)) 
  #define CMD_SIZE (SCSI_OFF + 18 + USER_BUF_SIZE)
  static unsigned char cmd[CMD_SIZE]; /* SCSI command buffer ***Used for i_buff***/


/*
** handle_scsi_cmd - process a scsi command using the generic scsi interface
**
** returns
**  success  int status - number of bytes read minus the sizeof(sg_header)
**  fail     CAM_S_INVLD_ARG -  bad function argument
**           CAM_S_GETSEM_FAIL - failed to get semaphore
**           CAM_S_DEV_IO_ERROR - scsi write or read returned bad status 
**           CAM_S_PUTSEM_FAIL - failed to release semaphore
*/


static int handle_scsi_cmd(       int branch,          /* CAMAC branch */
			     unsigned cmd_len,         /* command length */
                             unsigned in_size,         /* input data size */
                             unsigned char *i_buff,    /* input buffer */
                                  int out_size,        /* output data size */
                             unsigned char *o_buff     /* output buffer */
                             )
  {
      int status = 0;
      struct sg_header *sg_hd;
      sigset_t newmask,oldmask;
      register int idx = IDX(branch);
      int fd = sjy_controller[idx].fd;
      int defecit = 0;
      int tmp = 0;
#ifdef SJY411S
      unsigned char *j411cmd; /* pointer to 411S command */
      int cmdlen = 6;         /* 411S non-data command length */
#endif

      /* safety checks */
      ioctl(fd, SG_GET_RESERVED_SIZE, &tmp);
      if (sizeof(cmd) > tmp) {
 	fprintf( stderr, "sjy_interface: read buffer size %d, sg reserved size %d\n", 
		 sizeof(cmd),tmp);
	return (CAM_S_DEV_IO_ERROR);
      }

      if (!cmd_len) return (CAM_S_INVLD_ARG);           /* need a cmd_len != 0 */
      if (!i_buff) return (CAM_S_INVLD_ARG);            /* need an input buffer != NULL */
  #ifdef SG_BIG_BUFF
      if (SCSI_OFF + cmd_len + in_size > SG_BIG_BUFF) return -1;
      if (SCSI_OFF + out_size > SG_BIG_BUFF) return -1;
  #else
      if (SCSI_OFF + cmd_len + in_size > 4096) return -1;
      if (SCSI_OFF + out_size > 4096) return -1;
  #endif

      if (!o_buff) out_size = 0;

      /* generic scsi device header construction */
      sg_hd = (struct sg_header *) i_buff;
      sg_hd->reply_len   = SCSI_OFF + out_size;
      sg_hd->twelve_byte = cmd_len == 12;
      sg_hd->result = 0;

  #if 0
      sg_hd->pack_len    = SCSI_OFF + cmd_len + in_size; /* not necessary */
      sg_hd->pack_id;     /* not used */
      sg_hd->other_flags; /* not used */
  #endif

      /* protect the write/read from being interrupted by ^C */
      sigemptyset(&newmask);
      sigaddset(&newmask, SIGINT);
      /*  block SIGINT and save current signal mask */
      if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
	fprintf( stderr, "sjy_interface: SIG_BLOCK error");


      /* protect the write/read section for multi-user */
       if(sjy_getsem() != 0){ 
 	fprintf( stderr, "sjy_interface: getsem failed"); 
 	sjy_putsem(); 
 	return(CAM_S_GETSEM_FAIL); 
       } 


#ifdef SJY411S  /* special case for 411S non-data commands */
      j411cmd = i_buff + SCSI_OFF;
      if (j411cmd[0] == SJY_NONDATA_CMD) {
	ioctl( fd, SG_NEXT_CMD_LEN, &cmdlen);
      }
#endif

      /* send command */
      status = write( fd, i_buff, SCSI_OFF + cmd_len + in_size );

#ifdef DEBUG
      if ( status < 0 || status != SCSI_OFF + cmd_len + in_size ||
                         sg_hd->result ) {
          if (status < 0) perror("sjy_interface");
          /* some error happened */
          fprintf( stderr, "SCSI_OFF = %d,cmd_len = %d,in_size = %d\n", 
		   SCSI_OFF,cmd_len,in_size);
          fprintf( stderr, "write(generic) result = 0x%x cmd = 0x%x\n",
                      sg_hd->result, i_buff[SCSI_OFF] );
	  sjy_putsem();
          sigprocmask(SIG_SETMASK, &oldmask, NULL);
          return (CAM_S_DEV_IO_ERROR);
      }
#else
      if ( status < 0 ) {
	sjy_putsem();
        sigprocmask(SIG_SETMASK, &oldmask, NULL);
        return (CAM_S_DEV_IO_ERROR);
      }
#endif
 
     
      /* Since the sg.c driver _read_ always returns the sg_header, we must
      ** provide a buffer +SCSI_OFF in size. Since i_buff was already
      ** allocated as the largest size data buffer we could use (+SCSI_OFF),
      ** we use it. We have to copy the data read back to the user's 
      ** buffer, but this ultimatly saves an extra malloc call.
      */

      /* retrieve result */
      if ((out_size+SCSI_OFF) <= sizeof(cmd)) {
	status = read( fd, i_buff, SCSI_OFF+out_size);
	if (status < 0) perror("sjy_interface");
      } else {
 	fprintf( stderr, "sjy_interface: read buffer size %d, requested %d\n", 
		 sizeof(cmd),SCSI_OFF+out_size);
      } 


      /* return the semaphore */
       if(sjy_putsem() != 0){ 
 	fprintf( stderr, "sjy_interface: getsem failed"); 
 	return(CAM_S_PUTSEM_FAIL); 
       } 

      /* reset signal mask which unblocks SIGINT */
      if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
	fprintf( stderr, "sjy_interface: SIG_SETMASK error");


      /* copy data read back to the user buffer */
      /* The generic scsi driver keeps an internal buffer with the
      ** last data read, and does not clear it when a subsequent SCSI
      ** command completes with a non-zero SCSI response. sg_hd->result
      ** does not contain the SCSI response as observed from a SCSI
      ** analyzer, so it is not used. The only alternative is to test
      ** for a non-zero sense buffer sense key field returned from the
      ** Jorway. If the sense key is non-zero, the user buffer is
      ** zeroed instead of returning stale data from the internal SCSI
      ** buffer.
      */
      
      /* by rex hubbard (jorway): before doing the above, check if the
      ** sensekey is 9, indicating a transfer stopped by q-response. If
      ** so, get the defecit from the sense data and adjust the return
      ** value. The data in the internal buffer is valid, so copy it.
      */

      if(!(sg_hd->sense_buffer[SENSEKEY])) {
	if (o_buff) { 
	  memcpy(o_buff, i_buff+SCSI_OFF, out_size);
	}
      } else {
	if (sg_hd->sense_buffer[SENSEKEY] == 9){
	  defecit = sg_hd->sense_buffer[4] << 8 | sg_hd->sense_buffer[5];
	  defecit = defecit << 8 | sg_hd->sense_buffer[6];
          if (defecit <= out_size){ 
            if (o_buff) {
              memcpy(o_buff, i_buff+SCSI_OFF, out_size-defecit);
            }
          } else {
            bzero(o_buff, out_size);
          }
        }else
	  bzero(o_buff, out_size);
      }

      /* save scsi status and sense buffer */
      /*sjy_controller[idx].result = sg_hd->result; deprecated by use of target_status*/  
      sjy_controller[idx].result = ((sg_hd->target_status & STATUS_MASK)<<1);
      memcpy(sjy_controller[idx].sense_buffer, sg_hd->sense_buffer, SJY_SENSE_LENGTH);

      /* Unlike the Jorway 73A, the 411S returns SCSI status CONDITION_MET
      ** (0x04) for Q=1 on a data transfer. This is converted by the 
      ** generic SCSI driver sg.c to EIO (0x05). The CAMAC completion routines
      ** ignore this status, but the following code segment does not.
      */
#ifdef DEBUG 
      if ( status < 0 || status != SCSI_OFF + out_size || sg_hd->result ) {
          /* some error happened */
          fprintf( stderr, "read(generic) result = 0x%x cmd = 0x%x\n",
                  sg_hd->result, i_buff[SCSI_OFF] );
          fprintf( stderr, "read(generic) sense "
                  "%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n",
                  sg_hd->sense_buffer[0],         sg_hd->sense_buffer[1],
                  sg_hd->sense_buffer[2],         sg_hd->sense_buffer[3],
                  sg_hd->sense_buffer[4],         sg_hd->sense_buffer[5],
                  sg_hd->sense_buffer[6],         sg_hd->sense_buffer[7],
                  sg_hd->sense_buffer[8],         sg_hd->sense_buffer[9],
                  sg_hd->sense_buffer[10],        sg_hd->sense_buffer[11],
                  sg_hd->sense_buffer[12],        sg_hd->sense_buffer[13],
                  sg_hd->sense_buffer[14],        sg_hd->sense_buffer[15]);
          if (status < 0)
	    perror("sjy_interface");
          return (CAM_S_DEV_IO_ERROR);
      }
#endif
      return(status-SCSI_OFF-defecit);
  }

/*
** sjy_nondata - called by CAMAC IEEE routines. Sets up scsi command block non-data commands.
**
** returns
**  int status - return status from sjy_interface
**
*/

  int sjy_nondata( int branch, int camCmd)
  {
     static unsigned char *cmdblk;
     int cmdsize;
     int status;

     bzero(cmd, CMD_SIZE); 

#ifdef SJY411S

     /* cmdblk = lcmdblk; */
     cmdblk = cmd + SCSI_OFF;
     cmdsize = 6; /* op-code C1h is expected to have 10 byte command blocks */
     cmdblk [0] = SJY_NONDATA_CMD;        /* Op code */
     cmdblk [1] = (camCmd>>24) & 0x1F;    /* LUN/function */
     cmdblk [2] = (camCmd>>16) & 0x1F;    /* station */
     cmdblk [3] = (camCmd>> 8) & 0x0F;    /* address */
     cmdblk [4] = (camCmd)     & 0xFF;    /* S/P / crate */
     cmdblk [5] = 0;                      /* control byte, always 0 */
  
#endif

#ifdef SJY73A

     /* cmdblk = scmdblk; */
     cmdblk = cmd + SCSI_OFF;
     cmdsize = 6;
     cmdblk [0] = SJY_NONDATA_CMD;         /* Op code */
     cmdblk [1] = (camCmd>>24) & 0x1F;     /* lun/function code */
     cmdblk [2] = (camCmd>>16) & 0xFF;     /* mode/station */
     cmdblk [3] = (camCmd>>8)  & 0x0F;     /* address */
     cmdblk [4] = 0;                       /* allocation length */
     cmdblk [5] = 0;                       /* control byte, always 0 */

#endif

    /* copy cmdblk behind sg_header */
    /*
     * +------------------+
     * | struct sg_header | <- cmd
     * +------------------+
     * | copy of cmdblk   | <- cmd + SCSI_OFF
     * +------------------+
     */

     /*memcpy( cmd + SCSI_OFF, cmdblk, cmdsize );*/

    status = handle_scsi_cmd(branch, cmdsize, 0, cmd, 0, NULL);

    sjy_notify_camac_nondata(branch, 0, status);
    return (status);
  }



/*
** sjy_write - called by CAMAC IEEE routines. Sets up scsi command block for writes
**             and non-data commands.
**
** returns
**  int status - return status from sjy_interface
**
*/

  int sjy_write( int branch, int camCmd, void *dataBuffer, int xferlen )
  {
     static unsigned char *cmdblk;
     int cmdsize;
     int status;


     bzero(cmd, CMD_SIZE);

#ifdef SJY411S
     if (xferlen) {

       /* cmdblk = lcmdblk; */
       cmdblk = cmd + SCSI_OFF;
       cmdblk [0] = SJY_DATA_CMD;           /* Op code */
       cmdblk [1] = 0;                      /* LUN/reserved */
       cmdblk [2] = (camCmd>>24) & 0x3F;    /* list flag/BS/function code */
       cmdblk [3] = (camCmd>>16) & 0xFF;    /* mode/station */
       cmdblk [4] = (camCmd>>8)  & 0x3F;    /* disable disc./SCS/SNXC/address*/
       cmdblk [5] =  camCmd      & 0xFF;    /* S/P /crate */
       cmdblk [6] = (xferlen>>16) & 0xFF;   /* transfer length (MSB) */
       cmdblk [7] = (xferlen>>8)  & 0xFF;
       cmdblk [8] =  xferlen      & 0xFF;   /* transfer length (LSB) */
       cmdblk [9] = 0;                      /* control byte, always 0 */

     } else {

       /* cmdblk = scmdblk; */
       cmdblk = cmd + SCSI_OFF;
       cmdblk [0] = SJY_NONDATA_CMD;        /* Op code */
       cmdblk [1] = (camCmd>>24) & 0x1F;    /* LUN/function */
       cmdblk [2] = (camCmd>>24) & 0x1F;    /* station */
       cmdblk [3] = (camCmd>>16) & 0x0F;    /* address */
       cmdblk [4] = (camCmd>>8)  & 0xFF;    /* S/P / crate */
       cmdblk [5] = 0;                      /* control byte, always 0 */
  
     }
#endif

#ifdef SJY73A
     if (xferlen < 256) { /* This is also the nondata command block */

     /* cmdblk = scmdblk; */
     cmdblk = cmd + SCSI_OFF;
     cmdblk [0] = SJY_SHORTXFER_DATA_CMD;  /* Op code */
     cmdblk [1] = (camCmd>>24) & 0x1F;     /* lun/function code */
     cmdblk [2] = (camCmd>>16) & 0xFF;     /* mode/station */
     cmdblk [3] = (camCmd>>8)  & 0x0F;     /* address */
     cmdblk [4] = xferlen;                 /* allocation length */
     cmdblk [5] = 0;                       /* control byte, always 0 */


    } else {

     /* cmdblk = lcmdblk; */
     cmdblk = cmd + SCSI_OFF;
     cmdblk [0] = SJY_LONGXFER_DATA_CMD;   /* Op code */
     cmdblk [1] = 0;                       /* lun/reserved */
     cmdblk [2] = (camCmd>>24) & 0x1F;     /* reserved/function code */
     cmdblk [3] = (camCmd>>16) & 0xFF;     /* mode/S/station */
     cmdblk [4] = (camCmd>>8)  & 0x0F;     /* address */
     cmdblk [5] = 0;                       /* reserved */
     cmdblk [6] = (xferlen>>16) & 0xFF;    /* transfer length (MSB) */
     cmdblk [7] = (xferlen>>8)  & 0xFF;
     cmdblk [8] = xferlen       & 0xFF;    /* transfer length (LSB) */
     cmdblk [9] = 0;                       /* control byte, always 0 */ 
 
    }
#endif

    /* copy cmdblk behind sg_header */
    /*
     * +------------------+
     * | struct sg_header | <- cmd
     * +------------------+
     * | copy of cmdblk   | <- cmd + SCSI_OFF
     * +------------------+
     */

    if (cmdblk[0] == SJY_SHORTXFER_DATA_CMD)
      cmdsize = 6;
    else
      cmdsize = 10;

    /* memcpy( cmd + SCSI_OFF, cmdblk, cmdsize ); */

    /* copy user buffer behind cmdblk */
    if (dataBuffer != NULL)
      memcpy( cmd + SCSI_OFF + cmdsize, dataBuffer, xferlen );

    status = handle_scsi_cmd(branch, cmdsize, xferlen, cmd, 0, NULL);

    sjy_notify_camac_data(branch, xferlen, status);
    return(status);
  }


/*
** sjy_read - called by CAMAC IEEE routines. Sets up scsi command block for reads.
**
** returns
**  int status - return status from sjy_interface
**
*/

  int sjy_read( int branch, int camCmd, void *dataBuffer, int xferlen )
  {
     static unsigned char *cmdblk;
     int cmdsize;
     int status;


     bzero(cmd, CMD_SIZE);     /* clear cmd for case of short transfers */

#ifdef SJY411S
     if (xferlen) {

       /* cmdblk = lcmdblk; */
       cmdblk = cmd + SCSI_OFF;
       cmdblk [0] = SJY_DATA_CMD;           /* Op code */
       cmdblk [1] = 0;                      /* LUN/reserved */
       cmdblk [2] = (camCmd>>24) & 0x3F;    /* list flag/BS/function code */
       cmdblk [3] = (camCmd>>16) & 0xFF;    /* mode/station */
       cmdblk [4] = (camCmd>>8)  & 0x3F;    /* disable disc./SCS/SNXC/address*/
       cmdblk [5] =  camCmd      & 0xFF;    /* S/P /crate */
       cmdblk [6] = (xferlen>>16) & 0xFF;   /* transfer length (MSB) */
       cmdblk [7] = (xferlen>>8)  & 0xFF;
       cmdblk [8] =  xferlen      & 0xFF;   /* transfer length (LSB) */
       cmdblk [9] = 0;                      /* control byte, always 0 */

     } else {

       /* cmdblk = scmdblk; */
       cmdblk = cmd + SCSI_OFF;
       cmdblk [0] = SJY_NONDATA_CMD;        /* Op code */
       cmdblk [1] = (camCmd>>24) & 0x1F;    /* LUN/function */
       cmdblk [2] = (camCmd>>24) & 0x1F;    /* station */
       cmdblk [3] = (camCmd>>16) & 0x0F;    /* address */
       cmdblk [4] = (camCmd>>8)  & 0xFF;    /* S/P / crate */
       cmdblk [5] = 0;                      /* control byte, always 0 */
  
     }
#endif

#ifdef SJY73A
    if (xferlen < 256) {

     /* cmdblk = scmdblk; */
     cmdblk = cmd + SCSI_OFF;
     cmdblk [0] = SJY_SHORTXFER_DATA_CMD;   /* Op code */
     cmdblk [1] = (camCmd>>24) & 0x1F;      /* lun/function code */
     cmdblk [2] = (camCmd>>16) & 0xFF;      /* mode/station */
     cmdblk [3] = (camCmd>>8)  & 0x0F;      /* address */
     cmdblk [4] = xferlen;                  /* allocation length */
     cmdblk [5] = 0;                        /* control byte, always 0 */


    } else {

     /* cmdblk = lcmdblk; */
     cmdblk = cmd + SCSI_OFF;
     cmdblk [0] = SJY_LONGXFER_DATA_CMD;    /* Op code */
     cmdblk [1] = 0;                        /* lun/reserved */
     cmdblk [2] = (camCmd>>24) & 0x1F;      /* reserved/function code */
     cmdblk [3] = (camCmd>>16) & 0xFF;      /* mode/S/station */
     cmdblk [4] = (camCmd>>8)  & 0x0F;      /* address */
     cmdblk [5] = 0;                        /* reserved */
     cmdblk [6] = (xferlen>>16) & 0xFF;     /* transfer length (MSB) */
     cmdblk [7] = (xferlen>>8)  & 0xFF;
     cmdblk [8] = xferlen       & 0xFF;     /* transfer length (LSB) */
     cmdblk [9] = 0;                        /* control byte, always 0 */ 
 

    }
#endif


    /* copy cmdblk behind sg_header */
    /*
     * +------------------+
     * | struct sg_header | <- cmd
     * +------------------+
     * | copy of cmdblk   | <- cmd + SCSI_OFF
     * +------------------+
     */


    if (cmdblk[0] == SJY_SHORTXFER_DATA_CMD)
      cmdsize = 6;
    else
      cmdsize = 10;

    /* memcpy( cmd + SCSI_OFF, cmdblk, cmdsize ); */

    status = handle_scsi_cmd(branch, cmdsize, 0, cmd, xferlen, (unsigned char *)dataBuffer);

    sjy_notify_camac_data(branch, xferlen, status);
    return(status);
  }


/*
** sjy_inquiry - request information about the JORWAY
**
** returns
**  success - pointer to a static char buffer containing inquiry data
**  fail - pointer to an empty static char buffer
**
*/

  /* request vendor brand and model */
  unsigned char *sjy_inquiry ( int branch )
  {
    static unsigned char Inqbuffer[ SCSI_OFF + INQUIRY_REPLY_LEN ];


    unsigned char cmdblk [ INQUIRY_CMDLEN ] =
        { INQUIRY_CMD,  /* command */
                    0,  /* lun/reserved */
                    0,  /* page code */
                    0,  /* reserved */
    INQUIRY_REPLY_LEN,  /* allocation length */
                    0 };/* reserved/flag/link */

    memcpy( cmd + SCSI_OFF, cmdblk, sizeof(cmdblk) );

    /*
     * +------------------+
     * | struct sg_header | <- cmd
     * +------------------+
     * | copy of cmdblk   | <- cmd + SCSI_OFF
     * +------------------+
     */

    /* clear the Inquiry buffer */
    /*memset( Inqbuffer, '\0', sizeof(Inqbuffer) );*/

    handle_scsi_cmd(branch, sizeof(cmdblk), 0, cmd,
                        sizeof(Inqbuffer) - SCSI_OFF, Inqbuffer);

    /* handle_scsi_cmd removes sg_header in the copy from i_buff to o_buff,
    ** so don't add SCSI_OFF offset to Inqbuffer
    */
    /*
    printf("Inquiry buffer\n");
    printf("%s\n",Inqbuffer+INQUIRY_VENDOR);
    */  
    return (Inqbuffer);
  }


/*
** sjy_tur - sends Test Unit Ready command
**
** returns
**  int status - return status from sjy_interface 
*/

  int sjy_tur ( int branch )
  {
    int i;
    int status;


    /* request READY status */
    static unsigned char cmdblk [TESTUNITREADY_CMDLEN] = {
        TESTUNITREADY_CMD, /* command */
                        0, /* lun/reserved */
                        0, /* reserved */
                        0, /* reserved */
                        0, /* reserved */
                        0};/* reserved */

    memcpy( cmd + SCSI_OFF, cmdblk, sizeof(cmdblk) );

    /*
     * +------------------+
     * | struct sg_header | <- cmd
     * +------------------+
     * | copy of cmdblk   | <- cmd + SCSI_OFF
     * +------------------+
     */

    /* This must be done twice to get good status from power cycled Jorway */
    for (i=0; i<2; i++) {
      status = handle_scsi_cmd(branch, sizeof(cmdblk), 0, cmd, 0, NULL);
    }
    if (status) perror("sjy_tur");

    return (status);
  }



/*
** sjy_reqSense - REQUEST_SENSE from the JORWAY. Normally issued after a CHECK
**                CONDITION status is returned by any command.
**
** returns
**  success - pointer to a static char buffer containing sense data
**  fail - pointer to an empty static char buffer
**
*/

  unsigned char *sjy_reqSense ( int branch )
  {
    int idx = IDX(branch);
    static unsigned char sensebuffer[ SCSI_OFF + SJY_SENSE_LENGTH ];

    unsigned char cmdblk [ SENSE_CMDLEN ] =
          { SENSE_CMD,  /* command */
                    0,  /* lun/reserved */
                    0,  /* reserved */
                    0,  /* reserved */
     SJY_SENSE_LENGTH,  /* allocation length */
                    0 };/* reserved/flag/link */

    memcpy( cmd + SCSI_OFF, cmdblk, sizeof(cmdblk) );

    /*
     * +------------------+
     * | struct sg_header | <- cmd
     * +------------------+
     * | copy of cmdblk   | <- cmd + SCSI_OFF
     * +------------------+
     */

    /* clear the sense buffer */
    memset( sensebuffer, '\0', sizeof(sensebuffer) );

    handle_scsi_cmd(sjy_controller[idx].fd, sizeof(cmdblk), 0, cmd,
                        sizeof(sensebuffer) - SCSI_OFF, sensebuffer);

    /* handle_scsi_cmd removes sg_header in the copy from i_buff to o_buff,
    ** so don't add SCSI_OFF offset to sensebuffer
    */
    return (sensebuffer);
  }


/*
** sjy_notify_camac_data - SCSI command completion notification routine
**                         for CAMAC data transfers
**
** returns
**  int result - SCSI command completion result
*/

 int sjy_notify_camac_data( int branch,      /* CAMAC branch */
			    int xferlen,     /* expected transfer length */
                            int status)      /* actual transfer length */ 
 {
   int idx = IDX(branch);
   int sense_key = sjy_controller[idx].sense_buffer[SENSEKEY];
   int add_sense = sjy_controller[idx].sense_buffer[ADD_SENSECODE];

 
   sjy_controller[idx].errn = ENODEV;

   if (sense_key) {
     if (add_sense == SJY_NO_Q) {
       /* SCSI ok, CAMAC ok, no Q on transfer */
       sjy_controller[idx].errn = ENODEV;
     } else if (add_sense == SJY_NO_X) {
       /* SCSI ok, no X on transfer */
       sjy_controller[idx].errn = EFAULT;
     } else {
       /* SCSI problem */
       sjy_controller[idx].errn = EIO;
     }
   } else {
     if (sjy_controller[idx].result == SJY_CONDITION_MET) {
       /*if (status == xferlen) {*/
       /* SCSI ok, CAMAC ok, assuming Q on transfer */
       sjy_controller[idx].errn = 0;
     }
   }

   return sjy_controller[idx].result;
 }


/*
** sjy_notify_camac_nondata - SCSI command completion notification routine
**                            for CAMAC nondata transfers
**
** returns
**  int result - SCSI command completion result
*/

 int sjy_notify_camac_nondata( int branch,      /* CAMAC branch */
			       int xferlen,     /* expected transfer length */
                               int status)      /* actual transfer length */ 
 {
   int idx = IDX(branch);
   int sense_key = sjy_controller[idx].sense_buffer[SENSEKEY];
   int add_sense = sjy_controller[idx].sense_buffer[ADD_SENSECODE];

   sjy_controller[idx].errn = 0; 

   if ((sjy_controller[idx].result == SJY_GOOD) && !(sense_key)) {
     /* SCSI ok. CAMAC ok, no Q on transfer */
     sjy_controller[idx].errn = ENODEV;

   } else if ((sjy_controller[idx].result == SJY_CHECK_CONDITION) || (sense_key)) {
     if (add_sense == SJY_NO_Q) {
       /* SCSI ok, CAMAC ok, no Q on transfer */
       sjy_controller[idx].errn = ENODEV;
     } else if (add_sense == SJY_NO_X) {
       /* SCSI ok, no X on transfer */
       sjy_controller[idx].errn = EFAULT;
     } else {
       /* SCSI problem */
       sjy_controller[idx].errn = EIO;
     }
   } else if (sjy_controller[idx].result == SJY_CONDITION_MET) {
     /* SCSI ok, CAMAC ok, Q on transfer */
     sjy_controller[idx].errn = 0;
   }

#if 0
   if ((sjy_controller[idx].result == SJY_GOOD) && !(sense_key)) {
     /* SCSI ok. CAMAC ok, no Q on transfer */
     sjy_controller[idx].errn = ENODEV;

   } else if ((sjy_controller[idx].result == SJY_CHECK_CONDITION) || (sense_key)) {
     if (add_sense == SJY_NO_Q) {
       /* SCSI ok, CAMAC ok, no Q on transfer */
       sjy_controller[idx].errn = ENODEV;
     } else if (add_sense == SJY_NO_X) {
       /* SCSI ok, no X on transfer */
       sjy_controller[idx].errn = EFAULT;
     } else {
       /* SCSI problem */
       sjy_controller[idx].errn = EIO;
     }
   } else {
     /* SCSI ok, CAMAC ok, assuming Q on transfer */
     sjy_controller[idx].errn = 0;
   }
#endif
   return sjy_controller[idx].result;
 }



