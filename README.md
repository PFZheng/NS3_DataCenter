This extention can be used to build data centers more easier in NS-3 simulator(http://nsnam.org/). The key models are modified from 'bridge' and 'csma'. 
It provides following classes:
DCNode(dc-node.h/cc): the base class of DCVm, DCHost, DCSwitch.
DCSwitch(dc-switch.h/cc): switch.
DCHost(dc-host.h/cc): host.
DCVm(dc-vm.h/cc): virtual machines.
DCTenant(dc-tenant.h/cc): tenant.
......

Q: How to use it?
A: Put it in <NS-3 directory>/src/datacenter.

Q: Which version of NS-3 it support?
A: It is compatible with version 3.16, other versions are not test yet.

Q: Does it support python binding?
A: No.

Q: How can I know the relationships between classes?
A: See diagram.jpg.
