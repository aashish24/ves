/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 ========================================================================*/
#ifndef _MemoryUsageWatcher_h
#define _MemoryUsageWatcher_h


#include <mach/mach.h> 

// implementation from: http://stackoverflow.com/questions/7989864/watching-memory-usage-in-ios
class MemoryUsageWatcher {

public:

  static vm_size_t usedMemory()
  {
    struct task_basic_info info;
    mach_msg_type_number_t size = sizeof(info);
    kern_return_t kerr = task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&info, &size);
    return (kerr == KERN_SUCCESS) ? info.resident_size : 0; // size in bytes
  }

  static vm_size_t freeMemory()
  {
    mach_port_t host_port = mach_host_self();
    mach_msg_type_number_t host_size = sizeof(vm_statistics_data_t) / sizeof(integer_t);
    vm_size_t pagesize;
    vm_statistics_data_t vm_stat;

    host_page_size(host_port, &pagesize);
    (void) host_statistics(host_port, HOST_VM_INFO, (host_info_t)&vm_stat, &host_size);
    return vm_stat.free_count * pagesize;
  }

  static void logMemoryUsage()
  {
    static long prevMemUsage = 0;
    long curMemUsage = usedMemory();
    long memUsageDiff = curMemUsage - prevMemUsage;
    prevMemUsage = curMemUsage;

    double bytesToMb = 1.0/(1024.0*1024.0);

    if (abs(memUsageDiff) > 1024*1024)
    {
      NSLog(@"Memory used %.2f mb (+%.2f mb), free %.2f mb", curMemUsage*bytesToMb, memUsageDiff*bytesToMb, freeMemory()*bytesToMb);
    }
  }

  static void notifyMemoryUsage()
  {
    double memoryUsage = MemoryUsageWatcher::usedMemory()/(1024.0*1024.0);
    NSDictionary* args = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithDouble:memoryUsage], @"MemoryUsage", nil];
    [[NSNotificationCenter defaultCenter] postNotificationName:@"UpdateMemoryUsage" object:nil userInfo:args];
  }

};

#endif
