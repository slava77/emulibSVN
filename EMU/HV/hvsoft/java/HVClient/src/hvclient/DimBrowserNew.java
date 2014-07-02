package hvclient;

import dim.*;
/**
 * <p>Title: </p>
 *
 * <p>Description: </p>
 *
 * <p>Copyright: Copyright (c) 2004</p>
 *
 * <p>Company: </p>
 *
 * @author not attributable
 * @version 1.0
 */
public class DimBrowserNew
{
        static DimInfo answerSrvc;
        static DimLock dimLock;
        static String answer;
        static String[] allServices;

    private DimBrowserNew()// we do not allow instantiation of this class
    {
        answerSrvc = null;
        dimLock = null;
        allServices = null;
    }

    public static synchronized String[] getServices(String pattern)
    {
                if(dimLock == null)
                        dimLock = new DimLock();
                dimLock.reset();
        if(answerSrvc == null)
        {
                        answerSrvc = new DimUpdatedInfo("DIS_DNS/SERVICE_INFO/RpcOut","__DIM_NO_LINK__")
                        {
                                public void infoHandler()
                                {
                                        answer = getString();
                                        if(answer.compareTo("__DIM_NO_LINK__") == 0)
                                        {
                                                answer = "";
                                        }
                                        else
                                        {
                                        }
                                        dimLock.dimWakeUp();
                                }
                        };
        }
                DimClient.sendCommand("DIS_DNS/SERVICE_INFO/RpcIn", pattern);
                dimLock.dimWait();
                if(answer.length() != 0)
                {
                        allServices = answer.split("\n");
                }
                else
                {
                        allServices = new String[0];
                }
                String[] services = new String [allServices.length];
                for(int i = 0; i < allServices.length;i++)
                {
                        int index = allServices[i].indexOf('|');
                        services[i] = allServices[i].substring(0,index);
                }
                answerSrvc.releaseService();
                answerSrvc = null;
                return services;
    }
    public static synchronized boolean isCommand(String service)
    {
                int index = findService(service);
                if(index == -1)
                        return false;
                int i = allServices[index].lastIndexOf('|');
                if(allServices[index].substring(i+1).compareTo("CMD") == 0)
                        return true;
                return false;
    }
        public static synchronized String getFormat(String service)
        {
                int index = findService(service);
                if(index == -1)
                        return null;
                int i = allServices[index].indexOf('|');
                int j = allServices[index].lastIndexOf('|');
                String format = allServices[index].substring(i+1,j);
                return format;
        }
    private static int findService(String service)
    {
                for(int i = 0; i < allServices.length;i++)
                {
                        int index = allServices[i].indexOf('|');
                        if(service.compareTo(allServices[i].substring(0,index)) == 0)
                                return i;
                }
                return -1;
    }
}
