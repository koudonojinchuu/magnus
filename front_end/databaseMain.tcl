
#------------------------------------------------------------
#
#     Namespace:   Database 
#
#------------------------------------------------------------

namespace eval Database {

  namespace export                                \
    is_closed                                     \
    create save close                             \
    get_filename set_filename                     \
    get_sconadary_filename                        \
                                                  \
    get_directory_name                            \
    get_directory_type                            \
    get_directory_parent                          \
    get_directory_category                        \
    get_directory_children                        \
                                                  \
    get_object_name                               \
    get_object_definition                         \
    get_object_keywords                           \
    get_object_dependencies                       \
    get_object_dependants                         \
    get_object_description                        \
    get_object_properties                         \
                                                  \
    set_object_name                               \
    set_object_keywords                           \
    set_object_description                        \
    set_object_properties                         \
                                                  \
    exists_object                                 \
    exists_directory                              \
    root_dir                                      \
    new_entry new_object new_directory            \
    add_object                                    \
    delete_directory                              \
    get_all_objects_in_category                   \
    get_dir_id all_objects_from_dir               \
    sort_by_dependencies                          \
    qualify_object_category                       \
    get_dir_id                                    \
    set_category                                  \
    init                                          \
    is_valid_attachment                           \
    get_attachment_filename                       \
    get_attachment_startpos                       \
    get_attachment_length                         \
    get_attachment_counter                        \
    get_attachment_checksum                       \


  #------------------------------------------------------------
  #
  #  Variables
  #
  #------------------------------------------------------------


  #
  # Variable: filename
  #

  set filename ""
  set secondary_filename ""

  proc set_filename {name} {
    variable filename
    variable secondary_filename
    set filename $name
    regsub "\.db$" $name "" secondary_filename
    set secondary_filename "$secondary_filename.db2"
  } 

  proc get_filename {} {
    variable filename
    return $filename
  }

  proc get_secondary_filename {} {
    variable secondary_filename
    return $secondary_filename
  }

  #
  # Variable: db_objects 
  #   db_objects is array of elements
  #   db_objects($id,name)
  #   db_objects($id,category)
  #   db_objects($id,definition)
  #   db_objects($id,keywords)
  #   db_objects($id,description)
  #   db_objects($id,properties)
  #   db_objects($id,selected_properties)
  #   db_objects($id,dependencies)
  #   db_objects($id,dependants)
  #   db_objects($id,attachments)

  proc exists_object {object} {
    variable db_objects
    if { ![catch {set db_objects($object,name)}] } {
      return 1
    } 
    return 0
  }

  foreach op {name category definition keywords dependencies dependants \
              description properties unselected_properties} { 

    eval "proc get_object_$op \{id\} \{ 
            variable db_objects
            return \$db_objects(\$id,$op) 
          \}"
    eval "proc set_object_$op \{id value\} \{ 
            variable db_objects
            set db_objects(\$id,$op) \$value
          \}"
  }

  proc get_object_property {id property} {
    variable db_objects  
    set haveProperties [lindex $db_objects($id,properties) 0]
    set idx [lsearch $haveProperties $property]
    if {$idx != -1} {
      incr idx 1
      return [lindex $haveProperties $idx]
    }
    set haveNotProperties [lindex $db_objects($id,properties) 1]
    set idx [lsearch $haveNotProperties $property]
    if {$idx != -1} {
      incr idx 1
      return [lindex $haveNotProperties $idx]
    }
    return "unknown property"
  }

  proc add_object_property {id type name property} {
    variable db_objects  
    if {$type == "have"} {
      set haveProperties [lindex $db_objects($id,properties) 0]
      lappend haveProperties $name
      lappend haveProperties $property
      set db_objects($id,properties) \
        [lreplace $db_objects($id,properties) 0 0 $haveProperties]
    } elseif {$type == "have not"} {
      set haveNoProperties [lindex $db_objects($id,properties) 1]
      lappend haveNotProperties $name
      lappend haveNotProperties $property
      set db_objects($id,properties) \
        [lreplace $db_objects($id,properties) 1 1 $haveNotProperties]
    }
  }

  proc remove_object_property {id property {type ""}} {
    variable db_objects  

    if {$type == ""} {
      set type [get_object_property_type $id $property] 
    }

    set_object_select_property $id $property

    if {$type == "have"} {
      set haveProperties [lindex $db_objects($id,properties) 0]
      set idx [lsearch $haveProperties $property]
      if {$idx != -1} {
        set haveProperties [lreplace $haveProperties $idx [expr $idx+1]]
        set db_objects($id,properties) \
          [lreplace $db_objects($id,properties) 0 0 $haveProperties]
      } 
    } elseif {$type == "have not"} {
      set haveNotProperties [lindex $db_objects($id,properties) 1]
      set idx [lsearch $haveNotProperties $property]
      if {$idx != -1} {
        set haveNotProperties [lreplace $haveNotProperties $idx [expr $idx+1]]
        set db_objects($id,properties) \
          [lreplace $db_objects($id,properties) 1 1 $haveNotProperties]
      } 
    }
  }

  proc get_object_property_type {id property} {
    variable db_objects  
    set haveProperties [lindex $db_objects($id,properties) 0]
    set idx [lsearch $haveProperties $property]
    if {$idx != -1} {
      incr idx 1
      return "have"
    }
    set haveNotProperties [lindex $db_objects($id,properties) 1]
    set idx [lsearch $haveNotProperties $property]
    if {$idx != -1} {
      return "have not"
    }
    return "dontknow"
  } 
#
  proc get_object_properties_names {id} {
    variable db_objects  

    set properties_names []

    set haveProperties [lindex $db_objects($id,properties) 0]
    set len [llength $haveProperties]
    for {set i 0} {$i < $len} {incr i 2} {
      lappend properties_names [lindex $haveProperties $i]
    }

    set haveNotProperties [lindex $db_objects($id,properties) 1]
    set len [llength $haveNotProperties]
    for {set i 0} {$i < $len} {incr i 2} {
      lappend properties_names [lindex $haveNotProperties $i]
    }
    return $properties_names
  }

  proc get_object_selected_properties {id} {
    variable db_objects  

    set selected_properties []
    foreach property [get_object_properties_names $id] {
      if { [lsearch $db_objects($id,unselected_properties) $property] == -1} {
        lappend selected_properties $property
      }
    }
    return $selected_properties
  }

  proc set_object_unselect_property {id property} {
    variable db_objects  
    if { [lsearch $db_objects($id,unselected_properties) $property] == -1 } {
      lappend db_objects($id,unselected_properties) $property
    }
  }

  proc set_object_select_property {id property} {
    variable db_objects  
    set idx [lsearch $db_objects($id,unselected_properties) $property]
    if { $idx != -1 } {
      set db_objects($id,unselected_properties) \
        [lreplace $db_objects($id,unselected_properties) $idx $idx]
    }
  }

  proc add_object_dependants {id dependants} {
    variable db_objects
    set new_list $db_objects($id,dependants)
    foreach dep $dependants {
      if { [lsearch $new_list $dep] == -1} {
        lappend new_list $dep
      }
    }
    set db_objects($id,dependants) $new_list
  }

  proc delete_object_dependants {id dependants} {
    variable db_objects
    set new_list $db_objects($id,dependants)
    foreach dep $dependants {
      set idx [lsearch $new_list $dep]
      if { $idx != -1 } {
	set new_list [lreplace $new_list $idx $idx]
      }
    }
    set db_objects($id,dependants) $new_list
  }

  proc delete_object {object} {
    variable db_objects
    Database::Utility::unset_indices_by_pattern db_objects "$object,"
  }


  proc get_object_attachments {id} {
    variable db_objects
    return db_objects($id,attachments)
  }

  proc set_object_attachments {id attachments} {
    variable db_objects
    Debug::writelog "$id override attachments: $db_objects($id,attachments)"
    set db_objects($id,attachments) $attachments
  }

  proc add_object_attachments {id attachments} {
    variable db_objects
    set full_list $db_objects($id,attachments)
    foreach attach $attachments {
      if { [lindex $full_list $attach] == -1 } {
	lappend full_list $attach
        bind_attachment $attach
      }
    }
    set db_objects($id,attachments) $full_list
  }

  proc delete_object_attachments {id attachments} {
    variable db_objects
    set full_list $db_objects($id,attachments)
    foreach attach $attachments {
      set pos [lindex $full_list $attach]
      if { $pos != -1 } {
	set full_list [lreplace $full_list $pos $pos]
        unbind_attachment $attach
      }
    }
    set db_objects($id,attachments) $full_list
  }

  #
  # Variable: db_attachments 
  #
  # db_attachments(index)          -- list of IDs of all attachments
  # db_attachments($id,filename)  
  # db_attachments($id,length)
  # db_attachments($id,startpos)
  # db_attachments($id,counter)
  # db_attachments($id,checksum)

  set db_attachments(index) [list]

  proc init_attachments {unique_id} {
    variable db_attachments
    catch {unset db_attachments}
    set db_attachments(index) [list]
    set_unique_attachment_id $unique_id  
  }

  proc append_attachments_index {id} {
    variable db_attachments
    if { [is_valid_attachment $id] } {
      lappend db_attachments(index) $id
    } else {
      Debug::print "Database::append_attachments_index: attachment $id is illegal."
    }
  }

  foreach op {filename length startpos counter checksum} { 

    eval "proc get_attachment_$op \{id\} \{ 
            variable db_attachments
            return \$db_attachments(\$id,$op) 
          \}"
    eval "proc set_attachment_$op \{id value\} \{ 
            variable db_attachments
            set db_attachments(\$id,$op) \$value
          \}"
  }

  proc get_all_attachments {} {
    variable db_attachments
    return $db_attachments(index)
  }

  proc total_attachments {} {
    variable db_attachments
    return [llength $db_attachments(index)]
  }

  set attach_count 0

  proc unique_attachment_id {} {
    variable attach_count
    incr attach_count
    return $attach_count
  }

  proc reset_unique_attachment_id {} {
    variable attach_count
    set attach_count 0
  }

  proc set_unique_attachment_id {start} {
    variable attach_count
    set attach_count $start
  }

  proc bind_attachment {fullname} {
    Debug::enter
    variable db_attachments
    set length [file size $fullname]
    set filename [file tail $fullname]
    set checksum [Attachments::compute_file_checksum $fullname]
    set file_fd [::open $fullname r]
    fconfigure $file_fd -translation binary

    if { [file readable [get_secondary_filename] ] } {
      set db_fd [::open [get_secondary_filename] r]
      fconfigure $db_fd -translation binary
      foreach similar [Attachments::find_similar_attachments $filename $length $checksum] {
        # similar attachments have the same: file name, length and checksum.
        set thesame [Attahments::compare_attachment_and_file $file_fd \
		       $db_fd [get_attachment_startpos $similar] $length]
        if { $thesame == 0 } {
          # new file is already presented in the current database.
	  # just increment counter of this attachment.
          incr db_attachments($similar,count)
          ::close $file_id
          ::close $db_fd
          Debug::print "Found similar attachment for '$fullname': ID=$similar"
          return $similar
        }
      }
      ::close $db_fd
    }
    ::close $file_fd

    # check whether the secondary database file is writable.
    set db_fd 0
    if { [catch {set db_fd [::open [get_secondary_filename] a+] } ] } {
      #@dp: report error
      Debug::print "Cannot open secondary database file '[get_secondary_filename]'"
      Debug::print "Return to the caller the bad result: attachID=0"
      return 0
    }
    ::close $db_fd

    # update attachments table 
    set id [Database::unique_attachment_id]
    Debug::print "Attachemnt ID=$id for file $fullname, length=$length, checksum=$checksum"
    set_attachment_filename $id $filename 
    set_attachment_startpos $id [file size [get_secondary_filename]]
    set_attachment_length   $id $length 
    set_attachment_checksum $id $checksum 
    set_attachment_counter  $id 1         ; # 1 or 0 ???? 

    lappend db_attachments(index) $id
    
    Debug::examine_array db_attachments
    # write the atachment into the secondary database file.
    part_copy_file [get_secondary_filename] $fullname 0 $length

    return $id
  }

  proc unregister_attachment {id} {
    variable db_attachments
    Database::Utility::unset_indices_by_pattern db_attachments "$id,"
  }

  proc unbind_attachment {id} {
    variable db_attachments
    Debug::assert {$db_attachments($id,counter) > 0}
    decr db_attachments($id,counter)
    if { $db_attachments($id,counter) == 0 } {
      unregister_attachment $id
      set pos [lsearch $db_attachments(index) $id]
      set db_attachments(index) [lreplace $db_attachments(index) $pos $pos]
    }
  }

  proc is_valid_attachment {id} {
    Debug::enter
    variable db_attachments
    set startpos 0
    if { [catch {set startpos $db_attachments($id,startpos)} ] } {
      Debug::print "Cannot find attchment with ID=$id"
      return 0
    }
    # Attachment has valid 'id'.
    set length $db_attachments($id,length)
    set db2file [get_secondary_filename]
    if { [file exists $db2file] && [file readable $db2file] 
         && [file size $db2file] >= [expr $startpos+$length] } {
      Debug::print "Secondary database file has the attachment with ID=$id"
      return 1
    }
    Debug::print "Secondary database file doesn't contain the attachment with ID=$is"
    return 0
  }

  proc fix_attachment_table {} {
    variable db_attachments
  }

  #
  # Variable: db_directory
  #
  #     KEY                          DATUM
  #  db_directory($entry,type)      dir/object
  #  db_directory($entry,name)      name of the entry
  #  db_directory($entry,category) 
  #  db_directory($entry,parent)    reference to up-level entry
  #  db_directory($entry,children)  list of children objects


  foreach op {name type category parent children} { 

    eval "proc get_directory_$op \{id\} \{ 
            variable db_directory
            return \$db_directory(\$id,$op) 
          \}"
    eval "proc set_directory_$op \{id value\} \{ 
            variable db_directory
            set db_directory(\$id,$op) \$value
          \}"
  }

  proc exists_entry {entry} {
    variable db_directory
    if { ![catch {set db_directory($entry,type)}] } {
      return 1
    }
    return 0
  }

  proc exists_directory {entry} {
    if {[exists_entry $entry] && [get_directory_type $entry] == "dir"} {
      return 1
    }
    return 0 
  }

  proc delete_entry {entry} {
    variable db_directory
    Database::Utility::unset_indices_by_pattern db_directory "$entry,"
  }

  proc add_directory_child {dir child} {
    variable db_directory
    lappend db_directory($dir,children) $child
  }

  proc remove_dir_child {parent child} {
    variable db_directory
    set idx [lsearch $db_directory($parent,children) $child]
    set db_directory($parent,children) \
      [lreplace $db_directory($parent,children) $idx $idx]
  }

  #
  # Variable: unique_id
  #

  set unique_id 0

  proc unique_object_id {} {
    variable unique_id
    incr unique_id 
    while { [exists_entry D$unique_id] || [exists_entry O$unique_id] } { 
      incr unique_id
    }
    return "O$unique_id"
  }

  proc unique_directory_id {} {
    variable unique_id
    incr unique_id 
    while { [exists_entry D$unique_id] || [exists_entry O$unique_id] } { 
      incr unique_id
    }
    return "D$unique_id"
  }

  #
  # Variable: registered_objects
  #

  proc register_object {id} {
    Debug::enter 
    variable registered_objects
    lappend  registered_objects $id
    Debug::print "all registered objects = $registered_objects "
  }

  proc register_objects {args} {
    Debug::enter
    foreach id $args {
      register_object $id
    }
  }

  proc unregister_all_objects {} {
    Debug::enter
    variable registered_objects
    set registered_objects [list]
  }

  proc all_registered_objects {} {
    Debug::enter
    variable registered_objects
    Debug::print "all registered objects = $registered_objects "
    return $registered_objects
  }

  #
  # Variable: unique_oid 
  #
  
  set unique_oid 0

  #
  # Varibale: categories
  # Type: array
  #

  proc set_category {category index name} {
    variable categories 
    variable cath_index
    set cath_index($category) $index	
    set categories($category) $name
  }

  proc get_category_name {category} {
    variable categories
    if {[info exists categories($category)]} {
      return $categories($category)
    }

    #unknown category
    return ""
  }

  proc get_category_by_name {name} {
    Debug::enter 
    variable categories
    Debug::print "[array names categories]"
    foreach category [array names categories] {
      Debug::print "checking:  $categories($category) == $name ?"
      if {$categories($category) == $name} {
        Debug::leave $category
	return $category
      }
    }

    # unknown category
    Debug::leave ""
    return "" 
  }

  proc get_category_index {category} {
    variable cath_index
    return $cath_index($category)
  }

  #
  # Variable: unique_cath_index
  #

  set unique_cath_index 0

  proc unique_category_index {} {
    variable unique_cath_index
    incr unique_cath_index -1
    return $unique_cath_index
  }


  #------------------------------------------------------------
  #
  #  Work on entire database
  #
  #------------------------------------------------------------

  #
  #    is_closed
  #

  proc is_closed {} {
    if {[get_filename] == ""} { return 1 }
    return 0
  }

  #
  #    create
  #

  proc create {} {
    init
 
    db_set_category_ \
      EXPERIMENTS [unique_category_index] {Experiments} 
    db_set_category_ \
      INDEPENDENT [unique_category_index] {Independent objects}
    db_set_category_ \
      TMP         [unique_category_index] {Objects from all experiments}

    new_directory ROOT EXPERIMENTS
    new_directory ROOT INDEPENDENT
  }

  #
  # Save the entire database into the given file in tcl-like format.
  #

  proc save {filename old_secondary_filename} {
    Debug::enter 

    set          fileId [open $filename w]

    save_header      $fileId
    save_toc         $fileId
    save_objects     $fileId $old_secondary_filename
    save_attachments $fileId $old_secondary_filename
    puts             $fileId "\}"

    ::close          $fileId
    Debug::leave 1
    return 1
  }

  #
  #    close
  #

  proc close {} {
    set_filename ""
    init
  }


  #----------------------------------------------------------------------
  #
  # Operations with open database: accessors/modifiers
  #
  #----------------------------------------------------------------------

  proc init {} {
    variable db_directory 
    variable db_objects

    # Initialize db_directory.
    catch { unset db_directory }
    set root [root_dir]
    set_directory_name     $root {Magnus database}
    set_directory_type     $root dir
    set_directory_parent   $root $root
    set_directory_children $root [list]

    # Initialize db_objects.
    catch { unset db_objects }
    unregister_all_objects 

    init_attachments 0
  }

  proc root_dir {} {
    return ROOT
  }

  proc new_entry {parent entry type name category} {
    Debug::enter
    set_directory_type      $entry $type
    set_directory_name      $entry $name
    set_directory_category $entry $category
    set_directory_parent    $entry $parent
    set_directory_children  $entry [list]
    add_directory_child     $parent $entry 
    Debug::leave $entry   
    return $entry
  }

  proc new_tmp_entry {parent entry type name category} {
    Debug::enter
    set_directory_type      $entry $type
    set_directory_name      $entry $name
    set_directory_category $entry $category
    #set_directory_parent    $entry $parent
    set_directory_children  $entry [list]
    add_directory_child     $parent $entry 
    Debug::leave $entry   
    return $entry
  }

  proc new_object {id name definition keywords dependencies dependants description} {
    Debug::enter
    register_object         $id
    set_object_name         $id $name
    set_object_definition   $id $definition
    set_object_keywords     $id $keywords
    set_object_dependencies $id $dependencies
    set_object_dependants   $id $dependants
    set_object_description  $id $description
    set_object_properties   $id {}
    set_object_unselected_properties $id {}   

    foreach parent $dependencies {
      add_object_dependants $parent $id
    }

    Debug::leave $id
  }

  proc new_directory {parent category {name ""}} {
    Debug::enter

    if { ! [exists_entry $parent] } { return "" }

    set entry [unique_directory_id]
    if {$name == ""} {
      set name [get_category_name $category] 
      if {$name == ""} { set name $category } 
    }
    set result [new_entry $parent $entry dir $name $category] 
    Debug::leave $result
    return $result
  }

  proc add_object {dir name definition dependencies categoriesList} {
    Debug::enter

    if { ! [exists_directory $dir] } { return 0 }

    if {[llength $categoriesList] == 0} { 
      # add object in the directory 
      set entry [unique_object_id]
      new_entry $dir $entry object $name "ONLY DIRECTORY MAY HAVE A CATEGORY"
      new_object $entry $name $definition {} $dependencies {} {} 
      Debug::leave $entry
      return $entry
    }

    set category [lindex $categoriesList 0]
    set categoriesList [lreplace $categoriesList 0 0]

    foreach entry [get_directory_children $dir] {
      if {[get_directory_category $entry] == $category} {
	return [add_object $entry $name $definition $dependencies \
                           $categoriesList]
      } 
    } 

    # no dirs found with the given category -- make new one

    set entry [new_directory $dir $category]

    set r [add_object $entry $name $definition $dependencies $categoriesList]
    Debug::print "r = $r"
    Debug::leave $r
    return $r
  }

  proc delete_directory {dir {recursive "-no-recursive"}} {
    Debug::enter
    foreach child [get_directory_children $dir] {
      if {[get_directory_type $child] == "object"} {
	delete_object $child 
	delete_entry $child
      } elseif {$recursive == "-recursive"} {
	delete_directory $child -recursive
	delete_entry $child
      }
    }
    remove_dir_child [get_directory_parent $dir] $dir
    delete_entry $dir
    Debug::leave
  }

  proc delete_directory_structure {dir {recursive "-no-recursive"}} {
    Debug::enter
    foreach child [get_directory_children $dir] {
      if {[get_directory_type $child] == "object"} {
	delete_entry $child
      } elseif {$recursive == "-recursive"} {
	delete_directory $child -recursive
	delete_entry $child
      }
    }
    remove_dir_child [get_directory_parent $dir] $dir
    delete_entry $dir
    Debug::leave
  }

  proc all_objects_from_dir {dir} {
    Debug::enter
    if { ! [exists_entry $dir] } { return [list] }
    set objects [list]
    foreach child [get_directory_children $dir] {
      if {[get_directory_type $child] == "dir"} {
        foreach object [all_objects_from_dir $child] {
          lappend objects $object
        } 
      } else {
        lappend objects $child
      }
    }
    Debug::leave $objects 
    return $objects
  }

  proc find_category_dirs {dir category} {
    Debug::enter
    set subdirs [list]
    foreach child [get_directory_children $dir] {
      if {[get_directory_type $child] == "dir"} {
	if {[get_directory_category $child] == $category} {
	  lappend subdirs $child
	} else {
	  set sublist [find_category_dirs $child $category]
	  foreach subdir $sublist {
	    lappend subdirs $subdir
	  } 
	}
      }
    } 
    Debug::leave $subdirs
    return $subdirs
  }

  #
  #    get_all_objects_in_category
  #

  proc get_all_objects_in_category {category objects subcategories} {
    upvar $objects objlist 
    upvar $subcategories subcaths
    set root_dir [get_dir_id EXPERIMENTS]
    set subdirs [find_category_dirs $root_dir $category]
    set objects
    foreach dir $subdirs { 
      foreach object [get_directory_children $dir] {
	if {[get_directory_type $object] == "object"} {
	  lappend objlist $object
	} else {
	  set subcath [get_directory_category $object]
	  if {[lsearch $subcaths $subcath] == -1 } {
	    lappend subcaths $subcath
	  }
	}
      }
    }
    return $objects
  }

  proc get_dir_id {categoriesPath {parent ""}} {
    Debug::enter
    if {$parent == ""} { set parent [root_dir] } 
    if { [llength $categoriesPath] == 0 } { return $parent }
    set category [lindex $categoriesPath 0] 
    if { ![exists_directory $parent] } { return "" }
    foreach child [get_directory_children $parent] {
      if {[get_directory_category $child] == $category} {
        return [get_dir_id [lreplace $categoriesPath 0 0] $child ]
      }
    }
    Debug::leave ""
    return "" 
  }

  proc qualify_object_category {object} {
    Debug::enter 
    set parent $object
    set result ""
    set root [root_dir]
    while {$parent != $root} {
      set parent [get_directory_parent $parent]
      Debug::print "parent = $parent"
      set category [get_category_by_name [get_directory_name $parent]]
      if {$category != ""} { 
	set result [get_category_index $category]
	break
      }
    }

    Debug::leave $result
    return $result
  }

  proc sort_by_dependencies {objects} {
    Debug::enter
    set result [list]
    while {[llength $objects] > 0} {
      set object [lindex $objects 0]

      Debug::print "result: $result"
      Debug::print "objects: $objects"
      Debug::print "sorting element: $object"

      set objects [lreplace $objects 0 0]
      set parentSort "done"
      foreach parent [get_object_dependencies $object] {
	if {[lsearch $result $parent] == -1} {
	  if {[lsearch $objects $parent] == -1} { 
	    set parentSort "no parent at all"
	  } else {
	    set parentSort "not yet"
	  }
	  break
	}
      }
      Debug::print "parent search is: $parentSort"
      if {$parentSort == "done"} {
	lappend result $object
      } elseif {$parentSort == "not yet"} { 
	lappend objects $object
      }
    }
    Debug::leave
    return $result
  }


  proc save_header {fileId} {

    puts $fileId "#!/usr/bin/wish
#
# This is automatically generated file by MAGNUS v2.x.x
#

magnusdb \{

  header \{
    version 1.00.2
    modified \{
      date \"[theTime]\"
      author \"[exec whoami]\"
    \}
  \}
"

  }

  proc save_toc {fileId} {
    set offset 4
    puts $fileId "  table_of_contents \{"
    set root [root_dir]
    foreach dir [get_directory_children $root] {
      save_entry $fileId $dir $offset
    }
    puts $fileId "  \}"
  }

  proc save_entry {fileId entry offset} {
    Debug::enter
    puts -nonewline $fileId "[Utility::make_spaces $offset]"
    set name [get_directory_name $entry]
    if {[get_directory_type $entry] == "object"} { 
      puts $fileId "entry \{$entry\} \{$name\}"
    } else {
      set category [get_directory_category $entry]
      puts $fileId "directory \{$entry\} \{$name\} \{$category\} \{"
      incr offset 2
      foreach child [get_directory_children $entry] {
	save_entry $fileId $child $offset
      }
      incr offset -2
      puts $fileId "[Utility::make_spaces $offset]\}"
    }
    Debug::leave
  }

  proc save_objects {fileId old_secondary_filename} {
    Debug::enter
    foreach object [all_registered_objects] {
      save_object $fileId $object $old_secondary_filename
    }
    Debug::leave
  }

  proc save_object {fileId object old_secondary_filename} {
    Debug::enter
    puts $fileId ""
    puts -nonewline $fileId [Utility::make_spaces 2]

    puts $fileId "object \{$object\} \{
    name \{[get_object_name $object]\}
    definition \{[get_object_definition $object]\}
    keywords \{[get_object_keywords $object]\}"

    set deps [get_object_dependencies $object]
    if {[llength $deps] > 0} {
      puts $fileId "    dependencies \{$deps\}"
    }
    puts $fileId "    description \{[get_object_description $object]\}"
    save_object_properties $fileId $object $old_secondary_filename
    puts $fileId "  \}"
    Debug::leave
  }

  proc part_copy_file {dst src start len} {
    Debug::enter
    set bufsize 2048
    set pos $start
    set stop [expr $start+$len] 
    set dstId [::open $dst a]
    fconfigure $dstId -translation binary
    set srcId [::open $src]
    fconfigure $srcId -translation binary

    #Debug::print "copying $src\[$start,$len\] into $dst"

    seek $srcId $start
    while {1} {
      #Debug::print "iteration: pos=$pos"
      if {[expr $pos+$bufsize] >= $stop} {
        set bufsize [expr $stop-$pos]
      }
      set buf [read $srcId $bufsize]
      # Debug::print "\[$pos,[expr $pos+$bufsize]\]: $buf"
      puts -nonewline $dstId $buf
      #Debug::print "written."
      incr pos $bufsize 
      if {$pos >= $stop} { 
        puts $dstId ""
        break
      }
    }
    ::close $srcId 
    ::close $dstId
    #Debug::print "leave part_copy_file"
    Debug::leave
  }

  proc save_object_properties {fileId object old_secondary_filename} {
    Debug::enter

    set db2ndFileShortName \
      [Database::Utility::file_shortname [get_secondary_filename]]

    Debug::print "DatabaseSecondaryFile = [get_secondary_filename]"

    puts -nonewline $fileId "    properties \{ " 

    #[get_object_properties $object]"
    set properties [get_object_properties $object]

    Debug::print "properties is $properties"

    # save `have properties':
    set haveProperties [lindex $properties 0] 
    set pCount [llength $haveProperties]

    Debug::print "haveProperties: $haveProperties"
    Debug::print "pCount = $pCount"

    puts -nonewline $fileId "\{ " 
    for {set i 0} {$i < $pCount} {incr i 2} {
      set pname    [lindex $haveProperties $i]
      set property [lindex $haveProperties [expr $i+1]]
      set extraDetails 0
      set detailsFile ""
      set expl [lindex $property 0]
      catch {set extraDetails [lindex $property 1]}
      catch {set detailsFile  [lindex $property 2]}
      Debug::print "detailsFile dirname = [Database::Utility::file_path $detailsFile]"
      if { [Database::Utility::file_path $detailsFile] == "" } {
	set detailsFile [file join [file dirname $old_secondary_filename] $detailsFile]
      }

      Debug::print "pname = '$pname', property ='$property'"
      Debug::print "extraDetails = $extraDetails, detailsFile = $detailsFile"

      if { $extraDetails == 1 && $detailsFile != [get_secondary_filename] 
           && [file readable $detailsFile] } {

        Debug::print "moving extra properties"

        set oldStart [lindex $property 3]
        set length   [lindex $property 4]

        Debug::print "oldStart = $oldStart, length = $length" 

        catch {set fex [file exists [get_secondary_filename]] } err
        Debug::print "err = '$err'" 

        if {$fex} {
          Debug::print "secondary file exists."
          set newStart [file size [Database::get_secondary_filename]]
        } else {
          set newStart 0
        }

        Debug::print "newStart=$newStart"
	if { ![string match "\{*\}" $expl] } {
	  set expl "\{$expl\}"
	}
        part_copy_file [get_secondary_filename] $detailsFile $oldStart $length
        puts -nonewline $fileId "$pname \{ $expl $extraDetails $db2ndFileShortName $newStart $length \} "

      } else {

        # Save property unchanged.
        Debug::print "unchanged property:'$pname \{$property\}'"
        puts -nonewline $fileId "$pname \{$property\} "

      }         
    }
    puts -nonewline $fileId "\} \{"
 
    # Save `have not properties':
    set haveNotProperties [lindex $properties 1]
    Debug::print "haveNotProperties: '$haveNotProperties'"  
    puts -nonewline $fileId $haveNotProperties 
    puts $fileId "\} \}"

    Debug::leave
  }

  proc save_attachments {fileId old_secondary_filename} {
    puts $fileId ""
    puts $fileId "  attachments \{unique_attachment_id [unique_attachment_id]\} \{total_attachments [total_attachments]\} \{"
    set attachments [get_all_attachments]
    foreach attach $attachments {
      set startpos [Database::get_attachment_startpos $attach]
      set length   [Database::get_attachment_length   $attach]
      if { $old_secondary_filename != [get_secondary_filename] } {
        # copy attachment into new secondary database file.
        set oldStart $startpos
        catch {set fex [file exists [get_secondary_filename]] } err
        if {$fex} {
          Debug::print "secondary file exists."
          set startpos [file size [get_secondary_filename]]
        } else {
          set startpos 0
        }
        part_copy_file [get_secondary_filename] $old_secondary_filename $oldStart $length
      }
      puts $fileId "    attachment id $attach filename \{[Database::get_attachment_filename $attach]\} length $length startpos $startpos checksum [Database::get_attachment_checksum $attach] counter [Database::get_attachment_counter $attach]"
    }
    puts $fileId "  \}"

  }

  proc do_search {text where} {
    set found [list] 
    set wkey  [lindex $where 0]
    set wdesc [lindex $where 1]
    set wprop [lindex $where 2]

    foreach object [all_registered_objects] {

      if {$wkey} {
        set keywords [get_object_keywords $object]
        if {[string first $text $keywords] != -1} {
          lappend found $object
          continue
        }
      } 

      if {$wdesc} {
        set description [get_object_description $object]
        if {[string first $text $description] != -1} {
          lappend found $object
          continue
        }
      }

      if {$wprop} {
        set properties_names [get_object_properties_names $object]
        if {[string first $text $properties_names] != -1} {
          lappend found $object
          continue
        }
      }
    }
    return $found
  }

} ;# end of namespace: Database 



namespace eval Database::Attachments {

  namespace export find_similar_attachments 
  namespace export compute_file_checksum
  namespace export compare_attachment_and_file
    
  proc find_similar_attachments {filename length checksum} {
    variable db_attachments
    set similars [list]
    foreach name [array name db_attachments] {
      regexp "\[0-9\]" $name id
      if { [string match "*,filename" name] && 
           $db_attachments($name) == $filename &&
           $db_attachments($id,length) == $length &&
           $db_attachments($id,checksum) == $checksum } {
        lappend similars $id
      }
    }
    Debug::print "file $filename ($length,$checksum), similars: $similars"
    return $similars
  }

  proc compute_file_checksum {filename} {
    if { ![file readable $filename] } {
      return 0
    }

    set fileId [::open $filename r]
    fconfigure $fileId -translation binary

    set length [file size $filename]
    set block_size 2048
    set checksum 0

    for {set i 0} {$i < $length} {incr i $block_size} {
      if { [expr $i+$block_size] >= $length } {
        set block_size [expr $length-$i]
      }
      set data [read $fileId $block_size]
      binary scan $data i[expr $block_size/4] num_list
      foreach number $num_list {
        incr checksum $number
      }
    }
    ::close $fileId
    return $checksum
  }

  proc compare_attachment_and_file {file_fd db_fd startpos length} {
    seek $file_fd 0
    seek $db_fd $startpos
    set block_size 2048
    for {set count 0} {$count < $length} {incr count $block_size} {
      if { $count > $length } {
        set block_size [expr $block_size-($count-$length)]
      }
      set file_str [read $file_fd $block_size]
      set att_str  [read $db_fd $block_size]
      set comp [string compare $file_str $att_str]
      if { $comp != 0 } {
        return $comp
      }
    }
    return 0
  }

} ; # namespace Database::Attachments
  

