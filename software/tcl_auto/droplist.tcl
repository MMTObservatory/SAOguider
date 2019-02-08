# Drop-down list code.
#
proc set_droplist { w list } {
	upvar #0 $w x

    $x(list) delete 0 end
    set max 0
    foreach item $list {
        $x(list) insert end $item
	set len [string length $item]
	if { $len > $max } { set max $len }
    }

    $x(list) configure -width $max
}

proc droplist { w args } {
	upvar #0 $w x

    set text     {}
    set width    30
    set height    8
    set variable [lindex [split $w .] end]
    set value    {}
    set list     {}
    set command  {}

    foreach { arg val } $args {
	switch -exact -- $arg {
	    -text     { set text     $val }
	    -width    { set width    $val }
	    -height   { set height   $val }
	    -variable { set variable $val }
	    -value    { set value    $val }
	    -list     { set list     $val }
	    -command  { set command  $val }
	}
    }
    global $variable

    set x(list) [Droplist $w $text $width $height $variable $value $command]
    foreach item $list {
        $x(list) insert end $item
    }

    return $w
}

# The main procedure to use is Droplist.
# The test_drop_list procedure shows how to use
# the drop list. The rest of the procedures are
# private to the drop list.
#

# Creates a drop-down list and a few interface widgets
# (label, entry and drop-button) from your supplied basename.
#
# basename = base name for frame that holds entry widget.
# text = text for label widget.
# height = number of visible lines in list.
# width = number of columns in list and entry widget.
# variable = name of variable that gets the selected value.
# initial_value = first value.
#
# Returns name of actual listbox widget, 
# so you can fill in the listbox.
#
proc Droplist { 
    basename text width height variable { initial_value {} } { command {} } } {

    upvar #0 $variable var
    if { [string compare $initial_value {}] } { set var "$initial_value" }

    # Name of top-level widget to create.
    set top $basename.top

    #
    # Widgets to enter data.
    #
    frame $basename -bd 0
    #label $basename.lbl -text $text -anchor e
    entry $basename.ent -width $width -textvariable $variable
    #$basename.ent insert 0 "$initial_value"
    DropButton $basename.drop $basename.top $basename.ent

#    bind $basename.ent <Return> \
#        "DropListSetVal $basename.ent $variable"
 
    bind $basename.ent <Key-Escape> "wm withdraw $top"

    #pack $basename.lbl -side left -fill y
    pack $basename.ent -side left -expand 1 -fill both
    pack $basename.drop -side left -fill both

    #
    # Drop-list is a top-level temporary window.
    #
    toplevel $top -cursor top_left_arrow
    wm overrideredirect $top 1
    wm withdraw $top 

    # Create list
    set frm $top.frame
    frame $frm -bd 4 -relief sunken

    listbox $frm.list -height $height -width $width \
        -selectmode single \
        -yscrollcommand "$frm.scrollbar set" 

    bind $frm.list <Key-Escape> "wm withdraw $top"

    # Create scrollbar
    scrollbar $frm.scrollbar \
        -command "$frm.list yview" 

    pack $frm.scrollbar -side right -fill y
    pack $frm.list      -side left 
    pack $frm -side top 

    bind $frm.list <ButtonRelease-1> \
        "DropListClick $top $basename.ent $variable [list $command]"


    pack $basename

    #
    # Return list widget so you can fill it.
    #
    return $frm.list
}

# Returns selected item for a single-select list.
proc list_selected { listname } {
    set indx [$listname curselection]

    if { $indx != "" } {
        set item [$listname get $indx]

        return $item
    } else {
        return "";
    }
}



# Places value in global variable.
#proc DropListSetVal { entry variable } {
#    upvar #0 $variable var
#
#    set value [$entry get]
#
#    if { $value != "" } {
#        set var $value
#    }
#
#}

# Handles click on drop list widget.
proc DropListClick { basename entry variable command } {

    catch {
        set selected [list_selected $basename.frame.list]

        if { $selected != "" } {
            #
            # Put item into entry widget.
            #
            #$entry delete 0 end
            #$entry insert 0 "$selected"
            #DropListSetVal $entry $variable
	    upvar #0 $variable v
	    set v $selected
	    eval $command [$basename.frame.list curselection]
        }
    }

    wm withdraw $basename
}



# Makes drop list visible. Create with DropListCreate.
proc ShowDropList { basename associated_widget } {
    set x [winfo rootx $associated_widget]
    set w [winfo width $associated_widget]
    set y [winfo rooty $associated_widget]
    set y [expr $y + [winfo height $associated_widget]]

    set cw [$associated_widget cget -width]
    set pc [expr $w / $cw]
    set pd [expr $w - $pc*$cw]

    set cw [$basename.frame.list cget -width]
    set W  [expr $cw * $pc + $pd]

    wm geometry $basename "+[expr $x - ($W - $w)]+$y"
    wm deiconify $basename
    raise $basename

    focus $basename.frame.list 
}


# Creates a button with a drop-down bitmap.
proc DropButton { name toplevel entry } {

    button $name -image dnarrow \
    -command "ShowDropList $toplevel $entry"

    return $name
}

#
# Bitmap data for down arrow bitmap.
#
set dnarrow_data "
#define dnarrow2_width 18
#define dnarrow2_height 18
static unsigned char dnarrow2_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0x00, 0xf8, 0x7f, 0x00,
   0xf8, 0x7f, 0x00, 0xf0, 0x3f, 0x00, 0xf0, 0x3f, 0x00, 0xe0, 0x1f, 0x00,
   0xc0, 0x0f, 0x00, 0xc0, 0x0f, 0x00, 0x80, 0x07, 0x00, 0x80, 0x07, 0x00,
   0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0x00,
   0xfc, 0xff, 0x00, 0x00, 0x00, 0x00};
"
image create bitmap dnarrow -data $dnarrow_data


set drop_test_var "Unset"
global drop_test_var

proc test_value { } {
    global drop_test_var

    puts "Value of variable=$drop_test_var"
    exit
}


# Test procedure creating a drop-down list
proc test_drop_list { } {
    global drop_test_var

      #
      # This drop-list holds font families.
      #
    set families [font families]
    set sorted [lsort $families]

      #
      # Determine initial value.
      # Enclose in quotes because it may
      # have spaces in the value.
      #
    set initial_value [lindex $sorted 0]

      # Create drop list.
    set list [DropListCreate .font "Font: " \
        40 8 drop_test_var "$initial_value"]

      # Fill in drop list with font families.
    foreach family $sorted {
        $list insert end $family
    }

    button .quit -text "Exit" -command test_value
    pack .quit -side bottom
}

  # Comment out next line to remove test code.
#test_drop_list

# droplist.tcl

