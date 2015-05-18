##
##   $Id: nodeGraph.tcl 9 2005-01-23 05:27:26Z daly $
##

## Copyright (C) 1995 The New York Group Theory Cooperative
## See magnus/doc/COPYRIGHT for the full notice.

## Contents: Code to support drawing and maintaining directed
##           graphs with labelled nodes drawn as circles.
##
## Principal Author: Roger Needham
##
## Status: Useable.
##
## Usage:
##
## You may have as many graphs as you like.
## 
## You set up a new graph by calling initializeGraph, passing a unique
## name, the widget path of the canvas in which the graph should be drawn
## (it must already exist), and optionally parameters like the node
## radius and various colors.
## 
## You make nodes and edges one at a time, by calling graphMakeNode and
## graphMakeEdge.
## 
## One node may be selected at any time. You indicate which with
## graphSelectNode.
## 
## When you are done with a graph, politely free the storage used by it
## with destroyGraph.
##
##
## Next implementation steps:
##
## * graphMakeEdgeNode (86 advice)
## * Allow more specs for graph, e.g., color, size.
## * Better node placement.
##   - Allow a graph to be built all at once, for reasonable node placement?
## * Allow (optional) labels on edges, too?
##
##
## Revision History:
##


##
## Globals which are `private' to this:
##

## graphData
#
# Type: array
# This stores all data for all graphs. The keys begin with the name of a
# graph, followed by a further specification of the data. Currently,
# these are:
#
# KEY                       DATUM
#
# ($name,canvas)            Path of canvas widget for the graph
# ($name,nodeRadius)        Radius of graph nodes
# ($name,curX)              Scratch data for dragging nodes
# ($name,curY)
# ($name,selection)         Canvas item # of selected node
# ($name,nodeX,$item)       X coord of canvas item # $item
# ($name,nodeY,$item)       Y coord of canvas item # $item
# ($name,edgeFirst,$item)   List of edges beginning at canvas item # $item
# ($name,edgeSecond,$item)  List of edges ending at canvas item # $item
# ($name,widget,$label)     Canvas item # corresponding to $label
# ($name,callBack,$item)    The click callback script for $item
# ($name,targetOf,$edge)    Canvas item # to which $edge points
#



## randSeed
#
set randSeed 2112
#
# Type: integer
#
proc rand {l h} {
  global randSeed
  set randSeed [expr ($randSeed * 1021 + 25673) % 121500]
  expr $l + (($h - $l + 1) * $randSeed) / 121500
}



proc initializeGraph {name canvas {nodeRadius 10}} {

  global graphData

  set graphData($name,canvas) $canvas
  set graphData($name,nodeRadius) $nodeRadius

  $canvas bind node <Any-Enter> "\
    $canvas configure -cursor hand1
    $canvas itemconfigure current -fill MediumSeaGreen"

  $canvas bind node <Any-Leave> "\
    $canvas configure -cursor top_left_arrow
    $canvas itemconfigure current -fill OldLace
    if \{ \[info exists graphData($name,selection)\] \} \{
      $canvas itemconfigure \$graphData($name,selection) -fill black
    \}"

  $canvas bind node <Button-1> "\
    eval \$graphData($name,callBack,\[$canvas find withtag current\])"

  $canvas bind node <Button-2> "\
    set graphData($name,curX) %x; set graphData($name,curY) %y
    $canvas configure -cursor fleur"

  $canvas bind node <B2-Motion> "\
    graphMoveNode $name \
      \[$canvas find withtag current\] \
      \[expr %x - \$graphData($name,curX)\] \
      \[expr %y - \$graphData($name,curY)\]
    set graphData($name,curX) %x
    set graphData($name,curY) %y"

  $canvas bind node <ButtonRelease-2> "\
    $canvas configure -cursor top_left_arrow"
}


proc destroyGraph {name} {

  global graphData

  foreach key [array names graphData] {
    if { [string match "$name,*" $key] } {
      unset graphData($key)
    }
  }
}


## `label' is the label of the node to select.

proc graphSelectNode {name label} {

  global graphData

  set c $graphData($name,canvas)

  if { [info exists graphData($name,selection)] } {
    $c itemconfigure $graphData($name,selection) -fill OldLace
  }

  set node $graphData($name,widget,$label)
  set graphData($name,selection) $node
  $c itemconfigure $node -fill black
}


proc graphActivateNode {name label} {

  global graphData

  $graphData($name,canvas) itemconfigure $graphData($name,widget,$label) \
    -fill MediumSeaGreen
}


proc graphDeActivateNode {name label} {

  global graphData

  $graphData($name,canvas) itemconfigure $graphData($name,widget,$label) \
    -fill OldLace
}


## `label' is the text with which to label the node.
## `callBack' is the script to execute when the user clicks on the node.
## `x' and `y' are the desired coords; both 0 means no preference.

proc graphMakeNode {name label callBack {x 0} {y 0}} {

  global graphData

  set c $graphData($name,canvas)
  set r $graphData($name,nodeRadius)

  if { $x == 0 && $y == 0 } {

    # Make 10 random tries for a non-overlapping position, then give up.

    set count 10
    while { $count } {

      set x [rand 10 200]
      set y [rand 10 200]

      if { [$c find overlapping [expr $x - $r] [expr $y - $r] \
                                [expr $x + $r + 20] [expr $y + $r]] == "" } {
        break
      }

      incr count -1
    }
  }

  set new [$c create oval [expr $x - $r] [expr $y - $r] \
                          [expr $x + $r] [expr $y + $r] \
                     -outline black -fill OldLace -tags node]

  $c create text [expr $x + $r + 5] [expr $y + 2] -text $label \
    -font "*courier-medium-r-normal--*-120*" -anchor w

  eval $c create rectangle [$c bbox [expr $new + 1]] \
    -fill BlanchedAlmond -outline BlanchedAlmond
  $c raise [expr $new + 1]

  set graphData($name,nodeX,$new) $x
  set graphData($name,nodeY,$new) $y
  set graphData($name,edgeFirst,$new) {}
  set graphData($name,edgeSecond,$new) {}
  set graphData($name,widget,$label) $new
  set graphData($name,callBack,$new) $callBack
}


## Pass the labels of the source and target nodes.

proc graphMakeEdge {name firstLabel secondLabel} {

  global graphData

  set c $graphData($name,canvas)
  set r $graphData($name,nodeRadius)
  set first $graphData($name,widget,$firstLabel)
  set second $graphData($name,widget,$secondLabel)
  set x1 $graphData($name,nodeX,$first)
  set y1 $graphData($name,nodeY,$first)
  set x2 $graphData($name,nodeX,$second)
  set y2 $graphData($name,nodeY,$second)

  set h [expr sqrt([expr ($x1 - $x2)*($x1 - $x2) + ($y1 - $y2)*($y1 - $y2)])]
  if {$h != 0} {
    set x2 [expr $x2 + $r * ($x1 - $x2) / $h]
    set y2 [expr $y2 - $r * ($y2 - $y1) / $h]
  }

  set edge [$c create line $x1 $y1 $x2 $y2 -arrow last]
  $c lower $edge
  lappend graphData($name,edgeFirst,$first) $edge
  lappend graphData($name,edgeSecond,$second) $edge
  set graphData($name,targetOf,$edge) $second
}


## This is for internal use, in dragging nodes.

proc graphMoveNode {name node dx dy} {

  global graphData

  set c $graphData($name,canvas)
  set r $graphData($name,nodeRadius)

  $c move $node $dx $dy
  $c move [expr $node + 1] $dx $dy
  $c move [expr $node + 2] $dx $dy
  incr graphData($name,nodeX,$node) $dx
  incr graphData($name,nodeY,$node) $dy

  foreach edge $graphData($name,edgeFirst,$node) {

    set x1 $graphData($name,nodeX,$node)
    set y1 $graphData($name,nodeY,$node)
    set x2 $graphData($name,nodeX,$graphData($name,targetOf,$edge))
    set y2 $graphData($name,nodeY,$graphData($name,targetOf,$edge))

    set h [expr sqrt([expr ($x1 - $x2)*($x1 - $x2) + ($y1 - $y2)*($y1 - $y2)])]
    if {$h != 0} {
      set x2 [expr $x2 + $r * ($x1 - $x2) / $h]
      set y2 [expr $y2 - $r * ($y2 - $y1) / $h]
    }

    $c coords $edge $x1 $y1 $x2 $y2
  }

  foreach edge $graphData($name,edgeSecond,$node) {

    set x1 [lindex [$c coords $edge] 0]
    set y1 [lindex [$c coords $edge] 1]
    set x2 $graphData($name,nodeX,$node)
    set y2 $graphData($name,nodeY,$node)

    set h [expr sqrt([expr ($x1 - $x2)*($x1 - $x2) + ($y1 - $y2)*($y1 - $y2)])]
    if {$h != 0} {
      set x2 [expr $x2 + $r * ($x1 - $x2) / $h]
      set y2 [expr $y2 - $r * ($y2 - $y1) / $h]
    }

    $c coords $edge $x1 $y1 $x2 $y2
  }
}
