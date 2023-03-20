set ns [new Simulator]
$ns rtproto LS


set node1 [$ns node]
set node2 [$ns node]
set node3 [$ns node]
set node4 [$ns node]
set node5 [$ns node]
set node6 [$ns node]


set tf [open out.tr w]
$ns trace-all $tf
set nf [open out.nam w]
$ns namtrace-all $nf


$node1 label "node 1"
$node2 label "node 2"
$node3 label "node 3"
$node4 label "node 4"
$node5 label "node 5"
$node6 label "node 6"
$node1 label-color blue
$node2 label-color red
$node5 label-color blue
$node6 label-color red


$ns duplex-link $node1 $node3 1.5Mb 10ms DropTail
$ns duplex-link $node2 $node3 1.5Mb 10ms DropTail
$ns duplex-link $node3 $node4 0.05Mb 10ms DropTail
$ns duplex-link $node4 $node5 1.5Mb 10ms DropTail
$ns duplex-link $node4 $node6 1.5Mb 10ms DropTail

$ns queue-limit $node3 $node4 4


$ns duplex-link-op $node1 $node3 orient right-down
$ns duplex-link-op $node2 $node3 orient right-up
$ns duplex-link-op $node3 $node4 orient right
$ns duplex-link-op $node4 $node5 orient right-down
$ns duplex-link-op $node4 $node6 orient right-up


set tcp2 [new Agent/TCP]
$ns attach-agent $node1 $tcp2
set sink2 [new Agent/TCPSink]
$ns attach-agent $node5 $sink2
$ns connect $tcp2 $sink2

set udp3 [new Agent/UDP]
$ns attach-agent $node2 $udp3
set sink3 [new Agent/Null]
$ns attach-agent $node6 $sink3
$ns connect $udp3 $sink3


set traffic_ftp2 [new Application/FTP]
$traffic_ftp2 attach-agent $tcp2

set traffic_cbr3 [new Application/Traffic/CBR]
$traffic_cbr3 attach-agent $udp3


proc finish {} {
global ns tf nf
$ns flush-trace
close $tf
close $nf
exec nam out.nam &
exit 0

}

$ns at 1.0 "$traffic_ftp2 start"
$ns at 2.0 "$traffic_ftp2 stop"
$ns at 3.0 "$traffic_cbr3 start"
$ns at 4.0 "$traffic_cbr3 stop"
$ns at 5.0 "finish"
$ns run
