#!/usr/bin/wish -f
##
##   $Id: magnus.in 47 2005-06-24 17:54:56Z jzniu $
##

## Copyright (C) 1995 The New York Group Theory Cooperative
## See magnus/doc/COPYRIGHT for the full notice.

## Contents: The end user executable.
##
## Principal Author: Roger Needham
##
## Status: In progress.
##
## Revision History:
##

               
set release_info "Version 4.1.3 beta\nRelease date: June 24th,2005\n"

set magnus_home "/home/users/yegor/magnus"
set tcl_path $magnus_home/front_end
set executable "$magnus_home/back_end/SessionManager/test/bin/magnus"
set tmp_dir "/tmp"
set saveTmp $tmp_dir/MagnusTmp[pid]
set tmp_dir $saveTmp
set home_dir $env(HOME)

set other_executables "\
$magnus_home/back_end/black_boxes/rkbp/bin/rkbp \
$magnus_home/back_end/black_boxes/homology/bin/chom \
$magnus_home/back_end/black_boxes/orwp/orwp \
$magnus_home/back_end/black_boxes/tc5/tc \
"
#@rn This leaves out the kbmag stuff, since it's so damn scattered.


set preamble \
"$release_info
Copyright (C) 1994-2005
The New York Group Theory Cooperative.\n\n"

proc makeScripts {suffix_name} {
global home_dir
set script_dir $home_dir/.magnus
if { ![file exists $script_dir] } {
    exec mkdir $script_dir
}
	
    set name "$script_dir/$suffix_name"
    set scriptlist [glob -nocomplain $script_dir/$suffix_name*]
    set i 0
    set suffix 1
    set sav_suffix 0
    if { [llength $scriptlist]>0} {
	while {$i<[llength $scriptlist]} {
	    set a [lindex $scriptlist $i]
	    if { [scan $a $script_dir/$suffix_name%d sav_suffix]!=0 }  {
	            lappend suffixlist $sav_suffix
            }
	    incr i
	}
	
	set suffixlist [lsort -integer $suffixlist  ]
	set i 0
	while {$i<[llength $suffixlist]-9} {
	    exec rm -f $script_dir/$suffix_name[lindex $suffixlist $i]
	    incr i
	}
	set lastsuffix [lindex $suffixlist [expr [llength $suffixlist]-1] ]
	set suffix [expr $lastsuffix+1]
        unset lastsuffix suffixlist
    }	
  return $name$suffix
}

proc fatalError {message} {
  global tcl_path sessionManager
  global tmp_dir
  global sav_tmp_dir
  if { [file exists "$tcl_path/bitmaps/bomb.xbm"] } {
    tk_dialog .d {Fatal Error} "$message\nAborting." \
      @$tcl_path/bitmaps/bomb.xbm 0 OK
  } else {
    tk_dialog .d {Fatal Error} "$message\nAborting." error 0 OK
  }

  # Kill session manager if started
  if {[info exists sessionManager]} {
    catch { exec kill -9 [pid $sessionManager] }
  }
  exec rm -rf $tmp_dir
  exit 1
}

#@rn Just for now:

proc fakeQuit {} {
  global sessionManager
  global tmp_dir
  global sav_tmp_dir

  # Kill session manager if started
  if {[info exists sessionManager]} {
    catch { exec kill -9 [pid $sessionManager] }
  }
  exec rm -rf $tmp_dir
  exit 0
}


# Make sure the addinput patch is installed

#@db porting to tk4.0: add check a version of tk.

#if { [info commands addinput] == "" } { }
if { $tk_version == "3.6" && [info commands addinput] == "" } {
  set msg \
"Magnus cannot function without the addinput patch to Tk 3.6.
\nPlease ask your system administrator to read magnus/INSTALL.
\nThere is no need to recompile Magnus.
"

  puts stderr $msg
  flush stderr
  fatalError $msg
}


foreach name {global general options messages greeting workspace dialogs \
              helpStrings views updateMenus editing fileDialogs dialog \
              help nodeGraph files viewParameters tcltk-fixes testing\
              databaseMain databaseParser databaseBrowser databaseMessages \
              databaseUtility databaseHtmlConverter} {
  if { ![file exists "$tcl_path/$name.tcl"] } {
    fatalError "I Couldn't find the file `$name.tcl' in $tcl_path."
  }
  source "$tcl_path/$name.tcl"
}

# Make transcript by default

set_makeTranscript 1


# Check Tcl and Tk versions

#@db porting to tk4.0
#if { [info tclversion] != 7.3 } { }
if { [info tclversion] < 7.4 } { 

#@db porting to tk4.0
#  set msg \
#"Your version of Tcl is [info tclversion], not 7.3.
#It may not work with Magnus.
#"
  set msg \
"Your version of Tcl is [info tclversion], less then  7.4.
It may not work with Magnus.
"
  puts stderr $msg
  flush stderr
  alert $msg
}

#@db porting to tk4.0
#if { $tk_version != 3.6 } { }
if { $tk_version < 4.0 } {

#@db porting to tk4.0
#  set msg \
#"Your version of Tk is $tk_version, not 3.6.
#It may not work with Magnus.
#"
  set msg \
"Your version of Tk is $tk_version, less then 4.0.
It may not work with Magnus.
"
  puts stderr $msg
  flush stderr
  alert $msg
}


# Check machine configuration; looking for string like
# "SunOS rio 4.1.3_U1 2 sun4m sparc sun4"

set uname_str ""
catch { set uname_str "[exec uname -a] [exec mach] [exec arch]" }
# if { [string first SunOS $uname_str] == -1 || [string first "4.1" $uname_str] == -1 } {
# 
#   set msg \
# "I cannot verify that you are running SunOS 4.1.
# Magnus is likely to fail on your system.
# "
#   puts stderr $msg
#   flush stderr
#   alert $msg
# }
# 
# 
# if { [string first sparc $uname_str] == -1 } {
# 
#   set msg \
# "I cannot verify that your machine is a SPARCStation.
# Magnus will almost certainly fail.
# "
#   puts stderr $msg
#   flush stderr
#   alert $msg
# }


setDefaultOptions
makeGreeting
makeWorkspace


if { [file exists "$tcl_path/bitmaps/magn6.xbm"] } {
  wm iconbitmap . @$tcl_path/bitmaps/magn6.xbm
}
wm iconposition . 20 20

# if { [file exists "$tcl_path/bitmaps/workspace.xbm"] } {
#   wm iconbitmap .workspace @$tcl_path/bitmaps/workspace.xbm
# }
wm iconposition . -20 20


set who ""
catch { set who \
"Magnus started on [theTime]
by [exec whoami] at [exec hostname]." }

# Make transcript file when not debugging
set script_dir $env(HOME)/.magnus
if { ![file exists $script_dir] } {
    exec mkdir $script_dir
}
	
if {[makeTranscript] == 1} {
  set_transcriptName [makeScripts Magnus_Transcript]
  set_transcriptFD [open [get_transcriptName] w]

  # Put release info in transcript  

  puts [get_transcriptFD] "$release_info\n$who\n$uname_str"
  flush [get_transcriptFD]

  set_sessionLogFD [open [makeScripts SessionLog] w]
}


# Check existence of other executables

foreach name $other_executables {
  if { ![file exists $name] } {

    alert \
"I Couldn't find the file
`$name'
Please check that you have search permission along this path.\n
The functionality of Magnus will be impaired without it.
"

  } elseif { ![file executable $name] } {

    alert \
"The file
`$name'
is not executable by you.
The functionality of Magnus will be impaired unless you do:
`chmod ug+x $name'
"

  }
}
unset other_executables

# Make tmp directory


set i 0
while { [file exists $tmp_dir] } {
    puts "Temporary directory '$tmp_dir' was not deleted by previos sessions of Magnus."
    puts "You can try to delete it, or ask your system adminstrator."
    set tmp_dir $saveTmp.$i
    set i $i+1
}
exec mkdir $tmp_dir
unset saveTmp
unset i

# Open pipes to the session manager

if {[file executable $executable]} {
  if {[makeTranscript] == 1} {
    #set sessionManager [open "| $executable $magnus_home $tmp_dir 2>/dev/null" r+]
	 set sessionManager [open "| ./copier.tcl \"$executable $magnus_home $tmp_dir\" from-frontend.log from-backend.log 2>session.log" r+]
  } else {
    #set sessionManager [open "| $executable $magnus_home $tmp_dir 2>/dev/tty" r+]
	 set sessionManager [open "| ./copier.tcl \"$executable $magnus_home $tmp_dir\" from-frontend.log from-backend.log 2>/dev/tty" r+]
  }

#@db porting to tk4.0
#  addinput $sessionManager "fromSessionManager %E %F"
  fileevent $sessionManager readable "fromSessionManager"
} else {
  fatalError "I Couldn't find an executable `$executable'."
}


#@rn This ensures that a toplevel cannot be made smaller than the size
#@rn it wants, but doesn't work when the font size changes:

#@db We don't actually want it !
#
#bind Toplevel <Map> {
#  if {[regexp {([0-9]+)x([0-9]+)} [wm geometry %W] d w h]} {
#    wm minsize %W $w $h
#  }
#}
printHeader "Magnus\n\n"
printHeader $preamble
unset preamble

postToLog $who
postToLog "Please make explicit reference to Magnus in any
of your papers that has benefited from it's use."
unset who


# Set up timeout for session manager

after 15000 checkSessionManagerStarted
after 60000 checkBootDone


proc checkSessionManagerStarted {} {

  global handshakeComplete

  if { ! $handshakeComplete } {
    alert "The Session Manager has not started after 15 seconds.
You can wait longer, but there is probably something wrong."
  }
}


proc checkBootDone {} {

  global bootstrapComplete

  if { ! $bootstrapComplete } {
    alert "The Session Manager has not completed bootstrapping after 1 minute.
You can wait longer, but there is probably something wrong, unless your machine is very slow."
  }
}
