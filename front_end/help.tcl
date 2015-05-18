##
##   $Id: help.tcl 9 2005-01-23 05:27:26Z daly $
##

## Copyright (C) 1995 The New York Group Theory Cooperative
## See magnus/doc/COPYRIGHT for the full notice.

## Contents: Code supporting a hypertextual manual
##
## Principal Author: Roger Needham
##
## Status: Useable.
##
## Usage:
##
## A page spec is a string which matches {(.+)::(.+)}, where the first
## subexpression is the file name (without `.help'), and the second is
## the page title.
## 
## The main entry point is proc showHelp. Pass a page spec (the default
## is the root page) to bring up the manual window, or replace its
## contents.
## 
## There can be only one manual window. It would be easy to allow more
## than one, but then when the end user asks to look at the manual (say,
## by choosing a menu item), which window does he mean?
##
##
## Next implementation steps:
##
## * A tclsh script to check the integrity of all help files.
##
## * Key by text widget
##
## * Handle monochrome screens
##
## * Switch to TeX-like formatting of source
##
##
## Revision History:
##

##
## Globals which are `private' to this:
##

## helpPageLinkMap
#
# Type: array
# Keyed by a pair of text indices, this associates a range of text (the link)
# with the page spec of the link. Reset for each page.
#

## helpPageStack
#
# Type: list
# This is the backtrace of page specs of the pages the user has visited.
# Entry 0 is the most recent, and is always the current page.
#

## helpTraversal
#
# Type: array
# Keyed by page spec, associates each page with a list of page specs of
# the pages the user has jumped to from the key. All pages the user has
# visited appear as keys, possibly associated with an empty list.
#

## helpCurrentLink
#
# Type: string
# The page spec of the link the cursor is over, or "" if none.
#


## This is the heart of the help system. Given a page spec, it reads
## in the page, and sets the state of the (hard-wired) text widget to
## reflect the page's contents.

proc fetchHelpPage {pageSpec {pageTitle ""} {makeEdge 1}} {

  # We only make an edge in the graph when the user follows a link;
  # using the graph to jump around does not count, hence $makeEdge.

  global magnus_home tcl_path helpPageLinkMap helpPageStack helpTraversal

  set t .help.frame.frame1.text

  $t configure -cursor watch

  if { [info exists helpPageLinkMap] } {
    unset helpPageLinkMap
  }

  #@db
  if { [regexp { *tcl:(.+)} $pageSpec d command] != 0 } {
    eval $command
    return
  }

  if { [regexp {(.+)::(.+)} $pageSpec d file title] == 0 } {
    problemFetchingHelp "Malformed page specification: $pageSpec"
    return
  }
  # This is never actually triggered, since the `::' is put in when
  # a local link is resolved to a global one, below.

  set path "$tcl_path/help/$file.help"
  if { ! [file exists $path] || ! [file readable $path] } {
    problemFetchingHelp "File does not exist, or is not readable by you: $path"
    return
  }
  set fd [open $path r]
  # In Tcl, we have to just assume that this worked...

  # Search $path for $title

  set foundIt 0
  set lookingFor "@@$title"
  while { [gets $fd line] != -1 } {

    if { $line == $lookingFor } {

      set foundIt 1
      break
    }
  }

  if { ! $foundIt } {

    close $fd

    if { $file != "default" } {
	fetchHelpPage {probs/default::How this problem works}
    } else {
	problemFetchingHelp \
	    "There is no page named: $title\nin the file: $path"
    }
    
    return
  }

  set page ""

  # Cumulate lines until next title or eof

  while { [gets $fd line] != -1 } {

    if { [string index $line 0] == "@" && [string index $line 1] == "@" } {
      break
    }

    append page "\n" $line
  }

  close $fd

  # Format all whitespace, and protect escaped '@'s

  regsub -all \t $page " " page
  regsub -all "  +" $page " " page
  regsub -all " *\n *( *\n *)+" $page "\t\t  " page
  regsub -all " *\n *" $page " " page
  regsub -all \t $page \n page
  regsub -all {\\\@} $page \t page

  # Do formatting macros

  regsub -all {\\\s} $page " " page
  regsub -all { *\\ni *} $page "" page

  #@db
  if { $pageTitle != "" } {
      set title $pageTitle
  }

  set page "$title\n$page"

  # Find and reformat all links

  set tagIndices [list]
  # Used below for tagging links

  while { [regexp -indices {@([^@]+)@([^@]+)@} $page all idxs1 idxs2] } {

    set indices [list [lindex $all 0] [lindex $idxs1 1]]

    lappend tagIndices $indices

    set linkPageSpec [string range $page [lindex $idxs2 0] [lindex $idxs2 1]]

    if { ! [string match "*::*" $linkPageSpec] } {

      # Assume the link is local, and make it global
      set linkPageSpec "$file\:\:$linkPageSpec"
    }

    set helpPageLinkMap($indices) $linkPageSpec
    # Wrong kind of indices; will fix below

    regsub {@([^@]+)@([^@]+)@} $page {\1} page
  }

  # Put back protected '@'s

  regsub -all \t $page "@" page

  # Reset the state of the text widget

  $t configure -state normal
  $t delete 1.0 end
  $t tag delete title
  $t tag delete link
  $t insert 1.0 $page

  # Tag the title

  $t tag add title 1.0 1.0+[expr [string length $title] + 1]c

  # Tag all links

  foreach indices $tagIndices {

    set i [$t index 1.0+[lindex $indices 0]c]
    set j [$t index 1.0+[lindex $indices 1]c]

    $t tag add link $i $j

    # Have to adjust helpPageLinkMap, since text can't return raw indices

    set helpPageLinkMap($i,$j) $helpPageLinkMap($indices)
    unset helpPageLinkMap($indices)
  }

  # Bind tags

  $t tag configure title -font *-times-medium-r-normal--*-180-*-*-*-*-*-*
  #@rn Need to make compatible with size changes

  $t tag configure link -font *-times-medium-i-normal--*-140-*-*-*-*-*-*
  #@rn Need to make compatible with size changes

  $t tag bind link <Enter> { helpEnterLink %x %y }

  $t tag bind link <Leave> helpLeaveLink

  $t tag bind link <Button-1> helpFollowLink

  $t configure -cursor top_left_arrow -state disabled

  # Push this page onto the stack

  if { ! [info exists helpPageStack] } {
    set helpPageStack [list]
  }
  set helpPageStack [linsert $helpPageStack 0 $pageSpec]

  # Rebuild the Go menu completely

  set go .help.frame.mbar.go.menu
  $go delete 0 last
  set i 1
  set len [llength $helpPageStack]
  if { $len > 10 } { set len 10 }
  while { $i < $len } {
    regexp {.+::(.+)} [lindex $helpPageStack $i] d tit
    $go add command -label $tit -command "helpGoBack $i"
    incr i
  }

  # Enable/disable the Go menu and Go Back button, too

  if { $len > 1 } {
    .help.frame.frame2.gb configure -state normal
    .help.frame.mbar.go configure -state normal
  } else {
    .help.frame.frame2.gb configure -state disabled
    .help.frame.mbar.go configure -state disabled
  }

  # Record the traversal

  set graph_exists_p [winfo exists .help.frame2.c]
  set prev [lindex $helpPageStack 1]
  regexp {.+::(.+)} $prev d prev_title
  # The last two may have value ""

  if { ! [info exists helpTraversal($pageSpec)] } {

    # This is a page we have not seen yet.

    set helpTraversal($pageSpec) [list]
    # So every visited page shows up as a key

    if { $graph_exists_p } {
	graphMakeNode help $title "fetchHelpPage \{$pageSpec\} {} 0"
    }
    # The graph needs a new node, too
  }

  if { [llength $helpPageStack] > 1 } {

    # We have actually gone someplace

    if { $makeEdge && [lsearch -exact $helpTraversal($prev) $pageSpec] == -1 } {

      # The edge has not been recorded yet

      lappend helpTraversal($prev) $pageSpec

      if { $graph_exists_p } {
        graphMakeEdge help $prev_title $title
      }
    }
  }

  if { $graph_exists_p } {
    graphSelectNode help $title
  }
}


## This alerts the user that the help page could not be fetched (either
## to post the manual window initially, or to follow a link), but does
## nothing else to fix things.

proc problemFetchingHelp {msg} {

  .help.frame.frame1.text configure -cursor top_left_arrow

  alert "I could not follow the link. This is the problem:\n$msg"
}


## A text widget cannot say directly which tagged range of text a mouse
## click was in! So this laboriously searches for it. It returns a list
## {l1.c1 l2.c2} which is the range of the link text in which the point
## (x, y) falls.
## It returns "" if for some reason the range cannot be found.

proc helpGetLinkRange {x y} {

  set t .help.frame.frame1.text

  set i [$t index @$x,$y]
  set indices [$t tag ranges link]
  set j [llength $indices]

  while { $j > 0 } {

    set f [lindex $indices [expr $j - 2]]
    set l [lindex $indices [expr $j - 1]]
    if { [$t compare $f <= $i] && [$t compare $l > $i] } {
      return [list $f $l]
    }

    incr j -2
  }
  return ""
}


## Called when the mouse enters a link

proc helpEnterLink {x y} {

  global helpPageLinkMap helpCurrentLink helpTraversal

  set t .help.frame.frame1.text

  # Light up the text by making a new tag

  $t configure -cursor hand1
  set range [helpGetLinkRange $x $y]
  set f [lindex $range 0]
  set l [lindex $range 1]
  $t tag add current $f $l
  $t tag configure current -foreground MediumSeaGreen

  # Remember the link in case it is followed

  set helpCurrentLink $helpPageLinkMap($f,$l)

  # Light up the node if the link is to a page we have seen

  if { [winfo exists .help.frame2.c] &&
       [info exists helpTraversal($helpCurrentLink)] } {
    regexp {.+::(.+)} $helpCurrentLink d title
    graphActivateNode help $title
  }
}


## Called when the mouse leaves a link

proc helpLeaveLink {} {

  global helpCurrentLink helpTraversal

  set t .help.frame.frame1.text

  $t configure -cursor top_left_arrow
  $t tag configure current -foreground black
  $t tag delete current

  if { [winfo exists .help.frame2.c] &&
       [info exists helpTraversal($helpCurrentLink)] } {
    regexp {.+::(.+)} $helpCurrentLink d title
    graphDeActivateNode help $title
  }
}


## Called when a link is clicked on

proc helpFollowLink {} {

  global helpCurrentLink

  fetchHelpPage $helpCurrentLink
}


## When the user dismisses the manual window, we have to clean up some
## things to be polite.

proc helpQuit {} {

  global helpPageLinkMap helpPageStack

  if { [info exists helpPageLinkMap] } {
    unset helpPageLinkMap
  }
  if { [info exists helpPageStack] } {
    unset helpPageStack
  }
  if { [info exists helpTraversal] } {
    unset helpTraversal
  }

  #@rn What about:
  #@rn   destroyGraph help

  destroy .help
}


## This does $jump Go Backs.

proc helpGoBack {jump} {

  global helpPageStack

  set pageSpec [lindex $helpPageStack $jump]
  set helpPageStack [lreplace $helpPageStack 0 $jump]

  fetchHelpPage $pageSpec "" 0
}


## The main entry point for this stuff.

proc showHelp {{pageSpec "root::The Magnus Manual"} {title ""}} {

  set w .help

  if { [winfo exists $w] } {
    fetchHelpPage $pageSpec $title
    wm deiconify $w
    raise $w
    return
  }

  toplevel $w
  wm title $w "Magnus Manual"
  wm iconname $w "Magnus Manual"
#@rn  wm minsize $w 400 400

  frame $w.frame -relief flat

  # Make menu bar

  frame $w.frame.mbar -relief raised -bd 2

  menubutton $w.frame.mbar.go -text Go -menu $w.frame.mbar.go.menu \
    -state disabled
  menubutton $w.frame.mbar.search -text Search -menu $w.frame.mbar.search.menu \
    -state disabled

  pack $w.frame.mbar.go $w.frame.mbar.search -side left

  menu $w.frame.mbar.go.menu
  menu $w.frame.mbar.search.menu

  # Make text frame and buttons

  frame $w.frame.frame1 -relief raised -bd 2 -background OldLace 
  frame $w.frame.frame2 -relief raised -bd 2
  text $w.frame.frame1.text -relief flat -height 25 -width 50 -wrap word \
    -yscrollcommand "$w.frame.frame1.s set" -setgrid 1 -background OldLace \
    -cursor top_left_arrow -highlightthickness 0
  scrollbar $w.frame.frame1.s -relief sunken \
    -command "$w.frame.frame1.text yview"
  button $w.frame.frame2.gb -relief raised -bd 2 -text "Go Back" -width 12 \
    -state disabled -command "helpGoBack 1"
  button $w.frame.frame2.ok -relief raised -bd 2 -text "Dismiss" -width 12 \
    -command helpQuit
  button $w.frame.frame2.sg -relief raised -bd 2 -text "Show Graph" -width 12 \
    -command showHelpGraph
  pack $w.frame.mbar -side top -fill x
  pack $w.frame.frame1.s -side right -fill y
  pack $w.frame.frame1.text -side top -fill both -padx 2m -pady 1m -expand 1
  pack $w.frame.frame1 -fill both -expand 1
  pack $w.frame.frame2.gb -side left -expand 1 -padx 3m -pady 3m -ipady 1m
  pack $w.frame.frame2.ok -side left -expand 1 -padx 3m -pady 3m -ipady 1m
  pack $w.frame.frame2.sg -side left -expand 1 -padx 3m -pady 3m -ipady 1m
  pack $w.frame.frame2 -fill x
  pack $w.frame -side left -fill both -expand 1

  wm protocol $w WM_DELETE_WINDOW { helpQuit }

  # Fill in the text

  fetchHelpPage $pageSpec $title
}


## A helper of proc showHelp.

proc showHelpGraph {} {

  global helpPageStack helpTraversal

  set w .help
  set c $w.frame2.c

  # Make new frame for the canvas

  frame $w.frame2 -relief flat
  pack $w.frame2 -side left -fill both -expand 1

  # Make the canvas

  #@db porting to tk4.0
  #canvas $c -width 300 -scrollregion {0 0 1000 1000} \
  #       -xscroll "$w.frame2.hscroll set" -yscroll "$w.frame2.vscroll set"
  canvas $c -width 300 -scrollregion {0 0 1000 1000} \
         -xscrollcommand "$w.frame2.hscroll set" \
	 -yscrollcommand "$w.frame2.vscroll set"

  scrollbar $w.frame2.vscroll -relief sunken -command "$c yview"
  scrollbar $w.frame2.hscroll -orient horiz -relief sunken -command "$c xview"
  pack $w.frame2.vscroll -side right -fill y
  pack $w.frame2.hscroll -side bottom -fill x
  pack $c -fill both -expand 1

  $w.frame.frame2.sg configure -text "Hide Graph" -command hideHelpGraph

  # Set up the graph which is drawn in the canvas

  initializeGraph help $c 7

  # Make the nodes

  set keys [array names helpTraversal]
  # This variable always exists, since we can't show the graph until
  # we are looking at a page.

  if { [llength $keys] == 1 } {
    set source [lindex $keys 0]
    regexp {.+::(.+)} $source d tit
      graphMakeNode help $tit "fetchHelpPage \{$source\} {} 0" 50 20
  } else {
    foreach source $keys {
      regexp {.+::(.+)} $source d tit
	graphMakeNode help $tit "fetchHelpPage \{$source\} {} 0"
    }
  }

  # Make the edges

  foreach source $keys {

    foreach target $helpTraversal($source) {

      regexp {.+::(.+)} $source d tit1
      regexp {.+::(.+)} $target d tit2
      graphMakeEdge help $tit1 $tit2
    }
  }

  # Set the selection

  regexp {.+::(.+)} [lindex $helpPageStack 0] d tit
  graphSelectNode help $tit
}


## A helper of proc showHelp.

proc hideHelpGraph {} {

  set w .help
  destroy $w.frame2
  destroyGraph help
  $w.frame.frame2.sg configure -text "Show Graph" -command showHelpGraph
}
