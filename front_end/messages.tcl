##
##   $Id: messages.tcl 28 2005-05-02 15:07:27Z jzniu $
##

## Copyright (C) 1995 The New York Group Theory Cooperative
## See magnus/doc/COPYRIGHT for the full notice.

## Contents: Code supporting message passing to and from the session manager.
##
## Principal Author: Roger Needham
##
## Status: In progress.
##
## Revision History:
##


# This doesn't care if a keyword/arg pair is missing:

proc extractArguments {keywords arglist} {
  #puts "entering in extractArguments: \"$keywords\" \"$arglist\""
  set len [llength $arglist]
  foreach k $keywords {
    upvar $k temp
    set i 0
    while {$i < $len} {
      if {[lindex $arglist $i] == $k} {
        set temp [lindex $arglist [incr i]]
        break
      } else { incr i 2 }
    }
  }
}


proc toSessionManager {message} {

  global sessionManager handshakeComplete

  if { !$handshakeComplete } {
    fatalError "The Session Manager has failed to complete its startup."
  }

  #@njz: DEBUG
  #puts stdout "b<- $message"
  #flush stdout
  #

  puts $sessionManager $message
  flush $sessionManager

  if {[makeTranscript] == 1} {
    puts [get_transcriptFD] ">([get_transcriptLine]) $message"
    flush [get_transcriptFD]
      
    #@db
    if [inFixingMode] {
	if [inPlayingMode] {
	    playCommand $message
	}
    }

  }
}

#@db porting to tk4.0
#proc fromSessionManager {event fileID} { }
proc fromSessionManager {} {

#@db porting to tk4.0
#  if { "$event" != "READ" } {
#    puts stderr "Unexpected addinput event: $event\nIgnoring."
#    return
#  }

  #@db porting to tk4.0
  global sessionManager
  set fileID $sessionManager

  if { [gets $fileID message] < 0 } {
    # Don't want a loop here!
    
    #@db porting to tk4.0
    #removeinput $fileID
    fileevent $sessionManager readable ""

    fatalError "The Session Manager has unexpectedly quit."
  } else {

    # Did we get a dummy message?

    if {[string length $message] == 0} { return }

    #@njz: DEBUG
    #puts stdout "b-> $message"
    #flush stdout
    #

    if {[makeTranscript] == 0} {
      puts stderr "<-- $message"
      flush stderr
    } else {
	puts [get_transcriptFD] "<([get_transcriptLine]) $message"
	flush [get_transcriptFD]
    }

    #@rn Fragile, since info procs does pattern matching.
    #    Could use proc registry...
 
    if { [info procs [lindex $message 0]] != "" } {
	eval $message
	
	#if {[catch {eval $message}] == 1} {
	#    puts stderr "\nWarning: error in FE evaluating message:"
	#    puts stderr "$message\n"
	#}
	#

    } else {
      puts stderr "\nWarning: FE ignoring unrecognized message name:"
      puts stderr "[lindex $message 0]\n"
    }
  }
}


## Incoming message handlers:

proc magic_cookie_ {int} {
  global handshakeComplete magicCookie
#@rn register magicCookie

  if { $handshakeComplete } {
    alert "Bogus attempt to change the magic cookie"
  } else {
    set handshakeComplete 1
    set magicCookie int
    postToLog "Loading ... ."
  }
}


proc boot_done_ {} {
  global bootstrapComplete
  set bootstrapComplete 1

  postToLog "Magnus is now ready for use."

}


proc fatal_error_ {msg} {
  fatalError "Internal fatal error:\n$msg"
}


proc new_data_ {args} {
  extractArguments {key datum} $args

#  regsub -all { } $key {} key
#  regsub -all { } $datum {} datum
#@rn  setSMData $key $datum
#@rn Is this what we want anymore?

  global A
  set key [string trimleft $key {$}]
  #@rn Horrible kludge due to how  Datum prints
  eval set $key $datum

  updateMenus
  # Some menus may be different, in light of this new data.
}


proc menu_defn_ {args} {
  extractArguments {menu signature itemGroups} $args
  set theArray ${menu}MenuSpecs
  global $theArray


  # Sort $signature so that it is in order, and store the inverse
  # permutation with $itemGroups.

  set len [llength $signature]
  set permutation [identity_permutation $len]

  # Do transposition sort on $signature, recording the transpositions
  # in $permutation. We do not care about efficiency.

  set changed 1
  while { $changed } {
    set i 1
    set changed 0
    while { $i < $len } {
      if { [lindex $signature $i] < [lindex $signature [expr $i - 1]] } {
        transpose signature [expr $i - 1] $i
        transpose permutation [expr $i - 1] $i
        set changed 1
      }
      incr i
    }
   }

  set signature [join [concat [list [llength $signature]] $signature] "_"]
  set ${theArray}($signature) [list $permutation $itemGroups]

  updateMenus
}


proc eval_string {s} {
  global A
  return $s
}
#@rn kludge for below
#@rn Maybe this should be permanent?
#@rn Doesn't have $x0, $x1, ... context!


proc create_ {args} {

  global A
  #@rn Use accessor?

  #@db
  set haveAbbreviation 0
  

  extractArguments {objectID typeID iconID name dependencies \
                    properties viewStructure definition} $args

  if { $name == "" } {
    set pending_p 0
  } else {
    set pending_p 1
  }

  if { !$pending_p } {
    switch -exact $iconID {
      group {
        set name [uniqueObjectName G]
      }
  
      subgroup {
        set name [uniqueObjectName H]
      }

      SetOfWords {
        set name [uniqueObjectName S]
      }

      VectorOfWords {
        set name [uniqueObjectName T]
      }
  
      SystemOfEquations {
	set name [uniqueObjectName SE]
      }

      map {
        set name [uniqueObjectName m]
      }

      homomorphism {
        set name [uniqueObjectName h]
      }
  
      elt {
        set name [uniqueObjectName w]
      }

      enumerator_object {
        set name [uniqueObjectName En]
      }
      
      list_object {
	  set name [uniqueObjectName L]
      }
      
      problem {
	  set name [uniqueObjectName Problem_]
	  
	  #@db
	  extractArguments {viewParameters} $viewStructure
	  extractArguments {viewAbbreviation} $viewParameters
	  if { $viewAbbreviation != "none" } {
	      eval set name "\"$viewAbbreviation\""
	      set haveAbbreviation 1  
	  }
      }
      
      enumerator_problem {
	  set name [uniqueObjectName Enumerator_]
	  
	  #@db
	  extractArguments {viewParameters} $viewStructure
	  extractArguments {viewAbbreviation} $viewParameters
	  if { $viewAbbreviation != "none" } {
	      
	      eval set n "\"$viewAbbreviation\""
	      set name [uniqueObjectName $n]
	      set haveAbbreviation 1  
	  }
      }
      
      default {
	  set name [uniqueObjectName O]
      }
    }
  }
  # Maybe a proc chooseDefaultName {iconID} ?

  # This call stores the dependencies:
  set icon [workspace_placeIcon $iconID $name $dependencies $properties]
  set_objectID $icon $objectID
  set_objectType $icon $typeID
  set_objectName $icon $name
  set_objectHaveAbbreviation $icon $haveAbbreviation
  setName $objectID $name
  #set A($objectID,name) $name
  initViewStruct $icon $viewStructure
  if { $iconID != "problem" && $iconID != "enumerator_problem" } {
    if { $pending_p } {
      set_objectDefinition $icon [get_pendingDefn $name]
    } else {
	regsub -all {\[} $definition {\[} definition
	regsub -all {\]} $definition {\]} definition

	eval set str \"$definition\"
	set_objectDefinition $icon $str
    }
  }

  foreach pair $properties {
    extractArguments {key datum} $pair
    new_data_ key $key datum $datum
  }

  if { $iconID == "problem" || $iconID == "enumerator_problem" } {
    #@db
    if { $haveAbbreviation } {
      postToLog "Checked in the problem \"$name\"."
    } else {
      postToLog "Checked in $name."
    }
    openView $icon

  } else {
    if { $pending_p } {
      clear_pendingCheckin $name
    }

    extractArguments {viewParameters} $viewStructure
    extractArguments {heritage} $viewParameters
#@rn    set heritage [eval "eval_string \"$heritage\""]
      eval set heritage \"$heritage\"
      if { [ string length [get_objectDefinition $icon]] == 0 } {
        set definition "$heritage\: $name\.\n"
    } else {
	set definition "$heritage\:\n$name = [get_objectDefinition $icon]\.\n"
      }
    catch {postToLog $definition}

    #postToLog "$heritage\:\n$name = [get_objectDefinition $icon]\.\n"

    #@rn definition might be too long
  }
}


proc syntax_error_in_defn_ {args} {

    postToMinilog "Check-in failed."

  extractArguments {name errMesg} $args

  regsub -all {\\n} $errMesg "\n" errMesg
  set dialog [get_pendingDialog $name]
  set theArgs [list [get_pendingTitle $name] [get_pendingMessage $name] \
                    $name [get_pendingDefn $name] $errMesg]
  clear_pendingCheckin $name
  eval $dialog $theArgs
}


proc show_defn_ {args} {

    postToMinilog "Applying randomly generated definition."
    
    extractArguments {defn w} $args
    
    $w delete 1.0 end
    $w insert 1.0 $defn
}


foreach key {view_req arc start suspend resume terminate delete quit setName parameter map_gens db_new db_open db_save db_save_as db_close db_get_exp db_add_exp db_cancel db_success db_failure db_yes db_no db_select_objects db_object_definition} {
  eval [list proc init_${key}_msg_ message \
               " set_messageTemplate $key \$message "]
}


# The following five procs must ignore the message if the view does
# not exist:

proc arc_update_ {args} {
  extractArguments {viewID ARCSlotID value} $args

  if {[viewExists $viewID]} {

    # Here we update the total ARCs expended,
    # since there is no record of this in the SM:

    set totalARCSlotID [get_totalARCSlotID $viewID]
    set total [expr [get_ARCValue $viewID $totalARCSlotID] + $value]
    set_ARCValue $viewID $totalARCSlotID $total
    set entry [get_ARCSlotPath $viewID $totalARCSlotID]
    if {[winfo exists $entry]} {
      $entry configure -state normal
      $entry delete 0 end
      $entry insert 0 $total
      $entry configure -state disabled
    }

    # Now set the changed ARC slot:

    set new_value [expr [get_ARCValue $viewID $ARCSlotID] - $value]
    set_ARCValue $viewID $ARCSlotID $new_value
    set entry [get_ARCSlotPath $viewID $ARCSlotID]
    if {[winfo exists $entry]} {
      $entry configure -state normal
      $entry delete 0 end
      $entry insert 0 $new_value
      $entry configure -state disabled
  
      #@rn Scale stuff. Maybe belongs in view.tcl?:
      regsub entry $entry scale entry
      if {[winfo exists $entry]} {
        $entry configure -state normal
        $entry set $new_value
        $entry configure -state disabled
      }
    }
  }
}


proc confirm_start_ {args} {

  extractArguments {viewID} $args
  if {[viewExists $viewID]} {
    set_probState $viewID running
    set w [get_viewPath $viewID]
    if {[winfo exists $w.frame3.suspend]} {
      $w.frame3.suspend configure -state normal

      #@db
      #$w.frame3.suspend flash
      
      $w.frame3.terminate configure -state normal
      #@db
      #problem_setEntries $w 0
    }
    workspace_blink [get_viewIcon $viewID] MediumSeaGreen OldLace red gold 1000
      
    #@db
    set name [get_objectName [get_objectIcon $viewID]]
    
    if { [get_objectHaveAbbreviation [get_objectIcon $viewID]] } {
      set text "The problem \"$name\" started\n[theTime]."
    } else { 
      set text "$name started\n[theTime]."
    }

    problem_postToLog $viewID $text
    
    if [inPlayingMode] {
	problem_Close $w $viewID
    }
  }
}


proc confirm_suspend_ {args} {
  extractArguments {viewID} $args
  if {[viewExists $viewID]} {
    set_probState $viewID suspended
    set w [get_viewPath $viewID]
    if {[winfo exists $w.frame3.suspend]} {
      $w.frame3.suspend configure -state disabled
      $w.frame3.resume configure -state normal
      $w.frame3.terminate configure -state normal
      problem_setEntries $w 1
    }

    #@db
    set name [get_objectName [get_objectIcon $viewID]]
    
    if { [get_objectHaveAbbreviation [get_objectIcon $viewID]] } {
      set text "The problem \"$name\" suspended\n[theTime]."
    } else { 
      set text "$name suspended\n[theTime]."
    }

    problem_postToLog $viewID $text

#    problem_postToLog $viewID \
#      "[get_objectName [get_objectIcon $viewID]] suspended\n[theTime]."
  }
}


proc confirm_resume_ {args} {
  extractArguments {viewID} $args
  if {[viewExists $viewID]} {
    set old_state [get_probState $viewID]
    set_probState $viewID running
    set w [get_viewPath $viewID]
    if {[winfo exists $w.frame3.resume]} {
      $w.frame3.suspend configure -state normal

      #@db
      #$w.frame3.suspend flash

      $w.frame3.terminate configure -state normal
      #@db
      #problem_setEntries $w 0
      #@db 
      #postToLog "Hello confirm_resume."
   }
    workspace_blink [get_viewIcon $viewID] MediumSeaGreen OldLace red gold 1000
    if { $old_state != "stalled" } {

      #@db

      set name [get_objectName [get_objectIcon $viewID]]
    
	if { [get_objectHaveAbbreviation [get_objectIcon $viewID]] } {
	  set text "The problem \"$name\" resumed\n[theTime]."
	} else { 
	  set text "$name resumed\n[theTime]."
	}
      
      problem_postToLog $viewID $text
	
#    problem_postToLog $viewID \
#        "[get_objectName [get_objectIcon $viewID]] resumed\n[theTime]."
    }
  }
}


proc terminate_problem_ {args} {
  extractArguments {viewID} $args
  if {[viewExists $viewID]} {
    set_probState $viewID terminated
    set w [get_viewPath $viewID]
    if {[winfo exists $w.frame3.suspend]} {
      $w.frame3.suspend configure -state disabled
      $w.frame3.resume configure -state disabled
      $w.frame3.terminate configure -state disabled
    }

    #@db

    set name [get_objectName [get_objectIcon $viewID]]
      
      if { [get_objectHaveAbbreviation [get_objectIcon $viewID]] } {
	set text "The problem \"$name\" finished\n[theTime]."
      } else { 
	set text "$name finished\n[theTime]."
      }
    
    problem_postToLog $viewID $text
      
#    problem_postToLog $viewID \
#      "[get_objectName [get_objectIcon $viewID]] finished\n[theTime]."

    workspace_superimposeIconOfDeath [get_objectIcon $viewID]
  }
}


proc problem_stalled_ {args} {
  extractArguments {viewID} $args
  if {[viewExists $viewID]} {
    set_probState $viewID stalled
  }
}
#@rn Document this new function.


proc confirm_quit_ {} {
  exit
}


proc confirm_delete_ {args} {

  set logMsg ""

  foreach OID $args {

    if { [IDIsDefined $OID] } {

      set icon [get_objectIcon $OID]

      lappend logMsg [get_objectName $icon]

      # Delete $icon from canvas.

      workspace_deleteIcon $icon

      set view [get_viewPath [get_viewID $icon]]
      if {$view != ""} {
        destroy $view
      }

      # Unset absolutely all globals relating to $icon.

      unset_globals $icon
    }
  }

  updateMenus
  set temp [join $logMsg ", "]
  postToLog "Deleted $temp."
}


proc post_to_log_ {viewIDs text level} {

  global A lastLogLink lastLogLinkIsUpToDate

  if { !$lastLogLinkIsUpToDate } { 
    incr lastLogLink 
    set lastLogLinkIsUpToDate 1
  }

  foreach viewID $viewIDs {

    if { [catch {eval get_viewBuilder [get_viewIcon $viewID]} vb] == 0 } {

      # Need to know what kind of view we're posting to:

      if {$vb == "problemView" || $vb == "EnumertatorProblemView"} {
        catch {eval problem_postToLog $viewID "\"$text\"" 2}
      } else {
        catch {eval object_postToLog $viewID "\"$text\"" 2}
      }
      # Had to be objectView
    }
  }

  if {$level == 1} {
    catch {eval postToLog "\"$text\""}
  }
}


proc post_to_session_log_ {text} {
  postToLog $text
}
#@rn Document this new function.


set BEWarningLog "
Magnus may detect an internal problem which is not necessarily fatal.

As the system is under development, we use this window to at least alert you\
to these problems (even though you can't do anything about them).

Here is a cumulative log:\n\n"
#@rn Register this global

proc warn_ {message} {

  global BEWarningLog

  set BEWarningLog "$BEWarningLog\n\n$message"

  makeTextWindow .be_warnings "There's a problem..." $BEWarningLog
}



proc view_package_info_ { n c ch o } {

   set tmessage  "\n\nName : \"$n\" \n\nCommand : \"$c\" \n\nCheckin type : $ch \n\nObject : $o"

   makeTextWindow .pack_info  "Package information"  $tmessage

}


##@rn Provisional way of specifying types for the end user:

proc type_name_ {name prefix typeID} {

  global typeNames typePrefixes

  set typeNames($typeID) $name
  set typePrefixes($typeID) $prefix
}


proc _name_ {OID} {
  set name "<object name>"
  catch {set name [get_objectName [get_objectIcon $OID]]}
  return $name
}


## Action handlers:

# All must take a message as the last arg.


proc _defineFPGroup_ {title message} {
  defineFPGroupDialog $title $message
}

proc _defineORGroup_ {title message} {
  defineORGroupDialog $title $message
}

proc _defineSCGroup_ {title message} {
  defineSCGroupDialog $title $message
}

proc _defineFreeGroup_ {title message} {
  defineFreeGroupDialog $title $message
}

proc _defineAbelGroup_ {title message} {
  defineAbelGroupDialog $title $message
}

proc _defineNilpotentGroup_ {title message} {
  defineNilpGroupDialog $title $message
}
##//**
proc _defineFreeNilpotentGroup_ {title message} {
  defineFreeNilpGroupDialog $title $message
}

proc _definePermutation_ {title message} {
  definePermutationDialog $title $message
}

proc _defineSMNilpotentWord_ {title message} {
  defineNilpotentWordDialog $title $message
}

proc _defineSubgroup_ {title message} {
  defineSubgroupDialog $title $message
}

proc _defineSetOfWords_ {title message} {
  defineSetOfWordsDialog $title $message
}

proc _defineVectorOfWords_ {title message} {
  defineVectorOfWordsDialog $title $message
}

#proc _defineMap_ {title message } {
#  defineMapDialog $title $message 
#}

proc _defineMap_ { viewID oid2 message} {
    set message [IPCChars $message] 
    eval toSessionManager "\"[get_messageTemplate map_gens]\""
}

proc _defineWord_ {title message} {
  defineWordDialog $title $message
}

proc _defineEquation_ {title message} {
  defineEquationDialog $title $message
}

proc _defineSystemOfEquations_ {title message} {
  defineSystemOfEquationsDialog $title $message
}

proc _defineInt_ {title low hi message} {
  defineIntegerDialog $title $low $hi $message
}

proc _definePackage_ {title  objects checkintypes message } {
  definePackageDialog $title  $objects $checkintypes $message
}

proc _editPackage_ {title ch_types filenames message  } {
  editPackagesDialog $title $ch_types $filenames $message
}

proc _packageID_ {title id  message } {
    eval [concat toSessionManager "\"$message $id \""]	   
}

#@db
proc _defineSetOfRelators_ {title message} {
  defineSetOfRelatorsDialog $title $message
}
