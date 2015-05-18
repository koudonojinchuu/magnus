##
##   $Id $
##

## Copyright (C) 2000 The New York Group Theory Cooperative
## See magnus/doc/COPYRIGHT for the full notice.

## Contents: Automatic testing system and the system for sending comments 
##           to programmers
##
## Principal Author: Dmitry Bormotov
##
## Status: In progress.
##
## Revision History:
##


proc deleteOIDFromList { oid } {
# used by submitChanges

    global outList

    set lastCreate ""
    set tmp ""

    foreach s $outList {

	if [regexp {^-2.+} $s] {
	    set lastCreate $s
	    continue
	}

	if [regexp {^(-1 2 ([0-9]+) .+)} $s a command o] {
	    if {$oid != $o} {
		lappend tmp $lastCreate $command
	    } else { 
		continue 
	    } 
	}
	
	if [regexp {^(([0-9]+).+)} $s a command o] {
	    if {$oid != $o} {
		lappend tmp $command
	    } else { 
		continue 
	    } 
	}
    }

    set outList $tmp
}

#outList - list

proc submitChanges { FN w } {

# invoked when user submits highlighted objects to the programmer;
# goes through the transcript, selects commands related to the objects and 
# their dependencies, saves these commands to a file. 

# $w is the dialog widget which was OK'd

    global workspace_selection changesFN testsFN outList

    if { $FN == "tests" } {
	set testsFN [$w.frame1.entry get]
    } else {
	set changesFN [$w.frame1.entry get]
    }

    set icons ""
    set deps $workspace_selection

    while { [llength $deps] > 0 } {

	set dep [lindex $deps 0]
	set deps [lrange $deps 1 end]

	foreach object [get_objectDependencies $dep] {
	    if {[lsearch $deps $object] < 0} {
		lappend deps $object
	    }
	}

	if {[lsearch $icons $dep] < 0} {
	    lappend icons $dep
	}
    }

    set oids ""
    foreach object $icons {
       #postToLog [get_objectID $object]
       lappend oids [get_objectID $object]
    }

    
    set in [open [get_transcriptName] r]
    set fn ""
    if { $FN == "tests" } {
	set fn "~/.magnus/$testsFN"
    } else {
	set fn "~/.magnus/$changesFN"
    }
    set out [open $fn a+]
    
    set lastCreate ""
    set outList ""
    set usedOIDs ""

    while { [gets $in s] >= 0 } {

	if [regexp {^>\([0-9]+\) +(-2.+)} $s a c] {
	    set lastCreate $c
	}

	if [regexp {^<\([0-9]+\) +create_ objectID ([0-9]+).+} $s a oid] {
	    if {[lsearch $oids $oid] >= 0} {
		if {[lsearch $usedOIDs $oid] != -1} {
		    deleteOIDFromList $oid
		}
		lappend outList $lastCreate
		lappend usedOIDs $oid
		#puts $out $lastCreate
	    }  
	}

	if [regexp {^>\([0-9]+\) +(-1 2 ([0-9]+) .+)} $s a command oid] {
	    if {[lsearch $oids $oid] >= 0} {
		lappend outList $command
		#puts $out $command
	    }  
	}
	
	if [regexp {^>\([0-9]+\) +(([0-9]+).+)} $s a command oid] {
	    if {[lsearch $oids $oid] >= 0} {
		lappend outList $command
		#puts $out $command
	    }  
	}
    }

    close $in 

    foreach line $outList { 
	puts $out $line 
    }

    set comments [$w.frame2.frame.text get 1.0 end-1char]
    puts $out "#comments_begin"
    puts $out $comments
    puts $out "#comments_end"
    puts $out ""
    puts $out [clock format [clock seconds] -format "%y/%m/%d %T"]
    puts $out "----------" 
    puts $out ""
    close $out
}



set playFileID -1
#OIDMap - array
#nameToOID - array

proc playChanges { FN } {

# replays commands saved by proc saveChanges
    
    global playFileID playingMode changesFN testsFN

    if { $FN == "tests" } {
	set fn "~/.magnus/$testsFN"
    } else {
	set fn "~/.magnus/$changesFN"
    }
    set playFileID [open $fn r]
    set playingMode 1
    playCommand

    #save_pendingCheckin F2 "" "< a >" "" ""
    #toSessionManager "-2 2 F2 < a  >"
}

proc playCommand { {message ""} } {

# called by proc fromSessionManager each time new command has arrived;
# allows more control in the replaying(fixing) mode.

    global playFileID OIDMap nameToOID playingMode

    #postToLog "Got new message: $message"

    if [regexp {^-1 2 ([0-9]+) (.+)} $message a oid name] {
	set nameToOID($name) $oid
	#postToLog "New nameToOID: $name -> $nameToOID($name)"
    } elseif { $message != "" } {
	return
    }

    while { [gets $playFileID s] >= 0 } {
	
	set a \377
	if [regexp $a $s] {
	    #postToLog "Strange message: $s"
	    continue
	}

	if [regexp # $s] {
	    break;
	}

	if [regexp {^-2} $s a] {

	    regexp {^(-2 [0-9]+)(( [0-9]+)*)( [a-zA-Z]+.*)*$} $s a begin \
		    list c end

	    if { $end != "" } {
		regexp {^ ([^ ]*) (.*)$} $end a name defn
		save_pendingCheckin $name "" $defn "" ""
	    }

	    if { $list != "" } {
		set s $begin
		foreach oid $list {
		    set s "$s $OIDMap($oid)"
		}
		set s $s$end
	    } 
	    
	    toSessionManager $s
	    return 

	} elseif [regexp {^-1 2 ([0-9]+) (.+)} $s a oid name] {
	    
	    set OIDMap($oid) $nameToOID($name)
	    #postToLog "New OID map: $oid -> $OIDMap($oid)"
	    continue

	} elseif { ![regexp {[-]} $s] } {
	    if [regexp {^([0-9]+)(.+)} $s a oid end] {
		set s $OIDMap($oid)$end
	    }
	}


#	if [regexp {^([0-9]+) 2$} $s a OID] {
#
#	    set icon [get_objectIcon $OID]
#	    set viewID [get_viewID $icon]
#	    if {[viewExists $viewID]} {
#		set w [get_viewPath $viewID]
#		$w.frame3.start configure -state disabled
#	    }
#	}

	toSessionManager $s
    }
    close $playFileID
    catch { unset nameToOID OIDMap }
    after 2000 {set playingMode 0}
}

proc showCommentDialog { fn } {

    global changesFN testsFN

    #Make filename field

    toplevel .a
    set w .a  
    set name ""

    if { $fn == "tests" } {
	set name $testsFN
	wm title $w "Add problem to the testing database"
    } else {
	set name $changesFN
	wm title $w "Submit changes to programmers"
    }

    frame $w.frame1 -relief raised -bd 2
    label $w.frame1.label -text "Filename"
    entry $w.frame1.entry -relief sunken
    $w.frame1.entry insert 0 $name
    pack $w.frame1.label -side left
    pack $w.frame1.entry -fill x -padx 2m -pady 2m
    pack $w.frame1 -fill x
    bind $w.frame1.entry <Tab> "focus $w.frame2.frame.text"
    bind $w.frame1.entry <Meta-o> "focus $w.frame2.frame.text"
    bind $w.frame1.entry <Return> "focus $w.frame2.frame.text"
    #focus $w.frame1.entry
    
    # Make definition field
    
    frame $w.frame2 -relief raised -bd 2
    frame $w.frame2.frame -relief flat
    label $w.frame2.label -pady 1m -text "Comments"
    text $w.frame2.frame.text -relief sunken -bd 2 -height 7 -width 50 \
	    -wrap word -yscrollcommand "$w.frame2.frame.s set"
    scrollbar $w.frame2.frame.s -relief sunken \
	    -command "$w.frame2.frame.text yview"
    pack $w.frame2.label -side top -anchor nw
    pack $w.frame2.frame.text -side left -fill both -expand 1
    pack $w.frame2.frame.s -side right -fill y
    pack $w.frame2.frame -side top -fill both -expand 1 -padx 2m -pady 2m
    pack $w.frame2 -fill both -expand 1
    bind $w.frame2.frame.text <Tab> "focus $w.frame1.entry"
    bind $w.frame2.frame.text <Meta-o> "focus $w.frame1.entry"
    #$w.frame2.frame.text insert 1.0 $comments
    
    focus $w.frame2.frame.text
    #$w.frame2.frame.text mark set insert $cursorPos

    # Make OK and Cancel buttons
    
    frame $w.frame3 -relief raised -bd 2
    frame $w.frame3.frame -relief flat
    button $w.frame3.frame.cancel -relief raised -bd 2 -text "Cancel" -width 10\
	    -command "destroy $w"
    button $w.frame3.frame.ok -relief raised -bd 2 -text "Ok" -width 10 \
	    -command "submitChanges $fn $w; destroy $w"

    #@db 02/27 changed the order of Ok and Cancel buttons.

    pack $w.frame3.frame.ok $w.frame3.frame.cancel -side left\
	    -expand 1 -padx 3m -pady 3m -ipady 1m
    pack $w.frame3.frame -fill x
    pack $w.frame3 -fill x
}
