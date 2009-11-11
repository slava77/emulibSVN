package org.cern.cms.csc.cdw.model;
// Generated Oct 27, 2009 3:17:29 AM by Hibernate Tools 3.2.4.GA


import java.util.HashSet;
import java.util.Set;

/**
 * Severity generated by hbm2java
 */
public class Severity  implements java.io.Serializable {


     private String id;
     private Set<Fact> facts = new HashSet<Fact>(0);

    public Severity() {
    }

	
    public Severity(String id) {
        this.id = id;
    }
    public Severity(String id, Set<Fact> facts) {
       this.id = id;
       this.facts = facts;
    }
   
    public String getId() {
        return this.id;
    }
    
    public void setId(String id) {
        this.id = id;
    }
    public Set<Fact> getFacts() {
        return this.facts;
    }
    
    public void setFacts(Set<Fact> facts) {
        this.facts = facts;
    }




}

