package hvclient;

// import java.io.Serializable;
import java.io.*;

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
public class HVMonitoringThresholds
    implements Serializable {

  int vmon_min = 25;
  int vmon_max = 25;
  double imon_min = 1.0;
  double imon_max = 1.0;
  double imon_total = 45;

  public HVMonitoringThresholds() {
  }
  /*
     private void writeObject(java.io.ObjectOutputStream out)
    throws IOException {
     out.writeInt(vmon_min);
     out.writeInt(vmon_max);
     out.writeDouble(imon_min);
     out.writeDouble(imon_max);
     out.writeDouble(imon_total);


   }
     private void readObject(java.io.ObjectInputStream in)
    throws IOException, ClassNotFoundException {
       vmon_min = in.readInt();
       vmon_max = in.readInt();
       imon_min = in.readDouble();
       imon_max = in.readDouble();
       imon_total = in.readDouble();
    }
   */
}
