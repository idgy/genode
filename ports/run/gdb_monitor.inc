#
# \brief  GDB command line arguments for setting a breakpoint in the 'main()' function 
# \author Christian Prochaska
# \date   2013-09-04
#

proc gdb_main_breakpoint_cmds { target_binary_name } {

	#
	# We set a break in the 'main()' function of a dynamically linked
	# application by using the following gdb command sequence. It's important that
	# the 'main()' breakpoint gets set before the 'sharedlibrary' command is
	# executed. Otherwise the breakpoint would get set in ld.lib.so's main()
	# function.
	#

	set gdb_cmds ""
		
	# don't ask for y/n when loading a new symbol file
	append gdb_cmds {-ex "set interactive-mode off" }
	
	# load the symbols of ld.lib.so
	append gdb_cmds {-ex "symbol-file bin/ld.lib.so" }
	
	# set a breakpoint in the 'call_main()' function
	append gdb_cmds {-ex "b call_main" }
	
	# continue execution until the breakpoint triggers
	append gdb_cmds {-ex "c" }
	
	# delete the 'call_main()' breakpoint
	append gdb_cmds {-ex "delete 1" }
	
	# load the symbols of the test application
	append gdb_cmds "-ex \"symbol-file bin/$target_binary_name\" "
	
	# set a breakpoint in the application's 'main()' function
	append gdb_cmds {-ex "b main" }
	
	# load the symbols of loaded shared libraries
	append gdb_cmds {-ex "sharedlibrary" }
	
	# continue execution until the breakpoint triggers
	append gdb_cmds {-ex "c" }
	
	# delete the 'main()' breakpoint
	append gdb_cmds {-ex "delete 2" }

	return $gdb_cmds
}
