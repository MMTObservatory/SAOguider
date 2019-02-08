
rename label _label 
proc label { w args } {
   eval _label [entFixWidgetName $w] $args
}

rename entry _entry
proc entry { w args } {
   eval _entry [entFixWidgetName $w] $args
}

rename checkbutton _checkbutton
proc checkbutton { w args } {
   eval _checkbutton [entFixWidgetName $w] $args
}

rename QuitDS9 _QuitDS9
proc QuitDS9 {} {
    _QuitDS9
}

proc acquirefull { } {
    if { [info exists ::acquirefull] } { return }
    if { [CheckBlocked] == "cancel"  } { return }

    set ::acquirefull 1
    
    if { [ catch { 
	global Exposure FullImageList Image1_Camera
	set img [lindex $FullImageList 0]

    

	upvar #0 Image${img}_Camera cam
	upvar #0 Camera${cam}_Binning binning
	upvar #0 Camera${cam}_Split split
    
#  This is minicam specific
	set timeout [expr 1000 * int (20 + (60 / ($split+1) / ($binning*$binning)) + $Exposure) + 25000]

	if { $::cursor(count) }  {
	    after 20000 { puts Timer:UnsetWatch; set ::$cursor(count) 0 ; UnsetWatchCursor } 
	    return
	}
	
	SetWatchCursor
	msg_cmd GUIDE stop
	msg_cmd GUIDE "expose full $Exposure" $timeout

    } error ] } {
puts acquire:UnsetWatch

 	UnsetWatchCursor
puts $error
    }
    unset ::acquirefull 
}

proc acquirebox { } {
    global Exposure

    msg_cmd GUIDE "expose box $Exposure" 30000
}

trace variable Exposure w setexptime 
proc setexptime { args } {
	set ::NeedRTConfigFrame1 1
	set ::NeedRTConfigFrame2 1
}

trace variable guideexp w setguideexp 
proc setguideexp { args } {
	set ::NeedRTConfigFrame3 1
}

proc tweakboxes { } {
	msg_cmd GUIDE "tweakboxes data" 60000
}

proc transfer { } {
    global TransferBoxList
    msg_cmd GUIDE "transfer 1 [lindex $TransferBoxList 0 ] 2 [lindex $TransferBoxList 1]]" 12000
}

proc tick { } {
	msg_cmd GUIDE tick 12000
}

proc guidestartstop { } {
	global send
        global state

    if { $send == 0 } {
	set send 1
	if { $state == 0} {
	    camstartstop
	}
    } else {
	set send 0
    }
}

proc camstartstop { } {
	global state send

    if { $state == 0 } {
	set ::NeedRTConfigFrame1 2
	set ::NeedRTConfigFrame2 2
	camerastart
    } else {
	msg_cmd GUIDE stop 10000
	set send 0
    }
}

proc camrunidlelabel { name indx op } {
	upvar $name value
	global buttons info

    if { $value == 0 } {
	$buttons(name).guide.camstate configure -text "Start Camera" -background red
	set info(pCamera) Stopped
    } else {
	$buttons(name).guide.camstate configure -text "Stop  Camera" -background green
	set info(pCamera) Running
    }
}

proc camrunlocked { name indx op } {
	upvar $name value
	global buttons info

    if { $value == 0 } {
	set info(pLocked) "NOT Locked"
	$info(name).guideLocked configure -background red
    } else {
	set info(pLocked) "    Locked"
	$info(name).guideLocked configure -background green
    }
}

proc guiderunidlelabel { name indx op } {
	upvar $name value
	global buttons info

    if { $value == 0 && [CheckBlocked]=="ok" } {
	$buttons(name).guide.guidestate configure -text "Start Guiding" -background red
	set info(pGuide) Off
    } else {
	$buttons(name).guide.guidestate configure -text "Stop  Guiding" -background green
	set info(pGuide) On
    }
}

proc guidebox { w n } {
    frame $w

    label $w.gui_l -text "Guide Box $n"
    Grid  $w.gui_l - 

    foreach item { X Y } {
	eval Grid [msgentry_async GUIDE $item $w.Box${n}_$item  {} Up "movemarker $n" {} nowait -width 6 ]
    }
    foreach item { CenX CenY } {
	eval Grid [msgentry_async GUIDE $item $w.Box${n}_$item  {} Up "movecenmarker $n" {} nowait -width 6 -state disabled -relief flat]
    }
    foreach item { FWHM CenAz CenEl Counts Background Noise SigmaXY SigmaFWHM } {
	eval Grid [msgentry_async GUIDE $item $w.Box${n}_$item  {} {} {}              {} {}     -width 6 -state disabled -relief flat]
    }

    return $w
}

proc guide_row {w errnum text} {
    set err "Error${errnum}"
    global	${err}_Active   \
	        ${err}_Raw	\
		${err}_Rawtotal	\
		${err}  	\
		${err}_Total	\
		${err}_Gain 	\
		${err}_Step 	\
		${err}_Sample 	\
		${err}_Lookb	\
		${err}_LockTol

    global ${err}_rawtotal
    set ${err}_rawtotal 0.0

    # These extra traces drive the graphs
    #
    trace variable ${err}           w "settotal ${err}"
    trace variable ${err}_Raw       w "settotal ${err}raw"

     msg_subscribe GUIDE ${err}_Active   ${err}_Active   {} 1
     msg_subscribe GUIDE ${err}_Gain     ${err}_Gain     {} 1
     msg_subscribe GUIDE ${err}_Step     ${err}_Step     {} 1
     msg_subscribe GUIDE ${err}_Sample   ${err}_Sample   {} 1
     msg_subscribe GUIDE ${err}_Offset   ${err}_Offset   {} 1
     msg_subscribe GUIDE ${err}_LookB    ${err}_LookB    {} 1
     msg_subscribe GUIDE ${err}_LockTol  ${err}_LockTol  {} 1

     msg_variable  GUIDE ${err}_Active   ${err}_Active   w {} Up
     msg_variable  GUIDE ${err}_Gain     ${err}_Gain     w {} Up
     msg_variable  GUIDE ${err}_Step     ${err}_Step     w {} Up
     msg_variable  GUIDE ${err}_Sample   ${err}_Sample   w {} Up
     msg_variable  GUIDE ${err}_Offset   ${err}_Offset   w {} Up
     msg_variable  GUIDE ${err}_LookB    ${err}_LookB    w {} Up
     msg_variable  GUIDE ${err}_LockTol  ${err}_LockTol  w {} Up

    return "							\
     [checkbutton $w.${err}_Active -variable ${err}_Active]     \
     [label $w.${err}_Label        -width 10 -text $text]                       \
     [entry $w.${err}_Rawerror     -width 10 -textvariable ${err}_Raw   -relief flat -state disabled]	\
     [entry $w.${err}_Error        -width 10 -textvariable ${err}       -relief flat -state disabled]	\
     [entry $w.${err}_Total        -width 10 -textvariable ${err}_Total -relief flat -state disabled]	\
     [retentry $w.${err}_Gain      -width 8 -justify right]	\
     [retentry $w.${err}_Step      -width 8 -justify right]	\
     [retentry $w.${err}_Sample    -width 8 -justify right]	\
     [retentry $w.${err}_Offset    -width 8 -justify right]	\
     [retentry $w.${err}_LookB     -width 8 -justify right]	\
     [retentry $w.${err}_LockTol   -width 8 -justify right]"
}

proc imagecontrol { ww n } {
    set w [frame $ww.f1 -relief groove -borderwidth 2]
    grid $w -sticky w

    eval Grid \
 	      [msgentry_async GUIDE "Guide Box Size"     $w.BoxSize        {} Up { UpdateAllMarkers } {} {} -width 6] 	\
	      [label $w.boxsizeunits -text "Arcsec"] 		

    eval Grid \
	      [msgentry_async GUIDE "Background Width" $w.BackgroundWidth  {} Up { UpdateAllMarkers } {} {}  -width 6] \
	      [label $w.backgroundunits -text "pixels"] 		

    eval Grid \
	      [msgentry_async GUIDE "Exposure Time" $w.Exposure {} Up {} {} {} -width 5]  \
	      [label $w.exposureunits -text "seconds"] 

    set w [frame $ww.f2 -relief groove -borderwidth 2]
    grid $w -sticky w

    set boxes {}
    for { set i 1 } { $i <= $n } { incr i } {
	lappend boxes [guidebox $w.box$i $i]
    }
    eval Grid $boxes 
}

proc errorcontrol { ww } {
    set w [frame $ww.f3 -relief groove -borderwidth 2]
    grid $w
    global NumberOfErrors
    Grid  [label $w.active   -text Active]      \
          [label $w.label    -text Error]	\
	  [label $w.rawerror -text Raw]		\
	  [label $w.error    -text Current]	\
	  [label $w.total    -text Total]	\
	  [label $w.gain     -text Gain] 	\
	  [label $w.step     -text Step]	\
	  [label $w.sample   -text Sample]	\
	  [label $w.offset   -text Offset]	\
	  [label $w.lookb    -text LookBack]	\
	  [label $w.lockt    -text LockTol]

    for {set err 1} { $err <= $NumberOfErrors } {incr err} {
	upvar #0 Error${err}_Name name
	eval Grid [guide_row $w $err $name]
    }
}


proc camerastart { } {
    global NumberOfErrors
    for {set err 1} { $err <= $NumberOfErrors } {incr err} {
	global Error${err}_Total 
	set    Error${err}_Total  "  0.000"
    }
    msg_cmd GUIDE start 50000
}

proc menubar { } {
	global ds9 
    global NumberOfBoxes

    #$ds9(mb) delete "Frame"
    $ds9(mb) delete "Edit"
    $ds9(mb) delete "Bin"
    $ds9(mb) delete "Analysis"

    menu $ds9(mb).optsmenu      -tearoff 0

    $ds9(mb) add cascade -menu $ds9(mb).optsmenu -label "Guide"

    menu $ds9(mb).optsmenu.active -tearoff 0
    
    $ds9(mb).optsmenu add command -label "Tick"		\
	-command { tick }
    $ds9(mb).optsmenu add cascade -label "Active Boxes" -menu $ds9(mb).optsmenu.active
    for {set i 1} {$i <= $NumberOfBoxes} {incr i} {
	$ds9(mb).optsmenu.active add checkbutton -label "Box $i" -variable BoxActive($i)
    }

    $ds9(mb).optsmenu add command -label "Guider Control"		\
	-command { ToplevelRaise .guidercontrol }
    $ds9(mb).optsmenu add command -label "Raw Error Graphs"			\
	-command { ToplevelRaise .rgraphs }
    $ds9(mb).optsmenu add command -label "Raw Total Graphs"			\
	-command { ToplevelRaise .xgraphs }
    $ds9(mb).optsmenu add command -label "Cur Error Graphs"			\
	-command { ToplevelRaise .egraphs }
    $ds9(mb).optsmenu add command -label "Cur Total Graphs"			\
	-command { ToplevelRaise .tgraphs }
    $ds9(mb).optsmenu add command -label "Counts Graphs"			\
	-command { ToplevelRaise .cgraphs }
    $ds9(mb).optsmenu add command -label "FWHM Graphs"			\
	-command { ToplevelRaise .fgraphs }
}
 
proc UpdateAnalysisMenu { } { }
proc UpdateBinMenu { } { }


