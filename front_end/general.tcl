##
##   $Id: general.tcl 9 2005-01-23 05:27:26Z daly $
##

## Copyright (C) 1995 The New York Group Theory Cooperative
## See magnus/doc/COPYRIGHT for the full notice.

## Contents: General purpose procedures.
##
## Principal Author: Roger Needham
##
## Status: In progress.
##
## Revision History:
##
## @am 02/98 Added IPCChars { s } function. It converts regular string
## to the IPC format for Chars. 
proc setHeaderTag { } {
    set f [option get .workspace.frame3.log font {}]
    .frame3.log tag configure t -font  $f
    
    if {[regsub -all {(medium)} $f "Bold" value]} {
	.frame3.log tag configure t -font  $value
    } else {
	if {[regsub -all {(Medium)} $f "Bold" value]} {
	    .frame3.log tag configure t -font  $value
	}
    }
    .frame3.log tag configure t -justify center
    
    return t
}
proc printHeader {message} {

    .frame3.log tag configure t -justify center
    
    .frame3.log insert end  $message [ setHeaderTag ]

     puts [get_sessionLogFD] "***$message"
    flush [get_sessionLogFD]
}
proc postToLog {message} {

  global logHoldView

#  .frame3.log insert end "$message\n\n"
	
  logInsert .frame3.log "$message\n\n"

  puts [get_sessionLogFD] "***$message\n"
  flush [get_sessionLogFD]


#  if { $logHoldView(.frame3.log) == 0 } {
#    .frame3.log yview -pickplace end
#  }

  postToMinilog $message
}


proc theTime {} { return [exec date] }


# Manipulating lists:

proc identity_permutation {n} {
  set p [list]
  set i 0
  while { $i < $n } {
    lappend p $i
    incr i
  }
  return $p
}


proc transpose {listName i j} {
  # Assumes $j > $i
  upvar $listName L
  set L [linsert $L $i [lindex $L $j]]
  incr j
  set L [lreplace $L $j $j]
}


proc permute {listName p} {
  upvar $listName L
  set result [list]
  foreach i $p {
    lappend result [lindex $L $i]
  }
  set L $result
}


proc pop {listName} {
  upvar $listName L
  set temp [lindex $L 0]
  set L [lreplace $L 0 0]
  return $temp
}



## Geometry management stuff:

proc path_append {base ext} {
  if {$base == "."} {
    return .$ext
  } else {
    return $base.$ext
  }
}


proc framer {parent master side items} {

  set j 0
  set len [llength $items]
  while {$j < $len} {

    set what [lindex $items $j]

    if {$what != "top" && $what != "left" && $what != "bottom" && \
        $what != "right"} {
      set w [path_append $parent $what]
      eval pack $w [lindex $items [expr $j + 1]] -side $side -in $master
      raise $w
      incr j 2
    } else {
      set i 1
      while {[winfo exists [path_append $parent f$i]]} {incr i}
      set f [path_append $parent f$i]
      eval frame $f [lindex $items [expr $j + 1]]
      eval pack $f [lindex $items [expr $j + 2]] -side $side -in $master
      framer $parent $f $what [lindex $items [expr $j + 3]]
      incr j 4
    }
  }
}


proc slaves {w {prefix ""}} {
  foreach s [pack slaves $w] {
    puts $prefix$s
    slaves $s "$prefix  "
  }
}

proc IPCChars { s } {

    set retS "[string length $s] "

    set i 0

    while { $i < [string length $s] } {
	set ch [string index $s $i]
	if { [string compare $ch "\n"] == 0 || [string compare $ch " "] == 0} {
	    set retS $retS\377
	} else {
	    set retS $retS$ch
	}
	set i [expr $i + 1]
    }
    set retS "$retS "
    return $retS
}
