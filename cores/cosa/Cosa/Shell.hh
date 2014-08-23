/**
 * @file Cosa/Shell.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef COSA_SHELL_HH
#define COSA_SHELL_HH

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"

class Shell {
public:
  /**
   * Shell command privilege levels.
   */
  enum Level {
    GUEST = 0,			//!< Read-only and limited set.
    USER = 1,			//!< Local setting and restricted set.
    ADMIN = 2			//!< Full access.
  } __attribute__((packed));

  /**
   * Shell action funtion and run error codes.
   */
  enum {
    ILLEGAL_COMMAND = -1,	//!< Illegal command.
    PERMISSION_DENIED = -2,	//!< Unsufficent privilege.
    UNKNOWN_COMMAND = -3,	//!< Command not found.
    UNKNOWN_OPTION = -4,	//!< Option not found.
    ILLEGAL_OPTION = -5		//!< Illegal option value.
  };

  /**
   * Shell command action function. Called with number arguments
   * and NULL terminated argument vector. Should return zero(0) if
   * successful otherwise a negative error code; ILLEGAL_COMMAND,
   * PERMISSION_DENIED, UNKNOWN_COMMAND, UNKNOWN_OPTION and 
   * ILLEGAL_OPTION.
   * @param[in] argc argument count.
   * @param[in] argv argument vector.
   * @return zero or negative error code.
   */
  typedef int (*action_fn)(int argc, char* argv[]);

  /**
   * Shell command descriptor with name, help string and action function.
   */
  struct command_t {
    const char* name;		//!< Shell command name string (PROGMEM).
    const char* help;		//!< Short description of command.
    action_fn action;		//!< Shell command action function.
    Level level;		//!< Shell command privilege level.
  };
  
  /**
   * Construct command shell with given command list and prompt.
   * @param[in] cmdc number of commands in vector (max 255).
   * @param[in] cmdtab command table (in program memory).
   * @param[in] prompt to be written to cout.
   */
  Shell(uint8_t cmdc, const command_t* cmdtab, const char* prompt = NULL) :
    m_cmdc(cmdc),
    m_cmdtab(cmdtab),
    m_prompt(prompt == NULL ? DEFAULT_PROMPT : prompt),
    m_firstrun(true),
    m_echo(true),
    m_level(ADMIN)
  {
  }
  
  /**
   * Set local echo mode.
   * @param[in] mode.
   */
  void set_echo(bool mode)
  {
    m_echo = mode;
  }

  /**
   * Get local echo mode.
   * @return mode.
   */
  bool get_echo()
  {
    return (m_echo);
  }

  /**
   * Set new prompt. Pass NULL for default prompt.
   * @param[in] prompt string in program memory.
   */
  void set_prompt(const char* prompt)
  {
    m_prompt = (prompt == NULL ? DEFAULT_PROMPT : prompt);
  }

  /**
   * Get current prompt. Returns pointer to string in program
   * memory.
   * @return prompt.
   */
  const char* get_prompt()
  {
    return (m_prompt);
  }
  
  /**
   * Set new privilege level.
   * @param[in] level.
   */
  void set_privilege(Level level)
  {
    m_level = level;
  }

  /**
   * Get privilege level.
   * @return level.
   */
  Level get_privilege()
  {
    return (m_level);
  }

  /**
   * Validate privilege level.
   * @return bool.
   */
  bool is_privileged(Level level)
  {
    return (m_level >= level);
  }

  /**
   * Reset for new session.
   */
  void reset()
  {
    m_firstrun = true;
  }

  /**
   * Set new command table and associated prompt.
   * @param[in] cmdc number of commands in vector (max 255).
   * @param[in] cmdtab command table (in program memory).
   * @param[in] prompt to be written to cout.
   */
  void set_commands(uint8_t cmdc, const command_t* cmdtab, 
		    const char* prompt = NULL)
  {
    m_cmdc = cmdc;
    m_cmdtab = cmdtab;
    m_prompt = (prompt == NULL ? DEFAULT_PROMPT : prompt);
  }
  
  /**
   * Parse command parameter list for options. The command has the
   * format: NAME -X -XVALUE OPTION=VALUE ARGUMENT.., where X is an
   * option character with or without VALUE string, OPTION is an
   * option name (string), and ARGUMENT is the first non-option. 
   * Returns zero and option string and value if successful otherwise
   * the index of the first argument in the argument vector. 
   * @param[out] option string.
   * @param[out] value string.
   * @return zero or index of first argument.
   */
  int get(char* &option, char* &value);

  /**
   * Parse buffer and create command, option and parameter
   * list. Lookup command in given command vector. If found call
   * action function with arguments and count. Return value from 
   * action function or negative error code if not found.
   * @param[in] buf command line (zero terminated string).
   * @return value from action function or negative error code.
   */
  int execute(char* buf);

  /**
   * Prompt and read line from given stream and execute command when a
   * line has been completed. Return zero if commmand was not
   * completed, one if executed or negative error code. 
   * @param[in] ios in- and output stream.
   * @return zero or negative error code.
   */
  int run(IOStream& ios);

  /**
   * Print short description of commands to the given output
   * stream. Return zero or negative error code.
   * @param[in] outs output stream.
   * @return zero or negative error code.
   */
  int help(IOStream& outs);

  /**
   * @override Shell
   * Print prompt to given output stream. Default implementation will
   * print instance prompt string (in program memory).
   * @param[in] outs output stream.
   */
  virtual void prompt(IOStream& outs);
  
protected:
  /** Default prompt */
  static const char DEFAULT_PROMPT[] PROGMEM;

  /** Max command line buffer size */
  static const size_t BUF_MAX = 64;

  /** Max number of arguments (options and parameters) */
  static const size_t ARGV_MAX = 16;

  uint8_t m_cmdc;		//!< Number of shell commands.
  const command_t* m_cmdtab;	//!< Vector with shell command decriptors.
  const char* m_prompt;		//!< Shell prompt.
  bool m_firstrun;		//!< First time run.
  bool m_echo;			//!< Echo mode.
  Level m_level;		//!< Privilege level.
  char m_buf[BUF_MAX];		//!< Command buffer.
  uint8_t m_argc;		//!< Number of arguments.
  char** m_argv;		//!< Argument vector.
  uint8_t m_optind;		//!< Next option index.
  bool m_optend;		//!< End of options.
  
  /**
   * Lookup given command name in command set. Return command entry 
   * in program memory or NULL.
   * @param[in] name of shell command.
   * @return command entry or NULL.
   */
  const command_t* lookup(char* name);

  /**
   * Execute script in program memory. Return zero or the script
   * command line number of the failed. 
   * @param[in] sp pointer to script in program memory.
   * @param[in] argc argument count.
   * @param[in] argv argument vector.
   * @return zero or script line number.
   */
  int script(const char* sp, int argc, char* argv[]);
};

/** 
 * Shell script magic marker. 
 */
#define SHELL_SCRIPT_MAGIC "#!Cosa/Shell\n" 

#endif
