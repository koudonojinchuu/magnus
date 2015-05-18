#
# General
#
proc getType { list } {
    return [lindex $list 0]
}
proc getName { list } {
    return [lindex $list 1]
}

proc applyStatus { viewID } {
    global viewDataParameters
    
    return $viewDataParameters($viewID.Parameters.applied) 
}

proc setApplyStatus { viewID s } {
    global viewDataParameters

    if { $s=="abled" } {
	set b [ get_viewPath $viewID].parameters.apply 
	if { [winfo exists $b] } {
	    set viewDataParameters($viewID.Parameters.applied) abled
	    $b configure -state normal
	}
    } else {
	set b [ get_viewPath $viewID].parameters.apply 
	if { [winfo exists $b] } {
	    set viewDataParameters($viewID.Parameters.applied) disabled
	    $b configure -state disabled
	}
    }
}

proc showParameters { parent w title } {
    global viewDataParameters
    
    if { [winfo exists $w.parameters] } {
	wm deiconify $w.parameters
	raise $w.parameters
	return 
    }

    set w1 $w.parameters
    toplevel $w1 
    grab $w1
    wm title $w1 "Parameters for $title"
    
    callGroupWindow Parameters $parent $w1 $parent 
    button $w1.apply -text Apply -width 11 \
	-command "applyParameters $parent"

    pack $w1.apply -padx 5m -pady 3m -ipady 1m -side left
    button $w1.close -text Close -width 11 \
	-command "destroy $w1"

    pack $w1.close -padx 5m -pady 3m -ipady 1m -side left 

    switch -exact [get_probState $parent] {
	new {
	    setApplyStatus $parent disabled
	}
	running {
	    setApplyStatus $parent disabled
	}
	stalled {
	    setApplyStatus $parent disabled
	}
	suspended {
	    setApplyStatus $parent [applyStatus $parent]
	}
	terminated {
	    setApplyStatus $parent disabled
	}
    }
}


proc syntax_error_in_param_ { args } {
    extractArguments { OID name type errMesg} $args
    
    set icon [get_objectIcon $OID]
    set viewID [get_viewID $icon]
    set w [get_viewPath $viewID]

    if { $errMesg == "none" } { 
	commitChangesGroupWindow $viewID.Parameters
	grab release $w.parameters
	destroy $w.parameters
	return
    }

    postToMinilog "Apply parameters failed."
    
    regexp {([^:]*)(:)(.*)} $errMesg s hiStr sep errStr
    
    
    openView $icon
  
    if { ![winfo exists $w.parameters] } { 
	redrawParameters $viewID $w 
    }
    
    raise $w.parameters

    if { ![winfo exists $w.parameters.errMsg] } { 
	pack forget $w.parameters.close $w.parameters.apply
	
	frame $w.parameters.errMsg -relief sunken
	label $w.parameters.errMsg.label -text "Errors in parameters :" \
	    -foreground red
	
	pack $w.parameters.errMsg.label -side top -anchor nw
	
	text $w.parameters.errMsg.text -relief sunken -bd 2 -height 5 -width \
	    40 -wrap word -yscrollcommand "$w.parameters.errMsg.scrl set"
	
	scrollbar $w.parameters.errMsg.scrl -relief sunken \
	    -command "$w.parameters.errMsg.text yview"
	
	
	pack $w.parameters.errMsg.text -side left -fill both -expand 1
	pack $w.parameters.errMsg.scrl -side right -fill y
	pack $w.parameters.errMsg -side bottom 
	pack $w.parameters.apply -padx 5m -pady 3m -ipady 1m -side left
	
	pack $w.parameters.close -padx 5m -pady 3m -ipady 1m -side left
	
	setApplyStatus $viewID abled
    }
    $w.parameters.errMsg.text tag configure c -foreground red 
    $w.parameters.errMsg.text insert end "$hiStr :" c
    $w.parameters.errMsg.text insert end $errStr\n 
    
    blinkWidget $w.parameters.errMsg.label 0 red OldLace    
    
 
    $w.parameters.errMsg.text mark set insert end
}


#
#  apply parameters
#

proc applyParameters { parent } {
    global paramToSM
    set paramToSM($parent.applyMsg) ""
    
    applyGroupWindow $parent.Parameters $parent
    setApplyStatus $parent disabled
    set w [get_viewPath $parent]
    
    if { [winfo exists $w.parameters.errMsg] } { 
	$w.parameters.errMsg.text delete 0.0 end
    }
    grab set $w.parameters
    set viewID $parent
    set value $paramToSM($viewID.applyMsg)
    eval toSessionManager "\"[get_messageTemplate parameter]\""
}

proc addParameterToSMmessage { viewID type name value } {
    global paramToSM
    
    set pstr "$paramToSM($viewID.applyMsg) $type $name $value" 
    set paramToSM($viewID.applyMsg) $pstr   
}

#
# Change status
#


proc redrawParameters { viewID w } {
    global viewDataParameters

    if { ![winfo exists $w.parameters] } { return }

    set w1 $w.parameters
  
    pack forget $w1.close $w1.apply

    destroy $w1.frameParameters
    
    callGroupWindow Parameters $viewID $w1 $viewID 
    pack $w1.apply -padx 5m -pady 3m -ipady 1m -side left
    
    pack $w1.close -padx 5m -pady 3m -ipady 1m -side left 
    
    switch -exact [get_probState $viewID] {


	new {
	    setApplyStatus $viewID disabled
	}
	running  {
	    setApplyStatus $viewID disabled
	}
	stalled {
	    setApplyStatus $viewID disabled
	}
	suspended {
	    setApplyStatus $viewID [applyStatus $viewID]
	}
	terminated {
	    setApplyStatus $viewID disabled
	}
	waitStart  {
	    setApplyStatus $viewID disabled
	}
	waitSuspend {
	    setApplyStatus $viewID [applyStatus $viewID]
	}
	waitTerminate {
	    setApplyStatus $viewID disabled
	}
	waitResume  {
	    setApplyStatus $viewID disabled
	}
    }
}

proc getStatus { viewID changeable } {
    
   switch -exact [get_probState $viewID] {
	new {
	    return normal
	}
	running {
	    return disabled
	}
	stalled {
	    return disabled
	}
	suspended {
	    if { $changeable } {
		return normal
	    } else {
		return disabled
	    }
	}
	terminated {
	    return disabled
	}
	waitStart  {
	    return disabled
	}
	waitSuspend {
	    if { $changeable } {
		return normal
	    } else {
		return disabled
	    }
	}
	waitTerminate {
	    return disabled
	}
	waitResume  {
	    return disabled
	}
    }   
}

#
#    int field
#


proc setint { list parent } {
    global viewDataParameters

    set name [getName $list]

    if {[info exists viewDataParameters($parent.$name.text)] } {
	alert "Can't make integer field : already in use!"
	exit
    }
    set viewDataParameters($parent.$name.text) [lindex $list 2]
    set viewDataParameters($parent.$name.changeable) [lindex $list 3]
    set viewDataParameters($parent.$name.value) [lindex $list 4]
    set viewDataParameters($parent.$name.valueTmp) 0
    set viewDataParameters($parent.$name.min) [lindex $list 5]
    set viewDataParameters($parent.$name.max) [lindex $list 6]
    
}

proc unsetint { name } {
    global viewDataParameters

    if {[info exists viewDataParameters($name.text)] } {
	unset viewDataParameters($name.text)
	unset viewDataParameters($name.changeable)
	unset viewDataParameters($name.value)
	unset viewDataParameters($name.valueTmp)
	unset viewDataParameters($name.min)
	unset viewDataParameters($name.max)
    }
}

proc commitChangesint { name } {
    global viewDataParameters
    set viewDataParameters($name.value) $viewDataParameters($name.valueTmp)    
}

proc applyint { path name viewID } {
    global viewDataParameters

    set value $viewDataParameters($path.valueTmp)
    
    addParameterToSMmessage $viewID int $name $value 

    set type int

    ##eval toSessionManager "\"[get_messageTemplate parameter]\""
}
proc callint { o_name parent w viewID} {
    
    global viewDataParameters

    set minV $viewDataParameters($parent.$o_name.min)
    set maxV $viewDataParameters($parent.$o_name.max)
    set value $viewDataParameters($parent.$o_name.value)
    set viewDataParameters($parent.$o_name.valueTmp) $value

    set state [getStatus $viewID $viewDataParameters($parent.$o_name.changeable)]

    set t $o_name
    frame $w.frame$t -relief flat
    pack $w.frame$t -padx 1 -pady 1 -fill x
    entry $w.frame$t.entry$t -width 5 -relief sunken 
    $w.frame$t.entry$t insert 0 $value
    
    $w.frame$t.entry$t configure -state $state
    
    button $w.frame$t.label$t -text $viewDataParameters($parent.$o_name.text) -state $state -relief flat
    pack $w.frame$t.entry$t -side left
    pack $w.frame$t.label$t -side left



    bind  $w.frame$t.entry$t <KeyRelease> "checkIntField $parent.$o_name $w.frame$t.entry$t $viewID"
}

proc checkIntField { field w viewID} {
    global viewDataParameters

    set value [ $w get]
    set field_ok 1
    
    if { $value == $viewDataParameters($field.value) } { return }

    if { $value != "" } {
	if {![regexp {^([-0-9]*)$} $value]} {
	    alert "`$value' is not an integer."
	    set field_ok 0
	}
    
#	if { $value < $viewDataParameters($field.min) } {
#	    alert "`$value' too small. Suppose to be not less then $viewDataParameters($field.min)"

#	    set field_ok 0
#	}
	
#	if { $value > $viewDataParameters($field.max) } {
#	    alert "`$value' too large. Suppose to be not more then $viewDataParameters($field.max)"
#	    set field_ok 0
#	}   
    } else {
	set value 0
    }
    if { $field_ok} {
	set viewDataParameters($field.valueTmp) $value
	setApplyStatus $viewID abled
    } else {
	$w delete 0 end
	$w insert 0 $viewDataParameters($field.valueTmp)
    }
}

#
#    text field
#


proc settext { list parent } {
    global viewDataParameters
    set name [getName $list]
    if {[info exists viewDataParameters($parent.$name.text)] } {
	alert "Can't make text field : already in use!"
	exit
    }

    set viewDataParameters($parent.$name.text) [lindex $list 2]
    set viewDataParameters($parent.$name.changeable) [lindex $list 3]
    set viewDataParameters($parent.$name.value) [lindex $list 4]
    set viewDataParameters($parent.$name.valueTmp) ""
}

proc unsettext { name } {
    global viewDataParameters

    if {[info exists viewDataParameters($name.text)] } {
	unset viewDataParameters($name.text)
	unset viewDataParameters($name.changeable)
	unset viewDataParameters($name.value)
	unset viewDataParameters($name.valueTmp)
    }
}

proc commitChangestext { name } {
    global viewDataParameters
    set viewDataParameters($name.value) $viewDataParameters($name.valueTmp)    
}

proc applytext { path name viewID } {
    global viewDataParameters
       
    set type text
    set a $viewDataParameters($path.valueTmp)
    
    set value [ IPCChars $a]

    addParameterToSMmessage $viewID text $name $value 

}

proc calltext { o_name parent w viewID} {
    
    global viewDataParameters

    set value $viewDataParameters($parent.$o_name.value)
    set viewDataParameters($parent.$o_name.valueTmp) $value

    set state [getStatus $viewID $viewDataParameters($parent.$o_name.changeable)]

    set t $o_name
    frame $w.label$t -relief sunken
    button $w.label$t.label -text $viewDataParameters($parent.$o_name.text) -state $state -relief flat
 
    pack $w.label$t.label -side left
    pack $w.label$t -fill x

    frame $w.frame$t -relief flat
    pack $w.frame$t -padx 1 -pady 1 -fill x
    text $w.frame$t.text$t -relief sunken -bd 2 -height 5 -width 40 \
    -wrap word -yscrollcommand "$w.frame$t.s$t set"

    scrollbar $w.frame$t.s$t -relief sunken \
	-command "$w.frame$t.text$t yview"

    $w.frame$t.text$t insert 0.0 $value
    
    $w.frame$t.text$t configure -state $state
    
    pack $w.frame$t.text$t -side left -fill both -expand 1
    pack $w.frame$t.s$t -side right -fill y

    
    bind  $w.frame$t.text$t <KeyRelease> "checkTextField $parent.$o_name $w.frame$t.text$t $viewID"
}

proc checkTextField { field w viewID} {
    global viewDataParameters

    set value [ $w get 0.0 end]
    set field_ok 1
    
    if { $value == $viewDataParameters($field.valueTmp) } { return }

    if { $field_ok} {
	set viewDataParameters($field.valueTmp) $value
	setApplyStatus $viewID abled
    }
}


#
#    bool field
#


proc setbool { list parent } {
    global viewDataParameters
    set name [getName $list]
    if {[info exists viewDataParameters($parent.$name.bool)] } {
	alert "Can't make text field : already in use!"
	exit
    }

    set viewDataParameters($parent.$name.text) [lindex $list 2]
    set viewDataParameters($parent.$name.changeable) [lindex $list 3]
    set viewDataParameters($parent.$name.value) [lindex $list 4]
    set viewDataParameters($parent.$name.valueTmp) ""
}

proc unsetbool { name } {
    global viewDataParameters

    if {[info exists viewDataParameters($name.bool)] } {
	unset viewDataParameters($name.text)
	unset viewDataParameters($name.changeable)
	unset viewDataParameters($name.value)
	unset viewDataParameters($name.valueTmp)
    }
}

proc commitChangesbool { name } {
    global viewDataParameters
    set viewDataParameters($name.value) $viewDataParameters($name.valueTmp)    
}

proc applybool { path name viewID } {
    global viewDataParameters
       
    set type bool
    set value $viewDataParameters($path.valueTmp)
    
    
    addParameterToSMmessage $viewID bool $name $value 

}

proc callbool { o_name parent w viewID} {
    
    global viewDataParameters

    set value $viewDataParameters($parent.$o_name.value)
    set viewDataParameters($parent.$o_name.valueTmp) $value

    set state [getStatus $viewID $viewDataParameters($parent.$o_name.changeable)]

    set t $o_name
    frame $w.label$t -relief sunken
    button $w.label$t.label -text $viewDataParameters($parent.$o_name.text) -state $state -relief flat
 

    
    frame $w.frame$t -relief sunken
    checkbutton $w.frame$t.bool  -state $state \
	-variable viewDataParameters($parent.$o_name.valueTmp) \
	-command "checkBoolField  $parent.$o_name $viewID"
    
    pack $w.frame$t $w.frame$t.bool -side left
    pack $w.label$t.label $w.label$t -side left
  
}

proc checkBoolField { field  viewID} {
    global viewDataParameters

    
    if { $viewDataParameters($field.valueTmp) == $viewDataParameters($field.value) } { 
	return 
    } else {
	set viewDataParameters($field.value)  $viewDataParameters($field.valueTmp)
	setApplyStatus $viewID abled
    } 

}


#
#    RadioButtonGroup field
#

proc setRadioButtonGroup { list parent } {
    global viewDataParameters
    set name [getName $list]
    if {[info exists viewDataParameters($parent.$name)] } {
	alert "Can't make RadioButtonGroup field : already in use!"
	exit
    }

    set viewDataParameters($parent.$name) 1

    set c {}

    set children [lrange $list 2 end]
    foreach child $children {
	set[getType $child] $child $parent.[getName $list]
	lappend c [list [getType $child] [getName $child] ]
    }        
    set viewDataParameters($parent.$name.children) $c
    set viewDataParameters($parent.$name.Status) 0
    set viewDataParameters($parent.$name.StatusTmp) 0
}

proc unsetRadioButtonGroup { name } {
    global viewDataParameters

    if {[info exists viewDataParameters($name)] } {
	
	unset viewDataParameters($name)
	
	set children $viewDataParameters($name.children)
	foreach child $children {
	    unset[getType $child]  $name.[getName $child]
	}        
	unset viewDataParameters($name.children)
	unset viewDataParameters($name.Status)
	unset viewDataParameters($name.StatusTmp)
    }
}

proc commitChangesRadioButtonGroup { name } {
    global viewDataParameters
    set viewDataParameters($name.Status) $viewDataParameters($name.StatusTmp)
    commitChangesGroupWindow $name
}

proc applyRadioButtonGroup { path name viewID } {
    global viewDataParameters
    set type RadioButtonGroup
    set selButtonNum $viewDataParameters($path.StatusTmp)
    ##set viewDataParameters($path.Status) $selButtonNum
    set activeButton [lindex $viewDataParameters($path.children) $selButtonNum]
   
    set value [lindex $activeButton 1]
    

    addParameterToSMmessage $viewID RadioButtonGroup $name $value 

    #eval toSessionManager "\"[get_messageTemplate parameter]\""
    
    applyGroupWindow $path.$value $viewID
}
proc callRadioButtonGroup { o_name parent w viewID} {
    global viewDataParameters

    set groupName $o_name
    set commonFrame $w.rbglobal$groupName
    frame $commonFrame -relief raised -bd 1
    frame $w.rbwindow$groupName -relief raised -bd 1

    ## restore current selection
    set stat $viewDataParameters($parent.$o_name.Status)
    set viewDataParameters($parent.$o_name.StatusTmp) $stat

    # names of buttons
    set lbuttons $viewDataParameters($parent.$o_name.children)
    set i 0

    foreach b $lbuttons {

	#name of radio button
	set bName [getName $b]

	frame $commonFrame.rbg$groupName$i -relief raised 
	set bState [getStatus $viewID $viewDataParameters($parent.$o_name.$bName.changeable)]
	radiobutton $commonFrame.rbg$groupName$i.b$i -value $i -state $bState \
	    -variable viewDataParameters($parent.$o_name.StatusTmp) -text $viewDataParameters($parent.$o_name.$bName.text) \
	    -command "callRBWindow $bName $parent.$o_name $w.rbwindow$groupName $viewID"
	pack $commonFrame.rbg$groupName$i.b$i -side left

	pack $commonFrame.rbg$groupName$i -fill both
	
	incr i

   }

    pack $commonFrame  -fill both 
    pack $w.rbwindow$groupName -fill both

    set bName [lindex $lbuttons $viewDataParameters($parent.$o_name.StatusTmp)]
    
    callRBWindow [getName $bName] $parent.$o_name $w.rbwindow$groupName $viewID	


}


proc callRBWindow { o_name parent w viewID} {
    
    global viewDataParameters

    if { [winfo exists $w] } {
	
	destroy $w
	
    } 

    setApplyStatus $viewID abled

    set lcontent $viewDataParameters($parent.$o_name.children)

    if { [llength $lcontent] > 0 } {
	frame $w -relief raised -bd 1
	foreach i $lcontent {

	    call[getType $i] [getName $i] $parent.$o_name $w $viewID
	}
	pack $w -fill both 
    }
}

#
#    RadioButton field
#

proc setRadioButton { list parent } {
    global viewDataParameters
    
    set name [getName $list]

    if {[info exists viewDataParameters($parent.$name.text)] } {
	alert "Can't make RadioButton field : already in use!"
	exit
    }

    set viewDataParameters($parent.$name.text) [lindex $list 2]
    set viewDataParameters($parent.$name.changeable) [lindex $list 3]

    set c {}
    
    set children [lrange $list 4 end]
    foreach child $children {
	set[getType $child] $child $parent.[getName $list]
	lappend c [list  [getType $child] [getName $child] ]
    }        
    set viewDataParameters($parent.[getName $list].children) $c

}

proc unsetRadioButton { name } {
    global viewDataParameters
    
     if {[info exists viewDataParameters($name.text)] } {
	
	
	unset viewDataParameters($name.text)
	unset viewDataParameters($name.changeable)
	
	set children $viewDataParameters($name.children)
	foreach child $children {
	    unset[getType $child]  $name.[getName $child]
	}        
	unset viewDataParameters($name.children)
    }
}
proc commitChangesRadioButton { name } {
    commitChangesGroupWindow $name
}

#
#    GroupWindow field
#

proc setGroupWindow { list parent } {
    global viewDataParameters

    set name [getName $list]

    if {[info exists viewDataParameters($parent.$name)] } {
	alert "Can't make RadioButton field : already in use!"
	exit
    }

    set viewDataParameters($parent.$name) 1

    set c {}
    
    set children [lrange $list 2 end]
    foreach child $children {
	set[getType $child] $child $parent.[getName $list]
	lappend c [list  [getType $child] [getName $child] ]
    }        
    set viewDataParameters($parent.[getName $list].children) $c

}

proc unsetGroupWindow { name } {
    global viewDataParameters
    
    if {[info exists viewDataParameters($name)] } {
	
	unset viewDataParameters($name) 
	
	set children $viewDataParameters($name.children)

	foreach child $children {
	    unset[getType $child] $name.[getName $child]
	}        
	unset viewDataParameters($name.children)
    }
}
proc commitChangesGroupWindow { name } {
    global viewDataParameters
    set contentlist $viewDataParameters($name.children)
    foreach i $contentlist {
	set t [getType $i]
	commitChanges$t $name.[getName $i]
    }
}

proc applyGroupWindow { path viewID } {
    global viewDataParameters

    set babies $viewDataParameters($path.children)

    foreach b $babies {

	switch -exact [getType $b] {
	    int {
		applyint $path.[getName $b] [getName $b] $viewID
	    }

	    bool {
		applybool $path.[getName $b] [getName $b] $viewID
	    }

	    text {
		applytext $path.[getName $b] [getName $b] $viewID
	    }

	    RadioButtonGroup {
		applyRadioButtonGroup $path.[getName $b] [getName $b] $viewID
	    }

	    RadioButton {
		applyGroupWindow $path.[getName $b]  $viewID
	    }
	    GroupWindow {
		applyGroupWindow $path.[getName $b] $viewID
	    }
	}
    }
}
proc callGroupWindow { o_name parent w viewID} {
    
    global viewDataParameters

    frame $w.frame$o_name -relief flat -bd 1
    pack $w.frame$o_name -padx 4 -pady 4 -side top -fill x

    
    set contentlist $viewDataParameters($parent.$o_name.children)
    foreach i $contentlist {
	set t [getType $i]
	call$t [getName $i] $parent.$o_name $w.frame$o_name $viewID
    }

}



proc simpleParameters { msgid parent title theParameters} {
    global viewDataParameters
    
    if { [winfo exists $parent.parameters] } {
	wm deiconify $parent.parameters
	raise $parent.parameters
	return 
    }

    set_probState $parent new
    set_probParameters $parent $theParameters

    set w1 $parent.parameters
    toplevel $w1 
    grab $w1
    wm title $w1 "Parameters for $title"
    
    callGroupWindow Parameters $parent $w1 $parent 
    button $w1.apply -text Apply -width 11 \
	-command "applySimpleParameters $parent {$msgid}"

    pack $w1.apply -padx 5m -pady 3m -ipady 1m -side left
    button $w1.close -text Close -width 11 \
	-command "closeSimpleParameters $parent"

    pack $w1.close -padx 5m -pady 3m -ipady 1m -side left 

}

proc applySimpleParameters { w msgid } {
    global paramToSM
    set paramToSM($w.applyMsg) ""

    applyGroupWindow $w.Parameters $w

    set viewID $w
    set value $paramToSM($viewID.applyMsg)
    eval toSessionManager "\"$msgid [get_messageTemplate parameter]\""

    delete_probState $w
    delete_probParameters $w
    destroy $w.parameters
}

proc closeSimpleParameters { w } {
    delete_probState $w
    delete_probParameters $w

    destroy $w.parameters
}