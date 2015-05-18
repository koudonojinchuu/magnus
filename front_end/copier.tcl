#!/usr/bin/tclsh

set executable [lindex $argv 0]
set from_std_name [lindex $argv 1]
set from_pipe_name [lindex $argv 2]

set pipe [open "| $executable" r+]
set timestamp [clock format [clock seconds] -format "%Y%m%d%H%M%S"]
set from_std [open "$from_std_name.$timestamp" w]
set from_pipe [open "$from_pipe_name.$timestamp" w]
fconfigure $pipe -blocking 0
fconfigure stdin -blocking 0

set counter 0

proc ReadStream {stream out1 out2} {
	if {![eof $stream]} {
		set input [read $stream]
		puts -nonewline $out1 $input
		puts -nonewline $out2 $input
		flush $out1
		flush $out2
	} else {
		global counter
		incr counter
	}
}

fileevent stdin readable [list ReadStream stdin $from_std $pipe]
fileevent $pipe readable [list ReadStream $pipe $from_pipe stdout]

vwait counter
while [expr "$counter < 2"] {
	global counter
	vwait counter
}

flush $from_std
flush $from_pipe
flush $pipe
flush stdout
close $from_std
close $from_pipe
close $pipe

puts stderr "copier.tcl: exiting"
flush stderr
