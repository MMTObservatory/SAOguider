# wizard.tcl
#

option add *Text*cursor right_ptr
option add *Button*cursor left_ptr
option add *Entry*cursor xterm

set wz_PID {}

proc wizard { w title } {
	global wizard_nscreen
	global wizard_stack
	global wizard_frame
	global wizard_current
	global wizard_up

    set wizard_nscreen 0
    set wizard_current {}
    set wizard_up      {}

    if { [string compare $w .] } {
	toplevel $w
        wm title $w $title
	wm geometry $w +10+10
    } else {
        wm title . $title
	wm geometry . +10+10
    }

    set wizard_frame [frame $w.wizard_frame]
    Grid [label $wizard_frame.title -text $title] -
    Grid [frame $wizard_frame.info] -
    Grid [frame $wizard_frame.lbox -width 300]	\
	 [InFrame $wizard_frame.wcontrols {
	    upvar wizard_frame   wizard_frame
	    upvar wizard_nscreen wizard_nscreen

	Grid [button $wizard_frame.wcontrols.prev -text "Prev" \
			-width 12 -command wizard_prev -state disabled] \
	     [button $wizard_frame.wcontrols.next -text "Next" \
			-width 12 -command wizard_next] \
	     [button $wizard_frame.wcontrols.quit -text "Quit" \
			-width 12 -command wizard_quit]
    }]
    Grid [label $wizard_frame.space0]
    Grid [label $wizard_frame.text -anchor w -justify left] -

    return $w
}

procargs wizard_script { name params { type shell } { prescript {} } { postscript {} } } {

	global scripts prefix

	set scripts($name,prefix) 	"$prefix"
	set scripts($name,params) 	"$params"
	set scripts($name,type)   	"$type"
	set scripts($name,prescript)    "$prescript"
	set scripts($name,postscript)   "$postscript"
}

proc wz_script { name } {
	global scripts

    set prefix $scripts($name,prefix)

    if { $scripts($prefix,prescript) != {} } {
	if { [catch { eval $scripts($prefix,prescript) $name }] } {
	    return
	}
    }
    if { $scripts($name,prescript) != {} } {
	if { [catch { eval $scripts($name,prescript) $name }] } {
	    return
	}
    }

    switch -exact $scripts($name,type) {
	shell_bg {
	    uplevel #0 [concat exec ${prefix}_$name $scripts($name,params) &]
	}
	shell_async {
	    # Oh! Heavens No! Not That!
	    #
	    set fp [uplevel #0 open \[list |${prefix}_$name $scripts($name,params)\] r]
	    fileevent $fp r "wz_script_handler $fp"
	}
	shell    {
		global wz_PID

	    if { $wz_PID != {} } {
		return
	    }
	    # Oh! Heavens No! Not That!
	    #
	    set fp [uplevel #0 open \[list |${prefix}_$name $scripts($name,params)\] r]
	    fileevent $fp r "wz_script_handler $fp"
	    set wz_PID [pid $fp]
	}
	tcl {
	    uplevel #0 [concat eval           $name $scripts($name,params)]
	}
	proc {
	    uplevel #0 [concat eval ${prefix}_$name $scripts($name,params)]
	}
	default {
		error "wizard: Unknown script type: $scripts($name,type)"
	}
    }

    if { $scripts($name,postscript) != {} } {
	eval $scripts($name,postscript) $name
    }
    if { $scripts($prefix,postscript) != {} } {
	eval $scripts($prefix,postscript) $name
    }
}

proc wz_script_handler { fp } {
    set n [gets $fp line]

    if { $n <= 0  } {
	global wz_PID

        catch { close $fp }
	set wz_PID {}
        return
    }

    uplevel #0 $line
}

proc wz_B  { args } { upvar w w; eval wz_button  $args }
proc wz_1B { args } { upvar w w; eval wz_1button $args }
proc wz_nB { args } {
	upvar w w; 

    set W {}
    foreach { b text } $args {
	set W "$W [wz_button $b $text]"
    }
    eval Grid $W
}

proc wz_nV { args } {
	upvar w w; 

    set W {}
    foreach { b text arg } $args {
	set W "$W [wz_val $b $text $arg]"
    }
    eval Grid $W
}

proc wz_setvalue { glob name indx op } {
    global wizard_current
    upvar  #0 wizard_screen$wizard_current screen
    upvar $name valu

    if { $op == "r" } {
	    if { [catch { set valu $screen($name) }] } {
		upvar #0 $glob var
		set valu $var
	    }
    } else {
	set screen($name) $valu
    }
}

proc wz_val { value args } {
    global Values
    global Helps
    global $value;

    global wizard_nscreen

    upvar w w;

    set wname [string tolower $value]

    set A {}
    foreach { arg val } $args {
	switch -exact -- $arg {
	 -help {
    	    set Helps($w.$wname) $val
	 } 
         default {
	    set A "$A $arg $val"
	 }
	}
    }

    global $w.$wname
    upvar #0 $w.$wname W
    set W(topic) $value

    if { $A == "" } {
	set A "-width 10"
    }

    set screenval wizard_screen${wizard_nscreen}($value)
    upvar #0 $value d
    upvar #0 $screenval v
    catch { set v $d }

    if { [catch { set t $Values($value) }] } {
	set Values($value) Set

	trace variable $value rw "wz_setvalue $value"
    }

    grid [eval entry $w.$wname -textvariable $screenval -justify right $A]
}

proc wz_1button { args } {
        upvar w w
 
    Grid [eval wz_button $args]
}

proc wz_goto { name { text {} } { width 15 } } {
        upvar w w

    if { ![string compare $text {}] } {
        set text $name
    }
 
    grid [button $w.$name -text $text -width $width -command "wizard_goto $name"]
}

proc wz_button { script { text {} } { width 8 } } {
        upvar w w
 
    if { ![string compare $text {}] } {
        set text $script
    }
 
    button $w.$script -text $text -width $width -command "wz_script $script"
}

proc wizard_prefix { fix { prescript {} } { postscript {} } } {
	global prefix scripts

	set prefix $fix
	set scripts($prefix,prescript)	$prescript
	set scripts($prefix,postscript)	$postscript
}

proc wizard_up { name } {
	global wizard_up

	set wizard_up $name

}
proc wizard_screen { name { contents {} } } {
	global wizard_frame
	global wizard_nscreen
	global wizard_map
	global wizard_up

    if { [string compare $contents {}] == 0 } {
	set contents $name
	set name {}
    }

    set wizard_map($name)    	        $wizard_nscreen
    set wizard_map($wizard_nscreen)     $name
    set wizard_map($wizard_nscreen,up)  $wizard_up

    set screen [text $wizard_frame.info.w$wizard_nscreen -highlightthickness 0 -borderwidth 0]

    $screen tag configure center -justify center

    set n 0
    set stat text

    foreach line [split $contents "\n"] {
	switch -exact $stat {
            text { 
		switch -regexp $line {
		    "^@" {	set prog [string range $line 1 end]
				set stat prog
			  }
		    "@" {
			while {	[regexp -indices {@([^@]*)@} $line match substr] } {

				set prog [string range $line [lindex $substr 0] [lindex $substr 1]]
				set n [expr $n + 1]

				eval InFrame $wizard_frame.info.w$wizard_nscreen.control$n {
					$prog
				}

				$screen insert end [string range $line 1 [expr [lindex $match 0]-1]]

				$screen mark set s_control$n end
				$screen window create end -window $wizard_frame.info.w$wizard_nscreen.control$n
				$screen mark set e_control$n end

				set line "[string range $line [expr [lindex $match 1]+1] end]"
			}
			$screen insert end "$line\n"

			}
		    default { $screen insert end "$line\n" }
		}
	    }
	    prog {
		switch -regexp $line {
		    "^ *@" {	set prog "$prog\n[string range $line 1 end]"
				set stat text
				set n [expr $n + 1]

				eval InFrame $wizard_frame.info.w$wizard_nscreen.control$n {
					$prog
				}
				$screen insert end "                    "
				$screen mark set s_control$n end
				$screen window create end -window $wizard_frame.info.w$wizard_nscreen.control$n
				$screen mark set e_control$n end
				$screen tag  add center s_control$n e_control$n
			  }
		    default { set prog "$prog\n$line" }
		}
	    }
	}
    }

    $screen configure -state disabled

    if { $wizard_nscreen == 0 } {
	pack $screen
    }
    set wizard_nscreen [expr $wizard_nscreen + 1]
}

proc wizard_quit { } {
	global wizard_up

	if { ![string compare $wizard_up {}] } {
	    exit
	} else {
	    wizard_goto $wizard_up
	}
}

proc wizard_bstate { } {
	global wizard_current
	global wizard_nscreen
	global wizard_frame
	global wizard_up

	if { $wizard_current == 0 } {
	    $wizard_frame.wcontrols.prev configure -state disabled
	} else {
	    $wizard_frame.wcontrols.prev configure -state normal
	}
	if { $wizard_current == $wizard_nscreen-1 } {
	    $wizard_frame.wcontrols.next configure -text "Done"
	} else {
	    $wizard_frame.wcontrols.next configure -text "Next"
	}
	if { ![string compare $wizard_up {}] } {
	    $wizard_frame.wcontrols.quit configure -text "Quit"
	} else {
	    $wizard_frame.wcontrols.quit configure -text " Up "
	}
}

proc wizard_goto { name } {
	global wizard_current
	global wizard_nscreen
	global wizard_frame
	global wizard_map
	global wizard_up

	if { ![string compare $wizard_current {}] } {
	    pack $wizard_frame
	}

	pack forget $wizard_frame.info.w$wizard_current
	set wizard_current $wizard_map($name)
	set wizard_up      $wizard_map($wizard_current,up)

	wizard_bstate
	pack $wizard_frame.info.w$wizard_current
}

proc wizard_prev { } {
	global wizard_nscreen
	global wizard_current
	global wizard_frame
	global wizard_map
	global wizard_up

	pack forget $wizard_frame.info.w$wizard_current
	set wizard_current [expr $wizard_current - 1]
	set wizard_up      $wizard_map($wizard_current,up)

	wizard_bstate
	pack $wizard_frame.info.w$wizard_current
}

proc wizard_next { } {
	global wizard_current
	global wizard_nscreen
	global wizard_frame
	global wizard_map
	global wizard_up

	pack forget $wizard_frame.info.w$wizard_current

	if { $wizard_current == $wizard_nscreen-1 } {
	    set wizard_current 0
	} else {
	    set wizard_current [expr $wizard_current + 1]
	}
	set wizard_up      $wizard_map($wizard_current,up)

	wizard_bstate
	pack $wizard_frame.info.w$wizard_current
}

proc wizard_help { w string } {
	global Helps

    set Helps($w) $string
}

#__________________________________________________________________________
#
#  Print a hint at the bottom of the scripmaker window
#__________________________________________________________________________
proc hint { w } {
	global Helps
	global wizard_frame
	upvar #0 $w W

    if { [catch { $wizard_frame.text configure -text $Helps($w) }] == 1 } {
	if { [catch { $wizard_frame.text configure -text $Helps($W(topic)) }] == 1 } {
	    $wizard_frame.text configure -text {}
	} 
    }
}
proc hint_clear { w } {
    global Helps
    global wizard_frame
    upvar #0 $w W

    $wizard_frame.text configure -text {}
}

trace variable MessageText w messagetext
proc messagetext { name indx op } {
    upvar $name message
    global wizard_frame

	$wizard_frame.text configure -text $message
}
 
bind Button <Any-Enter> {+hint %W}
bind Text <Any-Enter> {+hint %W}
bind Entry <Any-Enter> {+hint %W}
bind Label <Any-Enter> {+hint %W}
bind Radiobutton <Any-Enter> {+hint %W}
bind Checkbutton <Any-Enter> {+hint %W}
bind Menubutton <Any-Enter> {+hint %W}
bind Frame <Any-Enter> {+hint %W}
bind Listbox <Any-Enter> {+hint %W}
 
bind Button <Any-Leave> {+hint_clear %W}
bind Text <Any-Leave> {+hint_clear %W}
bind Entry <Any-Leave> {+hint_clear %W}
bind Label <Any-Leave> {+hint_clear %W}
bind Radiobutton <Any-Leave> {+hint_clear %W}
bind Checkbutton <Any-Leave> {+hint_clear %W}
bind Menubutton <Any-Leave> {+hint_clear %W}
bind Frame <Any-Leave> {+hint_clear %W}
bind Listbox <Any-Leave> {+hint_clear %W}
 

