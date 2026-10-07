:: 
:: $Copyright
:: Copyright 1993, 1994 , 1995 Intel Corporation
:: INTEL CONFIDENTIAL
:: The technical data and computer software contained herein are subject
:: to the copyright notices; trademarks; and use and disclosure
:: restrictions identified in the file located in /etc/copyright on
:: this system.
:: Copyright$
:: 
 
àßHã U‰åƒìWVSU‰Uüz‰û‹ƒÃ…Àu÷;rƒÃü‰$àƒ=Œ-t¡Œ-ÿĞƒ=ˆ-t¡ˆ-ÿĞ…Àt‰Äƒ=„-t¡„-ÿĞƒ=-t‹5-ë¾x8SW‹Uüÿ2èoPÿÖeğ[^_ÉÃusage: fast.cmd [-K keyfile]  [1 - %d]
Internal error: test case (%d)

shmdt failed:errno = %d
U‰åƒì‹E‹‰|-ƒ}~‹EƒÀÿ0èD2‰À‰À‰EüƒÄ…Àtë#ÿ5,àh°¸ ƒÀHPèÓ?jè7ƒÄèx'ÿuüè(‰EøƒÄƒ=”-t ÿuühØ¸ ƒÀHPè—?jèÔ6ƒÄÿ5€-èÖ^‰EôƒÄƒ}ôÿuÿ5(àhùèXjè£6ƒÄjè™6ÉÃtest.%02d.stdinw+Can't create test file %s

TEST ABORTED

test.%02d.stdouttest.%02d.stderrChecking usage message error handling,  cmd: fast -K /tmp/fastkeytest
š™™™™™ñ?š™™™™™@unable to setup shared memory
fast -K /tmp/fastkeytestusage: fast [ -v ] command [ arg ] ...
Checking argument error handling,  cmd: fast -x -K /tmp/fastkeytest
fast -x -K /tmp/fastkeytestChecking argument error handling,  cmd: fast -v -K /tmp/fastkeytest
fast -v -K /tmp/fastkeytestChecking argument error handling with valid executable,  cmd: fast -x -K /tmp/fastkeytest /bin/hostname
fast -x -K /tmp/fastkeytest /bin/hostnameExpecting node 0, nonverbose,  cmd: fast -K /tmp/fastkeytest fast.cmd.remote
fast -K /tmp/fastkeytest fast.cmd.remotestdout: my node is 0
stdout: argc is 1
stdout: arg 0 is fast.cmd.remote
stderr: my node is %d
stderr: argc is 1
stderr: arg 0 is fast.cmd.remote
Expecting node 0, verbose, cmd: fast -v -K /tmp/fastkeytest fast.cmd.remote
fast -v -K /tmp/fastkeytest fast.cmd.remotefast node 0
stderr: my node is 0
Checking multiple -v handling,  cmd: fast -v -v -K /tmp/fastkeytest fast.cmd.remote
fast -v -v -K /tmp/fastkeytest  fast.cmd.remoteAttempting to execute nonexistant file, nonverbose; cmd: fast -K /tmp/fastkeytest ./not_found
rm -f ./not_found > /dev/null 2>&1fast -K /tmp/fastkeytest ./not_foundfast: ./not_found not found.
Attempting to execute nonexistant file, verbose; cmd: fast -K /tmp/fastkeytest ./not_found
fast -v -K /tmp/fastkeytest ./not_foundAttempting to execute non-executable file, nonverbose (file permissions test); cmd: fast -K /tmp/fastkeytest ./no_exec
rm -f no_exec; touch ./no_exec; chmod a-x no_execfast -K /tmp/fastkeytest ./no_execfast: can't execute ./no_exec.
Attempting to execute nonexistant file, verbose (file permissions test); cmd: fast -K /tmp/fastkeytest ./not_found
fast -v -K /tmp/fastkeytest ./no_execTesting argument handling with lots of args, nonverbose; cmd: fast -K /tmp/fastkeytest fast.cmd.remote arg1 arg2 arg3 arg4 arg5 arg6 arg7 arg8 arg9
 arg1 arg2 arg3 arg4 arg5 arg6 arg7 arg8 arg9stdout: argc is 10
stdout: arg 1 is arg1
stdout: arg 2 is arg2
stdout: arg 3 is arg3
stdout: arg 4 is arg4
stdout: arg 5 is arg5
stdout: arg 6 is arg6
stdout: arg 7 is arg7
stdout: arg 8 is arg8
stdout: arg 9 is arg9
stderr: argc is 10
stderr: arg 1 is arg1
stderr: arg 2 is arg2
stderr: arg 3 is arg3
stderr: arg 4 is arg4
stderr: arg 5 is arg5
stderr: arg 6 is arg6
stderr: arg 7 is arg7
stderr: arg 8 is arg8
stderr: arg 9 is arg9
Testing error condition: shared memory not setup or accessible, 
verbose: should default to 0; cmd: fast -K /tmp/fastkeytest -v fast.cmd.remote
fast node %d
stdout: my node is %d
fast: load information unavailable; defaulting to local node.
Default case -- unknown
Unknown test case #%d

%s
Can't write into test file %s
Can't close test file %s errno=%d
TZ=; PATH=; export TZ PATH;  <  >  2> Can't execute shell with 'system'.
Command was: %s
rCan't reopen test file %s
Test file %s too long
FAILED %s TEST %2d FILE STDOUT
Expected len=%d
'%s'
Got len=%d
'%s'
PASSED %s TEST %2d FILE STDOUT
FAILED %s TEST %2d FILE STDERR
PASSED %s TEST %2d FILE STDERR
FAILING COMMAND WAS: '%s'
U‰åì„Ç…¤oÿÿÇ… oÿÿÿuháEàPèÎLhñEàPèğ0‰EüƒÄƒ}üuKÿ”-EàPhô¸ ƒÀHPè/2ƒÄh¸ ƒÀHPè2¸ ƒÀHPèŸ,jèH)ƒÄÿuhEÌPèXLhñEÌPèz0‰EøƒÄƒ}øu"ÿ”-EÌPhô¸ ƒÀHPè¹1ƒÄëˆÿuh0E¸PèLhñE¸Pè-0‰EôƒÄƒ}ôu%ÿ”-E¸Phô¸ ƒÀHPèl1ƒÄé8ÿÿÿh…¸ïÿÿPèRh…¸oÿÿPèòQh…¸ßÿÿPèáQh…¸ÏÿÿPèĞQƒÄ h…¸¿ÿÿPè¼QƒÄ‹EHƒø‡‹…Aÿàxphhh¨ô@@888ÄhAh
è©P‹ˆ‹Œ‰•€oÿÿ‰„oÿÿÇ…ˆoÿÿ‹‹”‰•Œoÿÿ‰oÿÿÇ…”oÿÿ‹˜‹œ‰•˜oÿÿ‰œoÿÿ…|oÿÿPjè$‰… oÿÿƒÄƒ½ oÿÿt!ÿ”-h ¸ ƒÀHPè0ƒÄéäıÿÿh¿…¸ïÿÿPè#PhØ…¸ßÿÿPèPhØ…¸ÏÿÿPèPhÙ…¸¿ÿÿPèğOƒÄ éøhh
è±O‹ˆ‹Œ‰•€oÿÿ‰„oÿÿÇ…ˆoÿÿ‹‹”‰•Œoÿÿ‰oÿÿÇ…”oÿÿ‹˜‹œ‰•˜oÿÿ‰œoÿÿ…|oÿÿPjè‡#‰… oÿÿƒÄƒ½ oÿÿt!ÿ”-h ¸ ƒÀHPè /ƒÄéìüÿÿhF…¸ïÿÿPè+OhØ…¸ßÿÿPèOhØ…¸ÏÿÿPè	OhÙ…¸¿ÿÿPèøNƒÄ é‹ˆ‹Œ‰•€oÿÿ‰„oÿÿÇ…ˆoÿÿ‹‹”‰•Œoÿÿ‰oÿÿÇ…”oÿÿ‹˜‹œ‰•˜oÿÿ‰œoÿÿ…|oÿÿPjè"‰… oÿÿƒÄƒ½ oÿÿt$ÿ”-h ¸ ƒÀHPè7.ƒÄéüÿÿhbh
èNh§…¸ïÿÿPè0NhØ…¸ßÿÿPèNhØ…¸ÏÿÿPèNƒÄ hÙ…¸¿ÿÿPèúMƒÄé‹ˆ‹Œ‰•€oÿÿ‰„oÿÿÇ…ˆoÿÿ‹‹”‰•Œoÿÿ‰oÿÿÇ…”oÿÿ‹˜‹œ‰•˜oÿÿ‰œoÿÿ…|oÿÿPjè!‰… oÿÿƒÄƒ½ oÿÿt$ÿ”-h ¸ ƒÀHPè7-ƒÄéûÿÿhÃh
èMh,…¸ïÿÿPè0MhØ…¸ßÿÿPèMhØ…¸ÏÿÿPèMƒÄ hÙ…¸¿ÿÿPèúLƒÄéhVh
è¹LÇ…|oÿÿ‹ˆ‹Œ‰•€oÿÿ‰„oÿÿÇ…ˆoÿÿ‹‹”‰•Œoÿÿ‰oÿÿÇ…”oÿÿ‹˜‹œ‰•˜oÿÿ‰œoÿÿ…|oÿÿPjè… ‰… oÿÿƒÄƒ½ oÿÿt#ÿ”-h ¸ ƒÀHPè,ƒÄéêùÿÿh¤…¸ïÿÿPèÿKhÍ…¸ÏÿÿPèZFhã…¸ÏÿÿPèİKhö…¸ÏÿÿPèÌKƒÄ ÿ5@àh…¸¿ÿÿPèFh/…¸¿ÿÿPè¡KhB…¸¿ÿÿPèKƒÄéÀ
hdh
èyKÇ…|oÿÿ‹ˆ‹Œ‰•€oÿÿ‰„oÿÿÇ…ˆoÿÿ‹‹”‰•Œoÿÿ‰oÿÿÇ…”oÿÿ‹˜‹œ‰•˜oÿÿ‰œoÿÿ…|oÿÿPjèE‰… oÿÿƒÄƒ½ oÿÿt#ÿ”-h ¸ ƒÀHPèŞ*ƒÄéªøÿÿh±…¸ïÿÿPè¿Jhİ…¸ÏÿÿPèÖJhÍ…¸ÏÿÿPèJhã…¸ÏÿÿPèŒJƒÄ hö…¸ÏÿÿPèxJhê…¸¿ÿÿPèJh/…¸¿ÿÿPèVJhB…¸¿ÿÿPèEJƒÄ éu	hh
è-JÇ…|oÿÿ‹ˆ‹Œ‰•€oÿÿ‰„oÿÿÇ…ˆoÿÿ‹‹”‰•Œoÿÿ‰oÿÿÇ…”oÿÿ‹˜‹œ‰•˜oÿÿ‰œoÿÿ…|oÿÿPjèù‰… oÿÿƒÄƒ½ oÿÿt#ÿ”-h ¸ ƒÀHPè’)ƒÄé^÷ÿÿhU…¸ïÿÿPèsIhİ…¸ÏÿÿPèŠIhÍ…¸ÏÿÿPèQIhã…¸ÏÿÿPè@IƒÄ hö…¸ÏÿÿPè,Ihê…¸¿ÿÿPè‡Ch/…¸¿ÿÿPè
IhB…¸¿ÿÿPèùHƒÄ é)h…h
èáHÇ…|oÿÿ‹ˆ‹Œ‰•€oÿÿ‰„oÿÿÇ…ˆoÿÿ‹‹”‰•Œoÿÿ‰oÿÿÇ…”oÿÿ‹˜‹œ‰•˜oÿÿ‰œoÿÿ…|oÿÿPjè­‰… oÿÿƒÄƒ½ oÿÿt#ÿ”-h ¸ ƒÀHPèF(ƒÄéöÿÿhäèşDh…¸ïÿÿPèHhØ…¸ÏÿÿPè4Hh,…¸¿ÿÿPè#HƒÄé+hJh
èáGÇ…|oÿÿ‹ˆ‹Œ‰•€oÿÿ‰„oÿÿÇ…ˆoÿÿ‹‹”‰•Œoÿÿ‰oÿÿÇ…”oÿÿ‹˜‹œ‰•˜oÿÿ‰œoÿÿ…|oÿÿPjè­ƒÄƒ½ oÿÿÿu!ÿ”-h ¸ ƒÀHPèL'ƒÄéõÿÿhäèDh¦…¸ïÿÿPè%Ghİ…¸ÏÿÿPè<Gh,…¸¿ÿÿPè+GƒÄé3hÎh
èéFÇ…|oÿÿ‹ˆ‹Œ‰•€oÿÿ‰„oÿÿÇ…ˆoÿÿ‹‹”‰•Œoÿÿ‰oÿÿÇ…”oÿÿ‹˜‹œ‰•˜oÿÿ‰œoÿÿ…|oÿÿPjèµ‰… oÿÿƒÄƒ½ oÿÿÿu#ÿ”-h ¸ ƒÀHPèN&ƒÄéôÿÿhFèChx…¸ïÿÿPè%FhØ…¸ÏÿÿPè<Fh›…¸¿ÿÿPè+FƒÄé3h»h
èéEÇ…|oÿÿ‹ˆ‹Œ‰•€oÿÿ‰„oÿÿÇ…ˆoÿÿ‹‹”‰•Œoÿÿ‰oÿÿÇ…”oÿÿ‹˜‹œ‰•˜oÿÿ‰œoÿÿ…|oÿÿPjèµ‰… oÿÿƒÄƒ½ oÿÿÿu#ÿ”-h ¸ ƒÀHPèN%ƒÄéóÿÿhFèBh/	…¸ïÿÿPè%Ehİ…¸ÏÿÿPè<Eh›…¸¿ÿÿPè+EƒÄé3hU	h
èéDÇ…|oÿÿ‹ˆ‹Œ‰•€oÿÿ‰„oÿÿÇ…ˆoÿÿ‹‹”‰•Œoÿÿ‰oÿÿÇ…”oÿÿ‹˜‹œ‰•˜oÿÿ‰œoÿÿ…|oÿÿPjèµ‰… oÿÿƒÄƒ½ oÿÿÿu#ÿ”-h ¸ ƒÀHPèN$ƒÄéòÿÿh¤…¸ïÿÿPè/Dhê	…¸ïÿÿPèDhÍ…¸ÏÿÿPèy>h
…¸ÏÿÿPèüCƒÄ hö…¸ÏÿÿPèèCh,
…¸ÏÿÿPè×ChC
…¸ÏÿÿPèÆChZ
…¸ÏÿÿPèµCƒÄ hq
…¸ÏÿÿPè¡Chˆ
…¸ÏÿÿPèChŸ
…¸ÏÿÿPèCh¶
…¸ÏÿÿPènCƒÄ hÍ
…¸ÏÿÿPèZChä
…¸ÏÿÿPèIChê…¸¿ÿÿPè¤=hû
…¸¿ÿÿPè'CƒÄ hB…¸¿ÿÿPèCh…¸¿ÿÿPèCh&…¸¿ÿÿPèñBh=…¸¿ÿÿPèàBƒÄ hT…¸¿ÿÿPèÌBhk…¸¿ÿÿPè»Bh‚…¸¿ÿÿPèªBh™…¸¿ÿÿPè™BƒÄ h°…¸¿ÿÿPè…BhÇ…¸¿ÿÿPètBƒÄé¤hŞh
è]BÇ…|oÿÿ‹ˆ‹Œ‰•€oÿÿ‰„oÿÿÇ…ˆoÿÿ‹‹”‰•Œoÿÿ‰oÿÿÇ…”oÿÿ‹˜‹œ‰•˜oÿÿ‰œoÿÿ…|oÿÿPjè)‰… oÿÿƒÄƒ½ oÿÿÿu#ÿ”-h ¸ ƒÀHPèÂ!ƒÄéïÿÿh±…¸ïÿÿPè£AhØ…¸ßÿÿPèºAèõ‰ÀPèí‰ÀPho…¸ÏÿÿPèİ;ƒÄ hã…¸ÏÿÿPè]Ahö…¸ÏÿÿPèLAh“…¸¿ÿÿPè§;ÿ5@àh…¸¿ÿÿPè;ƒÄ$h/…¸¿ÿÿPèAhB…¸¿ÿÿPèÿ@ƒÄë2hÒh
èé@ÿuhë¸ ƒÀHPè× ƒÄé£îÿÿh
