##
##   $Id: greeting.tcl 96 2006-03-24 17:59:50Z ybryukhov $
##

## Copyright (C) 1995 The New York Group Theory Cooperative
## See magnus/doc/COPYRIGHT for the full notice.

## Contents: Code supporting the greeting window.
##
## Principal Author: Roger Needham, Dmitry Bormotov
##
## Status: In progress.
##
## Revision History:
##
## 03/96 Dmitry B. made porting to tcl7.4/tk4.0 
##

proc makeGreeting {} {
  global tcl_path magnus_home

  # Put the greeting stuff in the root widget.

  wm title . "Magnus"
  wm iconname . "Magnus"
  #db wm minsize . 450 400
  #@rn Test screen size
  wm geometry . +10+20


  # Put in menu bar

  menu .mbar -relief raised -bd 2
  . configure -menu .mbar

  .mbar add cascade -label File -menu .mbar.file
  #menubutton .mbar.file -text File -menu .mbar.file -padx 1m
  makeEditMenu .mbar
  .mbar add cascade -label Preferences -menu .mbar.prefs
  .mbar add cascade -label Credits -menu .mbar.credits
  .mbar add cascade -label Help -menu .mbar.help

#  pack .mbar.file .mbar.edit .mbar.prefs -side left -padx 1m
#  pack .mbar.help -side right -padx 1m
#  pack .mbar.credits -side right -padx 1m

  menu .mbar.file 
    .mbar.file add command -label "Open..." -state disabled
    .mbar.file add command -label "Save..." -state disabled
    .mbar.file add command -label "Save As..." -state disabled
    .mbar.file add separator
    .mbar.file add command -label "Save Session Log..." \
      -command "saveLogToFile .frame3.log"
    .mbar.file add separator
#@db    .mbar.file add command -label Quit -command fakeQuit
    .mbar.file add command -label Quit -command {
      eval toSessionManager "\"[get_messageTemplate quit]\""
    }

  menu .mbar.prefs
    .mbar.prefs add checkbutton -label {Novice mode} -variable noviceMode

    .mbar.prefs add cascade -label {Font size} -menu .mbar.prefs.fontsz

    menu .mbar.prefs.fontsz
      addFontSizeMenu Tiny 100 80
      addFontSizeMenu Small 120 100
      addFontSizeMenu Normal 140 120
      addFontSizeMenu Big 180 140
      addFontSizeMenu Huge 180 180
      addFontSizeMenu Gigantic 240 240

    .mbar.prefs add cascade -label {Colors} -menu .mbar.prefs.colors
    menu .mbar.prefs.colors
      .mbar.prefs.colors add radiobutton -label Barbados \
        -value Barbados -variable theColorSet -command switchColors
      .mbar.prefs.colors add radiobutton -label Siberia \
        -value Siberia -variable theColorSet -command switchColors
      .mbar.prefs.colors add radiobutton -label "Custom..." \
        -value Custom -variable theColorSet -command colorPicker \
        -state disabled

  menu .mbar.credits
    .mbar.credits add command -label "Contributors" \
      -command "showHelp {Contributors::Contributors}"

    .mbar.credits add command -label "Acknowledgments" \
      -command "showHelp {Acknowledgments::Acknowledgments}"

  menu .mbar.help
    .mbar.help add command -label "The Manual" -command "showHelp"
    .mbar.help add separator
    .mbar.help add command -label "About Magnus" \
      -command "showHelp {aboutMagnus::About Magnus}"
    .mbar.help add command -label "Release Notes" \
      -command "showHelp {releaseNotes::Release Notes}"
    .mbar.help add separator
    .mbar.help add command -label "Show copyright" \
      -command "makeHelp $magnus_home/doc/COPYRIGHT Copyright {Copyright}"
    .mbar.help add command -label "Show GNU General Public License" \
      -command "makeHelp $magnus_home/doc/COPYING GPL \
       {GNU General Public License}"
    .mbar.help add separator
    .mbar.help add command -label "Contacting the Authors" \
      -command "showHelp {authors::Contacting the Authors}"


  #pack .mbar -side top -fill x


  # Put in greeting

  frame .frame -relief raised -bd 2
  if { [file exists "$tcl_path/bitmaps/greeting.xbm"] } {
    label .frame.label -bitmap @$tcl_path/bitmaps/greeting.xbm
  }
  pack .frame.label -side left -ipadx 5m
  pack .frame -side top -fill x


  # Make the session log

  frame .frame3 -relief raised -bd 1
  frame .frame3.frame -relief raised -bd 1
  label .frame3.frame.label -text "Session Log" -relief flat
  text .frame3.log -height 30 -width 50 -relief flat \
       -wrap word -yscrollcommand ".frame3.s set" -highlightthickness 0
  scrollbar .frame3.s -relief sunken -command ".frame3.log yview"
  pack .frame3.frame.label -side left -fill x
  pack .frame3.frame -side top -fill x
  pack .frame3.s -side right -fill y
  pack .frame3.log -fill both -expand 1 -padx 1m -pady 1m
  pack .frame3 -side top -fill both -expand 1


  frame .frame4 -relief raised -bd 1

  button .frame4.goTop -text "Go Top" -command ".frame3.log yview 0"
  button .frame4.goBottom -text "Go Bottom" -command ".frame3.log yview end"
  button .frame4.search -text "Search" -command editSearch

  button .frame4.view -text [logGetViewButtonText .frame3.log] -width 9 \
    -command "logChangeViewButton .frame3.log .frame4.view"

  pack .frame4.goTop .frame4.goBottom .frame4.search .frame4.view \
       -pady 2m -ipadx 5m -ipady 2 -side left -expand 1 
  pack .frame4 -side top -fill x


  wm protocol . WM_DELETE_WINDOW {
    alert "Please do not try to delete the session log!"
  }
}


proc addFontSizeMenu {label cs hs} {
  if {[fontExists -Adobe-Helvetica-Bold-R-Normal-*-$hs-*] == 1 && \
      [fontExists *-Courier-medium-r-normal--*-$cs-*-*-*-*-*-*] == 1} {
    .mbar.prefs.fontsz add radiobutton -label $label \
      -value $label -variable fontSize -command "changeFontSize $cs $hs" \
      -font -Adobe-Helvetica-Bold-R-Normal-*-$hs-*
  }
}
