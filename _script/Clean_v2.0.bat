attrib -r -h -s ..\BuildTime.* /s
attrib -r -h -s ..\*.intermediate.manifest /s
attrib -r -h -s ..\*.embed.manifest /s
attrib -r -h -s ..\*.dep /s
attrib -r -h -s ..\*.user /s
attrib -r -h -s ..\*.obj /s
attrib -r -h -s ..\*.idb /s
attrib -r -h -s ..\*.aps /s
attrib -r -h -s ..\*.res /s
attrib -r -h -s ..\*.plg /s
attrib -r -h -s ..\*.opt /s
attrib -r -h -s ..\*.bsc /s
attrib -r -h -s ..\*.ncb /s
attrib -r -h -s ..\*.bak /s
attrib -r -h -s ..\*.tmp /s
attrib -r -h -s ..\*.idb /s
attrib -r -h -s ..\vc*.pdb /s
attrib -r -h -s ..\*.exp /s
attrib -r -h -s ..\*.ilk /s
attrib -r -h -s ..\*.map /s
attrib -r -h -s ..\disasm.asm /s
attrib -r -h -s ..\*.trg /s
attrib -r -h -s ..\*.pch /s
attrib -r -h -s ..\*.~* /s
attrib -r -h -s ..\*.ddp /s
attrib -r -h -s ..\*.suo /s
attrib -r -h -s ..\*.scc /s
attrib -r -h -s ..\readme.txt /s
attrib -r -h -s ..\thumbs.db /s
attrib -r -h -s ..\*.cod /s
attrib -r -h -s ..\*.ipch /s
attrib -r -h -s ..\*.sdf /s
attrib -r -h -s ..\*.tlog /s
attrib -r -h -s ..\*.log /s
attrib -r -h -s ..\*.lastbuildstate /s
attrib -r -h -s ..\Builds\vc7\temp\*.* /s
attrib -r -h -s ..\Builds\vc8\temp\*.* /s
attrib -r -h -s ..\Builds\vc10\temp\*.* /s

del /s ..\BuildLog.htm 
del /s ..\*.intermediate.manifest 
del /s ..\*.embed.manifest
del /s ..\*.dep
del /s ..\*.user 
del /s ..\*.obj 
del /s ..\*.idb 
del /s ..\*.aps 
del /s ..\*.res 
del /s ..\*.plg 
del /s ..\*.opt 
del /s ..\*.bsc
del /s ..\*.ncb 
del /s ..\*.bak 
del /s ..\*.tmp 
del /s ..\vc*.pdb
del /s ..\*.idb 
del /s ..\*.exp 
del /s ..\*.ilk 
del /s ..\*.map 
del /s ..\disasm.asm 
del /s ..\*.trg 
del /s ..\*.pch 
del /s ..\*.~* 
del /s ..\*.ddp 
del /s ..\*.suo 
del /s ..\*.scc 
del /s ..\readme.txt 
del /s ..\thumbs.db 
del /s ..\*.cod 
del /s ..\*.ipch 
del /s ..\*.sdf 
del /s ..\*.tlog 
del /s ..\*.log 
del /s ..\*.lastbuildstate

rd /s /q ..\Builds\vc7\temp\
rd /s /q ..\Builds\vc8\temp\
rd /s /q ..\Builds\vc10\temp\