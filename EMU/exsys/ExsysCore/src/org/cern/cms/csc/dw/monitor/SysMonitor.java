package org.cern.cms.csc.dw.monitor;

import com.jezhumble.javasysmon.CpuTimes;
import com.jezhumble.javasysmon.JavaSysMon;
import com.jezhumble.javasysmon.MemoryStats;
import com.jezhumble.javasysmon.ProcessInfo;

public class SysMonitor {

    private JavaSysMon sysmon = new JavaSysMon();
    private CpuTimes initialTimes = null;

    public String getOsName() {
        return sysmon.osName();
    }

    public Integer getNumCpus() {
        return sysmon.numCpus();
    }

    public Long getCpuFrequencyInHz() {
        return sysmon.cpuFrequencyInHz();
    }

    public Long getCpuFrequencyInMHz() {
        return getCpuFrequencyInHz() / (1000 * 1000);
    }

    public String getUpTime() {
        return secsInDaysAndHours(sysmon.uptimeInSeconds());
    }

    public int getCurrentPid() {
        return sysmon.currentPid();
    }

    public MemoryStats getRam() {
        return sysmon.physical();
    }

    public MemoryStats getSwap() {
        return sysmon.swap();
    }

    public String getRamAsString() {
        return sysmon.physical().toString();
    }

    public String getSwapAsString() {
        return sysmon.swap().toString();
    }

    public Float getRamUsedPercentage() {
        return calcUsedPercentage(getRam());
    }

    public Float getSwapUsedPercentage() {
        return calcUsedPercentage(getSwap());
    }

    private Float calcUsedPercentage(MemoryStats ms) {
        Double total = (double) ms.getTotalBytes();
        Double used = (total - (double) ms.getFreeBytes()) / total;
        return used.floatValue();
    }

    public ProcessInfo[] getProcesses() {
        return sysmon.processTable();
    }

    public void startCpuSampling() {
        this.initialTimes = sysmon.cpuTimes();
    }

    public Float getCpuUsage() {
        if (initialTimes == null) {
            startCpuSampling();
            try {
                Thread.sleep(500);
            } catch (InterruptedException ex) {
            }
        }
        float usage = sysmon.cpuTimes().getCpuUsage(this.initialTimes);
        this.initialTimes = null;
        return usage;
    }

    public String getProcessHeader() {
        return ProcessInfo.header();
    }

    private static String secsInDaysAndHours(long seconds) {
        long days = seconds / (60 * 60 * 24);
        long hours = (seconds / (60 * 60)) - (days * 24);
        return days + " days " + hours + " hours";
    }

}
