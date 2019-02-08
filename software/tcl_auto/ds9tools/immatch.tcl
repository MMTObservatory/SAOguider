#!/data/mmti/pkg/ds9/devel/bin/wish8.0
#

proc immatch { } {
	global ds9
	global immatch

    if [winfo exists .immatch] {
	ToplevelRaise .immatch
    } else {
	set immatch(N) 		 0
	set immatch(selecting)  0
	set immatch(selected)	{}

        set w [toplevel .immatch]
    
	Grid [radiobutton $w.crosshair -text "Crosshair"\
		-variable ds9(mode) -value crosshair	\
		-command immatch_crosshair]		\
	     [radiobutton $w.move -text "Move Marker"	\
		-variable ds9(mode) -value editMarker	\
		-command immatch_move]			\
	     [button $w.help -text Help]
	Grid [button $w.markcross -text "Create Marker at Crosshair"	\
			-command "immatch_marker crosshair"]
	Grid [button $w.update -text "Save Points to File"		\
			-command immatch_save]
    }
}

proc immatch_crosshair { } {
	global ds9
	global crosshair

	set ds9(mode) crosshair
	set crosshair(lock) fk5


	if { [string compare $ds9(displaymode) tile] } {
	    set ds9(displaymode) tile
	    LayoutFrames
	}
}

proc immatch_move { } {

}


proc immatch_marker { where } {
	global immatch
	global ds9

    set created 1
    foreach frame $ds9(frames) {
	# If the crosshair is in the frame make a marker.
	#
	set width  [$frame get width]
	set height [$frame get width]

	set framepos [$frame get crosshair frame scientific]
	set frameX   [lindex $framepos 0]
	set frameY   [lindex $framepos 1]

	if { ![string compare $where "crosshair"] } {
	    set coord image
	    set imagepos [$frame get crosshair image scientific]
	    set imageX   [lindex $imagepos 0]
	    set imageY   [lindex $imagepos 1]
	} else {
	    set coord canvas
	    set imageX   [expr $width/2]
	    set imageY   [expr $height/2]
	}

	if { $frameX > 0 && $frameX < $width	\
	  && $frameY > 0 && $frameY < $height } {

	    set id [$frame marker create circle point $imageX $imageY $coord]
	    set immatch($frame,id,$immatch(N)) $id
	    $frame marker $id callback select immatch_select \"$immatch(N)\"
	    $frame marker $id callback delete immatch_delete \"$immatch(N)\"

	    set created 1
	} else {
	    set immatch($frame,id,$immatch(N)) {}
	}
    }
    if $created {
	immatch_select $immatch(N) 0
    	incr immatch(N)
    }
}

proc immatch_select { n id } {
	global ds9
	global immatch

	if  $immatch(selecting) return
	set immatch(selecting)  1

	set sel $immatch(selected)

        foreach frame $ds9(frames) {
	    if { $sel != {} && $immatch($frame,id,$sel) != {} } {
		$frame marker $immatch($frame,id,$sel) unselect
	    }
	    if {               $immatch($frame,id,$n)   != {} } {
		$frame marker $immatch($frame,id,$n)     select
	    }
	}

	set immatch(selected) $n
	set immatch(selecting) 0
}

proc immatch_delete { n id } {
	global ds9
	global immatch

	if  $immatch(selecting) return
	set immatch(selecting)  1

        foreach frame $ds9(frames) {
	    if { $immatch($frame,id,$n) != {} } {
		$frame marker $immatch($frame,id,$n) delete
		set $immatch($frame,id,$n) {}
	    }
	}

	set immatch(selected) {}
	set immatch(selecting) 0
}


proc immatch_save { } {
	global ds9
	global current
	global immatch


    set filename [FileSave PointFileType]
    if { ![string compare $filename {}] } {
	return
    }

    set fp [open $filename w]

    set refer $current(frame)

    set first 1

    foreach frame $ds9(frames) {
	if { ![string compare $refer $frame] } { continue }

    	for { set i 0; } { $i < $immatch(N) } { incr i } {
	    if { [string compare $immatch($frame,id,$i) {}] 	\
	      && [string compare $immatch($refer,id,$i) {}] } {
		break
	    }
	}

	if { $i < $immatch(N) } {
	    if { !$first } {
		puts "\f"
	        set first 0
	    }


	    puts $fp "DS9 Matched Points"
	    puts $fp ""
	    puts $fp "Refer	[$refer get fits full file name number 1]"
	    puts $fp "Image	[$frame get fits full file name number 1]"
	    puts $fp ""
	    puts $fp "RefX\tRefY\tRefRA\tRefDec\tX\tY\tRA\tDec"
	    puts $fp "----\t----\t-----\t------\t-\t-\t--\t---"

	    for { set i 0; } { $i < $immatch(N) } { incr i } {
		if { [string compare $immatch($frame,id,$i) {}] 	\
		  && [string compare $immatch($refer,id,$i) {}] } {
		    set ref $immatch($refer,id,$i)	
		    set mrk $immatch($frame,id,$i)	

		    set referpos [$refer get marker $ref center image]
		    set refX     [lindex $referpos 0]
		    set refY     [lindex $referpos 1]
		    set referpos [$refer get marker $ref center fk5 hms]
		    set refRA    [lindex $referpos 0]
		    set refDec   [lindex $referpos 1]

		    set imagepos [$frame get marker $mrk center image]
		    set imgX     [lindex $imagepos 0]
		    set imgY     [lindex $imagepos 1]
		    set imagepos [$frame get marker $mrk center fk5 hms]
		    set imgRA    [lindex $imagepos 0]
		    set imgDec   [lindex $imagepos 1]

		    puts $fp "$refX\t$refY\t$refRA\t$refDec\t$imgX\t$imgY\t$imgRA\t$imgDec"
		}
	    }
	}
    }

    close $fp
}

.menuBar.analysis add command -label "Image Matcher" -command immatch

