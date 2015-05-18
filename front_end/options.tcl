##
##   $Id: options.tcl 96 2006-03-24 17:59:50Z ybryukhov $
##

## Copyright (C) 1995 The New York Group Theory Cooperative
## See magnus/doc/COPYRIGHT for the full notice.

## Contents: Code supporting global settings of widget configurations.
##
## Principal Author: Roger Needham, Dmitry Bormotov
##
## Status: In progress.
##
## Revision History:
##
## 03/96 Dmitry B. made porting to tcl7.4/tk4.0
##

# In specifying colors, make sure these at least are covered:
#
# activeBackground
# activeForeground
# background
# disabledForeground
# foreground
# insertBackground
# selectBackground
# selectForeground
#
# in general, with possible specialization for these widget classes:
#
# Frame, Label, Button, Checkbutton, Radiobutton, Message, Listbox,
# Scrollbar, Scale, Entry, Menu, Menubutton.



proc setDefaultOptions {} {

   setFontSizeWin32 10

#@db porting to tk4.0
#  if { [tk colormodel .] != "monochrome" } { }
   if { [winfo depth .] != "1" } {
    setDefaultColors
  }
}


proc fontExists {font} {

  # Need to check whether the fonts exist in a roundabout way

  set b .fakeFontTestButton
  button $b -text dummy

  if {[catch {$b configure -font $font}]} {
    set result 0
  } else {
    set result 1
  }
  destroy $b
  return $result
}


proc setFontSize {cs hs} {

  set cf *-Courier-medium-r-normal--*-$cs-*-*-*-*-*-*
  if {[fontExists $cf] == 0} {
    alert "Your system lacks the font:\n$cf\nso I can't change sizes."
  } else {
    option add *Entry.font $cf
    option add *Text.font $cf
  }

  set hf -Adobe-Helvetica-Bold-R-Normal-*-$hs-*
  if {[fontExists $hf] == 0} {
    alert "Your system lacks the font:\n$hf\nso I can't change sizes."
  } else {
    # the following should all be commented out if on Mac OS X.
    option add *Menubutton.font $hf
    option add *Menu.font $hf
    option add *Label.font $hf
    option add *Button.font $hf
    option add *Radiobutton.font $hf
    option add *Checkbutton.font $hf
    option add *Listbox.font $hf
  }

  if {$hs < 180} {set hs 180}
  set tf -Adobe-Times-Medium-R-Normal-*-$hs-*
  if {[fontExists $tf] == 0} {
    alert "Your system lacks the font:\n$tf\nso I can't change sizes."
  } else {
    # the following should all be commented out if on Mac OS X.
    option add *Message.font $tf
  }
}

proc setFontSizeWin32 {sz} {

   option add *Entry.font [list Courier $sz]
   option add *Text.font [list Courier $sz]

   option add *Menubutton.font [list Helvetica $sz]
   option add *Menu.font [list Helvetica $sz]
   option add *Label.font [list Helvetica $sz]
   option add *Button.font [list Helvetica $sz]
   option add *Radiobutton.font [list Helvetica $sz]
   option add *Checkbutton.font [list Helvetica $sz]
   option add *Listbox.font [list Helvetica $sz]

   option add *Message.font [list Adobe-Times $sz]
}

proc changeFontSize {cs hs} {

  # Return geometry management of toplevels to wm
  foreach w [winfo children .] {
    if {[winfo class $w] == "Toplevel"} {
      wm geometry $w ""
    }
  }

  setFontSize $cs $hs
  changeFontSize_aux . $cs $hs

  workspace_changeNameFontSize $cs
  setHeaderTag
}

proc changeFontSize_aux {root cs hs} {

  foreach w [winfo children $root] {
    foreach i [$w configure] {
      if {"[lindex $i 0]" == "-font"} {
        catch { $w configure -font [newFont [lindex $i 4] $cs $hs] }
        break
      }
    }
    changeFontSize_aux $w $cs $hs
  }
}


proc newFont {font cs hs} {
  # Assumes the only font families are courier and Helvetica
  #@rn Does not work for messages (because the font is roman!!).
  if {[regexp -nocase courier $font]} {
    regsub {[0-9]+} $font $cs font
  } else {
    regsub {[0-9]+} $font $hs font
  }
  return $font
}


proc setDefaultColors {{prio "userDefault"}} {

  # The default prio really should be startupFile, so that .Xresources
  # or .Xdefaults work, but Jehova only knows what surreal defaults
  # some halfwit SysAdmin gave our customer...

  #@rn Look in /usr/lib/X11/rgb.txt to resolve numbers.

  option add *activeBackground bisque2 $prio
  option add *activeForeground black $prio
  option add *background BlanchedAlmond $prio
  option add *highlightBackground BlanchedAlmond $prio
  option add *disabledForeground grey60 $prio
  option add *foreground black $prio
  option add *insertBackground black $prio
  option add *selectBackground Grey $prio
  option add *selectForeground black $prio

  option add *selector MediumSeaGreen $prio
  option add *Scale.activebackground PeachPuff2 $prio
  option add *Scale.disabledForeground grey60 $prio
  option add *Scale.background BlanchedAlmond $prio
  option add *Scale.troughColor bisque2 $prio
  option add *Scale.highlightBackground BlanchedAlmond $prio
  option add *Scrollbar.background BlanchedAlmond $prio
  option add *Scrollbar.troughColor bisque2 $prio
  option add *Scrollbar.highlightBackground bisque2 $prio
  option add *Scrollbar.activebackground PeachPuff2 $prio

}


proc switchColors {} {
  global theColorSet

  switch -exact $theColorSet {
    Barbados {
      setDefaultColors interactive
      #@rn Need a better way to bind `option add' and switchColors_aux

      switchColors_aux . activebackground bisque2
      switchColors_aux . activeforeground black
      switchColors_aux . selector black
      switchColors_aux . background BlanchedAlmond
      switchColors_aux . highlightBackground BlanchedAlmond
      switchColors_aux . disabledforeground gray60
      switchColors_aux . foreground black
      switchColors_aux . insertbackground black
      switchColors_aux . selectbackground Grey
      switchColors_aux . selectforeground black

      switchColors_aux . selector MediumSeaGreen
      switchColors_aux . activeForeground PeachPuff2 Scale
      switchColors_aux . disabledForeground gray60 Scale
      switchColors_aux . sliderForeground BlanchedAlmond Scale
      switchColors_aux . background bisque2 Scale

      #@db porting to tk4.0
      switchColors_aux . highlightBackground bisque2 Scale

      switchColors_aux . foreground BlanchedAlmond Scrollbar
      switchColors_aux . background bisque2 Scrollbar

      #@db porting to tk4.0
      switchColors_aux . highlightBackground bisque2 Scrollbar

      switchColors_aux . activeforeground PeachPuff2 Scrollbar
    }

    Siberia {
      option add *activeBackground white
      option add *activeForeground black
      option add *selector black
      option add *background #efefef
      option add *foreground black
      option add *selectBackground #bfdfff
      option add *Scale.sliderForeground #dfdfdf
      option add *Scrollbar.foreground #dfdfdf
      option add *Scrollbar.activeForeground white
      option add *disabledForeground #7f7f7f

      switchColors_aux . activebackground white
      switchColors_aux . activeforeground black
      switchColors_aux . selector black
      switchColors_aux . background #efefef
      switchColors_aux . foreground black
      switchColors_aux . selectbackground #bfdfff
      switchColors_aux . sliderforeground #dfdfdf Scale
      switchColors_aux . foreground #dfdfdf Scrollbar
      switchColors_aux . activeforeground white Scrollbar
      switchColors_aux . disabledforeground #7f7f7f
    }
  }
}


proc switchColors_aux {root option value {class ""}} {

  foreach w [winfo children $root] {
    if {$class == "" || $class == "[winfo class $w]"} {
      catch { $w configure -$option $value }
    }
    switchColors_aux $w $option $value $class
  }
}


proc colorPicker {} {

  set w [uniqueDialogName]
  toplevel $w
  wm title $w "Color Picker"
  wm iconname $w "Color Picker"
  #@db wm minsize $w 583 208

  # Color listbox

  label $w.color_l -text "Available colors" -relief raised -bd 2
  listbox $w.colors -relief raised -bd 2 -yscrollcommand "$w.color_sb set" \
	  -selectmode single
  scrollbar $w.color_sb -command "$w.colors yview" -relief flat

  # Instructions

  message $w.instr -width 4i -text \
    "This is just to test geometry management. Nothing is hooked up yet."

  # Example widgets

  label $w.label -text Label -relief raised -bd 2
  button $w.button -text Button -relief raised -bd 2
  checkbutton $w.checkbutton -text Checkbutton -relief raised -bd 2
  radiobutton $w.radiobutton -text Radiobutton -relief raised -bd 2 \
    -state disabled
  scrollbar $w.scrollbar -relief sunken -orient horizontal \
    -command "$w.entry xview"
  scale $w.scale -orient horizontal
  entry $w.entry -relief sunken -width 10 \
    -xscrollcommand "$w.scrollbar set"
  $w.entry insert 0 Entry
  text $w.text -relief sunken -bd 2 -width 10 -height 2
  $w.text insert 1.0 Text

  # Restriction checkbutton

  label $w.cb_label -text "Apply color to"
  checkbutton $w.cb1 -text "active background" -relief flat
  checkbutton $w.cb2 -text "active foreground" -relief flat
  checkbutton $w.cb3 -text "background" -relief flat
  checkbutton $w.cb4 -text "disabled foreground" -relief flat
  checkbutton $w.cb5 -text "foreground" -relief flat
  checkbutton $w.cb6 -text "insert background" -relief flat
  checkbutton $w.cb7 -text "select background" -relief flat
  checkbutton $w.cb8 -text "select foreground" -relief flat

  # Scope radiobuttons

  label $w.rb_label -text "Make changes"
  radiobutton $w.local -text "To those items above which I click" -relief flat
  radiobutton $w.global -text "To all items at once" -relief flat

  # Control buttons

  button $w.undo -text Undo -width 8 -state disabled
  button $w.apply -text Apply -width 8 -state disabled
  button $w.cancel -text Cancel -width 8 -command "destroy $w"
  button $w.ok -text OK -width 8 -state disabled

  # Now pack all of these

  framer $w $w right {
    right {} {-fill y -expand 1 -anchor e} {
      top {} {-fill y -expand 1} {
        color_l {-fill x}
        right {} {-fill y -expand 1} {
          color_sb {-fill y}
          colors {-fill y -expand 1}
        }
      }
    }
    left {} {} {
      top {} {} {
        top {-relief raised -bd 2} {-fill both -expand 1} {
          instr {-pady 3m}
        }
        top {-relief raised -bd 2} {-fill both -expand 1} {
          left {} {-padx 2m -pady 2m} {
            label {-padx 2m}
            button {-padx 2m}
            checkbutton {-padx 2m}
            radiobutton {-padx 2m}
          }
          left {} {-padx 2m -pady 2m} {
            top {} {} {
              entry {-padx 2m}
              scrollbar {-padx 2m -fill x}
            }
            text {-padx 2m}
            scale {-padx 2m}
          }
        }
        left {} {-anchor w} {
          top {} {} {
            top {-relief raised -bd 2} {-fill x} {
              cb_label {}
            }
            top {-relief raised -bd 2} {-fill both -expand 1} {
              cb1 {-anchor w} cb2 {-anchor w} cb3 {-anchor w} cb4 {-anchor w}
              cb5 {-anchor w} cb6 {-anchor w} cb7 {-anchor w} cb8 {-anchor w}
            }
          }
          top {} {-anchor n -fill both -expand 1} {
            top {-relief raised -bd 2} {-fill x} {
              rb_label {}
            }
            top {-relief raised -bd 2} {-fill x} {
              local {-anchor w} global {-anchor w}
            }
            top {-relief raised -bd 2} {-fill both -expand 1} {
              left {} {-expand 1} {
                undo {-padx 3m -pady 4m -ipady 1m}
                apply {-padx 3m -pady 4m -ipady 1m}
              }
              left {} {-expand 1} {
                cancel {-padx 3m -pady 4m -ipady 1m}
                ok {-padx 3m -pady 4m -ipady 1m}
              }
            }
          }
        }
      }
    }
  }

#@db  tk_listboxSingleSelect $w.colors

  set file [open /usr/lib/X11/rgb.txt]
  while {[gets $file line] >= 0} {
    $w.colors insert end [lrange $line 3 end]
  }
  close $file
}
