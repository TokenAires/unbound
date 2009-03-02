/*
 * winrc/unbound-service-install.c - windows services installation util
 *
 * Copyright (c) 2009, NLnet Labs. All rights reserved.
 *
 * This software is open source.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 
 * Neither the name of the NLNET LABS nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file
 *
 * This file contains functions to integrate with the windows services API.
 * This means it handles the commandline switches to install and remove
 * the service (via CreateService and DeleteService), it handles
 * the ServiceMain() main service entry point when started as a service,
 * and it handles the Handler[_ex]() to process requests to the service
 * (such as start and stop and status).
 */
#include "config.h"
#include "winrc/win_svc.h"

/** service name for unbound (internal to ServiceManager) */
#define SERVICE_NAME "unbound"

/** output file for diagnostics */
static FILE* out = NULL;

/** exit with windows error */
static void
fatal_win(const char* str)
{
	fprintf(out, "%s (%d)\n", str, (int)GetLastError());
	exit(1);
}

/** Remove installed service from servicecontrolmanager */
static void 
wsvc_remove(void)
{
	SC_HANDLE scm;
	SC_HANDLE sv;
	fprintf(out, "removing unbound service\n");
	scm = OpenSCManager(NULL, NULL, (int)SC_MANAGER_ALL_ACCESS);
	if(!scm) fatal_win("could not OpenSCManager");
	sv = OpenService(scm, SERVICE_NAME, DELETE);
	if(!sv) {
		CloseServiceHandle(scm);
		fatal_win("could not OpenService");
	}
	if(!DeleteService(sv)) {
		fatal_win("could not DeleteService");
	}
	CloseServiceHandle(sv);
	CloseServiceHandle(scm);
	fprintf(out, "unbound service removed\n");
}

/** Install service main */
int main(int ATTR_UNUSED(argc), char** ATTR_UNUSED(argv))
{
	out = fopen("unbound-service-remove.log", "w");
	wsvc_remove();
	return 0;
}
