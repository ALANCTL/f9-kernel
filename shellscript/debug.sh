pkill st-util

st-util &

rm ./gdbscript/result.log

arm-none-eabi-gdb -x ./gdbscript/.gdbinit build/discoveryf4/f9.elf -tui

mv gdb.txt ./gdbscript/result.log
