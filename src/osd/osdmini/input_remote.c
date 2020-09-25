// license:BSD-3-Clause
// copyright-holders:Aaron Giles
//============================================================
//
//  minimain.c - Main function for mini OSD
//
//============================================================

#include "osdepend.h"
#include "osdcore.h"
#include "osdmini.h"


#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <mqueue.h>
#include <errno.h>

#include <sys/shm.h>


/******************************************************************************/


#define SHM_KEY 0x76057810
#define SHM_SIZE 0x00010000


static int shmid;
static UINT8 *shm_addr;
static UINT8 *shm_ctrl;

struct shm_video
{
	int magic;

	int fbx;
	int fby;
	int fbpp;
	int fbpitch;

	int cmd_state;
};


void shm_init(void)
{

	shmid = shmget(SHM_KEY, SHM_SIZE, 0666);
	if(shmid<0){
		perror("shmget");
		exit(-1);
	}

	shm_addr = (UINT8*)shmat(shmid, NULL, 0);
	if(shm_addr==(void*)-1){
		perror("shmat");
		exit(-1);
	}

	printf("shm id:%08x addr:%p\n", shmid, shm_addr);

	shm_ctrl = shm_addr+SHM_SIZE-4096;

}


void shm_exit(void)
{
	shmdt(shm_addr);
}


void remote_set_state(int state)
{
	struct shm_video *sv = (struct shm_video*)shm_ctrl;
	sv->cmd_state = state;
}


/******************************************************************************/


/******************************************************************************/

mqd_t input_mq;

void input_init_remote(void)
{
	shm_init();

	input_mq = mq_open("/mq_input", O_RDWR|O_NONBLOCK);
	if(input_mq<0){
		perror("mq_open(/mq_input)");
		exit(-1);
	}

	input_keyboard_init();
}

extern int saveload_state;

void input_handle_cmd(int key, int value)
{
	char buf[4] = {0, 0, 0, 0};

	if(key==0x8001){
		// save
		buf[0] = '0'+value;
		printf("\nimmediate_save %s\n", buf);
		mame_immediate_save(g_machine, (const char*)buf);
		printf("  done. %d\n", saveload_state);
		remote_set_state(saveload_state);
	}else if(key==0x8002){
		// load
		buf[0] = '0'+value;
		printf("\nimmediate_load %s\n", buf);
		mame_immediate_load(g_machine, (const char*)buf);
		printf("  done. %d\n", saveload_state);
		remote_set_state(saveload_state);
	}else if(key==0x8003){
		mame_pause(g_machine, TRUE);
	}else if(key==0x8004){
		g_pause = 0;
		mame_pause(g_machine, FALSE);
	}else if(key==0x8005){
		mame_schedule_exit(g_machine);
	}
}


#define KEY_P           25

void input_update_remote(void)
{
	int retv;
	UINT32 key, value;

	while(1){
		retv = mq_receive(input_mq, (char*)&key, 4, NULL);
		if(retv<=0)
			break;
		value = key&0xffff;
		key >>= 16;
		if(key>127){
			input_handle_cmd(key, value);
			continue;
		}
		if(key==KEY_P && value){
			g_pause = 1;
		}

		if(value){
			vt_keystate[key] = 1;
		}else{
			vt_keystate[key] = 0;
		}
	}

}

void input_exit_remote(void)
{
	mq_close(input_mq);
	shm_exit();
}


/******************************************************************************/


void input_register_remote(void)
{
	osd_input_init = input_init_remote;
	osd_input_exit = input_exit_remote;
	osd_input_update = input_update_remote;
}

/******************************************************************************/


