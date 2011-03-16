package org.cern.cms.csc.dw.model.monitor;

import com.jezhumble.javasysmon.MemoryStats;
import javax.persistence.Basic;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.Table;

@Entity
@Table(name = "DWM_SYSTEM")
public class MonitorSystem extends MonitorEntity {

    private Float cpu;
    private Float ram;
    private Float swap;

    public MonitorSystem() { }

    public MonitorSystem(Float cpu, MemoryStats ramStats, MemoryStats swapStats) {
        this.cpu = cpu;
        this.ram = calcUsedPercentage(ramStats);
        this.swap = calcUsedPercentage(swapStats);
    }

    private Float calcUsedPercentage(MemoryStats ms) {
        Double total = (double) ms.getTotalBytes();
        Double used = (total - (double) ms.getFreeBytes()) / total;
        return used.floatValue();
    }

    @Basic
    @Column(name = "MON_CPU")
    public Float getCpu() {
        return cpu;
    }

    public void setCpu(Float cpu) {
        this.cpu = cpu;
    }

    @Basic
    @Column(name = "MON_RAM")
    public Float getRam() {
        return ram;
    }

    public void setRam(Float ram) {
        this.ram = ram;
    }

    @Basic
    @Column(name = "MON_SWAP")
    public Float getSwap() {
        return swap;
    }

    public void setSwap(Float swap) {
        this.swap = swap;
    }

}
