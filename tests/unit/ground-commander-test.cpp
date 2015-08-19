/******************************************************************************
*
* AUTHOR : Shawn
*
* FILE : ground-commander-test.cpp
*
* PURPOSE : Test the ground-commander using the Net2Com interface.
*
* CREATION DATE : 2015-05-18
*
******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>  
#include <sys/wait.h>

#include "CppUTest/TestHarness.h"
#include "CppUTest/MemoryLeakDetectorMallocMacros.h"

#include "SpaceDecl.h"
#include "common/command-factory.h"
#include "common/icommand.h"
#include "fileIO.h"
#include "SpaceString.h"
#include "space-commander/Net2Com.h"
#include "dirUtl.h"
#include "UTestUtls.h"

#include "ground-commander-main.h"

#define RESULT_BUF_SIZE 50 // TODO this should be globally defined
#define CMD_BUF_SIZE 25 // TODO this should be globally defined

static char command_buf[CMD_BUF_SIZE] = {'\0'};

#define GROUND_COMMANDER_BIN "bin/ground-commander/ground-commander" // use local bin, not the one unser CS1_APPS

#define UTEST_SIZE_OF_TEST_FILES 6

TEST_GROUP(GroundCommandTestGroup)
{
    	Net2Com *Test_GD_Commander;
	static const int BUFFER_SIZE = 50;

    void setup()
    {
        mkdir(CS1_TGZ, S_IRWXU);
        mkdir(CS1_LOGS, S_IRWXU);
	mkdir(CS1_PIPES, S_IRWXU);
	mkdir(GND_PIPES, S_IRWXU);

	Test_GD_Commander = new Net2Com(GDcom_w_net_r, GDnet_w_com_r, GIcom_w_net_r, GInet_w_com_r);
        pid_t pid = fork();

        if (pid == 0) {
            if(execl("./"GROUND_COMMANDER_BIN, GROUND_COMMANDER_BIN, NULL) == -1){
                fprintf(stderr, "[ERROR] %s:%s:%d ", __FILE__, __func__, __LINE__);
                exit(EXIT_FAILURE);
            }
        }

        // Make sure the commander is running
        while (system("ps aux | grep bin/ground-commander/ground-commander 1>/dev/null") != 0){
           	usleep(2000); 
        }
        memset(command_buf, '\0', CMD_BUF_SIZE);
	fprintf(stderr,"SETUP stage of ground-commander completed \n");
    }
    
    void teardown()
    {
#ifndef PRESERVE
        DeleteDirectoryContent(CS1_TGZ);
        rmdir(CS1_TGZ);

        DeleteDirectoryContent(CS1_LOGS);
        rmdir(CS1_LOGS);
#endif

        if (system("pidof ground-commander | xargs  kill -15") != 0) {
            fprintf(stderr, "[ERROR] pidof ground-commander | xargs -15 kill");
        }
	
	 if (Test_GD_Commander != NULL) {
            delete Test_GD_Commander;
            Test_GD_Commander = NULL;
        }
        DeleteDirectoryContent(CS1_PIPES);
	fprintf(stderr,"TEARDOWN stage completed, Ground-commander process killed and pipes directory deleted\n");
    }
};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * GROUP :  GroundCommanderTestGroup
 * NAME :   ReadCommand_Success 
 * DESC :   This test will make sure commands are read successfully from the 
 *          Command Input File, and that the commands are validated
 *
 *-----------------------------------------------------------------------------*/
TEST(GroundCommandTestGroup, Read_Command_Success) 
{
    //COMMAND_INPUT_PIPE
    // - write a command buffer to the Command Input File
    // - run read_command
    // - check if data pipe was written to, and validate the contents of the 
    //   command that was written
    // - check that the command was removed from the Command Input File
    // - verfiy data_bytes_written return result
    char buffer[BUFFER_SIZE];
//    char buffer_info_from_Pipe [BUFFER_SIZE];
    const char* data = "SomeCOMMAND";
    size_t BytesCommand;
    sleep(5); // Delay in order to give enough time to the "ground-commander" child process to create the cmd_input pipe
    BytesCommand = cmd_input.WriteToPipe(data, BUFFER_SIZE);
    sleep(5);

//    CHECK_EQUAL(BytesCommand, buffer);
   // STRCMP_EQUAL(data, buffer);
}

TEST(GroundCommandTestGroup, Delete_Command_Success) 
{
    // - write a command buffer to the Command Input File
    // - run delete_command
    // - check if the right number of bytes was removed from the command 
    //   input file
}

TEST(GroundCommandTestGroup, GetResultData_Success) 
{
    // - provide sample result buffers for all available commands
    // - write place each command in the Dnet-w-com-r pipe
    // - read the result buffer
    // - run ParseResult and validate all sample data
	const char *data= "SomeResult";
	sleep(1);
	Test_GD_Commander->WriteToInfoPipe(data);
}

TEST(GroundCommandTestGroup, Perform_Success) 
{
    // - test all switch cases
    // - test all conditional statements
    // - test null pointers
}

// Other tests
//
// Integration test:
//
// For each command (once API is available):
// call GetCmdStr to create a command buffer, put it in the Command Input Pipe, let it be parsed, 
// send it through the mock_sat netman system, run the command, return and test the result buffer 
// (ParseResult)
//
