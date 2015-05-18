###########################################################
###########################################################
##
##     DATABASE BROWSE ENGINE
##
###########################################################
###########################################################

namespace eval Database::Browser {

  namespace export browse get_selected std_list_hook 
  namespace export list_goto toggle_selection list_scroll list_moveto list_new
  namespace export browse_done

  #
  # Variables
  #

  variable db_browse 
  variable db_browsing

  proc get_selected {} {
    variable db_browsing
    return $db_browsing(selected)
  }

  #
  #    init
  #

  proc init {} {
    variable db_browse 
    variable db_browsing 
    set db_browse(-title)                "Browse the database..."
    set db_browse(-edit)                 no
    set db_browse(-selection)            single
    set db_browse(-browse-mode)          calssified
    set db_browse(-enabled-categories)   {*}
    set db_browse(-disabled-categories)  {}
    set db_browse(-update-hook)          {Database::Browser::std_list_hook}
    set db_browse(-root-dir)             [Database::root_dir]
    set db_browse(-attachments)          edit

    set db_browsing(objects)  [list]  ;# list of objects to browse now
    set db_browsing(current)  -1      ;# index of a last selected object
    set db_browsing(selected) [list]  ;# all selected objects during browsing
  }

  init

  ##
  ##   std_list_hook 
  ##

  proc std_list_hook {dir} {
    return [Database::get_directory_children $dir]
  }

  proc root_dir {} {
    variable db_browse
    return $db_browse(-root-dir)
  }

  #options:
  #  -title                {plain text}
  #  -edit                 {enable/disable}
  #  -selection            {single/many/none}
  #  -view-mode            {directory/all-in-one/both}
  #  -buttons              {name1 action1 name2 action2 ...}
  #  -enabled-categories   {mask}
  #  -disabled-categories  {mask}

  ##
  ##   browse
  ##

  proc browse {w args} {
    variable db_browse 
    variable db_browsing 
    global   browse_done 

    if {[winfo exists $w]} { destroy $w }

    init

    # Parse the given options.

    set -root-dir             [Database::root_dir]
    set -edit                 no
    set -selection            single
    set -title                "Browse the database..."
    set -browse-mode          classified
    set -enabled-categories  {*}
    set -disabled-categories {}
    set -update-hook          {Database::Browser::std_list_hook}
    set -properties           {unselectable}  
    set -search               {disable}  

    ::extractArguments {-title -edit -selection -browse-mode -buttons \
       -root-dir -update-hook -enabled-categories -disabled-categories \
       -properties -search} $args

    # Store the given options.

    set db_browse(-root-dir)             ${-root-dir}
    set db_browse(-edit)                 ${-edit}
    set db_browse(-selection)            ${-selection}
    set db_browse(-title)                ${-title}
    set db_browse(-browse-mode)          ${-browse-mode}
    set db_browse(-enable-categories)   ${-enabled-categories}
    set db_browse(-disabled-categories) ${-disabled-categories}
    set db_browse(-update-hook)          ${-update-hook}
    set db_browse(-properties)           ${-properties}
    set db_browse(-search)               ${-search}

    # Make window to browse.

    toplevel $w -class Dialog
    wm title $w $db_browse(-title)
    wm iconname $w "browse" 
    make_window $w ${-buttons}
    path_update $w ${-root-dir}
    list_update $w ${-root-dir}

    set db_browsing(search) [list]

    Database::Utility::wait_for_dialog $w browse_done
    destroy $w

    foreach search_dir $db_browsing(search) {
      #Database::delete_directory_structure $search

      # We do copy objects into temporary directory during the searching.
      # So we need to delete the directory structure *and* all objects from
      # the search directory.
      Database::delete_directory $search_dir
    }

    Debug::leave $browse_done
  }

  ##
  ##   make_window
  ##

  proc make_window {w buttons} {
    Debug::enter 
    pack [make_header  $w]           -side top    -fill x 
    pack [make_buttons $w $buttons]  -side bottom -fill x 
    pack [make_list    $w]           -side left   -fill y
    pack [make_object  $w]           -side left   -fill y
    Debug::leave
  }

  ##
  ##   make_header
  ##

  set browse_mode classified

  proc make_header {w} {

    variable browse_mode

    # Make header.

    frame $w.header -relief flat

    # make left part of the header frame.
    frame $w.header.left -relief raised -bd 1 

    frame $w.database
    label $w.database.label -text "Database:"
    label $w.database.name  -text [Database::get_filename]
    pack  $w.database.label -side left
    pack  $w.database.name  -fill x -padx 2m -pady 1m

    frame $w.path
    label $w.path.label -text "Path: "
    menubutton $w.path.current -text {123456789 123456789 123456789} \
      -menu $w.path.current.menu -relief raised
    menu $w.path.current.menu
    pack $w.path.label -side left
    pack $w.path.current -fill x -padx 2m -pady 1m

    pack $w.database -in $w.header.left -side top -anchor w
    pack $w.path     -in $w.header.left -side top -anchor w

    # make right part of the header frame. 
#    frame $w.header.right -relief raised -bd 1
#
#    frame $w.view 
#    label $w.view.label -text "Browse mode"
#    radiobutton $w.view.classified   -variable $browse_mode \
#      -text "Classified"   -value "classified"
#    radiobutton $w.view.unclassified -variable $browse_mode \
#      -text "Unclassified" -value "unclassified"
#    pack $w.view.label -fill x  
#    pack $w.view.classified   -side top -anchor w
#    pack $w.view.unclassified -side top -anchor w
#
#    pack $w.view -in $w.header.right 

    pack $w.header.left -side left -fill both -expand 1
#    pack $w.header.right -side left -fill y

    return $w.header
  }

  ##
  ##   make_list
  ##

  proc make_list {w} {
    frame $w.list -relief raised -bd 2
    scrollbar $w.list.scroll -relief sunken \
      -command "Database::Browser::list_scroll $w.list.listbox"
    listbox $w.list.listbox -yscroll "$w.list.scroll set" \
      -relief sunken -width 35 -height 20 -exportselection false 

    $w.list.listbox configure -selectmode single

    pack append $w.list \
      $w.list.listbox {left} \
      $w.list.scroll {right expand filly}

    bind $w.list.listbox <Button-1> \
      "Database::Browser::list_moveto $w %y 1"
    #bind $w.list.listbox <ButtonRelease-1> \
    #  "Database::Browser::list_moveto $w %y 1"
    bind $w.list.listbox <Double-Button-1> \
      "Database::Browser::list_goto $w %y"
    bind $w.list.listbox <Button-3>\
      "Database::Browser::toggle_selection $w %y"

    return $w.list
  }

  ##
  ##   make_object
  ##

  proc make_object {parent args} {
    variable db_browse
    set -edit no
    ::extractArguments {-edit} $args

    set w $parent.object
    frame $w

    # Make name field

    frame $w.name -relief raised -bd 2
    label $w.name.label -text "Name"
    entry $w.name.entry -relief sunken
    pack $w.name.label -side left
    pack $w.name.entry -fill x -padx 2m -pady 1m
    pack $w.name -fill x
    bind $w.name.entry <Tab>    "focus $w.keywords.entry"
    bind $w.name.entry <Meta-o> "focus $w.keywords.entry"
    bind $w.name.entry <Return> "focus $w.keywords.entry"

    # make `path/category' field

    frame $w.path -relief raised -bd 2
    label $w.path.label -text "Path"
    entry $w.path.entry -relief sunken -state disabled
    pack $w.path.label -side left
    pack $w.path.entry -fill x -padx 2m -pady 1m
    pack $w.path -fill x

    # Make definition field

    frame $w.def -relief raised -bd 2
    label $w.def.label -text "Definition"
    frame $w.def.frame -relief flat
    text $w.def.frame.text -relief sunken -bd 2 -height 3 -width 50 \
      -wrap word -yscrollcommand "$w.def.frame.s set"
    scrollbar $w.def.frame.s -relief sunken \
      -command "$w.def.frame.text yview"
    pack $w.def.label -side top -anchor nw
    pack $w.def.frame.text -side left -fill both -expand 1
    pack $w.def.frame.s -side right -fill y
    pack $w.def.frame -side top -fill both -expand 1 -padx 2m -pady 1m
    pack $w.def -fill both -expand 1

    # Make keywords field

    frame $w.keywords -relief raised -bd 2
    label $w.keywords.label -text "Keywords"
    entry $w.keywords.entry -relief sunken

    pack $w.keywords.label -side left
    pack $w.keywords.entry -fill x -padx 2m -pady 1m
    pack $w.keywords -fill x
    bind $w.keywords.entry <Tab>    "focus $w.descr.frame.text"
    bind $w.keywords.entry <Meta-o> "focus $w.descr.frame.text"
    bind $w.keywords.entry <Return> "focus $w.descr.frame.text"

    # Make description field

    frame $w.descr -relief raised -bd 2

    frame $w.descr.first_line
    label $w.descr.first_line.label -text "Description" 
    button $w.descr.first_line.insert_attachment -relief raised -bd 2 \
        -text "Insert attachment" -width 17 \
        -command "Database::Browser::descr_insert_attachment $w"
    $w.descr.first_line.insert_attachment configure -state disabled

    frame $w.descr.frame -relief flat

    set height 3
#    if { ${-edit} == "yes" } { 
#      set height 3
#    }
    text $w.descr.frame.text -relief sunken -bd 2 -height $height -width 50 \
      -wrap word -yscrollcommand "$w.descr.frame.s set"
    scrollbar $w.descr.frame.s -relief sunken \
      -command "$w.descr.frame.text yview"

    pack $w.descr.first_line.label -side left
    pack $w.descr.first_line.insert_attachment -side top -anchor e
    pack $w.descr.first_line -side top -anchor nw -fill x
    pack $w.descr.frame.text -side left -fill both -expand 1
    pack $w.descr.frame.s -side right -fill y
    pack $w.descr.frame -side top -fill both -expand 1 -padx 2m -pady 1m
    

    pack $w.descr -fill both -expand 1
    bind $w.descr.frame.text <Tab>    "focus $w.name.entry"
    bind $w.descr.frame.text <Meta-o> "focus $w.name.entry"

    # Make properties buttons

    frame $w.property -relief raised -bd 2

#    button $w.property.attachments -text "Attachments" \
\#      -command "Database::Browser::manage_attachments $db_browse(-attachments)"\
\#      -width 20 -state disabled
    button $w.property.moreproperties -text "More properties" \
      -command "Database::Browser::view_properties $db_browse(-properties)" \
      -width 20 -state disabled

#    pack $w.property.attachments   -side left -expand 1
    pack $w.property.moreproperties -side left -expand 1
    pack $w.property -fill both -expand 1
 
    if {"${-edit}" != "yes"} {
      $w.name.entry       configure -state disabled
      $w.def.frame.text   configure -state disabled
      $w.keywords.entry   configure -state disabled
      $w.descr.frame.text configure -state disabled
    }

    return $w
  }

  ##
  ##   make_buttons
  ##

  proc make_buttons {w buttons} {
    frame $w.buttons

    for {set i 0} {$i < [llength $buttons]} {incr i 2} {
      set title [lindex $buttons $i]
      set action [lindex $buttons [expr $i+1] ]
      set bwidth [expr [string length $title]+2] 
      set bname [string tolower $title]
      button $w.buttons.$bname -relief raised -bd 2 -text $title \
	-width $bwidth -command "Database::Browser::button_precondition $w; $action $w; set browse_done $i"
      pack $w.buttons.$title -side left -expand 1 -padx 3m -pady 3m -ipady 1m
    }

    variable db_browse
    if {$db_browse(-search) == "enable"} {
      button $w.buttons.search -relief raised -bd 2 -text "Search" \
        -width 8 -command "Database::Browser::search $w"
      pack $w.buttons.search -side left -expand 1 -padx 3m -pady 3m -ipady 1m
    }

    return $w.buttons
  }

  proc search {parent} {
    global search_choice 

    set w [uniqueDialogName] 
    toplevel $w -class Dialog
    wm title $w "Search window"
    wm iconname $w "search" 
    
    frame $w.text -relief raised -bd 2
    label $w.text.label -text "Search for:"
    entry $w.text.entry -relief sunken
    pack $w.text.label -side left
    pack $w.text.entry -fill x -padx 2m -pady 1m
    pack $w.text -fill x -side top

    frame $w.buttons -relief raised -bd 2
    foreach place {keywords description properties} {
      set bwidth [expr [string length $place]+2] 
      global search_$place; set search_$place 1
      checkbutton $w.buttons.$place -relief raised -bd 2 -anchor w \
        -text "Search in $place" -width $bwidth -variable search_$place 
      pack $w.buttons.$place -side top -fill x -expand 1 
    }
    pack $w.buttons -fill x -side top
 
    frame $w.choice -relief raised -bd 2
    button $w.choice.ok -relief raised -bd 2 -text "OK" \
      -width 8 -command "set search_choice do_search"
    pack $w.choice.ok -side left -expand 1 -padx 3m -pady 3m -ipady 1m
    button $w.choice.cancel -relief raised -bd 2 -text "Cancel" \
      -width 8 -command "set search_choice cancel_seacrh"
    pack $w.choice.cancel -side left -expand 1 -padx 3m -pady 3m -ipady 1m
    pack $w.choice -fill x -side top

    Database::Utility::wait_for_dialog $w search_choice

    set text [$w.text.entry get]
    destroy $w

    if {$search_choice == "do_search"} {
      if {$text == ""} {
        tk_dialog [uniqueDialogName] "Information" "There is no text to search for. The search is aborted." question 0 OK
        return
      } 

      global search_keywords search_description search_properties
      
      Database::Browser::do_search $parent $text \
        "$search_keywords $search_description $search_properties"
    }
  } 



  ##
  ## Search facilities
  ##

  proc do_search {w text places} {
    variable db_browsing
 
    set root [Database::Browser::root_dir]
    set search_dir [Database::new_directory $root TMP "Search: $text"]
    lappend db_browsing(search) $search_dir
    
    # Make copies of found objects and save them into 
    # the search directory
    foreach entry [Database::do_search $text $places] {
      set new_object [Database::unique_object_id]
      set name         [Database::get_object_name $entry]
      set definition   [Database::get_object_definition $entry]
      set keywords     [Database::get_object_keywords $entry]
      set dependencies {}
      set dependants   {}
      set description  [Database::get_object_description $entry]
      set properties   [Database::get_object_properties $entry]

      Database::new_object $new_object $name $definition $keywords \
        $dependencies $dependants $description
      Database::set_object_properties $new_object $properties

      Database::new_entry $search_dir $new_object object $name \
        "ONLY DIRECTORY MAY HAVE A CATEGORY" 
    }
    list_new $w $search_dir 1
  }

  #------------------------------------------------------------
  #
  #  Event handlers
  #
  #------------------------------------------------------------

  ##
  ##   list_scroll
  ##

  proc list_scroll {w a b {c ""}} {
    Debug::enter
    if {$c != ""} {
      $w yview $a $b $c
    } else {
      $w yview $a $b
    }
    Debug::leave
  }

  ##
  ##   list_moveto
  ##

  proc list_moveto {w y save_last} {
    Debug::enter

    variable db_browsing
    variable db_browse

    set current $db_browsing(current)
    Debug::print "current is $current"

    set pos [$w.list.listbox nearest $y]

    if {$current != -1 && $save_last && $db_browse(-edit) == "yes"} {
      object_apply_changes $w [lindex $db_browsing(objects) $current]
    }

    set object [lindex $db_browsing(objects) $pos]
    Debug::print "object: $object"
    if {$object == ""} { return }	

    if {[Database::get_directory_type $object] == "object"} {
      object_update $w $db_browse(-edit) [lindex $db_browsing(objects) $pos]
    } else {
      clear_object_details $w
    }
    set db_browsing(current) $pos

    Debug::leave
  }

  ##
  ##   list_goto
  ##

  proc list_goto {w y} {
    Debug::enter

    variable db_browsing

    list_moveto $w $y 1
    set pos [$w.list.listbox nearest $y]
    set object [lindex $db_browsing(objects) $pos]
    if {$object == ""} { return }
    if {[Database::get_directory_type $object] != "dir"} { return }
    list_new $w $object 0
    Debug::leave
  }

  #------------------------------------------------------------
  #
  #  Update views
  #
  #------------------------------------------------------------

  ##
  ##   update_entry 
  ##

  proc update_entry {entry edit text} {
    $entry configure -state normal
    $entry delete 0 end
    $entry insert 0 $text
    if {"$edit" != "yes"} {
      $entry configure -state disabled
    }
  }

  ##
  ##   update_text
  ##

  proc update_text {entry edit text} {
    $entry configure -state normal
    $entry delete 1.0 end
    $entry insert 1.0 $text
    if {"$edit" != "yes"} {
      $entry configure -state disabled
    }
  }

  ##
  ##   object_update_details
  ##

  proc object_update_details {oid parent edit name path def keywords descr} {
    Debug::enter
    set w $parent.object
    update_entry $w.name.entry       $edit $name
    update_entry $w.path.entry       no    $path
    update_text  $w.def.frame.text   no    $def
    update_entry $w.keywords.entry   $edit $keywords

    descr_load_text $w $descr
    if {"$edit" == "yes"} {
      $w.descr.frame.text configure -state normal
      $w.descr.first_line.insert_attachment configure -state normal
    } else {
      $w.descr.frame.text configure -state disabled
      $w.descr.first_line.insert_attachment configure -state disabled
    }
    #update_text  $w.descr.frame.text $edit $descr

    #variable db_browsing
    #set object [lindex $db_browsing(objects) $db_browsing(current)]    
    Debug::print "object_update_details: oid is '$oid'"

    set properties_names [Database::get_object_properties_names $oid]
    if { [llength $properties_names] > 0 } {
      $w.property.moreproperties configure -state normal
    } else {
      $w.property.moreproperties configure -state disabled
    }

    Debug::leave
  }

  proc clear_object_details {parent} {
    Debug::enter
    set w $parent.object
    update_entry $w.name.entry       no ""
    update_entry $w.path.entry       no ""
    update_text  $w.def.frame.text   no ""
    update_entry $w.keywords.entry   no ""
    update_text  $w.descr.frame.text no ""
#    $w.property.attachments    configure -state disabled
    $w.property.moreproperties configure -state disabled
    Debug::leave
  }

  ##
  ##   object_update
  ##

  proc object_update {winparent edit oid} {
    Debug::enter 

    set path ""
    set dir  $oid
    set EXP  [Database::get_dir_id EXPERIMENTS]
    set root [Database::Browser::root_dir]
    while {$dir != $root && $dir != $EXP} {
      set path "/[Database::get_directory_name $dir]$path"
      set dir  [Database::get_directory_parent $dir]
    }

    Debug::print "description is (oid $oid): [Database::get_object_description $oid]"

    object_update_details $oid $winparent $edit \
      [Database::get_object_name        $oid] $path \
      [Database::get_object_definition  $oid] \
      [Database::get_object_keywords    $oid] \
      [Database::get_object_description $oid] 

    Debug::leave
  }

  ##
  ##   list_update
  ##

  proc list_update {w dir} {
    Debug::enter

    variable db_browse
    variable db_browsing

    set user_action $db_browse(-update-hook)
    set db_browsing(objects) [$user_action $dir]

    if {$db_browse(-browse-mode) != "classified"} {
      set dirs $db_browsing(objects)
      set objs [list]
      while {[llength $dirs] > 0} {
	set curobj [lindex $dirs 0]
	Debug::print "inspect: $curobj"
	set dirs [lreplace $dirs 0 0]
	if {[Database::get_directory_type $curobj] == "dir"} {
	  foreach child [Database::get_directory_children $curobj] { 
	    lappend dirs $child
	  }
	} else {
	  lappend objs $curobj
	}
      } 
      set db_browsing(objects) $objs 
    }

    $w.list.listbox selection clear 0 end
    $w.list.listbox delete 0 end
    foreach object $db_browsing(objects) {
      set name [Database::get_directory_name $object]
      if {[is_selected $object]} {
	set name "*$name"
      } else {
	set name " $name"
      }  
      $w.list.listbox insert end $name
    }

    Debug::leave
  }

  #------------------------------------------------------------
  #
  #  
  #
  #------------------------------------------------------------

  ##
  ##   object_apply_changes
  ##

  proc object_apply_changes {w object} {
    Debug::enter 

    variable db_browsing
  
    if {[Database::get_directory_type $object] == "dir"} { return }

    set new_name [$w.object.name.entry get]
    set old_name [Database::get_object_name $object]
    if {$new_name != $old_name} {
      Database::set_object_name $object $new_name
      Database::set_directory_name $object $new_name
      set idx [lsearch $db_browsing(objects) $object]
      if {$idx == -1} { 
        Debug::print "error in Database::Browser::object_apply_changes" 
      }
      $w.list.listbox delete $idx 
      if {[is_selected $object]} {
	$w.list.listbox insert $idx "*$new_name"
      } else {
	$w.list.listbox insert $idx " $new_name"
      }
    }

    Database::set_object_keywords    $object [$w.object.keywords.entry get]
    # Database::set_object_description $object \
    #   [$w.object.descr.frame.text get 1.0 end-1char]

    object_apply_modified_description $w.object $object

    Debug::leave
  }

  ##
  ##   list_new 
  ##

  proc list_new {w dir save_last} {
    # make listbox for new directory

    Debug::enter 

    variable db_browsing

    if {$save_last} {
      set ypos [lindex [$w.list.listbox bbox $db_browsing(current)] 1]
      if {$ypos == ""} {set ypos 3}
      list_moveto $w $ypos $save_last
    }
    list_update $w $dir
    set $db_browsing(current) -1
    clear_object_details $w
    list_moveto $w 1 0
    path_update $w $dir

    Debug::leave
  }

  #------------------------------------------------------------
  #
  #  Selection support
  #
  #------------------------------------------------------------

  ##
  ##   is_selected
  ##

  proc is_selected {object} {
    Debug::enter 
    variable db_browsing
    if {[lsearch $db_browsing(selected) $object] != -1} {return 1}
    Debug::leave 0
  }

  ##
  ##   select_object
  ##

  proc select_object {w object} {
    variable db_browse 
    variable db_browsing 

    if {$db_browse(-selection) == "none"} { return }

    Debug::print "selected objects: $db_browsing(selected)"
    Debug::print "add object: $object"

    set index [lsearch $db_browsing(selected) $object]
    if {$index != -1} { return }

    if {$db_browse(-selection) == "single" 
	|| $db_browse(-selection) == "directory" } {
      Debug::print "remove other selections"
      deselect_object $w [lindex $db_browsing(selected) 0]
    }
    lappend db_browsing(selected) $object

    Debug::print "added object: $db_browsing(selected)"

    set listidx [lsearch $db_browsing(objects) $object]
    if {$listidx != -1} { 
      $w.list.listbox delete $listidx
      $w.list.listbox insert $listidx "*[Database::get_directory_name $object]"
    }

    update idletasks
  }


  ##
  ##   deselect_object
  ##

  proc deselect_object {w object} {
    variable db_browsing

    Debug::enter
    set index [lsearch $db_browsing(selected) $object]
    if {$index == -1} { return }

    Debug::print "old selection: $db_browsing(selected)"
    Debug::print "delete selected object: $object"

    set db_browsing(selected) [lreplace $db_browsing(selected) $index $index]

    Debug::print "new selection: $db_browsing(selected)"

    set listidx [lsearch $db_browsing(objects) $object]
    if {$listidx != -1} {
      $w.list.listbox delete $listidx
      $w.list.listbox insert $listidx " [Database::get_directory_name $object]"
    }
    update idletasks
    Debug::leave
  }

  ##
  ##   toggle_selection
  ##

  proc toggle_selection {w y} {
    variable db_browse
    variable db_browsing
    set pos [$w.list.listbox nearest $y]
    set object [lindex $db_browsing(objects) $pos]
    if { [Database::exists_object $object] 
         || [Database::exists_directory $object]  
            && $db_browse(-selection) == "directory" } {

      if {[is_selected $object]} {
	deselect_object $w $object
      } else {
	select_object $w $object
      }
      $w.list.listbox selection clear 0 end
      $w.list.listbox selection set $pos
    }
  }


  ##
  ##   path_update
  ##

  proc path_update {w dir} {
    Debug::enter
    $w.path.current configure -text [Database::get_directory_name $dir]

    set path $dir
    set EXP [Database::get_dir_id EXPERIMENTS]
    set root [Database::Browser::root_dir]
    while {$dir != $root && $dir != $EXP} {
      set dir [Database::get_directory_parent $dir]
      lappend path $dir
    } 

    $w.path.current.menu delete 0 last
    foreach dir $path {
      $w.path.current.menu add command \
	-label [Database::get_directory_name $dir] \
	-command "Database::Browser::list_new $w $dir 1"
    }

    Debug::leave
  }

  proc button_precondition {w} {
    variable db_browsing 
    variable db_browse
    set current $db_browsing(current)
    if {$current != -1 && $db_browse(-edit) == "yes"} {
      object_apply_changes $w [lindex $db_browsing(objects) $current]
    }
  }

  

  #------------------------------------------------------------
  #
  #  View properties
  #
  #------------------------------------------------------------

  proc property_button {w varname name have} {
    button $w.$varname -text "($have) $name" -anchor w
    $w.$varname configure -activefore [$w.$varname cget -fg] \
      -activeback [$w.$varname cget -bg]    
    pack $w.$varname -side top -fill x -expand on
  }

  proc property_sel_button {w varname name have selected} {
    upvar $varname var
    set var $selected
    checkbutton $w.$varname -text "($have) $name" -variable $varname \
      -justify left -anchor w
    pack $w.$varname -side top -fill x -expand on
  }

  #proc property_button {w varname name have} \{
  #  upvar $varname var
  #  set var $selected
  #  button $w.$varname -text "($have) $name" 
  #  $w.$varname configure -activefore [$w.$varname cget -fg] \
  #    -activeback [$w.$varname cget -bg] -justify left
  #  pack $w.$varname -side top -anchor w -fill x -expand on
  #\}

  ##
  ##   view_properties
  ##

  proc view_properties {selectable} {
    variable db_browsing

    set object [lindex $db_browsing(objects) $db_browsing(current)]
    set w [uniqueDialogName]

    set selected_properties \
      [Database::get_object_selected_properties $object] 
    set unselected_properties \
      [Database::get_object_unselected_properties $object]


    #if $object == -1  ...
    
    toplevel $w -class Dialog
    wm title $w "Properties window" 
    wm iconname $w "Properties" 
    wm iconify $w

    set oldFocus [focus]

    frame $w.f
    button $w.f.ok     -text "OK"     -width 8 -command "set view_code 0"
    button $w.f.cancel -text "Cancel" -width 8 -command "set view_code 1"
    pack $w.f.ok $w.f.cancel -side left -expand on
    pack $w.f -pady 10 -side bottom -anchor w -fill x

    frame $w.topline -relief ridge -borderwidth 2 -height 2
    pack $w.topline -side top -fill x

    frame $w.botline -relief ridge -borderwidth 2 -height 2
    pack $w.botline -side bottom -fill x

    frame $w.config
    pack $w.config -side right -fill y -expand on

    scrollbar $w.config.vscroll -command "$w.config.canvas yview"
    pack $w.config.vscroll -side right -fill y

    canvas $w.config.canvas -height 1 \
      -relief flat -borderwidth 0 -yscrollcommand "$w.config.vscroll set" \
      -width [expr [winfo screenwidth .] * 1 / 2] 

    frame $w.config.f
    pack $w.config.canvas -side right -fill y

    # add properties list
    
    set unsel_props [Database::get_object_unselected_properties $object]
    set properties_names [Database::get_object_properties_names $object] 
    set idx 0

    foreach property $properties_names {
      set have [Database::get_object_property_type $object $property]
      if {$selectable == "selectable"} {
        if {[lsearch $unsel_props $property] == -1} {
          set selected 1
        } else {
          set selected 0
        }
        global propvar$idx
        set propvar$idx $selected
        property_sel_button $w.config.f propvar$idx $property $have $selected
      } else {
        property_button $w.config.f propvar$idx $property $have
      }
      incr idx 1
    }
    set prop_count $idx

    # focus $w
    # update_menu1 $w.config.f

    global winx 
    global winy
    set winx [expr [winfo x .]+30]
    set winy [expr [winfo y .]+30]
    wm geometry $w +$winx+$winy
    update idletasks
    $w.config.canvas create window 0 0 -anchor nw -window $w.config.f

    $w.config.canvas configure \
      -width [expr [winfo reqwidth $w.config.f] + 1]\
      -scrollregion "-1 -1 [expr [winfo reqwidth $w.config.f] + 1] \
      [expr [winfo reqheight $w.config.f] + 1]"

    set winy [expr [winfo reqh $w] - [winfo reqh $w.config.canvas]]
    set scry [expr [winfo screenh $w] / 2]
    set maxy [expr [winfo screenh $w] * 3 / 4]
    set canvtotal [expr [winfo reqh $w.config.f] + 2]
    if [expr $winy + $canvtotal < $maxy] {
      $w.config.canvas configure -height $canvtotal
    } else {
      $w.config.canvas configure -height [expr $scry - $winy]
    }
    update idletasks
    wm maxsize $w [winfo width $w] [winfo screenheight $w]
    wm minsize $w [winfo width $w] 100

    wm deiconify $w

    global view_code
    Database::Utility::wait_for_dialog $w view_code
    destroy $w 

    #global view_code
    if {$view_code == 1} {
      # `cancel' button is pressed.
      return
    }

    if {$selectable == "selectable"} {
      for {set i 0} {$i < $prop_count} {incr i 1} {
        set propvar "propvar$i"
        set selected [expr $$propvar]
        set property [lindex $properties_names $i]
        if { $selected } {
          Database::set_object_select_property $object $property 
        } else { 
          Database::set_object_unselect_property $object $property
        }
      }
    }

  }	


  ##
  ## Description manipulations routines.
  ##

  set tag_links(count) 0

  proc descr_load_text {w text} {

    set t $w.descr.frame.text

    # Find and reformat all links

    set tagIndices [list]
    # Used below for tagging links

    # Save protected '@'s
    regsub -all {\\\@} $text \t text

    while { [regexp -indices {@([^@]+)@([^@]+)@} $text all idxs1 idxs2] } {
      set indices [list [lindex $idxs1 0] [lindex $idxs1 1]]
      lappend tagIndices $indices
      set attachID [string range $text [lindex $idxs2 0] [lindex $idxs2 1]]
      set attachIDs($indices) $attachID
      # Wrong kind of indices; will fix below
      regsub {@([^@]+)@([^@]+)@} $text {\1} text
    }

    # Put back protected '@'s
    regsub -all \t $text "@" text

    # Reset the state of the text widget
    $t configure -state normal
    $t delete 1.0 end
    $t tag delete link
    $t insert 1.0 $text

    variable tag_links
    unset tag_links
    set tag_links(count) 0

    # Tag all links
    set tagNumber 0
    foreach indices $tagIndices {
      set i [$t index 1.0+[lindex $indices 0]c-1c]
      set j [$t index 1.0+[lindex $indices 1]c]
      $t tag add $tagNumber $i $j
      set attachID $attachIDs($indices)
      set tag_links($tagNumber) $attachID
      # Bind tag
      $t tag configure $tagNumber \
        -font *-times-medium-i-normal--*-140-*-*-*-*-*-*
      if { ![Database::is_valid_attachment $attachID] } {
        $t tag configure -foreground grey60
      }
      $t tag bind $tagNumber <Enter>    \
        "Database::Browser::descr_enter_link $t"
      $t tag bind $tagNumber <Leave>    \
        "Database::Browser::descr_leave_link $t"
      $t tag bind $tagNumber <Button-1> \
        "Database::Browser::descr_show_link $t $attachID"
      $t tag bind $tagNumber <Button-3>  \
        "Database::Browser::descr_save_link $w $attachID"
      incr tagNumber
    }
    set tag_links(count) $tagNumber

    #@dp Need to make compatible with size changes
    $t configure -cursor top_left_arrow -state disabled
  }

  proc object_apply_modified_description {w object} {
    set t $w.descr.frame.text
    variable tag_links

    Debug::write_log ATTACHMENTS.EXTRA.DETAILS "Convert description into database format.\nDescription is: [$t get 1.0 end-1c]"

   # foreach tag [$t tag names] 
   #   if { [regexp "^\[0-9\]+$" $tag] }
   variable tag_links
   for {set tag 0} {$tag < $tag_links(count)} {incr tag} {
      set ranges [$t tag ranges $tag]
      Debug::assert {[llength $ranges] <= 2}
      if { [llength $ranges] == 0 } {
        # deleted tag
        unbind_attachment $tag_links($tag)
      } else {
        # existed tag
        set startpos [lindex $ranges 0]
        set endpos   [lindex $ranges 1]
        set txt [$t get $startpos $endpos]
        $t delete $startpos $endpos
        $t insert $startpos "@$txt@$tag_links($tag)@"
        Debug::write_log ATTACHMENTS.EXTRA.DETAILS \
          "updated tag $tag: [$t get 1.0 end-1c] "
      }
      $t tag delete $tag
    }

    Debug::write_log ATTACHMENTS.EXTRA.DETAILS "Conversion is done. Description is: [$t get 1.0 end-1c]"

    Database::set_object_description $object [$t get 1.0 end-1c]
  }

  proc descr_enter_link {w} {
    $w configure -cursor hand1
  }

  proc descr_leave_link {w} {
    $w configure -cursor xterm
  }

  set dvi_files(index) [list]

  proc unregister_all_dvi_files {} {
    #Debug::write_log ATTACHMENTS.TRACE
    variable dvi_files
    unset dvi_files
    set dvi_files(index) [list]
  }

  proc register_dvi_file {attachID} {
    set filename ""
    if { [catch {set filename $dvi_file(attachID)}] } {
      global tmp_dir
      set tmptmpdir [file join $tmp_dir dvi [Database::Utility::tmp_filename]]
      file mkdir $tmptmpdir
      set filename [file join $tmptmpdir [Database::get_attachment_filename $attachID]]
      Debug::print "Registered file '[Database::get_attachment_filename $attachID]' under the name '$filename'"
      set startpos [Database::get_attachment_startpos $attachID]
      set length   [Database::get_attachment_length   $attachID]
      Database::part_copy_file $filename [Database::get_secondary_filename] \
        $startpos $length
      set dvi_file($attachID) $filename 
    }
    Debug::assert {$filename != ""}
    return $filename
  }

  proc descr_show_link {w attachID} {
    Debug::enter
    if { ![Database::is_valid_attachment $attachID] } {
      tk_dialog [uniqueDialogName] "Warning" \
        "This attachment has broken link and cannot be viewed." \
        warning 1 OK
      return 
    }
    set filename [register_dvi_file $attachID]
    plugin_exec xdvi $filename
    Debug::leave
  }

  proc descr_save_link {w attachID} {
    Debug::enter
    set dir [pwd]
    if { ![Database::is_valid_attachment $attachID] } {
      tk_dialog [uniqueDialogName] "Warning" \
        "This attachment has broken link and cannot be saved." \
        warning 1 OK
      return 
    }
    set filename [Database::get_attachment_filename $attachID]
    set startpos [Database::get_attachment_startpos $attachID]
    set length   [Database::get_attachment_length   $attachID]
    set types {
      { {DVI Files} {.dvi} }
      { {All Files} {*} }
    }
    set filename [tk_getSaveFile -initialfile $filename -parent $w \
                                 -defaultextension {.dvi} -filetypes $types]
    if { $filename == "" } {
      return 
    }
    if { [catch {set fileId [::open $filename w+]} err] } {
      regsub -all \" $err "'" safe_err_msg
      tk_dialog [uniqueDialogName] "Warning" \
        "Could not write the specified file: $safe_err_msg." warning 1 OK
      return
    }
    Database::part_copy_file $filename [Database::get_secondary_filename] \
      $startpos $length
    Debug::leave
  }

  proc descr_insert_attachment {w} {
    # choose file for the attachment
    set types {
      { {DVI Files} {.dvi} }
      { {All Files} {*} }
    }
    set filename [tk_getOpenFile -parent $w -filetypes $types]
    if { $filename == "" } {
       return
    }
    if { ![file readable $filename] } {
      tk_dialog [uniqueDialogName] "Warning" \
        "Could not read the file '$filename'" warning 1 OK
      return
    }

    # bind new attachment
    set attachID [Database::bind_attachment $filename]
    Debug::print "Inserted attachment has ID = $attachID"
    set filename [file tail $filename]

    # get insert position
    set t $w.descr.frame.text
    set pos [$t index insert]

    # move new link to the right if there is an another attachment in the insert position.
    set tags [$t tag names insert]
    Debug::assert {[llength $tags] <= 1}
    if { [llength $tags] == 1 } {
      set tagrange [$t tag ranges $tags]
      Debug::assert {[llength $tagrange] == 2}
      set pos [lindex $tagrange 1]
    }

    regexp "(\[0-9\]+)\.(\[0-9\]+)" $pos "" line col

    # insert new link with optional spaces around.
    if { [$t get $pos] != " " && [$t get $pos] != "\n" } {
      $t insert $pos " "
    }
    if { $col > 0 && [$t get $pos-1c] != " " } {
      $t insert $pos " "
      incr col
      set pos $line.$col
    }
    $t insert $pos $filename

    # register new tag
    variable tag_links
    set tagNumber $tag_links(count)
    incr tag_links(count)
    set  tag_links($tagNumber) $attachID

    $t tag add $tagNumber $pos $pos+[string length $filename]c
    $t tag configure $tagNumber \
      -font *-times-medium-i-normal--*-140-*-*-*-*-*-*
    $t tag bind $tagNumber <Enter>    \
      "Database::Browser::descr_enter_link $t"
    $t tag bind $tagNumber <Leave>    \
      "Database::Browser::descr_leave_link $t"
    $t tag bind $tagNumber <Button-1> \
      "Database::Browser::descr_show_link $t $attachID"
    $t tag bind $tagNumber <Button-3>  \
      "Database::Browser::descr_save_link $w $attachID"
  }

} 
# end of namespace: Database::Browser 


#
# Global namespace
#

set plugin_list(index) [list]

proc plugin_register {plugin program} {
  global plugin_list
  set plugin_list($plugin) $program
}

proc plugin_exec {plugin args} {
  global plugin_list
  set program ""
  if { [catch { set program $plugin_list($plugin) }] } {
    tk_dialog [uniqueDialogName] "Information" "Could not run plugin '$plugin'." warning 0 OK
  } else {
    Debug::print "executing: $program $args"
    if { [file executable $program] } {
      set errMesg ""
      if { [catch { exec $program $args } errMesg] } {
        tk_dialog [uniqueDialogName] "Information" $errMesg warning 0 OK
      }
	# >>& ~/magnus_err
      # </dev/zero >&/dev/null
    } else {
      global errorInfo
      Debug::print "Error during plugin execution: $errorInfo"
      tk_dialog [uniqueDialogName] "Information" "Could not run plugin '$plugin'" warning 0 OK
    }
  }
}

plugin_register xdvi /home/users/pechkin6/bin/xdvi
#/usr/bin/xdvi.bin
