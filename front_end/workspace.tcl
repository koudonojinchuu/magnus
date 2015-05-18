##
##   $Id: workspace.tcl 96 2006-03-24 17:59:50Z ybryukhov $
##

## Copyright (C) 1995 The New York Group Theory Cooperative
## See magnus/doc/COPYRIGHT for the full notice.

## Contents: Code supporting the workspace window.
##
## Principal Author: Roger Needham, Dmitry Bormotov
##
## Status: In progress.
##
## Revision History:
##
## 03/96 Dmitry B. made porting to tcl7.4/tk4.0 
##
## 01/98 @am added icons for enumerator objects
##
#@rn Would be nice if workspace_clearSelection didn't trigger
#@rn unneeded calls to updateMenus with empty signature.

## destination constants
set dest(none) 0
set dest(from) 1
set dest(into) 2

proc makeWorkspace {} {
  global tcl_path

  set w .workspace
  toplevel $w
  wm title $w "Workspace"
  wm iconname $w "Workspace"
  #@db wm minsize $w 200 200
  wm geometry $w -18+20


  # Put in menu bar

  menu $w.mbar
  $w configure -menu $w.mbar

  $w.mbar add cascade -label "Check-in" -menu $w.mbar.checkin
  $w.mbar add cascade -label "Tools" -menu $w.mbar.tools
  $w.mbar add cascade -label "Make" -menu $w.mbar.make
  $w.mbar add cascade -label "Packages" -menu $w.mbar.packages

  $w.mbar add cascade -label "Database" -menu $w.mbar.database

  if { [inTestingMode] == 1 } {
      $w.mbar add cascade -label "Testing" -menu $w.mbar.testing
  }
  $w.mbar add cascade -label Help -menu $w.mbar.help

  menu $w.mbar.checkin

  menu $w.mbar.tools

  menu $w.mbar.make

  menu $w.mbar.packages

  menu $w.mbar.database

  menu $w.mbar.testing 

  menu $w.mbar.help
    $w.mbar.help add command -label "About the Workspace" \
      -command "showHelp {aboutWorkspace::About the Workspace}"

  # Make the canvas

  set c .workspace.frame2.c

  frame $w.frame2 -relief raised -bd 2
  pack $w.frame2 -side top -fill both -expand 1

  # The constant 1000 is hard-wired in workspace_itemStartDrag, below.
  
  canvas $c -width 600 -height 600 -scrollregion {0 0 1000 1000} \
         -xscrollcommand "$w.frame2.hscroll set" \
         -yscrollcommand "$w.frame2.vscroll set" \
	 -highlightthickness 0
  scrollbar $w.frame2.vscroll -relief sunken -command "$c yview"
  scrollbar $w.frame2.hscroll -orient horiz -relief sunken -command "$c xview"
  pack $w.frame2.vscroll -side right -fill y
  pack $w.frame2.hscroll -side bottom -fill x
  pack $c -fill both -expand 1

  bind $c <Button-1> {
    if {[.workspace.frame2.c find withtag current] == ""} {
      workspace_clearSelection
      workspace_startSelectionBox %x %y
    }
  }
  bind $c <B1-Motion> {
    if {[.workspace.frame2.c find withtag current] == ""} {
      workspace_selectionBox %x %y
    }
  }
  bind $c <ButtonRelease-1> "workspace_killSelectionBox 1"

#@rn I once saw this cause a crash, so bugger it:
#  bind $c <Button-2> {
#    if {[.workspace.frame2.c find withtag current] == ""} {
#      .workspace.frame2.c scan mark %x %y
#    }
#  }
#  bind $c <B2-Motion> {
#    if {[.workspace.frame2.c find withtag current] == ""} {
#      .workspace.frame2.c scan dragto %x %y
#    }
#  }

  bind $c <Button-3> {
    if {[.workspace.frame2.c find withtag current] == ""} {
      workspace_startSelectionBox %x %y
    }
  }
  bind $c <B3-Motion> {
    if {[.workspace.frame2.c find withtag current] == ""} {
      workspace_selectionBox %x %y
    }
  }
  bind $c <ButtonRelease-3> "workspace_killSelectionBox 3"
    
  #@db
  $c bind icon <Any-Enter> {
    #.workspace.frame2.c itemconfigure current -fill gray

    set icon [.workspace.frame2.c find withtag current]
    set nonactive [.workspace.frame2.c find withtag nonactive]

#    if {[lsearch $nonactive $icon] != -1} return

    set parentIcon ""
    catch { set parentIcon [get_iconOfCanvasWidget $icon] } 
    if {$parentIcon != ""} {
	set icon $parentIcon
    }

#    if { ![info exist workspace_objectDependencies($icon)] } return
      
    set grayObjects $workspace_objectDependencies($icon)
    set grayObjects [linsert $grayObjects 0 $icon]

    foreach object $grayObjects {
      .workspace.frame2.c itemconfigure $object -fill gray
    }
    
    set sources [get_edgeSources $icon]
    foreach edge $sources {
	color_edge $edge gray
    }

    
    #@db temp

    #set deps $workspace_objectDependencies($icon)
    #set depsOID ""`
    #foreach object $deps {
    #   lappend depsOID [get_objectID $object]
    #}

    #postToLog "OID:  [get_objectID $icon]   Depends on:  $depsOID"
    #postToLog [get_objectID $icon]
  }

  #@db
  $c bind icon <Any-Leave> {

    set icon [.workspace.frame2.c find withtag current]
    set nonactive [.workspace.frame2.c find withtag nonactive]

    set parentIcon ""
    catch { set parentIcon [get_iconOfCanvasWidget $icon] } 
    if {$parentIcon != ""} {
	set icon $parentIcon
    }
#    if {[lsearch $nonactive $icon] != -1} return
      
#    if { ![info exist workspace_objectDependencies($icon)] } return
     
    set grayObjects $workspace_objectDependencies($icon)
    set grayObjects [linsert $grayObjects 0 $icon]

    foreach object $grayObjects {
      if {[lsearch $workspace_selection $object] != -1} {
	workspace_hiliteIcon 1 $object
      } else {
	workspace_hiliteIcon 0 $object
      }
    }

    set sources [get_edgeSources $icon]
    foreach edge $sources {
	color_edge $edge black
    }
    
  }
  
  $c bind icon <Button-1> {workspace_select}
  $c bind icon <Double-Button-1> \
  {openView [get_iconOfCanvasWidget [.workspace.frame2.c find withtag current]]}
  $c bind icon <Button-2> {workspace_itemStartDrag %x %y}
  $c bind icon <B2-Motion> {workspace_itemDrag %x %y}
  $c bind icon <Button-3> {workspace_selectAlso}

  # Make the minilog

  frame $w.frame3 -relief raised -bd 2 
  text $w.frame3.log -height 5 -width 25 -relief flat -state disabled \
       -yscrollcommand "$w.frame3.s set" -highlightthickness 0
  scrollbar $w.frame3.s -relief sunken -command "$w.frame3.log yview"
  pack $w.frame3.s -side right -fill y
  pack $w.frame3.log -fill both -expand 1 -padx 1m -pady 1m
  pack $w.frame3 -side top -fill both -expand 1

  bind .workspace.frame2.c <Control-Shift-Button-1> {
    if {![winfo exists .workspace.frame2.c.ee]} {
      button .workspace.frame2.c.ee -text "Do What I Mean" \
              -command {tk_dialog .ee "Query" "But what do you mean?" \
                        question 0 "I don't know"}
      .workspace.frame2.c create window %x %y -window .workspace.frame2.c.ee
      after 5000 [list destroy .workspace.frame2.c.ee]
    }
  }

  wm protocol $w WM_DELETE_WINDOW {
    alert "Please do not try to delete the workspace!"
  }
}


proc workspace_startSelectionBox {x y} {

  global workspace_selectionBox workspace_selectionBoxX workspace_selectionBoxY

  set c .workspace.frame2.c
  set workspace_selectionBoxX [$c canvasx $x]
  set workspace_selectionBoxY [$c canvasy $y]
  set workspace_selectionBox [$c create rectangle \
        $workspace_selectionBoxX $workspace_selectionBoxY \
        $workspace_selectionBoxX $workspace_selectionBoxY \
        -outline Black]
}


proc workspace_selectionBox {x y} {

  global workspace_selectionBox workspace_selectionBoxX workspace_selectionBoxY

  set c .workspace.frame2.c
  set x [$c canvasx $x]
  set y [$c canvasy $y]

  if {$x >= $workspace_selectionBoxX} {
    if {$y >= $workspace_selectionBoxY} {
      $c coords $workspace_selectionBox \
                $workspace_selectionBoxX $workspace_selectionBoxY $x $y
    } else {
      $c coords $workspace_selectionBox \
                $workspace_selectionBoxX $y $x $workspace_selectionBoxY
    }
  } else {
    if {$y >= $workspace_selectionBoxY} {
      $c coords $workspace_selectionBox \
                $x $workspace_selectionBoxY $workspace_selectionBoxX $y
    } else {
      $c coords $workspace_selectionBox \
                $x $y $workspace_selectionBoxX $workspace_selectionBoxY
    }
  }
}


proc workspace_killSelectionBox {button} {

  global workspace_selection workspace_selectionBox

  set c .workspace.frame2.c

  if {$workspace_selectionBox == 0} { return }

  set dX1 [lindex [$c coords $workspace_selectionBox] 0]
  set dY1 [lindex [$c coords $workspace_selectionBox] 1]
  set dX2 [lindex [$c coords $workspace_selectionBox] 2]
  set dY2 [lindex [$c coords $workspace_selectionBox] 3]

  foreach icon [$c find withtag icon] {
    # Make sure $icon is a real icon (not a component)
    if { [get_iconOfCanvasWidget $icon] == $icon } {
      set x [get_iconXPosition $icon]
      set y [get_iconYPosition $icon]
      if {$dX1 <= $x && $dY1 <= $y && $dX2 > $x && $dY2 > $y} {
        if {$button == 1} {
          lappend workspace_selection $icon
          workspace_hiliteIcon 1 $icon
        } else {
          set i [lsearch -exact $workspace_selection $icon]
          if {$i == -1} {
            lappend workspace_selection $icon
            workspace_hiliteIcon 1 $icon
          } else {
            set workspace_selection [lreplace $workspace_selection $i $i]
            workspace_hiliteIcon 0 $icon
          }
        }
      }
    }
  }

  $c delete $workspace_selectionBox
  set workspace_selectionBox 0
  updateMenus
}


proc workspace_itemStartDrag {x y} {

  global workspace_lastX workspace_lastY workspace_dragBoundingBox \
         workspace_selection

  if {$workspace_selection == ""} { return }

  set c .workspace.frame2.c

  set workspace_lastX [$c canvasx $x]
  set workspace_lastY [$c canvasy $y]
  set BB [eval "$c bbox $workspace_selection"]

  # This hard-wires the canvas size. See the canvas command, above.
  set workspace_dragBoundingBox \
      [list [expr $x - [lindex $BB 0]] [expr $y - [lindex $BB 1]] \
            [expr [$c canvasx 1000] - [lindex $BB 2] + $x] \
            [expr [$c canvasy 1000] - [lindex $BB 3] + $y]]
}


proc workspace_itemDrag {x y} {

  global workspace_lastX workspace_lastY workspace_selection \
         workspace_dragBoundingBox

  if {$workspace_selection == ""} { return }

  set c .workspace.frame2.c
  set x [$c canvasx $x]
  set y [$c canvasy $y]
  set X1 [lindex $workspace_dragBoundingBox 0]
  set Y1 [lindex $workspace_dragBoundingBox 1]
  set X2 [lindex $workspace_dragBoundingBox 2]
  set Y2 [lindex $workspace_dragBoundingBox 3]

  if {$x < $X1 || $y < $Y1 || $x > $X2 || $y > $Y2} { return }

  set deltaX [expr $x - $workspace_lastX]
  set deltaY [expr $y - $workspace_lastY]

  # Move whole selection.

  foreach icon $workspace_selection {

    $c move $icon $deltaX $deltaY

    foreach comp [get_iconComponents $icon] {
      $c move $comp $deltaX $deltaY
    }

    set X [expr [get_iconXPosition $icon] + $deltaX]
    set Y [expr [get_iconYPosition $icon] + $deltaY]
    set_iconPosition $icon $X $Y

    foreach edge [get_edgeSources $icon] {
	move_source_edge $edge $X $Y
    }
    foreach edge [get_edgeTargets $icon] {
	move_target_edge $edge $X $Y
    }
  }

  set workspace_lastX $x
  set workspace_lastY $y
}


proc workspace_drawIcon {iconID name x y} {

  set c .workspace.frame2.c

  switch -exact $iconID {

    group {
      set r [$c create oval -20 -20 20 20 -tags icon]
    }

    subgroup {
      set r [$c create oval -15 -15 15 15 -tags icon]
    }

    SetOfWords {
      set r [$c create oval -20 -15 20 15 -tags icon]
    }

    VectorOfWords {
      set r [$c create oval -20 -15 20 15 -tags icon]
    }

    map {
      set r [$c create oval -12 -12 12 12 -tags icon]
    }

    homomorphism {
      set r [$c create oval -12 -12 12 12 -tags icon]
    }

    elt {
      set r [$c create oval -10 -10 10 10 -tags icon]
    }

    permutation {
	set r [$c create rectangle -15 -8 15 8 -tags icon]
    }

    problem {
      set r [$c create rectangle -20 -10 20 10 -tags icon]
      set b [$c create oval 6 -4 14 4 -fill red -tags {icon nonactive}]
      $c move $b $x $y
      set_iconComponent $r $b
      set_iconOfCanvasWidget $r $b
    }

    enumerator_problem {
      set r [$c create rectangle -30 -10 20 10 -tags icon]
      set b [$c create oval 6 -4 14 4 -fill red -tags {icon nonactive}]
      set l1 [$c create line -26 -5 -8 -5 -tags {icon nonactive}]
      set l2 [$c create line -26 0 -8 0 -tags {icon nonactive}]
      set l3 [$c create line -26 5 -8 5 -tags {icon nonactive}]
      $c move $b $x $y
      $c move $l1 $x $y
      $c move $l2 $x $y
      $c move $l3 $x $y
      set_iconComponent $r $b
      set_iconComponent $r $l1
      set_iconComponent $r $l2
      set_iconComponent $r $l3
      set_iconOfCanvasWidget $r $b
      set_iconOfCanvasWidget $r $l1
      set_iconOfCanvasWidget $r $l2
      set_iconOfCanvasWidget $r $l3
   }

    enumerator_object {
      set r [$c create rectangle -10 -20 10 20 -tags icon]
      set l1 [$c create line -7 -10 7 -10 -tags {icon nonactive}]
      set l2 [$c create line -7 0 7 0 -tags {icon nonactive} ]
      set l3 [$c create line -7 10 7 10 -tags {icon nonactive} ]
      $c move $l1 $x $y
      $c move $l2 $x $y
      $c move $l3 $x $y
      set_iconComponent $r $l1
      set_iconComponent $r $l2
      set_iconComponent $r $l3
      set_iconOfCanvasWidget $r $l1
      set_iconOfCanvasWidget $r $l2
      set_iconOfCanvasWidget $r $l3
   }

    list_object {
	set r [$c create oval -20 -15 20 15 -tags icon]
	set l1 [$c create line -10 -7 10 -7 -tags {icon nonactive}]
	set l2 [$c create line -10 0 10 0 -tags  {icon nonactive} ]
	set l3 [$c create line -10 7 10 7 -tags  {icon nonactive} ]
	$c move $l1 $x $y
	$c move $l2 $x $y
	$c move $l3 $x $y

	set_iconComponent $r $l1
	set_iconComponent $r $l2
	set_iconComponent $r $l3
	
	set_iconOfCanvasWidget $r $l1
	set_iconOfCanvasWidget $r $l2
	set_iconOfCanvasWidget $r $l3
    }
    
    equation {
      set r [$c create rectangle -16 -7 16 +7 -tags icon]
    }

    systemOfEquations {
      set r [$c create rectangle -16 -7 16 +7 -tags icon]
    }
}

  $c move $r $x $y

  # X database access seems pretty roundabout:
  set font [option get .workspace.frame3.log font {}]

  if {$iconID == "problem"} {
    set t [$c create text [expr $x + 2] [expr [lindex [$c coords $r] 3] + 13] \
                         -text $name -font $font -tags name]
  } else {
    set t [$c create text [expr $x + 2] [expr [lindex [$c coords $r] 3] + 13] \
                         -text $name -font $font -tags name]
  }
  set_iconComponent $r $t
  set_iconOfCanvasWidget $r $t
  set_iconOfCanvasWidget $r $r

  return $r
}


proc workspace_superimposeIconOfDeath {icon} {

  global tcl_path

  set c .workspace.frame2.c

  if { [file exists "$tcl_path/bitmaps/check.xbm"] } {
      
    set s [$c create bitmap \
              [expr [get_iconXPosition $icon] - 7] \
              [get_iconYPosition $icon] \
              -bitmap @$tcl_path/bitmaps/check.xbm \
              -background OldLace \
              -tags {icon nonactive}]

    set_iconComponent $icon $s
    set_iconOfCanvasWidget $icon $s
  }
}


proc workspace_changeNameFontSize {cs} {
  catch {
    .workspace.frame2.c itemconfigure name -font [newFont \
      [option get .workspace.frame3.log font {}] $cs $cs]
  }
}


proc workspace_placeIcon {iconID name dependencies properties} {

  # This stores the dependencies.
  global dest
  set c .workspace.frame2.c

  workspace_goodIconSpot $dependencies x y

  set result [workspace_drawIcon $iconID $name $x $y]

  set_iconPosition $result $x $y
  initialize_for_edges $result

  set_objectDependencies $result $dependencies
  # Need to convert from object IDs to icon numbers:
  set dependencies [get_objectDependencies $result]

  # Put in arrows to ojects this one depends on.
  set d $dest(none)
  set fromID -1
  set toID -1
  
  if { $iconID == "map" || $iconID == "homomorphism"} {
    extractArguments { datum } [lindex $properties 0]
      set fromID $datum
      extractArguments { datum } [lindex $properties 1]
      set toID $datum
  }

  foreach i $dependencies {

 

# set_edge receives the destination parameter  corresponding to the 
# source object of an edge
    if { $fromID == [get_objectID $i]} {
	set d $dest(into)
    }

    if { $toID == [get_objectID $i]} {
	set d $dest(from)
    }
    
    set edge [create_edge $x $y [get_iconXPosition $i] \
		  [get_iconYPosition $i] $d]
    
    set_edge $edge $result $i $d
    low_edge $edge
  }

  workspace_hiliteIcon 0 $result

  return $result
}


proc workspace_goodIconSpot {dependencies Xarg Yarg} {

  # $dependencies holds object ids

  upvar $Xarg x $Yarg y

  # If there are no dependencies, we just find the first empty spot on an
  # imaginary 100x100 grid which is inset (50,50).
  # With one dependency, we look for an empty spot in concentric hexagons.
  # With two dependencies, we start at the midpoint between them, then
  # look out along the perpendicular.
  # With three or more dependencies, we use the center and to hell with it.
  # Each of these may need to give up, and just chose a spot.

  set c .workspace.frame2.c

  # Get bounds of visible rectangle
  set X1 [expr [$c canvasx 0] + 50]
  set Y1 [expr [$c canvasy 0] + 50]
  set X2 [expr [$c canvasx [lindex [$c configure -width] 4]] - 50]
  set Y2 [expr [$c canvasy [lindex [$c configure -height] 4]] - 50]

  set l [llength $dependencies]
  if {$l == 0} {

    set x $X1
    set y $Y1
    while {[$c find overlapping [expr $x - 25] [expr $y - 25] \
                                [expr $x + 25] [expr $y + 25]] != ""} {
      #@db porting to tk4.0
      #incr x 100
      set x [expr $x + 100]

      if {$x > $X2} {
        set x $X1
        set y [expr $y + 100]
        if {$y > $Y2} {
          set x [expr $X1 + [rand 100 300]]
          set y [expr $Y1 + [rand 100 300]]
          return
        }
      }
    }

  } elseif {$l == 1} {

    set x1 [get_iconXPosition [get_objectIcon [lindex $dependencies 0]]]
    set y1 [get_iconYPosition [get_objectIcon [lindex $dependencies 0]]]
    set r 70
    set b 0
    while {$r < 211} {
      foreach a {60 120 0 180 300 240} {
        set x [expr $r * cos([expr 3.1415926 * ($a + $b) / 180]) + $x1]
        set y [expr $r * sin([expr 3.1415926 * ($a + $b) / 180]) + $y1]
        if {$x >= $X1 && $y >= $Y1 && $x <= $X2 && $y <= $Y2 &&
            [$c find overlapping [expr $x - 21] [expr $y - 21] \
                                 [expr $x + 21] [expr $y + 21]] == ""} {
          return
        }
      }
      incr r 70
      incr b 20
    }
    set x [expr $x1 + [rand 40 50]]
    set y [expr $y1 + [rand 40 50]]

  } elseif {$l == 2} {

    set x1 [get_iconXPosition [get_objectIcon [lindex $dependencies 0]]]
    set y1 [get_iconYPosition [get_objectIcon [lindex $dependencies 0]]]
    set x2 [get_iconXPosition [get_objectIcon [lindex $dependencies 1]]]
    set y2 [get_iconYPosition [get_objectIcon [lindex $dependencies 1]]]
    set x0 [expr ($x1 + $x2) / 2]
    set y0 [expr ($y1 + $y2) / 2]
    set x $x0; set y $y0
    foreach i {0 1 -1 2 -2 3 -3} {
      if {$y1 == $y2} {
        set x $x0
        set y [expr $y0 + 70 * $i]
      } else {
        set a [expr atan(($x1 - $x2) / ($y1 - $y2))]
        set x [expr $x0 + 70 * $i * cos($a)]
        set y [expr $y0 - 70 * $i * sin($a)]
      }

      if {$x >= $X1 && $y >= $Y1 && $x <= $X2 && $y <= $Y2 &&
          [$c find overlapping [expr $x - 21] [expr $y - 21] \
                               [expr $x + 21] [expr $y + 21]] == ""} {
        return
      }
    }
    set x [expr $x0 + [rand -10 10]]
    set y [expr $y0 + [rand -10 10]]

  } else {

    set x 0; set y 0
    foreach d $dependencies {
      incr x [get_iconXPosition [get_objectIcon $d]]
      incr y [get_iconYPosition [get_objectIcon $d]]
    }
    set x [expr x / $l]
    set y [expr y / $l]
  }
}


proc workspace_clearSelection {} {

  global workspace_selection

  set c .workspace.frame2.c

  foreach i $workspace_selection {
    workspace_hiliteIcon 0 $i
  }

  set workspace_selection [list]

  updateMenus
}


proc workspace_select {} {

  workspace_clearSelection
  workspace_selectAlso
}


proc workspace_selectAlso {} {

  global workspace_selection

  set c .workspace.frame2.c
  set icon [get_iconOfCanvasWidget [$c find withtag current]]
  set index [lsearch $workspace_selection $icon]

  if { $index == -1 } {
    lappend workspace_selection $icon

    workspace_hiliteIcon 1 $icon

  } else {
    set workspace_selection [lreplace $workspace_selection $index $index]

    workspace_hiliteIcon 0 $icon
  }

  updateMenus
}


proc workspace_unselect {icon} {

  global workspace_selection

  set index [lsearch $workspace_selection $icon]
  if { $index != -1 } {
    set workspace_selection [lreplace $workspace_selection $index $index]
    workspace_hiliteIcon 0 $icon
  }
}


proc workspace_hiliteIcon {b icon} {
  set c .workspace.frame2.c
  if {$b} {
    $c itemconfig $icon -fill Black
  } else {
    $c itemconfig $icon -fill OldLace
  }

}


proc workspace_deleteIcon {icon} {

  global workspace_iconComponents

  set c .workspace.frame2.c

  workspace_unselect $icon

  $c delete $icon
  foreach comp [get_iconComponents $icon] {
    $c delete $comp
  }
  unset workspace_iconComponents($icon,comp)
  unset workspace_iconComponents($icon,icon_of)

  set sources [get_edgeSources $icon]
  set targets [get_edgeTargets $icon]

  foreach edge $sources {
      delete_edge $edge
      clear_edge $edge
  }
  foreach edge $targets {
      delete_edge $edge
      clear_edge $edge
  }
  clear_of_edges $icon
}


proc workspace_deleteSelection {} {

  global workspace_selection

  set goners $workspace_selection

  # Close $goners under dependency

  set all [get_allObjectIcons]
  foreach j $goners {
    set k [lsearch -exact $all $j]
    if {$k != -1} {
      set all [lreplace $all $k $k]
    }
  }
  set changed 1
  while {$changed == 1} {
    set changed 0
    foreach i $all {
      foreach j $goners {
        if {[dependsOn $i $j] == 1} {
          lappend goners $i
          set k [lsearch -exact $all $i]
          set all [lreplace $all $k $k]
          set changed 1
          break
        }
      }
    }
  }

  set prompt {}
  foreach i $goners {
    lappend prompt [get_objectName $i]
  }

  set prompt [join $prompt ", "]
  if {[string length $prompt] > 50} {
    if {[tk_dialog .d {Confirm} \
        "Delete [llength $goners] objects?\n(too many to list; \
         try selecting fewer at a time)" \
        question 1 Yes No]} {
      return
    }
  } else {
    if {[tk_dialog .d {Confirm} "Delete $prompt?" question 1 Yes No]} {
      return
    }
  }

  set message [get_messageTemplate delete]
  lappend message [llength $goners]
  foreach i $goners {
    lappend message [get_objectID $i]
  }

  toSessionManager $message
  if {[inNoviceMode] == 1} {
    postToMinilog "Deletion may be delayed if the session manager is busy."
  }
}


proc postToMinilog {message} {
  .workspace.frame3.log configure -state normal
  .workspace.frame3.log delete 1.0 end
  #.workspace.frame3.log insert end "$message"
  logInsert .workspace.frame3.log $message
#@rn  .workspace.frame3.log yview -pickplace end
  .workspace.frame3.log configure -state disabled
}


proc workspace_blink {icon goColor blankColor stopColor waitColor interval} {
  if {[iconIsDefined $icon]} {
    set widget [lindex [get_iconComponents $icon] 0]
    set state [get_probState [get_viewID $icon]]
    if {$state == "running" || $state == "waitSuspend"} {
      .workspace.frame2.c itemconfigure $widget -fill $goColor
      after $interval \
        [list workspace_blink $icon $blankColor $goColor $stopColor $waitColor \
              $interval]
    } else {
      if { $state == "stalled" } {
        .workspace.frame2.c itemconfigure $widget -fill $waitColor
      } else {
        .workspace.frame2.c itemconfigure $widget -fill $stopColor
      }
    }
  }
}

proc create_edge { sx sy dx dy d } {
    global dest
    
    set l1 [.workspace.frame2.c create line $sx $sy $dx $dy]
    if { $d != $dest(none) } {
	set coor [ get_center_coords $sx $sy $dx $dy]
	if { $d == $dest(from) } {
	    set a [.workspace.frame2.c create line $sx $sy \
		       [lindex $coor 0] [lindex $coor 1]\
		       -arrow last]
	}
	if { $d == $dest(into) } {
	    set a [.workspace.frame2.c create line \
		       $dx $dy \
		       [lindex $coor 0] [lindex $coor 1]\
		       -arrow last]
	}
    } else {
	set a ""
    }
    return [list $l1 $a]
}

proc delete_edge { edge } {
     global dest
     if { [get_edgeDest $edge] != $dest(none) } {
	 .workspace.frame2.c delete [lindex [get_edgeObject $edge] 1]
     }
 
    .workspace.frame2.c delete [lindex [get_edgeObject $edge] 0]
}

proc color_edge { edge col } {
     global dest
     if { [get_edgeDest $edge] != $dest(none) } {
	 .workspace.frame2.c itemconfigure \
	     [lindex [get_edgeObject $edge] 1] -fill $col
     }
    .workspace.frame2.c itemconfigure [lindex [get_edgeObject $edge] 0] -fill $col
}

proc move_source_edge { edge X Y } {
    global dest
    set e [lindex [get_edgeObject $edge] 0]
    set d [get_edgeDest $edge]
    if { $d != $dest(none) } {
	set a [lindex [get_edgeObject $edge] 1]

	set coor [ get_center_coords $X $Y \
		       [lindex [.workspace.frame2.c coords $e] 2] \
		       [lindex [.workspace.frame2.c coords $e] 3]]
	if { $d == $dest(from) } {
	    .workspace.frame2.c coords $a $X $Y \
		[lindex $coor 0] [lindex $coor 1]
	}
	
	if { $d == $dest(into) } {
	    .workspace.frame2.c coords $a  \
		[lindex [.workspace.frame2.c coords $e] 2] \
		[lindex [.workspace.frame2.c coords $e] 3] \
		[lindex $coor 0] [lindex $coor 1] 
		
	}
    }
    
    .workspace.frame2.c coords $e $X $Y \
	[lindex [.workspace.frame2.c coords $e] 2] \
	[lindex [.workspace.frame2.c coords $e] 3]
}

proc move_target_edge { edge X Y } {
    global dest
    set e [lindex [get_edgeObject $edge] 0]
    set d [get_edgeDest $edge]
    if { $d != $dest(none) } {
	set a [lindex [get_edgeObject $edge] 1]
	
	set coor [ get_center_coords \
		       [lindex [.workspace.frame2.c coords $e] 0] \
		       [lindex [.workspace.frame2.c coords $e] 1] \
		       $X $Y]
	
	if { $d == $dest(from) } {
	    .workspace.frame2.c coords $a $X $Y \
		[lindex $coor 0] [lindex $coor 1]
	}	    
	
	if { $d == $dest(into) } {
	    .workspace.frame2.c coords $a  \
		[lindex [.workspace.frame2.c coords $e] 0] \
		[lindex [.workspace.frame2.c coords $e] 1] \
		[lindex $coor 0] [lindex $coor 1]
	}	    
		
    }
    .workspace.frame2.c coords $e \
	[lindex [.workspace.frame2.c coords $e] 0] \
	[lindex [.workspace.frame2.c coords $e] 1] \
	$X $Y
}

proc low_edge { edge } {
    .workspace.frame2.c lower [lindex  $edge 0]
    .workspace.frame2.c lower [lindex  $edge 1]
	
}

proc get_center_coords { x1 y1 x2 y2 } {
    if { $x1 < $x2 } {
	set xb $x1
	set xe $x2
    } else {
	set xb $x2
	set xe $x1
    }
    
    if { $y1 < $y2 } {
	set yb $y1
	set ye $y2
    } else {
	set yb $y2
	set ye $y1
    }
    
    set xc [expr $xb + ($xe-$xb)/2]
    set yc [expr $yb + ($ye-$yb)/2]
    return [list $xc $yc]
}
