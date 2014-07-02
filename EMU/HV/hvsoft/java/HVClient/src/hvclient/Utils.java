package hvclient;

import java.util.Map;
import java.nio.ByteBuffer;

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
public class Utils {

  public static int unsignedByteToInt(byte b) {
    return (int) b & 0xFF;
  };


  public static int NetByteArrayToShort(byte[] buf, int c) {
    int a = 0;
    for (int i = 0; i < 2; i++) {
      a += unsignedByteToInt(buf[c + i]) << 8 * i;
    }
    return a;
  };

  public static int NetByteArrayToInteger(byte[] buf, int c) {
    int a = 0;
    for (int i = 0; i < 4; i++) {
      a += unsignedByteToInt(buf[c + i]) << 8 * i;
    }
    return a;
  };

  public static float NetByteArrayToFloat(byte[] buf, int c) {
    Float d;
    float a = 0;
      for (int i = 0; i < 4; i++) {
        a += unsignedByteToInt(buf[c + i]) << 8 * i;
      }
      return a;
  };

  public static void writeMapValue(String key, Map map, ByteBuffer buf) {
      Class cl = map.get(key).getClass();
      if ( cl == Integer.class) {
        buf.putInt((Integer)map.get(key));
      } else
      if ( cl == Float.class) {
        buf.putFloat((Float)map.get(key));
      } else
      if ( cl == Double.class) {
        buf.putDouble((Double)map.get(key));
      }
    }

  public static void readMapValue(String key, Map map, ByteBuffer buf) {
      Class cl = map.get(key).getClass();
      if ( cl == Integer.class) {
        map.put(key, buf.getInt());
      } else
      if ( cl == Float.class) {
        map.put(key, buf.getFloat());
      } else
      if ( cl == Double.class) {
        map.put(key, buf.getDouble());
      }
  }

  public Utils() {
  }
}
