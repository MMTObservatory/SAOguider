#set env(TELESCOPE)     hacksaw:5403
#set env(FOCUS)         hexapod:5350
#set env(FOCUS)         hacksaw:5403

set fixtype mount

catch {
    menu $ds9(mb).observe -tearoff 0
    $ds9(mb) add cascade -menu $ds9(mb).observe -label Observe
    $ds9(mb).observe add command -label "Init Pointing"   -command "FixPoint init"
    $ds9(mb).observe add command -label "Offset Pointing" -command "FixPoint offs"
    $ds9(mb).observe add command -label "Fix Pointing"    -command "FixPoint both"
}

set instname videoscope

set f5hexscale	0.297
set f9hexscale	0.299

set videoscope(x0)    320
set videoscope(y0)    240
set videoscope(angle) 0
set videoscope(parity) -1
set videoscope(scale) 0.404
set videoscope(coordsys) image
set videoscope(hexscale)	$f9hexscale
set videoscope(fieldwidth)	00:04:00

proc pingcheck { s } {
    global env
    set host [lindex [split $env($s) :] 0]
    return [expr ![catch {exec ping -c 1 $host}]]
}

#source /data/mmti/src/msg.tcl
#source /data/mmti/tcl/msg.tcl
source $env(SAOGUIDER_ROOT)/software/tcl/msg.tcl

proc msg_connect { s } {
    global env
    if {[info exists env($s)]} {
	if {![info exists ${s}(up)]} {
	    if [pingcheck  $s] {
		msg_client $s
	    } else {
		error "$s server machine is down."
	    }
	}
    } else {
	error "$s not defined"
    }
}

proc miniKWLookup { keyword } {
    global current
    foreach line [split [$current(frame) get fits header 1] \n] {
	set ll [split $line =]
	if {[llength $ll] == 2} {
	    scan [lindex $ll 0] %s kw
	    set val [lindex [split [lindex $ll 1] /] 0]
	    regsub -all {\'} $val {} val
	    set key($kw) $val
	}
    }
    return $key($keyword)
}

proc PixToArcsec { instname x y { angle 0 } } {
    upvar #0 $instname caminfo
    set angle [expr ($angle + $caminfo(angle)) / 180. * 3.14159]
    set x [expr $x * $caminfo(parity)]
    set xx [expr ($x *  cos($angle) + $y * sin($angle)) * $caminfo(scale)]
    set yy [expr ($x * -sin($angle) + $y * cos($angle)) * $caminfo(scale)]
    return "$xx $yy"
}

proc ArcsecToPix { instname x y { angle 0 } } {
    upvar #0 $instname caminfo
    set angle [expr -($angle + $caminfo(angle)) / 180. * 3.1415926535]
    set xx [expr ($x *  cos($angle) + $y * sin($angle)) / $caminfo(scale)]
    set yy [expr ($x * -sin($angle) + $y * cos($angle)) / $caminfo(scale)]
    set xx [expr $xx * $caminfo(parity)]
    return "$xx $yy"
}

proc FixPoint { which } {
    global instaz instel rot instscale instpixx0 instpixy0 instname fixtype
    global initpos 
    global targid currid current

    upvar #0 $instname Instname

    if { ![$current(frame) has fits] } { 
	tk_messageBox -message "Sorry, no image in current frame!" -type ok
	return
    }

    msg_connect TELESCOPE
    msg_connect FOCUS

    switch $which {
	 init {
	    set message {
 Move the red marker to the actual target position.

 If you are fixing the pointing with the hexapod you may want to remind the operator to set the elcol ref when this operation is complete.
	    }
	 }
	 offs {
	    set message {
 Move the green marker to the desired target offset.
	    }
	 }
	 both {
	    set message {
 Move the red marker to the actual target position.

 Move the green marker to the desired target offset.

 If you are fixing the pointing with the hexapod you may want to remind the operator to set the elcol ref when this operation is complete.
	    }
	 }
    }

    set w .fixpointing
    if { [winfo exists .fixpointing] } {
	wm deiconify .fixpointing
	raise .fixpointing

	$w.msg configure -text $message
    } else {
	toplevel $w

	grid [message $w.msg -text $message] [frame $w.a] -sticky news

        if { [string compare $which offs] } {
	    grid [radiobutton $w.a.mount   -variable fixtype -value mount   -text "Fix with mount"]   -sticky news
	    grid [radiobutton $w.a.hexapod -variable fixtype -value hexapod -text "Fix with hexapod"] -sticky news
	}
	
	grid [frame $w.b] - -sticky new
	grid [button $w.b.ok     -text OK     -command fixpointingok]		\
	     [button $w.b.cancel -text Cancel -command fixpointingcancel]	\
		-sticky news
    }

    if { [ catch {
	set instaz [miniKWLookup INSTAZ]
    } ] } { 
	if { [catch { set instaz [msg_get TELESCOPE instazoff] }] } {
	    set instaz 0
	}
    }

    if { [ catch {
	set instel [miniKWLookup INSTEL]
    } ] } {
	if { [catch { set instel [msg_get TELESCOPE insteloff] }] } {
	    set instel 0
	}
    }

    if { [ catch {
	set rot    [miniKWLookup ROTANGLE]
    } ] } {
	if { [ catch {
	    set rot    [msg_get TELESCOPE rot]
	    puts "Got rot=$rot from telescope"
	} ] } {
	    puts "No rot info. Setting rot to 0"
	    set rot 0
	}
    }

    catch { $current(frame) marker $targid delete }
    catch { $current(frame) marker $currid delete }

    set zz [ArcsecToPix $instname $instaz $instel 0]
    set instpixx [expr $Instname(x0) - [lindex $zz 0]]
    set instpixy [expr $Instname(y0) - [lindex $zz 1]]
    
    if { [string compare $which offs] } { set amove 1
    } else {				  set amove 0 }
    if { [string compare $which init] } { set dmove 1
    } else {				  set dmove 0 }

    set currid [$current(frame) marker create circle $instpixx $instpixy 25 color = red text = Current]
    $current(frame) marker $currid move to $Instname(coordsys) $instpixx $instpixy
    $current(frame) marker $currid property move $amove

    set targid [$current(frame) marker create circle $instpixx $instpixy 50 color = green text = Desired]
    $current(frame) marker $targid move to $Instname(coordsys) $instpixx $instpixy
    $current(frame) marker $targid property move $dmove

    set initpos [$current(frame) get marker $currid center $Instname(coordsys)]
}

proc fixpointingok { } {
    global targid currid current initpos rot instscale instname fixtype
    upvar #0 $instname Instname

    set targ [$current(frame) get marker $targid center $Instname(coordsys)]
    set curr [$current(frame) get marker $currid center $Instname(coordsys)]

    puts "init: $initpos"
    puts "curr: $curr"
    puts "targ: $targ"
    
    set x0      [lindex $initpos 0]
    set y0      [lindex $initpos 1]

    set dx      [expr [lindex $curr 0] - $x0]
    set dy      [expr [lindex $curr 1] - $y0]
    foreach { daz del } [PixToArcsec $instname $dx $dy $rot] {}

    set dx      [expr -([lindex $targ 0] - $x0)]
    set dy      [expr -([lindex $targ 1] - $y0)]
    foreach { dinstaz dinstel } [PixToArcsec $instname $dx $dy 0] {}

    fixpointing $fixtype $daz $del $dinstaz $dinstel

    $current(frame) marker $targid delete
    $current(frame) marker $currid delete
    destroy .fixpointing
}

proc fixpointing { type daz del dinstaz dinstel } {
    puts "$type $daz $del $dinstaz $dinstel"

    if { $type == "mount" } {
	catch {
	    msg_cmd TELESCOPE "azelrel   $daz $del"        	5000
	}
       puts "Sent corrections to telescope Mount: $daz $del"
    } elseif {$type == "hexapod"} {
	global    instname
    	upvar #0 $instname Instname

	set scale $Instname(hexscale)

	set tilty [expr $daz / $scale]
	set tiltx [expr $del / $scale]

	msg_cmd FOCUS "tiltxerr_zc $tiltx" 10000
	msg_cmd FOCUS "tiltyerr_zc $tilty" 10000
        puts "Sent corrections to Hexapod: $tiltx $tilty"
    }

    msg_cmd TELESCOPE "instrel $dinstaz $dinstel" 10000
    puts "Sent corrections to telescope: Instr: $dinstaz $dinstel"
}

proc fixpointingcancel {} {
    global targid currid current
    $current(frame) marker $targid delete
    $current(frame) marker $currid delete
    destroy .fixpointing
}


proc UA2Overlay {} {
    global instname current ds9
    upvar #0 $instname Instname

    set ra [miniKWLookup RA]
    # Remove leading + space or tab from RA
    regsub {^[ \t+]*} $ra {} ra
    set dec [miniKWLookup DEC]
    set fieldwidth $Instname(fieldwidth)

    exec $env(SAOGUIDER_ROOT)/software/tcl_bin/catget ua2@sao $ra $dec $fieldwidth -n 10000 | $env(SAOGUIDER_ROOT)/software/tcl_bin/catreg | xpaset $ds9(title) regions &    
#    exec scat ua2 -t -j -r 540 -n 100000 -m 22 $ra $dec | catreg | xpaset $ds9(title) regions &

    set instaz [miniKWLookup INSTAZ]
    set instel [miniKWLookup INSTEL]
    set rot    [miniKWLookup ROTANGLE]
    set zz [ArcsecToPix $instname $instaz $instel 0]
    set instpixx [expr $Instname(x0) - [lindex $zz 0]]
    set instpixy [expr $Instname(y0) - [lindex $zz 1]]
    set currid [$current(frame) marker create circle $instpixx $instpixy 1 color = red]
    $current(frame) marker $currid move to $Instname(coordsys) $instpixx $instpixy

    puts "    $current(frame) marker $currid move to $Instname(coordsys) $instpixx $instpixy"

}
