#!/usr/bin/wish -f
## Copyright (C) 1998 The New York Group Theory Cooperative
## See magnus/doc/COPYRIGHT for the full notice.

## Contents: Convert Magnus inventory to HTML format 
##
## Principal Author: Alexei Myasnikov
##
## Status: In progress.
##
## Revision History:
##


## GLOBALS
##
set inv_dir ./ 
## Inventory directory     
if { ![file exists $inv_dir] } {
    puts "Define inventory directory first."
    exit
}

set html_dir ./html/  
## Destination directory for HTML files    
if { ![file exists $html_dir] } {
    puts "Define destination directory for html files first."
    exit
}

set open_comments 0
## Comments flag. 1 if '/*' occurs

set Status none
## Status of a current string in format.

set groups { \
		 {FP "Finitely Presented groups" } \
		 {Abelian "Abelian groups" } \
		 {AProducts "Amalgamated products" } \
		 {FreeGroups "Free groups" } \
		 {MSC "Small cancellation groups" } \
		 {FreeNilpotent "Free nilpotent groups"} \
		 {Nilpotent "Nilpotent groups" } \
		 {HNNExtensions " HNN extensions"} \
		 {OR "One relator groups"} \
	     }
## List of groups' directories and definitions

set objects {\
		 {group "Algorithms involving the whole group"} \
		 {subgroup "Algorithms involving a subgroup" }\
		 {word "Algorithms involving a word" } \
		 {word_subgroup "Algorithms involving words and subgroups" } \
		 {map "Algorithms involving maps"} \
		 {subgroup_map "Algorithms involving maps and subgroups" }\
		 {word_map "Algorithms involving maps and words" } \
		 {word_word "Algorithms involving two elements"}\
		 {make "Make another object"}\
		 {equation "Equations"} \
		 {quadratic_equation "Quadratic equation" } \
		 {set "Algorithms involving sets" }\
		 {word_homomorphism "Algorithms involving homomorphisms and words"}\
		 {homomorphism "Algorithms involving homomorphisms"}\
		 {tuple "Algorithms involving tuples"} \
		 {subgroup_subgroup "Algorithms involving two subgroups"} \
	     }
## List of all possible objects combinations and their descriptions

##
## END GLOBALS


## Returns directory name of an element from groups or objects list
proc getDir { list } {
    return [lindex $list 0]
}

## Returns description of an element from groups or objects list
proc getName { list } {
    return [lindex $list 1]
}

## Returns list of files in a directory
proc getFileList { directory } {
    
    if { ![file exists $directory] } {
	return { }
    }
    
    set filelist [glob -nocomplain $directory/*]
    return $filelist
}

## Scans file for Problem name
proc getProblemName { file } {
    global Status
    set f [open $file]
    
    set Status none
    set retS ""
    while { ![eof $f] } {
	set fstr [gets $f]
	
	set s [formatProbStr $fstr] 
	while { $Status == "menu_name" } {
	    regsub -all {<B>Name in the menu:</B>} $s "" s1
	    set retS "$retS$s1"
	    set fstr [gets $f]
	    set s [formatProbStr $fstr] 
	}
	if { [string length $retS] > 0} {
	    close $f
	    return $retS	    
	}
	
    }
    puts "Error! Can't find the problem name."
    close $f
    return $retS
}

## Scans inventory for problems
proc scanProblems { group objects } {
    global problems
    global inv_dir

    set problem_list [getFileList $inv_dir/$group/$objects]
    if { [llength $problem_list] == 0 } {
	puts "Problems for $group/$objects do not exist"
    } 
    set problems($group/$objects) {}
    foreach p $problem_list {
	if {![file isdirectory $p] } {
	    lappend problems($group/$objects) $p
	}
    }
    
}


## Prints index of inventory to a file
proc printGroupIndex { } {
    global problems
    global groups
    global objects
    global html_dir
    global inv_dir

    puts "Making general index file ..."
    set fileID [open $html_dir/grindex.html w]
    puts $fileID "<HTML>  <HEAD> \
  <TITLE>Magnus Inventory</TITLE> \
 </HEAD> "
    
    puts $fileID "  <H1 ALIGN=\"CENTER\">Index of Magnus Inventory</H1> <P>"
    
 

   puts $fileID "<OL TYPE=I>"
    foreach group $groups {
	puts $fileID "<LI> <H3><B> [getName $group]</B> </H3>"
	set group_dir [getDir $group]
	
	puts $fileID "<OL TYPE=1>"
	
	foreach object $objects {
	    if { [file exists "$inv_dir/$group_dir/[getDir $object]"] } {
		puts $fileID "<LI><A HREF=\"$group_dir[getDir $object].html\" > [getName $object] </A>"
	    }
	    	}
	puts $fileID "</OL> <P>"
	
    }
    puts $fileID "</OL>"
    close $fileID
}

## Prints indexes of problems corresponding to objects
proc printObjectsIndex {  } {
    global problems
    global groups
    global objects
    global html_dir

    puts "Making problems index ..."

    foreach group $groups {
	set group_dir [getDir $group]
	foreach object $objects {
	    set f [open $html_dir/$group_dir[getDir $object].html w]
	    puts $f "<HTML>  <HEAD> \
		 <TITLE>Magnus Inventory</TITLE> \
		</HEAD>" 
	    puts $f "<H1>[getName $group]: [getName $object]</H1>"
	    set prob_list $problems($group_dir/[getDir $object])
	    
	    puts $f "<OL TYPE=1>"
	    set i 1
	    foreach p $prob_list {
		puts $f "<LI> <A HREF=\"Prob$group_dir[getDir $object].html\#$i\"> [getProblemName $p] </A>"
		set i [expr $i+1]
		
	    }
	    puts $f "</OL>"
	    close $f
	}
	
    }
  
}



proc formatProbStr { str } {
    set s $str
    
    global open_comments
    global Status
    
    ## Make words lower case 
    regsub -all {( W)([.,1-9 ])} $s { <B>w</B>\2} s1
    regsub -all {( W)($)} $s1 { <B>w</B>\2} s

    ## Make maps lower case 
    regsub -all {( M)([.,1-9 ])} $s { <B>m</B>\2} s1
    regsub -all {( M)($)} $s1 { <B>m</B>\2} s
    
    ## Highlight the objects 
    regsub -all {( [hHAGNFEST])([.,1-9 ])} $s { <B>\1</B>\2} s1
    regsub -all {( [hHAGNFEST])($)} $s1 { <B>\1</B>\2} s

    ## If comments was opened
    if { $open_comments == 1 } {

	if {[regexp -indices {[.]*\*/[.]*} $s a ] } {
	    set sep [lindex $a 0]
	    set s [string  range $s 0 [expr $sep -1]]
	    set open_comments 0
	} else {
	    return ""
	}
    }

    ## If opening comments
    if {[regexp -indices {[.]*/\*[.]*} $s a ] } {
	set sep [lindex $a 0]
	set s [string  range $s 0 [expr $sep -1]]
	set open_comments 1
    }

    ## if // comment
    if {[regexp -indices {[.]*//[.]*} $s a ] } {
	set sep [lindex $a 0]
	set s [string  range $s 0 [expr $sep -1]]
    }

    ## If spaces only return empty
    if { [regexp {^[ ]*$} $s] } {
	return ""
    }
    
    ## Objects highlighted: field
    if {[regsub -all {Objects highlighted:} $s "\n<B>Objects highlighted:</B>" out ] } {
	set Status obj_high
	return $out
    } 

    ## Check-in type: field
     if {[regsub -all {Check-in type:} $s "\n<B>Check-in type:</B>" out ] } {
	set Status check_type
	return $out
    } 

    ## Name in the menu:
   if {[regsub -all {Name in the menu:} $s "\n<B>Name in the menu:</B>" out ] } {
	set Status menu_name
	return $out
    } 

    ## Problem type: field
   if {[regsub -all {Problem type:} $s "\n<B>Problem type:</B>" out ] } {
       set Status prob_type
       return $out
    } 

    ## Help file: field
    if {[regsub -all {Help file:} $s "\n<B>Help file:</B>" out ] } {
	set Status help_file
	return $out
    } 

    ## Other objects present:
    if {[regsub -all {Other objects present:} $s "\n<B>Other objects present:</B>" out ] } {
	set Status other_obj
	return $out
    } 

    ## Highlight Fast checks
    if {[regsub -all {Fast checks:} $s "</PRE><CENTER><B>Fast checks:</B></CENTER><PRE>" out ] } {
	set Status fast_checks
	return $out  
    }

    ## Highlight Algorithms:
    if {[regsub -all {Algorithms:} $s "</PRE><CENTER><B>Algorithms:</B></CENTER><PRE>" out  ] } {
  	set Status algorithms
	return $out  
    }
		
    ## Highlight Links:
     if {[regsub -all {Links:} $s "</PRE><B>Links:</B><PRE>" out ] } {
  	set Status links
	return $out  
    }  
    
    return "$s"
}

proc makeProbFiles {  } {
    global problems
    global groups
    global objects
    global html_dir

    puts "Generating problem files ..."

    foreach group $groups {
	set group_dir [getDir $group]
	foreach object $objects {
	    set f [open $html_dir/Prob$group_dir[getDir $object].html w]
	    puts $f "<HTML>  <HEAD> \
		 <TITLE>Magnus Inventory</TITLE> \
		</HEAD>" 

	    puts $f "<H1>[getName $group]: [getName $object] </H1>"
	    set prob_list $problems($group_dir/[getDir $object])
	    set i 1
	    foreach p $prob_list {
		puts $f "<A NAME = $i><CENTER><H3> $i. [getProblemName $p] </H3> </CENTER> </A><PRE>" 
	      
		set f1 [open $p]
		while { ![eof $f1] } {
		    set s [gets $f1]
		    set s1 [formatProbStr $s]
		    if { [string length $s1] > 0 } {
			puts $f $s1
		    }
		}
		set i [expr $i + 1]
		close $f1
		puts $f "</PRE>"
	    }
	    close $f
	}
	
    }
  
}

##
##        MAIN
##
####################################################################
#puts $groups
foreach group $groups {
    set group_dir [getDir $group]
    foreach object $objects {
	scanProblems $group_dir [getDir $object]
##	puts $problems($group_dir/$object)
    }
}

printGroupIndex 
printObjectsIndex
makeProbFiles

exit
