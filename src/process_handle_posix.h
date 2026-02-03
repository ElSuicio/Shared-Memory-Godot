
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

#pragma once

#include <ctime>
#include <unistd.h>

class ProcessHandle {

private:
	pid_t pid;
	time_t start_time;

	time_t _get_process_start_time(const pid_t p_pid) const;

public:
	ProcessHandle(pid_t p_pid);
	~ProcessHandle();

	pid_t get_pid() const;
	time_t get_start_time() const;

};
