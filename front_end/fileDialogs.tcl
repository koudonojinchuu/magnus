##
##   $Id: fileDialogs.tcl 9 2005-01-23 05:27:26Z daly $
##

## Copyright (C) 1994 The New York Group Theory Cooperative
## See magnus/doc/COPYRIGHT for the full notice.

## Contents: Code supporting open and save file dialogs
##
## Principal Author: Jon Lennox
##
## Status: Useable
##
## Revision History:
##
## 4/95 rn adapted Jon's version.

# Usage:
#
## openfile <dialog path>
#
# Returns the full pathname of the selected file, or "" if the dialog
# is cancelled.
#
## savefile <dialog path>
#
# Returns the full pathname of the selected file, or "" if the dialog
# is cancelled. It asks for confirmation of overwrite if the selected
# file exists.


# Since certain widgets don't take proper commands, but just a miserable
# `command prefix', we have to pass some parameters through globals.
# It's pretty fucking fragile, but we use the global name `w' for the
# relevant widget path, on the theory that the dialog is modal, and no
# one else would be insane enough to use that name.
# The globals file_returnval and file_cmd are also used for inter-proc
# communication.

proc file_scrollscrollbar {a b {c ""} } {

    global w
    
    if { $c != "" } {

	$w.lists.filelist yview $a $b $c
	$w.lists.aspectlist yview $a $b $c

    } else {

	$w.lists.filelist yview $a $b
	$w.lists.aspectlist yview $a $b
    }
}


proc file_scrollfilelist {c d} {
  global w

  $w.lists.scroll set $c $d
  $w.lists.aspectlist yview moveto $c
}

proc file_scrollaspectlist {c d} {
  global w

  $w.lists.scroll set $c $d
  $w.lists.filelist yview moveto $c
}

proc file_dirname {dirslist num} {
  set answer [lindex $dirslist $num]
  if {$answer == {}} {set answer /}
  return $answer
}

proc file_menu_update {} {
  global w

  set current_path [pwd]
  set current_dirs [split $current_path /]
  if {$current_dirs == {{} {}}} {set current_dirs {{}}}
  $w.lists.dirs configure -text \
       [file_dirname $current_dirs [expr [llength $current_dirs] - 1]]
  $w.lists.dirs.menu delete 0 last
  for {set i [expr [llength $current_dirs]-1]} {$i >= 0} {incr i -1} {
      set newpath [join [lrange $current_dirs 0 $i] /]
      if {$newpath == {}} {set newpath /}
      $w.lists.dirs.menu add command -label [file_dirname $current_dirs $i] \
          -command "file_chdir \"$newpath\""
  }
}

proc file_chdir {dir} {
    global w

    cd $dir
    file_menu_update
    $w.lists.filelist delete 0 end
    $w.lists.aspectlist delete 0 end
    if {[catch {set filelist [lsort [glob *]]}]} {set filelist {}}
    foreach file $filelist {
        $w.lists.filelist insert end $file
        $w.lists.aspectlist insert end "[file type $file]"
    }
    $w.lists.filelist selection set 0
    $w.lists.aspectlist selection set 0
}

proc file_setuplist {} {
    global w

    frame $w.lists    

    scrollbar $w.lists.scroll -relief sunken \
        -command "file_scrollscrollbar"
    listbox $w.lists.filelist -yscroll "file_scrollfilelist" \
        -relief sunken -width 15 -height 10 -exportselection false
    listbox $w.lists.aspectlist -yscroll "file_scrollaspectlist" \
        -relief sunken -width 11 -height 10 -exportselection false

    foreach event {<Button-1> <B1-Motion> <Shift-1> <Shift-B1-Motion>} {
        bind $w.lists.filelist $event {
            set pos [$w.lists.filelist nearest %y]
            #@db $w.lists.filelist select from $pos
            $w.lists.filelist selection set $pos
            #@db $w.lists.aspectlist select from $pos
            $w.lists.aspectlist selection set $pos
        }
    }

    foreach event {<Button-1> <B1-Motion> <Shift-1> <Shift-B1-Motion>} {
        bind $w.lists.aspectlist $event {
            set pos [$w.lists.aspectlist nearest %y]
            #@db $w.lists.filelist select from $pos
            $w.lists.filelist selection set $pos
            #@db $w.lists.aspectlist select from $pos
            $w.lists.aspectlist selection set $pos
        }
    }

    bind $w.lists.filelist <Double-Button-1> {
        set file [$w.lists.filelist get [$w.lists.filelist nearest %y]]
        file_open $file
    }

    bind $w.lists.aspectlist <Double-Button-1> {
        set file [$w.lists.filelist get [$w.lists.aspectlist nearest %y]]
        file_open $file
    }

    menubutton $w.lists.dirs -text {???} -menu $w.lists.dirs.menu \
        -relief raised
    menu $w.lists.dirs.menu

    pack append $w.lists \
       $w.lists.dirs {top pady 2m} \
       $w.lists.filelist {left} \
       $w.lists.aspectlist {left} \
       $w.lists.scroll {right expand filly}

    file_chdir [pwd]
}

proc file_open {file } {
    global file_returnval file_cmd

    if {[file isdirectory $file]} {
        cd "$file"
        file_chdir [pwd]
    } else {
        if {$file_cmd == "open"} {
            set file_returnval [pwd]/$file
        }
    }
}

proc file_save {file} {
    global file_returnval w seltype 

    set response 0
    if {[file exists $file]} {
        if {[file isdirectory $file]} {
            cd "$file"
            file_chdir [pwd]
        } else {
            set response [tk_dialog $w.d "Overwrite?" "Overwrite the file \
                $file?" question 1 OK Cancel]
        }
    }
    if {$response == 0} {set file_returnval [pwd]/$file}
}


proc openfile {win {title "Open..."}} {
    global file_returnval file_cmd w

    set w $win

    catch {destroy $w}
    toplevel $w -class Dialog
    wm title $w $title
    wm iconname $w Dialog

    set file_cmd open
    file_setuplist

    frame $w.buttons

    button $w.buttons.open -text Open -command {
        set file [$w.lists.filelist get [$w.lists.filelist curselection]]
        file_open $file
    }

    button $w.buttons.home -text Home -command {file_chdir ~}

    button $w.buttons.cancel -text Cancel -command {
        set file_returnval ""
    }

    frame $w.buttons.default -relief sunken -bd 1
    raise $w.buttons.open
    pack $w.buttons.open -in $w.buttons.default \
         -side top -padx 2m -pady 2m -ipadx 2m -ipady 2m

    bind $w <Return> {
        $w.buttons.open flash
        set file [$w.lists.filelist get [$w.lists.filelist curselection]]
        file_open $file
    }

    pack append $w.buttons \
        $w.buttons.default {top fillx expand pady 3m} \
        $w.buttons.home {top fillx expand pady 3m} \
        $w.buttons.cancel {top fillx expand pady 3m}

    pack append $w \
        $w.lists {left expand padx 3m pady 3m} \
        $w.buttons {right expand padx 3m pady 3m}

    set oldFocus [focus]
    grab $w
    focus $w

    wm withdraw $w
    update idletasks
    set x [expr [winfo screenwidth $w]/2 - [winfo reqwidth $w]/2 \
            - [winfo vrootx [winfo parent $w]]]
    set y [expr [winfo screenheight $w]/2 - [winfo reqheight $w]/2 \
            - [winfo vrooty [winfo parent $w]]]
    wm geom $w +$x+$y
    wm deiconify $w

    tkwait variable file_returnval
    destroy $w
    focus $oldFocus
    unset w
    return $file_returnval
}

proc savefile {win {title "Save..."} } {
    global file_returnval file_cmd w seltype

    set w $win

    catch {destroy $w}
    toplevel $w -class Dialog
    wm title $w $title
    wm iconname $w Dialog

    set file_cmd save
    file_setuplist

    frame $w.buttons

    button $w.buttons.save -text Save -command {
	if {$seltype == "dir"} {
	    set file_returnval [pwd]
	} else {
	    set file [$w.entry get]
	    if {$file == ""} {
		set file [$w.lists.filelist get [$w.lists.filelist curselection]]
		file_open $file
	    } else {
		file_save $file
	    }
	}
    }

    button $w.buttons.home -text Home -command {file_chdir ~}

    button $w.buttons.cancel -text Cancel -command {
        set file_returnval ""
    }

    frame $w.buttons.default -relief sunken -bd 1
    raise $w.buttons.save
    pack $w.buttons.save -in $w.buttons.default \
         -side top -padx 2m -pady 2m -ipadx 2m -ipady 2m

    bind $w <Return> {
        $w.buttons.save flash

	if {$seltype == "dir"} {
	    set file_returnval [pwd]
	} else {
	    set file [$w.entry get]
	    if {$file == ""} {
		set file [$w.lists.filelist get [$w.lists.filelist curselection]]
		file_open $file
	    } else {
		file_save $file
	    }
        }
    }

    entry $w.entry -relief sunken

    bind $w.entry <Return> {
        $w.buttons.save flash
	if {$seltype == "dir"} {
	    set file_returnval [pwd]
	} else {
	    set file [$w.entry get]
	    if {$file != ""} {
		file_save $file
	    }
        }
    }

    pack append $w.buttons \
        $w.buttons.default {top fillx expand padx 3m pady 3m} \
        $w.buttons.home {top fillx expand pady 3m} \
        $w.buttons.cancel {top fillx expand pady 3m}

#    pack append $w \
#        $w.buttons {right expand padx 3m pady 3m} \
#        $w.entry {bottom fillx padx 3m pady 3m} \
#        $w.lists {left expand padx 3m pady 3m}

    pack $w.buttons -side right -expand 1 -padx 3m -pady 3m
    pack $w.entry -side bottom -fill x -padx 3m -pady 3m
    pack $w.lists -side left -expand 1 -padx 3m -pady 3m

    set oldFocus [focus]
    grab $w
    focus $w.entry

    wm withdraw $w
    update idletasks
    set x [expr [winfo screenwidth $w]/2 - [winfo reqwidth $w]/2 \
            - [winfo vrootx [winfo parent $w]]]
    set y [expr [winfo screenheight $w]/2 - [winfo reqheight $w]/2 \
            - [winfo vrooty [winfo parent $w]]]
    wm geom $w +$x+$y
    wm deiconify $w

    tkwait variable file_returnval
    destroy $w
    focus $oldFocus
    unset w
    return $file_returnval
}

set seltype file
