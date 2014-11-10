#include <EXTERN.h>
#include <perl.h>

#ifndef PERL_DLL
#define PERL_DLL "perl" STRINGIFY(PERL_REVISION) STRINGIFY(PERL_VERSION) ".dll"
#endif

EXTERN_C void boot_DynaLoader(pTHX_ CV* cv);

static PerlInterpreter* my_perl;

static void xs_init(pTHX) {
	// enable dynamic loading perl modules
	newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, __FILE__);
}

void perl_deinit() {
	perl_destruct(my_perl);
	perl_free(my_perl);
	PERL_SYS_TERM();
}

static void perl_init() {
	char *perl_args[] = { "", "-e", "0", "-w" };
	int arg_count = 4;
	char* env[] = { "" };

	static const char def[] = {
#include "api.pm.h"
	};

	PERL_SYS_INIT3(&arg_count, (char ***)&perl_args, (char ***)&env);
	my_perl = perl_alloc();
	perl_construct(my_perl);
	PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
	perl_parse(my_perl, xs_init, arg_count, perl_args, (char**)NULL);

	eval_pv(def, TRUE);
}

static int execute_perl(SV* function, char* args) {
	int count, ret = 1;

	// init stack pointer
	dSP;
	// everything created here
	ENTER;
	// is temp var
	SAVETMPS;

	PUSHMARK(SP);

	// push to stack
	XPUSHs(sv_2mortal(newSVpv(args, 0)));
	// make stack pointer global
	PUTBACK;

	count = call_sv(function, G_EVAL | G_SCALAR);
	// refresh SP
	SPAGAIN;

	// dirty hack
	if (count == 0 && SvTRUE(ERRSV)) {
		printf("Perl error: %s\n", SvPV_nolen(ERRSV));
		// remove undef 
		if (!SvOK(POPs))
		{
		}
	}
	else if (count != 1) {
		printf("Perl error: expected value from %s, got %d\n", SvPV_nolen(function), count);
	}
	else {
		// pop integer
		ret = POPi;
	}

	PUTBACK;
	// destroy tmp vars
	FREETMPS;
	// ret
	LEAVE;

	return ret;
}

// uses perl to load file
#ifdef WIN32
static int perl_load_file(char* scriptName) {
	static HMODULE lib = NULL;
	lib = LoadLibraryA(PERL_DLL);
	if (!lib) {
		printf("Perl missing\n");
		return 0;
	}
	// perl exists
	FreeLibrary(lib);

	return execute_perl(sv_2mortal(newSVpv("InssiJeeJee::load", 0)), scriptName);
}
#endif


int main() {
	perl_init();
	perl_load_file("test.pl");
	perl_deinit();
	return 0;
}