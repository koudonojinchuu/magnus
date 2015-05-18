#
##   $Id: dialogs.tcl 96 2006-03-24 17:59:50Z ybryukhov $
##

## Copyright (C) 1995 The New York Group Theory Cooperative
## See magnus/doc/COPYRIGHT for the full notice.

## Contents: Code supporting hard-wired dialogs.
##
## Principal Author: Roger Needham
##
## Status: In progress.
##
## Revision History:
##


proc alert {message} {
  tk_dialog .d {Alert} $message warning 0 OK
}


proc uniqueObjectName {prefix} {
  set index 1;
  while {[object_name_is_taken $prefix$index]} { incr index }
  return $prefix$index
}


proc verifyDialogName {name} {

  # This returns "" if $name is unacceptable, otherwise returns $name
  # with whitespace replaced with `_'.

  if { $name == "" } {
    alert "You must give a name."
    return
  }

  # Make sure there is no whitespace in the name.
  regsub -all { } $name {_} name

  # No illicit characters in the name.
  if {[regexp {[^a-zA-Z0-9_]} $name] || ![regexp {^[a-zA-Z]} $name]} {
    alert "I can't accept `$name'.\nA name must begin with a letter, \
           and contain only letters, digits, and underscores."
    return
  }

  if {[object_name_is_taken $name]} {
    alert "The name $name is already taken."
    return
  }
  return $name
}


proc defineFPGroupDialog {title message {name ""} {defn ""} {errorMsg ""}} {
  if {$name == ""} { set name [uniqueObjectName G] }

  if {$defn == ""} { set defn "<  ;  >" }

  defineObjectDialog $message defineFPGroupDialog $title $name \
    {{{Presentation format} fpGroupDialog} {{Word format} wordConventions}} \
    $defn $errorMsg {FP}
}


proc defineORGroupDialog {title message {name ""} {defn ""} {errorMsg ""}} {
  if {$name == ""} { set name [uniqueObjectName G] }

  if {$defn == ""} { set defn "<  ;  >" }

  defineObjectDialog $message defineFPGroupDialog $title $name \
    {{{Presentation format} fpGroupDialog} {{Word format} wordConventions}} \
    $defn $errorMsg {OR}
}

proc defineSCGroupDialog {title message {name ""} {defn ""} {errorMsg ""}} {
  if {$name == ""} { set name [uniqueObjectName G] }

  if {$defn == ""} { set defn "<  ;  >" }

  defineObjectDialog $message defineFPGroupDialog $title $name \
    {{{Presentation format} fpGroupDialog} {{Word format} wordConventions}} \
    $defn $errorMsg {SC}
}

proc defineFreeGroupDialog {title message {name ""} {defn ""} {errorMsg ""}} {
  if {$name == ""} { set name [uniqueObjectName F] }

  if {$defn == ""} { set defn "<   >" }

  defineObjectDialog $message defineFPGroupDialog $title $name \
    {{{Presentation format} freeGroupDialog}} \
    $defn $errorMsg {FREE}
}


proc defineAbelGroupDialog {title message {name ""} {defn ""} {errorMsg ""}} {
  if {$name == ""} { set name [uniqueObjectName A] }

  if {$defn == ""} { set defn "<  ;  >" }

  defineObjectDialog $message defineFPGroupDialog $title $name \
    {{{Abelian presentations} abelGroupDialog} \
      {{Presentation format} fpGroupDialog} \
      {{Word format} wordConventions}} \
    $defn $errorMsg {ABELIAN}
}


proc defineNilpGroupDialog {title message {name ""} {defn ""} {errorMsg ""}} {
  if {$name == ""} { set name [uniqueObjectName G] }

  if {$defn == ""} { set defn "    <  ;  >" }

  defineObjectDialog $message defineNilpGroupDialog $title $name \
    {{{Nilpotent presentations} nilpGroupDialog} \
      {{Presentation format} fpGroupDialog} \
      {{Word format} wordConventions}} \
    $defn $errorMsg {NILPOTENT}
}

proc defineFreeNilpGroupDialog {title message {name ""} {defn ""} {errorMsg ""}} {
  if {$name == ""} { set name [uniqueObjectName N] }

  if {$defn == ""} { set defn "    <  >" }

  defineObjectDialog $message defineFreeNilpGroupDialog $title $name \
    {{{Nilpotent presentations} freeNilpGroupDialog} \
      {{Presentation format} fpGroupDialog} \
      {{Word format} wordConventions}} \
    $defn $errorMsg {FREE_NILPOTENT}
}

proc definePermutationDialog {title message {name ""} {defn ""} {errorMsg ""}} {

  if {$name == ""} { set name [uniqueObjectName p] }

  if {$defn == ""} { set defn "\{  \}" }

  set cursorPos "1.2"

  defineObjectDialog $message definePermutationDialog $title $name \
    {{{Permutation format} permutationConventions}} $defn $errorMsg $cursorPos
}

proc defineNilpotentWordDialog {title message {name ""} {defn ""} {errorMsg ""}} {

  if {$name == ""} { set name [uniqueObjectName w] }

  defineObjectDialog $message defineNilpotentWordDialog $title $name \
    {{{Word format} wordConventions}} $defn $errorMsg {WORD}
}
#@ep --------------------------------------------------------------

proc defineWordDialog {title message {name ""} {defn ""} {errorMsg ""}} {

  if {$name == ""} { set name [uniqueObjectName w] }

  defineObjectDialog $message defineWordDialog $title $name \
    {{{Word format} wordConventions}} $defn $errorMsg {WORD}
}


proc defineEquationDialog {title message {name ""} {defn ""} {errorMsg ""}} {

  if {$name == ""} { set name [uniqueObjectName E] }

  defineObjectDialog $message defineEquationDialog $title $name \
    {{{Equation format} equationConventions} {{Word format} wordConventions}} \
    $defn $errorMsg {EQUATION}
}

proc defineSystemOfEquationsDialog {title message {name ""} {defn ""} {errorMsg ""}} {

  if {$name == ""} { set name [uniqueObjectName SE] }

  if {$defn == ""} { set defn "\{\n  \n\}" }

  set cursorPos "2.2"
  
  defineObjectDialog $message defineSystemOfEquationsDialog $title $name \
    {{{Tuple of words format} SystemOfEquationsConventions}} $defn $errorMsg \
    {SYS_OF_EQUATION} $cursorPos
}

proc defineMapDialog {title message {name ""} {defn ""} {errorMsg ""} {cp ""}} {

    global A
    
    if {$name == ""} { set name [uniqueObjectName m] }
    
    if {$defn == ""} {
	set defn "\{\n  \n\}" 
    } 

    eval set title "\"$title\""

    if { $cp == ""} {  
	set cursorPos "2.2" 
    } else {
	set cursorPos "2.$cp"
    }
    
    #set errorMsg ""
    
    defineObjectDialog $message defineMapDialog $title $name \
	{{{Map format} mapConventions}} $defn $errorMsg {MAP}\
        $cursorPos
}


proc defineSubgroupDialog {title message {name ""} {defn ""} {errorMsg ""}} {

  if {$name == ""} { set name [uniqueObjectName H] }

  if {$defn == ""} { set defn "\{\n  \n\}" }

  set cursorPos "2.2"

  defineObjectDialog $message defineSubgroupDialog $title $name \
    {{{Subgroup format} subgroupConventions}} $defn $errorMsg \
    {SUBGROUP} $cursorPos
}


proc defineSetOfWordsDialog {title message {name ""} {defn ""} {errorMsg ""}} {

  if {$name == ""} { set name [uniqueObjectName S] }

  if {$defn == ""} { set defn "\{\n  \n\}" }

  set cursorPos "2.2"
  
  defineObjectDialog $message defineSetOfWordsDialog $title $name \
    {{{Set of words format} setOfWordsConventions}} $defn $errorMsg \
    {SET_OF_WORDS} $cursorPos
}


proc defineVectorOfWordsDialog {title message {name ""} {defn ""} {errorMsg ""}} {

  if {$name == ""} { set name [uniqueObjectName T] }

  if {$defn == ""} { set defn "\{\n  \n\}" }

  set cursorPos "2.2"
  
  defineObjectDialog $message defineVectorOfWordsDialog $title $name \
    {{{Tuple of words format} vectorOfWordsConventions}} $defn $errorMsg \
    {VECTOR_OF_WORDS} $cursorPos
}


proc defineObjectDialog {message dialog title name helps defn \
			 errorMsg cathegory {cursorPos "1.2"} } {

  # $message is the SM message template
  # $dialog is the name of the calling proc
  # $title is the dialog window title
  # $name is the default name of the object
  # $help is the help string to display
  # $defn is the default/erroneous definition
  # $errorMsg is the syntax error message (empty if none)

  set w [uniqueDialogName]
  toplevel $w
  wm title $w $title
  wm iconname $w "Dialog"
  #@db wm minsize $w 200 200

  # Make menu bar

  menu $w.mbar
  $w configure -menu $w.mbar

# @am change the look of File menu

  $w.mbar add command -label "Read from file" \
      -command "getDefnFromFile_Button $w.frame2.frame.text"
  $w.mbar add command -label "Read from database" \
      -command "getDefnFromDatabase_Button $cathegory $w.frame1.entry $w.frame2.frame.text"

  $w.mbar add command -label "Magnus' choice" \
      -command "getRandomDefn_Button $cathegory \"$message\" $w.frame2.frame.text"

#  menubutton $w.mbar.file -text File -menu $w.mbar.file.menu
  makeEditMenu $w.mbar
  $w.mbar add cascade -label Help -menu $w.mbar.help

#  pack  $w.mbar.filemenu1   $w.mbar.filemenu2  $w.mbar.filemenu3 $w.mbar.edit -side left

#  pack $w.mbar.file $w.mbar.edit -side left
#  pack $w.mbar.help -side right

#  menu $w.mbar.file.menu 
#    $w.mbar.file.menu add command -label "Read definition from file" \
#      -command "getDefnFromFile $w.frame2.frame.text"
#    $w.mbar.file.menu add command -label "Read object from database" \
\#      -command "getDefnFromDatabase $cathegory $w.frame1.entry $w.frame2.frame.text"

  menu $w.mbar.help
  foreach pair $helps {
    $w.mbar.help add command -label [lindex $pair 0] \
      -command "makeDefnHelp $w [lindex $pair 1] 1"
  }

  #pack $w.mbar -side top -fill x

  # Make name field

  frame $w.frame1 -relief raised -bd 2
  label $w.frame1.label -text "Name"
  entry $w.frame1.entry -relief sunken
  $w.frame1.entry insert 0 $name
  pack $w.frame1.label -side left
  pack $w.frame1.entry -fill x -padx 2m -pady 1m
  pack $w.frame1 -fill x
  bind $w.frame1.entry <Tab> "focus $w.frame2.frame.text"
  bind $w.frame1.entry <Meta-o> "focus $w.frame2.frame.text"
  bind $w.frame1.entry <Return> "focus $w.frame2.frame.text"
  #focus $w.frame1.entry

  # Make definition field

  frame $w.frame2 -relief raised -bd 2
  frame $w.frame2.frame -relief flat
  label $w.frame2.label -text "Definition"
  text $w.frame2.frame.text -relief sunken -bd 2 -height 5 -width 50 \
    -wrap word -yscrollcommand "$w.frame2.frame.s set"
  scrollbar $w.frame2.frame.s -relief sunken \
    -command "$w.frame2.frame.text yview"
  pack $w.frame2.label -side top -anchor nw
  pack $w.frame2.frame.text -side left -fill both -expand 1
  pack $w.frame2.frame.s -side right -fill y
  pack $w.frame2.frame -side top -fill both -expand 1 -padx 2m -pady 1m
  pack $w.frame2 -fill both -expand 1
  bind $w.frame2.frame.text <Tab> "focus $w.frame1.entry"
  bind $w.frame2.frame.text <Meta-o> "focus $w.frame1.entry"
  $w.frame2.frame.text insert 1.0 $defn

  focus $w.frame2.frame.text
  $w.frame2.frame.text mark set insert $cursorPos

  # Make error field if necessary

  if {$errorMsg != ""} {
    frame $w.frame5 -relief raised -bd 2
    frame $w.frame5.frame -relief flat
    label $w.frame5.label -text "Problem with check-in" -foreground red
    text $w.frame5.frame.text -relief sunken -bd 2 -height 5 -width 50 \
      -wrap word -yscrollcommand "$w.frame5.frame.s set"
    scrollbar $w.frame5.frame.s -relief sunken \
      -command "$w.frame5.frame.text yview"
    pack $w.frame5.label -side top -anchor nw
    pack $w.frame5.frame.text -side left -fill both -expand 1
    pack $w.frame5.frame.s -side right -fill y
    pack $w.frame5.frame -side top -fill both -expand 1 -padx 2m -pady 1m
    pack $w.frame5 -fill both -expand 1

    # The message string has the form {{n} {mesg}}, where n is the index
    # into the defn of the problem
    set index [lindex $errorMsg 0]
    set mesg [lindex $errorMsg 1]
    $w.frame5.frame.text insert 1.0 \
      "[string range $defn 0 $index]\n\n$mesg"
    $w.frame5.frame.text yview -pickplace end
    $w.frame5.frame.text configure -state disabled

    # Put cursor at error in definition field
    $w.frame2.frame.text mark set insert "1.0 + $index chars"
    $w.frame2.frame.text yview -pickplace insert
    focus $w.frame2.frame.text
  }

  # Make OK and Cancel buttons

  frame $w.frame3 -relief raised -bd 2
  frame $w.frame3.frame -relief flat
  button $w.frame3.frame.cancel -relief raised -bd 2 -text "Cancel" -width 8\
         -command "destroy $w"
  button $w.frame3.frame.ok -relief raised -bd 2 -text "Ok" -width 8 \
	  -command "defineObjectDialog_OK $w $dialog \{$title\} \{$message\}"

  #@db 02/27 changed the order of Ok and Cancel buttons.

  pack $w.frame3.frame.ok $w.frame3.frame.cancel -side left\
    -expand 1 -padx 3m -pady 3m -ipady 1m
  pack $w.frame3.frame -fill x
  pack $w.frame3 -fill x

  # Make the help field?

  if { [inNoviceMode] } {
    makeDefnHelp $w [lindex [lindex $helps 0] 1]
  }


  # Blink error field label
  if {$errorMsg != ""} {
    blinkWidget $w.frame5.label 0 red OldLace
  }
}


# @am the following two procs is the result of a glitch in tk:
# after a  button was  pressed,  it leaves it active sand at
# some point you can have more than one active button  
proc getDefnFromFile_Button { textw } {
    getDefnFromFile $textw
#    $butn configure -state normal
}

proc getDefnFromDatabase_Button { cathegory entry textw } {
    getDefnFromDatabase $cathegory $entry $textw
#    $butn configure -state normal
}


proc getRandomDefn_Button { cathegory msg w } {
    # redirect message to the definition generator
    regsub {2} $msg "4 $cathegory" tmpmsg

    set message "$tmpmsg $w"
    #puts $message

    eval [concat toSessionManager "\"$message\""]
    #show_defn_ defn { <a,b,c,d,e ;saasas> } w  $w 
}

proc blinkWidget {w count onColor offColor} {

  # Call this first with $count == 0. It blinks the widget
  # 3 times, leaving it with foreground color == $onColor.

  if {$count == 6} { return }

  if {[winfo exists $w]} {
    $w configure -foreground $offColor
    after 500 blinkWidget $w [incr count] $offColor $onColor
  }
}


proc makeDefnHelp {w helpKind {blink_p 0}} {
  if {![winfo exists $w.frame4]} {

    frame $w.frame4 -relief raised -bd 2
    frame $w.frame4.frame -relief flat
    label $w.frame4.label -text "Help"
    text $w.frame4.frame.text -relief sunken -bd 2 -height 5 -width 50 \
      -wrap word -yscrollcommand "$w.frame4.frame.s set"
    scrollbar $w.frame4.frame.s -relief sunken \
      -command "$w.frame4.frame.text yview"
    pack $w.frame4.label -side top -anchor nw
    pack $w.frame4.frame.text -side left -fill both -expand 1
    pack $w.frame4.frame.s -side right -fill y
    pack $w.frame4.frame -side top -fill both -expand 1 -padx 2m -pady 1m
    pack $w.frame4 -fill both -expand 1 -before $w.frame3
  }
  $w.frame4.frame.text configure -state normal
  $w.frame4.frame.text delete 1.0 end
  $w.frame4.frame.text insert 1.0 [getHelp $helpKind]
  $w.frame4.frame.text configure -state disabled

  if { $blink_p == 1 && [inNoviceMode] } {
    set temp [lindex [$w.frame4.frame.text configure -background] 4]
    $w.frame4.frame.text configure -background MediumSeaGreen
    after 50 "$w.frame4.frame.text configure -background $temp"
  }
}


#@rn Check for file exceptions
proc getDefnFromFile {w} {
  # $w is the path of the text widget
  set fileName [openfile [uniqueDialogName]]
  if {$fileName != ""} {
    $w delete 1.0 end
    $w insert 1.0 [read [open $fileName]]
  }
}


proc defineObjectDialog_OK {w dialog title message} {

  # $w is the dialog widget which was OK'd

  set name [verifyDialogName [$w.frame1.entry get]]
  if { $name == "" } { return }

  set defn [$w.frame2.frame.text get 1.0 end-1char]
  if { $defn == "" } {
    alert "You must give a definition of $name."
    return
  }

  destroy $w

  save_pendingCheckin $name $dialog $defn $title $message
  if {[inNoviceMode] == 1} {
    postToMinilog "Check-in may be delayed if the session manager is busy."
  }

  # The defn must be on a single line for the SM:
  regsub -all "\n" $defn " " defn
  set message "$message \$name \$defn"
  eval [concat toSessionManager "\"$message\""]
}


proc defineIntegerDialog {descr low hi message} {

  # $descr says what the integer is for
  # $message is the SM message template

  set w [uniqueDialogName]
  toplevel $w
  wm title $w "Choose an integer"
  wm iconname $w "Dialog"
  #@db wm minsize $w 100 100

  # Add bounds to descr

  set bounds {}
  if {$low != ""} { set bounds "at least $low" }
  if {$hi != ""} {
    if {$bounds != ""} { set bounds "$bounds, " }
    set bounds "${bounds}at most $hi"
  }
  if {$bounds != ""} { set descr "$descr\n($bounds)" }

  # Make descr field

  frame $w.frame1 -relief raised -bd 2
  message $w.frame1.msg -width 3i -text $descr
  pack $w.frame1.msg -side top -expand 1 -fill both -padx 2m -pady 2m
  pack $w.frame1 -side top -expand 1 -fill both

  # Make entry field

  frame $w.frame2 -relief raised -bd 2
  entry $w.frame2.entry -relief sunken
#  $w.frame2.entry insert 0 $name
  pack $w.frame2.entry -fill x -padx 2m -pady 2m
  pack $w.frame2 -fill x
  focus $w.frame2.entry
  bind $w.frame2.entry <Return> \
    "$w.frame3.frame.ok flash; $w.frame3.frame.ok invoke"

  # Make OK and Cancel buttons

  frame $w.frame3 -relief raised -bd 2
  frame $w.frame3.frame -relief flat
  button $w.frame3.frame.cancel -relief raised -bd 2 -text "Cancel" -width 8\
         -command "destroy $w"
  button $w.frame3.frame.ok -relief raised -bd 2 -text "OK" -width 8 \
	  -command "defineIntegerDialog_OK $w \{$low\} \{$hi\} \{$message\}"
  pack $w.frame3.frame.ok $w.frame3.frame.cancel -side left\
    -expand 1 -padx 3m -pady 3m -ipady 1m
  pack $w.frame3.frame -fill x
  pack $w.frame3 -fill x

}


proc defineIntegerDialog_OK {w low hi message} {

  # Originally intended for sending a message to the SM,
  # if message is empty, this returns the integer.

  set n [$w.frame2.entry get]
  
  if {![regexp {^(-?[1-9][0-9]*|0)$} $n]} {
    alert "`$n' is not an integer."
    return
  }
  if {$low != "" && $n < $low} {
    alert "`$n' is not at least $low."
    return
  }
  if {$hi != "" && $n > $hi} {
    alert "`$n' is greater than $hi."
    return
  }
  destroy $w

  if {$message == ""} {
    return $n
  } else {
    eval [concat toSessionManager "\"$message $n\""]
  }
}


proc makeTextWindow {w title text} {

  # $w must be a unique widget path for the window.

  if { [winfo exists $w] } {
    $w.frame.frame1.text configure -state normal
    $w.frame.frame1.text delete 1.0 end
    $w.frame.frame1.text insert 1.0 $text
    $w.frame.frame1.text configure -state disabled
    wm deiconify $w
    raise $w
    return
  }

  toplevel $w
  wm title $w $title
  wm iconname $w $title
  #@db wm minsize $w 30 10

  frame $w.frame -relief flat
  frame $w.frame.frame1 -relief raised -bd 2 -background OldLace
  frame $w.frame.frame2 -relief raised -bd 2
  text $w.frame.frame1.text -relief flat -height 25 -width 50 -wrap word \
       -yscrollcommand "$w.frame.frame1.s set" -setgrid 1 \
       -background OldLace -highlightthickness 0
  scrollbar $w.frame.frame1.s -relief sunken \
    -command "$w.frame.frame1.text yview"
  button $w.frame.frame2.ok -relief raised -bd 2 -text "OK" -width 8 \
         -command "destroy $w"
  pack $w.frame.frame1.s -side right -fill y
  pack $w.frame.frame1.text -side top -fill both -padx 2m -pady 1m -expand 1
  pack $w.frame.frame1 -fill both -expand 1
  pack $w.frame.frame2.ok -expand 1 -padx 3m -pady 3m -ipady 1m
  pack $w.frame.frame2 -fill x
  pack $w.frame -fill both -expand 1

  $w.frame.frame1.text insert 1.0 $text
  $w.frame.frame1.text configure -state disabled
}


proc makeTextWindowFromFile {w title filename} {

  if { ! [file exists $filename] } {
    alert \
      "The file $filename does not exist, or you do not have search permission"
    return
  }

  if { ! [file readable $filename] } {
    alert \
      "The file $filename exists, but you do not have read permission"
    return
  }

  # Yes, that's right, open does not return an error code if there is any
  # problem opening! Ditto, read!

  makeTextWindow $w $title [read [open $filename]]
}


proc makeHelp {fileName kind title} {

  # This still works, but has been superceded by the manual system.

  # The `kind' parameter should be a single token like Workspace or Magnus.

  set w ".help$kind"

  makeTextWindowFromFile $w $title $fileName
}


#@db
proc defineSetOfRelatorsDialog {title message {name ""} {defn ""}\
       {errorMsg ""}} {

  if {$name == ""} { set name [uniqueObjectName H] }

  if {$defn == ""} { set defn "\{\n\n\}" }

  set dialog defineSetOfRelatorsDialog
  set helps {{{Set of relators format} setOfRelatorsConventions}}

  # $message is the SM message template
  # $dialog is the name of the calling proc
  # $title is the dialog window title
  # $name is the default name of the object
  # $help is the help string to display
  # $defn is the default/erroneous definition
  # $errorMsg is the syntax error message (empty if none)

  set w [uniqueDialogName]
  toplevel $w
  wm title $w $title
  wm iconname $w "Dialog"
  #@db wm minsize $w 200 200

  # Make menu bar

  menu $w.mbar
  $w configure -menu $w.mbar

  $w.mbar add cascade -label File -menu $w.mbar.file
  makeEditMenu $w.mbar
  $w.mbar add cascade -label Help -menu $w.mbar.help

  #pack $w.mbar.file $w.mbar.edit -side left
  #pack $w.mbar.help -side right

  menu $w.mbar.file 
  $w.mbar.file add command -label "Read definition from file" \
    -command "getDefnFromFile $w.frame2.frame.text"

  menu $w.mbar.help
  foreach pair $helps {
    $w.mbar.help add command -label [lindex $pair 0] \
      -command "makeDefnHelp $w [lindex $pair 1] 1"
  }

  #pack $w.mbar -side top -fill x

  # Make name field

  frame $w.frame1 -relief raised -bd 2
#  label $w.frame1.label -text "Enter a set of relators"

  set banner "Enter a set of relators"	
  set height [expr [string length $banner] / 20]
  text $w.frame1.descr -height $height -width 20 -relief flat -wrap word
  $w.frame1.descr insert end $banner
  $w.frame1.descr configure -state disabled
  pack $w.frame1.descr -side top -fill x -padx 2m -pady 1m
  pack $w.frame1 -fill x

  entry $w.frame1.entry -relief sunken
  $w.frame1.entry insert 0 $name
#  pack $w.frame1.label -side left
#  pack $w.frame1.entry -fill x -padx 2m -pady 1m
#  pack $w.frame1 -fill x
#  bind $w.frame1.entry <Tab> "focus $w.frame2.frame.text"
#  bind $w.frame1.entry <Meta-o> "focus $w.frame2.frame.text"
#  bind $w.frame1.entry <Return> "focus $w.frame2.frame.text"
#  focus $w.frame1.entry

  # Make definition field

  frame $w.frame2 -relief raised -bd 2
  frame $w.frame2.frame -relief flat
  label $w.frame2.label -text "Definition"
  text $w.frame2.frame.text -relief sunken -bd 2 -height 5 -width 50 \
    -wrap word -yscrollcommand "$w.frame2.frame.s set"
  scrollbar $w.frame2.frame.s -relief sunken \
    -command "$w.frame2.frame.text yview"
  pack $w.frame2.label -side top -anchor nw
  pack $w.frame2.frame.text -side left -fill both -expand 1
  pack $w.frame2.frame.s -side right -fill y
  pack $w.frame2.frame -side top -fill both -expand 1 -padx 2m -pady 1m
  pack $w.frame2 -fill both -expand 1
#  bind $w.frame2.frame.text <Tab> "focus $w.frame1.entry"
#  bind $w.frame2.frame.text <Meta-o> "focus $w.frame1.entry"
  bind $w.frame2.frame.text
  bind $w.frame2.frame.text 
  $w.frame2.frame.text insert 1.0 $defn
  focus $w.frame2.frame.text

  # Make error field if necessary

  if {$errorMsg != ""} {
    frame $w.frame5 -relief raised -bd 2
    frame $w.frame5.frame -relief flat
    label $w.frame5.label -text "Problem with check-in" -foreground red
    text $w.frame5.frame.text -relief sunken -bd 2 -height 5 -width 50 \
      -wrap word -yscrollcommand "$w.frame5.frame.s set"
    scrollbar $w.frame5.frame.s -relief sunken \
      -command "$w.frame5.frame.text yview"
    pack $w.frame5.label -side top -anchor nw
    pack $w.frame5.frame.text -side left -fill both -expand 1
    pack $w.frame5.frame.s -side right -fill y
    pack $w.frame5.frame -side top -fill both -expand 1 -padx 2m -pady 1m
    pack $w.frame5 -fill both -expand 1

    # The message string has the form {{n} {mesg}}, where n is the index
    # into the defn of the problem
    set index [lindex $errorMsg 0]
    set mesg [lindex $errorMsg 1]
    $w.frame5.frame.text insert 1.0 \
      "[string range $defn 0 $index]\n\n$mesg"
    $w.frame5.frame.text yview -pickplace end
    $w.frame5.frame.text configure -state disabled

    # Put cursor at error in definition field
    $w.frame2.frame.text mark set insert "1.0 + $index chars"
    $w.frame2.frame.text yview -pickplace insert
    focus $w.frame2.frame.text
  }

  # Make OK and Cancel buttons

  frame $w.frame3 -relief raised -bd 2
  frame $w.frame3.frame -relief flat
  button $w.frame3.frame.cancel -relief raised -bd 2 -text "Cancel" -width 8\
         -command "destroy $w"
  button $w.frame3.frame.ok -relief raised -bd 2 -text "OK" -width 8 \
	  -command "defineObjectDialog_OK $w $dialog \{$title\} \{$message\}"
  pack $w.frame3.frame.ok $w.frame3.frame.cancel -side left\
    -expand 1 -padx 3m -pady 3m -ipady 1m
  pack $w.frame3.frame -fill x
  pack $w.frame3 -fill x

  # Make the help field?

  if { [inNoviceMode] } {
    makeDefnHelp $w [lindex [lindex $helps 0] 1]
  }


  # Blink error field label
  if {$errorMsg != ""} {
    blinkWidget $w.frame5.label 0 red OldLace
  }
}







#
# @dp Quick hack. The implementation must be refined.
#
proc defineMagnusBreakdown { args }  {

  extractArguments {title descr zero nonzero default message} $args

  global default_gens 
  set default_gens "$default"
  
  set w [uniqueDialogName]
  toplevel $w
  wm title $w "$title"
  wm iconname $w "Dialog"
  #@db wm minsize $w 100 100
  
  # Make descr field

  frame $w.frame1 -relief raised -bd 2
  message $w.frame1.msg -width 3i -text $descr
  pack $w.frame1.msg -side top -expand 1 -fill both -padx 2m -pady 2m
  pack $w.frame1 -side top -expand 1 -fill both

  # Make entry field

  frame $w.frame2 -relief raised -bd 2
  message $w.frame2.msg  -width 2i -text "Type a generator(s): "
  pack $w.frame2.msg -side left -expand 1 -fill both -padx 2m -pady 2m
  entry $w.frame2.entry -relief sunken

  pack $w.frame2.entry -fill x -padx 2m -pady 2m
  pack $w.frame2 -fill x
  focus $w.frame2.entry
  bind $w.frame2.entry <Return> "$w.frame4.frame.ok flash ; $w.frame4.frame.ok invoke"


  # Make lists of zero sum and non-zero sum generators.

  frame $w.frame3  -relief raised -bd 2
  message $w.frame3.zero  -width 4i -text "List of zero sum generators: $zero"
  message $w.frame3.nonzero  -width 4i -text "List of non-zero sum generators: $nonzero"
  pack $w.frame3.zero -anchor nw -side top  -expand 1 -fill both -padx 2m -pady 2m
  pack $w.frame3.nonzero -anchor nw -side top  -expand 1 -fill both -padx 2m -pady 2m
  pack $w.frame3 -side top -expand 1 -fill both

  # Make Default, OK and Cancel buttons

  frame $w.frame4 -relief raised -bd 2
  frame $w.frame4.frame -relief flat
  button $w.frame4.frame.default -relief raised -bd 2 \
	-text "Default generator(s)" -width 20 \
	-command " $w.frame2.entry delete 0 end ; $w.frame2.entry insert 0 \{$default_gens\} "
  button $w.frame4.frame.cancel -relief raised -bd 2 -text "Cancel" -width 8\
         -command "destroy $w"
  button $w.frame4.frame.ok -relief raised -bd 2 -text "OK" -width 8 \
	  -command "defineMagnusBreakdown_OK $w \{$message\}"
  pack $w.frame4.frame.default -side top -expand 1 -padx 3m -pady 3m -ipady 1m
  pack $w.frame4.frame.ok $w.frame4.frame.cancel -side left\
    -expand 1 -padx 3m -pady 3m -ipady 1m
  pack $w.frame4.frame -fill x
  pack $w.frame4 -fill x
}


proc defineMagnusBreakdown_OK {w message} {

  set entry_val [$w.frame2.entry get]

  destroy $w

  if {$message == ""} {
    return 
  } else {
     eval [concat toSessionManager "\"$message \{ $entry_val \}\""]	   
  }
}


#
# @am  dialogs to work with package mechanism
#
proc definePackageDialog { title  objects ch_types message }  {
  global paramList

    set paramList [list]

  set w [uniqueDialogName]
  toplevel $w
  wm title $w $title
  wm iconname $w "Dialog"
  
  # Make descr field

  frame $w.frame1 -relief raised -bd 1
  message $w.frame1.msg -width 3i -text "Add a new package to Magnus"
  pack $w.frame1.msg -side top -expand 1 -fill both -padx 1m -pady 1m
  pack $w.frame1 -side top -expand 1 -fill both

  # Make entry fields:

  # Enter name field
  frame $w.namef -relief raised -bd 1 
  message $w.namef.label -width 2i -text "Enter project name " 
  entry $w.namef.text -relief sunken -width 20
 
  pack $w.namef.label -side left -expand 1 -fill both -padx 1m -pady 1m
  pack $w.namef.text -side left -fill both -expand 1
  pack $w.namef -side top -expand 1 -fill both  -pady 1m

  # Enter command line field
  frame $w.command -relief raised -bd 1
  message $w.command.label  -width 2i  -text "Command line " 
  entry $w.command.text -relief sunken -width 40
  button $w.command.browse -relief raised -bd 2 -text "Browse" -width 8 \
	  -command "getFileName $w.command.text"


  pack $w.command.label -side left -expand 1 -fill both -padx 1m -pady 1m
  pack $w.command.text -side left -fill both -expand 1
  pack $w.command.browse -side left -fill both -expand 1
  pack $w.command -side top -expand 1 -fill both 


  # Select checkin type
  global checkin_value 
  set checkin_value -1
  set count 0
  set fn  0
  frame $w.checkin -relief raised -bd 1
  message $w.checkin.label -width 2i -text "Select checkin type " 
  pack $w.checkin.label -side top -expand 1 -fill both -padx 1m -pady 1m

  frame $w.checkin.$fn -bd 1

  foreach ch_type $ch_types {
      set count [expr $count + 1]
      if {$count > 4} { 
	  set count 0
	  pack $w.checkin.$fn -side top -expand 1 -fill both -padx 1m -pady 1m
	  set fn [expr $fn + 1]
	  frame $w.checkin.$fn  -bd 1
      }
      
      set c_name [lindex $ch_type 0]
      set c_value [lindex $ch_type 1]
      radiobutton $w.checkin.$fn.b$c_value  -value $c_value   -variable checkin_value -text "$c_name"
      pack $w.checkin.$fn.b$c_value -side left -fill both -expand 1
  }     
  
  pack $w.checkin.$fn -side top -expand 1 -fill both -padx 1m -pady 1m
  pack $w.checkin -side top -expand 1 -fill both -padx 1m 
 
  # Select object type
  global object_value 
  set object_value -1
  set count 0
  set fn  0
  frame $w.object -relief raised -bd 1
  message $w.object.label -width 2i -text "Select object " 
  pack $w.object.label -side top -expand 1 -fill both -padx 1m -pady 1m

  frame $w.object.$fn -bd 1

  foreach o_type $objects {
      set count [expr $count + 1]
      if {$count > 4} { 
	  set count 0
	  pack $w.object.$fn -side top -expand 1 -fill both -padx 1m -pady 1m
	  set fn [expr $fn + 1]
	  frame $w.object.$fn  -bd 1
      }
      
      set o_name [lindex $o_type 0]
      set o_value [lindex $o_type 1]
      radiobutton $w.object.$fn.b$o_value  -value $o_value   -variable object_value -text "$o_name"
      pack $w.object.$fn.b$o_value -side left -fill both -expand 1
  }     
  
  pack $w.object.$fn -side top -expand 1 -fill both -padx 1m -pady 1m
  pack $w.object -side top -expand 1 -fill both 
  
  set val 1
  # Select protocol
  frame $w.protocol -relief raised -bd 1
  message $w.protocol.label -width 2i -text "Select protocol " 
  radiobutton $w.protocol.magnus  -value 1   -variable val -text "Magnus"
  radiobutton $w.protocol.gap  -value 2 -state disabled  -variable val -text "GAP"
    
  pack $w.protocol.label -side left -expand 1 -fill both -padx 1m -pady 1m
  pack $w.protocol.magnus -side left -fill both -expand 1
  pack $w.protocol.gap -side left -fill both -expand 1
  pack $w.protocol -side top -expand 1 -fill both -padx 1m 

  # Make OK and Cancel buttons

  frame $w.frame3 -relief raised -bd 1
  frame $w.frame3.frame -relief flat

  button $w.frame3.frame.param -relief raised -bd 2 -text "Parameters" -width 8\
      -command "editPackageParameters \{$title\} "

  button $w.frame3.frame.cancel -relief raised -bd 2 -text "Cancel" -width 8\
         -command "destroy $w"
  button $w.frame3.frame.ok -relief raised -bd 2 -text "OK" -width 8 \
	  -command "definePackageDialog_OK $w  \{$message\}"
  pack $w.frame3.frame.param $w.frame3.frame.ok $w.frame3.frame.cancel -side left\
    -expand 1 -padx 3m -pady 3m -ipady 1m
  pack $w.frame3.frame -fill x
  pack $w.frame3 -fill x

}


proc editPackageParameters { title } {
    global paramList
    
    set w  [uniqueDialogName]
    toplevel $w
    grab $w
    wm title $w "Parameters for $title"
    wm iconname $w "Dialog"
    
    # Make descr field
    
    frame $w.frame1 -relief raised -bd 1
    message $w.frame1.msg -width 3i -text "List of parameters:"
    pack $w.frame1.msg -side top -expand 1 -fill both -padx 1m -pady 1m
    pack $w.frame1 -side top -expand 1 -fill both
    
    frame $w.topframe -relief raised -bd 1
    
    listbox $w.topframe.list -height 20 -width 60
    
    pack $w.topframe.list -side top -expand 1 -fill both -padx 1m -pady 1m
    # show the main frame
    pack $w.topframe -side top -expand 1 -fill both -padx 1m 
 
    for {set i 0} {$i<[llength $paramList]} {incr i} {
	set n [lindex $paramList $i]
	incr i
	set t [lindex $paramList $i]
	$w.topframe.list insert end "$n   $t"
    }
    
    
    # Make Delete and Cancel buttons
    frame $w.frame3 -relief raised -bd 1
    frame $w.frame3.frame -relief flat
    button $w.frame3.frame.cancel -relief raised -bd 2 -text "Close" -width 8\
	-command "destroy $w"
    
    button $w.frame3.frame.add -relief raised -bd 2 -text "Add" -width 8 \
	-command "editPackageParametersAdd $w"
    
    button $w.frame3.frame.del -relief raised -bd 2 -text "Delete" -width 8 \
	-command "editPackageParametersDel $w"
    
    pack $w.frame3.frame.add $w.frame3.frame.del  $w.frame3.frame.cancel -side left  -expand 1 -padx 3m -pady 3m -ipady 1m
    pack $w.frame3.frame -fill x
    pack $w.frame3 -fill x
    
}

proc editPackageParametersAdd { pw } {
    

    #set w [uniqueDialogName]
    set w $pw.add
    toplevel $w
#    grab $w
    wm title $w "Add"
    wm iconname $w "Dialog"
    
    # Make descr field
    
    frame $w.frame1 -relief raised -bd 1
    message $w.frame1.msg -width 3i -text "Add a new parameter"
    pack $w.frame1.msg -side top -expand 1 -fill both -padx 1m -pady 1m
    pack $w.frame1 -side top -expand 1 -fill both
    
    
    # Make entry fields:
    
    # Enter name field
    frame $w.namef -relief raised -bd 1 
    message $w.namef.label -width 2i -text "Enter parameter name " 
    entry $w.namef.text -relief sunken -width 40
    
    pack $w.namef.label -side left -expand 1 -fill both -padx 1m -pady 1m
    pack $w.namef.text -side left -fill both -expand 1
    pack $w.namef -side top -expand 1 -fill both  -pady 1m
    


    # Select parameter type
    global type
    set type -1
    frame $w.type -relief raised -bd 1
    message $w.type.label -width 2i -text "Select type " 
    radiobutton $w.type.int  -value "Integer"   -variable type -text "Integer"
    radiobutton $w.type.string  -value "String"  -variable type -text "String"
    radiobutton $w.type.bool  -value "Boolean"  -variable type -text "Boolean"

    pack $w.type.label -side left -expand 1 -fill both -padx 1m -pady 1m
    pack $w.type.int -side left -fill both -expand 1
    pack $w.type.string -side left -fill both -expand 1
    pack $w.type.bool -side left -fill both -expand 1
    pack $w.type -side top -expand 1 -fill both -padx 1m 
    
    # Make OK and Cancel buttons
    
    frame $w.frame3 -relief raised -bd 1
    frame $w.frame3.frame -relief flat
    
    button $w.frame3.frame.cancel -relief raised -bd 2 -text "Cancel" -width 8\
	-command "destroy $w"
    button $w.frame3.frame.ok -relief raised -bd 2 -text "OK" -width 8 \
	-command "editPackageParametersAdd_confirm $w  $pw"
    pack $w.frame3.frame.ok $w.frame3.frame.cancel -side left\
	-expand 1 -padx 3m -pady 3m -ipady 1m
    pack $w.frame3.frame -fill x
    pack $w.frame3 -fill x
    
}

proc editPackageParametersAdd_confirm { w  pw} {
    global paramList
    global type

     
    # get entered fields and check if correct
    # name field
    set name [$w.namef.text get] 
    #  set namef [string trim $namef " "]
    if { [string length $name] > 80 } {
	alert "Parameter name must be less than 80 characters"
	return 
    } 
    if { [string length $name] <= 0 } {
	alert "Please enter a name"
	return 
    }     

    
    #  type field
    if { $type == -1 } {
	alert "Please select  type"
	return 
    } 


    lappend paramList $name
    lappend paramList $type
    $pw.topframe.list insert end "$name   $type" 
    destroy $w

}


proc editPackageParametersDel { w }  {
    global paramList

    set i [$w.topframe.list curselection]
    if { [string length $i] == 0} { return } 

   if {[tk_dialog .d {Confirm} \
        "Delete parameter ?" \
        question 1 Yes No]} {
      return
    }

    $w.topframe.list delete $i

    set i [expr $i*2]
    set paramList [lreplace $paramList $i [expr $i+1] ]
    
}

proc getFileName {w} {
  # $w is the path of the text widget
  set fileName [openfile [uniqueDialogName]]
  if {$fileName != ""} {
    $w delete 0 end
    $w insert 1  $fileName
  }
}



proc definePackageDialog_OK {w message} {
    global checkin_value
    global object_value
    global paramList
  
    # get entered fields and check if correct
    # name field
    set namef [$w.namef.text get] 
    set namef [string trim $namef " "]
    if { [string length $namef] > 20 } {
	alert "Package name must be less than 20 characters"
	return 
    } 
    if { [string length $namef] <= 0 } {
	alert "Please enter a name"
	return 
    }     
    set namef [IPCChars [$w.namef.text get] ]

    # command field
    set command [$w.command.text get]
    set command [string trim $command " "]
    if { [string length $command] > 128 } {
	alert "Command must be less than 128 characters"
	return 
    } 
    if { [string length $command] <= 0 } {
	alert "Please enter a command"
	return 
    } 
    
#    if { ![file exists $command] } {
#	alert "File $command does not exist"
#	return 
#    }      

   set command [IPCChars [$w.command.text get] ]

    # checkin type field
    if { $checkin_value == -1 } {
	alert "Please select checkin type"
	return 
    } 
    if { $object_value == -1 } {
	alert "Please select object type"
	return 
    }     

    destroy $w

  if {$message == ""} {
      return 
  } else {
      set toSM "\"$message $namef $command $checkin_value $object_value "
      set l [llength $paramList]
      set l2 [expr $l / 2]
      set toSM "$toSM $l2"
      for {set i 0} {$i < $l} {incr i} {
	  set p [IPCChars [lindex $paramList $i] ]
	  incr i
	  set t [IPCChars [lindex $paramList $i] ]
	  set toSM "$toSM $p $t"
      }
      set toSM "$toSM \""
      eval [concat toSessionManager $toSM]

      

  }
}

proc editPackagesDialog { title ch_types filenames message }  {
 

  set w  [uniqueDialogName]
  toplevel $w
  wm title $w $title
  wm iconname $w "Dialog"
  
  # Make descr field

  frame $w.frame1 -relief raised -bd 1
  message $w.frame1.msg -width 3i -text "Select package form the list"
  pack $w.frame1.msg -side top -expand 1 -fill both -padx 1m -pady 1m
  pack $w.frame1 -side top -expand 1 -fill both

  frame $w.topframe -relief raised -bd 1

  # Select checkin type
  global checkin_value 
  set checkin_value 0
  set count 0
  set fn  0
  frame $w.topframe.checkin -relief raised -bd 1
  message $w.topframe.checkin.label -width 2i -text "Select checkin type " 
  pack $w.topframe.checkin.label -side top -expand 1 -fill both -padx 1m -pady 1m

  frame $w.topframe.checkin.$fn -bd 1

  foreach ch_type $ch_types {
      set count [expr $count + 1]
      if {$count > 0} { 
	  set count 0
	  pack $w.topframe.checkin.$fn -side top -expand 1 -fill both -padx 1m -pady 1m
	  set fn [expr $fn + 1]
	  frame $w.topframe.checkin.$fn  -bd 1
      }
      
      set c_name [lindex $ch_type 0]
      set c_value [lindex $ch_type 1]
      radiobutton $w.topframe.checkin.$fn.b$c_value  -value $c_value   -variable checkin_value -text "$c_name" -command "switchList $w $c_value"
      pack $w.topframe.checkin.$fn.b$c_value -side left
      
      listbox $w.topframe.list$c_value -height 20 -width 60

  }     
  
  pack $w.topframe.checkin.$fn -side top -expand 1 -fill both -padx 1m -pady 1m
  pack $w.topframe.checkin -side right -expand 1 -fill both -padx 1m 
 

  # lists of packages
  global object_id
  foreach filename $filenames {
      set i 0
      set f [open [lindex $filename 0] r]
      set object_type [gets $f]
      while {![eof $f] } {
	  set line [gets $f]
	  if {[regexp {(.*)([0-9])} $line tmp str ind] == 1} {
	      set k [$w.topframe.list$ind index end]
	      set object_id($ind$k,id) $object_type
	      set object_id($ind$k,i) $i
	      $w.topframe.list$ind insert end $str
	      set i [expr $i+1]
	  }
      }
  }
  
  global current_packed
  set current_packed $w.topframe.list0
  pack $current_packed -side left -expand 1 -fill both -padx 1m   

  # show the main frame
  pack $w.topframe -side top -expand 1 -fill both -padx 1m 
  

  # Make Delete and Cancel buttons

  frame $w.frame3 -relief raised -bd 1
  frame $w.frame3.frame -relief flat
  button $w.frame3.frame.cancel -relief raised -bd 2 -text "Cancel" -width 8\
         -command "destroy $w"

  button $w.frame3.frame.view -relief raised -bd 2 -text "View" -width 8 \
      -command "confirmPackageDialog_View $w   \{$message\}"

  button $w.frame3.frame.del -relief raised -bd 2 -text "Delete" -width 8 \
      -command "confirmPackageDialog_Del $w   \{$message\}"

  pack $w.frame3.frame.del $w.frame3.frame.view $w.frame3.frame.cancel -side left\
    -expand 1 -padx 3m -pady 3m -ipady 1m
  pack $w.frame3.frame -fill x
  pack $w.frame3 -fill x

}

proc confirmPackageDialog_View { w  message}  {
   global current_packed
   global checkin_value
   global object_id

   set l [$current_packed curselection]
   eval [concat toSessionManager "\" $message $object_id($checkin_value$l,id)  $object_id($checkin_value$l,i)  view \""]
}

proc editPackageDialog_Del { dw  message}  {
   global current_packed
   global checkin_value
   global object_id

  set w  [uniqueDialogName]
  toplevel $w
  wm title $w ""
  wm iconname $w "Dialog"
  
  # Make descr field

  frame $w.frame1 -relief raised -bd 1
  message $w.frame1.msg -width 3i -text "Press Ok to remove package"
  pack $w.frame1.msg -side top -expand 1 -fill both -padx 1m -pady 1m
  pack $w.frame1 -side top -expand 1 -fill both



  frame $w.frame3 -relief raised -bd 1
  frame $w.frame3.frame -relief flat
  button $w.frame3.frame.cancel -relief raised -bd 2 -text "Cancel" -width 8\
         -command "destroy $w"
  button $w.frame3.frame.del -relief raised -bd 2 -text "Delete" -width 8 \
      -command "confirmPackageDialog_Del $dw $w  \{$message\}"
  pack $w.frame3.frame.del $w.frame3.frame.cancel -side left\
    -expand 1 -padx 3m -pady 3m -ipady 1m
  pack $w.frame3.frame -fill x
  pack $w.frame3 -fill x

}

proc confirmPackageDialog_Del { w  message}  {
   global current_packed
    global checkin_value
   global object_id

   if {[tk_dialog .d {Confirm} \
        "Delete package ?" \
        question 1 Yes No]} {
      return
    }

   set l [$current_packed curselection]
  eval [concat toSessionManager "\" $message $object_id($checkin_value$l,id)  $object_id($checkin_value$l,i)  delete \""]
#   #$current_packed delete [lindex $l 0]	
#   $current_packed delete 0 end
	
#   # lists of packages
  
#   foreach filename $filenames {
#        set i 0
#        set f [open [lindex $filename 0] r]
#        set object_type [gets $f]
#        while {![eof $f] } {
#  	  set line [gets $f]
#  	  if {[regexp {(.*)([0-9])} $line tmp str ind] == 1} {
# 	      if { $ind == $checkin_value } {
# 		  set k [$current_packed index end]
# 		  set object_id($ind$k,id) $object_type
# 		  set object_id($ind$k,i) $i
# 		  $current_packed insert end $str
# 		  set i [expr $i+1]
# 	      }
#  	  }
#        }
#    }
 # destroy $w1
  destroy $w
}

proc switchList { w  index } {
  global current_packed
  pack forget $current_packed

  set to_pack $w.topframe.list$index
    
  pack $to_pack -side left -expand 1 -fill both -padx 1m   
  set current_packed $to_pack
    
}

