# Simple table menu commands
#


proc tablmenu { w } {
	upvar #0 $w.tablmenu tabl

    menu $w.tablmenu -tearoff 0
    $w add cascade -menu $w.tablmenu -label "Edit"
    $w.tablmenu add command -label "Columns ..."		\
	-command "tablcolumns $w.tablmenu"

    return $w.tablmenu
}
