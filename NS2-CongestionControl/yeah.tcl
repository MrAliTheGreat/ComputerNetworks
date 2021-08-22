Class TraceApp -superclass Application     
 
TraceApp instproc init {args} {
        $self set bytes_ 0
        eval $self next $args
}

TraceApp instproc recv {byte} {
        $self instvar bytes_
        set bytes_ [expr $bytes_ + $byte]
        return $bytes_
}


Agent/Ping instproc init {args} {
        $self set rtt_ 0
        eval $self next $args
}

Agent/Ping instproc recv {from rtt} {
        $self instvar node_
        $self instvar rtt_

        # puts "node [$node_ id] received ping answer from $from with RTT of $rtt ms."
        set rtt_ $rtt
        return $rtt_

}

###################################################################

set ns [new Simulator]

$ns color 1 Blue
$ns color 2 Red

set nf [open out.nam w]
$ns namtrace-all $nf
set traceFile1 [open ./yeah/allTraces.tr w]
$ns trace-all $traceFile1

# cwnd data
set wf1 [open ./yeah/cwnd_1.tr w]
set wf2 [open ./yeah/cwnd_2.tr w]

# goodput data
set gf1 [open ./yeah/goodput_1.tr w]
set gf2 [open ./yeah/goodput_2.tr w]

# # lost data
# set lr1 [open ./yeah/lost_1.tr w]
# set lr2 [open ./yeah/lost_2.tr w]

# # RTT data
# set rttR1 [open ./yeah/rtt_1.tr w]
# set rttR2 [open ./yeah/rtt_2.tr w]



proc finish {} {
   global ns nf
   $ns flush-trace
   close $nf
   
   exec xgraph ./yeah/cwnd_1.tr ./yeah/cwnd_2.tr -geometry 800x400 &
   exec xgraph ./yeah/goodput_1.tr ./yeah/goodput_2.tr -geometry 800x400 &
   # exec xgraph ./yeah/rtt_1.tr ./yeah/rtt_2.tr -geometry 800x400 &
   # exec xgraph ./yeah/lost_1.tr ./yeah/lost_2.tr -geometry 800x400 &
   exec nam out.nam &
   exit 0
}

set node_(s1) [$ns node]
set node_(s2) [$ns node]
set node_(r1) [$ns node]
set node_(r2) [$ns node]
set node_(s3) [$ns node]
set node_(s4) [$ns node]

# $ns duplex-link $node_(s1) $node_(r1) 10Mb 2ms DropTail 
# $ns duplex-link $node_(s2) $node_(r1) 10Mb 3ms DropTail 
# $ns duplex-link $node_(r1) $node_(r2) 1.5Mb 20ms DropTail 
# $ns queue-limit $node_(r1) $node_(r2) 25
# $ns queue-limit $node_(r2) $node_(r1) 25
# $ns duplex-link $node_(s3) $node_(r2) 10Mb 4ms DropTail 
# $ns duplex-link $node_(s4) $node_(r2) 10Mb 5ms DropTail 

$ns duplex-link $node_(s1) $node_(r1) 4000Mb 500ms DropTail 
$ns duplex-link $node_(s2) $node_(r1) 4000Mb 800ms DropTail 
$ns duplex-link $node_(r1) $node_(r2) 1000Mb 50ms DropTail 
$ns queue-limit $node_(r1) $node_(r2) 10
$ns queue-limit $node_(r2) $node_(r1) 10
$ns duplex-link $node_(s3) $node_(r2) 4000Mb 500ms DropTail 
$ns duplex-link $node_(s4) $node_(r2) 4000Mb 800ms DropTail 

# This part is for placing everything correctly in Nam
$ns duplex-link-op $node_(s1) $node_(r1) orient right-down
$ns duplex-link-op $node_(s2) $node_(r1) orient right-up
$ns duplex-link-op $node_(r1) $node_(r2) orient right
$ns duplex-link-op $node_(r1) $node_(r2) queuePos 0
$ns duplex-link-op $node_(r2) $node_(r1) queuePos 0
$ns duplex-link-op $node_(s3) $node_(r2) orient left-down
$ns duplex-link-op $node_(s4) $node_(r2) orient left-up



set tcp1 [new Agent/TCP/Linux]
$tcp1 set class_ 0
# $tcp1 set window_ 15
$tcp1 set window_ 8000
$tcp1 set packetSize_ 1000
$tcp1 set ttl_ 64
$ns at 0.0 "$tcp1 select_ca yeah"
$ns attach-agent $node_(s1) $tcp1

set sink1 [new Agent/TCPSink/Sack1]
set traceApp1 [new TraceApp]
$traceApp1 attach-agent $sink1
$sink1 set class_ 0
$sink1 set ts_echo_rfc1323_ true
$ns attach-agent $node_(s3) $sink1

$ns connect $tcp1 $sink1

set ftp1 [new Application/FTP]
$ftp1 attach-agent $tcp1
$ftp1 set type_ FTP

$tcp1 set fid_ 1


set tcp2 [new Agent/TCP/Linux]
$tcp2 set class_ 1
# $tcp2 set window_ 15
$tcp2 set window_ 8000
$tcp2 set packetSize_ 1000
$tcp2 set ttl_ 64
$ns at 0.0 "$tcp2 select_ca yeah"
$ns attach-agent $node_(s2) $tcp2

set sink2 [new Agent/TCPSink/Sack1]
set traceApp2 [new TraceApp]
$traceApp1 attach-agent $sink2
$sink2 set class_ 1
$sink2 set ts_echo_rfc1323_ true
$ns attach-agent $node_(s4) $sink2

$ns connect $tcp2 $sink2

set ftp2 [new Application/FTP]
$ftp2 attach-agent $tcp2
$ftp2 set type_ FTP

$tcp2 set fid_ 2


$tcp1 attach $traceFile1
$tcp1 tracevar rtt_

$tcp2 attach $traceFile1
$tcp2 tracevar rtt_


# set p_s1 [new Agent/Ping]
# $ns attach-agent $node_(s1) $p_s1

# set p_f1 [new Agent/Ping]
# $ns attach-agent $node_(s3) $p_f1

# $ns connect $p_s1 $p_f1

# set p_s2 [new Agent/Ping]
# $ns attach-agent $node_(s2) $p_s2

# set p_f2 [new Agent/Ping]
# $ns attach-agent $node_(s4) $p_f2

# $ns connect $p_s2 $p_f2


$ns at 0.1 "$ftp1 start"
$ns at 0.1 "$ftp2 start"


proc plotWindow {tcpSource1 tcpSource2 file1 file2} {
   global ns

   set time 0.1
   set now [$ns now]
   set cwnd1 [$tcpSource1 set cwnd_]
   set cwnd2 [$tcpSource2 set cwnd_]

   puts $file1 "$now $cwnd1"
   puts $file2 "$now $cwnd2"
   $ns at [expr $now+$time] "plotWindow $tcpSource1 $tcpSource2 $file1 $file2" 
}

proc plotGoodput {tcpSink1 tcpSink2 fileGF1 fileGF2} {
   global ns

   set now [$ns now]

   # Read number of bytes
   set nbytes1 [$tcpSink1 set bytes_]
   set nbytes2 [$tcpSink2 set bytes_] 

   # Reset for next epoch
   $tcpSink1 set bytes_ 0
   $tcpSink2 set bytes_ 0
   set time_incr 1.0

   # Prints "TIME Goodput" in Mb/sec units to output file
   set goodput1 [expr ($nbytes1 * 8.0 / 1000000) / $time_incr]
   set goodput2 [expr ($nbytes2 * 8.0 / 1000000) / $time_incr]
   puts $fileGF1 "$now $goodput1"
   puts $fileGF2 "$now $goodput2"


   $ns at [expr $now+$time_incr] "plotGoodput $tcpSink1 $tcpSink2 $fileGF1 $fileGF2"
}


# proc plotRTT {ping1_s ping1_f ping2_s ping2_f fileRTT1 fileRTT2} {
#    global ns

#    set time 0.1
#    set now [$ns now]

#    $ns at $now "$ping1_s send"
#    set startTime1 [$ping1_s set rtt_]

#    $ns at $now "$ping1_f send"
#    set endTime1 [$ping1_f set rtt_]

#    set rttTime1 [expr $startTime1 + $endTime1]
#    puts $fileRTT1 "$now $rttTime1"


#    $ns at $now "$ping2_s send"
#    set startTime2 [$ping2_s set rtt_]

#    $ns at $now "$ping2_f send"
#    set endTime2 [$ping2_f set rtt_]

#    set rttTime2 [expr $startTime2 + $endTime2]
#    puts $fileRTT2 "$now $rttTime2"

#    $ns at [expr $now+$time] "plotRTT $ping1_s $ping1_f $ping2_s $ping2_f $fileRTT1 $fileRTT2" 
# }


# setup plotting
$ns at 0.1 "plotWindow $tcp1 $tcp2 $wf1 $wf2"
$ns at 0.1 "plotGoodput $sink1 $sink2 $gf1 $gf2"
# $ns at 0.1 "plotRTT $p_s1 $p_f1 $p_s2 $p_f2 $rttR1 $rttR2"

# $ns at 10 "finish"
$ns at 1000 "finish"

$ns run
