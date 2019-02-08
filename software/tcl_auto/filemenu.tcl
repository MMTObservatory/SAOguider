# Simple file menu commands
#

set FileDialogType blueplate

proc Filemenu {} {}

proc verydirty { w bool { var {} } { indx {} } { op {} } } {
        set filemenu [winfo toplevel $w].menubar.filemenu
	upvar #0 $filemenu file

	set file(dirty) $bool
	set file(path) {}
}

proc filedirty { w bool { var {} } { indx {} } { op {} } } {
	set t [winfo toplevel $w]
	if { [string compare $t .] == 0 } {
	    set t ""
	}
        set filemenu $t.menubar.filemenu
	upvar #0 $filemenu file

	set file(dirty) $bool
}

proc filepath { w path { var {} } { indx {} } { op {} } } {
        set filemenu [winfo toplevel $w].menubar.filemenu
	upvar #0 $filemenu file

	set file(path) $path
}

proc filemenu_trace { w var index op } {
	    upvar $var V

	if { [string compare $index "dirty"]	\
	  || [string compare $index "save"] } {
	    if { $V(dirty) == "yes" && [info exists V(save)] } {
		$w entryconfigure Save -state normal
	    } else {
		$w entryconfigure Save -state disabled
	    }
	    
	}
	if { [string compare $index "save"] == 0 } {
	    if { [info exists V(save)] != 0 } {
		$w entryconfigure "Save As ..." -state normal
	    } else {
		$w entryconfigure "Save As ..." -state disabled
	    }
	}
	if { [string compare $index "open"] == 0 } {
	    if { [info exists V(open)] != 0 } {
		$w entryconfigure "Open ..." -state normal
	    } else {
		$w entryconfigure "Open ..." -state disabled
	    }
	}
	if { [string compare $index "new"] == 0 } {
	    if { [info exists V(new)] != 0 } {
		$w entryconfigure New -state normal
	    } else {
		$w entryconfigure New -state disabled
	    }
	}
	if { [string compare $index "print"] == 0 } {
	    if { [info exists V(print)] != 0 } {
		$w entryconfigure "Print ..." -state normal
	    } else {
		$w entryconfigure "Print ..." -state disabled
	    }
	}
}

proc filemenu { w class } {
	upvar #0 $w.filemenu file
	global $w.filemenu

    catch { unset $w.filemenu }

    if { $w == ".menubar" } {
        onexit [list filemenu_clos $w.filemenu]
        set file(top) 		.
	set lab "Quit"
    } else {
	onclosewindow $w [list filemenu_clos $w.filemenu]
        set file(top) 	.[lindex [split $w .] 1]
	set lab "Close"
    }

    set file(path) 		""
    set file(directory) 	""
    set file(name) 		""
    set file(dirty)		no
    set file(types) {
	    { "All files"			  * }
    }

    menu $w.filemenu -tearoff 0
    $w add cascade -menu $w.filemenu -label "File"
    $w.filemenu add command -label "New"			\
	-command "filemenu_new $w.filemenu  $class"
    $w.filemenu add command -label "Open ..."			\
	-command "filemenu_open $w.filemenu $class"
    $w.filemenu add command -label "Save" 			\
	-command "filemenu_save $w.filemenu $class"
    $w.filemenu add command -label "Save As ..."		\
	-command "filemenu_saas $w.filemenu $class"
    $w.filemenu add command -label "Print ..."			\
	-command "filemenu_prin $w.filemenu"

    if { $w == ".menubar" } {
      $w.filemenu add command -label $lab			\
	  -command "filemenu_clos $w.filemenu"
    } else {
      $w.filemenu add command -label $lab			\
	  -command "filemenu_clos $w.filemenu"
    }

    if { [info exists file(new)] == 0 } {
	 $w.filemenu entryconfigure New           -state disabled
    }
    if { [info exists file(open)] == 0 } {
	 $w.filemenu entryconfigure "Open ..."    -state disabled
    }
    if { [info exists file(save)] == 0 } {
	 $w.filemenu entryconfigure "Save"        -state disabled
    }
    if { [info exists file(save)] == 0 } {
	 $w.filemenu entryconfigure "Save As ..." -state disabled
    }
    if { [info exists file(prin)] == 0 } {
	 $w.filemenu entryconfigure "Print ..."   -state disabled
    }
	 
    trace variable file w "filemenu_trace $w.filemenu"
    set file(dirty) no

    return $w.filemenu
}

proc filemenu_dofilecommand { w cmd filepath message } {
	global errorInfo
	upvar #0 $w file

    set result {}
    if { $filepath != "" } {
	set file(path)      $filepath
	set file(name)      [file tail $filepath]
	set file(directory) [file dirname $filepath]

	if { [catch { eval $file($cmd) $file(path) } result ] == 0 } {
	    if { [string compare $cmd print] != 0 } {
		set file(dirty) no
	    }
	} else {
	    error "$message: $result: $filepath"
	}
    }
    return $result
}

proc filemenu_new { w { class NullFileClass } } {
	upvar #0 $w     file
	upvar #0 $class clas

    set filepath $file(directory)/$clas(newname)
    filemenu_dofilecommand $w new $filepath "new file"
}

proc filemenu_open { w { class NullFileClass } } {
	upvar #0 $w file

    set filepath [OpenFileDialog . $class]
    filemenu_dofilecommand $w open $filepath "opening file"
}

proc filemenu_saas { w { class NullFileClass } } {
	upvar #0 $w file

    set filepath [SaveFileDialog . $class]
    filemenu_dofilecommand $w save $filepath "saving to file"
}

proc filemenu_save { w { class NullFileClass } } {
	upvar #0 $w file

    if { $file(path) != "" } {
	filemenu_dofilecommand $w save $file(path) "saving to file"
    } else {
	filemenu_saas $w $class
    }
}

set printfiledialog_directory 	.
set printfiledialog_path	{}
set printfiledialog_file	{}

proc printfiledialog_browse { } {
	global printfiledialog_path
	global printfiledialog_file
	global printfiledialog_directory

    set printfiledialog_path [tk_getSaveFile		\
		    -initialdir $printfiledialog_directory\
		    -filetypes  { { Postscript *.ps } }	\
		    -blueplate 1		\
		    -title "Print To"		\
		    -parent .]

    set printfiledialog_file      [file tail    $printfiledialog_path]
    set printfiledialog_directory [file dirname $printfiledialog_path]
}

proc printfiledialog_ok {} {
	global printfiledialog_result

	set printfiledialog_result 1
	destroy .printfiledialog
}

proc printfiledialog_cancel {} {
	destroy .printfiledialog
}

proc filemenu_getPrintFile { args } {
	global printfiledialog_result
	global printfiledialog_path
	global printfiledialog_file
	global printfiledialog_directory
	global printer
	global printtofile

    set printer    	lp
    set printers    	lp
    set initialdir     	.
    set filetypes    	{ }
    set title 		"Print Dialog"
    set parent    	.

    foreach { arg val } $args {
	switch -exact -- $arg {
	    -printer    { set printer    $val }
	    -printers   { set printers   $val }
	    -initialdir { set initialdir $val }
	    -filetypes  { set filetypes  $val }
	    -title	{ set title 	 $val }
	    -parent    	{ set parent     $val }
	}
    }

    set d [toplevel .printfiledialog]
    wm title    $d $title

    grid [droplist $d.printer 			\
			-text "Select a Printer"\
			-width  8 		\
			-height 5 		\
			-value $printer		\
			-list  $printers] 
    grid [label $d.space1]

    grid [frame $d.f1] -sticky news
    grid [checkbutton $d.f1.printtofile -text "Print to file:" -variable printtofile] x x
    grid [label $d.f1.l1 -text dir:]  [entry $d.f1.dirsname -textvariable printfiledialog_directory]
    grid [label $d.f1.l2 -text file:] [entry $d.f1.filename -textvariable printfiledialog_file]	\
	 [button $d.f1.browsefile -text "Browse" -command printfiledialog_browse] 

    grid [label $d.space2]

    grid [frame $d.f2] -sticky news
    grid 										\
	 [button $d.f2.cancel -width 10 -text Cancel -command printfiledialog_cancel] 	\
	 [label $d.f2.space3 -width 5]							\
	 [button $d.f2.ok -width 10 -text Ok 	     -command printfiledialog_ok] 

    set printfiledialog_result 0

    focus $d
    grab  $d
    tkwait window $d

    set printfiledialog_path $printfiledialog_directory/$printfiledialog_file

    if { $printfiledialog_result == 0 } {
	return { }
    } else {
	if { $printtofile == 1 } {
	    return $printfiledialog_path
	} else {
	    return $printer
	}
    }
}

proc filemenu_prin { w } {
		global printfile

	upvar #0 $w file

	set printfile [filemenu_getPrintFile 			\
			-printer    { lp }			\
			-printers   { lw14 lp psp5 preview ghostview }		\
			-initialdir $file(directory)		\
			-filetypes  { { Postscript .ps } }	\
			-parent .]

	if { $printfile != { } } {
	    filemenu_dofilecommand $w print $printfile "print file"
	}
}

proc filemenu_clos { w } {
	upvar #0 $w file

    if { [info exists file(save)] } {

	if { $file(dirty) } {
	    if { [string compare $file(path) ""] == 0 } {
		set filename "a file"
	    } else {
		set filename $file(path)
	    }
	    switch [tk_messageBox 	\
		    -icon warning 	\
		    -type yesnocancel	\
		    -message "Do you want to save changes to $filename?."] {
		yes { filemenu_save $w }
	    }
	}
    }

    if { [info exists file(close)] } {
	if { ![ filemenu_dofilecommand $w close $file(path) "close file" ] } {
	     return
	}
    }

    trace vdelete file w "filemenu_trace $w"
    global $w
    unset $w

    destroy [winfo toplevel .[lindex [split $w .] 1]]
}

proc Menubar { w } {
	set W $w
	if { [string compare $w .] == 0 } {
	    set w ""
	}

    menu $w.menubar -type menubar
    $W configure -menu $w.menubar

    return $w.menubar
}

