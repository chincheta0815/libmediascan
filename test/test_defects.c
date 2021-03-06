#include <stdio.h>
#include <string.h>


#ifdef WIN32
#include <Windows.h>
#include <direct.h>
#else
#include <fcntl.h>
#include <sys/wait.h>
#define _rmdir rmdir
#define _mkdir mkdir
#define CopyFile copyfile
#define DeleteFile deletefile
#endif

#include <limits.h>
#include <libmediascan.h>

#include "../src/mediascan.h"
#include "../src/common.h"
#include "../src/database.h"
#include "CUnit/CUnit/Headers/Basic.h"

#ifndef WIN32

void Sleep(int ms)
{
	usleep(ms * 1000); // Convert to usec
} /* Sleep() */

int copyfile(char *source, char *dest, int not_used)
{
    int childExitStatus;
    pid_t pid;

    if (!source || !dest) {
        return FALSE;
    }

    pid = fork();

    if (pid == 0) { /* child */
        execl("/bin/cp", "/bin/cp", source, dest, (char *)0);
    }
    else if (pid < 0) {
        return FALSE;
    }
    else {
        /* parent - wait for child - this has all error handling, you
         * could just call wait() as long as you are only expecting to
         * have one child process at a time.
         */
        pid_t ws = waitpid( pid, &childExitStatus, WNOHANG);
        if (ws == -1)
        {
        return FALSE;
        }

        if( WIFEXITED(childExitStatus)) /* exit code in childExitStatus */
        {
            int status = WEXITSTATUS(childExitStatus); /* zero is normal exit */
            /* handle non-zero as you wish */
        }
    }
    return TRUE;
} /* copyfile() */

int deletefile(char *source)
{
    int childExitStatus;
    pid_t pid;

    if (!source) {
        return FALSE;
    }

    pid = fork();

    if (pid == 0) { /* child */
        execl("/bin/rm", "/bin/rm", source, (char *)0);
    }
    else if (pid < 0) {
        return FALSE;
    }
    else {
        /* parent - wait for child - this has all error handling, you
         * could just call wait() as long as you are only expecting to
         * have one child process at a time.
         */
        pid_t ws = waitpid( pid, &childExitStatus, WNOHANG);
        if (ws == -1)
        {
        return FALSE;
        }

        if( WIFEXITED(childExitStatus)) /* exit code in childExitStatus */
        {
            int status = WEXITSTATUS(childExitStatus); /* zero is normal exit */
            /* handle non-zero as you wish */
        }
    }
    return TRUE;
} /* deletefile() */

#endif

static int result_called = 0;
static int finish_called = 0;
static MediaScanResult result;
static int do_dump = 0;

static void my_result_callback(MediaScan *s, MediaScanResult *r, void *userdata) {

	result.type = r->type;
	result.path = strdup(r->path);
	result.flags = r->flags;

	fprintf(stderr, "my_result_callback for %s\n", result.path);

	if(r->error)
		memcpy(result.error, r->error, sizeof(MediaScanError));

	result.mime_type = strdup(r->mime_type);
	result.dlna_profile = strdup(r->dlna_profile);
	result.size = r->size;
	result.mtime = r->mtime;
	result.bitrate = r->bitrate;
	result.duration_ms = r->duration_ms;

	if(r->audio)
	{
		result.audio = malloc(sizeof(MediaScanAudio));
		memcpy( result.audio, r->audio, sizeof(MediaScanAudio));
	}

	if(r->video)
	{
		result.video = malloc(sizeof(MediaScanVideo));
		memcpy( result.video, r->video, sizeof(MediaScanVideo));
	}

	if(r->image)
	{
		result.image = malloc(sizeof(MediaScanImage));
		memcpy( result.image, r->image, sizeof(MediaScanImage));
	}

	result_called++;

	if(do_dump)
		ms_dump_result(r);

} /* my_result_callback() */

static void my_error_callback(MediaScan *s, MediaScanError *error, void *userdata) {


fprintf(stderr, "my_error_callback, err=%d, averr=%d\n", error->error_code, error->averror);

} /* my_error_callback() */

static void my_finish_callback(MediaScan *s, void *userdata) {
  fprintf(stderr, "finish_callback\n");
	finish_called = TRUE;
} /* my_finish_callback() */


static void test_defect_21069(void)	{
	const char *test_path = "data\\defect_21069";
	MediaScan *s;

	s = ms_create();

	CU_ASSERT_FATAL(s != NULL);

	/* Test #1 of the initial scan functionality */
	ms_set_flags(s, MS_RESCAN | MS_CLEARDB);
	ms_set_log_level(DEBUG);
	// Do some setup for the test
	result_called = 0;
	ms_errno = 0;
	do_dump = 0;
	CU_ASSERT(s->on_result == NULL);
	ms_set_result_callback(s, my_result_callback);
	CU_ASSERT(s->on_result == my_result_callback);

	CU_ASSERT(s->on_error == NULL);
	ms_set_error_callback(s, my_error_callback);
	CU_ASSERT(s->on_error == my_error_callback);

	CU_ASSERT(s->npaths == 0);
	ms_add_path(s, test_path);
	CU_ASSERT(s->npaths == 1);

	ms_scan(s);
	CU_ASSERT( result_called == 2 );

	ms_destroy(s);


} /* test_defect_21069() */


static void test_defect_19701(void)	{
	const char *test_path = "data\\defect_19701";
	MediaScan *s;

	s = ms_create();

	CU_ASSERT_FATAL(s != NULL);

	/* Test #1 of the initial scan functionality */
	ms_set_flags(s, MS_RESCAN | MS_CLEARDB);
	ms_set_log_level(DEBUG);
	ms_add_thumbnail_spec(s, THUMB_JPEG, 100,100, TRUE, 0, 90);
	// Do some setup for the test
	result_called = 0;
	ms_errno = 0;
	do_dump = 1;

	CU_ASSERT(s->on_result == NULL);
	ms_set_result_callback(s, my_result_callback);
	CU_ASSERT(s->on_result == my_result_callback);

	CU_ASSERT(s->on_error == NULL);
	ms_set_error_callback(s, my_error_callback);
	CU_ASSERT(s->on_error == my_error_callback);

	CU_ASSERT(s->npaths == 0);
	ms_add_path(s, test_path);
	CU_ASSERT(s->npaths == 1);

	ms_scan(s);
	CU_ASSERT( result_called == 3 );

	ms_destroy(s);


} /* test_defect_19701() */

static void test_defect_21070(void)	{
	const char *test_path = "data\\defect_21070";
	MediaScan *s;

	s = ms_create();

	CU_ASSERT_FATAL(s != NULL);

	/* Test #1 of the initial scan functionality */
	ms_set_flags(s, MS_RESCAN | MS_CLEARDB);
	ms_set_log_level(DEBUG);
	ms_add_thumbnail_spec(s, THUMB_JPEG, 100,100, TRUE, 0, 90);
	// Do some setup for the test
	result_called = 0;
	ms_errno = 0;
	do_dump = 1;

	CU_ASSERT(s->on_result == NULL);
	ms_set_result_callback(s, my_result_callback);
	CU_ASSERT(s->on_result == my_result_callback);

	CU_ASSERT(s->on_error == NULL);
	ms_set_error_callback(s, my_error_callback);
	CU_ASSERT(s->on_error == my_error_callback);

	CU_ASSERT(s->npaths == 0);
	ms_add_path(s, test_path);
	CU_ASSERT(s->npaths == 1);

	ms_scan(s);
	CU_ASSERT( result_called == 1 );

	ms_destroy(s);


} /* test_defect_21070() */

static void test_defect_21416(void)	{
	const char *test_path = "data\\defect_21416";
	MediaScan *s;

	s = ms_create();

	CU_ASSERT_FATAL(s != NULL);

	/* Test #1 of the initial scan functionality */
	ms_set_flags(s, MS_RESCAN | MS_CLEARDB);
	ms_set_log_level(DEBUG);
	ms_add_thumbnail_spec(s, THUMB_JPEG, 100,100, TRUE, 0, 90);
	// Do some setup for the test
	result_called = 0;
	ms_errno = 0;
	do_dump = 1;

	CU_ASSERT(s->on_result == NULL);
	ms_set_result_callback(s, my_result_callback);
	CU_ASSERT(s->on_result == my_result_callback);

	CU_ASSERT(s->on_error == NULL);
	ms_set_error_callback(s, my_error_callback);
	CU_ASSERT(s->on_error == my_error_callback);

	CU_ASSERT(s->npaths == 0);
	ms_add_path(s, test_path);
	CU_ASSERT(s->npaths == 1);

	ms_scan(s);
	CU_ASSERT( result_called == 1 );

	ms_destroy(s);


} /* test_defect_21416() */

static void test_defect_21352(void)	{
	const char *test_path = "data\\defect_21352";
	MediaScan *s;

	s = ms_create();

	CU_ASSERT_FATAL(s != NULL);

	/* Test #1 of the initial scan functionality */
	ms_set_flags(s, MS_RESCAN | MS_CLEARDB);
	ms_set_log_level(DEBUG);
	ms_add_thumbnail_spec(s, THUMB_JPEG, 100,100, TRUE, 0, 90);
	// Do some setup for the test
	result_called = 0;
	ms_errno = 0;
	do_dump = 1;

	CU_ASSERT(s->on_result == NULL);
	ms_set_result_callback(s, my_result_callback);
	CU_ASSERT(s->on_result == my_result_callback);

	CU_ASSERT(s->on_error == NULL);
	ms_set_error_callback(s, my_error_callback);
	CU_ASSERT(s->on_error == my_error_callback);

	CU_ASSERT(s->npaths == 0);
	ms_add_path(s, test_path);
	CU_ASSERT(s->npaths == 1);

	ms_scan(s);
	CU_ASSERT( result_called == 0 );

	ms_destroy(s);


} /* test_defect_21352() */

static void test_defect_21660(void)	{
	const char *test_path = "C:\\Users\\Fox\\Music\\iTunes\\iTunes Media";
	MediaScan *s;

	s = ms_create();

	CU_ASSERT_FATAL(s != NULL);

	ms_set_flags(s, MS_RESCAN | MS_CLEARDB);
	ms_set_log_level(DEBUG);
	ms_add_thumbnail_spec(s, THUMB_JPEG, 100,100, TRUE, 0, 90);
	ms_add_ignore_directory_substring(s, "itlp");
	// Do some setup for the test
	result_called = 0;
	ms_errno = 0;
	do_dump = 1;

	CU_ASSERT(s->on_result == NULL);
	ms_set_result_callback(s, my_result_callback);
	CU_ASSERT(s->on_result == my_result_callback);

	CU_ASSERT(s->on_error == NULL);
	ms_set_error_callback(s, my_error_callback);
	CU_ASSERT(s->on_error == my_error_callback);

	CU_ASSERT(s->npaths == 0);
	ms_add_path(s, test_path);
	CU_ASSERT(s->npaths == 1);

	ms_scan(s);
	CU_ASSERT( result_called == 0 );

	ms_destroy(s);


} /* test_defect_21660() */

static void test_defect__bugzilla_17564(void)	{
	const int depth = 20;
	char test_path[MAX_PATH_STR_LEN];
	char test_path_dest[MAX_PATH_STR_LEN];
	const char start_dir[] = "C:\\logitest";
	const char src_filename[] = "bars-mpeg1video-mp2.mpg";
	const char src_file[] = "data\\video\\bars-mpeg1video-mp2.mpg";
	MediaScan *s = NULL;
	int i = 0;


	s = ms_create();

	CU_ASSERT( _mkdir(start_dir) != -1 );
	strcpy(test_path, start_dir);
	for(i = 0; i < depth; i++)
	{
		// 10 characters
		strcat(test_path, "\\directory0");
		CU_ASSERT( _mkdir(test_path) != -1 );
	}

	CU_ASSERT_FATAL(s != NULL);

	ms_set_flags(s, MS_RESCAN | MS_CLEARDB);
	ms_set_log_level(DEBUG);
	// Do some setup for the test
	result_called = 0;
	ms_errno = 0;
	do_dump = 1;

	CU_ASSERT(s->on_result == NULL);
	ms_set_result_callback(s, my_result_callback);
	CU_ASSERT(s->on_result == my_result_callback);

	CU_ASSERT(s->on_error == NULL);
	ms_set_error_callback(s, my_error_callback);
	CU_ASSERT(s->on_error == my_error_callback);

	CU_ASSERT(s->npaths == 0);
	ms_add_path(s, test_path);
	CU_ASSERT(s->npaths == 1);

	strcpy(test_path_dest, test_path);
	strcat(test_path_dest, "\\");
	strcat(test_path_dest, src_filename);
	CopyFile(src_file, test_path_dest, FALSE);

	printf("\n\n\n\n path %d \n\n\n\n", strlen(test_path_dest));

	ms_scan(s);
	CU_ASSERT( result_called == 1 );

	ms_destroy(s);

	CU_ASSERT( DeleteFile(test_path_dest) == TRUE);
	for(i = 0; i < depth; i++)
	{
		CU_ASSERT( _rmdir(test_path) != -1 );
		test_path[ strlen(test_path) - 11 ] = 0;
	}

 CU_ASSERT( _rmdir(start_dir) != -1 );


} /* test_defect__bugzilla_17564() */

///-------------------------------------------------------------------------------------------------
///  Setup defect tests.
///
/// @author Henry Bennett
/// @date 03/22/2011
///-------------------------------------------------------------------------------------------------

int setupdefect_tests() {
	CU_pSuite pSuite = NULL;


   /* add a suite to the registry */
   pSuite = CU_add_suite("Defect Tests", NULL, NULL);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the background scanning suite */
   if (
//      NULL == CU_add_test(pSuite, "Test defect 21069", test_defect_21069) ||
//      NULL == CU_add_test(pSuite, "Test defect 19701", test_defect_19701) ||
//      NULL == CU_add_test(pSuite, "Test defect 21070", test_defect_21070) ||
//      NULL == CU_add_test(pSuite, "Test defect 21416", test_defect_21416) ||
//      NULL == CU_add_test(pSuite, "Test defect 21352", test_defect_21352) ||
//      NULL == CU_add_test(pSuite, "Test defect 21660", test_defect_21660) ||
      NULL == CU_add_test(pSuite, "Test defect Bugzilla 17564", test_defect__bugzilla_17564)

	   )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   return 0;

} /* setupdefect_tests() */
