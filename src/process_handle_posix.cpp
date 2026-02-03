
/*

Copyright 2026 Giovanni Gutiérrez Ramírez

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/

#include "process_handle_posix.h"

#include <signal.h>
#include <errno.h>
#include <fstream>

time_t ProcessHandle::_get_process_start_time(const pid_t p_pid) const {
    if (::kill(p_pid, 0) != 0) {
        if (errno == ESRCH) {
            return time_t();
        }
    }

    std::ifstream stat_file("/proc/" + std::to_string(p_pid) + "/stat");

    if (!stat_file.is_open()) {
        return time_t();
    }

    std::string line;
    std::getline(stat_file, line);

    auto rparen = line.rfind(")");

    if (rparen == std::string::npos) {
        return time_t();
    }

    return time_t();
}

ProcessHandle::ProcessHandle(pid_t p_pid) {
    pid = p_pid;
    start_time = _get_process_start_time(pid);
}

ProcessHandle::~ProcessHandle() {

}

pid_t ProcessHandle::get_pid() const {
    return pid;
}

time_t ProcessHandle::get_start_time() const {
    return start_time;
}
