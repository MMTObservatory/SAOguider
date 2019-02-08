
proc ds9iis { } {
    iis open	ds9iis_open
    iis close	ds9iis_close
    iis erase	ds9iis_erase
    iis crmode	ds9iis_crmode
    iis setcur	ds9iis_setcursor
    iis getcur	ds9iis_getcursor
    iis getwcs	ds9iis_getwcs
    iis setwcs	ds9iis_setwcs
    iis frame	ds9iis_frame

    iis read    ds9iis_read 
    iis write   ds9iis_write

    iis init
}

set iisX	-1
set iisY	-1
set iisN	 0

proc ds9iis_open { } {
	global iisN

    incr iisN
}


proc ds9iis_close { } {
	global iisN

    incr iisN -1
}

proc ds9iis_read { frame pixels x y nx ny } {
	puts "ds9iis_read $frame $pixels $x $y $nx $ny"
}

proc ds9iis_write { frame pixels x y nx ny } {
	puts "ds9iis_write $frame $pixels $x $y $nx $ny"
}


proc ds9iis_frame { frame } {
	#puts "SetFrame $frame"

 if not iis$frame {
	frame iis$frame
 }
}

proc ds9iis_erase { frame } {
	#puts "Erase $frame"
}

proc ds9iis_setwcs { frame buff } {
	global WCS

	#puts "SetWCS $frame"
	#set WCS $buff

	frame$frame 
}

proc ds9iis_write { frame x y data } {
	frame$frame load array $x $y $data_ptr
}

proc ds9iis_write { frame x y data } {
	set data_ptr [frame$frame load array $x $y]

	eval iis retpix $data_ptr $length
}

proc ds9iis_getwcs { frame } {
	global current iisX iisY N

    set filename [$current(frame) get fits full file name canvas $iisX $iisY]

    if { [string compare [string index $filename 0] "/"] } {
	set filename [file join [pwd] $filename]
    }

    if { $iisX != -1 } {
	set WCS "$filename
1. 0. 0. 1. 0. 0. 1. 32767. 1."
    } else {
	set WCS "$filename
1. 0. 0. 1. 0. 0. 1. 32767. 1."
    }

	return $WCS
}

proc ds9iis_canvastoimage { x y { key {} } } {
	global current

if { [catch {
	set image [$current(frame) get image coordinates canvas $x $y]
}] } {
		global errorInfo
	puts $errorInfo
}

	return "$image $key"
}

proc ds9iis_retval { x y { key {} } } {
	global iisX iisY

    set iisX $x
    set iisY $y

    set coords [ds9iis_canvastoimage $x $y $key]
    if { [llength $coords] == 1 } {
	switch $key {
	 : -
	 q {
		set coords "0 0 $key"
	 }
	 default {
		return
	 }
	}
    }

    eval iis retcur $coords
}

proc ds9iis_crmode { frame onoff } {
	global iisX iisY
	global current canvas
	global iisBack

	#puts "CRMode $frame $onoff:"

  if { $onoff == 1 } {
    if { $current(frame) != "" } {
	if { $iisX == -1 } {
	    set iisX [expr [$canvas(name) cget -width]/2]
	    set iisY [expr [$canvas(name) cget -height]/2]
	}

	set iisBack [iis query $canvas(name) root]
	iis warp $canvas(name) $iisX $iisY
	CursorMode 1 { ds9iis_retval }
    }
  } else {
	eval iis warp $iisBack
	CursorMode 0
  }
}

