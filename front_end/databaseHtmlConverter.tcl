
namespace eval HtmlConverter {

  namespace export exportDatabaseTo


  #
  # Constants:
  #

  set BriefDefinitionLimit 0
  set wwwdatabase ""
  #
  # Array: registry
  #

  set registry(array) yes

  proc exportDatabaseTo {save_dir} {
    variable registry
    catch {unset registry}
    set registry(array) yes

    variable wwwdatabase
    set wwwdatabase magnusdb.db

    make_html_filenames $save_dir
    write_index         $save_dir

    catch {exec /bin/rm $save_dir/${wwwdatabase}2 }
    set secFile [::open $save_dir/${wwwdatabase}2 w]
    ::close $secFile

    # quick hack. actually we don't need to change database filename during
    # saving.
    set old_filename [Database::get_filename]
    set old_secondary_filename [Database::get_secondary_filename]
    Database::set_filename $save_dir/$wwwdatabase

    Database::save    $save_dir/$wwwdatabase $old_secondary_filename

    # Set back the true filename.
    Database::set_filename $old_filename

    #write_experiments     $save_dir
    #write_categories     $save_dir
  }

  #
  #  Proc: replaceSpacesByUndersores
  #

  proc replaceSpacesByUnderscores {str} {
    regsub -all "_" $str " " result
    return $result
  }

  #
  #  Proc: make_html_filenames
  #

  proc make_html_filenames {save_dir} {
    Debug::enter
    variable registry
    catch {exec mkdir $save_dir}
    if { ![file writable $save_dir] } {
      return 0
    }
    catch { exec mkdir "$save_dir/experiments" }

    set root_dir [::Database::root_dir]
    foreach child [::Database::get_directory_children $root_dir] {
      if { [::Database::get_directory_category $child] == "EXPERIMENTS" } {
	set root_dir $child
	break
      }
    }

    set registry(categories,list) [list]
    set exp_counter 0
    foreach expID [::Database::get_directory_children $root_dir] {
      Debug::print "expID = $expID"

      incr exp_counter
      lappend registry(experiments,list) $expID
      set registry(url,experiment,$expID) "experiments/e$exp_counter.html"

      set exp_objects_counter 0
      foreach cathID [::Database::get_directory_children $expID] {
        Debug::print "cathID = $cathID"
	set category [::Database::get_directory_category $cathID]
        set cathname  [replaceSpacesByUnderscores $category]
        set children [::Database::get_directory_children $cathID]
	  if {[catch {set cath_counter [get_counter category $category]}]} {
          set cath_counter 0
	}
        set old_cath_counter $cath_counter

	Debug::print "cath_counter is $cath_counter for $category"
        foreach child $children {
	  Debug::print "child = $child"
	  if { [::Database::get_directory_type $child] == "object" } {
	    incr cath_counter
	    set registry(url,object,$child) "$cathname/o$cath_counter.html"
	  } else {
	    set children2 [::Database::get_directory_children $child]
	    foreach child2 $children2 {
 	      incr cath_counter
	      set registry(url,object,$child2) "$cathname/o$cath_counter.html"
	    }
	  }
	}
	Debug::print "checking category name..."
	if { [lsearch $registry(categories,list) $category] == -1 } {
	  lappend registry(categories,list) $category
	  set registry(url,category,$category) "${cathname}.html"
	  catch { exec mkdir "$save_dir/$cathname" }
	  set registry(counter,category,$category) $cath_counter
	} else {
	  set registry(counter,category,$category) $cath_counter
        }
	Debug::print "cath_counter update: [get_counter category $category]"
	# 
	# update counter of the experiment
	incr exp_objects_counter [expr $cath_counter - $old_cath_counter]
      }
      
      set registry(counter,experiment,$expID) $exp_objects_counter
    }

  }

  proc get_counter {type id} {
    variable registry
    return $registry(counter,$type,$id)
  }

  proc get_object_url {type id} {
    variable registry
    return $registry(url,$type,$id)
  }

  proc write_index {save_dir} {
    Debug::enter

    variable registry
    set fileID [open $save_dir/index.html w]

    # write HTML header
    puts $fileID "<HTML> \n\
<HEAD>\n\
  <TITLE>Magnus Database</TITLE>\n\
</HEAD>"

    puts $fileID "<H1 ALIGN=\"CENTER\">Index of Magnus Database</H1> <P> <P>"

    # write list of experiments with links.
    puts $fileID "<H2 ALIGN=\"CENTER\">List of experiments</H2> <P>"
    puts $fileID "<OL TYPE=I>"

    set root_dir [::Database::get_dir_id EXPERIMENTS]
    foreach expID [::Database::get_directory_children $root_dir] {
      set exp_name [::Database::get_directory_name $expID]
      set url [get_object_url experiment $expID]
      set exp_objects_counter [get_counter experiment $expID]
      puts $fileID "<LI><A HREF=\"$url\">$exp_name</A> ($exp_objects_counter)"
      write_experiment $save_dir $expID
    }
    puts $fileID "</OL><P><P>"

    # write list of categories with links.
    puts $fileID "<H2 ALIGN=\"CENTER\">List of categories</H2> <P>"
    puts $fileID "<OL TYPE=I>"
    foreach category $registry(categories,list) {
      set cath_name [::Database::get_category_name $category]
      Debug::print "category name = $category/$cath_name"
      set url [get_object_url category $category]
      set cath_objects [get_counter category $category]
      puts $fileID "<LI><A HREF=\"$url\">$cath_name</A> ($cath_objects)"
      write_category $save_dir $category
    }
    puts $fileID "</OL><P><P>"

    # write link to download the database.
    variable wwwdatabase
    puts $fileID "Download the Magnus database: \
<A HREF=\"$wwwdatabase\">index file</A> and <A HREF=\"${wwwdatabase}2\">data file</A>."

    # write time stamp.
    puts $fileID "<P>This file is generated on [exec /bin/date]"

    # close index file.
    close $fileID
  }

#  proc write_experiments {dir} {
#    set root_dir [Database::get_dir_id EXPERIMENTS]
#    foreach experiment [Databsse::get_dir_children $root_dir] {
#      write_experiment $dir $experiment
#    }
#  }

  proc write_experiment {save_dir expID} {
    Debug::enter

    variable registry

    set exp_name  [::Database::get_directory_name $expID]
    set filename [get_object_url experiment $expID]
    set fileID [open $save_dir/$filename w]

    # write experiment's title.
    puts $fileID "<HTML> <HEAD> <TITLE>Experiment: $exp_name</TITLE> </HEAD>\n\
<H1 ALIGN=\"CENTER\">Index of '$exp_name' experiment</H1> <P> <P>"

    set obj_index 1
    # write categorized list of objects of the experiment.
    puts $fileID "<OL TYPE=I>"
    foreach cathID [::Database::get_directory_children $expID] {
      set category [Database::get_directory_category $cathID]
      set cath_name [::Database::get_directory_name $cathID]
      puts $fileID "<LI> $cath_name <OL START=$obj_index>"
      set all_children [::Database::get_directory_children $cathID] 
      set subcaths [list]
      set objects [list]
      foreach child $all_children {
	if {[::Database::get_directory_type $child] == "object"} {
	  lappend objects $child
	} else {
	  lappend subcaths $child
	}
      }

      foreach object $objects {
	write_object_brief $fileID $object $obj_index ../
	write_object $save_dir $object $expID $category
	incr obj_index
      } 

      set sub_index 1
      foreach child $subcaths {
	puts $fileID "<LI TYPE=i VALUE=$sub_index>subcategory [::Database::get_directory_name $child] <OL>"
	incr sub_index
	foreach object [::Database::get_directory_children $child] {
          write_object_brief $fileID $object $obj_index ../
 	  write_object $save_dir $object $expID $category
	  incr obj_index
	}
	puts $fileID "</OL>"
      }
      puts $fileID "</OL>"
    }
    # write useful links.
    puts $fileID "<P><A HREF=\"../index.html\">Back to main database index</A>"
    # close experiment's file.
    close $fileID
  }

  proc write_object_brief {fileID object index {base_url {}} } {
    Debug::enter

    variable BriefDefinitionLimit
    set name [::Database::get_object_name $object]
    set definition [::Database::get_object_definition $object]
    if {$BriefDefinitionLimit != 0} {
      set definition [string range $definition 0 $BriefDefinitionLimit]
    }
    set url [get_object_url object $object]
      puts $fileID "<LI TYPE=1 VALUE=$index><A HREF=\"$base_url$url\">$name</A> $definition"
  }

  proc write_category {save_dir category} {
    Debug::enter
    
    set cath_name [::Database::get_category_name $category]
    set filename [get_object_url category $category]
    set fileID [open $save_dir/$filename w]
    set obj_index 0

    # write category's title.
    puts $fileID "<HTML><HEAD><TITLE>Category: $cath_name</TITLE></HEAD>\n\
<H2 ALIGN=\"CENTER\">Index of '$cath_name' category</H2> <P> <P>"

    #
    set objects [list]
    set subcaths [list]
    Database::get_all_objects_in_category $category objects subcaths

    if {[llength $objects] > 0} {
      puts $fileID "<OL>"
      set obj_index 1
      foreach object $objects {
        write_object_brief $fileID $object $obj_index
	incr obj_index
      }
      puts $fileID "</OL>"
    }

    if {[llength $subcaths] > 0} {
      puts $fileID "<P><OL TYPE=i>"
      foreach subcath $subcaths {
	puts $fileID "<LI>$subcath"
	puts $fileID "<OL>"
	set objects [list]
	set ss_dummy [list]
	Database::get_all_objects_in_category $subcath objects ss_dummy
	foreach object $objects {
	  write_object_brief $fileID $object $obj_index
 	  incr obj_index
	}
	puts $fileID "</OL>"
      }
      puts $fileID "</UL>"
    }
    # write useful links.
    puts $fileID "<P><A HREF=\"index.html\">Back to main database index</A>"
    # close experiment's file.
    close $fileID
  }

  proc write_object {save_dir object expID cathID} {
    Debug::enter

    set name         [::Database::get_object_name         $object]
    set definition   [::Database::get_object_definition   $object] 
    set keywords     [::Database::get_object_keywords     $object]
    set description  [::Database::get_object_description  $object]
    set dependencies [::Database::get_object_dependencies $object]
    set dependants   [::Database::get_object_dependants   $object]
    set properties   [::Database::get_object_properties   $object]

    set experiment [::Database::get_directory_name $expID]
    set category [::Database::get_category_name $cathID] 
    set filename [get_object_url object $object]
    set fileID [open $save_dir/$filename w]
    set exp_url [get_object_url experiment $expID]
    set cath_url [get_object_url category $cathID]
      
    # write object's title.
    puts $fileID "<HTML>\n\
<HEAD><TITLE>Object: $name</TITLE></HEAD>\n\
<B>Experiment</B>: <A HREF=\"../$exp_url\">$experiment</A>\n\
<BR> <B>Category</B>: <A HREF=\"../$cath_url\">$category</A>\n\
<P>  <B>Name</B>: $name\n\
<BR> <B>Definition</B>: $definition\n\
<P>  <B>Keywords</B>: $keywords\n\
<BR> <B>Description</B>: $description"

    if {[llength $dependencies] > 0} {
      puts $fileID "<BR> <B>Depends on</B>: "
      foreach parent $dependencies {
	set parent_url [get_object_url object $parent]
	set parent_name [::Database::get_object_name $parent]
	puts $fileID "<A HREF=\"../$parent_url\">$parent_name</A>"
      }
    }

    if {[llength $dependants] > 0} {
      puts $fileID "<BR> <B>Dependants</B>: "
      foreach dep $dependants {
	set dep_url [get_object_url object $dep]
	set dep_name [::Database::get_object_name $dep]
	puts $fileID "<A HREF=\"../$dep_url\">$dep_name</A>"
      }
    }

    # write properties
    puts $fileID "<P><B>Properties</B>:"
    set properties [::Database::get_object_properties_names $object]
    if { [llength $properties] > 0 } {
      puts $fileID "<BR><OL>"
      foreach property $properties {
        set type [::Database::get_object_property_type $object $property]
        if { $type == "have" } {
          puts $fileID "<LI>$property: yes"
        } elseif { $type == "have not" } {
          puts $fileID "<LI>$property: no"
        } else {
	  puts $fileID "<LI>$property: $type"
	}
      }
      puts $fileID "</OL>"
    }
    # write useful links.
    puts $fileID "<P><A HREF=\"../index.html\">Back to main database index</A>"
    # close experiment's file.
    close $fileID
  }

}