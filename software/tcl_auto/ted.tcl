# Table Editor Tcl Interface
#

proc Ted { } {
	return "Table Editor Version 1.1"
}

proc docol_add { cd } {
    foreach col [$cd.columns.list curselection] {
    	$cd.project.list insert end [$cd.columns.list get $col]
    }
}
proc docol_ins { cd } {
    set here [$cd.project.list curselection]
    if { [string compare $here {}] } {
	foreach col [$cd.columns.list curselection] {
	    $cd.project.list insert $here [$cd.columns.list get $col]
	}
    }
}
proc docol_rem { cd } {
    set this [$cd.project.list curselection]
    if { [string compare $this {}] } {
        $cd.project.list delete $this
    }
}

proc docol_can { cd } {
    upvar #0 $cd d
    ted_project [winfo parent $cd] $d

    destroy $cd
    set d 1
}
proc docol_app { cd } {
    ted_project [winfo parent $cd] [$cd.project.list get 0 end]
}
proc docol_ok { cd } {
    upvar #0 $cd d
    destroy $cd
    set d 1
}

proc ted_docolumn { E } {
     	set d [toplevel $E.cd]

    upvar #0 $d cd

    wm title $d "Pick Columns to Display"

    Grid   [label $d.display -text "Displayed"]	\
	 x [label $d.availab -text "Available"]
    Grid [scrolllist $d.project -exportselection 0]	\
	 [InFrame $d.frame {
	    upvar E E

	    grid [button $w.add -text "Add"    -command "docol_add $E.cd"] \
	    	-sticky new
	    grid [button $w.ins -text "Insert" -command "docol_ins $E.cd"] \
	    	-sticky new
	    grid [button $w.del -text "Remove" -command "docol_rem $E.cd"] \
	    	-sticky new
	 }]			\
	 [scrolllist $d.columns -exportselection 0]

    grid [button $d.cancel -width 6 -text Cancel   -command "docol_can $E.cd"]	\
	 [button $d.apply  -width 6 -text Apply    -command "docol_app $E.cd"]	\
	 [button $d.ok     -width 6 -text OK       -command "docol_ok  $E.cd"]

    eval $d.project.list insert 0 [ted_projection $E]
    eval $d.columns.list insert 0 [ted_columns    $E]

    grid rowconfigure    $d 1 -weight 1

    set cd [ted_projection $E]
    vwait $d
}

proc ted_save { E file } {
	set e $E
	if { [string compare $E .] == 0 } {
	    set e ""
	}
	global $e.data

	starbase_write $e.data $file
}

proc ted_new { E file args } {
	set e $E
	if { [string compare $E .] == 0 } {
	    set e ""
	}
	global $e.data

	starbase_new    $e.data
	starbase_driver $e.data

	eval ted_create $E $e.data $args
	if { [string compare $file None] && [string compare $file {}] } {
	    ted_open $e.edit $file
	    wm title [winfo toplevel $E] $file
	}
}

proc ted_http { E url } {
	set e $E
	if { [string compare $E .] == 0 } {
	    set e ""
	}
	global $e.data

    starbase_http $url $e.data
    starbase_driver $e.data

    ted_data $E $e.data
}

proc ted_read { E fp } {
	set e $E
	if { [string compare $E .] == 0 } {
	    set e ""
	}
	global $e.data

    starbase_readfp $e.data $fp
    starbase_driver $e.data

    ted_data $E $e.data
}

proc ted_open { E file } {
	set e $E
	if { [string compare $E .] == 0 } {
	    set e ""
	}

	global $e.data

	set fp [open $file]
	ted_read $E $fp
	close $fp

	filepath $E $file

	wm title [winfo toplevel $E] $file
}

set TableFileClass(file) 	{}
set TableFileClass(directory) 	[pwd]
set TableFileClass(dirslist)	{}
set TableFileClass(loadtype)  	{}
set TableFileClass(pattern)	 0
set TableFileClass(types) {
        { "Starbase Files"  	.tab    }
        { "Starbase Files"  	.cat    }
        { "Starbase Files"  	.gal    }
        { "Starbase Files"  	.fld    }
        { "Table Files"  	.tab    }
        { "Catalog Files"  	.cat    }
        { "Field Files"  	.fld    }
        { "Galaxy Files"	.gal    }
        { "All files"           *     	}
}

proc ted_create { E D args } {
	set e $E
	if { [string compare $E .] == 0 } {
	    set e ""
	}

    catch {
	set menubar [Menubar  [winfo toplevel $E]]

	set filemenu [filemenu $menubar TableFileClass]
	setvalue $filemenu new  "ted_new  $e.edit"
	setvalue $filemenu open "ted_open $e.edit"
	setvalue $filemenu save "ted_save $e.edit"
	setvalue $filemenu path ""

	set tablmenu [tablmenu $menubar]
	setvalue $tablmenu rowins "ted_dorowins $e.edit"
	setvalue $tablmenu rowdel "ted_dorowdel $e.edit"
	setvalue $tablmenu colins "ted_docolins $e.edit"
	setvalue $tablmenu coldel "ted_docoldel $e.edit"
	setvalue $tablmenu column "ted_docolumn $e.edit"
    }

    eval ted_editor $e.edit None $args
    grid $e.edit -row 1 -column 0 -sticky news
    grid rowconfigure $E    0 -weight 0
    grid rowconfigure $E    1 -weight 1
    grid columnconfigure $E 0 -weight 1
}

proc ted_grid { E D args } {
        set e $E
        if { [string compare $E .] == 0 } {
            set e ""
        }

    return [eval ted_editor $e.edit None $args]
}

# Set the data table for the editor
#
proc ted_data { E data } { 
	upvar #0 $E editor

    setvalue $E data $data

    if { ![string compare $data None] } { 
        setvalue $E nrows 10
	catch { unset editor(project) }
	return
    }

    setvalue $E nrows [ted_nrows $E] 
    upvar #0 $editor(data) D
    if { [info exists editor(project)] } {
	ted_project $E [ted_projection $E]
    } else {
	ted_project $E [$D(columns) D]
    }

    $E.table configure -rows [expr [ted_nrows $E] + 1]	\
		       -cols [expr [ted_projected $E] + 1]

}

# Set the data table for the editor
#
proc ted_see { E index } { 
	upvar #0 $E editor

    $E.table see $index 

}

# File reading drivers
#
proc ted_readers { E list } { setvalue E readers $list }

# Top level IO procs
#
proc ted_readfile  { E filename } {
    foreach { pattern reader } { [getvalue $E readers] } {
	if [regex $pattern $filename] {
	    [getvalue $E write] [getvalue $E data] $filename
	}
    }
}
proc ted_writfile { E filename } { [getvalue $E write] 	[getvalue $E data] $fp 	 	  	}

proc tablmenu_dotablcommand { w cmd } {
	global errorInfo
	upvar #0 $w tabl

	eval $tabl($cmd)
}

proc tablmenu { w } {
    menu $w.tablmenu -tearoff 0
    $w add cascade -menu $w.tablmenu -label "Table"
    $w.tablmenu add command -label "Columns ..."		\
	-command "tablmenu_dotablcommand $w.tablmenu column"
    $w.tablmenu add command -label "Insert Row"			\
	-command "tablmenu_dotablcommand $w.tablmenu rowins"
    $w.tablmenu add command -label "Delete Row"			\
	-command "tablmenu_dotablcommand $w.tablmenu rowdel"
    $w.tablmenu add command -label "Insert Col ..."		\
	-command "tablmenu_dotablcommand $w.tablmenu colins"
    $w.tablmenu add command -label "Delete Col"			\
	-command "tablmenu_dotablcommand $w.tablmenu coldel"

    return $w.tablmenu
}

proc ted_dorowdel { w } {
                upvar #0 $w E
 
    set row [lindex [$w.table curselection] 0]
    set row [lindex [split [$w.table curselection] ,] 0]
    if { $row == "" } { return }

    set r [lindex [split $row ","] 0]
    set c [lindex [split $row ","] 1]

    ted_rowdel $w $r
    #ted_setcell E $r $c $T($r,$c)
}
 
proc ted_dorowins { w } {
                upvar #0 $w.edit E
 
    set row [lindex [$w.table curselection] 0]
    if { $row == "" } {
	set r 1
    } else {
        set r [lindex [split $row ","] 0]
        set c [lindex [split $row ","] 1]
    }
    set r [lindex [split $row ","] 0]
    set c [lindex [split $row ","] 1]
 
    ted_rowins $w $r
    #ted_setcell E $r $c $T($r,$c)
}

proc ted_docolins { w } {
                upvar #0 $w E
 
    set row [lindex [$w.table curselection] 0]
    if { $row == "" } {
	set c 1
    } else {
        set r [lindex [split $row ","] 0]
        set c [lindex [split $row ","] 1]
    }

    ted_colins $w "new" $c
    #ted_setcell E $r $c $T($r,$c)
}

proc ted_docoldel { w } {
                upvar #0 $w E
 
    set row [lindex [$w.table curselection] 0]
    if { $row == "" } { return }

    set r [lindex [split $row ","] 0]
    set c [lindex [split $row ","] 1]

    ted_coldel $w $c
    #ted_setcell E $r $c $T($r,$c)
}


# Get a new table editor
#
proc ted_editor { E D args } {
	    frame $E
	    upvar #0 $E editor


	ted_data $E $D

	if { [string compare $D None] } {
	    set Nrows [expr [ted_nrows     $E] + 1]
	    set Ncols [expr [ted_projected $E] + 1]
	} else {
	    set editor(data) None
	    set Ncols 10
	    set Nrows 10
	}

	set command "ted_tablecommand $E %i %r %c %s"
	eval table $E.table				\
	    -command {$command}			 	\
	    -cache 0 					\
	    -titlerows 1				\
	    -titlecols 1				\
	    -yscrollcommand {[list $E.sy set]}		\
	    -xscrollcommand {[list $E.sx set]}		\
	    -rows $Nrows				\
	    -cols $Ncols				\
	    -anchor e					\
	    -colorigin 0 -roworigin 0			\
	    -colwidth	15				\
	    -height 24					\
	    -rowstretch none 				\
	    -colstretch none 				\
	    -selectmode extended			\
	    -rowseparator {"\n"}			\
	    -colseparator {"\t"}			\
	    $args
	scrollbar $E.sy -command [list $E.table yview] -orient v
	scrollbar $E.sx -command [list $E.table xview] -orient h

	#bind Table <Key-Return> { }

	$E.table width 0 6

	grid $E.table $E.sy -sticky nsew
	grid $E.sx          -sticky nsew
	grid rowconfigure    $E 0 -weight 1
	grid columnconfigure $E 0 -weight 1

	return $E
}


proc ted_nrows	  { E }	      		{ upvar #0 $E editor; upvar #0 $editor(data) D; $D(nrows)   D   	}
proc ted_ncols	  { E }			{ upvar #0 $E editor; upvar #0 $editor(data) D; $D(ncols)   D 		}
proc ted_set      { E row col value } 	{ upvar #0 $E editor; upvar #0 $editor(data) D; $D(set)     D $row $col $value }
proc ted_get      { E row col } 	{ upvar #0 $E editor; upvar #0 $editor(data) D; $D(get)     D $row $col }
proc ted_colnum   { E name } 		{ upvar #0 $E editor; upvar #0 $editor(data) D; $D(colnum)  D $name 	}
proc ted_colname  { E col }		{ upvar #0 $E editor; upvar #0 $editor(data) D; $D(colname) D $col 	}
proc ted_columns  { E }			{ upvar #0 $E editor; upvar #0 $editor(data) D; $D(columns) D 		}
proc ted_coldel   { E col }		{ upvar #0 $E editor; upvar #0 $editor(data) D; $D(coldel)  D $col
					  $E.table configure -cols [expr [ted_projected $E]+1] 		}
proc ted_colins   { E name here }	{ upvar #0 $E editor; upvar #0 $editor(data) D; $D(colins)  D $name $here
					  $E.table configure -cols [expr [ted_projected $E]+1] 		}
proc ted_colapp   { E name }		{ upvar #0 $E editor; upvar #0 $editor(data) D; $D(colapp)  D $name 
					  $E.table configure -cols [expr [ted_projected $E]+1] 		}
proc ted_rowdel   { E row } {
    	upvar #0 $E editor;  upvar #0 $editor(data) D
	$D(rowdel)  D $row
	ted_data $E $editor(data)
}
proc ted_rowins   { E here } {
    	upvar #0 $E editor;  upvar #0 $editor(data) D
    	$D(rowins)  D $here
	ted_data $E $editor(data)
}
proc ted_rowapp   { E } {
    	upvar #0 $E editor;  upvar #0 $editor(data) D
	$D(rowapp)  D 
	ted_data $E $editor(data)
}

# A user level proc to set a value in the data array and update the screen
#
proc ted_setcell { E row col value } {
	upvar #0 $E editor

    filedirty $E yes
    if { $editor(R-$col) <= [ted_projected $E] } {
	$E.table set $row,$editor(R-$col) $value
    } else {
        ted_set $E $row $col $value
    }
}

proc ted_project { E columns } {
	upvar #0 $E editor
	upvar #0 $editor(data) D

    set editor(project) $columns

    set i 1
    foreach c $columns { 
	set c [string trim $c]

	set editor(0,$i) $c
	set editor($c)   $i
	set editor($i)   [$D(colnum) D $c]
	set editor(R-[$D(colnum) D $c]) $i
	incr i
    }

    $E.table configure -cols [expr [ted_projected $E]+1]
}

proc ted_projection { E } { upvar #0 $E editor;  return $editor(project)	   }
proc ted_projected  { E } { upvar #0 $E editor;  return [llength $editor(project)] }

# This command is called from the widget to fill in the matrix
#
proc ted_tablecommand { E set row col value } {
	upvar #0 $E editor
	upvar #0 $editor(data) D

    if { $col == 0 } {
	if { $row == 0 } { 
	    return ""
	} else {
	    return $row
	}
    }

    if { $editor(data) == "None" } {
	return ""
    }
    if { [$D(nrows) D] < $row } {
	return ""
    }

    if { $row == 0 } {
	return $editor(0,$col)
    }

    if { $set } {
	filedirty $E yes
	return [$D(set) D $row $editor($col) $value]
    } else {
	#return [$D(get) D $row $editor($col)]
	return [string range [$D(get) D $row $editor($col)] 0 40]
    }
}

# Table driver wrappers
#
proc tab_nrows   { D }			{ upvar $D data; $data(nrows)   data 			}
proc tab_ncols   { D }			{ upvar $D data; $data(ncols)   data 			}
proc tab_set     { D row col value }	{ upvar $D data; $data(set)     data $row $col $value	}
proc tab_get     { D row col } 		{ upvar $D data; $data(get)     data $row $col 		}
proc tab_colnum  { D name } 		{ upvar $D data; $data(colnum)  data $name 		}
proc tab_colname { D col }		{ upvar $D data; $data(colname) data $col 		}
proc tab_columns { D     }		{ upvar $D data; $data(columns) data  			}
proc tab_coldel  { D col }		{ upvar $D data; $data(coldel)  data $col		}
proc tab_colins  { D name here }	{ upvar $D data; $data(colins)  data $name $here	}
proc tab_colapp  { D name }		{ upvar $D data; $data(colapp)  data $name		}
proc tab_rowdel  { D row }		{ upvar $D data; $data(rowdel)  data $row		}
proc tab_rowins  { D here }		{ upvar $D data; $data(rowins)  data $here		}
proc tab_rowapp  { D row }		{ upvar $D data; $data(rowapp)  data $row		}

proc tab_driver { D Dr } {
	upvar $D  data
	upvar $Dr driver

	set data(nrows)		$driver(nrows)
	set data(ncols)		$driver(ncols)
	set data(get)		$driver(get)
	set data(set)		$driver(set)
	set data(colname)	$driver(colname)
	set data(colnum)	$driver(colnum)
	set data(colins)	$driver(colins)
	set data(coldel)	$driver(coldel)
	set data(colapp)	$driver(colapp)
	set data(rowins)	$driver(rowins)
	set data(rowdel)	$driver(rowdel)
	set data(rowapp)	$driver(rowapp)
}

proc tab_foreachrow { T body } {
    upvar $T D

    set Nrows [tab_nrows D]
    uplevel [subst { 
	for { set row 1 } { \$row <= $Nrows } { incr row } {
	$body
    } }]
}
proc tab_foreachcol { T body } {
    upvar $T D

    set Ncols [tab_ncols D]
    uplevel [subst { 
	for { set col 1 } { \$col <= $Ncols } { incr col } {
	$body
    } }]
}
