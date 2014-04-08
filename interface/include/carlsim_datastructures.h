/*
 * Copyright (c) 2014 Regents of the University of California. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. The names of its contributors may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * *********************************************************************************************** *
 * CARLsim
 * created by: 		(MDR) Micah Richert, (JN) Jayram M. Nageswaran
 * maintained by:	(MA) Mike Avery <averym@uci.edu>, (MB) Michael Beyeler <mbeyeler@uci.edu>,
 *					(KDC) Kristofor Carlson <kdcarlso@uci.edu>
 *					(TSC) Ting-Shuo Chou <tingshuc@uci.edu>
 *
 * CARLsim available from http://socsci.uci.edu/~jkrichma/CARLsim/
 * Ver 2/21/2014
 */

#ifndef _CARLSIM_DATASTRUCTURES_H_
#define _CARLSIM_DATASTRUCTURES_H_

/*!
 * \brief Logger modes
 * The logger mode defines where to print all status, error, and debug messages. Several predefined
 * modes exist (USER, DEVELOPER, SHOWTIME, SILENT). However, the user can also set each file pointer to a
 * location of their choice (CUSTOM mode).
 * The following logger modes exist:
 *  USER 		User mode, for experiment-oriented simulations. Errors and warnings go to stderr,
 *              status information goes to stdout. Debug information can only be found in the log file.
 *  DEVELOPER   Developer mode, for developing and debugging code. Same as user, but additionally,
 *              all debug information is printed to stdout.
 *  SHOWTIME    Showtime mode, will only output warnings and errors. 
 *  SILENT      Silent mode, no output is generated.
 *  CUSTOM      Custom mode, the user can set the location of all the file pointers.
 *
 * The following file pointers exist:
 *  fpOut_	where CARLSIM_INFO messages go
 *  fpErr_ 	where CARLSIM_ERROR and CARLSIM_WARN messages go
 *  fpDeb_ 	where CARLSIM_DEBUG messages go
 *  fpLog_ 	typically a log file, where all of the above messages go
 *
 * The file pointers are automatically set to different locations, depending on the loggerMode:
 *
 *          |    USER    | DEVELOPER  |  SHOWTIME  |   SILENT   |  CUSTOM
 * ---------|------------|------------|------------|------------|---------
 * fpOut_   |   stdout   |   stdout   | /dev/null  | /dev/null  |    ?
 * fpErr_   |   stderr   |   stderr   |   stderr   | /dev/null  |    ?
 * fpDeb_   | /dev/null  |   stdout   | /dev/null  | /dev/null  |    ?
 * fpLog_   | debug.log  | debug.log  | debug.log  | /dev/null  |    ?
 *
 * Location of the debug log file can be set in any mode using CARLsim::setLogDebugFp.
 * In mode CUSTOM, the other file pointers can be set using CARLsim::setLogsFp.
 */
enum loggerMode_t {
	 USER,  DEVELOPER,  SHOWTIME,  SILENT,  CUSTOM
};
static const char* loggerMode_string[] = {
	"USER","DEVELOPER","SHOWTIME","SILENT","CUSTOM"
};

/*!
 * \brief simulation mode
 * CARLsim supports execution either on standard x86 central processing units (CPUs) or off-the-shelf NVIDIA GPUs.
 *
 * When creating a new CARLsim object, you can choose from the following:
 * CPU_MODE:	run on a single CPU core
 * GPU_MODE:	run on a single GPU card
 *
 * When running GPU mode on a multi-GPU system, you can specify on which CUDA device to establish a context (ithGPU,
 * 0-indexed) when you create a new CpuSNN object.
 * The simulation mode will be fixed throughout the lifetime of a CpuSNN object.
 */
enum simMode_t {
	 CPU_MODE,  GPU_MODE
};
static const char* simMode_string[] = {
	"CPU Mode","GPU Mode"
};

#endif