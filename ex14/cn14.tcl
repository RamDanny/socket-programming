set ns [new Simulator]
#$ns rtproto DV


set node0 [$ns node]
set node1 [$ns node]
set node2 [$ns node]
set node3 [$ns node]
set node4 [$ns node]
set node5 [$ns node]


set tf [open out.tr w]
$ns trace-all $tf
set nf [open out.nam w]
$ns namtrace-all $nf


$node0 label "node 0"
$node1 label "node 1"
$node2 label "node 2"
$node3 label "node 3"
$node4 label "node 4"
$node5 label "node 5"
$node0 label-color blue
$node4 label-color blue
$node1 label-color green
$node5 label-color green


$ns duplex-link $node0 $node2 1Mb 70ms DropTail
$ns duplex-link $node1 $node2 1Mb 70ms DropTail
$ns duplex-link $node2 $node3 1Mb 70ms DropTail
$ns duplex-link $node3 $node4 1Mb 70ms DropTail
$ns duplex-link $node3 $node5 1Mb 70ms DropTail

#$ns duplex-link-op $node2 $node3 queuePos 0.5

$ns queue-limit $node2 $node3 10


$ns duplex-link-op $node0 $node2 orient right-down
$ns duplex-link-op $node1 $node2 orient right-up
$ns duplex-link-op $node2 $node3 orient right
$ns duplex-link-op $node3 $node4 orient right-up
$ns duplex-link-op $node3 $node5 orient right-down


set tcp2 [new Agent/TCP/Reno]
$ns attach-agent $node0 $tcp2
set sink2 [new Agent/TCPSink]
$ns attach-agent $node4 $sink2
$ns connect $tcp2 $sink2

set tcp3 [new Agent/TCP]
$ns attach-agent $node1 $tcp3
set sink3 [new Agent/TCPSink]
$ns attach-agent $node5 $sink3
$ns connect $tcp3 $sink3

set traffic_ftp2 [new Application/FTP]
$traffic_ftp2 attach-agent $tcp2

set traffic_ftp3 [new Application/FTP]
$traffic_ftp3 attach-agent $tcp3

proc finish {} {

    global ns nf
    $ns flush-trace
    close $nf
    exec nam out.nam &
    #exec awk -f cn14.awk out.tr > output.tr
    exec ./xgraph reno.xg &
    exec ./xgraph tahoe.xg &
    exit 0

}

$ns at 0.1 "$traffic_ftp2 start"
$ns at 0.1 "$traffic_ftp3 start"
$ns at 4.0 "$traffic_ftp2 stop"
$ns at 4.0 "$traffic_ftp3 stop"
$ns at 5.0 "finish"

proc plotWindow {tcpSource outfile} {
    global ns
    set now [$ns now]
    set cwnd [$tcpSource set cwnd_]
    puts $outfile "$now $cwnd"
    $ns at [expr $now+0.1] "plotWindow $tcpSource $outfile"
}

set outfile [open "reno.xg" w]
$ns at 0.0 "plotWindow $tcp2 $outfile"
set outfile [open "tahoe.xg" w]
$ns at 0.0 "plotWindow $tcp3 $outfile"
$ns run
