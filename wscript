# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

# def options(opt):
#     pass

# def configure(conf):
#     conf.check_nonfatal(header_name='stdint.h',', define_name='HAVE_STDINT_H')

def build(bld):
    module = bld.create_ns3_module('datacenter', ['network', 'internet', 'applications'])
    module.source = [
        'model/dc-address-allocater.cc',
        'model/dc-backoff.cc',
        'model/dc-bridge-callback.cc',
        'model/dc-bridge-channel.cc',
        'model/dc-bridge-forward.cc',
        'model/dc-bridge-net-device-base.cc',
        'model/dc-bridge-net-device.cc',
        'model/dc-host.cc',
        'model/dc-node-list.cc',
        'model/dc-node-mapper.cc',
        'model/dc-node.cc',
        'model/dc-point-callback.cc',
        'model/dc-point-channel-base.cc',
        'model/dc-point-channel.cc',
        'model/dc-point-forward.cc',
        'model/dc-point-net-device-base.cc',
        'model/dc-point-net-device.cc',
        'model/dc-switch.cc',
        'model/dc-tenant-list.cc',
        'model/dc-tenant.cc',
        'model/dc-vm.cc',
        'helper/dc-helper.cc',
        'helper/dc-internet-stack-helper.cc',
        ]

    #module_test = bld.create_ns3_module_test_library('datacenter')
    #module_test.source = [
        #]

    headers = bld.new_task_gen(features=['ns3header'])
    headers.module = 'datacenter'
    headers.source = [
        'model/dc-address-allocater.h',
        'model/dc-backoff.h',
        'model/dc-bridge-callback.h',
        'model/dc-bridge-channel.h',
        'model/dc-bridge-net-device-base.h',
        'model/dc-bridge-net-device.h',
        'model/dc-bridge-forward.h',
        'model/dc-host.h',
        'model/dc-node-list.h',
        'model/dc-node-mapper.h',
        'model/dc-node.h',
        'model/dc-point-callback.h',
        'model/dc-point-channel-base.h',
        'model/dc-point-channel.h',
        'model/dc-point-forward.h',
        'model/dc-point-net-device-base.h',
        'model/dc-point-net-device.h',
        'model/dc-switch.h',
        'model/dc-tenant-list.h',
        'model/dc-tenant.h',
        'model/dc-vm.h',
        'helper/dc-node-container.h',
        'helper/dc-helper.h',
        'helper/dc-internet-stack-helper.h',
        'examples/option-parse.h'
        ]

    if bld.env.ENABLE_EXAMPLES:
        bld.add_subdirs('examples')

    #bld.ns3_python_bindings()

