===================
CDX Networking 2016
===================

Changes from last year
-----------------------

Changes:
 - Standardized network sizes as /27s (only one subnet mask to remember)
 - Removed redundant FTP/WebApp Subnet
 - Use VDSs and VLANs to partition network, rather than discrete switches

VMware ESXi VDS Setup/Description
---------------------------------

This year, we're using ESXi Virtual Distributed Swiches (VDSs) for all virtual networking tasks.
These VDSs have a few nice capabilities:
 - SPAN port mirroring
 - Collect NetFlow data
 - MAC-address based access control at layer 2

So, I created a VDS called CDXSwitch.  This VDS contains all of the networking for CDX.  VDSs
are slightly confusing as they abstract over physical NICs, calling them "uplinks".  These
uplinks are essentially VLAN trunk ports that daisy chain each host's virtual switch so it
appears to be one seamless switch.  Right clicking on the switch and saying "Add/manage hosts"
allows you to add both of the hosts, and allocate a physical NIC on each to an uplink.  If
you connect those physical NICs, then the virtual switch should "just work" for both hosts.

Next, to create all of the individual subnets that will be used for CDX, you create distributed
port groups.  You assign each port group a VLAN ID - one unique VLAN ID per layer two subnet.
See Network Diagram for details.  Note that any network that is going to have layer 2 bridging
(e.g. inline IPS or bridging router) needs to have all of the security settings disabled (i.e.
allow promiscuous, MAC address changes, and forged transmits) - you can do this by editing the
security settings on the port group.  One possible future improvement is allowing port-specific
overrides, and only allowing promiscuous mode on the individual ports that are connected to the
necessary VMs (the bridging machines).

One wrinkle in the VDS system is that interfacing with the physical world is cumbersome. The
easiest way to connect with the physical world is to make standard host-based networks.  These
can be done by going into the Host->Manage->Networking view.  Just create a new virtual switch,
pair that virtual switch the adapter, and then make a new network/port group on that virtual
switch.  All uplinks are VLAN trunk ports, so it would also be possible to connect an uplink
to a managed switch or bridging router potentially and break out a physical interface from
there.  That's probably more trouble than it's worth - just go with the legacy standard
switch.

VDSs allow port mirroring and NetFlow data - right now, it seems like the easiest way to
transmit the NetFlow data is just over the management network.  For SPAN ports, right now
we plan to set up the span to mirror individual ports on the switch out to an uplink, and
then monitor on that uplink.  I *think* this should just send it out over standard VLAN
trunk protocols that can get processed by any decent network monitoring suite, but I'm
not sure.

pfSense
---------

Just a few notes on setting up pfSense.

First, pfSense NATs by default - make sure you disable that in Firewall->NAT->Outbound.
Or, instead of disabling NAT, you can enable Advanced Outbound NAT (AON, e.g. manual NAT)
and write a NAT rule outbound from the 192.168.0.0/16 subnet so that non-exercise net
workstations can access SIMNET/{!USNA}.BLUENET sites, as without NAT the reverse route
does not exist.

pfSense also gets strange if you don't define a LAN interface, or if you don't intend to
manage it through the LAN interface.  At install time, put the LAN interface on the
interface you plan to use to access the webConfigurator, even if that isn't "LAN" in the
traditional sense.

FTP-Proxy Routing
------------------

In order to enable transparent FTP proxying, you first need to create a new gateway that
has as its destination the proxy server.  Then, you need to create a new rule that matches
FTP connections and under "Advanced Features", change the Gateway to the gateway you just
defined.  Don't bother with iptables bullshit, BUT IP forwarding needs to be enabled on
the proxy.

