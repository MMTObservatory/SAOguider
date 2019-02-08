# ds9 initilization file
#
option add *Entry.disabledBackground	[option get . Entry foreground]
option add *Entry.disabledForeground	[option get . Entry background]

option add *background  grey90
option add *foreground  black
#option add *font        default

#font create default -family Courier -size 14 -weight bold


if { [catch {
    rename bgerror {}
#    set auto_path [list $auto_path $env(SAOGUIDER_ROOT)/software/tcl_auto]
    lappend auto_path $env(SAOGUIDER_ROOT)/software/tcl_auto
    set env(MMTILIB) $env(SAOGUIDER_ROOT)/software/tcl_lib

    proc vformat { format name indx op } {
	    upvar $name var

	    if { [string compare $var "NaN"] } {
		set var [format $format $var]
	    }
    }


    set ::fltnname none

    if { ![string compare $env(inst) megacam] \
      || ![string compare $env(inst) megaguider] } {
        msg_client    TOPSERV
	msg_subscribe TOPSERV fltnname fltnname

	set FrameGroups {
	  { 1 2 }
	  { 3 4 }
	}
    } elseif { ![string compare $env(inst) az] \
      || ![string compare $env(inst) oldiccd85sm] \
      || ![string compare $env(inst) oldiccd200sm] \
      || ![string compare $env(inst) f5wfs_stella] \
      || ![string compare $env(inst) stellacam] } {
	set FrameGroups {
	  { 1 2 }
	  { 3  }
	}
    } else {
	set FrameGroups {
	  { 1 2 3 4 5 }
	  { 6 7}
	  { 8 9 10 }
	}
    }

    msg_client GUIDE
    msg_client FOCUS
    msg_client TELESCOPE


    set RX 350
    set RY   5
    set GX -300
    set BD   5

    Toplevel . "Guidegui"

    set X  300
    set Y    5
    set W  650
    set H  520

    SetWatchCursor
    update

    set sourcedir $env(SAOGUIDER_ROOT)/software/tcl

    set graphlen 50
    source $sourcedir/guideblt.tcl
    source $sourcedir/guideds9.tcl
    source $sourcedir/guideprocs.tcl
    source $sourcedir/Layout.tcl

    source $sourcedir/checkbutton.tcl
    source $sourcedir/tkcon.tcl
    $ds9(mb).file  insert 0 command -label TkConsole -command tkConInit

    msg_subscribe GUIDE  BoxBin          BoxBin
    msg_subscribe GUIDE  FullBin         fullbin
    msg_subscribe GUIDE  BoxSize         BoxSize
    msg_subscribe GUIDE  ImageSize       ImageSize
    msg_subscribe GUIDE  BackgroundWidth BackgroundWidth
    msg_subscribe GUIDE  NumberOfBoxes   NumberOfBoxes

    msg_subscribe GUIDE  NumberOfImages  NumberOfImages
    msg_subscribe GUIDE  NumberOfCameras NumberOfCameras
    msg_subscribe GUIDE  NumberOfErrors  NumberOfErrors
    msg_subscribe GUIDE  ActiveBoxList   ActiveBoxList    setboxes
    msg_subscribe GUIDE  FullImageList   FullImageList
    msg_subscribe GUIDE  TransferBoxList TransferBoxList

    msg_variable  GUIDE  send            send    w {} Up
    msg_variable  GUIDE  BoxSize         BoxSize w {} Up
    msg_variable  GUIDE  BackgroundWidth BackgroundWidth w {} Up

    msg_subscribe GUIDE  error       error      { erroralert      } 1
    for {set cam 1} {$cam <= $NumberOfCameras} {incr cam} {
	foreach item { Parity Rotation Split Binning } {
	    msg_subscribe GUIDE Camera${cam}_$item Camera${cam}_$item {}	.25
	}
    }

    for  { set bx 1 } { $bx <= $NumberOfBoxes } { incr bx } {
	set BoxActive($bx) 0
	trace variable BoxActive($bx) w "UpdateActiveBoxList $bx"

	msg_subscribe GUIDE Box${bx}_Counts Box${bx}_Counts { vformat %6.0f }	.25

	foreach item { Image BoxSizePix } {
	    msg_subscribe GUIDE Box${bx}_$item Box${bx}_$item {}	.25
	}
	foreach item { CenX CenY CenAz CenEl FWHM X Y XOff YOff Background Noise SigmaXY SigmaFWHM } {
	    msg_subscribe GUIDE Box${bx}_$item Box${bx}_$item {vformat %6.3f}	.25
	}
	foreach item { XOff YOff } {
	    trace variable Box${bx}_$item   w "boxmarkers Frame$bx"
	}

	# These extra traces drive the graphs
	#
	trace variable Box${bx}_FWHM         w "seterror Box${bx}FWHM"
	trace variable Box${bx}_Counts       w "seterror Box${bx}Counts"
    }

    for {set img 1} {$img <= $NumberOfImages} {incr img} {
	foreach item { ImageSizePix Grid     ShmKeys ShmSize GridRotate 
	               GridRotation GridZoom Name    Camera  FullImage } {
	    msg_subscribe GUIDE Image${img}_$item Image${img}_$item {}	.25
	}
    }

    
    for {set err 1} {$err <= $NumberOfErrors} {incr err} {
	foreach item { Raw } {
	    msg_subscribe GUIDE Error${err}_$item Error${err}_$item { vformat %6.3f }      .25
	}
	msg_subscribe GUIDE Error${err}_Name Error${err}_Name {}                    .25
	msg_subscribe GUIDE Error${err}      Error${err}      { vformat %6.3f }	    .25
	set Error${error}_Rawtotal 0
	set Error${error}total     0
    }
    
    msg_subscribe FOCUS focus focus
    msg_subscribe FOCUS tiltx tiltx
    msg_subscribe FOCUS tilty tilty
    
    msg_subscribe GUIDE rot rot

    MakeFrames

    set row 1
    foreach Group $FrameGroups {
        foreach f $Group {
            set frame Frame$f

            # Bind together zooming etc for all frames in a row
	    #
            lappend FrameRowList($row) $frame

            rename $frame _$frame
            proc $frame { args } [ subst {
                uplevel FrameRow $row $frame \$args
            } ]
        }
        incr row
    }


    foreach frame $FrameList {
	set NeedRTConfig$frame 0
	set DoRTUpdate($frame) 0
    }

    set DontSet 0
    set rot 0

    set guideexp 2

    set imagesize 250
    set BackgroundWidth 2

    Toplevel .guidercontrol "Guider Control" +1200+450
    imagecontrol .guidercontrol $NumberOfBoxes
    errorcontrol .guidercontrol
    ToplevelRaise .guidercontrol

    set tiltscale -0.25

    # Delete useless major buttons
    foreach button { frame bin file edit color1 zoom wcs1 bin1 } {
	pack forget $buttons(name).major.$button
    }

    # Delete useless scale options
    foreach scl { squared sqrt histogram minmax1 minmax2 minmax3 } {
	pack forget $buttons(name).scale.$scl
    }

    ButtonButton $buttons(name).scale.reset  "Reset Scale" {resetscale}

    # Add GUIDE buttons
    RadioButton $buttons(name).major.guide Guide buttons(majorCurrent) \
        $buttons(name).guide MajorButton

    frame $buttons(name).guide

    ButtonButton $buttons(name).guide.zoomin  "Zoom in"  "Zoom  2"
    ButtonButton $buttons(name).guide.zoomout "Zoom out" "Zoom .5"

    ButtonButton $buttons(name).guide.acquire "Full Acq"   { acquirefull	}
    ButtonButton $buttons(name).guide.boxes   "Box Acq"    { acquirebox		}
    ButtonButton $buttons(name).guide.rescale "ReScale"    { rescale		}
    if { [lsearch { spec chelle john } $env(USER)] >=0 } {
	ButtonButton $buttons(name).guide.transfer "Transfer"   { transfer		}
    } else {
	ButtonButton $buttons(name).guide.tweak   "Tweak"      { tweakboxes		}
    }
    if { [lsearch { john } $env(USER)] >=0 } {
	#ButtonButton $buttons(name).guide.tweak   "Tweak"      { tweakboxes		}
        ButtonButton $buttons(name).guide.tick    "Tick"       { tick		}
    }
    ButtonButton $buttons(name).guide.guidestate   "Guide Run"  { guidestartstop }
    ButtonButton $buttons(name).guide.camstate     "Camera Run" { camstartstop	 }

    # Make changes to the Info panel
    set info(frame) 0
    set info(filename) 0
    label $info(name).guideTitle -text "Guiding" -width 13
    label $info(name).guideValue -relief groove -bd 2 -width 13 -textvariable info(pGuide)
    label $info(name).guideLocked -relief groove -bd 2 -width 13 -textvariable info(pLocked)

    label $info(name).cameraTitle -text "Camera" -width 13
    label $info(name).cameraValue -relief groove -bd 2 -width 13 -textvariable info(pCamera)

    grid $info(name).guideTitle   $info(name).guideValue $info(name).guideLocked -sticky w
    grid configure $info(name).guideTitle -column 0
    grid configure $info(name).guideValue -column 2 -padx 2
    grid configure $info(name).guideLocked -column 4 -padx 2

    grid $info(name).cameraTitle  $info(name).cameraValue -sticky w 
    grid configure $info(name).cameraTitle  -column 0
    grid configure $info(name).cameraValue  -column 2 -padx 2
    UpdateInfoPanel
    
    # Add FOCUS buttons
    RadioButton $buttons(name).major.focus Focus buttons(majorCurrent) \
        $buttons(name).focus MajorButton

    frame $buttons(name).focus

    eval pack \
    [ labentry Focus     $buttons(name).focus.focus    -width 8 -state disabled -relief flat ]  \
    [ labentry Increment $buttons(name).focus.focusamt -width 6             ]  \
     -side left 
    ButtonButton $buttons(name).focus.up   "Up"   { focusup 1	}
    ButtonButton $buttons(name).focus.down "Down" { focusup -1  }
    ButtonButton $buttons(name).focus.rescale "ReScale"    { rescale		}

    set focusamt 20

    # Add COLLIMATE buttons
    RadioButton $buttons(name).major.coll Collimate buttons(majorCurrent) \
        $buttons(name).coll MajorButton

    frame $buttons(name).coll

    eval pack \
    [ labentry "Tilt X" $buttons(name).coll.tiltx    -width 6 -state disabled -relief flat ]  \
    [ labentry "Tilt Y" $buttons(name).coll.tilty    -width 6 -state disabled -relief flat ]  \
    [ labentry Increment $buttons(name).coll.collamt -width 6             ]  \
     -side left 
    ButtonButton $buttons(name).coll.px "+X" { tilt x  1 }
    ButtonButton $buttons(name).coll.mx "-X" { tilt x -1 }
    ButtonButton $buttons(name).coll.py "+Y" { tilt y  1 }
    ButtonButton $buttons(name).coll.my "-Y" { tilt y -1 }

    set collamt 20



    # Add MOVE buttons
    RadioButton $buttons(name).major.move Move buttons(majorCurrent) \
	$buttons(name).move MajorButton

    frame $buttons(name).move

    eval pack \
	[ labentry Increment $buttons(name).move.moveamt -width 6 ] -side left
    ButtonButton $buttons(name).move.pel "+El" { move_el 1 }
    ButtonButton $buttons(name).move.mel "-El" { move_el -1 }
    ButtonButton $buttons(name).move.paz "+Az" { move_az 1 }
    ButtonButton $buttons(name).move.maz "-Az" { move_az -1 }

    set moveamt 1.0


    proc move_el { direction } {
        global moveamt
	catch {msg_cmd GUIDE "ditherinst 0 [expr $direction * $moveamt]" 10000}
    }

    proc move_az { direction } {
        global moveamt
	catch {msg_cmd GUIDE "ditherinst [expr $direction * $moveamt] 0" 10000}
    }

    ### imageoffset functionality
    
    bind .ds9.image <Control-ButtonRelease-1> {
#	puts "imageoffset $info(pFileName) $info(pImageX) $info(pImageY)"
	catch {
	    msg_cmd GUIDE "imageoffset $info(pFileName) $info(pImageX) $info(pImageY) 1.0"
	}
    }
    bind .ds9.image <Shift-Control-ButtonRelease-1> {
#	puts "imageoffset $info(pFileName) $info(pImageX) $info(pImageY)"
	catch {
	    msg_cmd GUIDE "imageoffset $info(pFileName) $info(pImageX) $info(pImageY) 0.1"
	}
    }


    set buttons(majorCurrent) $buttons(name).guide
    MajorButton


    set ds9(display,mode) guide

    # Change the size of the canvas
    $canvas(name) configure -width [expr 512 + 8] -height [expr 512 + 8]

    # These subscriptions have callbacks that access the GUI
    #
    for { set img 1 } { $img <= $NumberOfImages } { incr img } {
	upvar #0 Image${img}_Frame fframe
	msg_subscribe GUIDE Image${img}_Config ${fframe}_up    "RTConfig $fframe"   0 10000
	msg_subscribe GUIDE Image${img}_Update ${fframe}_data  "RTUpdate $fframe"   0 10000
    }
    msg_subscribe GUIDE  send           send      { guiderunidlelabel  }
    msg_subscribe GUIDE  state          state     { camrunidlelabel  }
    msg_subscribe GUIDE  locked         locked    { camrunlocked     }

    set GuiGeometry [msg_get GUIDE GuiGeometry]

    mkgraphs [Toplevel .rgraphs "Raw Error Graphs"  +[expr $X + $W + $BD]+$RY] rawerr
    mkgraphs [Toplevel .xgraphs "Raw Total Graphs"  +[expr $X + $W + $BD]+$RY] rawtot
    mkgraphs [Toplevel .egraphs "Current Error Graphs" +[expr $X + $W + $BD]+$RY] err
    mkgraphs [Toplevel .tgraphs "Total Error Graphs"  +[expr $X + $W + $BD]+$RY] tot

    mkboxgraphs [Toplevel .cgraphs "Counts Graphs"  +[expr $X + $W + $BD]+$RY] Counts $NumberOfBoxes
    mkboxgraphs [Toplevel .fgraphs "FWHM Graphs"    +[expr $X + $W + $BD]+$RY] FWHM   $NumberOfBoxes

    menubar

    if { [lsearch { megacam minicam } $env(USER)] >=0 } {
	foreach button { guide focus coll } {
	    pack forget $buttons(name).major.$button
	    set buttons(majorCurrent) $buttons(name).scale
	    MajorButton
	}
	$ds9(mb) delete "Guide"
	wm withdraw .guidercontrol
    }

    set ActiveBoxList $ActiveBoxList
    if { $GuiGeometry != "" } {
	wm geometry . $GuiGeometry
    }
}] } {
    global errorInfo
    puts "error in guidegui.tcl init file."
    puts $errorInfo
    exit 1
}


