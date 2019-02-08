
proc LayoutFrames {} {
    global ds9
    global current
    global canvas
    global tile
    global pan
    global mag

    global debug
    if {$debug(tcl,layout)} {
	puts "LayoutFrames"
    }

    set canvas(width)  [expr [winfo width $canvas(name)]  - 4]
    set canvas(height) [expr [winfo height $canvas(name)] - 4]

    # turn everything off

    foreach f $ds9(frames) {
	$f hide
	$f highlite off
	$f panner off
	$f magnifier off
	UnBindEventsFrame $f
    }

    set n [llength $ds9(active)]
    if {$n > 0} {
	$ds9(display,mode)Layout
    } else {
        set current(frame) ""
        set ds9(next) ""

        # panner
        if {$panner(view)} {
            panner clear
        }

        # magnifier
        if {$magnifier(view)} {
            magnifier clear
        }

        # process proper colorbar
        switch -- $ds9(visual) {
            pseudocolor {
                colorbar show
                set current(colorbar) colorbar
                set colorbar(map) [colorbar get name]
                set colorbar(invert) [colorbar get invert]
            }
            truecolor {
                colorbar show
                colorbarrgb hide
                set current(colorbar) colorbar
                set colorbar(map) [colorbar get name]
                set colorbar(invert) [colorbar get invert]
            }
        }

        # update menus/dialogs
        UpdateDS9

    }
}


proc FrameToFront {which} {
    global ds9
    global current
    global canvas
    global colorbar
    global panner
    global info
    global blink

    global debug
    if {$debug(tcl,layout)} {
        puts "FrameToFront"
    }

    # process proper colorbar
    switch -- $ds9(visual) {
        pseudocolor {
            colorbar show
            set current(colorbar) colorbar
            set colorbar(map) [colorbar get name]
            set colorbar(invert) [colorbar get invert]
            colorbar set colormap [$which get colormap]
        }
        truecolor {
            switch -- [$which get type] {
                base {
                    colorbar show
                    colorbarrgb hide
                    set current(colorbar) colorbar
                    set colorbar(map) [colorbar get name]
                    set colorbar(invert) [colorbar get invert]
                    colorbar set colormap [$which get colormap]
                }
                rgb {
                    colorbar hide
                    colorbarrgb show
                    set current(colorbar) colorbarrgb
                    colorbarrgb set colormap rgb [$which get colormap]
                    colorbarrgb rgb channel [$which get rgb channel]
                }
            }
        }
    }

    $canvas(name) raise $which

    $which show
    if { [llength $ds9(active)] > 1 } {
        $which highlite on
    } else {
	RefreshInfoBox $which
	if {!$ds9(freeze)} {
	    BindEventsFrame $current(frame)
	}
    }

    if {$panner(view)} {
        $which panner on
    }

    UpdateDS9
}

proc guideLayout { } {
		global GuideRowCols tile

	set GuideRows [llength $GuideRowCols]

	for { set i 0 } { $i < $GuideRows } { incr i } {
       	 set GuideCols($i) [lindex $GuideRowCols $i]
	}

	TileRect GuideCols $GuideRows $tile(grid,gap)
}

proc tileLayout { } {
	global ds9
	global tile

    set n [llength $ds9(active)]
    switch $tile(mode) {
	column {TileRect 1 $n}
	row  {TileRect $n 1}
	grid {TileRect [expr int(sqrt($n-1)+1.0)] \
		       [expr int(sqrt($n  )+0.5)]}
    }

}

proc blinkLayout { } {
	TileOne
}


proc singleLayout { } {
	TileOne
}


proc TileRect {Numx ny gap} {
    global ds9
    global canvas

    global debug
    if {$debug(tcl,layout)} {
        puts "TileRect"
    }

    if { [string is integer $Numx] } {
        set nx $Numx
    } else {
	upvar $Numx numx
    }

    for {set c 0; set i 0; set b 1} {$i < $ny} {incr i; incr b 2} {
        if { ![string is integer $Numx] } {
	    set nx $numx($i)
	}

	set ww [expr $canvas(width) /$nx - $gap]
	set hh [expr $canvas(height)/$ny - $gap]

        for {set j 0; set a 1} {$j < $nx} {incr j; incr a 2} {
            set w($c) $ww
            set h($c) $hh
            set x($c) [expr int($canvas(width) *$a/(2.*$nx)) + 2]
            set y($c) [expr int($canvas(height)*$b/(2.*$ny)) + 2]

            incr c 
        }
    }

    TileIt w h x y
}

proc TileIt {ww hh xx yy} {
    upvar $xx x
    upvar $yy y
    upvar $ww w
    upvar $hh h
    global ds9
    global current
    global canvas

    global debug
    if {$debug(tcl,layout)} {
        puts "TileIt"
    }

    set i 0
    foreach f $ds9(active) {
        $f configure -x $x($i) -y $y($i)                \
                     -width $w($i) -height $h($i)       \
                     -anchor center
        $f show
        $canvas(name) raise $f
        if {!$ds9(freeze)} {
            BindEventsFrame $f
        }
        incr i
    }

    # if manual grid, current frame could be not included
    if { [lsearch $ds9(active) $current(frame)] == -1 } {
	set current(frame) [lindex $ds9(active) 0]
    }
    FrameToFront $current(frame)
}
