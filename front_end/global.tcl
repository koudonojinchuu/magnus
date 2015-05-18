##
##   $Id: global.tcl 9 2005-01-23 05:27:26Z daly $
##

## Copyright (C) 1995 The New York Group Theory Cooperative
## See magnus/doc/COPYRIGHT for the full notice.

## Contents: Global variables and procedures.
##
## Principal Author: Roger Needham
##
## Status: In progress.
##
## Revision History:
##
##   06/12/97 @db implemented 
##     proc get_subProblemName {viewID ARCSlotID}
##
##   01/28/98 @am implemented
##     proc set_probParameters {viewID probParameters} 
##     proc delete_probParameters { viewID } 
##     proc have_probParameters {viewID} 
##
##   04/06/00 @db added new variable and accesors for it: 
##     bool testingMode
##     proc inTestingMode { }


################################################################################
##
## Globals which store information about workspace objects:

# If you add anything here, MAKE SURE unset_globals knows about it!

#@rn It may be more efficient to fold all workspace_* arrays
#@rn into a single 2D array. Given the data abstraction, this is easy.


## workspace_objectIDs
#
# Type: array
# Keyed by canvas widget number, stores the object id supplied by the
# session manager.
#
# Accessors:
#
proc get_objectID {icon} {
  global workspace_objectIDs
  return $workspace_objectIDs($icon)
}
proc set_objectID {icon ID} {
  global workspace_objectIDs
  set workspace_objectIDs($icon) $ID
}
proc get_objectIcon {ID} {
  # Returns the icon number of the given object ID.
  global workspace_objectIDs
  foreach i [array names workspace_objectIDs] {
    if { $workspace_objectIDs($i) == $ID } { return $i }
  }
  error "Program asked for icon number of bad ID number: $ID"
}
proc IDIsDefined {ID} {
  global workspace_objectIDs
  foreach i [array names workspace_objectIDs] {
    if { $workspace_objectIDs($i) == $ID } { return 1 }
  }
  return 0
}
proc iconIsDefined {icon} {
  global workspace_objectIDs
  foreach i [array names workspace_objectIDs] {
    if { $i == $icon } { return 1 }
  }
  return 0
}
proc get_allObjectIcons {} {
  global workspace_objectIDs
  return [array names workspace_objectIDs]
}

## workspace_objectNames
#
# Type: array
# Keyed by canvas widget number, stores the name of the workspace object.
# See pendingCheckins and object_name_is_taken, below.
#
# Accessors:
#
proc get_objectName {icon} {
  global workspace_objectNames
  return $workspace_objectNames($icon)
}
proc set_objectName {icon name} {
  global workspace_objectNames
  set workspace_objectNames($icon) $name
}


#@db

## workspace_objectHaveAbbreviation
#
# Type: array
# Keyed by canvas widget number, stores 1 if we have abbreviation of 
# the name of the workspace object and 0 otherwise.
#
# Accessors:
#
proc get_objectHaveAbbreviation {icon} {
  global workspace_objectHaveAbbreviation
  return $workspace_objectHaveAbbreviation($icon)
}
proc set_objectHaveAbbreviation {icon bHaveAbbreviation} {
  global workspace_objectHaveAbbreviation
  set workspace_objectHaveAbbreviation($icon) $bHaveAbbreviation
}


## workspace_objectTypes
#
# Type: array
# Keyed by canvas widget number, stores the typeID of the workspace
# object.
#
# Accessors:
#
proc get_objectType {icon} {
  global workspace_objectTypes
  return $workspace_objectTypes($icon)
}
proc set_objectType {icon type} {
  global workspace_objectTypes
  set workspace_objectTypes($icon) $type
}


## workspace_objectDependencies
#
# Type: array
# Keyed by canvas widget number, stores the canvas widget numbers of
# the workspace objects the keyed one depends on.
#
# Accessors:
#<
proc get_objectDependencies {icon} {
  global workspace_objectDependencies
  return $workspace_objectDependencies($icon)
}
proc set_objectDependencies {icon dependencies} {

  global workspace_objectDependencies

  # dependencies holds the object ids of the dependencies, so we
  # have to convert:

  set deps [list]
  foreach i $dependencies {
    lappend deps [get_objectIcon $i]
  }
  set workspace_objectDependencies($icon) $deps
}
proc dependsOn {icon1 icon2} {
  # Returns 1 iff $icon1 depends on $icon2
  global workspace_objectDependencies

#@db 02/26 fixed an error here (changed the order of lsearch arguments)
if {[lsearch -exact $workspace_objectDependencies($icon1) $icon2] != -1} {
    return 1
  } else { return 0 }
}

## workspace_objectDefinitions
#
# Type: array
# Keyed by canvas widget number, stores the definition of the object.
#
# Accessors:
#
proc get_objectDefinition {icon} {
  global workspace_objectDefinitions
  return $workspace_objectDefinitions($icon)
}
proc set_objectDefinition {icon definition} {
  global workspace_objectDefinitions
  set workspace_objectDefinitions($icon) $definition
}


################################################################################
##
## Globals which store information about workspace object views:

# If you add anything here, MAKE SURE unset_globals knows about it!


## viewData
#
# Type: array
# This array stores all static and dynamic data for all workspace object
# views.  It uses compound keys, consisting of a variable part and a
# constant `field specifier', separated by a comma. The variable part
# must be globally unique with respect to the field specifier.
#
# Canonical Accessors:
#

proc viewExists {viewID} {
  global viewData
  info exists viewData($viewID,title)
}
#@rn I think it safe to assume that a view exists iff a title is defined

proc set_viewPath {viewID path} {
  global viewData
  set viewData($viewID,path) $path
}
# Puhleez set path to "" when window is destroyed!
proc get_viewPath {viewID} {
  global viewData
  expr {[info exists viewData($viewID,path)] ? $viewData($viewID,path) : ""}
}
proc set_viewBuilder {icon viewBuilder} {
  global viewData
  set viewData($icon,builder) $viewBuilder
}
proc get_viewBuilder {icon} {
  global viewData
  expr {[info exists viewData($icon,builder)] ? $viewData($icon,builder) : ""}
}
proc set_viewID {icon viewID} {
  global viewData
  set viewData($icon,ID) $viewID
}
proc get_viewID {icon} {
  global viewData
  expr {[info exists viewData($icon,ID)] ? $viewData($icon,ID) : ""}
}
proc set_viewIcon {viewID icon} {
  global viewData
  set viewData($viewID,icon) $icon
}
proc get_viewIcon {viewID} {
  global viewData
  return $viewData($viewID,icon)
}
proc set_viewTitle {viewID viewTitle} {
  global viewData
  set viewData($viewID,title) $viewTitle
}
proc get_viewTitle {viewID} {
  global viewData
  return $viewData($viewID,title)
}
proc set_viewBanner {viewID viewBanner} {
  global viewData
  set viewData($viewID,banner) $viewBanner
}
proc get_viewBanner {viewID} {
  global viewData
  return $viewData($viewID,banner)
}
proc set_viewLog {viewID log} {
  global viewData
  set viewData($viewID,log) $log
}
proc get_viewLog {viewID} {
  global viewData
  return $viewData($viewID,log)
}
proc set_viewHeritage {viewID heritage} {
  global viewData
  set viewData($viewID,heritage) $heritage
}
proc get_viewHeritage {viewID} {
  global viewData
  return $viewData($viewID,heritage)
}
#
# Accessors for problem views:
#
proc set_totalARCSlotID {viewID ARCSlotID} {
  global viewData
  set viewData($viewID,totalARCSlotID) $ARCSlotID
}
proc get_totalARCSlotID {viewID} {
  global viewData
  return $viewData($viewID,totalARCSlotID)
}
proc set_probState {viewID state} {
  global viewData
  set viewData($viewID,probState) $state
  redrawParameters $viewID [get_viewPath $viewID]

}
proc get_probState {viewID} {
  global viewData
  return $viewData($viewID,probState)
}
proc delete_probState {viewID} {
  global viewData
  unset viewData($viewID,probState)
}
proc set_ARCValue {viewID ARCSlotID value} {
  global viewData
  set viewData($viewID,$ARCSlotID,ARCValue) $value
}
proc get_ARCValue {viewID ARCSlotID} {
  global viewData
  return $viewData($viewID,$ARCSlotID,ARCValue)
}
proc set_ARCSlotPath {viewID ARCSlotID path} {
  global viewData
  set viewData($viewID,$ARCSlotID,ARCSlotPath) $path
}
proc get_ARCSlotPath {viewID ARCSlotID} {
  # Return value may be stale! Use winfo exists.
  global viewData
  return $viewData($viewID,$ARCSlotID,ARCSlotPath)
}
proc set_probComponents {viewID probComponents} {
  global viewData
  set viewData($viewID,probComponents) $probComponents
}
proc get_probComponents {viewID} {
  global viewData
  return $viewData($viewID,probComponents)
}
proc set_probParameters {viewID probParameters} {
    global viewDataParameters
    if { [lindex $probParameters 1] != "Parameters" } {
	alert "proc set_probParameters {viewID probParameters} : Sorry, this is not a parameters list."
    }
    setGroupWindow $probParameters $viewID
    set viewDataParameters($viewID.Parameters.applied) disabled
}
proc delete_probParameters { viewID } {
    
    unsetGroupWindow $viewID.Parameters

}

proc have_probParameters {viewID} {
    global viewDataParameters
    if { [llength $viewDataParameters($viewID.Parameters.children)] == 0 } {
	return 0
    } else {
	return 1
    }
}

proc set_viewHelpID {viewID helpID} {
  global viewData
  set viewData($viewID,helpID) $helpID
}
proc get_viewHelpID {viewID} {
  global viewData
  return $viewData($viewID,helpID)
}
proc set_ActiveARCSlotIDs {viewID ARCSlotIDs} {
  global viewData
  set viewData($viewID,ActiveARCSlotIDs) $ARCSlotIDs
}
proc get_ActiveARCSlotIDs {viewID} {
  global viewData
  return $viewData($viewID,ActiveARCSlotIDs)
}

proc get_subProblemName {viewID ARCSlotID} {
    global A
    
    foreach component [get_probComponents $viewID] {
	foreach subproblem [lindex $component 1] {
	    if {$ARCSlotID == [lindex $subproblem 2]} {
		eval set name "\"[lindex $subproblem 0]\""
		return $name
	    }
	}
    }
    return ""
}


################################################################################
##
## Globals which store the state of the workspace:

# If you add anything here keyed by icon, MAKE SURE workspace_deleteIcon
# (not unset_globals) knows about it! 


## workspace_selection
#
set workspace_selection [list]
#
# Type: list
# Stores the canvas widget numbers of the icons which are selected
# in the workspace.


## workspace_lastX
## workspace_lastY
#
# Type: integer
# Used by workspace_itemStartDrag and workspace_itemDrag to remember
# where the cursor is during a drag.


## workspace_dragBoundingBox
# Type: rectangle
#
# Set when a drag starts, it is the rectangle within which the cursor
# must stay so that the selection is not moved off of the canvas.


## workspace_selectionBox
#
set workspace_selectionBox 0
#
# Type: canvas widget number
# The rectangle in the canvas used as a selection box.
# Set to 0 when there is no selection box drag is in progress.


## workspace_selectionBoxX
## workspace_selectionBoxY
#
# Type: integer
# The coordinates of the anchor part of the selection box.


## workspace_iconXPositions
## workspace_iconYPositions
#
# Type: array
# Keyed by canvas widget numbers, stores the coordinates in the canvas
# of the center of the icon.
#
# Accessors:
#
proc get_iconXPosition {icon} {
  global workspace_iconXPositions
  return $workspace_iconXPositions($icon)
}
proc get_iconYPosition {icon} {
  global workspace_iconYPositions
  return $workspace_iconYPositions($icon)
}
proc set_iconPosition {icon x y} {
  global workspace_iconXPositions workspace_iconYPositions
  set workspace_iconXPositions($icon) $x
  set workspace_iconYPositions($icon) $y
}


## workspace_edgeSources
## workspace_edgeTargets
#
# Type: array
# Keyed by canvas widget numbers, workspace_edgeSources stores the
# canvas widget numbers of those edges which have the key as source;
# similarly workspace_edgeTargets.
#
# Accessors:
#
proc initialize_for_edges {icon} {
  global workspace_edgeSources workspace_edgeTargets
  set workspace_edgeSources($icon) {}
  set workspace_edgeTargets($icon) {}
}
proc clear_edge {edge} {
  global workspace_edgeSources workspace_edgeTargets
  foreach i [array names workspace_edgeSources] {
    set index [lsearch -exact $workspace_edgeSources($i) $edge]
    if {$index != -1} {
      set workspace_edgeSources($i) \
          [lreplace $workspace_edgeSources($i) $index $index]
    }
  }
  foreach i [array names workspace_edgeTargets] {
    set index [lsearch -exact $workspace_edgeTargets($i) $edge]
    if {$index != -1} {
      set workspace_edgeTargets($i) \
          [lreplace $workspace_edgeTargets($i) $index $index]
    }
  }
}
proc clear_of_edges {icon} {
  global workspace_edgeSources workspace_edgeTargets
  unset workspace_edgeSources($icon)
  unset workspace_edgeTargets($icon)
}

# set_edge receives the destination parameter  corresponding to the 
# source object of an edge
proc set_edge {edge source target destination } {
    global workspace_edgeSources workspace_edgeTargets
    global dest
    set targetD $dest(none)
    if { $destination == $dest(from) } {
	set targetD $dest(into)
    }
    if { $destination == $dest(into) } {
	set targetD $dest(from)
    }
    
    
    lappend workspace_edgeSources($source) [list $edge $destination]
    lappend workspace_edgeTargets($target) [list $edge $targetD]
    #lappend workspace_edgeSources($source) $edge 
    #lappend workspace_edgeTargets($target) $edge

}
proc get_edgeSources {icon} {
  global workspace_edgeSources
  return $workspace_edgeSources($icon)
}
proc get_edgeTargets {icon} {
  global workspace_edgeTargets
  return $workspace_edgeTargets($icon)
}

proc get_edgeObject {edge} {
    return [lindex $edge 0]
   # return $edge
}

proc get_edgeDest {edge} {
  return  [lindex $edge 1]
}

## workspace_iconComponents
#
# Type: array
# Keyed by canvas widget number, the datum is a list of the canvas widget
# numbers of all canvas items which are subordinate to the primary icon
# widget. For example, the name text item.
#
# Accessors:
#
proc get_iconComponents {icon} {
  global workspace_iconComponents
  return $workspace_iconComponents($icon,comp)
}
proc set_iconComponent {icon component} {
  global workspace_iconComponents
  lappend workspace_iconComponents($icon,comp) $component
}
proc set_iconOfCanvasWidget {icon widget} {
  global workspace_iconComponents
  set workspace_iconComponents($widget,icon_of) $icon
}
proc get_iconOfCanvasWidget {widget} {
  global workspace_iconComponents
  return $workspace_iconComponents($widget,icon_of)
}
proc set_iconName {icon name} {
  global workspace_iconComponents
  foreach i $workspace_iconComponents($icon,comp) {
    if {[lsearch -exact [.workspace.frame2.c gettags $i] "name"] != -1} {
      .workspace.frame2.c itemconfigure $i -text $name
      return
    }
  }
}



################################################################################
##
## Housekeeping:

## make_transcript_p
#
set make_transcript_p 0
#
# Type: boolean
# During debugging, we want to see all message traffic on stderr, but the
# end user will want a nice transcript file instead.
#
# Accessors:
#
proc set_makeTranscript {value} {
  global make_transcript_p
  set make_transcript_p $value
}
proc makeTranscript {} {
  global make_transcript_p
  return $make_transcript_p
}


## transcriptFD
#
# Type: file descriptor
# The transcript file used when not debugging
#
# Accessors:
#
proc set_transcriptFD {fd} {
  global transcriptFD
  set transcriptFD $fd
}
proc get_transcriptFD {} {
  global transcriptFD
  return $transcriptFD
}

## sessionLogFD
#
# Type: file descriptor
# The session log file used when not debugging
#
# Accessors:
#
proc set_sessionLogFD {fd} {
  global sessionLogFD
  set sessionLogFD $fd
}
proc get_sessionLogFD {} {
  global sessionLogFD
  return $sessionLogFD
}

## transcriptName
#
# Type: file name
# The transcript file used when not debugging
#
# Accessors:
#
proc set_transcriptName {name} {
  global transcriptName
  set transcriptName $name
}
proc get_transcriptName {} {
  global transcriptName
  return $transcriptName
}

## transcript_line_count
#
set transcript_line_count 0
#
# Type: integer
# Used to number the lines of the transcript
#
# Accessors:
#
proc get_transcriptLine {} {
  global transcript_line_count
  incr transcript_line_count
  return $transcript_line_count
}


## pendingCheckins
#
# Type: array
# This is keyed by names which have been given to objects in definition
# dialogs, but which the session manager has not checked in yet.
# The data are lists of: 0) dialog name 1) object defn 2) dialog title
# 3) message template (with ${name} and ${defn} yet to fill in).
# Used by syntax_error_in_defn_ to re-post the dialog.
#
# Accessors:
#
proc get_pendingDialog {name} {
  global pendingCheckins
  return [lindex $pendingCheckins($name) 0]
}
proc get_pendingDefn {name} {
  global pendingCheckins
  return [lindex $pendingCheckins($name) 1]
}
proc get_pendingTitle {name} {
  global pendingCheckins
  return [lindex $pendingCheckins($name) 2]
}
proc get_pendingMessage {name} {
  global pendingCheckins
  return [lindex $pendingCheckins($name) 3]
}
proc save_pendingCheckin {name dialog defn title message} {
  global pendingCheckins
  set pendingCheckins($name) [list $dialog $defn $title $message]
}
proc clear_pendingCheckin {name} {
  global pendingCheckins
  if {[info exists pendingCheckins($name)]} {
    unset pendingCheckins($name)
  }
}


proc object_name_is_taken {name} {
  global workspace_objectNames pendingCheckins
  if {[info exists workspace_objectNames]} {
    foreach i [array names workspace_objectNames] {
      if { $workspace_objectNames($i) == $name } { return 1 }
    }
  }
  if {[info exists pendingCheckins]} {
    foreach i [array names pendingCheckins] {
      if { $i == $name } { return 1 }
    }
  }
  return 0;
}


## dialogNameIndex
#
set dialogNameIndex 0
#
# Type: integer
# Used to index dialog index names, to guarantee uniqueness.
#
# Accessors:
#
proc uniqueDialogName {} {
  global dialogNameIndex
  incr dialogNameIndex
  return ".d$dialogNameIndex"
}


## helpStrings
#
# Type: array
# Holds all help strings, keyed by use. The values are defined in
# helpStrings.tcl.
#
# Accessors:
#
proc getHelp {descr} {
  global helpStrings
  return $helpStrings($descr)
}


## sessionManager
#
# Type: pipe identifier
# A two way pipe connected to the session manager.


## handshakeComplete
#
set handshakeComplete 0
#
# Type: boolean
# True iff the session manager has sent the first message.


## bootstrapComplete
#
set bootstrapComplete 0
#
# Type: boolean
# True iff the session manager has finished booting us.


## SMData
#
# Type: array
# Stores the keyed data from the session manager.
#
# Accessors:
#
proc _value_ {key} {
  global SMData
  expr {[info exists SMData($key)] ? $SMData($key) : 0}
}
proc setSMData {key datum} {
  global SMData
  set SMData($key) $datum
}


## messageTemplates
#
# Type: array
# Stores global message templates received by init_* messages, keyed by *.
#
# Accessors:
#
proc set_messageTemplate {key template} {
  global messageTemplates
  set messageTemplates($key) $template
}
proc get_messageTemplate {key} {
  global messageTemplates
  return $messageTemplates($key)
}


## checkinMenuSpecs, toolsMenuSpecs, makeMenuSpecs
#
# Type: array
# Filled by menu_defn_ and used by updateMenus, keys are signatures,
# data are itemGroups.


## clipboard
#
set clipboard {}
#
# Type: string
# This is a Macintosh-style clipboard for cut-copy-paste.
# I don't want to try an emacs-style kill-ring, since the key bindings
# to support yank-pop would be nightmarish.


## noviceMode
#
set noviceMode 1
#
# Type: boolean
#
# Accessors:
#
proc inNoviceMode {} {
  global noviceMode
  return $noviceMode
}


## fontSize
#
set fontSize Normal
#
# Type: string
# Stores one of the values: small, normal, big, huge, for the radiobuttons
# in the font size menu.


## theColorSet
#
set theColorSet Barbados
#
# Type: string
# Stores one of the values: Barbados, Siberia, for the radiobuttons
# in the Colors menu.


## randSeed
#
set randSeed 2112
#
# Type: integer
#
proc rand {l h} {
  global randSeed
  set randSeed [expr ($randSeed * 1021 + 25673) % 121500]
  expr $l + (($h - $l + 1) * $randSeed) / 121500
}


# boolean logHoldView[]
#
# Keyed by a log name, stores it's scrolling state 
# (true when the automatic scrolling feature is disabled)
#

set logHoldView(.frame3.log) 0

#
# Accessors:
#

# Returns text of the log view button matching the current view state.

proc logGetViewButtonText { logName } {

  global logHoldView
    
  if { ![info exists logHoldView($logName)] } {
    set logHoldView($logName) 0
  }

  if { $logHoldView($logName) } {
    return "Release View"
  } else {
    return "Hold View"
  }
  
}


# Changes the current view state and returns the matching button text.

proc logChangeViewState { logName } {

  global logHoldView
  set logHoldView($logName) [expr !$logHoldView($logName)]
  return [logGetViewButtonText $logName]
}


# Changes state of the view button.

proc logChangeViewButton { logName buttonName } {

  $buttonName configure -text [logChangeViewState $logName]
}


# Appends message to the specified log and scrolls it if its
# view is released (i.e. the autoscrolling feature is enabled)

proc logScrollAppend { logName message {tagList ""} } {

  global logHoldView

  $logName insert end $message $tagList

  if { [info exists logHoldView($logName)]
       && ($logHoldView($logName) == 0) } {
    $logName yview -pickplace end
  }
}


## testingMode
#

set testingMode 1

#
# Type: boolean
#
# Accessors:
#

proc inTestingMode {} {
  global testingMode
  return $testingMode
}


## fixingMode
#

set fixingMode 1

#
# Type: boolean
#
# Accessors:
#

proc inFixingMode {} {
  global fixingMode
  return $fixingMode
}


## playingMode
#

set playingMode 0

#
# Type: boolean
#
# Accessors:
#

proc inPlayingMode {} {
  global playingMode
  return $playingMode
}


## changesFN
#

set changesFN "changes"

#
# Type: string
#
# Filename for the changes to be submitted to programmers
#

## testsFN
#

set testsFN "tests"

#
# Type: string
#
# Filename for the tests for the automatic testing system (ATS)
#



## Utility procs:


proc unset_globals {icon} {

  # workspace_deleteIcon unsets all `state of the workspace' globals.

  global workspace_objectIDs workspace_objectNames workspace_objectTypes \
         workspace_objectDependencies workspace_objectDefinitions
  global viewData
         
  catch { unset workspace_objectIDs($icon) }
  catch { unset workspace_objectNames($icon) }
  catch { unset workspace_objectTypes($icon) }
  catch { unset workspace_objectDependencies($icon) }
  catch { unset workspace_objectDefinitions($icon) }

  set viewID [get_viewID $icon]

  catch { unset viewData($viewID,path) }
  catch { unset viewData($icon,builder) }
  catch { unset viewData($icon,ID) }
  catch { unset viewData($viewID,icon) }
  catch { unset viewData($viewID,title) }
  catch { unset viewData($viewID,banner) }
  catch { unset viewData($viewID,log) }

  catch { unset viewData($viewID,totalARCSlotID) }
  catch { unset viewData($viewID,probState) }
  catch { unset viewData($viewID,probComponents) }
  catch { delete_probParameters $viewID }

  #@rn Doesn't unset these:
  # viewData($viewID,$ARCSlotID,ARCValue)
  # viewData($viewID,$ARCSlotID,ARCSlotPath)
}

# use this procedure to change names of objects in FE
proc setName { oid name } {
    
    global A
    set A($oid,name) $name

    # send name to the Session Manager
    set message [get_messageTemplate setName]
    lappend message $oid
    lappend message $name
    toSessionManager $message
}


