target extended-remote :4242

set breakpoint pending on
set logging on
set logging file ./gdbscript/profiling_result.log
source ./gdbscript/setting.in
