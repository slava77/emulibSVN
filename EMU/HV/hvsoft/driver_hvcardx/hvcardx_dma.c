/* hvcard.c: PNPI PCI HV Host Card device driver.
 *
 * Copyright (C) 2004 Victor Barashko (barvic@ufl.edu)
 */
#include <linux/time.h>
#include "hvcardx_drv.h"


int hvcardx_open_dma_channel(int card)
{

  int rc = 0;

  unsigned long flags;

  struct hvcardx_endpoint *endpoint = NULL;
  struct hvcardx_channel *channel;


  if ((card > hvcard_pcie_found) || hostcard[card]->ep == NULL)
    {
      pr_err(PFX "failed to find device for card%d\n", card);
      return -ENODEV;
    }

  endpoint = hostcard[card]->ep;

  // dev_info(endpoint->dev, "card%d open_dma_channel() called\n", card);

  if (endpoint->fatal_error)
    return -EIO;

  if ( endpoint->num_channels < 1 )
    {
      pr_err(PFX "failed to allocate read channel for card%d\n", card);
      return -ENODEV;
    }

  channel = endpoint->channels[1]; // Channel 1 should be a read channel for ADC data

  if (!channel->num_wr_buffers)
    return -ENODEV;

  if (!channel->num_rd_buffers)
    return -ENODEV;

  rc = mutex_lock_interruptible(&channel->wr_mutex);
  if (rc)
    return rc;

  rc = mutex_lock_interruptible(&channel->rd_mutex);
  if (rc)
    goto unlock_wr;


  if ((channel->wr_ref_count != 0) &&
      (channel->wr_exclusive_open))
    {
      pr_err(PFX "busy read channel for card%d\n", card);
      rc = -EBUSY;
      goto unlock;
    }

  if ((channel->rd_ref_count != 0) &&
      (channel->rd_exclusive_open))
    {
      rc = -EBUSY;
      goto unlock;
    }

  {
    if (channel->wr_ref_count == 0)   /* First open of file */
      {
        /* Move the host to first buffer */
        spin_lock_irqsave(&channel->wr_spinlock, flags);
        channel->wr_host_buf_idx = 0;
        channel->wr_host_buf_pos = 0;
        channel->wr_fpga_buf_idx = -1;
        channel->wr_empty = 1;
        channel->wr_ready = 0;
        channel->wr_sleepy = 1;
        channel->wr_eof = -1;
        channel->wr_hangup = 0;

        spin_unlock_irqrestore(&channel->wr_spinlock, flags);

        iowrite32(1 | (channel->chan_num << 1) |
                  (4 << 24) |  /* Opcode 4, open channel */
                  ((channel->wr_synchronous & 1) << 23),
                  &channel->endpoint->registers[
                    fpga_buf_ctrl_reg]);
        mmiowb(); /* Just to appear safe */
      }

    channel->wr_ref_count++;
  }

  // if (false)
  {
    if (channel->rd_ref_count == 0)   /* First open of file */
      {
        /* Move the host to first buffer */
        spin_lock_irqsave(&channel->rd_spinlock, flags);
        channel->rd_host_buf_idx = 0;
        channel->rd_host_buf_pos = 0;
        channel->rd_leftovers[3] = 0; /* No leftovers. */
        channel->rd_fpga_buf_idx = channel->num_rd_buffers - 1;
        channel->rd_full = 0;

        spin_unlock_irqrestore(&channel->rd_spinlock, flags);

        iowrite32((channel->chan_num << 1) |
                  (4 << 24),   /* Opcode 4, open channel */
                  &channel->endpoint->registers[
                    fpga_buf_ctrl_reg]);
        mmiowb(); /* Just to appear safe */
      }

    channel->rd_ref_count++;
  }


unlock:
  mutex_unlock(&channel->rd_mutex);
unlock_wr:
  mutex_unlock(&channel->wr_mutex);
  // dev_info(endpoint->dev, "card%d open_dma_channel() OK\n", card);
  return rc;
}

int hvcardx_close_dma_channel(int card)
{
  int rc = 0;

  unsigned long flags;

  struct hvcardx_endpoint *endpoint = NULL;
  struct hvcardx_channel *channel;

  int buf_idx;
  int eof;

  if ((card > hvcard_pcie_found) || hostcard[card]->ep == NULL)
    {
      pr_err(PFX "failed to find device for card%d\n", card);
      return -ENODEV;
    }

  endpoint = hostcard[card]->ep;

  if ( endpoint->num_channels < 1 )
    {
      pr_err(PFX "failed to allocate read channel for card%d\n", card);
      return -ENODEV;
    }

  // dev_info(endpoint->dev, "card%d close_dma_channel() called\n", card);

  channel = endpoint->channels[1]; // Channel 1 should be a read channel for ADC data

  if (channel->endpoint->fatal_error)
    return -EIO;


  {
    // -- Close Write channel
    rc = mutex_lock_interruptible(&channel->rd_mutex);

    if (rc)
      {
        dev_warn(channel->endpoint->dev,
                 "Failed to close file. Hardware left in messy state.\n");
        return rc;
      }

    channel->rd_ref_count--;

    if (channel->rd_ref_count == 0)
      {

        /*
         * We rely on the kernel calling flush()
         * before we get here.
         */

        iowrite32((channel->chan_num << 1) | /* Channel ID */
                  (5 << 24),  /* Opcode 5, close channel */
                  &channel->endpoint->registers[
                    fpga_buf_ctrl_reg]);
        mmiowb(); /* Just to appear safe */
      }
    mutex_unlock(&channel->rd_mutex);
  }


  {
    // -- Close Read channel
    rc = mutex_lock_interruptible(&channel->wr_mutex);
    if (rc)
      {
        dev_warn(channel->endpoint->dev,
                 "Failed to close file. Hardware left in messy state.\n");
        return rc;
      }

    channel->wr_ref_count--;

    if (channel->wr_ref_count == 0)
      {

        iowrite32(1 | (channel->chan_num << 1) |
                  (5 << 24),  /* Opcode 5, close channel */
                  &channel->endpoint->registers[
                    fpga_buf_ctrl_reg]);
        mmiowb(); /* Just to appear safe */
        while (1)
          {
            spin_lock_irqsave(&channel->wr_spinlock,
                              flags);
            buf_idx = channel->wr_fpga_buf_idx;
            eof = channel->wr_eof;
            channel->wr_sleepy = 1;
            spin_unlock_irqrestore(&channel->wr_spinlock,
                                   flags);

            /*
             * Check if eof points at the buffer after
             * the last one the FPGA submitted. Note that
             * no EOF is marked by negative eof.
             */

            buf_idx++;
            if (buf_idx == channel->num_wr_buffers)
              buf_idx = 0;

            if (buf_idx == eof)
              break;

            /*
             * Steal extra 100 ms if awaken by interrupt.
             * This is a simple workaround for an
             * interrupt pending when entering, which would
             * otherwise result in declaring the hardware
             * non-responsive.
             */

            if (wait_event_interruptible(
                  channel->wr_wait,
                  (!channel->wr_sleepy)))
              msleep(100);

            if (channel->wr_sleepy)
              {
                mutex_unlock(&channel->wr_mutex);
                dev_warn(channel->endpoint->dev,
                         "Hardware failed to respond to close command, therefore left in messy state.\n");
                return -EINTR;
              }
          }
      }
    mutex_unlock(&channel->wr_mutex);
  }

  // dev_info(endpoint->dev, "card%d close_dma_channel() OK\n", card);
  return rc;
}

int hvcardx_read_adc_data(int card)
{
  int rc = 0;

  unsigned long flags;

  int bytes_done = 0;
  int no_time_left = 0;
  long deadline, left_to_sleep;

  int empty, reached_eof, exhausted, ready;
  /* Initializations are there only to silence warnings */

  int howmany = 0, bufpos = 0, bufidx = 0, bufferdone = 0;
  int waiting_bufidx;
  size_t count = MAX_ADC_DATA_SIZE;
  int adc_offset = 0;

  struct hvcardx_endpoint *endpoint = NULL;
  struct hvcardx_channel *channel;

  // uint32_t adc_buf[MAX_ADC_DATA_SIZE/4];

  loff_t pos = ADC_DATA_OFFSET;;

  if ((card > hvcard_pcie_found) || hostcard[card]->ep == NULL)
    {
      pr_err(PFX "failed to find device for card%d\n", card);
      return -ENODEV;
    }

  endpoint = hostcard[card]->ep;

  if ( endpoint->num_channels < 1 )
    {
      pr_err(PFX "failed to allocate read channel for card%d\n", card);
      return -ENODEV;
    }

  // dev_info(endpoint->dev, "card%d read_adc_data() called\n", card);

  channel = endpoint->channels[1]; // Channel 1 should be a read channel for ADC data

  if (channel->endpoint->fatal_error)
    return -EIO;

  mutex_lock(&dma_op_mutex);

  // -- Set buffer offset position

  mutex_lock(&channel->wr_mutex);
  mutex_lock(&channel->rd_mutex);

  pos = ADC_DATA_OFFSET;

  /* In any case, we must finish on an element boundary */
  if (pos & ((1 << channel->log2_element_size) - 1))
    {
      rc = -EINVAL;
      goto end_seek;
    }

  mutex_lock(&channel->endpoint->register_mutex);

  iowrite32(pos >> channel->log2_element_size,
            &channel->endpoint->registers[fpga_buf_offset_reg]);
  mmiowb();
  iowrite32((channel->chan_num << 1) |
            (6 << 24),  /* Opcode 6, set address */
            &channel->endpoint->registers[fpga_buf_ctrl_reg]);
  mmiowb(); /* Just to appear safe */

  mutex_unlock(&channel->endpoint->register_mutex);

end_seek:
  mutex_unlock(&channel->rd_mutex);
  mutex_unlock(&channel->wr_mutex);

  if (rc) /* Return error after releasing mutexes */
    {
      mutex_unlock(&dma_op_mutex);
      return rc;
    }



  /*
   * Since seekable devices are allowed only when the channel is
   * synchronous, we assume that there is no data pending in either
   * direction (which holds true as long as no concurrent access on the
   * file descriptor takes place).
   * The only thing we may need to throw away is leftovers from partial
   * write() flush.
   */

  channel->rd_leftovers[3] = 0;


  // -- Read buffer

  if (channel->endpoint->fatal_error)
    {
      mutex_unlock(&dma_op_mutex);
      return -EIO;
    }

  deadline = jiffies + 1 + HVCARDX_RX_TIMEOUT;


  rc = mutex_lock_interruptible(&channel->wr_mutex);

  if (rc)
    {
      mutex_unlock(&dma_op_mutex);
      return rc;
    }


  rc = 0; /* Just to be clear about it. Compiler optimizes this out */

  // memset(adc_buf, 0, sizeof(adc_buf));
  //

  while (1)   /* Note that we may drop mutex within this loop */
    {
      int bytes_to_do = count - bytes_done;
      spin_lock_irqsave(&channel->wr_spinlock, flags);

      empty = channel->wr_empty;
      ready = !empty || channel->wr_ready;


      if (!empty)
        {
          bufidx = channel->wr_host_buf_idx;
          bufpos = channel->wr_host_buf_pos;
          howmany = ((channel->wr_buffers[bufidx]->end_offset
                      + 1) << channel->log2_element_size)
                    - bufpos;


          /* Update wr_host_* to its post-operation state */
          if (howmany > bytes_to_do)
            {
              bufferdone = 0;

              howmany = bytes_to_do;
              channel->wr_host_buf_pos += howmany;
            }
          else
            {
              bufferdone = 1;

              channel->wr_host_buf_pos = 0;

              if (bufidx == channel->wr_fpga_buf_idx)
                {
                  channel->wr_empty = 1;
                  channel->wr_sleepy = 1;
                  channel->wr_ready = 0;
                }

              if (bufidx >= (channel->num_wr_buffers - 1))
                channel->wr_host_buf_idx = 0;
              else
                channel->wr_host_buf_idx++;
            }
        }

      /*
       * Marking our situation after the possible changes above,
       * for use after releasing the spinlock.
       *
       * empty = empty before change
       * exhasted = empty after possible change
       */

      reached_eof = channel->wr_empty &&
                    (channel->wr_host_buf_idx == channel->wr_eof);
      channel->wr_hangup = reached_eof;
      exhausted = channel->wr_empty;
      waiting_bufidx = channel->wr_host_buf_idx;

      spin_unlock_irqrestore(&channel->wr_spinlock, flags);
      if (!empty)   /* Go on, now without the spinlock */
        {

          if (bufpos == 0) /* Position zero means it's virgin */
            channel->endpoint->ephw->hw_sync_sgl_for_cpu(
              channel->endpoint,
              channel->wr_buffers[bufidx]->dma_addr,
              channel->wr_buf_size,
              DMA_FROM_DEVICE);


          spin_lock_irqsave(&hvdata_lock, flags);
          /*
                memcpy(adc_buf+adc_offset, channel->wr_buffers[bufidx]->addr
                       + bufpos, howmany);
          */
          memcpy(hostcard[card]->adcdata+adc_offset, channel->wr_buffers[bufidx]->addr
                 + bufpos, howmany);
          adc_offset += howmany;
          spin_unlock_irqrestore(&hvdata_lock, flags);
          // printk(KERN_INFO PFX "read %d bytes of ADC data\n", howmany);

          bytes_done += howmany;

          if (bufferdone)
            {
              /*
              spin_lock_irqsave(&hvdata_lock, flags);
                    memcpy(hostcard[card]->adcdata, adc_buf, bytes_done);
              spin_unlock_irqrestore(&hvdata_lock, flags);
              */

              channel->endpoint->ephw->
              hw_sync_sgl_for_device
              (
                channel->endpoint,
                channel->wr_buffers[bufidx]->
                dma_addr,
                channel->wr_buf_size,
                DMA_FROM_DEVICE);

              /*
               * Tell FPGA the buffer is done with. It's an
               * atomic operation to the FPGA, so what
               * happens with other channels doesn't matter,
               * and the certain channel is protected with
               * the channel-specific mutex.
               */

              iowrite32(1 | (channel->chan_num << 1)
                        | (bufidx << 12),
                        &channel->endpoint->registers[
                          fpga_buf_ctrl_reg]);
              mmiowb(); /* Just to appear safe */
            }

          if (rc)
            {
              mutex_unlock(&channel->wr_mutex);
              mutex_unlock(&dma_op_mutex);
              return rc;
            }
        }

      /* This includes a zero-count return = EOF */
      if ((bytes_done >= count) || reached_eof)
        {
          break;
        }

      if (!exhausted)
        {
          continue; /* More in RAM buffer(s)? Just go on. */
        }

      if ((bytes_done > 0) &&
          (no_time_left ||
           (channel->wr_synchronous && channel->wr_allow_partial)))
        {
          break;
        }

      /*
       * Nonblocking read: The "ready" flag tells us that the FPGA
       * has data to send. In non-blocking mode, if it isn't on,
       * just return. But if there is, we jump directly to the point
       * where we ask for the FPGA to send all it has, and wait
       * until that data arrives. So in a sense, we *do* block in
       * nonblocking mode, but only for a very short time.
       */
      /*
            if (!no_time_left)
              {
                if (bytes_done > 0)
                  break;

                if (ready)
                  goto desperate;

                bytes_done = -EAGAIN;
                break;
              }
      */
      if (!no_time_left || (bytes_done > 0))
        {
          /*
           * Note that in case of an element-misaligned read
           * request, offsetlimit will include the last element,
           * which will be partially read from.
           */
          int offsetlimit = ((count - bytes_done) - 1) >>
                            channel->log2_element_size;
          int buf_elements = channel->wr_buf_size >>
                             channel->log2_element_size;

          /*
           * In synchronous mode, always send an offset limit.
           * Just don't send a value too big.
           */

          if (channel->wr_synchronous)
            {
              /* Don't request more than one buffer */
              if (channel->wr_allow_partial &&
                  (offsetlimit >= buf_elements))
                offsetlimit = buf_elements - 1;

              /* Don't request more than all buffers */
              if (!channel->wr_allow_partial &&
                  (offsetlimit >=
                   (buf_elements * channel->num_wr_buffers)))
                offsetlimit = buf_elements *
                              channel->num_wr_buffers - 1;
            }

          /*
           * In asynchronous mode, force early flush of a buffer
           * only if that will allow returning a full count. The
           * "offsetlimit < ( ... )" rather than "<=" excludes
           * requesting a full buffer, which would obviously
           * cause a buffer transmission anyhow
           */

          if (channel->wr_synchronous ||
              (offsetlimit < (buf_elements - 1)))
            {

              mutex_lock(&channel->endpoint->register_mutex);

              iowrite32(offsetlimit,
                        &channel->endpoint->registers[
                          fpga_buf_offset_reg]);
              mmiowb();

              iowrite32(1 | (channel->chan_num << 1) |
                        (2 << 24) |  /* 2 = offset limit */
                        (waiting_bufidx << 12),
                        &channel->endpoint->registers[
                          fpga_buf_ctrl_reg]);

              mmiowb(); /* Just to appear safe */

              mutex_unlock(&channel->endpoint->
                           register_mutex);
            }

        }

      /*
       * If partial completion is disallowed, there is no point in
       * timeout sleeping. Neither if no_time_left is set and
       * there's no data.
       */

      if (!channel->wr_allow_partial ||
          (no_time_left && (bytes_done == 0)))
        {

          /*
           * This do-loop will run more than once if another
           * thread reasserted wr_sleepy before we got the mutex
           * back, so we try again.
           */

          do
            {
              mutex_unlock(&channel->wr_mutex);

              if (wait_event_interruptible(
                    channel->wr_wait,
                    (!channel->wr_sleepy)))
                goto interrupted;

              if (mutex_lock_interruptible(
                    &channel->wr_mutex))
                goto interrupted;
            }
          while (channel->wr_sleepy);

          continue;
interrupted: /* Mutex is not held if got here */
          if (channel->endpoint->fatal_error)
            {
              mutex_unlock(&dma_op_mutex);
              return -EIO;
            }
          if (bytes_done)
            {
              mutex_unlock(&dma_op_mutex);
              return bytes_done;
            }
          // if (filp->f_flags & O_NONBLOCK)
          //  return -EAGAIN; /* Don't admit snoozing */
          mutex_unlock(&dma_op_mutex);
          return -EINTR;
        }

      left_to_sleep = deadline - ((long) jiffies);

      /*
       * If our time is out, skip the waiting. We may miss wr_sleepy
       * being deasserted but hey, almost missing the train is like
       * missing it.
       */

      if (left_to_sleep > 0)
        {
          left_to_sleep =
            wait_event_interruptible_timeout(
              channel->wr_wait,
              (!channel->wr_sleepy),
              left_to_sleep);

          if (!channel->wr_sleepy)
            continue;

          if (left_to_sleep < 0)   /* Interrupt */
            {
              mutex_unlock(&channel->wr_mutex);
              mutex_unlock(&dma_op_mutex);
              if (channel->endpoint->fatal_error)
                return -EIO;
              if (bytes_done)
                return bytes_done;
              return -EINTR;
            }
        }

// desperate:
      no_time_left = 1; /* We're out of sleeping time. Desperate! */

      if (bytes_done == 0)
        {
          /*
           * Reaching here means that we allow partial return,
           * that we've run out of time, and that we have
           * nothing to return.
           * So tell the FPGA to send anything it has or gets.
           */

          iowrite32(1 | (channel->chan_num << 1) |
                    (3 << 24) |  /* Opcode 3, flush it all! */
                    (waiting_bufidx << 12),
                    &channel->endpoint->registers[
                      fpga_buf_ctrl_reg]);
          mmiowb(); /* Just to appear safe */
        }

      /*
       * Formally speaking, we should block for data at this point.
       * But to keep the code cleaner, we'll just finish the loop,
       * make the unlikely check for data, and then block at the
       * usual place.
       */
    }

  mutex_unlock(&channel->wr_mutex);
  mutex_unlock(&dma_op_mutex);

  if (channel->endpoint->fatal_error)
    return -EIO;

  return bytes_done;

//  return rc;
}

/***
 * Write DAC data
 ***/

int hvcardx_write_dac_data(int card)
{
  int rc = 0;

  unsigned long flags;

  int bytes_done = 0;

  int full, exhausted;
  /* Initializations are there only to silence warnings */

  int howmany = 0, bufpos = 0, bufidx = 0, bufferdone = 0;
  int end_offset_plus1 = 0;

  size_t count = MAX_DAC_DATA_SIZE;
  int dac_offset = 0;

  struct hvcardx_endpoint *endpoint = NULL;
  struct hvcardx_channel *channel;

  // uint32_t adc_buf[MAX_ADC_DATA_SIZE/4];

  loff_t pos = ADC_DATA_OFFSET;;

  if ((card > hvcard_pcie_found) || hostcard[card]->ep == NULL)
    {
      pr_err(PFX "failed to find device for card%d\n", card);
      return -ENODEV;
    }

  endpoint = hostcard[card]->ep;

  if ( endpoint->num_channels < 1 )
    {
      pr_err(PFX "failed to allocate read channel for card%d\n", card);
      return -ENODEV;
    }

  // dev_info(endpoint->dev, "card%d read_adc_data() called\n", card);

  channel = endpoint->channels[1]; // Channel 1 should be a read channel for ADC data

  if (channel->endpoint->fatal_error)
    return -EIO;

  mutex_lock(&dma_op_mutex);
  // -- Set buffer offset position

  mutex_lock(&channel->wr_mutex);
  mutex_lock(&channel->rd_mutex);

  pos = DAC_DATA_OFFSET;

  /* In any case, we must finish on an element boundary */
  if (pos & ((1 << channel->log2_element_size) - 1))
    {
      rc = -EINVAL;
      goto end_seek;
    }

  mutex_lock(&channel->endpoint->register_mutex);

  iowrite32(pos >> channel->log2_element_size,
            &channel->endpoint->registers[fpga_buf_offset_reg]);
  mmiowb();
  iowrite32((channel->chan_num << 1) |
            (6 << 24),  /* Opcode 6, set address */
            &channel->endpoint->registers[fpga_buf_ctrl_reg]);
  mmiowb(); /* Just to appear safe */

  mutex_unlock(&channel->endpoint->register_mutex);

end_seek:
  mutex_unlock(&channel->rd_mutex);
  mutex_unlock(&channel->wr_mutex);

  if (rc) /* Return error after releasing mutexes */
    {
      mutex_unlock(&dma_op_mutex);
      return rc;
    }


  /*
   * Since seekable devices are allowed only when the channel is
   * synchronous, we assume that there is no data pending in either
   * direction (which holds true as long as no concurrent access on the
   * file descriptor takes place).
   * The only thing we may need to throw away is leftovers from partial
   * write() flush.
   */

  channel->rd_leftovers[3] = 0;

  rc = mutex_lock_interruptible(&channel->rd_mutex);

  if (rc)
    {
      mutex_unlock(&dma_op_mutex);
      return rc;
    }

  rc = 0; /* Just to be clear about it. Compiler optimizes this out */

  while (1)
    {
      int bytes_to_do = count - bytes_done;

      spin_lock_irqsave(&channel->rd_spinlock, flags);

      full = channel->rd_full;

      if (!full)
        {
          bufidx = channel->rd_host_buf_idx;
          bufpos = channel->rd_host_buf_pos;
          howmany = channel->rd_buf_size - bufpos;

          /*
          *            * Update rd_host_* to its state after this operation.
          *                       * count=0 means committing the buffer immediately,
          *                                  * which is like flushing, but not necessarily block.
          *                                             */

          if ((howmany > bytes_to_do) &&
              (count ||
               ((bufpos >> channel->log2_element_size) == 0)))
            {
              bufferdone = 0;

              howmany = bytes_to_do;
              channel->rd_host_buf_pos += howmany;
            }
          else
            {
              bufferdone = 1;

              if (count)
                {
                  end_offset_plus1 =
                    channel->rd_buf_size >>
                    channel->log2_element_size;
                  channel->rd_host_buf_pos = 0;
                }
              else
                {
                  unsigned char *tail;
                  int i;

                  end_offset_plus1 = bufpos >>
                                     channel->log2_element_size;

                  channel->rd_host_buf_pos -=
                    end_offset_plus1 <<
                    channel->log2_element_size;

                  tail = channel->
                         rd_buffers[bufidx]->addr +
                         (end_offset_plus1 <<
                          channel->log2_element_size);

                  for (i = 0;
                       i < channel->rd_host_buf_pos;
                       i++)
                    channel->rd_leftovers[i] =
                      *tail++;
                }

              if (bufidx == channel->rd_fpga_buf_idx)
                channel->rd_full = 1;

              if (bufidx >= (channel->num_rd_buffers - 1))
                channel->rd_host_buf_idx = 0;
              else
                channel->rd_host_buf_idx++;
            }
        }

      /*
      *        * Marking our situation after the possible changes above,
      *               * for use  after releasing the spinlock.
      *                      *
      *                             * full = full before change
      *                                    * exhasted = full after possible change
      *                                           */

      exhausted = channel->rd_full;

      spin_unlock_irqrestore(&channel->rd_spinlock, flags);

      if (!full)   /* Go on, now without the spinlock */
        {
          unsigned char *head =
            channel->rd_buffers[bufidx]->addr;
          int i;

          if ((bufpos == 0) || /* Zero means it's virgin */
              (channel->rd_leftovers[3] != 0))
            {
              channel->endpoint->ephw->hw_sync_sgl_for_cpu(
                channel->endpoint,
                channel->rd_buffers[bufidx]->dma_addr,
                channel->rd_buf_size,
                DMA_TO_DEVICE);

              /* Virgin, but leftovers are due */
              for (i = 0; i < bufpos; i++)
                *head++ = channel->rd_leftovers[i];

              channel->rd_leftovers[3] = 0; /* Clear flag */
            }
          /*
                    if (copy_from_user(
                          channel->rd_buffers[bufidx]->addr + bufpos,
                          userbuf, howmany))
                      rc = -EFAULT;
          */

          spin_lock_irqsave(&hvdata_lock, flags);
          memcpy(channel->rd_buffers[bufidx]->addr + bufpos,
                 hostcard[card]->dacdata+dac_offset, howmany);
          dac_offset += howmany;
          spin_unlock_irqrestore(&hvdata_lock, flags);

          // userbuf += howmany;
          bytes_done += howmany;

          if (bufferdone)
            {
              channel->endpoint->ephw->
              hw_sync_sgl_for_device(
                channel->endpoint,
                channel->rd_buffers[bufidx]->
                dma_addr,
                channel->rd_buf_size,
                DMA_TO_DEVICE);

              mutex_lock(&channel->endpoint->register_mutex);

              iowrite32(end_offset_plus1 - 1,
                        &channel->endpoint->registers[
                          fpga_buf_offset_reg]);
              mmiowb();
              iowrite32((channel->chan_num << 1) |
                        (2 << 24) |  /* 2 = submit buffer */
                        (bufidx << 12),
                        &channel->endpoint->registers[
                          fpga_buf_ctrl_reg]);
              mmiowb(); /* Just to appear safe */

              mutex_unlock(&channel->endpoint->
                           register_mutex);

              channel->rd_leftovers[3] =
                (channel->rd_host_buf_pos != 0);
            }

          if (rc)
            {
              mutex_unlock(&channel->rd_mutex);

              if (channel->endpoint->fatal_error)
                {
                  mutex_unlock(&dma_op_mutex);
                  return -EIO;
                }

              if (!channel->rd_synchronous)
                queue_delayed_work(
                  hvcardx_wq,
                  &channel->rd_workitem,
                  HVCARDX_RX_TIMEOUT);
              mutex_unlock(&dma_op_mutex);
              return rc;
            }
        }

      if (bytes_done >= count)
        break;

      if (!exhausted)
        continue; /* If there's more space, just go on */

      if ((bytes_done > 0) && channel->rd_allow_partial)
        break;

      /*
      *        * Indefinite sleep with mutex taken. With data waiting for
      *               * flushing, user should not be surprised if open() for write
      *                      * sleeps.
      *                             */
      /*
            if (filp->f_flags & O_NONBLOCK)
              {
                bytes_done = -EAGAIN;
                break;
              }
      */
      wait_event_interruptible(channel->rd_wait,
                               (!channel->rd_full));

      if (channel->rd_full)
        {
          mutex_unlock(&channel->rd_mutex);

          if (channel->endpoint->fatal_error)
            {
              mutex_unlock(&dma_op_mutex);
              return -EIO;
            }

          if (bytes_done)
            {
              mutex_unlock(&dma_op_mutex);
              return bytes_done;
            }

          mutex_unlock(&dma_op_mutex);
          return -EINTR;
        }
    }

  mutex_unlock(&channel->rd_mutex);

  if (!channel->rd_synchronous)
    queue_delayed_work(hvcardx_wq,
                       &channel->rd_workitem,
                       HVCARDX_RX_TIMEOUT);

  if ((channel->rd_synchronous) && (bytes_done > 0))
    {
      // rc = hvcardx_myflush(filp->private_data, 0); /* No timeout */
      rc = hvcardx_myflush(channel, 0); /* No timeout */

      if (rc && (rc != -EINTR))
        {
          mutex_unlock(&dma_op_mutex);
          return rc;
        }
    }

  mutex_unlock(&dma_op_mutex);
  if (channel->endpoint->fatal_error)
    {
      return -EIO;
    }

  return bytes_done;

}

