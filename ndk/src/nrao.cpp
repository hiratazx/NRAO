#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>

#include "util_functions.hpp"

using namespace std;

// Declare functions.
void mem_preload_tweak();
void cmd_services_tweak();
void main_dex_tweak();
void secondary_dex_tweak();
void apply_all_tweaks();

// Tweak to preload important system objects into memory.
void mem_preload_tweak() {
  vector<string> apex_objects = {
    "linker",
    "linker64",
    "libm.so",
    "libdl.so"
  };
  for (const string& apex_object : apex_objects) {
    if (is_path_exists("/apex/com.android.runtime/bin/" + apex_object)) {
      preload_item("obj", "/apex/com.android.runtime/bin/" + apex_object);
    } 
    if (is_path_exists("/apex/com.android.runtime/lib/bionic/" + apex_object)) {
      preload_item("obj", "/apex/com.android.runtime/lib/bionic/" + apex_object);
    } 
    if (is_path_exists("/apex/com.android.runtime/lib64/bionic/" + apex_object)) {
      preload_item("obj", "/apex/com.android.runtime/lib64/bionic/" + apex_object);
    }         
  }
  
  vector<string> bin_objects = {
    "dalvikvm",
    "app_process"
  };
  for (const string& bin_object : bin_objects) {
    if (is_path_exists("/bin/" + bin_object)) {
      preload_item("obj", "/bin/" + bin_object);
    }    
  }  

  vector<string> lib_objects = {     
    "libandroid.so",        
    "libandroid_runtime.so",
    "libandroid_servers.so",    
    "libEGL.so", 
    "libGLESv1_CM.so",      
    "libGLESv2.so", 
    "libGLESv3.so",       
    "libsurfaceflinger.so",    
    "libhwui.so",
    "libui.so",
    "libskia.so",     
    "libvulkan.so", 
    "libRScpp.so",
    "libinputflinger.so",  
    "libharfbuzz_ng.so",  
    "libprotobuf-cpp-full.so", 
    "libprotobuf-cpp-lite.so", 
    "libblas.so",
    "libutils.so"    
  };
  for (const string& lib_object : lib_objects) {
    if (is_path_exists("/system/lib/" + lib_object)) {
      preload_item("obj", "/system/lib/" + lib_object);
    } 
    if (is_path_exists("/system/lib64/" + lib_object)) {
      preload_item("obj", "/system/lib64/" + lib_object);    
    } 
  }

  vector<string> framework_objects = {
    "framework.jar",
    "services.jar",
    "protobuf.jar"
  };
  for (const string& framework_object : framework_objects) {
    if (is_path_exists("/system/framework/" + framework_object)) {
      preload_item("obj", "/system/framework/" + framework_object);
    }    
  }  

  vector<string> pkg_names = {
    "com.android.systemui",
    get_home_pkg_name(),
    get_ime_pkg_name()
  };   
  for (const string& pkg_name : pkg_names) {
    preload_item("app", pkg_name);
  }

  // Print tweak completion message.  
  xlog("date", "Preloaded important system items into RAM.");  
}

// Tweak to disable debugging, statistics, unnecessary background apps, phantom process killer, lock profiling (analysis tool), make changes to `DeviceConfig` flags persistent, enable IORAP readahead feature, improve FSTrim time interval and tweak `ActivityManager`.
void cmd_services_tweak() {
  // List of service commands of `cmd` to execute.
  vector<string> svc_cmds = {
    "settings put system anr_debugging_mechanism 0",
    "looper_stats disable",
    "settings put global netstats_enabled 0",  
    "appops set com.android.backupconfirm RUN_IN_BACKGROUND ignore",
    "appops set com.google.android.setupwizard RUN_IN_BACKGROUND ignore",
    "appops set com.android.printservice.recommendation RUN_IN_BACKGROUND ignore",
    "appops set com.android.onetimeinitializer RUN_IN_BACKGROUND ignore",
    "appops set com.qualcomm.qti.perfdump RUN_IN_BACKGROUND ignore",
    "power set-fixed-performance-mode-enabled true",
    "activity idle-maintenance",
    "thermalservice override-status 1",
    "settings put global settings_enable_monitor_phantom_procs false",
    "device_config put runtime_native_boot disable_lock_profiling true",
    "device_config set_sync_disabled_for_tests persistent",
    "device_config put runtime_native_boot iorap_readahead_enable true",
    "settings put global fstrim_mandatory_interval 3600",
    "device_config put activity_manager max_phantom_processes 2147483647",
    "device_config put activity_manager max_cached_processes 256",
    "device_config put activity_manager max_empty_time_millis 43200000"
  };
  
  // Iterate through the list of service commands.
  for (const string& svc_cmd : svc_cmds) {
    // Use exec_shell() function to execute shell command.
    exec_shell("cmd " + svc_cmd, false);  
  }  

  // Print tweak completion message.
  xlog("date", "Tweaked `cmd` services.");
}

void apply_all_tweaks() {
  xlog("info", "Started NRAO Tweaks at " + print_date("full"));
  xlog("", "");
  
  mem_preload_tweak();
  cmd_services_tweak();

  xlog("", "");
  xlog("info", "Completed NRAO Tweaks at " + print_date("full"));
}

int main(int argc, char *argv[]) {        
  // Write all modified data to disk.
  sync();

  // Apply all tweaks.
  apply_all_tweaks();
  
  // Write all modified data to disk.
  sync();    
        
  return 0;
}
