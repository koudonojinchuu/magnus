#####################################################################
##
##   I/O operations with entire database 
##
#####################################################################

proc new_filename_ {} {
  global db 

  set w [uniqueDialogName]
  set filename [savefile $w "Choosing a name for new database..."]

  if {$filename == ""} {
    toSessionManager [get_messageTemplate db_cancel]
    return
  }

  exec touch $filename

  if { ![file writable $filename] } {
    toSessionManager "[get_messageTemplate db_failure] File is not writable."
    return
  }
  toSessionManager "[get_messageTemplate db_success] $filename"
}

##
##   new_database_
##

proc new_database_ {filename} {
  Database::create
  Database::set_filename $filename
}

##
##   open_database_
##

proc open_database_ {} {
  set w [uniqueDialogName]
  set filename [openfile $w "Open a database..."]

  if {$filename == ""} {
    toSessionManager [get_messageTemplate db_cancel]
  } elseif { ! [file exists $filename] || ![file readable $filename] } {
    toSessionManager "[get_messageTemplate db_failure] File is not readable."
  } else {
    set old_filename [Database::get_filename]
    Database::set_filename $filename 
    if { [catch {source $filename} err] } {
      global errorInfo 
      Debug::print "Error during database opening: $err"
      Debug::print "errorInfo is: $errorInfo"
      Database::set_filename $old_filename
      toSessionManager \
        "[get_messageTemplate db_failure] Database is corrupted."
    } else {
      Database::set_filename $filename
      if { ! [file exists [Database::get_secondary_filename] ] } {
        tk_dialog [uniqueDialogName] "Warning!" "The secondary database file does not exist. You could encounter problems in the future." question 0 OK
      }
      toSessionManager "[get_messageTemplate db_success] $filename"
    }
  }
}

##
##   save_database_
##

proc save_database_ {} {
  set filename [Database::get_filename]
  set old_secondary_filename [Database::get_secondary_filename]
  if { [catch {Database::save $filename $old_secondary_filename} err] } { 
    global errorInfo
    Debug::print "Error during saving: $err"
    Debug::print "ErrorInfo: $errorInfo"
    toSessionManager "[get_messageTemplate db_failure] File is not writable."
  } else {
    toSessionManager "[get_messageTemplate db_success] $filename"
  }
}

##
##   save_database_as_
##

proc save_database_as_ {} {
  set w [uniqueDialogName]
  set filename [savefile $w "Save database as..."]

  Debug::print "filename in save_database_as_ is '$filename'"

  set old_filename [Database::get_filename]
  set old_secondary_filename [Database::get_secondary_filename]
  if {$filename == ""} {
    toSessionManager [get_messageTemplate db_cancel]
  } else { 
    Database::set_filename $filename
    if { ! [catch {Database::save $filename $old_secondary_filename} ssss] } {
      #Database::set_filename $filename
      toSessionManager "[get_messageTemplate db_success] $filename"
    } else {
      Database::set_filename $old_filename
      Debug::print $ssss
      toSessionManager [get_messageTemplate db_failure]
    }
  }  
}

##
##   close_database_
##

proc close_database_ {} {
  Database::close
}

proc save_on_close_ {} {
  set response [tk_dialog [uniqueDialogName] "Save the database?" "There are unsaved data in the database. Do you want to save it?" question 0 Yes No Cancel]
  if {$response == 0} {
    toSessionManager "[get_messageTemplate db_yes]"
  } elseif {$response == 1} { 
    toSessionManager "[get_messageTemplate db_no]"
  } else {
    toSessionManager "[get_messageTemplate db_cancel]"
  }
}


##
##   getDefnFromDatabase
##

set getDefnObjects(index) "" 

proc get_defn_update_hook {dir} {
  Debug::enter
  variable getDefnObjects
  set category [Database::get_directory_category $dir]
  if {$category == "TMP"} {
    return [Database::Browser::std_list_hook $dir]
  }
  Debug::leave $getDefnObjects($category)
  return $getDefnObjects($category)
}

proc getDefnFromDatabase {category name defn} {

  #namespace import Database::Browser::browse

  global getDefnObjects

  if {[Database::is_closed]} {
    tk_dialog [uniqueDialogName] "Warning!" \
  "Database is closed. Try to create new or to open existing database before."\
      question 0 OK
    return
  }  

  set w [uniqueDialogName]

  set getDefnObjects($category) [list]
  set subcategories [list]
  Database::get_all_objects_in_category $category \
    getDefnObjects($category) subcategories

  set tmp_dir [Database::new_directory [Database::root_dir] TMP]
  set tmp_cath_dir [Database::new_directory $tmp_dir $category]

  foreach subcath $subcategories {
    set getDefnObjects($subcath) [list]
    Database::get_all_objects_in_category $subcath getDefnObjects($subcath) ss
    set tmp_subcath_dir [Database::new_directory $tmp_cath_dir $subcath]
    lappend getDefnObjects($category) $tmp_subcath_dir
    Debug::print "$subcath subcath objects: $getDefnObjects($subcath)"    
  }

  Debug::print "$category cath objects: $getDefnObjects($category)"
  
  set response [Database::Browser::browse $w \
    -title "Select object's definition..." \
    -root-dir $tmp_cath_dir -selection single -edit no \
    -browse-mode classified -update-hook get_defn_update_hook \
    -buttons {ok std_ok cancel std_cancel} -properties nonselectable \
    -search enable ]

  unset getDefnObjects
  Database::delete_directory $tmp_dir -recursive

  if {$response == 0} {
    set object [Database::Browser::get_selected]
    if {[llength $object]==1 && [winfo exists $name] && [winfo exists $defn]} {
      $name delete 0 end
      $name insert 0 [Database::get_object_name $object]
      $defn delete 1.0 end
      $defn insert end [Database::get_object_definition $object]
    }
  }
}

##
##   db_get_experiment_
##

proc encodeSpacesAndCBraces {str} {
  regsub -all "a" $str  "aa" str1
  regsub -all " " $str1 "ab" str2 
  regsub -all "\{" $str2 "ac" str3 
  regsub -all "\}" $str3 "ad" result 
  return $result
}

proc decodeSpacesAndCBraces {str} {
  regsub -all "ad" $str  "\}" str1
  regsub -all "ac" $str1 "\{" str2
  regsub -all "ab" $str2 " " str3
  regsub -all "aa" $str3 "a" result
  return "\{$result\}"
}

proc db_get_experiment_ {} {
  Debug::enter
  #namespace import Database::Browser::browse

  global db db_directory db_objects

  set w   [uniqueDialogName]
  set dir [Database::get_dir_id EXPERIMENTS]

  set response [Database::Browser::browse $w \
    -title "Load an experiment..." \
    -selection directory -edit no -root-dir $dir -browse-mode classified \
    -update-hook Database::Browser::std_list_hook \
    -buttons {ok std_ok cancel std_cancel} \
    -properties nonselectable \
    -search enable ]

  set selected [Database::Browser::get_selected]
  if {$response == 0 && [llength $selected] > 0} {
    set selection [lindex $selected 0]
    while { [Database::get_directory_parent $selection] != $dir } {
      Debug::print "selection $selection"
      set selection [Database::get_directory_parent $selection]
    }

    set pardir [Database::get_directory_parent $selection]
    if { "[Database::get_directory_category $selection]" == "TMP" } {
      tk_dialog [uniqueDialogName] "Information" "You cannot load objects from the search results. Please choose just one of the experiments." "" 1 OK
      toSessionManager "[get_messageTemplate db_cancel]"
      return
    }
 
    set objects [Database::all_objects_from_dir $selection]

    Debug::print "unsorted list of objects: $objects"

    set objects [Database::sort_by_dependencies $objects]  

    Debug::print "sorted list of objects: $objects"

    foreach object $objects {
      set objectType [Database::qualify_object_category $object]
      set name [encodeSpacesAndCBraces [Database::get_directory_name $object]]
      set dependencies [list]
      foreach parent [Database::get_object_dependencies $object] {
        lappend dependencies [lsearch $objects $parent]
      }
      regsub -all "\n" [Database::get_object_definition $object] " " definition
      set def "\{$name\} $objectType $dependencies $definition"
      set dproperty [Database::get_object_selected_properties $object]
      Debug::print "selected properties are: '$dproperty'"
      toSessionManager "[get_messageTemplate db_object_definition] 1 $def"
      toSessionManager "[get_messageTemplate db_object_definition] 2 \{ [Database::get_object_properties $object] \}"
    }
    toSessionManager "[get_messageTemplate db_object_definition] 0"
    toSessionManager "[get_messageTemplate db_success]"
    return [Debug::leave]
  }
  toSessionManager "[get_messageTemplate db_cancel]"
  Debug::leave
}

##
##   db_add_experiment_ 
##

proc request_for_add_experiment {} { 
  global workspace_selection
  set savedTypes {SMSubgroup SMMap SMWord SMEquation SMEquation2 SMHomomorphism SMHomomorphism2 SMSetOfWords SMVectorOfWords}
  set groups [list]
  set others [list]
  foreach icon $workspace_selection {
    set type [get_objectType $icon] 
    if {$type == "SMFPGroup" && [lsearch $groups $icon] == -1} {
      lappend groups $icon
    } elseif { [lsearch $savedTypes $type] != -1 } {
      lappend others $icon
      set parents [get_objectDependencies $icon]
      foreach parent $parents {
        if {[lsearch $groups $parent] == -1} {
          lappend groups $parent
        } 
      }
    }
  }

  foreach other $others {
    lappend groups $other
  }

  if {[llength $groups] == 0} {
    tk_dialog [uniqueDialogName] "Information" \
      "There are no selected object that can be added to the database as a new experiment." \
      "" 1 OK
    return
  }
  set oids [list] 
  foreach icon $groups {
    lappend oids [get_objectID $icon]
  }
  toSessionManager \
    "[get_messageTemplate db_select_objects] [llength $oids] $oids"
  toSessionManager [get_messageTemplate db_add_exp]
}

set workspace_selection_details ""

proc db_select_objects_details_ {details} {
  global workspace_selection_details
  set workspace_selection_details $details
}

set returnval 0

proc decodeSpaces {str} {
  
  return str
}
proc db_add_experiment_ {} {
  global  workspace_selection_details returnval

  # Make `experiment's name' dialog

  set w [uniqueDialogName]
  toplevel $w -class Dialog
  wm title $w "Experiment's name"
  wm iconname $w Dialog

  frame $w.frame
  label $w.label -text "Type an experiment's name"
  entry $w.entry -state normal
  frame $w.frame2
  button $w.ok -text "OK" \
    -command "set returnval \[$w.entry get\]" \
    -relief raised -bd 2 -width 8
  button $w.cancel -text "Cancel" \
    -command "set returnval {}" -relief raised \
    -bd 2 -width 8

  wm protocol $w WM_DELETE_WINDOW {
    global returnval
    set returnval ""
    ::Debug::print "WM_DELETE_WINDOW: returnval = $returnval"
  }

  bind $w.entry <Return> "set returnval \[$w.entry get\]"
  bind $w.entry  <Tab> "focus $w.ok"
  bind $w.ok     <Tab> "focus $w.cancel"
  bind $w.cancel <Tab> "focus $w.entry"

  pack $w.ok     -side left -in $w.frame2 
  pack $w.cancel -side left -in $w.frame2
  pack $w.label  -side top -in $w.frame -fill x
  pack $w.entry  -side top -in $w.frame -fill x
  pack $w.frame2 -side top -in $w.frame -fill y
  pack $w.frame

  focus $w.entry

  set exp [Database::get_dir_id EXPERIMENTS]

  while {1} {
    set name [Database::Utility::wait_for_dialog $w returnval]
    Debug::print "Exp name: '$name', length = [string length $name]"
    unset returnval
    set name [Database::Utility::trim_cbraces $name]
    Debug::print "Exp name: '$name', length = [string length $name]"
    
    if {[string length $name] == 0} {
      toSessionManager [get_messageTemplate db_cancel]
      destroy $w
      return
    }
    Debug::print "Non-empty name for experiment"

    set uniqueName 1
    foreach Entry [Database::get_directory_children $exp] {
      if {[Database::get_directory_name $Entry] == $name} {
        set uniqueName 0
        break
      }
    }
    if { $uniqueName } {
      break
    }
    tk_dialog [uniqueDialogName] "Information" "There is an experiment with the name. Please choose another name for your new experiment" "" 1 OK
  }
  destroy $w

  set dir [Database::new_directory $exp EXPERIMENT $name]

  # entries(icon) entry

  set objects []
  foreach object $workspace_selection_details {
    set oid         [lindex $object 0]
    set categories [lindex $object 1]
    set properties  [lindex $object 2]	

    Debug::print "oid = $oid ; cath = $categories "
    set icon [get_objectIcon $oid]
    set name [get_objectName $icon]
    set definition [get_objectDefinition $icon]
    set dependencies [list]
    foreach parent [get_objectDependencies $icon] {
      if {[info exists entry($parent)]} {
        lappend dependencies $entry($parent)
      }
    }
    set new_obj \
      [Database::add_object $dir $name $definition $dependencies $categories]
    # Database::set_object_description $new_obj $properties
    Database::set_object_properties  $new_obj $properties
    Debug::print "properties is [Database::get_object_properties $new_obj]"
    lappend entry($icon) $new_obj
    lappend objects $new_obj 
  }

  #@dp debug: Database::save  xxx

  set commit \
    [Database::Browser::browse $w -title "Edit properties of added objects..."\
       -selection none -edit yes -root-dir $dir -browse-mode unclassified \
       -update-hook Database::Browser::std_list_hook \
       -buttons {ok std_ok cancel std_cancel} -properties selectable]

  Debug::print "commit $commit"

  if {$commit == 2} {
    Database::delete_directory $dir -recursive
    toSessionManager [get_messageTemplate db_cancel]
  } else {
    foreach object $objects {
      foreach property [Database::get_object_unselected_properties $object] {
        Database::remove_object_property $object $property
      } 
    }
    toSessionManager [get_messageTemplate db_success]
  }
}

proc std_ok {w} {}
proc std_cancel {w} {}

proc db_set_category_ {category index name} {
  Database::set_category $category $index $name
}

proc convert_database2html {} {
  if { [Database::is_closed] } {
    tk_dialog [uniqueDialogName] "Information" "A database is currently closed. Please open one database and try again." "" 1 OK    
    return
  }

  set w [uniqueDialogName]

  global seltype
  set seltype dir
  set save_dir [savefile $w "Choosing a directory for html-files..."]  
  set seltype file

  if {$save_dir != ""} {
    HtmlConverter::exportDatabaseTo $save_dir
  }
}
