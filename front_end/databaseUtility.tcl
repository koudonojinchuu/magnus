#------------------------------------------------------------
#
#   Namespace: Debug
#
#------------------------------------------------------------

namespace eval Debug { 

  namespace export enter leave print assert init_log write_log examine_array

  set debug_on 1
  set debug_level 0

  ##
  ##   enter
  ##

  proc enter {args} {
    set caller [info level -1]
    set caller [linsert $caller 1 :]
    if {[llength $args] > 0} {
      variable debug_level 
      set cur_debug [lindex $args 0] 
      if {$cur_debug > $debug_level} {
        force_print "Entering in $caller"
      }
    }
    print "Entering in $caller"
  }

  ##
  ##   leave
  ##

  proc leave {args} {
    set caller [info level -1]
    set caller [linsert $caller 1 :]
    set msg "Leaving $caller"
    if {[llength $args] > 0} {
      set msg "$msg -- $args"
    }  
    print $msg
    return $args
  }

  ##
  ##   print
  ##

  proc print {args} {
    variable debug_on 
    if {$debug_on} {
      puts "$args"
    }
  }

  ##
  ##   assert
  ##

  proc assert {expression} {
    variable debug_on
    if {!$debug_on} {
      return
    }
    global _1_assert_eval
    if { ![catch {set _1_assert_eval}] } {
      puts "Debug::assert: Somebody uses the global variable _1_assert_eval."
      puts "Debug::assert: Could not check the assertion. Return."
      return
    }
    if {![uplevel "global _1_assert_eval ; if \{$expression\} \{set _1_assert_eval 1\} else \{set _1_assert_eval 0\}"]} {
      puts "ASSERT:'$expression' is failed. Stack back trace is:"
      set level [info level] 
      for {set i [expr $level-1]} {$i > 0} {incr i -1} { 
        puts "Level $i: [info level $i]"
      }
    }      
    unset _1_assert_eval
  }

  ##
  ##   force_print
  ##

  proc force_print {args} {
    puts "$args"
  }

  ##
  ##   init_log
  ##

  set logfile(name) ""

  proc init_log {filename {message ""}} {
    varable debug_on
    if {!debug_on} {
      return
    }
    variable logfile
    set logfile(name) $filename
    set logfile(fd)   [::open $filename a+]
    puts $logfile(fd) "Application started on [exec /bin/date]"
    if { $message != "" } {
      puts $logfile(fd) 
    }
  }

  ##
  ##   write_log
  ##

  proc write_log {level message} {
    return
    variable logfile
    print $logfile(fd) "level $level: $message"
  }

  ##
  ##   examine_array
  ##

  proc examine_array {arrname} {
    Debug::enter
    upvar $arrname arr
    set indices [array names arr]
    foreach index $indices {
      print "${arrname}\($index\) = $arr($index)"
    }
    Debug::leave
  }

} ;# end of namespace: Debug  


#----------------------------------------------------------------------
#
# Namespace: Database::Utility
#
#----------------------------------------------------------------------

namespace eval Database::Utility {
  namespace export unset_indices_by_pattern wait_for_dialog
  namespace export trim_cbraces trim_double_cbraces make_spaces
  namespace export file_path file_shortname tmp_filename

  proc unset_indices_by_pattern {arr pattern} {
    upvar $arr a
    set names [array names a]
    foreach name $names { 
      if {[string match "$pattern" $name]} {
        unset a($name)
      }
    }
  }

  # the function trims initial spaces until and including first '{'.
  # Also it trims the last '}' and following spaces until end of line.

  proc trim_cbraces {args} {
    regsub "^ *{" $args "" tmp
    regsub "} *$" $tmp "" args
    return $args
  }

  proc trim_double_cbraces {args} {
    Debug::enter
    regsub "^ *{ *{" $args "" tmp
    regsub "} *} *$" $tmp "" args
    return $args
  }

  proc touch_var {value} {
    variable returnval
    set returnval $value
    #upvar $var v
    #set v $value
  }
  

  proc wait_for_dialog {w varname} {

    #upvar $varname var
    #variable $varname 
    global $varname 

    set oldFocus [focus]
    grab $w

    wm withdraw $w
    update idletasks
    set x [expr [winfo screenwidth $w]/2 - [winfo reqwidth $w]/2 \
	    - [winfo vrootx [winfo parent $w]]]
    set y [expr [winfo screenheight $w]/2 - [winfo reqheight $w]/2 \
	    - [winfo vrooty [winfo parent $w]]]
    wm geom $w +$x+$y
    wm deiconify $w

    tkwait variable $varname
    #tkwait variable var

    if {[winfo exists $oldFocus] } {
      focus $oldFocus
    }

    #Debug::leave $var
    Debug::print "Debug::tkwait_for_dialog = '[expr $$varname]', length = [string length [expr $$varname]]"
    #Debug::leave [expr $$varname]
    return [expr $$varname]
  }

  proc make_spaces {count} {
    set str ""
    for {set i 0} {$i < $count} {incr i} {
      set str "$str "
    }
    return $str
  }
  
  proc file_shortname {filename} {
    #set path [split $filename]
    #return [lindex $path end]
    return [file tail $filename]
  }

  proc file_path {filename} {
    set path [file split $filename]
    set path [lreplace $path end end] 
    return [file join $path ]
  }

  proc tmp_filename {} {
    return tmp[clock clicks]
  }

} ;# end of namespace: Database::Utility


