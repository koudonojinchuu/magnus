
#----------------------------------------------------------------------
#
#     Namespace:   Database::Parser 
#
#----------------------------------------------------------------------

namespace eval Database::Parser {

  ##
  ##  Export entries: 
  ##

  namespace export init header table_of_contents directory entry object
  namespace export attachments attachment

  variable dir_stack
  variable parent_dir

  ##
  ##  Parser::init
  ##

  proc init {} {
    variable dir_stack
    variable parent_dir
    set root [::Database::root_dir]
    set dir_stack $root
    lappend dir_stack $root
    set parent_dir $root
    clear_consistency_info
  }

  proc done {} {
    variable dir_stack 
    variable parent_dir
    catch {unset dir_stack}
    catch {unset parent_dir}
    if {![exists_undefined_objects]} { 
      Debug::print "Objects are loaded successfully"
    } else { 
      puts "There are undefined objects in the database"
      set xxxxxxx 
      # it should break the normal execution
      # the exception should be handled in upper level.
    }
  }

  proc header {head} {
    #@dp to be implemented.
  }

  proc table_of_contents {toc} {
    eval $toc
  }

  proc directory {entry name category entries} {
    Debug::enter 

    variable parent_dir 
    variable dir_stack

    Debug::print "stack: $dir_stack"
    Debug::print "parent: $parent_dir"

    ::Database::new_entry $parent_dir $entry dir $name $category

    # Set a parent directory to current for all subordinate entries.
    lappend dir_stack $entry
    set parent_dir $entry

    # Evaluate and plug list of subordinate entries. Step down.
    eval $entries 

    # Back to up-level directory
    set topStackElt  [expr [llength $dir_stack]-2]
    set parent_dir   [lindex $dir_stack $topStackElt]
    set dir_stack    [lrange $dir_stack 0 $topStackElt ]

    Debug::leave
  }

  proc entry {entry name} {
    Debug::enter 
    variable  parent_dir
    ::Database::new_entry $parent_dir $entry object $name\
      "ONLY DIRECTORY MAY HAVE A CATEGORY" 
    check_consistency entry $entry
    Debug::leave
  }

  proc load_object_properties {id properties} {
    #Debug::enter

    set haveProperties [list]
    catch { set haveProperties [lindex $properties 0] } 
    set pCount [llength $haveProperties]

    Debug::print "haveProperties: $haveProperties"
    Debug::print "pCount = $pCount"

    set fixed_properties [list]
    for {set i 0} {$i < $pCount} {incr i 2} {
      Debug::print "i=$i"
      set pname    [lindex $haveProperties $i]
      set property [lindex $haveProperties [expr $i+1]]

      Debug::print "pname = '$pname', property ='$property'"

      set expl "\{[lindex $property 0]\}"
      set extraDetails 0
      catch {set extraDetails [lindex $property 1]}

      Debug::print "expl ='$expl',  extraDetails = '$extraDetails'"

      if {$extraDetails == 1 } {
        Debug::print "fixing extra details of a property"

        set detailsFile  [lindex $property 2]
        set start        [lindex $property 3]
        set length       [lindex $property 4]

        Debug::print "start = $start, length = $length" 

        set db_filename [Database::get_filename]
        set path [Database::Utility::file_path $db_filename]
        Debug::print "db_filename = '$db_filename', path = '$path'"
        if {$path != ""} {
          set detailsFile $path/$detailsFile
        }

	if { [file exists $detailsFile] } {
          set fixed_properties \
            "$fixed_properties $pname \{ \{$expl\} $extraDetails $detailsFile $start $length \}"
          Debug::print "fixed_properties = '$fixed_properties'" 
        } else {
          # Save property unchanged.
          Debug::print "unchanged property:'$pname \{$property\}'"
          set fixed_properties "$fixed_properties $pname \{$property\}"
        }

      } else {
        # Save property unchanged.
        Debug::print "unchanged property:'$pname \{$property\}'"
        set fixed_properties "$fixed_properties $pname \{$property\}"
      }         
    }
    #set fixed_properties "\{[join $fixed_properties]\} "
    set fixed_properties "\{$fixed_properties\}"

    set haveNotProperties {}
    catch { set haveNotProperties [lindex $properties 1]}
    lappend fixed_properties $haveNotProperties

    Database::set_object_properties $id $fixed_properties

    Debug::print "loaded properties: [Database::get_object_properties $id]"

    Debug::leave
  }

  proc object {id args} {
    Debug::enter 
    # trim first and last braces from the args.
    set args [Database::Utility::trim_double_cbraces $args]
    set dependencies ""
    set dependants ""
    set properties ""
    ::extractArguments {name definition keywords dependencies \
       dependants description properties} $args
    # Check consistency of the given information
    check_consistency object $id
    Database::new_object $id $name $definition $keywords \
      $dependencies $dependants $description

    Debug::print "parsing object properties: $id \{ $properties \}"
    
    load_object_properties $id $properties
    
    Debug::leave
  }

  proc attachments {unique_id_str totals_str attaches} {
    set unique_id [lindex $unique_id_str 1]
    set totals [lindex $totals_str 1]
    Database::init_attachments $unique_id
    eval $attaches
    Debug::assert {[Database::total_attachments] == $totals}
  }

  proc attachment args {
    ::extractArguments {id filename length startpos checksum counter} $args
    Database::set_attachment_filename $id $filename
    Database::set_attachment_length   $id $length
    Database::set_attachment_startpos $id $startpos
    Database::set_attachment_checksum $id $checksum
    Database::set_attachment_counter  $id $counter
    Database::append_attachments_index $id
  }

  proc check_consistency { objType {id ""} } {
    catch { check_consistency_$objType $id }
  }

  variable db_check_consistency 1
  variable all_entries [list]

  proc clear_consistency_info {} {
    variable all_entries
    set all_entries [list]
  }

  proc check_consistency_entry {id} {
    variable db_check_consistency
    if{ $db_check_consistency } {
      variable all_entries
      lappend all_entries id
    }  
  }

  proc check_consistency_object {id} {
    variable db_check_consistency
    if{ $db_check_consistency == yes } {
      variable all_entries
      set i [lsearch $all_entries $id]
      if{ $i == -1 } {
	Debug::print "Object $name (id=$id) is not listed in directory"
      } else { 
	# remove the object from the list of unchecked references
	set all_entries [lreplace $all_entries $i $i]
      }
    }
  }

  proc exists_undefined_objects {} {
    Debug::enter
    variable all_entries
    if {[llength $all_entries] > 0} { return [Debug::leave 1] }
    Debug::leave 0
  }

 
} ;# end of namespace: Database::Parser

#----------------------------------------------------------------------
#
# Namespace: Database::ParsingDatabase
#
#----------------------------------------------------------------------

namespace eval Database::ParsingDatabase {
  namespace export parse

  proc parse {database} {
    namespace import -force ::Database::Parser::header 
    namespace import -force ::Database::Parser::table_of_contents
    namespace import -force ::Database::Parser::directory 
    namespace import -force ::Database::Parser::entry  
    namespace import -force ::Database::Parser::object
    namespace import -force ::Database::Parser::attachments
    namespace import -force ::Database::Parser::attachment

    Database::init
    Database::Parser::init
    eval $database
    Database::Parser::done
  }
}

proc magnusdb {database} {
  Database::ParsingDatabase::parse $database
}
