##
##   $Id: updateMenus.tcl 96 2006-03-24 17:59:50Z ybryukhov $
##

## Copyright (C) 1995 The New York Group Theory Cooperative
## See magnus/doc/COPYRIGHT for the full notice.

## Contents: Code for setting the contents of the workspace menus.
##
## Principal Author: Roger Needham
##
## Status: In progress.
##
## Revision History:
##


proc selectionSignature {} {

  # This sorts the selection so that it matches its signature in
  # sorted order, and returns the signature.

  global workspace_selection

  set new_selection [list]
  set signature [list]

  # Do insertion sort into both lists, comparing by typeID:

  set len 0
  foreach icon $workspace_selection {
    set typeID [get_objectType $icon]
    set i 0
    while {$i < $len} {
      if {$typeID > [lindex $signature $i]} {
        incr i
      } else {
        break
      }
    }
    set new_selection [linsert $new_selection $i $icon]
    set signature [linsert $signature $i $typeID]
    incr len
  }
  set workspace_selection $new_selection
  return [join [concat [list [llength $signature]] $signature] "_"]
}


proc updateMenus {} {

  global workspace_selection

  set signature [selectionSignature]

  set m .workspace.mbar.checkin
  foreach c [winfo children $m] { destroy $c }
  set filled [fillMenu $m checkin $signature]
  #.workspace.mbar.checkin configure -state normal
  if {$filled == 1} {
    $m add separator
  }
  $m add command -label Synopsis \
    -command "showHelp {checkinSynopsis::Synopsis of Group Theoretic Objects}"

  $m add command -label Quit -command {
     eval toSessionManager "\"[get_messageTemplate quit]\""
  }

  set m .workspace.mbar.tools
  foreach c [winfo children $m] { destroy $c }
  set filled [fillMenu $m tools $signature]
  set num [llength $workspace_selection]
  #.workspace.mbar.tools configure -state normal
  if {$num > 0} {
    if {$filled == 1} {
      $m add separator
    }

    $m add command -label "Delete selected objects and dependencies" \
      -command workspace_deleteSelection
  }
  if { $num > 0 || $filled == 1 } {
    $m add separator
  }
  $m add command -label Synopsis \
    -command "showHelp {toolsSynopsis::Synopsis of Tools}"

  set m .workspace.mbar.make
  foreach c [winfo children $m] { destroy $c }
  set filled [fillMenu $m make $signature]
  set num [llength $workspace_selection]
  #.workspace.mbar.make configure -state normal
#  if {$num > 0} {
#    if {$filled == 1} {
#      $m add separator
#    }
#
#    $m add command -label "Delete selected objects and dependencies" \
#      -command workspace_deleteSelection
#  }
  if { $filled == 1 } {
    $m add separator
  }
  $m add command -label Synopsis -state disabled
  #  -command "showHelp {makeSynopsis::Synopsis of Make}"

  set m .workspace.mbar.packages
  foreach c [winfo children $m] { destroy $c }
  set filled [fillMenu $m packages $signature]
  set num [llength $workspace_selection]
  #.workspace.mbar.packages configure -state normal

  if { $filled == 1 } {
      $m add separator
  }
  $m add command -label Synopsis -state disabled
 
  set m .workspace.mbar.database
  foreach c [winfo children $m] { destroy $c }
  set filled [fillMenu $m database $signature]
  set num [llength $workspace_selection]
  #.workspace.mbar.database configure -state normal

  $m add command -label "New database" \
    -command "toSessionManager \"[get_messageTemplate db_new]\""
  $m add command -label "Open database" \
    -command "toSessionManager \"[get_messageTemplate db_open]\""
  $m add command -label "Save database" \
    -command "toSessionManager \"[get_messageTemplate db_save]\""
  $m add command -label "Save database as" \
    -command "toSessionManager \"[get_messageTemplate db_save_as]\""
  $m add command -label "Close database" \
    -command "toSessionManager \"[get_messageTemplate db_close]\""
  $m add separator
  $m add command -label "Convert database into html" \
    -command "convert_database2html"
  $m add separator
  $m add command -label "Load experiment from the database" \
    -command "toSessionManager \"[get_messageTemplate db_get_exp]\""
  $m add command -label "Add  experiment to the database" \
    -command "request_for_add_experiment"

  set m .workspace.mbar.testing
  foreach c [winfo children $m] { destroy $c }
  set filled [fillMenu $m testing $signature]
  set num [llength $workspace_selection]
  #.workspace.mbar.testing configure -state normal

  $m add command -label "Submit changes to programmers" \
    -command "showCommentDialog changes"
  $m add command -label "Add problem to the testing database" \
    -command "showCommentDialog tests"
  if [inFixingMode] {
      $m add separator
      $m add command -label "Play" \
	      -command "playChanges changes" -state disabled
  }

  $m add separator
  $m add command -label Synopsis -state disabled
}


proc fillMenu {m menu signature} {

  # Returns 1 if any items were put in, 0 otherwise.

  global workspace_selection A

  set theArray ${menu}MenuSpecs
  global $theArray

  set result 0
  $m delete 0 last
  if {![info exists ${theArray}($signature)]} {
    return $result
  }

  set icon_selection $workspace_selection
  permute icon_selection [lindex [set ${theArray}($signature)] 0]

  # Set up context for evaluating ${namei} and ${OIDi}.
  # Maybe a little dicey, since I have other variables visible here...

  set i 1
  set selection [list]
  foreach icon $icon_selection {
    set name$i [get_objectName $icon]
    set oid [get_objectID $icon]
    set x$i $oid
    lappend selection $oid
    incr i
  }
  #@rn What should these be now? Anything? (maybe done by database refs)

  # Fill in the menu

  set childIndex 0
  set itemGroups [lindex [set ${theArray}($signature)] 1]
  foreach itemGroup $itemGroups {

    set cond [lindex $itemGroup 0]
    set val 0
    if { [catch {eval expr $cond} val] == 0 && $val } {

      set result 1
      foreach item [lindex $itemGroup 1] {

        # The evals below are to force tcl to substitute the
        # values of ${namei} and $xi into the item.

        set temp [lindex $item 0]
	
        if {$temp == "s"} {
          $m add separator
        } elseif {$temp == "c"} {
          set cm $m.child$childIndex
          incr childIndex
          eval $m add cascade -menu $cm -label "\"[lindex $item 1]\""
          menu $cm
          foreach realItem [lindex $item 2] {
            set temp [lindex $realItem 0]
            set action [lindex $realItem 1]
            if {$action == ""} {
              set action toSessionManager
            }
            set message [lindex $realItem 2]
            if {$message == ""} {
	        if {$temp == "s"} {
		    eval $cm add separator
		} else {
		    eval $cm add command -label "\"$temp\"" -state disabled
		}
            } else {
              set message [concat $message $selection]
              eval $cm add command -label "\"$temp\"" \
                -command "\"$action {$message}\""
            }
          }
        } else {
          set action [lindex $item 1]
          if {$action == ""} {
            set action toSessionManager
          }
          set message [lindex $item 2]
          if {$message == ""} {
            eval $m add command -label "\"$temp\"" -state disabled
          } else {
            set message [concat $message $selection]
            eval $m add command -label "\"$temp\"" \
              -command "\"$action {$message}\""
          }
        }
      }
    }
  }
  return $result
}


proc checkinSynopsis {} {

  global checkinMenuSpecs

#@rn temp decl:
  global typeNames typePrefixes

  set w .checkinSynopsis
  if { [winfo exists $w] } {
    wm deiconify $w
    raise $w
    return
  }

  set synopsis \
"  This is a synopsis of the group theoretic objects which you can check\
in to the workspace.
  Since the Check in menu changes depending on the context (e.g., what\
you have selected in the workspace), some of the items listed here may\
not always be available. Also, some hypothetical object names cannot (yet)\
be sensibly resolved, so they are marked with `<object name>'."

  foreach signature [lsort [array names checkinMenuSpecs]] {

    if {$signature == "0"} {

      append synopsis \
        "\n\n---------------------------\nIf there are no selected objects,"

    } else {

      append synopsis \
        "\n\n---------------------------\nIf the selected objects are:\n\n"
      set index 1
      foreach type [lreplace [split $signature _] 0 0] {
        set name $typePrefixes($type)$index
        append synopsis "  $name, a $typeNames($type)\n"
        set name$index $name
        set OID$index -1
        incr index
      }
    }
    append synopsis "\nthen the objects which can be checked in are:\n\n"

    foreach itemGroup $checkinMenuSpecs($signature) {
  
      foreach item [lindex $itemGroup 1] {

        # The evals below are to force tcl to substitute the
        # values of ${namei} and ${OIDi} into the item.
  
        set temp [lindex $item 0]
        if {$temp == "s"} {
          append synopsis "  ----------\n"
        } elseif {$temp == "c"} {
          append synopsis "  [lindex $item 1]\n"
          foreach realItem [lindex $item 2] {
            if {[lindex $realItem 2] != ""} {
              eval append synopsis "\"    [lindex $realItem 0]\n\""
            }
          }
        } else {
          if {[lindex $item 2] != ""} {
            eval append synopsis "\"  $temp\n\""
          }
        }
      }
    }
  }

  append synopsis \
"\n\n\n---------------------------\nThe details, if you care (or, why\
is this so lame?):\nThe Check in and Tools menus of Magnus are built\
dynamically as the selected objects change. The front end builds the\
menus from an abstract description of what is available, which is\
supplied by the session manager.  But the description depends on the\
current context; for example, one cannot compute the intersection of\
two subgroups unless they have a common supergroup.  The details of\
context evaluation are (deliberately) hidden from the front end, so it\
is difficult to give the synopsis generating code satisfactory\
intelligence without violating the separation of front end and session\
manager. We're working on it, and as always, we welcome suggestions."

  makeTextWindow $w "Check in menu synopsis" $synopsis
}


proc toolsSynopsis {} {

  global toolsMenuSpecs typeNames

#@rn temp decl:
  global typeNames typePrefixes

  set w .toolsSynopsis
  if { [winfo exists $w] } {
    wm deiconify $w
    raise $w
    return
  }

  set synopsis \
"  This is a synopsis of the computations which Magnus supports.
  Since the Tools menu changes depending on the context (e.g., what\
you have selected in the workspace), some of the items listed here may\
not always be available. Also, some hypothetical object names cannot (yet)\
be sensibly resolved, so they are marked with `<object name>'."

  foreach signature [lsort [array names toolsMenuSpecs]] {

    append synopsis \
      "\n\n---------------------------\nIf the selected objects are:\n\n"
    set index 1
    foreach type [lreplace [split $signature _] 0 0] {
      set name $typePrefixes($type)$index
      append synopsis "  $name, a $typeNames($type)\n"
      set name$index $name
      set OID$index -1
      incr index
    }
    append synopsis "\nthen the tool options are:\n\n"

    foreach itemGroup $toolsMenuSpecs($signature) {
  
      foreach item [lindex $itemGroup 1] {

        # The evals below are to force tcl to substitute the
        # values of ${namei} and ${OIDi} into the item.
  
        set temp [lindex $item 0]
        if {$temp == "s"} {
          append synopsis "  ----------\n"
        } elseif {$temp == "c"} {
          append synopsis "  [lindex $item 1]\n"
          foreach realItem [lindex $item 2] {
            if {[lindex $realItem 2] != ""} {
              eval append synopsis "\"    [lindex $realItem 0]\n\""
            }
          }
        } else {
          if {[lindex $item 2] != ""} {
            eval append synopsis "\"  $temp\n\""
          }
        }
      }
    }
  }

  append synopsis \
"\n\n\n---------------------------\nThe details, if you care (or, why\
is this so lame?):\nThe Check in and Tools menus of Magnus are built\
dynamically as the selected objects change. The front end builds the\
menus from an abstract description of what is available, which is\
supplied by the session manager.  But the description depends on the\
current context; for example, one cannot compute the intersection of\
two subgroups unless they have a common supergroup.  The details of\
context evaluation are (deliberately) hidden from the front end, so it\
is difficult to give the synopsis generating code satisfactory\
intelligence without violating the separation of front end and session\
manager. We're working on it, and as always, we welcome suggestions."

  makeTextWindow $w "Tools menu synopsis" $synopsis
}
