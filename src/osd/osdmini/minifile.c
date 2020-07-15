//============================================================
//
//  minifile.c - Minimal core file access functions
//
//============================================================

#include "osdcore.h"
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>


struct _osd_directory
{
	DIR *fd;
	char path[256];
	osd_directory_entry ent;
};

//============================================================
//  osd_open
//============================================================

static void mkdir_p(char *dname)
{
	char name[256];
	char *p, *cp;

	strcpy(name, dname);

	cp = name;
	while(1){
		p = strchr(cp, '/');
		if(p==NULL)
			break;

		*p = 0;
		mkdir(cp, 0777);
		*p = '/';
		cp = p+1;
	};
}

file_error osd_open(const char *path, UINT32 openflags, osd_file **file, UINT64 *filesize)
{
	const char *mode;
	FILE *fileptr;
	char pbuf[256];
	int i=0, p=0;

	while(1){
		char c = path[p++];
		if(c==0)
			break;
		if(c=='\\')
			c = '/';
		pbuf[i++] = c;
	}
	pbuf[i] = 0;

	// based on the flags, choose a mode
	if (openflags & OPEN_FLAG_WRITE) {
		if (openflags & OPEN_FLAG_READ)
			mode = (openflags & OPEN_FLAG_CREATE) ? "w+b" : "r+b";
		else
			mode = "wb";
	}else if (openflags & OPEN_FLAG_READ)
		mode = "rb";
	else
		return FILERR_INVALID_ACCESS;

	// open the file
	fileptr = fopen(pbuf, mode);
	if (fileptr == NULL) {
		if(openflags & OPEN_FLAG_CREATE_PATHS){
			mkdir_p(pbuf);
			fileptr = fopen(pbuf, mode);
		}
	}
	if (fileptr == NULL) {
		return FILERR_NOT_FOUND;
	}

	// store the file pointer directly as an osd_file
	*file = (osd_file *)fileptr;

	// get the size -- note that most fseek/ftell implementations are limited to 32 bits
	fseek(fileptr, 0, SEEK_END);
	*filesize = ftell(fileptr);
	fseek(fileptr, 0, SEEK_SET);

	return FILERR_NONE;
}


//============================================================
//  osd_close
//============================================================

file_error osd_close(osd_file *file)
{
	// close the file handle
	fclose((FILE *)file);
	return FILERR_NONE;
}


//============================================================
//  osd_read
//============================================================

file_error osd_read(osd_file *file, void *buffer, UINT64 offset, UINT32 length, UINT32 *actual)
{
	size_t count;

	// seek to the new location; note that most fseek implementations are limited to 32 bits
	fseek((FILE *)file, offset, SEEK_SET);

	// perform the read
	count = fread(buffer, 1, length, (FILE *)file);
	if (actual != NULL)
		*actual = count;

	return FILERR_NONE;
}


//============================================================
//  osd_write
//============================================================

file_error osd_write(osd_file *file, const void *buffer, UINT64 offset, UINT32 length, UINT32 *actual)
{
	size_t count;

	// seek to the new location; note that most fseek implementations are limited to 32 bits
	fseek((FILE *)file, offset, SEEK_SET);

	// perform the write
	count = fwrite(buffer, 1, length, (FILE *)file);
	if (actual != NULL)
		*actual = count;

	return FILERR_NONE;
}


//============================================================
//  osd_rmfile
//============================================================

file_error osd_rmfile(const char *filename)
{
	return remove(filename) ? FILERR_FAILURE : FILERR_NONE;
}


//============================================================
//  osd_get_physical_drive_geometry
//============================================================

int osd_get_physical_drive_geometry(const char *filename, UINT32 *cylinders, UINT32 *heads, UINT32 *sectors, UINT32 *bps)
{
	// there is no standard way of doing this, so we always return FALSE, indicating
	// that a given path is not a physical drive
	return FALSE;
}


//============================================================
//  osd_uchar_from_osdchar
//============================================================

int osd_uchar_from_osdchar(UINT32 /* unicode_char */ *uchar, const char *osdchar, size_t count)
{
	// we assume a standard 1:1 mapping of characters to the first 256 unicode characters
	*uchar = (UINT8)*osdchar;
	return 1;
}


//============================================================
//  osd_opendir
//============================================================


osd_directory *osd_opendir(const char *dirname)
{

	osd_directory *dir = (osd_directory*)malloc(sizeof(osd_directory));
	if(dir==NULL)
		return NULL;

	dir->fd = opendir(dirname);
	if(dir->fd==NULL){
		printf("osd_opendir(%s) failed!\n", dirname);
		return NULL;
	}

	strncpy(dir->path, dirname, 256);

	return dir;
}


//============================================================
//  osd_readdir
//============================================================

const osd_directory_entry *osd_readdir(osd_directory *dir)
{
	int retv;
	char full_path[256];
	struct dirent *dt;
	struct stat st;

	dt = readdir(dir->fd);
	if(dt==NULL){
		return NULL;
	}

	dir->ent.name = dt->d_name;

	sprintf(full_path, "%s/%s", dir->path, dt->d_name);
	retv = stat(full_path, &st);
	if(retv<0){
		printf("osd_readdir: can stat %s!\n", full_path);
		return NULL;
	}

	if(S_ISDIR(st.st_mode)){
		dir->ent.type = ENTTYPE_DIR;
	}else{
		dir->ent.type = ENTTYPE_FILE;
	}

	dir->ent.size = st.st_size;

	return &dir->ent;
}


//============================================================
//  osd_closedir
//============================================================

void osd_closedir(osd_directory *dir)
{
	if (dir->fd != NULL)
		closedir(dir->fd);
	free(dir);
}


//============================================================
//  osd_is_absolute_path
//============================================================

int osd_is_absolute_path(const char *path)
{
	if( (path[0] == '/') || (path[0] == '\\') )
		return TRUE;
	else
		return FALSE;
}

