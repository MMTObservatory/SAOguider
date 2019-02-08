
source $env(SAOGUIDER_ROOT)/software/tcl/msg.tcl

set camera_host [lindex [split $env(GUIDECAM) :] 0]
set        host $env(HOST)

set ::ShmCopy 0

if { [string compare $camera_host $host] } {
    package require critcl
    package require shm
    package require bob

    msg_client GUIDECAM
    set ::ShmCopy 1
}

proc RecvImage { config shmkey frame server sock msgid ack size } {
    set sock [msg_sock GUIDECAM]
    set tran [fconfigure $sock -translation]

    fconfigure $sock -translation binary
    bob::read [set ::Shm$shmkey] 0 $size $sock
    fconfigure $sock -translation $tran

    upvar #0 NeedRTConfig$frame NeedRTConfig
    global DoRTUpdate

    if { $config } {
	DoRTConfig $frame
	set NeedRTConfig 0
    } else {
        RTImageUpdate $frame
    }
}

proc ReadImage { frame { config 0 } } {
    upvar #0 ${frame}_Image img

    upvar #0 Image${img}_ShmKeys shmkeys

    foreach key $shmkeys {
	if { ![info exists ::Shm$key] } {
	    set id [shm::new $key [expr 10000 + 1024*1024] 0666]
	    if { $id == -1 } { 
		set id [shm::get $key]
	    }
	    if { $id == -1 } { 
		error "Cannot create or get shared memory : $key"
	    }
	    set ::Shm$key [shm::att $id]
	}

    }

    msg_blk GUIDECAM Image$img [expr 10000 + 1024*1024] 5000 async "RecvImage $config $shmkeys $frame"
}


proc BoxDialog { frame id } {
}

proc CircleDialog { frame id } {
}

proc setglobal {var value} {
    upvar #0 $var V
    set V $value
}

proc boxmarkers { frame args } {
    if { ![$frame has fits] } { return }

    global BoxActive 
    global BoxBin  BackgroundWidth 

    upvar #0 ${frame}_Box box

    $frame marker delete all

    upvar #0 Box${box}_Image          img
    upvar #0 Image${img}_ImageSizePix ImageSizePix
    upvar #0 Box${box}_BoxSizePix     BoxSizePix
    upvar #0 Box${box}_CenID          cenid
    upvar #0 Box${box}_XOff           xo
    upvar #0 Box${box}_YOff           yo

#set CamBin 1
    upvar #0 Image${img}_Camera       cam
    upvar #0 Camera${cam}_Binning     CamBin

    global BoxSize ImageSize

    set pixsize [expr $ImageSize/$ImageSizePix]

  catch {
    set xx     [expr ($ImageSizePix/$BoxBin)/2 + 0.5]
    set yy     [expr ($ImageSizePix/$BoxBin)/2 + 0.5]
    set cx     [expr ($ImageSizePix/$BoxBin)/2 + 0.5 + $xo/$CamBin]
    set cy     [expr ($ImageSizePix/$BoxBin)/2 + 0.5 + $yo/$CamBin]
    set width  [expr $BoxSize/(2*$pixsize)]
    set backg  [expr $ImageSizePix - 2 * $BackgroundWidth]

    $frame pan to image $cx $cy

    set boxid [ $frame marker create cross point $cx $cy 	\
	    edit = no select = no color = green]
    set bxxid [ $frame marker create circle $cx $cy $width 	\
	    edit = yes select = no color = green]
    $frame marker $boxid move to image $cx $cy
    $frame marker $bxxid move to image $cx $cy
    $frame marker $boxid property move no
    $frame marker $bxxid property move no

    if { !$BoxActive($box) } { return }

    set bkgid [ $frame marker create box $cx $cy $backg $backg 0 \
	    edit = yes select = no color = red ]
    set cenid [ $frame marker create circle $cx $cy [expr $width-2] \
	    edit = no  select = no color = cyan ]

    $frame marker $bkgid move to image $xx $yy
    $frame marker $cenid move to image $xx $yy

    $frame marker $bkgid property move no
  }
}


proc boxresize { frame id } {
    if { ![$frame has fits] } { return }

    upvar #0 ${frame}_Box box
    upvar #0 Box${box}_Image          img
    upvar #0 Image${img}_ImageSizePix ImageSizePix
    upvar #0 Box${box}_BoxSizePix     BoxSizePix
    global BoxSize ImageSize

    set pixsize [expr $ImageSize/$ImageSizePix]

    set size [$frame get marker $id circle radius pixels]

    set BoxSize [expr 2*$size*$pixsize]

    global marker
}


proc bkgresize { frame id } {
    if { ![$frame has fits] } { return }

    upvar #0 ${frame}_Box box
    upvar #0 Box${box}_Image          img
    upvar #0 Image${img}_ImageSizePix ImageSizePix
    upvar #0 Box${box}_BoxSizePix     BoxSizePix
    global BackgroundWidth
    global BoxSize ImageSize

    set pixsize [expr $ImageSize/$ImageSizePix]

    set size [$frame get marker $id circle radius pixels]

    set BackgroundWidth [expr int($size - $BoxSize/(2*$pixsize))]

    if {$BackgroundWidth < 2} {
	set BackgroundWidth 2
    }

}
    
proc movemarker { box args } {
    global	DontSet
    upvar #0 Box${box}_X x
    upvar #0 Box${box}_Y y

    upvar #0 Box${box}_MarkerID id
    upvar #0 Box${box}_FullFrame frame

    set DontSet 1

    if { ![$frame has fits] } { return }

    catch { $frame marker $id move to physical $x $y }
    set DontSet 0
}

proc movecenmarker { box args } {
    global BoxBin
    upvar #0 Box${box}_Image          img
    upvar #0 Image${img}_ImageSizePix ImageSizePix
    upvar #0 Box${box}_CenX x
    upvar #0 Box${box}_CenY y
    upvar #0 Box${box}_CenID id
    upvar #0 Box${box}_Frame frame
    upvar #0 Box${box}_X x0
    upvar #0 Box${box}_Y y0
    upvar #0 Box${box}_XOff xo
    upvar #0 Box${box}_YOff yo

    #upvar #0 Image${img}_Camera       cam
    #upvar #0 Camera${cam}_Binning     CamBin

#puts $CamBin

    if { ![$frame has fits] } { return }

    #set center [expr ($ImageSizePix/$BoxBin)/2 + 0.5]
    set xx [expr $x + $x0 + $xo + 0.5]
    set yy [expr $y + $y0 + $yo + 0.5]
    catch { $frame marker $id move to physical $xx $yy }
}

proc makemarker { box args } {
    upvar #0 Box${box}_X x
    upvar #0 Box${box}_Y y
    
    global BoxActive fullbin
    
    upvar #0 Box${box}_MarkerID id

    upvar #0 Box${box}_FullFrame frame
    upvar #0 Box${box}_Image img
    upvar #0 Box${box}_BoxSizePix width
    set rad [expr $width/2]
    set rad 15

    if { ![$frame has fits] } { return }

    if {  ![info exists id] } {
	$frame update
	set id [$frame marker create circle $x $y $rad fixed = no edit = no]
	$frame marker $id move to physical $x $y
	$frame marker $id callback move moved \"Box${box}_X Box${box}_Y $frame\"
    }

    return

    if {  $BoxActive($box) && ![info exists id] } {
	$frame update
	set id [$frame marker create circle $x $y $rad fixed = no edit = no]
	$frame marker $id move to physical $x $y
	$frame marker $id callback move moved \"Box${box}_X Box${box}_Y $frame\"
    }
    if { !$BoxActive($box) &&  [info exists id] } {
	$frame marker unselect all
	$frame marker $id select
	$frame marker delete
	unset id
    }

    # Make boundries for the guide boxes
    # GENERALIZE THIS
#	set border 55
#	set xdim 2148
#	set ydim 4608

#	set glassy1 2054
#	set glassy2 2510

#	set x1 [expr          $fullbin/2+$border]
#	set x2 [expr $xdim-  ($fullbin/2+$border)]

#	if { $box == 1 } {
#	    set y1 [expr          $fullbin/2+$border]
#	    set y2 [expr $glassy1-$fullbin/2-$border]
#	} else {
#	    set y1 [expr $glassy2+$fullbin/2+$border]
#	    set y2 [expr $ydim   -$fullbin/2-$border]
#	}

#	set cx [expr ($x1+$x2)/2]
#	set cy [expr ($y1+$y2)/2]
#	set xwidth [expr $x2-$x1]
#	set ywidth [expr $y2-$y1]

#	set bigboxid [$frame marker create box $cx $cy $xwidth $ywidth \
#			edit = no move = no select = no color = red]
#        $frame marker $bigboxid move to detector $cx $cy
}

proc UpdateBoxMarkers { box } {
	upvar #0 Box${box}_Frame frame
	boxmarkers $frame
	makemarker $box
}

proc UpdateAllMarkers { args } {
    global NumberOfBoxes

    for { set box 1 } { $box <= $NumberOfBoxes } { incr box } {
	UpdateBoxMarkers $box
    }
}

proc MakeFrames { } {
    global NumberOfImages NumberOfBoxes GuideRowCols FrameList
    global FrameRowList
    global current
    set maxrow 0
    for { set img 1 } { $img <= $NumberOfImages } { incr img } {
	upvar #0 Image${img}_Grid gridpos
	set row [lindex $gridpos 0]
	set col [lindex $gridpos 1]
	if {$row > $maxrow} {
	    set maxrow $row
	}
	if {![info exists maxcol($row)]} {
	    set maxcol($row) $col
	} elseif { ($col > $maxcol($row))} {
	    set maxcol($row) $col
	}
	set g($row,$col) $img
    }

    set frm 1
    set GuideRowCols ""
    for {set row 1} {$row <= $maxrow} {incr row} {
	for {set col 1} {$col <= $maxcol($row)} {incr col} {
	    set frame Frame$frm
	    if { [info exists g($row,$col)] } {
		set img $g($row,$col)
		setglobal ${frame}_Image $img
		setglobal Image${img}_Frame $frame
		upvar #0  Image${img}_Camera cam
		upvar #0  Image${img}_GridZoom zoom
		upvar #0  Image${img}_GridRotation rotation
		upvar #0  Camera${cam}_Parity parity

		setglobal ${frame}_Camera $cam
		lappend FrameList $frame

		if { [info commands $frame] == "" } {
		    CreateFrame
		}
		set current(frame) $frame
		$frame zoom to $zoom
		set current(zoom) $zoom
		$frame rotate to $rotation
		$frame clip mode zscale
		global scale
		set scale(mode) zscale
		
		if {$parity == -1} {
		    $frame orient x
		    set current(orient) x
		} else {
		    $frame orient none
		    set current(orient) none
		}
	    }
	    incr frm
	}
	lappend GuideRowCols $maxcol($row)
    }

    # Correspondence between frames and boxes
    for { set box 1 } { $box <= $NumberOfBoxes } { incr box } {
	upvar #0 Box${box}_Image    iimg
	upvar #0 Image${iimg}_Frame  fframe
	upvar #0 Image${iimg}_FullImage fullimg
	upvar #0 Image${fullimg}_Frame fullframe
	setglobal ${fframe}_Box    $box
	setglobal Box${box}_Frame $fframe
	setglobal Box${box}_FullFrame $fullframe

	global ${fullframe}_BoxList
	lappend ${fullframe}_BoxList $box
    }
}

proc focusup { direction } {
    global focusamt
    catch {msg_cmd FOCUS "focuserr [expr $direction * $focusamt]" 10000}
}

proc tilt { axis direction } {
    global collamt tiltscale
    set teldir(x) 0
    set teldir(y) 0
    catch {msg_cmd FOCUS "tilt${axis}err [expr $direction * $collamt]" 10000}
    
    set teldir($axis) [expr $direction * $collamt * $tiltscale]
    msg_cmd TELESCOPE "azelrel $teldir(y) $teldir(x)" 10000
}

proc FrameRow { row frame args } {
    global FrameRowList
    
        set result ""

	switch [lindex $args 0] {
	 colorscale -
	 clip -
	 zoom { 
	     foreach f $FrameRowList($row) {
		 uplevel _$f $args
	     }
	 }
	 default {
		set result [uplevel _$frame $args]
	 }
	}

	return $result
}

proc RTUpdate { frame args } {
    global DoRTUpdate

	global  ds9

	if { $DoRTUpdate($frame) == 0 } {
	    set DoRTUpdate($frame) 1
	    after idle "DoRTUpdater $frame"
	} 
}


proc DoRTUpdater { frame } {
    upvar #0 NeedRTConfig$frame NeedRTConfig
    global DoRTUpdate

#    if { $NeedRTConfig == 0 } { incr NeedRTConfig }

    if { $NeedRTConfig > 0 } {
	incr NeedRTConfig -1
        set DoRTUpdate($frame) 0
        if { $NeedRTConfig > 0 } { return }

	if { $::ShmCopy } {
	    ReadImage $frame 1
	} else {
	    DoRTConfig $frame
	    set NeedRTConfig 0
	}
	return
    }

    if { $::ShmCopy } {
	ReadImage $frame
    } else {
        RTImageUpdate $frame
    }
}

set zscalefudge 1

proc RTImageUpdate { frame } {
    upvar #0 NeedRTConfig$frame NeedRTConfig
    global DoRTUpdate
    global ds9
    set DoRTUpdate($frame) 0

    upvar #0 ${frame}_Image img
    upvar #0 ${frame}_Camera cam
    upvar #0 Image${img}_GridRotate gridrotate
    if { $gridrotate } {
	global rot
	upvar #0 Image${img}_GridRotation gridrotation
	upvar #0 Camera${cam}_Rotation    camrotation
	upvar #0 Camera${cam}_Parity      camparity
	$frame rotate to [expr -($camparity)*($rot + $gridrotation + $camrotation)]
    } else {
	$frame update
    }
    $frame warp 0 0

    # The zscaling is recomputed by ds9 only if the parameters change 
    #   so we force a small change before calling clip
    global zscalefudge
    set params [$frame get clip zscale param]
    set zscontrast [lindex $params 0]
    set zssize [lindex $params 1]
    set zsline [lindex $params 2]
    set zssize [expr $zssize + $zscalefudge]
    set zscalefudge [expr $zscalefudge * -1]
    $frame clip zscale param $zscontrast $zssize $zsline

    if { ![set ::Image${img}_FullImage] } {
       UnsetWatchCursor
    }

    if { ![string compare $ds9(mode) crosshair] } {
	MotionGraphCrosshair $frame
    }
}

proc CheckBlocked { } {
    switch -regexp $::fltnname {
	.*gblk.* { 	
	    return [tk_messageBox -type okcancel  -icon info -message "The filter wheel is blocking the guide chip"] 
	}
    }
    return ok
}
proc chekmarker { args } {
	
}

proc rescale { } {
    global FrameList

    foreach frame $FrameList {
	RTConfig $frame
    }
}

proc resetscale { } {
    global current
    global scale
    global colorbar
    global buttons

    set frame $current(frame)

    $frame clip mode zscale
    set scale(mode) zscale

    $frame colorscale linear
    set scale(type) linear

    $buttons(name).color1.grey invoke
}


# This needs to be genericized.
#
proc RTConfig { frame args } {
        upvar #0 NeedRTConfig$frame NeedRTConfig
	global DoRTUpdate

	set  NeedRTConfig 1

	if { $DoRTUpdate($frame) == 0 } {
	    set DoRTUpdate($frame) 1
	    after idle "DoRTUpdater $frame"
	}
	return
}

proc DoRTConfig { frame args } {
    upvar #0 ${frame}_Image img
    upvar #0 Image${img}_ShmKeys shmkeys
    upvar #0 Image${img}_ShmSize shmsize
    upvar #0 Image${img}_Name    name
    upvar #0 ${frame}_Box        box
    upvar #0 ${frame}_BoxList    boxlist

    if {[info exists boxlist]} {
	foreach box $boxlist {
	    global Box${box}_MarkerID 
	    catch {unset Box${box}_MarkerID}
	}
    }

    if { [catch {
	if {[llength $shmkeys] > 1} {
	    $frame unload
	    set nkey [llength $shmkeys]
	    foreach shmkey $shmkeys {
	        $frame load fits mosaic iraf "\"$name\"" shared key $shmkey
	    }
	} else {
	    $frame unload
	    $frame load fits "\"$name\"" shared key $shmkeys
	}
	    
    	if { [info exists box] } {
	    after 5000 UpdateBoxMarkers $box
	}
    } reply] } {
	puts $reply
    }
}

proc moved { x y frame id } {
	if { $::DontSet } return

	global $x
	global $y

	set coords [$frame get marker $id center physical]

	set $x [lindex $coords 0]
	set $y [lindex $coords 1]
}

proc erroralert { args } {
	global error
	switch -glob $error {
	OK {}
	default {
		tk_messageBox -type ok -icon error -message $error
	        msg_set GUIDE error "OK"
	}
	}
}

# Update the menu widgets whenever ActiveBoxList msg subscription is updated

# ActiveBoxList msg subscription callback
#
proc setboxes { name indx op } {
    global NumberOfBoxes BoxActive
    upvar $name boxlist

    set ::DontBox 1

catch {
    for { set i 1 } { $i <= $NumberOfBoxes } { incr i } {
	set active [expr [lsearch -exact $boxlist $i] != -1]

	if {  $active  && $BoxActive($i) == 0 } {
	    set BoxActive($i) 1 
	} 
	if { !$active  && $BoxActive($i) == 1 } {
	    set BoxActive($i) 0
	}
    }
}

    set ::DontBox 0
}

# Send out a new ActiveBoxList whenever the menu is used
# Invoked by traces on BoxActive($i)
#
proc UpdateActiveBoxList { box name indx op } {
    UpdateBoxMarkers $box

    if { $::DontBox } { return }

    global NumberOfBoxes ActiveBoxList BoxActive
    set locallist ""

    for { set i 1 } { $i <= $NumberOfBoxes } { incr i } {
	if  { $BoxActive($i) } {
	    lappend locallist $i
	}
    }
    set ActiveBoxList $locallist
    msg_set GUIDE ActiveBoxList $ActiveBoxList 5000
}
	
proc DoubleMarker 	{which x y} {}
proc DeleteMarker 	{which x y} {}
proc SelectAllMarker 	{} {}
proc UnselectAllMarker 	{} {}
proc DeleteAllMarker 	{} {}
proc UnselectAllMarkers {} {}

proc ButtonMarker {which x y} {
    global marker
    global current

    # if nothing is loaded, abort

    set marker(motion) {}
    set marker(handle) {}

    if {![$current(frame) has fits]} {
	return
    }

    # see if we are on a handle

    set h [$which get marker handle $x $y]
    set id [lindex $h 0]
    set marker(handle) [lindex $h 1]

    if {$marker(handle)} {
	$which marker $id edit begin $marker(handle)
	set marker(motion) beginEdit
	return
    }

    # else, see if we are on a marker

    if {[$which get marker id $x $y]} {
	$which marker select $x $y
	$which marker move begin $x $y
	set marker(motion) beginMove
	UpdateMarkerMenu
	return
    }

    # see if any markers are selected

    if {[$which get marker select number]>0} {
	$which marker unselect all
	set marker(motion) none
	return
    }

}


