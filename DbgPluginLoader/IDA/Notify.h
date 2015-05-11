#pragma once

//
// Callback function. IDP module can take appropriate
// actions when some events occurs in the kernel.
//
enum idp_notify
{
	init,                   // The IDP module is just loaded
	// arg - const char *idp_modname
	//       processor module name
	// Processor modules should return zero to indicate failure
	term,                   // The IDP module is being unloaded
	newprc,                 // Before changing proccesor type
	// arg - int processor number in the
	//       array of processor names
	// return 1-ok,0-prohibit
	newasm,                 // Before setting a new assembler
	// arg = int asmnum
	newfile,                // A new file is loaded (already)
	// arg - char * input file name
	oldfile,                // An old file is loaded (already)
	// arg - char * input file name
	newbinary,              // Before loading a binary file
	// args:
	//  char *filename - binary file name
	//  uint32 fileoff  - offset in the file
	//  ea_t basepara  - base loading paragraph
	//  ea_t binoff    - loader offset
	//  uint32 nbytes   - number of bytes to load
	endbinary,              // After loading a binary file
	//  bool ok        - file loaded successfully?
	newseg,                 // A new segment is about to be created
	// arg = segment_t *
	// return 1-ok, 0-segment should not be created
	assemble,               // Assemble an instruction
	// (display a warning if an error is found)
	// args:
	//  ea_t ea -  linear address of instruction
	//  ea_t cs -  cs of instruction
	//  ea_t ip -  ip of instruction
	//  bool use32 - is 32bit segment?
	//  const char *line - line to assemble
	//  uchar *bin - pointer to output opcode buffer
	// returns size of the instruction in bytes
	obsolete_makemicro,     // Generate microcode for the instruction
	// in 'cmd' structure.
	// arg - mblock_t *
	// returns MICRO_... error codes
	outlabel,               // The kernel is going to generate an instruction
	// label line or a function header
	// args:
	//   ea_t ea -
	//   const char *colored_name -
	// If returns value <=0, then the kernel should
	// not generate the label
	FIXME_rename,                 // The kernel is going to rename a byte
	// args:
	//   ea_t ea
	//   const char *new_name
	//   int flags (SN_...)
	// If returns value <=0, then the kernel should
	// not rename it. See also the 'renamed' event
	may_show_sreg,          // The kernel wants to display the segment registers
	// in the messages window.
	// arg - ea_t current_ea
	// if this function returns 0
	// then the kernel will not show
	// the segment registers.
	// (assuming that the module have done it)
	closebase,              // The database will be closed now
	load_idasgn,            // FLIRT signature have been loaded
	// for normal processing (not for
	// recognition of startup sequences)
	// arg - const char *short_sig_name
	// returns: nothing
	coagulate,              // Try to define some unexplored bytes
	// This notification will be called if the
	// kernel tried all possibilities and could
	// not find anything more useful than to
	// convert to array of bytes.
	// The module can help the kernel and convert
	// the bytes into something more useful.
	// arg:
	//      ea_t start_ea
	// returns: number of converted bytes + 1
	auto_empty,             // Info: all analysis queues are empty
	// args: none
	// returns: none
	// This callback is called once when the
	// initial analysis is finished. If the queue is
	// not empty upon the return from this callback,
	// it will be called later again.
	// See also auto_empty_finally.
	auto_queue_empty,       // One analysis queue is empty
	// args: atype_t type
	// returns: 1-yes, keep the queue empty
	//        <=0-no, the queue is not empty anymore
	// This callback can be called many times, so
	// only the autoMark() functions can be used from it
	// (other functions may work but it is not tested)
	func_bounds,            // find_func_bounds() finished its work
	// The module may fine tune the function bounds
	// args: int *possible_return_code
	//       func_t *pfn
	//       ea_t max_func_end_ea (from the kernel's point of view)
	// returns: none
	may_be_func,            // can a function start here?
	// the instruction is in 'cmd'
	// arg: int state -- autoanalysis phase
	//   state == 0: creating functions
	//         == 1: creating chunks
	// returns: probability 0..100
	// the idp module is allowed to modify 'cmd'
	is_sane_insn,           // is the instruction sane for the current file type?
	// arg:  int no_crefs
	// 1: the instruction has no code refs to it.
	//    ida just tries to convert unexplored bytes
	//    to an instruction (but there is no other
	//    reason to convert them into an instruction)
	// 0: the instruction is created because
	//    of some coderef, user request or another
	//    weighty reason.
	// The instruction is in 'cmd'
	// returns: 1-ok, <=0-no, the instruction isn't
	// likely to appear in the program
	is_jump_func,           // is the function a trivial "jump" function?
	// args:  func_t *pfn
	//        ea_t *jump_target
	//        ea_t *func_pointer
	// returns: 0-no, 1-don't know, 2-yes, see jump_target
	// and func_pointer
	gen_regvar_def,         // generate register variable definition line
	// args:  regvar_t *v
	// returns: 0-ok
	setsgr,                 // The kernel has changed a segment register value
	// args:  ea_t startEA
	//        ea_t endEA
	//        int regnum
	//        sel_t value
	//        sel_t old_value
	//        uchar tag (SR_... values)
	// returns: 1-ok, 0-error
	set_compiler,           // The kernel has changed the compiler information
	// (inf.cc structure)
	is_basic_block_end,     // Is the current instruction end of a basic block?
	// This function should be defined for processors
	// with delayed jump slots. The current instruction
	// is stored in 'cmd'
	// args:  bool call_insn_stops_block
	// returns: 1-unknown, 0-no, 2-yes
	reglink,                // IBM PC only, ignore it
	get_vxd_name,           // IBM PC only, ignore it
	// Get Vxd function name
	// args: int vxdnum
	//       int funcnum
	//       char *outbuf
	// returns: nothing

	// PROCESSOR EXTENSION NOTIFICATIONS
	// They are used to add support of new instructions
	// to the existing processor modules.
	// They should be processed only in notification callbacks
	// set by hook_to_notification_point(HK_IDP,...)
	custom_ana,             // args: none, the address to analyze is in cmd.ea
	//   cmd.ip and cmd.cs are initialized too
	//   cmd.itype must be set >= 0x8000
	//   cmd.size must be set to the instruction length
	//   (good plugin would fill the whole 'cmd' including the operand fields)
	//   in the case of error the cmd structure should be kept intact
	// returns: 1+cmd.size
	custom_out,             // args: none (cmd structure contains information about the instruction)
	//   optional notification
	//   (depends on the processor module)
	//   generates the instruction text using
	//   the printf_line() function
	// returns: 2
	custom_emu,             // args: none (cmd structure contains information about the instruction)
	//   optional notification. if absent,
	//   the instruction is supposed to be an regular one
	//   the kernel will proceed to the analysis of the next instruction
	// returns: 2
	custom_outop,           // args: op_t *op
	//   optional notification to generate operand text. if absent,
	//   the standard operand output function will be called.
	//   the output buffer is inited with init_output_buffer()
	//   and this notification may use out_...() functions from ua.hpp
	//   to form the operand text
	// returns: 2
	custom_mnem,            // args: char *outbuffer, size_t bufsize (cmd structure contains information about the instruction)
	//   optional notification. if absent,
	//   the IDC function GetMnem() won't work
	// returns: 2
	// At least one of custom_out or custom_mnem
	// should be implemented. custom_ana should always be
	// implemented. These custom_... callbacks will be
	// called for all instructions. It is the responsability
	// of the plugin to ignore the undesired callbacks
	// END OF PROCESSOR EXTENSION NOTIFICATIONS

	undefine,               // An item in the database (insn or data) is being deleted
	// args: ea_t ea
	// returns: >0-ok, <=0-the kernel should stop
	// if the return value is positive:
	//   bit0 - ignored
	//   bit1 - do not delete srareas at the item end
	make_code,              // An instruction is being created
	// args: ea_t ea, asize_t size
	// returns: 1-ok, <=0-the kernel should stop
	make_data,              // A data item is being created
	// args: ea_t ea, flags_t flags, tid_t tid, asize_t len
	// returns: 1-ok, <=0-the kernel should stop

	moving_segm,            // May the kernel move the segment?
	// args: segment_t - segment to move
	//       ea_t to   - new segment start address
	//       int flags - combination of MSF_... bits
	// returns: 1-yes, <=0-the kernel should stop
	move_segm,              // A segment is moved
	// Fix processor dependent address sensitive information
	// args: ea_t from  - old segment address
	//       segment_t* - moved segment
	// returns: nothing

	is_call_insn,           // Is the instruction a "call"?
	// ea_t ea  - instruction address
	// returns: 1-unknown, 0-no, 2-yes

	is_ret_insn,            // Is the instruction a "return"?
	// ea_t ea  - instruction address
	// bool strict - 1: report only ret instructions
	//               0: include instructions like "leave"
	//                  which begins the function epilog
	// returns: 1-unknown, 0-no, 2-yes

	get_stkvar_scale_factor,// Should stack variable references be multiplied by
	// a coefficient before being used in the stack frame?
	// Currently used by TMS320C55 because the references into
	// the stack should be multiplied by 2
	// Returns: scaling factor
	// Note: PR_SCALE_STKVARS should be set to use this callback

	create_flat_group,      // Create special segment representing the flat group
	// (to use for PC mainly)
	// args - ea_t image_base, int bitness, sel_t dataseg_sel

	kernel_config_loaded,   // This callback is called when ida.cfg is parsed
	// args - none, returns - nothing

	might_change_sp,        // Does the instruction at 'ea' modify the stack pointer?
	// args: ea_t ea
	// returns: 1-yes, 0-false
	// (not used yet)

	is_alloca_probe,        // Does the function at 'ea' behave as __alloca_probe?
	// args: ea_t ea
	// returns: 2-yes, 1-false

	out_3byte,              // Generate text representation of 3byte data
	// init_out_buffer() is called before this function
	// and all Out... function can be used.
	// uFlag contains the flags.
	// This callback might be implemented by the processor
	// module to generate custom representation of 3byte data.
	// args:
	// ea_t dataea - address of the data item
	// uint32 value - value to output
	// bool analyze_only - only create xrefs if necessary
	//              do not generate text representation
	// returns: 2-yes, 1-false

	get_reg_name,           // Generate text representation of a register
	// int reg        - internal register number as defined in the processor module
	// size_t width   - register width in bytes
	// char *buf      - output buffer
	// size_t bufsize - size of output buffer
	// int reghi      - if not -1 then this function will return the register pair
	// returns: -1 if error, strlen(buf)+2 otherwise
	// Most processor modules do not need to implement this callback
	// It is useful only if ph.regNames[reg] does not provide
	// the correct register names
	savebase,               // The database is being saved. Processor module should
	// save its local data
	gen_asm_or_lst,         // Callback: generating asm or lst file
	// The kernel calls this callback twice, at the beginning
	// and at the end of listing generation. Processor
	// module can intercept this event and adjust its output
	// bool starting - beginning listing generation
	// FILE *fp      - output file
	// bool is_asm   - true:assembler, false:listing
	// int flags     - flags passed to gen_file()
	// gen_outline_t **outline - ptr to ptr to outline callback
	// the outline callback, if defined by gen_asm_or_lst,
	// will be used by the kernel to output the generated lines
	// returns: nothing
	out_src_file_lnnum,     // Callback: generate analog of
	//   #line "file.c" 123
	// directive.
	// const char *file - source file (may be NULL)
	// size_t lnnum     - line number
	// returns: 2-directive has been generated
	get_autocmt,            // Callback: get dynamic auto comment
	// Will be called if the autocomments are enabled
	// and the comment retrieved from ida.int starts with
	// '$!'. 'cmd' contains valid info.
	// char *buf  - output buffer
	// size_t bufsize - output buffer size
	// returns: 2-new comment has been generated
	//          1-callback has not been handled
	//            the buffer must not be changed in this case
	is_insn_table_jump,     // Callback: determine if instruction is a table jump or call
	// If CF_JUMP bit can not describe all kinds of table
	// jumps, please define this callback.
	// It will be called for insns with CF_JUMP bit set.
	// input: cmd structure contains the current instruction
	// returns: 1-yes, 0-no
	auto_empty_finally,     // Info: all analysis queues are empty definitively
	// args: none
	// returns: none
	// This callback is called only once.
	// See also auto_empty.
	loader_finished,        // Event: external file loader finished its work
	// linput_t *li
	// uint16 neflags
	// const char *filetypename
	// Use this event to augment the existing loader functionality
	loader_elf_machine,     // Event: ELF loader machine type checkpoint
	//   linput_t *li
	//   int machine_type
	//   const char **p_procname
	//   proc_def **p_pd (see ldr\elf.h)
	//   set_elf_reloc_t *set_reloc
	// A plugin check of the machine_type. If it is the desired one,
	// the the plugin fills p_procname with the processor name.
	// (one of the names present in ph.psnames)
	// p_pd is used to handle relocations, otherwise can be left untouched
	// set_reloc can be later used by the plugin to specify relocations
	// returns: e_machine value (if it is different from the
	// original e_machine value, procname and p_pd will be ignored
	// and the new value will be used)
	// This event occurs for each newly loaded ELF file
	is_indirect_jump,       // Callback: determine if instruction is an indrect jump
	// If CF_JUMP bit can not describe all jump types
	// jumps, please define this callback.
	// input: cmd structure contains the current instruction
	// returns: 1-use CF_JUMP, 2-no, 3-yes
	verify_noreturn,        // The kernel wants to set 'noreturn' flags for a function
	// func_t *pfn
	// Returns: 1-ok, any other value-do not set 'noreturn' flag
	verify_sp,              // All function instructions have been analyzed
	// Now the processor module can analyze the stack pointer
	// for the whole function
	// input: func_t *pfn
	// Returns: 1-ok, 0-bad stack pointer
	renamed,                // The kernel has renamed a byte
	// args:
	//   ea_t ea
	//   const char *new_name
	//   bool local_name
	// Returns: nothing. See also the 'rename' event
	add_func,               // The kernel has added a function
	// args: func_t *pfn
	// Returns: nothing
	del_func,               // The kernel is about to delete a function
	// args: func_t *pfn
	// Returns: 1-ok,<=0-do not delete
	set_func_start,         // Function chunk start address will be changed
	// args: func_t *pfn
	//       ea_t new_start
	// Returns: 1-ok,<=0-do not change
	set_func_end,           // Function chunk end address will be changed
	// args: func_t *pfn
	//       ea_t new_end
	// Returns: 1-ok,<=0-do not change
	treat_hindering_item,   // An item hinders creation of another item
	// args: ea_t hindering_item_ea
	//       flags_t new_item_flags (0 for code)
	//       ea_t new_item_ea
	//       asize_t new_item_length
	// Returns: 1-no reaction, <=0-the kernel may delete the hindering item
	str2reg,                // Convert a register name to a register number
	// args: const char *regname
	// Returns: register number + 2
	// The register number is the register index in the regNames array
	// Most processor modules do not need to implement this callback
	// It is useful only if ph.regNames[reg] does not provide
	// the correct register names
	create_switch_xrefs,    // Create xrefs for a custom jump table
	// in: ea_t jumpea;        - address of the jump insn
	//     switch_info_ex_t *; - switch information
	// returns: must return 2
	calc_switch_cases,      // Calculate case values and targets for a custom jump table
	// in:  ea_t insn_ea - address of the 'indirect jump' instruction
	//      switch_info_ex_t *si      - switch information
	//      casevec_t *casevec - vector of case values...
	//      evec_t *targets - ...and corresponding target addresses
	// casevec and targets may be NULL
	// returns: 2-ok, 1-failed
	determined_main,        // The main() function has been determined
	// in:  ea_t main - address of the main() function
	// returns: none
	preprocess_chart,       // gui has retrieved a function flow chart
	// in: qflow_chart_t *fc
	// returns: none
	// Plugins may modify the flow chart in this callback
	get_bg_color,           // Get item background color
	// in: ea_t ea, bgcolor_t *color
	// Returns: 1-not implemented, 2-color set
	// Plugins can hook this callback to color disassembly lines
	// dynamically
	validate_flirt_func,    // flirt has recognized a library function
	// this callback can be used by a plugin or proc module
	// to intercept it and validate such a function
	// args: ea_t start_ea
	//       const char *funcname
	// returns: -1-do not create a function,
	//           1-function is validated
	// the idp module is allowed to modify 'cmd'
	get_operand_string,     // Request text string for operand (cli, java, ...)
	// args: int opnum
	//       char *buf
	//       size_t buflen
	// (cmd structure must contain info for the desired insn)
	// opnum is the operand number; -1 means any string operand
	// returns: 1 - no string (or empty string)
	//         >1 - original string length with terminating zero

	// the following 5 events are very low level
	// take care of possible recursion
	add_cref,               // a code reference is being created
	// args: ea_t from, ea_t to, cref_t type
	// returns: <0 - cancel cref creation
	add_dref,               // a data reference is being created
	// args: ea_t from, ea_t to, dref_t type
	// returns: <0 - cancel dref creation
	del_cref,               // a code reference is being deleted
	// args: ea_t from, ea_t to, bool expand
	// returns: <0 - cancel cref deletion
	del_dref,               // a data reference is being deleted
	// args: ea_t from, ea_t to
	// returns: <0 - cancel dref deletion
	coagulate_dref,         // data reference is being analyzed
	// args: ea_t from, ea_t to, bool may_define, ea_t *code_ea
	// plugin may correct code_ea (e.g. for thumb mode refs, we clear the last bit)
	// returns: <0 - cancel dref analysis
	register_custom_fixup,  // request to register a new custom fixup type
	// args: const char *name
	// returns: fixup id + 1
	custom_refinfo,         // called from get_offset_expr, when refinfo_t
	// contains flag REFINFO_CUSTOM. Normally this
	// notification used in a combination with custom_fixup
	// args: ea_t ea, int numop, ea_t* opval, const refinfo_t* ri,
	//       char* buf, size_t bufsize, ea_t* target,
	// ea_t* fullvalue, ea_t from, int getn_flags
	// returns: 2 - buf filled as simple expression
	//          3 - buf filled as complex expression
	//          4 - apply standard processing (with - possible - changed values)
	//     others - can't convert to offset expression

	set_proc_options,       // called if the user specified an option string in the command line:
	//  -p<processor name>:<options>
	// can be used for e.g. setting a processor subtype
	// also called if option string is passed to set_processor_type()
	// and IDC's SetProcessorType()
	// args: const char * options
	// returns: <0 - bad option string

	adjust_libfunc_ea,      // called when a signature module has been matched against
	// bytes in the database. This is used to compute the
	// offset at which a particular module's libfunc should
	// be applied.
	// args: const idasgn_t *sig, const libfunc_t *libfun, ea_t *ea
	// returns: 2 - the ea_t pointed to by the third argument was modified.
	//     others - not modified. Use default algorithm.
	//
	// Note: "*ea" initially contains the ea_t of the start
	//       of the pattern match.

	extlang_changed,        // the list of extlangs or the default extlang was changed
	// int kind
	//    0 - extlang installed
	//    1 - extlang removed
	//    2 - default extlang changed
	// const extlang_t *el - pointer to the extlang affected
	// Returns: nothing

	last_cb_before_debugger,
	// START OF DEBUGGER CALLBACKS
	obsolete_get_operand_info = 100, // Get operand information
	// same as get_operand_info (below), but uses
	// idd_opinfo_old_t* as the last argument (less info)

	OBSOLETE(get_reg_info), // the same as get_reg_info2 but the
	// position and size of the subvalue is defined
	// by uint64 (not bitrange_t) and
	// the 3d argument is uint64 *mask (0-no mask)
	// example: "ah" returns main_regname="eax" and mask=0xFF00
#ifdef NO_OBSOLETE_FUNCS        //
	next_exec_insn,         // Get next address to be executed
#else                           //
	get_jump_target,        // Get jump target (old name)
#endif                          //
	// ea_t ea                   - instruction address
	// int tid                   - current therad id
	// const regval_t &(*idaapi getreg)(const char *name,
	//                                  const regval_t *regvalues))
	//                           - function to get register values
	// const regval_t *regvalues - register values array
	// ea_t *target              - pointer to the answer
	// This function must return the next address to be executed.
	// If the instruction following the current one is executed, then it must return BADADDR
	// Usually the instructions to consider are: jumps, branches, calls, returns
	// This function is essential if the 'single step' is not supported in hardware
	// returns: 1-unimplemented, 0-implemented

	calc_step_over,         // Calculate the address of the instruction which will be
	// executed after "step over". The kernel will put a breakpoint there.
	// If the step over is equal to step into or we can not calculate
	// the address, return BADADDR.
	// ea_t ip - instruction address
	// ea_t *target - pointer to the answer
	// returns: 1-unimplemented, 0-implemented

	get_macro_insn_head,    // Calculate the start of a macro instruction
	// This notification is called if IP points to the middle of an instruction
	// ea_t ip - instruction address
	// ea_t *head - answer, BADADDR means normal instruction
	// returns: 1-unimplemented, 0-implemented

	get_dbr_opnum,          // Get the number of the operand to be displayed in the
	// debugger reference view (text mode)
	// ea_t ea - instruction address
	// int *opnum - operand number (out, -1 means no such operand)
	// returns: 1-unimplemented, 0-implemented

	insn_reads_tbit,        // Check if insn will read the TF bit
	// ea_t ea - instruction address
	// const regval_t &(*idaapi getreg)(const char *name,
	//                                  const regval_t *regvalues))
	//                           - function to get register values
	// const regval_t *regvalues - register values array
	// returns: 3-yes, will generate 'step' exception
	//          2-yes, will store the TF bit in memory
	//          1-no

	get_operand_info,       // Get operand information.
	// This callback is used to calculate the operand
	// value for double clicking on it, hints, etc
	// ea_t ea  - instruction address
	// int n    - operand number
	// int thread_id - current thread id
	// const regval_t &(*idaapi getreg)(const char *name,
	//                                  const regval_t *regvalues))
	//                           - function to get register values
	// const regval_t *regvalues - register values array
	// idd_opinfo_t *opinf       - the output buffer
	// returns: 0-ok, otherwise failed

	calc_next_eas,          // Calculate list of addresses the instruction in cmd
	// may pass control to.
	// bool over - calculate for step over (ignore call targets)
	// ea_t *res - array for the results.
	//             This array has NEXTEAS_ANSWER_SIZE elements.
	// int *nsubcalls - number of addresses of called functions
	//                  in the above array. they must be put
	//                  at the beginning of the array.
	//                  if over=true, this answer will be zero.
	// returns: number of calculated addresses+1
	// If there are too many addresses or they are
	// incalculable (indirect jumps, for example), return -1.
	// This callback is required for source level debugging.

	clean_tbit,             // Clear the TF bit after an insn like pushf stored it in memory
	// ea_t ea - instruction address
	// const regval_t &(*idaapi getreg)(const char *name,
	//                                  const regval_t *regvalues))
	//                           - function to get register values
	// const regval_t *regvalues - register values array
	// returns: 2-ok, 1-failed

	get_reg_info2,          // Get register information by its name
	// const char *regname
	// const char **main_regname (NULL-failed)
	// bitrange_t *bitrange - position and size of the value within 'main_regname' (empty bitrange == whole register)
	// returns: 1-unimplemented, 2-implemented
	// example: "ah" returns:
	//   main_regname="eax"
	//   bitrange_t = { offset==8, nbits==8 }
	// this callback may be unimplemented if the register
	// names are all present in ph.regNames and they all have
	// the same size

	// END OF DEBUGGER CALLBACKS

	// START OF TYPEINFO CALLBACKS
	// The codes below will be called only if PR_TYPEINFO is set.
	// The codes based_ptr, max_ptr_size, get_default_enum_size MUST be implemented.
	// (other codes are optional but still require for normal
	// operation of the type system. without calc_arglocs3,
	// for example, ida will not know about the argument
	// locations for function calls.

	OBSOLETE(decorate_name) = 500,
	// Decorate/undecorate a C symbol name
	// const til_t *ti    - pointer to til
	// const char *name   - name of symbol
	// const type_t *type - type of symbol. If NULL then it will try to guess.
	// char *outbuf       - output buffer
	// size_t bufsize     - size of the output buffer
	// bool mangle        - true-mangle, false-unmangle
	// cm_t cc            - real calling convention for VOIDARG functions
	// returns: true if success

	setup_til,              // Setup default type libraries (called after loading
	// a new file into the database)
	// The processor module may load tils, setup memory
	// model and perform other actions required to set up
	// the type system
	// args:    none
	// returns: nothing
	// Optional callback

	based_ptr,              // get prefix and size of 'segment based' ptr
	// type (something like char _ss *ptr)
	// see description in typeinf.hpp
	// args:  unsigned ptrt
	//        const char **ptrname (output arg)
	// returns: size of type

	max_ptr_size,           // get maximal size of a pointer in bytes
	// args:  none
	// returns: max possible size of a pointer plus 1

	get_default_enum_size,  // get default enum size
	// args:  cm_t cm
	// returns: sizeof(enum)

	OBSOLETE(calc_arglocs),
	OBSOLETE(use_stkarg_type),
	OBSOLETE(use_regarg_type),
	OBSOLETE(use_arg_types),
	OBSOLETE(get_fastcall_regs),
	OBSOLETE(get_thiscall_regs),
	OBSOLETE(calc_cdecl_purged_bytes),
	OBSOLETE(get_stkarg_offset),
	OBSOLETE(calc_purged_bytes),
	OBSOLETE(calc_arglocs2),
	OBSOLETE(calc_retloc),
	OBSOLETE(calc_varglocs),
	OBSOLETE(get_varcall_regs),
	OBSOLETE(use_regarg_type2),
	OBSOLETE(use_arg_types2),
	OBSOLETE(get_fastcall_regs2),
	OBSOLETE(get_thiscall_regs2),
	OBSOLETE(get_varcall_regs2),

	calc_cdecl_purged_bytes2,// calculate number of purged bytes after call
	// args: ea_t - address of the call instruction
	// returns: number of purged bytes+2 (usually add sp, N)

	get_stkarg_offset2,     // get offset from SP to the first stack argument
	// args: none
	// returns: the offset + 2
	// for example: pc: 0, hppa: -0x34, ppc: 0x38

	til_for_file,           // internal notification, do not use

	equal_reglocs,          // are 2 register arglocs the same?
	// args:    argloc_t *a1
	//          argloc_t *a2
	// we need this callback for the pc module
	// returns: 1-not implemented, 2-yes, -1-no

	decorate_name3,         // Decorate/undecorate a C symbol name
	// qstring *outbuf    - output buffer
	// const char *name   - name of symbol
	// bool mangle        - true-mangle, false-unmangle
	// cm_t cc            - calling convention
	// returns: 2 if success
	// the kernel uses this callback only if PR_TINFO is set

	calc_retloc3,           // calculate return value location
	// args:
	//   const tinfo_t *rettype
	//   cm_t cc
	//   argloc_t *retloc - the result
	// returns: 1-not implemented, 2-ok, -1-error
	// the kernel uses this callback only if PR_TINFO is set

	calc_varglocs3,         // calculate locations of the arguments that correspond to '...'
	// args:
	//   const func_type_data_t *ftd - info about all arguments (including varargs)
	//   regobjs_t *regs             - buffer for register values
	//   relobj_t *stkargs           - stack arguments
	//   int nfixed                  - number of fixed arguments
	// returns: 1-not implemented, 2-ok, -1-error
	// the kernel uses this callback only if PR_TINFO is set

	calc_arglocs3,          // calculate function argument locations
	// args:    func_type_data_t *fti - points to the func type info
	// the callback should fill retloc, all arglocs, and stkargs
	// This callback supersedes calc_argloc2.
	// returns: 1-not implemented, 2-ok, -1-error
	// this callback is never called for CM_CC_SPECIAL functions
	// the kernel uses this callback only if PR_TINFO is set

	use_stkarg_type3,       // use information about a stack argument
	// args:
	//   ea_t ea            - address of the push instruction which
	//                        pushes the function argument into the stack
	//   const funcarg_t *arg - argument info
	// returns: true - ok, false - failed, the kernel will create
	//          a comment with the argument name or type for the instruction
	// the kernel uses this callback only if PR_TINFO is set

	use_regarg_type3,       // use information about register argument
	// args:
	//   int *idx             - pointer to the returned value
	//   ea_t ea              - address of the instruction
	//   const funcargvec_t *rargs - vector of register arguments
	//                               (including regs extracted from scattered arguments)
	// at the end, *idx may contain the following values:
	//   idx of the used argument - if the argument is defined in the current instruction
	//                              a comment will be applied by the kernel
	//   idx|REG_SPOIL            - argument is spoiled by the instruction
	//   -1                       - if the instruction doesn't change any registers
	//   -2                       - if the instruction spoils all registers
	// returns: 2
	// the kernel uses this callback only if PR_TINFO is set

	use_arg_types3,         // use information about callee arguments
	// args:
	//   ea_t ea              - address of the call instruction
	//   func_type_data *fti  - info about function type
	//   funcargvec_t *rargs  - array of register arguments
	// returns: 2 (and removes handled arguments from fti and rargs)
	// the kernel uses this callback only if PR_TINFO and PR_USE_ARG_TYPES are set

	calc_purged_bytes3,     // calculate number of purged bytes by the given function type
	// args:
	//   int *p_purged_bytes - ptr to output
	//   const func_type_data_t *fti - func type details
	// returns: 2
	// the kernel uses this callback only if PR_TINFO is set

	shadow_args_size,       // get size of shadow args in bytes
	// args:
	//   int *shadow_args_size
	//   func_t *pfn (may be NULL)
	// returns: 2 if filled *shadow_args_size

	get_varcall_regs3,      // get register allocation convention used in the ellipsis (...) calling convention
	// args: callregs_t * - see typeinf.hpp
	// returns: max_possible_number_of_varcall_regs+2

	get_fastcall_regs3,     // get register allocation convention used in the fastcall calling convention
	// args: callregs_t * - see typeinf.hpp
	// returns: max_possible_number_of_fastcall_regs+2

	get_thiscall_regs3,     // get register allocation convention used in the thiscall calling convention
	// args: callregs_t * - see typeinf.hpp
	// returns: max_possible_number_of_thiscall_regs+2
	// END OF TYPEINFO CALLBACKS

	loader = 1000,            // this code and higher ones are reserved
	// for the loaders.
	// the arguments and the return values are
	// defined by the loaders
};

//int (idaapi *notify)(idp_notify msgid, ...); // Various notifications for the idp

//--------------------------------------------------------------------------
// A plugin can hook to the notification point and receive notifications
// of all major events in IDA. The callback function will be called
// for each event. The parameters of the callback:
//      user_data         - data supplied in call to hook_to_notification_point()
//      notification_code - idp_notify or ui_notification_t code, depending on
//                          the hoot type
//      va                - additional parameters supplied with the notification
//                          see the event descriptions for information
// The callback should return:
//      0   - ok, the event should be processed further
//      !=0 - the event is blocked and should be discarded
//            in the case of processor modules, the returned value is used
//            as the return value of notify()

typedef int idaapi hook_cb_t(void *user_data, int notification_code, va_list va);

enum hook_type_t
{
	HT_IDP,         // Hook to the processor module.
	// The callback will receive all idp_notify events.
	// See file idp.hpp for the list of events.
	HT_UI,          // Hook to the user interface.
	// The callback will receive all ui_notification_t events.
	// See file kernwin.hpp for the list of events.
	HT_DBG,         // Hook to the debugger.
	// The callback will receive all dbg_notification_t events.
	// See file dbg.hpp for the list of events.
	HT_IDB,         // Hook to the database events.
	// These events are separated from the HT_IDP group
	// to speed up things (there are too many plugins and
	// modules hooking to the HT_IDP). Some essential events
	// are still generated in th HT_IDP group:
	// make_code, make_data, undefine, rename, add_func, del_func.
	// This list is not exhaustive.
	// A common trait of all events in this group: the kernel
	// does not expect any reaction to the event and does not
	// check the return code. For event names, see the idb_event
	// in idp.hpp
	HT_DEV,         // Internal debugger events.
	// Not stable and undocumented for the moment
	HT_VIEW,        // Custom/IDA views notifications.
	// Refer to kernwin.hpp view_notification_t
	// enum for notification codes
	HT_OUTPUT,      // Output window notifications.
	// Refer to kernwin.hpp msg_notification_t
	// (viewer_notification_t)
	HT_LAST
};

idaman bool ida_export hook_to_notification_point(hook_type_t hook_type,
	hook_cb_t *cb,
	void *user_data);


// The plugin should unhook before being unloaded:
// (preferably in its termination function)
// Returns number of unhooked functions.
// If different callbacks have the same callback function pointer
// and user_data is not NULL, only the callback whose associated
// user defined data matchs will be removed.

idaman int ida_export unhook_from_notification_point(hook_type_t hook_type,
	hook_cb_t *cb,
	void *user_data = NULL);