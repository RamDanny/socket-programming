set ns [new Simulator]
$ns rtproto LS


set node0 [$ns node]
set node1 [$ns node]
set node2 [$ns node]
set node3 [$ns node]
set node4 [$ns node]
set node5 [$ns node]
set node6 [$ns node]
set node7 [$ns node]


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
$node6 label "node 6"
$node7 label "node 7"
$node0 label-color blue
$node7 label-color red
$node4 label-color green


$ns duplex-link $node0 $node1 1Mb 10ms DropTail
$ns duplex-link $node1 $node2 1Mb 10ms DropTail
$ns duplex-link $node2 $node3 1Mb 10ms DropTail
$ns duplex-link $node3 $node4 1Mb 10ms DropTail
$ns duplex-link $node0 $node7 0.5Mb 10ms DropTail
$ns duplex-link $node7 $node6 0.5Mb 10ms DropTail
$ns duplex-link $node6 $node5 0.5Mb 10ms DropTail
$ns duplex-link $node5 $node4 0.5Mb 10ms DropTail

$ns queue-limit $node3 $node4 5


$ns duplex-link-op $node0 $node1 orient down-left
$ns duplex-link-op $node1 $node2 orient down
$ns duplex-link-op $node2 $node3 orient down
$ns duplex-link-op $node3 $node4 orient down-right
$ns duplex-link-op $node0 $node7 orient right-down
$ns duplex-link-op $node7 $node6 orient down
$ns duplex-link-op $node6 $node5 orient down
$ns duplex-link-op $node5 $node4 orient down-left


set udp2 [new Agent/UDP]
$ns attach-agent $node0 $udp2
set sink2 [new Agent/Null]
$ns attach-agent $node4 $sink2
$ns connect $udp2 $sink2

set udp3 [new Agent/UDP]
$ns attach-agent $node7 $udp3
set sink3 [new Agent/Null]
$ns attach-agent $node4 $sink3
$ns connect $udp3 $sink3


set traffic_cbr2 [new Application/Traffic/CBR]
$traffic_cbr2 attach-agent $udp2

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

$ns at 1.0 "$traffic_cbr2 start"
$ns at 2.0 "$traffic_cbr3 start"
$ns at 4.0 "$traffic_cbr3 stop"
$ns at 4.0 "$traffic_cbr2 stop"
$ns rtmodel-at 2.0 down $node2 $node3
$ns rtmodel-at 2.0 down $node3 $node4
$ns rtmodel-at 3.0 up $node2 $node3
$ns rtmodel-at 3.0 up $node3 $node4
$ns at 5.0 "finish"
$ns run
