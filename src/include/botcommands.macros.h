/* vim: ft=c tabstop=2 shiftwidth=2
 * 
 * Marcoses for botcommands.h
 */
#ifndef __RDximC_botcommands_macros_h
#define __RDximC_botcommands_macros_h

#define BOTCOMMAND_ATTR __attribute__((section("botcommands")))

#define BOTCOMMAND_NEWVAR(_varname, cmdname, cmdfunc) \
	cmdfn_t _varname BOTCOMMAND_ATTR = {.name = cmdname, .func = cmdfunc}

#define BOTCOMMAND_NEW(cmdname, cmdfunc) BOTCOMMAND_NEWVAR(cmdname, #cmdname, cmdfunc)

#endif
