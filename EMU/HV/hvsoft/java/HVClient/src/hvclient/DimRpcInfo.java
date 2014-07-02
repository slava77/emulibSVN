package hvclient;

import dim.*;
import java.nio.*;

/**
 * <p>Title: HVClient</p>
 *
 * <p>Description: HVClient Java GUI Application</p>
 *
 * <p>Copyright: Copyright (c) 2005</p>
 *
 * <p>Company: University of Florida</p>
 *
 * @author Victor Barashko <barvic@ufl.edu>
 * @version 1.0
 */
public class DimRpcInfo {
  boolean debug = false;
  protected boolean useServiceID = true;

  protected String ServiceName;
  protected String NameRpcIn;
  protected String NameRpcOut;

  protected ByteBuffer dataOut = ByteBuffer.allocate(4);
  protected ByteBuffer dataIn = ByteBuffer.allocate(4);
  protected DimInfo answerSrvc;
  protected DimLock dimLock;
  protected int srvcID;
  protected int timeout = 2;

  public void setDebug(boolean setdebug) {
    debug = setdebug;
  }

  public void setTimeout(int tout) {
    timeout = tout;
  }

  public int setData(String data) {
    dataOut.clear();
    dimLock.reset();
    if (debug) {
     System.out.println(ServiceName + "#" + Integer.toString(answerSrvc.
         getServiceID()) + "> sending command");
   }

    int status = DimClient.sendCommand(NameRpcIn, data);
    if (status > 0) {
     // System.out.println("Locking");   
     // dimLock.dimWait(timeout);
     // System.out.println("UnLocked");
   }

    if (debug) {
      System.out.println(ServiceName + "#" + Integer.toString(answerSrvc.
          getServiceID()) + "> send status:" + ( (status > 0) ? "OK" : "FAILED"));
    }


    return status;
  }

  public int setData(ByteBuffer data) {
    dataOut.clear();
    dimLock.reset();
    data.order(ByteOrder.LITTLE_ENDIAN);
    data.rewind();
    dataIn.clear();

    if (useServiceID) {
      dataIn = ByteBuffer.allocate(data.capacity() + 8);
      dataIn.putLong(srvcID);
    }
    else {
      dataIn = ByteBuffer.allocate(data.capacity());
    }

    dataIn.put(data);
    if (debug) {
      System.out.println(ServiceName + "#" + Integer.toString(answerSrvc.
          getServiceID()) + "> sending command");
    }

    int status = DimClient.sendCommand(NameRpcIn, dataIn.array());
    if (status > 0) {
      // System.out.println("Locking2");
      // dimLock.dimWait(timeout);
      // System.out.println("UnLocked2");
    }

    if (debug) {
      System.out.println(ServiceName + "#" + Integer.toString(answerSrvc.
          getServiceID()) + "> send status:" + ( (status > 0) ? "OK" : "FAILED"));
    }


    return status;
  }

  public String getString() {

    int pos = 0;
    dataOut.rewind();
    while ( (pos < dataOut.array().length) && (dataOut.array()[pos] != (byte) 0)) {
      pos++;
    }
    // System.out.println(Integer.toString(pos));
    byte[] data = new byte[pos];
    System.arraycopy(dataOut.array(), 0, data, 0, pos);

    return new String(data);
  }

  public int getSize() {
    return dataOut.array().length - 8;
  }

  public ByteBuffer getData() {
    dataOut.rewind();
    return dataOut;
  }

  protected void finalize() {
    dimLock.reset();
    answerSrvc.releaseService();
  }

  public boolean getServiceIDMode() {
    return useServiceID;
  }

  public void setServiceIDMode(boolean useID) {
    useServiceID = useID;
  }

  public int getServiceID() {
    return srvcID;
  }

  public String getServiceName() {
    return ServiceName;
  }

  public DimRpcInfo(String servicename) {
    ServiceName = servicename;
    NameRpcIn = ServiceName + "/RpcIn";
    NameRpcOut = ServiceName + "/RpcOut";
    dataOut.allocate(8);
    dataOut.order(ByteOrder.LITTLE_ENDIAN);
    try {
      if (dimLock == null) {
        dimLock = new DimLock();
      }
      dimLock.reset();
      if (answerSrvc == null) {
        answerSrvc = new DimInfo(
            NameRpcOut, 0,"__DIM_NO_LINK__") {
          public void infoHandler() {
            if (useServiceID) {
              ByteBuffer data = ByteBuffer.wrap(answerSrvc.getByteArray());
              if (data.array().length > 4 && (data.getLong() == srvcID)) {
                dataOut = data.slice();
              }
            }
            else {
              dataOut = ByteBuffer.wrap(answerSrvc.getByteArray());
            }

            if (debug) {
              System.out.println(ServiceName + "#" +
                                 Integer.toString(answerSrvc.
                                                  getServiceID()) +
                                 "> data received:" +
                                 Integer.toString(dataOut.array().length));
            }

            dimLock.dimWakeUp();
            // System.out.println("UnLocking3");
          }
        };
        srvcID = answerSrvc.getServiceID();
      }
    }
    catch (Exception exception) {
      exception.printStackTrace();
    }
  }
}
