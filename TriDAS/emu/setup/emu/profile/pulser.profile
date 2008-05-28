<?xml version='1.0'?>
<!-- Order of specification will determine the sequence of installation. all modules are loaded prior instantiation of plugins -->
<xp:Profile xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/" 
xmlns:xp="http://xdaq.web.cern.ch/xdaq/xsd/2005/XMLProfile-11">

        <!-- Compulsory  Plugins -->
        <xp:Application class="executive::Application" id="0"  service="executive" network="local">
                <properties xmlns="urn:xdaq-application:Executive" xsi:type="soapenc:Struct">
                        <logUrl xsi:type="xsd:string">console</logUrl>
                        <logLevel xsi:type="xsd:string">INFO</logLevel>
                </properties>
        </xp:Application>
        <xp:Module>${XDAQ_ROOT}/lib/libexecutive.so</xp:Module>

        <xp:Application class="pt::http::PeerTransportHTTP" id="1"  network="local">
                 <properties xmlns="urn:xdaq-application:pt::http::PeerTransportHTTP" xsi:type="soapenc:Struct">
                        <documentRoot xsi:type="xsd:string">${XDAQ_DOCUMENT_ROOT}</documentRoot>
                        <aliasName xsi:type="xsd:string">tmp</aliasName>
                        <aliasPath xsi:type="xsd:string">/tmp</aliasPath>
                </properties>
        </xp:Application>
        <xp:Module>${XDAQ_ROOT}/lib/libpthttp.so</xp:Module>

        <xp:Application class="pt::fifo::PeerTransportFifo" id="8"  network="local"/>
        <xp:Module>${XDAQ_ROOT}/lib/libptfifo.so</xp:Module>

        <!-- XRelay -->
        <xp:Application class="xrelay::Application" id="4"  service="xrelay"  network="local"/>
        <xp:Module>${XDAQ_ROOT}/lib/libxrelay.so</xp:Module>

        <!-- HyperDAQ -->
        <xp:Application class="hyperdaq::Application" id="3"  service="hyperdaq" network="local"/>
        <xp:Module>${XDAQ_ROOT}/lib/libhyperdaq.so</xp:Module>

        <!-- XPlore requires the installation of Power Pack  -->
        <xp:Application class="xplore::Application" id="9"  network="local">
                <properties xmlns="urn:xdaq-application:xplore::Application" xsi:type="soapenc:Struct">
                        <settings xsi:type="xsd:string">${XDAQ_SETUP_ROOT}/${XDAQ_ZONE}/xplore/shortcuts.xml</settings>
                        <republishInterval xsi:type="xsd:string">60</republishInterval>
                </properties>
        </xp:Application>
        <xp:Module>/lib/libslp.so</xp:Module>
        <xp:Module>${XDAQ_ROOT}/lib/libxslp.so</xp:Module>
        <xp:Module>${XDAQ_ROOT}/lib/libxploreutils.so</xp:Module>
        <xp:Module>${XDAQ_ROOT}/lib/libxplore.so</xp:Module>

        <xp:Application class="sentinel::Application" id="21" network="local" group="exception" service="sentinel">
                <properties xmlns="urn:xdaq-application:Sentinel" xsi:type="soapenc:Struct">
                        <useDiscovery xsi:type="xsd:boolean">true</useDiscovery>
                        <publish xsi:type="xsd:string">exception</publish>
                        <watchdog xsi:type="xsd:string">PT5S</watchdog>
                </properties>
        </xp:Application>
        <xp:Module>${XDAQ_ROOT}/lib/libwsaddressing.so</xp:Module>
        <xp:Module>${XDAQ_ROOT}/lib/libwseventing.so</xp:Module>
        <xp:Module>${XDAQ_ROOT}/lib/libsentinelutils.so</xp:Module>
        <xp:Module>${XDAQ_ROOT}/lib/libsentinel.so</xp:Module>

        <xp:Application class="xmas::pulser::Application" id="100" network="local" group="%domain,init2" service="pulser">
                <properties xmlns="urn:xdaq-application:xmas::pulser::Application" xsi:type="soapenc:Struct">
                        <url xsi:type="soapenc:Array" soapenc:arrayType="xsd:ur-type[7]">
                                <item xsi:type="xsd:string" soapenc:position="[1]">${XDAQ_SETUP_ROOT}/${XDAQ_ZONE}/pulser/jobcontrol.pulser</item>
                        </url>
                        <useDiscovery xsi:type="xsd:boolean">true</useDiscovery>
                </properties>
        </xp:Application>
        <xp:Module>${XDAQ_ROOT}/lib/libxmasutils.so</xp:Module>
        <xp:Module>${XDAQ_ROOT}/lib/libxmaspulser.so</xp:Module>
</xp:Profile>

