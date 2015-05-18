##
##   $Id: views.tcl 96 2006-03-24 17:59:50Z ybryukhov $
##

## Copyright (C) 1995 The New York Group Theory Cooperative
## See magnus/doc/COPYRIGHT for the full notice.

## Contents: Code supporting workspace object view windows.
##
## Principal Author: Roger Needham, Dmitry Bormotov
##
## Status: In progress.
##
## Revision History:
##
##    01/28/98 @am implemented
##        proc init_EnumertatorProblemView {icon viewParameters} 
##        proc EnumertatorProblemView {viewID} 


# This is the proc to call when an icon is double-clicked, to open
# a view on it:

proc openView {icon} {
  set viewID [get_viewID $icon]
  set w [get_viewPath $viewID]
  if {$w != ""} {
    wm deiconify $w
    raise $w
  } else {
    set viewBuilder [get_viewBuilder $icon]
    if {$viewBuilder == ""} {
      alert "Sorry, there is no view builder defined for [get_objectName $icon]."
    } else {
      set_viewPath $viewID [$viewBuilder $viewID]
    }
  }
}


# The create_ message handler calls this to salt away the viewStructure
# for an object:

proc initViewStruct {icon viewStructure} {
  if {$viewStructure != ""} {
    extractArguments {viewBuilder viewParameters} $viewStructure
    set_viewBuilder $icon $viewBuilder
    init_$viewBuilder $icon $viewParameters
  }
}


## Specialized view builders:

################################################################################
##
## problemView


proc init_problemView {icon viewParameters} {

  extractArguments {viewID viewTitle viewBanner totalARCs helpID subproblems} \
                   $viewParameters
  extractArguments {ARCSlotID value} $totalARCs

  set_viewID $icon $viewID
  set_viewIcon $viewID $icon
  set_viewTitle $viewID $viewTitle
  set_viewBanner $viewID $viewBanner
  set_ARCValue $viewID $ARCSlotID $value
  set_totalARCSlotID $viewID $ARCSlotID
  set_viewHelpID $viewID $helpID
  set_probState $viewID new
  set_viewLog $viewID {}

  # Pick apart the subproblems structure to get the info we want now,
  # and reassemble the stuff we want to store.

  set probComponents {}
  foreach itemGroup $subproblems {

    set condition [lindex $itemGroup 0]
    set subproblemList {}
    foreach subproblem [lindex $itemGroup 1] {

      extractArguments {name state ARCSlotID value helpID} $subproblem
      set_ARCValue $viewID $ARCSlotID $value
      lappend subproblemList [list $name $state $ARCSlotID $helpID]
    }
    lappend probComponents [list $condition $subproblemList]
  }
  set_probComponents $viewID $probComponents
}


proc showProblemHelp { helpID } {
    
    set problemTitle "How this problem works"

    if { [regexp {(.+)::(.+)} $helpID d file title] > 0 } {
	showHelp "probs/$helpID" $problemTitle
	return
    }
    
    showHelp "probs/$helpID::How this problem works" $problemTitle
}


proc problemView {viewID} {

  global A tcl_path

  set w [uniqueDialogName]

  toplevel $w
  eval wm title $w "\"[get_viewTitle $viewID]\""
  eval wm iconname $w "\"[get_viewTitle $viewID]\""
  #@db wm minsize $w 300 300

  # Make menu bar

  menu $w.mbar
  $w configure -menu $w.mbar

  $w.mbar add cascade -label File -menu $w.mbar.file
  makeEditMenu $w.mbar
  $w.mbar add cascade -label Help -menu $w.mbar.help

  #pack $w.mbar.file $w.mbar.edit -side left
  #pack $w.mbar.help -side right

  menu $w.mbar.file 
    $w.mbar.file add command -label "Save log" \
      -command "saveLogToFile $w.frame4.frame.log"

  menu $w.mbar.help
    $w.mbar.help add command -label "General" \
      -command "showHelp {generalProb::How Problem Views Work}"
    set helpID [get_viewHelpID $viewID]
    $w.mbar.help add command -label "This Problem" \
      -command "showProblemHelp $helpID"
#@db "showHelp {probs/$helpID::How this problem works}"

  #pack $w.mbar -side top -fill x

  # The banner frame:

  eval set banner "\"[get_viewBanner $viewID]\""

  #@rn The banner cannot contain newlines yet, so we kludge.
#  set i [string length $banner]
#  set height 1
#  while {$i > 0} {
#    incr i -1
#    if {[string index $banner $i] == "\n"} {incr height}
#  }
  set height [expr [string length $banner] / 20]

  frame $w.frame -relief raised -bd 2
  text $w.frame.descr -height $height -width 20 -relief flat -wrap word
  $w.frame.descr insert end $banner
  $w.frame.descr configure -state disabled
  pack $w.frame.descr -side top -fill x -padx 2m -pady 1m
  pack $w.frame -fill x

  # The total ARCs frame:

  set f $w.frame5
  frame $f -relief raised -bd 2
  label $f.label -text "Total computation units expended"
  entry $f.entry -width 5 -relief sunken
  pack $f.label -side left -padx 2m
  pack $f.entry -side right -padx 2m -pady 1m
  pack $f -side top -fill x
  $f.entry insert 0 [get_ARCValue $viewID [get_totalARCSlotID $viewID]]
  $f.entry configure -state disabled
  set_ARCSlotPath $viewID [get_totalARCSlotID $viewID] $f.entry

  # The subproblems frame:

  frame $w.frame2 -relief raised -bd 2
  set counter 1
  set entries {}
  set theARCSlotIDs {}
  foreach component [get_probComponents $viewID] {

    set cond [lindex $component 0]
    set val 0
    if { [catch {eval expr $cond} val] != 0 || $val == 0 } { continue }

    foreach subproblem [lindex $component 1] {

      eval set name "\"[lindex $subproblem 0]\""
      set state [lindex $subproblem 1]
      set ARCSlotID [lindex $subproblem 2]
      set helpID [lindex $subproblem 3]
      set f $w.frame2.frame$counter
      incr counter
      frame $f -relief flat
      if {$state == 1} {
        lappend theARCSlotIDs $ARCSlotID
        button $f.button -text $name -relief flat \
                         -command "subviewRequest $viewID $ARCSlotID" \
			 -pady 1
        entry $f.entry -width 5 -relief sunken 
        $f.entry insert 0 [get_ARCValue $viewID $ARCSlotID]
        set_ARCSlotPath $viewID $ARCSlotID $f.entry
        lappend entries $f.entry
        set cmd "fixScale $f.entry $f.scale"
        bind $f.entry <KeyRelease> +$cmd
  
        scale $f.scale -from 0 -to 100 -showvalue 0 -orient horiz
        $f.scale set [get_ARCValue $viewID $ARCSlotID]
        set cmd "$f.entry delete 0 end; $f.entry insert 0 \[$f.scale get\]"
        bind $f.scale <B1-Motion> +$cmd
        bind $f.scale <1> +$cmd
  
        pack $f.entry -side right -padx 2m
        pack $f.scale -side right -padx 2m
  
      } else {
        button $f.button -text $name -relief flat -state disabled
      }
      pack $f.button -side left -padx 2m
      pack $f -side top -fill x -pady 1m
    }
  }
  set_ActiveARCSlotIDs $viewID $theARCSlotIDs

  set i [expr [llength $entries] - 1]
  if {$i >= 0} {
    bind [lindex $entries $i] <Tab> "focus [lindex $entries 0]"
    bind [lindex $entries $i] <Return> "focus [lindex $entries 0]"
  }
  while {$i > 0} {
    bind [lindex $entries [expr $i - 1]] <Tab> "focus [lindex $entries $i]"
    bind [lindex $entries [expr $i - 1]] <Return> "focus [lindex $entries $i]"
    incr i -1
  }

  pack $w.frame2 -side top -fill x
  if {[winfo exists $w.frame2.frame1.entry]} {
    focus $w.frame2.frame1.entry
  }
  # Maybe all subproblems are disabled

  # The control panel:

  frame $w.frame3 -relief raised -bd 2
  set f $w.frame3
  button $f.start -text Start -state disabled -width 11 \
    -command "problem_start $w $viewID"
  button $f.suspend -text Suspend -state disabled -width 11 \
    -command "problem_suspend $w $viewID"
  button $f.resume -text Resume -state disabled -width 11 \
    -command "problem_resume $w $viewID"

  button $f.terminate -text Terminate -state disabled -width 11 \
    -command "problem_Terminate $w $viewID"
  button $f.cancel -text Cancel -width 11 \
    -command "problem_Cancel $w $viewID"
  button $f.close -text Close -width 11 \
    -command "problem_Close $w $viewID"

  switch -exact [get_probState $viewID] {
    new {
      $f.start configure -state normal
    }
    running {
      $f.suspend configure -state normal
      $f.terminate configure -state normal
      problem_setEntries $w 0
    }
    stalled {
      $f.suspend configure -state normal
      $f.terminate configure -state normal
      problem_setEntries $w 0
    }
    suspended {
      $f.resume configure -state normal
      $f.terminate configure -state normal
    }
    terminated {
      problem_setEntries $w 0
    }
  }

  framer $f $f left {
    top {} {-fill both -expand 1} {
      left {} {-expand 1} {
        start {-padx 5m -pady 3m -ipady 1m}
        suspend {-padx 5m -pady 3m -ipady 1m}
        resume {-padx 5m -pady 3m -ipady 1m}
      }
      left {} {-expand 1} {
        terminate {-padx 5m -pady 3m -ipady 1m}
        cancel {-padx 5m -pady 3m -ipady 1m}
        close {-padx 5m -pady 3m -ipady 1m}
      }
    }
  }
  pack $w.frame3 -side top -fill x

  # The log:

  frame $w.frame4 -relief raised -bd 2
  frame $w.frame4.frame -relief flat
  label $w.frame4.label -text "Log"
  text $w.frame4.frame.log -relief sunken -bd 2 -height 5 -width 30 \
    -wrap word -yscrollcommand "$w.frame4.frame.s set"
  scrollbar $w.frame4.frame.s -relief sunken \
    -command "$w.frame4.frame.log yview"
  pack $w.frame4.label -side top -anchor nw
  pack $w.frame4.frame.log -side left -fill both -expand 1
  pack $w.frame4.frame.s -side right -fill y
  pack $w.frame4.frame -side top -fill both -expand 1 -padx 2m -pady 1m
  pack $w.frame4 -fill both -expand 1
  #@db $w.frame4.frame.log insert end [get_viewLog $viewID]
  viewLogInsert $w.frame4.frame.log [get_viewLog $viewID]

  wm protocol $w WM_DELETE_WINDOW  "problem_Cancel $w $viewID"

  return $w
}

##  
proc init_EnumertatorProblemView {icon viewParameters} {

  extractArguments {viewID viewTitle viewBanner totalARCs helpID subproblems  \
		    parameters } \
                   $viewParameters
  extractArguments {ARCSlotID value} $totalARCs

  set_viewID $icon $viewID
  set_viewIcon $viewID $icon
  set_viewTitle $viewID $viewTitle
  set_viewBanner $viewID $viewBanner
  set_ARCValue $viewID $ARCSlotID $value
  set_totalARCSlotID $viewID $ARCSlotID
  set_viewHelpID $viewID $helpID
  set_probState $viewID new
  set_viewLog $viewID {}

  set_probComponents $viewID $subproblems
  set_probParameters $viewID $parameters
  
  set componentList [lrange $subproblems 2 end]
  foreach subproblem $componentList {
      set ARCSlotID [lindex $subproblem 4]
      set value [lindex $subproblem 2]
      set_ARCValue $viewID $ARCSlotID $value
  }
}

proc EnumertatorProblemView {viewID} {

  global A tcl_path

  
  set w [uniqueDialogName]

  toplevel $w
  eval wm title $w "\"[get_viewTitle $viewID]\""
  eval wm iconname $w "\"[get_viewTitle $viewID]\""
  
  # Make menu bar

  menu $w.mbar
  $w configure -menu $w.mbar

  $w.mbar add cascade -label File -menu $w.mbar.file
  makeEditMenu $w.mbar
  $w.mbar add cascade -label Help -menu $w.mbar.help

  #pack $w.mbar.file $w.mbar.edit -side left
  #pack $w.mbar.help -side right

  ## Show if have parameters for problem
  if { [have_probParameters $viewID]  } {
      $w.mbar add cascade -label Parameters -menu $w.mbar.parameters
      menu $w.mbar.parameters
      $w.mbar.parameters add command -label "Show parameters" \
	  -command "showParameters  $viewID $w \"[get_viewTitle $viewID]\""
      #pack $w.mbar.parameters -side left
      
  }
    

  menu $w.mbar.file 
    $w.mbar.file add command -label "Save log" \
      -command "saveLogToFile $w.frame4.frame.log"

  menu $w.mbar.help
    $w.mbar.help add command -label "General" \
      -command "showHelp {generalProb::How Problem Views Work}"
    set helpID [get_viewHelpID $viewID]
    $w.mbar.help add command -label "This Problem" \
      -command "showProblemHelp $helpID"
#@db "showHelp {probs/$helpID::How this problem works}"

  #pack $w.mbar -side top -fill x

  # The banner frame:

  eval set banner "\"[get_viewBanner $viewID]\""

  #@rn The banner cannot contain newlines yet, so we kludge.
#  set i [string length $banner]
#  set height 1
#  while {$i > 0} {
#    incr i -1
#    if {[string index $banner $i] == "\n"} {incr height}
#  }
  set height [expr [string length $banner] / 20]

  frame $w.frame -relief raised -bd 2
  text $w.frame.descr -height $height -width 20 -relief flat -wrap word
  $w.frame.descr insert end $banner
  $w.frame.descr configure -state disabled
  pack $w.frame.descr -side top -fill x -padx 2m -pady 1m
  pack $w.frame -fill x

  # The total ARCs frame:

  set f $w.frame5
  frame $f -relief raised -bd 2
  label $f.label -text "Total computation units expended"
  entry $f.entry -width 5 -relief sunken
  pack $f.label -side left -padx 2m
  pack $f.entry -side right -padx 2m -pady 1m
  pack $f -side top -fill x
  $f.entry insert 0 [get_ARCValue $viewID [get_totalARCSlotID $viewID]]
  $f.entry configure -state disabled
  set_ARCSlotPath $viewID [get_totalARCSlotID $viewID] $f.entry

  # The subproblems frame:

  frame $w.frame2 -relief raised -bd 2
  set counter 1
  set entries {}
  set theARCSlotIDs {}
  set componentList [lrange [get_probComponents $viewID] 2 end]
  foreach subproblem $componentList {

      eval set name "\"[lindex $subproblem 3]\""
      #set name [lindex $subproblem 3]
      
      set ARCSlotID [lindex $subproblem 4]
      set helpID [lindex $subproblem 5]
      set f $w.frame2.frame$counter
      incr counter
      frame $f -relief flat
      lappend theARCSlotIDs $ARCSlotID
      button $f.button -text $name -relief flat \
	  -command "subviewRequest $viewID $ARCSlotID" \
	  -pady 1
      entry $f.entry -width 5 -relief sunken 
      $f.entry insert 0 [get_ARCValue $viewID $ARCSlotID]
      set_ARCSlotPath $viewID $ARCSlotID $f.entry
      lappend entries $f.entry
      set cmd "fixScale $f.entry $f.scale"
      bind $f.entry <KeyRelease> +$cmd
      
      scale $f.scale -from 0 -to 100 -showvalue 0 -orient horiz
      $f.scale set [get_ARCValue $viewID $ARCSlotID]
      set cmd "$f.entry delete 0 end; $f.entry insert 0 \[$f.scale get\]"
      bind $f.scale <B1-Motion> +$cmd
      bind $f.scale <1> +$cmd
      
      pack $f.entry -side right -padx 2m
      pack $f.scale -side right -padx 2m
      
      pack $f.button -side left -padx 2m
      pack $f -side top -fill x -pady 1m
  }
  
  set_ActiveARCSlotIDs $viewID $theARCSlotIDs
  
  set i [expr [llength $entries] - 1]
  if {$i >= 0} {
      bind [lindex $entries $i] <Tab> "focus [lindex $entries 0]"
      bind [lindex $entries $i] <Return> "focus [lindex $entries 0]"
  }
  while {$i > 0} {
      bind [lindex $entries [expr $i - 1]] <Tab> "focus [lindex $entries $i]"
      bind [lindex $entries [expr $i - 1]] <Return> "focus [lindex $entries $i]"
      incr i -1
  }

  pack $w.frame2 -side top -fill x
  if {[winfo exists $w.frame2.frame1.entry]} {
    focus $w.frame2.frame1.entry
  }
  # Maybe all subproblems are disabled

  # The control panel:

  frame $w.frame3 -relief raised -bd 2
  set f $w.frame3
  button $f.start -text Start -state disabled -width 11 \
    -command "problem_start $w $viewID"
  button $f.suspend -text Suspend -state disabled -width 11 \
    -command "problem_suspend $w $viewID"
  button $f.resume -text Resume -state disabled -width 11 \
    -command "problem_resume $w $viewID"

  button $f.terminate -text Terminate -state disabled -width 11 \
    -command "problem_Terminate $w $viewID"
  button $f.cancel -text Cancel -width 11 \
    -command "problem_Cancel $w $viewID"
  button $f.close -text Close -width 11 \
    -command "problem_Close $w $viewID"

  switch -exact [get_probState $viewID] {
    new {
      $f.start configure -state normal
    }
    running {
      $f.suspend configure -state normal
      $f.terminate configure -state normal
      problem_setEntries $w 0
    }
    stalled {
      $f.suspend configure -state normal
      $f.terminate configure -state normal
      problem_setEntries $w 0
    }
    suspended {
      $f.resume configure -state normal
      $f.terminate configure -state normal
    }
    terminated {
      problem_setEntries $w 0
    }
  }

  framer $f $f left {
    top {} {-fill both -expand 1} {
      left {} {-expand 1} {
        start {-padx 5m -pady 3m -ipady 1m}
        suspend {-padx 5m -pady 3m -ipady 1m}
        resume {-padx 5m -pady 3m -ipady 1m}
      }
      left {} {-expand 1} {
        terminate {-padx 5m -pady 3m -ipady 1m}
        cancel {-padx 5m -pady 3m -ipady 1m}
        close {-padx 5m -pady 3m -ipady 1m}
      }
    }
  }
  pack $w.frame3 -side top -fill x

  # The log:

  frame $w.frame4 -relief raised -bd 2
  frame $w.frame4.frame -relief flat
  label $w.frame4.label -text "Log"
  text $w.frame4.frame.log -relief sunken -bd 2 -height 5 -width 30 \
    -wrap word -yscrollcommand "$w.frame4.frame.s set"
  scrollbar $w.frame4.frame.s -relief sunken \
    -command "$w.frame4.frame.log yview"
  pack $w.frame4.label -side top -anchor nw
  pack $w.frame4.frame.log -side left -fill both -expand 1
  pack $w.frame4.frame.s -side right -fill y
  pack $w.frame4.frame -side top -fill both -expand 1 -padx 2m -pady 1m
  pack $w.frame4 -fill both -expand 1
  #@db $w.frame4.frame.log insert end [get_viewLog $viewID]
  viewLogInsert $w.frame4.frame.log [get_viewLog $viewID]

  wm protocol $w WM_DELETE_WINDOW "problem_Cancel $w $viewID"

  return $w
}

proc fixScale {entry scale} {
  set value [$entry get]
  if {[regexp {^([1-9][0-9]*|0)$} $value]} {
    $scale set $value
  }
}


proc problem_getARCs {w} {

  # This returns "" if any entry was not valid, otherwise the ARC vector.

  set ARCs ""
  foreach frame [winfo children $w.frame2] {
    if {[winfo exists $frame.entry]} {
      set value [$frame.entry get]

      if {![regexp {^([1-9][0-9]*|0)$} $value]} {
        focus $frame.entry
        $frame.entry select from 0
        $frame.entry select to end
        if {[regexp {^0[0-9]+$} $value]} {
          alert "Octal?\nWho still uses octal?\n\
                 I need the decimal form of `$value'."
          return
        } elseif {[regexp {^0[xX][0-9]+$} $value]} {
          alert "Sorry, no hex.\n\
                 I need the decimal form of `$value'."
          return
        } else {
          alert "The entry `$value' is not a non-negative integer."
          return
        }
      }
  
      lappend ARCs $value
    }
  }
  return $ARCs
}


proc problem_saveARCS {w viewID ARCs} {

  foreach ARCSlotID [get_ActiveARCSlotIDs $viewID] {
    set_ARCValue $viewID $ARCSlotID [pop ARCs]
  }
}


proc problem_setEntries {w val} {

  # Make each entry box in the view editable iff val == 1.

  foreach frame [winfo children $w.frame2] {
    if {[winfo exists $frame.entry]} {
      if {$val} {
        $frame.entry configure -state normal
        $frame.scale configure -state normal
      } else {
        $frame.entry configure -state disabled
        $frame.scale configure -state disabled
      }
    }
  }
}


proc problem_start {w viewID} {

  set ARCs [problem_getARCs $w]
  if {$ARCs == ""} { return }

  problem_saveARCS $w $viewID $ARCs

  $w.frame3.start configure -state disabled
  set_probState $viewID waitStart
  problem_setEntries $w 0

  #redrawParameters $viewID $w
     
  # Send ARC updates

  foreach ARCSlotID [get_ActiveARCSlotIDs $viewID] {
    set value [pop ARCs]
    eval toSessionManager "\"[get_messageTemplate arc]\""
  }

  eval toSessionManager "\"[get_messageTemplate start]\""

  if {[inNoviceMode] == 1} {
    postToMinilog "Starting may be delayed if the session manager is busy."
  }
}


proc problem_suspend {w viewID} {

  #@db
  #postToLog "Hello suspend."

  $w.frame3.suspend configure -state disabled
  $w.frame3.terminate configure -state disabled
  set_probState $viewID waitSuspend

  #redrawParameters $viewID $w

  eval toSessionManager "\"[get_messageTemplate suspend]\""

  if {[inNoviceMode] == 1} {
    postToMinilog "Suspending may be delayed if the session manager is busy."
  }
}


proc problem_resume {w viewID} {

  set ARCs [problem_getARCs $w]
  if {$ARCs == ""} { return }

  problem_saveARCS $w $viewID $ARCs

  $w.frame3.resume configure -state disabled
  $w.frame3.terminate configure -state disabled
  set_probState $viewID waitResume
  problem_setEntries $w 0

  #redrawParameters $viewID $w

  # Send ARC updates

  foreach ARCSlotID [get_ActiveARCSlotIDs $viewID] {
    set value [pop ARCs]
    eval toSessionManager "\"[get_messageTemplate arc]\""
  }

  eval toSessionManager "\"[get_messageTemplate resume]\""

  if {[inNoviceMode] == 1} {
    postToMinilog "Resuming may be delayed if the session manager is busy."
  }
}


proc problem_Terminate {w viewID} {

  #@db
  #postToLog "Hello terminate."

  $w.frame3.suspend configure -state disabled
  $w.frame3.resume configure -state disabled
  $w.frame3.terminate configure -state disabled
  set_probState $viewID waitTerminate
  problem_setEntries $w 0

  #redrawParameters $viewID $w

  eval toSessionManager "\"[get_messageTemplate terminate]\""

  if {[inNoviceMode] == 1} {
    postToMinilog "Termination may be delayed if the session manager is busy."
  }
}


proc problem_Close {w viewID} {

  set ARCs [problem_getARCs $w]
  if {$ARCs == ""} { return }

  set state [get_probState $viewID]
  if {$state == "suspended" || $state == "new"} {
    problem_saveARCS $w $viewID $ARCs
  }

  #@db set_viewLog $viewID [$w.frame4.frame.log get 1.0 end]
  set_viewPath $viewID ""
  destroy $w
}


proc problem_Cancel {w viewID} {
  set_viewPath $viewID ""
  destroy $w
}


proc problem_postToLog {viewID message {level 1}} {

  set w [get_viewPath $viewID]
  if {$w != ""} {
    #db $w.frame4.frame.log insert end "\n\n$message"
    logInsert $w.frame4.frame.log "\n\n$message"
    $w.frame4.frame.log yview -pickplace end
  }
  append_viewLog $viewID "\n\n$message"

  if {$level == 1} {
#@db    eval postToLog "\"$message\""
    postToLog $message
  }
}


proc subviewRequest {viewID ARCSlotID} {

  return
  #@rn We take no action yet.

  if {[inNoviceMode] == 1} {
    postToMinilog "Subview request may be delayed if the session manager is busy"
  }

  eval toSessionManager "\"[get_messageTemplate view_req]\""
}



################################################################################
##
## objectView


proc init_objectView {icon viewStructure} {
  extractArguments {viewID heritage} $viewStructure

  set_viewID $icon $viewID
  set_viewIcon $viewID $icon
  set_viewLog $viewID {}
  set_viewHeritage $viewID $heritage
  # We delay the evaluation of heritage hot text, since names mentioned in it
  # can change.
}


proc objectView {viewID} {

  global A

  set w [uniqueDialogName]

  toplevel $w
  wm title $w {Object View}
  wm iconname $w {Object View}
  #@db wm minsize $w 300 300

  set icon [get_viewIcon $viewID]

  # The menu
  menu $w.mbar
  $w configure -menu $w.mbar

  $w.mbar add cascade -label File -menu $w.mbar.file
  makeEditMenu $w.mbar
  $w.mbar add cascade -label Help -menu $w.mbar.help

  menu $w.mbar.file
    $w.mbar.file add command -label "Save log" \
      -command "saveLogToFile $w.log"
  menu $w.mbar.help

  # The name field:

  label $w.nameLabel -text {View on} -relief flat
  entry $w.nameEntry -relief sunken
  $w.nameEntry insert 0 [get_objectName $icon]

  # The heritage field:

  label $w.heritageLabel -text Heritage -relief flat
  text $w.heritage -relief sunken -bd 2 -height 3 -width 50 \
    -wrap word -yscrollcommand "$w.heritageScrollbar set"
  eval $w.heritage insert 1.0 "\"[get_viewHeritage $viewID]\.\""
  # heritage is hot text with delayed evaluation
  $w.heritage configure -state disabled
  scrollbar $w.heritageScrollbar -relief sunken -command "$w.heritage yview"

  # The definition field:
  if { [string length [get_objectDefinition $icon] ] } {
      label $w.defnLabel -text Definition -relief flat
      text $w.defn -relief sunken -bd 2 -height 5 -width 50 \
	  -wrap word -yscrollcommand "$w.defnScrollbar set"
      $w.defn insert 1.0 [get_objectDefinition $icon]
      $w.defn configure -state disabled
      scrollbar $w.defnScrollbar -relief sunken -command "$w.defn yview"
  } 
  # The log field:

  label $w.logLabel -text Log -relief flat
  text $w.log -relief sunken -bd 2 -height 5 -width 50 \
    -wrap word -yscrollcommand "$w.logScrollbar set"
  #$w.log insert end [get_viewLog $viewID]
  viewLogInsert $w.log [get_viewLog $viewID]
  scrollbar $w.logScrollbar -relief sunken -command "$w.log yview"

  # The button field:

  button $w.cancel -text Cancel -relief raised -bd 2 -width 8\
    -command "destroy $w; set_viewPath $viewID {}"
  button $w.ok -text OK -relief raised -bd 2 -width 8 \
    -command "openView_OK $w $viewID"

  # Pack the widgets

  #@db 02/27 changed the order of the ok and cancel buttons.
    
  framer $w $w top {
    left {-relief raised -bd 2} {-fill x} {
      nameLabel {-pady 1m} nameEntry {-padx 3m -pady 1m -fill x -expand 1}
    }
    top {-relief raised -bd 2} {-fill x} {
      heritageLabel {-anchor nw}
      left {} {-fill x -expand 1 -padx 2m -pady 1m} {
        heritage {-fill both -expand 1}
        heritageScrollbar {-fill y}
      }
    }
  }
  if { [string length [get_objectDefinition $icon] ] } {
      framer $w $w top {
	  top {-relief raised -bd 2} {-fill both -expand 1} {
	      defnLabel {-anchor nw}
	      left {} {-fill both -expand 1 -padx 2m -pady 1m} {
		  defn {-fill both -expand 1}
		  defnScrollbar {-fill y}
	      }
	  }
      }
  }
  framer $w $w top {
      
    top {-relief raised -bd 2} {-fill both -expand 1} {
	logLabel {-anchor nw}
	left {} {-fill both -expand 1 -padx 2m -pady 1m} {
	    log {-fill both -expand 1}
	    logScrollbar {-fill y}
	}
    }
    left {-relief raised -bd 2} {-fill x} {
      ok {-expand 1 -padx 3m -pady 3m -ipady 1m}
      cancel {-expand 1 -padx 3m -pady 3m -ipady 1m}
    }
  }

  wm protocol $w WM_DELETE_WINDOW "destroy $w; set_viewPath $viewID {}"
  #@rn Should call a proc, so this stays in sync with cancel button.

  return $w
}


proc openView_OK {w viewID} {

  set icon [get_viewIcon $viewID]
  set name [$w.nameEntry get]
  if {$name != [get_objectName $icon]} {
      set name [verifyDialogName [$w.nameEntry get]]
      if { $name == "" } { return }
      postToLog "Changed name of [get_objectName $icon] to $name."
      set_objectName $icon $name
      set_iconName $icon $name
      setName [get_objectID $icon] $name
  }

  #@db set_viewLog $viewID [$w.log get 1.0 end]
  set_viewPath $viewID ""
  destroy $w
}


proc object_postToLog {viewID message {level 1}} {

  set w [get_viewPath $viewID]
  if {$w != ""} {

    #$w.log insert end "\n\n$message"
    logInsert $w.log "\n\n$message"

    $w.log yview -pickplace end
  }
  append_viewLog $viewID "\n\n$message"

  if {$level == 1} {
    eval postToLog "\"$message\""
  }
}


################################################################################
##
## helper procs


#@rn Check for file exceptions
proc saveLogToFile {w} {
  # $w is the path of the text widget
  set fileName [savefile [uniqueDialogName]]
  if {$fileName != ""} {
    set f [open $fileName w]
    puts $f [$w get 1.0 end]
    close $f
  }
}
