sendSOAP -v  -u emuslice02 -p 20210 -A emu::daq::manager::Application -I 0 -b "<esd:factRequestCollection xmlns:esd='http://www.cern.ch/cms/csc/dw/data'><esd:requestId>'98765'</esd:requestId><esd:factRequest><esd:component>COMPONENT 1</esd:component><esd:factType>slidingTmbTriggerCounterFact</esd:factType></esd:factRequest><esd:factRequest><esd:component>COMPONENT 2</esd:component><esd:factType>slidingTmbTriggerCounterFact</esd:factType></esd:factRequest><esd:factRequest><esd:component>COMPONENT 3</esd:component><esd:factType>cumulativeTmbTriggerCounterFact</esd:factType></esd:factRequest></esd:factRequestCollection>"
