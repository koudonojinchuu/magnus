
##
##   $Id $
##

## Copyright (C) 1996 The New York Group Theory Cooperative
## See magnus/doc/COPYRIGHT for the full notice.

## Contents: General implementation of computation files
##
## Principal Author: Dmitry Bormotov, Alexei Myasnikov
##
## Status: In progress.
##
## Revision History:
##


## lastLogLink
#
set lastLogLink 0
#
# Type: integer
#
# Keeps the number of last link available


# bool lastLogLinkIsUpToDate 
#
#   False if there's no link with the number $lastLogLink.
#   logInsert makes it false, post_to_log_ updates it if necessary.

set lastLogLinkIsUpToDate 1


## logLink
#
# Type: array
# Keyed by log link number, stores the following information:
# 
#   string  problemName  - name of the problem which has links,
#	                   by convention the following function
#			   must exist:
#                          handle$problemName and update$problemName
#
#   string  fileName     - name of the information file
#   bool    isDynamic    - a file is dynamic if it's going to be
#			   changed during the magnus session
#   string  currentFG    - current foreground
#
# The following data members exist only for dynamic files:
#
#   integer readfileSize     - current size of the file 
#   integer seekPosition - current seek position of the file (all characters
#                          after that position have not been read yet)
#
#   firstPosition - begining position of the buffer in a file
#   lastPosition - ending  position of the buffer in a file
#   curColStartPos - position of a current color in color file
#   nextColStartPos - position of the next color in color file
#   curColFilePos - position in a file in which current color starts


# list linkIdleList
#
# Stores numbers of opened links
# 

set linkIdleList ""


# const int linkIdleInterval
#
# Keeps the number of milliseconds between file updates
#

set linkIdleInterval 1000


proc append_viewLog {viewID msg} {
  global viewData lastLogLink
  global logLink lastLogLink lastLogLinkIsUpToDate

  set message $msg
  if { [regexp {(.*)\@(.+)\@(.+)\@(.+)\@(.+)\@(.*)} $msg \
	exp leftRest text problemName fileName isDynamic rightRest] > 0 } {
      
      set message $leftRest@$lastLogLink@$text@$rightRest
  }

  set viewData($viewID,log) $viewData($viewID,log)$message
}


proc fileUpdate { linkNumber } {
    global logLink
    global logHoldView

    set readfileSize $logLink($linkNumber,readfileSize)
    set t [getLinkWindowName $linkNumber].logFrame.log
    set fileName $logLink($linkNumber,fileName)

    set fileSize [ file size $fileName ]
 
    #  find an end of ready to use data in a file

    if { $readfileSize < $fileSize} {
	set logLink($linkNumber,readfileSize) $fileSize

	if { ([info exists logHoldView($t)]
	     && ($logHoldView($t) == 0) ) || $readfileSize == 0 } {
	    goBottom $linkNumber
	} else {
	    set fr1 [lindex [$t yview] 0]
	    set fr2 [lindex [$t yview] 1]
	    textChanged $linkNumber $fr1 $fr2
	}
    }
}

proc putInLinkIdleList {linkNumber} {
	
    global logLink linkIdleList	

    set logLink($linkNumber,readfileSize) 0
    set logLink($linkNumber,fileSize) 0
    set logLink($linkNumber,firstPosition) 0
    set logLink($linkNumber,lastPosition) 0
    set logLink($linkNumber,curColStartPos) 0
    set logLink($linkNumber,nextColStartPos) 0
    set logLink($linkNumber,curColFilePos) 0
    
    lappend linkIdleList $linkNumber

    if { [llength $linkIdleList] == 1 } {
    	linkIdleUpdate
    }
}


proc linkIdleUpdate { } {
	   
  global linkIdleList linkIdleInterval logLink
    
  set newList ""  

  foreach linkNumber $linkIdleList {

    if { [winfo exists [getLinkWindowName $linkNumber]] \
	 && $logLink($linkNumber,isDynamic) } {

      lappend newList $linkNumber
    }
  }

  set linkIdleList $newList
  if { [llength $linkIdleList] == 0 } { return }
  
  foreach linkNumber $linkIdleList {
    fileUpdate $linkNumber
  }

  after $linkIdleInterval { linkIdleUpdate }
}


proc linkStaticUpdate { linkNumber } {
    global logLink
    set logLink($linkNumber,readfileSize) 0
    set logLink($linkNumber,firstPosition) 0
    set logLink($linkNumber,lastPosition) 0
    set logLink($linkNumber,curColStartPos) 0
    set logLink($linkNumber,nextColStartPos) 0
    set logLink($linkNumber,curColFilePos) 0

    fileUpdate $linkNumber
    
    #set f [open $logLink($linkNumber,fileName)]
    #while { ![eof $f] } {
    #	regsub -all {(@@)|(@%)} [read $f 1024] "" s
    #	putInFile $linkNumber $s
    #}
    #close $f
    
}


proc getLinkWindowName { linkNumber } {
    return ".file$linkNumber"
}


proc logInsert {logName message} {

  global logLink lastLogLink lastLogLinkIsUpToDate

  if { [regexp {(.*)\@(.+)\@(.+)\@(.+)\@(.+)\@(.*)} $message \
	exp leftRest text problemName fileName isDynamic rightRest] > 0 } {
      
    set linkNumber $lastLogLink
    set lastLogLinkIsUpToDate 0

    set logLink($linkNumber,problemName) $problemName
    set logLink($linkNumber,fileName) $fileName
    set logLink($linkNumber,isDynamic) [expr int($isDynamic)]
    set logLink($linkNumber,currentFG) ""

    $logName insert end $leftRest
    #@db logInsert $logName $leftRest
    $logName insert end $text "Link $linkNumber"
    #$logName insert end $rightRest
    logScrollAppend $logName $rightRest
    bindLinks $logName $linkNumber

  } else {
    #$logName insert end $message
    logScrollAppend $logName $message
  }
}


proc bindLinks { logName linkNumber } {

    $logName tag bind $linkNumber <Button-1> {
	
      set i [%W index @%x,%y]
      set tags [%W tag names $i]
      
      foreach tag $tags {
	if { [regexp {^[0-9]+$} $tag] > 0 } {
	    
	  global logLink
	  set windowName [getLinkWindowName $tag]	  

	  if { [winfo exists $windowName] } { 
	    if { [wm state $windowName] == "iconic" } {
	      wm deiconify $windowName
            }
            return 
	  }
	  
	  eval { handle$logLink($tag,problemName) \
		 $tag $logLink($tag,fileName) }
	  
	  if { $logLink($tag,isDynamic) } {
	    putInLinkIdleList $tag
	  } else {
	    linkStaticUpdate $tag
	  }
	  
	  break
        }
      }
    }
	
    $logName tag configure Link \
      -foreground MediumSeaGreen \
      -font *-times-medium-i-normal--*-140-*-*-*-*-*-* \
      -underline 1
    
	
    $logName tag bind Link <Enter> {
      %W configure -cursor hand1
    }
	
    $logName tag bind Link <Leave> {
      %W configure -cursor xterm
    }
}


proc viewLogInsert {logName msg} {

  global logLink 
  set message $msg

  while { [regexp -indices {@([0-9]*)@([^@]*)@} $message \
	       exp indices textIndices] > 0 } {
      
      set leftIndex  [lindex $indices 0]
      set rightIndex [lindex $indices 1]
      set textLeftIndex  [lindex $textIndices 0]
      set textRightIndex [lindex $textIndices 1]
      set linkNumber [string range $message $leftIndex $rightIndex]
      set text [string range $message $textLeftIndex $textRightIndex]

      $logName insert end [ string range $message 0 [expr $leftIndex-2] ]
      $logName insert end $text "Link $linkNumber"
      bindLinks $logName $linkNumber
      set message [string range $message [expr [lindex $textIndices 1]+2] end]
  }

  logScrollAppend $logName $message
}
# ------------------------------- Buffers ---------------------------------- #
# returns number of bytes to position in a file where previous color starts
proc lenToPrevCol { pos linkNumber } {
    global logLink
    return [expr $pos-$logLink($linkNumber,curColStartPos)]
}

# returns number of bytes to position in a file where next color starts
proc lenToNextCol { pos linkNumber } {
    global logLink
    return [expr $logLink($linkNumber,nextColStartPos)-$pos]
}

# sets current color to the first color in a color file
proc setToFirstColor { linkNumber } { 
    global logLink
    set cF [open  $logLink($linkNumber,fileName).color r]
    seek $cF 0
    set colStr [read $cF 13]
    regexp {[\#]([^%]*)[%]([^@]*)[@]} $colStr s p c
    scan $p "%d" curColP
    
    set nextColP $curColP
    set cn $c
    set filePos -13
    
    set sizeOfCColFile [file size  $logLink($linkNumber,fileName).color]

    while { $curColP == $nextColP } {
	incr filePos 13
	if { [expr [tell $cF]+13] > $sizeOfCColFile } {
	    set $nextColP  $sizeOfCColFile
	    break
	} else {
	    set curColP $nextColP
	    set c $cn
	    set colStr [read $cF 13]
	    regexp {[\#]([^%]*)[%]([^@]*)[@]} $colStr s p cn
	    scan $p "%d" nextColP
	} 
   }
    set logLink($linkNumber,curColStartPos) $curColP
    set logLink($linkNumber,nextColStartPos) $nextColP
    set logLink($linkNumber,curColFilePos) $filePos

    set t [getLinkWindowName $linkNumber].logFrame.log 
    set color \#$c    
    set logLink($linkNumber,currentFG) $color
    $t tag configure $color -foreground $color 
    close $cF
}

# sets current color to the last color in a color file
proc setToLastColor { linkNumber } { 
    global logLink
    set cF [open  $logLink($linkNumber,fileName).color r]
    seek $cF -13 end
    set colStr [read $cF 13]
    regexp {[\#]([^%]*)[%]([^@]*)[@]} $colStr s p c
    set color \#$c    
    scan $p "%d" logLink($linkNumber,curColStartPos) 
    set logLink($linkNumber,nextColStartPos) [file size $logLink($linkNumber,fileName) ]
    set newColFilePos [expr [file size $logLink($linkNumber,fileName).color ]-13]
    set logLink($linkNumber,curColFilePos) $newColFilePos
    
    set t [getLinkWindowName $linkNumber].logFrame.log 
    set logLink($linkNumber,currentFG) $color
    $t tag configure $color -foreground $color 
    close $cF
}

# sets current color to the color which corresponds to the given position
# starting from the current position it seeks color in a color file, which
# is cover the position
proc setCurrentColor { pos linkNumber} {
    global logLink
    set color blue
    set colFile [open $logLink($linkNumber,fileName).color r]
    while { $pos <= $logLink($linkNumber,curColStartPos) || 
	    $pos > $logLink($linkNumber,nextColStartPos) } {
	set bufSize 13
	if { $pos <= $logLink($linkNumber,curColStartPos)} {
	    set newPos [expr $logLink($linkNumber,curColFilePos)-$bufSize]
	    if { $newPos >= 0 } {
		seek $colFile $newPos
		set colStr [read $colFile $bufSize]
		regexp {[\#]([^%]*)[%]([^@]*)[@]} $colStr s p c
		set color \#$c
		set logLink($linkNumber,nextColStartPos) $logLink($linkNumber,curColStartPos)
		scan $p "%d" np
		set logLink($linkNumber,curColStartPos) $np 
		set logLink($linkNumber,curColFilePos) $newPos
	    } else { 
		close $colFile
		setToFirstColor $linkNumber
		return
	    } 
	} else {
	    set newPos [expr $logLink($linkNumber,curColFilePos)+$bufSize]
	    if { [expr $newPos+13] <= [ file size $logLink($linkNumber,fileName).color ] } {
		seek $colFile $newPos
		set colStr [read $colFile $bufSize]
		regexp {[\#]([^%]*)[%]([^@]*)[@]} $colStr s p c
		set color \#$c
		set logLink($linkNumber,curColStartPos) $logLink($linkNumber,nextColStartPos)
		set colStr [read $colFile $bufSize]
		regexp {[\#]([^%]*)[%]([^@]*)[@]} $colStr s p c
		scan $p "%d" logLink($linkNumber,nextColStartPos) 
		set logLink($linkNumber,curColFilePos) $newPos
	    } else {
		close $colFile
		setToLastColor $linkNumber
		return
	    }
	}

	set t [getLinkWindowName $linkNumber].logFrame.log
     
	set logLink($linkNumber,currentFG) $color
	if { $color!="" } { $t tag configure $color -foreground $color }

   }
    close $colFile
} 

# prepends a string to the text window
proc putAtTopInWindow { linkNumber str } {
    global logLink
    global A
    set D ""
    set windowName [getLinkWindowName $linkNumber].logFrame.log

    set subStr $str
    while { [regexp {\$A\([\$a-zA-Z0-9,]*\)} $subStr Aname] } {
	eval set name \"$Aname\"
	regsub {\$A\([\$a-zA-Z0-9,]*\)} $subStr $name new 
	set subStr $new
    }

    
    #$windowName configure -state normal
    set text $subStr
    $windowName insert 0.0 $text $logLink($linkNumber,currentFG)
    #$windowName configure -state disabled
}

# appends a string to the text window
proc putToEndInWindow { linkNumber str } {
    global logLink
    set D ""
    global A

    set windowName [getLinkWindowName $linkNumber].logFrame.log
    
    set subStr $str
    while { [regexp {\$A\([\$a-zA-Z0-9,]*\)} $subStr Aname] } {
	eval set name \"$Aname\"
	regsub {\$A\([\$a-zA-Z0-9,]*\)} $subStr $name new 
	set subStr $new
    }

    set text $subStr

    #$windowName configure -state normal
    $windowName insert end $text $logLink($linkNumber,currentFG)
    #$windowName configure -state disabled
}

# erase everething in a window, between ind1 and ind2
proc deleteInWindow { windowName ind1 ind2 } {
   #$windowName configure -state normal
   $windowName delete $ind1 $ind2
   #$windowName configure -state disabled
}

# reads from the file and prepends given number of bytes to the buffer
proc prependBuffer { linkNumber size } {
    global logLink
    set firstPosition $logLink($linkNumber,firstPosition) 
    set lastPosition $logLink($linkNumber,lastPosition) 
    set t [getLinkWindowName $linkNumber].logFrame.log
    set fileName $logLink($linkNumber,fileName)
    
    if { [expr $firstPosition-$size] < 0 } {
	set size $firstPosition
    }

    set file [open $fileName r]
    set tmpPos [expr $firstPosition]
    set logLink($linkNumber,firstPosition) [expr $firstPosition-$size]

    while { $size > 0 } {
	setCurrentColor [expr $tmpPos-1] $linkNumber
	set newDelta [lenToPrevCol $tmpPos $linkNumber]
	if { $newDelta > $size } {
	    set newDelta $size
	}
	
	seek $file [expr $tmpPos-$newDelta]
	set str [read $file $newDelta] 
	putAtTopInWindow $linkNumber $str
	
	incr size -$newDelta
	incr tmpPos [expr -$newDelta]
	
    }
    close $file
}

# sets the buffer to the end of a file
proc goBottom { linkNumber } {
    global logLink
    set t [getLinkWindowName $linkNumber].logFrame.log
    set readfileSize $logLink($linkNumber,readfileSize)

    deleteInWindow $t 0.0 end

    set logLink($linkNumber,firstPosition) $readfileSize 
    set logLink($linkNumber,lastPosition)  $readfileSize 
    setToLastColor $linkNumber
    prependBuffer $linkNumber 3000
    $t yview end
    $t mark set insert end
}
# sets the buffer to the begining of a file
proc goTop { linkNumber } {
    global logLink

    set t [getLinkWindowName $linkNumber].logFrame.log
    set readfileSize $logLink($linkNumber,readfileSize)

    deleteInWindow $t 0.0 end

    set logLink($linkNumber,firstPosition) 0
    set logLink($linkNumber,lastPosition) 0
    setToFirstColor $linkNumber
    appendBuffer $linkNumber 3000
    $t yview 0.0
    $t mark set insert 0.0
}

# reads from the file and appends given number of bytes to the buffer
proc appendBuffer { linkNumber size } {
    global logLink
    set firstPosition $logLink($linkNumber,firstPosition) 
    set lastPosition $logLink($linkNumber,lastPosition) 
    set t [getLinkWindowName $linkNumber].logFrame.log
    set readfileSize $logLink($linkNumber,readfileSize)
    set fileName $logLink($linkNumber,fileName)
    
    if { [expr $lastPosition+$size] > $readfileSize } {
	set size [expr $readfileSize-$lastPosition]
    }
    
    set file [open $fileName r]
    set tmpPos [expr $lastPosition]
    set logLink($linkNumber,lastPosition) [expr $lastPosition+$size]

    while { $size > 0 } {
	setCurrentColor [expr $tmpPos+1] $linkNumber
	set newDelta [lenToNextCol $tmpPos $linkNumber]
	
	if { $newDelta > $size } {
	    set newDelta $size
	}
	
	seek $file [expr $tmpPos]
	set str [read $file $newDelta] 
	putToEndInWindow $linkNumber $str
	
	incr size -$newDelta
	incr tmpPos $newDelta
	
    }
   
    close $file
}

# save as
proc saveLog { linkNumber w } {
    global logLink
    set sourceFile $logLink($linkNumber,fileName)
    set destFile [savefile [uniqueDialogName]]
    exec cp -f $sourceFile $destFile
}
# ------------------------------- fileView ---------------------------------- #
# recalculate and redraw srollbar when text window was changed
proc textChanged { linkNumber s1 s2 } {
   
    global logLink
    set s [getLinkWindowName $linkNumber].logFrame.logScrollbar
    
    set firstPosition $logLink($linkNumber,firstPosition)
    set lastPosition $logLink($linkNumber,lastPosition)
    set readfileSize $logLink($linkNumber,readfileSize)
    
    # nothing to do if empty file
    if {$readfileSize == 0 } { return }
    
    set tmp [expr  $lastPosition - $firstPosition]
    set firstFraction [expr ($firstPosition+$tmp*$s1)/$readfileSize]
    set secondFraction [expr ($firstPosition+$tmp*$s2)/$readfileSize]
    $s set $firstFraction $secondFraction
}

# moves and/or change the buffer when changing position of a scrollbar
proc sbChanged { linkNumber s1 {s2 0} {s3 ""} {KeyPr 0} } {
   
    global logLink
    set t [getLinkWindowName $linkNumber].logFrame.log
       
    set firstPosition $logLink($linkNumber,firstPosition)
    set lastPosition $logLink($linkNumber,lastPosition)
    set readfileSize $logLink($linkNumber,readfileSize)
    # nothing to do if empty file
    if {$readfileSize == 0 } { return }
    
    set fr1 [lindex [$t yview] 0]
    set fr2 [lindex [$t yview] 1]
    set fr1Pos [expr $fr1*($lastPosition-$firstPosition)+$firstPosition] 
    set fr2Pos [expr $fr2*($lastPosition-$firstPosition)+$firstPosition] 
    set pageSize 1600.0
    
    if { $s1 == "scroll" } {

	if { [expr $s2<0 && [expr $fr1Pos+$s2*2*$pageSize]<$firstPosition]} {
	    prependBuffer $linkNumber [expr round($firstPosition-($fr1Pos+$s2*3*$pageSize) ) ]
	}
	if { [expr $s2>0 && [expr $fr2Pos+$s2*$pageSize]>$lastPosition]} {
	    appendBuffer $linkNumber [expr round($fr2Pos+$s2*3*$pageSize-$lastPosition)]
	}
	if { $KeyPr == 0 } {
	    $t yview $s1 $s2 $s3
	    $t mark set insert @0,0
	}
    }
    
    if { $s1 == "moveto" } {
	set movetoPosition [expr $s2*$readfileSize]

	if { $movetoPosition <= 0 } {
	    goTop $linkNumber
	    return
	} 
	if { $movetoPosition >= $readfileSize } {
	    goBottom $linkNumber
	    return
	}
	if { [expr $movetoPosition-$pageSize] <= 0 } {
	    set logLink($linkNumber,firstPosition) 0
	    set visiblePos $movetoPosition
	} else {
	    set logLink($linkNumber,firstPosition) [expr round($movetoPosition-$pageSize)]
	    set visiblePos $pageSize
	}
	
	set logLink($linkNumber,lastPosition) $logLink($linkNumber,firstPosition)
	
	deleteInWindow $t 0.0 end
	    
	appendBuffer $linkNumber 5000
	set newMove [expr $visiblePos/($logLink($linkNumber,lastPosition)-$logLink($linkNumber,firstPosition) )]

	$t yview $s1 $newMove
	$t mark set insert @0,0
    }
}

proc fileView { linkNumber fileName {title " "} } {

  set w [getLinkWindowName $linkNumber]
  toplevel $w
  wm title $w $title
  

  # Make a menu bar

  menu $w.mbar
  $w configure -menu $w.mbar

  $w.mbar add cascade -label File -menu $w.mbar.file
  makeEditMenu $w.mbar read-only
  #$w.mbar add cascade -label Help

  #pack $w.mbar.file $w.mbar.edit -side left
  #pack $w.mbar.help -side right

  menu $w.mbar.file 
    $w.mbar.file add command -label "Save log" \
      -command "saveLogToFile $w.logFrame.log"

  #pack $w.mbar -side top -fill x
 

  # The log field:
  
  frame $w.logFrame -relief raised -bd 1
  text $w.logFrame.log -relief sunken -bd 2 -wrap word \
    -yscrollcommand "textChanged $linkNumber" -bg white
  scrollbar $w.logFrame.logScrollbar  -relief sunken -jump 1 \
    -command "sbChanged $linkNumber"
  
  pack $w.logFrame.logScrollbar -side right -fill y
  pack $w.logFrame.log -fill both -expand 1 -padx 1m -pady 1m
  pack $w.logFrame -side top -fill both -expand 1
  

  # The buttons  
  button $w.dismiss -text "Dismiss" -command "destroy $w"
  button $w.goTop -text "Go Top" -command "goTop $linkNumber"
  button $w.goBottom -text "Go Bottom" -command "goBottom $linkNumber"
  button $w.search -text "Search" -command editSearch

  button $w.view -text [logGetViewButtonText $w.logFrame.log] -width 9 \
    -command "logChangeViewButton $w.logFrame.log $w.view"
	
  pack $w.dismiss $w.goTop $w.goBottom $w.search $w.view \
       -pady 2m -ipadx 5m -ipady 2 -side left -expand 1 

  bind $w.logFrame.log <Up> {
      regexp {(.file)([^.]*)} %W a b linkNumber
      sbChanged $linkNumber scroll -1 units 1
  }

  bind $w.logFrame.log <Down> {
      regexp {(.file)([^.]*)} %W a b linkNumber
      sbChanged $linkNumber scroll 1 units 1
  }

  bind $w.logFrame.log <Next> {
      regexp {(.file)([^.]*)} %W a b linkNumber
      sbChanged $linkNumber scroll 1 pages 1     
  }

  bind $w.logFrame.log <Prior> {
      regexp {(.file)([^.]*)} %W a b linkNumber
      sbChanged $linkNumber scroll -1 pages 1     
  }

  bind $w.logFrame.log <Home> {
      regexp {(.file)([^.]*)} %W a b linkNumber
      goTop $linkNumber  
  }

  bind $w.logFrame.log <End> {
      regexp {(.file)([^.]*)} %W a b linkNumber
      goBottom $linkNumber  
  }
}


proc fv { } {

  set w .a
  toplevel $w

  frame $w.logFrame -relief raised -bd 1
  text $w.logFrame.log -relief sunken -bd 2 -wrap word \
    -yscrollcommand "$w.logFrame.logScrollbar set" -bg white
  scrollbar $w.logFrame.logScrollbar -relief sunken \
    -command "$w.logFrame.log yview"
  
  pack $w.logFrame.logScrollbar -side right -fill y
  pack $w.logFrame.log -fill both -expand 1 -padx 1m -pady 1m
  pack $w.logFrame -side top -fill both -expand 1
  
}


# ----------------------------- File handlers ------------------------------- #


proc handleFreeIsPartOfBasis { linkNumber fileName } {

    fileView $linkNumber $fileName \
	{Details for the problem "Is part of free basis ?"}
}

proc updateFreeIsPartOfBasis { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}


proc handleCyclicDecomposition { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Generators of the canonical decomposition"
}

proc updateCyclicDecomposition { windowName {message ""} {tagList ""} } {

    logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleTTProblem { linkNumber fileName } {

    fileView $linkNumber $fileName {Other presentations}
}

proc updateTTProblem { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handlePrimaryDecomposition { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Generators of the primary decomposition"
}

proc updatePrimaryDecomposition { windowName {message ""} {tagList ""} } {

    logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleEquationBasicSolutions { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Basic solutions for the quadratic equation"
}

proc updateEquationBasicSolutions { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleEquationRegStabGenerators { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"RegStab generators for the quadratic equation"
}

proc updateEquationRegStabGenerators { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleEquationRandomSolutions { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"A list of sample random solutions for the quadratic equation"
}

proc updateEquationRandomSolutions { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}


proc handleNielsenBasis { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Nielsen basis for subgroup of free group"
}

proc updateNielsenBasis { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleNielsenBasisDetails { windowName fileName } {

    fileView $windowName $fileName \
	"Computation of Nielsen basis for subgroup of free group"
}

proc updateNielsenBasisDetails { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}


proc handleWhiteheadDecomposition { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Whitehead decomposition of automorphism of free group"
}

proc updateWhiteheadDecomposition { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}
proc handleNGstructure { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Finitely presented nilpotent group"
}

proc updateNGstructure { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleFNGBasicCommutators { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Basic commutators of free nilpotent group"
}

proc updateFNGBasicCommutators { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}
proc handleNGSubgroupBasis { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Basis of a subgroup"
}

proc updateNGSubgroupBasis { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}
proc handleNGNCgenerators { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Generators of the subgroup"
}

proc updateNGNCgenerators { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}
proc handleNilpotentBasis { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Basis of nilpotent group"
}

proc updateNilpotentBasis { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}
proc handleFNGStructureConstants { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Presentation of nilpotent group"
}

proc updateFNGStructureConstants { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleRelatorEnumerator { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Relator enumerator of a subgroup"
}

proc updateRelatorEnumerator { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleConsequencesEnumerator { windowName fileName } {

    fileView $windowName $fileName \
	"Enumeration of relator's consequences for one-relator group"
}

proc updateConsequencesEnumerator { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleORWordProblem { windowName fileName } {

    fileView $windowName $fileName \
	"Word problem in one-relator group"
}

proc updateORWordProblem { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleOREltDeco { windowName fileName } {

    fileView $windowName $fileName \
	"Word decomposition in one-relator group"
}

proc updateOREltDeco { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

# ------------------ Links for Automatic Structure ------------------------- #


proc handleAutWordAcceptor { windowName fileName } {

    fileView $windowName $fileName \
	"Word Acceptor"
}

proc updateAutWordAcceptor { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleAutDiffMachine1 { windowName fileName } {

    fileView $windowName $fileName \
	"The First Difference Machine"
}

proc updateAutDiffMachine1 { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleAutDiffMachine2 { windowName fileName } {

    fileView $windowName $fileName \
	"The Second Difference Machine"
}

proc updateAutDiffMachine2 { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleAutGeneralMultiplier { windowName fileName } {

    fileView $windowName $fileName \
	"General Multiplier"
}

proc updateAutGeneralMultiplier { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}


proc handleGeneticWP { windowName fileName } {

    fileView $windowName $fileName \
	"Genetic algorithm for solving word problem"
}

proc updateGAEquationProblem { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleGAEquationProblem { windowName fileName } {

    fileView $windowName $fileName \
	"Genetic algorithm for solving equations"
}

proc updateTwoCommProblem { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleTwoCommProblem { windowName fileName } {

    fileView $windowName $fileName \
	"Genetic algorithm for solving two commutators problem"
}

proc updateGeneticWP { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleSchreierTransversal { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Schreier representatives"
}

proc updateSchreierTransversal { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handlePermutationRepresentation { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Permutation Representation"
}

proc updatePermutationRepresentation { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleApproximationsRS-process { linkNumber fileName } {

    fileView $linkNumber $fileName \
        "Approximations RS-process"
}

proc updateApproximationsRS-process { windowName {message ""} {tagList ""} } {
 
   logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleToddCoxeterRS-process { linkNumber fileName } {

    fileView $linkNumber $fileName \
        "ToddCoxeter RS-process"
}

proc updateToddCoxeterRS-process { windowName {message ""} {tagList ""} } {
 
   logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleSchreierGens { linkNumber fileName } {

    fileView $linkNumber $fileName \
        "Schreier generators"
}

proc updateSchreierGens { windowName {message ""} {tagList ""} } {
 
   logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleCommutators { linkNumber fileName } {

    fileView $linkNumber $fileName \
        "Commutators rewriting process"
}

proc updateCommutators { windowName {message ""} {tagList ""} } {
 
   logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleSquares { linkNumber fileName } {

    fileView $linkNumber $fileName \
        "Squares rewriting process"
}

proc updateSquares { windowName {message ""} {tagList ""} } {
 
   logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleFastCommutators { linkNumber fileName } {

    fileView $linkNumber $fileName \
        "Commutators rewriting process"
}

proc updateFastCommutators { windowName {message ""} {tagList ""} } {
 
   logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleFastSquares { linkNumber fileName } {

    fileView $linkNumber $fileName \
        "Squares rewriting process"
}

proc updateFastSquares { windowName {message ""} {tagList ""} } {
 
   logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleDehnTransformations { linkNumber fileName } {

    fileView $linkNumber $fileName \
        "Dehn's transformations"
}

proc updateDehnTransformations { windowName {message ""} {tagList ""} } {
 
   logScrollAppend $windowName.logFrame.log $message $tagList
}


proc handleMSCInfo { linkNumber fileName } {

    fileView $linkNumber $fileName "Information about MSC-property"
}

proc updateMSCInfo { windowName {message ""} {tagList ""} } {
 
   logScrollAppend $windowName.logFrame.log $message $tagList
}

# ---------------------- Links for Enumerators -------------------

proc handleREnumerator { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Relators of a group"
}

proc updateREnumerator { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleWEnumerator { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Words of a group"
}

proc updateWEnumerator { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleSGEnumerator { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Subgroups of a group"
}

proc updateSGEnumerator { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleSubgroupContainmentDetails { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Details of containment problem"
}

proc updateSubgroupContainmentDetails { windowName {message ""} {tagList ""} } {
  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleWordInSubgroupGenerators { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Word expressed in a generators of a subgroup"
}

proc updateWordInSubgroupGenerators { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleRandomAutomorphisms { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Automorphisms for free group"
}

proc updateRandomAutomorphisms { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleRandomHomomorphisms { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Homomorphisms"
}

proc updateRandomHomomorphisms { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleTotalHomomorphisms { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Totally enumerated homomorphisms"
}

proc updateTotalHomomorphisms { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleGroupPresentation { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Presentation of a group"
}

proc updateGroupPresentation { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleIsTrivialDetails { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Details of is a group trivial problem" 
}

proc updateIsTrivialDetails { windowName {message ""} {tagList ""} } {
  logScrollAppend $windowName.logFrame.log $message $tagList
}


proc handleIsAbelianDetails { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Details of is a group abelian problem" 
}

proc updateIsAbelianDetails { windowName {message ""} {tagList ""} } {
  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleWordProblemDetails { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Details of word problem"
}

proc updateWordProblemDetails { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleACConjecture { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Find Andrews-Curtis minimal"
}

proc updateACConjecture { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handlePackage { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Package details"
}

proc updatePackage { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleWhiteheadMinimal { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Find a Whitehead minimal"
}

proc updateWhiteheadMinimal { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleRipsConstruction { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"The relators of the Rips construction"
}

proc updateRipsConstruction { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}


proc handleAbEqSysSol { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Finding solutions for the system"
}

proc updateAbEqSysSol { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleAbEqSol { linkNumber fileName } {

    fileView $linkNumber $fileName \
	"Finding solutions for the equation"
}

proc updateAbEqSol { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleIntersection { linkNumber fileName } {
	
    fileView $linkNumber $fileName \
	"Finding intersection of two subgroups"
}

proc updateIntersection { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}


proc handleGACPforORG { linkNumber fileName } {

    fileView $linkNumber $fileName \
	{Details for the problem "Are words conjugates ?"}
}

proc updateGACPforORG { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}


proc handleGAWPforORG { linkNumber fileName } {

    fileView $linkNumber $fileName \
	{Details for the problem "Is Word Trivial?"}
}

proc updateGAWPforORG { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}


proc handleIsWordAPE { linkNumber fileName } {

    fileView $linkNumber $fileName \
	{Details for the problem "Is word APE ?"}
}

proc updateIsWordAPE { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}


proc handleRankOfSubgroup { linkNumber fileName } {

    fileView $linkNumber $fileName \
	{Details for the problem "Rank?"}
}

proc updateRankOfSubgroup { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handleACE { linkNumber fileName } {

    fileView $linkNumber $fileName \
	{Output of ACE}
}

proc updateACE { windowName {message ""} {tagList ""} } {

  logScrollAppend $windowName.logFrame.log $message $tagList
}

proc handlePreAbelianRepProblem { linkNumber fileName } {

    fileView $linkNumber $fileName \
	{Details for the problem "Find a pre-Abelian Representation"}
}
