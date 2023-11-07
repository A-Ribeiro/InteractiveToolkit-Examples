@echo off

echo script started
ping 192.0.2.0 -n 1 -w 1000 > nul
echo will print message in: 3
ping 192.0.2.0 -n 1 -w 1000 > nul
echo will print message in: 2
ping 192.0.2.0 -n 1 -w 1000 > nul
echo will print message in: 1
ping 192.0.2.0 -n 1 -w 1000 > nul
echo %1%
ping 192.0.2.0 -n 1 -w 1000 > nul
echo script finished
ping 192.0.2.0 -n 1 -w 1000 > nul

exit 0
