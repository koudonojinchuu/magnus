##
##   $Id: editing.tcl 96 2006-03-24 17:59:50Z ybryukhov $
##

## Copyright (C) 1995 The New York Group Theory Cooperative
## See magnus/doc/COPYRIGHT for the full notice.

## Contents: Procedures which support emacs, Macintosh, and X style
##            editing in text and entry widgets.
##
## Principal Author: Roger Needham
##
## Status: In progress.
##
## Revision History:
##
## Bugs:
##
## * Editing bindings (except mousing) do not work in read-only text.
##


################################################################################
##
## The standard global edit menu:

proc makeEditMenu {w {mode "read-write"} } {

  # $w is the path of the desired edit menu. This creates the edit menu
  # but does not pack it.

  $w add cascade -label Edit -menu $w.edit

  menu $w.edit
  if {$mode != "read-only"} {
    $w.edit add command -label Undo -state disabled
    $w.edit add separator
    $w.edit add command -label Cut -command editCut
  }
  $w.edit add command -label Copy -command editCopy
  if {$mode != "read-only"} {
    $w.edit add command -label Paste -command editPaste
    $w.edit add command -label Clear -command editClear
  }
  $w.edit add separator
  $w.edit add command -label "Search..." -command editSearch
  if {$mode != "read-only"} {
    $w.edit add command -label "Replace..." -command editReplace -state disabled
  }
  $w.edit add separator
  $w.edit add command -label "Help with editing" \
    -command "showHelp {editing::Editing Text in Magnus}"
}


proc editCut {} {
  set w [selection own]
  if {$w != ""} {
    set s ""
    catch { set s [selection get] }
    if {$s != ""} {
      push_kill_ring $s

      switch -exact [winfo class $w] {
  
        Text {
          catch {
            $w delete sel.first sel.last
          }
          $w tag remove sel 0.0 end
          $w mark unset theMark
        }
  
        Entry {
          catch {
            $w delete sel.first sel.last
            $w select clear; selection clear $w
          }
          tkEntrySeeInsert $w
        }
      }
    }
  }
}


proc editCopy {} {
  set w [selection own]
  if {$w != ""} {
    set s ""
    catch { set s [selection get] }
    if {$s != ""} {
      push_kill_ring $s
    }
  }
}


proc editPaste {} {
  global clipboard

  set w [focus]
  if {$w != ""} {
    switch -exact [winfo class $w] {

      Text {
        $w insert insert $clipboard
        $w yview -pickplace insert
      }

      Entry {
        $w insert insert $clipboard
        tkEntrySeeInsert $w
      }
    }
  }
}


proc editClear {} {
  set w [selection own]
  if {$w != ""} {

    switch -exact [winfo class $w] {
  
      Text {
        catch {
          $w delete sel.first sel.last
        }
        $w tag remove sel 0.0 end
        $w mark unset theMark
      }
  
      Entry {
        catch {
          $w delete sel.first sel.last
          $w select clear; selection clear $w
        }
        tkEntrySeeInsert $w
      }
    }
  }
}


proc editSearch {} {

  set w .searchDialog
  if {[winfo exists $w]} {
    wm deiconify $w
    raise $w
    return
  }

  toplevel $w
  wm title $w "Search"
  wm iconname $w "Search"
  #@db wm minsize $w 100 100

  frame $w.frame1 -relief raised -bd 2
  label $w.frame1.label -text "Search for:" -relief flat
  entry $w.frame1.entry -width 30 -relief sunken
  frame $w.frame2 -relief raised -bd 2
  button $w.frame2.close -text Close -relief raised -bd 2 -width 8\
    -command "destroy $w"
  button $w.frame2.search -text Search -relief raised -bd 2 -width 8 \
    -command "editDoSearch [focus]"

  pack $w.frame1.label -side left -padx 2m -pady 2m
  pack $w.frame1.entry -side left -expand 1 -fill x -padx 2m -pady 2m
  pack $w.frame1 -side top -expand 1 -fill both
  pack $w.frame2.close $w.frame2.search -side left \
    -expand 1 -padx 3m -pady 3m -ipady 1m
  pack $w.frame2 -side top -fill x

  focus $w.frame1.entry
}


proc editDoSearch {focus} {

  set e .searchDialog.frame1.entry
  set text [$e get]
  set w [focus]
  if {$w == $e} {set w $focus}
  if {$w != "" && $w != "none"} {
    switch -exact [winfo class $w] {

      Text {
        set s [$w get insert end]
        set indices ""
        eval [list regexp -indices "($text)" $s dummy indices]
        unset s
        if {$indices != ""} {
	  set sss [$w index "insert + [lindex $indices 0] c"]
	  scan $sss "%d.%d" line pos 
	  $w yview [expr $line-1]
          $w tag remove sel 0.0 end
          catch {
            $w mark set theMark [$w index "insert + [lindex $indices 0] c"]
            set i [$w index "insert + [lindex $indices 1] c"]
            textSelectTo $w $i
            $w mark set insert "$i + 1 c"
          }
        }
      }

#@rn Implement entry searching
      Entry {

      }
    }
  }
}


proc editReplace {} {


}


################################################################################
##
## Global key bindings:

# Find:
bind all <F19> editSearch


################################################################################
##
## Emacs bindings for entry widgets:

# requests:
#
# Control-l
# Meta-d
# Control-x u
#
# M-C-f
# M-C-b
#
# Control-t
# Meta-t


# Motion bindings:

bind Entry <Control-f> {
  %W icursor [expr [%W index insert] + 1]
  if {"[selection own]" == "%W"} {
    %W select to insert
  }
  tkEntrySeeInsert %W
}
bind Entry <Right> {
  %W icursor [expr [%W index insert] + 1]
  if {"[selection own]" == "%W"} {
    %W select to insert
  }
  tkEntrySeeInsert %W
}

bind Entry <Control-b> {
  %W icursor [expr [%W index insert] - 1]
  if {"[selection own]" == "%W"} {
    %W select to insert
  }
  tkEntrySeeInsert %W
}
bind Entry <Left> {
  %W icursor [expr [%W index insert] - 1]
  if {"[selection own]" == "%W"} {
    %W select to insert
  }
  tkEntrySeeInsert %W
}

bind Entry <Control-a> {
  %W icursor 0
  if {"[selection own]" == "%W"} {
    %W select to insert
  }
  tkEntrySeeInsert %W
}

bind Entry <Control-e> {
  %W icursor end
  if {"[selection own]" == "%W"} {
    %W select to insert
  }
  tkEntrySeeInsert %W
}

bind Entry <Control-p> {
  %W icursor 0
  if {"[selection own]" == "%W"} {
    %W select to insert
  }
  tkEntrySeeInsert %W
}
bind Entry <Up> {
  %W icursor 0
  if {"[selection own]" == "%W"} {
    %W select to insert
  }
  tkEntrySeeInsert %W
}

bind Entry <Control-n> {
  %W icursor end
  if {"[selection own]" == "%W"} {
    %W select to insert
  }
  tkEntrySeeInsert %W
}
bind Entry <Down> {
  %W icursor end
  if {"[selection own]" == "%W"} {
    %W select to insert
  }
  tkEntrySeeInsert %W
}

bind Entry <Meta-v> {
  %W icursor 0
  if {"[selection own]" == "%W"} {
    %W select to insert
  }
  tkEntrySeeInsert %W
}

bind Entry <Control-v> {
  %W icursor end
  if {"[selection own]" == "%W"} {
    %W select to insert
  }
  tkEntrySeeInsert %W
}

bind Entry <Meta-less> {
  %W icursor 0
  if {"[selection own]" == "%W"} {
    %W select to insert
  }
  tkEntrySeeInsert %W
}

bind Entry <Meta-greater> {
  %W icursor end
  if {"[selection own]" == "%W"} {
    %W select to insert
  }
  tkEntrySeeInsert %W
}

bind Entry <Meta-f> {
  set s [%W get]
  set i [%W index insert]
  set j 0
  while {$j == 0} {
    set j [string first " " [string range $s $i end]]
    incr i
  }
  if {$j == -1} {
    %W icursor end
  } else {
    %W icursor [expr $j + $i - 1]
  }
  if {"[selection own]" == "%W"} {
    %W select to insert
  }
  tkEntrySeeInsert %W
}

bind Entry <Meta-b> {
  set s [%W get]
  set i [%W index insert]
  set j $i
  while {$j == $i} {
    set j [string last " " [string range $s 0 [expr $i - 1]]]
    incr i -1
  }
  if {$j == -1} {
    %W icursor 0
  } else {
    %W icursor [expr $j + 1]
  }
  if {"[selection own]" == "%W"} {
    %W select to insert
  }
  tkEntrySeeInsert %W
}


# Editing bindings:

#bind Entry <Button-2> {
#  catch { %W insert [selection get]; tkEntrySeeInsert %W }
#}

bind Entry <Control-g> {%W select clear; selection clear %W}

bind Entry <Control-space> {
  %W select clear; selection clear %W
  %W select from insert
  %W select to [expr [%W index insert] - 1]
}

bind Entry <Control-k> {
  set s [%W get]
  set i [%W index insert]
  set j [%W index end]
  push_kill_ring [string range $s $i $j]
  %W delete insert end
  tkEntrySeeInsert %W
}

bind Entry <Control-d> {
  %W delete insert
}

bind Entry <Meta-BackSpace> {entry_binding_meta_bs %W}
bind Entry <Meta-Delete> {entry_binding_meta_bs %W}

bind Entry <Control-w> {
  set s [%W get]
  set i [%W index sel.first]
  set j [%W index sel.last]
  push_kill_ring [string range $s $i $j]
  %W delete sel.first sel.last
  %W select clear; selection clear %W
  tkEntrySeeInsert %W
}
# Cut:
bind Entry <F20> {
  set s [%W get]
  set i [%W index sel.first]
  set j [%W index sel.last]
  push_kill_ring [string range $s $i $j]
  %W delete sel.first sel.last
  %W select clear; selection clear %W
  tkEntrySeeInsert %W
}

bind Entry <Meta-w> {
  if {"[selection own]" == "%W"} {
    push_kill_ring [selection get]
    %W select clear; selection clear %W
  }
}
# Copy:
bind Entry <F16> {
  if {"[selection own]" == "%W"} {
    push_kill_ring [selection get]
    %W select clear; selection clear %W
  }
}

bind Entry <Control-y> {
  global clipboard
  %W insert insert $clipboard
  tkEntrySeeInsert %W
}
# Paste:
bind Entry <F18> {
  global clipboard
  %W insert insert $clipboard
  tkEntrySeeInsert %W
}


# Helper procs:

proc entry_binding_meta_bs {entry} {

  set s [$entry get]
  set i [$entry index insert]
  set j $i
  while {$j == $i} {
    set j [string last " " [string range $s 0 [expr $i - 1]]]
    incr i -1
  }
  if {$j == -1} {
    $entry delete 0 [expr [$entry index insert] - 1]
  } else {
    set i [expr $j + 1]
    set j [expr [$entry index insert] - 1]
    push_kill_ring [string range $s $i $j]
    $entry delete $i $j
  }
  tkEntrySeeInsert %W
}


proc push_kill_ring {text} {
  global clipboard
  set clipboard $text
}




################################################################################
##
## Emacs bindings for text widgets:

#@@@rn
# Problem: we want the emacs management of the selection to be compatible
# with X mousing, but we have to set the `mark' with each Button-1 (to
# prepare for drag selecting), which is incompatible. So, we have to
# bash these tk bindings:

set set_mark_cmd {%W mark set theMark [%W index anchor]}
bind Text <B1-Motion> +$set_mark_cmd
bind Text <Double-1> +$set_mark_cmd
bind Text <Triple-1> +$set_mark_cmd
bind Text <Shift-1> +$set_mark_cmd
bind Text <Shift-B1-Motion> +$set_mark_cmd
unset set_mark_cmd

set unset_mark_cmd { %W mark unset theMark }
bind Text <1> +$unset_mark_cmd
unset unset_mark_cmd

# Motion bindings:

bind Text <Control-f> {
  if {[catch {%W index theMark}] == 0} {
    textSelectTo %W insert
  }
  %W mark set insert "insert + 1 chars"
  %W yview -pickplace insert
}
bind Text <Right> {
  if {[catch {%W index theMark}] == 0} {
    textSelectTo %W insert
  }
  %W mark set insert "insert + 1 chars"
  %W yview -pickplace insert
}

bind Text <Control-b> {
  %W mark set insert "insert - 1 chars"
  if {[catch {%W index theMark}] == 0} {
    textSelectTo %W insert
  }
  %W yview -pickplace insert
}
bind Text <Left> {
  %W mark set insert "insert - 1 chars"
  if {[catch {%W index theMark}] == 0} {
    textSelectTo %W insert
  }
  %W yview -pickplace insert
}

bind Text <Control-a> {
  %W mark set insert "insert linestart"
  if {[catch {%W index theMark}] == 0} {
    textSelectTo %W insert
  }
  %W yview -pickplace insert
}

bind Text <Control-e> {
  %W mark set insert "insert lineend"
  if {[catch {%W index theMark}] == 0} {
    textSelectTo %W insert
  }
  %W yview -pickplace insert
}


bind Text <Control-p> {
  %W mark set insert "insert - 1 lines"
  if {[catch {%W index theMark}] == 0} {
    textSelectTo %W insert
  }
  %W yview -pickplace insert
}
bind Text <Up> {
  %W mark set insert "insert - 1 lines"
  if {[catch {%W index theMark}] == 0} {
    textSelectTo %W insert
  }
  %W yview -pickplace insert
}

bind Text <Control-n> {
  %W mark set insert "insert + 1 lines"
  if {[catch {%W index theMark}] == 0} {
    textSelectTo %W insert
  }
  %W yview -pickplace insert
}
bind Text <Down> {
  %W mark set insert "insert + 1 lines"
  if {[catch {%W index theMark}] == 0} {
    textSelectTo %W insert
  }
  %W yview -pickplace insert
}

#@rn Hacky:
bind Text <Meta-v> {
  %W mark set insert "insert - 10 lines"
  if {[catch {%W index theMark}] == 0} {
    textSelectTo %W insert
  }
  %W yview -pickplace insert
}

bind Text <Control-v> {
  %W mark set insert "insert + 10 lines"
  if {[catch {%W index theMark}] == 0} {
    textSelectTo %W insert
  }
  %W yview -pickplace insert
}

bind Text <Meta-less> {
  %W mark set insert 1.0
  if {[catch {%W index theMark}] == 0} {
    textSelectTo %W insert
  }
  %W yview -pickplace insert
}

bind Text <Meta-greater> {
  %W mark set insert end
  if {[catch {%W index theMark}] == 0} {
    textSelectTo %W insert
  }
  %W yview -pickplace insert
}

#@rn Not right:

bind Text <Meta-f> {
  %W mark set insert "insert wordend + 1 chars"
  if {[catch {%W index theMark}] == 0} {
    textSelectTo %W insert
  }
  %W yview -pickplace insert
}

bind Text <Meta-b> {
  %W mark set insert "insert wordstart - 1 chars"
  if {[catch {%W index theMark}] == 0} {
    textSelectTo %W insert
  }
  %W yview -pickplace insert
}

#@rn Doesn't cut it:
bind Text <Control-l> {
  %W yview -pickplace insert
}


# Editing bindings:


#bind Text <Button-2> {
#  catch { %W insert  [selection get] }
#  %W yview -pickplace insert
#}

bind Text <Control-g> {
  %W tag remove sel 0.0 end
  %W mark unset theMark
}

bind Text <Control-space> {
  %W tag remove sel 0.0 end
  %W mark set theMark insert
}

#@rn Doesn't kill empty lines
bind Text <Control-k> {
  push_kill_ring [%W get insert "insert lineend"]
  %W delete insert "insert lineend"
}

bind Text <Control-d> {
  %W delete insert
}

bind Text <Meta-BackSpace> {text_binding_meta_bs %W}
bind Text <Meta-Delete> {text_binding_meta_bs %W}

bind Text <Control-w> {
  catch {
    push_kill_ring [%W get sel.first sel.last]
    %W delete sel.first sel.last
  }
  %W tag remove sel 0.0 end
  %W mark unset theMark
}
# Cut:
bind Text <F20> {
  catch {
    push_kill_ring [%W get sel.first sel.last]
    %W delete sel.first sel.last
  }
  %W tag remove sel 0.0 end
  %W mark unset theMark
}

bind Text <Meta-w> {
  catch { push_kill_ring [%W get sel.first sel.last] }
  %W tag remove sel 0.0 end
  %W mark unset theMark
}
# Copy:
bind Text <F16> {
  catch { push_kill_ring [%W get sel.first sel.last] }
  %W tag remove sel 0.0 end
  %W mark unset theMark
}

bind Text <Control-y> {
  global clipboard
  %W insert insert $clipboard
  %W yview -pickplace insert
}
# Paste:
bind Text <F18> {
  global clipboard
  %W insert insert $clipboard
  %W yview -pickplace insert
}


# Helper procs:

proc textSelectTo {w index} {
  if [catch {$w index theMark}] {
    $w mark set theMark $index
  }

  if [$w compare $index < theMark] {
    set first $index
    set last theMark
  } else {
    set first theMark
    set last [$w index $index+1c]
  }

  $w tag remove sel 0.0 $first
  $w tag add sel $first $last
  $w tag remove sel $last end
}

#@rn Implement:
proc text_binding_meta_bs {w} {

}
