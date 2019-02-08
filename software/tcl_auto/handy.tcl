# This is a bunch of tcl that is usefull in general applications.
#

proc nop { x } { }

proc laddset { list args } {

    foreach item $args {
	if { [lsearch $list $item] == -1 } {
	    lappend list $item
	}
    }

    return $list
}

set OnExit ""

proc null { args } {
}


rename exit _exit
proc   exit { { retvalue 0 } } {
		global	OnExit

	foreach code $OnExit {
		eval $code
	}
	_exit $retvalue
}

proc onexit { code } {
		global	OnExit

	lappend OnExit $code
}


proc closewindow { w } {
		set top [winfo toplevel $w]
		upvar #0 ${top}-OnCloseWindow OnCloseWindow


	if { [info exists OnCloseWindow] } {
	    foreach code $OnCloseWindow {
		eval $code
	    }
	    unset OnCloseWindow
	}
        destroy $top
}

proc onclosewindow { w code } {
		set top [winfo toplevel $w]
		upvar #0 ${top}-OnCloseWindow OnCloseWindow

	lappend OnCloseWindow $code
}

proc unmapwindow { w } {
		set top [winfo toplevel $w]

	if { [info exists ${top}-OnUnMapWindow] } {
		upvar #0 ${top}-OnUnMapWindow OnUnMapWindow

	    foreach code $OnUnMapWindow {
		eval $code
	    }
	}
	wm withdraw [winfo toplevel $w]
}

proc onunmapwindow { w code } {
		set top [winfo toplevel $w]
		upvar #0 ${top}-OnUnMapWindow OnUnMapWindow

	lappend OnUnMapWindow $code
}

proc delvalue { w name } {
	upvar #0 $w V

    if { [catch { unset $V($name) } ] } {
	return 
    }
}

proc getvalue { w name } {
	upvar #0 $w V

    if { [catch { set value $V($name) } ] } {
	return 
    }

    return $value
}

proc setvalue { w name value } {
	upvar #0 $w V

    set V($name) $value
}

proc InFrame { w args } {
        eval frame $w [lrange $args 0 [expr [llength $args] -2]]
	set body [lindex $args end]
	regsub -all "%%" $body "\01" body
	regsub -all "%w" $body $w body
	regsub -all "\01" $body "%" body
        uplevel $body
        return $w
}

proc Pack { args } {
	eval pack $args  -fill both -expand yes
}

proc Grid { args } {
	eval grid $args -sticky nsew
}

proc Scroll { w type name args } {
    frame $w
    eval $type  $w.$name 					\
		    -yscrollcommand [list "$w.yscroll set"]	\
		    $args
    scrollbar $w.yscroll -command "$w.list yview"

    grid $w.list $w.yscroll -sticky news
    grid rowconfigure    $w 0 -weight 1

    return $w
}

proc scrolllist { w args } {
	frame $w
	eval listbox  $w.list 					\
			-yscrollcommand [list "$w.yscroll set"]	\
			$args
	scrollbar $w.yscroll -command "$w.list yview"

	grid $w.list $w.yscroll -sticky news
	grid rowconfigure    $w 0 -weight 1

	return $w
}

proc ScrollText { w args } {
	frame $w
	eval text  $w.text 	-wrap none 		\
			-yscrollcommand [list "$w.yscroll set"]\
			-xscrollcommand [list "$w.xscroll set"]\
			-width  80			\
			-height 25			\
			$args

	scrollbar $w.xscroll 				\
			-command "$w.text xview"	\
			-orient horizontal
			
	scrollbar $w.yscroll -command "$w.text yview"

	grid $w.text $w.yscroll -sticky news
	grid $w.xscroll         -sticky  ew

	grid rowconfigure    $w 0 -weight 1
	grid columnconfigure $w 0 -weight 1

	return $w.text
}

proc Toplevel { w title { geometry {} } } {
		setvalue $w title $title

	if { $w == "." } {
	    wm protocol $w WM_DELETE_WINDOW "exit"

	    bind [winfo class .] <Unmap> "unmap \[getvalue $w dialogs]"
	    bind [winfo class .] <Map>   "remap \[getvalue $w unmapps]"
	} else {
	    toplevel $w -class ExtraTop
	    wm withdraw $w
	    wm protocol $w WM_DELETE_WINDOW "unmapwindow $w"

	    bind ExtraTop <Unmap> { wm withdraw %W }

	    setvalue . dialogs "[getvalue . dialogs] $w"
	}

	wm title    $w $title
	if { [string compare $geometry ""] != 0 } {
	    wm geometry $w $geometry
	}

	return $w
}

proc ToplevelRaise { w } {
	if { [winfo ismapped $w] == 1 } {
		raise $w
	} else {
		wm deiconify $w
	}
}

proc ToplevelToggle { w } {
	if { [winfo ismapped $w] == 1 } {
		wm iconify $w
	} else {
		wm deiconify $w
	}
}


proc unmap { windows } {
	global unmapps

    setvalue . unmapps ""

    foreach w $windows {
	if { [winfo ismapped $w] == 1 } {
		wm withdraw $w
		setvalue . unmapps "[getvalue . unmapps] $w"
	}
    }
}

proc remap { unmapps } {
    foreach w [getvalue . unmapps] {
	wm deiconify $w
    }
}

